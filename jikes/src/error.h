// $Id$ -*- c++ -*-
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, 1999, 2000, 2001 International Business
// Machines Corporation and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#ifndef error_INCLUDED
#define error_INCLUDED

#include "platform.h"
#include "stream.h"
#include "tuple.h"
#include "jikesapi.h"

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif

class Control;
class LexStream;
class Ast_CompilationUnit;
class SymbolSet;
class Semantic;
class SemanticError;

class ErrorInfo : public JikesError
{
    friend class SemanticError;
    
 public:
    
    virtual const wchar_t *getErrorMessage();
    virtual const wchar_t *getErrorReport();

    virtual JikesErrorSeverity getSeverity();
    virtual const char *getFileName();

    virtual int getLeftLineNo();
    virtual int getLeftColumnNo();
    virtual int getRightLineNo();
    virtual int getRightColumnNo();

    ErrorInfo();
    virtual ~ErrorInfo();

 protected:

 private:
    int left_line_no;
    int left_column_no;
    int right_line_no;
    int right_column_no;
    
    LexStream::TokenIndex left_token;
    LexStream::TokenIndex right_token;
    
    wchar_t *insert1,
        *insert2,
        *insert3,
        *insert4,
        *insert5,
        *insert6,
        *insert7,
        *insert8,
        *insert9;

    wchar_t *msg;
    unsigned num;
    short msg_code;
    short right_string_length;
    JikesErrorSeverity severity;

    static bool emacs_style_report;
    LexStream *lex_stream;

    wchar_t *regularErrorString();
    wchar_t *emacsErrorString();

    void PrintLargeSource(ErrorString &s);
    void PrintSmallSource(ErrorString &s);

    void Initialize(LexStream *, wchar_t *, JikesErrorSeverity);
};

class SemanticError
{
    friend class ErrorInfo;
    friend class JikesAPI;

 public:
    enum SemanticErrorKind
    {
        BAD_ERROR,
        DEFAULT_ERROR,

