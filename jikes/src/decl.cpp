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
#include "depend.h"
#include "table.h"
#include "tuple.h"
#include "spell.h"

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif

//
// If this compilation unit contains a package declaration, make sure
// the package is not also associated with a type. We used to also
// require that the package exist at compile time, but this was
// changed so that we are compatible with other Java compilers.
//
inline void Semantic::CheckPackage()
{
    if (compilation_unit -> package_declaration_opt)
    {
        //
        // Make sure that the package or any of its parents does not match the
        // name of a type.
        //
        for (PackageSymbol *subpackage = this_package,
                 *package = subpackage -> owner;
             package;
             subpackage = package, package = package -> owner)
        {
            FileSymbol *file_symbol =
                Control::GetFile(control, package, subpackage -> Identity());
            if (file_symbol)
            {
                char *file_name = file_symbol -> FileName();
                int length = file_symbol -> FileNameLength();
                wchar_t *error_name = new wchar_t[length + 1];
                for (int i = 0; i < length; i++)
                    error_name[i] = file_name[i];
                error_name[length] = U_NULL;

                ReportSemError(SemanticError::PACKAGE_TYPE_CONFLICT,
                               compilation_unit -> package_declaration_opt -> name -> LeftToken(),
                               compilation_unit -> package_declaration_opt -> name -> RightToken(),
                               package -> PackageName(),
                               subpackage -> Name(),
                               error_name);
                delete [] error_name;
            }
        }
    }
}


//
// Pass 1: Introduce the main package, the current package and all types
// specified into their proper scope
//
void Semantic::ProcessTypeNames()
{
    import_on_demand_packages.Next() = control.system_package;
    compilation_unit = source_file_symbol -> compilation_unit;

    //
    // If we are supposed to be verbose, report empty declarations...
    //
    if (control.option.pedantic)
    {
        if (compilation_unit -> EmptyCompilationUnitCast())
        {
            ReportSemError(SemanticError::NO_TYPES,
                           compilation_unit -> LeftToken(),
                           compilation_unit -> RightToken());
        }

        for (int i = 0; i < compilation_unit -> NumTypeDeclarations(); i++)
        {
            Ast *type_declaration = compilation_unit -> TypeDeclaration(i);
            if (type_declaration -> EmptyDeclarationCast())
            {
                ReportSemError(SemanticError::EMPTY_DECLARATION,
                               type_declaration -> LeftToken(),
                               type_declaration -> RightToken());
            }
        }
    }

    //
    // If we have a bad compilation unit insert its types as "bad types"
    //
    if (compilation_unit -> BadCompilationUnitCast())
    {
        for (int i = 0; i < lex_stream -> NumTypes(); i++)
        {
            LexStream::TokenIndex identifier_token =
                lex_stream -> Next(lex_stream -> Type(i));
            if (lex_stream -> Kind(identifier_token) == TK_Identifier)
            {
                NameSymbol *name_symbol =
                    lex_stream -> NameSymbol(identifier_token);
                TypeSymbol *type = this_package -> FindTypeSymbol(name_symbol);

                assert(type);

                type -> MarkBad();
                type -> MarkSourceNoLongerPending();
                type -> supertypes_closure = new SymbolSet;
                type -> subtypes = new SymbolSet;
                type -> semantic_environment =
                    new SemanticEnvironment(this, type, NULL);
                if (type != control.Object())
                    type -> super = control.no_type;
                if (! type -> FindConstructorSymbol())
                    AddDefaultConstructor(type);
                source_file_symbol -> types.Next() = type;
            }
        }
        return;
    }

    //
    // Use this tuple to compute the list of valid types encountered in this
    // compilation unit.
    //
    Tuple<TypeSymbol *> type_list;

    //
    // Process each type in this compilation unit, in turn
    //
    for (int k = 0; k < compilation_unit -> NumTypeDeclarations(); k++)
    {
        LexStream::TokenIndex identifier_token = LexStream::BadToken();
        TypeSymbol *type = NULL;

        Ast *type_declaration = compilation_unit -> TypeDeclaration(k);
        switch (type_declaration -> kind)
        {
            case Ast::CLASS:
            {
                AstClassDeclaration *class_declaration =
                    (AstClassDeclaration *) type_declaration;
                identifier_token = class_declaration -> identifier_token;
                NameSymbol *name_symbol =
                    lex_stream -> NameSymbol(identifier_token);

                type = this_package -> FindTypeSymbol(name_symbol);
                if (type)
                {
                    if (! type -> SourcePending())
                    {
                        ReportSemError(SemanticError::DUPLICATE_TYPE_DECLARATION,
                                       identifier_token,
                                       identifier_token,
                                       name_symbol -> Name(),
                                       type -> FileLoc());
                        type = NULL;
                    }
                    else
                    {
                        if (type -> ContainingPackage() ==
                            control.unnamed_package)
                        {
                            TypeSymbol *old_type = (TypeSymbol *) control.
                                unnamed_package_types.Image(name_symbol);
                            if (old_type != type)
                            {
                                ReportSemError(SemanticError::DUPLICATE_TYPE_DECLARATION,
                                               identifier_token,
                                               identifier_token,
                                               name_symbol -> Name(),
                                               old_type -> FileLoc());
                            }
                        }

                        type_list.Next() = type; // Save valid type for later processing. See below

                        type -> MarkSourceNoLongerPending();
                        type -> semantic_environment =
                            new SemanticEnvironment(this, type, NULL);
                        type -> declaration = class_declaration;
                        type -> SetFlags(ProcessClassModifiers(class_declaration));
                        //
                        // Add 3 extra elements for padding. May need a default
                        // constructor and other support elements.
                        //
                        type -> SetSymbolTable(class_declaration -> class_body ->
                                               NumClassBodyDeclarations() + 3);
                        type -> SetLocation();

                        if (lex_stream -> IsDeprecated(lex_stream -> Previous(class_declaration -> LeftToken())))
                            type -> MarkDeprecated();

                        source_file_symbol -> types.Next() = type;
                        class_declaration -> semantic_environment =
                            type -> semantic_environment; // save for processing bodies later.

                        CheckClassMembers(type,
                                          class_declaration -> class_body);
                    }
                }

                break;
            }
            case Ast::INTERFACE:
            {
                AstInterfaceDeclaration *interface_declaration =
                    (AstInterfaceDeclaration *) type_declaration;
                identifier_token = interface_declaration -> identifier_token;
                NameSymbol *name_symbol =
                    lex_stream -> NameSymbol(identifier_token);

                type = this_package -> FindTypeSymbol(name_symbol);
                if (type)
                {
                    if (! type -> SourcePending())
                    {
                        ReportSemError(SemanticError::DUPLICATE_TYPE_DECLARATION,
                                       identifier_token,
                                       identifier_token,
                                       name_symbol -> Name(),
                                       type -> FileLoc());
                        type = NULL;
                    }
                    else
                    {
                        if (type -> ContainingPackage() ==
                            control.unnamed_package)
                        {
                            TypeSymbol *old_type = (TypeSymbol *) control.
                                unnamed_package_types.Image(name_symbol);
                            if (old_type != type)
                            {
                                ReportSemError(SemanticError::DUPLICATE_TYPE_DECLARATION,
                                               identifier_token,
                                               identifier_token,
                                               name_symbol -> Name(),
                                               old_type -> FileLoc());
                            }
                        }

                        type_list.Next() = type; // Save valid type for later processing. See below

                        type -> MarkSourceNoLongerPending();
                        type -> semantic_environment =
                            new SemanticEnvironment(this, type, NULL);
                        type -> declaration = interface_declaration;
                        type -> file_symbol = source_file_symbol;
                        type -> SetFlags(ProcessInterfaceModifiers(interface_declaration));
                        type -> SetSymbolTable(interface_declaration ->
                                               NumInterfaceMemberDeclarations());
                        type -> SetLocation();

                        if (lex_stream -> IsDeprecated(lex_stream -> Previous(interface_declaration -> LeftToken())))
                            type -> MarkDeprecated();

                        source_file_symbol -> types.Next() = type;
                        interface_declaration -> semantic_environment =
                            type -> semantic_environment;

                        CheckInterfaceMembers(type, interface_declaration);
                    }
                }
                break;
            }
            case Ast::EMPTY_DECLARATION:
                break;
            default:
                assert(false);
                break;
        }

        //
        // If we successfully processed this type, check that
        //     . its name does not conflict with a subpackage
        //     . if it is contained in a file with a different name
        //       than its own name that there does not also exist a
        //       (java or class) file with its name.
        //
        if (type)
        {
            NameSymbol *name_symbol =
                lex_stream -> NameSymbol(identifier_token);
            for (int i = 0; i < this_package -> directory.Length(); i++)
            {
                //
                // The unnamed package cannot contain subpackages, as
                // subpackages require a named top-level package. In other
                // words, java.lang is not a subpackage of the unnamed one.
                //
                if ((this_package -> directory[i] ->
                     FindDirectorySymbol(name_symbol)) &&
                    this_package != control.unnamed_package)
                {
                    char *file_name = type -> file_symbol -> FileName();
                    int length = type -> file_symbol -> FileNameLength();
                    wchar_t *error_name = new wchar_t[length + 1];
                    for (int j = 0; j < length; j++)
                        error_name[j] = file_name[j];
                    error_name[length] = U_NULL;

                    ReportSemError(SemanticError::PACKAGE_TYPE_CONFLICT,
                                   identifier_token,
                                   identifier_token,
                                   this_package -> PackageName(),
                                   name_symbol -> Name(),
                                   error_name);

                    delete [] error_name;
                }
            }

            if (type -> Identity() != source_file_symbol -> Identity())
            {
                PackageSymbol *package = this_package;
                FileSymbol *file_symbol =
                    Control::GetJavaFile(package, type -> Identity());

                if (file_symbol)
                {
                    ReportSemError(SemanticError::TYPE_IN_MULTIPLE_FILES,
                                   identifier_token,
                                   identifier_token,
                                   this_package -> PackageName(),
                                   source_file_symbol -> Name(),
                                   package -> PackageName(),
                                   type -> Name());
                }
            }
        }
    }

    CheckPackage();
    ProcessImports();

    //
    // Make sure that compilation unit contains exactly one public type, and
    // that it matches the file name.
    //
    TypeSymbol *public_type = NULL;
    for (int i = 0; i < type_list.Length(); i++)
    {
        TypeSymbol *type = type_list[i];

        AstClassDeclaration *class_declaration =
            type -> declaration -> ClassDeclarationCast();
        AstInterfaceDeclaration *interface_declaration =
            type -> declaration -> InterfaceDeclarationCast();

        if (type && type -> ACC_PUBLIC())
        {
            if (! public_type)
            {
                public_type = type;

                if (source_file_symbol -> Identity() !=
                    public_type -> Identity())
                {
                    if (class_declaration)
                    {
                        ReportSemError(SemanticError::MISMATCHED_TYPE_AND_FILE_NAMES,
                                       class_declaration -> identifier_token,
                                       class_declaration -> identifier_token,
                                       public_type -> Name());
                    }
                    else
                    {
                        ReportSemError(SemanticError::MISMATCHED_TYPE_AND_FILE_NAMES,
                                       interface_declaration -> identifier_token,
                                       interface_declaration -> identifier_token,
                                       public_type -> Name());
                    }
                }
            }
            else
            {
                if (class_declaration)
                {
                    ReportSemError(SemanticError::MULTIPLE_PUBLIC_TYPES,
                                   class_declaration -> identifier_token,
                                   class_declaration -> identifier_token,
                                   type -> Name(),
                                   public_type -> Name());
                }
                else
                {
                    ReportSemError(SemanticError::MULTIPLE_PUBLIC_TYPES,
                                   interface_declaration -> identifier_token,
                                   interface_declaration -> identifier_token,
                                   type -> Name(),
                                   public_type -> Name());
                }
            }
        }
    }
}


void Semantic::CheckClassMembers(TypeSymbol *containing_type,
                                 AstClassBody *class_body)
{
    for (int i = 0; i < class_body -> NumNestedClasses(); i++)
    {
        AstClassDeclaration *class_declaration = class_body -> NestedClass(i);
        ProcessNestedClassName(containing_type, class_declaration);
    }

    for (int j = 0; j < class_body -> NumNestedInterfaces(); j++)
    {
        AstInterfaceDeclaration *interface_declaration =
            class_body -> NestedInterface(j);
        ProcessNestedInterfaceName(containing_type, interface_declaration);
    }

    for (int l = 0; l < class_body -> NumEmptyDeclarations(); l++)
    {
        if (control.option.pedantic)
        {
            ReportSemError(SemanticError::EMPTY_DECLARATION,
                           class_body -> EmptyDeclaration(l) -> LeftToken(),
                           class_body -> EmptyDeclaration(l) -> RightToken());
        }
    }
}


//
// Given a type shadow symbol, returns the first accessible type, and reports
// an error for any other accessible types.
//
inline TypeSymbol *Semantic::FindTypeInShadow(TypeShadowSymbol *type_shadow_symbol,
                                              LexStream::TokenIndex identifier_token)
{
    TypeSymbol *type = type_shadow_symbol -> type_symbol;
    int i = 0;
    if (! TypeAccessCheck(type))
    {
        if (type_shadow_symbol -> NumConflicts())
        {
            //
            // The conflicts are necessarily accessible, because they are
            // public types inherited from interfaces.
            //
            type = type_shadow_symbol -> Conflict(0);
            i = 1;
        }
        else type = NULL;
    }
    for ( ; i < type_shadow_symbol -> NumConflicts(); i++)
    {
        ReportSemError(SemanticError::AMBIGUOUS_TYPE,
                       identifier_token,
                       identifier_token,
                       type -> Name(),
                       type -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                       type -> owner -> TypeCast() -> ExternalName(),
                       type_shadow_symbol -> Conflict(i) -> owner -> TypeCast() -> ContainingPackage() -> PackageName(),
                       type_shadow_symbol -> Conflict(i) -> owner -> TypeCast() -> ExternalName());
    }

    return type;
}


//
// Look for a type within an environment stack, without regard to inheritance.
//
TypeSymbol *Semantic::FindTypeInEnvironment(SemanticEnvironment *env_stack,
                                            NameSymbol *name_symbol)
{
    for (SemanticEnvironment *env = env_stack; env; env = env -> previous)
    {
        TypeSymbol *type = env -> symbol_table.FindTypeSymbol(name_symbol);
        if (type)
            return type;
        type = env -> Type() -> FindTypeSymbol(name_symbol);
        if (type)
            return type;
        if (name_symbol == env -> Type() -> Identity())
            return env -> Type();
    }

    return (TypeSymbol *) NULL;
}


void Semantic::CheckNestedTypeDuplication(SemanticEnvironment *env,
                                          LexStream::TokenIndex identifier_token)
{
    NameSymbol *name_symbol = lex_stream -> NameSymbol(identifier_token);

    //
    // First check to see if we have a duplication at the same level...
    //
    TypeSymbol *old_type = NULL;
    if (env -> symbol_table.Size() > 0)
    {
        for (int i = env -> symbol_table.Size(); --i >= 0; )
        {
            old_type = env -> symbol_table[i] -> FindTypeSymbol(name_symbol);
            if (old_type)
                break;
        }
    }
    else old_type = env -> Type() -> FindTypeSymbol(name_symbol);
    if (old_type)
    {
        ReportSemError(SemanticError::DUPLICATE_TYPE_DECLARATION,
                       identifier_token,
                       identifier_token,
                       name_symbol -> Name(),
                       old_type -> FileLoc());
    }
    else
    {
        //
        // ... Then check the enclosing environments...
        //
        for (; env; env = env -> previous)
        {
            if (env -> Type() -> Identity() == name_symbol)
            {
                ReportSemError(SemanticError::DUPLICATE_INNER_TYPE_NAME,
                               identifier_token,
                               identifier_token,
                               name_symbol -> Name(),
                               env -> Type() -> FileLoc());
                break;
            }
        }
    }
}


TypeSymbol *Semantic::ProcessNestedClassName(TypeSymbol *containing_type,
                                             AstClassDeclaration *class_declaration)
{
    CheckNestedTypeDuplication(containing_type -> semantic_environment,
                               class_declaration -> identifier_token);

    NameSymbol *name_symbol =
        lex_stream -> NameSymbol(class_declaration -> identifier_token);
    TypeSymbol *outermost_type = containing_type -> outermost_type;

    int length = containing_type -> ExternalNameLength() + 1 +
        name_symbol -> NameLength(); // +1 for $,... +1 for $
    wchar_t *external_name = new wchar_t[length + 1]; // +1 for '\0';
    wcscpy(external_name, containing_type -> ExternalName());
    wcscat(external_name, StringConstant::US_DS);
    wcscat(external_name, name_symbol -> Name());

    TypeSymbol *inner_type =
        containing_type -> InsertNestedTypeSymbol(name_symbol);
    inner_type -> outermost_type = outermost_type;
    inner_type -> supertypes_closure = new SymbolSet;
    inner_type -> subtypes = new SymbolSet;
    inner_type -> SetExternalIdentity(control.FindOrInsertName(external_name,
                                                               length));
    inner_type -> semantic_environment =
        new SemanticEnvironment(this, inner_type,
                                containing_type -> semantic_environment);
    inner_type -> declaration = class_declaration;
    inner_type -> file_symbol = source_file_symbol;
    inner_type -> SetFlags(containing_type -> ACC_INTERFACE()
                           ? ProcessStaticNestedClassModifiers(class_declaration)
                           : ProcessNestedClassModifiers(class_declaration));
    inner_type -> SetOwner(containing_type);
    //
    // Add 3 extra elements for padding. May need a default constructor and
    // other support elements.
    //
    inner_type -> SetSymbolTable(class_declaration -> class_body ->
                                 NumClassBodyDeclarations() + 3);
    inner_type -> SetLocation();
    inner_type -> SetSignature(control);

    if (lex_stream -> IsDeprecated(lex_stream -> Previous(class_declaration -> LeftToken())))
        inner_type -> MarkDeprecated();

    //
    // If not a top-level type, then add pointer to enclosing type.
    //
    if (! inner_type -> ACC_STATIC())
        inner_type -> InsertThis0();
    else if (containing_type -> IsInner())
    {
        ReportSemError(SemanticError::STATIC_TYPE_IN_INNER_CLASS,
                       class_declaration -> identifier_token,
                       class_declaration -> identifier_token,
                       lex_stream -> NameString(class_declaration -> identifier_token),
                       containing_type -> Name(),
                       containing_type -> FileLoc());
        // Change its status so we can continue compiling...
        inner_type -> ResetACC_STATIC();
    }

    if (inner_type -> IsLocal())
    {
        if (! outermost_type -> local)
            outermost_type -> local = new SymbolSet;
        outermost_type -> local -> AddElement(inner_type);
    }
    else
    {
        if (! outermost_type -> non_local)
            outermost_type -> non_local = new SymbolSet;
        outermost_type -> non_local -> AddElement(inner_type);
    }

    class_declaration -> semantic_environment =
        inner_type -> semantic_environment; // save for processing bodies later.

    CheckClassMembers(inner_type, class_declaration -> class_body);

    delete [] external_name;

    return inner_type;
}


