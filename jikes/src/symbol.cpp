// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, 1999, 2000, 2001 International Business
// Machines Corporation and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#include "symbol.h"
#include "stream.h"
#include "control.h"
#include "ast.h"
#include "semantic.h"
#include "table.h"
#include "zip.h"
#include "set.h"
#include "case.h"

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif

char *FileSymbol::java_suffix = StringConstant::U8S_DO_java;
int FileSymbol::java_suffix_length = strlen(java_suffix);
char *FileSymbol::class_suffix = StringConstant::U8S_DO_class;
int FileSymbol::class_suffix_length = strlen(class_suffix);

bool MethodSymbol::IsFinal()
{
    return ((AccessFlags *) this) -> ACC_FINAL() || ((AccessFlags *) this) -> ACC_PRIVATE() || containing_type -> ACC_FINAL();
}

wchar_t *MethodSymbol::Header()
{
    assert(type_);

    if (! header)
    {
        bool is_constructor = false;
        for (MethodSymbol *constr = containing_type -> FindConstructorSymbol(); constr; constr = constr -> next_method)
        {
            if (this == constr)
            {
                is_constructor = true;
                break;
            }
        }

        int length = (Type() -> ContainingPackage() -> PackageNameLength() +
                      Type() -> ExternalNameLength() +
                      (is_constructor ? containing_type -> NameLength() : NameLength())
                      + 5); // '/' after package_name, ' ' after type,
                            // '(' after name, ')' after all parameters,
                            // ';' to terminate
        for (int i = 0; i < NumFormalParameters(); i++)
        {
            VariableSymbol *formal = FormalParameter(i);
            length += (formal -> Type() -> ContainingPackage() -> PackageNameLength() +
                       formal -> Type() -> ExternalNameLength() +
                       formal -> NameLength() + 4); // '/' after package_name
                                                    // ' ' after type
                                                    // ',' and ' ' to separate this formal parameter from the next one
        }

        header = new wchar_t[length + 1]; // +1 for '\0'
        wchar_t *s = header;

        if (is_constructor)
        {
            for (wchar_t *s2 = containing_type -> Name(); *s2; s2++)
                 *s++ = *s2;
        }
        else
        {
            PackageSymbol *package = Type() -> ContainingPackage();
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

            for (wchar_t *s2 = Type() -> ExternalName(); *s2; s2++)
                 *s++ = *s2;
            *s++ = U_SPACE;
            for (wchar_t *s3 = Name(); *s3; s3++)
                 *s++ = *s3;
        }
        *s++ = U_LEFT_PARENTHESIS;
        if (NumFormalParameters() > 0)
        {
            for (int k = 0; k < NumFormalParameters(); k++)
            {
                VariableSymbol *formal = FormalParameter(k);

                PackageSymbol *package = formal -> Type() -> ContainingPackage();
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

                for (wchar_t *s2 = formal -> Type() -> ExternalName(); *s2; s2++)
                    *s++ = *s2;
                *s++ = U_SPACE;
                for (wchar_t *s3 = formal -> Name(); *s3; s3++)
                     *s++ = *s3;
                *s++ = U_COMMA;
                *s++ = U_SPACE;
            }

            s -= 2; // remove the last ',' and ' '
        }
        *s++ = U_RIGHT_PARENTHESIS;
        *s++ = U_SEMICOLON;
        *s = U_NULL;

        assert((s - header) <= length);
    }

    return header;
}


MethodSymbol *SymbolTable::FindOverloadMethod(MethodSymbol *base_method, AstMethodDeclarator *method_declarator)
{
    for (MethodSymbol *method = base_method; method; method = method -> next_method)
    {
        assert(method -> IsTyped());

        if (method -> NumFormalParameters() == method_declarator -> NumFormalParameters())
        {
            int i;
            for (i = method -> NumFormalParameters() - 1; i >= 0; i--)
            {
                AstFormalParameter *parameter = method_declarator -> FormalParameter(i);
                if (method -> FormalParameter(i) -> Type() != parameter -> formal_declarator -> symbol -> Type())
                    break;
            }
            if (i < 0)
                return method;
        }
    }

    return (MethodSymbol *) NULL;
}


void TypeSymbol::ProcessTypeHeaders()
{
    AstClassDeclaration *class_declaration = declaration -> ClassDeclarationCast();

    if (class_declaration)
         semantic_environment -> sem -> ProcessTypeHeaders(class_declaration);
    else semantic_environment -> sem -> ProcessTypeHeaders((AstInterfaceDeclaration *) declaration);
}

void TypeSymbol::ProcessMembers()
{
    AstClassDeclaration *class_declaration = declaration -> ClassDeclarationCast();

    if (class_declaration)
         semantic_environment -> sem -> ProcessMembers(class_declaration -> semantic_environment, class_declaration -> class_body);
    else semantic_environment -> sem -> ProcessMembers((AstInterfaceDeclaration *) declaration);
}

void TypeSymbol::CompleteSymbolTable()
{
    AstClassDeclaration *class_declaration = declaration -> ClassDeclarationCast();

    if (class_declaration)
         semantic_environment -> sem -> CompleteSymbolTable(class_declaration -> semantic_environment,
                                                            class_declaration -> identifier_token, class_declaration -> class_body);
    else semantic_environment -> sem -> CompleteSymbolTable((AstInterfaceDeclaration *) declaration);
}


void TypeSymbol::ProcessExecutableBodies()
{
    AstClassDeclaration *class_declaration = declaration -> ClassDeclarationCast();

    if (class_declaration)
         semantic_environment -> sem -> ProcessExecutableBodies(class_declaration -> semantic_environment,
                                                                class_declaration -> class_body);
    else semantic_environment -> sem -> ProcessExecutableBodies((AstInterfaceDeclaration *) declaration);
}


void TypeSymbol::RemoveCompilationReferences()
{
    if (semantic_environment)
    {
        semantic_environment = NULL;
        declaration = NULL;

        //
        // TODO: What else needs to be reset?
        //
        if (table)
        {
            for (int i = 0; i < table -> NumVariableSymbols(); i++)
                table -> VariableSym(i) -> declarator = NULL;

            for (int j = 0; j < table -> NumMethodSymbols(); j++)
                table -> MethodSym(j) -> method_or_constructor_declaration = NULL;

            for (int k = 0; k < table -> NumTypeSymbols(); k++)
                table -> TypeSym(k) -> declaration = NULL;

            for (int l = 0; l < table -> NumAnonymousSymbols(); l++)
                table -> AnonymousSym(l) -> declaration = NULL;
        }
    }
}


TypeSymbol *TypeSymbol::GetArrayType(Semantic *sem, int num_dimensions_)
{
    if (num_dimensions_ == 0)
        return this;
    if (num_dimensions_ < NumArrays())
        return Array(num_dimensions_);

    if (NumArrays() == 0)
        AddArrayType(this);

    TypeSymbol *previous_array_type = Array(array -> Length() - 1);
    wchar_t *name = new wchar_t[ExternalNameLength() + (num_dimensions_ * 2) + 1];
    wcscpy(name, previous_array_type -> ExternalName());

    for (int num = array -> Length(), len = previous_array_type -> ExternalNameLength() + 2;
         num <= num_dimensions_;
         num++, len = len + 2)
    {
        wcscat(name, StringConstant::US_LB_RB);
        NameSymbol *name_sym = sem -> control.FindOrInsertName(name, len);

        TypeSymbol *type = new TypeSymbol(name_sym);

        type -> MarkHeaderProcessed();
        type -> MarkConstructorMembersProcessed();
        type -> MarkMethodMembersProcessed();
        type -> MarkFieldMembersProcessed();
        type -> MarkLocalClassProcessingCompleted();
        type -> MarkSourceNoLongerPending();
        type -> outermost_type = type;

        //
        // An array type has the same accessibility as its component.
        //
        if (ACC_PUBLIC())
          type -> SetACC_PUBLIC();
        else if (ACC_PROTECTED())
          type -> SetACC_PROTECTED();
        else if (ACC_PRIVATE())
          type -> SetACC_PRIVATE();
        type -> SetACC_FINAL();

        type -> super = sem -> control.Object();
        //
        // All arrays implement the interfaces java.io.Serializable and
        // java.io.Cloneable
        //
        type -> AddInterface(sem -> control.Serializable());
        type -> AddInterface(sem -> control.Cloneable());
        type -> base_type = this;
        type -> num_dimensions = num;
        type -> SetOwner(ContainingPackage());
        type -> table = new SymbolTable(2); // only 2 elements will be added to this table
        type -> SetSignature(sem -> control);

        MethodSymbol *method = type -> InsertMethodSymbol(sem -> control.clone_name_symbol);
        method -> SetType(sem -> control.Object());
        method -> SetContainingType(type);
        method -> SetACC_PUBLIC();
        method -> SetACC_FINAL();
        method -> SetBlockSymbol(new BlockSymbol(1)); // the associated symbol table will remain empty
        method -> SetSignature(sem -> control);

        VariableSymbol *symbol = type -> InsertVariableSymbol(sem -> control.length_name_symbol);
        symbol -> SetACC_PUBLIC();
        symbol -> SetACC_FINAL();
        symbol -> SetOwner(type);
        symbol -> SetType(sem -> control.int_type);

        type -> CompressSpace(); // space optimization

        AddArrayType(type);
    }

    delete [] name;
    return Array(num_dimensions_);
}

void TypeSymbol::SetLocation()
{
    if (! declaration)
        file_location = new FileLocation(file_symbol);
    else
    {
        AstClassDeclaration *class_declaration = declaration -> ClassDeclarationCast();
        AstInterfaceDeclaration *interface_declaration = declaration -> InterfaceDeclarationCast();

        file_location = new FileLocation(semantic_environment -> sem -> lex_stream,
                                         (class_declaration
                                               ? class_declaration -> identifier_token
                                               : (interface_declaration
                                                           ? interface_declaration -> identifier_token
                                                           : ((AstClassInstanceCreationExpression *) declaration)
                                                                                           -> class_body_opt -> left_brace_token)));
    }
}


