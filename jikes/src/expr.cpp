// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#include "double.h"
#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <sys/stat.h>
#include "parser.h"
#include "semantic.h"
#include "control.h"
#include "table.h"
#include "tuple.h"

inline bool Semantic::IsIntValueRepresentableInType(AstExpression *expr, TypeSymbol *type)
{
    IntLiteralValue *literal = (IntLiteralValue *) expr -> value;

    return (expr -> IsConstant() && 
//
// TODO: this test makes more sense than the one below. However, the JLS
// specifies the "IsInt" test below?
//control.IsSimpleIntegerValueType(expr -> type())) &&
//
           expr -> Type() == control.int_type) &&
           (type == control.int_type ||
            type == control.no_type  ||
            (type == control.char_type && (literal -> value >= 0)  && (literal -> value <= 65535)) ||
            (type == control.byte_type && (literal -> value >= -128) && (literal -> value <= 127)) ||
            (type == control.short_type && (literal -> value >= -32768)  && (literal -> value <= 32767)));
}


inline bool Semantic::MoreSpecific(MethodSymbol *source_method, MethodSymbol *target_method)
{
    if (! CanMethodInvocationConvert(target_method -> containing_type, source_method -> containing_type))
        return false;

    for (int k = target_method -> NumFormalParameters() - 1; k >= 0; k--)
    {
        if (! CanMethodInvocationConvert(target_method -> FormalParameter(k) -> Type(),
                                         source_method -> FormalParameter(k) -> Type()))
            return false;
    }

    return true;
}


inline bool Semantic::MoreSpecific(MethodSymbol *method, Tuple<MethodSymbol *> &maximally_specific_method)
{
    for (int i = 0; i < maximally_specific_method.Length(); i++)
    {
        if (! MoreSpecific(method, maximally_specific_method[i]))
            return false;
    }

    return true;
}


inline bool Semantic::NoMethodMoreSpecific(Tuple<MethodSymbol *> &maximally_specific_method, MethodSymbol *method)
{
    for (int i = 0; i < maximally_specific_method.Length(); i++)
    {
        if (MoreSpecific(maximally_specific_method[i], method))
            return false;
    }

    return true;
}


void Semantic::ReportMethodNotFound(Ast *ast, wchar_t *name)
{
    SemanticError::SemanticErrorKind kind;

    int num_arguments;
    AstExpression **argument;

    AstMethodInvocation *method_call;
    AstClassInstanceCreationExpression *class_creation;
    AstThisCall *this_call;
    AstSuperCall *super_call;
    if (method_call = ast -> MethodInvocationCast())
    {
        kind = SemanticError::METHOD_NOT_FOUND;
        num_arguments = method_call -> NumArguments();
        argument = new AstExpression*[num_arguments + 1];
        for (int i = 0; i < num_arguments; i++)
            argument[i] = method_call -> Argument(i);
    }
    else
    {
        kind = SemanticError::CONSTRUCTOR_NOT_FOUND;

        if (class_creation = ast -> ClassInstanceCreationExpressionCast())
        {
            num_arguments = class_creation -> NumArguments();
            argument = new AstExpression*[num_arguments + 1];
            for (int i = 0; i < num_arguments; i++)
                argument[i] = class_creation -> Argument(i);
        }
        else if (super_call = ast -> SuperCallCast())
        {
            num_arguments = super_call -> NumArguments();
            argument = new AstExpression*[num_arguments + 1];
            for (int i = 0; i < num_arguments; i++)
                argument[i] = super_call -> Argument(i);
        }
        else
        {
            this_call = ast -> ThisCallCast();
assert(this_call);
            num_arguments = this_call -> NumArguments();
            argument = new AstExpression*[num_arguments + 1];
            for (int i = 0; i < num_arguments; i++)
                argument[i] = this_call -> Argument(i);
        }
    }

    int length = wcslen(name);

    for (int i = 0; i < num_arguments; i++)
    {
        TypeSymbol *arg_type = argument[i] -> Type();
        length += arg_type -> ContainingPackage() -> PackageNameLength() +
                  arg_type -> NameLength() + 3; // '/' after package_name
                                                 // ',' and ' ' to separate this formal parameter from the next one
    }

    wchar_t *header = new wchar_t[length + 3]; // +1 for (, +1 for ), +1 for '\0'
    wchar_t *s = header;

    for (wchar_t *s2 = name; *s2; s2++)
         *s++ = *s2;
    *s++ = U_LEFT_PARENTHESIS;
    if (num_arguments > 0)
    {
        for (int i = 0; i < num_arguments; i++)
        {
            TypeSymbol *arg_type = argument[i] -> Type();

            PackageSymbol *package = arg_type -> ContainingPackage();
            wchar_t *package_name = package -> PackageName();
            if (package -> PackageNameLength() > 0 && wcscmp(package_name, StringConstant::US__DO_) != 0)
            {
                while (*package_name)
                {
                    *s++ = (*package_name == U_SLASH ? (wchar_t) U_DOT : *package_name);
                    package_name++;
                }
                *s++ = U_DOT;
            }

            for (wchar_t *s2 = arg_type -> Name(); *s2; s2++)
                *s++ = *s2;
            *s++ = U_COMMA;
            *s++ = U_SPACE;
        }

        s -= 2; // remove the last ',' and ' '
    }
    *s++ = U_RIGHT_PARENTHESIS;
    *s = U_NULL;

    ReportSemError(kind,
                   ast -> LeftToken(),
                   ast -> RightToken(),
                   header);

    delete [] header;
    delete [] argument;

    return;
}


MethodSymbol *Semantic::FindConstructor(TypeSymbol *containing_type, Ast *ast,
                                        LexStream::TokenIndex left_tok, LexStream::TokenIndex right_tok)
{
    Tuple<MethodSymbol *> constructor_set(2);

    int num_arguments;
    AstExpression **argument; 

    AstClassInstanceCreationExpression *class_creation;
    AstThisCall *this_call;
    AstSuperCall *super_call;
    if (class_creation = ast -> ClassInstanceCreationExpressionCast())
    {
        num_arguments = class_creation -> NumArguments();
        argument = new AstExpression*[num_arguments + 1];
        for (int i = 0; i < num_arguments; i++)
            argument[i] = class_creation -> Argument(i);
    }
    else if (super_call = ast -> SuperCallCast())
    {
        num_arguments = super_call -> NumArguments();
        argument = new AstExpression*[num_arguments + 1];
        for (int i = 0; i < num_arguments; i++)
            argument[i] = super_call -> Argument(i);
    }
    else
    {
        this_call = ast -> ThisCallCast();
assert(this_call);
        num_arguments = this_call -> NumArguments();
        argument = new AstExpression*[num_arguments + 1];
        for (int i = 0; i < num_arguments; i++)
            argument[i] = this_call -> Argument(i);
    }

assert(containing_type -> ConstructorMembersProcessed());

    for (MethodSymbol *constructor = containing_type -> FindConstructorSymbol();
         constructor; constructor = constructor -> next_method)
    {
        if (! constructor -> IsTyped())
            constructor -> ProcessMethodSignature((Semantic *) this, right_tok);

        if (num_arguments == constructor -> NumFormalParameters())
        {
            int i;
            for (i = 0; i < num_arguments; i++)
            {
                if (! CanMethodInvocationConvert(constructor -> FormalParameter(i) -> Type(), argument[i] -> Type()))
                    break;
            }
            if (i == num_arguments)
            {
                if (MoreSpecific(constructor, constructor_set))
                {
                    constructor_set.Reset();
                    constructor_set.Next() = constructor;
                }
                else if (NoMethodMoreSpecific(constructor_set, constructor))
                    constructor_set.Next() = constructor;
            }
        }
    }

    if (constructor_set.Length() == 0)
    {
        MethodSymbol *method;
        for (method = containing_type -> FindMethodSymbol(containing_type -> Identity()); method; method = method -> next_method)
        {
            if (! method -> IsTyped())
                method -> ProcessMethodSignature((Semantic *) this, right_tok);

            if (num_arguments == method -> NumFormalParameters())
            {
                int i;
                for (i = 0; i < num_arguments; i++)
                {
                    if (! CanMethodInvocationConvert(method -> FormalParameter(i) -> Type(), argument[i] -> Type()))
                        break;
                }
                if (i == num_arguments)
                    break;
            }
        }

        if (method)
        {
            if (method -> method_or_constructor_declaration)
            {
                AstMethodDeclaration *method_declaration = (AstMethodDeclaration *) method -> method_or_constructor_declaration;
                FileLocation loc(method -> containing_type -> semantic_environment -> sem -> lex_stream,
                                 method_declaration -> method_declarator -> identifier_token);

                ReportSemError(SemanticError::METHOD_FOUND_FOR_CONSTRUCTOR,
                               left_tok,
                               right_tok,
                               containing_type -> Name(),
                               loc.location);
            }
            else
            {
                ReportSemError(SemanticError::METHOD_FOUND_FOR_CONSTRUCTOR,
                               left_tok,
                               right_tok,
                               containing_type -> Name(),
                               method -> containing_type -> file_location -> location);
            }
        }
        else ReportMethodNotFound(ast, containing_type -> Name());

        delete [] argument;

        return (MethodSymbol *) NULL;
    }
    else if (constructor_set.Length() > 1)
    {
        ReportSemError(SemanticError::AMBIGUOUS_CONSTRUCTOR_INVOCATION,
                       left_tok,
                       right_tok,
                       containing_type -> Name());
    }

    delete [] argument;

    return constructor_set[0];
}


//
// Search the type in question for a method. Note that name_symbol is an optional argument.
// If it was not passed to this function then its default value is NULL (see semantic.h) and
// we assume that the name to search for is the name specified in the field_access of the
// method_call. 
//
MethodSymbol *Semantic::FindMethodInType(TypeSymbol *type, AstMethodInvocation *method_call, NameSymbol *name_symbol)
{
    Tuple<MethodSymbol *> method_set(2);
    AstFieldAccess *field_access = method_call -> method -> FieldAccessCast();
    if (! name_symbol)
        name_symbol = (NameSymbol *) lex_stream -> NameSymbol(field_access -> identifier_token);

    if (! type -> expanded_method_table)
        ComputeMethodsClosure(type, field_access -> identifier_token);

//
// TODO: Confirm that this is no longer the case as of javac 1.2
//
/*
    //
    // First look for the method in the "type". If it is not found, look for
    // it in the superclasses in the proper order. It is possible that the
    // method in question is a private field that is contained in the body
    // of the type that we are currently processing (this_type()), in which
    // case it is accessible even though it is not directly inherited by "type".
    //
    for (TypeSymbol *type_symbol = type; type_symbol && method_set.Length() == 0; type_symbol = type_symbol -> super)
    {
*/
        TypeSymbol *type_symbol = type;

        for (MethodShadowSymbol *method_shadow = type_symbol -> expanded_method_table -> FindMethodShadowSymbol(name_symbol);
             method_shadow; method_shadow = method_shadow -> next_method)
        {
            MethodSymbol *method = method_shadow -> method_symbol;
            TypeSymbol *containing_type = method -> containing_type;

            if (! method -> IsTyped())
                method -> ProcessMethodSignature((Semantic *) this, field_access -> identifier_token);

            if (method_call -> NumArguments() == method -> NumFormalParameters())
            {
                int i;
                for (i = 0; i < method_call -> NumArguments(); i++)
                {
                    AstExpression *expr =  method_call -> Argument(i);
                    if (! CanMethodInvocationConvert(method -> FormalParameter(i) -> Type(), expr -> Type()))
                        break;
                }
                if (i == method_call -> NumArguments())
                {
                    if (MoreSpecific(method, method_set))
                    {
                        method_set.Reset();
                        method_set.Next() = method;
                    }
                    else if (NoMethodMoreSpecific(method_set, method))
                        method_set.Next() = method;
                }
            }
        }

/*
See comment above...
    }
*/

    if (method_set.Length() == 0)
    {
        if (! type -> expanded_field_table)
            ComputeFieldsClosure(type, field_access -> identifier_token);

        VariableShadowSymbol *variable_shadow_symbol = type -> expanded_field_table -> FindVariableShadowSymbol(name_symbol);

        if (variable_shadow_symbol)
        {
            VariableSymbol *variable_symbol = variable_shadow_symbol -> variable_symbol;
            TypeSymbol *enclosing_type = variable_symbol -> owner -> TypeCast();
assert(enclosing_type);
            ReportSemError(SemanticError::FIELD_NOT_METHOD,
                           method_call -> LeftToken(),
                           method_call -> RightToken(),
                           variable_symbol -> Name(),
                           enclosing_type -> ContainingPackage() -> PackageName(),
                           enclosing_type -> ExternalName());
        }
        else
        {
            TypeSymbol *super_type;
            MethodShadowSymbol *method_shadow;

            for (super_type = type -> super; super_type; super_type = super_type -> super)
            {
                for (method_shadow = super_type -> expanded_method_table -> FindMethodShadowSymbol(name_symbol);
                     method_shadow; method_shadow = method_shadow -> next_method)
                {
                    MethodSymbol *method = method_shadow -> method_symbol;
                    if (! method -> IsTyped())
                        method -> ProcessMethodSignature((Semantic *) this, field_access -> identifier_token);

                    if (method_call -> NumArguments() == method -> NumFormalParameters())
                    {
                        int i;
                        for (i = 0; i < method_call -> NumArguments(); i++)
                        {
                            AstExpression *expr =  method_call -> Argument(i);
                            if (! CanMethodInvocationConvert(method -> FormalParameter(i) -> Type(), expr -> Type()))
                                break;
                        }
                        if (i == method_call -> NumArguments()) // found a match ?
                            break;
                    }
                }

                if (method_shadow) // found a match ?
                    break;
            }

            if (super_type)
            {
                ReportSemError((method_shadow -> method_symbol -> ACC_PRIVATE()
                                               ? SemanticError::METHOD_WITH_PRIVATE_ACCESS_NOT_ACCESSIBLE
                                               : SemanticError::METHOD_WITH_DEFAULT_ACCESS_NOT_ACCESSIBLE),
                               method_call -> LeftToken(),
                               method_call -> RightToken(),
                               name_symbol -> Name(),
                               super_type -> ContainingPackage() -> PackageName(),
                               super_type -> ExternalName(),
                               type -> ContainingPackage() -> PackageName(),
                               type -> ExternalName());
            }
            else
            {
                if (FindNestedType(type, field_access -> identifier_token))
                {
                    ReportSemError(SemanticError::TYPE_NOT_METHOD,
                                   method_call -> LeftToken(),
                                   method_call -> RightToken(),
                                   name_symbol -> Name());
                }
                else ReportMethodNotFound(method_call, name_symbol -> Name());
            }
        }

        return (MethodSymbol *) NULL;
    }
    else if (method_set.Length() > 1)
    {
        ReportSemError(SemanticError::AMBIGUOUS_METHOD_INVOCATION,
                       method_call -> LeftToken(),
                       method_call -> RightToken(),
                       name_symbol -> Name(),
                       method_set[0] -> Header(),
                       method_set[0] -> containing_type -> ContainingPackage() -> PackageName(),
                       method_set[0] -> containing_type -> ExternalName(),
                       method_set[1] -> Header(),
                       method_set[1] -> containing_type -> ContainingPackage() -> PackageName(),
                       method_set[1] -> containing_type -> ExternalName());
    }

    return method_set[0];
}


void Semantic::SearchForMethodInEnvironment(Tuple<MethodSymbol *> &methods_found,
                                            SemanticEnvironment *&where_found,
                                            SemanticEnvironment *stack,
                                            AstMethodInvocation *method_call)
{
    AstSimpleName *simple_name = method_call -> method -> SimpleNameCast();
    NameSymbol *name_symbol = (NameSymbol *) lex_stream -> NameSymbol(simple_name -> identifier_token);

    SemanticEnvironment *env;
    for (env = stack; env; env = env -> previous)
    {
        TypeSymbol *type = env -> Type();
        if (! type -> expanded_method_table)
            ComputeMethodsClosure(type, simple_name -> identifier_token);

        methods_found.Reset();

        for (MethodShadowSymbol *method_shadow = type -> expanded_method_table -> FindMethodShadowSymbol(name_symbol);
             method_shadow; method_shadow = method_shadow -> next_method)
        {
            MethodSymbol *method = method_shadow -> method_symbol;
            TypeSymbol *containing_type = method -> containing_type;

            if (! method -> IsTyped())
                method -> ProcessMethodSignature((Semantic *) this, simple_name -> identifier_token);

            //
            // Since type -> IsOwner(this_type()), i.e., type encloses this_type(),
            // method is accessible, even if it is private.
            //
            if (method_call -> NumArguments() == method -> NumFormalParameters())
            {
                int i;
                for (i = 0; i < method_call -> NumArguments(); i++)
                {
                    AstExpression *expr =  method_call -> Argument(i);
                    if (! CanMethodInvocationConvert(method -> FormalParameter(i) -> Type(), expr -> Type()))
                        break;
                }
                if (i == method_call -> NumArguments())
                {
                    if (MoreSpecific(method, methods_found))
                    {
                        methods_found.Reset();
                        methods_found.Next() = method;
                    }
                    else if (NoMethodMoreSpecific(methods_found, method))
                        methods_found.Next() = method;
                }
            }
        }

        if (methods_found.Length() > 0)
            break;
    }

    where_found = env;

    return;
}


MethodSymbol *Semantic::FindMethodInEnvironment(SemanticEnvironment *&where_found,
                                                SemanticEnvironment *stack,
                                                AstMethodInvocation *method_call)
{
    Tuple<MethodSymbol *> methods_found(2);
    SearchForMethodInEnvironment(methods_found, where_found, stack, method_call);

    MethodSymbol *method_symbol = (methods_found.Length() > 0 ? methods_found[0] : (MethodSymbol *) NULL);
    if (method_symbol)
    {
        for (int i = 1; i < methods_found.Length(); i++)
        {
            ReportSemError(SemanticError::AMBIGUOUS_METHOD_INVOCATION,
                           method_call -> LeftToken(),
                           method_call -> RightToken(),
                           method_symbol -> Name(),
                           methods_found[0] -> Header(),
                           method_symbol -> containing_type -> ContainingPackage() -> PackageName(),
                           method_symbol -> containing_type -> ExternalName(),
                           methods_found[i] -> Header(),
                           methods_found[i] -> containing_type -> ContainingPackage() -> PackageName(),
                           methods_found[i] -> containing_type -> ExternalName());
        }

        if (method_symbol -> containing_type != where_found -> Type())  // is symbol an inherited field?
        {
            Tuple<MethodSymbol *> others(2);
            SemanticEnvironment *found_other;
            SearchForMethodInEnvironment(others, found_other, where_found -> previous, method_call);

            for (int i = 0; i < others.Length();  i++)
            {
                if (others[i] != method_symbol)
                {
                    ReportSemError(SemanticError::INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER,
                                   method_call -> LeftToken(),
                                   method_call -> RightToken(),
                                   method_symbol -> Name(),
                                   method_symbol -> containing_type -> ContainingPackage() -> PackageName(),
                                   method_symbol -> containing_type -> ExternalName(),
                                   found_other -> Type() -> ContainingPackage() -> PackageName(),
                                   found_other -> Type() -> ExternalName());
                    break;
                }
            }
        }
    }
    else
    {
        AstSimpleName *simple_name = method_call -> method -> SimpleNameCast();
        NameSymbol *name_symbol = (NameSymbol *) lex_stream -> NameSymbol(simple_name -> identifier_token);
        bool symbol_found = false;

        for (SemanticEnvironment *env = stack; env; env = env -> previous)
        {
            TypeSymbol *type = env -> Type();

            if (! type -> expanded_field_table)
                ComputeFieldsClosure(type, simple_name -> identifier_token);

            VariableShadowSymbol *variable_shadow_symbol = type -> expanded_field_table -> FindVariableShadowSymbol(name_symbol);
            if (variable_shadow_symbol)
            {
                VariableSymbol *variable_symbol = variable_shadow_symbol -> variable_symbol;
                TypeSymbol *enclosing_type = variable_symbol -> owner -> TypeCast();
assert(enclosing_type);
                ReportSemError(SemanticError::FIELD_NOT_METHOD,
                               method_call -> LeftToken(),
                               method_call -> RightToken(),
                               variable_symbol -> Name(),
                               enclosing_type -> ContainingPackage() -> PackageName(),
                               enclosing_type -> ExternalName());
                symbol_found = true;
                break;
            }
            else
            {
                TypeSymbol *super_type;
                MethodShadowSymbol *method_shadow;

                for (super_type = type -> super; super_type; super_type = super_type -> super)
                {
                    for (method_shadow = super_type -> expanded_method_table -> FindMethodShadowSymbol(name_symbol);
                         method_shadow; method_shadow = method_shadow -> next_method)
                    {
                        MethodSymbol *method = method_shadow -> method_symbol;
                        if (! method -> IsTyped())
                            method -> ProcessMethodSignature((Semantic *) this, simple_name -> identifier_token);

                        if (method_call -> NumArguments() == method -> NumFormalParameters())
                        {
                            int i;
                            for (i = 0; i < method_call -> NumArguments(); i++)
                            {
                                AstExpression *expr = method_call -> Argument(i);
                                if (! CanMethodInvocationConvert(method -> FormalParameter(i) -> Type(), expr -> Type()))
                                    break;
                            }
                            if (i == method_call -> NumArguments()) // found a match ?
                                break;
                        }
                    }

                    if (method_shadow) // found a match ?
                        break;
                }

                if (super_type)
                {
                    ReportSemError((method_shadow -> method_symbol -> ACC_PRIVATE()
                                                   ? SemanticError::METHOD_WITH_PRIVATE_ACCESS_NOT_ACCESSIBLE
                                                   : SemanticError::METHOD_WITH_DEFAULT_ACCESS_NOT_ACCESSIBLE),
                                   method_call -> LeftToken(),
                                   method_call -> RightToken(),
                                   name_symbol -> Name(),
                                   super_type -> ContainingPackage() -> PackageName(),
                                   super_type -> ExternalName(),
                                   type -> ContainingPackage() -> PackageName(),
                                   type -> ExternalName());
                    symbol_found = true;
                    break;
                }    
            }
        }

        if (! symbol_found)
        {
            if (FindType(simple_name -> identifier_token))
            {
                ReportSemError(SemanticError::TYPE_NOT_METHOD,
                               method_call -> LeftToken(),
                               method_call -> RightToken(),
                               name_symbol -> Name());
            }
            else ReportMethodNotFound(method_call, name_symbol -> Name());
        }
    }


    return method_symbol;
}



//
// Search the type in question for a variable. Note that name_symbol is an optional argument.
// If it was not passed to this function then its default value is NULL (see semantic.h) and
// we assume that the name to search for is the last identifier specified in the field_access.
//
inline VariableSymbol *Semantic::FindVariableInType(TypeSymbol *type, AstFieldAccess *field_access, NameSymbol *name_symbol)
{
    if (! name_symbol)
        name_symbol = (NameSymbol *) lex_stream -> NameSymbol(field_access -> identifier_token);

    if (! type -> expanded_field_table)
        ComputeFieldsClosure(type, field_access -> identifier_token);

//
// TODO: Confirm that this is no longer the case as of javac 1.2
//
/*
    //
    // First look for the variable in the "type". If it is not found, look for
    // it in the superclasses in the proper order. It is possible that the
    // field in question is a private field that is contained in the body
    // of the type that we are currently processing (this_type()), in which case
    // it is accessible even though it is not directly inherited by "type".
    //
    VariableShadowSymbol *variable_shadow_symbol;
    for (variable_shadow_symbol = NULL; (! variable_shadow_symbol) && type; type = type -> super)
        variable_shadow_symbol = type -> expanded_field_table -> FindVariableShadowSymbol(name_symbol);
*/

    VariableShadowSymbol *variable_shadow_symbol = type -> expanded_field_table -> FindVariableShadowSymbol(name_symbol);

    //
    // Recall that even an inaccessible member x of a super class (or interface) S,
    // in addition to not been inherited by a subclass, hides all other occurrences of x that may
    // appear in a super class (or super interface) of S (see 8.3).
    //
    if (variable_shadow_symbol)
    {
        VariableSymbol *variable_symbol = variable_shadow_symbol -> variable_symbol;

        for (int i = 0; i < variable_shadow_symbol -> NumConflicts(); i++)
        {
            ReportSemError(SemanticError::AMBIGUOUS_NAME,
                           field_access -> LeftToken(),
                           field_access -> RightToken(),
                           name_symbol -> Name(),
                           variable_symbol -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                           variable_symbol -> owner -> TypeCast() -> ExternalName(),
                           variable_shadow_symbol -> Conflict(i) -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                           variable_shadow_symbol -> Conflict(i) -> owner -> TypeCast() -> ExternalName());
        }

        return variable_symbol;
    }

    return (VariableSymbol *) NULL;
}


void Semantic::ReportAccessedFieldNotFound(AstFieldAccess *field_access, TypeSymbol *type)
{
    NameSymbol *name_symbol = (NameSymbol *) lex_stream -> NameSymbol(field_access -> identifier_token);
    VariableShadowSymbol *variable_shadow_symbol;

    if (! type -> expanded_field_table)
        ComputeFieldsClosure(type, field_access -> base -> LeftToken());
    TypeSymbol *super_type;
    for (super_type = type -> super; super_type; super_type = super_type -> super)
    {
        variable_shadow_symbol = super_type -> expanded_field_table -> FindVariableShadowSymbol(name_symbol);
        if (variable_shadow_symbol)
            break;
    }

    if (super_type)
    {
        VariableSymbol *variable_symbol = variable_shadow_symbol -> variable_symbol;
        ReportSemError((variable_symbol -> ACC_PRIVATE()
                                         ? SemanticError::FIELD_WITH_PRIVATE_ACCESS_NOT_ACCESSIBLE
                                         : SemanticError::FIELD_WITH_DEFAULT_ACCESS_NOT_ACCESSIBLE),
                       field_access -> LeftToken(),
                       field_access -> RightToken(),
                       variable_symbol -> Name(),
                       super_type -> ContainingPackage() -> PackageName(),
                       super_type -> ExternalName(),
                       type -> ContainingPackage() -> PackageName(),
                       type -> ExternalName());
    }    
    else
    {
        ReportSemError(SemanticError::FIELD_NOT_FOUND,
                       field_access -> LeftToken(),
                       field_access -> RightToken(),
                       lex_stream -> Name(field_access -> identifier_token));
    }

    return;
}


