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
#include "double.h"
#include "parser.h"
#include "semantic.h"
#include "control.h"
#include "table.h"
#include "tuple.h"
#include "spell.h"

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif

bool Semantic::IsIntValueRepresentableInType(AstExpression *expr,
                                             TypeSymbol *type)
{
    if (! expr -> IsConstant() ||
        ! control.IsSimpleIntegerValueType(expr -> Type()))
    {
        return false;
    }

    IntLiteralValue *literal = DYNAMIC_CAST<IntLiteralValue *> (expr -> value);

    return (type == control.int_type || type == control.no_type ||
            (type == control.char_type && (literal -> value >= 0) &&
             (literal -> value <= 65535)) ||
             (type == control.byte_type && (literal -> value >= -128) &&
              (literal -> value <= 127)) ||
             (type == control.short_type && (literal -> value >= -32768) &&
              (literal -> value <= 32767)));
}


inline bool Semantic::MoreSpecific(MethodSymbol *source_method,
                                   MethodSymbol *target_method)
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


inline bool Semantic::MoreSpecific(MethodSymbol *method,
                                   Tuple<MethodSymbol *> &maximally_specific_method)
{
    for (int i = 0; i < maximally_specific_method.Length(); i++)
    {
        if (! MoreSpecific(method, maximally_specific_method[i]))
            return false;
    }

    return true;
}


inline bool Semantic::NoMethodMoreSpecific(Tuple<MethodSymbol *> &maximally_specific_method,
                                           MethodSymbol *method)
{
    for (int i = 0; i < maximally_specific_method.Length(); i++)
    {
        if (MoreSpecific(maximally_specific_method[i], method))
            return false;
    }

    return true;
}


//
// Called when no accessible method was found. This checks in order: an
// accessible method of the same name but different parameters, favoring
// methods with the same parameter count; for a no-arg method, an accessible
// field by the same name; an inaccessible method in a superclass; a
// misspelled method name; a type by the same name; and finally no method
// was found.
//
void Semantic::ReportMethodNotFound(AstMethodInvocation *method_call,
                                    TypeSymbol *type)
{
    AstFieldAccess *field_access = method_call -> method -> FieldAccessCast();
    AstSimpleName *simple_name = method_call -> method -> SimpleNameCast();
    assert(field_access || simple_name);

    LexStream::TokenIndex id_token = (field_access
                                      ? field_access -> identifier_token
                                      : simple_name -> identifier_token);
    NameSymbol *name_symbol = lex_stream -> NameSymbol(id_token);
    MethodShadowSymbol *method_shadow;

    if (! type -> expanded_method_table)
        ComputeMethodsClosure(type, id_token);
    if (! type -> expanded_field_table)
        ComputeFieldsClosure(type, id_token);

    //
    // Search for an accessible method with different arguments. Favor the
    // earliest method found with the smallest difference in parameter count.
    // Since the JVMS limits methods to 255 parameters, we initialize our
    // difference detection with 255.
    //
    MethodSymbol *best_match = NULL;
    int difference = 255;
    for (method_shadow = type -> expanded_method_table -> FindMethodShadowSymbol(name_symbol);
         method_shadow; method_shadow = method_shadow -> next_method)
    {
        MethodSymbol *method = method_shadow -> method_symbol;

        if (! method -> IsTyped())
            method -> ProcessMethodSignature((Semantic *) this, id_token);

        if (MemberAccessCheck(field_access, type, method))
        {
            int diff = method_call -> NumArguments() - method -> NumFormalParameters();
            if (diff < 0)
                diff = - diff;
            if (diff < difference)
            {
                best_match = method;
                difference = diff;
            }
        }
    }
    if (best_match)
    {
        ReportSemError(SemanticError::METHOD_OVERLOAD_NOT_FOUND,
                       method_call -> LeftToken(),
                       method_call -> RightToken(),
                       name_symbol -> Name(), // FIXME: should be method_call -> symbol -> Header(), except that method_call has no symbol yet
                       best_match -> containing_type -> ContainingPackage() -> PackageName(),
                       best_match -> containing_type -> ExternalName(),
                       best_match -> Header());
        return;
    }

    //
    // For a no-arg method, search for an accessible field of the same name.
    //
    if (method_call -> NumArguments() == 0)
    {
        VariableShadowSymbol *variable_shadow = type -> expanded_field_table -> FindVariableShadowSymbol(name_symbol);

        if (variable_shadow)
        {
            VariableSymbol *variable = variable_shadow -> variable_symbol;
            if (MemberAccessCheck((AstFieldAccess *) NULL, type, variable))
            {
                TypeSymbol *enclosing_type = variable -> owner -> TypeCast();
                assert(enclosing_type);

                ReportSemError(SemanticError::FIELD_NOT_METHOD,
                               method_call -> LeftToken(),
                               method_call -> RightToken(),
                               variable -> Name(),
                               enclosing_type -> ContainingPackage() -> PackageName(),
                               enclosing_type -> ExternalName());
                return;
            }
        }
    }

    //
    // Check if the method is inaccessible.
    //
    for (TypeSymbol *super_type = type;
         super_type; super_type = super_type -> super)
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
                    AstExpression *expr = method_call -> Argument(i);
                    if (! CanMethodInvocationConvert(method -> FormalParameter(i) -> Type(), expr -> Type()))
                        break;
                }
                if (i == method_call -> NumArguments()) // found a match?
                {
                    //
                    // JLS 9.2: Interfaces do not have protected members,
                    // even though jikes treats interfaces as subtypes of
                    // Object.
                    //
                    if (field_access && method -> ACC_PROTECTED() &&
                        field_access -> base -> Type() -> ACC_INTERFACE())
                    {
                        assert(method -> containing_type == control.Object());
                        ReportSemError(SemanticError::PROTECTED_INTERFACE_METHOD_NOT_ACCESSIBLE,
                                       method_call -> LeftToken(),
                                       method_call -> RightToken(),
                                       method -> Header());
                    }
                    //
                    // A protected instance method in the superclass is
                    // inaccessible if the base expression is the wrong type.
                    //
                    else if (method -> ACC_PROTECTED() &&
                             ! method -> ACC_STATIC() &&
                             ThisType() -> HasProtectedAccessTo(method -> containing_type))
                    {
                        ReportSemError(SemanticError::PROTECTED_INSTANCE_METHOD_NOT_ACCESSIBLE,
                                       method_call -> LeftToken(),
                                       method_call -> RightToken(),
                                       method -> Header(),
                                       method -> containing_type -> ContainingPackage() -> PackageName(),
                                       method -> containing_type -> ExternalName(),
                                       ThisType() -> ContainingPackage() -> PackageName(),
                                       ThisType() -> ExternalName());
                    }
                    else
                    {
                        ReportSemError(SemanticError::METHOD_NOT_ACCESSIBLE,
                                       method_call -> LeftToken(),
                                       method_call -> RightToken(),
                                       method -> Header(),
                                       method -> containing_type -> ContainingPackage() -> PackageName(),
                                       method -> containing_type -> ExternalName(),
                                       method -> AccessString());
                    }
                    return;
                }
            }
        }
    }

    //
    // Search for a misspelled method name.
    //
    best_match = FindMisspelledMethodName(type, method_call, name_symbol);
    if (best_match)
        ReportSemError(SemanticError::METHOD_NAME_MISSPELLED,
                       method_call -> LeftToken(),
                       method_call -> RightToken(),
                       name_symbol -> Name(),
                       type -> ContainingPackage() -> PackageName(),
                       type -> ExternalName(),
                       best_match -> Name());
    //
    // Search for a type of the same name.
    //
    else if (FindType(id_token))
        ReportSemError(SemanticError::TYPE_NOT_METHOD,
                       method_call -> LeftToken(),
                       method_call -> RightToken(),
                       name_symbol -> Name());
    //
    // Give up. We didn't find it.
    //
    else ReportSemError(SemanticError::METHOD_NOT_FOUND,
                        method_call -> LeftToken(),
                        method_call -> RightToken(),
                        name_symbol -> Name(),
                        type -> ContainingPackage() -> PackageName(),
                        type -> ExternalName());
}


void Semantic::ReportConstructorNotFound(Ast *ast, TypeSymbol *type)
{
    wchar_t *name = type -> Name();

    int num_arguments;
    AstExpression **argument;

    AstClassInstanceCreationExpression *class_creation = ast -> ClassInstanceCreationExpressionCast();
    AstSuperCall *super_call = ast -> SuperCallCast();

    if (class_creation)
    {
        num_arguments = class_creation -> NumArguments();
        argument = new AstExpression*[num_arguments + 1];
        for (int i = 0; i < num_arguments; i++)
            argument[i] = class_creation -> Argument(i);
    }
    else if (super_call)
    {
        num_arguments = super_call -> NumArguments();
        argument = new AstExpression*[num_arguments + 1];
        for (int i = 0; i < num_arguments; i++)
            argument[i] = super_call -> Argument(i);
    }
    else
    {
        AstThisCall *this_call = ast -> ThisCallCast();
        assert(this_call);

        num_arguments = this_call -> NumArguments();
        argument = new AstExpression*[num_arguments + 1];
        for (int i = 0; i < num_arguments; i++)
            argument[i] = this_call -> Argument(i);
    }

    int length = wcslen(name);

    for (int i = 0; i < num_arguments; i++)
    {
        TypeSymbol *arg_type = argument[i] -> Type();
        length += arg_type -> ContainingPackage() -> PackageNameLength() +
                  arg_type -> ExternalNameLength() + 3; // '/' after package_name
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
            if (package -> PackageNameLength() > 0 &&
                wcscmp(package_name, StringConstant::US_DO) != 0)
            {
                while (*package_name)
                {
                    *s++ = (*package_name == U_SLASH ? (wchar_t) U_DOT : *package_name);
                    package_name++;
                }
                *s++ = U_DOT;
            }

            for (wchar_t *s2 = arg_type -> ExternalName(); *s2; s2++)
                *s++ = (*s2 == U_DOLLAR ? (wchar_t) U_DOT : *s2);
            *s++ = U_COMMA;
            *s++ = U_SPACE;
        }

        s -= 2; // remove the last ',' and ' '
    }
    *s++ = U_RIGHT_PARENTHESIS;
    *s = U_NULL;

    ReportSemError(SemanticError::CONSTRUCTOR_NOT_FOUND,
                   ast -> LeftToken(),
                   ast -> RightToken(),
                   header);

    delete [] header;
    delete [] argument;
}


MethodSymbol *Semantic::FindConstructor(TypeSymbol *containing_type, Ast *ast,
                                        LexStream::TokenIndex left_tok,
                                        LexStream::TokenIndex right_tok)
{
    Tuple<MethodSymbol *> constructor_set(2); // Stores constructor overloads.

    int num_arguments;
    AstExpression **argument;

    AstClassInstanceCreationExpression *class_creation = ast -> ClassInstanceCreationExpressionCast();
    AstSuperCall *super_call = ast -> SuperCallCast();

    if (class_creation)
    {
        num_arguments = class_creation -> NumArguments();
        argument = new AstExpression*[num_arguments + 1];
        for (int i = 0; i < num_arguments; i++)
            argument[i] = class_creation -> Argument(i);
    }
    else if (super_call)
    {
        num_arguments = super_call -> NumArguments();
        argument = new AstExpression*[num_arguments + 1];
        for (int i = 0; i < num_arguments; i++)
            argument[i] = super_call -> Argument(i);
    }
    else
    {
        AstThisCall *this_call = ast -> ThisCallCast();

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
        else if ((! containing_type -> Bad()) || NumErrors() == 0)
            ReportConstructorNotFound(ast, containing_type);

        delete [] argument;

        return (MethodSymbol *) NULL;
    }
    else if (constructor_set.Length() > 1)
    {
        ReportSemError(SemanticError::AMBIGUOUS_CONSTRUCTOR_INVOCATION,
                       left_tok,
                       right_tok,
                       containing_type -> Name(),
                       constructor_set[0] -> Header(),
                       constructor_set[1] -> Header());
    }

    delete [] argument;

    MethodSymbol *constructor_symbol = constructor_set[0];

    if (constructor_symbol -> IsSynthetic())
    {
        ReportSemError(SemanticError::SYNTHETIC_CONSTRUCTOR_INVOCATION,
                       left_tok,
                       right_tok,
                       constructor_symbol -> Header(),
                       containing_type -> ContainingPackage() -> PackageName(),
                       containing_type -> ExternalName());
    }

    //
    // If this constructor came from a class file, make sure that its throws
    // clause has been processed.
    //
    constructor_symbol -> ProcessMethodThrows((Semantic *) this, right_tok);

    if (control.option.deprecation &&
        constructor_symbol -> IsDeprecated() &&
        constructor_symbol -> containing_type -> outermost_type != ThisType() -> outermost_type)
    {
        ReportSemError(SemanticError::DEPRECATED_METHOD,
                       left_tok,
                       right_tok,
                       constructor_symbol -> Header(),
                       constructor_symbol -> containing_type -> ContainingPackage() -> PackageName(),
                       constructor_symbol -> containing_type -> ExternalName());
    }

    return constructor_symbol;
}


//
//
//
VariableSymbol *Semantic::FindMisspelledVariableName(TypeSymbol *type,
                                                     LexStream::TokenIndex identifier_token)
{
    VariableSymbol *misspelled_variable = NULL;
    int index = 0;
    wchar_t *name = lex_stream -> NameString(identifier_token);

    for (int k = 0; k < type -> expanded_field_table -> symbol_pool.Length(); k++)
    {
        VariableShadowSymbol *variable_shadow = type -> expanded_field_table -> symbol_pool[k];
        VariableSymbol *variable = variable_shadow -> variable_symbol;
        if (! variable -> IsTyped())
            variable -> ProcessVariableSignature((Semantic *) this, identifier_token);

        int new_index = Spell::Index(name, variable -> Name());
        if (new_index > index)
        {
            misspelled_variable = variable;
            index = new_index;
        }
    }

    int length = wcslen(name);

    return ((length == 3 && index >= 5) ||
            (length == 4 && index >= 6) ||
            (length >= 5 && index >= 7)
            ? misspelled_variable : (VariableSymbol *) NULL);
}

//
//
//
MethodSymbol *Semantic::FindMisspelledMethodName(TypeSymbol *type,
                                                 AstMethodInvocation *method_call,
                                                 NameSymbol *name_symbol)
{
    MethodSymbol *misspelled_method = NULL;
    int index = 0;
    AstSimpleName *simple_name = method_call -> method -> SimpleNameCast();
    AstFieldAccess *field_access = method_call -> method -> FieldAccessCast();
    LexStream::TokenIndex identifier_token = (simple_name ? simple_name -> identifier_token : field_access -> identifier_token);

    for (int k = 0; k < type -> expanded_method_table -> symbol_pool.Length(); k++)
    {
        MethodShadowSymbol *method_shadow = type -> expanded_method_table -> symbol_pool[k];
        MethodSymbol *method = method_shadow -> method_symbol;

        if (! method -> IsTyped())
            method -> ProcessMethodSignature((Semantic *) this, identifier_token);

        if (method_call -> NumArguments() == method -> NumFormalParameters())
        {
            int i;
            for (i = 0; i < method_call -> NumArguments(); i++)
            {
                AstExpression *expr = method_call -> Argument(i);
                if (! CanMethodInvocationConvert(method -> FormalParameter(i) -> Type(), expr -> Type()))
                    break;
            }
            if (i == method_call -> NumArguments())
            {
                int new_index = Spell::Index(name_symbol -> Name(), method -> Name());
                if (new_index > index)
                {
                    misspelled_method = method;
                    index = new_index;
                }
            }
        }
    }

    int length = name_symbol -> NameLength(),
         num_args = method_call -> NumArguments();

    //
    // If we have a name of length 2, accept >= 30% probality if the function
    // takes at least one argument. If we have a name of length 3,
    // accept >= 50% probality if the function takes at least one argument.
    // Otherwise, if the length of the name is > 3, accept >= 60% probability.
    //
    return (index < 3 ? (MethodSymbol *) NULL
            : ((length == 2 && (index >= 3 || num_args > 0)) ||
               (length == 3 && (index >= 5 || num_args > 0)) ||
               (length  > 3 && (index >= 6 || (index >= 5 && num_args > 0))))
            ? misspelled_method : (MethodSymbol *) NULL);
}