void TypeSymbol::SetSignature(Control &control)
{
    if (num_dimensions > 0)
    {
        char *type_signature;
        TypeSymbol *subtype = ArraySubtype();
        int signature_len = strlen(subtype -> SignatureString()) + 1; // +1 for '['
        type_signature = new char[signature_len + 1];                  // +1 for '\0'
        type_signature[0] = U_LEFT_BRACKET;
        strcpy(type_signature + 1, subtype -> SignatureString());
        signature = control.Utf8_pool.FindOrInsert(type_signature, signature_len);
        fully_qualified_name = signature;
        delete [] type_signature;
    }
    else
    {
        wchar_t *package_name = ContainingPackage() -> PackageName();
        wchar_t *type_name = ExternalName();

        int len = ContainingPackage() -> PackageNameLength() +
                  ExternalNameLength() + 4; // +1 for 'L' +1 for '/' +1 for ';' +1 for '\0'
        wchar_t *type_signature = new wchar_t[len];
        wcscpy(type_signature, StringConstant::US_L);
        if (ContainingPackage() -> PackageNameLength() > 0 &&
            wcscmp(package_name, StringConstant::US_DO) != 0)
        {
            wcscat(type_signature, package_name);
            wcscat(type_signature, StringConstant::US_SL);
        }
        wcscat(type_signature, type_name);
        fully_qualified_name = control.ConvertUnicodeToUtf8(type_signature + 1); // +1 to skip the initial L'L'

        wcscat(type_signature, StringConstant::US_SC);
        signature = control.ConvertUnicodeToUtf8(type_signature);

        delete [] type_signature;

        if (! (Anonymous() || IsLocal()))
            control.type_table.InsertType((TypeSymbol *) this);
    }
}


int SymbolTable::primes[] = {DEFAULT_HASH_SIZE, 101, 401, MAX_HASH_SIZE};

void SymbolTable::Rehash()
{
    hash_size = primes[++prime_index];

    delete [] base;
    base = (Symbol **) memset(new Symbol *[hash_size], 0, hash_size * sizeof(Symbol *));

    int k;
    for (k = 0; k < NumTypeSymbols(); k++)
    {
        TypeSymbol *symbol = TypeSym(k);
        int i = symbol -> name_symbol -> index % hash_size;
        symbol -> next = base[i];
        base[i] = symbol;
    }

    for (k = 0; k < NumMethodSymbols(); k++)
    {
        MethodSymbol *symbol = MethodSym(k);
        if (symbol -> next != symbol) // not an overload
        {
            int i = symbol -> name_symbol -> index % hash_size;
            symbol -> next = base[i];
            base[i] = symbol;
        }
    }

    for (k = 0; k < NumVariableSymbols(); k++)
    {
        VariableSymbol *symbol = VariableSym(k);
        int i = symbol -> name_symbol -> index % hash_size;
        symbol -> next = base[i];
        base[i] = symbol;
    }

    for (k = 0; k < NumOtherSymbols(); k++)
    {
        Symbol *symbol = OtherSym(k);

        if (! symbol -> BlockCast())
        {
            int i = symbol -> Identity() -> index % hash_size;
            symbol -> next = base[i];
            base[i] = symbol;
        }
    }
}


SymbolTable::SymbolTable(int hash_size_) : type_symbol_pool(NULL),
                                           anonymous_symbol_pool(NULL),
                                           method_symbol_pool(NULL),
                                           variable_symbol_pool(NULL),
                                           other_symbol_pool(NULL),
                                           constructor(NULL)
{
    hash_size = (hash_size_ <= 0 ? 1 : hash_size_);

    prime_index = -1;
    do
    {
        if (hash_size < primes[prime_index + 1])
            break;
        prime_index++;
    } while (primes[prime_index] < MAX_HASH_SIZE);

    base = (Symbol **) memset(new Symbol *[hash_size], 0, hash_size * sizeof(Symbol *));
}

SymbolTable::~SymbolTable()
{
/*
int n;
int num_slots = 0;
int total = 0;
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (Symbol *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
{
num_slots++;
total += num;
}
}

if (num_slots > 0)
{
cout << "\nDestroying a symbol table with base size " << hash_size << " containing " << num_slots << " non-empty slots\n";
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (Symbol *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
cout << "    slot " << n << " contains " << num << " element(s)\n";
}
}
if (hash_size < total)
    total = total;
*/
    for (int i = 0; i < NumAnonymousSymbols(); i++)
        delete AnonymousSym(i);
    delete anonymous_symbol_pool;
    for (int j = 0; j < NumTypeSymbols(); j++)
        delete TypeSym(j);
    delete type_symbol_pool;
    for (int k = 0; k < NumMethodSymbols(); k++)
        delete MethodSym(k);
    delete method_symbol_pool;
    for (int l = 0; l < NumVariableSymbols(); l++)
        delete VariableSym(l);
    delete variable_symbol_pool;
    for (int m = 0; m < NumOtherSymbols(); m++)
        delete OtherSym(m);
    delete other_symbol_pool;
    delete [] base;
}


PackageSymbol::~PackageSymbol()
{
    delete [] package_name;
    delete table;
}


void PackageSymbol::SetPackageName()
{
    package_name_length = (owner ? owner -> PackageNameLength() + 1 : 0) + NameLength(); // +1 for '/'
    package_name = new wchar_t[package_name_length + 1]; // +1 for '\0'

    if (owner)
    {
        wcscpy(package_name, owner -> PackageName());
        wcscat(package_name, StringConstant::US_SL);
    }
    else package_name[0] = U_NULL;
    wcscat(package_name, Name());

    assert(wcslen(package_name) == package_name_length);
}


TypeSymbol::TypeSymbol(NameSymbol *name_symbol_) : semantic_environment(NULL),
                                                   declaration(NULL),
                                                   file_symbol(NULL),
                                                   file_location(NULL),
                                                   name_symbol(name_symbol_),
                                                   owner(NULL),
                                                   outermost_type(NULL),
                                                   super(NULL),
                                                   base_type(NULL),
                                                   index(CycleChecker::OMEGA),
                                                   unit_index(CycleChecker::OMEGA),
                                                   incremental_index(CycleChecker::OMEGA),
                                                   local(NULL),
                                                   non_local(NULL),
                                                   supertypes_closure(NULL),
                                                   subtypes(NULL),
                                                   subtypes_closure(NULL),
                                                   innertypes_closure(NULL),
                                                   dependents(new SymbolSet()),
                                                   parents(new SymbolSet()),
                                                   static_parents(new SymbolSet()),
                                                   dependents_closure(NULL),
                                                   parents_closure(NULL),
                                                   signature(NULL),
                                                   fully_qualified_name(NULL),
                                                   expanded_type_table(NULL),
                                                   expanded_field_table(NULL),
                                                   expanded_method_table(NULL),
                                                   num_dimensions(0),
                                                   static_initializer_method(NULL),
                                                   block_initializer_method(NULL),
                                                   external_name_symbol(NULL),
                                                   table(NULL),
                                                   local_shadow_map(NULL),
                                                   status(0),
                                                   package(NULL),
                                                   class_name(NULL),
                                                   class_literal_class(NULL),
                                                   class_literal_method(NULL),
                                                   class_literal_name(NULL),
                                                   assert_variable(NULL),
                                                   local_constructor_call_environments(NULL),
                                                   private_access_methods(NULL),
                                                   private_access_constructors(NULL),
                                                   read_methods(NULL),
                                                   write_methods(NULL),
                                                   constructor_parameters(NULL),
                                                   generated_constructors(NULL),
                                                   enclosing_instances(NULL),
                                                   class_literals(NULL),
                                                   nested_type_signatures(NULL),
                                                   nested_types(NULL),
                                                   interfaces(NULL),
                                                   anonymous_types(NULL),
                                                   array(NULL)
{
    Symbol::_kind = TYPE;
}


TypeSymbol::~TypeSymbol()
{
    if (read_methods) delete read_methods;
    if (write_methods) delete write_methods;
    delete semantic_environment;
    delete local;
    delete non_local;
    delete supertypes_closure;
    delete subtypes;
    delete subtypes_closure;
    delete innertypes_closure;
    delete dependents;
    delete parents;
    delete static_parents;
    delete table;
    delete local_shadow_map;
    delete expanded_type_table;
    delete expanded_field_table;
    delete expanded_method_table;
    delete file_location;
    delete [] class_name;
    for (int i = 1; i < NumArrays(); i++)
        delete Array(i);
    for (int k = 0; k < NumNestedTypeSignatures(); k++)
        delete [] NestedTypeSignature(k);
    delete nested_type_signatures;

    delete local_constructor_call_environments;
    delete private_access_methods;
    delete private_access_constructors;
    delete constructor_parameters;
    delete generated_constructors;
    delete enclosing_instances;
    delete class_literals;
    delete nested_types;
    delete interfaces;
    delete anonymous_types;
    delete array;
}

MethodSymbol::~MethodSymbol()
{
    for (int i = 0; i < NumThrowsSignatures(); i++)
        delete [] ThrowsSignature(i);
    delete throws_signatures;
    delete formal_parameters;
    delete throws;
    delete initializer_constructors;

    delete block_symbol; // overload(s)
    delete [] header;
}


BlockSymbol::BlockSymbol(int hash_size) : max_variable_index(-1),
                                          try_or_synchronized_variable_index(0),
                                          table(hash_size > 0 ? new SymbolTable(hash_size) : (SymbolTable *) NULL)
{
    Symbol::_kind = BLOCK;
}

BlockSymbol::~BlockSymbol()
{
    delete table;
}

PathSymbol::PathSymbol(NameSymbol *name_symbol_) : name_symbol(name_symbol_),
                                                   zipfile(NULL)
{
    Symbol::_kind = PATH;
}

PathSymbol::~PathSymbol()
{
    if (zipfile)
        delete zipfile;
}

