// $Id$ -*- c++ -*-
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, 1999, 2000, 2001, 2002 International Business
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
class SymbolSet;
class Semantic;
class SemanticError;

//
// Since basic_ostringstream<wchar_t> is not supported correctly by all
// compilers, we now use this workaround class.
// WARNING: It is not threadsafe - calling Array() in multiple threads may
// cause a race condition, getting the wrong string as a result.
//
class ErrorString : public ConvertibleArray<wchar_t>
{
public:
    ErrorString();

    ErrorString& operator<<(const wchar_t* s);
    ErrorString& operator<<(const wchar_t c);
    ErrorString& operator<<(const char* s);
    ErrorString& operator<<(const char c);
    ErrorString& operator<<(int n);
    ErrorString& operator<<(ostream&(*f)(ostream&))
    {
        assert(f == (ostream&(*)(ostream&)) endl);
        return *this << '\n';
    }

    void width(int w);
    void fill(const char c);

    //
    // The returned value is not threadsafe, and is only guaranteed valid
    // until the next call to Array().
    //
    const wchar_t* Array();

private:
    void DoFill(int n);
    char fill_char;
    int field_width;
};


class ErrorInfo : public JikesError
{
    friend class SemanticError;

public:
    virtual const wchar_t* getErrorMessage();
    virtual const wchar_t* getErrorReport();

    virtual JikesErrorSeverity getSeverity();
    virtual const char* getFileName();

    virtual int getLeftLineNo();
    virtual int getLeftColumnNo();
    virtual int getRightLineNo();
    virtual int getRightColumnNo();

    const wchar_t* getInsert(unsigned which);

    ErrorInfo();
    virtual ~ErrorInfo();

private:
    int left_line_no;
    int left_column_no;
    int right_line_no;
    int right_column_no;

    LexStream::TokenIndex left_token;
    LexStream::TokenIndex right_token;

    enum { MAX_INSERTS = 9 };
    const wchar_t* insert[MAX_INSERTS];
    const wchar_t* msg;
    unsigned num;
    short msg_code;
    JikesErrorSeverity severity;

    static bool emacs_style_report;
    LexStream* lex_stream;

    const wchar_t* regularErrorString();
    const wchar_t* emacsErrorString();

    void Initialize(LexStream*);
};

class SemanticError
{
    friend class ErrorInfo;
    friend class JikesAPI;

public:
    enum WarningLevel
    {
        MANDATORY_ERROR = ErrorInfo::JIKES_ERROR,
        STRONG_WARNING = ErrorInfo::JIKES_CAUTION,
        WEAK_WARNING = ErrorInfo::JIKES_WARNING,
        DISABLED,
        NAMED_STRONG_ON,
        NAMED_STRONG_OFF,
        NAMED_WEAK_ON,
        NAMED_WEAK_OFF
    };

    enum SemanticErrorKind
    {
        BAD_ERROR,
        DEFAULT_ERROR,

        // File related errors.
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
        ASSERT_UNSUPPORTED_IN_TARGET,
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
        COMPRESSED_ZIP_FILE,
        INVALID_CLASS_FILE,
        CANNOT_OPEN_CLASS_FILE,

        // Warnings and pedantic errors.
        NEGATIVE_ARRAY_SIZE,
        UNNECESSARY_PARENTHESIS,
        EMPTY_DECLARATION,
        REDUNDANT_MODIFIER,
        RECOMMENDED_MODIFIER_ORDER,
        SWITCH_FALLTHROUGH,
        OBSOLESCENT_BRACKETS,
        NO_TYPES,
        MULTIPLE_PUBLIC_TYPES,
        TYPE_IN_MULTIPLE_FILES,
        PACKAGE_TYPE_CONFLICT,
        FILE_FILE_CONFLICT,
        MISMATCHED_TYPE_AND_FILE_NAMES,
        REFERENCE_TO_TYPE_IN_MISMATCHED_FILE,
        ZERO_DIVIDE_CAUTION,
        VOID_TO_STRING,

        // Type and package related errors.
        DUPLICATE_INNER_TYPE_NAME,
        DUPLICATE_TYPE_DECLARATION,
        DUPLICATE_IMPORT_NAME,
        UNNECESSARY_TYPE_IMPORT,
        DUPLICATE_ACCESS_MODIFIER,
        DUPLICATE_MODIFIER,
        FINAL_ABSTRACT_ENTITY,
        VOLATILE_FINAL_FIELD,
        INVALID_MODIFIER,
        RECOMPILATION,
        PACKAGE_NOT_TYPE,
        TYPE_NOT_FOUND,
        INVALID_TYPE_FOUND,
        IMPORT_FROM_UNNAMED_PACKAGE,
        DUPLICATE_ON_DEMAND_IMPORT,
        UNKNOWN_ON_DEMAND_IMPORT,
        IMPORT_NOT_CANONICAL,
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
        UNIMPLEMENTABLE_INTERFACE,
        UNIMPLEMENTABLE_CLASS,
        MISMATCHED_INHERITED_METHOD_EXTERNALLY,
        DUPLICATE_FORMAL_PARAMETER,
        MISSPELLED_CONSTRUCTOR_NAME,
        MISMATCHED_CONSTRUCTOR_NAME,
        METHOD_WITH_CONSTRUCTOR_NAME,