void Semantic::SearchForVariableInEnvironment(Tuple<VariableSymbol *> &variables_found,
                                              SemanticEnvironment *&where_found,
                                              SemanticEnvironment *stack,
                                              NameSymbol *name_symbol,
                                              LexStream::TokenIndex identifier_token)
{
    variables_found.Reset();
    where_found = (SemanticEnvironment *) NULL;

    for (SemanticEnvironment *env = stack; env; env = env -> previous)
    {
        VariableSymbol *variable_symbol = env -> symbol_table.FindVariableSymbol(name_symbol);
        if (variable_symbol) // a local variable
        {
            variables_found.Next() = variable_symbol;
            where_found = env;
            break;
        }

        TypeSymbol *type = env -> Type();
        if (! type -> expanded_field_table)
            ComputeFieldsClosure(type, identifier_token);
        VariableShadowSymbol *variable_shadow_symbol = type -> expanded_field_table -> FindVariableShadowSymbol(name_symbol);
        if (variable_shadow_symbol) 
        {
            //
            // Since type -> IsOwner(this_type()), i.e., type encloses this_type(),
            // variable_symbol is accessible, even if it is private.
            //
            variables_found.Next() = variable_shadow_symbol -> variable_symbol;

            //
            // Recall that even an inaccessible member x of a super class (or interface) S,
            // in addition to not been inherited by a subclass, hides all other occurrences of x that may
            // appear in a super class (or super interface) of S (see 8.3).
            //
            for (int i = 0; i < variable_shadow_symbol -> NumConflicts(); i++)
                variables_found.Next() = variable_shadow_symbol -> Conflict(i);
            where_found = env;
            break;
        }
    }

    return;
}


VariableSymbol *Semantic::FindVariableInEnvironment(SemanticEnvironment *&where_found,
                                                    SemanticEnvironment *stack, LexStream::TokenIndex identifier_token)
{
    Tuple<VariableSymbol *> variables_found(2);
    NameSymbol *name_symbol = (NameSymbol *) lex_stream -> NameSymbol(identifier_token);
    SearchForVariableInEnvironment(variables_found, where_found, stack, name_symbol, identifier_token);

    VariableSymbol *variable_symbol = (VariableSymbol *) (variables_found.Length() > 0 ? variables_found[0] : NULL);

    if (variable_symbol)
    {
        if (variable_symbol -> IsLocal()) // a local variable
        {
            if (where_found != stack)
            {
                TypeSymbol *type = stack -> Type();

                if (! variable_symbol -> ACC_FINAL())
                {
                    MethodSymbol *method = variable_symbol -> owner -> MethodCast();

                    ReportSemError(SemanticError::INNER_CLASS_REFERENCE_TO_NON_FINAL_LOCAL_VARIABLE,
                                   identifier_token,
                                   identifier_token,
                                   type -> ContainingPackage() -> PackageName(),
                                   type -> ExternalName(),
                                   lex_stream -> Name(identifier_token),
                                   //
                                   // TODO: What if the method is a constructor ?
                                   //        if (method -> Identity() != control.init_symbol &&
                                   //            method -> Identity() != control.block_init_symbol &&
                                   //            method -> Identity() != control.clinit_symbol)
                                   //
                                   //
                                   method -> Name());
                }

                type -> FindOrInsertLocalShadow(variable_symbol);

                variable_symbol = stack -> symbol_table.FindVariableSymbol(name_symbol);
                if (! variable_symbol)
                    variable_symbol = type -> FindVariableSymbol(name_symbol);
assert(variable_symbol);
                where_found = stack;
            }
        }
        else if (variable_symbol -> owner != where_found -> Type())  // is symbol an inherited field?
        {
            TypeSymbol *type = (TypeSymbol *) variable_symbol -> owner;
            Tuple<VariableSymbol *> others(2);
            SemanticEnvironment *found_other;
            SearchForVariableInEnvironment(others, found_other, where_found -> previous, name_symbol, identifier_token);

            for (int i = 0; i < others.Length();  i++)
            {
                if (others[i] != variable_symbol)
                {
                    MethodSymbol *method = others[i] -> owner -> MethodCast();

                    if (method)
                    {
                        ReportSemError(SemanticError::INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_LOCAL,
                                       identifier_token,
                                       identifier_token,
                                       lex_stream -> Name(identifier_token),
                                       type -> ContainingPackage() -> PackageName(),
                                       type -> ExternalName(),
                                       method -> Name());
                    }
                    else
                    {
                        ReportSemError(SemanticError::INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER,
                                       identifier_token,
                                       identifier_token,
                                       lex_stream -> Name(identifier_token),
                                       type -> ContainingPackage() -> PackageName(),
                                       type -> ExternalName(),
                                       found_other -> Type() -> ContainingPackage() -> PackageName(),
                                       found_other -> Type() -> ExternalName());
                    }
                }
            }
        }
    }

    for (int i = 1; i < variables_found.Length(); i++)
    {
        ReportSemError(SemanticError::AMBIGUOUS_NAME,
                       identifier_token,
                       identifier_token,
                       variable_symbol -> Name(),
                       variable_symbol -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                       variable_symbol -> owner -> TypeCast() -> ExternalName(),
                       variables_found[i] -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                       variables_found[i] -> owner -> TypeCast() -> ExternalName());
    }

    return variable_symbol;
}


VariableSymbol *Semantic::FindInstance(TypeSymbol *base_type, TypeSymbol *environment_type)
{
    for (int i = 0; i < base_type -> NumEnclosingInstances(); i++)
    {
        VariableSymbol *variable = base_type -> EnclosingInstance(i);
        //
        // 1.2 change. In 1.1, we used to allow access to any subclass of type. Now, there must
        // be a perfect match. I.e.,
        //
        // if (variable -> Type() -> IsSubclass(environment_type))
        //
        if (variable -> Type() == environment_type)
            return variable;
    }

    AstClassDeclaration *class_declaration = base_type -> declaration -> ClassDeclarationCast();
    AstClassInstanceCreationExpression *class_creation = base_type -> declaration -> ClassInstanceCreationExpressionCast();
assert(class_declaration || class_creation);
    AstClassBody *class_body = (class_declaration ? class_declaration -> class_body : class_creation -> class_body_opt);

    LexStream::TokenIndex loc = class_body -> left_brace_token;
    AstBlock *this_block = class_body -> this_block;
    if (! this_block)
    {
        this_block = compilation_unit -> ast_pool -> GenBlock();
        this_block -> left_brace_token  = loc;
        this_block -> right_brace_token = loc;

        this_block -> is_reachable = true;
        this_block -> can_complete_normally = true;

        class_body -> this_block = this_block;
    }

    int k = base_type -> NumEnclosingInstances();
    for (TypeSymbol *type = base_type -> EnclosingInstance(k - 1) -> Type(); type; type = type -> ContainingType(), k++)
    {
        AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(loc);
        simple_name -> symbol = base_type -> EnclosingInstance(k - 1);

        AstFieldAccess *field_access = compilation_unit -> ast_pool -> GenFieldAccess();
        field_access -> base = simple_name;
        field_access -> dot_token = loc;
        field_access -> identifier_token = loc;
        field_access -> symbol = type -> FindThis(0);

        AstMethodInvocation *rhs      = compilation_unit -> ast_pool -> GenMethodInvocation();
        rhs -> method                  = field_access;
        rhs -> left_parenthesis_token  = loc;
        rhs -> right_parenthesis_token = loc;
        rhs -> symbol                  = TypeSymbol::GetReadAccessMethod((VariableSymbol *) field_access -> symbol);

        AstSimpleName *lhs = compilation_unit -> ast_pool -> GenSimpleName(loc);
        VariableSymbol *variable = base_type -> FindThis(k);
        lhs -> symbol = (variable ? variable : base_type -> InsertThis(k));

        AstAssignmentExpression *assign = compilation_unit -> ast_pool
                                                            -> GenAssignmentExpression(AstAssignmentExpression::EQUAL, loc);
        assign -> left_hand_side = lhs;
        assign -> expression     = rhs;
        assign -> symbol         = lhs -> Type();

        AstExpressionStatement *stmt = compilation_unit -> ast_pool -> GenExpressionStatement();
        stmt -> expression          = assign;
        stmt -> semicolon_token_opt = loc;
        stmt -> is_reachable = true;
        stmt -> can_complete_normally = true;

        this_block -> AddStatement(stmt);

        //
        // 1.2 change. In 1.1, we used to allow access to any subclass of type. Now, there must
        // be a perfect match. I.e.,
        //
        // if (lhs -> Type() -> IsSubclass(environment_type))
        //
        if (lhs -> Type() == environment_type)
            break;
    }

    return base_type -> EnclosingInstance(k);
}


AstExpression *Semantic::CreateAccessToType(Ast *source, TypeSymbol *environment_type)
{
    TypeSymbol *this_type = ThisType();

    LexStream::TokenIndex tok;

    AstSimpleName *simple_name = source -> SimpleNameCast();
    AstFieldAccess *field_access = source -> FieldAccessCast();
    AstSuperCall *super_call = source -> SuperCallCast();
    AstThisCall *this_call = source -> ThisCallCast();
    AstClassInstanceCreationExpression *class_creation = source -> ClassInstanceCreationExpressionCast();

    if (simple_name)
         tok = simple_name -> identifier_token;
    else if (class_creation)
         tok = class_creation -> new_token;
    else if (super_call)
         tok = super_call -> super_token;
    else if (this_call)
         tok = this_call -> this_token;
    else if (field_access)
         tok = field_access -> dot_token;
    else assert(0);

    AstExpression *resolution;

    if (! this_type -> CanAccess(environment_type))
    {
        ReportSemError(SemanticError::ENCLOSING_INSTANCE_NOT_ACCESSIBLE,
                       (field_access ? field_access -> base -> LeftToken() : tok),
                       (field_access ? field_access -> base -> RightToken() : tok),
                       environment_type -> ContainingPackage() -> PackageName(),
                       environment_type -> ExternalName());

        resolution = compilation_unit -> ast_pool -> GenSimpleName(tok);
        resolution -> symbol = control.no_type;
    }
    else if (ExplicitConstructorInvocation() && ExplicitConstructorInvocation() -> SuperCallCast())
    {
        VariableSymbol *variable = LocalSymbolTable().FindVariableSymbol(control.this0_name_symbol);
        if (variable)
        {
            resolution = compilation_unit -> ast_pool -> GenSimpleName(tok);
            resolution -> symbol = variable;
            TypeSymbol *containing_type = this_type -> ContainingType();
            //
            // 1.2 change. In 1.1, we used to allow access to any subclass of type. Now, there must
            // be a perfect match. I.e.,
            //
            // if (! containing_type -> IsSubclass(environment_type))
            //
            if (containing_type != environment_type)
            {
                variable = FindInstance(containing_type, environment_type);

                AstFieldAccess *field_access = compilation_unit -> ast_pool -> GenFieldAccess();
                field_access -> base = resolution;
                field_access -> dot_token = field_access -> dot_token;
                field_access -> identifier_token = field_access -> dot_token;
                field_access -> symbol = variable;

                AstMethodInvocation *method_call      = compilation_unit -> ast_pool -> GenMethodInvocation();
                method_call -> method                  = field_access;
                method_call -> left_parenthesis_token  = field_access -> dot_token;
                method_call -> right_parenthesis_token = field_access -> dot_token;
                method_call -> symbol                  = TypeSymbol::GetReadAccessMethod(variable);

                resolution = method_call;
            }
        }
        else
        {
assert(this_type -> ACC_STATIC());
            resolution = compilation_unit -> ast_pool -> GenThisExpression(tok);
            resolution -> symbol = this_type;
        }
    }
    //
    // 1.2 change. In 1.1, we used to allow access to any subclass of type. Now, there must
    // be a perfect match. I.e.,
    //
    // else if (this_type -> IsSubclass(environment_type))
    //
    else if (this_type == environment_type)
    {
        resolution = compilation_unit -> ast_pool -> GenThisExpression(tok);
        resolution -> symbol = this_type;
    }
    else
    {
        resolution = compilation_unit -> ast_pool -> GenSimpleName(tok);
        resolution -> symbol = FindInstance(this_type, environment_type);
    }

    return ((resolution -> symbol == control.no_type) || (resolution -> Type() == environment_type)
                                                       ? resolution
                                                       : ConvertToType(resolution, environment_type));
}


void Semantic::CreateAccessToScopedVariable(AstSimpleName *simple_name, TypeSymbol *environment_type)
{
assert(environment_type -> IsOwner(ThisType()));
    VariableSymbol *variable_symbol = (VariableSymbol *) simple_name -> symbol;

    AstExpression *access_expression;
    if (variable_symbol -> ACC_STATIC())
    {
        access_expression = compilation_unit -> ast_pool -> GenSimpleName(simple_name -> identifier_token);
        access_expression -> symbol = environment_type;
    }
    else access_expression = CreateAccessToType(simple_name, environment_type);

    if (access_expression -> symbol != control.no_type)
    {
        AstFieldAccess *field_access = compilation_unit -> ast_pool -> GenFieldAccess();
        field_access -> base = access_expression;
        field_access -> dot_token = simple_name -> identifier_token;
        field_access -> identifier_token = simple_name -> identifier_token;
        field_access -> symbol = variable_symbol;

        //
        // TODO: we have filed a query to Sun regarding the necessity of this check!
        //
        // SimpleNameAccessCheck(simple_name, variable_symbol -> owner -> TypeCast(), variable_symbol);
        //

        if (variable_symbol -> ACC_PRIVATE())
        {
            AstMethodInvocation *method_call       = compilation_unit -> ast_pool -> GenMethodInvocation();
            method_call -> method                  = field_access;
            method_call -> left_parenthesis_token  = simple_name -> identifier_token;
            method_call -> right_parenthesis_token = simple_name -> identifier_token;
            method_call -> symbol                  = TypeSymbol::GetReadAccessMethod(variable_symbol);

            simple_name -> resolution_opt = method_call;
        }
        else simple_name -> resolution_opt = field_access;
    }

    return;
}


void Semantic::CreateAccessToScopedMethod(AstMethodInvocation *method_call, TypeSymbol *environment_type)
{
assert(environment_type -> IsOwner(ThisType()));
    MethodSymbol *method = (MethodSymbol *) method_call -> symbol;
    AstSimpleName *simple_name = (AstSimpleName *) method_call -> method;
assert(simple_name -> SimpleNameCast());

    AstExpression *access_expression;
    if (method -> ACC_STATIC())
    {
        access_expression = compilation_unit -> ast_pool -> GenSimpleName(simple_name -> identifier_token);
        access_expression -> symbol = environment_type;
    }
    else access_expression = CreateAccessToType(simple_name, environment_type);

    if (access_expression -> symbol != control.no_type)
    {
        //
        // TODO: we have filed a query to Sun regarding the necessity of this check!
        //
        // SimpleNameAccessCheck(simple_name, method -> containing_type, method);
        //

        simple_name -> resolution_opt = access_expression;
        if (method -> ACC_PRIVATE())
            method_call -> symbol = TypeSymbol::GetReadAccessMethod(method);
    }

    return;
}


void Semantic::ProcessSimpleName(Ast *expr)
{
    TypeSymbol *this_type = ThisType();

    AstSimpleName *simple_name = (AstSimpleName *) expr;
    SemanticEnvironment *where_found;
    VariableSymbol *variable_symbol = FindVariableInEnvironment(where_found, state_stack.Top(), simple_name -> identifier_token);
    if (variable_symbol)
    {
        simple_name -> symbol = variable_symbol;

        if (! variable_symbol -> IsTyped())
            variable_symbol -> ProcessVariableSignature((Semantic *) this, simple_name -> identifier_token);

        //
        // A variable_symbol FINAL must have an initial value.
        //
        if (variable_symbol -> ACC_FINAL())
        {
            if (variable_symbol -> IsDeclarationComplete())
                simple_name -> value = variable_symbol -> initial_value;
            else if (variable_symbol -> declarator)
            {
                AstVariableDeclarator *declarator = variable_symbol -> declarator -> VariableDeclaratorCast();
                //
                // If the variable declarator in question exists and its computation is not
                // pending (to avoid looping) and it has a simple expression initializer.
                //
                if (declarator &&
                    (! declarator -> pending) && 
                    declarator -> variable_initializer_opt &&
                    (! declarator -> variable_initializer_opt -> ArrayInitializerCast()))
                {
                    TypeSymbol *type = (TypeSymbol *) variable_symbol -> owner;
                    Semantic *sem = type -> semantic_environment -> sem;
                    simple_name -> value = sem -> ComputeFinalValue(declarator);
                }
            }
        }

        //
        // If the variable belongs to an outer type, add the proper
        // pointer dereferences (and method access in the case of a
        // private variable) necessary to get to it.
        //
        if (where_found != state_stack.Top())
            CreateAccessToScopedVariable(simple_name, where_found -> Type());

        // 
        // CreateAccessToScopedVariable did not detect an error?
        // 
        if (simple_name -> symbol != control.no_type)
        {
            if (StaticRegion())
            {
                if (! (variable_symbol -> IsLocal() || variable_symbol -> ACC_STATIC()))
                {
                    ReportSemError(SemanticError::NAME_NOT_CLASS_VARIABLE,
                                   simple_name -> identifier_token,
                                   simple_name -> identifier_token,
                                   lex_stream -> Name(simple_name -> identifier_token));
                }
                else if (! variable_symbol -> IsDeclarationComplete())
                {
                    ReportSemError(SemanticError::NAME_NOT_YET_AVAILABLE,
                                   simple_name -> identifier_token,
                                   simple_name -> identifier_token,
                                   lex_stream -> Name(simple_name -> identifier_token));
                }
            }
            else if (! variable_symbol -> ACC_STATIC()) // an instance variable?
            {
                TypeSymbol *containing_type = variable_symbol -> owner -> TypeCast(); // an instance field member ?

                if (containing_type)
                {
                    if (containing_type == this_type && (! variable_symbol -> IsDeclarationComplete()))
                    {
                        ReportSemError(SemanticError::NAME_NOT_YET_AVAILABLE,
                                       simple_name -> identifier_token,
                                       simple_name -> identifier_token,
                                       lex_stream -> Name(simple_name -> identifier_token));
                    }
                    else if (ExplicitConstructorInvocation() && where_found == state_stack.Top())
                    {
                        //
                        // If the variable in question is an instance variable that is
                        // declared in this_type (this_type is definitely a class) or
                        // one of its super classes, then we have an error:
                        //
                        ReportSemError(SemanticError::INSTANCE_VARIABLE_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
                                       simple_name -> identifier_token,
                                       simple_name -> identifier_token,
                                       lex_stream -> Name(simple_name -> identifier_token),
                                       containing_type -> Name());
                    }
                }
            }
        }
    }
    else
    {
        //
        // We make a little effort to issue a better error message if we can identify
        // the name in question as the name of a method in the local type.
        //
        NameSymbol *name_symbol = (NameSymbol *) lex_stream -> NameSymbol(simple_name -> identifier_token);

        MethodShadowSymbol *method_shadow;
        MethodSymbol *method;

        for (method_shadow = this_type -> expanded_method_table -> FindMethodShadowSymbol(name_symbol);
             method_shadow; method_shadow = method_shadow -> next_method)
        {
            method = method_shadow -> method_symbol;
            TypeSymbol *containing_type = method -> containing_type;

            if (method -> NumFormalParameters((Semantic *) this, simple_name -> identifier_token) == 0)
                break;
        }
                                                   
        if (method_shadow)
        {
             ReportSemError(SemanticError::METHOD_NOT_FIELD,
                            simple_name -> identifier_token,
                            simple_name -> identifier_token,
                            lex_stream -> Name(simple_name -> identifier_token),
                            method -> containing_type -> ContainingPackage() -> PackageName(),
                            method -> containing_type -> ExternalName());
        }
        else if (FindType(simple_name -> identifier_token))
        {
             ReportSemError(SemanticError::TYPE_NOT_FIELD,
                            simple_name -> identifier_token,
                            simple_name -> identifier_token,
                            lex_stream -> Name(simple_name -> identifier_token));
        }
        else ReportSemError(SemanticError::NAME_NOT_FOUND,
                            simple_name -> identifier_token,
                            simple_name -> identifier_token,
                            lex_stream -> Name(simple_name -> identifier_token));
        simple_name -> symbol = control.no_type;
    }

    return;
}


void Semantic::TypeAccessCheck(Ast *ast, TypeSymbol *type)
{
    //
    // Unless we are processing the body of a type do not do type checking.
    // (This method may be invoked, for example, when FindFirstType invokes ProcessPackageOrType)
    //
    if (state_stack.Size() > 0)
    {
        TypeSymbol *this_type = ThisType();

        //
        // Type checking is necessary only for two types that are not enclosed within
        // the same outermost type. Note that if we are trying to access an inner type
        // T1.T2...Tn from this type, TypeAccessCheck is expected to be invoked first
        // for T1, then T1.T2, ... and finally for T1.T2...Tn, in turn. When invoked
        // for T1.T2, for example, this function only checks whether or not T1.T2
        // is accessible from "this" type. It does not recheck whether or not T1 is
        // accessible.
        //
        if (this_type -> outermost_type != type -> outermost_type)
        {
            if (type -> ACC_PRIVATE())
                 ReportTypeInaccessible(ast, type);
            else if (type -> ACC_PROTECTED())
            {
                //
                // TODO: we have filed a query to Sun regarding which test is required here!
                //
                // if (! (type -> ContainingPackage() == this_package || this_type -> IsSubclass(type)))
                //
                if (! (type -> ContainingPackage() == this_package || this_type -> HasProtectedAccessTo(type)))
                    ReportTypeInaccessible(ast, type);
            }
            else if (! (type -> ACC_PUBLIC() || type -> ContainingPackage() == this_package))
                 ReportTypeInaccessible(ast, type);
        }
    }

    return;
}


void Semantic::TypeNestAccessCheck(AstExpression *name)
{
    AstSimpleName *simple_name = name -> SimpleNameCast();
    AstFieldAccess *field_access = name -> FieldAccessCast();
assert(simple_name || field_access);
    if (field_access)
        TypeNestAccessCheck(field_access -> base);

    TypeSymbol *type = (simple_name ? simple_name -> Type() : field_access -> Type());
    if (type)
        TypeAccessCheck(name, type);

    return;
}


void Semantic::ConstructorAccessCheck(AstClassInstanceCreationExpression *class_creation, MethodSymbol *constructor)
{
    TypeSymbol *this_type = ThisType();
    TypeSymbol *containing_type = constructor -> containing_type;

    if (this_type -> outermost_type != containing_type -> outermost_type)
    {
        if (constructor -> ACC_PRIVATE())
        {
            ReportSemError(SemanticError::PRIVATE_CONSTRUCTOR_NOT_ACCESSIBLE,
                           class_creation -> class_type -> LeftToken(),
                           class_creation -> right_parenthesis_token,
                           constructor -> Header(),
                           containing_type -> ContainingPackage() -> PackageName(),
                           containing_type -> ExternalName());
        }
        else if (! class_creation -> symbol -> TypeCast() -> Anonymous())
        {
            if (constructor -> ACC_PROTECTED())
            {
                //
                // TODO: we need to file a query to Sun regarding which test is required here!
                // According to the rules 6.6.2 in the JLS, access to a protected constructor
                // that is not contained in the same package is only valid through a "super(...)" call.
                // However, javac seems to have relaxed this restriction to allow subclass access...
                //
                //
                // TODO: we have filed a query to Sun regarding which test is required here!
                //
                // if (! (containing_type -> ContainingPackage() == this_package || this_type -> IsSubclass(containing_type)))
                //
                if (! (containing_type -> ContainingPackage() == this_package || this_type -> HasProtectedAccessTo(containing_type)))
                {
                    ReportSemError(SemanticError::PROTECTED_CONSTRUCTOR_NOT_ACCESSIBLE,
                                   class_creation -> class_type -> LeftToken(),
                                   class_creation -> right_parenthesis_token,
                                   constructor -> Header(),
                                   containing_type -> ContainingPackage() -> PackageName(),
                                   containing_type -> ExternalName());
                }
            }
            else if (! (constructor -> ACC_PUBLIC() || containing_type -> ContainingPackage() == this_package))
            {
                ReportSemError(SemanticError::DEFAULT_CONSTRUCTOR_NOT_ACCESSIBLE,
                               class_creation -> class_type -> LeftToken(),
                               class_creation -> right_parenthesis_token,
                               constructor -> Header(),
                               containing_type -> ContainingPackage() -> PackageName(),
                               containing_type -> ExternalName());
            }
        }
    }

    return;
}


void Semantic::MemberAccessCheck(AstFieldAccess *field_access, TypeSymbol *base_type, TypeSymbol *containing_type, Symbol *symbol)
{
    TypeSymbol *this_type = ThisType();

    VariableSymbol *variable_symbol = symbol -> VariableCast();
    MethodSymbol *method_symbol = symbol -> MethodCast();
assert(variable_symbol || method_symbol);
    AccessFlags *flags = (variable_symbol ? (AccessFlags *) variable_symbol : (AccessFlags *) method_symbol);

    AstExpression *base = field_access -> base;
    if (! (containing_type -> ACC_PUBLIC() || base_type -> ContainingPackage() == containing_type -> ContainingPackage()))
        ReportTypeInaccessible(base, containing_type);

    if (this_type -> outermost_type != containing_type -> outermost_type)
    {
        if (flags -> ACC_PRIVATE())
        {
            ReportSemError((variable_symbol ? SemanticError::PRIVATE_FIELD_NOT_ACCESSIBLE
                                            : SemanticError::PRIVATE_METHOD_NOT_ACCESSIBLE),
                           field_access -> identifier_token,
                           field_access -> identifier_token,
                           lex_stream -> Name(field_access -> identifier_token),
                           containing_type -> ContainingPackage() -> PackageName(),
                           containing_type -> ExternalName());
        }
        else if (flags -> ACC_PROTECTED())
        {
            //
            // TODO: we have filed a query to Sun regarding which test is required here!
            //       there also appears to be a mistake in the Language Spec in section
            //       6.6.2 re. access to protected members. The phrase "Q is S or a subclass of S"
            //       should have been "Q is S or a superclass of S".
            //       
            // if (! (containing_type -> ContainingPackage() == this_package || this_type -> IsSubclass(containing_type)))
            //
            if (! (base -> SuperExpressionCast() ||
                   containing_type -> ContainingPackage() == this_package ||
                   (this_type -> HasProtectedAccessTo(containing_type) && 
                    (this_type -> IsSubclass(base_type) || base_type -> IsOwner(this_type)))))
            {
                ReportSemError((variable_symbol ? SemanticError::PROTECTED_FIELD_NOT_ACCESSIBLE
                                                : SemanticError::PROTECTED_METHOD_NOT_ACCESSIBLE),
                               field_access -> identifier_token,
                               field_access -> identifier_token,
                               lex_stream -> Name(field_access -> identifier_token),
                               containing_type -> ContainingPackage() -> PackageName(),
                               containing_type -> ExternalName());
            }
        }
        else if (! (flags -> ACC_PUBLIC() || containing_type -> ContainingPackage() == this_package))
        {
            ReportSemError((variable_symbol ? SemanticError::DEFAULT_FIELD_NOT_ACCESSIBLE
                                            : SemanticError::DEFAULT_METHOD_NOT_ACCESSIBLE),
                           field_access -> identifier_token,
                           field_access -> identifier_token,
                           lex_stream -> Name(field_access -> identifier_token),
                           containing_type -> ContainingPackage() -> PackageName(),
                           containing_type -> ExternalName());
        }
    }

    return;
}


