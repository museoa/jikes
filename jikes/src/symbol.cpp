// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#include "config.h"
#include <sys/stat.h>
#include <assert.h>
#include "stream.h"
#include "control.h"
#include "ast.h"
#include "semantic.h"
#include "table.h"
#include "zip.h"

#ifdef UNIX_FILE_SYSTEM
#include <dirent.h>
#elif defined(WIN32_FILE_SYSTEM)
#include <windows.h>
#endif

char *FileSymbol::java_suffix = StringConstant::U8S__DO_java;
int FileSymbol::java_suffix_length = strlen(java_suffix);
char *FileSymbol::class_suffix = StringConstant::U8S__DO_class;
int FileSymbol::class_suffix_length = strlen(class_suffix);

bool MethodSymbol::IsFinal()
{
    return ((AccessFlags *) this) -> ACC_FINAL() || ((AccessFlags *) this) -> ACC_PRIVATE() || containing_type -> ACC_FINAL();
}

wchar_t *MethodSymbol::Header(Semantic *sem, LexStream::TokenIndex token_location)
{
    if (! this -> IsTyped())
        this -> ProcessMethodSignature(sem, token_location);

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

        int length = this -> Type() -> ContainingPackage() -> PackageNameLength() +
                     this -> Type() -> ExternalNameLength() +
                     (is_constructor ? containing_type -> NameLength() : this -> NameLength()) + 5; // '/' after package_name
                                                                                                    // ' ' after type
                                                                                                    // '(' after name
                                                                                                    // ')' after all parameters
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
            for (wchar_t *s2 = this -> containing_type -> Name(); *s2; s2++)
                 *s++ = *s2;
        }
        else
        {
            PackageSymbol *package = this -> Type() -> ContainingPackage();
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

            for (wchar_t *s2 = this -> Type() -> ExternalName(); *s2; s2++)
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
                if (package -> PackageNameLength() > 0 && wcscmp(package_name, StringConstant::US__DO_) != 0)
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
                if (method -> FormalParameter(i) -> Type() != parameter -> parameter_symbol -> Type())
                    break;
            }
            if (i < 0)
                return method;
        }
    }

    return(MethodSymbol *)  NULL;
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

    return;
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
    wchar_t *name = new wchar_t[this -> ExternalNameLength() + (num_dimensions_ * 2) + 1];
    wcscpy(name, previous_array_type -> ExternalName());

    for (int num = array -> Length(), len = previous_array_type -> ExternalNameLength() + 2;
         num <= num_dimensions_;
         num++, len = len + 2)
    {
        wcscat(name, StringConstant::US__LB__RB_);
        NameSymbol *name_sym = sem -> control.FindOrInsertName(name, len);

        AccessFlags acc_flags;
        acc_flags.SetACC_PUBLIC();
        acc_flags.SetACC_FINAL();

        TypeSymbol *type = new TypeSymbol(name_sym);

        type -> MarkHeaderProcessed();
        type -> MarkConstructorMembersProcessed();
        type -> MarkMethodMembersProcessed();
        type -> MarkFieldMembersProcessed();
        type -> MarkLocalClassProcessingCompleted();
        type -> MarkSourceNoLongerPending();
        type -> outermost_type = type;

        type -> SetFlags(acc_flags);
        type -> super = sem -> control.Object();
        //
        // All arrays implement the interfaces java.io.Serializable and
        // java.io.Cloneable
        //
        if (sem -> control.option.one_one)
            type -> AddInterface(sem -> control.Serializable());
        type -> AddInterface(sem -> control.Cloneable());
        type -> base_type = this;
        type -> num_dimensions = num;
        type -> SetOwner(this -> ContainingPackage());
        type -> table = new SymbolTable(3); // only 2 elements will be added to this table
        type -> SetSignature(sem -> control);

        MethodSymbol *method = type -> InsertMethodSymbol(sem -> control.clone_name_symbol);
        method -> SetType(sem -> control.Object());
        method -> SetContainingType(type);
        method -> SetFlags(acc_flags);
        method -> SetBlockSymbol(new BlockSymbol(1)); // the associated symbol table will remain empty

        VariableSymbol *symbol = type -> InsertVariableSymbol(sem -> control.length_name_symbol);
        symbol -> SetFlags(acc_flags);
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

    return;
}


