// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#include "config.h"
#include <sys/stat.h>
#include "control.h"
#include "semantic.h"
#include "zip.h"

int Control::ConvertUnicodeToUtf8(wchar_t *source, char *target)
{
    int length = 0;

    for (; *source; source++)
    {
        int ch = *source;

        if (ch == 0)
        {
             target[length++] = (char) 0xC0;
             target[length++] = (char) 0x80;
        }
        else if (ch <= 0x007F)
             target[length++] = (char) ch;
        else if (ch <= 0x07FF)
        {
             target[length++] = (char) ((char) 0xC0 | (char) ((ch >> 6) & 0x001F)); // bits 6-10
             target[length++] = (char) ((char) 0x80 | (char) (ch & 0x003F));        // bits 0-5
        }
        else
        {
             target[length++] = (char) ((char) 0xE0 | (char) ((ch >> 12) & 0x000F));
             target[length++] = (char) ((char) 0x80 | (char) ((ch >> 6) & 0x003F));
             target[length++] = (char) ((char) 0x80 | (char) (ch & 0x3F));
        }
    }
    target[length] = U_NULL;

    return length;
}


void Control::FindPathsToDirectory(PackageSymbol *package)
{
    if (package -> directory.Length() == 0)
    {
        PackageSymbol *owner_package = package -> owner;
        if (owner_package) // package is a subpackage?
        {
            for (int i = 0; i < owner_package -> directory.Length(); i++)
            {
                DirectorySymbol *owner_directory_symbol = owner_package -> directory[i],
                                *subdirectory_symbol = owner_directory_symbol -> FindDirectorySymbol(package -> Identity());
                if ((! subdirectory_symbol) && (! owner_directory_symbol -> IsZip()))
                {
                    int length = owner_directory_symbol -> DirectoryNameLength() + package -> Utf8NameLength() + 1; // +1 for '/'
                    char *directory_name = new char[length + 1]; // +1 for '\0';

                    strcpy(directory_name, owner_directory_symbol -> DirectoryName());
                    if (owner_directory_symbol -> DirectoryName()[owner_directory_symbol -> DirectoryNameLength() - 1] != U_SLASH)
                        strcat(directory_name, StringConstant::U8S__SL_);
                    strcat(directory_name, package -> Utf8Name());

                    if (::SystemIsDirectory(directory_name))
                        subdirectory_symbol = owner_directory_symbol -> InsertAndReadDirectorySymbol(package -> Identity());

                    delete [] directory_name;
                }

                if (subdirectory_symbol)
                    package -> directory.Next() = subdirectory_symbol;
            }
        }
        else
        {
            //
            // Recall that since classpath[0] contains the default directory, we always
            // start searching at location 1.
            //
            for (int k = 1; k < classpath.Length(); k++)
            {
                PathSymbol *path_symbol = classpath[k];
                DirectorySymbol *directory_symbol = path_symbol -> RootDirectory() -> FindDirectorySymbol(package -> Identity());
                if ((! directory_symbol) && (! path_symbol -> IsZip()))
                {
                    int length = path_symbol -> Utf8NameLength() + package -> Utf8NameLength() + 1; // +1 for '/'
                    char *directory_name = new char[length + 1]; // +1 for '/' +1 for '\0'
                    strcpy(directory_name, path_symbol -> Utf8Name());
                    strcat(directory_name, StringConstant::U8S__SL_);
                    strcat(directory_name, package -> Utf8Name());

                    if (::SystemIsDirectory(directory_name))
                        directory_symbol = path_symbol -> RootDirectory() -> InsertAndReadDirectorySymbol(package -> Identity());
                    delete [] directory_name;
                }

                if (directory_symbol)
                    package -> directory.Next() = directory_symbol;
            }
        }
    }

    return;
}