DirectorySymbol::DirectorySymbol(NameSymbol *name_symbol_, Symbol *owner_, bool source_dir_only_) : owner(owner_),
                                                                             name_symbol(name_symbol_),
                                                                             mtime(0),
                                                                             table(NULL),
                                                                             entries(NULL),
                                                                             directory_name(NULL),
                                                                             source_dir_only(source_dir_only_)
{
    Symbol::_kind = _DIRECTORY;
}




DirectorySymbol::~DirectorySymbol()
{
    delete [] directory_name;
    delete entries;
    delete table;
}

void DirectorySymbol::SetDirectoryName()
{
    PathSymbol *path_symbol = owner -> PathCast();
    if (path_symbol)
    {
        if (strcmp(path_symbol -> Utf8Name(), StringConstant::U8S_DO) == 0)
        {
            directory_name_length = Utf8NameLength();
            directory_name = new char[directory_name_length + 1]; // +1 for '\0'

            strcpy(directory_name, Utf8Name());
        }
        else
        {
            directory_name_length = path_symbol -> Utf8NameLength();
            directory_name = new char[directory_name_length + 1]; // +1 for '\0'

            strcpy(directory_name, path_symbol -> Utf8Name());
        }
    }
    else
    {
        DirectorySymbol *owner_directory = owner -> DirectoryCast();
        if (Name()[NameLength() - 1] == U_SLASH ||                     // An absolute file name ?
            strcmp(owner_directory -> DirectoryName(),
                   StringConstant::U8S_DO) == 0) // or is the owner "." ?
        {
            directory_name_length = Utf8NameLength();
            directory_name = new char[directory_name_length + 1];  // +1 for '\0'
            strcpy(directory_name, Utf8Name());
        }
        else
        {
            int owner_length = owner_directory -> DirectoryNameLength();
            char *owner_name = owner_directory -> DirectoryName();
            directory_name_length = owner_length +
                                            Utf8NameLength() +
                                            (owner_name[owner_length - 1] != U_SLASH ? 1 : 0); // +1 for '/'

            directory_name = new char[directory_name_length + 1]; // +1 for '\0'

            strcpy(directory_name, owner_directory -> DirectoryName());
            if (owner_name[owner_length - 1] != U_SLASH)
                strcat(directory_name, StringConstant::U8S_SL);
            strcat(directory_name, Utf8Name());
        }
    }

    assert(strlen(directory_name) == directory_name_length);
}


void DirectorySymbol::ResetDirectory()
{
    //
    // TODO: the stat function does not work for directories in that the
    // "modified" time stamp associated with a directory is not updated when
    // a file contained in the directory is changed.
    // For now, we always reread the directory.
    //
    //    struct stat status;
    //    if ((SystemStat(DirectoryName(), &status) == 0) && status.st_mtime > mtime)
    //    {
    //        mtime = status.st_mtime;
    //
    //        delete entries;
    //        entries = NULL;
    //    }
    //
    //    ReadDirectory();
    //

    delete entries;
    entries = NULL;

    ReadDirectory();
}


void DirectorySymbol::ReadDirectory()
{
    assert(! IsZip());

    if (! entries)
    {
        entries = new DirectoryTable();

//FIXME: these need to go into platform.cpp
#ifdef UNIX_FILE_SYSTEM
        DIR *directory = opendir(DirectoryName());
        if (directory)
        {
            for (dirent *entry = readdir(directory); entry; entry = readdir(directory))
            {
                int length = strlen(entry -> d_name);

                //
                // Check if the file is a java source, a java class file or a
                // subdirectory. Since packages cannot start with '.', we skip
                // all files that start with a dot. That includes this
                // directory "." and its parent ".."
                //
                // Don't add the class file if the source_dir_only flag is set.
                if ((length > FileSymbol::java_suffix_length &&
                     FileSymbol::IsJavaSuffix(&entry -> d_name[length - FileSymbol::java_suffix_length]))  ||
                    ((! source_dir_only) && length > FileSymbol::class_suffix_length &&
                     FileSymbol::IsClassSuffix(&entry -> d_name[length - FileSymbol::class_suffix_length])) ||
                    ((Case::Index(entry -> d_name, U_DOT) < 0) && SystemIsDirectory(entry -> d_name)))
                    entries -> InsertEntry((DirectorySymbol *) this, entry -> d_name, length);
            }
            closedir(directory);
        }

#elif defined(WIN32_FILE_SYSTEM)

        char *directory_name = new char[DirectoryNameLength() + 3]; // +2 for "/*" +1 for '\0'
        strcpy(directory_name, DirectoryName());
        if (directory_name[DirectoryNameLength() - 1] != U_SLASH)
            strcat(directory_name, StringConstant::U8S_SL);
        strcat(directory_name, StringConstant::U8S_ST);

        WIN32_FIND_DATA entry;
        HANDLE file_handle = FindFirstFile(directory_name, &entry);
        if (file_handle != INVALID_HANDLE_VALUE)
        {
            do
            {
                int length = strlen(entry.cFileName);

                //
                // Check if the file is a java source, a java class file or a
                // subdirectory. Since packages cannot start with '.', we skip
                // all files that start with a dot. That includes this
                // directory "." and its parent ".."
                //
                bool is_java  = (length > FileSymbol::java_suffix_length &&
                                 FileSymbol::IsJavaSuffix(&entry.cFileName[length - FileSymbol::java_suffix_length])),
                     is_class = ((! source_dir_only) && length > FileSymbol::class_suffix_length &&
                                 FileSymbol::IsClassSuffix(&entry.cFileName[length - FileSymbol::class_suffix_length]));

                if (is_java ||
                    is_class ||
                    (entry.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && Case::Index(entry.cFileName, U_DOT) < 0))
                {
                    char *clean_name = new char[length + 1];
                    for (int i = 0; i < length; i++)
                        clean_name[i] = (entry.cFileName[i] == U_BACKSLASH ? U_SLASH : entry.cFileName[i]);
                    if (is_java)
                         strcpy(&clean_name[length - FileSymbol::java_suffix_length], FileSymbol::java_suffix);
                    else if (is_class)
                         strcpy(&clean_name[length - FileSymbol::class_suffix_length], FileSymbol::class_suffix);
                    DirectoryEntry *entry = entries -> InsertEntry((DirectorySymbol *) this, clean_name, length);
                    if (! is_java)
                        entries -> InsertCaseInsensitiveEntry(entry);
                    delete [] clean_name;
                }
            } while (FindNextFile(file_handle, &entry));
            FindClose(file_handle);
        }

        delete [] directory_name;
#endif
    }
}


DirectorySymbol *FileSymbol::OutputDirectory()
{
    return (output_directory ? output_directory : output_directory = Control::GetOutputDirectory(this));
}


void FileSymbol::SetFileName()
{
    PathSymbol *path_symbol = PathSym();
    char *directory_name = directory_symbol -> DirectoryName();
    size_t directory_name_length = directory_symbol -> DirectoryNameLength();
    bool dot_directory = (strcmp(directory_name, StringConstant::U8S_DO) == 0);
    file_name_length = (dot_directory ? 0 : directory_name_length) +
                               Utf8NameLength()   +
                               (path_symbol -> IsZip() // For zip files, we need "()"; for regular directory, we need 1 '/'
                                             ? 2
                                             : (dot_directory || directory_name[directory_name_length - 1] == U_SLASH ? 0 : 1)) +
                               (kind == JAVA ? java_suffix_length : class_suffix_length);

    file_name = new char[file_name_length + 1]; // +1 for '\0'
    if (dot_directory)
         file_name[0] = U_NULL;
    else
    {
        strcpy(file_name, directory_symbol -> DirectoryName());
        if (path_symbol -> IsZip())
             strcat(file_name, StringConstant::U8S_LP);
        else if (directory_name[directory_name_length - 1] != U_SLASH)
             strcat(file_name, StringConstant::U8S_SL);
    }
    strcat(file_name, Utf8Name());
    strcat(file_name, kind == JAVA ? FileSymbol::java_suffix : FileSymbol::class_suffix);
    if (path_symbol -> IsZip())
        strcat(file_name, StringConstant::U8S_RP);

    assert(strlen(file_name) == file_name_length);
}


void FileSymbol::SetFileNameLiteral(Control *control)
{
    if (! file_name_literal)
    {
        char *file_name = FileName();

        int i;
        for (i = FileNameLength() - 1; i >= 0; i--)
        {
            if (file_name[i] == U_SLASH)
                break;
        }

        int file_name_start = i + 1,
            file_name_length = FileNameLength() - file_name_start;
        file_name_literal = control -> Utf8_pool.FindOrInsert(file_name + file_name_start, file_name_length);
    }
}


void FileSymbol::CleanUp()
{
    delete lex_stream;
    lex_stream = NULL;

    if (compilation_unit)
    {
        delete compilation_unit -> ast_pool;
        compilation_unit = NULL;
    }

    delete semantic;
    semantic = NULL;
}


void TypeSymbol::SetClassName()
{
    size_t length;

    if (semantic_environment -> sem -> control.option.directory)
    {
        DirectorySymbol *output_directory = file_symbol -> OutputDirectory();
        int directory_length = output_directory -> DirectoryNameLength();
        char *directory_name = output_directory -> DirectoryName();
        length = directory_length + ExternalUtf8NameLength() + FileSymbol::class_suffix_length + 1; // +1 for /
        class_name = new char[length + 1]; // +1 for '\0'

        strcpy(class_name, directory_name);

        if (directory_name[directory_length - 1] != U_SLASH)
            strcat(class_name, StringConstant::U8S_SL);
    }
    else
    {
        char *file_name = semantic_environment -> sem -> lex_stream -> FileName();
        int n;
        for (n = semantic_environment -> sem -> lex_stream -> FileNameLength() - 1; n >= 0; n--)
        {
            if (file_name[n] == U_SLASH)
                break;
        }
        n++;

        length = n + ExternalUtf8NameLength() + FileSymbol::class_suffix_length;
        class_name = new char[length + 1]; // +1 for '\0'
        strncpy(class_name, file_name, n);
        class_name[n] = U_NULL;
    }

    strcat(class_name, ExternalUtf8Name());
    strcat(class_name, FileSymbol::class_suffix);

    assert(strlen(class_name) <= length);
}


