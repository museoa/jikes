// $Id$ -*- c++ -*-
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 2004 IBM Corporation and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#ifndef control_INCLUDED
#define control_INCLUDED

#include "platform.h"
#include "symbol.h"
#include "tuple.h"
#include "set.h"

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif

class StoragePool;
class Option;
class Scanner;
class Parser;
class Semantic;
class LexStream;
class AstPackageDeclaration;
class AstName;
class TypeDependenceChecker;

//
// This class represents the control information common across all compilation
// units.  It provides a cache for essential classes and objects, as well as
// the command line options in force.
//
class Control : public StringConstant
{
public:
    int return_code;
    Option& option;
    SymbolTable classpath_table;
    SymbolTable external_table;

    unsigned dot_classpath_index;
    Tuple<PathSymbol*> classpath;
    Tuple<wchar_t*> bad_dirnames;
    Tuple<wchar_t*> bad_zip_filenames;
    Tuple<wchar_t*> bad_input_filenames;
    Tuple<wchar_t*> unreadable_input_filenames;

    SystemTable* system_table;
    Tuple<DirectorySymbol*> system_directories;

    Semantic* system_semantic;
    Tuple<Semantic*> semantic;
    Tuple<TypeSymbol*> needs_body_work;
    Tuple<TypeSymbol*> type_trash_bin;

    NameSymbolMap unnamed_package_types;

    SymbolSet input_java_file_set;
    SymbolSet input_class_file_set;
    SymbolSet expired_file_set;
    SymbolSet recompilation_file_set;

    Parser* parser;
    Scanner* scanner;

    //
    // Tables for hashing everything we've seen so far.
    //
    LiteralLookupTable string_table;
    LiteralLookupTable int_table;
    LiteralLookupTable long_table;
    LiteralLookupTable char_table;
    LiteralLookupTable float_table;
    LiteralLookupTable double_table;
    NameLookupTable name_table;
    TypeLookupTable type_table;

    //
    // This cache of name symbols is initialized in system.cpp.
    //
    NameSymbol* access_name_symbol;
    NameSymbol* array_name_symbol;
    NameSymbol* assert_name_symbol;
    NameSymbol* block_init_name_symbol;
    NameSymbol* class_name_symbol;
    NameSymbol* clinit_name_symbol;
    NameSymbol* clone_name_symbol;
    NameSymbol* dot_name_symbol;
    NameSymbol* dot_dot_name_symbol;
    NameSymbol* equals_name_symbol;
    NameSymbol* false_name_symbol;
    NameSymbol* hashCode_name_symbol;
    NameSymbol* init_name_symbol;
    NameSymbol* length_name_symbol;
    NameSymbol* null_name_symbol;
    NameSymbol* object_name_symbol;
    NameSymbol* package_info_name_symbol;
    NameSymbol* question_name_symbol;
    NameSymbol* serialPersistentFields_name_symbol;
    NameSymbol* serialVersionUID_name_symbol;
    NameSymbol* this_name_symbol;
    NameSymbol* true_name_symbol;
    NameSymbol* type_name_symbol;
    NameSymbol* val_name_symbol;

    Utf8LiteralValue* ConstantValue_literal;
    Utf8LiteralValue* Exceptions_literal;
    Utf8LiteralValue* InnerClasses_literal;
    Utf8LiteralValue* Synthetic_literal;
    Utf8LiteralValue* Deprecated_literal;
    Utf8LiteralValue* LineNumberTable_literal;
    Utf8LiteralValue* LocalVariableTable_literal;
    Utf8LiteralValue* Code_literal;
    Utf8LiteralValue* SourceFile_literal;
    Utf8LiteralValue* EnclosingMethod_literal;

    //
    // The primitive types.
    //
    TypeSymbol* byte_type;
    TypeSymbol* short_type;
    TypeSymbol* int_type;
    TypeSymbol* long_type;
    TypeSymbol* char_type;
    TypeSymbol* float_type;
    TypeSymbol* double_type;
    TypeSymbol* boolean_type;
    TypeSymbol* void_type;
    TypeSymbol* null_type;
    TypeSymbol* no_type;