        NO_CURRENT_DIRECTORY,
        CANNOT_OPEN_ZIP_FILE,
        CANNOT_OPEN_PATH_DIRECTORY,
        PACKAGE_NOT_FOUND,
        CANNOT_OPEN_DIRECTORY,
        BAD_INPUT_FILE,
        UNREADABLE_INPUT_FILE,
        NON_STANDARD_LIBRARY_TYPE,
        LIBRARY_METHOD_NOT_FOUND,
        CANNOT_REOPEN_FILE,
        CANNOT_WRITE_FILE,
        CONSTANT_POOL_OVERFLOW,
        INTERFACES_OVERFLOW,
        METHODS_OVERFLOW,
        STRING_OVERFLOW,
        PARAMETER_OVERFLOW,
        ARRAY_OVERFLOW,
        FIELDS_OVERFLOW,
        LOCAL_VARIABLES_OVERFLOW,
        STACK_OVERFLOW,
        CODE_OVERFLOW,
        CANNOT_COMPUTE_COLUMNS,
        EMPTY_DECLARATION,
        REDUNDANT_MODIFIER,
        RECOMMENDED_MODIFIER_ORDER,
        OBSOLESCENT_BRACKETS,
        NO_TYPES,
        MULTIPLE_PUBLIC_TYPES,
        TYPE_IN_MULTIPLE_FILES,
        PACKAGE_TYPE_CONFLICT,
        DIRECTORY_FILE_CONFLICT,
        FILE_FILE_CONFLICT,
        MISMATCHED_TYPE_AND_FILE_NAMES,
        REFERENCE_TO_TYPE_IN_MISMATCHED_FILE,
        DUPLICATE_INNER_TYPE_NAME,
        DUPLICATE_TYPE_DECLARATION,
        UNNECESSARY_TYPE_IMPORT,
        DUPLICATE_ACCESS_MODIFIER,
        DUPLICATE_MODIFIER,
        FINAL_ABSTRACT_CLASS,
        VOLATILE_FINAL_FIELD,
        INVALID_TOP_LEVEL_CLASS_MODIFIER,
        INVALID_INNER_CLASS_MODIFIER,
        INVALID_STATIC_INNER_CLASS_MODIFIER,
        INVALID_LOCAL_CLASS_MODIFIER,
        INVALID_INTERFACE_MODIFIER,
        INVALID_FIELD_MODIFIER,
        INVALID_LOCAL_MODIFIER,
        INVALID_METHOD_MODIFIER,
        INVALID_SIGNATURE_MODIFIER,
        INVALID_CONSTRUCTOR_MODIFIER,
        INVALID_CONSTANT_MODIFIER,
        UNINITIALIZED_FIELD,
        RECOMPILATION,
        TYPE_NOT_FOUND,
        IMPORT_FROM_UNNAMED_PACKAGE,
        DUPLICATE_ON_DEMAND_IMPORT,
        UNKNOWN_ON_DEMAND_IMPORT,
        NOT_A_TYPE,
        NOT_A_CLASS,
        NOT_AN_INTERFACE,
        SUPER_IS_FINAL,
        OBJECT_WITH_SUPER_TYPE,
        OBJECT_HAS_NO_SUPER_TYPE,
        DUPLICATE_FIELD,
        DUPLICATE_METHOD,
        DUPLICATE_CONSTRUCTOR,
        MISMATCHED_INHERITED_METHOD,
        MISMATCHED_IMPLICIT_METHOD,
        MISMATCHED_INHERITED_METHOD_EXTERNALLY,
        DUPLICATE_FORMAL_PARAMETER,
        MISSPELLED_CONSTRUCTOR_NAME,
        MISMATCHED_CONSTRUCTOR_NAME,
        METHOD_WITH_CONSTRUCTOR_NAME,
        DUPLICATE_LOCAL_VARIABLE_DECLARATION,
        DUPLICATE_LOCAL_TYPE_DECLARATION,
        MULTIPLE_DEFAULT_LABEL,
        UNDECLARED_LABEL,
        DUPLICATE_LABEL,
        CATCH_PRIMITIVE_TYPE,
        CATCH_ARRAY_TYPE,
        AMBIGUOUS_FIELD,
        AMBIGUOUS_TYPE,
        FIELD_IS_TYPE,
        FIELD_NOT_FOUND,
        FIELD_NAME_MISSPELLED,
        METHOD_NOT_FIELD,
        NAME_NOT_YET_AVAILABLE,
        NAME_NOT_CLASS_VARIABLE,
        NOT_A_NUMERIC_VARIABLE,
        METHOD_OVERLOAD_NOT_FOUND,
        METHOD_NOT_FOUND,
        METHOD_NAME_MISSPELLED,
        HIDDEN_METHOD_IN_ENCLOSING_CLASS,
        FIELD_NOT_METHOD,
        TYPE_NOT_METHOD,
        TYPE_NOT_FIELD,
        METHOD_NOT_CLASS_METHOD,
        AMBIGUOUS_CONSTRUCTOR_INVOCATION,
        AMBIGUOUS_METHOD_INVOCATION,
        CONSTRUCTOR_NOT_FOUND,
        METHOD_FOUND_FOR_CONSTRUCTOR,
        ABSTRACT_TYPE_CREATION,
        INVALID_INSTANCEOF_CONVERSION,
        INVALID_CAST_CONVERSION,
        INVALID_CAST_TYPE,
        INCOMPATIBLE_TYPE_FOR_INITIALIZATION,
        INCOMPATIBLE_TYPE_FOR_ASSIGNMENT,
        INCOMPATIBLE_TYPE_FOR_BINARY_EXPRESSION,
        INCOMPATIBLE_TYPE_FOR_CONDITIONAL_EXPRESSION,
        VOID_ARRAY,
        VOID_TYPE_IN_EQUALITY_EXPRESSION,
        TYPE_NOT_THROWABLE,
        TYPE_NOT_PRIMITIVE,
        TYPE_NOT_INTEGRAL,
        TYPE_NOT_NUMERIC,
        TYPE_NOT_INTEGER,
        TYPE_NOT_BOOLEAN,
        TYPE_NOT_ARRAY,
        TYPE_NOT_REFERENCE,
        TYPE_NOT_VALID_FOR_SWITCH,
        TYPE_IS_VOID,
        VALUE_NOT_REPRESENTABLE_IN_SWITCH_TYPE,
        TYPE_NOT_CONVERTIBLE_TO_SWITCH_TYPE,
        DUPLICATE_CASE_VALUE,
        MISPLACED_THIS_EXPRESSION,
        MISPLACED_SUPER_EXPRESSION,
        VARIABLE_NOT_DEFINITELY_UNASSIGNED,
        VARIABLE_NOT_DEFINITELY_UNASSIGNED_IN_LOOP,
        FINAL_VARIABLE_NOT_BLANK,
        UNINITIALIZED_FINAL_VARIABLE,
        UNINITIALIZED_STATIC_FINAL_VARIABLE,
        UNINITIALIZED_FINAL_VARIABLE_IN_CONSTRUCTOR,
        UNINITIALIZED_FINAL_VARIABLE_IN_INTERFACE,
        INIT_SCALAR_WITH_ARRAY,
        INIT_ARRAY_WITH_SCALAR,
        INVALID_BYTE_VALUE,
        INVALID_SHORT_VALUE,
        INVALID_CHARACTER_VALUE,
        INVALID_INT_VALUE,
        INVALID_LONG_VALUE,
        INVALID_FLOAT_VALUE,
        INVALID_DOUBLE_VALUE,
        INVALID_STRING_VALUE,
        RETURN_STATEMENT_IN_INITIALIZER,
        ABRUPT_INITIALIZER,
        MISPLACED_RETURN_WITH_EXPRESSION,
        MISPLACED_RETURN_WITH_NO_EXPRESSION,
        MISMATCHED_RETURN_AND_METHOD_TYPE,
        EXPRESSION_NOT_THROWABLE,
        BAD_THROWABLE_EXPRESSION_IN_TRY,
        BAD_THROWABLE_EXPRESSION_IN_METHOD,
        BAD_THROWABLE_EXPRESSION,
        MISPLACED_BREAK_STATEMENT,
        MISPLACED_CONTINUE_STATEMENT,
        MISPLACED_EXPLICIT_CONSTRUCTOR_INVOCATION,
        INVALID_CONTINUE_TARGET,
        NON_ABSTRACT_TYPE_CONTAINS_ABSTRACT_METHOD,
        NON_ABSTRACT_TYPE_INHERITS_ABSTRACT_METHOD,
        NON_ABSTRACT_TYPE_CANNOT_OVERRIDE_DEFAULT_ABSTRACT_METHOD,
        DUPLICATE_INTERFACE,
        UNKNOWN_AMBIGUOUS_NAME,
        CIRCULAR_INTERFACE,
        CIRCULAR_CLASS,
        TYPE_NOT_ACCESSIBLE,
        FIELD_NOT_ACCESSIBLE,
        PROTECTED_INSTANCE_FIELD_NOT_ACCESSIBLE,
        METHOD_NOT_ACCESSIBLE,
        PROTECTED_INSTANCE_METHOD_NOT_ACCESSIBLE,
        PROTECTED_INTERFACE_METHOD_NOT_ACCESSIBLE,
        CONSTRUCTOR_NOT_ACCESSIBLE,
        PARAMETER_REDECLARED,
        BAD_ABSTRACT_METHOD_MODIFIER,
        ABSTRACT_METHOD_MODIFIER_CONFLICT,
        STRICTFP_NATIVE_METHOD,
        ABSTRACT_METHOD_INVOCATION,
        FINAL_METHOD_OVERRIDE,
        FINAL_IMPLICIT_METHOD_OVERRIDE,
        INSTANCE_METHOD_OVERRIDE,
        INSTANCE_METHOD_OVERRIDE_EXTERNALLY,
        CLASS_METHOD_OVERRIDE,
        MISMATCHED_OVERRIDDEN_EXCEPTION,
        MISMATCHED_IMPLICIT_OVERRIDDEN_EXCEPTION,
        MISMATCHED_OVERRIDDEN_EXCEPTION_EXTERNALLY,
        ABSTRACT_METHOD_WITH_BODY,
        NON_ABSTRACT_METHOD_WITHOUT_BODY,
        BAD_ACCESS_METHOD_OVERRIDE,
        BAD_ACCESS_METHOD_OVERRIDE_EXTERNALLY,
        CIRCULAR_THIS_CALL,
        INSTANCE_VARIABLE_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
        INSTANCE_METHOD_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
        SYNTHETIC_VARIABLE_ACCESS,
        SYNTHETIC_METHOD_INVOCATION,
        SYNTHETIC_CONSTRUCTOR_INVOCATION,
        THIS_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
        SUPER_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
        INNER_CONSTRUCTOR_IN_EXPLICIT_CONSTRUCTOR_INVOCATION,
        EXPRESSION_NOT_CONSTANT,
        UNCAUGHT_METHOD_EXCEPTION,
        UNCAUGHT_CONSTRUCTOR_EXCEPTION,
        UNCAUGHT_ANONYMOUS_CONSTRUCTOR_EXCEPTION,
        UNCAUGHT_THROWN_EXCEPTION,
        UNCAUGHT_EXPLICIT_THIS_EXCEPTION,
        UNCAUGHT_EXPLICIT_SUPER_EXCEPTION,
        UNREACHABLE_CATCH_CLAUSE,
        UNREACHABLE_DEFAULT_CATCH_CLAUSE,
        UNREACHABLE_STATEMENT,
        UNREACHABLE_STATEMENTS,
        BLOCKED_CATCH_CLAUSE,
        VARIABLE_NOT_DEFINITELY_ASSIGNED,
        TYPED_METHOD_WITH_NO_RETURN,