void TypeSymbol::ProcessNestedTypeSignatures(Semantic *sem, LexStream::TokenIndex tok)
{
    for (int i = 0; i < NumNestedTypeSignatures(); i++)
    {
        NameSymbol *name_symbol = sem -> control.ConvertUtf8ToUnicode(NestedTypeSignature(i), strlen(NestedTypeSignature(i)));
        delete [] NestedTypeSignature(i);
        sem -> ProcessNestedType(this, name_symbol, tok);
    }

    delete nested_type_signatures;
    nested_type_signatures = NULL;
}


void MethodSymbol::ProcessMethodThrows(Semantic *sem, LexStream::TokenIndex tok)
{
    if (throws_signatures)
    {
        assert(sem);

        //
        // Process throws clause
        //
        for (int i = 0; i < NumThrowsSignatures(); i++)
        {
            TypeSymbol *type = sem -> ReadTypeFromSignature(containing_type,
                                                            ThrowsSignature(i),
                                                            strlen(ThrowsSignature(i)),
                                                            tok);
            AddThrows(type);
            delete [] ThrowsSignature(i);
        }

        delete throws_signatures;
        throws_signatures = NULL;
    }
}


void MethodSymbol::SetSignature(Control &control, VariableSymbol *this0_variable)
{
    int len = 2 + strlen(Type() -> SignatureString()); // +1 for '(' +1 for ')'

    if (this0_variable)
        len += strlen(this0_variable -> Type() -> SignatureString());
    for (int i = 0; i < NumFormalParameters(); i++)
    {
        TypeSymbol *formal_type = FormalParameter(i) -> Type();
        len += strlen(formal_type -> SignatureString());
    }

    char *method_signature = new char[len + 1]; // +1 for '\0'
    method_signature[0] = U_LEFT_PARENTHESIS;
    int k = 1;
    if (this0_variable)
    {
        for (char *str = this0_variable -> Type() -> SignatureString(); *str; str++, k++)
            method_signature[k] = *str;
    }
    for (int j = 0; j < NumFormalParameters(); j++)
    {
        TypeSymbol *formal_type = FormalParameter(j) -> Type();
        for (char *str = formal_type -> SignatureString(); *str; str++, k++)
            method_signature[k] = *str;
    }
    method_signature[k++] = U_RIGHT_PARENTHESIS;
    for (char *str = Type() -> SignatureString(); *str; str++, k++)
         method_signature[k] = *str;
    method_signature[k] = U_NULL;

    signature = control.Utf8_pool.FindOrInsert(method_signature, len);

    delete [] method_signature;
}


void MethodSymbol::ProcessMethodSignature(Semantic *sem, LexStream::TokenIndex token_location)
{
    if (! type_)
    {
        assert(sem);

        int num_parameters = 0;
        char *signature = SignatureString();
        signature++; // +1 to skip initial '('

        //
        // For the constructor of an inner type, skip the "this$0" argument.
        //
        if (containing_type -> IsInner() && Identity() == sem -> control.init_name_symbol)
        {
            if (*signature != U_RIGHT_PARENTHESIS)
            {
                //
                // Move to next signature
                //
                char *str;
                for (str = signature; *str == U_LEFT_BRACKET; str++)
                    ;
                if (*str == U_L)
                {
                    for (str++; *str != U_SEMICOLON; str++)
                        ;
                }

                int len = str - signature + 1;
                signature += len; // make signature point to next type
            }
        }

        while (*signature != U_RIGHT_PARENTHESIS)
        {
            //
            // Make up a name for each parameter... Recall that for an inner
            // class we need to skip the this$0 parameter
            //
            NameSymbol *name_symbol = sem -> control.MakeParameter(++num_parameters);
            VariableSymbol *symbol = new VariableSymbol(name_symbol);
            symbol -> SetType(sem -> ProcessSignature(containing_type, signature, token_location));
            symbol -> MarkComplete();
            AddFormalParameter(symbol);

            //
            // Move to next signature
            //
            char *str;
            for (str = signature; *str == U_LEFT_BRACKET; str++)
                ;
            if (*str == U_L)
            {
                for (str++; *str != U_SEMICOLON; str++)
                    ;
            }

            int len = str - signature + 1;
            signature += len; // make signature point to next type
        }
        signature++; // skip L')'

        //
        // Now set the type of the method.
        //
        SetType(sem -> ProcessSignature(containing_type, signature, token_location));

        //
        // Create a symbol table for this method for consistency... and in
        // order to release the space used by the variable paramaters later.
        //
        BlockSymbol *block_symbol = new BlockSymbol(num_parameters);
        for (int k = 0; k < num_parameters; k++)
            block_symbol -> InsertVariableSymbol((*formal_parameters)[k]);
        block_symbol -> CompressSpace(); // space optimization
        SetBlockSymbol(block_symbol);
    }
}


void MethodSymbol::CleanUp()
{
    BlockSymbol *block = new BlockSymbol(NumFormalParameters());

    //
    // Make a copy of each parameter into the new pared-down symbol table and
    // fix the FormalParameter information to identify the new symbol.
    //
    for (int k = 0; k < NumFormalParameters(); k++)
    {
        VariableSymbol *formal_parameter = (*formal_parameters)[k],
                       *symbol = block -> InsertVariableSymbol(formal_parameter -> Identity());
        symbol -> SetType(formal_parameter -> Type());
        symbol -> MarkComplete();
        (*formal_parameters)[k] = symbol;
    }

    //
    // Destroy the old symbol and replace it by the new one.
    //
    delete block_symbol;
    block -> CompressSpace(); // space optimization
    SetBlockSymbol(block);

    method_or_constructor_declaration = NULL; // remove reference to Ast structure
}


void VariableSymbol::ProcessVariableSignature(Semantic *sem, LexStream::TokenIndex token_location)
{
    if (! type_)
    {
        assert(sem);

        SetType(sem -> ProcessSignature((TypeSymbol *) owner, signature_string, token_location));
    }
}


bool TypeSymbol::IsNestedIn(TypeSymbol *type)
{
    for (SemanticEnvironment *env = semantic_environment; env != NULL; env = env -> previous)
    {
        if (env -> Type() == type)
             return true;
    }

    return false;
}


bool TypeSymbol::CanAccess(TypeSymbol *type)
{
    assert(semantic_environment);

    SemanticEnvironment *env = semantic_environment;
    //
    // Note that this case is only possible in the current environment (i.e.,
    // it is not recursively applicable) as it is not possible to declare an
    // inner class (even an anonymous one) within an explicit constructor
    // invocation. This is the case precisely because of the unavailability
    // of the "this" which one would need to pass to the inner class in
    // question. See comment below...
    //
    if (env -> explicit_constructor_invocation && env -> explicit_constructor_invocation -> SuperCallCast())
    {
        //
        // If we are processing a super call statement in the constructor of
        // an inner class then we have access to the this$0 (passed to the
        // constructor of the inner class as parameter) but not to the "this"
        // of the inner class in question, unless it is static.
        //
        if (IsSubclass(type))
            return ACC_STATIC(); // "this" class is accessible only if it is static.
        if (IsInner())
        {
            if (ContainingType() -> IsSubclass(type))
                return true;
            env = env -> previous; // skip "this" type.
        }
    }

    for (; env != NULL; env = env -> previous)
    {
        if (env -> StaticRegion()) // are we in a static environment?
             break;
        else if (env -> Type() -> IsSubclass(type)) // then, check if we reached the type in question or one of its superclasses;
             return true;
        else if (env -> Type() -> ACC_STATIC())     // next, check whether or not the current type is static...
             break;
    }

    return false;
}


//
// Given two types T and T2 in different packages, the type T can access
// protected members of T2 iff T or any class in which T is lexically enclosed
// is a subclass of T2 or of some other type T3 that lexically encloses T2.
//
// Of course, T2 and all its enclosing classes, if any, must have been declared
// either public or protected, otherwise they could not be eligible as a
// superclass candidate. We do not need to check for that condition here.
//
bool TypeSymbol::HasProtectedAccessTo(TypeSymbol *target_type)
{
    assert(semantic_environment && ! target_type -> IsArray());

    // Loop through T and enclosing classes.
    for (SemanticEnvironment *env = semantic_environment;
         env; env = env -> previous)
    {
        TypeSymbol *main_type = env -> Type();
        // Loop through T2 and enclosing classes.
        for (TypeSymbol *type = target_type; type; type = type -> owner -> TypeCast())
        {
            if (main_type -> IsSubclass(type))
                return true;
        }
    }

    return false;
}


inline NameSymbol *TypeSymbol::GetThisName(Control &control, int n)
{
    IntToWstring value(n);

    int length = wcslen(StringConstant::US_this_DOLLAR) + value.Length();
    wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
    wcscpy(name, StringConstant::US_this_DOLLAR);
    wcscat(name, value.String());
    NameSymbol *name_symbol = control.FindOrInsertName(name, length);

    delete [] name;

    return name_symbol;
}


VariableSymbol *TypeSymbol::InsertThis(int n)
{
    assert(IsInner());

    Control &control = semantic_environment -> sem -> control;
    VariableSymbol *variable_symbol = NULL;

    if (n == 0)
    {
        assert(NumConstructorParameters() == 0); // no local shadows and no this$0 !!!
        assert(NumEnclosingInstances() == 0);

        //
        // Create a this0 pointer for an inner class.
        //
        variable_symbol = InsertVariableSymbol(control.this0_name_symbol);
        variable_symbol -> MarkSynthetic();
        variable_symbol -> SetType(ContainingType());
        variable_symbol -> SetACC_PRIVATE();
//        variable_symbol -> SetACC_FINAL();
        variable_symbol -> SetOwner((TypeSymbol *) this);
        variable_symbol -> MarkComplete();

        AddConstructorParameter(variable_symbol);
        AddEnclosingInstance(variable_symbol); // must at least have this$0
    }
    else
    {
        assert(NumEnclosingInstances() == n);

        //
        // Create a this$n pointer.
        //
        variable_symbol = InsertVariableSymbol(GetThisName(control, n));
        variable_symbol -> MarkSynthetic();
        variable_symbol -> SetType(EnclosingInstance(n - 1) -> Type() -> ContainingType());
        variable_symbol -> SetACC_PRIVATE();
//        variable_symbol -> SetACC_FINAL();
        variable_symbol -> SetOwner((TypeSymbol *) this);
        variable_symbol -> MarkComplete();

        AddEnclosingInstance(variable_symbol);
    }

    return variable_symbol;
}