void Control::ProcessGlobalNameSymbols()
{
    this -> dot_name_symbol        = FindOrInsertName(US__DO_, wcslen(US__DO_));
    this -> dot_dot_name_symbol    = FindOrInsertName(US__DO__DO_, wcslen(US__DO__DO_));
    this -> length_name_symbol     = FindOrInsertName(US_length, wcslen(US_length));
    this -> init_name_symbol       = FindOrInsertName(US__LT_init_GT_, wcslen(US__LT_init_GT_));
    this -> clinit_name_symbol     = FindOrInsertName(US__LT_clinit_GT_, wcslen(US__LT_clinit_GT_));
    this -> block_init_name_symbol = FindOrInsertName(US_block_DOLLAR, wcslen(US_block_DOLLAR));
    this -> this0_name_symbol      = FindOrInsertName(US_this0, wcslen(US_this0));
    this -> clone_name_symbol      = FindOrInsertName(US_clone, wcslen(US_clone));
    this -> object_name_symbol     = FindOrInsertName(US_Object, wcslen(US_Object));
    this -> type_name_symbol       = FindOrInsertName(US_TYPE, wcslen(US_TYPE));

    return;
}


//
// Create the unnamed package and set up global names.
//
void Control::ProcessUnnamedPackage()
{
    unnamed_package = external_table.InsertPackageSymbol(FindOrInsertName(US_EMPTY, wcslen(US_EMPTY)), NULL);

    //
    // Create an entry for no_type. no_type is used primarily to signal an error
    //
    no_type = unnamed_package -> InsertSystemTypeSymbol(FindOrInsertName(US__QU__QU_, wcslen(US__QU__QU_)));
    no_type -> SetSignature(Utf8_pool.FindOrInsert(U8S__DO_, strlen(U8S__DO_))); // give it some signature...
    no_type -> outermost_type = no_type;
    no_type -> SetOwner(unnamed_package);
    no_type -> MarkBad();

    //
    // Create an entry for the null type.
    //
    null_type = unnamed_package -> InsertSystemTypeSymbol(FindOrInsertName(US_null, wcslen(US_null)));
    null_type -> outermost_type = null_type;
    null_type -> SetOwner(unnamed_package);
    null_type -> SetACC_PUBLIC();

    return;
}