void Semantic::CheckInterfaceMembers(TypeSymbol *containing_type,
                                     AstInterfaceDeclaration *interface_declaration)
{
    for (int i = 0; i < interface_declaration -> NumNestedClasses(); i++)
    {
        AstClassDeclaration *class_declaration =
            interface_declaration -> NestedClass(i);
        ProcessNestedClassName(containing_type, class_declaration);
    }

    for (int j = 0; j < interface_declaration -> NumNestedInterfaces(); j++)
    {
        AstInterfaceDeclaration *inner_interface_declaration =
            interface_declaration -> NestedInterface(j);
        ProcessNestedInterfaceName(containing_type,
                                   inner_interface_declaration);
    }

    for (int l = 0; l < interface_declaration -> NumEmptyDeclarations(); l++)
    {
        if (control.option.pedantic)
        {
            ReportSemError(SemanticError::EMPTY_DECLARATION,
                           interface_declaration -> EmptyDeclaration(l) -> LeftToken(),
                           interface_declaration -> EmptyDeclaration(l) -> RightToken());
        }
    }
}


TypeSymbol *Semantic::ProcessNestedInterfaceName(TypeSymbol *containing_type,
                                                 AstInterfaceDeclaration *interface_declaration)
{
    CheckNestedTypeDuplication(containing_type -> semantic_environment,
                               interface_declaration -> identifier_token);

    NameSymbol *name_symbol =
        lex_stream -> NameSymbol(interface_declaration -> identifier_token);
    TypeSymbol *outermost_type = containing_type -> outermost_type;

    int length = containing_type -> ExternalNameLength() + 1 +
        name_symbol -> NameLength(); // +1 for $,... +1 for $
    wchar_t *external_name = new wchar_t[length + 1]; // +1 for '\0';
    wcscpy(external_name, containing_type -> ExternalName());
    wcscat(external_name, StringConstant::US_DS);
    wcscat(external_name, name_symbol -> Name());

    TypeSymbol *inner_type =
        containing_type -> InsertNestedTypeSymbol(name_symbol);
    inner_type -> outermost_type = outermost_type;
    inner_type -> supertypes_closure = new SymbolSet;
    inner_type -> subtypes = new SymbolSet;
    inner_type -> SetExternalIdentity(control.FindOrInsertName(external_name,
                                                               length));
    inner_type -> semantic_environment =
        new SemanticEnvironment(this, inner_type,
                                containing_type -> semantic_environment);
    inner_type -> declaration = interface_declaration;
    inner_type -> file_symbol = source_file_symbol;

    inner_type -> SetFlags(containing_type -> ACC_INTERFACE()
                           ? ProcessStaticNestedInterfaceModifiers(interface_declaration)
                           : ProcessNestedInterfaceModifiers(interface_declaration));

    inner_type -> SetOwner(containing_type);
    inner_type -> SetSymbolTable(interface_declaration -> NumInterfaceMemberDeclarations());
    inner_type -> SetLocation();
    inner_type -> SetSignature(control);

    if (lex_stream -> IsDeprecated(lex_stream -> Previous(interface_declaration -> LeftToken())))
        inner_type -> MarkDeprecated();

    if (inner_type -> IsLocal())
    {
        if (! outermost_type -> local)
            outermost_type -> local = new SymbolSet;
        outermost_type -> local -> AddElement(inner_type);
    }
    else
    {
        if (! outermost_type -> non_local)
            outermost_type -> non_local = new SymbolSet;
        outermost_type -> non_local -> AddElement(inner_type);
    }

    interface_declaration -> semantic_environment =
        inner_type -> semantic_environment; // save for processing bodies later.

    CheckInterfaceMembers(inner_type, interface_declaration);

    delete [] external_name;

    return inner_type;
}


//
// Pass 1.2: Process all import statements
//
void Semantic::ProcessImports()
{
    for (int i = 0; i < compilation_unit -> NumImportDeclarations(); i++)
    {
        AstImportDeclaration *import_declaration =
            compilation_unit -> ImportDeclaration(i);

        if (import_declaration -> star_token_opt)
            ProcessTypeImportOnDemandDeclaration(import_declaration);
        else ProcessSingleTypeImportDeclaration(import_declaration);
    }
}


//
// Pass 2: Process "extends" and "implements" clauses associated with the
// types.
//
void Semantic::ProcessTypeHeader(AstClassDeclaration *class_declaration)
{
    TypeSymbol *type = class_declaration -> semantic_environment -> Type();

    assert(! type -> HeaderProcessed() || type -> Bad());
    type -> MarkHeaderProcessed();
    if (type -> Bad())
        return;

    //
    // Special case java.lang.Object, the only class with no supertype.
    //
    if (type -> Identity() == control.object_name_symbol &&
        this_package == control.system_package && ! type -> IsNested())
    {
        if (class_declaration -> super_opt ||
            class_declaration -> NumInterfaces() > 0)
        {
            ReportSemError(SemanticError::OBJECT_WITH_SUPER_TYPE,
                           class_declaration -> LeftToken(),
                           (class_declaration -> class_body ->
                            left_brace_token - 1));
            type -> MarkBad();
        }
        type -> MarkHeaderProcessed();
        return;
    }

    if (class_declaration -> super_opt)
    {
        TypeSymbol *super_type = MustFindType(class_declaration -> super_opt);
        assert(! super_type -> SourcePending());
        if (! super_type -> HeaderProcessed())
        {
            AstClassDeclaration *class_decl =
                super_type -> declaration -> ClassDeclarationCast();
            AstInterfaceDeclaration *interface_decl =
                super_type -> declaration -> InterfaceDeclarationCast();
            Semantic *sem = super_type -> semantic_environment -> sem;
            if (class_decl)
                sem -> ProcessTypeHeaders(class_decl);
            else if (interface_decl)
                sem -> ProcessTypeHeaders(interface_decl);
            else assert(false && "supertype not processed");
        }
        if (control.option.deprecation && state_stack.Size() == 0 &&
            super_type -> IsDeprecated() && ! type -> IsDeprecated())
        {
            ReportSemError(SemanticError::DEPRECATED_TYPE,
                           class_declaration -> super_opt -> LeftToken(),
                           class_declaration -> super_opt -> RightToken(),
                           super_type -> ContainingPackage() -> PackageName(),
                           super_type -> ExternalName());
        }
        if (super_type -> ACC_INTERFACE())
        {
            ReportSemError(SemanticError::NOT_A_CLASS,
                           class_declaration -> super_opt -> LeftToken(),
                           class_declaration -> super_opt -> RightToken(),
                           super_type -> ContainingPackage() -> PackageName(),
                           super_type -> ExternalName());
        }
        else if (super_type -> ACC_FINAL())
        {
            ReportSemError(SemanticError::SUPER_IS_FINAL,
                           class_declaration -> super_opt -> LeftToken(),
                           class_declaration -> super_opt -> RightToken(),
                           super_type -> ContainingPackage() -> PackageName(),
                           super_type -> ExternalName());
        }
        else if (super_type -> Bad())
            ; // ignore
        else
        {
            super_type -> subtypes -> AddElement(type);
            type -> super = super_type;
            while (super_type)
            {
                type -> supertypes_closure -> AddElement(super_type);
                type -> supertypes_closure -> Union(*super_type ->
                                                    supertypes_closure);
                if (super_type -> owner -> TypeCast())
                    super_type = super_type -> ContainingType();
                else super_type = NULL;
            }
        }
    }
    if (! type -> super)
    {
        type -> super = control.Object();
        type -> supertypes_closure -> AddElement(control.Object());
        control.Object() -> subtypes -> AddElement(type);
    }
    AddDependence(type, type -> super);

    for (int i = 0; i < class_declaration -> NumInterfaces(); i++)
        ProcessInterface(type, class_declaration -> Interface(i));

    // if there is a cycle, break it and issue an error message
    if (type -> supertypes_closure -> IsElement(type))
    {
        type -> super = control.Object();
        type -> ResetInterfaces();
        type -> MarkCircular();
        ReportSemError(SemanticError::CIRCULAR_CLASS,
                       class_declaration -> identifier_token,
                       class_declaration -> class_body -> LeftToken() - 1,
                       type -> ContainingPackage() -> PackageName(),
                       type -> ExternalName());
    }
}


void Semantic::ProcessTypeHeader(AstInterfaceDeclaration *interface_declaration)
{
    TypeSymbol *type = interface_declaration -> semantic_environment -> Type();

    assert(! type -> HeaderProcessed() || type -> Bad());
    type -> MarkHeaderProcessed();

    //
    // Although interfaces do not have a superclass in source code, in
    // bytecode they are treated as subclasses of Object.
    //
    type -> super = control.Object();
    AddDependence(type, control.Object());
    for (int k = 0; k < interface_declaration -> NumExtendsInterfaces(); k++)
        ProcessInterface(type, interface_declaration -> ExtendsInterface(k));

    if (type -> supertypes_closure -> IsElement(type))
    {
        //
        // Remove all the interfaces if a loop is detected.
        //
        type -> ResetInterfaces();
        type -> MarkCircular();
        ReportSemError(SemanticError::CIRCULAR_INTERFACE,
                       interface_declaration -> identifier_token,
                       interface_declaration -> left_brace_token - 1,
                       type -> ContainingPackage() -> PackageName(),
                       type -> ExternalName());
    }
}


void Semantic::ProcessInterface(TypeSymbol *base_type, AstExpression *name)
{
    TypeSymbol *interf = MustFindType(name);

    assert(! interf -> SourcePending());
    if (! interf -> HeaderProcessed())
    {
        AstClassDeclaration *class_decl =
            interf -> declaration -> ClassDeclarationCast();
        AstInterfaceDeclaration *interface_decl =
            interf -> declaration -> InterfaceDeclarationCast();
        Semantic *sem = interf -> semantic_environment -> sem;
        if (class_decl)
            sem -> ProcessTypeHeaders(class_decl);
        else if (interface_decl)
            sem -> ProcessTypeHeaders(interface_decl);
        else assert(false && "supertype not processed");
    }

    if (control.option.deprecation && state_stack.Size() == 0 &&
        interf -> IsDeprecated() && ! base_type -> IsDeprecated())
    {
        ReportSemError(SemanticError::DEPRECATED_TYPE,
                       name -> LeftToken(),
                       name -> RightToken(),
                       interf -> ContainingPackage() -> PackageName(),
                       interf -> ExternalName());
    }
    if (! interf -> ACC_INTERFACE())
    {
        if (! interf -> Bad())
        {
            ReportSemError(SemanticError::NOT_AN_INTERFACE,
                           name -> LeftToken(),
                           name -> RightToken(),
                           interf -> ContainingPackage() -> PackageName(),
                           interf -> ExternalName());
        }

        name -> symbol = NULL;
    }
    else
    {
        for (int k = 0; k < base_type -> NumInterfaces(); k++)
        {
            if (base_type -> Interface(k) == interf)
            {
                ReportSemError(SemanticError::DUPLICATE_INTERFACE,
                               name -> LeftToken(),
                               name -> RightToken(),
                               interf -> ContainingPackage() -> PackageName(),
                               interf -> ExternalName(),
                               base_type -> ExternalName());

                name -> symbol = NULL;

                return;
            }
        }
        name -> symbol = interf; // save type name in ast.
        base_type -> AddInterface(interf);
        interf -> subtypes -> AddElement(base_type);
        AddDependence(base_type, interf);
        while (interf)
        {
            base_type -> supertypes_closure -> AddElement(interf);
            base_type -> supertypes_closure -> Union(*interf ->
                                                     supertypes_closure);
            if (interf -> owner -> TypeCast())
                interf = interf -> ContainingType();
            else interf = NULL;
        }
    }
}


void Semantic::ProcessTypeHeaders(AstClassDeclaration *class_declaration)
{
    TypeSymbol *type = class_declaration -> semantic_environment -> Type();

    if (type -> HeaderProcessed())
        return; // Possible if a subclass was declared in the same file.
    ProcessTypeHeader(class_declaration);
    state_stack.Push(class_declaration -> semantic_environment);
    AstClassBody *class_body = class_declaration -> class_body;
    for (int i = 0; i < class_body -> NumNestedClasses(); i++)
    {
        AstClassDeclaration *nested_class = class_body -> NestedClass(i);
        ProcessTypeHeaders(nested_class);
        type -> AddNestedType(nested_class -> semantic_environment -> Type());
    }
    for (int j = 0; j < class_body -> NumNestedInterfaces(); j++)
    {
        AstInterfaceDeclaration *nested_interface =
            class_body -> NestedInterface(j);
        ProcessTypeHeaders(nested_interface);
        type -> AddNestedType(nested_interface -> semantic_environment ->
                              Type());
    }
    state_stack.Pop();
}


void Semantic::ProcessTypeHeaders(AstInterfaceDeclaration *interface_declaration)
{
    TypeSymbol *type = interface_declaration -> semantic_environment -> Type();

    if (type -> HeaderProcessed())
        return; // Possible if a subclass was declared in the same file.
    ProcessTypeHeader(interface_declaration);
    state_stack.Push(interface_declaration -> semantic_environment);
    for (int i = 0; i < interface_declaration -> NumNestedClasses(); i++)
    {
        AstClassDeclaration *nested_class =
            interface_declaration -> NestedClass(i);
        ProcessTypeHeaders(nested_class);
        type -> AddNestedType(nested_class -> semantic_environment -> Type());
    }
    for (int j = 0; j < interface_declaration -> NumNestedInterfaces(); j++)
    {
        AstInterfaceDeclaration *nested_interface =
            interface_declaration -> NestedInterface(j);
        ProcessTypeHeaders(nested_interface);
        type -> AddNestedType(nested_interface -> semantic_environment ->
                              Type());
    }
    state_stack.Pop();
}


void Semantic::ProcessTypeHeaders(TypeSymbol *anon_type, AstClassBody *body)
{
    anon_type -> MarkHeaderProcessed();
    state_stack.Push(anon_type -> semantic_environment);
    for (int i = 0; i < body -> NumNestedClasses(); i++)
    {
        AstClassDeclaration *nested_class = body -> NestedClass(i);
        ProcessTypeHeaders(nested_class);
        anon_type -> AddNestedType(nested_class -> semantic_environment ->
                                   Type());
    }
    for (int j = 0; j < body -> NumNestedInterfaces(); j++)
    {
        AstInterfaceDeclaration *nested_interface = body -> NestedInterface(j);
        ProcessTypeHeaders(nested_interface);
        anon_type -> AddNestedType(nested_interface -> semantic_environment ->
                              Type());
    }
    state_stack.Pop();
}


void Semantic::ReportTypeInaccessible(LexStream::TokenIndex left_tok,
                                      LexStream::TokenIndex right_tok,
                                      TypeSymbol *type)
{
    ReportSemError(SemanticError::TYPE_NOT_ACCESSIBLE,
                   left_tok,
                   right_tok,
                   type -> ContainingPackage() -> PackageName(),
                   type -> ExternalName(),
                   type -> AccessString());
}


//
// Finds an accessible member type named identifier_token within type, or
// returns NULL. Issues an error if there are multiple ambiguous types. The
// caller is responsible for searching for inaccessible member types.
//
TypeSymbol *Semantic::FindNestedType(TypeSymbol *type,
                                     LexStream::TokenIndex identifier_token)
{
    if (type == control.null_type || type -> Bad() || type -> Primitive())
    {
        return NULL;
    }

    NameSymbol *name_symbol = lex_stream -> NameSymbol(identifier_token);

    if (! type -> expanded_type_table)
        ComputeTypesClosure(type, identifier_token);
    TypeShadowSymbol *type_shadow_symbol =
        type -> expanded_type_table -> FindTypeShadowSymbol(name_symbol);

    return (type_shadow_symbol
            ? FindTypeInShadow(type_shadow_symbol, identifier_token)
            : (TypeSymbol *) NULL);
}


//
// Finds a nested type named name within the enclosing type, and establishes
// a dependence relation. This also searches for inaccessible types, and
// reports an error before returning the inaccessible type. For any other error,
// the return is control.no_type.
//
TypeSymbol *Semantic::MustFindNestedType(TypeSymbol *type, Ast *name)
{
    LexStream::TokenIndex identifier_token = name -> RightToken();
    TypeSymbol *inner_type = FindNestedType(type, identifier_token);
    if (! inner_type)
    {
        //
        // Before failing completely, check whether or not the user is trying
        // to access an inaccessible nested type.
        //
        NameSymbol *name_symbol = lex_stream -> NameSymbol(identifier_token);
        for (TypeSymbol *super_type = type -> super;
             super_type && ! super_type -> Bad();
             super_type = super_type -> super)
        {
            assert(super_type -> expanded_type_table);

            TypeShadowSymbol *type_shadow_symbol = super_type ->
                expanded_type_table -> FindTypeShadowSymbol(name_symbol);
            if (type_shadow_symbol)
            {
                inner_type = FindTypeInShadow(type_shadow_symbol,
                                              identifier_token);
                break;
            }
        }

        if (inner_type)
            ReportTypeInaccessible(name -> LeftToken(),
                                   name -> RightToken(), inner_type);
        else inner_type = GetBadNestedType(type, identifier_token);
    }

    return inner_type -> Bad() ? control.no_type : inner_type;
}


//
// Pass 3: Process all method and constructor declarations within the
// compilation unit so that any field initialization enclosed in the
// compilation unit can invoke any constructor or method within the unit.
//
inline void Semantic::ProcessConstructorMembers(AstClassBody *class_body)
{
    TypeSymbol *this_type = ThisType();

    assert(this_type -> HeaderProcessed());

    //
    // If the class contains no constructor, ...
    //
    if (class_body -> NumConstructors() > 0)
    {
        for (int k = 0; k < class_body -> NumConstructors(); k++)
            ProcessConstructorDeclaration(class_body -> Constructor(k));
    }
    else if (! this_type -> Anonymous())
        AddDefaultConstructor(this_type);

    this_type -> MarkConstructorMembersProcessed();
}