//
// Search the type in question for a method. Note that name_symbol is an
// optional argument. If it was not passed to this function then its default
// value is NULL (see semantic.h) and we assume that the name to search for
// is the name specified in the field_access of the method_call.
//
MethodSymbol *Semantic::FindMethodInType(TypeSymbol *type,
                                         AstMethodInvocation *method_call,
                                         NameSymbol *name_symbol)
{
    Tuple<MethodSymbol *> method_set(2); // Stores method overloads.
    AstFieldAccess *field_access = method_call -> method -> FieldAccessCast();
    if (! name_symbol)
        name_symbol = lex_stream -> NameSymbol(field_access -> identifier_token);

    if (! type -> expanded_method_table)
        ComputeMethodsClosure(type, field_access -> identifier_token);

    //
    // Here, we ignore any conflicts in a method declaration. If there are
    // conflicts, they are necessarily abstract methods inherited from
    // interfaces, so either the original method implements them all, or it
    // is also abstract and we are free to choose which one to use.
    //
    for (MethodShadowSymbol *method_shadow = type -> expanded_method_table -> FindMethodShadowSymbol(name_symbol);
         method_shadow; method_shadow = method_shadow -> next_method)
    {
        MethodSymbol *method = method_shadow -> method_symbol;

        if (! method -> IsTyped())
            method -> ProcessMethodSignature((Semantic *) this,
                                             field_access -> identifier_token);

        if (method_call -> NumArguments() == method -> NumFormalParameters() &&
            MemberAccessCheck(field_access, type, method))
        {
            int i;
            for (i = 0; i < method_call -> NumArguments(); i++)
            {
                AstExpression *expr = method_call -> Argument(i);
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

    if (method_set.Length() == 0)
    {
        ReportMethodNotFound(method_call, type);
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

    MethodSymbol *method = method_set[0];
    if (method -> IsSynthetic())
    {
        ReportSemError(SemanticError::SYNTHETIC_METHOD_INVOCATION,
                       method_call -> LeftToken(),
                       method_call -> RightToken(),
                       method -> Header(),
                       method -> containing_type -> ContainingPackage() -> PackageName(),
                       method -> containing_type -> ExternalName());
    }

    //
    // If this method came from a class file, make sure that its throws clause
    // has been processed.
    //
    method -> ProcessMethodThrows((Semantic *) this, field_access -> identifier_token);

    if (control.option.deprecation &&
        method -> IsDeprecated() && method -> containing_type -> outermost_type != ThisType() -> outermost_type)
    {
        ReportSemError(SemanticError::DEPRECATED_METHOD,
                       method_call -> LeftToken(),
                       method_call -> RightToken(),
                       method -> Header(),
                       method -> containing_type -> ContainingPackage() -> PackageName(),
                       method -> containing_type -> ExternalName());
    }

    return method;
}


void Semantic::FindMethodInEnvironment(Tuple<MethodSymbol *> &methods_found,
                                       SemanticEnvironment *&where_found,
                                       SemanticEnvironment *stack,
                                       AstMethodInvocation *method_call)
{
    AstSimpleName *simple_name = method_call -> method -> SimpleNameCast();
    NameSymbol *name_symbol = lex_stream -> NameSymbol(simple_name -> identifier_token);

    for (SemanticEnvironment *env = stack; env; env = env -> previous)
    {
        TypeSymbol *type = env -> Type();
        if (! type -> expanded_method_table)
            ComputeMethodsClosure(type, simple_name -> identifier_token);

        methods_found.Reset();
        where_found = NULL;

        //
        // If this environment contained a method with the right name, the
        // search stops:
        //
        //    "Class scoping does not influence overloading: if the inner
        //     class has one print method, the simple method name 'print'
        //     refers to that method, not any of the ten 'print' methods in
        //     the enclosing class."
        //
        MethodShadowSymbol *method_shadow = type -> expanded_method_table -> FindMethodShadowSymbol(name_symbol);
        if (method_shadow)
        {
            for (; method_shadow; method_shadow = method_shadow -> next_method)
            {
                MethodSymbol *method = method_shadow -> method_symbol;

                if (! method -> IsTyped())
                    method -> ProcessMethodSignature((Semantic *) this, simple_name -> identifier_token);

                //
                // Since type -> IsOwner(this_type()), i.e., type encloses
                // this_type(), method is accessible, even if it is private.
                //
                if (method_call -> NumArguments() == method -> NumFormalParameters())
                {
                    int i;
                    for (i = 0; i < method_call -> NumArguments(); i++)
                    {
                        AstExpression *expr = method_call -> Argument(i);
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

            //
            // If a match was found, save the environment
            //
            where_found = (methods_found.Length() > 0 ? env : (SemanticEnvironment *) NULL);
            break;
        }
    }
}


MethodSymbol *Semantic::FindMethodInEnvironment(SemanticEnvironment *&where_found,
                                                SemanticEnvironment *stack,
                                                AstMethodInvocation *method_call)
{
    Tuple<MethodSymbol *> methods_found(2);
    FindMethodInEnvironment(methods_found, where_found, stack, method_call);

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
            if (method_symbol -> IsSynthetic())
            {
                ReportSemError(SemanticError::SYNTHETIC_METHOD_INVOCATION,
                               method_call -> LeftToken(),
                               method_call -> RightToken(),
                               method_symbol -> Header(),
                               method_symbol -> containing_type -> ContainingPackage() -> PackageName(),
                               method_symbol -> containing_type -> ExternalName());
            }
            else if (! where_found -> Type() -> ACC_STATIC())
            {
                Tuple<MethodSymbol *> others(2);
                SemanticEnvironment *found_other,
                                    *previous_env = where_found -> previous;
                FindMethodInEnvironment(others, found_other, previous_env, method_call);

                if (others.Length() > 0 && where_found -> Type() -> CanAccess(found_other -> Type()))
                {
                    for (int i = 0; i < others.Length();  i++)
                    {
                        if (! (others[i] == method_symbol && method_symbol -> ACC_STATIC()))
                        {
                            ReportSemError(SemanticError::INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER,
                                           method_call -> LeftToken(),
                                           method_call -> RightToken(),
                                           method_symbol -> Name(),
                                           method_symbol -> containing_type -> ContainingPackage() -> PackageName(),
                                           method_symbol -> containing_type -> ExternalName(),
                                           found_other -> Type() -> ContainingPackage() -> PackageName(),
                                           found_other -> Type() -> ExternalName());
                            break; // emit only one error message
                        }
                    }
                }
            }
        }
    }
    else
    {
        AstSimpleName *simple_name = method_call -> method -> SimpleNameCast();
        NameSymbol *name_symbol = lex_stream -> NameSymbol(simple_name -> identifier_token);
        bool symbol_found = false;

        //
        // First, search for a perfect visible method match in an enclosing
        // class.
        //
        assert(stack);
        for (SemanticEnvironment *env = stack -> previous; env; env = env -> previous)
        {
            Tuple<MethodSymbol *> others(2);
            SemanticEnvironment *found_other;
            FindMethodInEnvironment(others, found_other, env, method_call);

            if (others.Length() > 0)
            {
                ReportSemError(SemanticError::HIDDEN_METHOD_IN_ENCLOSING_CLASS,
                               method_call -> LeftToken(),
                               method_call -> RightToken(),
                               others[0] -> Header(),
                               others[0] -> containing_type -> ContainingPackage() -> PackageName(),
                               others[0] -> containing_type -> ExternalName());

                symbol_found = true;
                break;
            }
        }

        //
        // If a method in an enclosing class was not found. Search for a
        // similar visible field or a private method with the name.
        //
        for (SemanticEnvironment *env2 = stack; env2 && (! symbol_found); env2 = env2 -> previous)
        {
            TypeSymbol *type = env2 -> Type();

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
                    ReportSemError(SemanticError::METHOD_NOT_ACCESSIBLE,
                                   method_call -> LeftToken(),
                                   method_call -> RightToken(),
                                   name_symbol -> Name(),
                                   super_type -> ContainingPackage() -> PackageName(),
                                   super_type -> ExternalName(),
                                   method_shadow -> method_symbol -> AccessString());
                    assert(! method_shadow -> method_symbol -> ACC_PROTECTED()); // protected access in subclass is always possible by simple name
                    symbol_found = true;
                    break;
                }
            }
        }

        //
        // Finally, if we did not find a method or field name that matches,
        // look for a type with that name.
        //
        if (! symbol_found)
        {
            TypeSymbol *this_type = ThisType();

            if (FindType(simple_name -> identifier_token))
            {
                ReportSemError(SemanticError::TYPE_NOT_METHOD,
                               simple_name -> identifier_token,
                               simple_name -> identifier_token,
                               name_symbol -> Name());
            }
            else if (this_type -> expanded_method_table -> FindMethodShadowSymbol(name_symbol))
                ReportMethodNotFound(method_call, this_type);
            else
            {
                MethodSymbol *method = FindMisspelledMethodName(this_type, method_call, name_symbol);
                if (method)
                     ReportSemError(SemanticError::METHOD_NAME_MISSPELLED,
                                    method_call -> LeftToken(),
                                    method_call -> RightToken(),
                                    name_symbol -> Name(),
                                    this_type -> ContainingPackage() -> PackageName(),
                                    this_type -> ExternalName(),
                                    method -> Name());
                else ReportSemError(SemanticError::METHOD_NOT_FOUND,
                                    method_call -> LeftToken(),
                                    method_call -> RightToken(),
                                    name_symbol -> Name(),
                                    this_type -> ContainingPackage() -> PackageName(),
                                    this_type -> ExternalName());
            }
        }
    }

    //
    // If this method came from a class file, make sure that its throws clause
    // has been processed.
    //
    if (method_symbol)
    {
        method_symbol -> ProcessMethodThrows((Semantic *) this, method_call -> method -> RightToken());

        if (control.option.deprecation &&
            method_symbol -> IsDeprecated() && method_symbol -> containing_type -> outermost_type != ThisType() -> outermost_type)
        {
            ReportSemError(SemanticError::DEPRECATED_METHOD,
                           method_call -> LeftToken(),
                           method_call -> RightToken(),
                           method_symbol -> Header(),
                           method_symbol -> containing_type -> ContainingPackage() -> PackageName(),
                           method_symbol -> containing_type -> ExternalName());
        }
    }

    return method_symbol;
}



//
// Search the type in question for a variable. Note that name_symbol is an
// optional argument. If it was not passed to this function then its default
// value is NULL (see semantic.h) and we assume that the name to search for
// is the last identifier specified in the field_access. Error reporting if
// the field is not found is up to the callee, since for qualified names,
// the name may successfully resolve to a nested type.
//
VariableSymbol *Semantic::FindVariableInType(TypeSymbol *type,
                                             AstFieldAccess *field_access,
                                             NameSymbol *name_symbol)
{
    Tuple<VariableSymbol *> variable_set(2); // Stores variable conflicts.
    VariableSymbol *variable;
    if (! name_symbol)
        name_symbol = lex_stream -> NameSymbol(field_access -> identifier_token);

    if (! type -> expanded_field_table)
        ComputeFieldsClosure(type, field_access -> identifier_token);

    //
    // Find the accessible fields with the correct name in the type.
    //
    VariableShadowSymbol *variable_shadow = type -> expanded_field_table -> FindVariableShadowSymbol(name_symbol);

    if (variable_shadow)
    {
        variable = variable_shadow -> variable_symbol;

        if (! variable -> IsTyped())
            variable -> ProcessVariableSignature((Semantic *) this, field_access -> identifier_token);

        if (MemberAccessCheck(field_access, type, variable))
            variable_set.Next() = variable;

        for (int i = 0; i < variable_shadow -> NumConflicts(); i++)
        {
            VariableSymbol *variable = variable_shadow -> Conflict(i);

            if (! variable -> IsTyped())
                variable -> ProcessVariableSignature((Semantic *) this, field_access -> identifier_token);

            if (MemberAccessCheck(field_access, type, variable))
                variable_set.Next() = variable;
        }
    }

    if (variable_set.Length() == 0)
    {
        return (VariableSymbol *) NULL;
    }
    else if (variable_set.Length() > 1)
    {
        ReportSemError(SemanticError::AMBIGUOUS_FIELD,
                       field_access -> LeftToken(),
                       field_access -> RightToken(),
                       name_symbol -> Name(),
                       variable_set[0] -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                       variable_set[0] -> owner -> TypeCast() -> ExternalName(),
                       variable_set[1] -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                       variable_set[1] -> owner -> TypeCast() -> ExternalName());
    }

    variable = variable_set[0];
    if (variable -> IsSynthetic())
    {
        ReportSemError(SemanticError::SYNTHETIC_VARIABLE_ACCESS,
                       field_access -> LeftToken(),
                       field_access -> RightToken(),
                       variable -> Name(),
                       variable -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                       variable -> owner -> TypeCast() -> ExternalName());
    }

    if (control.option.deprecation &&
        variable -> IsDeprecated() &&
        variable -> owner -> TypeCast() -> outermost_type != type -> outermost_type)
    {
        ReportSemError(SemanticError::DEPRECATED_FIELD,
                       field_access -> LeftToken(),
                       field_access -> RightToken(),
                       variable -> Name(),
                       variable -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                       variable -> owner -> TypeCast() -> ExternalName());
    }

    return variable;
}


//
// Called when no accessible variable was found. The access must be one of
// AstFieldAccess or AstSimpleName. This checks in order: an accessible no-arg
// method by the same name, an inaccessible field in a superclass, a
// misspelled field name, a type by the same name, and finally the field was
// not found.
//
void Semantic::ReportVariableNotFound(AstExpression *access, TypeSymbol *type)
{
    AstFieldAccess *field_access = access -> FieldAccessCast();
    AstSimpleName *simple_name = access -> SimpleNameCast();
    assert(field_access || simple_name);

    LexStream::TokenIndex id_token = field_access ? field_access -> identifier_token : simple_name -> identifier_token;
    NameSymbol *name_symbol = lex_stream -> NameSymbol(id_token);
    VariableShadowSymbol *variable_shadow;

    if (! type -> expanded_field_table)
        ComputeFieldsClosure(type, id_token);
    if (! type -> expanded_method_table)
        ComputeMethodsClosure(type, id_token);

    //
    // Search for an accessible no-arg method of the same name.
    //
    MethodShadowSymbol *method_shadow;
    for (method_shadow = type -> expanded_method_table -> FindMethodShadowSymbol(name_symbol);
         method_shadow; method_shadow = method_shadow -> next_method)
    {
        MethodSymbol *method = method_shadow -> method_symbol;

        //
        // Make sure that method has been fully prepared.
        //
        if (! method -> IsTyped())
            method -> ProcessMethodSignature((Semantic *) this, id_token);

        if (method -> NumFormalParameters() == 0 &&
            MemberAccessCheck((AstFieldAccess *) NULL, type, method))
        {
            ReportSemError(SemanticError::METHOD_NOT_FIELD,
                           id_token,
                           id_token,
                           name_symbol -> Name(),
                           method -> containing_type -> ContainingPackage() -> PackageName(),
                           method -> containing_type -> ExternalName());
            return;
        }
    }

    //
    // Check if the field is inaccessible.
    //
    for (TypeSymbol *super_type = type;
         super_type; super_type = super_type -> super)
    {
        variable_shadow = super_type -> expanded_field_table -> FindVariableShadowSymbol(name_symbol);
        if (variable_shadow)
        {
            VariableSymbol *variable = variable_shadow -> variable_symbol;
            TypeSymbol *containing_type = variable -> owner -> TypeCast();

            //
            // A protected instance field in the superclass is inaccessible if
            // the base expression is the wrong type.
            //
            if (variable -> ACC_PROTECTED() &&
                ! variable -> ACC_STATIC() &&
                ThisType() -> HasProtectedAccessTo(containing_type))
            {
                ReportSemError(SemanticError::PROTECTED_INSTANCE_FIELD_NOT_ACCESSIBLE,
                               id_token,
                               id_token,
                               name_symbol -> Name(),
                               containing_type -> ContainingPackage() -> PackageName(),
                               containing_type -> ExternalName(),
                               ThisType() -> ContainingPackage() -> PackageName(),
                               ThisType() -> ExternalName());
            }
            else
            {
                ReportSemError(SemanticError::FIELD_NOT_ACCESSIBLE,
                               id_token,
                               id_token,
                               name_symbol -> Name(),
                               containing_type -> ContainingPackage() -> PackageName(),
                               containing_type -> ExternalName(),
                               variable -> AccessString());
            }
            return;
        }
    }

    //
    // Search for a misspelled field name.
    //
    VariableSymbol *variable = FindMisspelledVariableName(type, id_token);
    if (variable)
        ReportSemError(SemanticError::FIELD_NAME_MISSPELLED,
                       id_token,
                       id_token,
                       name_symbol -> Name(),
                       type -> ContainingPackage() -> PackageName(),
                       type -> ExternalName(),
                       variable -> Name());
    //
    // Search for a type of the same name.
    //
    else if (FindType(id_token))
        ReportSemError(SemanticError::TYPE_NOT_FIELD,
                       id_token,
                       id_token,
                       name_symbol -> Name());
    //
    // Give up. We didn't find it.
    //
    else ReportSemError(SemanticError::FIELD_NOT_FOUND,
                        id_token,
                        id_token,
                        name_symbol -> Name(),
                        type -> ContainingPackage() -> PackageName(),
                        type -> ExternalName());
}


void Semantic::FindVariableInEnvironment(Tuple<VariableSymbol *> &variables_found,
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
        VariableShadowSymbol *variable_shadow = type -> expanded_field_table -> FindVariableShadowSymbol(name_symbol);
        if (variable_shadow)
        {
            //
            // Since type -> IsOwner(this_type()), i.e., type encloses
            // this_type(), variable_symbol is accessible, even if it is
            // private.
            //
            variables_found.Next() = variable_shadow -> variable_symbol;

            //
            // Recall that even an inaccessible member x of a super class (or
            // interface) S, in addition to not been inherited by a subclass,
            // hides all other occurrences of x that may appear in a super
            // class (or super interface) of S (see 8.3).
            //
            for (int i = 0; i < variable_shadow -> NumConflicts(); i++)
                variables_found.Next() = variable_shadow -> Conflict(i);
            where_found = env;
            break;
        }
    }
}


VariableSymbol *Semantic::FindVariableInEnvironment(SemanticEnvironment *&where_found,
                                                    SemanticEnvironment *stack, LexStream::TokenIndex identifier_token)
{
    Tuple<VariableSymbol *> variables_found(2);
    NameSymbol *name_symbol = lex_stream -> NameSymbol(identifier_token);
    FindVariableInEnvironment(variables_found, where_found, stack, name_symbol, identifier_token);

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
                                   lex_stream -> NameString(identifier_token),
                                   //
                                   // TODO: What if the method is a constructor ?
                                   //        if (method -> Identity() != control.init_symbol &&
                                   //            method -> Identity() != control.block_init_symbol &&
                                   //            method -> Identity() != control.clinit_symbol)
                                   //
                                   //
                                   method -> Name());
                }

                //
                // Insert a local shadow in the type. If we are currently
                // processing a constructor, the local shadow would have been
                // passed to it as an argument. If so, use the local argument;
                // otherwise, use the local shadow.
                //
                VariableSymbol *local_shadow = type -> FindOrInsertLocalShadow(variable_symbol),
                               *local_symbol = stack -> symbol_table.FindVariableSymbol(local_shadow -> Identity());
                variable_symbol = (local_symbol ? local_symbol : local_shadow);

                assert(variable_symbol);

                where_found = stack;
            }
        }
        else if (variable_symbol -> owner != where_found -> Type())  // is symbol an inherited field?
        {
            TypeSymbol *type = (TypeSymbol *) variable_symbol -> owner;

            if (variable_symbol -> IsSynthetic())
            {
                ReportSemError(SemanticError::SYNTHETIC_VARIABLE_ACCESS,
                               identifier_token,
                               identifier_token,
                               variable_symbol -> Name(),
                               type -> ContainingPackage() -> PackageName(),
                               type -> ExternalName());
            }
            else if (! where_found -> Type() -> ACC_STATIC())
            {
                Tuple<VariableSymbol *> others(2);
                SemanticEnvironment *found_other,
                                    *previous_env = where_found -> previous;
                FindVariableInEnvironment(others, found_other, previous_env, name_symbol, identifier_token);

                if (others.Length() > 0 && where_found -> Type() -> CanAccess(found_other -> Type()))
                {
                    for (int i = 0; i < others.Length();  i++)
                    {
                        if (! (others[i] == variable_symbol && variable_symbol -> ACC_STATIC()))
                        {
                            MethodSymbol *method = others[i] -> owner -> MethodCast();

                            if (method)
                            {
                                ReportSemError(SemanticError::INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_LOCAL,
                                               identifier_token,
                                               identifier_token,
                                               lex_stream -> NameString(identifier_token),
                                               type -> ContainingPackage() -> PackageName(),
                                               type -> ExternalName(),
                                               method -> Name());
                                break;
                            }
                            else
                            {
                                ReportSemError(SemanticError::INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER,
                                               identifier_token,
                                               identifier_token,
                                               lex_stream -> NameString(identifier_token),
                                               type -> ContainingPackage() -> PackageName(),
                                               type -> ExternalName(),
                                               found_other -> Type() -> ContainingPackage() -> PackageName(),
                                               found_other -> Type() -> ExternalName());
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    for (int i = 1; i < variables_found.Length(); i++)
    {
        ReportSemError(SemanticError::AMBIGUOUS_FIELD,
                       identifier_token,
                       identifier_token,
                       variable_symbol -> Name(),
                       variable_symbol -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                       variable_symbol -> owner -> TypeCast() -> ExternalName(),
                       variables_found[i] -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                       variables_found[i] -> owner -> TypeCast() -> ExternalName());
    }

    if (variable_symbol)
    {
        if (control.option.deprecation &&
            variable_symbol -> IsDeprecated() &&
            variable_symbol -> owner -> TypeCast() -> outermost_type != ThisType() -> outermost_type)
        {
            ReportSemError(SemanticError::DEPRECATED_FIELD,
                           identifier_token,
                           identifier_token,
                           variable_symbol -> Name(),
                           variable_symbol -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                           variable_symbol -> owner -> TypeCast() -> ExternalName());
        }

        if (! variable_symbol -> IsTyped())
            variable_symbol -> ProcessVariableSignature((Semantic *) this, identifier_token);
    }

    return variable_symbol;
}


VariableSymbol *Semantic::FindInstance(TypeSymbol *base_type,
                                       TypeSymbol *environment_type,
                                       bool innermost)
{
    assert(base_type != environment_type);

    for (int i = 0; i < base_type -> NumEnclosingInstances(); i++)
    {
        VariableSymbol *variable = base_type -> EnclosingInstance(i);
        TypeSymbol *type = variable -> Type();
        if (environment_type == type ||
            (innermost && variable -> Type() -> IsSubclass(environment_type)))
        {
            return variable;
        }
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
        AstSimpleName *parm = compilation_unit -> ast_pool -> GenSimpleName(loc);
        parm -> symbol = base_type -> EnclosingInstance(k - 1);

        VariableSymbol *variable_symbol = (VariableSymbol *) type -> FindThis(0);

        AstFieldAccess *method_name = compilation_unit -> ast_pool -> GenFieldAccess();
        method_name -> base = parm;
        method_name -> dot_token = loc;
        method_name -> identifier_token = loc;
        method_name -> symbol = variable_symbol; // the variable in question

        AstMethodInvocation *rhs       = compilation_unit -> ast_pool -> GenMethodInvocation();
        rhs -> method                  = method_name;
        rhs -> left_parenthesis_token  = loc;
        rhs -> right_parenthesis_token = loc;
        rhs -> symbol                  = type -> GetReadAccessMethod(variable_symbol);
        rhs -> AddArgument(parm); // TODO: WARNING: sharing of Ast subtree !!!


        AstSimpleName *lhs = compilation_unit -> ast_pool -> GenSimpleName(loc);
        VariableSymbol *variable = base_type -> FindThis(k);
        lhs -> symbol = (variable ? variable : base_type -> InsertThis(k));

        AstAssignmentExpression *assign = compilation_unit -> ast_pool
                                                           -> GenAssignmentExpression(AstAssignmentExpression::SIMPLE_EQUAL, loc);
        assign -> left_hand_side = lhs;
        assign -> expression     = rhs;
        assign -> symbol         = lhs -> Type();

        AstExpressionStatement *stmt  = compilation_unit -> ast_pool -> GenExpressionStatement();
        stmt -> expression            = assign;
        stmt -> semicolon_token_opt   = loc;
        stmt -> is_reachable          = true;
        stmt -> can_complete_normally = true;

        this_block -> AddStatement(stmt);

        if (lhs -> Type() -> IsSubclass(environment_type))
            break;
    }

    return base_type -> EnclosingInstance(k);
}


AstExpression *Semantic::CreateAccessToType(Ast *source,
                                            TypeSymbol *environment_type)
{
    TypeSymbol *this_type = ThisType();

    LexStream::TokenIndex tok;

    AstSimpleName *simple_name = source -> SimpleNameCast();
    AstFieldAccess *field_access = source -> FieldAccessCast();
    AstSuperCall *super_call = source -> SuperCallCast();
    AstClassInstanceCreationExpression *class_creation = source -> ClassInstanceCreationExpressionCast();
    bool is_super = false;
    bool is_this = false;

    if (simple_name)
         tok = simple_name -> identifier_token;
    else if (class_creation)
         tok = class_creation -> new_token;
    else if (super_call)
         tok = super_call -> super_token;
    else if (field_access)
    {
        if (field_access -> IsSuperAccess())
            is_super = true;
        else if (field_access -> IsThisAccess())
            is_this = true;
        assert(is_super || is_this);
        tok = field_access -> dot_token;
    }
    else assert(false);

    AstExpression *resolution;

    if (! this_type -> CanAccess(environment_type))
    {
        if (ExplicitConstructorInvocation())
            ReportSemError(SemanticError::ENCLOSING_INSTANCE_ACCESS_FROM_CONSTRUCTOR_INVOCATION,
                           (field_access ? field_access -> base -> LeftToken() : tok),
                           (field_access ? field_access -> base -> RightToken() : tok),
                           environment_type -> ContainingPackage() -> PackageName(),
                           environment_type -> ExternalName());
        else
        {
            SemanticEnvironment *env = state_stack.Top();
            for (; env; env = env -> previous) // check whether or not there is an intervening static type...
            {
                if (env -> StaticRegion() || env -> Type() -> ACC_STATIC())
                    break;
            }

            if (env)
                 ReportSemError(SemanticError::ENCLOSING_INSTANCE_ACCESS_ACROSS_STATIC_REGION,
                                (field_access ? field_access -> base -> LeftToken() : tok),
                                (field_access ? field_access -> base -> RightToken() : tok),
                                environment_type -> ContainingPackage() -> PackageName(),
                                environment_type -> ExternalName(),
                                env -> Type() -> ContainingPackage() -> PackageName(),
                                env -> Type() -> ExternalName());
            else ReportSemError(SemanticError::ENCLOSING_INSTANCE_NOT_ACCESSIBLE,
                                (field_access ? field_access -> base -> LeftToken() : tok),
                                (field_access ? field_access -> base -> RightToken() : tok),
                                environment_type -> ContainingPackage() -> PackageName(),
                                environment_type -> ExternalName());
        }

        resolution = compilation_unit -> ast_pool -> GenSimpleName(tok);
        resolution -> symbol = control.no_type;
    }
    else if (ExplicitConstructorInvocation())
    {
        VariableSymbol *variable = LocalSymbolTable().FindVariableSymbol(control.this0_name_symbol);

        assert(variable);

        resolution = compilation_unit -> ast_pool -> GenSimpleName(tok);
        resolution -> symbol = variable;

        //
        // TODO: Document this !!!
        //
        if (ExplicitConstructorInvocation() -> SuperCallCast())
        {
            TypeSymbol *containing_type = this_type -> ContainingType();
            if (! containing_type -> IsSubclass(environment_type))
            {
                variable = FindInstance(containing_type, environment_type,
                                        true);

                AstFieldAccess *method_name = compilation_unit -> ast_pool -> GenFieldAccess();
                method_name -> base = resolution; // TODO: WARNING: sharing of Ast subtree !!!
                method_name -> dot_token = tok;
                method_name -> identifier_token = tok;
                method_name -> symbol = variable;

                AstMethodInvocation *method_call       = compilation_unit -> ast_pool -> GenMethodInvocation();
                method_call -> method                  = method_name;
                method_call -> left_parenthesis_token  = tok;
                method_call -> right_parenthesis_token = tok;

                assert(containing_type == variable -> owner -> TypeCast());

                method_call -> symbol = containing_type -> GetReadAccessMethod(variable);
                method_call -> AddArgument(resolution);

                resolution = method_call;
            }
        }
    }
    //
    // Collapse everything except qualified this or super to the innermost
    // class.
    //
    else if (this_type == environment_type ||
             (this_type -> IsSubclass(environment_type) && (! is_super) &&
              (! is_this)))
    {
        resolution = compilation_unit -> ast_pool -> GenThisExpression(tok);
        resolution -> symbol = this_type;
    }
    else
    {
        resolution = compilation_unit -> ast_pool -> GenSimpleName(tok);
        resolution -> symbol = FindInstance(this_type, environment_type,
                                            (! is_super) && (! is_this));
    }

    if (is_super)
        environment_type = environment_type -> super;

    return ((resolution -> symbol == control.no_type) ||
            (resolution -> Type() == environment_type)
            ? resolution : ConvertToType(resolution, environment_type));
}


void Semantic::CreateAccessToScopedVariable(AstSimpleName *simple_name,
                                            TypeSymbol *environment_type)
{
    assert(environment_type -> IsOwner(ThisType()) &&
           (! simple_name -> IsConstant()));

    VariableSymbol *variable = (VariableSymbol *) simple_name -> symbol;

    AstExpression *access_expression;
    if (variable -> ACC_STATIC())
    {
        access_expression = compilation_unit -> ast_pool -> GenSimpleName(simple_name -> identifier_token);
        access_expression -> symbol = environment_type;
    }
    else access_expression = CreateAccessToType(simple_name, environment_type);

    if (access_expression -> symbol != control.no_type)
    {
        assert(variable -> owner -> TypeCast());

        TypeSymbol *containing_type = variable -> owner -> TypeCast();

        if (variable -> ACC_PRIVATE() ||
            (variable -> ACC_PROTECTED() &&
             ! ProtectedAccessCheck(containing_type)))
        {
            assert((variable -> ACC_PRIVATE() &&
                    environment_type == containing_type) ||
                   (variable -> ACC_PROTECTED() &&
                    environment_type -> IsSubclass(containing_type)));

            AstFieldAccess *method_name     = compilation_unit -> ast_pool -> GenFieldAccess();
            method_name -> base             = access_expression;
            method_name -> dot_token        = simple_name -> identifier_token;
            method_name -> identifier_token = simple_name -> identifier_token;
            method_name -> symbol           = variable;

            AstMethodInvocation *accessor       = compilation_unit -> ast_pool -> GenMethodInvocation();
            accessor -> method                  = method_name;
            accessor -> left_parenthesis_token  = simple_name -> identifier_token;
            accessor -> right_parenthesis_token = simple_name -> identifier_token;
            accessor -> symbol                  = environment_type -> GetReadAccessMethod(variable); // default base type is appropriate

            if (! variable -> ACC_STATIC())
                accessor -> AddArgument(access_expression); // TODO: WARNING: sharing of Ast subtree !!!

            simple_name -> resolution_opt = accessor;
        }
        else
        {
            AstFieldAccess *field_access     = compilation_unit -> ast_pool -> GenFieldAccess();
            field_access -> base             = access_expression;
            field_access -> dot_token        = simple_name -> identifier_token;
            field_access -> identifier_token = simple_name -> identifier_token;
            field_access -> symbol           = variable;

            simple_name -> resolution_opt = field_access;
        }
    }
}


void Semantic::CreateAccessToScopedMethod(AstMethodInvocation *method_call,
                                          TypeSymbol *environment_type)
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
        // TODO: we have filed a query to Sun regarding the necessity of this
        // check!
        //
        // SimpleNameAccessCheck(simple_name, method -> containing_type, method);
        //
        simple_name -> resolution_opt = access_expression;

        TypeSymbol *containing_type = method -> containing_type;

        if (method -> ACC_PRIVATE() ||
            (method -> ACC_PROTECTED() &&
             ! ProtectedAccessCheck(containing_type)))
        {
            assert((method -> ACC_PRIVATE() &&
                    environment_type == containing_type) ||
                   (method -> ACC_PROTECTED() &&
                    environment_type -> IsSubclass(containing_type)));

            AstMethodInvocation *accessor       = compilation_unit -> ast_pool -> GenMethodInvocation();
            accessor -> method                  = method_call -> method; // TODO: WARNING: sharing of subtrees...
            accessor -> left_parenthesis_token  = method_call -> left_parenthesis_token;
            accessor -> right_parenthesis_token = method_call -> right_parenthesis_token;
            accessor -> symbol                  = environment_type -> GetReadAccessMethod(method); // default base type is appropriate

            if (! method -> ACC_STATIC())
                accessor -> AddArgument(access_expression);
            for (int i = 0; i < method_call -> NumArguments(); i++)
                accessor -> AddArgument(method_call -> Argument(i));

            method_call -> symbol = method;
            method_call -> resolution_opt = accessor;
        }
    }
}


void Semantic::CheckSimpleName(AstSimpleName *simple_name,
                               SemanticEnvironment *where_found)
{
    VariableSymbol *variable_symbol = simple_name -> symbol -> VariableCast();

    assert(variable_symbol);

    if (StaticRegion())
    {
        if (! (variable_symbol -> IsLocal() || variable_symbol -> ACC_STATIC()))
        {
            ReportSemError(SemanticError::NAME_NOT_CLASS_VARIABLE,
                           simple_name -> identifier_token,
                           simple_name -> identifier_token,
                           lex_stream -> NameString(simple_name -> identifier_token));
        }
        else if (! variable_symbol -> IsDeclarationComplete() &&
                 ! processing_simple_assignment)
        {
            ReportSemError(SemanticError::NAME_NOT_YET_AVAILABLE,
                           simple_name -> identifier_token,
                           simple_name -> identifier_token,
                           lex_stream -> NameString(simple_name -> identifier_token));
        }
    }
    else if (! variable_symbol -> ACC_STATIC()) // an instance variable?
    {
        TypeSymbol *containing_type = variable_symbol -> owner -> TypeCast(); // an instance field member ?

        if (containing_type) // variable must be a field
        {
            if (containing_type == ThisType() &&
                ! variable_symbol -> IsDeclarationComplete() &&
                ! processing_simple_assignment) // forward reference?
            {
                ReportSemError(SemanticError::NAME_NOT_YET_AVAILABLE,
                               simple_name -> identifier_token,
                               simple_name -> identifier_token,
                               lex_stream -> NameString(simple_name -> identifier_token));
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
                               lex_stream -> NameString(simple_name -> identifier_token),
                               containing_type -> Name());
            }
        }
    }
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

        assert(variable_symbol -> IsTyped());

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
                // If the variable declarator in question exists and its
                // computation is not pending (to avoid looping) and it has a
                // simple expression initializer.
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

        CheckSimpleName(simple_name, where_found);

        //
        // If the variable belongs to an outer type, add the proper
        // pointer dereferences (and method access in the case of a
        // private variable) necessary to get to it.
        //
        if (where_found != state_stack.Top() &&
            (! simple_name -> IsConstant()))
        {
            CreateAccessToScopedVariable(simple_name, where_found -> Type());
        }
    }
    else
    {
        ReportVariableNotFound(simple_name, this_type);
        simple_name -> symbol = control.no_type;
    }
}


bool Semantic::TypeAccessCheck(Ast *ast, TypeSymbol *type)
{
    //
    // Unless we are processing the body of a type do not do type checking.
    // (This method may be invoked, for example, when FindFirstType invokes
    // ProcessPackageOrType)
    //
    if (state_stack.Size() > 0)
    {
        TypeSymbol *this_type = ThisType();

        // According to JLS 6.6.1, a type T[] is accessible if T is accessible.
        if (type -> IsArray())
            type = type -> base_type;

        //
        // Type checking is necessary only for two types that are not enclosed
        // within the same outermost type. Note that if we are trying to access
        // an inner type T1.T2...Tn from this type, TypeAccessCheck is expected
        // to be invoked first for T1, then T1.T2, ... and finally for
        // T1.T2...Tn, in turn. When invoked for T1.T2, for example, this
        // function only checks whether or not T1.T2 is accessible from "this"
        // type. It does not recheck whether or not T1 is accessible.
        //
        if (this_type -> outermost_type != type -> outermost_type)
        {
            if (type -> ACC_PRIVATE())
            {
                 ReportTypeInaccessible(ast, type);
                 return false;
            }
            else if (type -> ACC_PROTECTED())
            {
                if (type -> ContainingPackage() != this_package &&
                    ! this_type -> HasProtectedAccessTo(type))
                {
                    ReportTypeInaccessible(ast, type);
                    return false;
                }
            }
            else if (! type -> ACC_PUBLIC() &&
                     type -> ContainingPackage() != this_package)
            {
                 ReportTypeInaccessible(ast, type);
                 return false;
            }
        }
    }

    return true;
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
}


void Semantic::ConstructorAccessCheck(AstClassInstanceCreationExpression *class_creation,
                                      MethodSymbol *constructor)
{
    TypeSymbol *this_type = ThisType();
    TypeSymbol *containing_type = constructor -> containing_type;

    if (this_type -> outermost_type != containing_type -> outermost_type)
    {
        if (constructor -> ACC_PRIVATE())
        {
            ReportSemError(SemanticError::CONSTRUCTOR_NOT_ACCESSIBLE,
                           class_creation -> class_type -> LeftToken(),
                           class_creation -> right_parenthesis_token,
                           constructor -> Header(),
                           containing_type -> ContainingPackage() -> PackageName(),
                           containing_type -> ExternalName(),
                           StringConstant::US_private);
        }
        else if (constructor -> ACC_PROTECTED() &&
                 containing_type -> ContainingPackage() != this_package)
        {
            if (! class_creation -> symbol -> TypeCast() -> Anonymous())
                ReportSemError(SemanticError::CONSTRUCTOR_NOT_ACCESSIBLE,
                               class_creation -> class_type -> LeftToken(),
                               class_creation -> right_parenthesis_token,
                               constructor -> Header(),
                               containing_type -> ContainingPackage() -> PackageName(),
                               containing_type -> ExternalName(),
                               StringConstant::US_protected);
        }
        else if (! constructor -> ACC_PUBLIC() &&
                 containing_type -> ContainingPackage() != this_package)
        {
            ReportSemError(SemanticError::CONSTRUCTOR_NOT_ACCESSIBLE,
                           class_creation -> class_type -> LeftToken(),
                           class_creation -> right_parenthesis_token,
                           constructor -> Header(),
                           containing_type -> ContainingPackage() -> PackageName(),
                           containing_type -> ExternalName(),
                           StringConstant::US_default);
        }
    }
}


//
// Returns whether or not the symbol is accessible from within the innermost
// enclosing class base_type. field_access is the qualifying expression for
// the access, and is NULL in the case of a SimpleName access.
//
bool Semantic::MemberAccessCheck(AstFieldAccess *field_access,
                                 TypeSymbol *base_type, Symbol *symbol)
{
    TypeSymbol *this_type = ThisType();

    VariableSymbol *variable_symbol = symbol -> VariableCast();
    MethodSymbol *method_symbol = symbol -> MethodCast();

    assert(variable_symbol || method_symbol);

    AccessFlags *flags = (variable_symbol ? (AccessFlags *) variable_symbol : (AccessFlags *) method_symbol);
    TypeSymbol *containing_type = (variable_symbol ? variable_symbol -> owner -> TypeCast() : method_symbol -> containing_type);

    assert(containing_type);

    //
    // When this function, MemberAccessCheck is invoked, it is assumed that
    // the function TypeAccessCheck has already been invoked as follows:
    //
    //    TypeAccessCheck(base, base_type);
    //
    // and that the check below has already been performed.
    //
    //    if (! (base_type -> ACC_PUBLIC() ||
    //           base_type -> ContainingPackage() == this_package))
    //        ReportTypeInaccessible(base, base_type);
    //

    if (this_type -> outermost_type != containing_type -> outermost_type)
    {
        if (flags -> ACC_PRIVATE())
            return false;
        else if (flags -> ACC_PROTECTED())
        {
            //
            // Within the same package, protected is accessible. Super access
            // has special priveleges (contrary to JLS2 15.11.2,
            // super.name != ((S)this).name; ). JLS2 6.6.2: When packages
            // differ, subclasses may access protected static members without
            // further restrictions, but accessing instance members requires
            // that the qualifier be the subclass or lower.
            // JLS 9.2: Interfaces have no protected members.
            //
            if (field_access &&
                field_access -> base -> Type() -> ACC_INTERFACE())
            {
                assert(method_symbol); // Object has no fields.
                return false;
            }
            if (containing_type -> ContainingPackage() == this_package ||
                (field_access && field_access -> base -> IsSuperExpression()))
            {
                return true;
            }
            if (this_type -> HasProtectedAccessTo(containing_type))
            {
                if (flags -> ACC_STATIC())
                    return true;
                for (SemanticEnvironment *env = this_type -> semantic_environment;
                     env; env = env -> previous)
                {
                    if (base_type -> IsSubclass(env -> Type()))
                        return true;
                }
            }
            return false;
        }
        else if (! flags -> ACC_PUBLIC() &&
                 containing_type -> ContainingPackage() != this_package)
        {
            return false;
        }
    }

    return true;
}


void Semantic::SimpleNameAccessCheck(AstSimpleName *simple_name,
                                     TypeSymbol *containing_type,
                                     Symbol *symbol)
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
            ReportSemError((variable_symbol ? SemanticError::FIELD_NOT_ACCESSIBLE
                                            : SemanticError::METHOD_NOT_ACCESSIBLE),
                           simple_name -> identifier_token,
                           simple_name -> identifier_token,
                           lex_stream -> NameString(simple_name -> identifier_token),
                           containing_type -> ContainingPackage() -> PackageName(),
                           containing_type -> ExternalName(),
                           StringConstant::US_private);
        }
        else if (flags -> ACC_PROTECTED())
        {
            if (! (containing_type -> ContainingPackage() == this_package || this_type -> IsSubclass(containing_type)))
            {
                ReportSemError((variable_symbol ? SemanticError::FIELD_NOT_ACCESSIBLE
                                                : SemanticError::METHOD_NOT_ACCESSIBLE),
                               simple_name -> identifier_token,
                               simple_name -> identifier_token,
                               lex_stream -> NameString(simple_name -> identifier_token),
                               containing_type -> ContainingPackage() -> PackageName(),
                               containing_type -> ExternalName(),
                               StringConstant::US_protected);
            }
        }
        else if (! (flags -> ACC_PUBLIC() || containing_type -> ContainingPackage() == this_package))
        {
            ReportSemError((variable_symbol ? SemanticError::FIELD_NOT_ACCESSIBLE
                                            : SemanticError::METHOD_NOT_ACCESSIBLE),
                           simple_name -> identifier_token,
                           simple_name -> identifier_token,
                           lex_stream -> NameString(simple_name -> identifier_token),
                           containing_type -> ContainingPackage() -> PackageName(),
                           containing_type -> ExternalName(),
                           StringConstant::US_default);
        }
    }
}