        DEFAULT_METHOD_NOT_OVERRIDDEN,

        ONE_UNNAMED_PACKAGE,
        TYPE_NOT_IN_UNNAMED_PACKAGE,
        TYPE_IN_WRONG_PACKAGE,
        TYPE_NAME_MISMATCH,

        DEPRECATED_TYPE,
        DEPRECATED_FIELD,
        DEPRECATED_METHOD,
        DEPRECATED_CONSTRUCTOR,

        COMPRESSED_ZIP_FILE,
        INVALID_CLASS_FILE,
        CANNOT_OPEN_CLASS_FILE,

        INTERFACE_NOT_INNER_CLASS,
        STATIC_NOT_INNER_CLASS,
        TYPE_NOT_INNER_CLASS,
        SUPER_TYPE_NOT_INNER_CLASS,
        STATIC_FIELD_IN_INNER_CLASS_NOT_FINAL,
        STATIC_FIELD_IN_INNER_CLASS_NOT_CONSTANT,
        STATIC_METHOD_IN_INNER_CLASS,
        STATIC_TYPE_IN_INNER_CLASS,
        STATIC_INITIALIZER_IN_INNER_CLASS,
        INNER_CLASS_REFERENCE_TO_NON_FINAL_LOCAL_VARIABLE,
        INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_LOCAL,
        INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER,
        ILLEGAL_THIS_FIELD_ACCESS,
        CONSTRUCTOR_FOUND_IN_ANONYMOUS_CLASS,
        ENCLOSING_INSTANCE_ACCESS_FROM_CONSTRUCTOR_INVOCATION,
        ENCLOSING_INSTANCE_ACCESS_ACROSS_STATIC_REGION,
        ENCLOSING_INSTANCE_NOT_ACCESSIBLE,
        INVALID_ENCLOSING_INSTANCE,