inline void Semantic::ProcessMethodMembers(AstClassBody *class_body)
{
    assert(ThisType() -> HeaderProcessed());

    for (int k = 0; k < class_body -> NumMethods(); k++)
        ProcessMethodDeclaration(class_body -> Method(k));

    ThisType() -> MarkMethodMembersProcessed();
}


inline void Semantic::ProcessFieldMembers(AstClassBody *class_body)
{
    assert(ThisType() -> HeaderProcessed());

    for (int i = 0; i < class_body -> NumInstanceVariables(); i++)
        ProcessFieldDeclaration(class_body -> InstanceVariable(i));

    for (int k = 0; k < class_body -> NumClassVariables(); k++)
        ProcessFieldDeclaration(class_body -> ClassVariable(k));

    ThisType() -> MarkFieldMembersProcessed();
}


void Semantic::ProcessMembers(SemanticEnvironment *environment,
                              AstClassBody *class_body)
{
    state_stack.Push(environment);
    TypeSymbol *this_type = ThisType();

    assert(! this_type -> ConstructorMembersProcessed() || this_type -> Bad());
    assert(! this_type -> MethodMembersProcessed() || this_type -> Bad());
    assert(! this_type -> FieldMembersProcessed() || this_type -> Bad());

    ProcessConstructorMembers(class_body);
    ProcessMethodMembers(class_body);
    ProcessFieldMembers(class_body);

    delete this_type -> innertypes_closure; // save some space !!!
    this_type -> innertypes_closure = NULL;

    if (this_type -> IsInner())
    {
        for (int i = 0; i < class_body -> NumStaticInitializers(); i++)
        {
            ReportSemError(SemanticError::STATIC_INITIALIZER_IN_INNER_CLASS,
                           class_body -> StaticInitializer(i) -> LeftToken(),
                           class_body -> StaticInitializer(i) -> RightToken(),
                           this_type -> Name(),
                           this_type -> FileLoc());
        }
    }

    for (int i = 0; i < this_type -> NumNestedTypes(); i++)
    {
        TypeSymbol *inner_type = this_type -> NestedType(i);

        if (inner_type -> ACC_INTERFACE())
        {
            AstInterfaceDeclaration *interface_declaration =
                (AstInterfaceDeclaration *) inner_type -> declaration;

            ProcessMembers(interface_declaration);

            if (this_type -> IsInner() &&
                interface_declaration -> semantic_environment)
            {
                //
                // Nested interfaces are implicitly static
                //
                ReportSemError(SemanticError::STATIC_TYPE_IN_INNER_CLASS,
                               interface_declaration -> identifier_token,
                               interface_declaration -> identifier_token,
                               lex_stream -> NameString(interface_declaration -> identifier_token),
                               this_type -> Name(),
                               this_type -> FileLoc());
            }
        }
        else
        {
            AstClassDeclaration *class_declaration =
                (AstClassDeclaration *) inner_type -> declaration;

            ProcessMembers(class_declaration -> semantic_environment,
                           class_declaration -> class_body);
        }
    }

    state_stack.Pop();
}


inline void Semantic::ProcessMethodMembers(AstInterfaceDeclaration *interface_declaration)
{
    assert(ThisType() -> HeaderProcessed());

    for (int k = 0; k < interface_declaration -> NumMethods(); k++)
        ProcessMethodDeclaration(interface_declaration -> Method(k));

    ThisType() -> MarkMethodMembersProcessed();
}


inline void Semantic::ProcessFieldMembers(AstInterfaceDeclaration *interface_declaration)
{
    assert(ThisType() -> HeaderProcessed());

    for (int k = 0; k < interface_declaration -> NumClassVariables(); k++)
        ProcessFieldDeclaration(interface_declaration -> ClassVariable(k));

    ThisType() -> MarkFieldMembersProcessed();
}


void Semantic::ProcessMembers(AstInterfaceDeclaration *interface_declaration)
{
    state_stack.Push(interface_declaration -> semantic_environment);
    TypeSymbol *this_type = ThisType();

    assert(! this_type -> MethodMembersProcessed() || this_type -> Bad());
    assert(! this_type -> FieldMembersProcessed() || this_type -> Bad());

    ProcessMethodMembers(interface_declaration);
    ProcessFieldMembers(interface_declaration);

    delete this_type -> innertypes_closure; // save some space !!!
    this_type -> innertypes_closure = NULL;

    for (int i = 0; i < this_type -> NumNestedTypes(); i++)
    {
        TypeSymbol *inner_type = this_type -> NestedType(i);

        if (inner_type -> ACC_INTERFACE())
        {
            AstInterfaceDeclaration *interface_declaration =
                (AstInterfaceDeclaration *) inner_type -> declaration;
            ProcessMembers(interface_declaration);
        }
        else
        {
            AstClassDeclaration *class_declaration =
                (AstClassDeclaration *) inner_type -> declaration;
            ProcessMembers(class_declaration -> semantic_environment,
                           class_declaration -> class_body);
        }
    }

    state_stack.Pop();
}


//
// Pass 4: Process the field declarations at the top level of the types
//
void Semantic::CompleteSymbolTable(SemanticEnvironment *environment,
                                   LexStream::TokenIndex identifier_token,
                                   AstClassBody *class_body)
{
    if (compilation_unit -> BadCompilationUnitCast())
        return;

    state_stack.Push(environment);
    TypeSymbol *this_type = ThisType();

    assert(this_type -> ConstructorMembersProcessed());
    assert(this_type -> MethodMembersProcessed());
    assert(this_type -> FieldMembersProcessed());

    if (! this_type -> expanded_method_table)
        ComputeMethodsClosure(this_type, identifier_token);

    if (this_type -> super && ! this_type -> Bad())
    {
        if (! this_type -> ACC_ABSTRACT())
        {
            //
            // We already checked that this class does not declare abstract
            // methods. Now see that there are no unimplemented abstract
            // methods in any of the superclasses or superinterfaces. Exploit
            // the fact that the method table is built with the first element
            // being from a superclass; all conflicts are inherited from
            // interfaces and are necessarily abstract.
            //
            ExpandedMethodTable *expanded_table =
                this_type -> expanded_method_table;
            for (int i = 0; i < expanded_table -> symbol_pool.Length(); i++)
            {
                MethodSymbol *method =
                    expanded_table -> symbol_pool[i] -> method_symbol;

                if (method -> ACC_ABSTRACT())
                {
                    TypeSymbol *containing_type = method -> containing_type;
                    if (containing_type != this_type)
                    {
                        if (! method -> IsTyped())
                            method -> ProcessMethodSignature(this,
                                                             identifier_token);

                        ReportSemError(SemanticError::NON_ABSTRACT_TYPE_INHERITS_ABSTRACT_METHOD,
                                       identifier_token,
                                       identifier_token,
                                       method -> Header(),
                                       containing_type -> ContainingPackage() -> PackageName(),
                                       containing_type -> ExternalName(),
                                       this_type -> ContainingPackage() -> PackageName(),
                                       this_type -> ExternalName());
                    }
                }
            }
        }

        //
        // If any superclass of this_type is abstract and is contained in a
        // different package, check to see if its members include abstract
        // methods with default access. If so, this class must be abstract,
        // as it cannot override them. And if this class has a protected or
        // public method with a conflicting signature, then it is cannot be
        // implemented. However, this can be tricky: suppose abstract p1.A
        // declares abstract foo(), abstract p2.B extends p1.A, abstract p1.C
        // extends p2.B and implements foo(). Then, p2.B does not inherit foo()
        // and thus neither does p1.C, but p1.C DOES override foo() with a
        // valid implementation. And thus, p2.D extends p1.C may be concrete.
        //
        PackageSymbol *package = this_type -> ContainingPackage();
        for (TypeSymbol *super_type = this_type -> super;
             super_type && super_type -> ACC_ABSTRACT();
             super_type = super_type -> super)
        {
            if (super_type -> ContainingPackage() == package)
                continue;

            package = super_type -> ContainingPackage();
            ExpandedMethodTable *super_expanded_table =
                super_type -> expanded_method_table;
            for (int i = 0;
                 i < super_expanded_table -> symbol_pool.Length(); i++)
            {
                MethodSymbol *method =
                    super_expanded_table -> symbol_pool[i] -> method_symbol;

                //
                // Remember that abstract methods cannot be private, and that
                // non-default methods were inherited.
                //
                if (! method -> ACC_ABSTRACT() || method -> ACC_PUBLIC() ||
                    method -> ACC_PROTECTED())
                {
                    continue;
                }
                TypeSymbol *containing_type = method -> containing_type;
                if (! method -> IsTyped())
                    method -> ProcessMethodSignature(this, identifier_token);

                //
                // Search all intermediate superclasses in the same package
                // as the current super_class for an override of the abstract
                // method in question. Also report any protected or public
                // methods outside super's package that cause this class
                // to be uninstantiable.
                //
                TypeSymbol *intermediate;
                MethodSymbol *method_clash = NULL;
                for (intermediate = this_type;
                     intermediate != super_type;
                     intermediate = intermediate -> super)
                {
                    MethodShadowSymbol *shadow = intermediate ->
                        expanded_method_table ->
                        FindOverloadMethodShadow(method, this,
                                                 identifier_token);
                    if (! shadow)
                        continue;
                    if (intermediate -> ContainingPackage() != package)
                    {
                        if ((shadow -> method_symbol -> ACC_PUBLIC() ||
                             shadow -> method_symbol -> ACC_PROTECTED()) &&
                            (shadow -> method_symbol -> Type() !=
                             method -> Type()))
                        {
                            //
                            // No need to repeat the warning for subclasses of
                            // where the problem originally occurred.
                            //
                            if (method_clash == shadow -> method_symbol)
                                method_clash = NULL;
                            else method_clash = shadow -> method_symbol;
                        }
                    }
                    else if (shadow -> method_symbol -> containing_type ==
                             intermediate)
                    {
                        break;
                    }
                }

                if (intermediate == super_type && ! this_type -> ACC_ABSTRACT())
                {
                    ReportSemError((this_type -> Anonymous()
                                    ? SemanticError::ANONYMOUS_TYPE_CANNOT_OVERRIDE_DEFAULT_ABSTRACT_METHOD
                                    : SemanticError::NON_ABSTRACT_TYPE_CANNOT_OVERRIDE_DEFAULT_ABSTRACT_METHOD),
                                   identifier_token,
                                   identifier_token,
                                   method -> Header(),
                                   containing_type -> ContainingPackage() -> PackageName(),
                                   containing_type -> ExternalName(),
                                   this_type -> ContainingPackage() -> PackageName(),
                                   this_type -> ExternalName());
                }
                if (method_clash)
                {
                    TypeSymbol *base_type = method_clash -> containing_type;
                    ReportSemError(SemanticError::UNIMPLEMENTABLE_CLASS,
                                   identifier_token,
                                   identifier_token,
                                   this_type -> ContainingPackage() -> PackageName(),
                                   this_type -> ExternalName(),
                                   method_clash -> Header(),
                                   base_type -> ContainingPackage() -> PackageName(),
                                   base_type -> ExternalName(),
                                   method -> Header(),
                                   containing_type -> ContainingPackage() -> PackageName(),
                                   containing_type -> ExternalName());
                }
            }
        }
    }

    //
    // Compute the set of final variables declared by the user in this type.
    // Then process all variable initializers and initializer blocks.
    //
    DefiniteSetup();
    ProcessStaticInitializers(class_body);
    ProcessInstanceInitializers(class_body);

    //
    // Reset the this_variable and this_method may have been set in
    // ProcessStaticInitializers and/or ProcessInstanceInitializers.
    // Indicate that there is no method being currently compiled
    // in this environment.
    //
    ThisVariable() = NULL;
    ThisMethod() = NULL;

    //
    // Recursively process all inner types
    //
    for (int l = 0; l < this_type -> NumNestedTypes(); l++)
    {
        TypeSymbol *inner_type = this_type -> NestedType(l);
        if (inner_type -> ACC_INTERFACE())
            CompleteSymbolTable((AstInterfaceDeclaration *) inner_type -> declaration);
        else
        {
            AstClassDeclaration *class_declaration =
                (AstClassDeclaration *) inner_type -> declaration;
            CompleteSymbolTable(class_declaration -> semantic_environment,
                                class_declaration -> identifier_token,
                                class_declaration -> class_body);
        }
    }

    state_stack.Pop();
}


void Semantic::CompleteSymbolTable(AstInterfaceDeclaration *interface_declaration)
{
    if (compilation_unit -> BadCompilationUnitCast())
        return;

    state_stack.Push(interface_declaration -> semantic_environment);
    TypeSymbol *this_type = ThisType();

    assert(this_type -> MethodMembersProcessed());
    assert(this_type -> FieldMembersProcessed());

    if (! this_type -> expanded_method_table)
        ComputeMethodsClosure(this_type,
                              interface_declaration -> identifier_token);

    DefiniteSetup();
    int count = interface_declaration -> NumClassVariables();
    for (int k = 0; k < count; k++)
        InitializeVariable(interface_declaration -> ClassVariable(k),
                           GetStaticInitializerMethod(count - k));

    //
    // Recursively process all inner types
    //
    for (int l = 0; l < this_type -> NumNestedTypes(); l++)
    {
        TypeSymbol *inner_type = this_type -> NestedType(l);
        if (inner_type -> ACC_INTERFACE())
            CompleteSymbolTable((AstInterfaceDeclaration *) inner_type -> declaration);
        else
        {
            AstClassDeclaration *class_declaration =
                (AstClassDeclaration *) inner_type -> declaration;
            CompleteSymbolTable(class_declaration -> semantic_environment,
                                class_declaration -> identifier_token,
                                class_declaration -> class_body);
        }
    }

    state_stack.Pop();
}


//
// Pass 5: Free up unneeded space.
//
void Semantic::CleanUp()
{
    if (control.option.nocleanup)
        return;

    for (int i = 0; i < compilation_unit -> NumTypeDeclarations(); i++)
    {
        TypeSymbol *type = NULL;
        Ast *type_declaration = compilation_unit -> TypeDeclaration(i);
        switch (type_declaration -> kind)
        {
            case Ast::CLASS:
            {
                AstClassDeclaration *class_declaration =
                    (AstClassDeclaration *) type_declaration;
                if (class_declaration -> semantic_environment)
                    type = class_declaration -> semantic_environment -> Type();
                break;
            }
            case Ast::INTERFACE:
            {
                AstInterfaceDeclaration *interface_declaration =
                    (AstInterfaceDeclaration *) type_declaration;
                if (interface_declaration -> semantic_environment)
                    type = interface_declaration -> semantic_environment ->
                        Type();
                break;
            }
            case Ast::EMPTY_DECLARATION:
                break;
            default:
                assert(false);
                break;
        }

        if (type)
            CleanUpType(type);
    }
}


void Semantic::CleanUpType(TypeSymbol *type)
{
    type -> DeleteAnonymousTypes();
    for (int i = 0; i < type -> NumNestedTypes(); i++)
        CleanUpType(type -> NestedType(i));

    type -> CompressSpace(); // space optimization

    for (int j = 0; j < type -> NumMethodSymbols(); j++)
        type -> MethodSym(j) -> CleanUp();

    delete type -> local;

    type -> local = NULL;

    delete type -> non_local;
    type -> non_local = NULL;

    delete type -> semantic_environment;
    type -> semantic_environment = NULL;

    type -> declaration = NULL;
}


TypeSymbol *Semantic::ReadType(FileSymbol *file_symbol,
                               PackageSymbol *package,
                               NameSymbol *name_symbol,
                               LexStream::TokenIndex tok)
{
    TypeSymbol *type;

    if (file_symbol && file_symbol -> IsJava())
    {
        if (! file_symbol -> semantic)
            control.ProcessHeaders(file_symbol);
        type = package -> FindTypeSymbol(name_symbol);
        if (! type)
        {
            type = package -> InsertOuterTypeSymbol(name_symbol);
            type -> MarkBad();
            type -> outermost_type = type;
            type -> supertypes_closure = new SymbolSet;
            type -> subtypes = new SymbolSet;
            type -> semantic_environment =
                new SemanticEnvironment(this, type, NULL);
            if (type != control.Object())
                type -> super = (type == control.Throwable()
                                 ? control.Object() : control.Throwable());
            type -> SetOwner(package);
            type -> SetSignature(control);
            AddDefaultConstructor(type);
            type -> file_symbol = file_symbol;
            file_symbol -> types.Next() = type;

            ReportSemError(SemanticError::TYPE_NOT_FOUND,
                           tok,
                           tok,
                           type -> ContainingPackage() -> PackageName(),
                           type -> ExternalName());
        }
    }
    else if (file_symbol)
    {
        // Read class file.
        type = package -> InsertOuterTypeSymbol(name_symbol);
        type -> outermost_type = type;
        type -> supertypes_closure = new SymbolSet;
        type -> subtypes = new SymbolSet;
        type -> SetOwner(package);
        type -> SetSignature(control);
        type -> file_symbol = file_symbol;
        type -> SetLocation();

        file_symbol -> package = package;
        file_symbol -> types.Next() = type;

        ReadClassFile(type, tok);
        assert (! type -> IsNested());
        control.input_class_file_set.AddElement(file_symbol);
    }
    else
    {
        //
        // No file found. See if a package by the same name exists, otherwise
        // create a placeholder type to avoid errors when the type name is
        // subsequently used.
        //
        PackageSymbol *subpackage = package -> FindPackageSymbol(name_symbol);
        if (! subpackage)
            subpackage = package -> InsertPackageSymbol(name_symbol);
        control.FindPathsToDirectory(subpackage);
        if (subpackage -> directory.Length())
        {
            if (package -> directory.Length())
                ReportSemError(SemanticError::PACKAGE_NOT_TYPE,
                               tok,
                               tok,
                               subpackage -> PackageName());
            type = control.no_type;
        }
        else
        {
            type = package -> InsertOuterTypeSymbol(name_symbol);
            type -> outermost_type = type;
            type -> SetOwner(package);
            type -> SetSignature(control);
            control.ProcessBadType(type);
            type -> MarkBad();
            if (package -> directory.Length())
                ReportSemError(SemanticError::TYPE_NOT_FOUND,
                               tok,
                               tok,
                               type -> ContainingPackage() -> PackageName(),
                               type -> ExternalName());
        }
    }

    return type;
}