//
// Returns true if the current type can access a protected member declared in
// the containing type, without an accessor method. This does not test
// whether the target type and member are accessible, since those checks are
// assumed to be already done.
//
bool Semantic::ProtectedAccessCheck(TypeSymbol *containing_type)
{
    TypeSymbol *this_type = ThisType();
    return (this_type -> IsSubclass(containing_type) ||
            this_type -> ContainingPackage() == containing_type -> ContainingPackage());
}


//
// FindVariableMember resolves a qualified field reference. The parameter
// type is the type of the qualifying expression, field_access is the
// expression being resolved.
//
void Semantic::FindVariableMember(TypeSymbol *type,
                                  AstFieldAccess *field_access)
{
    //
    // TypeCast() returns true for super, this, and instance creation as
    // well as true type names, hence the extra check
    //
    assert(field_access -> base);
    bool base_is_type = field_access -> base -> symbol -> TypeCast() != NULL &&
                        field_access -> base -> IsName();

    if (type -> Bad())
    {
        //
        // If no error has been detected so far, report this as an error so
        // that we don't try to generate code later. On the other hand, if an
        // error had been detected prior to this, don't flood the user with
        // spurious messages.
        //
        if (NumErrors() == 0)
            ReportVariableNotFound(field_access, type);
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
        TypeSymbol *this_type = ThisType();
        if (! TypeAccessCheck(field_access -> base, type))
        {
            field_access -> symbol = control.no_type;
            return;
        }

        VariableSymbol *variable = FindVariableInType(type, field_access);
        if (variable)
        {
            assert(variable -> IsTyped());

            if (base_is_type)
            {
                if (! variable -> ACC_STATIC())
                {
                    ReportSemError(SemanticError::NAME_NOT_CLASS_VARIABLE,
                                   field_access -> identifier_token,
                                   field_access -> identifier_token,
                                   lex_stream -> NameString(field_access -> identifier_token));
                    field_access -> symbol = control.no_type;
                    return;
                }
                //
                // If a variable is FINAL, initialized with a constant
                // expression, and of the form TypeName.Identifier, we
                // substitute the expression here. - JLS 15.28
                //
                if (variable -> ACC_FINAL())
                {
                    //
                    // If the field declaration of the type has been
                    // completely processed, simply retrieve the value.
                    // Otherwise, compute the value of the initialization
                    // expression in question on the fly if the variable in
                    // question is not in the same type. Recall that static
                    // variables must be processed in the textual order in
                    // which they appear in the body of a type. Therefore, if
                    // the static initialization of a field refers to another
                    // variable in the same type it must have appeared before
                    // the current field declaration otherwise we will emit an
                    // error message later...
                    //
                    if (variable -> IsDeclarationComplete())
                        field_access -> value = variable -> initial_value;
                    else if (variable -> declarator)
                    {
                        AstVariableDeclarator *declarator = variable -> declarator -> VariableDeclaratorCast();
                        //
                        // If the variable declarator in question exists and
                        // its computation is not pending (to avoid looping)
                        // and it has a simple expression initializer.
                        //
                        if (declarator && (! declarator -> pending) &&
                            declarator -> variable_initializer_opt &&
                            (! declarator -> variable_initializer_opt -> ArrayInitializerCast()))
                        {
                            TypeSymbol *variable_type = (TypeSymbol *) variable -> owner;
                            Semantic *sem = variable_type -> semantic_environment -> sem;
                            field_access -> value = sem -> ComputeFinalValue(declarator);
                        }
                    }
                }
            }

            //
            // Access to a private or protected variable in an enclosing type ?
            //
            TypeSymbol *containing_type = (TypeSymbol *) variable -> owner;
            if (this_type != containing_type &&
                (variable -> ACC_PRIVATE() ||
                 (variable -> ACC_PROTECTED() &&
                  ! ProtectedAccessCheck(containing_type))))
            {
                if (field_access -> IsConstant())
                    field_access -> symbol = variable;
                else
                {
                    //
                    // Find the right enclosing class to place the accessor
                    // method in. For private fields, the containing type; for
                    // protected fields, an enclosing class which is related
                    // to the containing type.
                    //
                    TypeSymbol *environment_type = containing_type;
                    if (variable -> ACC_PROTECTED())
                    {
                        for (SemanticEnvironment *env = this_type -> semantic_environment;
                             env; env = env -> previous)
                        {
                            if (env -> Type() -> IsSubclass(containing_type))
                            {
                                environment_type = env -> Type();
                                break;
                            }
                        }
                        assert(environment_type != containing_type);
                    }

                    AstFieldAccess *method_name     = compilation_unit -> ast_pool -> GenFieldAccess();
                    method_name -> base             = field_access -> base; // TODO: WARNING: sharing of Ast subtree !!!
                    method_name -> dot_token        = field_access -> identifier_token;
                    method_name -> identifier_token = field_access -> identifier_token;
                    method_name -> symbol           = variable;

                    AstMethodInvocation *accessor       = compilation_unit -> ast_pool -> GenMethodInvocation();
                    accessor -> method                  = method_name;
                    accessor -> left_parenthesis_token  = field_access -> identifier_token;
                    accessor -> right_parenthesis_token = field_access -> identifier_token;
                    accessor -> symbol                  = environment_type -> GetReadAccessMethod(variable, field_access -> base -> Type());

                    if (! variable -> ACC_STATIC())
                        accessor -> AddArgument(field_access -> base);

                    field_access -> resolution_opt = accessor;
                    field_access -> symbol = accessor -> symbol;
                }
            }
            else
            {
                field_access -> symbol = variable;
            }
        }
        else
        {
            TypeSymbol *inner_type = FindNestedType(type, field_access -> identifier_token);
            if (inner_type)
            {
                if (base_is_type)
                {
                    field_access -> symbol = inner_type;
                    TypeAccessCheck(field_access, inner_type);
                }
                else
                {
                    ReportSemError(SemanticError::TYPE_NOT_FIELD,
                                   field_access -> identifier_token,
                                   field_access -> identifier_token,
                                   lex_stream -> NameString(field_access -> identifier_token));
                    field_access -> symbol = control.no_type;
                }
            }
            else
            {
                ReportVariableNotFound(field_access, type);
                field_access -> symbol = control.no_type;
            }
        }
    }
}