void TypeSymbol::SetSignature(Control &control)
{
    if (this -> num_dimensions > 0)
    {
        char *type_signature;
        TypeSymbol *subtype = this -> ArraySubtype();
        int signature_len = strlen(subtype -> SignatureString()) + 1; // +1 for '['
        type_signature = new char[signature_len + 1];                  // +1 for '\0'
        type_signature[0] = U_LEFT_BRACKET;
        strcpy(type_signature + 1, subtype -> SignatureString());
        this -> signature = control.Utf8_pool.FindOrInsert(type_signature, signature_len);
        this -> fully_qualified_name = this -> signature;
        delete [] type_signature;
    }
    else
    {
        wchar_t *package_name = this -> ContainingPackage() -> PackageName();
        wchar_t *type_name = this -> ExternalName();

        int len = this -> ContainingPackage() -> PackageNameLength() +
                  this -> ExternalNameLength() + 4; // +1 for 'L' +1 for '/' +1 for ';' +1 for '\0'
        wchar_t *type_signature = new wchar_t[len];
        wcscpy(type_signature, StringConstant::US_L);
        if (this -> ContainingPackage() -> PackageNameLength() > 0 && wcscmp(package_name, StringConstant::US__DO_) != 0)
        {
            wcscat(type_signature, package_name);
            wcscat(type_signature, StringConstant::US__SL_);
        }
        wcscat(type_signature, type_name);
        this -> fully_qualified_name = control.ConvertUnicodeToUtf8(type_signature + 1); // +1 to skip the initial L'L'

        wcscat(type_signature, StringConstant::US__SC_); 
        this -> signature = control.ConvertUnicodeToUtf8(type_signature);

        delete [] type_signature;
    }

    return;
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

    return;
}


SymbolTable::SymbolTable(int hash_size_) : anonymous_symbol_pool(NULL),
                                           type_symbol_pool(NULL),
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

    return;
}


PackageSymbol::~PackageSymbol()
{
    delete [] package_name;
    delete table;
}


void PackageSymbol::SetPackageName()
{
    this -> package_name_length = (owner ? owner -> PackageNameLength() + 1 : 0) + NameLength(); // +1 for '/'
    this -> package_name = new wchar_t[package_name_length + 1]; // +1 for '\0'

    if (owner)
    {
        wcscpy(this -> package_name, owner -> PackageName());
        wcscat(this -> package_name, StringConstant::US__SL_);
    }
    else this -> package_name[0] = U_NULL;
    wcscat(this -> package_name, this -> Name());

assert(wcslen(this -> package_name) == this -> package_name_length);

    return;
}


TypeSymbol::TypeSymbol(NameSymbol *name_symbol_) : semantic_environment(NULL),
                                                   declaration(NULL),
                                                   file_symbol(NULL),
                                                   file_location(NULL),
                                                   name_symbol(name_symbol_),
                                                   external_name_symbol(NULL),
                                                   owner(NULL),
                                                   outermost_type(NULL),
                                                   super(NULL),
                                                   base_type(NULL),
                                                   index(CycleChecker::OMEGA),
                                                   incremental_index(CycleChecker::OMEGA),
                                                   status(0),
                                                   local(NULL),
                                                   non_local(NULL),
                                                   supertypes_closure(NULL),
                                                   subtypes(NULL),
                                                   subtypes_closure(NULL),
                                                   innertypes_closure(NULL),
                                                   dependents(new SymbolSet()),
                                                   parents(new SymbolSet()),
                                                   dependents_closure(NULL),
                                                   parents_closure(NULL),
                                                   num_dimensions(0),
                                                   class_literal_class(NULL),
                                                   class_literal_method(NULL),
                                                   static_initializer_method(NULL),
                                                   block_initializer_method(NULL),
                                                   signature(NULL),
                                                   fully_qualified_name(NULL),
                                                   class_literal_name(NULL),
                                                   table(NULL),
                                                   expanded_type_table(NULL),
                                                   expanded_field_table(NULL),
                                                   expanded_method_table(NULL),
                                                   package(NULL),
                                                   class_name(NULL),
                                                   local_constructor_call_environments(NULL),
                                                   private_access_methods(NULL),
                                                   private_access_constructors(NULL),
                                                   constructor_parameters(NULL),
                                                   generated_constructors(NULL),
                                                   enclosing_instances(NULL),
                                                   class_literals(NULL),
                                                   nested_types(NULL),
                                                   interfaces(NULL),
                                                   anonymous_types(NULL),
                                                   array(NULL),
                                                   nested_type_signatures(NULL)
{
    Symbol::_kind = TYPE;
}