void Semantic::SimpleNameAccessCheck(AstSimpleName *simple_name, TypeSymbol *containing_type, Symbol *symbol)
{
    TypeSymbol *this_type = ThisType();

    VariableSymbol *variable_symbol = symbol -> VariableCast();
    MethodSymbol *method_symbol = symbol -> MethodCast();
assert(variable_symbol || method_symbol);
    AccessFlags *flags = (variable_symbol ? (AccessFlags *) variable_symbol : (AccessFlags *) method_symbol);

    if (! (containing_type -> ACC_PUBLIC() || this_type -> ContainingPackage() == containing_type -> ContainingPackage()))
        ReportTypeInaccessible(simple_name, containing_type);

    if (this_type -> outermost_type != containing_type -> outermost_type)
    {
        if (flags -> ACC_PRIVATE())
        {
            ReportSemError((variable_symbol ? SemanticError::PRIVATE_FIELD_NOT_ACCESSIBLE
                                            : SemanticError::PRIVATE_METHOD_NOT_ACCESSIBLE),
                           simple_name -> identifier_token,
                           simple_name -> identifier_token,
                           lex_stream -> Name(simple_name -> identifier_token),
                           containing_type -> ContainingPackage() -> PackageName(),
                           containing_type -> ExternalName());
        }
        else if (flags -> ACC_PROTECTED())
        {
            if (! (containing_type -> ContainingPackage() == this_package || this_type -> IsSubclass(containing_type)))
            {
                ReportSemError((variable_symbol ? SemanticError::PROTECTED_FIELD_NOT_ACCESSIBLE
                                                : SemanticError::PROTECTED_METHOD_NOT_ACCESSIBLE),
                               simple_name -> identifier_token,
                               simple_name -> identifier_token,
                               lex_stream -> Name(simple_name -> identifier_token),
                               containing_type -> ContainingPackage() -> PackageName(),
                               containing_type -> ExternalName());
            }
        }
        else if (! (flags -> ACC_PUBLIC() || containing_type -> ContainingPackage() == this_package))
        {
            ReportSemError((variable_symbol ? SemanticError::DEFAULT_FIELD_NOT_ACCESSIBLE
                                            : SemanticError::DEFAULT_METHOD_NOT_ACCESSIBLE),
                           simple_name -> identifier_token,
                           simple_name -> identifier_token,
                           lex_stream -> Name(simple_name -> identifier_token),
                           containing_type -> ContainingPackage() -> PackageName(),
                           containing_type -> ExternalName());
        }
    }

    return;
}


void Semantic::FindVariableMember(TypeSymbol *type, AstFieldAccess *field_access)
{
    TypeSymbol *this_type = ThisType();

    //
    // This operation may throw NullPointerException
    //
    SymbolSet *exception_set = TryExceptionTableStack().Top();
    if (exception_set)
    {
        exception_set -> AddElement(control.RuntimeException());
        exception_set -> AddElement(control.Error());
    }
    
    if (type -> Bad())
    {
        //
        // If no error has been detected so far, report this as an error so that
        // we don't try to generate code later. On the other hand, if an error
        // had been detected prior to this, don't flood the user with spurious
        // messages.
        //
        if (NumErrors() == 0)
            ReportAccessedFieldNotFound(field_access, type);
        field_access -> symbol = control.no_type;
    }
    else if (type == control.null_type || type -> Primitive())
    {
        ReportSemError(SemanticError::TYPE_NOT_REFERENCE,
                       field_access -> base -> LeftToken(),
                       field_access -> base -> RightToken(),
                       type -> Name());
        field_access -> symbol = control.no_type;
    }
    else
    {
        TypeAccessCheck(field_access -> base, type);

        VariableSymbol *variable_symbol = FindVariableInType(type, field_access);
        if (variable_symbol)
        {
            //
            // If a variable is FINAL and initialized with a constant expression,
            // we substitute the expression here. JLS section 15.27, pp 381-382.
            //
            // TODO: Note that the JLS is a bit ambiguous and that a strict reading
            // of 15.27 would prohibit substitution of a final constant value here. 
            // However, javac seems to accept it and as the section on qualified name
            // only mentions "final" but not "static" and as it is not possible to derefence
            // a non-static final with a TypeName, we decided to relax the rules also.
            //
            if (variable_symbol -> ACC_FINAL() && field_access -> base -> IsName())
            {
                //
                // If the field declaration of the type has been completely processed,
                // simply retrieve the value. Otherwise, compute the value of the
                // initialization expression in question on the fly if the variable
                // in question is not in the same type. Recall that static variables 
                // must be processed in the textual order in which they appear in the
                // body of a type. Therefore, if the static initialization of a field
                // refers to another variable in the same type it must have appeared
                // before the current field declaration otherwise we will emit an error
                // message later...
                //
                if (variable_symbol -> IsDeclarationComplete())
                    field_access -> value = variable_symbol -> initial_value;
                else if (variable_symbol -> declarator)
                {
                    AstVariableDeclarator *declarator = variable_symbol -> declarator -> VariableDeclaratorCast();
                    //
                    // If the variable declarator in question exists and its computation is not
                    // pending (to avoid looping) and it has a simple expression initializer.
                    //
                    if (declarator && (! declarator -> pending) && 
                        declarator -> variable_initializer_opt &&
                        (! declarator -> variable_initializer_opt -> ArrayInitializerCast()))
                    {
                        TypeSymbol *variable_type = (TypeSymbol *) variable_symbol -> owner;
                        Semantic *sem = variable_type -> semantic_environment -> sem;
                        field_access -> value = sem -> ComputeFinalValue(declarator);
                    }
                }
            }

            TypeSymbol *containing_type = (TypeSymbol *) variable_symbol -> owner;
            if (variable_symbol -> ACC_PRIVATE() && // access to an private variable in an enclosing type ?
                this_type != containing_type && this_type -> outermost_type == containing_type -> outermost_type)
            {
                if (field_access -> IsConstant())
                    field_access -> symbol = variable_symbol;
                else
                {
                    AstFieldAccess *method_access = compilation_unit -> ast_pool -> GenFieldAccess();
                    method_access -> base = field_access -> base;  // TODO: WARNING: sharing of Ast subtree !!!
                    method_access -> dot_token = field_access -> identifier_token;
                    method_access -> identifier_token = field_access -> identifier_token;
                    method_access -> symbol = variable_symbol; // the variable in question

                    AstMethodInvocation *p      = compilation_unit -> ast_pool -> GenMethodInvocation();
                    p -> method                  = method_access;
                    p -> left_parenthesis_token  = field_access -> identifier_token;
                    p -> right_parenthesis_token = field_access -> identifier_token;
                    p -> symbol                  = TypeSymbol::GetReadAccessMethod(variable_symbol);

                    field_access -> resolution_opt = p;
                    field_access -> symbol = p -> symbol;
                }
            }
            else
            {
                field_access -> symbol = variable_symbol;
                MemberAccessCheck(field_access, type, containing_type, variable_symbol);
            }
        }
        else
        {
            TypeSymbol *inner_type;
            if (inner_type = FindNestedType(type, field_access -> identifier_token))
                 ReportSemError(SemanticError::FIELD_IS_TYPE,
                                field_access -> identifier_token,
                                field_access -> identifier_token,
                                lex_stream -> Name(field_access -> identifier_token));
            else ReportAccessedFieldNotFound(field_access, type);

            field_access -> symbol = control.no_type;
        }
    }

    return;
}