        ZERO_DIVIDE_ERROR,
        ZERO_DIVIDE_CAUTION,
        VOID_TO_STRING,

        _num_kinds
    };

    static void StaticInitializer();

    void Report(SemanticErrorKind,
                LexStream::TokenIndex,
                LexStream::TokenIndex,
                wchar_t * = NULL,
                wchar_t * = NULL,
                wchar_t * = NULL,
                wchar_t * = NULL,
                wchar_t * = NULL,
                wchar_t * = NULL,
                wchar_t * = NULL,
                wchar_t * = NULL,
                wchar_t * = NULL);

    SemanticError(Control &, FileSymbol *);
    ~SemanticError()
    {
        for (int i = 0; i < buffer.Length(); i++)
            delete [] buffer[i];
    }

    int num_errors,
        num_warnings;

    void EnteringClone() { clone_count++; }
    void ExitingClone()  { clone_count--; }

    int PrintMessages();

private:
    friend class Semantic;

    void reportError(int k);

    Control &control;
    LexStream *lex_stream;

    int clone_count;

    Tuple<wchar_t *> buffer;
    Tuple<ErrorInfo> error;

    static unsigned char warning[];
    static wchar_t *(*print_message[_num_kinds])(ErrorInfo &, LexStream *, Control &);

    static bool NotDot(wchar_t *str)
    {
        return str ? (! (wcslen(str) == 0 ||
                         wcscmp(str, StringConstant::US_DO) == 0))
                   : false;
    }