TypeSymbol::~TypeSymbol()
{
    delete semantic_environment;
    delete local;
    delete non_local;
    delete supertypes_closure;
    delete subtypes;
    delete subtypes_closure;
    delete innertypes_closure;
    delete dependents;
    delete parents;
    delete table;
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


BlockSymbol::BlockSymbol(int hash_size) : table(hash_size > 0 ? new SymbolTable(hash_size) : (SymbolTable *) NULL),
                                          max_variable_index(-1),
                                          try_variable_index(0)
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


DirectorySymbol::DirectorySymbol(NameSymbol *name_symbol_, Symbol *owner_) : name_symbol(name_symbol_),
                                                                             owner(owner_),
                                                                             mtime(0),
                                                                             table(NULL),
                                                                             directory_name(NULL),
                                                                             entries(NULL)
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
        if (strcmp(path_symbol -> Utf8Name(), StringConstant::U8S__DO_) == 0)
        {
            this -> directory_name_length = this -> Utf8NameLength();
            this -> directory_name = new char[this -> directory_name_length + 1]; // +1 for '\0'

            strcpy(this -> directory_name, this -> Utf8Name());
        }
        else
        {
            this -> directory_name_length = path_symbol -> Utf8NameLength();
            this -> directory_name = new char[this -> directory_name_length + 1]; // +1 for '\0'

            strcpy(this -> directory_name, path_symbol -> Utf8Name());
        }
    }
    else
    {
        DirectorySymbol *owner_directory = owner -> DirectoryCast();
        if (this -> Name()[this -> NameLength() - 1] == U_SLASH ||                     // An absolute file name ?
            strcmp(owner_directory -> DirectoryName(), StringConstant::U8S__DO_) == 0) // or is the owner "." ?
        {
            this -> directory_name_length = this -> Utf8NameLength();
            this -> directory_name = new char[this -> directory_name_length + 1];  // +1 for '\0'
            strcpy(this -> directory_name, this -> Utf8Name());
        }
        else
        {
            int owner_length = owner_directory -> DirectoryNameLength();
            char *owner_name = owner_directory -> DirectoryName();
            this -> directory_name_length = owner_length +
                                            this -> Utf8NameLength() +
                                            (owner_name[owner_length - 1] != U_SLASH ? 1 : 0); // +1 for '/'

            this -> directory_name = new char[this -> directory_name_length + 1]; // +1 for '\0'

            strcpy(this -> directory_name, owner_directory -> DirectoryName());
            if (owner_name[owner_length - 1] != U_SLASH)
                strcat(this -> directory_name, StringConstant::U8S__SL_);
            strcat(this -> directory_name, this -> Utf8Name());
        }
    }

assert(strlen(this -> directory_name) == this -> directory_name_length);

    return;
}


void DirectorySymbol::ResetDirectory()
{
assert(! this -> IsZip());

    //
    // TODO: the stat function does not work for directories in that the "modified" time stamp associated
    // with a directory is not updated when a file contained in the directory is changed.
    // For now, we always reread the directory.
    //
    //    struct stat status;
    //    if ((::SystemStat(this -> DirectoryName(), &status) == 0) && status.st_mtime > mtime)
    //    {
    //        this -> mtime = status.st_mtime;
    //
    //        delete this -> entries;
    //        this -> entries = new DirectoryTable();
    //
    //        ReadDirectory();
    //    }
    //
    //    else if (! entries) // if we cannot read the directory, allocate a default directory.
    //        this -> entries = new DirectoryTable();
    //

        delete this -> entries;
        this -> entries = new DirectoryTable();

        ReadDirectory();

    return;
}