//
// NOTE that method names are not processed here but by the function
// ProcessMethodName. Also, for ease in parsing, we treat name.class,
// name.super, and name.this like field accesses, so those cases are covered
// here under qualified names.
//
void Semantic::ProcessAmbiguousName(Ast *name)
{
    TypeSymbol *this_type = ThisType();

    AstSimpleName *simple_name = name -> SimpleNameCast();

    //
    // JLS2 6.5.2: If the ambiguous name is a simple name,...
    //
    if (simple_name)
    {
        TypeSymbol *type;
        //
        // ... If the Identifier appears within the scope (6.3) if a local
        // variable declaration (14.3) or parameter declaration (8.4.1,
        // 8.6.1, 14.18) with that name, then the ambiguous name is
        // reclassified as an ExpressionName...
        //
        // ...Otherwise, consider the class or interface C within whose
        // declaration the Identifier occurs. If C has one or more fields
        // with that name, which may be either declared within it or inherited,
        // then the Ambiguous name is reclassified as an ExpressionName....
        //
        SemanticEnvironment *where_found;
        VariableSymbol *variable_symbol = FindVariableInEnvironment(where_found, state_stack.Top(), simple_name -> identifier_token);
        if (variable_symbol)
        {
            assert(variable_symbol -> IsTyped());

            //
            // A variable_symbol that is FINAL may have an initial value.
            // If variable_symbol is not final then its initial value is NULL.
            //
            simple_name -> value = variable_symbol -> initial_value;
            simple_name -> symbol = variable_symbol;

            CheckSimpleName(simple_name, where_found);

            //
            // If the variable belongs to an outer type, add the proper
            // pointer dereferences (and method access in the case of a
            // private variable) necessary to  get to it.
            //
            if (where_found != state_stack.Top() && ! simple_name -> value)
                CreateAccessToScopedVariable(simple_name, where_found -> Type());
        }
        //
        // ...Otherwise, if a type of that name is declared in the compilation
        // unit (7.3) containing the Identifier, either by a
        // single-type-import declaration (7.5.1) or by a class or interface
        // type declaration (7.6), then the Ambiguous name is reclassified as
        // a TypeName...
        //
        // ...Otherwise, if a type of that name is declared in another
        // compilation unit (7.3) of the package (7.1) of the compilation unit
        // containing the Identifier, then the Ambiguous Name is reclassified
        // as a TypeName...
        //
        // ...Otherwise, if a type of that name is declared by exactly one
        // type-import-on-demand declaration (7.5.2) of the compilation unit
        // containing the Identifier, then the AmbiguousName is reclassified
        // as a TypeName
        //
        // ...Otherwise, if a type of that name is declared by more than one
        // type-import-on-demand declaration of the compilation unit
        // containing the Identifier, then a compile-time error results.
        //
        else if ((type = FindType(simple_name -> identifier_token)))
             simple_name -> symbol = type;
        //
        // ...Otherwise, the Ambiguous name is reclassified as a PackageName.
        // While the JLS claims a later step determines whether or not 
        // a package of that name actually exists, it is pointless to defer
        // the error that long, as a package cannot qualify a method or field
        // access, and a subpackage requires the base package to exist.
        //
        else
        {
            NameSymbol *name_symbol = lex_stream -> NameSymbol(simple_name -> identifier_token);
            PackageSymbol *package = control.external_table.FindPackageSymbol(name_symbol);
            if (! package)
            {
                //
                // One last check in case the package was not imported.
                //
                package = control.external_table.InsertPackageSymbol(name_symbol, NULL);
                control.FindPathsToDirectory(package);
            }
            if (package -> directory.Length())
                simple_name -> symbol = package;
            else
            {
                ReportVariableNotFound(simple_name, this_type);
                simple_name -> symbol = control.no_type;
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

        //
        // ...First, classify the name or expression to the left of the '.'...
        // If there are more names to the left, we short-circuit
        // ProcessFieldAccess, since we already know what context the name
        // is in.
        //
        AstExpression *base = field_access -> base;
        if (base -> FieldAccessCast() || base -> SimpleNameCast())
        {
            assert(field_access -> IsNameAccess());
            ProcessAmbiguousName(base);
        }
        else ProcessExpression(base);

        TypeSymbol *type = base -> Type();
        assert(type || base -> symbol -> PackageCast());

        if (type == control.no_type)
        {
            field_access -> symbol = control.no_type;
            return;
        }

        //
        // Class literal access
        //
        if (field_access -> IsClassAccess())
        {
            AddDependence(this_type, control.NoClassDefFoundError(), field_access -> identifier_token);
            AddDependence(this_type, control.ClassNotFoundException(), field_access -> identifier_token);

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

                VariableSymbol *variable_symbol =
                    type -> FindVariableSymbol(control.type_name_symbol);

                if (variable_symbol)
                {
                    if (! variable_symbol -> IsTyped())
                        variable_symbol -> ProcessVariableSignature((Semantic *) this,
                                                                    field_access -> identifier_token);
                    field_access -> symbol = variable_symbol;
                }
                else
                {
                    ReportSemError(SemanticError::NON_STANDARD_LIBRARY_TYPE,
                                   0,
                                   0,
                                   type -> ContainingPackage() -> PackageName(),
                                   type -> ExternalName());
                    field_access -> symbol = control.no_type;
                }
            }
            else
            {
                //
                // We have already checked that the type is accessible
                //
                TypeSymbol *outermost_type = this_type -> outermost_type;

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
                    AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(field_access -> identifier_token);
                    simple_name -> symbol = variable_symbol;

                    field_access -> symbol = variable_symbol;
                    field_access -> resolution_opt = simple_name;
                }
            }
        }
        //
        // Qualified this or super access (simple this and super are covered
        // elsewhere).
        //
        else if (field_access -> IsThisAccess() ||
                 field_access -> IsSuperAccess())
        {
            TypeSymbol *enclosing_type = base -> symbol -> TypeCast();
            if (! enclosing_type)
            {
                ReportSemError(SemanticError::NOT_A_TYPE,
                               base -> LeftToken(),
                               base -> RightToken());
                field_access -> symbol = control.no_type;
            }
            else if (enclosing_type -> ACC_INTERFACE())
            {
                ReportSemError(SemanticError::NOT_A_CLASS,
                               base -> LeftToken(),
                               base -> RightToken(),
                               enclosing_type -> ContainingPackage() -> PackageName(),
                               enclosing_type -> ExternalName());
                field_access -> symbol = control.no_type;
            }
            else if (this_type == control.Object() &&
                     field_access -> IsSuperAccess())
            {
                ReportSemError(SemanticError::OBJECT_HAS_NO_SUPER_TYPE,
                               field_access -> LeftToken(),
                               field_access -> RightToken());
                field_access -> symbol = control.no_type;
            }
            else if (! (this_type -> IsNestedIn(enclosing_type) &&
                        this_type -> CanAccess(enclosing_type)))
            {
                if (this_type == enclosing_type)
                {
                    ReportSemError((field_access -> IsThisAccess() ? SemanticError::MISPLACED_THIS_EXPRESSION
                                                                   : SemanticError::MISPLACED_SUPER_EXPRESSION),
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
                field_access -> resolution_opt = CreateAccessToType(field_access, enclosing_type);
                field_access -> symbol = field_access -> resolution_opt -> symbol;
            }
        }
        //
        // Regular field access
        //
        else
        {
            PackageSymbol *package = base -> symbol -> PackageCast();

            if (package)
            {
                //
                // ... If there is a package whose name is the name to the
                // left of the '.' and that package contains a declaration of
                // a type whose name is the same as the Identifier, then the
                // AmbiguousName is reclassified as a TypeName...
                //
                NameSymbol *name_symbol = lex_stream -> NameSymbol(field_access -> identifier_token);
                type = package -> FindTypeSymbol(name_symbol);

                if (type)
                {
                    if (type -> SourcePending())
                        control.ProcessHeaders(type -> file_symbol);
                    field_access -> symbol = type;
                }
                else
                {
                    FileSymbol *file_symbol = Control::GetFile(control, package, name_symbol);
                    if (file_symbol)
                    {
                        type = ReadType(file_symbol, package, name_symbol, field_access -> identifier_token);
                        field_access -> symbol = type;
                    }
                    //
                    // ... Otherwise, this AmbiguousName is reclassified as a
                    // PackageName. While the JLS claims a later step
                    // determines whether or not a package of that name
                    // actually exists, it is pointless to defer the error
                    // that long, as a package cannot qualify a method or field
                    // access, and a subpackage requires the base package to
                    // exist.
                    //
                    else
                    {
                        PackageSymbol *subpackage = package -> FindPackageSymbol(name_symbol);
                        if (! subpackage)
                        {
                            //
                            // One last check in case the subpackage was not
                            // imported.
                            //
                            subpackage = package -> InsertPackageSymbol(name_symbol);
                            control.FindPathsToDirectory(subpackage);
                        }
                        if (subpackage -> directory.Length())
                            field_access -> symbol = subpackage;
                        else
                        {
                            ReportSemError(SemanticError::UNKNOWN_AMBIGUOUS_NAME,
                                           field_access -> LeftToken(),
                                           field_access -> RightToken(),
                                           name_symbol -> Name());
                            field_access -> symbol = control.no_type;
                        }
                    }
                }
            }
            // ...Whether the qualifier is a type name, variable, or method
            // call, this is a regular field access
            //
            else
            {
                FindVariableMember(type, field_access);
                TypeSymbol *parent_type = (type -> IsArray() ? type -> base_type : type);
                if (! parent_type -> Primitive())
                    AddDependence(this_type, parent_type,
                                  field_access -> identifier_token,
                                  field_access -> IsConstant());
            }
        }
    }
}


void Semantic::ProcessFieldAccess(Ast *expr)
{
    AstFieldAccess *field_access = (AstFieldAccess *) expr;

    ProcessAmbiguousName(field_access);

    TypeSymbol *type;

    if (field_access -> symbol != control.no_type)
    {
        PackageSymbol *package = field_access -> symbol -> PackageCast();
        if (package)
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
        else // Assertion: either it's not a variable (an error) or the signature of the variable has been typed
        {
            assert((! field_access -> symbol -> VariableCast()) || field_access -> symbol -> VariableCast() -> IsTyped());
        }
    }
}


void Semantic::ProcessCharacterLiteral(Ast *expr)
{
    AstCharacterLiteral *char_literal = (AstCharacterLiteral *) expr;

    LiteralSymbol *literal = lex_stream -> LiteralSymbol(char_literal -> character_literal_token);

    if (! literal -> value)
        control.int_pool.FindOrInsertChar(literal);
    assert(literal -> value != control.BadValue());
    char_literal -> value = literal -> value;
    char_literal -> symbol = control.char_type;
}


void Semantic::ProcessIntegerLiteral(Ast *expr)
{
    AstIntegerLiteral *int_literal = (AstIntegerLiteral *) expr;

    LiteralSymbol *literal = lex_stream -> LiteralSymbol(int_literal -> integer_literal_token);

    if (! literal -> value)
        control.int_pool.FindOrInsertInt(literal);
    if (literal -> value == control.BadValue())
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

    LiteralSymbol *literal = lex_stream -> LiteralSymbol(long_literal -> long_literal_token);

    if (! literal -> value)
        control.long_pool.FindOrInsertLong(literal);
    if (literal -> value == control.BadValue())
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


void Semantic::ProcessFloatLiteral(Ast *expr)
{
    AstFloatLiteral *float_literal = (AstFloatLiteral *) expr;

    LiteralSymbol *literal =
        lex_stream -> LiteralSymbol(float_literal -> float_literal_token);

    if (! literal -> value)
        control.float_pool.FindOrInsertFloat(literal);
    if (literal -> value == control.BadValue())
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

    LiteralSymbol *literal =
        lex_stream -> LiteralSymbol(double_literal -> double_literal_token);

    if (! literal -> value)
        control.double_pool.FindOrInsertDouble(literal);
    if (literal -> value == control.BadValue())
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

    LiteralSymbol *literal =
        lex_stream -> LiteralSymbol(string_literal -> string_literal_token);

    if (! literal -> value)
        control.Utf8_pool.FindOrInsertString(literal);
    assert(literal -> value != control.BadValue());
    string_literal -> value = literal -> value;
    string_literal -> symbol = control.String();
}


void Semantic::ProcessArrayAccess(Ast *expr)
{
    AstArrayAccess *array_access = (AstArrayAccess *) expr;

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
}


void Semantic::FindMethodMember(TypeSymbol *type,
                                AstMethodInvocation *method_call)
{
    AstFieldAccess *field_access = method_call -> method -> FieldAccessCast();

    //
    // TypeCast() returns true for super, this, and instance creation as
    // well as true type names, hence the extra check
    //
    assert(field_access && field_access -> base);
    bool base_is_type = field_access -> base -> symbol -> TypeCast() != NULL &&
                        field_access -> base -> IsName();

    if (type -> Bad())
    {
        //
        // If no error has been detected so far, report this as an error so
        // that we don't try to generate code later. On the other hand, if an
        // error had been detected prior to this, don't flood the user with
        // spurious messages.
        //
        if (NumErrors() == 0)
            ReportMethodNotFound(method_call, type);
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
        if (! TypeAccessCheck(field_access -> base, type))
        {
            method_call -> symbol = control.no_type;
            return;
        }

        MethodSymbol *method = FindMethodInType(type, method_call);
        if (method)
        {
            assert(method -> IsTyped());

            if (base_is_type && (! method -> ACC_STATIC()))
            {
                ReportSemError(SemanticError::METHOD_NOT_CLASS_METHOD,
                               field_access -> LeftToken(),
                               field_access -> identifier_token,
                               lex_stream -> NameString(field_access -> identifier_token));
                method_call -> symbol = control.no_type;
                return;
            }

            //
            // Apply method invocation conversion to the parameters
            //
            assert(method_call -> NumArguments() == method -> NumFormalParameters());

            for (int i = 0; i < method_call -> NumArguments(); i++)
            {
                AstExpression *expr = method_call -> Argument(i);
                if (expr -> Type() != method -> FormalParameter(i) -> Type())
                    method_call -> Argument(i) = ConvertToType(expr, method -> FormalParameter(i) -> Type());
            }

            //
            // Access to a private or protected method in an enclosing type ?
            //
            TypeSymbol *containing_type = (TypeSymbol *) method -> containing_type;
            if (this_type != containing_type &&
                (method -> ACC_PRIVATE() ||
                 (method -> ACC_PROTECTED() &&
                  ! ProtectedAccessCheck(containing_type)) ||
                 (field_access -> base -> IsSuperExpression() &&
                  ! method -> ACC_STATIC() &&
                  ! this_type -> IsSubclass(containing_type))))
            {
                //
                // Find the right enclosing class to place the accessor method
                // in. For private methods, the containing type; for protected
                // methods or superclass methods, an enclosing class which is
                // related to the containing type.
                //
                TypeSymbol *environment_type = containing_type;
                if (! method -> ACC_PRIVATE())
                {
                    for (SemanticEnvironment *env = this_type -> semantic_environment;
                         env; env = env -> previous)
                    {
                        if (env -> Type() -> IsSubclass(containing_type))
                        {
                            environment_type = env -> Type();
                            break;
                        }
                    }
                    assert(environment_type != containing_type);
                }

                AstMethodInvocation *accessor       = compilation_unit -> ast_pool -> GenMethodInvocation();
                accessor -> method                  = method_call -> method; // TODO: WARNING: sharing of subtrees...
                accessor -> left_parenthesis_token  = method_call -> left_parenthesis_token;
                accessor -> right_parenthesis_token = method_call -> right_parenthesis_token;
                accessor -> symbol = environment_type -> GetReadAccessMethod(method, field_access -> base -> Type());

                if (! method -> ACC_STATIC())
                    accessor -> AddArgument(field_access -> base);
                for (int i = 0; i < method_call -> NumArguments(); i++)
                    accessor -> AddArgument(method_call -> Argument(i));

                method_call -> symbol = method;
                method_call -> resolution_opt = accessor;
            }
            else
            {
                method_call -> symbol = method;
            }
        }
        else
        {
            method_call -> symbol = control.no_type;
        }
    }
}


void Semantic::ProcessMethodName(AstMethodInvocation *method_call)
{
    TypeSymbol *this_type = ThisType();

    AstSimpleName *simple_name = method_call -> method -> SimpleNameCast();
    if (simple_name)
    {
        SemanticEnvironment *where_found;
        MethodSymbol *method = FindMethodInEnvironment(where_found, state_stack.Top(), method_call);

        if (! method)
            method_call -> symbol = control.no_type;
        else
        {
            assert(method -> IsTyped());

            if (! method -> ACC_STATIC())
            {
                //
                // We are in a static region if we are:
                //     . in the body of a static method
                //     . in the body of a static initializer
                //     . processing an initializer expression for a static
                //       variable.
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
                                   lex_stream -> NameString(simple_name -> identifier_token));
                    method_call -> symbol = control.no_type;
                    return;
                }
                else if (ExplicitConstructorInvocation())
                {
                    if (this_type -> IsSubclass(method -> containing_type))
                    {
                        //
                        // If the method in question is an instance method
                        // that is declared in this_type (this_type is
                        // definitely a class) or one of its super classes,
                        // then we have an error ->
                        //
                        ReportSemError(SemanticError::INSTANCE_METHOD_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
                                       method_call -> LeftToken(),
                                       method_call -> RightToken(),
                                       method -> Header(),
                                       method -> containing_type -> Name());
                        method_call -> symbol = control.no_type;
                        return;
                    }
                }
            }

            //
            // Apply method invocation conversion to the parameters
            //
            assert(method_call -> NumArguments() == method -> NumFormalParameters());

            for (int i = 0; i < method_call -> NumArguments(); i++)
            {
                AstExpression *expr = method_call -> Argument(i);
                if (expr -> Type() != method -> FormalParameter(i) -> Type())
                    method_call -> Argument(i) = ConvertToType(expr, method -> FormalParameter(i) -> Type());
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
        //
        // ...First, classify the name or expression to the left of the '.'...
        // If there are more names to the left, we short-circuit
        // ProcessFieldAccess, since we already know what context the name
        // is in.
        //
        AstFieldAccess *field_access = method_call -> method -> FieldAccessCast();
        AstExpression* base = field_access -> base;
        if (base -> FieldAccessCast() || base -> SimpleNameCast())
            ProcessAmbiguousName(base);
        else // The qualifier might be a complex String constant
            ProcessExpressionOrStringConstant(base);

        if (base -> symbol -> PackageCast())
        {
            ReportSemError(SemanticError::UNKNOWN_AMBIGUOUS_NAME,
                           base -> LeftToken(),
                           base -> RightToken(),
                           base -> symbol -> PackageCast() -> PackageName());
            method_call -> symbol = control.no_type;
            return;
        }

        TypeSymbol *type = base -> Type();
        assert(type);

        if (type == control.no_type)
        {
            method_call -> symbol = control.no_type;
            return;
        }
        if (base -> IsSuperExpression())
        {
            FindMethodMember(type, method_call);

            //
            // JLS2 15.12.3 requires this test
            //
            MethodSymbol *method = method_call -> symbol -> MethodCast();
            if (method && method -> ACC_ABSTRACT())
            {
                ReportSemError(SemanticError::ABSTRACT_METHOD_INVOCATION,
                               field_access -> LeftToken(),
                               field_access -> identifier_token,
                               lex_stream -> NameString(field_access -> identifier_token));
            }
        }
        else
        {
            FindMethodMember(type, method_call);

            TypeSymbol *parent_type = (type -> IsArray() ? type -> base_type : type);
            if (! parent_type -> Primitive())
                AddDependence(this_type, parent_type, field_access -> identifier_token);
        }
    }

    if (method_call -> symbol != control.no_type)
    {
        MethodSymbol *method = (MethodSymbol *) method_call -> symbol;

        SymbolSet *exception_set = TryExceptionTableStack().Top();
        if (exception_set)
            for (int i = method -> NumThrows() - 1; i >= 0; i--)
                exception_set -> AddElement(method -> Throws(i));

        for (int k = method -> NumThrows() - 1; k >= 0; k--)
        {
            TypeSymbol *exception = method -> Throws(k);
            if (UncaughtException(exception))
                ReportSemError(SemanticError::UNCAUGHT_METHOD_EXCEPTION,
                               method_call -> LeftToken(),
                               method_call -> RightToken(),
                               method -> Header(),
                               exception -> ContainingPackage() -> PackageName(),
                               exception -> ExternalName(),
                               UncaughtExceptionContext());
        }
    }
}


void Semantic::ProcessMethodInvocation(Ast *expr)
{
    AstMethodInvocation *method_call = (AstMethodInvocation *) expr;

    bool no_bad_argument = true;

    for (int i = 0; i < method_call -> NumArguments(); i++)
    {
        AstExpression *expr = method_call -> Argument(i);
        ProcessExpressionOrStringConstant(expr);
        if (expr -> symbol == control.no_type)
            no_bad_argument = false;
        else if (expr -> Type() == control.void_type)
        {
            no_bad_argument = false;
            ReportSemError(SemanticError::TYPE_IS_VOID,
                           expr -> LeftToken(),
                           expr -> RightToken(),
                           expr -> Type() -> Name());
        }
    }

    if (no_bad_argument)
         ProcessMethodName(method_call);
    else method_call -> symbol = control.no_type;

    assert(method_call -> symbol == control.no_type ||
           ((MethodSymbol *) method_call -> symbol) -> IsTyped());
}


void Semantic::ProcessNullLiteral(Ast *expr)
{
    //
    // Null is not a compile-time constant, so don't give it a value
    //
    AstNullLiteral *null_literal = (AstNullLiteral *) expr;
    null_literal -> symbol = control.null_type;
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
                       lex_stream -> NameString(this_expression -> this_token));
        this_expression -> symbol = control.no_type;
    }
    else this_expression -> symbol = ThisType();
}


void Semantic::ProcessSuperExpression(Ast *expr)
{
    AstSuperExpression *super_expression = (AstSuperExpression *) expr;

    if (ThisType() == control.Object())
    {
        ReportSemError(SemanticError::OBJECT_HAS_NO_SUPER_TYPE,
                       super_expression -> LeftToken(),
                       super_expression -> RightToken());
        super_expression -> symbol = control.no_type;
    }
    else if (StaticRegion())
    {
         ReportSemError(SemanticError::MISPLACED_SUPER_EXPRESSION,
                        super_expression -> LeftToken(),
                        super_expression -> RightToken());
         super_expression -> symbol = control.no_type;
    }
    else if (ExplicitConstructorInvocation())
    {
        ReportSemError(SemanticError::SUPER_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
                       super_expression -> LeftToken(),
                       super_expression -> RightToken(),
                       lex_stream -> NameString(super_expression -> super_token));
         super_expression -> symbol = control.no_type;
    }
    else super_expression -> symbol = ThisType() -> super;
}


void Semantic::ProcessParenthesizedExpression(Ast *expr)
{
    AstParenthesizedExpression *parenthesized = (AstParenthesizedExpression *) expr;

    //
    // Do not use ProcessExpressionOrStringConstant here, to avoid generating
    // intermediate Strings - see CheckConstantString in lookup.cpp
    //
    ProcessExpression(parenthesized -> expression);
    parenthesized -> value = parenthesized -> expression -> value;
    parenthesized -> symbol = parenthesized -> expression -> symbol;
}


void Semantic::UpdateGeneratedLocalConstructor(MethodSymbol *constructor)
{
    if (! constructor) // because of an earlier error
        return;

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
        if (control.IsDoubleWordType(symbol -> Type()))
            block_symbol -> max_variable_index++;
        local_constructor -> AddFormalParameter(symbol);
    }

    //
    // If we are dealing with a constructor generated for an anonymous type and
    // the super type of the anonymous type is an inner type then the generated
    // constructor accepts an additional formal parameter which is the
    // containing type of the super type, and the name of the parameter is #0.
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
        // Note that we do not initialize the this$0 here as the real
        // constructor will do that. If the local_type is static, its
        // constructor_parameters list does not start with this$0.
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
                                                               -> GenAssignmentExpression(AstAssignmentExpression::SIMPLE_EQUAL,
                                                                                          constructor_block -> left_brace_token);
            assign -> left_hand_side = lhs;
            assign -> expression     = rhs;
            assign -> symbol         = lhs -> Type();

            AstExpressionStatement *stmt = compilation_unit -> ast_pool -> GenExpressionStatement();
            stmt -> expression           = assign;
            stmt -> semicolon_token_opt  = constructor_block -> left_brace_token;

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

    AstMethodInvocation *method_call       = compilation_unit -> ast_pool -> GenMethodInvocation();
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

    AstExpressionStatement *stmt  = compilation_unit -> ast_pool -> GenExpressionStatement();
    stmt -> expression            = method_call;
    stmt -> semicolon_token_opt   = constructor_block -> left_brace_token;

    stmt -> is_reachable          = true;
    stmt -> can_complete_normally = true;

    constructor_block -> original_constructor_invocation = stmt;
}