    static wchar_t *PrintBAD_ERROR(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDEFAULT_ERROR(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNO_CURRENT_DIRECTORY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCANNOT_OPEN_ZIP_FILE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCANNOT_OPEN_PATH_DIRECTORY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintPACKAGE_NOT_FOUND(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCANNOT_OPEN_DIRECTORY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintBAD_INPUT_FILE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNREADABLE_INPUT_FILE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNON_STANDARD_LIBRARY_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintLIBRARY_METHOD_NOT_FOUND(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCANNOT_REOPEN_FILE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCANNOT_WRITE_FILE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCONSTANT_POOL_OVERFLOW(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINTERFACES_OVERFLOW(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMETHODS_OVERFLOW(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSTRING_OVERFLOW(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintPARAMETER_OVERFLOW(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintARRAY_OVERFLOW(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintFIELDS_OVERFLOW(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintLOCAL_VARIABLES_OVERFLOW(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSTACK_OVERFLOW(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCODE_OVERFLOW(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCANNOT_COMPUTE_COLUMNS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintEMPTY_DECLARATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintREDUNDANT_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintRECOMMENDED_MODIFIER_ORDER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintOBSOLESCENT_BRACKETS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNO_TYPES(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMULTIPLE_PUBLIC_TYPES(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_IN_MULTIPLE_FILES(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintPACKAGE_TYPE_CONFLICT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDIRECTORY_FILE_CONFLICT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintFILE_FILE_CONFLICT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISMATCHED_TYPE_AND_FILE_NAMES(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintREFERENCE_TO_TYPE_IN_MISMATCHED_FILE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_INNER_TYPE_NAME(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_TYPE_DECLARATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNNECESSARY_TYPE_IMPORT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_ACCESS_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintFINAL_ABSTRACT_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintVOLATILE_FINAL_FIELD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_TOP_LEVEL_CLASS_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_INNER_CLASS_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_STATIC_INNER_CLASS_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_LOCAL_CLASS_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_INTERFACE_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_FIELD_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_LOCAL_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_METHOD_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_SIGNATURE_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_CONSTRUCTOR_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_CONSTANT_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNINITIALIZED_FIELD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintRECOMPILATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_FOUND(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintIMPORT_FROM_UNNAMED_PACKAGE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_ON_DEMAND_IMPORT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNKNOWN_ON_DEMAND_IMPORT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNOT_A_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNOT_A_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNOT_AN_INTERFACE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSUPER_IS_FINAL(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintOBJECT_WITH_SUPER_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintOBJECT_HAS_NO_SUPER_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_FIELD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_METHOD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_CONSTRUCTOR(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISMATCHED_INHERITED_METHOD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISMATCHED_IMPLICIT_METHOD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISMATCHED_INHERITED_METHOD_EXTERNALLY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_FORMAL_PARAMETER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISSPELLED_CONSTRUCTOR_NAME(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISMATCHED_CONSTRUCTOR_NAME(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMETHOD_WITH_CONSTRUCTOR_NAME(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_LOCAL_VARIABLE_DECLARATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_LOCAL_TYPE_DECLARATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMULTIPLE_DEFAULT_LABEL(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNDECLARED_LABEL(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_LABEL(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCATCH_PRIMITIVE_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCATCH_ARRAY_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintAMBIGUOUS_FIELD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintAMBIGUOUS_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintFIELD_IS_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintFIELD_NOT_FOUND(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintFIELD_NAME_MISSPELLED(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMETHOD_NOT_FIELD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNAME_NOT_YET_AVAILABLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNAME_NOT_CLASS_VARIABLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNOT_A_NUMERIC_VARIABLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMETHOD_OVERLOAD_NOT_FOUND(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMETHOD_NOT_FOUND(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMETHOD_NAME_MISSPELLED(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintHIDDEN_METHOD_IN_ENCLOSING_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintFIELD_NOT_METHOD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_METHOD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_FIELD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMETHOD_NOT_CLASS_METHOD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintAMBIGUOUS_CONSTRUCTOR_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintAMBIGUOUS_METHOD_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCONSTRUCTOR_NOT_FOUND(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMETHOD_FOUND_FOR_CONSTRUCTOR(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintABSTRACT_TYPE_CREATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_INSTANCEOF_CONVERSION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_CAST_CONVERSION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_CAST_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINCOMPATIBLE_TYPE_FOR_INITIALIZATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINCOMPATIBLE_TYPE_FOR_ASSIGNMENT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINCOMPATIBLE_TYPE_FOR_BINARY_EXPRESSION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINCOMPATIBLE_TYPE_FOR_CONDITIONAL_EXPRESSION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintVOID_ARRAY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintVOID_TYPE_IN_EQUALITY_EXPRESSION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_THROWABLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_PRIMITIVE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_INTEGRAL(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_NUMERIC(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_INTEGER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_BOOLEAN(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_ARRAY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_REFERENCE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_VALID_FOR_SWITCH(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_IS_VOID(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintVALUE_NOT_REPRESENTABLE_IN_SWITCH_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_CONVERTIBLE_TO_SWITCH_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_CASE_VALUE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISPLACED_THIS_EXPRESSION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISPLACED_SUPER_EXPRESSION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintVARIABLE_NOT_DEFINITELY_UNASSIGNED(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintVARIABLE_NOT_DEFINITELY_UNASSIGNED_IN_LOOP(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintFINAL_VARIABLE_NOT_BLANK(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNINITIALIZED_FINAL_VARIABLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNINITIALIZED_STATIC_FINAL_VARIABLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNINITIALIZED_FINAL_VARIABLE_IN_CONSTRUCTOR(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNINITIALIZED_FINAL_VARIABLE_IN_INTERFACE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINIT_SCALAR_WITH_ARRAY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINIT_ARRAY_WITH_SCALAR(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_BYTE_VALUE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_SHORT_VALUE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_CHARACTER_VALUE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_INT_VALUE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_LONG_VALUE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_FLOAT_VALUE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_DOUBLE_VALUE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_STRING_VALUE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintRETURN_STATEMENT_IN_INITIALIZER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintABRUPT_INITIALIZER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISPLACED_RETURN_WITH_EXPRESSION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISPLACED_RETURN_WITH_NO_EXPRESSION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISMATCHED_RETURN_AND_METHOD_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintEXPRESSION_NOT_THROWABLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISPLACED_BREAK_STATEMENT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISPLACED_CONTINUE_STATEMENT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISPLACED_EXPLICIT_CONSTRUCTOR_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_CONTINUE_TARGET(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNON_ABSTRACT_TYPE_CONTAINS_ABSTRACT_METHOD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNON_ABSTRACT_TYPE_INHERITS_ABSTRACT_METHOD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNON_ABSTRACT_TYPE_CANNOT_OVERRIDE_DEFAULT_ABSTRACT_METHOD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDUPLICATE_INTERFACE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNKNOWN_AMBIGUOUS_NAME(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCIRCULAR_INTERFACE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCIRCULAR_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_ACCESSIBLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintFIELD_NOT_ACCESSIBLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintPROTECTED_INSTANCE_FIELD_NOT_ACCESSIBLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMETHOD_NOT_ACCESSIBLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintPROTECTED_INSTANCE_METHOD_NOT_ACCESSIBLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintPROTECTED_INTERFACE_METHOD_NOT_ACCESSIBLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCONSTRUCTOR_NOT_ACCESSIBLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintPARAMETER_REDECLARED(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintBAD_ABSTRACT_METHOD_MODIFIER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintABSTRACT_METHOD_MODIFIER_CONFLICT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSTRICTFP_NATIVE_METHOD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintABSTRACT_METHOD_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintFINAL_METHOD_OVERRIDE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintFINAL_IMPLICIT_METHOD_OVERRIDE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINSTANCE_METHOD_OVERRIDE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINSTANCE_METHOD_OVERRIDE_EXTERNALLY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCLASS_METHOD_OVERRIDE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISMATCHED_OVERRIDDEN_EXCEPTION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISMATCHED_IMPLICIT_OVERRIDDEN_EXCEPTION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintMISMATCHED_OVERRIDDEN_EXCEPTION_EXTERNALLY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintABSTRACT_METHOD_WITH_BODY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintNON_ABSTRACT_METHOD_WITHOUT_BODY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintBAD_ACCESS_METHOD_OVERRIDE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintBAD_ACCESS_METHOD_OVERRIDE_EXTERNALLY(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCIRCULAR_THIS_CALL(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINSTANCE_VARIABLE_IN_EXPLICIT_CONSTRUCTOR_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINSTANCE_METHOD_IN_EXPLICIT_CONSTRUCTOR_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSYNTHETIC_VARIABLE_ACCESS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSYNTHETIC_METHOD_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSYNTHETIC_CONSTRUCTOR_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTHIS_IN_EXPLICIT_CONSTRUCTOR_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSUPER_IN_EXPLICIT_CONSTRUCTOR_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINNER_CONSTRUCTOR_IN_EXPLICIT_CONSTRUCTOR_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintEXPRESSION_NOT_CONSTANT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNCAUGHT_METHOD_EXCEPTION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNCAUGHT_CONSTRUCTOR_EXCEPTION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNCAUGHT_ANONYMOUS_CONSTRUCTOR_EXCEPTION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNCAUGHT_THROWN_EXCEPTION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNCAUGHT_EXPLICIT_THIS_EXCEPTION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNCAUGHT_EXPLICIT_SUPER_EXCEPTION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNREACHABLE_CATCH_CLAUSE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNREACHABLE_DEFAULT_CATCH_CLAUSE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNREACHABLE_STATEMENT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintUNREACHABLE_STATEMENTS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintBLOCKED_CATCH_CLAUSE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintVARIABLE_NOT_DEFINITELY_ASSIGNED(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPED_METHOD_WITH_NO_RETURN(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDEFAULT_METHOD_NOT_OVERRIDDEN(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintONE_UNNAMED_PACKAGE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_IN_UNNAMED_PACKAGE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_IN_WRONG_PACKAGE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NAME_MISMATCH(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDEPRECATED_TYPE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDEPRECATED_FIELD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDEPRECATED_METHOD(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintDEPRECATED_CONSTRUCTOR(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCOMPRESSED_ZIP_FILE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_CLASS_FILE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCANNOT_OPEN_CLASS_FILE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINTERFACE_NOT_INNER_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSTATIC_NOT_INNER_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintTYPE_NOT_INNER_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSUPER_TYPE_NOT_INNER_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSTATIC_FIELD_IN_INNER_CLASS_NOT_FINAL(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSTATIC_FIELD_IN_INNER_CLASS_NOT_CONSTANT(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSTATIC_METHOD_IN_INNER_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSTATIC_TYPE_IN_INNER_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintSTATIC_INITIALIZER_IN_INNER_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINNER_CLASS_REFERENCE_TO_NON_FINAL_LOCAL_VARIABLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_LOCAL(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintILLEGAL_THIS_FIELD_ACCESS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintCONSTRUCTOR_FOUND_IN_ANONYMOUS_CLASS(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintENCLOSING_INSTANCE_ACCESS_FROM_CONSTRUCTOR_INVOCATION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintENCLOSING_INSTANCE_ACCESS_ACROSS_STATIC_REGION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintENCLOSING_INSTANCE_NOT_ACCESSIBLE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintINVALID_ENCLOSING_INSTANCE(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintZERO_DIVIDE_ERROR(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintZERO_DIVIDE_CAUTION(ErrorInfo &, LexStream *, Control &);
    static wchar_t *PrintVOID_TO_STRING(ErrorInfo &, LexStream *, Control &);

    void SortMessages();
};

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

#endif // error_INCLUDED