void DirectorySymbol::ReadDirectory()
{
#ifdef UNIX_FILE_SYSTEM
    DIR *directory = opendir(this -> DirectoryName());
    if (directory)
    {
        for (dirent *entry = readdir(directory); entry; entry = readdir(directory))
        {
            int length = strlen(entry -> d_name);

            //
            // Check if the file is a java source, a java class file or a subdirectory.
            // Since packages cannot start with '.', we skip all files that start with 
            // a dot. That includes this directory "." and its parent ".."
            //
            if ((length > FileSymbol::java_suffix_length &&
                 FileSymbol::IsJavaSuffix(&entry -> d_name[length - FileSymbol::java_suffix_length]))  ||
                (length > FileSymbol::class_suffix_length &&
                 FileSymbol::IsClassSuffix(&entry -> d_name[length - FileSymbol::class_suffix_length])) ||
                ((Case::Index(entry -> d_name, U_DOT) < 0) && SystemIsDirectory(entry -> d_name)))
                entries -> InsertEntry((DirectorySymbol *) this, entry -> d_name, length);
        }
        closedir(directory);
    }
#elif defined(WIN32_FILE_SYSTEM)
    char *directory_name = new char[this -> DirectoryNameLength() + 3]; // +2 for "/*" +1 for '\0'
    strcpy(directory_name, this -> DirectoryName());
    if (directory_name[this -> DirectoryNameLength() - 1] != U_SLASH)
        strcat(directory_name, StringConstant::U8S__SL_);
    strcat(directory_name, StringConstant::U8S__ST_);

    WIN32_FIND_DATA entry;
    HANDLE file_handle = FindFirstFile(directory_name, &entry);
    if (file_handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            int length = strlen(entry.cFileName);

            //
            // Check if the file is a java source, a java class file or a subdirectory.
            // Since packages cannot start with '.', we skip all files that start with 
            // a dot. That includes this directory "." and its parent ".."
            //
            bool is_java  = (length > FileSymbol::java_suffix_length &&
                             FileSymbol::IsJavaSuffix(&entry.cFileName[length - FileSymbol::java_suffix_length])),
                 is_class = (length > FileSymbol::class_suffix_length &&
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

    return;
}


DirectorySymbol *FileSymbol::OutputDirectory()
{
    return (output_directory ? output_directory : output_directory = Control::GetOutputDirectory(this));
}


void FileSymbol::SetFileName()
{
    PathSymbol *path_symbol = this -> PathSym();
    char *directory_name = directory_symbol -> DirectoryName();
    int directory_name_length = directory_symbol -> DirectoryNameLength();
    bool dot_directory = (strcmp(directory_name, StringConstant::U8S__DO_) == 0);
    this -> file_name_length = (dot_directory ? 0 : directory_name_length) +
                               this -> Utf8NameLength()   +
                               (path_symbol -> IsZip() // For zip files we need "()" for regular directory, we need 1 '/'
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
             strcat(file_name, StringConstant::U8S__LP_);
        else if (directory_name[directory_name_length - 1] != U_SLASH)
             strcat(file_name, StringConstant::U8S__SL_);
    }
    strcat(file_name, this -> Utf8Name());
    strcat(file_name, kind == JAVA ? FileSymbol::java_suffix : FileSymbol::class_suffix);
    if (path_symbol -> IsZip())
        strcat(file_name, StringConstant::U8S__RP_);

assert(strlen(this -> file_name) == this -> file_name_length);

    return;
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

    return;
}


void TypeSymbol::SetClassName()
{
    int length;

    if (semantic_environment -> sem -> control.option.directory)
    {
        DirectorySymbol *output_directory = file_symbol -> OutputDirectory();
        int directory_length = output_directory -> DirectoryNameLength();
        char *directory_name = output_directory -> DirectoryName();
        length = directory_length + this -> ExternalUtf8NameLength() + FileSymbol::class_suffix_length + 1; // +1 for /
        class_name = new char[length + 1]; // +1 for '\0'

        strcpy(class_name, directory_name);

        if (directory_name[directory_length - 1] != U_SLASH)
            strcat(class_name, StringConstant::U8S__SL_);
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

        length = n + this -> ExternalUtf8NameLength() + FileSymbol::class_suffix_length;
        class_name = new char[length + 1]; // +1 for '\0'
        strncpy(class_name, file_name, n);
        class_name[n] = U_NULL;
    }

    strcat(class_name, ExternalUtf8Name());
    strcat(class_name, FileSymbol::class_suffix);

assert(strlen(this -> class_name) <= length);

    return;
}


void TypeSymbol::ProcessNestedTypeSignatures(Semantic *sem, LexStream::TokenIndex tok)
{
    for (int i = 0; i < NumNestedTypeSignatures(); i++)
    {
        int length = strlen(NestedTypeSignature(i));
        wchar_t *nested_class_name = new wchar_t[length + 1];
        sem -> ConvertUtf8ToUnicode(nested_class_name, NestedTypeSignature(i), length);
        NameSymbol *name_symbol = sem -> control.FindOrInsertName(nested_class_name, length);

        delete [] nested_class_name;
        delete [] NestedTypeSignature(i);

        sem -> ProcessNestedType(this, name_symbol, tok);
    }

    delete nested_type_signatures;
    nested_type_signatures = NULL;

    return;
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
            int length = strlen(ThrowsSignature(i));
            wchar_t *type_name = new wchar_t[length + 1];
            sem -> ConvertUtf8ToUnicode(type_name, ThrowsSignature(i), length);
            this -> AddThrows(sem -> ReadType(this -> containing_type, type_name, tok));
            delete [] type_name;
            delete [] ThrowsSignature(i);
        }

        delete throws_signatures;
        throws_signatures = NULL;
    }

    return;
}