TypeSymbol *Semantic::GetBadNestedType(TypeSymbol *type,
                                       LexStream::TokenIndex identifier_token)
{
    NameSymbol *name_symbol = lex_stream -> NameSymbol(identifier_token);

    TypeSymbol *outermost_type = type -> outermost_type;
    if (! outermost_type -> non_local)
        outermost_type -> non_local = new SymbolSet;
    if (! outermost_type -> local)
        outermost_type -> local = new SymbolSet;

    int length = type -> ExternalNameLength() + 1 +
        name_symbol -> NameLength(); // +1 for $,... +1 for $
    wchar_t *external_name = new wchar_t[length + 1]; // +1 for '\0';
    wcscpy(external_name, type -> ExternalName());
    wcscat(external_name, StringConstant::US_DS);
    wcscat(external_name, name_symbol -> Name());

    TypeSymbol *inner_type = type -> InsertNestedTypeSymbol(name_symbol);
    inner_type -> MarkBad();
    inner_type -> outermost_type = type -> outermost_type;
    inner_type -> supertypes_closure = new SymbolSet;
    inner_type -> subtypes = new SymbolSet;
    inner_type -> SetExternalIdentity(control.FindOrInsertName(external_name,
                                                               length));
    inner_type -> super = control.Object();
    inner_type -> SetOwner(type);
    if (! type -> Bad())
        ReportSemError(SemanticError::TYPE_NOT_FOUND,
                       identifier_token,
                       identifier_token,
                       inner_type -> ContainingPackage() -> PackageName(),
                       inner_type -> ExternalName());

    delete [] external_name;

    return inner_type;
}


void Semantic::ProcessImportQualifiedName(AstExpression *name)
{
    AstFieldAccess *field_access = name -> FieldAccessCast();
    if (field_access)
    {
        ProcessImportQualifiedName(field_access -> base);
        Symbol *symbol = field_access -> base -> symbol;

        TypeSymbol *type = symbol -> TypeCast();
        NameSymbol *name_symbol =
            lex_stream -> NameSymbol(field_access -> identifier_token);
        if (type) // The base name is a type
        {
            if (type -> Bad()) // Avoid chain-reaction errors.
            {
                field_access -> symbol = control.no_type;
                return;
            }
            if (! type -> expanded_type_table)
                ComputeTypesClosure(type, field_access -> identifier_token);
            TypeSymbol *inner_type = NULL;
            TypeShadowSymbol *type_shadow_symbol = type ->
                expanded_type_table -> FindTypeShadowSymbol(name_symbol);
            //
            // Only canonical names may be used in import statements, so we
            // don't worry about ambiguous names (which are necessarily
            // inherited and hence non-canonical). But we do need an extra
            // filter on the containing type being correct.
            //
            if (type_shadow_symbol)
                inner_type = type_shadow_symbol -> type_symbol;
            if (! inner_type)
                inner_type = control.no_type;
            else if (type != inner_type -> owner)
            {
                ReportSemError(SemanticError::IMPORT_NOT_CANONICAL,
                               field_access -> LeftToken(),
                               field_access -> RightToken(),
                               name_symbol -> Name(),
                               inner_type -> ContainingPackage() -> PackageName(),
                               inner_type -> ExternalName());
            }
            else if (inner_type -> ACC_PRIVATE() ||
                     (! inner_type -> ACC_PUBLIC() &&
                      inner_type -> ContainingPackage() != this_package))
            {
                ReportTypeInaccessible(field_access, inner_type);
            }
            field_access -> symbol = inner_type;
        }
        else
        {
            PackageSymbol *package = symbol -> PackageCast();
            type = package -> FindTypeSymbol(name_symbol);
            if (! type)
            {
                FileSymbol *file_symbol =
                    Control::GetFile(control, package, name_symbol);
                if (file_symbol)
                    type = ReadType(file_symbol, package, name_symbol,
                                    field_access -> identifier_token);
            }
            else if (type -> SourcePending())
                control.ProcessHeaders(type -> file_symbol);

            //
            // If the field_access was resolved to a type, save it later use.
            // Otherwise, assume the field_access is a package name.
            //
            if (type)
            {
                if (! type -> ACC_PUBLIC() &&
                    type -> ContainingPackage() != this_package)
                {
                    ReportTypeInaccessible(field_access, type);
                }
                field_access -> symbol = type;
            }
            else
            {
                PackageSymbol *subpackage =
                    package -> FindPackageSymbol(name_symbol);
                if (! subpackage)
                    subpackage = package -> InsertPackageSymbol(name_symbol);
                control.FindPathsToDirectory(subpackage);
                field_access -> symbol = subpackage;
            }
        }
    }
    else
    {
        AstSimpleName *simple_name = name -> SimpleNameCast();

        assert(simple_name);

        //
        // JLS 6.3 The leading simple name of a type import must be a package
        // name, as class names are not in scope. JLS 7.5: Nested classes of
        // all sorts (top-level or inner) can be imported by either kind of
        // import statement. Class names in import statements must be the
        // canonical version.
        //
        TypeSymbol *type = FindSimpleNameType(control.unnamed_package,
                                              simple_name -> identifier_token);

        //
        // If the simple_name is a type, detect the error. Otherwise, assume
        // it is a package, and legal.
        //
        if (type)
        {
            ReportSemError(SemanticError::IMPORT_FROM_UNNAMED_PACKAGE,
                           simple_name -> identifier_token,
                           simple_name -> identifier_token,
                           lex_stream -> NameString(simple_name -> identifier_token));
            simple_name -> symbol = control.no_type;
        }
        else
        {
            NameSymbol *name_symbol =
                lex_stream -> NameSymbol(simple_name -> identifier_token);
            PackageSymbol *package =
                control.external_table.FindPackageSymbol(name_symbol);
            if (! package)
                package = control.external_table.
                    InsertPackageSymbol(name_symbol, NULL);
            control.FindPathsToDirectory(package);
            simple_name -> symbol = package;
        }
    }
}


//
// Processes a package-or-type name. If an accessible type exists, it is chosen.
// Next, if a package exists, it is chosen. Then, an error is issued, but a
// check for an inaccessible type is made before inventing a package. The result
// is stored in name->symbol.
//
void Semantic::ProcessPackageOrType(AstExpression *name)
{
    AstFieldAccess *field_access = name -> FieldAccessCast();
    if (field_access)
    {
        ProcessPackageOrType(field_access -> base);
        Symbol *symbol = field_access -> base -> symbol;

        TypeSymbol *type = symbol -> TypeCast();
        if (type) // The base name is a type
        {
            field_access -> symbol = MustFindNestedType(type, field_access);
        }
        else
        {
            // Base name is package. Search for type, then subpackage.
            PackageSymbol *package = symbol -> PackageCast();
            NameSymbol *name_symbol =
                lex_stream -> NameSymbol(field_access -> identifier_token);
            type = package -> FindTypeSymbol(name_symbol);
            if (! type)
            {
                FileSymbol *file_symbol =
                    Control::GetFile(control, package, name_symbol);
                if (file_symbol)
                    type = ReadType(file_symbol, package, name_symbol,
                                    field_access -> identifier_token);
            }
            else if (type -> SourcePending())
                control.ProcessHeaders(type -> file_symbol);

            //
            // If the field access was resolved into a type, then save it.
            // Otherwise, assume it is a package
            //
            if (type)
                // save the resolved type of this expression for later use...
                field_access -> symbol = type;
            else
            {
                NameSymbol *name_symbol =
                    lex_stream -> NameSymbol(field_access -> identifier_token);
                PackageSymbol *subpackage =
                    package -> FindPackageSymbol(name_symbol);
                if (! subpackage)
                    subpackage = package -> InsertPackageSymbol(name_symbol);
                control.FindPathsToDirectory(subpackage);
                field_access -> symbol = subpackage;
                if (subpackage -> directory.Length() == 0)
                {
                    ReportSemError(SemanticError::PACKAGE_NOT_FOUND,
                                   field_access -> identifier_token,
                                   field_access -> identifier_token,
                                   subpackage -> PackageName());
                }
            }
        }
    }
    else
    {
        AstSimpleName *simple_name = name -> SimpleNameCast();
        assert(simple_name);

        TypeSymbol *type = FindType(simple_name -> identifier_token);
        if (type)
        {
            simple_name -> symbol = type;
        }
        else
        {
            NameSymbol *name_symbol =
                lex_stream -> NameSymbol(simple_name -> identifier_token);
            PackageSymbol *package =
                control.external_table.FindPackageSymbol(name_symbol);
            if (! package)
                package = control.external_table.
                    InsertPackageSymbol(name_symbol, NULL);
            control.FindPathsToDirectory(package);
            if (package -> directory.Length() == 0)
            {
                //
                // If there is no package, see if the user is trying to access
                // an inaccessible nested type before giving up.
                //
                if (state_stack.Size() > 0)
                {
                    NameSymbol *name_symbol = lex_stream ->
                        NameSymbol(simple_name -> identifier_token);
                    for (TypeSymbol *super_type = ThisType() -> super;
                         super_type; super_type = super_type -> super)
                    {
                        assert(super_type -> expanded_type_table);

                        TypeShadowSymbol *type_shadow_symbol =
                            super_type -> expanded_type_table ->
                            FindTypeShadowSymbol(name_symbol);
                        if (type_shadow_symbol)
                        {
                            type = type_shadow_symbol -> type_symbol;
                            break;
                        }
                    }
                }
                if (type)
                {
                    ReportTypeInaccessible(simple_name -> identifier_token,
                                           simple_name -> identifier_token,
                                           type);
                    simple_name -> symbol = type;
                }
                else
                {
                    ReportSemError(SemanticError::PACKAGE_NOT_FOUND,
                                   simple_name -> identifier_token,
                                   simple_name -> identifier_token,
                                   package -> PackageName());
                    simple_name -> symbol = package;
                }
            }
            else simple_name -> symbol = package;
        }
    }
}


void Semantic::ProcessTypeImportOnDemandDeclaration(AstImportDeclaration *import_declaration)
{
    ProcessImportQualifiedName(import_declaration -> name);
    Symbol *symbol = import_declaration -> name -> symbol;

    PackageSymbol *package = symbol -> PackageCast();
    if (package && package -> directory.Length() == 0)
    {
        ReportSemError(SemanticError::PACKAGE_NOT_FOUND,
                       import_declaration -> name -> LeftToken(),
                       import_declaration -> name -> RightToken(),
                       package -> PackageName());
    }

    //
    // Two or more type-import-on-demand may name the same package; the effect
    // is as if there were only one such declaration. Likewise, importing the
    // current package or java.lang.* is ok, although useless.
    // TODO: In pedantic mode, warn about duplicate imports of the same package,
    // of the current package, or of java.lang.*.
    //
    if (symbol == this_package)
        return;
    for (int i = 0; i < import_on_demand_packages.Length(); i++)
    {
        if (symbol == import_on_demand_packages[i])
            return;
    }

    import_on_demand_packages.Next() = symbol;

    //
    //
    //
    TypeSymbol *type = symbol -> TypeCast();
    if (control.option.deprecation && type &&
        type -> IsDeprecated() && type -> file_symbol != source_file_symbol)
    {
        ReportSemError(SemanticError::DEPRECATED_TYPE,
                       import_declaration -> name -> LeftToken(),
                       import_declaration -> name -> RightToken(),
                       type -> ContainingPackage() -> PackageName(),
                       type -> ExternalName());
    }
}


TypeSymbol *Semantic::FindSimpleNameType(PackageSymbol *package,
                                         LexStream::TokenIndex identifier_token)
{
    NameSymbol *name_symbol = lex_stream -> NameSymbol(identifier_token);
    TypeSymbol *type = package -> FindTypeSymbol(name_symbol);
    if (type)
    {
        if (type -> SourcePending())
            control.ProcessHeaders(type -> file_symbol);
    }
    else
    {
        //
        // Check whether or not the type was declared in another compilation
        // unit in the main package.
        //
        FileSymbol *file_symbol = Control::GetFile(control, package,
                                                   name_symbol);
        if (file_symbol)
            type = ReadType(file_symbol, package, name_symbol,
                            identifier_token);
    }

    return type;
}

void Semantic::ProcessSingleTypeImportDeclaration(AstImportDeclaration *import_declaration)
{
    ProcessImportQualifiedName(import_declaration -> name);
    Symbol *symbol = import_declaration -> name -> symbol;
    PackageSymbol *package = symbol -> PackageCast();
    TypeSymbol *type = symbol -> TypeCast();

    //
    // Technically, the JLS grammar forbids "import foo;". However, our
    // grammar parses it, and will either find or create the package foo, so
    // we can give a better message than "expected '.'". If a non-type is
    // imported, we create a place-holder type so that the use of the
    // unqualified type name won't cause cascading errors elsewhere in the file.
    //
    if (package)
    {
        ReportSemError(SemanticError::UNKNOWN_ON_DEMAND_IMPORT,
                       import_declaration -> name -> LeftToken(),
                       import_declaration -> name -> RightToken(),
                       package -> PackageName());
        NameSymbol *name_symbol = lex_stream ->
            NameSymbol(import_declaration -> name -> RightToken());
        type = package -> InsertOuterTypeSymbol(name_symbol);
        type -> MarkBad();
        type -> super = control.no_type;
        type -> outermost_type = control.no_type;
    }

    //
    // If two single-type-import declarations in the same compilation unit
    // attempt to import types with the same simple name, then a compile-time
    // error occurs, unless the two types are the same type, in which case the
    // duplicate declaration is ignored.
    // TODO: Give pedantic warnings about duplicate type declarations.
    //
    for (int i = 0; i < single_type_imports.Length(); i++)
    {
        if (type == single_type_imports[i])
            return;
    }

    TypeSymbol *old_type = NULL;
    int k;
    for (k = 0; k < compilation_unit -> NumTypeDeclarations(); k++)
    {
        AstClassDeclaration *class_declaration =
            compilation_unit -> TypeDeclaration(k) -> ClassDeclarationCast();
        AstInterfaceDeclaration *interface_declaration =
            compilation_unit -> TypeDeclaration(k) -> InterfaceDeclarationCast();

        if (class_declaration)
        {
            if (class_declaration -> semantic_environment)
            {
                old_type = class_declaration -> semantic_environment -> Type();
                if (old_type -> Identity() == type -> Identity())
                    break;
            }
        }
        else if (interface_declaration)
        {
            if (interface_declaration -> semantic_environment)
            {
                old_type =
                    interface_declaration -> semantic_environment -> Type();
                if (old_type -> Identity() == type -> Identity())
                    break;
            }
        }
    }

    if (k < compilation_unit -> NumTypeDeclarations())
    {
        AstFieldAccess *field_access =
            import_declaration -> name -> FieldAccessCast();
        package = (field_access
                   ? field_access -> base -> symbol -> PackageCast()
                   : control.unnamed_package);

        //
        // It's ok to import a type that is being compiled...
        //
        if (type == old_type && package == this_package)
        {
            ReportSemError(SemanticError::UNNECESSARY_TYPE_IMPORT,
                           import_declaration -> name -> LeftToken(),
                           import_declaration -> name -> RightToken(),
                           lex_stream -> NameString(import_declaration -> name -> RightToken()),
                           old_type -> FileLoc());
        }
        else
        {
            ReportSemError(SemanticError::DUPLICATE_IMPORT_NAME,
                           import_declaration -> name -> RightToken(),
                           import_declaration -> name -> RightToken(),
                           lex_stream -> NameString(import_declaration -> name -> RightToken()),
                           old_type -> FileLoc());
        }
    }
    else
    {
        int i = 0;
        for (i = 0; i < compilation_unit -> NumImportDeclarations(); i++)
        {
            TypeSymbol *other_type =
                compilation_unit -> ImportDeclaration(i) -> name -> Type();
            if (compilation_unit -> ImportDeclaration(i) == import_declaration ||
                (other_type && other_type -> Identity() == type -> Identity()))
            {
                break;
            }
        }

        assert(i < compilation_unit -> NumImportDeclarations());

        if (compilation_unit -> ImportDeclaration(i) == import_declaration) // No duplicate found
        {
            import_declaration -> name -> symbol = type;
            single_type_imports.Next() = type;
        }
        else
        {
            FileLocation file_location(lex_stream,
                                       compilation_unit -> ImportDeclaration(i) -> LeftToken());
            ReportSemError(SemanticError::DUPLICATE_IMPORT_NAME,
                           import_declaration -> name -> RightToken(),
                           import_declaration -> name -> RightToken(),
                           lex_stream -> NameString(import_declaration -> name -> RightToken()),
                           file_location.location);
        }
    }

    //
    //
    //
    if (control.option.deprecation && type -> IsDeprecated() &&
        type -> file_symbol != source_file_symbol)
    {
        ReportSemError(SemanticError::DEPRECATED_TYPE,
                       import_declaration -> name -> LeftToken(),
                       import_declaration -> name -> RightToken(),
                       type -> ContainingPackage() -> PackageName(),
                       type -> ExternalName());
    }
}