void Semantic::UpdateLocalConstructors(TypeSymbol *inner_type)
{
    if (! ThisType() -> IsLocal()) // the method containing inner_type is not itself embedded in another method
    {
        //
        // Compute the set of local_classes we need to process here - they are
        // the inner_type itself and all the classes that are embedded in its
        // body.
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

        for (int j = 0; j < outermost_type -> NumAnonymousTypes(); j++)
        {
            TypeSymbol *anon_type = outermost_type -> AnonymousType(j);
            //
            // Skip anonymous classes created as an extra parameter for
            // private constructor accessors, and add all others to the list.
            //
            if (! anon_type -> declaration -> IsGenerated() &&
                anon_type -> CanAccess(inner_type))
            {
                local_classes.Next() = anon_type;
            }
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

                        //
                        // If there does not exist a variable with the same
                        // identity as the local or there exists such a
                        // variable but it is not the local then make a copy of
                        // the local in the source type.
                        //
                        if (env -> symbol_table.FindVariableSymbol(local -> Identity()) != local)
                            (void) source_local_type -> FindOrInsertLocalShadow(local);
                    }
                }
            }
        }

        //
        // Now update the constructor bodies to reflect the new local variable
        // counts and mark the local_type completed.
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

                if ((class_creation = call -> ClassInstanceCreationExpressionCast()))
                {
                    if (class_creation -> symbol != control.no_type)
                    {
                        if (source_local_type -> CanAccess(target_local_type))
                        {
                            for (int j = (target_local_type -> ACC_STATIC() ? 0 : 1);
                                 j < target_local_type -> NumConstructorParameters(); j++)
                            {
                                AstSimpleName *simple_name = compilation_unit -> ast_pool
                                                                              -> GenSimpleName(class_creation -> new_token);
                                VariableSymbol *variable_symbol = target_local_type -> ConstructorParameter(j);
                                simple_name -> symbol = variable_symbol;
                                if (source_local_type != target_local_type)
                                {
                                    simple_name -> symbol = variable_symbol -> accessed_local;

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
                                //
                                // If there does not exist a variable with the
                                // same identity as the local or there exists
                                // such a variable but it is not the local
                                // then make a copy of the local in the source
                                // type.
                                //
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
                else if ((super_call = call -> SuperCallCast()))
                {
                    if (super_call -> symbol -> MethodCast())
                    {
                        for (int j = (target_local_type -> ACC_STATIC() ? 0 : 1);
                             j < target_local_type -> NumConstructorParameters(); j++)
                        {
                            VariableSymbol *local = target_local_type -> ConstructorParameter(j) -> accessed_local,
                                           *local_shadow = source_local_type -> FindOrInsertLocalShadow(local);

                            AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(super_call -> super_token);
                            simple_name -> symbol = env -> symbol_table.FindVariableSymbol(local_shadow -> Identity());

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
                            VariableSymbol *local = target_local_type -> ConstructorParameter(j);

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
}


void Semantic::GetAnonymousConstructor(AstClassInstanceCreationExpression *class_creation,
                                       TypeSymbol *anonymous_type)
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

    assert(super_constructor -> IsTyped());

    //
    // Make constructor symbol. The associated symbol table will not contain
    // too many elements...
    //
    BlockSymbol *block_symbol = new BlockSymbol(super_constructor -> NumFormalParameters() + 3);
    block_symbol -> max_variable_index = 1; // All types need a spot for "this".

    MethodSymbol *constructor = anonymous_type -> InsertConstructorSymbol(control.init_name_symbol);
    constructor -> SetType(control.void_type);
    constructor -> SetContainingType(anonymous_type);
    constructor -> SetBlockSymbol(block_symbol);

    //
    // Anonymous class constructors may throw any exception listed in the
    // superclass.
    //
    int num_throws = super_constructor -> NumThrows();
    for (int i = 0; i < num_throws; i++)
    {
        TypeSymbol *exception = super_constructor -> Throws(i);
        constructor -> AddThrows(exception);
    }

    VariableSymbol *this0_variable = NULL;
    if (anonymous_type -> IsInner())
    {
        this0_variable = block_symbol -> InsertVariableSymbol(control.this0_name_symbol);
        this0_variable -> MarkSynthetic();
        this0_variable -> SetType(anonymous_type -> ContainingType());
        this0_variable -> SetOwner(constructor);
        this0_variable -> SetACC_FINAL();
        this0_variable -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
    }

    for (int j = 0; j < super_constructor -> NumFormalParameters(); j++)
    {
        VariableSymbol *param = super_constructor -> FormalParameter(j),
                       *symbol = block_symbol -> InsertVariableSymbol(param -> Identity());
        symbol -> SetType(param -> Type());
        symbol -> SetOwner(constructor);
        symbol -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
        if (control.IsDoubleWordType(symbol -> Type()))
            block_symbol -> max_variable_index++;
        constructor -> AddFormalParameter(symbol);
    }

    //
    //
    //
    AstSuperCall *super_call              = compilation_unit -> ast_pool -> GenSuperCall();
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
    // If we are in a static region, the anonymous constructor does not need
    // a this$0 argument. Otherwise, a this$0 argument that points to an
    // instance of the immediately enclosing class is required.
    //
    if (anonymous_type -> ACC_STATIC())
        class_creation -> base_opt = NULL;
    else
    {
        //
        // Within an explicit constructor invocation, a class that is
        // immediately nested in the class being created is not accessible.
        //
        if (ExplicitConstructorInvocation() && anonymous_type -> ContainingType() == ThisType())
        {
            ReportSemError(SemanticError::INNER_CONSTRUCTOR_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
                           class_creation -> LeftToken(),
                           class_creation -> RightToken(),
                           anonymous_type -> ContainingPackage() -> PackageName(),
                           anonymous_type -> ExternalName(),
                           ThisType() -> ContainingPackage() -> PackageName(),
                           ThisType() -> ExternalName());
            class_creation -> base_opt = NULL;
        }
        else class_creation -> base_opt = CreateAccessToType(class_creation, anonymous_type -> ContainingType());
    }

    AstClassBody *class_body = class_creation -> class_body_opt;

    AstReturnStatement *return_statement = compilation_unit -> ast_pool -> GenReturnStatement();
    return_statement -> return_token     = class_body -> left_brace_token;
    return_statement -> expression_opt   = NULL;
    return_statement -> semicolon_token  = class_body -> left_brace_token;
    return_statement -> is_reachable     = true;

    AstBlock *block                = compilation_unit -> ast_pool -> GenBlock();
    block -> block_symbol          = constructor -> block_symbol -> InsertBlockSymbol(1); // this symbol table will be empty
    block -> left_brace_token      = class_body -> left_brace_token;
    block -> right_brace_token     = class_body -> left_brace_token;

    block -> is_reachable          = true;
    block -> can_complete_normally = false;
    block -> AllocateBlockStatements(1); // this block contains one statement
    block -> AddStatement(return_statement);

    AstConstructorBlock *constructor_block                   = compilation_unit -> ast_pool -> GenConstructorBlock();
    constructor_block -> left_brace_token                    = class_body -> left_brace_token;
    constructor_block -> explicit_constructor_invocation_opt = super_call;
    constructor_block -> block                               = block;
    constructor_block -> right_brace_token                   = class_body -> left_brace_token;

    AstMethodDeclarator *method_declarator       = compilation_unit -> ast_pool -> GenMethodDeclarator();
    method_declarator -> identifier_token        = left_loc;
    method_declarator -> left_parenthesis_token  = class_creation -> left_parenthesis_token;
    method_declarator -> right_parenthesis_token = right_loc;

    AstConstructorDeclaration *constructor_declaration  = compilation_unit -> ast_pool -> GenConstructorDeclaration();
    constructor_declaration -> constructor_declarator   = method_declarator;
    constructor_declaration -> constructor_body         = constructor_block;

    constructor_declaration -> constructor_symbol = constructor;
    constructor -> method_or_constructor_declaration = constructor_declaration;

    //
    // Note that the constructor for the anonymous type is not added to the
    // class body here beacause we've already completely compiled it and the
    // arguments to its super call do not contain "valid" SimpleName Ast
    // expressions. It is added to the constructor body later in
    // get_anonymous_type...
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
            // Add the this$0 parameter for the super type. However, only
            // mark it complete and do not yet assign a number to it. This
            // will be done after we know how many extra "local" variable
            // shadows are needed. See UpdateGeneratedLocalConstructor
            //
            super_this0_variable = block_symbol -> InsertVariableSymbol(control.MakeParameter(0));
            super_this0_variable -> MarkSynthetic();
            super_this0_variable -> SetType(super_call -> base_opt -> Type());
            super_this0_variable -> SetOwner(generated_constructor);
            super_this0_variable -> MarkComplete();
        }

        if (super_type -> IsLocal()) // a local type may use enclosed local variables?
        {
            if (super_type -> LocalClassProcessingCompleted())
            {
                assert(super_constructor -> LocalConstructor() && (! super_constructor -> IsGeneratedLocalConstructor()));

                super_call -> symbol = super_constructor -> LocalConstructor();

                //
                // TODO: Should we set the size for the super_call arguments here ???
                //
                for (int i = (super_type -> ACC_STATIC() ? 0 : 1); i < super_type -> NumConstructorParameters(); i++)
                {
                    VariableSymbol *local = super_type -> ConstructorParameter(i) -> accessed_local,
                                   *local_shadow = anonymous_type -> FindOrInsertLocalShadow(local);

                    AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(super_call -> super_token);
                    simple_name -> symbol = block_symbol -> FindVariableSymbol(local_shadow -> Identity());

                    assert(simple_name -> symbol);

                    super_call -> AddLocalArgument(simple_name);
                }
            }
            else // are we currently within the body of the type in question ?
                super_type -> AddLocalConstructorCallEnvironment(GetEnvironment(super_call));
        }
    }
    else if (super_call -> base_opt)
    {
        super_this0_variable = block_symbol -> InsertVariableSymbol(control.MakeParameter(0));
        super_this0_variable -> MarkSynthetic();
        super_this0_variable -> SetType(super_call -> base_opt -> Type());
        super_this0_variable -> SetOwner(constructor);
        super_this0_variable -> SetLocalVariableIndex(block_symbol -> max_variable_index++);

        constructor -> AddFormalParameter(super_this0_variable);
    }

    //
    // Complete the definition of the constructor and update the super call
    // accordingly.
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
    // Are we guaranteed to have all the info available here? Yes, because if
    // the anonymous type is not local to a method, then its super type
    // cannot be local to a method. Therefore, no extra argument (other than
    // the proper this$0 specified in the base) is needed. If on the other
    // hand the anonymous type is local and its supertype is also local, it
    // must have appeared before the anonymous type and therefore its
    // information has already been computed.
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
}

//
// super_type is the type specified in the anonymous constructor,
// which is the supertype of the created anonymous type.
//
TypeSymbol *Semantic::GetAnonymousType(AstClassInstanceCreationExpression *class_creation,
                                       TypeSymbol *super_type)
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
    assert(class_body);
    TypeSymbol *outermost_type = this_type -> outermost_type;

    //
    // Make up a proper name for the anonymous type
    //
    IntToWstring value(outermost_type -> NumAnonymousTypes() + 1);

    int length = value.Length() + outermost_type -> NameLength() + 1; // +1 for $
    wchar_t *anonymous_name = new wchar_t[length + 1];
    wcscpy(anonymous_name, outermost_type -> Name());
    wcscat(anonymous_name, StringConstant::US_DS);
    wcscat(anonymous_name, value.String());

    NameSymbol *name_symbol = control.FindOrInsertName(anonymous_name, length);

    assert((! ThisMethod()) || LocalSymbolTable().Top());

    TypeSymbol *inner_type = (ThisMethod() ? LocalSymbolTable().Top() -> InsertAnonymousTypeSymbol(name_symbol)
                                           : this_type -> InsertAnonymousTypeSymbol(name_symbol));
    inner_type -> SetACC_PRIVATE();
    inner_type -> MarkAnonymous();
    inner_type -> outermost_type = outermost_type;
    inner_type -> supertypes_closure = new SymbolSet;
    inner_type -> subtypes_closure = new SymbolSet;
    inner_type -> semantic_environment = new SemanticEnvironment((Semantic *) this, inner_type, state_stack.Top());
    inner_type -> declaration = class_creation;
    inner_type -> file_symbol = source_file_symbol;
    inner_type -> SetOwner(ThisMethod() ? (Symbol *) ThisMethod() : (Symbol *) this_type);
    //
    // Add 3 extra elements for padding. Need a default constructor and
    // other support elements.
    //
    inner_type -> SetSymbolTable(class_body -> NumClassBodyDeclarations() + 3);
    inner_type -> SetLocation();
    inner_type -> SetSignature(control);

    //
    // By JLS2 15.9.5, an anonymous class is implicitly final, but never
    // static.  However, in a static context, error checking is much
    // easier if the STATIC flag is temporarily set; we turn it off below.
    //
    inner_type -> SetACC_FINAL();
    if (StaticRegion() || (ExplicitConstructorInvocation() && inner_type -> ContainingType() == ThisType()))
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

    AstClassDeclaration *class_declaration = compilation_unit -> ast_pool -> GenClassDeclaration();
    class_declaration -> class_token = class_body -> left_brace_token;
    class_declaration -> identifier_token = class_body -> left_brace_token;
    class_declaration -> class_body = class_body;

    class_creation -> anonymous_declaration = class_declaration;

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
    // If the symbol was resolved to "no_type" then constructor will be NULL
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
    // See above - anonymous classes are never static.
    //
    inner_type -> ResetACC_STATIC();

    return inner_type;
}


void Semantic::ProcessClassInstanceCreationExpression(Ast *expr)
{
    AstClassInstanceCreationExpression *class_creation = (AstClassInstanceCreationExpression *) expr;

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
        if (type -> ACC_INTERFACE())
        {
            ReportSemError(SemanticError::INTERFACE_NOT_INNER_CLASS,
                           actual_type -> LeftToken(),
                           actual_type -> RightToken(),
                           type -> ContainingPackage() -> PackageName(),
                           type -> ExternalName());
            class_creation -> symbol = control.no_type;
            return;
        }
        if (type -> ACC_STATIC())
        {
            ReportSemError(SemanticError::STATIC_NOT_INNER_CLASS,
                           actual_type -> LeftToken(),
                           actual_type -> RightToken(),
                           type -> ContainingPackage() -> PackageName(),
                           type -> ExternalName());
            class_creation -> symbol = control.no_type;
            return;
        }
    }
    else
    {
        type = MustFindType(actual_type);
        if (type -> IsInner())
        {
            //
            // Within an explicit constructor invocation, a class that is
            // immediately nested in the class being created is not accessible.
            //
            if (ExplicitConstructorInvocation() && type -> ContainingType() == ThisType())
            {
                ReportSemError(SemanticError::INNER_CONSTRUCTOR_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
                               class_creation -> LeftToken(),
                               class_creation -> RightToken(),
                               type -> ContainingPackage() -> PackageName(),
                               type -> ExternalName(),
                               ThisType() -> ContainingPackage() -> PackageName(),
                               ThisType() -> ExternalName());
                class_creation -> symbol = control.no_type;
                return;
            }

            class_creation -> base_opt = CreateAccessToType(class_creation, type -> ContainingType());
        }
    }

    bool no_bad_argument = true;
    for (int i = 0; i < class_creation -> NumArguments(); i++)
    {
        AstExpression *expr = class_creation -> Argument(i);
        ProcessExpressionOrStringConstant(expr);
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
            assert(method -> IsTyped());

            if (class_creation -> base_opt &&
                (class_creation -> base_opt -> symbol != control.no_type) &&
                (class_creation -> base_opt -> Type() != method -> containing_type -> ContainingType()))
            {
                assert(method -> containing_type);
                assert(method -> containing_type -> ContainingType());
                assert(class_creation -> base_opt -> Type());
                assert(CanMethodInvocationConvert(method -> containing_type -> ContainingType(),
                                                  class_creation -> base_opt -> Type()));

                class_creation -> base_opt = ConvertToType(class_creation -> base_opt, method -> containing_type -> ContainingType());
            }

            for (int i = 0; i < class_creation -> NumArguments(); i++)
            {
                AstExpression *expr = class_creation -> Argument(i);
                if (expr -> Type() != method -> FormalParameter(i) -> Type())
                    class_creation -> Argument(i) = ConvertToType(expr, method -> FormalParameter(i) -> Type());
            }

            if (class_creation -> class_body_opt)
            {
                anonymous_type = GetAnonymousType(class_creation, type);
                method = anonymous_type -> FindConstructorSymbol();
                if (method -> LocalConstructor())
                    method = method -> LocalConstructor();
            }
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
            }

            SymbolSet *exception_set = TryExceptionTableStack().Top();
            for (int j = method -> NumThrows() - 1; j >= 0; j--)
            {
                TypeSymbol *exception = method -> Throws(j);
                if (exception_set)
                    exception_set -> AddElement(exception);

                if (UncaughtException(exception))
                    ReportSemError((class_creation -> class_body_opt
                                    ? SemanticError::UNCAUGHT_CONSTRUCTOR_EXCEPTION
                                    : SemanticError::UNCAUGHT_ANONYMOUS_CONSTRUCTOR_EXCEPTION),
                                   actual_type -> LeftToken(),
                                   actual_type -> RightToken(),
                                   type -> ExternalName(),
                                   exception -> ContainingPackage() -> PackageName(),
                                   exception -> ExternalName(),
                                   UncaughtExceptionContext());
            }

            class_creation -> symbol = (anonymous_type ? anonymous_type : type);

            //
            // Note that since constructors are not inherited, we do not need
            // to worry about the protected case here.
            //
            if (ThisType() != type &&
                ThisType() -> outermost_type == type -> outermost_type &&
                method -> ACC_PRIVATE())
            {
                if (! method -> LocalConstructor())
                {
                    method = type -> GetReadAccessConstructor(method);
                    class_creation -> class_type -> symbol = method;

                    //
                    // Add extra argument for read access constructor;
                    //
                    class_creation -> AddNullArgument();
                }
            }
            else ConstructorAccessCheck(class_creation, method);

            //
            // A local type may use enclosed local variables. So, we at least
            // allocate the space for adding these extra arguments. If the
            // type being created has already been fully processed, add the
            // extra arguments here.
            //
            if ((! anonymous_type) && type -> IsLocal())
            {
                if (type -> LocalClassProcessingCompleted() && method -> LocalConstructor())
                {
                    assert(! method -> IsGeneratedLocalConstructor());

                    class_creation -> class_type -> symbol = method -> LocalConstructor();

                    assert(method -> LocalConstructor() -> signature);

                    for (int i = (type -> ACC_STATIC() ? 0 : 1); i < type -> NumConstructorParameters(); i++)
                    {
                        VariableSymbol *local = type -> ConstructorParameter(i) -> accessed_local;

                        AstSimpleName *simple_name = compilation_unit -> ast_pool -> GenSimpleName(class_creation -> new_token);
                        //
                        // Are we currently within the body of the method
                        // that contains the local type in question?
                        //
                        simple_name -> symbol = (type -> owner == ThisMethod()
                                                                ? local
                                                                : ThisType() -> FindOrInsertLocalShadow(local));
                        class_creation -> AddLocalArgument(simple_name);
                    }
                }
                else // are we within body of type in question? Save processing for later. See ProcessClassDeclaration in body.cpp
                    type -> AddLocalConstructorCallEnvironment(GetEnvironment(class_creation));
            }
        }
    }
}