        // Statement and expression related errors.
        DUPLICATE_LOCAL_VARIABLE_DECLARATION,
        MULTIPLE_DEFAULT_LABEL,
        UNDECLARED_LABEL,
        DUPLICATE_LABEL,
        AMBIGUOUS_FIELD,
        AMBIGUOUS_TYPE,
        FIELD_NOT_FOUND,
        FIELD_NAME_MISSPELLED,
        METHOD_NOT_FIELD,
        NAME_NOT_YET_AVAILABLE,
        NAME_NOT_CLASS_VARIABLE,
        NOT_A_VARIABLE,
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
        CONSTRUCTOR_OVERLOAD_NOT_FOUND,
        ABSTRACT_TYPE_CREATION,
        INVALID_INSTANCEOF_CONVERSION,
        INVALID_CAST_CONVERSION,
        INCOMPATIBLE_TYPE_FOR_INITIALIZATION,
        INCOMPATIBLE_TYPE_FOR_ASSIGNMENT,
        INCOMPATIBLE_TYPE_FOR_BINARY_EXPRESSION,
        INCOMPATIBLE_TYPE_FOR_CONDITIONAL_EXPRESSION,
        VOID_ARRAY,
        DUPLICATE_THROWS_CLAUSE_CLASS,
        REDUNDANT_THROWS_CLAUSE_CLASS,
        UNCHECKED_THROWS_CLAUSE_CLASS,
        TYPE_NOT_THROWABLE,
        TYPE_NOT_INTEGRAL,
        TYPE_NOT_NUMERIC,
        TYPE_NOT_INTEGER,
        TYPE_NOT_BOOLEAN,
        TYPE_NOT_ARRAY,
        TYPE_NOT_REFERENCE,
        TYPE_IS_VOID,
        VALUE_NOT_REPRESENTABLE_IN_SWITCH_TYPE,
        DUPLICATE_CASE_VALUE,
        MISPLACED_THIS_EXPRESSION,
        MISPLACED_SUPER_EXPRESSION,

        // Definite assignment related errors.
        VARIABLE_NOT_DEFINITELY_UNASSIGNED,
        VARIABLE_NOT_DEFINITELY_UNASSIGNED_IN_LOOP,
        FINAL_VARIABLE_NOT_BLANK,
        FINAL_FIELD_ASSIGNMENT_NOT_SIMPLE,
        UNINITIALIZED_FINAL_VARIABLE,
        UNINITIALIZED_STATIC_FINAL_VARIABLE,
        UNINITIALIZED_FINAL_VARIABLE_IN_CONSTRUCTOR,
        INIT_SCALAR_WITH_ARRAY,
        INIT_ARRAY_WITH_SCALAR,
        INVALID_BYTE_VALUE,
        INVALID_SHORT_VALUE,
        INVALID_CHARACTER_VALUE,
        INVALID_INT_VALUE,
        INVALID_LONG_VALUE,
        INVALID_FLOAT_VALUE,
        INVALID_DOUBLE_VALUE,
        RETURN_STATEMENT_IN_INITIALIZER,
        ABRUPT_INITIALIZER,
        MISPLACED_RETURN_WITH_EXPRESSION,
        MISPLACED_RETURN_WITH_NO_EXPRESSION,
        MISMATCHED_RETURN_AND_METHOD_TYPE,
        EXPRESSION_NOT_THROWABLE,
        MISPLACED_BREAK_STATEMENT,
        MISPLACED_CONTINUE_STATEMENT,
        MISPLACED_EXPLICIT_CONSTRUCTOR,
        INVALID_CONTINUE_TARGET,