void Control::ProcessPath()
{
    //
    // We need a place to start. Allocate a "." directory with no owner initially. (Hence, the null argument.)
    // Allocate a "." path whose associated directory is the "." directory.
    // Identify the "." path as the owner of the "." directory.
    //
    DirectorySymbol *dot_directory = new DirectorySymbol(dot_name_symbol, NULL);
    classpath.Next() = classpath_table.InsertPathSymbol(dot_name_symbol, dot_directory);
    dot_directory -> ResetDirectory(); // Note that main_root_directory is reset after it has been assigned an owner above
    root_directories.Next() = dot_directory;

    //
    //
    //
    if (option.classpath)
    {
        int max_path_name_length = strlen(option.classpath) + 1; // The longest possible path name we can encounter
        wchar_t *path_name = new wchar_t[max_path_name_length + 1]; // +1 for '\0'

        wchar_t *input_name = NULL;
        char *full_directory_name = NULL;

        for (char *path = option.classpath, *path_tail = &path[strlen(path)]; path < path_tail; path++)
        {
#ifdef WIN32_FILE_SYSTEM
            delete [] full_directory_name;
            delete [] input_name;
#endif
            char *head;
            for (head = path; path < path_tail && *path != ::PathSeparator(); path++)
                ;
            *path = U_NULL; // If a seperator was encountered, replace it by \0 to terminate the string.
            int input_name_length = path - head;

            int path_name_length = input_name_length;
            for (int i = 0; i < path_name_length; i++)
                path_name[i] = head[i];
            path_name[path_name_length] = U_NULL;

#ifdef UNIX_FILE_SYSTEM

            input_name = path_name;

#elif defined(WIN32_FILE_SYSTEM)

            input_name = NULL;
            full_directory_name = NULL;
            char disk = (input_name_length >= 2 && Case::IsAsciiAlpha(head[0]) && head[1] == U_COLON ? head[0] : 0);

            //
            // Look for the directory. If it is found, update input_name and head.
            //
            option.SaveCurrentDirectoryOnDisk(disk);
            if (SetCurrentDirectory(head))
            {
                char tmp[1];
                DWORD directory_length = GetCurrentDirectory(0, tmp); // first, get the right size
                full_directory_name = new char[directory_length + 1];  // allocate the directory
                DWORD length = GetCurrentDirectory(directory_length, full_directory_name);
                if (length <= directory_length)
                {
                    for (char *ptr = full_directory_name; *ptr; ptr++)
                        *ptr = (*ptr != U_BACKSLASH ? *ptr : (char) U_SLASH); // turn '\' to '/'.

                    input_name_length = length;
                    input_name = new wchar_t[input_name_length + 1];
                    for (int k = 0; k < input_name_length; k++)
                        input_name[k] = full_directory_name[k];
                    input_name[input_name_length] = U_NULL;
                    head = full_directory_name;
                }
            }

            //
            // Default input_name, in case we do not succeed in finding the directory
            //
            if (! input_name)
            {
                input_name = new wchar_t[input_name_length + 1];
                for (int j = 0; j < input_name_length; j++)
                    input_name[j] = path_name[j];
                input_name[input_name_length] = U_NULL;
            }

            option.ResetCurrentDirectoryOnDisk(disk); // reset the current directory on disk
            option.SetMainCurrentDirectory();         // reset the real current directory...
#endif

            //
            //
            //
            if (input_name_length > 0)
            {
                NameSymbol *name_symbol = FindOrInsertName(path_name, path_name_length);

                //
                // If a directory is specified more than once, ignore the duplicates.
                //
                PathSymbol *path_symbol = classpath_table.FindPathSymbol(name_symbol);
                if (path_symbol)
                {
                    if (name_symbol == dot_name_symbol)
                    {
                        dot_classpath_index = classpath.Length(); // The next index
                        classpath.Next() = classpath[0];          // share the "." directory
                        unnamed_package -> directory.Next() = classpath[0] -> RootDirectory();
                    }

                    continue;
                }

                //
                // Check whether or not the path points to a system directory or a zip file
                //
                Zip *zipinfo = (::SystemIsDirectory(head) ? (Zip *) NULL : new Zip(*this, head));

                //
                // If we have an invalid zip file issue an error and continue...
                //
                DirectorySymbol *dot_directory;
                if (zipinfo)
                {
                     if (! zipinfo -> IsValid())
                     {
                         wchar_t *name = new wchar_t[input_name_length + 1];
                         for (int i = 0; i < input_name_length; i++)
                             name[i] = input_name[i];
                         name[input_name_length] = U_NULL;
                         bad_zip_filenames.Next() = name;
   
                         delete zipinfo;
                         continue;
                     }

                     dot_directory = new DirectorySymbol(dot_name_symbol, NULL);
                     root_directories.Next() = dot_directory;
                }
                else dot_directory = ProcessSubdirectories(input_name, input_name_length);

                //
                // Create the new path symbol and update the class path with it.
                //
                path_symbol = classpath_table.InsertPathSymbol(name_symbol, dot_directory);
                classpath.Next() = path_symbol;

                //
                // If the path symbol is associated with a zip file, read the zip directory.
                //
                if (zipinfo)
                {
                    path_symbol -> zipfile = zipinfo;
                    zipinfo -> ReadDirectory(dot_directory);
                }

                unnamed_package -> directory.Next() = dot_directory;
            }
        }

#ifdef WIN32_FILE_SYSTEM
        delete [] full_directory_name;
        delete [] input_name;
#endif

        delete [] path_name;
    }

    //
    // TODO: If the user did not specify "." in the class path we assume it.
    // javac makes that assumption also. Is that correct?
    //
    if (dot_classpath_index == 0)
        unnamed_package -> directory.Next() = classpath[0] -> RootDirectory();

    return;
}


