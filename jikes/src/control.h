// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#ifndef control_INCLUDED
#define control_INCLUDED

#include "config.h"
#include <iostream.h>
#include <stdio.h>
#include "option.h"
#include "symbol.h"
#include "tuple.h"
#include "set.h"

class StoragePool;
class Scanner;
class Parser;
class Semantic;
class LexStream;
class AstPackageDeclaration;

class Control : public StringConstant
{
public:
    int return_code;
    Option &option;
    SymbolTable classpath_table,
                external_table;
    PackageSymbol *system_package,
                  *unnamed_package;
    int dot_classpath_index;
    Tuple<PathSymbol *> classpath;
    Tuple<wchar_t *> bad_zip_filenames,
                     bad_input_filenames,
                     unreadable_input_filenames;

    DirectorySymbol *default_directory;
                     
    Semantic *system_semantic;
    Tuple<Semantic *> semantic;
    Tuple<TypeSymbol *> needs_body_work,
                        type_trash_bin;

    NameSymbolMap unnamed_package_types;

    SymbolSet input_java_file_set,
              input_class_file_set,
              expired_file_set,
              recompilation_file_set;

    Parser *parser;
    Scanner *scanner;

    //
    //
    //
    LiteralLookupTable string_table,
                       int_table,
                       long_table,
                       char_table,
                       float_table,
                       double_table;
    NameLookupTable name_table;
 
    //
    //
    //
    NameSymbol *dot_name_symbol,
               *dot_dot_name_symbol,
               *length_name_symbol,
               *init_name_symbol,
               *clinit_name_symbol,
               *block_init_name_symbol,
               *this0_name_symbol,
               *clone_name_symbol,
               *object_name_symbol,
               *type_name_symbol,
               *class_name_symbol;

    //
    //
    //
    TypeSymbol *byte_type,
               *short_type,
               *int_type,
               *long_type,
               *char_type,
               *float_type,
               *double_type,
               *boolean_type,
               *void_type,
               *null_type,
               *no_type,

               *Serializable_type,

               *Object_type,
               *Cloneable_type,
               *String_type,
               *Void_type,
               *Boolean_type,
               *Byte_type,
               *Short_type,
               *Character_type,
               *Integer_type,
               *Long_type,
               *Float_type,
               *Double_type,
               *Class_type,
               *StringBuffer_type,
               *Throwable_type,
               *RuntimeException_type,
               *Error_type;
    //
    TypeSymbol *GetType(PackageSymbol *, wchar_t *);

    inline TypeSymbol *Serializable()
    {
        if ((! Serializable_type) && option.one_one)
        {
            PackageSymbol *io_package = ProcessPackage(StringConstant::US_java_SL_io);
            FindPathsToDirectory(io_package);
            Serializable_type = GetType(io_package, StringConstant::US_Serializable);
        }

        return Serializable_type;
    }

    inline TypeSymbol *Object()
    {
        return (Object_type ? Object_type : Object_type = GetType(system_package, StringConstant::US_Object));
    }

    inline TypeSymbol *Cloneable()
    {
        return (Cloneable_type ? Cloneable_type : Cloneable_type = GetType(system_package, StringConstant::US_Cloneable));
    }

    inline TypeSymbol *String()
    {
        return (String_type ? String_type : String_type = GetType(system_package, StringConstant::US_String));
    }

    inline TypeSymbol *Void()
    {
        return (Void_type ? Void_type : Void_type = GetType(system_package, StringConstant::US_Void));
    }

    inline TypeSymbol *Boolean()
    {
        return (Boolean_type ? Boolean_type : Boolean_type = GetType(system_package, StringConstant::US_Boolean));
    }

    inline TypeSymbol *Byte()
    {
        return (Byte_type ? Byte_type : Byte_type = GetType(system_package, StringConstant::US_Byte));
    }

    inline TypeSymbol *Short()
    {
        return (Short_type ? Short_type : Short_type = GetType(system_package, StringConstant::US_Short));
    }

    inline TypeSymbol *Character()
    {
        return (Character_type ? Character_type : Character_type = GetType(system_package, StringConstant::US_Character));
    }

    inline TypeSymbol *Integer()
    {
        return (Integer_type ? Integer_type : Integer_type = GetType(system_package, StringConstant::US_Integer));
    }