void MethodSymbol::SetSignature(Control &control, VariableSymbol *this0_variable)
{
    int len = 2 + strlen(this -> Type() -> SignatureString()); // +1 for '(' +1 for ')'

    if (this0_variable)
        len += strlen(this0_variable -> Type() -> SignatureString());
    for (int i = 0; i < this -> NumFormalParameters(); i++)
    {
        TypeSymbol *formal_type = this -> FormalParameter(i) -> Type();
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
    for (int j = 0; j < this -> NumFormalParameters(); j++)
    {
        TypeSymbol *formal_type = this -> FormalParameter(j) -> Type();
        for (char *str = formal_type -> SignatureString(); *str; str++, k++)
            method_signature[k] = *str;
    }
    method_signature[k++] = U_RIGHT_PARENTHESIS;
    for (char *str = this -> Type() -> SignatureString(); *str; str++, k++)
         method_signature[k] = *str;
    method_signature[k] = U_NULL;

    this -> signature = control.Utf8_pool.FindOrInsert(method_signature, len);

    delete [] method_signature;

    return;
}


void MethodSymbol::ProcessMethodSignature(Semantic *sem, LexStream::TokenIndex token_location)
{
    if (! this -> type_)
    {
assert(sem);
        int num_parameters = 0;
        char *signature = this -> SignatureString();
        signature++; // +1 to skip initial '('

        //
        // For the constructor of an inner type, skip the "this$0" argument.
        //
        if (this -> containing_type -> IsInner() && this -> Identity() == sem -> control.init_name_symbol)
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
            // Make up a name for each parameter... Recall that for an inner class we need to skip the this$0 parameter
            //
            NameSymbol *name_symbol = sem -> control.MakeParameter(++num_parameters);
            VariableSymbol *symbol = new VariableSymbol(name_symbol);
            symbol -> SetType(sem -> ProcessSignature(this -> containing_type, signature, token_location));
            symbol -> MarkComplete();
            this -> AddFormalParameter(symbol);

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
        this -> SetType(sem -> ProcessSignature(this -> containing_type, signature, token_location));

        //
        // Create a symbol table for this method for consistency... and in order
        // to release the space used by the variable paramaters later.
        //
        BlockSymbol *block_symbol = new BlockSymbol(num_parameters);
        for (int k = 0; k < num_parameters; k++)
            block_symbol -> InsertVariableSymbol((*this -> formal_parameters)[k]);
        block_symbol -> CompressSpace(); // space optimization
        this -> SetBlockSymbol(block_symbol);
    }

    return;
}


void MethodSymbol::CleanUp()
{
    BlockSymbol *block_symbol = new BlockSymbol(this -> NumFormalParameters());

    //
    // Make a copy of each parameter into the new pared-down symbol table and
    // fix the FormalParameter information to identify the new symbol.
    //
    for (int k = 0; k < this -> NumFormalParameters(); k++)
    {
        VariableSymbol *formal_parameter = (*formal_parameters)[k],
                       *symbol = block_symbol -> InsertVariableSymbol(formal_parameter -> Identity());
        symbol -> SetType(formal_parameter -> Type());
        symbol -> MarkComplete();
        (*formal_parameters)[k] = symbol;
    }

    //
    // Destroy the old symbol and replace it by the new one.
    //
    delete this -> block_symbol;
    block_symbol -> CompressSpace(); // space optimization
    this -> SetBlockSymbol(block_symbol);

    this -> method_or_constructor_declaration = NULL; // remove reference to Ast structure

    return;
}


void VariableSymbol::ProcessVariableSignature(Semantic *sem, LexStream::TokenIndex token_location)
{
    if (! this -> type_)
    {
assert(sem);
        this -> SetType(sem -> ProcessSignature((TypeSymbol *) owner, signature_string, token_location));
    }

    return;
}


bool TypeSymbol::CanAccess(TypeSymbol *type)
{
assert(semantic_environment);
    SemanticEnvironment *env = this -> semantic_environment;
    //
    // Note that this case is only possible in the current environment (i.e., it is not recursively applicable)
    // as it is not possible to declare an inner class (even an anonymous one) within an explicit
    // constructor invocation. This is the case precisely because of the unavailability of the "this"
    // which one would need to pass to the inner class in question. See comment below...
    //
    if (env -> explicit_constructor_invocation && env -> explicit_constructor_invocation -> SuperCallCast())
    {
        //
        // If we are processing a super call statement in the constructor of an inner class then
        // we have access to the this$0 (passed to the constructor of the inner class as parameter) but
        // not to the "this" of the inner class in question, unless it is static.
        //
        // 1.2 change. In 1.1, we used to allow access to any subclass of type. Now, there must
        // be a perfect match. I.e.,
        //
        // "if (this -> IsSubclass(type))" becomes "if (this == type)"
        // "if (this -> ContainingType() -> IsSubclass(type))" becomes "if (this -> ContainingType() == type)"
        // "else if (env -> Type() -> IsSubclass(type))" becomes ""else if (env -> Type() == type)"
        //
        if (this == type)
            return this -> ACC_STATIC(); // "this" class is accessible only if it is static.
        if (this -> IsInner())
        {
            if (this -> ContainingType() == type)
                return true;
            env = env -> previous; // skip "this" type.
        }
    }

    for (; env != NULL; env = env -> previous)
    {
        if (env -> StaticRegion()) // are we in a static environment?
             break;
        else if (env -> Type() == type) // then, check if we reached the type in question.
             return true;
        else if (env -> Type() -> ACC_STATIC())     // next, check whether or not the current type is static...
             break;
    }

    return false;
}


//
// Given two types T and T2 in different packages, the type T has protected
// access to T2 iff T or any class in which T is lexically enclosed is a subclass
// of T2 or of some other type T3 that lexically encloses T2.
//
// Of course, T2 and all its enclosing classes, if any, must have been declared
// either public or protected, otherwise they could not be eligible as a superclass
// candidate. We do not (and need not) check for that condition here.
//
bool TypeSymbol::HasProtectedAccessTo(TypeSymbol *target_type)
{
assert(semantic_environment);
    
    for (SemanticEnvironment *env = this -> semantic_environment; env != NULL; env = env -> previous)
    {
        TypeSymbol *main_type = env -> Type();
        for (TypeSymbol *type = target_type; type; type = type -> owner -> TypeCast())
        {
            if (main_type -> IsSubclass(type)) // then, check if we reached the type in question or one of its superclasses;
                return true;
        }
    }

    return false;
}


inline NameSymbol *TypeSymbol::GetThisName(Control &control, int n)
{
    wchar_t info[12],
            *str = &info[11];
    *str = U_NULL;
    int num = n;
    do
    {
        *--str = (U_0 + num % 10);
        num /= 10;
    } while (num != 0);

    int length = wcslen(StringConstant::US__this_DOLLAR) + (&info[11] - str);
    wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
    wcscpy(name, StringConstant::US__this_DOLLAR);
    wcscat(name, str);
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

assert(this == outermost_type);

    if (! this -> class_literal_class)
    {
        AstClassInstanceCreationExpression *class_creation = compilation_unit -> ast_pool -> GenClassInstanceCreationExpression();
        class_creation -> base_opt      = NULL;
        class_creation -> dot_token_opt = 0;
        class_creation -> new_token = tok;

             AstSimpleName *ast_type= compilation_unit -> ast_pool -> GenSimpleName(tok);

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

        this -> class_literal_class = class_literal_type;
    }

    return this -> class_literal_class;
}


MethodSymbol *TypeSymbol::FindOrInsertClassLiteralMethod(Control &control)
{
    if (! class_literal_method)
    {
        BlockSymbol *block_symbol = new BlockSymbol(1); // the associated symbol table will contain 1 element
        block_symbol -> max_variable_index = 0;

        NameSymbol *name_symbol = control.FindOrInsertName(StringConstant::US__class_DOLLAR,
                                                           wcslen(StringConstant::US__class_DOLLAR));

        class_literal_method = InsertMethodSymbol(name_symbol);
        class_literal_method -> SetType(control.Class());
        class_literal_method -> SetACC_STATIC();
        class_literal_method -> SetContainingType((TypeSymbol *) this);
        class_literal_method -> SetBlockSymbol(block_symbol);

            VariableSymbol *variable_symbol = block_symbol -> InsertVariableSymbol(control.MakeParameter(1));
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
assert(IsTopLevel() && (! type -> Primitive()));
    Semantic *sem = semantic_environment -> sem;
    Control &control = sem -> control;
 
    (void) this -> FindOrInsertClassLiteralMethod(control);

    (void) type -> FindOrInsertClassLiteralName(control);

    NameSymbol *name_symbol = NULL;
    char *signature = type -> SignatureString();
    if (signature[0] == U_LEFT_BRACKET) // an array?
    {
        int array_length = wcslen(StringConstant::US__array),
            length = strlen(signature) + array_length;
        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US__array);
        int i,
            k;
        for (i = 0, k = array_length; signature[i] == U_LEFT_BRACKET; i++, k++)
            name[k] = U_DOLLAR;
        for (wchar_t ch = signature[i++]; ch && ch != U_SEMICOLON; ch = signature[i++])
            name[k++] = (ch == U_SLASH ? (wchar_t) U_DOLLAR : ch);
        name[k] = U_NULL;
        name_symbol = control.FindOrInsertName(name, k);

        delete [] name;
    }
    else if (signature[0] == U_L) // a reference type ?
    {
        int class_length = wcslen(StringConstant::US__class_DOLLAR),
            length = strlen(signature) + class_length;

        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US__class_DOLLAR);
        int i = 0,
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
        variable_symbol = InsertVariableSymbol(name_symbol);
        variable_symbol -> SetType(control.Class());
        variable_symbol -> SetACC_PRIVATE();
        variable_symbol -> SetACC_STATIC();
        variable_symbol -> SetOwner((TypeSymbol *) this);
        variable_symbol -> MarkComplete();

        AddClassLiteral(variable_symbol);
    }

    return variable_symbol;
}