TypeSymbol *TypeSymbol::FindOrInsertClassLiteralClass(LexStream::TokenIndex tok)
{
    Semantic *sem = semantic_environment -> sem;
    AstCompilationUnit *compilation_unit = sem -> compilation_unit;
    Control &control = sem -> control;

    assert(this == outermost_type && ACC_INTERFACE());

    if (! class_literal_class)
    {
        AstClassInstanceCreationExpression *class_creation = compilation_unit -> ast_pool -> GenClassInstanceCreationExpression();
        class_creation -> base_opt      = NULL;
        class_creation -> dot_token_opt = 0;
        class_creation -> new_token = tok;

             AstSimpleName *ast_type = compilation_unit -> ast_pool -> GenSimpleName(tok);

        class_creation -> class_type = compilation_unit -> ast_pool -> GenTypeExpression(ast_type);
        class_creation -> left_parenthesis_token  = tok;
        class_creation -> right_parenthesis_token = tok;

            AstClassBody *class_body = compilation_unit -> ast_pool -> GenClassBody();
            class_body -> left_brace_token = tok;
            class_body -> right_brace_token = tok;

        class_creation -> class_body_opt = class_body;

        TypeSymbol *class_literal_type = sem -> GetAnonymousType(class_creation, control.Object());
        (void) class_literal_type -> FindOrInsertClassLiteralMethod(control);

        sem -> AddDependence(class_literal_type, control.Class(), tok);

        class_literal_class = class_literal_type;
    }

    return class_literal_class;
}


MethodSymbol *TypeSymbol::FindOrInsertClassLiteralMethod(Control &control)
{
    if (! class_literal_method)
    {
        //
        // Note that max_variable_index is initialized to 1 (instead of 0),
        // even though the class literal method is static. The reason is that
        // in generating code for this method, a try statement with a catch
        // will be used. Therefore, an extra "local" slot is required for the
        // local Exception parameter of the catch clause.
        //
        BlockSymbol *block_symbol = new BlockSymbol(1); // the associated symbol table will contain 1 element
        block_symbol -> max_variable_index = 1;

        class_literal_method = InsertMethodSymbol(control.class_name_symbol);
        class_literal_method -> MarkSynthetic();
        class_literal_method -> SetType(control.Class());
        class_literal_method -> SetACC_STATIC();
        class_literal_method -> SetContainingType((TypeSymbol *) this);
        class_literal_method -> SetBlockSymbol(block_symbol);

            VariableSymbol *variable_symbol = block_symbol -> InsertVariableSymbol(control.MakeParameter(1));
            variable_symbol -> MarkSynthetic();
            variable_symbol -> SetType(control.String());
            variable_symbol -> SetOwner(class_literal_method);
            variable_symbol -> SetLocalVariableIndex(block_symbol -> max_variable_index++);

        class_literal_method -> AddFormalParameter(variable_symbol);
        class_literal_method -> SetSignature(control);
    }

    return class_literal_method;
}


Utf8LiteralValue *TypeSymbol::FindOrInsertClassLiteralName(Control &control)
{
    if (! class_literal_name)
    {
        int length = fully_qualified_name -> length;
        char *slashed_name = fully_qualified_name -> value;
        char *name = new char[length + 1];
        for (int i = 0; i < length; i++)
            name[i] = (slashed_name[i] == U_SLASH ? (wchar_t) U_DOT : slashed_name[i]);
        name[length] = U_NULL;
        class_literal_name = control.Utf8_pool.FindOrInsert(name, length);
        delete [] name;
    }

    return class_literal_name;
}


VariableSymbol *TypeSymbol::FindOrInsertClassLiteral(TypeSymbol *type)
{
    assert((! IsInner()) && (! type -> Primitive()));

    Semantic *sem = semantic_environment -> sem;
    Control &control = sem -> control;

    (void) FindOrInsertClassLiteralMethod(control);

    (void) type -> FindOrInsertClassLiteralName(control);

    NameSymbol *name_symbol = NULL;
    char *signature = type -> SignatureString();
    if (signature[0] == U_LEFT_BRACKET) // an array?
    {
        int array_length = wcslen(StringConstant::US_array),
            length = strlen(signature) + array_length;
        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US_array);
        int i,
            k;
        for (i = 0, k = array_length; signature[i] == U_LEFT_BRACKET; i++, k++)
            name[k] = U_DOLLAR;
        // Leave leading 'L', since there can be conflicts with primitive
        // array types otherwise
        for (wchar_t ch = signature[i++]; ch && ch != U_SEMICOLON; ch = signature[i++])
            name[k++] = (ch == U_SLASH ? (wchar_t) U_DOLLAR : ch);
        name[k] = U_NULL;
        name_symbol = control.FindOrInsertName(name, k);

        delete [] name;
    }
    else if (signature[0] == U_L) // a reference type ?
    {
        int class_length = wcslen(StringConstant::US_class_DOLLAR),
            length = strlen(signature) + class_length;

        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US_class_DOLLAR);
        int i = 1, // skip leading 'L'
            k = class_length;
        for (wchar_t ch = signature[i++]; ch && ch != U_SEMICOLON; ch = signature[i++])
            name[k++] = (ch == U_SLASH ? (wchar_t) U_DOLLAR : ch);
        name[k] = U_NULL;
        name_symbol = control.FindOrInsertName(name, k);

        delete [] name;
    }

    VariableSymbol *variable_symbol = FindVariableSymbol(name_symbol);
    if (! variable_symbol)
    {
        //
        // Generate a caching variable (no need to make it private, so that
        // nested classes can share it easily).  Foo.Bar.class is cached in:
        //
        //     /*synthetic*/ static java.lang.Class class$Foo$Bar;
        //
        variable_symbol = InsertVariableSymbol(name_symbol);
        variable_symbol -> MarkSynthetic();
        variable_symbol -> SetType(control.Class());
        variable_symbol -> SetACC_STATIC();
        variable_symbol -> SetOwner((TypeSymbol *) this);
        variable_symbol -> MarkComplete();

        AddClassLiteral(variable_symbol);
    }

    return variable_symbol;
}


VariableSymbol *TypeSymbol::FindOrInsertAssertVariable()
{
    if (! assert_variable)
    {
        assert(! (Primitive() || ACC_INTERFACE() || IsArray()));

        Semantic *sem = semantic_environment -> sem;
        Control &control = sem -> control;

        NameSymbol *name_symbol = NULL;
        int length = wcslen(StringConstant::US_DOLLAR_noassert);
        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US_DOLLAR_noassert);
        name_symbol = control.FindOrInsertName(name, length);
        delete [] name;

        assert_variable = InsertVariableSymbol(name_symbol);
        assert_variable -> MarkSynthetic();
        assert_variable -> SetType(control.boolean_type);
        assert_variable -> SetACC_PRIVATE();
        assert_variable -> SetACC_STATIC();
        assert_variable -> SetACC_FINAL();
        assert_variable -> SetOwner((TypeSymbol *) this);
        assert_variable -> MarkComplete();
        assert_variable -> MarkDefinitelyAssigned();

        //
        // We'll create the field initializer later in bytecode.cpp, but we
        // create the static initializer that will contain the field
        // initializer now, if it was not already created.
        //
        sem -> GetStaticInitializerMethod();
    }

    return assert_variable;
}


VariableSymbol *TypeSymbol::FindOrInsertLocalShadow(VariableSymbol *local)
{
    assert(IsLocal());

    Control &control = semantic_environment -> sem -> control;
    VariableSymbol *variable = (VariableSymbol *) (local_shadow_map ? local_shadow_map -> Image(local) : NULL);

    //
    // For a local class, if it does not yet have a shadow for a local variable
    // that it needs access to, create one.
    //
    // Note that since this function is always invoked after the symbol
    // table for the type in question has been extended, the local shadow will
    // not appear in the expanded_field_table. Creating a shadow in the
    // expanded_field_table as well would cut down on the number of calls to
    // this function. However, the reason why we don't create such a shadow is
    // that since the new symbol is assigned a new name on the fly, its
    // name_symbol will fall outside the range of the expanded field table in
    // question.
    //
    if (! variable)
    {
        int length = 4 + local -> NameLength(); // +4 for val$
        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US_val_DOLLAR);
        wcscat(name, local -> Name());
        NameSymbol *name_symbol = control.FindOrInsertName(name, length);

        variable = InsertVariableSymbol(name_symbol);
        variable -> MarkSynthetic();
        variable -> accessed_local = local;
        variable -> SetType(local -> Type());
        variable -> SetACC_PRIVATE();
//        variable -> SetACC_FINAL();
        variable -> SetOwner((TypeSymbol *) this);
        variable -> MarkComplete();

        AddConstructorParameter(variable);

        delete [] name;

        for (int i = 0; i < NumGeneratedConstructors(); i++)
        {
            MethodSymbol *constructor = GeneratedConstructor(i);

            VariableSymbol *symbol = constructor -> block_symbol -> InsertVariableSymbol(name_symbol);
            symbol -> MarkSynthetic();
            symbol -> SetType(variable -> Type());
            symbol -> SetOwner(constructor);
            symbol -> SetLocalVariableIndex(constructor -> block_symbol -> max_variable_index++);
            if (control.IsDoubleWordType(symbol -> Type()))
                constructor -> block_symbol -> max_variable_index++;
            constructor -> AddFormalParameter(symbol);
        }

        if (! local_shadow_map)
            local_shadow_map = new SymbolMap();
        local_shadow_map -> Map(local, variable);
    }