    inline TypeSymbol *Long()
    {
        return (Long_type ? Long_type : Long_type = GetType(system_package, StringConstant::US_Long));
    }

    inline TypeSymbol *Float()
    {
        return (Float_type ? Float_type : Float_type = GetType(system_package, StringConstant::US_Float));
    }

    inline TypeSymbol *Double()
    {
        return (Double_type ? Double_type : Double_type = GetType(system_package, StringConstant::US_Double));
    }

    inline TypeSymbol *Class()
    {
        return (Class_type ? Class_type : Class_type = GetType(system_package, StringConstant::US_Class));
    }

    inline TypeSymbol *StringBuffer()
    {
        return (StringBuffer_type ? StringBuffer_type
                                  : StringBuffer_type = GetType(system_package, StringConstant::US_StringBuffer));
    }

    inline TypeSymbol *Throwable()
    {
        return (Throwable_type ? Throwable_type : Throwable_type = GetType(system_package, StringConstant::US_Throwable));
    }

    inline TypeSymbol *RuntimeException()
    {
        return (RuntimeException_type
                       ? RuntimeException_type
                       : RuntimeException_type = GetType(system_package, StringConstant::US_RuntimeException));
    }

    inline TypeSymbol *Error()
    {
        return (Error_type ? Error_type : Error_type = GetType(system_package, StringConstant::US_Error));
    }

    //
    //
    //
    LiteralValue bad_value;

    IntLiteralTable    int_pool;
    LongLiteralTable   long_pool;
    FloatLiteralTable  float_pool;
    DoubleLiteralTable double_pool;
    Utf8LiteralTable   Utf8_pool;

    Utf8LiteralValue *LP_C_RP_Ljava_SL_lang_SL_StringBuffer_SC_literal,
                     *LP_D_RP_Ljava_SL_lang_SL_StringBuffer_SC_literal,
                     *LP_F_RP_Ljava_SL_lang_SL_StringBuffer_SC_literal,
                     *LP_I_RP_Ljava_SL_lang_SL_StringBuffer_SC_literal,
                     *LP_J_RP_Ljava_SL_lang_SL_StringBuffer_SC_literal,
                     *LP_LB_C_RP_Ljava_SL_lang_SL_StringBuffer_SC_literal,
                     *LP_Ljava_SL_lang_SL_Object_SC_RP_Ljava_SL_lang_SL_StringBuffer_SC_literal,
                     *LP_Ljava_SL_lang_SL_String_SC_RP_Ljava_SL_lang_SL_StringBuffer_SC_literal,
                     *LP_Ljava_SL_lang_SL_String_SC_RP_V_literal,
                     *LP_RP_Ljava_SL_lang_SL_Object_SC_literal,
                     *LP_RP_Ljava_SL_lang_SL_String_SC_literal,
                     *LP_RP_V_literal,
                     *LP_Z_RP_Ljava_SL_lang_SL_StringBuffer_SC_literal,
                     *LT_init_GT_literal,
                     *append_literal,
                     *clone_literal,
                     *getMessage_literal,
                     *java_SL_lang_SL_InternalError_literal,
                     *java_SL_lang_SL_Object_literal,
                     *java_SL_lang_SL_StringBuffer_literal,
                     *java_SL_lang_SL_Throwable_literal,
                     *toString_literal,

                     *java_SL_lang_SL_ClassNotFoundException_literal,
                     *java_SL_lang_SL_Class_literal,
                     *java_SL_lang_SL_CloneNotSupportedException_literal,
                     *java_SL_lang_SL_NoClassDefFoundError_literal,

                     *ConstantValue_literal,
                     *Exceptions_literal,
                     *InnerClasses_literal,
                     *Synthetic_literal,
                     *Deprecated_literal,
                     *LineNumberTable_literal,
                     *LocalVariableTable_literal,
                     *Code_literal,
                     *Sourcefile_literal,

                     *LP_Ljava_SL_lang_SL_String_SC_RP_Ljava_SL_lang_SL_Class_SC_literal,
                     *LT_clinit_GT_literal,
                     *forName_literal,
                     *null_literal,
                     *this_literal;


    Control(ArgumentExpander &, Option &);
    ~Control();