//
// NOTE that method names are not processed here but by the function
// ProcessMethodName.
//
void Semantic::ProcessAmbiguousName(Ast *name)
{
    TypeSymbol *this_type = ThisType();

    AstSimpleName *simple_name;

    //
    // ...If the ambiguous name is a simple name,...
    //
    if (simple_name = name -> SimpleNameCast())
    {
        TypeSymbol *type;
        //
        // ... If the Identifier appears within the scope (6.3) if a local variable declaration (14.3)
        // or parameter declaration (8.4.1, 8.6.1, 14.18) with that name, then the ambiguous name is
        // reclassified as an ExpressionName...
        //
        // ...Otherwise, consider the class or interface C within whose declaration the Identifier occurs.
        // If C has one or more fields with that name, which may be either declared within it or inherited,
        // then the Ambiguous name is reclassified as an ExpressionName....
        //
        SemanticEnvironment *where_found;
        VariableSymbol *variable_symbol = FindVariableInEnvironment(where_found, state_stack.Top(), simple_name -> identifier_token);
        if (variable_symbol)
        {
            //
            // A variable_symbol that is FINAL may have an initial value.
            // If variable_symbol is not final then its initial value is NULL.
            //
            simple_name -> value = variable_symbol -> initial_value;
            simple_name -> symbol = variable_symbol;

            //
            // If the variable belongs to an outer type, add the proper
            // pointer dereferences (and method access in the case of a
            // private variable) necessary to  get to it.
            //
            if (where_found != state_stack.Top())
                CreateAccessToScopedVariable(simple_name, where_found -> Type());

            if (StaticRegion())
            {
                if (! (variable_symbol -> IsLocal() || variable_symbol -> ACC_STATIC()))
                {
                    ReportSemError(SemanticError::NAME_NOT_CLASS_VARIABLE,
                                   simple_name -> identifier_token,
                                   simple_name -> identifier_token,
                                   lex_stream -> Name(simple_name -> identifier_token));
                }
                else if (! variable_symbol -> IsDeclarationComplete())
                {
                    ReportSemError(SemanticError::NAME_NOT_YET_AVAILABLE,
                                   simple_name -> identifier_token,
                                   simple_name -> identifier_token,
                                   lex_stream -> Name(simple_name -> identifier_token));
                }
            }

            if (ExplicitConstructorInvocation() && (! variable_symbol -> ACC_STATIC()))
            {
                //
                // If the variable in question is an instance variable
                // that is declared in this_type (this_type is definitely
                // a class) or one of its super classes, then we have an error -> 
                //
                TypeSymbol *type = variable_symbol -> owner -> TypeCast();
                if (type)
                {
                    ReportSemError(SemanticError::INSTANCE_VARIABLE_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
                                   simple_name -> identifier_token,
                                   simple_name -> identifier_token,
                                   lex_stream -> Name(simple_name -> identifier_token),
                                   type -> Name());
                }
            }
        }

        //
        // ...Otherwise, if a type of that name is declared in the compilation unit (7.3) containing
        // the Identifier, either by a single-type-import declaration (7.5.1) or by a class or interface
        // type declaration (7.6), then the Ambiguous name is reclassified as a TypeName...
        //
        // ...Otherwise, if a type of that name is declared in another compilation unit (7.3) of the
        // package (7.1) of the compilation unit containing the Identifier, then the Ambiguous Name
        // is reclassified as a TypeName...
        //
        // ...Otherwise, if a type of that name is declared by exactly one type-import-on-demand declaration
        // (7.5.2) of the compilation unit containing the Identifier, then the AmbiguousName is reclassified
        // as a TypeName
        //
        // ...Otherwise, if a type of that name is declared by more than one type-import-on-demand declaration
        // of the compilation unit containing the Identifier, then a compile-time error results.
        //
        else if (type = FindType(simple_name -> identifier_token))
             simple_name -> symbol = type;
        //
        // ...Otherwise, the Ambiguous name is reclassified as a PackageName. A later step determines
        // whether or not a package of that name actually exists.
        //
        else
        {
            NameSymbol *name_symbol = (NameSymbol *) lex_stream -> NameSymbol(simple_name -> identifier_token);
            PackageSymbol *package = control.external_table.FindPackageSymbol(name_symbol);
            if (package)
                simple_name -> symbol = package;
            else
            {
                package = control.external_table.InsertPackageSymbol(name_symbol, NULL);
                control.FindPathsToDirectory(package);
                simple_name -> symbol = package;
            }
        }
    }
    //
    // ...If the ambiguous name is a qualified name,...
    //
    else
    {
        AstFieldAccess *field_access = (AstFieldAccess *) name;
assert(name -> FieldAccessCast());
        TypeSymbol *type = NULL;

        if (field_access -> IsClassAccess())
        {
            if (! control.option.one_one)
            {
                ReportSemError(SemanticError::ONE_ONE_FEATURE,
                               field_access -> LeftToken(),
                               field_access -> RightToken());
            }

            AstTypeExpression *base = (AstTypeExpression *) field_access -> base;

            AstArrayType *array_type = base -> type -> ArrayTypeCast();
            if (array_type)
            {
                AstPrimitiveType *primitive_type = array_type -> type -> PrimitiveTypeCast();
                TypeSymbol *unit = (primitive_type ? FindPrimitiveType(primitive_type) : MustFindType(array_type -> type));
                type = unit -> GetArrayType((Semantic *) this, array_type -> NumBrackets());
            }
            else
            {
                AstPrimitiveType *primitive_type = base -> type -> PrimitiveTypeCast();
                type = (primitive_type ? FindPrimitiveType(primitive_type) : MustFindType(base -> type));
            }

            TypeSymbol *outermost_type = this_type -> outermost_type;

            if (type -> Primitive())
            {
                if (type == control.int_type)
                     type = control.Integer();
                else if (type == control.double_type)
                     type = control.Double();
                else if (type == control.char_type)
                     type = control.Character();
                else if (type == control.long_type)
                     type = control.Long();
                else if (type == control.float_type)
                     type = control.Float();
                else if (type == control.byte_type)
                     type = control.Byte();
                else if (type == control.short_type)
                     type = control.Short();
                else if (type == control.boolean_type)
                     type = control.Boolean();
                else // (type == control.void_type)
                     type = control.Void();
                base -> symbol = type;
                field_access -> symbol = type -> FindVariableSymbol(control.type_name_symbol);
assert(field_access -> symbol);
            }
            else
            {
                base -> symbol = type;

                if (outermost_type -> ACC_INTERFACE())
                {
                    TypeSymbol *class_literal_type = outermost_type -> FindOrInsertClassLiteralClass(field_access -> identifier_token);
                    AddDependence(this_type, class_literal_type, field_access -> identifier_token);

                    AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(field_access -> identifier_token);
                    simple_name -> symbol = class_literal_type;

                    AstFieldAccess *method_access = compilation_unit -> ast_pool -> GenFieldAccess();
                    method_access -> base = simple_name;
                    method_access -> dot_token = field_access -> identifier_token;
                    method_access -> identifier_token = field_access -> identifier_token;

                    AstStringLiteral *string_literal = compilation_unit -> ast_pool -> GenStringLiteral(field_access -> identifier_token);
                    string_literal -> value = type -> FindOrInsertClassLiteralName(control);
                    string_literal -> symbol = control.String();

                    AstMethodInvocation *method_call       = compilation_unit -> ast_pool -> GenMethodInvocation();
                    method_call -> method                  = method_access;
                    method_call -> left_parenthesis_token  = field_access -> identifier_token;
                    method_call -> AddArgument(string_literal);
                    method_call -> right_parenthesis_token = field_access -> identifier_token;
                    method_call -> symbol                  = class_literal_type -> ClassLiteralMethod();

                    field_access -> resolution_opt = method_call;
                    field_access -> symbol = (method_call -> symbol ? method_call -> symbol : control.no_type);
                }
                else
                {
                    AddDependence(this_type, control.Class(), field_access -> identifier_token);

                    VariableSymbol *variable_symbol = outermost_type -> FindOrInsertClassLiteral(type);
                    if (this_type == outermost_type)
                    {
                        AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(field_access -> identifier_token);
                        simple_name -> symbol = variable_symbol;

                        field_access -> symbol = variable_symbol;
                        field_access -> resolution_opt = simple_name;
                    }
                    else
                    {
                        AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(field_access -> identifier_token);
                        simple_name -> symbol = outermost_type;

                        AstFieldAccess *method_access = compilation_unit -> ast_pool -> GenFieldAccess();
                        method_access -> base = simple_name;
                        method_access -> dot_token = field_access -> identifier_token;
                        method_access -> identifier_token = field_access -> identifier_token;
                        method_access -> symbol = variable_symbol; // the variable in question

                        AstMethodInvocation *method_call       = compilation_unit -> ast_pool -> GenMethodInvocation();
                        method_call -> method                  = method_access;
                        method_call -> left_parenthesis_token  = field_access -> identifier_token;
                        method_call -> right_parenthesis_token = field_access -> identifier_token;
                        method_call -> symbol                  = TypeSymbol::GetWriteAccessMethod(variable_symbol);

                        field_access -> resolution_opt = method_call;
                        field_access -> symbol = TypeSymbol::GetReadAccessMethod(variable_symbol);
                    }
                }
            }
        }
        else
        {
            AstExpression* base = field_access -> base;
            AstFieldAccess *sub_field_access = base -> FieldAccessCast();
            simple_name = base -> SimpleNameCast();

            //
            // ...First, classify the name or expression to the left of the '.'...
            //
            if (simple_name || sub_field_access)
                 ProcessAmbiguousName(base);
            else ProcessExpression(base);

            if (base -> symbol == control.no_type)
            {
                field_access -> symbol = control.no_type;
                return;
            }

            wchar_t *identifier_name = lex_stream -> Name(field_access -> identifier_token);
            PackageSymbol *package;

            Symbol *symbol = base -> symbol;

            if (field_access -> IsThisAccess() || field_access -> IsSuperAccess())
            {
                if (! control.option.one_one)
                {
                    ReportSemError(SemanticError::ONE_ONE_FEATURE,
                                   field_access -> LeftToken(),
                                   field_access -> RightToken());
                }

                TypeSymbol *enclosing_type = symbol -> TypeCast();
                if (enclosing_type == control.no_type)
                    field_access -> symbol = control.no_type;
                else
                {
                    if (! enclosing_type)
                    {
                        ReportSemError(SemanticError::NOT_A_TYPE,
                                       field_access -> base -> LeftToken(),
                                       field_access -> base -> RightToken());
                        field_access -> symbol = control.no_type;
                    }
                    else if (enclosing_type -> ACC_INTERFACE())
                    {
                        ReportSemError(SemanticError::NOT_A_CLASS,
                                       field_access -> base -> LeftToken(),
                                       field_access -> base -> RightToken(),
                                       enclosing_type -> ContainingPackage() -> PackageName(),
                                       enclosing_type -> ExternalName());
                        field_access -> symbol = control.no_type;
                    }
                    else
                    {
                        if (! this_type -> CanAccess(enclosing_type))
                        {
                            if (this_type == enclosing_type && field_access -> IsThisAccess())
                            {
                                ReportSemError(SemanticError::MISPLACED_THIS_EXPRESSION,
                                               field_access -> LeftToken(),
                                               field_access -> RightToken());
                            }
                            else
                            {
                                ReportSemError(SemanticError::ILLEGAL_THIS_FIELD_ACCESS,
                                               field_access -> LeftToken(),
                                               field_access -> RightToken(),
                                               enclosing_type -> ContainingPackage() -> PackageName(),
                                               enclosing_type -> ExternalName(),
                                               this_type -> ContainingPackage() -> PackageName(),
                                               this_type -> ExternalName());
                            }

                            field_access -> symbol = control.no_type;
                        }
                        else
                        {
                            //
                            // Note that in the case of a Super access, there will be further resolution later.
                            //
                            field_access -> resolution_opt = CreateAccessToType(field_access, enclosing_type);
                            field_access -> symbol = field_access -> resolution_opt -> symbol;
                        }
                    }
                }
            }
            else if (package = symbol -> PackageCast())
            {
                //
                // ... If there is a package whose name is the name to the left of the '.' and that package
                // contains a declaration of a type whose name is the same as the Identifier, then the
                // AmbiguousName is reclassified as a TypeName...
                //
                NameSymbol *name_symbol = (NameSymbol *) lex_stream -> NameSymbol(field_access -> identifier_token);
                type = package -> FindTypeSymbol(name_symbol);

                if (type)
                {
                    if (type -> SourcePending())
                        control.ProcessHeaders(type -> file_symbol);
                    field_access -> symbol = type;
                }
                else
                {
                    FileSymbol *file_symbol = Control::GetFile(package, name_symbol, control.option.depend);
                    if (file_symbol)
                    {
                        type = ReadType(file_symbol, package, name_symbol, field_access -> identifier_token);
                        field_access -> symbol = type;
                    }
                    //
                    // ... Otherwise, this AmbiguousName is reclassified as a PackageName. A later step determines
                    // whether or not a package of that name actually exists...
                    //
                    else
                    {
                        PackageSymbol *subpackage = package -> FindPackageSymbol(name_symbol);
                        if (! subpackage) // A new package ?
                        {
                            subpackage = package -> InsertPackageSymbol(name_symbol);
                            control.FindPathsToDirectory(subpackage);
                        }
                        field_access -> symbol = subpackage;
                    }
                }
            }
            else if (sub_field_access && sub_field_access -> IsSuperAccess())
            {
                if (sub_field_access -> Type() == control.no_type)
                    field_access -> symbol = control.no_type;
                else if (sub_field_access -> Type() == control.Object())
                {
                    ReportSemError(SemanticError::OBJECT_HAS_NO_SUPER_TYPE,
                                   sub_field_access -> LeftToken(),
                                   sub_field_access -> RightToken(),
                                   sub_field_access -> Type() -> ContainingPackage() -> PackageName(),
                                   sub_field_access -> Type() -> ExternalName());
                    field_access -> symbol = control.no_type;
                }
                else
                {
                    type = sub_field_access -> Type() -> super;
                    FindVariableMember(type, field_access);
                }
            }
            //
            // ...If the name to the left of the '.' is reclassified as a TypeName, then this AmbiguousName is
            // reclassified as an ExpressionName
            //
            else if (type = symbol -> TypeCast())
            {
                if (type -> Bad())
                {
                    //
                    // If no error has been detected so far, report this as an error so that
                    // we don't try to generate code later. On the other hand, if an error
                    // had been detected prior to this, don't flood the user with spurious
                    // messages.
                    //
                    if (NumErrors() == 0)
                        ReportAccessedFieldNotFound(field_access, type);
                    field_access -> symbol = control.no_type;
                }
                else if (type == control.null_type || type -> Primitive())
                {
                    ReportSemError(SemanticError::TYPE_NOT_REFERENCE,
                                   base -> LeftToken(),
                                   base -> RightToken(),
                                   type -> Name());
                    field_access -> symbol = control.no_type;
                }
                else
                {
                    TypeAccessCheck(base, type);

                    VariableSymbol *variable_symbol = FindVariableInType(type, field_access);

                    if (variable_symbol)
                    {
                        if (base -> IsName()) // a type name (as opposed to an expression) ? 
                        {
                            if (variable_symbol -> ACC_STATIC())
                            {
                                //
                                // A variable_symbol that is STATIC and FINAL must have an initial value.
                                // If it is dereferenced by a type name, then associate the value with the
                                // subexpression to identify it as a constant subexpression.
                                //
                                if (variable_symbol -> ACC_FINAL())
                                {
                                    //
                                    // If the field declaration of the type has been completely processed,
                                    // simply retrieve the value. Otherwise, compute the value of the
                                    // initialization expression in question on the fly if the variable
                                    // in question is not in the same type. Recall that static variables 
                                    // must be processed in the textual order in which they appear in the
                                    // body of a type. Therefore, if the static initialization of a field
                                    // refers to another variable in the same type it must have appeared
                                    // before the current field declaration otherwise we will emit an error
                                    // message later...
                                    //
                                    if (variable_symbol -> IsDeclarationComplete())
                                        field_access -> value = variable_symbol -> initial_value;
                                    else if (variable_symbol -> declarator)
                                    {
                                        AstVariableDeclarator *declarator = variable_symbol -> declarator -> VariableDeclaratorCast();
                                        //
                                        // If the variable declarator in question exists and its computation is not
                                        // pending (to avoid looping) and it has a simple expression initializer.
                                        //
                                        if (declarator &&
                                            (! declarator -> pending) && 
                                            declarator -> variable_initializer_opt &&
                                            (! declarator -> variable_initializer_opt -> ArrayInitializerCast()))
                                        {
                                            TypeSymbol *type = (TypeSymbol *) variable_symbol -> owner;
                                            Semantic *sem = type -> semantic_environment -> sem;
                                            field_access -> value = sem -> ComputeFinalValue(declarator);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                ReportSemError(SemanticError::NAME_NOT_CLASS_VARIABLE,
                                               field_access -> identifier_token,
                                               field_access -> identifier_token,
                                               identifier_name);
                            }
                        }

                        TypeSymbol *containing_type = (TypeSymbol *) variable_symbol -> owner;
                        if (variable_symbol -> ACC_PRIVATE() &&
                            this_type != containing_type && this_type -> outermost_type == containing_type -> outermost_type)
                        {
                            if (field_access -> IsConstant())
                                field_access -> symbol = variable_symbol;
                            else
                            {
                                AstFieldAccess *method_access = compilation_unit -> ast_pool -> GenFieldAccess();
                                method_access -> base = field_access -> base;  // TODO: WARNING: sharing of Ast subtree !!!
                                method_access -> dot_token = field_access -> identifier_token;
                                method_access -> identifier_token = field_access -> identifier_token;
                                method_access -> symbol = variable_symbol; // the variable in question

                                AstMethodInvocation *p      = compilation_unit -> ast_pool -> GenMethodInvocation();
                                p -> method                  = method_access;
                                p -> left_parenthesis_token  = field_access -> identifier_token;
                                p -> right_parenthesis_token = field_access -> identifier_token;
                                p -> symbol                  = TypeSymbol::GetReadAccessMethod(variable_symbol);

                                field_access -> resolution_opt = p;
                                field_access -> symbol = p -> symbol;
                            }
                        }
                        else
                        {
                            field_access -> symbol = variable_symbol;
                            MemberAccessCheck(field_access, type, containing_type, variable_symbol);
                        }
                    }
                    else 
                    {
                        TypeSymbol *inner_type = FindNestedType(type, field_access -> identifier_token);
                        if (inner_type)
                        {
                            field_access -> symbol = inner_type;
                            TypeAccessCheck(field_access, inner_type);
                        }
                        else
                        {
                            ReportAccessedFieldNotFound(field_access, type);
                            field_access -> symbol = control.no_type;
                        }
                    }
                }
            }
            //
            // ...If the name to the left of the '.' is reclassified as an ExpressionName, then this
            // AmbiguousName is reclassified as an ExpressionName
            //
            else if (type = (TypeSymbol *)
                            (symbol -> VariableCast()
                                     ? symbol -> VariableCast() -> Type((Semantic *) this, field_access -> dot_token)
                                     : (symbol -> MethodCast()
                                                ? symbol -> MethodCast() -> Type((Semantic *) this, field_access -> dot_token)
                                                : NULL)))
            {
                FindVariableMember(type, field_access);
            }
            else // illegal Name !!!
            {
                ReportSemError(SemanticError::UNKNOWN_QUALIFIED_NAME_BASE,
                               base -> LeftToken(),
                               base -> RightToken(),
                               symbol -> Name());
                field_access -> symbol = control.no_type;
            }
        }

        if (type)
        {
            TypeSymbol *parent_type = (type -> IsArray() ? type -> base_type : type);
            if (! parent_type -> Primitive())
                AddDependence(this_type, parent_type, field_access -> identifier_token);
        }
    }

    return;
}


void Semantic::ProcessFieldAccess(Ast *expr)
{
    AstFieldAccess *field_access = (AstFieldAccess *) expr;

    ProcessAmbiguousName(field_access);

    if (field_access -> symbol != control.no_type)
    {
        PackageSymbol *package;
        TypeSymbol *type;

        if (package = field_access -> symbol -> PackageCast())
        {
            ReportSemError(SemanticError::UNKNOWN_AMBIGUOUS_NAME,
                           field_access -> LeftToken(),
                           field_access -> RightToken(),
                           package -> PackageName());
            field_access -> symbol = control.no_type;
        }
        else if ((type = field_access -> symbol -> TypeCast()) && (! field_access -> IsThisAccess()))
        {
            ReportSemError(SemanticError::TYPE_NOT_FIELD,
                           field_access -> LeftToken(),
                           field_access -> RightToken(),
                           type -> Name());
            field_access -> symbol = control.no_type;
        }
        else
        {
            VariableSymbol *variable = field_access -> symbol -> VariableCast();
            if (variable && (! variable -> IsTyped()))
                variable -> ProcessVariableSignature((Semantic *) this, field_access -> identifier_token);
        }
    }

    return;
}


void Semantic::ProcessCharacterLiteral(Ast *expr)
{
    AstCharacterLiteral *char_literal = (AstCharacterLiteral *) expr;

    LiteralSymbol *literal = (LiteralSymbol *) lex_stream -> NameSymbol(char_literal -> character_literal_token);

    if (! literal -> value)
        control.int_pool.FindOrInsertChar(literal);
    if (literal -> value == &control.bad_value)
    {
        ReportSemError(SemanticError::INVALID_CHARACTER_VALUE,
                       char_literal -> LeftToken(),
                       char_literal -> RightToken());
        char_literal -> symbol = control.no_type;
    }
    else
    {
        char_literal -> value = literal -> value;
        char_literal -> symbol = control.char_type;
    }
}


void Semantic::ProcessIntegerLiteral(Ast *expr)
{
    AstIntegerLiteral *int_literal = (AstIntegerLiteral *) expr;

    LiteralSymbol *literal = (LiteralSymbol *) lex_stream -> NameSymbol(int_literal -> integer_literal_token);

    if (! literal -> value)
        control.int_pool.FindOrInsertInt(literal);
    if (literal -> value == &control.bad_value)
    {
        ReportSemError(SemanticError::INVALID_INT_VALUE,
                       int_literal -> LeftToken(),
                       int_literal -> RightToken());
        int_literal -> symbol = control.no_type;
    }
    else
    {
        int_literal -> value = literal -> value;
        int_literal -> symbol = control.int_type;
    }
}


void Semantic::ProcessLongLiteral(Ast *expr)
{
    AstLongLiteral *long_literal = (AstLongLiteral *) expr;

    LiteralSymbol *literal = (LiteralSymbol *) lex_stream -> NameSymbol(long_literal -> long_literal_token);

    if (! literal -> value)
        control.long_pool.FindOrInsertLong(literal);
    if (literal -> value == &control.bad_value)
    {
        ReportSemError(SemanticError::INVALID_LONG_VALUE,
                       long_literal -> LeftToken(),
                       long_literal -> RightToken());
        long_literal -> symbol = control.no_type;
    }
    else
    {
        long_literal -> value = literal -> value;
        long_literal -> symbol = control.long_type;
    }
}


void Semantic::ProcessFloatingPointLiteral(Ast *expr)
{
    AstFloatingPointLiteral *float_literal = (AstFloatingPointLiteral *) expr;

    LiteralSymbol *literal = (LiteralSymbol *) lex_stream -> NameSymbol(float_literal -> floating_point_literal_token);

    if (! literal -> value)
        control.float_pool.FindOrInsertFloat(literal);
    if (literal -> value == &control.bad_value)
    {
        ReportSemError(SemanticError::INVALID_FLOAT_VALUE,
                       float_literal -> LeftToken(),
                       float_literal -> RightToken());
        float_literal -> symbol = control.no_type;
    }
    else
    {
        float_literal -> value = literal -> value;
        float_literal -> symbol = control.float_type;
    }
}


void Semantic::ProcessDoubleLiteral(Ast *expr)
{
    AstDoubleLiteral *double_literal = (AstDoubleLiteral *) expr;

    LiteralSymbol *literal = (LiteralSymbol *) lex_stream -> NameSymbol(double_literal -> double_literal_token);

    if (! literal -> value)
        control.double_pool.FindOrInsertDouble(literal);
    if (literal -> value == &control.bad_value)
    {
        ReportSemError(SemanticError::INVALID_DOUBLE_VALUE,
                       double_literal -> LeftToken(),
                       double_literal -> RightToken());
        double_literal -> symbol = control.no_type;
    }
    else
    {
        double_literal -> value = literal -> value;
        double_literal -> symbol = control.double_type;
    }
}


void Semantic::ProcessTrueLiteral(Ast *expr)
{
    AstExpression *true_literal = (AstTrueLiteral *) expr;

    true_literal -> value = control.int_pool.FindOrInsert((int) 1);
    true_literal -> symbol = control.boolean_type;
}


void Semantic::ProcessFalseLiteral(Ast *expr)
{
    AstExpression *false_literal = (AstFalseLiteral *) expr;

    false_literal -> value = control.int_pool.FindOrInsert((int) 0);
    false_literal -> symbol = control.boolean_type;
}


void Semantic::ProcessStringLiteral(Ast *expr)
{
    AstStringLiteral *string_literal = (AstStringLiteral *) expr;

    LiteralSymbol *literal = (LiteralSymbol *) lex_stream -> NameSymbol(string_literal -> string_literal_token);

    if (! literal -> value)
        control.Utf8_pool.FindOrInsertString(literal);
    if (literal -> value == &control.bad_value)
    {
        ReportSemError(SemanticError::INVALID_STRING_VALUE,
                       string_literal -> LeftToken(),
                       string_literal -> RightToken());
        string_literal -> symbol = control.no_type;
    }
    else
    {
        string_literal -> value = literal -> value;
        string_literal -> symbol = control.String();
    }
}


void Semantic::ProcessArrayAccess(Ast *expr)
{
    AstArrayAccess *array_access = (AstArrayAccess *) expr;

    //
    // This operation may throw NullPointerException or IndefOutOfBoundsException
    //
    SymbolSet *exception_set = TryExceptionTableStack().Top();
    if (exception_set)
    {
        exception_set -> AddElement(control.RuntimeException());
        exception_set -> AddElement(control.Error());
    }

    ProcessExpression(array_access -> base);
    ProcessExpression(array_access -> expression);
    array_access -> expression = PromoteUnaryNumericExpression(array_access -> expression);
    if (array_access -> expression -> Type() != control.int_type && array_access -> expression -> symbol != control.no_type)
    {
        ReportSemError(SemanticError::TYPE_NOT_INTEGER,
                       array_access -> expression -> LeftToken(),
                       array_access -> expression -> RightToken(),
                       array_access -> expression -> Type() -> Name());
    }

    TypeSymbol *array_type = array_access -> base -> Type();
    if (array_type -> IsArray())
        array_access -> symbol = array_type -> ArraySubtype();
    else
    {
        if (array_type != control.no_type)
            ReportSemError(SemanticError::TYPE_NOT_ARRAY,
                           array_access -> base -> LeftToken(),
                           array_access -> base -> RightToken(),
                           array_access -> base -> Type() -> Name());
        array_access -> symbol = control.no_type;
    }

    return;
}


MethodSymbol *Semantic::FindMethodMember(TypeSymbol *type, AstMethodInvocation *method_call)
{
    MethodSymbol *method = NULL;
    AstFieldAccess *field_access = method_call -> method -> FieldAccessCast();

    if (type -> Bad())
    {
        //
        // If no error has been detected so far, report this as an error so that
        // we don't try to generate code later. On the other hand, if an error
        // had been detected prior to this, don't flood the user with spurious
        // messages.
        //
        if (NumErrors() == 0)
            ReportMethodNotFound(method_call, lex_stream -> Name(field_access -> identifier_token));
        method_call -> symbol = control.no_type;
    }
    else if (type == control.null_type || type -> Primitive())
    {
        ReportSemError(SemanticError::TYPE_NOT_REFERENCE,
                       field_access -> base -> LeftToken(),
                       field_access -> base -> RightToken(),
                       type -> Name());
        method_call -> symbol = control.no_type;
    }
    else
    {
        TypeSymbol *this_type = ThisType();
        TypeAccessCheck(field_access -> base, type);

        method = FindMethodInType(type, method_call);

        if (method)
        {
            if (method -> ACC_PRIVATE() && this_type != method -> containing_type
                                        && this_type -> outermost_type == method -> containing_type -> outermost_type)
                method_call -> symbol = TypeSymbol::GetReadAccessMethod(method);
            else
            {
                method_call -> symbol = method;
                MemberAccessCheck(field_access, type, method -> containing_type, method);
            }
        }
        else method_call -> symbol = control.no_type;
    }

    return method;
}


void Semantic::ProcessMethodName(AstMethodInvocation *method_call)
{
    TypeSymbol *this_type = ThisType();

    //
    // This operation may throw:
    //
    //        OutOfMemoryError
    //        NoSuchMethodError
    //        IllegalAccessError
    //        IncompatibleClassChangeError
    //
    SymbolSet *exception_set = TryExceptionTableStack().Top();
    if (exception_set)
    {
        exception_set -> AddElement(control.RuntimeException());
        exception_set -> AddElement(control.Error());
    }

    AstSimpleName *simple_name;

    if (simple_name = method_call -> method -> SimpleNameCast())
    {
        SemanticEnvironment *where_found;
        MethodSymbol *method = FindMethodInEnvironment(where_found, state_stack.Top(), method_call);

        if (! method)
            method_call -> symbol = control.no_type;
        else
        {
            if (! method -> ACC_STATIC())
            {
                //
                // We are in a static region if we are:
                //     . in the body of a static method
                //     . in the body of a static initializer
                //     . precessing an initializer expression for a static variable.
                //
                // See StaticRegion() Semantic.h for more detail.
                //
                // Note that a constructor is never static.
                //
                if (StaticRegion())
                {
                    ReportSemError(SemanticError::METHOD_NOT_CLASS_METHOD,
                                   simple_name -> identifier_token,
                                   method_call -> right_parenthesis_token,
                                   lex_stream -> Name(simple_name -> identifier_token));
                }
                else if (ExplicitConstructorInvocation())
                {
                    if (this_type -> IsSubclass(method -> containing_type))
                    {
                        //
                        // If the method in question is an instance method
                        // that is declared in this_type (this_type is definitely
                        // a class) or one of its super classes, then we have an error -> 
                        //
                        ReportSemError(SemanticError::INSTANCE_METHOD_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
                                       method_call -> LeftToken(),
                                       method_call -> RightToken(),
                                       method -> Header(),
                                       method -> containing_type -> Name());
                    }
                }
            }

            method_call -> symbol = method;

            //
            // If the method is a private method belonging to an outer type,
            // give the ast simple_name access to its read_method.
            //
            if (where_found != state_stack.Top())
                CreateAccessToScopedMethod(method_call, where_found -> Type());
        }
    }
    else
    {
        AstFieldAccess *field_access = method_call -> method -> FieldAccessCast();
        AstExpression* base = field_access -> base;
        AstFieldAccess *sub_field_access = base -> FieldAccessCast();

        if (base -> SimpleNameCast() || sub_field_access)
             ProcessAmbiguousName(base);
        else ProcessExpression(base);

        if (base -> symbol == control.no_type)
        {
            method_call -> symbol = control.no_type;
            return;
        }

        TypeSymbol *type = NULL;
        Symbol *symbol = base -> symbol;

        //
        // If the base is a "super" field access, resolve it before proceeding 
        //
        if (sub_field_access && sub_field_access -> IsSuperAccess())
        {
            if (sub_field_access -> Type() == control.no_type)
                method_call -> symbol = control.no_type;
            else if (sub_field_access -> Type() == control.Object())
            {
                ReportSemError(SemanticError::OBJECT_HAS_NO_SUPER_TYPE,
                               sub_field_access -> LeftToken(),
                               sub_field_access -> RightToken(),
                               sub_field_access -> Type() -> ContainingPackage() -> PackageName(),
                               sub_field_access -> Type() -> ExternalName());
                method_call -> symbol = control.no_type;
            }
            else
            {
                MethodSymbol *method = FindMethodMember(sub_field_access -> Type() -> super, method_call);

                //
                // TODO: This test was added in order to pass the test in section 8.4.3.1, page 159.
                //       All I can find in the spec is that one example. Nowhere else could I find a
                //       more formal statement.
                //
                if (method && method -> ACC_ABSTRACT())
                {
                    ReportSemError(SemanticError::ABSTRACT_METHOD_INVOCATION,
                                   field_access -> LeftToken(),
                                   field_access -> identifier_token,
                                   lex_stream -> Name(field_access -> identifier_token));
                }
            }
        }
        else if (type = symbol -> TypeCast())
        {
            if (type -> Bad())
            {
                //
                // If no error has been detected so far, report this as an error so that
                // we don't try to generate code later. On the other hand, if an error
                // had been detected prior to this, don't flood the user with spurious
                // messages.
                //
                if (NumErrors() == 0)
                    ReportMethodNotFound(method_call, lex_stream -> Name(field_access -> identifier_token));
                method_call -> symbol = control.no_type;
            }
            else if (type == control.null_type || type -> Primitive())
            {
                ReportSemError(SemanticError::TYPE_NOT_REFERENCE,
                               base -> LeftToken(),
                               base -> RightToken(),
                               type -> Name());
                method_call -> symbol = control.no_type;
            }
            else
            {
                TypeAccessCheck(base, type);

                MethodSymbol *method = FindMethodInType(type, method_call);

                if (method)
                {
                    if (base -> IsName() && (! method -> ACC_STATIC()))
                    {
                        ReportSemError(SemanticError::METHOD_NOT_CLASS_METHOD,
                                       field_access -> LeftToken(),
                                       field_access -> identifier_token,
                                       lex_stream -> Name(field_access -> identifier_token));
                    }
                    //
                    // TODO: This test was added in order to pass the test in section 8.4.3.1, page 159.
                    //       All I can find in the spec is that one example. Nowhere else could I find a
                    //       more formal statement.
                    //
                    else if (base -> SuperExpressionCast() && method -> ACC_ABSTRACT())
                        ReportSemError(SemanticError::ABSTRACT_METHOD_INVOCATION,
                                       field_access -> LeftToken(),
                                       field_access -> identifier_token,
                                       lex_stream -> Name(field_access -> identifier_token));

                    if (method -> ACC_PRIVATE() && this_type != method -> containing_type
                                                && this_type -> outermost_type == method -> containing_type -> outermost_type)
                        method_call -> symbol = TypeSymbol::GetReadAccessMethod(method);
                    else
                    {
                        method_call -> symbol = method;
                        MemberAccessCheck(field_access, type, method -> containing_type, method);
                    }
                }
                else method_call -> symbol = control.no_type;
            }
        }
        else if (type = (TypeSymbol *)
                        (symbol -> VariableCast()
                                 ? symbol -> VariableCast() -> Type((Semantic *) this, field_access -> dot_token)
                                 : (symbol -> MethodCast()
                                            ? symbol -> MethodCast() -> Type((Semantic *) this, field_access -> dot_token)
                                            : NULL)))
        {
            (void) FindMethodMember(type, method_call);
        }
        else // illegal Name !!!
        {
            PackageSymbol *package = symbol -> PackageCast();
            NameSymbol *name_symbol = (NameSymbol *) lex_stream -> NameSymbol(field_access -> identifier_token);

            if (package && (package -> FindTypeSymbol(name_symbol) ||
                            Control::GetFile(package, name_symbol, control.option.depend)))
            {
                ReportSemError(SemanticError::TYPE_NOT_METHOD,
                               method_call -> LeftToken(),
                               method_call -> RightToken(),
                               name_symbol -> Name());
            }
            else
            {
                ReportSemError(SemanticError::UNKNOWN_QUALIFIED_NAME_BASE,
                               field_access -> base -> LeftToken(),
                               field_access -> base -> RightToken(),
                               symbol -> Name());
            }
            method_call -> symbol = control.no_type;
        }

        if (type)
        {
            TypeSymbol *parent_type = (type -> IsArray() ? type -> base_type : type);
            if (! parent_type -> Primitive())
                AddDependence(this_type, parent_type, field_access -> identifier_token);
        }
    }

    if (method_call -> symbol != control.no_type)
    {
        MethodSymbol *method = (MethodSymbol *) method_call -> symbol;

        if (exception_set)
        {
            for (int i = method -> NumThrows((Semantic *) this, method_call -> method -> RightToken()) - 1; i >= 0; i--)
                exception_set -> AddElement(method -> Throws(i));
// TODO: what to do?
//            if (! method -> ACC_STATIC())
//                exception_set -> AddElement(NullPointerException());
//            if (method -> ACC_NATIVE())
//                exception_set -> AddElement(control.UnsatisfiedLinkError());
        }

        for (int i = 0; i < method_call -> NumArguments(); i++)
        {
            AstExpression *expr = method_call -> Argument(i);
            if (expr -> Type() != method -> FormalParameter(i) -> Type())
                method_call -> Argument(i) = ConvertToType(expr, method -> FormalParameter(i) -> Type());
        }

        for (int k = method -> NumThrows((Semantic *) this, method_call -> method -> RightToken()) - 1; k >= 0; k--)
        {
            TypeSymbol *exception = method -> Throws(k);
            if (! CatchableException(exception))
            {
                ReportSemError(SemanticError::UNCATCHABLE_METHOD_THROWN_CHECKED_EXCEPTION,
                               method_call -> LeftToken(),
                               method_call -> RightToken(),
                               method -> Header(),
                               exception -> ContainingPackage() -> PackageName(),
                               exception -> ExternalName());
            }
        }
    }

    return;
}


void Semantic::ProcessMethodInvocation(Ast *expr)
{
    AstMethodInvocation *method_call = (AstMethodInvocation *) expr;

    bool no_bad_argument = true;

    for (int i = 0; i < method_call -> NumArguments(); i++)
    {
        AstExpression *expr = method_call -> Argument(i);
        ProcessExpression(expr); // TODO: ProcessExpressionOrStringConstant(expr);
        no_bad_argument = no_bad_argument && (expr -> symbol != control.no_type);
    }

    if (no_bad_argument)
    {
        ProcessMethodName(method_call);
        if (method_call -> symbol != control.no_type)
        {
            MethodSymbol *method = (MethodSymbol *) method_call -> symbol;
            if (! method -> IsTyped())
                method -> ProcessMethodSignature((Semantic *) this, method_call -> left_parenthesis_token);
        }
    }
    else method_call -> symbol = control.no_type;

    return;
}


void Semantic::ProcessNullLiteral(Ast *expr)
{
    AstNullLiteral *null_literal = (AstNullLiteral *) expr;
    null_literal -> value = control.NullValue();
    null_literal -> symbol = control.null_type;

    return;
}


void Semantic::ProcessThisExpression(Ast *expr)
{
    AstThisExpression *this_expression = (AstThisExpression *) expr;

    if (StaticRegion())
    {
        ReportSemError(SemanticError::MISPLACED_THIS_EXPRESSION,
                       this_expression -> LeftToken(),
                       this_expression -> RightToken());
        this_expression -> symbol = control.no_type;
    }
    else if (ExplicitConstructorInvocation())
    {
        ReportSemError(SemanticError::THIS_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
                       this_expression -> LeftToken(),
                       this_expression -> RightToken(),
                       lex_stream -> Name(this_expression -> this_token));
        this_expression -> symbol = control.no_type;
    }
    else this_expression -> symbol = ThisType();

    return;
}


void Semantic::ProcessSuperExpression(Ast *expr)
{
    AstSuperExpression *super_expression = (AstSuperExpression *) expr;

    if (StaticRegion() || ThisType() == control.Object())
    {
         ReportSemError(SemanticError::MISPLACED_SUPER_EXPRESSION,
                        super_expression -> LeftToken(),
                        super_expression -> RightToken());
         super_expression -> symbol = control.no_type;
    }
    else if (ExplicitConstructorInvocation())
    {
        ReportSemError(SemanticError::THIS_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
                       super_expression -> LeftToken(),
                       super_expression -> RightToken(),
                       lex_stream -> Name(super_expression -> super_token));
         super_expression -> symbol = control.no_type;
    }
    else super_expression -> symbol = ThisType() -> super;
}


void Semantic::ProcessParenthesizedExpression(Ast *expr)
{
    AstParenthesizedExpression *parenthesized = (AstParenthesizedExpression *) expr;

    ProcessExpression(parenthesized -> expression);
    parenthesized -> value = parenthesized -> expression -> value;
    parenthesized -> symbol = parenthesized -> expression -> symbol;
}              


void Semantic::UpdateGeneratedLocalConstructor(MethodSymbol *constructor)
{
    TypeSymbol *local_type = constructor -> containing_type;
    MethodSymbol *local_constructor = constructor -> LocalConstructor();
assert(local_constructor -> IsGeneratedLocalConstructor());
    BlockSymbol *block_symbol = local_constructor -> block_symbol;

    for (int i = 0; i < constructor -> NumFormalParameters(); i++)
    {
        VariableSymbol *param = constructor -> FormalParameter(i),
                       *symbol = block_symbol -> FindVariableSymbol(param -> Identity());
assert(symbol);
        symbol -> SetExternalIdentity(param -> ExternalIdentity()); // TODO: do we really need this ?
        symbol -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
        if (symbol -> Type() == control.long_type || symbol -> Type() == control.double_type)
            block_symbol -> max_variable_index++;
        local_constructor -> AddFormalParameter(symbol);
    }

    //
    // If we are dealing with a constructor generated for an anonymous type and
    // the super type of the anonymous type is an inner type then the generated
    // constructor accepts an additional formal parameter which is the containing
    // type of the super type, and the name of the parameter is #0.
    //
    VariableSymbol *super_this0_variable = block_symbol -> FindVariableSymbol(control.MakeParameter(0));
    if (super_this0_variable)
    {
        super_this0_variable -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
        local_constructor -> AddFormalParameter(super_this0_variable);
    }
    local_constructor -> SetSignature(control);

    //
    //
    //
    AstConstructorDeclaration *constructor_declaration = (AstConstructorDeclaration *)
                                                          constructor -> method_or_constructor_declaration;
assert(constructor_declaration -> ConstructorDeclarationCast());
    AstConstructorBlock *constructor_block = constructor_declaration -> constructor_body;

    if (! (constructor_block -> explicit_constructor_invocation_opt &&
           constructor_block -> explicit_constructor_invocation_opt -> ThisCallCast()))
    {
        constructor_block -> AllocateLocalInitStatements(local_type -> NumConstructorParameters());

        //
        // Generate an assignment statement for each local variable parameter.
        // Note that we do not initialize the this$0 here as the real constructor
        // will do that. If the local_type is static, its constructor_parameters
        // list does not start with this$0.
        //
        for (int i = (local_type -> ACC_STATIC() ? 0 : 1); i < local_type -> NumConstructorParameters(); i++)
        {
            VariableSymbol *param = local_type -> ConstructorParameter(i),
                           *symbol = block_symbol -> FindVariableSymbol(param -> Identity());
assert(symbol);
            AstSimpleName *lhs = compilation_unit -> ast_pool -> GenSimpleName(constructor_block -> left_brace_token);
            lhs -> symbol = param;

            AstSimpleName *rhs = compilation_unit -> ast_pool -> GenSimpleName(constructor_block -> left_brace_token);
            rhs -> symbol = symbol;

            AstAssignmentExpression *assign = compilation_unit -> ast_pool
                                                                -> GenAssignmentExpression(AstAssignmentExpression::EQUAL,
                                                                                            constructor_block -> left_brace_token);
            assign -> left_hand_side = lhs;
            assign -> expression     = rhs;
            assign -> symbol         = lhs -> Type();

            AstExpressionStatement *stmt = compilation_unit -> ast_pool -> GenExpressionStatement();
            stmt -> expression          = assign;
            stmt -> semicolon_token_opt = constructor_block -> left_brace_token;

            stmt -> is_reachable = true;
            stmt -> can_complete_normally = true;

            constructor_block -> AddLocalInitStatement(stmt);
        }
    }

    //
    //
    //
    AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(constructor_block -> left_brace_token);
    simple_name -> symbol = constructor;
assert(! constructor -> IsGeneratedLocalConstructor());

    AstMethodInvocation *method_call = compilation_unit -> ast_pool -> GenMethodInvocation();
    method_call -> method                  = simple_name;
    method_call -> left_parenthesis_token  = constructor_block -> left_brace_token;
    method_call -> right_parenthesis_token = constructor_block -> left_brace_token;
    method_call -> symbol                  = simple_name -> symbol;

    method_call -> AllocateArguments(constructor -> NumFormalParameters() + 1);
    if (! local_type -> ACC_STATIC())
    {
        AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(constructor_block -> left_brace_token);
        simple_name -> symbol = block_symbol -> FindVariableSymbol(control.this0_name_symbol);
assert(simple_name -> symbol);
        method_call -> AddArgument(simple_name);
    }

    for (int k = 0; k < constructor -> NumFormalParameters(); k++)
    {
        VariableSymbol *param = constructor -> FormalParameter(k),
                       *symbol = block_symbol -> FindVariableSymbol(param -> Identity());
        AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(constructor_block -> left_brace_token);
        simple_name -> symbol = symbol;
        method_call -> AddArgument(simple_name);
    }

    AstExpressionStatement *stmt = compilation_unit -> ast_pool -> GenExpressionStatement();
    stmt -> expression          = method_call;
    stmt -> semicolon_token_opt = constructor_block -> left_brace_token;

    stmt -> is_reachable = true;
    stmt -> can_complete_normally = true;

    constructor_block -> original_constructor_invocation = stmt;

    return;
}


void Semantic::UpdateLocalConstructors(TypeSymbol *inner_type)
{
    if (! ThisType() -> IsLocal()) // the method containing inner_type is not itself embedded in another method
    {
        //
        // Compute the set of local_classes we need to process here - they are
        // the inner_type itself and all the classes that are embedded in its body.
        //
        Tuple<TypeSymbol *> local_classes(8);

        TypeSymbol *outermost_type = inner_type -> outermost_type;
        if (outermost_type -> local) // The set of local types in the outermost type is not empty?
        {
            for (TypeSymbol *local_type = (TypeSymbol *) outermost_type -> local -> FirstElement();
                             local_type;
                             local_type = (TypeSymbol *) outermost_type -> local -> NextElement())
            {
                if (local_type -> CanAccess(inner_type))
                    local_classes.Next() = local_type;
            }
        }

        for (int j = 0; j < outermost_type -> num_anonymous_types(); j++)
        {
            if (outermost_type -> AnonymousType(j) -> CanAccess(inner_type))
                local_classes.Next() = outermost_type -> AnonymousType(j);
        }

        //
        // We now update each type T2 containing a call to a constructor of
        // T1 to make sure that T2 has a copy of or access to all the local
        // variables required by T1.
        //
        for (int k = 0; k < local_classes.Length(); k++)
        {
            TypeSymbol *target_local_type = local_classes[k];

            for (int i = 0; i < target_local_type -> NumLocalConstructorCallEnvironments(); i++)
            {
                SemanticEnvironment *env = target_local_type -> LocalConstructorCallEnvironment(i);
                TypeSymbol *source_local_type = env -> Type();
                if (! source_local_type -> CanAccess(target_local_type))
                {
                    for (int j = (target_local_type -> ACC_STATIC() ? 0 : 1);
                         j < target_local_type -> NumConstructorParameters(); j++)
                    {
                        VariableSymbol *local = target_local_type -> ConstructorParameter(j) -> accessed_local;
                        if (env -> symbol_table.FindVariableSymbol(local -> Identity()) != local)
                            source_local_type -> FindOrInsertLocalShadow(local);
                    }
                }
            }
        }

        //
        // Now update the constructor bodies to reflect the new local variable counts and mark the local_type completed.
        //
        for (int l = 0; l < local_classes.Length(); l++)
        {
            TypeSymbol *local_type = local_classes[l];

            AstClassDeclaration *class_declaration = local_type -> declaration -> ClassDeclarationCast();
            AstClassInstanceCreationExpression *class_creation = local_type -> declaration -> ClassInstanceCreationExpressionCast();
assert(class_declaration || class_creation);
            AstClassBody *class_body = (class_declaration ? class_declaration -> class_body : class_creation -> class_body_opt);

            if (class_body -> default_constructor)
                 UpdateGeneratedLocalConstructor(class_body -> default_constructor -> constructor_symbol);
            else
            {
                for (int i = 0; i < class_body -> NumConstructors(); i++)
                    UpdateGeneratedLocalConstructor(class_body -> Constructor(i) -> constructor_symbol);

                for (int k = 0; k < local_type -> NumPrivateAccessConstructors(); k++)
                    UpdateGeneratedLocalConstructor(local_type -> PrivateAccessConstructor(k));
            }

            local_type -> MarkLocalClassProcessingCompleted();
        }

        //
        // Now update the constructor calls
        //
        for (int m = 0; m < local_classes.Length(); m++)
        {
            TypeSymbol *target_local_type = local_classes[m];
assert(target_local_type -> LocalClassProcessingCompleted());

            for (int i = 0; i < target_local_type -> NumLocalConstructorCallEnvironments(); i++)
            {
                Ast *call = target_local_type -> LocalConstructorCallEnvironment(i) -> ast_construct;
                SemanticEnvironment *env = target_local_type -> LocalConstructorCallEnvironment(i);
                TypeSymbol *source_local_type = env -> Type();

                AstClassInstanceCreationExpression *class_creation;
                AstSuperCall *super_call;
                AstThisCall *this_call;

                if (class_creation = call -> ClassInstanceCreationExpressionCast())
                {
                    if (class_creation -> symbol != control.no_type)
                    {
                        if (source_local_type -> CanAccess(target_local_type))
                        {
                            for (int j = (target_local_type -> ACC_STATIC() ? 0 : 1);
                                 j < target_local_type -> NumConstructorParameters(); j++)
                            {
                                VariableSymbol *local = target_local_type -> ConstructorParameter(j) -> accessed_local;
                                AstSimpleName *simple_name = compilation_unit -> ast_pool
                                                                               -> GenSimpleName(class_creation -> new_token);
                                simple_name -> symbol = local;
                                if (source_local_type != target_local_type)
                                {
                                    state_stack.Push(source_local_type -> semantic_environment);
                                    CreateAccessToScopedVariable(simple_name, target_local_type);
                                    state_stack.Pop();
                                }
                                class_creation -> AddLocalArgument(simple_name);
                            }
                        }
                        else
                        {
                            for (int j = (target_local_type -> ACC_STATIC() ? 0 : 1);
                                 j < target_local_type -> NumConstructorParameters(); j++)
                            {
                                VariableSymbol *local = target_local_type -> ConstructorParameter(j) -> accessed_local;

                                AstSimpleName *simple_name = compilation_unit -> ast_pool
                                                                               -> GenSimpleName(class_creation -> new_token);
                                simple_name -> symbol = (env -> symbol_table.FindVariableSymbol(local -> Identity()) == local
                                                              ? local
                                                              : source_local_type -> FindOrInsertLocalShadow(local));
assert(simple_name -> symbol -> VariableCast());
                                class_creation -> AddLocalArgument(simple_name);
                            }
                        }

                        MethodSymbol *constructor = (MethodSymbol *) class_creation -> class_type -> symbol;
assert(constructor);
assert(constructor -> MethodCast());
assert(! constructor -> IsGeneratedLocalConstructor());
assert(constructor -> LocalConstructor());
                        class_creation -> class_type -> symbol = constructor -> LocalConstructor();
                    }
                }
                else if (super_call = call -> SuperCallCast())
                {
                    if (super_call -> symbol -> MethodCast())
                    {
                        for (int j = (target_local_type -> ACC_STATIC() ? 0 : 1);
                             j < target_local_type -> NumConstructorParameters(); j++)
                        {
                            VariableSymbol *local = target_local_type -> ConstructorParameter(j) -> accessed_local;

                            AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(super_call -> super_token);
                            simple_name -> symbol = env -> symbol_table.FindVariableSymbol(local -> Identity());
assert(simple_name -> symbol -> VariableCast()); 
                            super_call -> AddLocalArgument(simple_name);
                        }

                        MethodSymbol *constructor = (MethodSymbol *) super_call -> symbol;
assert(constructor -> MethodCast() && (! constructor -> IsGeneratedLocalConstructor()));
assert(constructor -> LocalConstructor());
                        super_call -> symbol = constructor -> LocalConstructor();
                    }
                }
                else
                {
                    this_call = (AstThisCall *) call;
assert(this_call -> ThisCallCast());

                    if (this_call -> symbol -> MethodCast())
                    {
                        for (int j = (target_local_type -> ACC_STATIC() ? 0 : 1);
                             j < target_local_type -> NumConstructorParameters(); j++)
                        {
                            VariableSymbol *local = target_local_type -> ConstructorParameter(j) -> accessed_local;

                            AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(this_call -> this_token);
                            simple_name -> symbol = env -> symbol_table.FindVariableSymbol(local -> Identity());
assert(simple_name -> symbol -> VariableCast()); 
                            this_call -> AddLocalArgument(simple_name);
                        }

                        MethodSymbol *constructor = (MethodSymbol *) this_call -> symbol;
assert(constructor -> MethodCast() && (! constructor -> IsGeneratedLocalConstructor()));
assert(constructor -> LocalConstructor());
                        this_call -> symbol = constructor -> LocalConstructor();
                    }
                }
            }
        }
    }

    return;
}


void Semantic::GetAnonymousConstructor(AstClassInstanceCreationExpression *class_creation, TypeSymbol *anonymous_type)
{
    LexStream::TokenIndex left_loc  = class_creation -> class_type -> LeftToken(),
                          right_loc = class_creation -> right_parenthesis_token;

    TypeSymbol *super_type = anonymous_type -> super;
    MethodSymbol *super_constructor = FindConstructor(super_type, class_creation, left_loc, right_loc);
    if (! super_constructor)
    {
        class_creation -> class_type -> symbol = control.no_type;
        return;
    }

    //
    // Make constructor symbol. The associated symbol table will not contain too many elements...
    //
    BlockSymbol *block_symbol = new BlockSymbol(super_constructor -> NumFormalParameters() + 3);
    block_symbol -> max_variable_index = 1; // All types need a spot for "this".

    MethodSymbol *constructor = anonymous_type -> InsertConstructorSymbol(control.init_name_symbol);
    constructor -> SetType(control.void_type);
    constructor -> SetContainingType(anonymous_type);
    constructor -> SetBlockSymbol(block_symbol);
    constructor -> SetACC_PUBLIC();

    //
    // Report error is super constructor has throws clause, but add the exceptions to the local throws
    // clause to avoid spurious errors later !!!
    //
    int num_throws = super_constructor -> NumThrows((Semantic *) this, left_loc);
    if (num_throws > 0)
    {
        for (int i = 0; i < num_throws; i++)
        {
            TypeSymbol *exception = super_constructor -> Throws(i);
            ReportSemError(SemanticError::DEFAULT_CONSTRUCTOR_CANNOT_THROW,
                          left_loc,
                          right_loc,
                          exception -> ContainingPackage() -> PackageName(),
                          exception -> ExternalName());
            constructor -> AddThrows(exception);
        }
    }

    VariableSymbol *this0_variable = NULL;
    if (anonymous_type -> IsInner())
    {
        this0_variable = block_symbol -> InsertVariableSymbol(control.this0_name_symbol);
        this0_variable -> SetType(anonymous_type -> ContainingType());
        this0_variable -> SetOwner(constructor);
        this0_variable -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
    }

    for (int j = 0; j < super_constructor -> NumFormalParameters(); j++)
    {
        VariableSymbol *param = super_constructor -> FormalParameter(j),
                       *symbol = block_symbol -> InsertVariableSymbol(param -> Identity());
        symbol -> SetType(param -> Type());
        symbol -> SetOwner(constructor);
        symbol -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
        if (symbol -> Type() == control.long_type || symbol -> Type() == control.double_type)
            block_symbol -> max_variable_index++;
        constructor -> AddFormalParameter(symbol);
    }

    //
    //
    //
    AstSuperCall *super_call             = compilation_unit -> ast_pool -> GenSuperCall();
    super_call -> base_opt                = class_creation -> base_opt; // save initial base_opt
    super_call -> dot_token_opt           = class_creation -> new_token;
    super_call -> super_token             = class_creation -> new_token;
    super_call -> left_parenthesis_token  = class_creation -> new_token;
    super_call -> right_parenthesis_token = class_creation -> new_token;
    super_call -> semicolon_token         = class_creation -> new_token;

    super_call -> is_reachable            = true;
    super_call -> can_complete_normally   = true;
    super_call -> symbol                  = super_constructor;

    //
    // If it is in a static region, the anonymous class does not need a this$0 argument
    //
    class_creation -> base_opt = (anonymous_type -> ACC_STATIC()
                                           ? (AstExpression *) NULL
                                           : CreateAccessToType(class_creation, anonymous_type -> ContainingType()));

    AstClassBody *class_body = class_creation -> class_body_opt;

    AstReturnStatement *return_statement = compilation_unit -> ast_pool -> GenReturnStatement();
    return_statement -> return_token = class_body -> left_brace_token;
    return_statement -> expression_opt = NULL;
    return_statement -> semicolon_token = class_body -> left_brace_token;
    return_statement -> is_reachable = true;

    AstBlock *block = compilation_unit -> ast_pool -> GenBlock();
    block -> block_symbol = constructor -> block_symbol -> InsertBlockSymbol(1); // this symbol table will be empty
    block -> AllocateBlockStatements(1); // this block contains one statement
    block -> left_brace_token  = class_body -> left_brace_token;
    block -> right_brace_token = class_body -> left_brace_token;

    block -> is_reachable = true;
    block -> can_complete_normally = false;
    block -> AddStatement(return_statement);

    AstConstructorBlock *constructor_block                  = compilation_unit -> ast_pool -> GenConstructorBlock();
    constructor_block -> left_brace_token                    = class_body -> left_brace_token;
    constructor_block -> explicit_constructor_invocation_opt = super_call;
    constructor_block -> block                               = block;
    constructor_block -> right_brace_token                   = class_body -> left_brace_token;

    AstMethodDeclarator *method_declarator      = compilation_unit -> ast_pool -> GenMethodDeclarator();
    method_declarator -> identifier_token        = left_loc;
    method_declarator -> left_parenthesis_token  = class_creation -> left_parenthesis_token;
    method_declarator -> right_parenthesis_token = right_loc;

    AstConstructorDeclaration *constructor_declaration = compilation_unit -> ast_pool -> GenConstructorDeclaration();
    constructor_declaration -> constructor_declarator   = method_declarator;
    constructor_declaration -> constructor_body         = constructor_block;

    constructor_declaration -> constructor_symbol = constructor;
    constructor -> method_or_constructor_declaration = constructor_declaration;

    //
    // Note that the constructor for the anonymous type is not added to the class body here
    // beacause we've already completely compiled it and the arguments to its super call
    // do not contain "valid" SimpleName Ast expressions. It is added to the constructor
    // body later in get_anonymous_type...
    //
    // class_body -> default_constructor = constructor_declaration;
    //
    VariableSymbol *super_this0_variable = NULL;

    if (anonymous_type -> IsLocal())
    {
        GenerateLocalConstructor(constructor);

        MethodSymbol *generated_constructor = constructor -> LocalConstructor();
assert(! constructor -> IsGeneratedLocalConstructor());
assert(generated_constructor);
        block_symbol = generated_constructor -> block_symbol; // use the environment of the generated constructor...

        if (super_call -> base_opt)
        {
            //
            // Add the this$0 parameter for the super type. However, only mark it complete and
            // do not yet assign a number to it. This will be done after we know
            // how many extra "local" variable shadows are needed. See UpdateGeneratedLocalConstructor
            //
            super_this0_variable = block_symbol -> InsertVariableSymbol(control.MakeParameter(0));
            super_this0_variable -> SetType(super_call -> base_opt -> Type());
            super_this0_variable -> SetOwner(generated_constructor);
            super_this0_variable -> MarkComplete();
        }

        if (super_type -> IsLocal()) // a local type may use enclosed local variables?
        {
            if (super_type -> LocalClassProcessingCompleted())
            {
                //
                // TODO: Should we set the size for the super_call arguments here ???
                //
                for (int i = 1; i < super_type -> NumConstructorParameters(); i++)
                {
                    VariableSymbol *local = super_type -> ConstructorParameter(i) -> accessed_local;

                    AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(super_call -> super_token);

                    anonymous_type -> FindOrInsertLocalShadow(local);
                    simple_name -> symbol = block_symbol -> FindVariableSymbol(local -> Identity());
assert(simple_name -> symbol);
                    super_call -> AddLocalArgument(simple_name);
                }
assert(super_constructor -> LocalConstructor() && (! super_constructor -> IsGeneratedLocalConstructor()));
                super_call -> symbol = super_constructor -> LocalConstructor();
            }
            else // are we currently within the body of the type in question ?
                super_type -> AddLocalConstructorCallEnvironment(GetEnvironment(super_call));
        }
    }
    else if (super_call -> base_opt)
    {
        super_this0_variable = block_symbol -> InsertVariableSymbol(control.MakeParameter(0));
        super_this0_variable -> SetType(super_call -> base_opt -> Type());
        super_this0_variable -> SetOwner(constructor);
        super_this0_variable -> SetLocalVariableIndex(block_symbol -> max_variable_index++);

        constructor -> AddFormalParameter(super_this0_variable);
    }

    //
    // Complete the definition of the constructor and update the super call accordingly.
    //
    if (super_this0_variable)
    {
        class_creation -> AddArgument(super_call -> base_opt); // pass the original base expression as argument to anonymous class.

        AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(class_creation -> new_token);
        simple_name -> symbol = super_this0_variable;
        super_call -> base_opt = simple_name; // pass the base expression argument to the super class
    }

    constructor -> SetSignature(control, this0_variable); // we now have all the information to set the signature of the constructor.

    //
    // Are we guaranteed to have all the info available here? Yes,
    // because if the anonymous type is not local to a method, then its super
    // type cannot be local to a method. Therefore, no extra argument (other than
    // the proper this$0 specified in the base) is needed. If on the other hand the
    // anonymous type is local and its supertype is also local, it must have appeared
    // before the anonymous type and therefore its information has already been computed.
    //
    for (int k = 0; k < super_constructor -> NumFormalParameters(); k++)
    {
        VariableSymbol *param = super_constructor -> FormalParameter(k),
                       *symbol = block_symbol -> FindVariableSymbol(param -> Identity());
assert(symbol);
        AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(class_creation -> new_token);
        simple_name -> symbol = symbol;
        super_call -> AddArgument(simple_name);
    }

    class_creation -> class_type -> symbol = constructor;

    return;
}


TypeSymbol *Semantic::GetAnonymousType(AstClassInstanceCreationExpression *class_creation, TypeSymbol *super_type)
{
    TypeSymbol *this_type = ThisType();

    if (super_type -> ACC_FINAL())
    {
         ReportSemError(SemanticError::SUPER_IS_FINAL,
                        class_creation -> class_type -> LeftToken(),
                        class_creation -> class_type -> RightToken(),
                        super_type -> ContainingPackage() -> PackageName(),
                        super_type -> ExternalName());
    }

    AstClassBody *class_body = class_creation -> class_body_opt;
    TypeSymbol *outermost_type = this_type -> outermost_type;

    //
    // Make up a proper name for the anonymous type
    //
    int num = outermost_type -> num_anonymous_types() + 1;
    wchar_t str[11],
            *p = &str[10];
    *p = U_NULL;
    do
    {
        p--;
        *p = U_0 + (num % 10);
        num /= 10;
    } while (num > 0);

    int length = wcslen(p) + outermost_type -> NameLength() + 1; // +1 for $
    wchar_t *anonymous_name = new wchar_t[length + 1];
    wcscpy(anonymous_name, outermost_type -> Name());
    wcscat(anonymous_name, StringConstant::US__DS_);
    wcscat(anonymous_name, p);

    NameSymbol *name_symbol = control.FindOrInsertName(anonymous_name, length);

assert((! ThisMethod()) || LocalSymbolTable().Top());
    TypeSymbol *inner_type = (ThisMethod() ? LocalSymbolTable().Top() -> InsertAnonymousTypeSymbol(name_symbol)
                                           : this_type -> InsertAnonymousTypeSymbol(name_symbol));
    inner_type -> MarkAnonymous();
    inner_type -> outermost_type = outermost_type;
    inner_type -> supertypes_closure = new SymbolSet;
    inner_type -> subtypes_closure = new SymbolSet;
    inner_type -> semantic_environment = new SemanticEnvironment((Semantic *) this, inner_type, state_stack.Top());
    inner_type -> declaration = class_creation;
    inner_type -> file_symbol = source_file_symbol;
    inner_type -> SetOwner(ThisMethod() ? (Symbol *) ThisMethod() : (Symbol *) this_type);
    //
    // Add 3 extra elements for padding. May need a default constructor and other support elements.
    //
    inner_type -> SetSymbolTable(class_body -> NumClassBodyDeclarations() + 3);
    inner_type -> SetLocation();
    inner_type -> SetSignature(control);

    //
    // TODO: As an anonymous type cannot be a super class, it makes sense to mark
    // is final. This allows jikes to be consistent with javac in emitting an
    // error message when the anonymous class is checked in an instanceof
    // operation against an interface. However, this fact is not documented
    // in the 1.1 document. Furthermore, the class file that is emitted for an
    // anonymous flag (when processed by javac) does not have the FINAL flag turned on.
    // We also turn this flag off after processing the body of the anonymmous type.
    // See bolow...
    //
    inner_type -> SetACC_FINAL();

    if (StaticRegion())
         inner_type -> SetACC_STATIC();
    else inner_type -> InsertThis(0);

    if (super_type -> ACC_INTERFACE())
    {
         inner_type -> AddInterface(super_type);
         inner_type -> super = control.Object();
    }
    else inner_type -> super = super_type;

    outermost_type -> AddAnonymousType(inner_type);
    delete [] anonymous_name;

    //
    //
    //
    GetAnonymousConstructor(class_creation, inner_type);

    //
    // Now process the body of the anonymous class !!!
    //
    CheckClassMembers(inner_type, class_body);
    ProcessNestedTypeHeaders(inner_type, class_body);
    if (inner_type -> owner -> MethodCast())
         ProcessSuperTypesOfOuterType(inner_type);
    else ProcessNestedSuperTypes(inner_type);

    //
    // If the class body has not yet been parsed, do so now.
    //
    if (class_body -> UnparsedClassBodyCast())
    {
        if (! control.parser -> InitializerParse(lex_stream, class_body))
             compilation_unit -> kind = Ast::BAD_COMPILATION; // mark the fact that syntax errors were detected
        else
        {
            inner_type -> MarkHeaderProcessed();
            ProcessMembers(inner_type -> semantic_environment, class_body);
            CompleteSymbolTable(inner_type -> semantic_environment, class_body -> left_brace_token, class_body);
        }

        if (! control.parser -> BodyParse(lex_stream, class_body))
             compilation_unit -> kind = Ast::BAD_COMPILATION; // mark the fact that syntax errors were detected
        else ProcessExecutableBodies(inner_type -> semantic_environment, class_body);
    }
    else // The relevant bodies have already been parsed
    {
        inner_type -> MarkHeaderProcessed();
        ProcessMembers(inner_type -> semantic_environment, class_body);
        CompleteSymbolTable(inner_type -> semantic_environment, class_body -> left_brace_token, class_body);
        ProcessExecutableBodies(inner_type -> semantic_environment, class_body);
    }

    //
    // Add the default constructor to the body of the anonymous type.
    // If the symbol was resolve to "no_type" then constructor will be NULL
    //
    MethodSymbol *constructor = class_creation -> class_type -> symbol -> MethodCast();
    if (constructor)
    {
        class_body -> default_constructor = (AstConstructorDeclaration *) constructor -> method_or_constructor_declaration;

        if (inner_type -> IsLocal())
        {
            inner_type -> AddLocalConstructorCallEnvironment(GetEnvironment(class_creation));
            UpdateLocalConstructors(inner_type);
        }
    }

    //
    // TODO: See comment above regarding the setting of this flag.
    //
    inner_type -> ResetACC_FINAL();

    return inner_type;
}


void Semantic::ProcessClassInstanceCreationExpression(Ast *expr)
{
    AstClassInstanceCreationExpression *class_creation = (AstClassInstanceCreationExpression *) expr;

    if (class_creation -> base_opt || class_creation -> class_body_opt)
    {
        if (! control.option.one_one)
        {
            ReportSemError(SemanticError::ONE_ONE_FEATURE,
                           class_creation -> LeftToken(),
                           class_creation -> RightToken());
        }
    }

    //
    // This operation may throw OutOfMemoryError
    //
    SymbolSet *exception_set = TryExceptionTableStack().Top();
    if (exception_set)
    {
        exception_set -> AddElement(control.RuntimeException());
        exception_set -> AddElement(control.Error());
    }

    Ast *actual_type = class_creation -> class_type -> type;
    TypeSymbol *type;
    if (class_creation -> base_opt)
    {
        ProcessExpression(class_creation -> base_opt);

        TypeSymbol *enclosing_type = class_creation -> base_opt -> Type();
        if (enclosing_type == control.no_type)
        {
            class_creation -> symbol = control.no_type;
            return;
        }
        else if (enclosing_type == control.null_type || enclosing_type -> Primitive())
        {
            ReportSemError(SemanticError::TYPE_NOT_REFERENCE,
                           class_creation -> base_opt -> LeftToken(),
                           class_creation -> base_opt -> RightToken(),
                           enclosing_type -> ExternalName());
            class_creation -> symbol = control.no_type;
            return;
        }

        //
        // The grammar guarantees that the actual type is a simple name.
        //
        type = MustFindNestedType(enclosing_type, actual_type);
        if (type -> ACC_STATIC())
        {
            ReportSemError(SemanticError::STATIC_NOT_INNER_CLASS,
                           actual_type -> LeftToken(),
                           actual_type -> RightToken(),
                           type -> ContainingPackage() -> PackageName(),
                           type -> ExternalName());
        }
    }
    else
    {
        type = MustFindType(actual_type);
        if (type -> IsInner())
            class_creation -> base_opt = CreateAccessToType(class_creation, type -> ContainingType());
    }

 
    bool no_bad_argument = true;
    for (int i = 0; i < class_creation -> NumArguments(); i++)
    {
        AstExpression *expr = class_creation -> Argument(i);
        ProcessExpression(expr); // TODO: ProcessExpressionOrStringConstant(expr);
        no_bad_argument = no_bad_argument && (expr -> symbol != control.no_type);
    }

    TypeSymbol *anonymous_type = NULL;

    if (! no_bad_argument)
    {
        class_creation -> class_type -> symbol = control.no_type;
        class_creation -> symbol = type;
    }
    else
    {
        MethodSymbol *method = FindConstructor((type -> ACC_INTERFACE() ? control.Object() : type),
                                                class_creation,
                                                actual_type -> LeftToken(),
                                                class_creation -> right_parenthesis_token);

        if (! method)
        {
            class_creation -> class_type -> symbol = control.no_type;
            class_creation -> symbol = type;
        }
        else
        {
            if (class_creation -> base_opt &&
                (class_creation -> base_opt -> symbol != control.no_type) && 
                (class_creation -> base_opt -> Type() != method -> containing_type -> ContainingType()))
            {
assert(method -> containing_type);
assert(method -> containing_type -> ContainingType());
assert(class_creation -> base_opt -> Type());
assert(CanMethodInvocationConvert(method -> containing_type -> ContainingType(), class_creation -> base_opt -> Type()));
                class_creation -> base_opt = ConvertToType(class_creation -> base_opt, method -> containing_type -> ContainingType());
            }

            for (int i = 0; i < class_creation -> NumArguments(); i++)
            {
                AstExpression *expr = class_creation -> Argument(i);
                if (expr -> Type() != method -> FormalParameter(i) -> Type())
                    class_creation -> Argument(i) = ConvertToType(expr, method -> FormalParameter(i) -> Type());
            }        

            if (class_creation -> class_body_opt)
                anonymous_type = GetAnonymousType(class_creation, type);
            else
            {
                if (type -> ACC_INTERFACE())
                {
                    ReportSemError(SemanticError::NOT_A_CLASS,
                                   actual_type -> LeftToken(),
                                   actual_type -> RightToken(),
                                   type -> ContainingPackage() -> PackageName(),
                                   type -> ExternalName());
                    class_creation -> symbol = control.no_type;
                    return;
                }
                else if (type -> ACC_ABSTRACT())
                {
                    ReportSemError(SemanticError::ABSTRACT_TYPE_CREATION,
                                   actual_type -> LeftToken(),
                                   actual_type -> RightToken(),
                                   type -> ExternalName());
                }

                class_creation -> class_type -> symbol = method;

                if (exception_set)
                {
                    for (int i = method -> NumThrows((Semantic *) this, actual_type -> RightToken()) - 1; i >= 0; i--)
                        exception_set -> AddElement(method -> Throws(i));
                }

                for (int k = method -> NumThrows((Semantic *) this, actual_type -> RightToken()) - 1; k >= 0; k--)
                {
                    TypeSymbol *exception = method -> Throws(k);
                    if (! CatchableException(exception))
                    {
                        ReportSemError(SemanticError::UNCATCHABLE_CONSTRUCTOR_THROWN_CHECKED_EXCEPTION,
                                       actual_type -> LeftToken(),
                                       actual_type -> RightToken(),
                                       type -> ExternalName(),
                                       exception -> ContainingPackage() -> PackageName(),
                                       exception -> ExternalName());
                    }
                }
            }

            class_creation -> symbol = (anonymous_type ? anonymous_type : type);

            if (method -> ACC_PRIVATE() && ThisType() != type && ThisType() -> outermost_type == type -> outermost_type)
            {
                //
                // TODO: Awaiting language clarification.
                //
                ReportSemError(SemanticError::PRIVATE_ENCLOSED_CONSTRUCTOR,
                               class_creation -> new_token,
                               class_creation -> right_parenthesis_token,
                               method -> Header());

                method = TypeSymbol::GetReadAccessMethod(method);
                class_creation -> class_type -> symbol = method;
            }
            else ConstructorAccessCheck(class_creation, method);

            //
            // A local type may use enclosed local variables. So, we at least allocate the 
            // space for adding these extra arguments. If the type being created has already been
            // fully processed, add the extra arguments here.
            //
            if ((! anonymous_type) && type -> IsLocal())
            {
                if (type -> LocalClassProcessingCompleted() && method -> LocalConstructor())
                {
assert(! method -> IsGeneratedLocalConstructor());
                    class_creation -> class_type -> symbol = method -> LocalConstructor();
assert(method -> LocalConstructor() -> signature);

                    //
                    // Are we currently within the body of the method that contains
                    // the local type in question?
                    //
                    if (type -> owner == ThisMethod())
                    {
                        for (int i = (type -> ACC_STATIC() ? 0 : 1); i < type -> NumConstructorParameters(); i++)
                        {
                            VariableSymbol *local = type -> ConstructorParameter(i) -> accessed_local;

                            AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(class_creation -> new_token);
                            simple_name -> symbol = local;
                            class_creation -> AddLocalArgument(simple_name);
                        }
                    }
                    else
                    {
                        for (int i = (type -> ACC_STATIC() ? 0 : 1); i < type -> NumConstructorParameters(); i++)
                        {
                            VariableSymbol *local = type -> ConstructorParameter(i) -> accessed_local;

                            AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(class_creation -> new_token);
                            simple_name -> symbol = ThisType() -> FindOrInsertLocalShadow(local);
                            class_creation -> AddLocalArgument(simple_name);
                        }
                    }
                }
                else // are we within body of type in question? Save processing for later. See ProcessClassDeclaration in body.cpp
                    type -> AddLocalConstructorCallEnvironment(GetEnvironment(class_creation));
            }
        }
    }

    return;
}


void Semantic::ProcessArrayCreationExpression(Ast *expr)
{
    AstArrayCreationExpression *array_creation = (AstArrayCreationExpression *) expr;

    //
    // This operation may throw OutOfMemoryError or NegativeArraySizeException
    //
    SymbolSet *exception_set = TryExceptionTableStack().Top();
    if (exception_set)
    {
        exception_set -> AddElement(control.RuntimeException());
        exception_set -> AddElement(control.Error());
    }

    AstArrayType *array_type;

    TypeSymbol *type;

    if (array_type = array_creation -> array_type -> ArrayTypeCast())
    {
        if (! control.option.one_one)
        {
            ReportSemError(SemanticError::ONE_ONE_FEATURE,
                           array_creation -> LeftToken(),
                           array_creation -> RightToken());
        }

        AstPrimitiveType *primitive_type = array_type -> type -> PrimitiveTypeCast();
        type = (primitive_type ? FindPrimitiveType(primitive_type) : MustFindType(array_type -> type));
    }
    else
    {
        AstPrimitiveType *primitive_type = array_creation -> array_type -> PrimitiveTypeCast();
        type = (primitive_type ? FindPrimitiveType(primitive_type) : MustFindType(array_creation -> array_type));
    }

    int num_dimensions = (array_type ? array_type -> NumBrackets()
                                     : array_creation -> NumDimExprs() + array_creation -> NumBrackets());

    if (num_dimensions > 0)
        type = type -> GetArrayType((Semantic *) this, num_dimensions);
    array_creation -> symbol = type;

    for (int i = 0; i < array_creation -> NumDimExprs(); i++)
    {
        AstDimExpr *dim_expr = array_creation -> DimExpr(i);
        ProcessExpression(dim_expr -> expression);
        AstExpression *expr = PromoteUnaryNumericExpression(dim_expr -> expression);
        if (expr -> Type() != control.int_type && expr -> symbol != control.no_type)
        {
            ReportSemError(SemanticError::TYPE_NOT_INTEGER,
                           dim_expr -> expression -> LeftToken(),
                           dim_expr -> expression -> RightToken(),
                           dim_expr -> expression -> Type() -> Name());
        }
        dim_expr -> expression = expr;
    }

    if (array_creation -> array_initializer_opt)
        ProcessArrayInitializer((AstArrayInitializer *) array_creation -> array_initializer_opt, type);

    return;
}


void Semantic::ProcessPostUnaryExpression(Ast *expr)
{
    AstPostUnaryExpression *postfix_expression = (AstPostUnaryExpression *) expr;

    ProcessExpression(postfix_expression -> expression);
    postfix_expression -> symbol = postfix_expression -> expression -> symbol;

    if (postfix_expression -> symbol != control.no_type)
    {
        if (! postfix_expression -> expression -> IsLeftHandSide())
        {
            ReportSemError(SemanticError::NOT_A_NUMERIC_VARIABLE,
                           postfix_expression -> expression -> LeftToken(),
                           postfix_expression -> expression -> RightToken(),
                           postfix_expression -> expression -> Type() -> Name());
            postfix_expression -> symbol = control.no_type;
        }
        else if (! control.IsNumeric(postfix_expression -> Type()))
        {
            ReportSemError(SemanticError::TYPE_NOT_NUMERIC,
                           postfix_expression -> expression -> LeftToken(),
                           postfix_expression -> expression -> RightToken(),
                           postfix_expression -> Type() -> Name());
            postfix_expression -> symbol = control.no_type;
        }
        else if (! postfix_expression -> expression -> ArrayAccessCast()) // some kind of name
        {
            MethodSymbol *read_method = NULL;
            AstSimpleName *simple_name = postfix_expression -> expression -> SimpleNameCast();
            if (simple_name)
            {
                if (simple_name -> resolution_opt)
                   read_method = simple_name -> resolution_opt -> symbol -> MethodCast();
            }
            else
            {
                AstFieldAccess *field_access = (AstFieldAccess *) postfix_expression -> expression;
                if (field_access -> resolution_opt)
                    read_method = field_access -> resolution_opt -> symbol -> MethodCast();
            }

            VariableSymbol *variable_symbol;
            if (read_method)
            {
                variable_symbol = (VariableSymbol *) read_method -> accessed_member;
                postfix_expression -> write_method = TypeSymbol::GetWriteAccessMethod(variable_symbol);
            }
            else variable_symbol = postfix_expression -> expression -> symbol -> VariableCast();
        }
    }

    return;
}


void Semantic::ProcessPLUS(AstPreUnaryExpression *expr)
{
    ProcessExpression(expr -> expression);
    expr -> expression = PromoteUnaryNumericExpression(expr -> expression);
    expr -> value = expr -> expression -> value;
    expr -> symbol = expr -> expression -> symbol;
}


void Semantic::ProcessMINUS(AstPreUnaryExpression *expr)
{
    AstIntegerLiteral *int_literal;
    AstLongLiteral *long_literal;

    if (int_literal = expr -> expression -> IntegerLiteralCast())
    {
        LiteralSymbol *literal = (LiteralSymbol *) lex_stream -> NameSymbol(int_literal -> integer_literal_token);

        expr -> value = control.int_pool.FindOrInsertNegativeInt(literal);
        if (expr -> value == &control.bad_value)
        {
            ReportSemError(SemanticError::INVALID_INT_VALUE,
                           expr -> LeftToken(),
                           expr -> RightToken());
            expr -> symbol = control.no_type;
        }
        else expr -> symbol = control.int_type;
    }
    else if (long_literal = expr -> expression -> LongLiteralCast())
    {
        LiteralSymbol *literal = (LiteralSymbol *) lex_stream -> NameSymbol(long_literal -> long_literal_token);

        expr -> value = control.long_pool.FindOrInsertNegativeLong(literal);
        if (expr -> value == &control.bad_value)
        {
            ReportSemError(SemanticError::INVALID_LONG_VALUE,
                           expr -> LeftToken(),
                           expr -> RightToken());
            expr -> symbol = control.no_type;
        }
        else expr -> symbol = control.long_type;
    }
    else
    {
        ProcessExpression(expr -> expression);

        expr -> expression = PromoteUnaryNumericExpression(expr -> expression);
        expr -> symbol = expr -> expression -> symbol;
        if (expr -> expression -> IsConstant())
        {
            TypeSymbol *type = expr -> Type();

            if (type == control.double_type)
            {
                DoubleLiteralValue *literal = (DoubleLiteralValue *) expr -> expression -> value;
                expr -> value = control.double_pool.FindOrInsert(-literal -> value);
            }
            else if (type == control.float_type)
            {
                FloatLiteralValue *literal = (FloatLiteralValue *) expr -> expression -> value;
                expr -> value = control.float_pool.FindOrInsert(-literal -> value);
            }
            else if (type == control.long_type)
            {
                LongLiteralValue *literal = (LongLiteralValue *) expr -> expression -> value;
                expr -> value = control.long_pool.FindOrInsert(-literal -> value);
            }
            else
            {
                IntLiteralValue *literal = (IntLiteralValue *) expr -> expression -> value;
                expr -> value = control.int_pool.FindOrInsert(-literal -> value);
            }
        }
    }

    return;
}


void Semantic::ProcessTWIDDLE(AstPreUnaryExpression *expr)
{
    ProcessExpression(expr -> expression);

    if (expr -> expression -> symbol != control.no_type && (! control.IsIntegral(expr -> expression -> Type())))
    {
        ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                       expr -> expression -> LeftToken(),
                       expr -> expression -> RightToken(),
                       expr -> expression -> Type() -> Name());
        expr -> symbol = control.no_type;
    }
    else
    {
        expr -> expression = PromoteUnaryNumericExpression(expr -> expression);

        if (expr -> expression -> IsConstant())
        {
            if (expr -> expression -> Type() == control.long_type)
            {
                LongLiteralValue *literal = (LongLiteralValue *) expr -> expression -> value;
                expr -> value = control.long_pool.FindOrInsert(~literal -> value);
            }
            else // assert(expr -> expression -> Type() == control.int_type)
            {
                IntLiteralValue *literal = (IntLiteralValue *) expr -> expression -> value;
                expr -> value = control.int_pool.FindOrInsert(~literal -> value);
            }
        }
        expr -> symbol = expr -> expression -> symbol;
    }

    return;
}


void Semantic::ProcessNOT(AstPreUnaryExpression *expr)
{
    ProcessExpression(expr -> expression);

    if (expr -> expression -> symbol != control.no_type && expr -> expression -> Type() != control.boolean_type)
    {
        ReportSemError(SemanticError::TYPE_NOT_BOOLEAN,
                       expr -> expression -> LeftToken(),
                       expr -> expression -> RightToken(),
                       expr -> expression -> Type() -> Name());
        expr -> symbol = control.no_type;
    }
    else
    {
        if (expr -> expression -> IsConstant())
        {
            IntLiteralValue *literal = (IntLiteralValue *) expr -> expression -> value;
            expr -> value = control.int_pool.FindOrInsert(literal -> value ? 0 : 1);
        }
        expr -> symbol = control.boolean_type;
    }

    return;
}


void Semantic::ProcessPLUSPLUSOrMINUSMINUS(AstPreUnaryExpression *expr)
{
    ProcessExpression(expr -> expression);

    if (expr -> expression -> symbol != control.no_type)
    {
        if (! expr -> expression -> IsLeftHandSide())
        {
            ReportSemError(SemanticError::NOT_A_NUMERIC_VARIABLE,
                           expr -> expression -> LeftToken(),
                           expr -> expression -> RightToken(),
                           expr -> expression -> Type() -> Name());
            expr -> symbol = control.no_type;
        }
        else if (! control.IsNumeric(expr -> expression -> Type()))
        {
            ReportSemError(SemanticError::TYPE_NOT_NUMERIC,
                           expr -> expression -> LeftToken(),
                           expr -> expression -> RightToken(),
                           expr -> expression -> Type() -> Name());
            expr -> symbol = control.no_type;
        }
        else if (! expr -> expression -> ArrayAccessCast()) // some kind of name
        {
            MethodSymbol *read_method = NULL;
            AstSimpleName *simple_name = expr -> expression -> SimpleNameCast();
            if (simple_name)
            {
                if (simple_name -> resolution_opt)
                   read_method = simple_name -> resolution_opt -> symbol -> MethodCast();
            }
            else
            {
                AstFieldAccess *field_access = (AstFieldAccess *) expr -> expression;
                if (field_access -> resolution_opt)
                    read_method = field_access -> resolution_opt -> symbol -> MethodCast();
            }

            VariableSymbol *variable_symbol;
            if (read_method)
            {
                variable_symbol = (VariableSymbol *) read_method -> accessed_member;
                expr -> write_method = TypeSymbol::GetWriteAccessMethod(variable_symbol);
            }
            else variable_symbol = expr -> expression -> symbol -> VariableCast();
        }
    }
    expr -> symbol = expr -> expression -> symbol;

    return;
}


void Semantic::ProcessPreUnaryExpression(Ast *expr)
{
    AstPreUnaryExpression *prefix_expression = (AstPreUnaryExpression *) expr;
    (this ->* ProcessPreUnaryExpr[prefix_expression -> pre_unary_tag])(prefix_expression);

    return;
}


inline bool Semantic::CanWideningPrimitiveConvert(TypeSymbol *target_type, TypeSymbol *source_type)
{
    if (target_type == control.double_type)
         return (source_type == control.float_type || source_type == control.long_type  || source_type == control.int_type ||
                 source_type == control.char_type  || source_type == control.short_type || source_type == control.byte_type);
    else if (target_type == control.float_type)
         return (source_type == control.long_type  || source_type == control.int_type   ||
                 source_type == control.char_type  || source_type == control.short_type || source_type == control.byte_type);
    else if (target_type == control.long_type)
         return (source_type == control.int_type   || source_type == control.char_type  ||
                 source_type == control.short_type || source_type == control.byte_type);
    else if (target_type == control.int_type)
         return (source_type == control.char_type  || source_type == control.short_type || source_type == control.byte_type);
    else if (target_type == control.short_type)
         return source_type == control.byte_type;

    return false;
}


inline bool Semantic::CanNarrowingPrimitiveConvert(TypeSymbol *target_type, TypeSymbol *source_type)
{
    if (target_type == control.byte_type)
         return (source_type == control.double_type || source_type == control.float_type || source_type == control.long_type ||
                 source_type == control.int_type    || source_type == control.char_type  || source_type == control.short_type);
    else if (target_type == control.char_type)
         return (source_type == control.double_type || source_type == control.float_type || source_type == control.long_type ||
                 source_type == control.int_type    || source_type == control.short_type || source_type == control.byte_type);
    else if (target_type == control.short_type)
         return (source_type == control.double_type || source_type == control.float_type ||
                 source_type == control.long_type   || source_type == control.int_type   || source_type == control.char_type);
    else if (target_type == control.int_type)
         return (source_type == control.double_type || source_type == control.float_type || source_type == control.long_type);
    else if (target_type == control.long_type)
         return (source_type == control.double_type || source_type == control.float_type);
    else if (target_type == control.float_type)
         return source_type == control.double_type;

    return false;
}


bool Semantic::CanMethodInvocationConvert(TypeSymbol *target_type, TypeSymbol *source_type)
{
    if (target_type == control.no_type || source_type == control.no_type)
        return false;

    if (source_type -> Primitive())
    {
        if (! target_type -> Primitive())
            return false;

        return (target_type == source_type || CanWideningPrimitiveConvert(target_type, source_type));
    }
    else
    {
        if (target_type -> Primitive())
            return false;

        if (source_type -> IsArray())
        {
            if (target_type -> IsArray())
            {
                TypeSymbol *source_subtype = source_type -> ArraySubtype();
                TypeSymbol *target_subtype = target_type -> ArraySubtype();
                return (source_subtype -> Primitive() && target_subtype -> Primitive()
                                                       ? (source_subtype == target_subtype)
                                                       : CanMethodInvocationConvert(target_subtype, source_subtype));
            }
            return (target_type == control.Object() ||
                    target_type == control.Cloneable() ||
                    //
                    // TODO: This is an undocumented feature, but this fix appears to make sense. 
                    //
                    (control.option.one_one && target_type == control.Serializable() && source_type -> Implements(target_type)));
        }
        else if (source_type -> ACC_INTERFACE())
        {
            if (target_type -> ACC_INTERFACE())
                 return source_type -> IsSubinterface(target_type);
            else if (target_type != control.Object()) // target is a class type
                 return false;
        }
        else if (source_type != control.null_type) // source_type is a class
        {
            if (target_type -> IsArray())
                 return false;
            else if (target_type -> ACC_INTERFACE())
                 return source_type -> Implements(target_type);
            else if (! source_type -> IsSubclass(target_type))
                 return false;
        }
    }

    return true;
}


bool Semantic::CanAssignmentConvertReference(TypeSymbol *target_type, TypeSymbol *source_type)
{
    return (target_type == control.no_type || 
            source_type == control.no_type ||
            CanMethodInvocationConvert(target_type, source_type)
           );
}


bool Semantic::CanAssignmentConvert(TypeSymbol *target_type, AstExpression *expr)
{
    return (target_type == control.no_type || 
            expr -> symbol == control.no_type ||
            CanMethodInvocationConvert(target_type, expr -> Type()) ||
            IsIntValueRepresentableInType(expr, target_type)
           );
}


bool Semantic::CanCastConvert(TypeSymbol *target_type, TypeSymbol *source_type, LexStream::TokenIndex tok)
{
    if (source_type == target_type || source_type == control.no_type || target_type == control.no_type)
        return true;

    if (source_type -> Primitive())
    {
        if (! target_type -> Primitive())
            return false;

        return (CanWideningPrimitiveConvert(target_type, source_type) || CanNarrowingPrimitiveConvert(target_type, source_type));
    }
    else
    {
        if (target_type -> Primitive())
            return false;

        if (source_type -> IsArray())
        {
            if (target_type -> IsArray())
            {
                TypeSymbol *source_subtype = source_type -> ArraySubtype();
                TypeSymbol *target_subtype = target_type -> ArraySubtype();
                return (source_subtype -> Primitive() && target_subtype -> Primitive()
                                                       ? (source_subtype == target_subtype)
                                                       : CanCastConvert(target_subtype, source_subtype, tok));
            }
            return (target_type == control.Object() ||
                    target_type == control.Cloneable() ||
                    //
                    // TODO: This is an undocumented feature, but this fix appears to make sense. 
                    //
                    (control.option.one_one && target_type == control.Serializable() && source_type -> Implements(target_type)));
        }
        else if (source_type -> ACC_INTERFACE())
        {
            if (target_type -> ACC_INTERFACE())
            {
                if (! source_type -> expanded_method_table)
                    ComputeMethodsClosure(source_type, tok);
                if (! target_type -> expanded_method_table)
                    ComputeMethodsClosure(target_type, tok);

                //
                // Iterate over all methods in the source symbol table of the source_type interface;
                // For each such method, if the target_type contains a method with the same signature,
                // then make sure that the two methods have the same return type.
                //
                ExpandedMethodTable *source_method_table = source_type -> expanded_method_table;
                int i;
                for (i = 0; i < source_method_table -> symbol_pool.Length(); i++)
                {
                    MethodSymbol *method1 = source_method_table -> symbol_pool[i] -> method_symbol;
                    MethodShadowSymbol *method_shadow2 = target_type -> expanded_method_table
                                                                     -> FindOverloadMethodShadow(method1, (Semantic *) this, tok);
                    if (method_shadow2)
                    {
                        if (! method1 -> IsTyped())
                            method1 -> ProcessMethodSignature((Semantic *) this, tok);

                        MethodSymbol *method2 = method_shadow2 -> method_symbol;
                        if (! method2 -> IsTyped())
                            method2 -> ProcessMethodSignature((Semantic *) this, tok);
                        if (method1 -> Type() != method2 -> Type())
                            break;
                    }
                }

                return (i == source_method_table -> symbol_pool.Length()); // all the methods passed the test
            }
            else if (target_type -> ACC_FINAL() && (! target_type -> Implements(source_type)))
                 return false;
        }
        else if (source_type != control.null_type) // source_type is a class
        {
            if (target_type -> IsArray())
            {
                if (source_type != control.Object())
                    return false;
            }
            else if (target_type -> ACC_INTERFACE())
            {
                if (source_type -> ACC_FINAL() && (! source_type -> Implements(target_type)))
                    return false;
            }
            else if ((! source_type -> IsSubclass(target_type)) && (! target_type -> IsSubclass(source_type)))
                 return false;
        }
    }

    return true;
}


LiteralValue *Semantic::CastPrimitiveValue(TypeSymbol *target_type, AstExpression *expr)
{
    LiteralValue *literal_value = NULL;
    TypeSymbol *source_type = expr -> Type();

    if (target_type == source_type)
        literal_value = expr -> value;
    else if (source_type != control.no_type)
    {
        char output_string[25];
        int len;

        if (target_type == control.String())
        {
            if (source_type == control.double_type)
            {
                //
                // TODO: Check correctness !!!
                //
                DoubleLiteralValue *literal = (DoubleLiteralValue *) expr -> value;
                // sprintf(output_string, "%E", literal -> value);
                literal -> value.String(output_string);
                len = strlen(output_string);
                literal_value = control.Utf8_pool.FindOrInsert(output_string, len);
            }
            else if (source_type == control.float_type)
            {
                //
                // TODO: Check correctness !!!
                //
                FloatLiteralValue *literal = (FloatLiteralValue *) expr -> value;
                // sprintf(output_string, "%E", literal -> value);
                literal -> value.String(output_string);
                len = strlen(output_string);
                //
                // javac does not add the L suffix
                //
                // output_string[len++] = U_F;
                //
                output_string[len] = U_NULL;
                literal_value = control.Utf8_pool.FindOrInsert(output_string, len);
            }
            else if (source_type == control.long_type)
            {
                LongLiteralValue *literal = (LongLiteralValue *) expr -> value;
                literal -> value.String(output_string);
                len = strlen(output_string);
                //
                // javac does not add the L suffix
                //
                // output_string[len++] = U_L;
                //
                output_string[len] = U_NULL;
                literal_value = control.Utf8_pool.FindOrInsert(output_string, len);
            }
            else if (source_type == control.char_type)
            {
                IntLiteralValue *literal = (IntLiteralValue *) expr -> value;
                literal_value = control.Utf8_pool.FindOrInsert(literal -> value);
            }
            else if (control.IsSimpleIntegerValueType(source_type))
            {
                //
                // TODO: Check correctness !!!
                //
                IntLiteralValue *literal = (IntLiteralValue *) expr -> value;
                sprintf(output_string, "%i", literal -> value);
                len = strlen(output_string);
                literal_value = control.Utf8_pool.FindOrInsert(output_string, len);
            }
        }
        else if (target_type == control.double_type)
        {
            if (source_type == control.float_type)
            {
                FloatLiteralValue *literal = (FloatLiteralValue *) expr -> value;
                IEEEdouble value(literal -> value);
                literal_value = control.double_pool.FindOrInsert(value);
            }
            else if (source_type == control.long_type)
            {
                LongLiteralValue *literal = (LongLiteralValue *) expr -> value;
                IEEEdouble value(literal -> value);
                literal_value = control.double_pool.FindOrInsert(value);
            }
            else
            {
                IntLiteralValue *literal = (IntLiteralValue *) expr -> value;
                IEEEdouble value(literal -> value);
                literal_value = control.double_pool.FindOrInsert(value);
            }
        }
        else if (target_type == control.float_type)
        {
            if (source_type == control.double_type)
            {
                DoubleLiteralValue *literal = (DoubleLiteralValue *) expr -> value;
                IEEEfloat value(literal -> value);
                literal_value = control.float_pool.FindOrInsert(value);
            }
            else if (source_type == control.long_type)
            {
                LongLiteralValue *literal = (LongLiteralValue *) expr -> value;
                IEEEfloat value(literal -> value);
                literal_value = control.float_pool.FindOrInsert(value);
            }
            else
            {
                IntLiteralValue *literal = (IntLiteralValue *) expr -> value;
                IEEEfloat value(literal -> value);
                literal_value = control.float_pool.FindOrInsert(value);
            }
        }
        else if (target_type == control.long_type)
        {
            if (source_type == control.double_type)
            {
                DoubleLiteralValue *literal = (DoubleLiteralValue *) expr -> value;
                LongInt value(literal -> value);
                literal_value = control.long_pool.FindOrInsert(value);
            }
            else if (source_type == control.float_type)
            {
                FloatLiteralValue *literal = (FloatLiteralValue *) expr -> value;
                LongInt value(literal -> value);
                literal_value = control.long_pool.FindOrInsert(value);
            }
            else
            {
                IntLiteralValue *literal = (IntLiteralValue *) expr -> value;
                literal_value = control.long_pool.FindOrInsert((LongInt) literal -> value);
            }
        }
        else if (target_type == control.int_type)
        {
            if (source_type == control.double_type)
            {
                DoubleLiteralValue *literal = (DoubleLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((literal -> value).IntValue());
            }
            else if (source_type == control.float_type)
            {
                FloatLiteralValue *literal = (FloatLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert(literal -> value.IntValue());
            }
            else if (source_type == control.long_type)
            {
                LongLiteralValue *literal = (LongLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (literal -> value).LowWord());
            }
            else literal_value = expr -> value;
        }
        else if (target_type == control.char_type)
        {
            if (source_type == control.double_type)
            {
                DoubleLiteralValue *literal = (DoubleLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (u2) (literal -> value.IntValue()));
            }
            else if (source_type == control.float_type)
            {
                FloatLiteralValue *literal = (FloatLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (u2) (literal -> value.IntValue()));
            }
            else if (source_type == control.long_type)
            {
                LongLiteralValue *literal = (LongLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (u2) (literal -> value).LowWord());
            }
            else
            {
                IntLiteralValue *literal = (IntLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (u2) literal -> value);
            }
        }
        else if (target_type == control.short_type)
        {
            if (source_type == control.double_type)
            {
                DoubleLiteralValue *literal = (DoubleLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (i2) (literal -> value.IntValue()));
            }
            else if (source_type == control.float_type)
            {
                FloatLiteralValue *literal = (FloatLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (i2) (literal -> value.IntValue()));
            }
            else if (source_type == control.long_type)
            {
                LongLiteralValue *literal = (LongLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (i2) (literal -> value).LowWord());
            }
            else
            {
                IntLiteralValue *literal = (IntLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (i2) literal -> value);
            }
        }
        else if (target_type == control.byte_type)
        {
            if (source_type == control.double_type)
            {
                DoubleLiteralValue *literal = (DoubleLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (i1) (literal -> value.IntValue()));
            }
            else if (source_type == control.float_type)
            {
                FloatLiteralValue *literal = (FloatLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (i1) (literal -> value.IntValue()));
            }
            else if (source_type == control.long_type)
            {
                LongLiteralValue *literal = (LongLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (i1) (literal -> value).LowWord());
            }
            else
            {
                IntLiteralValue *literal = (IntLiteralValue *) expr -> value;
                literal_value = control.int_pool.FindOrInsert((int) (i1) literal -> value);
            }
        }
    }

    return literal_value;
}


//
// We only need to cast the value of constant primitive expressions.
//
inline LiteralValue *Semantic::CastValue(TypeSymbol *target_type, AstExpression *expr)
{
    return (LiteralValue *) (expr -> IsConstant() && target_type -> Primitive()
                                                   ? CastPrimitiveValue(target_type, expr)
                                                   : (expr -> value == control.NullValue() ? expr -> value : NULL));
}


void Semantic::ProcessCastExpression(Ast *expr)
{
    AstCastExpression *cast_expression = (AstCastExpression *) expr;

    //
    // This operation may throw ClassCastException
    //
    SymbolSet *exception_set = TryExceptionTableStack().Top();
    if (exception_set)
    {
        exception_set -> AddElement(control.RuntimeException());
        exception_set -> AddElement(control.Error());
    }

    ProcessExpression(cast_expression -> expression);

    TypeSymbol *source_type = cast_expression -> expression -> Type();

    //
    // Recall that the type is optional only when the compiler inserts
    // a CAST conversion node into the program.
    //
    AstPrimitiveType *primitive_type = cast_expression -> type_opt -> PrimitiveTypeCast();
    TypeSymbol *target_type;
    if (primitive_type)
         target_type = FindPrimitiveType(primitive_type);
    else if (cast_expression -> type_opt -> IsName())
         target_type = MustFindType(cast_expression -> type_opt);
    else
    {
        ReportSemError(SemanticError::INVALID_CAST_TYPE,
                       cast_expression -> type_opt -> LeftToken(),
                       cast_expression -> type_opt -> RightToken());
        cast_expression -> symbol = control.no_type;

        return;
    }

    int num_dimensions = cast_expression -> NumBrackets();
    target_type = (num_dimensions == 0 ? target_type : target_type -> GetArrayType((Semantic *) this, num_dimensions));
    
    if (CanAssignmentConvert(target_type, cast_expression -> expression))
    {
        cast_expression -> symbol = target_type;
        cast_expression -> value = CastValue(target_type, cast_expression -> expression);
    }
    else if (CanCastConvert(target_type, source_type, cast_expression -> right_parenthesis_token_opt))
    {
        cast_expression -> kind = Ast::CHECK_AND_CAST;
        cast_expression -> symbol = target_type;
        cast_expression -> value = CastValue(target_type, cast_expression -> expression);
    }
    else
    {
        ReportSemError(SemanticError::INVALID_CAST_CONVERSION,
                       cast_expression -> expression -> LeftToken(),
                       cast_expression -> expression -> RightToken(),
                       source_type -> Name(),
                       target_type -> Name());
        cast_expression -> symbol = control.no_type;
    }

    return;
}


AstExpression *Semantic::ConvertToType(AstExpression *expr, TypeSymbol *type)
{
    if (expr -> Type() == control.null_type)
        return expr;

    LexStream::TokenIndex loc = expr -> LeftToken();

    AstCastExpression *result = compilation_unit -> ast_pool -> GenCastExpression();
    result -> left_parenthesis_token_opt = loc;
    result -> type_opt = NULL;
    result -> right_parenthesis_token_opt = loc;
    result -> expression = expr;

    result -> symbol = type;
    result -> value = CastValue(type, expr);

    return result;
}


AstExpression *Semantic::PromoteUnaryNumericExpression(AstExpression *unary_expression)
{
    TypeSymbol *type = unary_expression -> Type();

    if (type == control.no_type)
        return unary_expression;

    if (! control.IsNumeric(type))
    {
       ReportSemError(SemanticError::TYPE_NOT_NUMERIC,
                      unary_expression -> LeftToken(),
                      unary_expression -> RightToken(),
                      type -> Name());
        unary_expression -> symbol = control.no_type;
        return unary_expression;
    }

    return ((type == control.byte_type || type == control.short_type || type == control.char_type)
                                                ? ConvertToType(unary_expression, control.int_type)
                                                : unary_expression);
}


void Semantic::BinaryNumericPromotion(AstBinaryExpression *binary_expression)
{
    AstExpression *left_expr = binary_expression -> left_expression;
    AstExpression *right_expr = binary_expression -> right_expression;

    TypeSymbol *left_type  = left_expr -> Type(),
               *right_type = right_expr -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
    {
        binary_expression -> symbol = control.no_type;
        return;
    }

    if (! control.IsNumeric(left_type))
    {
       ReportSemError(SemanticError::TYPE_NOT_NUMERIC,
                      left_expr -> LeftToken(),
                      left_expr -> RightToken(),
                      left_type -> Name());
        binary_expression -> symbol = control.no_type;
        return;
    }
    else if (! control.IsNumeric(right_type))
    {
       ReportSemError(SemanticError::TYPE_NOT_NUMERIC,
                      right_expr -> LeftToken(),
                      right_expr -> RightToken(),
                      right_type -> Name());
        binary_expression -> symbol = control.no_type;
        return;
    }

    if (left_type == control.double_type)
    {
        if (right_type != control.double_type)
            binary_expression -> right_expression = ConvertToType(binary_expression -> right_expression, control.double_type);
        binary_expression -> symbol = control.double_type;
    }
    else if (right_type == control.double_type)
    {
        if (left_type != control.double_type)
            binary_expression -> left_expression = ConvertToType(binary_expression -> left_expression, control.double_type);
        binary_expression -> symbol = control.double_type;
    }
    else if (left_type == control.float_type)
    {
        if (right_type != control.float_type)
            binary_expression -> right_expression = ConvertToType(binary_expression -> right_expression, control.float_type);
        binary_expression -> symbol = control.float_type;
    }
    else if (right_type == control.float_type)
    {
        if (left_type != control.float_type)
            binary_expression -> left_expression = ConvertToType(binary_expression -> left_expression, control.float_type);
        binary_expression -> symbol = control.float_type;
    }
    else if (left_type == control.long_type)
    {
        if (right_type != control.long_type)
            binary_expression -> right_expression = ConvertToType(binary_expression -> right_expression, control.long_type);
        binary_expression -> symbol = control.long_type;
    }
    else if (right_type == control.long_type)
    {
        if (left_type != control.long_type)
            binary_expression -> left_expression = ConvertToType(binary_expression -> left_expression, control.long_type);
        binary_expression -> symbol = control.long_type;
    }
    else
    {
        if (left_type != control.int_type)
            binary_expression -> left_expression = ConvertToType(binary_expression -> left_expression, control.int_type);
        if (right_type != control.int_type)
            binary_expression -> right_expression = ConvertToType(binary_expression -> right_expression, control.int_type);
        binary_expression -> symbol = control.int_type;
    }

    return;
}


void Semantic::BinaryNumericPromotion(AstAssignmentExpression *assignment_expression)
{
    AstExpression *left_expr = assignment_expression -> left_hand_side;
    AstExpression *right_expr = assignment_expression -> expression;

    TypeSymbol *left_type  = left_expr -> Type(),
               *right_type = right_expr -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        return;

    if (! control.IsNumeric(left_type))
    {
       ReportSemError(SemanticError::TYPE_NOT_NUMERIC,
                      left_expr -> LeftToken(),
                      left_expr -> RightToken(),
                      left_type -> Name());
        return;
    }
    else if (! control.IsNumeric(right_type))
    {
       ReportSemError(SemanticError::TYPE_NOT_NUMERIC,
                      right_expr -> LeftToken(),
                      right_expr -> RightToken(),
                      right_type -> Name());
        return;
    }

    if (left_type == control.double_type)
    {
        if (right_type != control.double_type)
            assignment_expression -> expression = ConvertToType(assignment_expression -> expression, control.double_type);
    }
    else if (right_type == control.double_type)
    {
        if (left_type != control.double_type)
            assignment_expression -> left_hand_side = ConvertToType(assignment_expression -> left_hand_side, control.double_type);
    }   
    else if (left_type == control.float_type)
    {
        if (right_type != control.float_type)
            assignment_expression -> expression = ConvertToType(assignment_expression -> expression, control.float_type);
    }
    else if (right_type == control.float_type)
    {
        if (left_type != control.float_type)
            assignment_expression -> left_hand_side = ConvertToType(assignment_expression -> left_hand_side, control.float_type);
    }
    else if (left_type == control.long_type)
    {
        if (right_type != control.long_type)
            assignment_expression -> expression = ConvertToType(assignment_expression -> expression, control.long_type);
    }
    else if (right_type == control.long_type)
    {
        if (left_type != control.long_type)
            assignment_expression -> left_hand_side = ConvertToType(assignment_expression -> left_hand_side, control.long_type);
    }
    else
    {
        if (left_type != control.int_type)
            assignment_expression -> left_hand_side = ConvertToType(assignment_expression -> left_hand_side, control.int_type);
        if (right_type != control.int_type)
            assignment_expression -> expression = ConvertToType(assignment_expression -> expression, control.int_type);
    }

    return;
}


void Semantic::BinaryNumericPromotion(AstConditionalExpression *conditional_expression)
{
    AstExpression *left_expr = conditional_expression -> true_expression;
    AstExpression *right_expr = conditional_expression -> false_expression;

    TypeSymbol *left_type  = left_expr -> Type(),
               *right_type = right_expr -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
    {
        conditional_expression -> symbol = control.no_type;
        return;
    }

    if (! control.IsNumeric(left_type))
    {
       ReportSemError(SemanticError::TYPE_NOT_NUMERIC,
                      left_expr -> LeftToken(),
                      left_expr -> RightToken(),
                      left_type -> Name());
        conditional_expression -> symbol = control.no_type;
        return;
    }
    else if (! control.IsNumeric(right_type))
    {
       ReportSemError(SemanticError::TYPE_NOT_NUMERIC,
                      right_expr -> LeftToken(),
                      right_expr -> RightToken(),
                      right_type -> Name());
        conditional_expression -> symbol = control.no_type;
        return;
    }

    if (left_type == control.double_type)
    {
        if (right_type != control.double_type)
            conditional_expression -> false_expression =
                        ConvertToType(conditional_expression -> false_expression, control.double_type);
        conditional_expression -> symbol = control.double_type;
    }
    else if (right_type == control.double_type)
    {
        if (left_type != control.double_type)
            conditional_expression -> true_expression =
                        ConvertToType(conditional_expression -> true_expression, control.double_type);
        conditional_expression -> symbol = control.double_type;
    }
    else if (left_type == control.float_type)
    {
        if (right_type != control.float_type)
            conditional_expression -> false_expression =
                        ConvertToType(conditional_expression -> false_expression, control.float_type);
        conditional_expression -> symbol = control.float_type;
    }
    else if (right_type == control.float_type)
    {
        if (left_type != control.float_type)
            conditional_expression -> true_expression =
                        ConvertToType(conditional_expression -> true_expression, control.float_type);
        conditional_expression -> symbol = control.float_type;
    }
    else if (left_type == control.long_type)
    {
        if (right_type != control.long_type)
            conditional_expression -> false_expression =
                        ConvertToType(conditional_expression -> false_expression, control.long_type);
        conditional_expression -> symbol = control.long_type;
    }
    else if (right_type == control.long_type)
    {
        if (left_type != control.long_type)
            conditional_expression -> true_expression = ConvertToType(conditional_expression -> true_expression, control.long_type);
        conditional_expression -> symbol = control.long_type;
    }
    else
    {
        if (left_type != control.int_type)
            conditional_expression -> true_expression = ConvertToType(conditional_expression -> true_expression, control.int_type);
        if (right_type != control.int_type)
            conditional_expression -> false_expression =
                        ConvertToType(conditional_expression -> false_expression, control.int_type);
        conditional_expression -> symbol = control.int_type;
    }

    return;
}


void Semantic::ProcessPLUS(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else if (left_type == control.String() || right_type == control.String())
    {
        //
        // This operation may throw OutOfMemoryException
        //
        SymbolSet *exception_set = TryExceptionTableStack().Top();
        if (exception_set)
        {
            exception_set -> AddElement(control.RuntimeException());
            exception_set -> AddElement(control.Error());
        }

        if (left_type != control.String())
        {
            AddStringConversionDependence(left_type, expr -> binary_operator_token);
            AstExpression *left_expression = expr -> left_expression;
            if (left_type == control.void_type)
                ReportSemError(SemanticError::VOID_TO_STRING,
                               left_expression -> LeftToken(),
                               left_expression -> RightToken());
            expr -> left_expression = ConvertToType(left_expression, control.String());
            if (left_expression -> IsConstant())
                expr -> left_expression -> value = CastPrimitiveValue(control.String(), left_expression);
        }
        else if (right_type != control.String())
        {
            AddStringConversionDependence(right_type, expr -> binary_operator_token);
            AstExpression *right_expression = expr -> right_expression;
            if (right_type == control.void_type)
                ReportSemError(SemanticError::VOID_TO_STRING,
                               right_expression -> LeftToken(),
                               right_expression -> RightToken());
            expr -> right_expression = ConvertToType(right_expression, control.String());
            if (right_expression -> IsConstant())
                expr -> right_expression -> value = CastPrimitiveValue(control.String(), right_expression);
        }
        AddDependence(ThisType(), control.StringBuffer(), expr -> binary_operator_token);

        //
        // If both subexpressions are strings constants, identify the result as
        // as a string constant, but do not perform the concatenation here. The
        // reason being that if we have a long expression of the form
        //
        //  s1 + s2 + ... + sn
        //
        // where each subexpression s(i) is a string constant, we want to perform
        // one concatenation and enter a single result into the constant pool instead
        // of n-1 subresults. See CheckStringConstant in lookup.cpp.
        //

        expr -> symbol = control.String();
    }
    else
    {
        BinaryNumericPromotion(expr);

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
             if (expr -> Type() == control.double_type)
             {
                 DoubleLiteralValue *left = (DoubleLiteralValue *) expr -> left_expression -> value;
                 DoubleLiteralValue *right = (DoubleLiteralValue *) expr -> right_expression -> value;

                 expr -> value = control.double_pool.FindOrInsert(left -> value + right -> value);
             }
             else if (expr -> Type() == control.float_type)
             {
                 FloatLiteralValue *left = (FloatLiteralValue *) expr -> left_expression -> value;
                 FloatLiteralValue *right = (FloatLiteralValue *) expr -> right_expression -> value;

                 expr -> value = control.float_pool.FindOrInsert(left -> value + right -> value);
             }
             else if (expr -> Type() == control.long_type)
             {
                 LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                 LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                 expr -> value = control.long_pool.FindOrInsert(left -> value + right -> value);
             }
             else // assert(expr -> Type() == control.int_type)
             {
                 IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                 IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                 expr -> value = control.int_pool.FindOrInsert(left -> value + right -> value);
             }
        }
    }

    return;
}


void Semantic::ProcessLEFT_SHIFT(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else if (! control.IsIntegral(left_type))
    {
        ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                       expr -> left_expression -> LeftToken(),
                       expr -> left_expression -> RightToken(),
                       left_type -> Name());
        expr -> symbol = control.no_type;
    }
    else if (! control.IsIntegral(right_type))
    {
        ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                       expr -> right_expression -> LeftToken(),
                       expr -> right_expression -> RightToken(),
                       right_type -> Name());
        expr -> symbol = control.no_type;
    }
    else
    {
        expr -> left_expression  = PromoteUnaryNumericExpression(expr -> left_expression);
        expr -> right_expression = PromoteUnaryNumericExpression(expr -> right_expression);
        expr -> symbol = expr -> left_expression -> symbol;

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                int right_value;
                if (expr -> right_expression -> Type() == control.long_type)
                {
                    LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;
                    right_value = right -> value.LowWord();
                }
                else
                {
                    IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;
                    right_value = right -> value;
                }
                right_value &= 0x3F;

                expr -> value = control.long_pool.FindOrInsert(left -> value << right_value);
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value << (0x1F & right -> value));
            }
        }
    }

    return;
}