void Semantic::ProcessArrayCreationExpression(Ast *expr)
{
    AstArrayCreationExpression *array_creation = (AstArrayCreationExpression *) expr;

    AstArrayType *array_type;

    TypeSymbol *type;

    if ((array_type = array_creation -> array_type -> ArrayTypeCast()))
    {
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
        if (expr -> Type() != control.int_type &&
            expr -> symbol != control.no_type)
        {
            ReportSemError(SemanticError::TYPE_NOT_INTEGER,
                           dim_expr -> expression -> LeftToken(),
                           dim_expr -> expression -> RightToken(),
                           dim_expr -> expression -> Type() -> Name());
        }
        dim_expr -> expression = expr;
        if (expr -> IsConstant() &&
            (DYNAMIC_CAST<IntLiteralValue *> (expr -> value)) -> value < 0)
        {
            ReportSemError(SemanticError::NEGATIVE_ARRAY_SIZE,
                           dim_expr -> expression -> LeftToken(),
                           dim_expr -> expression -> RightToken());
        }
    }

    if (array_creation -> array_initializer_opt)
        ProcessArrayInitializer((AstArrayInitializer *) array_creation -> array_initializer_opt, type);
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
                postfix_expression -> write_method = read_method -> containing_type -> GetWriteAccessFromReadAccess(read_method);
            }
            else variable_symbol = postfix_expression -> expression -> symbol -> VariableCast();
        }
    }
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

    if ((int_literal = expr -> expression -> IntegerLiteralCast()))
    {
        LiteralSymbol *literal = lex_stream -> LiteralSymbol(int_literal -> integer_literal_token);

        expr -> value = control.int_pool.FindOrInsertNegativeInt(literal);
        if (expr -> value == control.BadValue())
        {
            ReportSemError(SemanticError::INVALID_INT_VALUE,
                           expr -> LeftToken(),
                           expr -> RightToken());
            expr -> symbol = control.no_type;
        }
        else expr -> symbol = control.int_type;
    }
    else if ((long_literal = expr -> expression -> LongLiteralCast()))
    {
        LiteralSymbol *literal = lex_stream -> LiteralSymbol(long_literal -> long_literal_token);

        expr -> value = control.long_pool.FindOrInsertNegativeLong(literal);
        if (expr -> value == control.BadValue())
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

            if ((type == control.double_type))
            {
                DoubleLiteralValue *literal = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> expression -> value);
                expr -> value = control.double_pool.FindOrInsert(-literal -> value);
            }
            else if ((type == control.float_type))
            {
                FloatLiteralValue *literal = DYNAMIC_CAST<FloatLiteralValue *> (expr -> expression -> value);
                expr -> value = control.float_pool.FindOrInsert(-literal -> value);
            }
            else if ((type == control.long_type))
            {
                LongLiteralValue *literal = DYNAMIC_CAST<LongLiteralValue *> (expr -> expression -> value);
                expr -> value = control.long_pool.FindOrInsert(-literal -> value);
            }
            else
            {
                IntLiteralValue *literal = DYNAMIC_CAST<IntLiteralValue *> (expr -> expression -> value);
                expr -> value = control.int_pool.FindOrInsert(-literal -> value);
            }
        }
    }
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
                LongLiteralValue *literal = DYNAMIC_CAST<LongLiteralValue *> (expr -> expression -> value);
                expr -> value = control.long_pool.FindOrInsert(~literal -> value);
            }
            else // assert(expr -> expression -> Type() == control.int_type)
            {
                IntLiteralValue *literal = DYNAMIC_CAST<IntLiteralValue *> (expr -> expression -> value);
                expr -> value = control.int_pool.FindOrInsert(~literal -> value);
            }
        }
        expr -> symbol = expr -> expression -> symbol;
    }
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
            expr -> value = control.int_pool.FindOrInsert(IsConstantTrue(expr -> expression) ? 0 : 1);
        expr -> symbol = control.boolean_type;
    }
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
                expr -> write_method = read_method -> containing_type -> GetWriteAccessFromReadAccess(read_method);
            }
            else variable_symbol = expr -> expression -> symbol -> VariableCast();
        }
    }
    expr -> symbol = expr -> expression -> symbol;
}


void Semantic::ProcessPreUnaryExpression(Ast *expr)
{
    AstPreUnaryExpression *prefix_expression = (AstPreUnaryExpression *) expr;
    (this ->* ProcessPreUnaryExpr[prefix_expression -> pre_unary_tag])(prefix_expression);
}


inline bool Semantic::CanWideningPrimitiveConvert(TypeSymbol *target_type,
                                                  TypeSymbol *source_type)
{
    if (target_type == control.double_type)
        return (source_type == control.float_type ||
                source_type == control.long_type ||
                source_type == control.int_type ||
                source_type == control.char_type ||
                source_type == control.short_type ||
                source_type == control.byte_type);
    if (target_type == control.float_type)
        return (source_type == control.long_type ||
                source_type == control.int_type ||
                source_type == control.char_type ||
                source_type == control.short_type ||
                source_type == control.byte_type);
    if (target_type == control.long_type)
        return (source_type == control.int_type ||
                source_type == control.char_type ||
                source_type == control.short_type ||
                source_type == control.byte_type);
    if (target_type == control.int_type)
        return (source_type == control.char_type ||
                source_type == control.short_type ||
                source_type == control.byte_type);
    if (target_type == control.short_type)
        return source_type == control.byte_type;

    return false;
}


inline bool Semantic::CanNarrowingPrimitiveConvert(TypeSymbol *target_type,
                                                   TypeSymbol *source_type)
{
    if (target_type == control.byte_type)
        return (source_type == control.double_type ||
                source_type == control.float_type ||
                source_type == control.long_type ||
                source_type == control.int_type ||
                source_type == control.char_type ||
                source_type == control.short_type);
    if (target_type == control.char_type)
        return (source_type == control.double_type ||
                source_type == control.float_type ||
                source_type == control.long_type ||
                source_type == control.int_type ||
                source_type == control.short_type ||
                source_type == control.byte_type);
    if (target_type == control.short_type)
        return (source_type == control.double_type ||
                source_type == control.float_type ||
                 source_type == control.long_type ||
                source_type == control.int_type ||
                source_type == control.char_type);
    if (target_type == control.int_type)
        return (source_type == control.double_type ||
                source_type == control.float_type ||
                source_type == control.long_type);
    if (target_type == control.long_type)
        return (source_type == control.double_type ||
                source_type == control.float_type);
    if (target_type == control.float_type)
        return source_type == control.double_type;

    return false;
}