assert(variable -> accessed_local == local);

    return variable;
}


inline void TypeSymbol::MapSymbolToReadMethod(Symbol *symbol,
                                              TypeSymbol *base_type,
                                              MethodSymbol *method)
{
    if (! read_methods)
        read_methods = new Map<Symbol, Map<TypeSymbol, MethodSymbol> >(); // default size
 
    Map<TypeSymbol, MethodSymbol> *map = read_methods -> Image(symbol);
    if (! map)
    {
        map = new Map<TypeSymbol, MethodSymbol>(1); // small size
        read_methods -> Add(symbol, map);
    }

    map -> Add(base_type, method);
}

inline MethodSymbol *TypeSymbol::ReadMethod(Symbol *symbol,
                                            TypeSymbol *base_type)
{
    if (read_methods)
    {
        Map<TypeSymbol, MethodSymbol> *map = read_methods -> Image(symbol);
        if (map)
            return map -> Image(base_type);
    }

    return (MethodSymbol *) NULL;
}

inline void TypeSymbol::MapSymbolToWriteMethod(VariableSymbol *symbol,
                                               TypeSymbol *base_type,
                                               MethodSymbol *method)
{
    if (! write_methods)
        write_methods = new Map<VariableSymbol, Map<TypeSymbol, MethodSymbol> >(); // default size

    Map<TypeSymbol, MethodSymbol> *map = write_methods -> Image(symbol);
    if (! map)
    {
        map = new Map<TypeSymbol, MethodSymbol>(1); // small size
        write_methods -> Add(symbol, map);
    }
 
    map -> Add(base_type, method);
}

inline MethodSymbol *TypeSymbol::WriteMethod(VariableSymbol *symbol,
                                             TypeSymbol *base_type)
{
    if (write_methods)
    {
        Map<TypeSymbol, MethodSymbol> *map = write_methods -> Image(symbol);
        if (map)
            return map -> Image(base_type);
    }

    return (MethodSymbol *) NULL;
}

MethodSymbol *TypeSymbol::GetReadAccessMethod(MethodSymbol *member,
                                              TypeSymbol *base_type)
{
    assert(member -> Identity() != semantic_environment -> sem -> control.init_name_symbol); // accessing a method

    TypeSymbol *containing_type = member -> containing_type;
    if (! base_type)
        base_type = this;

    assert((member -> ACC_PRIVATE() && this == containing_type) ||
           (member -> ACC_PROTECTED() &&
            ! semantic_environment -> sem -> ProtectedAccessCheck(base_type, containing_type)) ||
           containing_type == super);

    MethodSymbol *read_method = ReadMethod(member, base_type);

    if (! read_method)
    {
        //
        // BaseType is the qualifying type of we are accessing.  If the method
        // is private, BaseType should be this type, but for protected
        // variables, BaseType should be a superclass or subclass of this type
        // that is not in this package.
        //
        // To access
        // "static Type name(Type1 p1, Type2 p2, ...) throws Exception;",
        // expand to:
        //
        // /*synthetic*/ static Type access$<num>(Type1 p1, Type2 p2, ...)
        // throws Exception
        // {
        //     return BaseType.name(p1, p2, ...);
        // }
        //
        // If we are accessing
        // "void name(Type1 p1, Type2 p2, ...) throws Throwable;",
        // expand to:
        //
        // /*synthetic*/ static void access$<num>(BaseType $0, Type1 p1,
        //                                        Type2 p2, ...)
        // throws Throwable
        // {
        //     $0.name(p1, p2, ...);
        //     return;
        // }
        //
        Semantic *sem = semantic_environment -> sem;
        assert(sem);

        Control &control = sem -> control;
        StoragePool *ast_pool = sem -> compilation_unit -> ast_pool;

        IntToWstring value(NumPrivateAccessMethods());

        int length = 7 + value.Length(); // +7 for access$
        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US_access_DOLLAR);
        wcscat(name, value.String());

        //
        // Use the location of the class name for all elements of this method.
        //
        LexStream::TokenIndex loc = LexStream::Badtoken();
        if (declaration -> ClassDeclarationCast())
            loc = ((AstClassDeclaration *) declaration) -> identifier_token;
        else if (declaration -> ClassInstanceCreationExpressionCast())
        {
            assert(((AstClassInstanceCreationExpression *) declaration) -> class_body_opt);
            loc = ((AstClassInstanceCreationExpression *) declaration) -> class_body_opt -> left_brace_token;
        }
        assert(loc != LexStream::Badtoken());

        int parameter_count = member -> NumFormalParameters();

        read_method = InsertMethodSymbol(control.FindOrInsertName(name, length));
        read_method -> MarkSynthetic();
        read_method -> SetType(member -> Type());
        read_method -> SetACC_STATIC();
        read_method -> SetContainingType(this);

        //
        // A read access method for a method has a formal parameter per
        // parameter of the member in question, plus one more if it is not
        // static.
        //
        BlockSymbol *block_symbol = new BlockSymbol(parameter_count +
                                                    (member -> ACC_STATIC() ? 0 : 1));
        block_symbol -> max_variable_index = 0;
        read_method -> SetBlockSymbol(block_symbol);

        for (int j = 0; j < member -> NumThrows(); j++)
            read_method -> AddThrows(member -> Throws(j));

        AstSimpleName *base = ast_pool -> GenSimpleName(loc);

        AstFieldAccess::FieldAccessTag tag = AstFieldAccess::NONE;
        if (! member -> ACC_STATIC() && base_type == super)
        {
            //
            // Special case - for Outer.super.m() where m() is an instance
            // method, we mark the field access as a super access, to
            // make sure we emit invokespecial instead of invokevirtual in
            // bytecode.cpp.  Notice that in this case,
            // ((Super) Outer.this).m() cannot generate an accessor method
            // (either m() is public or in the same package and thus already
            // accessible, or m is protected in a different package and
            // therefore inaccessible), so we don't have to worry about a
            // conflict in accessor methods for the same base type.
            //
            tag = AstFieldAccess::SUPER_TAG;
        }

        AstFieldAccess *field_access     = ast_pool -> GenFieldAccess(tag);
        field_access -> base             = base;
        field_access -> dot_token        = loc;
        field_access -> identifier_token = loc;

        AstMethodInvocation *method_invocation       = ast_pool -> GenMethodInvocation();
        method_invocation -> method                  = field_access;
        method_invocation -> left_parenthesis_token  = loc;
        method_invocation -> right_parenthesis_token = loc;
        method_invocation -> symbol                  = member;
        method_invocation -> AllocateArguments(parameter_count);

        AstMethodDeclarator *method_declarator       = ast_pool -> GenMethodDeclarator();
        method_declarator -> identifier_token        = loc;
        method_declarator -> left_parenthesis_token  = loc;
        method_declarator -> right_parenthesis_token = loc;

        if (member -> ACC_STATIC())
        {
            method_declarator -> AllocateFormalParameters(parameter_count);
            base -> symbol = base_type;
        }
        else
        {
            method_declarator -> AllocateFormalParameters(parameter_count + 1);
            NameSymbol *instance_name = control.MakeParameter(1);

            VariableSymbol *instance = block_symbol -> InsertVariableSymbol(instance_name);
            instance -> MarkSynthetic();
            instance -> SetType(base_type);
            instance -> SetOwner(read_method);
            instance -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
            read_method -> AddFormalParameter(instance);
            base -> symbol = instance;
        }

        for (int i = 0; i < parameter_count; i++)
        {
            VariableSymbol *parm = block_symbol -> InsertVariableSymbol(member -> FormalParameter(i) -> Identity());
            parm -> MarkSynthetic();
            parm -> SetType(member -> FormalParameter(i) -> Type());
            parm -> SetOwner(read_method);
            parm -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
            if (control.IsDoubleWordType(parm -> Type()))
                block_symbol -> max_variable_index++;
            read_method -> AddFormalParameter(parm);

            AstSimpleName *simple_name = ast_pool -> GenSimpleName(loc);
            simple_name -> symbol      = parm;

            method_invocation -> AddArgument(simple_name);
        }
        read_method -> SetSignature(control);

        AstReturnStatement *return_statement = ast_pool -> GenReturnStatement();
        return_statement -> return_token     = loc;
        return_statement -> semicolon_token  = loc;
        return_statement -> is_reachable     = true;

        AstBlock *block            = ast_pool -> GenBlock();
        block -> left_brace_token  = loc;
        block -> right_brace_token = loc;
        block -> block_symbol      = new BlockSymbol(0); // the symbol table associated with this block will contain no element
        block -> is_reachable      = true;

        if (member -> Type() == control.void_type)
        {
            AstExpressionStatement *expression_statement  = ast_pool -> GenExpressionStatement();
            expression_statement -> expression            = method_invocation;
            expression_statement -> semicolon_token_opt   = loc;
            expression_statement -> is_reachable          = true;
            expression_statement -> can_complete_normally = true;

            block -> AllocateBlockStatements(2); // this block contains two statements
            block -> AddStatement(expression_statement);
        }
        else
        {
            return_statement -> expression_opt = method_invocation;

            block -> AllocateBlockStatements(1); // this block contains one statement
        }
        block -> AddStatement(return_statement);

        AstMethodDeclaration *method_declaration  = ast_pool -> GenMethodDeclaration();
        method_declaration -> method_symbol       = read_method;
        method_declaration -> method_declarator   = method_declarator;
        method_declaration -> method_body         = block;

        read_method -> method_or_constructor_declaration = method_declaration;
        read_method -> accessed_member = member;
        MapSymbolToReadMethod(member, base_type, read_method);
        AddPrivateAccessMethod(read_method);

        delete [] name;
    }

    return read_method;
}