void Semantic::ProcessRIGHT_SHIFT(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else if (! control.IsIntegral(left_type))
    {
        ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                       expr -> left_expression -> LeftToken(),
                       expr -> left_expression -> RightToken(),
                       left_type -> Name());
        expr -> symbol = control.no_type;
    }
    else if (! control.IsIntegral(right_type))
    {
        ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                       expr -> right_expression -> LeftToken(),
                       expr -> right_expression -> RightToken(),
                       right_type -> Name());
        expr -> symbol = control.no_type;
    }
    else
    {
        expr -> left_expression  = PromoteUnaryNumericExpression(expr -> left_expression);
        expr -> right_expression = PromoteUnaryNumericExpression(expr -> right_expression);
        expr -> symbol = expr -> left_expression -> symbol;

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                int right_value;
                if (expr -> right_expression -> Type() == control.long_type)
                {
                    LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;
                    right_value = right -> value.LowWord();
                }
                else
                {
                    IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;
                    right_value = right -> value;
                }
                right_value &= 0x3F;

                expr -> value = control.long_pool.FindOrInsert(left -> value >> right_value);
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value >> (0x1F & right -> value));
            }
        }
    }

    return;
}


void Semantic::ProcessUNSIGNED_RIGHT_SHIFT(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else if (! control.IsIntegral(left_type))
    {
        ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                       expr -> left_expression -> LeftToken(),
                       expr -> left_expression -> RightToken(),
                       left_type -> Name());
        expr -> symbol = control.no_type;
    }
    else if (! control.IsIntegral(right_type))
    {
        ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                       expr -> right_expression -> LeftToken(),
                       expr -> right_expression -> RightToken(),
                       right_type -> Name());
        expr -> symbol = control.no_type;
    }
    else
    {
        expr -> left_expression  = PromoteUnaryNumericExpression(expr -> left_expression);
        expr -> right_expression = PromoteUnaryNumericExpression(expr -> right_expression);
        expr -> symbol = expr -> left_expression -> symbol;

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                int right_value;
                if (expr -> right_expression -> Type() == control.long_type)
                {
                    LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;
                    right_value = right -> value.LowWord();
                }
                else
                {
                    IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;
                    right_value = right -> value;
                }
                right_value &= 0x3F;

                LongInt value = left -> value >> right_value;
                if (left -> value < 0)
                    value += (LongInt(2) << (63 - right_value));
                expr -> value = control.long_pool.FindOrInsert(value);
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                int value = left -> value >> (0x1F & right -> value);
                if (left -> value < 0)
                     value += (2 << (31 - (0x1F & right -> value)));
                expr -> value = control.int_pool.FindOrInsert(value);
            }
        }
    }

    return;
}