bool Semantic::CanMethodInvocationConvert(TypeSymbol *target_type,
                                          TypeSymbol *source_type)
{
    if (target_type == control.no_type || source_type == control.no_type)
        return false;

    if (source_type -> Primitive())
    {
        if (! target_type -> Primitive())
            return false;

        return (target_type == source_type ||
                CanWideningPrimitiveConvert(target_type, source_type));
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
                return (source_subtype -> Primitive() &&
                        target_subtype -> Primitive()
                        ? (source_subtype == target_subtype)
                        : CanMethodInvocationConvert(target_subtype,
                                                     source_subtype));
            }
            return (target_type == control.Object() ||
                    target_type == control.Cloneable() ||
                    target_type == control.Serializable());
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


bool Semantic::CanAssignmentConvertReference(TypeSymbol *target_type,
                                             TypeSymbol *source_type)
{
    return (target_type == control.no_type ||
            source_type == control.no_type ||
            CanMethodInvocationConvert(target_type, source_type));
}


bool Semantic::CanAssignmentConvert(TypeSymbol *target_type,
                                    AstExpression *expr)
{
    return (target_type == control.no_type ||
            expr -> symbol == control.no_type ||
            CanMethodInvocationConvert(target_type, expr -> Type()) ||
            IsIntValueRepresentableInType(expr, target_type));
}


bool Semantic::CanCastConvert(TypeSymbol *target_type,
                              TypeSymbol *source_type,
                              LexStream::TokenIndex tok)
{
    if (source_type == target_type || source_type == control.no_type || target_type == control.no_type)
        return true;

    if (source_type -> Primitive())
    {
        if (! target_type -> Primitive())
            return false;

        return (CanWideningPrimitiveConvert(target_type, source_type) ||
                CanNarrowingPrimitiveConvert(target_type, source_type));
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
                return (source_subtype -> Primitive() &&
                        target_subtype -> Primitive()
                        ? (source_subtype == target_subtype)
                        : CanCastConvert(target_subtype, source_subtype, tok));
            }
            return (target_type == control.Object() ||
                    target_type == control.Cloneable() ||
                    target_type == control.Serializable());
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
                // Iterate over all methods in the source symbol table of the
                // source_type interface; For each such method, if the
                // target_type contains a method with the same signature, then
                // make sure that the two methods have the same return type.
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

                // all the methods passed the test
                return (i == source_method_table -> symbol_pool.Length());
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


//
// We only need to cast the value of constant primitive expressions.
//
LiteralValue *Semantic::CastValue(TypeSymbol *target_type,
                                  AstExpression *expr)
{
    TypeSymbol *source_type = expr -> Type();

    if (target_type == source_type || source_type == control.no_type ||
        ! expr -> IsConstant())
    {
        assert(target_type == source_type || ! expr -> value);
        return expr -> value;
    }
    if (source_type == control.String())
        return NULL; // A string cast to a supertype is not constant.

    LiteralValue *literal_value = NULL;
    if (target_type == control.String())
    {
        if (source_type == control.double_type)
        {
            DoubleLiteralValue *literal = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> value);
            DoubleToString ieee_double(literal -> value);
            literal_value = control.Utf8_pool.FindOrInsert(ieee_double.String(), ieee_double.Length());
        }
        else if (source_type == control.float_type)
        {
            FloatLiteralValue *literal = DYNAMIC_CAST<FloatLiteralValue *> (expr -> value);
            FloatToString ieee_float(literal -> value);
            literal_value = control.Utf8_pool.FindOrInsert(ieee_float.String(), ieee_float.Length());
        }
        else if (source_type == control.long_type)
        {
            LongLiteralValue *literal = DYNAMIC_CAST<LongLiteralValue *> (expr -> value);
            LongToDecString long_integer(literal -> value);
            literal_value = control.Utf8_pool.FindOrInsert(long_integer.String(), long_integer.Length());
        }
        else if (source_type == control.char_type)
        {
            IntLiteralValue *literal = DYNAMIC_CAST<IntLiteralValue *> (expr -> value);
            literal_value = control.Utf8_pool.FindOrInsert(literal -> value);
        }
        else if (source_type == control.boolean_type)
        {
            if (IsConstantFalse(expr))
                literal_value = control.Utf8_pool.FindOrInsert(StringConstant::U8S_false, 5);
            else
            {
                assert(IsConstantTrue(expr));
                literal_value = control.Utf8_pool.FindOrInsert(StringConstant::U8S_true, 4);
            }
        }
        else if (control.IsSimpleIntegerValueType(source_type))
        {
            IntLiteralValue *literal = DYNAMIC_CAST<IntLiteralValue *> (expr -> value);
            IntToString integer(literal -> value);
            literal_value = control.Utf8_pool.FindOrInsert(integer.String(), integer.Length());
        }
    }
    else if (target_type == control.double_type)
    {
        if (source_type == control.float_type)
        {
            FloatLiteralValue *literal = DYNAMIC_CAST<FloatLiteralValue *> (expr -> value);
            literal_value = control.double_pool.FindOrInsert(literal -> value.DoubleValue());
        }
        else if (source_type == control.long_type)
        {
            LongLiteralValue *literal = DYNAMIC_CAST<LongLiteralValue *> (expr -> value);
            IEEEdouble value(literal -> value);
            literal_value = control.double_pool.FindOrInsert(value);
        }
        else
        {
            IntLiteralValue *literal = DYNAMIC_CAST<IntLiteralValue *> (expr -> value);
            IEEEdouble value(literal -> value);
            literal_value = control.double_pool.FindOrInsert(value);
        }
    }
    else if (target_type == control.float_type)
    {
        if (source_type == control.double_type)
        {
            DoubleLiteralValue *literal = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> value);
            literal_value = control.float_pool.FindOrInsert(literal -> value.FloatValue());
        }
        else if (source_type == control.long_type)
        {
            LongLiteralValue *literal = DYNAMIC_CAST<LongLiteralValue *> (expr -> value);
            IEEEfloat value(literal -> value);
            literal_value = control.float_pool.FindOrInsert(value);
        }
        else
        {
            IntLiteralValue *literal = DYNAMIC_CAST<IntLiteralValue *> (expr -> value);
            IEEEfloat value(literal -> value);
            literal_value = control.float_pool.FindOrInsert(value);
        }
    }
    else if (target_type == control.long_type)
    {
        if (source_type == control.double_type)
        {
            DoubleLiteralValue *literal = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> value);
            literal_value = control.long_pool.FindOrInsert(literal -> value.LongValue());
        }
        else if (source_type == control.float_type)
        {
            FloatLiteralValue *literal = DYNAMIC_CAST<FloatLiteralValue *> (expr -> value);
            literal_value = control.long_pool.FindOrInsert(literal -> value.LongValue());
        }
        else
        {
            IntLiteralValue *literal = DYNAMIC_CAST<IntLiteralValue *> (expr -> value);
            literal_value = control.long_pool.FindOrInsert((LongInt) literal -> value);
        }
    }
    else if (target_type == control.int_type)
    {
        if (source_type == control.double_type)
        {
            DoubleLiteralValue *literal = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((literal -> value).IntValue());
        }
        else if (source_type == control.float_type)
        {
            FloatLiteralValue *literal = DYNAMIC_CAST<FloatLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert(literal -> value.IntValue());
        }
        else if (source_type == control.long_type)
        {
            LongLiteralValue *literal = DYNAMIC_CAST<LongLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (literal -> value).LowWord());
        }
        else literal_value = expr -> value;
    }
    else if (target_type == control.char_type)
    {
        if (source_type == control.double_type)
        {
            DoubleLiteralValue *literal = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (u2) (literal -> value.IntValue()));
        }
        else if (source_type == control.float_type)
        {
            FloatLiteralValue *literal = DYNAMIC_CAST<FloatLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (u2) (literal -> value.IntValue()));
        }
        else if (source_type == control.long_type)
        {
            LongLiteralValue *literal = DYNAMIC_CAST<LongLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (u2) (literal -> value).LowWord());
        }
        else
        {
            IntLiteralValue *literal = DYNAMIC_CAST<IntLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (u2) literal -> value);
        }
    }
    else if (target_type == control.short_type)
    {
        if (source_type == control.double_type)
        {
            DoubleLiteralValue *literal = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (i2) (literal -> value.IntValue()));
        }
        else if (source_type == control.float_type)
        {
            FloatLiteralValue *literal = DYNAMIC_CAST<FloatLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (i2) (literal -> value.IntValue()));
        }
        else if (source_type == control.long_type)
        {
            LongLiteralValue *literal = DYNAMIC_CAST<LongLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (i2) (literal -> value).LowWord());
        }
        else
        {
            IntLiteralValue *literal = DYNAMIC_CAST<IntLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (i2) literal -> value);
        }
    }
    else if (target_type == control.byte_type)
    {
        if (source_type == control.double_type)
        {
            DoubleLiteralValue *literal = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (i1) (literal -> value.IntValue()));
        }
        else if (source_type == control.float_type)
        {
            FloatLiteralValue *literal = DYNAMIC_CAST<FloatLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (i1) (literal -> value.IntValue()));
        }
        else if (source_type == control.long_type)
        {
            LongLiteralValue *literal = DYNAMIC_CAST<LongLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((int) (i1) (literal -> value).LowWord());
        }
        else
        {
            IntLiteralValue *literal = DYNAMIC_CAST<IntLiteralValue *> (expr -> value);
            literal_value = control.int_pool.FindOrInsert((i4) (i1) literal -> value);
        }
    }

    assert(literal_value);
    return literal_value;
}


void Semantic::ProcessCastExpression(Ast *expr)
{
    AstCastExpression *cast_expression = (AstCastExpression *) expr;

    //
    // Do not use ProcessExpressionOrStringConstant here, to avoid generating
    // intermediate Strings - see CheckConstantString in lookup.cpp
    //
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
    target_type = (num_dimensions == 0 ? target_type
                   : target_type -> GetArrayType((Semantic *) this,
                                                 num_dimensions));

    if (CanAssignmentConvert(target_type, cast_expression -> expression))
    {
        cast_expression -> symbol = target_type;
        cast_expression -> value = CastValue(target_type,
                                             cast_expression -> expression);
    }
    else if (CanCastConvert(target_type, source_type,
                            cast_expression -> right_parenthesis_token_opt))
    {
        cast_expression -> kind = Ast::CHECK_AND_CAST;
        cast_expression -> symbol = target_type;
        cast_expression -> value = CastValue(target_type,
                                             cast_expression -> expression);
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
}


AstExpression *Semantic::ConvertToType(AstExpression *expr, TypeSymbol *type)
{
    if (expr -> Type() == control.null_type || type -> Bad())
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

    return ((type == control.byte_type ||
             type == control.short_type ||
             type == control.char_type)
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
            binary_expression -> right_expression =
                ConvertToType(binary_expression -> right_expression,
                              control.double_type);
        binary_expression -> symbol = control.double_type;
    }
    else if (right_type == control.double_type)
    {
        binary_expression -> left_expression =
            ConvertToType(binary_expression -> left_expression,
                          control.double_type);
        binary_expression -> symbol = control.double_type;
    }
    else if (left_type == control.float_type)
    {
        if (right_type != control.float_type)
            binary_expression -> right_expression =
                ConvertToType(binary_expression -> right_expression,
                              control.float_type);
        binary_expression -> symbol = control.float_type;
    }
    else if (right_type == control.float_type)
    {
        binary_expression -> left_expression =
            ConvertToType(binary_expression -> left_expression,
                          control.float_type);
        binary_expression -> symbol = control.float_type;
    }
    else if (left_type == control.long_type)
    {
        if (right_type != control.long_type)
            binary_expression -> right_expression =
                ConvertToType(binary_expression -> right_expression,
                              control.long_type);
        binary_expression -> symbol = control.long_type;
    }
    else if (right_type == control.long_type)
    {
        binary_expression -> left_expression =
            ConvertToType(binary_expression -> left_expression,
                          control.long_type);
        binary_expression -> symbol = control.long_type;
    }
    else
    {
        if (left_type != control.int_type)
            binary_expression -> left_expression =
                ConvertToType(binary_expression -> left_expression,
                              control.int_type);
        if (right_type != control.int_type)
            binary_expression -> right_expression =
                ConvertToType(binary_expression -> right_expression,
                              control.int_type);
        binary_expression -> symbol = control.int_type;
    }
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
            assignment_expression -> expression =
                ConvertToType(assignment_expression -> expression,
                              control.double_type);
    }
    else if (right_type == control.double_type)
    {
        assignment_expression -> left_hand_side =
            ConvertToType(assignment_expression -> left_hand_side,
                          control.double_type);
    }
    else if (left_type == control.float_type)
    {
        if (right_type != control.float_type)
            assignment_expression -> expression =
                ConvertToType(assignment_expression -> expression,
                              control.float_type);
    }
    else if (right_type == control.float_type)
    {
        assignment_expression -> left_hand_side =
            ConvertToType(assignment_expression -> left_hand_side,
                          control.float_type);
    }
    else if (left_type == control.long_type)
    {
        if (right_type != control.long_type)
            assignment_expression -> expression =
                ConvertToType(assignment_expression -> expression,
                              control.long_type);
    }
    else if (right_type == control.long_type)
    {
        assignment_expression -> left_hand_side =
            ConvertToType(assignment_expression -> left_hand_side,
                          control.long_type);
    }
    else
    {
        if (left_type != control.int_type)
            assignment_expression -> left_hand_side =
                ConvertToType(assignment_expression -> left_hand_side,
                              control.int_type);
        if (right_type != control.int_type)
            assignment_expression -> expression =
                ConvertToType(assignment_expression -> expression,
                              control.int_type);
    }
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
                ConvertToType(conditional_expression -> false_expression,
                              control.double_type);
        conditional_expression -> symbol = control.double_type;
    }
    else if (right_type == control.double_type)
    {
        conditional_expression -> true_expression =
            ConvertToType(conditional_expression -> true_expression,
                          control.double_type);
        conditional_expression -> symbol = control.double_type;
    }
    else if (left_type == control.float_type)
    {
        if (right_type != control.float_type)
            conditional_expression -> false_expression =
                ConvertToType(conditional_expression -> false_expression,
                              control.float_type);
        conditional_expression -> symbol = control.float_type;
    }
    else if (right_type == control.float_type)
    {
        conditional_expression -> true_expression =
            ConvertToType(conditional_expression -> true_expression,
                          control.float_type);
        conditional_expression -> symbol = control.float_type;
    }
    else if (left_type == control.long_type)
    {
        if (right_type != control.long_type)
            conditional_expression -> false_expression =
                ConvertToType(conditional_expression -> false_expression,
                              control.long_type);
        conditional_expression -> symbol = control.long_type;
    }
    else if (right_type == control.long_type)
    {
        conditional_expression -> true_expression =
            ConvertToType(conditional_expression -> true_expression,
                          control.long_type);
        conditional_expression -> symbol = control.long_type;
    }
    else
    {
        if (left_type != control.int_type)
            conditional_expression -> true_expression =
                ConvertToType(conditional_expression -> true_expression,
                              control.int_type);
        if (right_type != control.int_type)
            conditional_expression -> false_expression =
                ConvertToType(conditional_expression -> false_expression,
                              control.int_type);
        conditional_expression -> symbol = control.int_type;
    }
}