    //
    // System package accessors.
    //
    inline PackageSymbol* AnnotationPackage()
    {
        if (! annotation_package)
            annotation_package = ProcessPackage(US_java_SL_lang_SL_annotation);
        return annotation_package;
    }
    inline PackageSymbol* IoPackage()
    {
        if (! io_package)
            io_package = ProcessPackage(US_java_SL_io);
        return io_package;
    }
    inline PackageSymbol* LangPackage()
    {
        assert(lang_package);
        return lang_package;
    }
    inline PackageSymbol* UtilPackage()
    {
        if (! util_package)
            util_package = ProcessPackage(US_java_SL_util);
        return util_package;
    }
    inline PackageSymbol* UnnamedPackage()
    {
        assert(unnamed_package);
        return unnamed_package;
    }

    //
    // System type and method accessors. Useful boilerplate macros.
    //
#define TYPE_ACCESSOR(type, package)                           \
    inline TypeSymbol* type()                                  \
    {                                                          \
        if (! type ## _type)                                   \
            type ## _type = ProcessSystemType(package, #type); \
        return type ## _type;                                  \
    }
#define METHOD_ACCESSOR(method, type, name, signature)                      \
    inline MethodSymbol* method ## Method()                                 \
    {                                                                       \
        if (! method ## _method)                                            \
            method ## _method = ProcessSystemMethod(type, name, signature); \
        return method ## _method;                                           \
    }
    TYPE_ACCESSOR(Annotation, AnnotationPackage())
        TYPE_ACCESSOR(AssertionError, lang_package)
        METHOD_ACCESSOR(AssertionError_Init, AssertionError(),
                        "<init>", "()V")
        METHOD_ACCESSOR(AssertionError_InitWithChar, AssertionError(),
                        "<init>", "(C)V")
        METHOD_ACCESSOR(AssertionError_InitWithBoolean, AssertionError(),
                        "<init>", "(Z)V")
        METHOD_ACCESSOR(AssertionError_InitWithInt, AssertionError(),
                        "<init>", "(I)V")
        METHOD_ACCESSOR(AssertionError_InitWithLong, AssertionError(),
                        "<init>", "(J)V")
        METHOD_ACCESSOR(AssertionError_InitWithFloat, AssertionError(),
                        "<init>", "(F)V")
        METHOD_ACCESSOR(AssertionError_InitWithDouble, AssertionError(),
                        "<init>", "(D)V")
        METHOD_ACCESSOR(AssertionError_InitWithObject, AssertionError(),
                        "<init>", "(Ljava/lang/Object;)V")
        TYPE_ACCESSOR(Boolean, lang_package)
        TYPE_ACCESSOR(Byte, lang_package)
        TYPE_ACCESSOR(Character, lang_package)
        TYPE_ACCESSOR(Class, lang_package)
        METHOD_ACCESSOR(Class_forName, Class(),
                        "forName", "(Ljava/lang/String;)Ljava/lang/Class;")
        METHOD_ACCESSOR(Class_getComponentType, Class(),
                        "getComponentType", "()Ljava/lang/Class;")
        METHOD_ACCESSOR(Class_desiredAssertionStatus, Class(),
                        "desiredAssertionStatus", "()Z")
        TYPE_ACCESSOR(ClassNotFoundException, lang_package)
        TYPE_ACCESSOR(Cloneable, lang_package)
        TYPE_ACCESSOR(Comparable, lang_package)
        TYPE_ACCESSOR(Double, lang_package)
        TYPE_ACCESSOR(Enum, lang_package)
        METHOD_ACCESSOR(Enum_Init, Enum(),
                        "<init>", "(Ljava/lang/String;I)V")
        METHOD_ACCESSOR(Enum_ordinal, Enum(),
                        "ordinal", "()I")
        METHOD_ACCESSOR(Enum_valueOf, Enum(), "valueOf",
                        "(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/Enum;")
        TYPE_ACCESSOR(Error, lang_package)
        TYPE_ACCESSOR(Exception, lang_package)
        TYPE_ACCESSOR(Float, lang_package)
        TYPE_ACCESSOR(Integer, lang_package)
        TYPE_ACCESSOR(Iterable, lang_package)
        METHOD_ACCESSOR(Iterable_iterator, Iterable(),
                        "iterator", "()Ljava/util/Iterator;")
        TYPE_ACCESSOR(Iterator, UtilPackage())
        METHOD_ACCESSOR(Iterator_hasNext, Iterator(),
                        "hasNext", "()Z")
        METHOD_ACCESSOR(Iterator_next, Iterator(),
                        "next", "()Ljava/lang/Object;")
        TYPE_ACCESSOR(Long, lang_package)
        TYPE_ACCESSOR(NoClassDefFoundError, lang_package)
        METHOD_ACCESSOR(NoClassDefFoundError_Init,
                        NoClassDefFoundError(),
                        "<init>", "()V")
        METHOD_ACCESSOR(NoClassDefFoundError_InitString,
                        NoClassDefFoundError(),
                        "<init>", "(Ljava/lang/String;)V")
        TYPE_ACCESSOR(Object, lang_package)
        METHOD_ACCESSOR(Object_getClass, Object(),
                        "getClass", "()Ljava/lang/Class;")
        TYPE_ACCESSOR(Overrides, lang_package)
        TYPE_ACCESSOR(Retention, AnnotationPackage())
        TYPE_ACCESSOR(RuntimeException, lang_package)
        TYPE_ACCESSOR(Serializable, IoPackage())
        TYPE_ACCESSOR(Short, lang_package)
        TYPE_ACCESSOR(String, lang_package)
        TYPE_ACCESSOR(StringBuffer, lang_package)
        METHOD_ACCESSOR(StringBuffer_Init, StringBuffer(),
                        "<init>", "()V")
        METHOD_ACCESSOR(StringBuffer_InitWithString, StringBuffer(),
                        "<init>", "(Ljava/lang/String;)V")
        METHOD_ACCESSOR(StringBuffer_toString, StringBuffer(),
                        "toString", "()Ljava/lang/String;")
        METHOD_ACCESSOR(StringBuffer_append_char, StringBuffer(),
                        "append", "(C)Ljava/lang/StringBuffer;")
        METHOD_ACCESSOR(StringBuffer_append_boolean, StringBuffer(),
                        "append", "(Z)Ljava/lang/StringBuffer;")
        METHOD_ACCESSOR(StringBuffer_append_int, StringBuffer(),
                        "append", "(I)Ljava/lang/StringBuffer;")
        METHOD_ACCESSOR(StringBuffer_append_long, StringBuffer(),
                        "append", "(J)Ljava/lang/StringBuffer;")
        METHOD_ACCESSOR(StringBuffer_append_float, StringBuffer(),
                        "append", "(F)Ljava/lang/StringBuffer;")
        METHOD_ACCESSOR(StringBuffer_append_double, StringBuffer(),
                        "append", "(D)Ljava/lang/StringBuffer;")
        METHOD_ACCESSOR(StringBuffer_append_string, StringBuffer(), "append",
                        "(Ljava/lang/String;)Ljava/lang/StringBuffer;")
        METHOD_ACCESSOR(StringBuffer_append_object, StringBuffer(), "append",
                        "(Ljava/lang/Object;)Ljava/lang/StringBuffer;")
        TYPE_ACCESSOR(StringBuilder, lang_package)
        METHOD_ACCESSOR(StringBuilder_Init, StringBuilder(),
                        "<init>", "()V")
        METHOD_ACCESSOR(StringBuilder_InitWithString, StringBuilder(),
                        "<init>", "(Ljava/lang/String;)V")
        METHOD_ACCESSOR(StringBuilder_toString, StringBuilder(),
                        "toString", "()Ljava/lang/String;")
        METHOD_ACCESSOR(StringBuilder_append_char, StringBuilder(),
                        "append", "(C)Ljava/lang/StringBuilder;")
        METHOD_ACCESSOR(StringBuilder_append_boolean, StringBuilder(),
                        "append", "(Z)Ljava/lang/StringBuilder;")
        METHOD_ACCESSOR(StringBuilder_append_int, StringBuilder(),
                        "append", "(I)Ljava/lang/StringBuilder;")
        METHOD_ACCESSOR(StringBuilder_append_long, StringBuilder(),
                        "append", "(J)Ljava/lang/StringBuilder;")
        METHOD_ACCESSOR(StringBuilder_append_float, StringBuilder(),
                        "append", "(F)Ljava/lang/StringBuilder;")
        METHOD_ACCESSOR(StringBuilder_append_double, StringBuilder(),
                        "append", "(D)Ljava/lang/StringBuilder;")
        METHOD_ACCESSOR(StringBuilder_append_string, StringBuilder(), "append",
                        "(Ljava/lang/String;)Ljava/lang/StringBuilder;")
        METHOD_ACCESSOR(StringBuilder_append_object, StringBuilder(), "append",
                        "(Ljava/lang/Object;)Ljava/lang/StringBuilder;")
        TYPE_ACCESSOR(Target, AnnotationPackage())
        TYPE_ACCESSOR(Throwable, lang_package)
        METHOD_ACCESSOR(Throwable_getMessage, Throwable(),
                        "getMessage", "()Ljava/lang/String;")
        METHOD_ACCESSOR(Throwable_initCause, Throwable(), "initCause",
                        "(Ljava/lang/Throwable;)Ljava/lang/Throwable;")
        TYPE_ACCESSOR(Void, lang_package)
#undef TYPE_ACCESSOR
#undef METHOD_ACCESSOR

    IntLiteralTable int_pool;
    LongLiteralTable long_pool;
    FloatLiteralTable float_pool;
    DoubleLiteralTable double_pool;
    Utf8LiteralTable Utf8_pool;

    Control(char**, Option&);
    ~Control();

    Utf8LiteralValue* ConvertUnicodeToUtf8(const wchar_t* source)
    {
        // Should be big enough for the worst case.
        char* target = new char[wcslen(source) * 3 + 1];
        int length = ConvertUnicodeToUtf8(source, target);
        Utf8LiteralValue* literal = Utf8_pool.FindOrInsert(target, length);
        delete [] target;
        return literal;
    }

    static int ConvertUtf8ToUnicode(wchar_t*, const char*, int);

    NameSymbol* ConvertUtf8ToUnicode(const char* source, int length)
    {
        wchar_t* name = new wchar_t[length + 1];
        int name_length = ConvertUtf8ToUnicode(name, source, length);
        NameSymbol* name_symbol = FindOrInsertName(name, name_length);
        delete [] name;
        return name_symbol;
    }

    void FindPathsToDirectory(PackageSymbol*);

    DirectoryEntry* FindInputFile(FileSymbol*);
    void FindMoreRecentInputFiles(SymbolSet&);
    void RemoveTrashedTypes(SymbolSet&);
    void RereadDirectory(DirectorySymbol*);
    void RereadDirectories();
    void ComputeRecompilationSet(TypeDependenceChecker&);
    bool IncrementalRecompilation();

    //
    // The one and only bad value constant.
    //
    LiteralValue* BadValue() { return &bad_value; }

    //
    // Note that only names are converted here and not literals, since
    // no error can occur in a name.
    // A literal is converted during the semantic pass so that an
    // accurate diagnostic can be issued in case it is invalid.
    //
    NameSymbol* FindOrInsertName(const wchar_t* name, int len)
    {
        NameSymbol* name_symbol = name_table.FindOrInsertName(name, len);
        if (! name_symbol -> Utf8_literal)
            name_symbol -> Utf8_literal =
                ConvertUnicodeToUtf8(name_symbol -> Name());
        return name_symbol;
    }

    //
    // Make up a parameter name of the form $(num) and return its name symbol.
    //
    NameSymbol* MakeParameter(int num)
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
    static DirectorySymbol* GetOutputDirectory(FileSymbol*);
    static FileSymbol* GetJavaFile(PackageSymbol*, const NameSymbol*);
    static FileSymbol* GetFile(Control&, PackageSymbol*, const NameSymbol*);
    static FileSymbol* GetFileFirst(Control&, PackageSymbol*,
                                    const NameSymbol*);
    static FileSymbol* GetFileBoth(Control&, PackageSymbol*,
                                   const NameSymbol*);

    PackageSymbol* FindOrInsertPackage(LexStream*, AstName*);
    void ProcessPackageDeclaration(FileSymbol*, AstPackageDeclaration*);
    void CleanUp(FileSymbol*);

    inline bool IsSimpleIntegerValueType(const TypeSymbol* type)
    {
        return type == byte_type || type == short_type ||
            type == int_type || type == char_type;
    }

    inline bool IsIntegral(const TypeSymbol* type)
    {
        return IsSimpleIntegerValueType(type) || type == long_type;
    }

    inline bool IsFloatingPoint(const TypeSymbol* type)
    {
        return type == float_type || type == double_type;
    }

    inline bool IsNumeric(const TypeSymbol* type)
    {
        return IsIntegral(type) || IsFloatingPoint(type);
    }

    inline bool IsDoubleWordType(const TypeSymbol* type)
    {
        return type == long_type || type == double_type;
    }

    inline bool IsPrimitive(const TypeSymbol* type)
    {
        return IsNumeric(type) || type == boolean_type;
    }

    inline void ProcessBadType(TypeSymbol* type_symbol)
    {
        type_trash_bin.Next() = type_symbol;
    }

    void ProcessHeaders(FileSymbol*);

#ifdef JIKES_DEBUG
    int input_files_processed,
        class_files_read,
        class_files_written,
        line_count;
#endif // JIKES_DEBUG

    PackageSymbol* ProcessPackage(const wchar_t*);

    DirectorySymbol* FindSubdirectory(PathSymbol*, wchar_t*, int);
    DirectorySymbol* ProcessSubdirectories(wchar_t*, int, bool);

private:
    LiteralValue bad_value;

    //
    // Cache of system packages. lang and unnamed are always valid, because of
    // ProcessUnnamedPackage and ProcessSystemInformation in system.cpp, the
    // constructor initializes the rest to NULL in control.cpp; see accessor
    // methods above for assignment.
    //
    PackageSymbol* annotation_package;
    PackageSymbol* io_package;
    PackageSymbol* lang_package;
    PackageSymbol* util_package;
    PackageSymbol* unnamed_package;

    //
    // Cache of system types and methods. Constructor initializes these to
    // NULL in control.cpp; see accessor methods above for assignment. The
    // types are in java.lang unless commented otherwise.
    //
    TypeSymbol* Annotation_type; // java.lang.annotation
    TypeSymbol* AssertionError_type;
    MethodSymbol* AssertionError_Init_method;
    MethodSymbol* AssertionError_InitWithChar_method;
    MethodSymbol* AssertionError_InitWithBoolean_method;
    MethodSymbol* AssertionError_InitWithInt_method;
    MethodSymbol* AssertionError_InitWithLong_method;
    MethodSymbol* AssertionError_InitWithFloat_method;
    MethodSymbol* AssertionError_InitWithDouble_method;
    MethodSymbol* AssertionError_InitWithObject_method;
    TypeSymbol* Boolean_type;
    TypeSymbol* Byte_type;
    TypeSymbol* Character_type;
    TypeSymbol* Class_type;
    MethodSymbol* Class_forName_method;
    MethodSymbol* Class_getComponentType_method;
    MethodSymbol* Class_desiredAssertionStatus_method;
    TypeSymbol* ClassNotFoundException_type;
    TypeSymbol* Cloneable_type;
    TypeSymbol* Comparable_type;
    TypeSymbol* Double_type;
    TypeSymbol* Enum_type;
    MethodSymbol* Enum_Init_method;
    MethodSymbol* Enum_ordinal_method;
    MethodSymbol* Enum_valueOf_method;
    TypeSymbol* Error_type;
    TypeSymbol* Exception_type;
    TypeSymbol* Float_type;
    TypeSymbol* Integer_type;
    TypeSymbol* Iterable_type;
    MethodSymbol* Iterable_iterator_method;
    TypeSymbol* Iterator_type; // java.util
    MethodSymbol* Iterator_hasNext_method;
    MethodSymbol* Iterator_next_method;
    TypeSymbol* Long_type;
    TypeSymbol* NoClassDefFoundError_type;
    MethodSymbol* NoClassDefFoundError_Init_method;
    MethodSymbol* NoClassDefFoundError_InitString_method;
    TypeSymbol* Object_type;
    MethodSymbol* Object_getClass_method;
    TypeSymbol* Overrides_type; // spelled Override? JSR 175 not sure...
    TypeSymbol* Retention_type; // java.lang.annotation
    TypeSymbol* RuntimeException_type;
    TypeSymbol* Serializable_type; // java.io
    TypeSymbol* Short_type;
    TypeSymbol* String_type;
    TypeSymbol* StringBuffer_type;
    MethodSymbol* StringBuffer_Init_method;
    MethodSymbol* StringBuffer_InitWithString_method;
    MethodSymbol* StringBuffer_toString_method;
    MethodSymbol* StringBuffer_append_char_method;
    MethodSymbol* StringBuffer_append_boolean_method;
    MethodSymbol* StringBuffer_append_int_method;
    MethodSymbol* StringBuffer_append_long_method;
    MethodSymbol* StringBuffer_append_float_method;
    MethodSymbol* StringBuffer_append_double_method;
    MethodSymbol* StringBuffer_append_string_method;
    MethodSymbol* StringBuffer_append_object_method;
    TypeSymbol* StringBuilder_type;
    MethodSymbol* StringBuilder_Init_method;
    MethodSymbol* StringBuilder_InitWithString_method;
    MethodSymbol* StringBuilder_toString_method;
    MethodSymbol* StringBuilder_append_char_method;
    MethodSymbol* StringBuilder_append_boolean_method;
    MethodSymbol* StringBuilder_append_int_method;
    MethodSymbol* StringBuilder_append_long_method;
    MethodSymbol* StringBuilder_append_float_method;
    MethodSymbol* StringBuilder_append_double_method;
    MethodSymbol* StringBuilder_append_string_method;
    MethodSymbol* StringBuilder_append_object_method;
    TypeSymbol* Target_type; // java.lang.annotation
    TypeSymbol* Throwable_type;
    MethodSymbol* Throwable_getMessage_method;
    MethodSymbol* Throwable_initCause_method;
    TypeSymbol* Void_type;

    static int ConvertUnicodeToUtf8(const wchar_t*, char*);
    NameSymbol* FindOrInsertSystemName(const char* name);

    void ProcessGlobals();
    void ProcessUnnamedPackage();
    void ProcessPath();
    void ProcessBootClassPath();
    void ProcessExtDirs();
    void ProcessClassPath();
    void ProcessSourcePath();
    TypeSymbol* GetPrimitiveType(const char*, char);
    void ProcessSystemInformation();
    TypeSymbol* ProcessSystemType(PackageSymbol*, const char*);
    MethodSymbol* ProcessSystemMethod(TypeSymbol*, const char*, const char*);

    void ProcessFile(FileSymbol*);
    void ProcessMembers();
    void CollectTypes(TypeSymbol*, Tuple<TypeSymbol*>&);
    void ProcessBodies(TypeSymbol*);
    void CheckForUnusedImports(Semantic *);

    void ProcessNewInputFiles(SymbolSet&, char**);

    FileSymbol* FindOrInsertJavaInputFile(DirectorySymbol*, NameSymbol*);
    FileSymbol* FindOrInsertJavaInputFile(wchar_t*, int);
};

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

#endif // control_INCLUDED