void Semantic::ProcessLESS(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type != control.no_type && right_type != control.no_type)
    {
        BinaryNumericPromotion(expr);

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.double_type)
            {
                DoubleLiteralValue *left = (DoubleLiteralValue *) expr -> left_expression -> value;
                DoubleLiteralValue *right = (DoubleLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value < right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = (FloatLiteralValue *) expr -> left_expression -> value;
                FloatLiteralValue *right = (FloatLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value < right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.long_type)
           {
                LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value < right -> value ? 1 : 0);
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value < right -> value ? 1 : 0);
            }
        }
    }

    expr -> symbol = control.boolean_type;

    return;
}


void Semantic::ProcessGREATER(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type != control.no_type && right_type != control.no_type)
    {
        BinaryNumericPromotion(expr);

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.double_type)
            {
                DoubleLiteralValue *left = (DoubleLiteralValue *) expr -> left_expression -> value;
                DoubleLiteralValue *right = (DoubleLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value > right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = (FloatLiteralValue *) expr -> left_expression -> value;
                FloatLiteralValue *right = (FloatLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value > right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value > right -> value ? 1 : 0);
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value > right -> value ? 1 : 0);
            }
        }
    }

    expr -> symbol = control.boolean_type;

    return;
}


void Semantic::ProcessLESS_EQUAL(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type != control.no_type && right_type != control.no_type)
    {
        BinaryNumericPromotion(expr);

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.double_type)
            {
                DoubleLiteralValue *left = (DoubleLiteralValue *) expr -> left_expression -> value;
                DoubleLiteralValue *right = (DoubleLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value <= right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = (FloatLiteralValue *) expr -> left_expression -> value;
                FloatLiteralValue *right = (FloatLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value <= right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value <= right -> value ? 1 : 0);
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value <= right -> value ? 1 : 0);
            }
        }
    }

    expr -> symbol = control.boolean_type;

    return;
}