MethodSymbol *TypeSymbol::GetReadAccessConstructor(MethodSymbol *member)
{
    assert(member -> Identity() == semantic_environment -> sem -> control.init_name_symbol); // accessing a constructor
    TypeSymbol *containing_type = member -> containing_type;
    if (! base_type)
        base_type = this;

    //
    // Protected superconstructors are always accessible, and class instance
    // creation expressions can only invoke a protected constructor in the
    // current package, where an accessor is not needed.
    //
    assert(member -> ACC_PRIVATE() && this == containing_type);

    MethodSymbol *read_method = ReadMethod(member, base_type);

    if (! read_method)
    {
        //
        // There are two cases for accessing a private constructor.  First, as
        // a superclass:
        //
        // class Outer {
        //     private Outer(Type1 $1, Type2 $2, ...) {}
        //     static class Inner extends Outer {
        //         Inner() { super(expr1, expr2, ...); }
        //     }
        // }
        //
        // We must create a synthetic place-holder class, and expand this to:
        // (TODO: can someone come up with a way to do this without a
        // placeholder class?)
        //
        // class Outer {
        //     private Outer(Type1 $1, Type2 $2, ...) {}
        //     /*synthetic*/ Outer(Outer$ $0, Type1 $1, Type2 $2, ...)
        //     {
        //         this($1, $2, ...);
        //     }
        // }
        // /*synthetic*/ class Outer$ {} // placeholder only
        // class Outer$Inner extends Outer {
        //     Outer$Inner() { super((Outer$) null, expr1, expr2, ...); }
        // }
        //
        // The other use is in class instance creation expressions (recall
        // that the default constructor for a private class is private):
        //
        // class Outer {
        //     private class Inner {}
        //     Inner i = new Inner();
        // }
        //
        // Here again, we create a place-holder class for now.  TODO:
        // alternatives have been proposed, such as using a static generator
        // method instead of an alternate constructor.
        //
        // class Outer {
        //     Outer$Inner i = new Outer$Inner(this, (Outer$) null);
        // }
        // /*synthetic*/ class Outer$ {} // placeholder only
        // class Outer$Inner {
        //     private final Outer this$0;
        //     private Outer$Inner(Outer $0) { super(); this$0 = $0; }
        //     /*synthetic*/ Outer$Inner(Outer $0, Outer$ $1) { this($0); }
        // }
        // 
        Semantic *sem = semantic_environment -> sem;

        assert(sem);

        Control &control = sem -> control;

        StoragePool *ast_pool = sem -> compilation_unit -> ast_pool;

        IntToWstring value(NumPrivateAccessConstructors());
        
        int length = 7 + value.Length(); // +7 for access$
        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US_access_DOLLAR);
        wcscat(name, value.String());

        BlockSymbol *block_symbol = new BlockSymbol(member -> NumFormalParameters() + 3);

        read_method = InsertMethodSymbol(control.FindOrInsertName(name, length));
        read_method -> MarkSynthetic();
        read_method -> SetType(member -> Type());
        read_method -> SetContainingType(this);
        read_method -> SetBlockSymbol(block_symbol);

        for (int j = 0; j < member -> NumThrows(); j++)
            read_method -> AddThrows(member -> Throws(j));

        //
        // A constructor in a local class already has a "LocalConstructor()"
        // associated with it that can be used as a read access method.
        //
        assert(! IsLocal());

        //
        //
        //
        block_symbol -> max_variable_index = 1;
        read_method -> SetExternalIdentity(member -> Identity());

        Ast *declaration = member -> method_or_constructor_declaration;
        AstMethodDeclaration *method_declaration = declaration -> MethodDeclarationCast();
        AstMethodDeclarator *declarator = (method_declaration
                                           ? method_declaration -> method_declarator
                                           : ((AstConstructorDeclaration *) declaration) -> constructor_declarator);
        LexStream::TokenIndex loc = declarator -> identifier_token;

        //
        // Create a new anonymous type in order to create a unique substitute
        // constructor.
        //
        AstSimpleName *ast_type = ast_pool -> GenSimpleName(loc);
 
        AstClassInstanceCreationExpression *class_creation = ast_pool -> GenClassInstanceCreationExpression();
        class_creation -> base_opt      = NULL;
        class_creation -> dot_token_opt = 0;
        class_creation -> new_token = loc;
        class_creation -> class_type = ast_pool -> GenTypeExpression(ast_type);
        class_creation -> left_parenthesis_token  = loc;
        class_creation -> right_parenthesis_token = loc;

        AstClassBody *class_body = ast_pool -> GenClassBody();
        class_body -> left_brace_token = loc;
        class_body -> right_brace_token = loc;

        class_creation -> class_body_opt = class_body;

        TypeSymbol *anonymous_type = sem -> GetAnonymousType(class_creation, control.Object());

        //
        //
        //
        AstMethodDeclarator *method_declarator       = ast_pool -> GenMethodDeclarator();
        method_declarator -> identifier_token        = loc;
        method_declarator -> left_parenthesis_token  = declarator -> LeftToken();
        method_declarator -> right_parenthesis_token = declarator -> RightToken();
        
        AstThisCall *this_call = ast_pool -> GenThisCall();
        this_call -> base_opt                = NULL;
        this_call -> this_token              = loc;
        this_call -> left_parenthesis_token  = loc;
        this_call -> right_parenthesis_token = loc;
        this_call -> semicolon_token         = loc;
        this_call -> symbol = member;

        VariableSymbol *this0_variable = NULL;
        if (IsInner())
        {
            this0_variable = block_symbol -> InsertVariableSymbol(control.this0_name_symbol);
            this0_variable -> MarkSynthetic();
            this0_variable -> SetType(ContainingType());
            this0_variable -> SetOwner(read_method);
            this0_variable -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
        }

        //
        // Add extra parameter with anonymous type...
        //
        VariableSymbol *parm = block_symbol -> InsertVariableSymbol(control.MakeParameter(1));
        parm -> MarkSynthetic();
        parm -> SetType(anonymous_type);
        parm -> SetOwner(read_method);
        parm -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
        read_method -> AddFormalParameter(parm);

        //
        // Since private_access_constructors will be compiled (see
        // body.cpp), we must create valid ast_simple_names for its
        // parameters.
        //
        for (int i = 0; i < member -> NumFormalParameters(); i++)
        {
            parm = block_symbol -> InsertVariableSymbol(member -> FormalParameter(i) -> Identity());
            parm -> MarkSynthetic();
            parm -> SetType(member -> FormalParameter(i) -> Type());
            parm -> SetOwner(read_method);
            parm -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
            if (control.IsDoubleWordType(parm -> Type()))
                block_symbol -> max_variable_index++;
            read_method -> AddFormalParameter(parm);

            //
            // Since private_access_constructors will be compiled (see
            // body.cpp), we must create valid ast_simple_names for its
            // parameters.
            //
            AstVariableDeclaratorId *variable_declarator_name = declarator -> FormalParameter(i) -> formal_declarator -> variable_declarator_name;
            AstSimpleName *simple_name = ast_pool -> GenSimpleName(variable_declarator_name -> identifier_token);
            simple_name -> symbol = parm;
            this_call -> AddArgument(simple_name);
        }
        read_method -> SetSignature(control, this0_variable);

        AstReturnStatement *return_statement = ast_pool -> GenReturnStatement();
        return_statement -> return_token = loc;
        return_statement -> expression_opt = NULL;
        return_statement -> semicolon_token = loc;
        return_statement -> is_reachable = true;

        AstBlock *block = ast_pool -> GenBlock();
        block -> AllocateBlockStatements(1); // this block contains one statement
        block -> left_brace_token  = loc;
        block -> right_brace_token = loc;

        block -> block_symbol = new BlockSymbol(0); // the symbol table associated with this block will contain no element
        block -> is_reachable = true;
        block -> can_complete_normally = false;
        block -> AddStatement(return_statement);

        AstConstructorBlock *constructor_block                   = ast_pool -> GenConstructorBlock();
        constructor_block -> left_brace_token                    = loc;
        constructor_block -> explicit_constructor_invocation_opt = this_call;
        constructor_block -> block                               = block;
        constructor_block -> right_brace_token                   = loc;

        AstConstructorDeclaration *constructor_declaration  = ast_pool -> GenConstructorDeclaration();
        constructor_declaration -> constructor_declarator   = method_declarator;
        constructor_declaration -> constructor_body         = constructor_block;

        constructor_declaration -> constructor_symbol = read_method;
        read_method -> method_or_constructor_declaration = constructor_declaration;

        AddPrivateAccessConstructor(read_method);

        read_method -> accessed_member = member;
        MapSymbolToReadMethod(member, base_type, read_method);

        delete [] name;
    }

    return read_method;
}