void Semantic::ProcessFieldDeclaration(AstFieldDeclaration *field_declaration)
{
    TypeSymbol *this_type = ThisType();
    AccessFlags access_flags = this_type -> ACC_INTERFACE()
        ? ProcessInterfaceFieldModifiers(field_declaration)
        : ProcessFieldModifiers(field_declaration);

    //
    // JLS2 8.1.2 - Inner classes may not have static fields unless they are
    // final and initialized by a constant.  Hence, the type of the static
    // field may only be a primitive or String.  Here, we check that the
    // entire declaration is final, then that each variableDeclarator is
    // of the right type and is initialized.  Later, when processing the
    // initializer, we check that it is indeed a compile-time constant
    // (see init.cpp, Semantic::ProcessVariableInitializer)
    //
    bool must_be_constant = false;
    if (this_type -> IsInner() && access_flags.ACC_STATIC())
    {
        if (access_flags.ACC_FINAL())
            must_be_constant = true;
        else
        {
            AstModifier *modifier = NULL;
            for (int i = 0;
                 i < field_declaration -> NumVariableModifiers(); i++)
            {
                if (field_declaration -> VariableModifier(i) -> kind ==
                    Ast::STATIC)
                {
                    modifier = field_declaration -> VariableModifier(i);
                }
            }

            assert(modifier);

            ReportSemError(SemanticError::STATIC_FIELD_IN_INNER_CLASS_NOT_FINAL,
                           modifier -> modifier_kind_token,
                           modifier -> modifier_kind_token,
                           this_type -> Name(),
                           this_type -> FileLoc());
        }
    }

    //
    // To avoid deprecated type warnings when processing a deprecated field
    // declaration, we must temporarily mark this type as deprecated, because
    // the field variable symbol(s) do not yet exist.
    //
    bool deprecated_declarations = lex_stream ->
        IsDeprecated(lex_stream -> Previous(field_declaration -> LeftToken()));
    bool deprecated_type = this_type -> IsDeprecated();
    if (deprecated_declarations)
        this_type -> MarkDeprecated();
    AstArrayType *array_type = field_declaration -> type -> ArrayTypeCast();
    TypeSymbol *field_type;
    {
        Ast *actual_type = (array_type ? array_type -> type
                            : field_declaration -> type);
        AstPrimitiveType *primitive_type = actual_type -> PrimitiveTypeCast();
        field_type = (primitive_type ? FindPrimitiveType(primitive_type)
                      : MustFindType(actual_type));
    }
    if (! deprecated_type && deprecated_declarations)
        this_type -> ResetDeprecated();

    for (int i = 0; i < field_declaration -> NumVariableDeclarators(); i++)
    {
        AstVariableDeclarator *variable_declarator =
            field_declaration -> VariableDeclarator(i);
        AstVariableDeclaratorId *name =
            variable_declarator -> variable_declarator_name;
        NameSymbol *name_symbol =
            lex_stream -> NameSymbol(name -> identifier_token);

        if (this_type -> FindVariableSymbol(name_symbol))
        {
            ReportSemError(SemanticError::DUPLICATE_FIELD,
                           name -> identifier_token,
                           name -> identifier_token,
                           name_symbol -> Name(),
                           this_type -> Name());
        }
        else
        {
            VariableSymbol *variable =
                this_type -> InsertVariableSymbol(name_symbol);
            int num_dimensions =
                (array_type ? array_type -> NumBrackets() : 0) +
                name -> NumBrackets();
            if (num_dimensions == 0)
                variable -> SetType(field_type);
            else variable -> SetType(field_type ->
                                     GetArrayType(this, num_dimensions));
            variable -> SetFlags(access_flags);
            variable -> SetOwner(this_type);
            variable -> declarator = variable_declarator;
            if (must_be_constant &&
                (num_dimensions != 0 ||
                 ! variable_declarator -> variable_initializer_opt ||
                 (! field_type -> Primitive() &&
                  field_type != control.String())))
            {
                ReportSemError(SemanticError::STATIC_FIELD_IN_INNER_CLASS_NOT_CONSTANT,
                               name -> identifier_token,
                               name -> identifier_token,
                               name_symbol -> Name(),
                               this_type -> Name(),
                               this_type -> FileLoc());
            }

            variable_declarator -> symbol = variable;

            if (deprecated_declarations)
                variable -> MarkDeprecated();
        }
    }
}


void Semantic::ProcessConstructorDeclaration(AstConstructorDeclaration *constructor_declaration)
{
    TypeSymbol *this_type = ThisType();

    AccessFlags access_flags =
        ProcessConstructorModifiers(constructor_declaration);

    AstMethodDeclarator *constructor_declarator =
        constructor_declaration -> constructor_declarator;
    wchar_t *constructor_name =
        lex_stream -> NameString(constructor_declarator -> identifier_token);

    //
    // A bad name indicates either a misspelling, or a method missing
    // a return type.  In an anonymous class, assume a missing return
    // type.  In all other classes, if the probability of misspelling
    // >= 50%, correct the name, otherwise treat it as a method with
    // bad return type.
    //
    bool treat_as_method = false;
    if (this_type -> Anonymous())
    {
        ReportSemError(SemanticError::CONSTRUCTOR_FOUND_IN_ANONYMOUS_CLASS,
                       constructor_declarator -> LeftToken(),
                       constructor_declarator -> RightToken(),
                       constructor_name);
        treat_as_method = true;
    }
    else if (lex_stream -> NameSymbol(constructor_declarator -> identifier_token) != this_type -> Identity())
    {
        if (Spell::Index(constructor_name, this_type -> Name()) >= 5)
        {
            ReportSemError(SemanticError::MISSPELLED_CONSTRUCTOR_NAME,
                           constructor_declarator -> identifier_token,
                           constructor_declarator -> identifier_token,
                           constructor_name,
                           this_type -> Name());
            constructor_name = this_type -> Name(); // correct the name
        }
        else
        {
            ReportSemError(SemanticError::MISMATCHED_CONSTRUCTOR_NAME,
                           constructor_declarator -> identifier_token,
                           constructor_declarator -> identifier_token,
                           constructor_name,
                           this_type -> Name());
            treat_as_method = true;
        }
    }

    //
    // As the body of the constructor may not have been parsed yet, we estimate
    // a size for its symbol table based on the number of lines in the body + a
    // margin for one-liners.
    //
    BlockSymbol *block_symbol =
        new BlockSymbol(constructor_declarator -> NumFormalParameters() + 3);
    block_symbol -> max_variable_index = 1; // All types need a spot for "this".

    ProcessFormalParameters(block_symbol, constructor_declarator);

    //
    // Note that constructors are always named "<init>", but if this is a
    // method with missing return type, we use the method name.
    //
    NameSymbol *name_symbol = treat_as_method
        ? lex_stream -> NameSymbol(constructor_declarator -> identifier_token)
        : control.init_name_symbol;
    MethodSymbol *constructor = this_type -> FindMethodSymbol(name_symbol);

    if (! constructor) // there exists a constructor already in type -> table.
        if (! treat_as_method)
            constructor = this_type -> InsertConstructorSymbol(name_symbol);
        else
            constructor = this_type -> InsertMethodSymbol(name_symbol);
    else
    {
        if (this_type -> FindOverloadMethod(constructor,
                                            constructor_declarator))
        {
            ReportSemError(SemanticError::DUPLICATE_CONSTRUCTOR,
                           constructor_declarator -> LeftToken(),
                           constructor_declarator -> RightToken(),
                           this_type -> Name());
            delete block_symbol;
            return;
        }

        constructor = this_type -> Overload(constructor);
    }

    //
    // If the method is not static, leave a slot for the "this" pointer.
    //
    constructor -> SetType(treat_as_method ? control.no_type
                           : control.void_type);
    constructor -> SetFlags(access_flags);
    constructor -> SetContainingType(this_type);
    constructor -> SetBlockSymbol(block_symbol);
    constructor -> declaration = constructor_declaration;

    if (this_type -> EnclosingType())
    {
        VariableSymbol *this0_variable =
            block_symbol -> InsertVariableSymbol(control.this0_name_symbol);
        this0_variable -> SetType(this_type -> ContainingType());
        this0_variable -> SetOwner(constructor);
        this0_variable -> SetLocalVariableIndex(block_symbol ->
                                                max_variable_index++);
        this0_variable -> MarkComplete();
        this0_variable -> MarkSynthetic();
    }

    for (int i = 0; i < constructor_declarator -> NumFormalParameters(); i++)
    {
        AstVariableDeclarator *formal_declarator =
            constructor_declarator -> FormalParameter(i) -> formal_declarator;
        VariableSymbol *symbol = formal_declarator -> symbol;

        symbol -> SetOwner(constructor);
        symbol -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
        symbol -> MarkComplete();
        if (control.IsDoubleWordType(symbol -> Type()))
            block_symbol -> max_variable_index++;
        symbol -> declarator = formal_declarator;
        constructor -> AddFormalParameter(symbol);
    }

    constructor -> SetSignature(control);

    for (int k = 0; k < constructor_declaration -> NumThrows(); k++)
    {
        AstExpression *throw_expression = constructor_declaration -> Throw(k);
        TypeSymbol *throw_type = MustFindType(throw_expression);
        throw_expression -> symbol = throw_type;
        constructor -> AddThrows(throw_type);
    }

    constructor_declaration -> constructor_symbol = constructor; // save for processing bodies later.

    if (lex_stream -> IsDeprecated(lex_stream -> Previous(constructor_declaration -> LeftToken())))
        constructor -> MarkDeprecated();
}


void Semantic::AddDefaultConstructor(TypeSymbol *type)
{
    MethodSymbol *constructor =
        type -> InsertConstructorSymbol(control.init_name_symbol);

    BlockSymbol *block_symbol = new BlockSymbol(1);
    block_symbol -> max_variable_index = 1; // All types need a spot for "this"

    constructor -> SetType(control.void_type);
    constructor -> SetContainingType(type);
    constructor -> SetBlockSymbol(block_symbol);
    if (type -> ACC_PUBLIC())
        constructor -> SetACC_PUBLIC();
    else if (type -> ACC_PROTECTED())
        constructor -> SetACC_PROTECTED();
    else if (type -> ACC_PRIVATE())
        constructor -> SetACC_PRIVATE();

    if (type -> EnclosingType())
    {
        VariableSymbol *this0_variable =
            block_symbol -> InsertVariableSymbol(control.this0_name_symbol);
        this0_variable -> SetType(type -> ContainingType());
        this0_variable -> SetOwner(constructor);
        this0_variable -> SetLocalVariableIndex(block_symbol ->
                                                max_variable_index++);
        this0_variable -> MarkComplete();
        this0_variable -> MarkSynthetic();
    }

    constructor -> SetSignature(control);

    AstClassDeclaration *class_declaration =
        (type -> declaration ? type -> declaration -> ClassDeclarationCast()
         : (AstClassDeclaration *) NULL);
    if (class_declaration)
    {
        AstClassBody *class_body = class_declaration -> class_body;
        LexStream::TokenIndex left_loc = class_declaration -> identifier_token,
                              right_loc = (class_declaration -> super_opt
                                           ? class_declaration -> super_opt -> RightToken()
                                           : class_declaration -> identifier_token);

        AstMethodDeclarator *method_declarator =
            compilation_unit -> ast_pool -> GenMethodDeclarator();
        method_declarator -> identifier_token = left_loc;
        method_declarator -> left_parenthesis_token = left_loc;
        method_declarator -> right_parenthesis_token = right_loc;

        AstSuperCall *super_call = NULL;
        if (type != control.Object())
        {
            super_call = compilation_unit -> ast_pool -> GenSuperCall();
            super_call -> base_opt = NULL;
            super_call -> dot_token_opt = 0;
            super_call -> super_token = left_loc;
            super_call -> left_parenthesis_token = left_loc;
            super_call -> right_parenthesis_token = right_loc;
            super_call -> semicolon_token = right_loc;
        }

        AstReturnStatement *return_statement =
            compilation_unit -> ast_pool -> GenReturnStatement();
        return_statement -> return_token = left_loc;
        return_statement -> expression_opt = NULL;
        return_statement -> semicolon_token = left_loc;
        return_statement -> is_reachable = true;

        AstMethodBody *constructor_block =
            compilation_unit -> ast_pool -> GenMethodBody();
        // This symbol table will be empty.
        constructor_block -> block_symbol = new BlockSymbol(0);
        constructor_block -> left_brace_token  = left_loc;
        constructor_block -> right_brace_token = right_loc;
        constructor_block -> AllocateBlockStatements(1);
        constructor_block -> AddStatement(return_statement);
        constructor_block -> explicit_constructor_opt = super_call;

        AstConstructorDeclaration *constructor_declaration =
            compilation_unit -> ast_pool -> GenConstructorDeclaration();
        constructor_declaration -> constructor_declarator = method_declarator;
        constructor_declaration -> constructor_body = constructor_block;

        constructor_declaration -> constructor_symbol = constructor;
        constructor -> declaration = constructor_declaration;
        class_body -> default_constructor = constructor_declaration;
    }
}


//
// This is called by AddInheritedMethods in two conditions: First, method
// belongs to base_type, so it must successfully override hidden_method,
// including a compatible throws clause. Second, method belongs to a supertype
// of base_type, in which case base_type is inheriting two declarations, one
// of which is necessarily abstract. If one is non-abstract, it must
// successfully override the abstract version, including the throws clause;
// but if both are abstract the throws clauses are inconsequential. It is
// possible that both method and hidden_method were declared in the same
// class, and inherited through two paths, in which case we do nothing.
// See JLS 8.4.6.4, and 9.4.1.
//
void Semantic::CheckMethodOverride(MethodSymbol *method,
                                   MethodSymbol *hidden_method,
                                   TypeSymbol *base_type)
{
    assert(! hidden_method -> ACC_PRIVATE());

    //
    // If we inherit the same method from multiple paths (including methods
    // of Object via interfaces), we already know the result.
    //
    if (hidden_method == method ||
        (method -> containing_type -> ACC_INTERFACE() &&
         method -> containing_type != base_type &&
         hidden_method -> containing_type == control.Object()))
    {
        return;
    }

    LexStream::TokenIndex left_tok;
    LexStream::TokenIndex right_tok;

    if (method -> containing_type == base_type && ThisType() == base_type)
    {
        AstMethodDeclaration *method_declaration =
            (AstMethodDeclaration *) method -> declaration;
        assert(method_declaration);
        AstMethodDeclarator *method_declarator =
            method_declaration -> method_declarator;

        left_tok = method_declarator -> LeftToken();
        right_tok = method_declarator -> RightToken();
    }
    else
    {
        AstInterfaceDeclaration *interface_declaration =
            ThisType() -> declaration -> InterfaceDeclarationCast();
        AstClassDeclaration *class_declaration =
            ThisType() -> declaration -> ClassDeclarationCast();
        if (interface_declaration)
            left_tok = right_tok = interface_declaration -> identifier_token;
        else if (class_declaration)
            left_tok = right_tok = class_declaration -> identifier_token;
        else
        {
            AstClassInstanceCreationExpression *class_creation =
                ThisType() -> declaration -> ClassInstanceCreationExpressionCast();

            assert(class_creation);

            left_tok = class_creation -> class_type -> LeftToken();
            right_tok = class_creation -> class_type -> RightToken();
        }
    }

    //
    // Return types must match.
    //
    if (hidden_method -> Type() != method -> Type())
    {
        //
        // We support covariant return types when loading from .class files,
        // even though this is not strictly legal in JLS2 (according to binary
        // compatibility, changing the return type need not be supported).
        // This is done in anticipation of JDK 1.5, when covariance is likely
        // to be introduced. The resultant effect is that the subclass must
        // conform to the narrower return type. Note that we currently only
        // support Object->Object covariance (and not primitive->primitive,
        // void->primitive, or void->Object). When loading from .java files,
        // however, we enforce exact return type matching.
        //
        if (method -> containing_type -> file_symbol -> IsClass() &&
            hidden_method -> Type() -> IsSubtype(control.Object()) &&
            method -> Type() -> IsSubtype(hidden_method -> Type()))
        {
            // Silent acceptance. TODO: Should we add a pedantic warning?
        }
        else if (method -> containing_type == base_type)
        {
            if (base_type -> ACC_INTERFACE() &&
                hidden_method -> containing_type == control.Object())
            {
                //
                // TODO: Review this when JLS3 is published.  See Sun bug
                // 4479715, which explains our current stance of allowing
                // int clone() throws MyException; or Object finalize();.
                //
                if (hidden_method -> ACC_PUBLIC())
                {
                    ReportSemError(SemanticError::MISMATCHED_IMPLICIT_METHOD,
                                   left_tok,
                                   right_tok,
                                   method -> Header(),
                                   hidden_method -> Header());
                    base_type -> MarkBad();
                }
                else
                {
                    ReportSemError(SemanticError::UNIMPLEMENTABLE_INTERFACE,
                                   left_tok,
                                   right_tok,
                                   base_type -> ContainingPackage() -> PackageName(),
                                   base_type -> ExternalName(),
                                   method -> Header(),
                                   hidden_method -> Header());
                }
            }
            else
            {
                ReportSemError(SemanticError::MISMATCHED_INHERITED_METHOD,
                               left_tok,
                               right_tok,
                               method -> Header(),
                               hidden_method -> Header(),
                               hidden_method -> containing_type -> ContainingPackage() -> PackageName(),
                               hidden_method -> containing_type -> ExternalName());
                base_type -> MarkBad();
            }
        }
        else
        {
            ReportSemError(SemanticError::MISMATCHED_INHERITED_METHOD_EXTERNALLY,
                           left_tok,
                           right_tok,
                           base_type -> ExternalName(),
                           method -> Header(),
                           method -> containing_type -> ContainingPackage() -> PackageName(),
                           method -> containing_type -> ExternalName(),
                           hidden_method -> Header(),
                           hidden_method -> containing_type -> ContainingPackage() -> PackageName(),
                           hidden_method -> containing_type -> ExternalName());
            base_type -> MarkBad();
        }
    }


    //
    // If base_type declared method, hidden_method must not be final. On the
    // other hand, if a type inherits a final method from a superclass and
    // an abstract method from an interface, it is legal.
    //
    if (method -> containing_type == base_type &&
        (hidden_method -> ACC_FINAL() ||
         hidden_method -> containing_type -> ACC_FINAL()))
    {
        if (base_type -> ACC_INTERFACE())
        {
            ReportSemError(SemanticError::FINAL_IMPLICIT_METHOD_OVERRIDE,
                           left_tok,
                           right_tok,
                           method -> Header(),
                           hidden_method -> Header());
        }
        else
        {
            ReportSemError(SemanticError::FINAL_METHOD_OVERRIDE,
                           left_tok,
                           right_tok,
                           method -> Header(),
                           hidden_method -> Header(),
                           hidden_method -> containing_type -> ContainingPackage() -> PackageName(),
                           hidden_method -> containing_type -> ExternalName());
        }
        base_type -> MarkBad();
    }


    //
    // Both or neither versions must be static.
    //
    if (method -> ACC_STATIC() != hidden_method -> ACC_STATIC())
    {
        if (method -> containing_type == base_type)
        {
            ReportSemError((method -> ACC_STATIC()
                            ? SemanticError::INSTANCE_METHOD_OVERRIDE
                            : SemanticError::CLASS_METHOD_OVERRIDE),
                           left_tok,
                           right_tok,
                           method -> Header(),
                           hidden_method -> Header(),
                           hidden_method -> containing_type -> ContainingPackage() -> PackageName(),
                           hidden_method -> containing_type -> ExternalName());
        }
        else
        {
            assert(method -> ACC_STATIC());
            ReportSemError(SemanticError::INSTANCE_METHOD_OVERRIDE_EXTERNALLY,
                           left_tok,
                           right_tok,
                           base_type -> ExternalName(),
                           method -> Header(),
                           method -> containing_type -> ContainingPackage() -> PackageName(),
                           method -> containing_type -> ExternalName(),
                           hidden_method -> Header(),
                           hidden_method -> containing_type -> ContainingPackage() -> PackageName(),
                           hidden_method -> containing_type -> ExternalName());
        }
        base_type -> MarkBad();
    }

    //
    // An overriding method cannot be less accessible. On the other hand, it
    // is legal to inherit two abstract methods when one is not public.
    //
    if (hidden_method -> ACC_PUBLIC())
    {
        if (! method -> ACC_PUBLIC())
        {
            if (method -> containing_type == base_type)
            {
                ReportSemError(SemanticError::BAD_ACCESS_METHOD_OVERRIDE,
                               left_tok,
                               right_tok,
                               method -> Header(),
                               method -> AccessString(),
                               hidden_method -> Header(),
                               StringConstant::US_public,
                               hidden_method -> containing_type -> ContainingPackage() -> PackageName(),
                               hidden_method -> containing_type -> ExternalName());
                base_type -> MarkBad();
            }
            else if (! method -> ACC_ABSTRACT())
            {
                ReportSemError(SemanticError::BAD_ACCESS_METHOD_OVERRIDE_EXTERNALLY,
                               left_tok,
                               right_tok,
                               base_type -> ExternalName(),
                               method -> Header(),
                               method -> AccessString(),
                               method -> containing_type -> ContainingPackage() -> PackageName(),
                               method -> containing_type -> ExternalName(),
                               hidden_method -> Header(),
                               StringConstant::US_public,
                               hidden_method -> containing_type -> ContainingPackage() -> PackageName(),
                               hidden_method -> containing_type -> ExternalName());
                base_type -> MarkBad();
            }
        }
    }
    else if (hidden_method -> ACC_PROTECTED())
    {
        if (! method -> ACC_PROTECTED() &&
            ! method -> ACC_PUBLIC())
        {
            ReportSemError(SemanticError::BAD_ACCESS_METHOD_OVERRIDE,
                           left_tok,
                           right_tok,
                           method -> Header(),
                           method -> AccessString(),
                           hidden_method -> Header(),
                           StringConstant::US_protected,
                           hidden_method -> containing_type -> ContainingPackage() -> PackageName(),
                           hidden_method -> containing_type -> ExternalName());
            base_type -> MarkBad();
        }
    }
    else if (method -> ACC_PRIVATE())
    {
        ReportSemError(SemanticError::BAD_ACCESS_METHOD_OVERRIDE,
                       left_tok,
                       right_tok,
                       method -> Header(),
                       StringConstant::US_private,
                       hidden_method -> Header(),
                       StringConstant::US_default,
                       hidden_method -> containing_type -> ContainingPackage() -> PackageName(),
                       hidden_method -> containing_type -> ExternalName());
        base_type -> MarkBad();
    }

    //
    // Check the throws clause, unless base_type is inheriting two abstract
    // methods.
    //
    if (method -> containing_type != base_type && method -> ACC_ABSTRACT())
        return;

    method -> ProcessMethodThrows(this, left_tok);
    hidden_method -> ProcessMethodThrows(this, left_tok);

    for (int i = method -> NumThrows() - 1; i >= 0; i--)
    {
        TypeSymbol *exception = method -> Throws(i);

        if (! CheckedException(exception))
            continue;

        int k;
        for (k = hidden_method -> NumThrows() - 1; k >= 0; k--)
        {
            if (exception -> IsSubclass(hidden_method -> Throws(k)))
                break;
        }

        if (k < 0)
        {
            if (method -> containing_type == base_type)
            {
                if (base_type -> ACC_INTERFACE() &&
                    hidden_method -> containing_type == control.Object())
                {
                    //
                    // TODO: Review this when JLS3 is published.  See Sun bug
                    // 4479715, which explains our current stance of allowing
                    // int clone() throws MyException; or Object finalize();.
                    //
                    if (hidden_method -> ACC_PUBLIC())
                    {
                        ReportSemError(SemanticError::MISMATCHED_IMPLICIT_OVERRIDDEN_EXCEPTION,
                                       left_tok,
                                       right_tok,
                                       exception -> Name(),
                                       method -> Header());
                        base_type -> MarkBad();
                    }
                }
                else
                {
                    ReportSemError(SemanticError::MISMATCHED_OVERRIDDEN_EXCEPTION,
                                   left_tok,
                                   right_tok,
                                   exception -> Name(),
                                   hidden_method -> Header(),
                                   hidden_method -> containing_type -> ContainingPackage() -> PackageName(),
                                   hidden_method -> containing_type -> ExternalName());
                    base_type -> MarkBad();
                }
            }
            else
            {
                ReportSemError(SemanticError::MISMATCHED_OVERRIDDEN_EXCEPTION_EXTERNALLY,
                               left_tok,
                               right_tok,
                               base_type -> ExternalName(),
                               exception -> Name(),
                               method -> Header(),
                               method -> containing_type -> ContainingPackage() -> PackageName(),
                               method -> containing_type -> ExternalName(),
                               hidden_method -> Header(),
                               hidden_method -> containing_type -> ContainingPackage() -> PackageName(),
                               hidden_method -> containing_type -> ExternalName());
                base_type -> MarkBad();
            }
        }
    }
}