VariableSymbol *TypeSymbol::FindOrInsertLocalShadow(VariableSymbol *local)
{
assert(this -> IsLocal());
    Control &control = semantic_environment -> sem -> control;
    VariableSymbol *variable = FindVariableSymbol(local -> Identity());

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
        variable = InsertVariableSymbol(local -> Identity());
        variable -> accessed_local = local;
        variable -> SetType(local -> Type());
        variable -> SetACC_PRIVATE();
//        variable -> SetACC_FINAL();
        variable -> SetOwner((TypeSymbol *) this);
        variable -> MarkComplete();

        int length = 4 + local -> NameLength(); // +4 for val$
        wchar_t *external_name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(external_name, StringConstant::US__val_DOLLAR);
        wcscat(external_name, local -> Name());

        variable -> SetExternalIdentity(control.FindOrInsertName(external_name, length));
        AddConstructorParameter(variable);

        delete [] external_name;

        for (int i = 0; i < NumGeneratedConstructors(); i++)
        {
            MethodSymbol *constructor = GeneratedConstructor(i);
            VariableSymbol *symbol = constructor -> block_symbol -> FindVariableSymbol(local -> Identity());
            if (! symbol)
            {
                symbol = constructor -> block_symbol -> InsertVariableSymbol(local -> Identity());
                symbol -> SetType(variable -> Type());
                symbol -> SetOwner(constructor);
                symbol -> SetExternalIdentity(external_name_symbol);
                symbol -> SetLocalVariableIndex(constructor -> block_symbol -> max_variable_index++);
                if (symbol -> Type() == control.long_type || symbol -> Type() == control.double_type)
                    constructor -> block_symbol -> max_variable_index++;
                constructor -> AddFormalParameter(symbol);
            }
        }
    }

    return variable;
}


