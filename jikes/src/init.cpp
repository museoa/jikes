// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, 1999, 2000, 2001, 2002 International Business
// Machines Corporation and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#include "platform.h"
#include "semantic.h"
#include "control.h"

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif

void Semantic::ProcessVariableInitializer(AstVariableDeclarator *variable_declarator)
{
    VariableSymbol *symbol = variable_declarator -> symbol;

    if (! variable_declarator -> variable_initializer_opt)
    {
        symbol -> MarkComplete();
        return;
    }

    AstArrayInitializer *array_initializer = variable_declarator ->
        variable_initializer_opt -> ArrayInitializerCast();
    if (array_initializer)
        ProcessArrayInitializer(array_initializer, symbol -> Type());
    else
    {
        AstExpression *init =
            (AstExpression *) variable_declarator -> variable_initializer_opt;
        ProcessExpressionOrStringConstant(init);
        TypeSymbol *field_type = symbol -> Type();

        if (field_type != init -> Type() && init -> Type() != control.no_type)
        {
            if (CanAssignmentConvert(field_type, init))
            {
                init = ConvertToType(init, field_type);
                variable_declarator -> variable_initializer_opt = init;
            }
            else if (init -> IsConstant() &&
                     control.IsSimpleIntegerValueType(init -> Type()) &&
                     control.IsSimpleIntegerValueType(field_type))
            {
                if (field_type == control.byte_type)
                     ReportSemError(SemanticError::INVALID_BYTE_VALUE,
                                    init -> LeftToken(),
                                    init -> RightToken());
                else if (field_type == control.char_type)
                     ReportSemError(SemanticError::INVALID_CHARACTER_VALUE,
                                    init -> LeftToken(),
                                    init -> RightToken());
                else ReportSemError(SemanticError::INVALID_SHORT_VALUE,
                                    init -> LeftToken(),
                                    init -> RightToken());
                init -> value = NULL;
            }
            else
            {
                ReportSemError(SemanticError::INCOMPATIBLE_TYPE_FOR_ASSIGNMENT,
                               variable_declarator -> LeftToken(),
                               init -> RightToken(),
                               field_type -> ContainingPackage() -> PackageName(),
                               field_type -> ExternalName(),
                               init -> Type() -> ContainingPackage() -> PackageName(),
                               init -> Type() -> ExternalName());
                init -> value = NULL;
            }
        }
    
        if (symbol -> ACC_FINAL() &&
            (field_type -> Primitive() || field_type == control.String()))
        {
            if (init -> IsConstant())
            {
                symbol -> initial_value = init -> value;
            }
            else if (symbol -> ACC_STATIC() && ThisType() -> IsInner())
            {
                ReportSemError(SemanticError::STATIC_FIELD_IN_INNER_CLASS_NOT_CONSTANT,
                               variable_declarator -> LeftToken(),
                               variable_declarator -> RightToken(),
                               lex_stream -> NameString(variable_declarator -> variable_declarator_name -> identifier_token),
                               ThisType() -> Name(),
                               ThisType() -> FileLoc());
            }
        }
    }
    symbol -> MarkComplete();
}


void Semantic::ProcessArrayInitializer(AstArrayInitializer *array_initializer,
                                       TypeSymbol *type)
{
    if (! type -> IsArray())
    {
        ReportSemError(SemanticError::INIT_SCALAR_WITH_ARRAY,
                       array_initializer -> LeftToken(),
                       array_initializer -> RightToken(),
                       type -> Name());
    }
    else
    {
        for (int i = 0; i < array_initializer -> NumVariableInitializers(); i++)
        {
            AstArrayInitializer *sub_array_initializer = array_initializer ->
                VariableInitializer(i) -> ArrayInitializerCast();
            TypeSymbol *array_subtype = type -> ArraySubtype();
            if (sub_array_initializer)
                 ProcessArrayInitializer(sub_array_initializer, array_subtype);
            else
            {
                AstExpression *init = (AstExpression *) array_initializer ->
                    VariableInitializer(i);
                ProcessExpressionOrStringConstant(init);

                if (array_subtype != init -> Type())
                {
                    if (CanAssignmentConvert(array_subtype, init))
                        array_initializer -> VariableInitializer(i) =
                            ConvertToType(init, array_subtype);
                    else if (array_subtype -> IsArray() &&
                             init -> Type() -> Primitive())
                    {
                        ReportSemError(SemanticError::INIT_ARRAY_WITH_SCALAR,
                                       init -> LeftToken(),
                                       init -> RightToken(),
                                       array_subtype -> Name());
                    }
                    else if (init -> IsConstant() &&
                             control.IsSimpleIntegerValueType(init -> Type()) &&
                             control.IsSimpleIntegerValueType(array_subtype))
                    {
                        if (array_subtype == control.byte_type)
                             ReportSemError(SemanticError::INVALID_BYTE_VALUE,
                                            init -> LeftToken(),
                                            init -> RightToken());
                        else if (array_subtype == control.char_type)
                             ReportSemError(SemanticError::INVALID_CHARACTER_VALUE,
                                            init -> LeftToken(),
                                            init -> RightToken());
                        else ReportSemError(SemanticError::INVALID_SHORT_VALUE,
                                            init -> LeftToken(),
                                            init -> RightToken());
                    }
                    else
                    {
                        ReportSemError(SemanticError::INCOMPATIBLE_TYPE_FOR_INITIALIZATION,
                                       init -> LeftToken(),
                                       init -> RightToken(),
                                       array_subtype -> ContainingPackage() -> PackageName(),
                                       array_subtype -> ExternalName(),
                                       init -> Type() -> ContainingPackage() -> PackageName(),
                                       init -> Type() -> ExternalName());
                    }
                }
            }
        }
    }
}


LiteralValue *Semantic::ComputeFinalValue(AstVariableDeclarator *variable_declarator)
{
    VariableSymbol *variable = variable_declarator -> symbol;
    TypeSymbol *type = (TypeSymbol *) variable -> owner;
    assert(variable -> ACC_FINAL() && ! variable -> IsDeclarationComplete() &&
           variable_declarator -> variable_initializer_opt -> ExpressionCast());

    state_stack.Push(type -> semantic_environment);
    variable_declarator -> pending = true;

    ProcessVariableInitializer(variable_declarator);

    variable_declarator -> pending = false;
    state_stack.Pop();

    return variable -> initial_value;
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