TypeSymbol *Control::GetPrimitiveType(wchar_t *name, char *signature)
{
    NameSymbol *name_symbol = FindOrInsertName(name, wcslen(name));
    TypeSymbol *type = unnamed_package -> InsertSystemTypeSymbol(name_symbol);

    type -> SetSignature(Utf8_pool.FindOrInsert(signature, strlen(signature)));
    type -> outermost_type = type;
    type -> SetOwner(unnamed_package);
    type -> SetACC_PUBLIC();
    type -> MarkPrimitive();

    return type;
}


void Control::ProcessSystemInformation()
{
    //
    // Add entry for system package
    //
    system_package = ProcessPackage(StringConstant::US_java_SL_lang);

    //
    // Create an entry for each primitive type. Note that the type void is treated as a primitive.
    //
    void_type = GetPrimitiveType(US_void, U8S_V);
    boolean_type = GetPrimitiveType(US_boolean, U8S_Z);
    byte_type = GetPrimitiveType(US_byte, U8S_B);
    char_type = GetPrimitiveType(US_char, U8S_C);
    short_type = GetPrimitiveType(US_short, U8S_S);
    int_type = GetPrimitiveType(US_int, U8S_I);
    long_type = GetPrimitiveType(US_long, U8S_J);
    float_type = GetPrimitiveType(US_float, U8S_F);
    double_type = GetPrimitiveType(US_double, U8S_D);

    return;
}    


DirectorySymbol *Control::GetOutputDirectory(FileSymbol *file_symbol)
{
    DirectorySymbol *directory_symbol;

    Control &control = file_symbol -> semantic -> control;
    if (control.option.directory == NULL)
        directory_symbol = file_symbol -> directory_symbol;
    else
    {
        char *directory_prefix = control.option.directory;
        int directory_prefix_length = strlen(directory_prefix),
            utf8_name_length = file_symbol -> package -> PackageNameLength() * 3,
            estimated_length = directory_prefix_length + utf8_name_length + 1; // +1 for slash

        char *directory_name = new char[estimated_length + 1]; // +1 for '\0'

        strcpy(directory_name, directory_prefix);

        if (file_symbol -> package != control.unnamed_package) // if there was a package declaration, then...
        {
            strcat(directory_name, StringConstant::U8S__SL_);
            char *utf8_name = new char[utf8_name_length + 1];
            (void) ConvertUnicodeToUtf8(file_symbol -> package -> PackageName(), utf8_name);
            strcat(directory_name, utf8_name);
            delete [] utf8_name;

            if (! ::SystemIsDirectory(directory_name)) // The directory does not yet exist. 
            {
                for (char *ptr = &directory_name[directory_prefix_length + 1]; *ptr; ptr++)
                {
                    if (*ptr == U_SLASH) // all the slashes in a package_name are forward slashes
                    {
                        *ptr = U_NULL;
                        if (! ::SystemIsDirectory(directory_name))
                            ::SystemMkdir(directory_name);
                        *ptr = U_SLASH; // restore slash
                    }
                }
                ::SystemMkdir(directory_name);
            }
        }

        //
        // Read the directory and insert the directory symbol. Note that since 
        // the original length computation was an estimate, we compute the real
        // length here.
        //
        int length = strlen(directory_name);
        wchar_t *name = new wchar_t[length + 1];
        for (int i = 0; i < length; i++)
            name[i] = directory_name[i];
        name[length] = U_NULL;

        directory_symbol = control.ProcessSubdirectories(name, length);

        delete [] directory_name;
    }

    return directory_symbol;
}