void Semantic::ProcessPLUS(AstBinaryExpression *expr)
{
    //
    // Do not use ProcessExpressionOrStringConstant here, to avoid generating
    // intermediate Strings - see CheckConstantString in lookup.cpp
    //
    AstExpression *left = expr -> left_expression;
    AstExpression *right = expr -> right_expression;
    ProcessExpression(left);
    ProcessExpression(right);

    TypeSymbol *left_type = left -> Type(),
               *right_type = right -> Type();

    if (left_type == control.no_type || right_type == control.no_type)
        expr -> symbol = control.no_type;
    else if (left_type == control.String() || right_type == control.String())
    {
        //
        // Convert the left expression if necessary.
        //
        if (left_type != control.String())
        {
            AddStringConversionDependence(left_type,
                                          expr -> binary_operator_token);
            if (left_type == control.void_type)
                ReportSemError(SemanticError::VOID_TO_STRING,
                               left -> LeftToken(),
                               left -> RightToken());
            else if (left_type == control.null_type)
                left -> symbol = control.String();
            else left -> value = CastValue(control.String(), left);
        }

        //
        // Convert the right expression if necessary.
        //
        if (right_type != control.String())
        {
            AddStringConversionDependence(right_type,
                                          expr -> binary_operator_token);
            if (right_type == control.void_type)
                ReportSemError(SemanticError::VOID_TO_STRING,
                               right -> LeftToken(),
                               right -> RightToken());
            else if (right_type == control.null_type)
                right -> symbol = control.String();
            else right -> value = CastValue(control.String(), right);
        }

        AddDependence(ThisType(), control.StringBuffer(),
                      expr -> binary_operator_token);

        //
        // If both subexpressions are strings constants, identify the result as
        // as a string constant, but do not perform the concatenation here. The
        // reason being that if we have a long expression of the form
        //
        //  s1 + s2 + ... + sn
        //
        // where each subexpression s(i) is a string constant, we want to
        // perform one concatenation and enter a single result into the
        // constant pool instead of n-1 subresults. See CheckStringConstant
        // in lookup.cpp.
        //

        expr -> symbol = control.String();
    }
    else
    {
        BinaryNumericPromotion(expr);
        left = expr -> left_expression;
        right = expr -> right_expression;

        if (left -> IsConstant() && right -> IsConstant())
        {
             if (expr -> Type() == control.double_type)
             {
                 DoubleLiteralValue *left_value =
                     DYNAMIC_CAST<DoubleLiteralValue *> (left -> value);
                 DoubleLiteralValue *right_value =
                     DYNAMIC_CAST<DoubleLiteralValue *> (right -> value);

                 expr -> value =
                     control.double_pool.FindOrInsert(left_value -> value +
                                                      right_value -> value);
             }
             else if (expr -> Type() == control.float_type)
             {
                 FloatLiteralValue *left_value =
                     DYNAMIC_CAST<FloatLiteralValue *> (left -> value);
                 FloatLiteralValue *right_value =
                     DYNAMIC_CAST<FloatLiteralValue *> (right -> value);

                 expr -> value =
                     control.float_pool.FindOrInsert(left_value -> value +
                                                     right_value -> value);
             }
             else if (expr -> Type() == control.long_type)
             {
                 LongLiteralValue *left_value =
                     DYNAMIC_CAST<LongLiteralValue *> (left -> value);
                 LongLiteralValue *right_value =
                     DYNAMIC_CAST<LongLiteralValue *> (right -> value);

                 expr -> value =
                     control.long_pool.FindOrInsert(left_value -> value +
                                                    right_value -> value);
             }
             else // assert(expr -> Type() == control.int_type)
             {
                 IntLiteralValue *left_value =
                     DYNAMIC_CAST<IntLiteralValue *> (left -> value);
                 IntLiteralValue *right_value =
                     DYNAMIC_CAST<IntLiteralValue *> (right -> value);

                 expr -> value =
                     control.int_pool.FindOrInsert(left_value -> value +
                                                   right_value -> value);
             }
        }
    }
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
        if (expr -> right_expression -> Type() == control.long_type)
            expr -> right_expression = ConvertToType(expr -> right_expression, control.int_type);
        expr -> symbol = expr -> left_expression -> symbol;

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.long_pool.FindOrInsert(left -> value << (right -> value & LONG_SHIFT_MASK));
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value << (right -> value & INT_SHIFT_MASK));
            }
        }
    }
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
        if (expr -> right_expression -> Type() == control.long_type)
            expr -> right_expression = ConvertToType(expr -> right_expression, control.int_type);
        expr -> symbol = expr -> left_expression -> symbol;

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.long_pool.FindOrInsert(left -> value >> (right -> value & LONG_SHIFT_MASK));
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value >> (right -> value & INT_SHIFT_MASK));
            }
        }
    }
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
        if (expr -> right_expression -> Type() == control.long_type)
            expr -> right_expression = ConvertToType(expr -> right_expression, control.int_type);
        expr -> symbol = expr -> left_expression -> symbol;

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.long_pool.FindOrInsert((LongInt)
                    ((ULongInt) left -> value >> (right -> value & LONG_SHIFT_MASK)));
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert((i4)
                    ((u4) left -> value >> (right -> value & INT_SHIFT_MASK)));
            }
        }
    }
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
                DoubleLiteralValue *left = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> left_expression -> value);
                DoubleLiteralValue *right = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value < right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = DYNAMIC_CAST<FloatLiteralValue *> (expr -> left_expression -> value);
                FloatLiteralValue *right = DYNAMIC_CAST<FloatLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value < right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.long_type)
           {
                LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value < right -> value ? 1 : 0);
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value < right -> value ? 1 : 0);
            }
        }
    }

    expr -> symbol = control.boolean_type;
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
                DoubleLiteralValue *left = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> left_expression -> value);
                DoubleLiteralValue *right = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value > right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = DYNAMIC_CAST<FloatLiteralValue *> (expr -> left_expression -> value);
                FloatLiteralValue *right = DYNAMIC_CAST<FloatLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value > right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value > right -> value ? 1 : 0);
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value > right -> value ? 1 : 0);
            }
        }
    }

    expr -> symbol = control.boolean_type;
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
                DoubleLiteralValue *left = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> left_expression -> value);
                DoubleLiteralValue *right = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value <= right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = DYNAMIC_CAST<FloatLiteralValue *> (expr -> left_expression -> value);
                FloatLiteralValue *right = DYNAMIC_CAST<FloatLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value <= right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value <= right -> value ? 1 : 0);
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value <= right -> value ? 1 : 0);
            }
        }
    }

    expr -> symbol = control.boolean_type;
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
                DoubleLiteralValue *left = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> left_expression -> value);
                DoubleLiteralValue *right = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value >= right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = DYNAMIC_CAST<FloatLiteralValue *> (expr -> left_expression -> value);
                FloatLiteralValue *right = DYNAMIC_CAST<FloatLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value >= right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value >= right -> value ? 1 : 0);
            }
            else // assert(expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value >= right -> value ? 1 : 0);
            }
        }
    }

    expr -> symbol = control.boolean_type;
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
        if (left_type == control.boolean_type &&
            right_type == control.boolean_type)
        {
             expr -> symbol = control.boolean_type;
             if (expr -> left_expression -> IsConstant() &&
                 expr -> right_expression -> IsConstant())
             {
                 expr -> value =
                     control.int_pool.FindOrInsert((IsConstantTrue(expr -> left_expression) &&
                                                    IsConstantTrue(expr -> right_expression))
                                                   ? 1 : 0);
             }
        }
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
                        LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                        LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *> (expr -> right_expression -> value);

                        expr -> value = control.long_pool.FindOrInsert(left -> value & right -> value);
                    }
                    else // assert(expr_type == control.int_type)
                    {
                        IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                        IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                        expr -> value = control.int_pool.FindOrInsert(left -> value & right -> value);
                    }
                }
            }
        }
    }
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
        if (left_type == control.boolean_type &&
            right_type == control.boolean_type)
        {
             expr -> symbol = control.boolean_type;
             if (expr -> left_expression -> IsConstant() &&
                 expr -> right_expression -> IsConstant())
             {
                 expr -> value =
                     control.int_pool.FindOrInsert((IsConstantTrue(expr -> left_expression) !=
                                                    IsConstantTrue(expr -> right_expression))
                                                   ? 1 : 0);
             }
        }
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
                        LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                        LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *> (expr -> right_expression -> value);

                        expr -> value = control.long_pool.FindOrInsert(left -> value ^ right -> value);
                    }
                    else // assert(expr_type == control.int_type)
                    {
                        IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                        IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                        expr -> value = control.int_pool.FindOrInsert(left -> value ^ right -> value);
                    }
                }
            }
        }
    }
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
        if (left_type == control.boolean_type &&
            right_type == control.boolean_type)
        {
             expr -> symbol = control.boolean_type;
             if (expr -> left_expression -> IsConstant() &&
                 expr -> right_expression -> IsConstant())
             {
                 expr -> value =
                     control.int_pool.FindOrInsert((IsConstantTrue(expr -> left_expression) ||
                                                    IsConstantTrue(expr -> right_expression))
                                                   ? 1 : 0);
             }
        }
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
                        LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                        LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *> (expr -> right_expression -> value);

                        expr -> value = control.long_pool.FindOrInsert(left -> value | right -> value);
                    }
                    else // assert(expr_type == control.int_type)
                    {
                        IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                        IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                        expr -> value = control.int_pool.FindOrInsert(left -> value | right -> value);
                    }
                }
            }
        }
    }
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
        else if (expr -> left_expression -> IsConstant() &&
                 expr -> right_expression -> IsConstant())
        {
            //
            // Even when evaluating false && x, x must be constant for && to
            // be constant
            //
            expr -> value = control.int_pool.
                FindOrInsert((IsConstantTrue(expr -> left_expression) &&
                              IsConstantTrue(expr -> right_expression))
                             ? 1 : 0);
        }
    }

    expr -> symbol = control.boolean_type;
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
        else if (expr -> left_expression -> IsConstant() &&
                 expr -> right_expression -> IsConstant())
        {
            //
            // Even when evaluating true || x, x must be constant for && to
            // be constant
            //
            expr -> value = control.int_pool.
                FindOrInsert((IsConstantTrue(expr -> left_expression) ||
                              IsConstantTrue(expr -> right_expression))
                             ? 1 : 0);
        }
    }

    expr -> symbol = control.boolean_type;
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
                ReportSemError(SemanticError::TYPE_IS_VOID,
                               expr -> LeftToken(),
                               expr -> RightToken(),
                               expr -> Type() -> Name());
            expr -> symbol = left_type;
        }

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            LiteralValue *left = expr -> left_expression -> value;
            LiteralValue *right = expr -> right_expression -> value;

            if (expr -> Type() == control.double_type)
            {
                DoubleLiteralValue *left = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> left_expression -> value);
                DoubleLiteralValue *right = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value == right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = DYNAMIC_CAST<FloatLiteralValue *> (expr -> left_expression -> value);
                FloatLiteralValue *right = DYNAMIC_CAST<FloatLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value == right -> value ? 1 : 0);
            }
            else expr -> value = control.int_pool.FindOrInsert(left == right ? 1 : 0);
        }
    }

    expr -> symbol = control.boolean_type;
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
                ReportSemError(SemanticError::TYPE_IS_VOID,
                               expr -> LeftToken(),
                               expr -> RightToken(),
                               expr -> Type() -> Name());
            expr -> symbol = left_type;
        }

        if (expr -> left_expression -> IsConstant() && expr -> right_expression -> IsConstant())
        {
            LiteralValue *left = expr -> left_expression -> value;
            LiteralValue *right = expr -> right_expression -> value;

            if (expr -> Type() == control.double_type)
            {
                DoubleLiteralValue *left = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> left_expression -> value);
                DoubleLiteralValue *right = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value != right -> value ? 1 : 0);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = DYNAMIC_CAST<FloatLiteralValue *> (expr -> left_expression -> value);
                FloatLiteralValue *right = DYNAMIC_CAST<FloatLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value != right -> value ? 1 : 0);
            }
            else expr -> value = control.int_pool.FindOrInsert(left != right ? 1 : 0);
        }
    }

    expr -> symbol = control.boolean_type;
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
                DoubleLiteralValue *left = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> left_expression -> value);
                DoubleLiteralValue *right = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.double_pool.FindOrInsert(left -> value * right -> value);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = DYNAMIC_CAST<FloatLiteralValue *> (expr -> left_expression -> value);
                FloatLiteralValue *right = DYNAMIC_CAST<FloatLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.float_pool.FindOrInsert(left -> value * right -> value);
            }
            else if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.long_pool.FindOrInsert(left -> value * right -> value);
            }
            else if (expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value * right -> value);
            }
        }
    }
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
                DoubleLiteralValue *left = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> left_expression -> value);
                DoubleLiteralValue *right = DYNAMIC_CAST<DoubleLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.double_pool.FindOrInsert(left -> value - right -> value);
            }
            else if (expr -> Type() == control.float_type)
            {
                FloatLiteralValue *left = DYNAMIC_CAST<FloatLiteralValue *> (expr -> left_expression -> value);
                FloatLiteralValue *right = DYNAMIC_CAST<FloatLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.float_pool.FindOrInsert(left -> value - right -> value);
            }
            else if (expr -> Type() == control.long_type)
            {
                LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (expr -> left_expression -> value);
                LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.long_pool.FindOrInsert(left -> value - right -> value);
            }
            else if (expr -> Type() == control.int_type)
            {
                IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (expr -> left_expression -> value);
                IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (expr -> right_expression -> value);

                expr -> value = control.int_pool.FindOrInsert(left -> value - right -> value);
            }
        }
    }
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

        AstExpression *left_expression = expr -> left_expression,
                      *right_expression = expr -> right_expression;
        if (right_expression -> IsConstant())
        {
            //
            // If the type of the expression is int or long and the right-hand
            // side is 0 then issue an error message. Otherwise, if both
            // subexpressions are constant, calculate result.
            //
            if ((expr -> Type() == control.int_type &&
                 DYNAMIC_CAST<IntLiteralValue *> (right_expression -> value) -> value == 0) ||
                (expr -> Type() == control.long_type &&
                 DYNAMIC_CAST<LongLiteralValue *> (right_expression -> value) -> value == 0))
            {
                ReportSemError((left_expression -> IsConstant()
                                ? SemanticError::ZERO_DIVIDE_ERROR
                                : SemanticError::ZERO_DIVIDE_CAUTION),
                               expr -> LeftToken(),
                               expr -> RightToken());
                if (left_expression -> IsConstant())
                    expr -> symbol = control.no_type;
            }
            else if (left_expression -> IsConstant())
            {
                if (expr -> Type() == control.double_type)
                {
                    DoubleLiteralValue *left = DYNAMIC_CAST<DoubleLiteralValue *> (left_expression -> value);
                    DoubleLiteralValue *right = DYNAMIC_CAST<DoubleLiteralValue *> (right_expression -> value);

                    expr -> value = control.double_pool.FindOrInsert(left -> value / right -> value);
                }
                else if (expr -> Type() == control.float_type)
                {
                    FloatLiteralValue *left = DYNAMIC_CAST<FloatLiteralValue *> (left_expression -> value);
                    FloatLiteralValue *right = DYNAMIC_CAST<FloatLiteralValue *> (right_expression -> value);

                    expr -> value = control.float_pool.FindOrInsert(left -> value / right -> value);
                }
                else if (expr -> Type() == control.long_type)
                {
                    LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (left_expression -> value);
                    LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *> (right_expression -> value);

                    expr -> value = control.long_pool.FindOrInsert(left -> value / right -> value);
                }
                else if (expr -> Type() == control.int_type)
                {
                    IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (left_expression -> value);
                    IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (right_expression -> value);

                    //
                    // There is a bug in the intel hardware where if one tries
                    // to compute ((2**32-1) / -1), he gets a ZeroDivide
                    // exception. Thus, instead of using the straightforward
                    // code below, we use the short-circuited one that follows:
                    //
                    //  expr -> value = control.int_pool.FindOrInsert(left -> value / right -> value);
                    //
                    expr -> value = control.int_pool.FindOrInsert(right -> value == -1 ? -(left -> value)
                                                                                       : left -> value / right -> value);
                }
            }
        }
    }
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

        AstExpression *left_expression = expr -> left_expression,
                      *right_expression = expr -> right_expression;
        if (right_expression -> IsConstant())
        {
            //
            // If the type of the expression is int or long and the right-hand
            // side is 0 then issue an error message. Otherwise, if both
            // subexpressions are constant, calculate result.
            //
            if ((expr -> Type() == control.int_type &&
                 DYNAMIC_CAST<IntLiteralValue *> (right_expression -> value) -> value == 0) ||
                (expr -> Type() == control.long_type &&
                 DYNAMIC_CAST<LongLiteralValue *> (right_expression -> value) -> value == 0))
            {
                ReportSemError((left_expression -> IsConstant()
                                ? SemanticError::ZERO_DIVIDE_ERROR
                                : SemanticError::ZERO_DIVIDE_CAUTION),
                               expr -> LeftToken(),
                               expr -> RightToken());
                if (left_expression -> IsConstant())
                    expr -> symbol = control.no_type;
            }
            else if (left_expression -> IsConstant())
            {
                if (expr -> Type() == control.double_type)
                {
                    DoubleLiteralValue *left = DYNAMIC_CAST<DoubleLiteralValue *> (left_expression -> value);
                    DoubleLiteralValue *right = DYNAMIC_CAST<DoubleLiteralValue *> (right_expression -> value);
                    
                    expr -> value = control.double_pool.FindOrInsert(left -> value % right -> value);
                }
                else if (expr -> Type() == control.float_type)
                {
                    FloatLiteralValue *left = DYNAMIC_CAST<FloatLiteralValue *> (left_expression -> value);
                    FloatLiteralValue *right = DYNAMIC_CAST<FloatLiteralValue *> (right_expression -> value);

                    expr -> value = control.float_pool.FindOrInsert(left -> value % right -> value);
                }
                else if (expr -> Type() == control.long_type)
                {
                    LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *> (left_expression -> value);
                    LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *> (right_expression -> value);

                    expr -> value = control.long_pool.FindOrInsert(left -> value % right -> value);
                }
                else if (expr -> Type() == control.int_type)
                {
                    IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *> (left_expression -> value);
                    IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *> (right_expression -> value);

                    //
                    // There is a bug in the intel hardware where if one tries
                    // to compute ((2**32-1) / -1), he gets a ZeroDivide
                    // exception. Thus, instead of using the straightforward
                    // code below, we use the short-circuited one that follows:
                    //
                    // expr -> value = control.int_pool.FindOrInsert(left -> value % right -> value);
                    //
                    expr -> value = control.int_pool.FindOrInsert((left -> value  == (signed) 0x80000000 &&
                                                                   right -> value == (signed) 0xffffffff)
                                                                                   ? 0
                                                                                   : left -> value % right -> value);
                }
            }
        }
    }
}


void Semantic::ProcessINSTANCEOF(AstBinaryExpression *expr)
{
    ProcessExpressionOrStringConstant(expr -> left_expression);
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
}


void Semantic::ProcessBinaryExpression(Ast *expr)
{
    AstBinaryExpression *binary_expression = (AstBinaryExpression *) expr;
    (this ->* ProcessBinaryExpr[binary_expression -> binary_tag])(binary_expression);
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
}


void Semantic::ProcessConditionalExpression(Ast *expr)
{
    AstConditionalExpression *conditional_expression = (AstConditionalExpression *) expr;

    ProcessExpression(conditional_expression -> test_expression);
    //
    // TODO: Should we delay calculating results of true/false expressions
    // until CheckStringConstant in lookup.cpp to put fewer intermediate
    // strings in the storage pools?
    //
    ProcessExpressionOrStringConstant(conditional_expression -> true_expression);
    ProcessExpressionOrStringConstant(conditional_expression -> false_expression);

    TypeSymbol *test_type  = conditional_expression -> test_expression -> Type();
    TypeSymbol *true_type  = conditional_expression -> true_expression -> Type();
    TypeSymbol *false_type = conditional_expression -> false_expression -> Type();

    if (test_type == control.no_type ||
        true_type == control.no_type ||
        false_type == control.no_type)
    {
        conditional_expression -> symbol = control.no_type;
    }
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
            (true_type != false_type &&
             (true_type == control.boolean_type ||
              false_type == control.boolean_type)))
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
                if (true_type == control.byte_type &&
                    false_type == control.short_type)
                {
                    conditional_expression -> true_expression =
                                ConvertToType(conditional_expression -> true_expression, control.short_type);
                    conditional_expression -> symbol = control.short_type;
                }
                else if (true_type == control.short_type &&
                         false_type == control.byte_type)
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
            // If all the subexpressions are constants, compute the results and
            // set the value of the expression accordingly.
            //
            if (conditional_expression -> true_expression -> IsConstant() &&
                conditional_expression -> false_expression -> IsConstant())
            {
                if (IsConstantTrue(conditional_expression -> test_expression))
                     conditional_expression -> value = conditional_expression -> true_expression -> value;
                else if (IsConstantFalse(conditional_expression -> test_expression))
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
            return;
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

        //
        // If all the subexpressions are constants, compute the results and
        // set the value of the expression accordingly.
        //
        // Since null should not be a compile-time constant anymore, the assert
        // should not need to check for null type, so it was removed....
        //
        if (conditional_expression -> true_expression -> IsConstant() &&
            conditional_expression -> false_expression -> IsConstant())
        {
            assert(conditional_expression -> symbol == control.String());
            //  || conditional_expression -> symbol == control.null_type);
            
            if (IsConstantTrue(conditional_expression -> test_expression))
                conditional_expression -> value = conditional_expression -> true_expression -> value;
            else if (IsConstantFalse(conditional_expression -> test_expression))
                conditional_expression -> value = conditional_expression -> false_expression -> value;
        }
    }
}


void Semantic::ProcessAssignmentExpression(Ast *expr)
{
    AstAssignmentExpression *assignment_expression = (AstAssignmentExpression *) expr;

    AstExpression *left_hand_side = assignment_expression -> left_hand_side;

    //
    // JLS2 8.3.2.3 permits simple assignment to a variable that has not
    // yet been declared in an initializer.  If the left_hand_side is a
    // variable, we use processing_simple_assignment to inform
    // CheckSimpleName() to treat it specially.
    //
    {
        if (assignment_expression -> assignment_tag == AstAssignmentExpression::SIMPLE_EQUAL)
        {
            AstSimpleName *simple_name = left_hand_side -> SimpleNameCast();
            if (simple_name)
                processing_simple_assignment = true;
        }

        ProcessExpression(left_hand_side);

        processing_simple_assignment = false;
    }

    ProcessExpressionOrStringConstant(assignment_expression -> expression);
    TypeSymbol *left_type = left_hand_side -> Type(),
               *right_type = assignment_expression -> expression -> Type();

    assignment_expression -> symbol = left_type;

    if (left_type == control.no_type || right_type == control.no_type)
        return;

    if (! left_hand_side -> ArrayAccessCast()) // the left-hand-side is a name
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
            assignment_expression -> write_method = read_method -> containing_type -> GetWriteAccessFromReadAccess(read_method);
    }

    if (assignment_expression -> assignment_tag == AstAssignmentExpression::SIMPLE_EQUAL)
    {
        if (left_type != right_type)
        {
            if (CanAssignmentConvert(left_type,
                                     assignment_expression -> expression))
            {
                assignment_expression -> expression =
                    ConvertToType(assignment_expression -> expression,
                                  left_type);
            }
            else if (assignment_expression -> expression -> IsConstant() &&
                     control.IsSimpleIntegerValueType(left_type) &&
                     control.IsSimpleIntegerValueType(right_type))
            {
                if (left_type == control.byte_type)
                    ReportSemError(SemanticError::INVALID_BYTE_VALUE,
                                   assignment_expression -> expression -> LeftToken(),
                                   assignment_expression -> expression -> RightToken());
                else if (left_type == control.short_type)
                    ReportSemError(SemanticError::INVALID_SHORT_VALUE,
                                   assignment_expression -> expression -> LeftToken(),
                                   assignment_expression -> expression -> RightToken());
                else
                {
                    assert(left_type == control.char_type);
                    ReportSemError(SemanticError::INVALID_CHARACTER_VALUE,
                                   assignment_expression -> expression -> LeftToken(),
                                   assignment_expression -> expression -> RightToken());
                }
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
    // In JLS 2, it states that the only reference type on the left can
    // be String, for +=.  However, some compilers accept any type on the left
    // that can be assigned a String, provided the right side is a String.
    // In the process, that means an array access could then throw an
    // ArrayStoreException when the left type is not String.
    //
    // TODO: Get the definative answer from Sun which behavior is correct
    //
    if (left_type == control.String() &&
        assignment_expression -> assignment_tag == AstAssignmentExpression::PLUS_EQUAL)
    {
        if (right_type != control.String())
        {
            if (right_type == control.void_type)
                 ReportSemError(SemanticError::VOID_TO_STRING,
                                assignment_expression -> expression -> LeftToken(),
                                assignment_expression -> expression -> RightToken());
            else assignment_expression -> expression -> value = CastValue(control.String(), assignment_expression -> expression);
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

    switch (assignment_expression -> assignment_tag)
    {
        case AstAssignmentExpression::PLUS_EQUAL:
        case AstAssignmentExpression::STAR_EQUAL:
        case AstAssignmentExpression::MINUS_EQUAL:
            BinaryNumericPromotion(assignment_expression);
            break;
        case AstAssignmentExpression::SLASH_EQUAL:
        case AstAssignmentExpression::MOD_EQUAL:
            BinaryNumericPromotion(assignment_expression);
            {
                AstExpression *right_expression = assignment_expression -> expression;
                if (right_expression -> IsConstant())
                {
                    //
                    // If the type of the expression is integral and the right
                    // hand side is constant 0 then issue an error message.
                    //
                    if (control.IsIntegral(left_type) &&
                        (right_expression -> Type() == control.int_type &&
                         DYNAMIC_CAST<IntLiteralValue *>
                         (right_expression -> value) -> value == 0) ||
                        (right_expression -> Type() == control.long_type &&
                         DYNAMIC_CAST<LongLiteralValue *>
                         (right_expression -> value) -> value == 0))
                    {
                        ReportSemError(SemanticError::ZERO_DIVIDE_CAUTION,
                                       assignment_expression -> LeftToken(),
                                       assignment_expression -> RightToken());
                    }
                }
            }
            break;
        case AstAssignmentExpression::LEFT_SHIFT_EQUAL:
        case AstAssignmentExpression::RIGHT_SHIFT_EQUAL:
        case AstAssignmentExpression::UNSIGNED_RIGHT_SHIFT_EQUAL:
             if (! control.IsIntegral(left_type))
             {
                 ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                                left_hand_side -> LeftToken(),
                                left_hand_side -> RightToken(),
                                left_type -> Name());
             }

             if (! control.IsIntegral(right_type))
             {
                 ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                                assignment_expression -> expression -> LeftToken(),
                                assignment_expression -> expression -> RightToken(),
                                right_type -> Name());
             }

             assignment_expression -> left_hand_side = PromoteUnaryNumericExpression(left_hand_side);
             assignment_expression -> expression = PromoteUnaryNumericExpression(assignment_expression -> expression);
             if (assignment_expression -> expression -> Type() == control.long_type)
                 assignment_expression -> expression = ConvertToType(assignment_expression -> expression, control.int_type);
             break;
        case AstAssignmentExpression::AND_EQUAL:
        case AstAssignmentExpression::XOR_EQUAL:
        case AstAssignmentExpression::IOR_EQUAL:
             if (left_type == control.boolean_type)
             {
                 if (right_type != control.boolean_type)
                     ReportSemError(SemanticError::TYPE_NOT_BOOLEAN,
                                    assignment_expression -> expression -> LeftToken(),
                                    assignment_expression -> expression -> RightToken(),
                                    right_type -> Name());
             }
             else
             {
                 if (! control.IsIntegral(left_type))
                     ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                                    left_hand_side -> LeftToken(),
                                    left_hand_side -> RightToken(),
                                    left_type -> Name());
                 if (! control.IsIntegral(right_type))
                     ReportSemError(SemanticError::TYPE_NOT_INTEGRAL,
                                    assignment_expression -> expression -> LeftToken(),
                                    assignment_expression -> expression -> RightToken(),
                                    right_type -> Name());
                 BinaryNumericPromotion(assignment_expression);
             }
             break;
        default:
            assert(false);
            break;
    }
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