void Semantic::ProcessGREATER_EQUAL(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type != control.no_type && right_type != control.no_type)
    {
        BinaryNumericPromotion(expr);

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.double_type)
            {
                DoubleLiteralValue *left = (DoubleLiteralValue *) expr -> left_expression -> value;
                DoubleLiteralValue *right = (DoubleLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value >= right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = (FloatLiteralValue *) expr -> left_expression -> value;
                FloatLiteralValue *right = (FloatLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value >= right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value >= right -> value ? 1 : 0);
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value >= right -> value ? 1 : 0);
            }
        }
    }

    expr -> symbol = control.boolean_type;

    return;
}


void Semantic::ProcessAND(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else
    {
        if (left_type == control.boolean_type && right_type == control.boolean_type)
             expr -> symbol = control.boolean_type;
        else
        {
            BinaryNumericPromotion(expr);

            TypeSymbol *expr_type = expr -> Type();

            if (expr_type != control.no_type)
            {
                if (! control.IsIntegral(expr_type))
                {
                    ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                                   expr -> LeftToken(),
                                   expr -> RightToken(),
                                   expr_type -> Name());
                    expr -> symbol = control.no_type;
                }
                else if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
                {
                    if (expr_type == control.long_type)
                    {
                        LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                        LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                        expr -> value = control.long_pool.FindOrInsert(left -> value & right -> value);
                    }
                    else // assert(expr_type == control.int_type)
                    {
                        IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                        IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                        expr -> value = control.int_pool.FindOrInsert(left -> value & right -> value);
                    }
                }
            }
        }
    }

    return;
}


void Semantic::ProcessXOR(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else
    {
        if (left_type == control.boolean_type && right_type == control.boolean_type)
             expr -> symbol = control.boolean_type;
        else
        {
            BinaryNumericPromotion(expr);

            TypeSymbol *expr_type = expr -> Type();

            if (expr_type != control.no_type)
            {
                if (! control.IsIntegral(expr_type))
                {
                    ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                                   expr -> LeftToken(),
                                   expr -> RightToken(),
                                   expr_type -> Name());
                    expr -> symbol = control.no_type;
                }
                else if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
                {
                    if (expr_type == control.long_type)
                    {
                        LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                        LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                        expr -> value = control.long_pool.FindOrInsert(left -> value ^ right -> value);
                    }
                    else // assert(expr_type == control.int_type)
                    {
                        IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                        IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                        expr -> value = control.int_pool.FindOrInsert(left -> value ^ right -> value);
                    }
                }
            }
        }
    }

    return;
}


void Semantic::ProcessIOR(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else
    {
        if (left_type == control.boolean_type && right_type == control.boolean_type)
             expr -> symbol = control.boolean_type;
        else
        {
            BinaryNumericPromotion(expr);

            TypeSymbol *expr_type = expr -> Type();

            if (expr_type != control.no_type)
            {
                if (! control.IsIntegral(expr_type))
                {
                    ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                                   expr -> LeftToken(),
                                   expr -> RightToken(),
                                   expr_type -> Name());
                    expr -> symbol = control.no_type;
                }
                else if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
                {
                    if (expr_type == control.long_type)
                    {
                        LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                        LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                        expr -> value = control.long_pool.FindOrInsert(left -> value | right -> value);
                    }
                    else // assert(expr_type == control.int_type)
                    {
                        IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                        IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                        expr -> value = control.int_pool.FindOrInsert(left -> value | right -> value);
                    }
                }
            }
        }
    }

    return;
}