FileSymbol *Control::GetJavaFile(PackageSymbol *package, NameSymbol *name_symbol)
{
    FileSymbol *file_symbol = NULL;

    //
    //
    //
    int length = name_symbol -> Utf8NameLength() + FileSymbol::java_suffix_length;
    char *full_filename = new char[length + 1]; // +1 for \0
    strcpy(full_filename, name_symbol -> Utf8Name());
    strcat(full_filename, FileSymbol::java_suffix);

    DirectoryEntry *entry = NULL;
    DirectorySymbol *directory_symbol = NULL;
    for (int k = 0; k < package -> directory.Length(); k++)
    {
        directory_symbol = package -> directory[k];
        if (entry = directory_symbol -> FindEntry(full_filename, length))
            break;
    }

    if (entry)
    {
        PathSymbol *path_symbol = directory_symbol -> PathSym();

        file_symbol = directory_symbol -> FindFileSymbol(name_symbol);
        if (! ((file_symbol && file_symbol -> IsJava()) || path_symbol -> IsZip()))
        {
            file_symbol = directory_symbol -> InsertFileSymbol(name_symbol);

            file_symbol -> directory_symbol = directory_symbol;
            file_symbol -> SetJava();
            file_symbol -> mtime = entry -> Mtime();
        }
    }

    delete [] full_filename;

    return file_symbol;
}


FileSymbol *Control::GetFile(PackageSymbol *package, NameSymbol *name_symbol, bool depend)
{
    FileSymbol *file_symbol = NULL;

    //
    // calculate a length that is large enough...
    //
    int class_length = name_symbol -> Utf8NameLength() + FileSymbol::class_suffix_length,
        java_length = name_symbol -> Utf8NameLength() + FileSymbol::java_suffix_length;

    char *class_name = new char[class_length + 1]; // +1 for \0
    strcpy(class_name, name_symbol -> Utf8Name());
    strcat(class_name, FileSymbol::class_suffix);

    char *java_name = new char[java_length + 1]; // +1 for \0
    strcpy(java_name, name_symbol -> Utf8Name());
    strcat(java_name, FileSymbol::java_suffix);

    for (int k = 0; k < package -> directory.Length(); k++)
    {
        DirectorySymbol *directory_symbol = package -> directory[k];
        file_symbol = directory_symbol -> FindFileSymbol(name_symbol);
        if (file_symbol)
             break;

        PathSymbol *path_symbol = directory_symbol -> PathSym();
        if (! path_symbol -> IsZip())
        {
            DirectoryEntry *java_entry = directory_symbol -> FindEntry(java_name, java_length),
                           *class_entry = (((! depend) || (java_entry == NULL))
                                                        ? directory_symbol -> FindEntry(class_name, class_length)
                                                        : (DirectoryEntry *) NULL);

            if (java_entry || class_entry)
            {
                file_symbol = directory_symbol -> InsertFileSymbol(name_symbol);
                file_symbol -> directory_symbol = directory_symbol;
                
                if (java_entry && ((! class_entry) || class_entry -> Mtime() < java_entry -> Mtime()))
                {
                    file_symbol -> SetJava();
                    file_symbol -> mtime = java_entry -> Mtime();
                }
                else
                {
                    if (java_entry)
                         file_symbol -> SetClass();
                    else file_symbol -> SetClassOnly();
                    file_symbol -> mtime = class_entry -> Mtime();
                }
                break;
            }
        }
    }

    delete [] java_name;
    delete [] class_name;

    return file_symbol;
}


TypeSymbol *Control::GetType(PackageSymbol *package, wchar_t *name)
{
    NameSymbol *name_symbol = FindOrInsertName(name, wcslen(name));
    TypeSymbol *type = package -> FindTypeSymbol(name_symbol);

    if (! type)
    {
        FileSymbol *file_symbol = GetFile(package, name_symbol, option.depend);
        type = system_semantic -> ReadType(file_symbol, package, name_symbol, 0);
    }
    else if (type -> SourcePending())
         ProcessHeaders(type -> file_symbol);

    return type;
}