MethodSymbol *TypeSymbol::GetReadAccessMethod(MethodSymbol *member)
{
    if (! member -> read_method)
    {
        TypeSymbol *type = member -> containing_type;
        Semantic *sem = type -> semantic_environment -> sem;
        Control &control = sem -> control;
        StoragePool *ast_pool = sem -> compilation_unit -> ast_pool;

        wchar_t info[12],
                *str = &info[11];
        int n = (member -> Identity() == control.init_name_symbol ? type -> NumPrivateAccessConstructors()
                                                                  : type -> NumPrivateAccessMethods());
        *str = U_NULL;
        do
        {
            *--str = (U_0 + n % 10);
            n /= 10;
        } while (n != 0);

        int length = 7 + (&info[11] - str); // +7 for access$
        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US__access_DOLLAR);
        wcscat(name, str);

        BlockSymbol *block_symbol = new BlockSymbol(member -> NumFormalParameters() + 3);

        MethodSymbol *method = type -> InsertMethodSymbol(control.FindOrInsertName(name, length));
        method -> SetType(member -> Type());
        if (member -> ACC_STATIC())
        {
            method -> SetACC_STATIC();
            block_symbol -> max_variable_index = 0;
        }
        else block_symbol -> max_variable_index = 1;
        method -> SetContainingType(type);
        method -> SetBlockSymbol(block_symbol);

        for (int i = 0; i < member -> NumThrows(); i++)
            method -> AddThrows(member -> Throws(i));

        //
        //
        //
        if (member -> Identity() != control.init_name_symbol) // not a constructor
        {
            for (int i = 0; i < member -> NumFormalParameters(); i++)
            {
                VariableSymbol *parm = method -> block_symbol -> InsertVariableSymbol(member -> FormalParameter(i) -> Identity());
                parm -> SetType(member -> FormalParameter(i) -> Type());
                parm -> SetOwner(method);
                parm -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
                if (parm -> Type() == control.long_type || parm -> Type() == control.double_type)
                    block_symbol -> max_variable_index++;
                method -> AddFormalParameter(parm);
            }
            method -> SetSignature(control);
            // A read access method has no throws clause !

            type -> AddPrivateAccessMethod(method);
        }
        else
        {
            method -> SetExternalIdentity(member -> Identity());

            Ast *declaration = member -> method_or_constructor_declaration;
            AstMethodDeclaration *method_declaration = declaration -> MethodDeclarationCast();
            AstMethodDeclarator *declarator = (method_declaration
                                                     ? method_declaration -> method_declarator
                                                     : ((AstConstructorDeclaration *) declaration) -> constructor_declarator);
            LexStream::TokenIndex loc = declarator -> identifier_token;

            AstMethodDeclarator *method_declarator      = ast_pool -> GenMethodDeclarator();
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
            if (type -> IsInner())
            {
                this0_variable = block_symbol -> InsertVariableSymbol(control.this0_name_symbol);
                this0_variable -> SetType(type -> ContainingType());
                this0_variable -> SetOwner(method);
                this0_variable -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
            }
            for (int i = 0; i < member -> NumFormalParameters(); i++)
            {
                VariableSymbol *parm = method -> block_symbol -> InsertVariableSymbol(member -> FormalParameter(i) -> Identity());
                parm -> SetType(member -> FormalParameter(i) -> Type());
                parm -> SetOwner(method);
                parm -> SetLocalVariableIndex(block_symbol -> max_variable_index++);
                if (parm -> Type() == control.long_type || parm -> Type() == control.double_type)
                    block_symbol -> max_variable_index++;
                method -> AddFormalParameter(parm);

                //
                // Since private_access_constructors will compiled (see body.cpp), we must create
                // valid ast_simple_names for its parameters.
                //
                AstVariableDeclaratorId *variable_declarator_name = declarator -> FormalParameter(i) -> variable_declarator_name;
                AstSimpleName *simple_name = ast_pool -> GenSimpleName(variable_declarator_name -> identifier_token);
                simple_name -> symbol = parm;
                this_call -> AddArgument(simple_name);
            }
            method -> SetSignature(control, this0_variable);
            // A read access method has no throws clause !

            AstReturnStatement *return_statement = ast_pool -> GenReturnStatement();
            return_statement -> return_token = loc;
            return_statement -> expression_opt = NULL;
            return_statement -> semicolon_token = loc;
            return_statement -> is_reachable = true;

            AstBlock *block = ast_pool -> GenBlock();
            block -> AllocateBlockStatements(1); // this block contains one statement
            block -> left_brace_token  = loc;
            block -> right_brace_token = loc;

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

            constructor_declaration -> constructor_symbol = method;
            method -> method_or_constructor_declaration = constructor_declaration;

            if (type -> IsLocal())
                sem -> GenerateLocalConstructor(method);

            type -> AddPrivateAccessConstructor(method);
        }

        method -> accessed_member = member;
        member -> read_method = method;

        delete [] name;
    }

    return member -> read_method;
}