void Semantic::AddInheritedTypes(TypeSymbol *base_type, TypeSymbol *super_type)
{
    if (super_type -> Bad())
    {
        base_type -> MarkBad();
        return;
    }

    ExpandedTypeTable &base_expanded_table = *(base_type -> expanded_type_table),
                      &super_expanded_table = *(super_type -> expanded_type_table);

    for (int i = 0; i < super_expanded_table.symbol_pool.Length(); i++)
    {
        TypeShadowSymbol *type_shadow_symbol =
            super_expanded_table.symbol_pool[i];
        TypeSymbol *type = type_shadow_symbol -> type_symbol;

        //
        // Note that since all types in an interface are implicitly public,
        // all other types encountered here are enclosed in a type that is a
        // super class of base_type.
        //
        if (type -> ACC_PUBLIC() ||
            type -> ACC_PROTECTED() ||
            (! type -> ACC_PRIVATE() &&
             super_type -> ContainingPackage() == base_type -> ContainingPackage()))
        {
            TypeShadowSymbol *shadow =
                base_expanded_table.FindTypeShadowSymbol(type -> Identity());

            if (! shadow || shadow -> type_symbol -> owner != base_type)
            {
                if (! shadow)
                    shadow = base_expanded_table.InsertTypeShadowSymbol(type);
                else shadow -> AddConflict(type);

                assert(type -> owner != super_type ||
                       type_shadow_symbol -> NumConflicts() == 0);
                for (int j = 0; j < type_shadow_symbol -> NumConflicts(); j++)
                    shadow -> AddConflict(type_shadow_symbol -> Conflict(j));
            }
        }
        //
        // The main type was not accessible. But it may have been inherited
        // from yet another class, in which case any conflicts (which are
        // necessarily public types from interfaces) are still inherited in
        // the base_type.
        //
        else if (! type -> ACC_PRIVATE() &&
                 type_shadow_symbol -> NumConflicts() > 0)
        {
            assert(type -> owner != super_type);
            TypeShadowSymbol *shadow =
                base_expanded_table.FindTypeShadowSymbol(type -> Identity());

            if (shadow)
                assert(shadow -> type_symbol -> owner == base_type);
            else
            {
                shadow = base_expanded_table.
                    InsertTypeShadowSymbol(type_shadow_symbol -> Conflict(0));

                for (int k = 1; k < type_shadow_symbol -> NumConflicts(); k++)
                    shadow -> AddConflict(type_shadow_symbol -> Conflict(k));
            }
        }
    }
}


void Semantic::AddInheritedFields(TypeSymbol *base_type,
                                  TypeSymbol *super_type)
{
    if (super_type -> Bad())
    {
        base_type -> MarkBad();
        return;
    }

    ExpandedFieldTable &base_expanded_table = *(base_type -> expanded_field_table),
                       &super_expanded_table = *(super_type -> expanded_field_table);

    for (int i = 0; i < super_expanded_table.symbol_pool.Length(); i++)
    {
        VariableShadowSymbol *variable_shadow_symbol =
            super_expanded_table.symbol_pool[i];
        VariableSymbol *variable = variable_shadow_symbol -> variable_symbol;
        //
        // Note that since all fields in an interface are implicitly public,
        // all other fields encountered here are enclosed in a type that is a
        // super class of base_type.
        //
        if (variable -> ACC_PUBLIC() ||
            variable -> ACC_PROTECTED() ||
            (! variable -> ACC_PRIVATE() &&
             super_type -> ContainingPackage() == base_type -> ContainingPackage()))
        {
            VariableShadowSymbol *shadow = base_expanded_table.
                FindVariableShadowSymbol(variable -> Identity());

            if (! shadow || shadow -> variable_symbol -> owner != base_type)
            {
                if (! shadow)
                    shadow = base_expanded_table.
                        InsertVariableShadowSymbol(variable);
                else shadow -> AddConflict(variable);

                assert(variable -> owner != super_type ||
                       variable_shadow_symbol -> NumConflicts() == 0);
                for (int j = 0;
                     j < variable_shadow_symbol -> NumConflicts(); j++)
                {
                    shadow -> AddConflict(variable_shadow_symbol ->
                                          Conflict(j));
                }
            }
        }
        //
        // The main field was not accessible. But it may have been inherited
        // from yet another class, in which case any conflicts (which are
        // necessarily public fields from interfaces) are still inherited in
        // the base_type.
        //
        else if (! variable -> ACC_PRIVATE() &&
                 ! variable -> IsSynthetic() &&
                 variable_shadow_symbol -> NumConflicts() > 0)
        {
            assert(variable -> owner != super_type);
            VariableShadowSymbol *shadow = base_expanded_table.
                FindVariableShadowSymbol(variable -> Identity());

            if (shadow)
                assert(shadow -> variable_symbol -> owner == base_type);
            else
            {
                shadow = base_expanded_table.
                    InsertVariableShadowSymbol(variable_shadow_symbol ->
                                               Conflict(0));

                for (int k = 1;
                     k < variable_shadow_symbol -> NumConflicts(); k++)
                {
                    shadow -> AddConflict(variable_shadow_symbol ->
                                          Conflict(k));
                }
            }
        }
    }
}


void Semantic::AddInheritedMethods(TypeSymbol *base_type,
                                   TypeSymbol *super_type,
                                   LexStream::TokenIndex tok)
{
    if (super_type -> Bad())
    {
        base_type -> MarkBad();
        return;
    }

    ExpandedMethodTable *base_expanded_table =
        base_type -> expanded_method_table;
    ExpandedMethodTable *super_expanded_table =
        super_type -> expanded_method_table;
    PackageSymbol *base_package = base_type -> ContainingPackage();
    int i;

    for (i = 0; i < super_expanded_table -> symbol_pool.Length(); i++)
    {
        MethodShadowSymbol *method_shadow_symbol =
            super_expanded_table -> symbol_pool[i];
        MethodSymbol *method = method_shadow_symbol -> method_symbol;

        //
        // We have to special case interfaces, since they implicitly declare
        // the public methods of Object. In ComputeMethodsClosure, we add all
        // methods from Object after adding those from interfaces. Also, since
        // user code cannot invoke synthetic methods, we ignore those.
        //
        if ((base_type -> ACC_INTERFACE() &&
             super_type -> ACC_INTERFACE() &&
             method -> containing_type == control.Object()) ||
            method -> IsSynthetic())
        {
            continue;
        }

        //
        // Note that since all methods in an interface are implicitly
        // public, all other methods encountered here are enclosed in a
        // type that is a super class of base_type.
        //
        if (method -> ACC_PUBLIC() || method -> ACC_PROTECTED() ||
            (! method -> ACC_PRIVATE() &&
             super_type -> ContainingPackage() == base_package))
        {
            MethodShadowSymbol *shadow = base_expanded_table ->
                FindOverloadMethodShadow(method, this, tok);

            //
            // Check that method is compatible with every method it
            // overrides.
            //
            if (shadow)
            {
                CheckMethodOverride(shadow -> method_symbol, method,
                                    base_type);
                for (int m = 0; m < method_shadow_symbol -> NumConflicts(); m++)
                    CheckMethodOverride(shadow -> method_symbol,
                                        method_shadow_symbol -> Conflict(m),
                                        base_type);
            }

            if (! shadow ||
                shadow -> method_symbol -> containing_type != base_type)
            {
                if (! shadow)
                    shadow = base_expanded_table -> Overload(method);
                else shadow -> AddConflict(method);

                assert(method -> containing_type != super_type ||
                       method_shadow_symbol -> NumConflicts() == 0);
                for (int j = 0; j < method_shadow_symbol -> NumConflicts(); j++)
                    shadow -> AddConflict(method_shadow_symbol -> Conflict(j));
            }
        }
        //
        // The main method was not accessible. But it may have been inherited
        // from yet another class, in which case any conflicts (which are
        // necessarily public methods from interfaces) are still inherited in
        // the base_type.
        //
        else if (! method -> ACC_PRIVATE())
        {
            MethodShadowSymbol *shadow = base_expanded_table ->
                FindOverloadMethodShadow(method, this, tok);
            if (method_shadow_symbol -> NumConflicts() > 0)
            {
                assert(method -> containing_type != super_type);

                if (shadow)
                {
                    assert(shadow -> method_symbol -> containing_type == base_type);
                    for (int k = 0;
                         k < method_shadow_symbol -> NumConflicts(); k++)
                    {
                        CheckMethodOverride(shadow -> method_symbol,
                                            method_shadow_symbol -> Conflict(k),
                                            base_type);
                    }
                }
                else
                {
                    shadow = base_expanded_table ->
                        Overload(method_shadow_symbol -> Conflict(0));

                    for (int l = 1;
                         l < method_shadow_symbol -> NumConflicts(); l++)
                    {
                        shadow -> AddConflict(method_shadow_symbol ->
                                              Conflict(l));
                    }
                }
            }
            else if (shadow && control.option.pedantic)
            {
                //
                // The base_type declares a method by the same name as a
                // method in the superclass, but the new method does not
                // override or hide the old. Warn the user about this fact,
                // although it is usually not an error.
                //
                assert(shadow -> method_symbol -> containing_type == base_type);
                LexStream::TokenIndex left_tok,
                                      right_tok;

                if (ThisType() == base_type)
                {
                    AstMethodDeclaration *method_declaration =
                        (AstMethodDeclaration *) shadow -> method_symbol -> declaration;
                    AstMethodDeclarator *method_declarator =
                        method_declaration -> method_declarator;

                    left_tok = method_declarator -> LeftToken();
                    right_tok = method_declarator -> RightToken();
                }
                else
                {
                    AstInterfaceDeclaration *interface_declaration =
                        ThisType() -> declaration -> InterfaceDeclarationCast();
                    AstClassDeclaration *class_declaration =
                        ThisType() -> declaration -> ClassDeclarationCast();
                    if (interface_declaration)
                    {
                        left_tok = right_tok =
                            interface_declaration -> identifier_token;
                    }
                    else if (class_declaration)
                    {
                        left_tok = right_tok =
                            class_declaration -> identifier_token;
                    }
                    else
                    {
                        AstClassInstanceCreationExpression *class_creation =
                            ThisType() -> declaration -> ClassInstanceCreationExpressionCast();

                        assert(class_creation);

                        left_tok = class_creation -> class_type -> LeftToken();
                        right_tok = class_creation -> class_type -> RightToken();
                    }
                }

                if (! method -> IsTyped())
                    method -> ProcessMethodSignature(this, tok);

                //
                // We filter here, because CompleteSymbolTable gives a different
                // warning for unimplementable abstract classes.
                //
                if (! method -> ACC_ABSTRACT() ||
                    method -> Type() == shadow -> method_symbol -> Type() ||
                    (! shadow -> method_symbol -> ACC_PUBLIC() &&
                     ! shadow -> method_symbol -> ACC_PROTECTED()))
                {
                    ReportSemError(SemanticError::DEFAULT_METHOD_NOT_OVERRIDDEN,
                                   left_tok,
                                   right_tok,
                                   method -> Header(),
                                   base_type -> ContainingPackage() -> PackageName(),
                                   base_type -> ExternalName(),
                                   super_type -> ContainingPackage() -> PackageName(),
                                   super_type -> ExternalName());
                }
            }
        }
    }
    //
    // Now, we must ensure that any time the inheritance tree left and
    // reentered the package, the non-inherited default methods were
    // correctly overridden or hidden if redeclared in this class. A method
    // is non-inherited only if a class C is in the package, it's subclass
    // is not, and there is no interface method also inherited into C.
    //
    while (super_type -> super)
    {
        TypeSymbol *prev = super_type;
        super_type = super_type -> super;
        if (prev -> ContainingPackage() == base_package ||
            super_type -> ContainingPackage() != base_package)
        {
            continue;
        }
        super_expanded_table = super_type -> expanded_method_table;
        for (i = 0; i < super_expanded_table -> symbol_pool.Length(); i++)
        {
            MethodShadowSymbol *method_shadow_symbol =
                super_expanded_table -> symbol_pool[i];
            MethodSymbol *method = method_shadow_symbol -> method_symbol;
            if (! method -> ACC_PUBLIC() && ! method -> ACC_PROTECTED() &&
                ! method -> ACC_PRIVATE() && ! method -> IsSynthetic() &&
                method_shadow_symbol -> NumConflicts() == 0)
            {
                // found a non-inherited package scope method
                MethodShadowSymbol *shadow = base_expanded_table ->
                    FindOverloadMethodShadow(method, this, tok);
                if (shadow &&
                    shadow -> method_symbol -> containing_type == base_type)
                {
                    CheckMethodOverride(shadow -> method_symbol, method,
                                        base_type);
                }
            }
        }
    }
}


void Semantic::ComputeTypesClosure(TypeSymbol *type, LexStream::TokenIndex tok)
{
    if (! type -> HeaderProcessed())
    {
        AstClassDeclaration *class_decl =
            type -> declaration -> ClassDeclarationCast();
        AstInterfaceDeclaration *interface_decl =
            type -> declaration -> InterfaceDeclarationCast();
        Semantic *sem = type -> semantic_environment -> sem;
        if (class_decl)
            sem -> ProcessTypeHeaders(class_decl);
        else if (interface_decl)
            sem -> ProcessTypeHeaders(interface_decl);
        else assert(false && "type not processed");
    }
    type -> expanded_type_table = new ExpandedTypeTable();

    TypeSymbol *super_class = type -> super;
    if (super_class)
    {
        if (! super_class -> expanded_type_table)
            ComputeTypesClosure(super_class, tok);
    }

    for (int j = 0; j < type -> NumInterfaces(); j++)
    {
        TypeSymbol *interf = type -> Interface(j);
        if (! interf -> expanded_type_table)
            ComputeTypesClosure(interf, tok);
    }

    if (! type -> NestedTypesProcessed())
        type -> ProcessNestedTypeSignatures(this, tok);
    for (int i = 0; i < type -> NumTypeSymbols(); i++)
    {
        if (! type -> TypeSym(i) -> Bad())
            type -> expanded_type_table -> InsertTypeShadowSymbol(type -> TypeSym(i));
    }
    if (super_class)
        AddInheritedTypes(type, super_class);
    for (int k = 0; k < type -> NumInterfaces(); k++)
        AddInheritedTypes(type, type -> Interface(k));
    type -> expanded_type_table -> CompressSpace();
}