    Utf8LiteralValue *ConvertUnicodeToUtf8(wchar_t *source)
    {
        char *target = new char[wcslen(source) * 3 + 1]; // should be big enough for the worst case
        int length = ConvertUnicodeToUtf8(source, target);
        Utf8LiteralValue *literal = Utf8_pool.FindOrInsert(target, length);
        delete [] target;

        return literal;
    }

    void FindPathsToDirectory(PackageSymbol *);

    DirectoryEntry *FindInputFile(FileSymbol *);
    void FindMoreRecentInputFiles(SymbolSet &);
    void RemoveTrashedTypes(SymbolSet &);
    void RereadDirectory(DirectorySymbol *);
    void RereadDirectories();
    void ComputeRecompilationSet(TypeDependenceChecker &);
    bool IncrementalRecompilation();

    //
    // The one and only null value constant.
    //
    LiteralValue *NullValue() { return &null_value; }

    //
    // Note that only names are converted here and not literals, since
    // no error can occur in a name.
    // A literal is converted during the semantic pass so that an
    // accurate diagnostic can be issued in case it is invalid.
    //
    NameSymbol *FindOrInsertName(wchar_t *name, int len)
    {
        NameSymbol *name_symbol = name_table.FindOrInsertName(name, len);
        if (! name_symbol -> Utf8_literal)
            name_symbol -> Utf8_literal = ConvertUnicodeToUtf8(name_symbol -> Name());

        return name_symbol;
    }

    //
    // Make up a parameter name of the form #(num) and return its name symbol.
    //
    NameSymbol *MakeParameter(int num)
    {
        IntToWstring value(num);

        wchar_t str[13];
        str[0] = U_DOLLAR; // '$'
        wcscpy(&str[1], value.String());

        return FindOrInsertName(str, value.Length() + 1);
    }

    //
    //
    //
    static DirectorySymbol *GetOutputDirectory(FileSymbol *);
    static FileSymbol *GetJavaFile(PackageSymbol *, NameSymbol *);
    static FileSymbol *GetFile(Control &, PackageSymbol *, NameSymbol *);

    PackageSymbol *FindOrInsertPackage(LexStream *, AstExpression *);
    void ProcessPackageDeclaration(FileSymbol *, AstPackageDeclaration *);
    void CleanUp(FileSymbol *);

    inline bool IsSimpleIntegerValueType(TypeSymbol *type)
    {
        return (type == byte_type || type == short_type || type == int_type || type == char_type);
    }

    inline bool IsIntegral(TypeSymbol *type)
    {
        return (IsSimpleIntegerValueType(type) || type == long_type);
    }

    inline bool IsFloatingPoint(TypeSymbol *type)
    {
        return (type == float_type || type == double_type);
    }

    inline bool IsNumeric(TypeSymbol *type)
    {
        return IsIntegral(type) || IsFloatingPoint(type);
    }

    inline bool IsDoubleWordType(TypeSymbol *type)
    {
        return (type == long_type || type == double_type);
    }

    inline void ProcessBadType(TypeSymbol *type_symbol)
    {
        type_trash_bin.Next() = type_symbol;
    }

    void ProcessHeaders(FileSymbol *);

#ifdef TEST
    int input_files_processed,
        class_files_read,
        class_files_written,
        line_count;
#endif

    int class_file_id;

    PackageSymbol *ProcessPackage(wchar_t *);

    DirectorySymbol *FindSubdirectory(PathSymbol *, wchar_t *, int);
    DirectorySymbol *ProcessSubdirectories(wchar_t *, int);

private:

    LiteralValue null_value;

    static int ConvertUnicodeToUtf8(wchar_t *, char *);

    void ProcessGlobals();
    void ProcessUnnamedPackage();
    void ProcessPath();
    TypeSymbol *GetPrimitiveType(wchar_t *, char *);
    void ProcessSystemInformation();

    void ProcessFile(FileSymbol *);
    void ProcessMembers();
    void ProcessBodies(TypeSymbol *);

    void ProcessNewInputFiles(SymbolSet &, ArgumentExpander &, int = 0);

    FileSymbol *FindOrInsertJavaInputFile(DirectorySymbol *, NameSymbol *);
    FileSymbol *FindOrInsertJavaInputFile(wchar_t *, int);
};

#endif /* control_INCLUDED */