MethodSymbol *TypeSymbol::GetReadAccessMethod(VariableSymbol *member)
{
    if (! member -> read_method)
    {
        TypeSymbol *type = member -> owner -> TypeCast();
        Semantic *sem = type -> semantic_environment -> sem;
        Control &control = sem -> control;

        wchar_t info[12],
                *str = &info[11];
        int n = type -> NumPrivateAccessMethods();
        *str = U_NULL;
        do
        {
            *--str = (U_0 + n % 10);
            n /= 10;
        } while (n != 0);

        int length = 7 + (&info[11] - str); // +7 for access$
        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US__access_DOLLAR);
        wcscat(name, str);

        MethodSymbol *method = type -> InsertMethodSymbol(control.FindOrInsertName(name, length));
        method -> SetType(member -> Type());
        if (member -> ACC_STATIC())
            method -> SetACC_STATIC();
        method -> SetContainingType(type);
        // No block_symbol is associated with an access method !
        // A read access method for a field has no formal parameters !
        method -> SetSignature(control);
        // A read access method has no throws clause !

        method -> accessed_member = member;
        member -> read_method = method;
        type -> AddPrivateAccessMethod(method);

        delete [] name;
    }

    return member -> read_method;
}


MethodSymbol *TypeSymbol::GetWriteAccessMethod(VariableSymbol *member)
{
    if (! member -> write_method)
    {
        TypeSymbol *type = member -> owner -> TypeCast();
        Semantic *sem = type -> semantic_environment -> sem;
        Control &control = sem -> control;

        wchar_t info[12],
                *str = &info[11];
        int n = type -> NumPrivateAccessMethods();
        *str = U_NULL;
        do
        {
            *--str = (U_0 + n % 10);
            n /= 10;
        } while (n != 0);

        int length = 7 + (&info[11] - str); // +7 for access$
        wchar_t *name = new wchar_t[length + 1]; // +1 for '\0';
        wcscpy(name, StringConstant::US__access_DOLLAR);
        wcscat(name, str);

        MethodSymbol *method = type -> InsertMethodSymbol(control.FindOrInsertName(name, length));
        method -> SetType(sem -> control.void_type);
        if (member -> ACC_STATIC())
            method -> SetACC_STATIC();
        method -> SetContainingType(type);
        method -> SetBlockSymbol(new BlockSymbol(3)); // the associated symbol table will contain 1 element
        //
        // A write access method takes exactly one argument of the same type
        // as the variable. For a member named "m", its body will consist of
        // the single statement:
        //
        //    this.m = m;
        //
        VariableSymbol *symbol = method -> block_symbol -> InsertVariableSymbol(member -> Identity());
        symbol -> SetType(member -> Type());
        method -> AddFormalParameter(symbol);

        method -> SetSignature(control);
        // A write access method has no throws clause !

        method -> accessed_member = member;
        member -> write_method = method;
        type -> AddPrivateAccessMethod(method);

        delete [] name;
    }

    return member -> write_method;
}