void Semantic::ComputeFieldsClosure(TypeSymbol *type,
                                    LexStream::TokenIndex tok)
{
    type -> expanded_field_table = new ExpandedFieldTable();

    TypeSymbol *super_class = type -> super;
    if (super_class)
    {
        if (! super_class -> expanded_field_table)
            ComputeFieldsClosure(super_class, tok);
    }

    for (int j = 0; j < type -> NumInterfaces(); j++)
    {
        TypeSymbol *interf = type -> Interface(j);
        if (! interf -> expanded_field_table)
            ComputeFieldsClosure(interf, tok);
    }

    assert(type -> FieldMembersProcessed());

    for (int i = 0; i < type -> NumVariableSymbols(); i++)
    {
        VariableSymbol *variable = type -> VariableSym(i);
        type -> expanded_field_table -> InsertVariableShadowSymbol(variable);
    }

    //
    // As the type Object which is the super type of all interfaces does
    // not contain any field declarations, we don't have to do any special
    // check here as we have to when computing method closures.
    //
    if (super_class)
        AddInheritedFields(type, super_class);
    for (int k = 0; k < type -> NumInterfaces(); k++)
        AddInheritedFields(type, type -> Interface(k));
    type -> expanded_field_table -> CompressSpace();
}


void Semantic::ComputeMethodsClosure(TypeSymbol *type,
                                     LexStream::TokenIndex tok)
{
    type -> expanded_method_table = new ExpandedMethodTable();

    TypeSymbol *super_class = type -> super;
    if (super_class)
    {
        if (! super_class -> expanded_method_table)
            ComputeMethodsClosure(super_class, tok);
    }

    for (int j = 0; j < type -> NumInterfaces(); j++)
    {
        TypeSymbol *interf = type -> Interface(j);

        if (! interf -> expanded_method_table)
            ComputeMethodsClosure(interf, tok);
    }

    assert(type -> MethodMembersProcessed());

    for (int i = 0; i < type -> NumMethodSymbols(); i++)
    {
        MethodSymbol *method = type -> MethodSym(i);
        //
        // If the method in question is neither a constructor nor an
        // initializer, then ...
        //
        if (*(method -> Name()) != U_LESS)
        {
            type -> expanded_method_table -> Overload(method);
        }
    }

    //
    // We build in this order to guarantee that the first method listed in
    // the table will be declared in a class.  Conflicts, if any, are from
    // interfaces and are necessarily abstract; but if the first method
    // is not abstract, it implements all the conflicts.
    //
    if (super_class && (! type -> ACC_INTERFACE()))
        AddInheritedMethods(type, super_class, tok);
    for (int k = 0; k < type -> NumInterfaces(); k++)
        AddInheritedMethods(type, type -> Interface(k), tok);
    if (type -> ACC_INTERFACE()) // the super class is Object
        AddInheritedMethods(type, control.Object(), tok);
    type -> expanded_method_table -> CompressSpace();
}


void Semantic::ProcessFormalParameters(BlockSymbol *block,
                                       AstMethodDeclarator *method_declarator)
{
    for (int i = 0; i < method_declarator -> NumFormalParameters(); i++)
    {
        AstFormalParameter *parameter = method_declarator -> FormalParameter(i);
        AstArrayType *array_type = parameter -> type -> ArrayTypeCast();
        Ast *actual_type =
            (array_type ? array_type -> type : parameter -> type);

        AccessFlags access_flags = ProcessFormalModifiers(parameter);

        AstPrimitiveType *primitive_type = actual_type -> PrimitiveTypeCast();
        TypeSymbol *parm_type = (primitive_type
                                 ? FindPrimitiveType(primitive_type)
                                 : MustFindType(actual_type));

        AstVariableDeclaratorId *name =
            parameter -> formal_declarator -> variable_declarator_name;
        NameSymbol *name_symbol =
            lex_stream -> NameSymbol(name -> identifier_token);
        VariableSymbol *symbol = block -> FindVariableSymbol(name_symbol);
        if (symbol)
        {
            ReportSemError(SemanticError::DUPLICATE_FORMAL_PARAMETER,
                           name -> identifier_token,
                           name -> identifier_token,
                           name_symbol -> Name());
        }
        else symbol = block -> InsertVariableSymbol(name_symbol);

        int num_dimensions = (array_type ? array_type -> NumBrackets() : 0) +
            name -> NumBrackets();
        if (num_dimensions == 0)
            symbol -> SetType(parm_type);
        else symbol -> SetType(parm_type -> GetArrayType(this, num_dimensions));
        symbol -> SetFlags(access_flags);
        symbol -> MarkComplete();
        symbol -> MarkInitialized();

        parameter -> formal_declarator -> symbol = symbol;
    }
}


void Semantic::ProcessMethodDeclaration(AstMethodDeclaration *method_declaration)
{
    TypeSymbol *this_type = ThisType();
    AccessFlags access_flags = this_type -> ACC_INTERFACE()
        ? ProcessInterfaceMethodModifiers(method_declaration)
        : ProcessMethodModifiers(method_declaration);

    //
    // By JLS2 8.4.3.3, a private method and all methods declared in a
    // final class are implicitly final. Also, all methods in a strictfp
    // class are strictfp.
    //
    if (access_flags.ACC_PRIVATE() || this_type -> ACC_FINAL())
        access_flags.SetACC_FINAL();
    if (this_type -> ACC_STRICTFP())
        access_flags.SetACC_STRICTFP();

    //
    // A method enclosed in an inner type may not be declared static.
    //
    if (access_flags.ACC_STATIC() && this_type -> IsInner())
    {
        AstModifier *modifier = NULL;
        for (int i = 0; i < method_declaration -> NumMethodModifiers(); i++)
        {
            if (method_declaration -> MethodModifier(i) -> kind == Ast::STATIC)
                modifier = method_declaration -> MethodModifier(i);
        }

        assert(modifier);

        ReportSemError(SemanticError::STATIC_METHOD_IN_INNER_CLASS,
                       modifier -> modifier_kind_token,
                       modifier -> modifier_kind_token,
                       lex_stream -> NameString(method_declaration -> method_declarator -> identifier_token),
                       this_type -> Name(),
                       this_type -> FileLoc());
    }

    //
    // To avoid deprecated type warnings when processing a deprecated method
    // declaration, we must temporarily mark this type as deprecated, because
    // the method symbol does not yet exist. We fix it after formal parameter
    // processing.
    //
    bool deprecated_method = lex_stream ->
        IsDeprecated(lex_stream -> Previous(method_declaration ->
                                            LeftToken()));
    bool deprecated_type = this_type -> IsDeprecated();
    if (deprecated_method)
        this_type -> MarkDeprecated();
    AstArrayType *array_type = method_declaration -> type -> ArrayTypeCast();
    Ast *actual_type = (array_type ? array_type -> type
                        : method_declaration -> type);
    AstPrimitiveType *primitive_type = actual_type -> PrimitiveTypeCast();
    TypeSymbol *method_type = (primitive_type
                               ? FindPrimitiveType(primitive_type)
                               : MustFindType(actual_type));

    AstMethodDeclarator *method_declarator =
        method_declaration -> method_declarator;
    if (method_declarator -> NumBrackets() > 0)
    {
        if (method_type == control.void_type)
            ReportSemError(SemanticError::VOID_ARRAY,
                           method_declaration -> type -> LeftToken(),
                           method_declarator -> RightToken());
        else ReportSemError(SemanticError::OBSOLESCENT_BRACKETS,
                            method_declarator -> LeftToken(),
                            method_declarator -> RightToken());
    }

    NameSymbol *name_symbol =
        lex_stream -> NameSymbol(method_declarator -> identifier_token);

    if (name_symbol == this_type -> Identity())
    {
        ReportSemError(SemanticError::METHOD_WITH_CONSTRUCTOR_NAME,
                       method_declaration -> type -> LeftToken(),
                       method_declarator -> identifier_token,
                       name_symbol -> Name());
    }

    //
    // As the body of the method may not have been parsed yet, we estimate a
    // size for its symbol table based on the number of lines in the body + a
    // margin for one-liners.
    //
    BlockSymbol *block_symbol =
        new BlockSymbol(method_declarator -> NumFormalParameters());
    block_symbol -> max_variable_index = (access_flags.ACC_STATIC() ? 0 : 1);
    ProcessFormalParameters(block_symbol, method_declarator);
    if (! deprecated_type && deprecated_method)
        this_type -> ResetDeprecated();

    MethodSymbol *method = this_type -> FindMethodSymbol(name_symbol);

    if (! method)
        method = this_type -> InsertMethodSymbol(name_symbol);
    else
    {
        if (this_type -> FindOverloadMethod(method, method_declarator))
        {
            ReportSemError(SemanticError::DUPLICATE_METHOD,
                           method_declarator -> LeftToken(),
                           method_declarator -> RightToken(),
                           name_symbol -> Name(),
                           this_type -> Name());
            delete block_symbol;
            return;
        }

        method = this_type -> Overload(method);
    }

    int num_dimensions = ((method_type == control.void_type)
                          ? 0
                          : (array_type ? array_type -> NumBrackets() : 0))
                         + method_declarator -> NumBrackets();
    if (num_dimensions == 0)
        method -> SetType(method_type);
    else method -> SetType(method_type -> GetArrayType(this, num_dimensions));

    //
    // if the method is not static, leave a slot for the "this" pointer.
    //
    method -> SetFlags(access_flags);
    method -> SetContainingType(this_type);
    method -> SetBlockSymbol(block_symbol);
    method -> declaration = method_declaration;
    for (int i = 0; i < method_declarator -> NumFormalParameters(); i++)
    {
        AstVariableDeclarator *formal_declarator =
            method_declarator -> FormalParameter(i) -> formal_declarator;
        VariableSymbol *symbol = formal_declarator -> symbol;

        symbol -> SetOwner(method);
        symbol -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
        symbol -> MarkComplete();
        if (control.IsDoubleWordType(symbol -> Type()))
            block_symbol -> max_variable_index++;
        symbol -> declarator = formal_declarator;
        method -> AddFormalParameter(symbol);
    }
    method -> SetSignature(control);

    for (int k = 0; k < method_declaration -> NumThrows(); k++)
    {
        AstExpression *throw_expression = method_declaration -> Throw(k);
        TypeSymbol *throw_type = MustFindType(throw_expression);
        throw_expression -> symbol = throw_type;
        method -> AddThrows(throw_type);
    }

    method_declaration -> method_symbol = method; // save for processing bodies later.

    if (method -> ACC_ABSTRACT() && ! this_type -> ACC_ABSTRACT())
    {
        ReportSemError(SemanticError::NON_ABSTRACT_TYPE_CONTAINS_ABSTRACT_METHOD,
                       method_declaration -> LeftToken(),
                       method_declarator -> identifier_token,
                       name_symbol -> Name(),
                       this_type -> Name());
    }

    if (deprecated_method)
        method -> MarkDeprecated();
}


//
// Return the type corresponding to a primitive type keyword.
//
TypeSymbol *Semantic::FindPrimitiveType(AstPrimitiveType *primitive_type)
{
    switch (primitive_type -> kind)
    {
    case Ast::INT:
        return control.int_type;
    case Ast::DOUBLE:
        return control.double_type;
    case Ast::CHAR:
        return control.char_type;
    case Ast::LONG:
        return control.long_type;
    case Ast::FLOAT:
        return control.float_type;
    case Ast::BYTE:
        return control.byte_type;
    case Ast::SHORT:
        return control.short_type;
    case Ast::BOOLEAN:
        return control.boolean_type;
    default:
        break;
    }

    return control.void_type;
}


//
// Search the import-on-demand locations for a type with the given name. This
// returns inaccessible types if found, with no error message, but favors
// accessible ones. It will issue an error if the only way an accessible type
// was found is non-canonical. If no type is found, NULL is returned.
//
TypeSymbol *Semantic::ImportType(LexStream::TokenIndex identifier_token,
                                 NameSymbol *name_symbol)
{
    //
    // To keep track of inaccessible types, we note the first one we find,
    // while leaving the location as NULL. Once we find an accessible type, we
    // set location, so that we know that future types are duplicates. We
    // pre-filtered duplicate import-on-demands, as well as adding java.lang.*.
    //
    TypeSymbol *type = NULL;
    PackageSymbol *location = NULL;

    for (int i = 0; i < import_on_demand_packages.Length(); i++)
    {
        PackageSymbol *import_package =
            import_on_demand_packages[i] -> PackageCast();
        TypeSymbol *possible_type = NULL;

        if (import_package)
        {
            possible_type = import_package -> FindTypeSymbol(name_symbol);
            if (! possible_type)
            {
                FileSymbol *file_symbol =
                    Control::GetFile(control, import_package, name_symbol);
                if (file_symbol)
                    possible_type = ReadType(file_symbol, import_package,
                                             name_symbol, identifier_token);
            }
            else if (possible_type -> SourcePending())
                control.ProcessHeaders(possible_type -> file_symbol);
        }
        else
        {
            TypeSymbol *import_type =
                (TypeSymbol *) import_on_demand_packages[i];
            if (! import_type -> expanded_type_table)
                ComputeTypesClosure(import_type, identifier_token);
            TypeShadowSymbol *type_shadow_symbol = import_type ->
                expanded_type_table -> FindTypeShadowSymbol(name_symbol);
            if (type_shadow_symbol)
            {
                //
                // Only canonical names may be used in import statements, hence
                // the extra filter on the containing type being correct. If
                // we encounter conflicts, they are necessarily accessible
                // inherited types from interfaces (and hence non-canonical).
                //
                possible_type = (type_shadow_symbol -> NumConflicts()
                                 ? type_shadow_symbol -> Conflict(0)
                                 : type_shadow_symbol -> type_symbol);
                if (! possible_type -> ACC_PRIVATE() &&
                    import_type == possible_type -> owner)
                {
                    import_package = import_type -> ContainingPackage();
                }
            }
        }
        if (possible_type)
        {
            if (location && import_package &&
                (possible_type -> ACC_PUBLIC() ||
                 import_package == this_package))
            {
                ReportSemError(SemanticError::DUPLICATE_ON_DEMAND_IMPORT,
                               identifier_token,
                               identifier_token,
                               name_symbol -> Name(),
                               location -> PackageName(),
                               import_package -> PackageName());
            }
            else
            {
                type = possible_type;
                if (type -> ACC_PUBLIC() || import_package == this_package)
                    location = import_package; // may be NULL
            }
        }
    }

    if (type && ! location && ! type -> ACC_PRIVATE() &&
        (type -> ACC_PUBLIC() || type -> ContainingPackage() == this_package))
    {
        ReportSemError(SemanticError::IMPORT_NOT_CANONICAL,
                       identifier_token, identifier_token,
                       type -> Name(),
                       type -> ContainingPackage() -> PackageName(),
                       type -> ExternalName());
    }
    return type;
}


//
// Finds an accessible type by the name located at identifier_token, or returns
// NULL. If there are ambiguous accessible types, this issues an error in the
// process. Note that inaccessible types are skipped - if the caller wishes
// to use an inaccessible type, they must search for it.
//
TypeSymbol *Semantic::FindType(LexStream::TokenIndex identifier_token)
{
    TypeSymbol *type;
    NameSymbol *name_symbol = lex_stream -> NameSymbol(identifier_token);

    SemanticEnvironment *env = NULL;
    if (state_stack.Size())
        env = state_stack.Top();
    for ( ; env; env = env -> previous)
    {
        // Search for local types, which are always accessible.
        type = env -> symbol_table.FindTypeSymbol(name_symbol);
        if (type)
            break;
        // Search for declared or inherited member types.
        type = env -> Type();
        if (! type -> expanded_type_table)
            ComputeTypesClosure(type, identifier_token);
        TypeShadowSymbol *type_shadow_symbol =
            type -> expanded_type_table -> FindTypeShadowSymbol(name_symbol);
        if (type_shadow_symbol)
        {
            type = FindTypeInShadow(type_shadow_symbol, identifier_token);
            if (type)
                break;
        }
    }

    if (env) // The type was found in some enclosing environment?
    {
        //
        // If the type was inherited, give a warning if it shadowed another
        // type of the same name within an enclosing lexical scope.
        //
        if (type -> owner -> TypeCast() && type -> owner != env -> Type())
        {
            TypeSymbol *supertype = (TypeSymbol *) type -> owner;
            for ( ; env; env = env -> previous)
            {
                //
                // First, check the enclosing type name - this is a caution,
                // because this behavior is opposite C++ when a type inherits
                // a membertype with the same name.
                //
                if (name_symbol == env -> Type() -> Identity() &&
                    env -> Type() != type)
                {
                    ReportSemError(SemanticError::INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_TYPE,
                                   identifier_token,
                                   identifier_token,
                                   lex_stream -> NameString(identifier_token),
                                   type -> ContainingPackage() -> PackageName(),
                                   type -> ExternalName(),
                                   env -> Type() -> ContainingPackage() -> PackageName(),
                                   env -> Type() -> ExternalName());
                    break;
                }
                if (env -> previous && control.option.pedantic)
                {
                    // Next, in pedantic mode, check local type
                    SemanticEnvironment *env2 = env -> previous;
                    TypeSymbol *outer_type =
                        env2 -> symbol_table.FindTypeSymbol(name_symbol);
                    if (outer_type)
                    {
                        assert(outer_type -> owner -> MethodCast());
                        ReportSemError(SemanticError::INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_LOCAL,
                                       identifier_token,
                                       identifier_token,
                                       lex_stream -> NameString(identifier_token),
                                       supertype -> ContainingPackage() -> PackageName(),
                                       supertype -> ExternalName(),
                                       ((MethodSymbol *) outer_type -> owner) -> Name());
                        break;
                    }
                    // If local type not found, check inner type.
                    if (! env2 -> Type() -> expanded_type_table)
                        ComputeTypesClosure(env2 -> Type(), identifier_token);
                    TypeShadowSymbol *type_shadow_symbol =
                        env2 -> Type() -> expanded_type_table ->
                        FindTypeShadowSymbol(name_symbol);
                    if (type_shadow_symbol)
                        outer_type = FindTypeInShadow(type_shadow_symbol,
                                                      identifier_token);
                    if (outer_type && outer_type != type &&
                        outer_type -> owner == env2 -> Type())
                    {
                        ReportSemError(SemanticError::INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER,
                                       identifier_token,
                                       identifier_token,
                                       lex_stream -> NameString(identifier_token),
                                       supertype -> ContainingPackage() -> PackageName(),
                                       supertype -> ExternalName(),
                                       env2 -> Type() -> ContainingPackage() -> PackageName(),
                                       env2 -> Type() -> ExternalName());
                        break;
                    }
                }
            }
        }

        return type;
    }

    //
    // Search for the type in the current compilation unit if it was declared as
    // a class or interface or imported by a single-type-import declaration.
    //
    for (int i = 0; i < single_type_imports.Length(); i++)
    {
        type = single_type_imports[i];
        if (name_symbol == type -> Identity())
            return type;
    }

    //
    // Search for another file in the current package, and if that fails, check
    // for an accessible import-on-demand.
    //
    type = FindSimpleNameType(this_package, identifier_token);
    TypeSymbol *imported_type = (! type || type -> Bad()
                                 ? ImportType(identifier_token, name_symbol)
                                 : (TypeSymbol *) NULL);

    //
    // If a valid type can be imported on demand, choose that type.
    // Otherwise, if a type was found at all, do some final checks on it.
    //
    // Note that a type T contained in a package P is always accessible to all
    // other types contained in P. I.e., we do not need to perform access check
    // for type...
    //
    if (imported_type && TypeAccessCheck(imported_type))
        type = imported_type;
    else if (type)
    {
        //
        // If a type T was specified in a source file that is not called T.java
        // but X.java (where X != T) and we are not currently compiling file X,
        // issue a warning to alert the user that in some circumstances, this
        // may not be visible. (i.e., if the file X has not yet been compiled,
        // then T is invisile as the compiler will only look for T in T.java.)
        //
        FileSymbol *file_symbol = type -> file_symbol;
        if (file_symbol && type -> Identity() != file_symbol -> Identity() &&
            file_symbol != this -> source_file_symbol)
        {
            ReportSemError(SemanticError::REFERENCE_TO_TYPE_IN_MISMATCHED_FILE,
                           identifier_token,
                           identifier_token,
                           type -> Name(),
                           file_symbol -> Name());
        }
    }

    return type;
}