        // More type-related errors.
        NON_ABSTRACT_TYPE_CONTAINS_ABSTRACT_METHOD,
        NON_ABSTRACT_TYPE_INHERITS_ABSTRACT_METHOD,
        NON_ABSTRACT_TYPE_CANNOT_OVERRIDE_DEFAULT_ABSTRACT_METHOD,
        ANONYMOUS_TYPE_CANNOT_OVERRIDE_DEFAULT_ABSTRACT_METHOD,
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
        BAD_ABSTRACT_METHOD_MODIFIER,
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
        INSTANCE_VARIABLE_IN_EXPLICIT_CONSTRUCTOR,
        INSTANCE_METHOD_IN_EXPLICIT_CONSTRUCTOR,
        SYNTHETIC_VARIABLE_ACCESS,
        SYNTHETIC_METHOD_INVOCATION,
        SYNTHETIC_CONSTRUCTOR_INVOCATION,
        SYNTHETIC_TYPE_ACCESS,
        SELF_IN_EXPLICIT_CONSTRUCTOR,
        EXPRESSION_NOT_CONSTANT,
        UNCAUGHT_METHOD_EXCEPTION,
        UNCAUGHT_CONSTRUCTOR_EXCEPTION,
        UNCAUGHT_ANONYMOUS_CONSTRUCTOR_EXCEPTION,
        UNCAUGHT_THROWN_EXCEPTION,
        UNCAUGHT_EXPLICIT_THIS_EXCEPTION,
        UNCAUGHT_EXPLICIT_SUPER_EXCEPTION,
        UNREACHABLE_CATCH_CLAUSE,
        UNREACHABLE_STATEMENT,
        UNREACHABLE_STATEMENTS,
        BLOCKED_CATCH_CLAUSE,
        VARIABLE_NOT_DEFINITELY_ASSIGNED,
        TYPED_METHOD_WITH_NO_RETURN,
        DEFAULT_METHOD_NOT_OVERRIDDEN,

        // Package related errors.
        TYPE_NOT_IN_UNNAMED_PACKAGE,
        TYPE_IN_WRONG_PACKAGE,
        TYPE_NAME_MISMATCH,
        DEPRECATED_TYPE,
        DEPRECATED_FIELD,
        DEPRECATED_METHOD,
        DEPRECATED_CONSTRUCTOR,

        // Inner type related errors.
        INTERFACE_NOT_INNER_CLASS,
        STATIC_NOT_INNER_CLASS,
        SUPER_TYPE_NOT_INNER_CLASS,
        STATIC_FIELD_IN_INNER_CLASS_NOT_FINAL,
        STATIC_FIELD_IN_INNER_CLASS_NOT_CONSTANT,
        STATIC_METHOD_IN_INNER_CLASS,
        STATIC_TYPE_IN_INNER_CLASS,
        STATIC_INITIALIZER_IN_INNER_CLASS,
        INNER_CLASS_REFERENCE_TO_NON_FINAL_LOCAL_VARIABLE,
        INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_LOCAL,
        INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER,
        INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_TYPE,
        ILLEGAL_THIS_FIELD_ACCESS,
        CONSTRUCTOR_FOUND_IN_ANONYMOUS_CLASS,
        ENCLOSING_INSTANCE_ACCESS_FROM_CONSTRUCTOR_INVOCATION,
        ENCLOSING_INSTANCE_ACCESS_ACROSS_STATIC_REGION,
        ENCLOSING_INSTANCE_NOT_ACCESSIBLE,
        INVALID_ENCLOSING_INSTANCE,

        _num_kinds
    };

    //
    // Describes an error code for the purpose of turning it on or off by name.
    // The name is used on the command-line, in Jikes' -help output.
    //
    struct NamedError
    {
        const char* name;
        const char* reason;
        SemanticErrorKind code;
        WarningLevel level;
    };
    static NamedError named_errors[];

    static void StaticInitializer();
    static void InitializeMessages();

    static bool ProcessWarningSwitch(const char*);
    static void PrintNamedWarnings();
    static void EnableDefaultWarnings();

    void Report(SemanticErrorKind, LexStream::TokenIndex,
                LexStream::TokenIndex, const wchar_t* = NULL,
                const wchar_t* = NULL, const wchar_t* = NULL,
                const wchar_t* = NULL, const wchar_t* = NULL,
                const wchar_t* = NULL, const wchar_t* = NULL,
                const wchar_t* = NULL, const wchar_t* = NULL);

    SemanticError(Control&, FileSymbol*);
    ~SemanticError()
    {
        for (unsigned i = 0; i < buffer.Length(); i++)
            delete [] buffer[i];
    }

    int num_errors,
        num_warnings;

    void EnteringClone() { clone_count++; }
    void ExitingClone() { clone_count--; }
    bool InClone() { return clone_count > 0; }

    int PrintMessages();

private:
    friend class Semantic;

    void reportError(int k);
    void FormatError(ErrorInfo& err);

    Control& control;
    LexStream* lex_stream;

    int clone_count;

    Tuple<wchar_t*> buffer;
    Tuple<ErrorInfo> error;

    static WarningLevel warning[_num_kinds];
    static const char* messages[_num_kinds];

    void SortMessages();
};

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

#endif // error_INCLUDED