MethodSymbol *TypeSymbol::GetReadAccessMethod(VariableSymbol *member,
                                              TypeSymbol *base_type)
{
    TypeSymbol *containing_type = member -> owner -> TypeCast();
    if (! base_type)
        base_type = this;        

    assert((member -> ACC_PRIVATE() && this == containing_type) ||
           (member -> ACC_PROTECTED() &&
            (! semantic_environment -> sem -> ProtectedAccessCheck(base_type, containing_type))));

    MethodSymbol *read_method = ReadMethod(member, base_type);

    if (! read_method)
    {
        //
        // BaseType is the qualifying type of we are accessing.  If the
        // variable is private, BaseType should be this type, but for
        // protected variables, BaseType should be a superclass or subclass
        // of this type that is not in this package.
        //
        // If we are accessing "static Type name;", expand to:
        //
        // /*synthetic*/ static Type access$<num>()
        // {
        //     return BaseType.name;
        // }
        //
        // If we are accessing "Type name;", expand to:
        //
        // /*synthetic*/ static Type access$<num>(BaseType $1)
        // {
        //     return $1.name;
        // }
        //
        Semantic *sem = semantic_environment -> sem;
        assert(sem);

        Control &control = sem -> control;
        StoragePool *ast_pool = sem -> compilation_unit -> ast_pool;

        IntToWstring value(NumPrivateAccessMethods());

        int length = 7 + value.Length(); // +7 for access$
        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US_access_DOLLAR);
        wcscat(name, value.String());

        //
        // Use the location of the class name for all elements of this method
        //
        LexStream::TokenIndex loc = LexStream::Badtoken();
        if (declaration -> ClassDeclarationCast())
            loc = ((AstClassDeclaration *) declaration) -> identifier_token;
        else if (declaration -> ClassInstanceCreationExpressionCast())
        {
            assert(((AstClassInstanceCreationExpression *) declaration) -> class_body_opt);
            loc = ((AstClassInstanceCreationExpression *) declaration) -> class_body_opt -> left_brace_token;
        }
        assert(loc != LexStream::Badtoken());

        read_method = InsertMethodSymbol(control.FindOrInsertName(name, length));
        read_method -> MarkSynthetic();
        read_method -> SetType(member -> Type());
        read_method -> SetACC_STATIC();
        read_method -> SetContainingType(this);

        //
        // A read access method for a field has 1 formal parameter if the
        // member in question is not static
        //
        BlockSymbol *block_symbol = new BlockSymbol(member -> ACC_STATIC() ? 0 : 1);
        block_symbol -> max_variable_index = 0;
        read_method -> SetBlockSymbol(block_symbol);

        AstSimpleName *base = ast_pool -> GenSimpleName(loc);

        AstFieldAccess *field_access     = ast_pool -> GenFieldAccess();
        field_access -> base             = base;
        field_access -> dot_token        = loc;
        field_access -> identifier_token = loc;
        field_access -> symbol           = member;

        AstMethodDeclarator *method_declarator       = ast_pool -> GenMethodDeclarator();
        method_declarator -> identifier_token        = loc;
        method_declarator -> left_parenthesis_token  = loc;
        method_declarator -> right_parenthesis_token = loc;

        if (member -> ACC_STATIC())
        {
            base -> symbol = base_type;
        }
        else
        {
            method_declarator -> AllocateFormalParameters(1);

            NameSymbol *instance_name = control.MakeParameter(1);

            VariableSymbol *instance = block_symbol -> InsertVariableSymbol(instance_name);
            instance -> MarkSynthetic();
            instance -> SetType(base_type);
            instance -> SetOwner(read_method);
            instance -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
            read_method -> AddFormalParameter(instance);
            base -> symbol = instance;
        }

        // A read access method has no throws clause !
        read_method -> SetSignature(control);

        AstReturnStatement *return_statement = ast_pool -> GenReturnStatement();
        return_statement -> return_token     = loc;
        return_statement -> expression_opt   = field_access;
        return_statement -> semicolon_token  = loc;
        return_statement -> is_reachable     = true;

        AstBlock *block            = ast_pool -> GenBlock();
        block -> left_brace_token  = loc;
        block -> right_brace_token = loc;
        block -> block_symbol      = new BlockSymbol(0); // the symbol table associated with this block will contain no element
        block -> is_reachable      = true;
        block -> AllocateBlockStatements(1); // this block contains one statement
        block -> AddStatement(return_statement);

        AstMethodDeclaration *method_declaration = ast_pool -> GenMethodDeclaration();
        method_declaration -> method_symbol      = read_method;
        method_declaration -> method_declarator  = method_declarator;
        method_declaration -> method_body        = block;

        read_method -> method_or_constructor_declaration = method_declaration;
        read_method -> accessed_member = member;
        MapSymbolToReadMethod(member, base_type, read_method);
        AddPrivateAccessMethod(read_method);

        delete [] name;
    }

    return read_method;
}


MethodSymbol *TypeSymbol::GetWriteAccessMethod(VariableSymbol *member,
                                               TypeSymbol *base_type)
{
    TypeSymbol *containing_type = member -> owner -> TypeCast();
    if (! base_type)
        base_type = this;

    assert((member -> ACC_PRIVATE() && this == containing_type) ||
           (member -> ACC_PROTECTED() &&
            (! semantic_environment -> sem -> ProtectedAccessCheck(base_type, containing_type))));

    MethodSymbol *write_method = WriteMethod(member, base_type);

    if (! write_method)
    {
        //
        // BaseType is the qualifying type of we are accessing.  If the
        // variable is private, BaseType should be this type, but for
        // protected variables, BaseType should be a superclass or subclass
        // of this type that is not in this package.
        //
        // If we are accessing "static Type name;", expand to:
        //
        // /*synthetic*/ static void access$<num>(Type name)
        // {
        //     BaseType.name = name;
        //     return;
        // }
        //
        // If we are accessing "Type name;", expand to:
        //
        // /*synthetic*/ static void access$<num>(BaseType $1, Type name)
        // {
        //     $1.name = name;
        //     return;
        // }
        //
        Semantic *sem = semantic_environment -> sem;
        assert(sem);

        Control &control = sem -> control;
        StoragePool *ast_pool = sem -> compilation_unit -> ast_pool;

        IntToWstring value(NumPrivateAccessMethods());

        int length = 7 + value.Length(); // +7 for access$
        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US_access_DOLLAR);
        wcscat(name, value.String());

        //
        // Use the location of the class name for all elements of this method
        //
        LexStream::TokenIndex loc = LexStream::Badtoken();
        if (declaration -> ClassDeclarationCast())
            loc = ((AstClassDeclaration *) declaration) -> identifier_token;
        else if (declaration -> ClassInstanceCreationExpressionCast())
        {
            assert(((AstClassInstanceCreationExpression *) declaration) -> class_body_opt);
            loc = ((AstClassInstanceCreationExpression *) declaration) -> class_body_opt -> left_brace_token;
        }
        assert(loc != LexStream::Badtoken());

        write_method = InsertMethodSymbol(control.FindOrInsertName(name, length));
        write_method -> MarkSynthetic();
        write_method -> SetType(sem -> control.void_type);
        write_method -> SetACC_STATIC();
        write_method -> SetContainingType(this);

        BlockSymbol *block_symbol = new BlockSymbol(member -> ACC_STATIC() ? 1 : 2);
        block_symbol -> max_variable_index = 0;
        write_method -> SetBlockSymbol(block_symbol);

        AstSimpleName *base = ast_pool -> GenSimpleName(loc);

        AstFieldAccess *left_hand_side     = ast_pool -> GenFieldAccess();
        left_hand_side -> base             = base;
        left_hand_side -> dot_token        = loc;
        left_hand_side -> identifier_token = loc;
        left_hand_side -> symbol           = member;

        AstMethodDeclarator *method_declarator       = ast_pool -> GenMethodDeclarator();
        method_declarator -> identifier_token        = loc;
        method_declarator -> left_parenthesis_token  = loc;
        method_declarator -> right_parenthesis_token = loc;

        if (member -> ACC_STATIC())
        {
            method_declarator -> AllocateFormalParameters(1);

            base -> symbol = base_type;
        }
        else
        {
            method_declarator -> AllocateFormalParameters(2);

            NameSymbol *instance_name = control.MakeParameter(1);
                
            VariableSymbol *instance = block_symbol -> InsertVariableSymbol(instance_name);
            instance -> MarkSynthetic();
            instance -> SetType(base_type);
            instance -> SetOwner(write_method);
            instance -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
            write_method -> AddFormalParameter(instance);
            base -> symbol = instance;
        }

        VariableSymbol *symbol = block_symbol -> InsertVariableSymbol(member -> Identity());
        symbol -> MarkSynthetic();
        symbol -> SetType(member -> Type());
        symbol -> SetOwner(write_method);
        symbol -> SetLocalVariableIndex(block_symbol -> max_variable_index++);

        if (control.IsDoubleWordType(member -> Type()))
            block_symbol -> max_variable_index++;
        write_method -> AddFormalParameter(symbol);
        // A write access method has no throws clause !
        write_method -> SetSignature(control);

        AstSimpleName *simple_name = ast_pool -> GenSimpleName(loc);
        simple_name -> symbol      = symbol;

        AstAssignmentExpression *assignment_expression = ast_pool -> GenAssignmentExpression(AstAssignmentExpression::SIMPLE_EQUAL, loc);
        assignment_expression -> left_hand_side        = left_hand_side;
        assignment_expression -> expression            = simple_name;

        AstExpressionStatement *expression_statement  = ast_pool -> GenExpressionStatement();
        expression_statement -> expression            = assignment_expression;
        expression_statement -> semicolon_token_opt   = loc;
        expression_statement -> is_reachable          = true;
        expression_statement -> can_complete_normally = true;

        AstReturnStatement *return_statement = ast_pool -> GenReturnStatement();
        return_statement -> return_token     = loc;
        return_statement -> semicolon_token  = loc;
        return_statement -> is_reachable     = true;

        AstBlock *block            = ast_pool -> GenBlock();
        block -> left_brace_token  = loc;
        block -> right_brace_token = loc;
        block -> block_symbol      = new BlockSymbol(0); // the symbol table associated with this block will contain no element
        block -> is_reachable      = true;
        block -> AllocateBlockStatements(2); // this block contains two statements
        block -> AddStatement(expression_statement);
        block -> AddStatement(return_statement);

        AstMethodDeclaration *method_declaration  = ast_pool -> GenMethodDeclaration();
        method_declaration -> method_symbol       = write_method;
        method_declaration -> method_declarator   = method_declarator;
        method_declaration -> method_body         = block;

        write_method -> method_or_constructor_declaration = method_declaration;
        write_method -> accessed_member = member;
        MapSymbolToWriteMethod(member, base_type, write_method);
        AddPrivateAccessMethod(write_method);

        delete [] name;
    }

    return write_method;
}


MethodSymbol *TypeSymbol::GetWriteAccessFromReadAccess(MethodSymbol *read_method)
{
    assert(read_method && read_method -> IsSynthetic() &&
           read_method -> containing_type == this);
    VariableSymbol *variable = (VariableSymbol *) read_method -> accessed_member;
    assert(variable);

    AstMethodDeclaration *method_declaration = (AstMethodDeclaration *) read_method -> method_or_constructor_declaration;
    AstBlock *block = (AstBlock *) method_declaration -> method_body;
    AstReturnStatement *return_statement = (AstReturnStatement *) block -> Statement(0);
    AstFieldAccess *field_access = (AstFieldAccess *) return_statement -> expression_opt;

    return GetWriteAccessMethod(variable, field_access -> base -> Type());
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