//
// Finds a type by the given name, and add the dependence information. If one
// exists, but is not accessible, it is returned after an error. After other
// errors, control.no_type is returned.
//
TypeSymbol *Semantic::MustFindType(Ast *name)
{
    TypeSymbol *type;
    LexStream::TokenIndex identifier_token = name -> RightToken();
    NameSymbol *name_symbol = lex_stream -> NameSymbol(identifier_token);

    AstSimpleName *simple_name = name -> SimpleNameCast();
    if (simple_name)
    {
        type = FindType(identifier_token);

        //
        // If the type was not found, generate an appropriate error message.
        //
        if (! type)
        {
            // Check for inaccessible member type.
            if (state_stack.Size() > 0)
            {
                for (TypeSymbol *super_type = ThisType() -> super;
                     super_type; super_type = super_type -> super)
                {
                    assert(super_type -> expanded_type_table);
                    TypeShadowSymbol *type_shadow_symbol = super_type ->
                        expanded_type_table -> FindTypeShadowSymbol(name_symbol);
                    if (type_shadow_symbol)
                    {
                        type = type_shadow_symbol -> type_symbol;
                        break;
                    }
                }
            }
            // Check for an inaccessible import.
            if (! type)
                type = ImportType(identifier_token, name_symbol);
            // Report the error.
            if (type)
                ReportTypeInaccessible(name -> LeftToken(),
                                       name -> RightToken(), type);
            else
            {
                // Try reading the file again, to force an error.
                NameSymbol *name_symbol =
                    lex_stream -> NameSymbol(identifier_token);
                FileSymbol *file_symbol =
                    Control::GetFile(control, this_package, name_symbol);
                type = ReadType(file_symbol, this_package, name_symbol,
                                identifier_token);
            }
        }
    }
    else
    {
        AstFieldAccess *field_access = name -> FieldAccessCast();
        assert(field_access);
        identifier_token = field_access -> identifier_token;

        ProcessPackageOrType(field_access -> base);
        Symbol *symbol = field_access -> base -> symbol;

        type = symbol -> TypeCast();
        if (type)
        {
            type = MustFindNestedType(type, field_access);
        }
        else
        {
            PackageSymbol *package = symbol -> PackageCast();
            type = package -> FindTypeSymbol(name_symbol);
            if (! type)
            {
                FileSymbol *file_symbol =
                    Control::GetFile(control, package, name_symbol);
                type = ReadType(file_symbol, package, name_symbol,
                                identifier_token);
            }
            else if (type -> SourcePending())
                control.ProcessHeaders(type -> file_symbol);
            if (! TypeAccessCheck(type))
                ReportTypeInaccessible(name -> LeftToken(), identifier_token,
                                       type);
        }
    }

    //
    // Establish a dependence from the base_type to a type that it "must find".
    // Note that the only time an environment is not available is when were are
    // processing the type header of an outermost type.
    //
    if (state_stack.Size() > 0)
    {
        AddDependence(ThisType(), type);
        if (control.option.deprecation && type -> IsDeprecated() &&
            ! InDeprecatedContext())
        {
            ReportSemError(SemanticError::DEPRECATED_TYPE,
                           name -> LeftToken(),
                           name -> RightToken(),
                           type -> ContainingPackage() -> PackageName(),
                           type -> ExternalName());
        }
    }
    return type -> Bad() ? control.no_type : type;
}


//
// Initializes a static or instance field. In addition to checking the
// semantics, the initialization is added as a statement in the init_method,
// for easy bytecode emission, if it has an initializer and is not a static
// constant.
//
void Semantic::InitializeVariable(AstFieldDeclaration *field_declaration,
                                  MethodSymbol *init_method)
{
    ThisMethod() = NULL;
    AstMethodDeclaration *declaration =
        (AstMethodDeclaration *) init_method -> declaration;
    AstBlock *init_block = (AstBlock *) declaration -> method_body;

    for (int i = 0; i < field_declaration -> NumVariableDeclarators(); i++)
    {
        AstVariableDeclarator *variable_declarator =
            field_declaration -> VariableDeclarator(i);
        VariableSymbol *variable = variable_declarator -> symbol;

        if (variable)
        {
            ThisVariable() = variable;
            if (variable_declarator -> variable_initializer_opt)
            {
                variable_declarator -> pending = true;

                int start_num_errors = NumErrors();
                ProcessVariableInitializer(variable_declarator);
                if (NumErrors() == start_num_errors)
                {
                    DefiniteFieldInitializer(variable_declarator);
                    if (! variable -> ACC_STATIC() ||
                        ! variable -> initial_value)
                    {
                        init_block -> AddStatement(variable_declarator);
                    }
                }
                else if (variable -> ACC_FINAL())
                {
                    // Suppress further error messages.
                    DefinitelyAssignedVariables() ->
                        AssignElement(variable -> LocalVariableIndex());
                }

                variable_declarator -> pending = false;
            }
            variable -> MarkComplete();
        }
    }
}


//
// Adds an initializer block to the init_method, after checking its semantics,
// for easier bytecode emission.
//
inline void Semantic::ProcessInitializer(AstMethodBody *initializer,
                                         MethodSymbol *init_method)
{
    ThisVariable() = NULL;
    ThisMethod() = init_method;
    AstMethodDeclaration *declaration =
        (AstMethodDeclaration *) init_method -> declaration;
    AstBlock *init_block = (AstBlock *) declaration -> method_body;

    assert(initializer && init_block);

    LocalBlockStack().Push(init_block);
    LocalSymbolTable().Push(init_method -> block_symbol -> Table());

    //
    // Initializer blocks are always reachable, as prior blocks must be able
    // to complete normally.
    //
    initializer -> is_reachable = true;

    if (initializer -> explicit_constructor_opt)
        ReportSemError(SemanticError::MISPLACED_EXPLICIT_CONSTRUCTOR,
                       initializer -> explicit_constructor_opt -> LeftToken(),
                       initializer -> explicit_constructor_opt -> RightToken());
    ProcessBlock(initializer);
    DefiniteBlockInitializer(initializer, LocalBlockStack().max_size);

    init_block -> AddStatement(initializer);

    //
    // If the initializer has a higher max_variable_index than the overall
    // block, update max_variable_index in the init_block, accordingly.
    //
    if (init_method -> block_symbol -> max_variable_index <
        LocalBlockStack().TopMaxEnclosedVariableIndex())
    {
        init_method -> block_symbol -> max_variable_index =
            LocalBlockStack().TopMaxEnclosedVariableIndex();
    }

    if (! initializer -> can_complete_normally)
        ReportSemError(SemanticError::ABRUPT_INITIALIZER,
                       initializer -> LeftToken(),
                       initializer -> RightToken());

    LocalBlockStack().Pop();
    LocalSymbolTable().Pop();
}


//
// Lazily create and return the static initializer for this type. The estimate
// is for the number of initializers that will be grouped into this method.
// This is called both when processing static initializers, and any time an
// assert statement is encountered (since assertions require an initialized
// static variable to operate).
//
MethodSymbol *Semantic::GetStaticInitializerMethod(int estimate)
{
    TypeSymbol *this_type = ThisType();
    if (this_type -> static_initializer_method)
        return this_type -> static_initializer_method;

    StoragePool *ast_pool = compilation_unit -> ast_pool;
    LexStream::TokenIndex loc = this_type -> GetLocation();

    // The symbol table associated with this block has no elements.
    BlockSymbol *block_symbol = new BlockSymbol(0);
    block_symbol -> max_variable_index = 0;

    // The body of the static initializer. This will contain each initializer
    // block in sequence.
    AstMethodBody *block = ast_pool -> GenMethodBody();
    block -> left_brace_token = loc;
    block -> right_brace_token = loc;
    block -> block_symbol = block_symbol;
    block -> AllocateBlockStatements(estimate);

    // The return type (void).
    AstSimpleName *return_type = ast_pool -> GenSimpleName(loc);
    return_type -> symbol = control.void_type;

    // The method declaration. We leave some fields uninitialized, because
    // they are not needed in bytecode.cpp.
    AstMethodDeclaration *declaration = ast_pool -> GenMethodDeclaration();
    MethodSymbol *init_method =
        this_type -> InsertMethodSymbol(control.clinit_name_symbol);
    declaration -> type = return_type;
    declaration -> method_symbol = init_method;
    declaration -> method_body = block;

    // The method symbol.
    init_method -> SetType(control.void_type);
    init_method -> SetACC_PRIVATE();
    init_method -> SetACC_FINAL();
    init_method -> SetACC_STATIC();
    if (this_type -> ACC_STRICTFP())
        init_method -> SetACC_STRICTFP();
    init_method -> SetContainingType(this_type);
    init_method -> SetBlockSymbol(block_symbol);
    init_method -> SetSignature(control);
    init_method -> declaration = declaration;

    this_type -> static_initializer_method = init_method;
    return init_method;
}


void Semantic::ProcessStaticInitializers(AstClassBody *class_body)
{
    //
    // Notice that the bodies of methods have not been processed yet when this
    // is called.  If any method contains an assert, it will generate a static
    // initializer for the $noassert variable as needed.  On the other hand, if
    // we already encountered an assert statement in an instance initializer,
    // the static initializer already exists.  The assert variable initializer
    // is magically implemented by bytecode.cpp, rather than adding all the AST
    // structure to the block of the static initializer.
    //
    if (class_body -> NumStaticInitializers() == 0 &&
        class_body -> NumClassVariables() == 0)
    {
        return;
    }

    TypeSymbol *this_type = ThisType();
    LocalBlockStack().max_size = 1;
    assert(FinalFields());

    //
    // The static initializers and class variable initializers are executed
    // in textual order, with the exception that assignments may occur before
    // declaration. See JLS 8.5.
    //
    int j = 0,
        k = 0;
    int estimate = class_body -> NumClassVariables() +
        class_body -> NumStaticInitializers();
    MethodSymbol *init_method = GetStaticInitializerMethod(estimate);
    while (j < class_body -> NumClassVariables() &&
           k < class_body -> NumStaticInitializers())
    {
        //
        // Note that since there cannot be any overlap in the declarations,
        // we can use either location position. The RightToken of the field
        // declaration is used because it does not have to be computed (it
        // is the terminating semicolon). Similarly, the LeftToken of the
        // static initializer is used because it is the initial "static"
        // keyword that marked the initializer.
        //
        if (class_body -> ClassVariable(j) -> semicolon_token <
            class_body -> StaticInitializer(k) -> static_token)
        {
            InitializeVariable(class_body -> ClassVariable(j++),
                               init_method);
        }
        else
        {
            ProcessInitializer(class_body -> StaticInitializer(k++) -> block,
                               init_method);
        }
    }
    while (j < class_body -> NumClassVariables())
    {
        InitializeVariable(class_body -> ClassVariable(j++), init_method);
    }
    while (k < class_body -> NumStaticInitializers())
    {
        ProcessInitializer(class_body -> StaticInitializer(k++) -> block,
                           init_method);
    }

    //
    // Check that each static final variable has been initialized by now.
    // If not, issue an error and assume it is.  Notice that for inner
    // classes, we have already reported that a non-constant static
    // field is illegal, so we only need an error here for top-level
    // and static classes.
    //
    for (int l = 0; l < FinalFields() -> Length(); l++)
    {
        VariableSymbol *final_var = (*FinalFields())[l];
        if (final_var -> ACC_STATIC() &&
            ! DefinitelyAssignedVariables() -> da_set[l])
        {
            if (! this_type -> IsInner())
            {
                ReportSemError(SemanticError::UNINITIALIZED_STATIC_FINAL_VARIABLE,
                               final_var -> declarator -> LeftToken(),
                               final_var -> declarator -> RightToken(),
                               final_var -> Name());
            }
            DefinitelyAssignedVariables() -> AssignElement(l);
        }
    }

    //
    // If an initialization method has been defined, update its
    // max_block_depth.
    //
    if (this_type -> static_initializer_method)
    {
        MethodSymbol *init_method = this_type -> static_initializer_method;
        init_method -> max_block_depth = LocalBlockStack().max_size;
        init_method -> block_symbol -> CompressSpace(); // space optimization
    }
}


void Semantic::ProcessInstanceInitializers(AstClassBody *class_body)
{
    //
    // For instance initializers, we create a method to do all the
    // initialization. We name the method 'this', which is legal in VM's
    // but an illegal user name, to avoid name clashes. Constructors which
    // do not invoke another constructor via the this() statement will call
    // the instance initializer method after calling super(). We rely on the
    // fact that VM's allow assignment of final instance variables in an
    // instance method, rather than requiring it to be in a constructor.
    //
    if (class_body -> NumInstanceInitializers() == 0 &&
        class_body -> NumInstanceVariables() == 0)
    {
        return;
    }

    TypeSymbol *this_type = ThisType();
    LocalBlockStack().max_size = 1;
    StoragePool *ast_pool = compilation_unit -> ast_pool;
    LexStream::TokenIndex loc = this_type -> GetLocation();

    // The symbol table associated with this block has one element, the
    // current instance 'this'.
    BlockSymbol *block_symbol = new BlockSymbol(1);
    block_symbol -> max_variable_index = 1;

    // The combined block of the instance initializations. This will contain
    // each initializer block in sequence, and be inlined into constructors.
    AstMethodBody *block = ast_pool -> GenMethodBody();
    block -> left_brace_token = loc;
    block -> right_brace_token = loc;
    block -> block_symbol = block_symbol;

    // The return type (void).
    AstSimpleName *return_type = ast_pool -> GenSimpleName(loc);
    return_type -> symbol = control.void_type;

    // The method declaration. We leave some fields uninitialized, because
    // they are not needed in bytecode.cpp.
    AstMethodDeclaration *declaration = ast_pool -> GenMethodDeclaration();
    MethodSymbol *init_method =
        this_type -> InsertMethodSymbol(control.block_init_name_symbol);
    declaration -> type = return_type;
    declaration -> method_symbol = init_method;
    declaration -> method_body = block;

    // The method symbol.
    init_method -> SetType(control.void_type);
    init_method -> SetACC_PRIVATE();
    init_method -> SetACC_FINAL();
    if (this_type -> ACC_STRICTFP())
        init_method -> SetACC_STRICTFP();
    init_method -> MarkSynthetic();
    init_method -> SetContainingType(this_type);
    init_method -> SetBlockSymbol(block_symbol);
    init_method -> SetSignature(control);
    init_method -> declaration = declaration;

    assert(this_type -> instance_initializer_method == NULL);
    this_type -> instance_initializer_method = init_method;

    //
    // Make sure the instance final fields are properly set.
    //
    assert(FinalFields());
    for (int i = 0; i < FinalFields() -> Length(); i++)
    {
        VariableSymbol *variable_symbol = (*FinalFields())[i];
        if (variable_symbol -> ACC_STATIC())
        {
            DefinitelyAssignedVariables() -> AssignElement(i);
            BlankFinals() -> RemoveElement(i);
        }
        else
            DefinitelyAssignedVariables() -> ReclaimElement(i);
    }

    //
    // Initialization code is executed by every constructor, just after the
    // superclass constructor is called, in textual order along with any
    // instance variable initializations.
    //
    int j = 0,
        k = 0;
    int estimate = class_body -> NumInstanceVariables() +
        class_body -> NumInstanceInitializers();
    block -> AllocateBlockStatements(estimate);
    while (j < class_body -> NumInstanceVariables() &&
           k < class_body -> NumInstanceInitializers())
    {
        //
        // Note that since there cannot be any overlap in the declarations,
        // we can use either location position. The RightToken of the field
        // declaration is used because it does not have to be computed (it
        // is the terminating semicolon). Similarly, the LeftToken of the
        // instance initializer is used because it is the initial "{".
        //
        if (class_body -> InstanceVariable(j) -> semicolon_token <
            class_body -> InstanceInitializer(k) -> left_brace_token)
        {
            InitializeVariable(class_body -> InstanceVariable(j++),
                               init_method);
        }
        else
        {
            ProcessInitializer(class_body -> InstanceInitializer(k++),
                               init_method);
        }
    }
    while (j < class_body -> NumInstanceVariables())
    {
        InitializeVariable(class_body -> InstanceVariable(j++), init_method);
    }
    while (k < class_body -> NumInstanceInitializers())
    {
        ProcessInitializer(class_body -> InstanceInitializer(k++),
                           init_method);
    }

    //
    // Note that unlike the case of static fields, we do not ensure here that
    // each final instance variable has been initialized at this point. This
    // is because the user may choose instead to initialize such a final
    // variable in every constructor instead. See body.cpp
    //
    init_method -> max_block_depth = LocalBlockStack().max_size;
    init_method -> block_symbol -> CompressSpace(); // space optimization
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