void Semantic::ProcessAND_AND(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type != control.no_type && right_type != control.no_type)
    {
        if (left_type != control.boolean_type)
        {
            ReportSemError(SemanticError::TYPE_NOT_BOOLEAN,
                           expr -> left_expression -> LeftToken(),
                           expr -> left_expression -> RightToken(),
                           left_type -> Name());
        }
        else if (right_type != control.boolean_type)
        {
            ReportSemError(SemanticError::TYPE_NOT_BOOLEAN,
                           expr -> right_expression -> LeftToken(),
                           expr -> right_expression -> RightToken(),
                           right_type -> Name());
        }
        else if (expr -> left_expression -> IsConstant())
        {
            IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
            if (! left -> value)
                expr -> value = control.int_pool.FindOrInsert(0);
            else if (expr -> right_expression -> IsConstant())
            {
                IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;
                expr -> value = control.int_pool.FindOrInsert(left -> value && right -> value ? 1 : 0);
            }
        }
    }

    expr -> symbol = control.boolean_type;

    return;
}


void Semantic::ProcessOR_OR(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type != control.no_type && right_type != control.no_type)
    {
        if (left_type != control.boolean_type)
        {
            ReportSemError(SemanticError::TYPE_NOT_BOOLEAN,
                           expr -> left_expression -> LeftToken(),
                           expr -> left_expression -> RightToken(),
                           left_type -> Name());
        }
        else if (right_type != control.boolean_type)
        {
            ReportSemError(SemanticError::TYPE_NOT_BOOLEAN,
                           expr -> right_expression -> LeftToken(),
                           expr -> right_expression -> RightToken(),
                           right_type -> Name());
        }
        else if (expr -> left_expression -> IsConstant())
        {
            IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
            if (left -> value)
                expr -> value = control.int_pool.FindOrInsert(1);
            else if (expr -> right_expression -> IsConstant())
            {
                IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;
                expr -> value = control.int_pool.FindOrInsert(left -> value || right -> value ? 1 : 0);
            }
        }
    }

    expr -> symbol = control.boolean_type;

    return;
}


void Semantic::ProcessEQUAL_EQUAL(AstBinaryExpression *expr)
{
    ProcessExpressionOrStringConstant(expr -> left_expression);
    ProcessExpressionOrStringConstant(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type != control.no_type && right_type != control.no_type)
    {
        if (left_type != right_type)
        {
            if (left_type -> Primitive() && right_type -> Primitive())
                 BinaryNumericPromotion(expr);
            else if (CanCastConvert(left_type, right_type, expr -> binary_operator_token))
                 expr -> right_expression = ConvertToType(expr -> right_expression, left_type);
            else if (CanCastConvert(right_type, left_type, expr -> binary_operator_token))
                 expr -> left_expression = ConvertToType(expr -> left_expression, right_type);
            else
            {
                ReportSemError(SemanticError::INCOMPATIBLE_TYPE_FOR_BINARY_EXPRESSION,
                               expr -> LeftToken(),
                               expr -> RightToken(),
                               expr -> left_expression -> Type() -> ContainingPackage() -> PackageName(),
                               expr -> left_expression -> Type() -> ExternalName(),
                               expr -> right_expression -> Type() -> ContainingPackage() -> PackageName(),
                               expr -> right_expression -> Type() -> ExternalName());
            }
        }
        else 
        {
            if (left_type == control.void_type)
                ReportSemError(SemanticError::VOID_TYPE_IN_EQUALITY_EXPRESSION,
                               expr -> LeftToken(),
                               expr -> RightToken(),
                               expr -> left_expression -> Type() -> Name(),
                               expr -> right_expression -> Type() -> Name());
            expr -> symbol = left_type;
        }

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            LiteralValue *left = expr -> left_expression -> value;
            LiteralValue *right = expr -> right_expression -> value;

            if (expr -> Type() == control.double_type)
            {
                DoubleLiteralValue *left = (DoubleLiteralValue *) expr -> left_expression -> value;
                DoubleLiteralValue *right = (DoubleLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value == right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = (FloatLiteralValue *) expr -> left_expression -> value;
                FloatLiteralValue *right = (FloatLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value == right -> value ? 1 : 0);
            }
            else expr -> value = control.int_pool.FindOrInsert(left == right ? 1 : 0);
        }
    }

    expr -> symbol = control.boolean_type;

    return;
}


void Semantic::ProcessNOT_EQUAL(AstBinaryExpression *expr)
{
    ProcessExpressionOrStringConstant(expr -> left_expression);
    ProcessExpressionOrStringConstant(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type != control.no_type && right_type != control.no_type)
    {
        if (left_type != right_type)
        {
            if (left_type -> Primitive() && right_type -> Primitive())
                 BinaryNumericPromotion(expr);
            else if (CanCastConvert(left_type, right_type, expr -> binary_operator_token))
                 expr -> right_expression = ConvertToType(expr -> right_expression, left_type);
            else if (CanCastConvert(right_type, left_type, expr -> binary_operator_token))
                 expr -> left_expression = ConvertToType(expr -> left_expression, right_type);
            else
            {
                ReportSemError(SemanticError::INCOMPATIBLE_TYPE_FOR_BINARY_EXPRESSION,
                               expr -> LeftToken(),
                               expr -> RightToken(),
                               expr -> left_expression -> Type() -> ContainingPackage() -> PackageName(),
                               expr -> left_expression -> Type() -> ExternalName(),
                               expr -> right_expression -> Type() -> ContainingPackage() -> PackageName(),
                               expr -> right_expression -> Type() -> ExternalName());
            }
        }
        else 
        {
            if (left_type == control.void_type)
                ReportSemError(SemanticError::VOID_TYPE_IN_EQUALITY_EXPRESSION,
                               expr -> LeftToken(),
                               expr -> RightToken());
            expr -> symbol = left_type;
        }

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            LiteralValue *left = expr -> left_expression -> value;
            LiteralValue *right = expr -> right_expression -> value;

            if (expr -> Type() == control.double_type)
            {
                DoubleLiteralValue *left = (DoubleLiteralValue *) expr -> left_expression -> value;
                DoubleLiteralValue *right = (DoubleLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value != right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = (FloatLiteralValue *) expr -> left_expression -> value;
                FloatLiteralValue *right = (FloatLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value != right -> value ? 1 : 0);
            }
            else expr -> value = control.int_pool.FindOrInsert(left != right ? 1 : 0);
        }
    }

    expr -> symbol = control.boolean_type;

    return;
}


void Semantic::ProcessSTAR(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else
    {
        BinaryNumericPromotion(expr);

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.double_type)
            {
                DoubleLiteralValue *left = (DoubleLiteralValue *) expr -> left_expression -> value;
                DoubleLiteralValue *right = (DoubleLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.double_pool.FindOrInsert(left -> value * right -> value);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = (FloatLiteralValue *) expr -> left_expression -> value;
                FloatLiteralValue *right = (FloatLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.float_pool.FindOrInsert(left -> value * right -> value);
            }
            else if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.long_pool.FindOrInsert(left -> value * right -> value);
            }
            else if (expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value * right -> value);
            }
        }
    }

    return;
}


void Semantic::ProcessMINUS(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else
    {
        BinaryNumericPromotion(expr);

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.double_type)
            {
                DoubleLiteralValue *left = (DoubleLiteralValue *) expr -> left_expression -> value;
                DoubleLiteralValue *right = (DoubleLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.double_pool.FindOrInsert(left -> value - right -> value);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = (FloatLiteralValue *) expr -> left_expression -> value;
                FloatLiteralValue *right = (FloatLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.float_pool.FindOrInsert(left -> value - right -> value);
            }
            else if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.long_pool.FindOrInsert(left -> value - right -> value);
            }
            else if (expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                expr -> value = control.int_pool.FindOrInsert(left -> value - right -> value);
            }
        }
    }

    return;
}


void Semantic::ProcessSLASH(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else
    {
        BinaryNumericPromotion(expr);

        //
        // This operation may throw ArithmeticException
        //
        SymbolSet *exception_set = TryExceptionTableStack().Top();
        if (exception_set)
        {
            exception_set -> AddElement(control.RuntimeException());
            exception_set -> AddElement(control.Error());
        }

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
             if (expr -> Type() == control.double_type)
             {
                 DoubleLiteralValue *left = (DoubleLiteralValue *) expr -> left_expression -> value;
                 DoubleLiteralValue *right = (DoubleLiteralValue *) expr -> right_expression -> value;

                 expr -> value = control.double_pool.FindOrInsert(left -> value / right -> value);
             }
             else if (expr -> Type() == control.float_type)
             {
                 FloatLiteralValue *left = (FloatLiteralValue *) expr -> left_expression -> value;
                 FloatLiteralValue *right = (FloatLiteralValue *) expr -> right_expression -> value;

                 expr -> value = control.float_pool.FindOrInsert(left -> value / right -> value);
             }
             else if (expr -> Type() == control.long_type)
             {
                 LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                 LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                 if (right -> value == 0)
                 {
                     ReportSemError(SemanticError::ZERO_DIVIDE,
                                    expr -> LeftToken(),
                                    expr -> RightToken());
                 }
                 else expr -> value = control.long_pool.FindOrInsert(left -> value / right -> value);
             }
             else if (expr -> Type() == control.int_type)
             {
                 IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                 IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                 if (right -> value == 0)
                 {
                     ReportSemError(SemanticError::ZERO_DIVIDE,
                                    expr -> LeftToken(),
                                    expr -> RightToken());
                 }
//
// There is a bug in the intel hardware where if one tries to compute ((2**32-1) / -1), he gets a ZeroDivide exception.
// Thus, instead of using the straightforward code below, we use the short-circuited one that follows:
//
//                 else expr -> value = control.int_pool.FindOrInsert(left -> value / right -> value);
//
                 else expr -> value = control.int_pool.FindOrInsert(right -> value == -1 ? -(left -> value)
                                                                                         : left -> value / right -> value);
             }
        }
    }

    return;
}


void Semantic::ProcessMOD(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else
    {
        BinaryNumericPromotion(expr);

        //
        // This operation may throw ArithmeticException
        //
        SymbolSet *exception_set = TryExceptionTableStack().Top();
        if (exception_set)
        {
            exception_set -> AddElement(control.RuntimeException());
            exception_set -> AddElement(control.Error());
        }

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
             if (expr -> Type() == control.double_type)
             {
                 DoubleLiteralValue *left = (DoubleLiteralValue *) expr -> left_expression -> value;
                 DoubleLiteralValue *right = (DoubleLiteralValue *) expr -> right_expression -> value;
                 IEEEdouble result =  IEEEdouble((u4) 0);
                 IEEEdouble::Fmodulus(left -> value, right -> value, result);

                 expr -> value = control.double_pool.FindOrInsert(result);
             }
             else if (expr -> Type() == control.float_type)
             {
                 FloatLiteralValue *left = (FloatLiteralValue *) expr -> left_expression -> value;
                 FloatLiteralValue *right = (FloatLiteralValue *) expr -> right_expression -> value;
                 IEEEfloat result = IEEEfloat(0);
                 IEEEfloat::Fmodulus(left -> value, right -> value, result);

                 expr -> value = control.float_pool.FindOrInsert(result);
             }
             else if (expr -> Type() == control.long_type)
             {
                 LongLiteralValue *left = (LongLiteralValue *) expr -> left_expression -> value;
                 LongLiteralValue *right = (LongLiteralValue *) expr -> right_expression -> value;

                 if (right -> value == 0)
                 {
                     ReportSemError(SemanticError::ZERO_DIVIDE,
                                    expr -> LeftToken(),
                                    expr -> RightToken());
                 }
                 else expr -> value = control.long_pool.FindOrInsert(left -> value % right -> value);
             }
             else if (expr -> Type() == control.int_type)
             {
                 IntLiteralValue *left = (IntLiteralValue *) expr -> left_expression -> value;
                 IntLiteralValue *right = (IntLiteralValue *) expr -> right_expression -> value;

                 if (right -> value == 0)
                 {
                     ReportSemError(SemanticError::ZERO_DIVIDE,
                                    expr -> LeftToken(),
                                    expr -> RightToken());
                 }
//
// There is a bug in the intel hardware where if one tries to compute ((2**32-1) / -1), he gets a ZeroDivide exception.
// Thus, instead of using the straightforward code below, we use the short-circuited one that follows:
//
//                 else expr -> value = control.int_pool.FindOrInsert(left -> value % right -> value);
//
                 else expr -> value = control.int_pool.FindOrInsert((left -> value  == (signed) 0x80000000 &&
                                                                     right -> value == (signed) 0xffffffff)
                                                                                     ? 0
                                                                                     : left -> value % right -> value);
             }
         }
    }

    return;
}


void Semantic::ProcessINSTANCEOF(AstBinaryExpression *expr)
{
    ProcessExpression(expr -> left_expression);
    ProcessExpression(expr -> right_expression);

    TypeSymbol *left_type  = expr -> left_expression -> Type(),
               *right_type = expr -> right_expression -> Type();

    if (left_type -> Primitive())
    {
        ReportSemError(SemanticError::TYPE_NOT_REFERENCE,
                       expr -> left_expression -> LeftToken(),
                       expr -> left_expression -> RightToken(),
                       expr -> left_expression -> Type() -> Name());
    }
    else if (! CanCastConvert(right_type, left_type, expr -> binary_operator_token)) // can left_type (source) be cast into right_type
    {
        ReportSemError(SemanticError::INVALID_INSTANCEOF_CONVERSION,
                       expr -> LeftToken(),
                       expr -> RightToken(),
                       left_type -> ContainingPackage() -> PackageName(),
                       left_type -> ExternalName(),
                       right_type -> ContainingPackage() -> PackageName(),
                       right_type -> ExternalName());
    }

    expr -> symbol = control.boolean_type;

    return;
}


void Semantic::ProcessBinaryExpression(Ast *expr)
{
    AstBinaryExpression *binary_expression = (AstBinaryExpression *) expr;
    (this ->* ProcessBinaryExpr[binary_expression -> binary_tag])(binary_expression);

    return;
}


void Semantic::ProcessTypeExpression(Ast *expr)
{
    AstTypeExpression *type_expression = (AstTypeExpression *) expr;

    AstArrayType *array_type = type_expression -> type -> ArrayTypeCast();
    Ast *actual_type = (array_type ? array_type -> type : type_expression -> type);

    AstPrimitiveType *primitive_type = actual_type -> PrimitiveTypeCast();
    TypeSymbol *type = (primitive_type ? FindPrimitiveType(primitive_type) : MustFindType(actual_type));

    if (array_type)
        type = type -> GetArrayType((Semantic *) this, array_type -> NumBrackets());

    type_expression -> symbol = type;

    return;
}


void Semantic::ProcessConditionalExpression(Ast *expr)
{
    AstConditionalExpression *conditional_expression = (AstConditionalExpression *) expr;

    ProcessExpression(conditional_expression -> test_expression);
    ProcessExpressionOrStringConstant(conditional_expression -> true_expression);
    ProcessExpressionOrStringConstant(conditional_expression -> false_expression);

    TypeSymbol *test_type  = conditional_expression -> test_expression -> Type();
    TypeSymbol *true_type  = conditional_expression -> true_expression -> Type();
    TypeSymbol *false_type = conditional_expression -> false_expression -> Type();

    if (test_type == control.no_type || true_type == control.no_type || false_type == control.no_type)
        conditional_expression -> symbol = control.no_type;
    else if (test_type != control.boolean_type)
    {
        ReportSemError(SemanticError::TYPE_NOT_BOOLEAN,
                       conditional_expression -> test_expression -> LeftToken(),
                       conditional_expression -> test_expression -> RightToken(),
                       conditional_expression -> test_expression -> Type() -> Name());
        conditional_expression -> symbol = control.no_type;
    }
    else if (true_type == control.void_type)
    {
        ReportSemError(SemanticError::TYPE_IS_VOID,
                       conditional_expression -> true_expression -> LeftToken(),
                       conditional_expression -> true_expression -> RightToken(),
                       conditional_expression -> true_expression -> Type() -> Name());
        conditional_expression -> symbol = control.no_type;
    }
    else if (false_type == control.void_type)
    {
        ReportSemError(SemanticError::TYPE_IS_VOID,
                       conditional_expression -> false_expression -> LeftToken(),
                       conditional_expression -> false_expression -> RightToken(),
                       conditional_expression -> false_expression -> Type() -> Name());
        conditional_expression -> symbol = control.no_type;
    }
    else if (true_type -> Primitive())
    {
        if (! false_type -> Primitive() ||
            (true_type != false_type && (true_type == control.boolean_type || false_type == control.boolean_type)))
        {
            ReportSemError(SemanticError::INCOMPATIBLE_TYPE_FOR_CONDITIONAL_EXPRESSION,
                           conditional_expression -> false_expression -> LeftToken(),
                           conditional_expression -> false_expression -> RightToken(),
                           conditional_expression -> false_expression -> Type() -> ContainingPackage() -> PackageName(),
                           conditional_expression -> false_expression -> Type() -> ExternalName(),
                           conditional_expression -> true_expression -> Type() -> ContainingPackage() -> PackageName(),
                           conditional_expression -> true_expression -> Type() -> ExternalName());
            conditional_expression -> symbol = control.no_type;
        }
        else // must be a numeric type
        {
            if (true_type == false_type)
                conditional_expression -> symbol = true_type;
            else // must be a numeric type
            {
                if (true_type == control.byte_type && false_type == control.short_type)
                {
                    conditional_expression -> true_expression =
                                ConvertToType(conditional_expression -> true_expression, control.short_type);
                    conditional_expression -> symbol = control.short_type;
                }
                else if (true_type == control.short_type && false_type == control.byte_type)
                {
                    conditional_expression -> false_expression =
                        ConvertToType(conditional_expression -> false_expression, control.short_type);
                    conditional_expression -> symbol = control.short_type;
                }
                else if (IsIntValueRepresentableInType(conditional_expression -> false_expression, true_type))
                {
                    conditional_expression -> false_expression =
                        ConvertToType(conditional_expression -> false_expression, true_type);
                    conditional_expression -> symbol = true_type;
                }
                else if (IsIntValueRepresentableInType(conditional_expression -> true_expression, false_type))
                {
                    conditional_expression -> true_expression =
                         ConvertToType(conditional_expression -> true_expression, false_type);
                    conditional_expression -> symbol = false_type;
                }
                else BinaryNumericPromotion(conditional_expression);
            }

            //
            // If all the relevant subexpressions are constants, compute the results and
            // set the value of the expression accordingly.
            //
            if (conditional_expression -> test_expression -> IsConstant())
            {
                IntLiteralValue *test = (IntLiteralValue *) conditional_expression -> test_expression -> value;

                if (test -> value && conditional_expression -> true_expression -> IsConstant())
                     conditional_expression -> value = conditional_expression -> true_expression -> value;
                else if ((! test -> value) && conditional_expression -> false_expression -> IsConstant())
                     conditional_expression -> value = conditional_expression -> false_expression -> value;
            }
        }
    }
    else
    {
        if (true_type == false_type)
            conditional_expression -> symbol = true_type;
        else if (false_type -> Primitive())
        {
            ReportSemError(SemanticError::INCOMPATIBLE_TYPE_FOR_CONDITIONAL_EXPRESSION,
                           conditional_expression -> false_expression -> LeftToken(),
                           conditional_expression -> false_expression -> RightToken(),
                           conditional_expression -> false_expression -> Type() -> ContainingPackage() -> PackageName(),
                           conditional_expression -> false_expression -> Type() -> ExternalName(),
                           conditional_expression -> true_expression -> Type() -> ContainingPackage() -> PackageName(),
                           conditional_expression -> true_expression -> Type() -> ExternalName());
            conditional_expression -> symbol = control.no_type;
        }
        else if (true_type == control.null_type)
            conditional_expression -> symbol = false_type;
        else if (false_type == control.null_type)
            conditional_expression -> symbol = true_type;
        else if (CanAssignmentConvert(false_type, conditional_expression -> true_expression))
        {
            conditional_expression -> true_expression = ConvertToType(conditional_expression -> true_expression, false_type);
            conditional_expression -> symbol = false_type;
        }
        else if (CanAssignmentConvert(true_type, conditional_expression -> false_expression))
        {
            conditional_expression -> false_expression = ConvertToType(conditional_expression -> false_expression, true_type);
            conditional_expression -> symbol = true_type;
        }
        else
        {
            ReportSemError(SemanticError::INCOMPATIBLE_TYPE_FOR_CONDITIONAL_EXPRESSION,
                           conditional_expression -> false_expression -> LeftToken(),
                           conditional_expression -> false_expression -> RightToken(),
                           conditional_expression -> false_expression -> Type() -> ContainingPackage() -> PackageName(),
                           conditional_expression -> false_expression -> Type() -> ExternalName(),
                           conditional_expression -> true_expression -> Type() -> ContainingPackage() -> PackageName(),
                           conditional_expression -> true_expression -> Type() -> ExternalName());
            conditional_expression -> symbol = control.no_type;
        }
    }

    return;
}


void Semantic::ProcessAssignmentExpression(Ast *expr)
{
    AstAssignmentExpression *assignment_expression = (AstAssignmentExpression *) expr;

    AstExpression *left_hand_side = assignment_expression -> left_hand_side;

    ProcessExpression(left_hand_side);
    ProcessExpressionOrStringConstant(assignment_expression -> expression);
    TypeSymbol *left_type = left_hand_side -> Type(),
               *right_type = assignment_expression -> expression -> Type();

    assignment_expression -> symbol = left_type;

    if (left_type == control.no_type || right_type == control.no_type)
        return;

    if (left_hand_side -> ArrayAccessCast())
    {
        //
        // This operation may throw ArrayStoreException
        //
        SymbolSet *exception_set = TryExceptionTableStack().Top();
        if (exception_set && (! left_type -> Primitive()))
        {
            exception_set -> AddElement(control.RuntimeException());
            exception_set -> AddElement(control.Error());
        }
    }
    else // the left-hand-side is a name
    {
        MethodSymbol *read_method = NULL;
        AstSimpleName *simple_name = left_hand_side -> SimpleNameCast();
        if (simple_name)
        {
            if (simple_name -> resolution_opt)
               read_method = simple_name -> resolution_opt -> symbol -> MethodCast();
        }
        else
        {
            AstFieldAccess *field_access = (AstFieldAccess *) left_hand_side;
            if (field_access -> resolution_opt)
                read_method = field_access -> resolution_opt -> symbol -> MethodCast();
        }

        if (read_method)
        {
            VariableSymbol *symbol = (VariableSymbol *) read_method -> accessed_member;
            assignment_expression -> write_method = TypeSymbol::GetWriteAccessMethod(symbol);
        }
    }

    if (assignment_expression -> assignment_tag == AstAssignmentExpression::EQUAL)
    {
        if (left_type != right_type)
        {
            if (CanAssignmentConvert(left_type, assignment_expression -> expression))
                assignment_expression -> expression = ConvertToType(assignment_expression -> expression, left_type);
            else if (assignment_expression -> expression -> IsConstant() &&
                     control.IsSimpleIntegerValueType(assignment_expression -> expression -> Type()))
            {
                if (left_type == control.byte_type)
                     ReportSemError(SemanticError::INVALID_BYTE_VALUE,
                                    assignment_expression -> expression -> LeftToken(),
                                    assignment_expression -> expression -> RightToken());
                else if (left_type == control.char_type)
                     ReportSemError(SemanticError::INVALID_CHARACTER_VALUE,
                                    assignment_expression -> expression -> LeftToken(),
                                    assignment_expression -> expression -> RightToken());
                else ReportSemError(SemanticError::INVALID_SHORT_VALUE,
                                    assignment_expression -> expression -> LeftToken(),
                                    assignment_expression -> expression -> RightToken());
            }
            else
            {
                ReportSemError(SemanticError::INCOMPATIBLE_TYPE_FOR_ASSIGNMENT,
                               assignment_expression -> LeftToken(),
                               assignment_expression -> RightToken(),
                               left_type -> ContainingPackage() -> PackageName(),
                               left_type -> ExternalName(),
                               right_type -> ContainingPackage() -> PackageName(),
                               right_type -> ExternalName());
            }
        }

        return;
    }

    //
    // In the current spec, it is stated that the type of both the left-hand
    // and right-hand side of an "op=" assignment must be primitive. However,
    // the left-hand side may be of type String if the operator is "+=" and
    // that case, the right-hand side may also be of type String (or anything
    // else).
    //
    // TODO: CONFIRM THAT THERE WAS A MISTAKE IN THE SPEC.
    //
    if (left_type == control.String() && assignment_expression -> assignment_tag == AstAssignmentExpression::PLUS_EQUAL)
    {
        if (right_type != control.String())
        {
            if (right_type == control.void_type)
                ReportSemError(SemanticError::VOID_TO_STRING,
                               assignment_expression -> expression -> LeftToken(),
                               assignment_expression -> expression -> RightToken());
            assignment_expression -> expression = ConvertToType(assignment_expression -> expression, control.String());
        }

        return;
    }

    if (! left_type -> Primitive())
    {
        ReportSemError(SemanticError::TYPE_NOT_PRIMITIVE,
                       left_hand_side -> LeftToken(),
                       left_hand_side -> RightToken(),
                       left_type -> Name());
        return;
    }

    if (! right_type -> Primitive())
    {
        ReportSemError(SemanticError::TYPE_NOT_PRIMITIVE,
                       assignment_expression -> expression -> LeftToken(),
                       assignment_expression -> expression -> RightToken(),
                       right_type -> Name());
        return;
    }

    switch(assignment_expression -> assignment_tag)
    {
        case AstAssignmentExpression::PLUS_EQUAL:
        case AstAssignmentExpression::STAR_EQUAL:
        case AstAssignmentExpression::SLASH_EQUAL:
        case AstAssignmentExpression::MOD_EQUAL:
        case AstAssignmentExpression::MINUS_EQUAL:
            BinaryNumericPromotion(assignment_expression);
            break;
        case AstAssignmentExpression::LEFT_SHIFT_EQUAL:
        case AstAssignmentExpression::RIGHT_SHIFT_EQUAL:
        case AstAssignmentExpression::UNSIGNED_RIGHT_SHIFT_EQUAL:
             assignment_expression -> left_hand_side = PromoteUnaryNumericExpression(left_hand_side);
             assignment_expression -> expression = PromoteUnaryNumericExpression(assignment_expression -> expression);
             break;
        case AstAssignmentExpression::AND_EQUAL:
        case AstAssignmentExpression::XOR_EQUAL:
        case AstAssignmentExpression::IOR_EQUAL:
             if (left_type != control.boolean_type || right_type != control.boolean_type) // if anyont of the exprs is not boolean
                 BinaryNumericPromotion(assignment_expression);
             break;
        default:
            assert(0);
            break;
    }

    return;
}
