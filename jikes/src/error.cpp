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
#include "unicode.h"
#include "error.h"
#include "control.h"
#include "semantic.h"
#include "ast.h"

unsigned char SemanticError::warning[SemanticError::_num_kinds] = { 0 };
void (*SemanticError::print_message[SemanticError::_num_kinds]) (SemanticError::ErrorInfo &, LexStream *, Control &) = { NULL };

SemanticError::SemanticError(Control &control_, FileSymbol *file_symbol) : control(control_),
                                                                           lex_stream(file_symbol -> lex_stream),

                                                                           buffer(1024),
                                                                           error(512),
                                                                           clone_count(0),
                                                                           num_errors(0),
                                                                           num_warnings(0)
{}

//
// This procedure is invoked by an JIKES PARSER or a semantic
// routine to process an error message.  The JIKES parser always
// passes the value 0 to msg_level to indicate an error.
// This routine simply stores all necessary information about
// the message into an array: error.
//
void SemanticError::Report(SemanticErrorKind msg_code,
                           LexStream::TokenIndex left_token,
                           LexStream::TokenIndex right_token,
                           wchar_t *insert1,
                           wchar_t *insert2,
                           wchar_t *insert3,
                           wchar_t *insert4,
                           wchar_t *insert5,
                           wchar_t *insert6,
                           wchar_t *insert7,
                           wchar_t *insert8,
                           wchar_t *insert9)
{
    if (clone_count > 0) // do not report errors detected while processing a clone !!!
        return;

    int i = error.NextIndex();

    if (warning[msg_code] > 0)
         num_warnings++;
    else num_errors++;

    error[i].msg_code = msg_code;

    int total_length = 0,
        length1,
        length2,
        length3,
        length4,
        length5,
        length6,
        length7,
        length8,
        length9;

    if (insert1)
    {
        length1 = wcslen(insert1);
        total_length += (length1 + 1);
    }
    else error[i].insert1 = NULL;

    if (insert2)
    {
        length2 = wcslen(insert2);
        total_length += (length2 + 1);
    }
    else error[i].insert2 = NULL;

    if (insert3)
    {
        length3 = wcslen(insert3);
        total_length += (length3 + 1);
    }
    else error[i].insert3 = NULL;

    if (insert4)
    {
        length4 = wcslen(insert4);
        total_length += (length4 + 1);
    }
    else error[i].insert4 = NULL;

    if (insert5)
    {
        length5 = wcslen(insert5);
        total_length += (length5 + 1);
    }
    else error[i].insert5 = NULL;

    if (insert6)
    {
        length6 = wcslen(insert6);
        total_length += (length6 + 1);
    }
    else error[i].insert6 = NULL;

    if (insert7)
    {
        length7 = wcslen(insert7);
        total_length += (length7 + 1);
    }
    else error[i].insert7 = NULL;

    if (insert8)
    {
        length8 = wcslen(insert8);
        total_length += (length8 + 1);
    }
    else error[i].insert8 = NULL;

    if (insert9)
    {
        length9 = wcslen(insert9);
        total_length += (length9 + 1);
    }
    else error[i].insert9 = NULL;

    if (total_length > 0)
    {
        wchar_t *ptr = new wchar_t[total_length];
        buffer.Next() = ptr;

        if (insert1)
        {
            memmove(ptr, insert1, length1 * sizeof(wchar_t));
            error[i].insert1 = ptr;
            ptr += length1;
            *ptr++ = U_NULL;
        }

        if (insert2)
        {
            memmove(ptr, insert2, length2 * sizeof(wchar_t));
            error[i].insert2 = ptr;
            ptr += length2;
            *ptr++ = U_NULL;
        }

        if (insert3)
        {
            memmove(ptr, insert3, length3 * sizeof(wchar_t));
            error[i].insert3 = ptr;
            ptr += length3;
            *ptr++ = U_NULL;
        }

        if (insert4)
        {
            memmove(ptr, insert4, length4 * sizeof(wchar_t));
            error[i].insert4 = ptr;
            ptr += length4;
            *ptr++ = U_NULL;
        }

        if (insert5)
        {
            memmove(ptr, insert5, length5 * sizeof(wchar_t));
            error[i].insert5 = ptr;
            ptr += length5;
            *ptr++ = U_NULL;
        }

        if (insert6)
        {
            memmove(ptr, insert6, length6 * sizeof(wchar_t));
            error[i].insert6 = ptr;
            ptr += length6;
            *ptr++ = U_NULL;
        }

        if (insert7)
        {
            memmove(ptr, insert7, length7 * sizeof(wchar_t));
            error[i].insert7 = ptr;
            ptr += length7;
            *ptr++ = U_NULL;
        }

        if (insert8)
        {
            memmove(ptr, insert8, length8 * sizeof(wchar_t));
            error[i].insert8 = ptr;
            ptr += length8;
            *ptr++ = U_NULL;
        }

        if (insert9)
        {
            memmove(ptr, insert9, length9 * sizeof(wchar_t));
            error[i].insert9 = ptr;
            ptr += length9;
            *ptr++ = U_NULL;
        }
    }

    error[i].num         = i;
    error[i].left_token  = (left_token > right_token ? right_token : left_token);
    error[i].right_token = right_token;
    error[i].right_string_length = lex_stream -> NameLength(right_token);

    //
    // Dump the error immediately ?
    //
    if (control.option.dump_errors)
    {
        PrintEmacsMessage(i);

        if (buffer.Length() > 0)
        {
            delete [] buffer[0];
            buffer.Reset();
        }
        error.Reset(1); // we need at least 1 error in order for the return code to be set properly. See print_messages
        cout.flush();
    }
 
    return;
}

void SemanticError::StaticInitializer()
{
    memset(warning, 0, _num_kinds * sizeof(bool));

    warning[INVALID_OPTION] = 1;
    warning[CANNOT_OPEN_ZIP_FILE] = 1;

    warning[EMPTY_DECLARATION] = 1;
    warning[NO_TYPES] = 1;
    warning[PARENT_TYPE_IN_UNNAMED_PACKAGE] = 1;

    warning[MULTIPLE_PUBLIC_TYPES] = 1;
    warning[TYPE_IN_MULTIPLE_FILES] = 1;
    warning[MISMATCHED_TYPE_AND_FILE_NAMES] = 1;
    warning[REFERENCE_TO_TYPE_IN_MISMATCHED_FILE] = 1;
    warning[ONE_UNNAMED_PACKAGE] = 1;
    warning[RECOMPILATION] = 1;
    warning[METHOD_WITH_CONSTRUCTOR_NAME] = 1;

    //
    // TODO: Review the cases below. They should be flagged as errors.
    //       However, since javac does not flag them at all, we only issue
    //       a warning.
    warning[INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_LOCAL] = 1;
    warning[INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER] = 1;
    warning[PRIVATE_METHOD_OVERRIDE] = 1;
    warning[PRIVATE_METHOD_OVERRIDE_EXTERNALLY] = 1;

    //
    // Something stronger than a warning, but code will be generated anyway
    //
    warning[BAD_INPUT_FILE] = 2;
    warning[UNREADABLE_INPUT_FILE] = 2;

//
// TODO: Let these conditions be flagged as errors now.
//
//    warning[UNREACHABLE_CATCH_CLAUSE] = 2;
//    warning[VARIABLE_NOT_DEFINITELY_ASSIGNED] = 2;
//    warning[TARGET_VARIABLE_IS_FINAL] = 2;
//    warning[FINAL_VARIABLE_TARGET_IN_LOOP] = 2;
//    warning[VOID_TO_STRING] = 2;
//

#ifdef TEST
    for (int i = 0; i < _num_kinds; i++)
        print_message[i] = NULL;
#endif

    print_message[BAD_ERROR] = PrintBAD_ERROR;
    print_message[DEFAULT_ERROR] = PrintDEFAULT_ERROR;
    print_message[INVALID_OPTION] = PrintINVALID_OPTION;
    print_message[INVALID_K_OPTION] = PrintINVALID_K_OPTION;
    print_message[INVALID_K_TARGET] = PrintINVALID_K_TARGET;
    print_message[INVALID_DIRECTORY] = PrintINVALID_DIRECTORY;
    print_message[UNSUPPORTED_OPTION] = PrintUNSUPPORTED_OPTION;
    print_message[NO_CURRENT_DIRECTORY] = PrintNO_CURRENT_DIRECTORY;
    print_message[CANNOT_OPEN_ZIP_FILE] = PrintCANNOT_OPEN_ZIP_FILE;
    print_message[PACKAGE_NOT_FOUND] = PrintPACKAGE_NOT_FOUND;
    print_message[CANNOT_OPEN_DIRECTORY] = PrintCANNOT_OPEN_DIRECTORY;
    print_message[BAD_INPUT_FILE] = PrintBAD_INPUT_FILE;
    print_message[UNREADABLE_INPUT_FILE] = PrintUNREADABLE_INPUT_FILE;
    print_message[CANNOT_REOPEN_FILE] = PrintCANNOT_REOPEN_FILE;
    print_message[CANNOT_WRITE_FILE] = PrintCANNOT_WRITE_FILE;
    print_message[CANNOT_COMPUTE_COLUMNS] = PrintCANNOT_COMPUTE_COLUMNS;
    print_message[EMPTY_DECLARATION] = PrintEMPTY_DECLARATION;
    print_message[NO_TYPES] = PrintNO_TYPES;
    print_message[MULTIPLE_PUBLIC_TYPES] = PrintMULTIPLE_PUBLIC_TYPES;
    print_message[TYPE_IN_MULTIPLE_FILES] = PrintTYPE_IN_MULTIPLE_FILES;
    print_message[PACKAGE_TYPE_CONFLICT] = PrintPACKAGE_TYPE_CONFLICT;
    print_message[DIRECTORY_FILE_CONFLICT] = PrintDIRECTORY_FILE_CONFLICT;
    print_message[FILE_FILE_CONFLICT] = PrintFILE_FILE_CONFLICT;
    print_message[MISMATCHED_TYPE_AND_FILE_NAMES] = PrintMISMATCHED_TYPE_AND_FILE_NAMES;
    print_message[REFERENCE_TO_TYPE_IN_MISMATCHED_FILE] = PrintREFERENCE_TO_TYPE_IN_MISMATCHED_FILE;
    print_message[DUPLICATE_INNER_TYPE_NAME] = PrintDUPLICATE_INNER_TYPE_NAME;
    print_message[DUPLICATE_TYPE_DECLARATION] = PrintDUPLICATE_TYPE_DECLARATION;
    print_message[DUPLICATE_ACCESS_MODIFIER] = PrintDUPLICATE_ACCESS_MODIFIER;
    print_message[DUPLICATE_MODIFIER] = PrintDUPLICATE_MODIFIER;
    print_message[FINAL_ABSTRACT_CLASS] = PrintFINAL_ABSTRACT_CLASS;
    print_message[VOLATILE_FINAL] = PrintVOLATILE_FINAL;
    print_message[FINAL_VOLATILE] = PrintFINAL_VOLATILE;
    print_message[INVALID_TOP_LEVEL_CLASS_MODIFIER] = PrintINVALID_TOP_LEVEL_CLASS_MODIFIER;
    print_message[INVALID_INNER_CLASS_MODIFIER] = PrintINVALID_INNER_CLASS_MODIFIER;
    print_message[INVALID_STATIC_INNER_CLASS_MODIFIER] = PrintINVALID_STATIC_INNER_CLASS_MODIFIER;
    print_message[INVALID_LOCAL_CLASS_MODIFIER] = PrintINVALID_LOCAL_CLASS_MODIFIER;
    print_message[INVALID_INTERFACE_MODIFIER] = PrintINVALID_INTERFACE_MODIFIER;
    print_message[INVALID_FIELD_MODIFIER] = PrintINVALID_FIELD_MODIFIER;
    print_message[INVALID_LOCAL_MODIFIER] = PrintINVALID_LOCAL_MODIFIER;
    print_message[INVALID_METHOD_MODIFIER] = PrintINVALID_METHOD_MODIFIER;
    print_message[INVALID_SIGNATURE_MODIFIER] = PrintINVALID_SIGNATURE_MODIFIER;
    print_message[INVALID_CONSTRUCTOR_MODIFIER] = PrintINVALID_CONSTRUCTOR_MODIFIER;
    print_message[INVALID_CONSTANT_MODIFIER] = PrintINVALID_CONSTANT_MODIFIER;
    print_message[UNINITIALIZED_FIELD] = PrintUNINITIALIZED_FIELD;
    print_message[PARENT_TYPE_IN_UNNAMED_PACKAGE] = PrintPARENT_TYPE_IN_UNNAMED_PACKAGE;
    print_message[RECOMPILATION] = PrintRECOMPILATION;
    print_message[TYPE_NOT_FOUND] = PrintTYPE_NOT_FOUND;
    print_message[DUPLICATE_ON_DEMAND_IMPORT] = PrintDUPLICATE_ON_DEMAND_IMPORT;
    print_message[NOT_A_TYPE] = PrintNOT_A_TYPE;
    print_message[NOT_A_CLASS] = PrintNOT_A_CLASS;
    print_message[NOT_AN_INTERFACE] = PrintNOT_AN_INTERFACE;
    print_message[SUPER_IS_FINAL] = PrintSUPER_IS_FINAL;
    print_message[OBJECT_WITH_SUPER_TYPE] = PrintOBJECT_WITH_SUPER_TYPE;
    print_message[OBJECT_HAS_NO_SUPER_TYPE] = PrintOBJECT_HAS_NO_SUPER_TYPE;
    print_message[DUPLICATE_FIELD] = PrintDUPLICATE_FIELD;
    print_message[DUPLICATE_METHOD] = PrintDUPLICATE_METHOD;
    print_message[DUPLICATE_CONSTRUCTOR] = PrintDUPLICATE_CONSTRUCTOR;
    print_message[MISMATCHED_INHERITED_METHOD] = PrintMISMATCHED_INHERITED_METHOD;
    print_message[MISMATCHED_INHERITED_METHOD_EXTERNALLY] = PrintMISMATCHED_INHERITED_METHOD_EXTERNALLY;
    print_message[DUPLICATE_FORMAL_PARAMETER] = PrintDUPLICATE_FORMAL_PARAMETER;
    print_message[MISMATCHED_CONSTRUCTOR_NAME] = PrintMISMATCHED_CONSTRUCTOR_NAME;
    print_message[METHOD_WITH_CONSTRUCTOR_NAME] = PrintMETHOD_WITH_CONSTRUCTOR_NAME;
    print_message[DUPLICATE_LOCAL_VARIABLE_DECLARATION] = PrintDUPLICATE_LOCAL_VARIABLE_DECLARATION;
    print_message[DUPLICATE_LOCAL_TYPE_DECLARATION] = PrintDUPLICATE_LOCAL_TYPE_DECLARATION;
    print_message[MULTIPLE_DEFAULT_LABEL] = PrintMULTIPLE_DEFAULT_LABEL;
    print_message[UNDECLARED_LABEL] = PrintUNDECLARED_LABEL;
    print_message[DUPLICATE_LABEL] = PrintDUPLICATE_LABEL;
    print_message[CATCH_PRIMITIVE_TYPE] = PrintCATCH_PRIMITIVE_TYPE;
    print_message[CATCH_ARRAY_TYPE] = PrintCATCH_ARRAY_TYPE;
    print_message[AMBIGUOUS_NAME] = PrintAMBIGUOUS_NAME;
    print_message[FIELD_IS_TYPE] = PrintFIELD_IS_TYPE;
    print_message[FIELD_NOT_FOUND] = PrintFIELD_NOT_FOUND;
    print_message[FIELD_WITH_PRIVATE_ACCESS_NOT_ACCESSIBLE] = PrintFIELD_WITH_PRIVATE_ACCESS_NOT_ACCESSIBLE;
    print_message[FIELD_WITH_DEFAULT_ACCESS_NOT_ACCESSIBLE] = PrintFIELD_WITH_DEFAULT_ACCESS_NOT_ACCESSIBLE;
    print_message[NAME_NOT_FOUND] = PrintNAME_NOT_FOUND;
    print_message[METHOD_NOT_FIELD] = PrintMETHOD_NOT_FIELD;
    print_message[NAME_NOT_YET_AVAILABLE] = PrintNAME_NOT_YET_AVAILABLE;
    print_message[NAME_NOT_VARIABLE] = PrintNAME_NOT_VARIABLE;
    print_message[NAME_NOT_CLASS_VARIABLE] = PrintNAME_NOT_CLASS_VARIABLE;
    print_message[NOT_A_NUMERIC_VARIABLE] = PrintNOT_A_NUMERIC_VARIABLE;
    print_message[METHOD_NOT_FOUND] = PrintMETHOD_NOT_FOUND;
    print_message[METHOD_WITH_PRIVATE_ACCESS_NOT_ACCESSIBLE] = PrintMETHOD_WITH_PRIVATE_ACCESS_NOT_ACCESSIBLE;
    print_message[METHOD_WITH_DEFAULT_ACCESS_NOT_ACCESSIBLE] = PrintMETHOD_WITH_DEFAULT_ACCESS_NOT_ACCESSIBLE;
    print_message[FIELD_NOT_METHOD] = PrintFIELD_NOT_METHOD;
    print_message[TYPE_NOT_METHOD] = PrintTYPE_NOT_METHOD;
    print_message[TYPE_NOT_FIELD] = PrintTYPE_NOT_FIELD;
    print_message[METHOD_NOT_CLASS_METHOD] = PrintMETHOD_NOT_CLASS_METHOD;
    print_message[AMBIGUOUS_CONSTRUCTOR_INVOCATION] = PrintAMBIGUOUS_CONSTRUCTOR_INVOCATION;
    print_message[AMBIGUOUS_METHOD_INVOCATION] = PrintAMBIGUOUS_METHOD_INVOCATION;
    print_message[CONSTRUCTOR_NOT_FOUND] = PrintCONSTRUCTOR_NOT_FOUND;
    print_message[METHOD_FOUND_FOR_CONSTRUCTOR] = PrintMETHOD_FOUND_FOR_CONSTRUCTOR;
    print_message[ABSTRACT_TYPE_CREATION] = PrintABSTRACT_TYPE_CREATION;
    print_message[INVALID_INSTANCEOF_CONVERSION] = PrintINVALID_INSTANCEOF_CONVERSION;
    print_message[INVALID_CAST_CONVERSION] = PrintINVALID_CAST_CONVERSION;
    print_message[INVALID_CAST_TYPE] = PrintINVALID_CAST_TYPE;
    print_message[INCOMPATIBLE_TYPE_FOR_INITIALIZATION] = PrintINCOMPATIBLE_TYPE_FOR_INITIALIZATION;
    print_message[INCOMPATIBLE_TYPE_FOR_ASSIGNMENT] = PrintINCOMPATIBLE_TYPE_FOR_ASSIGNMENT;
    print_message[INCOMPATIBLE_TYPE_FOR_BINARY_EXPRESSION] = PrintINCOMPATIBLE_TYPE_FOR_BINARY_EXPRESSION;
    print_message[INCOMPATIBLE_TYPE_FOR_CONDITIONAL_EXPRESSION] = PrintINCOMPATIBLE_TYPE_FOR_CONDITIONAL_EXPRESSION;
    print_message[VOID_TYPE_IN_EQUALITY_EXPRESSION] = PrintVOID_TYPE_IN_EQUALITY_EXPRESSION;
    print_message[TYPE_NOT_THROWABLE] = PrintTYPE_NOT_THROWABLE;
    print_message[TYPE_NOT_PRIMITIVE] = PrintTYPE_NOT_PRIMITIVE;
    print_message[TYPE_NOT_INTEGRAL] = PrintTYPE_NOT_INTEGRAL;
    print_message[TYPE_NOT_NUMERIC] = PrintTYPE_NOT_NUMERIC;
    print_message[TYPE_NOT_INTEGER] = PrintTYPE_NOT_INTEGER;
    print_message[TYPE_NOT_BOOLEAN] = PrintTYPE_NOT_BOOLEAN;
    print_message[TYPE_NOT_ARRAY] = PrintTYPE_NOT_ARRAY;
    print_message[TYPE_NOT_REFERENCE] = PrintTYPE_NOT_REFERENCE;
    print_message[TYPE_NOT_VALID_FOR_SWITCH] = PrintTYPE_NOT_VALID_FOR_SWITCH;
    print_message[TYPE_IS_VOID] = PrintTYPE_IS_VOID;
    print_message[VALUE_NOT_REPRESENTABLE_IN_TYPE] = PrintVALUE_NOT_REPRESENTABLE_IN_TYPE;
    print_message[DUPLICATE_CASE_VALUE] = PrintDUPLICATE_CASE_VALUE;
    print_message[MISPLACED_THIS_EXPRESSION] = PrintMISPLACED_THIS_EXPRESSION;
    print_message[MISPLACED_SUPER_EXPRESSION] = PrintMISPLACED_SUPER_EXPRESSION;
    print_message[TARGET_VARIABLE_IS_FINAL] = PrintTARGET_VARIABLE_IS_FINAL;
    print_message[FINAL_VARIABLE_TARGET_IN_LOOP] = PrintFINAL_VARIABLE_TARGET_IN_LOOP;
    print_message[UNINITIALIZED_FINAL_VARIABLE] = PrintUNINITIALIZED_FINAL_VARIABLE;
    print_message[UNINITIALIZED_STATIC_FINAL_VARIABLE] = PrintUNINITIALIZED_STATIC_FINAL_VARIABLE;
    print_message[UNINITIALIZED_FINAL_VARIABLE_IN_CONSTRUCTOR] = PrintUNINITIALIZED_FINAL_VARIABLE_IN_CONSTRUCTOR;
    print_message[INIT_SCALAR_WITH_ARRAY] = PrintINIT_SCALAR_WITH_ARRAY;
    print_message[INIT_ARRAY_WITH_SCALAR] = PrintINIT_ARRAY_WITH_SCALAR;
    print_message[INVALID_BYTE_VALUE] = PrintINVALID_BYTE_VALUE;
    print_message[INVALID_SHORT_VALUE] = PrintINVALID_SHORT_VALUE;
    print_message[INVALID_CHARACTER_VALUE] = PrintINVALID_CHARACTER_VALUE;
    print_message[INVALID_INT_VALUE] = PrintINVALID_INT_VALUE;
    print_message[INVALID_LONG_VALUE] = PrintINVALID_LONG_VALUE;
    print_message[INVALID_FLOAT_VALUE] = PrintINVALID_FLOAT_VALUE;
    print_message[INVALID_DOUBLE_VALUE] = PrintINVALID_DOUBLE_VALUE;
    print_message[INVALID_STRING_VALUE] = PrintINVALID_STRING_VALUE;
    print_message[RETURN_STATEMENT_IN_INITIALIZER] = PrintRETURN_STATEMENT_IN_INITIALIZER;
    print_message[MISPLACED_RETURN_WITH_EXPRESSION] = PrintMISPLACED_RETURN_WITH_EXPRESSION;
    print_message[MISPLACED_RETURN_WITH_NO_EXPRESSION] = PrintMISPLACED_RETURN_WITH_NO_EXPRESSION;
    print_message[MISMATCHED_RETURN_AND_METHOD_TYPE] = PrintMISMATCHED_RETURN_AND_METHOD_TYPE;
    print_message[EXPRESSION_NOT_THROWABLE] = PrintEXPRESSION_NOT_THROWABLE;
    print_message[BAD_THROWABLE_EXPRESSION_IN_TRY] = PrintBAD_THROWABLE_EXPRESSION_IN_TRY;
    print_message[BAD_THROWABLE_EXPRESSION_IN_METHOD] = PrintBAD_THROWABLE_EXPRESSION_IN_METHOD;
    print_message[BAD_THROWABLE_EXPRESSION] = PrintBAD_THROWABLE_EXPRESSION;
    print_message[MISPLACED_BREAK_STATEMENT] = PrintMISPLACED_BREAK_STATEMENT;
    print_message[MISPLACED_CONTINUE_STATEMENT] = PrintMISPLACED_CONTINUE_STATEMENT;
    print_message[INVALID_CONTINUE_TARGET] = PrintINVALID_CONTINUE_TARGET;
    print_message[NON_ABSTRACT_TYPE_CONTAINS_ABSTRACT_METHOD] = PrintNON_ABSTRACT_TYPE_CONTAINS_ABSTRACT_METHOD;
    print_message[NON_ABSTRACT_TYPE_INHERITS_ABSTRACT_METHOD] = PrintNON_ABSTRACT_TYPE_INHERITS_ABSTRACT_METHOD;
    print_message[NO_ABSTRACT_METHOD_IMPLEMENTATION] = PrintNO_ABSTRACT_METHOD_IMPLEMENTATION;
    print_message[DUPLICATE_INTERFACE] = PrintDUPLICATE_INTERFACE;
    print_message[UNKNOWN_QUALIFIED_NAME_BASE] = PrintUNKNOWN_QUALIFIED_NAME_BASE;
    print_message[UNKNOWN_AMBIGUOUS_NAME] = PrintUNKNOWN_AMBIGUOUS_NAME;
    print_message[CIRCULAR_INTERFACE] = PrintCIRCULAR_INTERFACE;
    print_message[CIRCULAR_CLASS] = PrintCIRCULAR_CLASS;
    print_message[TYPE_NOT_ACCESSIBLE] = PrintTYPE_NOT_ACCESSIBLE;
    print_message[PRIVATE_FIELD_NOT_ACCESSIBLE] = PrintPRIVATE_FIELD_NOT_ACCESSIBLE;
    print_message[PROTECTED_FIELD_NOT_ACCESSIBLE] = PrintPROTECTED_FIELD_NOT_ACCESSIBLE;
    print_message[DEFAULT_FIELD_NOT_ACCESSIBLE] = PrintDEFAULT_FIELD_NOT_ACCESSIBLE;
    print_message[PRIVATE_METHOD_NOT_ACCESSIBLE] = PrintPRIVATE_METHOD_NOT_ACCESSIBLE;
    print_message[PROTECTED_METHOD_NOT_ACCESSIBLE] = PrintPROTECTED_METHOD_NOT_ACCESSIBLE;
    print_message[DEFAULT_METHOD_NOT_ACCESSIBLE] = PrintDEFAULT_METHOD_NOT_ACCESSIBLE;
    print_message[PRIVATE_CONSTRUCTOR_NOT_ACCESSIBLE] = PrintPRIVATE_CONSTRUCTOR_NOT_ACCESSIBLE;
    print_message[PROTECTED_CONSTRUCTOR_NOT_ACCESSIBLE] = PrintPROTECTED_CONSTRUCTOR_NOT_ACCESSIBLE;
    print_message[DEFAULT_CONSTRUCTOR_NOT_ACCESSIBLE] = PrintDEFAULT_CONSTRUCTOR_NOT_ACCESSIBLE;
    print_message[DEFAULT_CONSTRUCTOR_CANNOT_THROW] = PrintDEFAULT_CONSTRUCTOR_CANNOT_THROW;
    print_message[PARAMETER_REDECLARED] = PrintPARAMETER_REDECLARED;
    print_message[BAD_ABSTRACT_METHOD_MODIFIER] = PrintBAD_ABSTRACT_METHOD_MODIFIER;
    print_message[ABSTRACT_METHOD_MODIFIER_CONFLICT] = PrintABSTRACT_METHOD_MODIFIER_CONFLICT;
    print_message[ABSTRACT_METHOD_INVOCATION] = PrintABSTRACT_METHOD_INVOCATION;
    print_message[FINAL_METHOD_OVERRIDE] = PrintFINAL_METHOD_OVERRIDE;
    print_message[FINAL_METHOD_OVERRIDE_EXTERNALLY] = PrintFINAL_METHOD_OVERRIDE_EXTERNALLY;
    print_message[PRIVATE_METHOD_OVERRIDE] = PrintPRIVATE_METHOD_OVERRIDE;
    print_message[PRIVATE_METHOD_OVERRIDE_EXTERNALLY] = PrintPRIVATE_METHOD_OVERRIDE_EXTERNALLY;
    print_message[INSTANCE_METHOD_OVERRIDE] = PrintINSTANCE_METHOD_OVERRIDE;
    print_message[INSTANCE_METHOD_OVERRIDE_EXTERNALLY] = PrintINSTANCE_METHOD_OVERRIDE_EXTERNALLY;
    print_message[CLASS_METHOD_OVERRIDE] = PrintCLASS_METHOD_OVERRIDE;
    print_message[CLASS_METHOD_OVERRIDE_EXTERNALLY] = PrintCLASS_METHOD_OVERRIDE_EXTERNALLY;
    print_message[MISMATCHED_OVERRIDDEN_EXCEPTION] = PrintMISMATCHED_OVERRIDDEN_EXCEPTION;
    print_message[MISMATCHED_OVERRIDDEN_EXCEPTION_EXTERNALLY] = PrintMISMATCHED_OVERRIDDEN_EXCEPTION_EXTERNALLY;
    print_message[ABSTRACT_METHOD_WITH_BODY] = PrintABSTRACT_METHOD_WITH_BODY;
    print_message[NON_ABSTRACT_METHOD_WITHOUT_BODY] = PrintNON_ABSTRACT_METHOD_WITHOUT_BODY;
    print_message[BAD_ACCESS_METHOD_OVERRIDE] = PrintBAD_ACCESS_METHOD_OVERRIDE;
    print_message[BAD_ACCESS_METHOD_OVERRIDE_EXTERNALLY] = PrintBAD_ACCESS_METHOD_OVERRIDE_EXTERNALLY;
    print_message[STATIC_OVERRIDE_ABSTRACT] = PrintSTATIC_OVERRIDE_ABSTRACT;
    print_message[STATIC_OVERRIDE_ABSTRACT_EXTERNALLY] = PrintSTATIC_OVERRIDE_ABSTRACT_EXTERNALLY;
    print_message[CIRCULAR_THIS_CALL] = PrintCIRCULAR_THIS_CALL;
    print_message[INSTANCE_VARIABLE_IN_EXPLICIT_CONSTRUCTOR_INVOCATION] = PrintINSTANCE_VARIABLE_IN_EXPLICIT_CONSTRUCTOR_INVOCATION;
    print_message[INSTANCE_METHOD_IN_EXPLICIT_CONSTRUCTOR_INVOCATION] = PrintINSTANCE_METHOD_IN_EXPLICIT_CONSTRUCTOR_INVOCATION;
    print_message[THIS_IN_EXPLICIT_CONSTRUCTOR_INVOCATION] = PrintTHIS_IN_EXPLICIT_CONSTRUCTOR_INVOCATION;
    print_message[SUPER_IN_EXPLICIT_CONSTRUCTOR_INVOCATION] = PrintSUPER_IN_EXPLICIT_CONSTRUCTOR_INVOCATION;
    print_message[EXPRESSION_NOT_CONSTANT] = PrintEXPRESSION_NOT_CONSTANT;
    print_message[UNCATCHABLE_METHOD_THROWN_CHECKED_EXCEPTION] = PrintUNCATCHABLE_METHOD_THROWN_CHECKED_EXCEPTION;
    print_message[UNCATCHABLE_CONSTRUCTOR_THROWN_CHECKED_EXCEPTION] = PrintUNCATCHABLE_CONSTRUCTOR_THROWN_CHECKED_EXCEPTION;
    print_message[UNREACHABLE_CATCH_CLAUSE] = PrintUNREACHABLE_CATCH_CLAUSE;
    print_message[UNREACHABLE_STATEMENT] = PrintUNREACHABLE_STATEMENT;
    print_message[UNREACHABLE_STATEMENTS] = PrintUNREACHABLE_STATEMENTS;
    print_message[UNREACHABLE_CONSTRUCTOR_BODY] = PrintUNREACHABLE_CONSTRUCTOR_BODY;
    print_message[BLOCKED_CATCH_CLAUSE] = PrintBLOCKED_CATCH_CLAUSE;
    print_message[VARIABLE_NOT_DEFINITELY_ASSIGNED] = PrintVARIABLE_NOT_DEFINITELY_ASSIGNED;
    print_message[TYPED_METHOD_WITH_NO_RETURN] = PrintTYPED_METHOD_WITH_NO_RETURN;

    print_message[DEFAULT_METHOD_NOT_OVERRIDDEN] = PrintDEFAULT_METHOD_NOT_OVERRIDDEN;
    print_message[PRIVATE_METHOD_NOT_OVERRIDDEN] = PrintPRIVATE_METHOD_NOT_OVERRIDDEN;

    print_message[ONE_UNNAMED_PACKAGE] = PrintONE_UNNAMED_PACKAGE;
    print_message[TYPE_NOT_IN_UNNAMED_PACKAGE] = PrintTYPE_NOT_IN_UNNAMED_PACKAGE;
    print_message[TYPE_IN_WRONG_PACKAGE] = PrintTYPE_IN_WRONG_PACKAGE;
    print_message[TYPE_NAME_MISMATCH] = PrintTYPE_NAME_MISMATCH;

    print_message[COMPRESSED_ZIP_FILE] = PrintCOMPRESSED_ZIP_FILE;
    print_message[INVALID_CLASS_FILE] = PrintINVALID_CLASS_FILE;
    print_message[CANNOT_OPEN_CLASS_FILE] = PrintCANNOT_OPEN_CLASS_FILE;

    print_message[ONE_ONE_FEATURE] = PrintONE_ONE_FEATURE;
    print_message[STATIC_NOT_INNER_CLASS] = PrintSTATIC_NOT_INNER_CLASS;
    print_message[TYPE_NOT_INNER_CLASS] = PrintTYPE_NOT_INNER_CLASS;
    print_message[SUPER_TYPE_NOT_INNER_CLASS] = PrintSUPER_TYPE_NOT_INNER_CLASS;
    print_message[STATIC_FIELD_IN_INNER_CLASS] = PrintSTATIC_FIELD_IN_INNER_CLASS;
    print_message[STATIC_METHOD_IN_INNER_CLASS] = PrintSTATIC_METHOD_IN_INNER_CLASS;
    print_message[STATIC_TYPE_IN_INNER_CLASS] = PrintSTATIC_TYPE_IN_INNER_CLASS;
    print_message[STATIC_INITIALIZER_IN_INNER_CLASS] = PrintSTATIC_INITIALIZER_IN_INNER_CLASS;
    print_message[INNER_CLASS_REFERENCE_TO_NON_FINAL_LOCAL_VARIABLE] = PrintINNER_CLASS_REFERENCE_TO_NON_FINAL_LOCAL_VARIABLE;
    print_message[INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_LOCAL] = PrintINHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_LOCAL;
    print_message[INHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER] = PrintINHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER;
    print_message[ILLEGAL_THIS_FIELD_ACCESS] = PrintILLEGAL_THIS_FIELD_ACCESS;
    print_message[CONSTRUCTOR_FOUND_IN_ANONYMOUS_CLASS] = PrintCONSTRUCTOR_FOUND_IN_ANONYMOUS_CLASS;
    print_message[ENCLOSING_INSTANCE_NOT_ACCESSIBLE] = PrintENCLOSING_INSTANCE_NOT_ACCESSIBLE;
    print_message[INVALID_ENCLOSING_INSTANCE] = PrintINVALID_ENCLOSING_INSTANCE;
    print_message[PRIVATE_ENCLOSED_CONSTRUCTOR] = PrintPRIVATE_ENCLOSED_CONSTRUCTOR;
    print_message[ZERO_DIVIDE] = PrintZERO_DIVIDE;
    print_message[VOID_TO_STRING] = PrintVOID_TO_STRING;

#ifdef TEST
    for (int k = 0; k < _num_kinds; k++)
        assert(print_message[k] != NULL);
#endif
}


//
// This procedure uses a  quick sort algorithm to sort the ERRORS
// by the left_line_no and left_column_no fields.
//
void SemanticError::SortMessages()
{
     int lower,
         upper,
         lostack[32],
         histack[32];
 
     int top,
         i,
         j;
     ErrorInfo pivot,
               temp;

     top = 0;
     lostack[top] = 0;
     histack[top] = error.Length() - 1;
 
     while(top >= 0)
     {
         lower = lostack[top];
         upper = histack[top];
         top--;
 
         while(upper > lower)
         {
             //
             // The array is most-likely almost sorted. Therefore,
             // we use the middle element as the pivot element.
             //
             i = (lower + upper) / 2;
             pivot = error[i];
             error[i] = error[lower];
 
             //
             // Split the array section indicated by LOWER and UPPER
             // using ARRAY(LOWER) as the pivot.
             //
             i = lower;
             for (j = lower + 1; j <= upper; j++)
                 if ((error[j].left_token < pivot.left_token) ||
                 //
                 // When two error messages start in the same location
                 // and one is nested inside the other, the outer one
                 // is placed first so that it can be printed last.
                 // Recall that its right-span location is reached
                 // after the inner one has been completely processed.
                 //
                     (error[j].left_token == pivot.left_token &&
                      error[j].right_token > pivot.right_token) ||
                 //
                 // When two error messages are at the same location
                 // span, check the NUM field to keep the sort stable.
                 // When the location spans only a single symbol,
                 // the one with the lowest "num" is placed first.
                 //
                     (error[j].left_token  == pivot.left_token  &&
                      error[j].right_token == pivot.right_token &&
                      pivot.left_token == pivot.right_token     &&
                      error[j].num < pivot.num)                       ||
                 //
                 // When two error messages are at the same location
                 // which spans more than one symbol in the source,
                 // the first message is treated as being nested into
                 // the second message and (just like the nested case
                 // above) it is placed last in the sorted sequence.
                 //
                     (error[j].left_token  == pivot.left_token  &&
                      error[j].right_token == pivot.right_token &&
                      pivot.left_token < pivot.right_token      &&
                      error[j].num > pivot.num))
                 {
                     temp = error[++i];
                     error[i] = error[j];
                     error[j] = temp;
                 }
             error[lower] = error[i];
             error[i] = pivot;

             top++;
             if ((i - lower) < (upper - i))
             {
                 lostack[top] = i + 1;
                 histack[top] = upper;
                 upper = i - 1;
             }
             else
             {
                 histack[top] = i - 1;
                 lostack[top] = lower;
                 lower = i + 1;
             }
         }
     }

     return;
}


//
// This is the local private procedure that prints the semantic error messages.
//
int SemanticError::PrintMessages()
{
    int return_code = (num_errors > 0 ? 1 : 0);

    //
    // If the errors have not yet been dumped,...
    //
    if (! control.option.dump_errors)
    {
        if (control.option.errors)
        {
            if (num_errors == 0)
            {
                if (control.option.nowarn) // we only had warnings and they should not be reported
                    return return_code;

                cout << "\nIssued "
                     << num_warnings 
                     << (lex_stream -> file_symbol -> semantic == control.system_semantic ? " system" : " semantic")
                     << " warning" << (num_warnings <= 1 ? "" : "s");
            }
            else // we had some errors, and possibly warnings as well
            {
                cout << "\nFound "
                     << num_errors
                     << (lex_stream -> file_symbol -> semantic == control.system_semantic ? " system" : " semantic")
                     << " error" << (num_errors <= 1 ? "" : "s");
                if (num_warnings > 0 && !control.option.nowarn) 
                {
                     cout << " and issued "
                          << num_warnings << " warning" << (num_warnings <= 1 ? "" : "s");
                }
            }

            if (lex_stream -> file_symbol -> semantic != control.system_semantic)
            {
                cout << " compiling \"";
                Unicode::Cout(lex_stream -> FileName());
                cout << '\"';
            }
            cout << ':';

            //
            //
            //
            if (lex_stream -> file_symbol -> semantic != control.system_semantic)
            {
                lex_stream -> RereadInput();

                if (! lex_stream -> InputBuffer())
                {
                    char *file_name = lex_stream -> FileName();
                    int length = lex_stream -> FileNameLength();
                    wchar_t *name = new wchar_t[length + 1];
                    for (int i = 0; i < length; i++)
                        name[i] = file_name[i];
                    name[length] = U_NULL;
                    control.system_semantic -> ReportSemError(SemanticError::CANNOT_REOPEN_FILE,
                                                              0,
                                                              0,
                                                              name);
                    delete [] name;
                }
            }

            if (lex_stream -> file_symbol -> semantic == control.system_semantic || lex_stream -> InputBuffer())
            {
                SortMessages();
                for (int k = 0; k < error.Length(); k++)
                {
                    if ((warning[error[k].msg_code] != 1) || (! control.option.nowarn))
                    {
                        if (error[k].left_token < error[k].right_token)
                             PrintLargeSource(k);
                        else PrintSmallSource(k);
                        cout << "\n*** " << (warning[error[k].msg_code] == 1
                                                    ? "Warning: "
                                                    : (warning[error[k].msg_code] == 2 && (! control.option.zero_defect)
                                                              ? "Caution: "
                                                              : "Error: "));
                        (print_message[error[k].msg_code]) (error[k], lex_stream, control);
                        cout << '\n';
                    }
                }
                lex_stream -> DestroyInput();
            }
        }
        else
        {
            if (lex_stream -> file_symbol -> semantic != control.system_semantic)
            {
                if (! lex_stream -> ComputeColumns())
                {
                    char *file_name = lex_stream -> FileName();
                    int length = lex_stream -> FileNameLength();
                    wchar_t *name = new wchar_t[length + 1];
                    for (int i = 0; i < length; i++)
                        name[i] = file_name[i];
                    name[length] = U_NULL;
                    control.system_semantic -> ReportSemError(SemanticError::CANNOT_COMPUTE_COLUMNS,
                                                              0,
                                                              0,
                                                              name);
                    delete [] name;
                }
            }

            SortMessages();
            for (int k = 0; k < error.Length(); k++)
            {
                if ((warning[error[k].msg_code] != 1) || (! control.option.nowarn))
                    PrintEmacsMessage(k);
            }
        }
    }

    cout.flush();
    cerr.flush();

    return return_code;
}


void SemanticError::PrintEmacsMessage(int k)
{
    int left_line_no    = lex_stream -> Line(error[k].left_token),
        left_column_no  = lex_stream -> Column(error[k].left_token),
        right_line_no   = lex_stream -> Line(error[k].right_token),
        right_column_no = lex_stream -> Column(error[k].right_token);

    if (right_column_no != 0) // could not compute a column number
        right_column_no += (error[k].right_string_length - 1); // point to last character in right token

    Unicode::Cout(lex_stream -> FileName());
    cout << ':' << left_line_no  << ':' << left_column_no 
         << ':' << right_line_no << ':' << right_column_no
         << (warning[error[k].msg_code] == 1
                    ? ":\n    Warning: "
                    : (warning[error[k].msg_code] == 2  && (! control.option.zero_defect)
                              ? ":\n    Caution: "
                              : ":\n    Error: "));
    (print_message[error[k].msg_code]) (error[k], lex_stream, control);
    cout << '\n';

    return;
}


//
// This procedure is invoked to print a large message that may
// span more than one line. The parameter message points to the
// starting line. The parameter k points to the error message in
// the error structure.
//
void SemanticError::PrintLargeSource(int k)
{
    int left_line_no    = lex_stream -> Line(error[k].left_token),
        left_column_no  = lex_stream -> Column(error[k].left_token),
        right_line_no   = lex_stream -> Line(error[k].right_token),
        right_column_no = lex_stream -> Column(error[k].right_token);
 
    if (left_line_no == right_line_no)
    {
        if (left_line_no == 0)
            cout << "\n";
        else
        {
            cout << "\n\n";
            cout.width(6);
            cout << left_line_no << ". ";
            for (int i = lex_stream -> LineStart(left_line_no); i <= lex_stream -> LineEnd(left_line_no); i++)
                Unicode::Cout(lex_stream -> InputBuffer()[i]);

            cout.width(left_column_no + 8);
            cout << "<";
            cout.width(right_column_no + error[k].right_string_length - left_column_no - 1);
            cout.fill('-');
            cout << ">";
            cout.fill(' ');
        }
    }
    else
    {
        cout << "\n\n";
        cout.width(left_column_no + 8);
        cout << "<";

        cout.width(lex_stream -> LineSegmentLength(error[k].left_token));
        cout.fill('-');
        cout << "\n";
        cout.fill(' ');

        cout.width(6);
        cout << left_line_no << ". ";
        for (int i = lex_stream -> LineStart(left_line_no); i <= lex_stream -> LineEnd(left_line_no); i++)
            Unicode::Cout(lex_stream -> InputBuffer()[i]);

        if (right_line_no > left_line_no + 1)
        {
            cout.width(left_column_no + 7);
            cout << " ";
            cout << ". . .\n";
        }

        cout.width(6);
        cout << right_line_no << ". ";
        for (int j = lex_stream -> LineStart(right_line_no); j <= lex_stream -> LineEnd(right_line_no); j++)
            Unicode::Cout(lex_stream -> InputBuffer()[j]);

        cout.width(8);
        cout << "";
        cout.width(right_column_no + error[k].right_string_length - 1);
        cout.fill('-');
        cout << ">";
        cout.fill(' ');
    }
 
    return;
}
 
//
// This procedure is invoked to print a small message that may
// only span a single line. The parameter k points to the error
// message in the error structure.
//
void SemanticError::PrintSmallSource(int k)
{
    int left_line_no = lex_stream -> Line(error[k].left_token);

    if (left_line_no == 0)
        cout << "\n";
    else
    {
        cout << "\n\n";
        cout.width(6);
        cout << left_line_no;
        cout << ". ";

        for (int i = lex_stream -> LineStart(left_line_no); i <= lex_stream -> LineEnd(left_line_no); i++)
            Unicode::Cout(lex_stream -> InputBuffer()[i]);

        int left_column_no = lex_stream -> Column(error[k].left_token),
            right_column_no = lex_stream -> Column(error[k].right_token);

        cout.width(left_column_no + 7);
        cout << "";
        if (left_column_no == right_column_no && error[k].right_string_length == 1)
            cout << '^';
        else
        {
            cout << '<';
            cout.width(right_column_no + error[k].right_string_length - left_column_no - 1);
            cout.fill('-');
            cout << ">";
            cout.fill(' ');
        }
    }
 
    return;
}


//
// These "print_" procedures are invoked to print specific
// error messages. The parameter err identifies the error to
// be processed.
//
void SemanticError::PrintBAD_ERROR(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cerr << "chaos: Error code " << err.msg_code << " is not a valid error message code\n";

    return;
}


void SemanticError::PrintDEFAULT_ERROR(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    if (err.insert1)
        Unicode::Cout(err.insert1);
    if (err.insert2)
        Unicode::Cout(err.insert2);
    if (err.insert3)
        Unicode::Cout(err.insert3);
    if (err.insert4)
        Unicode::Cout(err.insert4);
    if (err.insert5)
        Unicode::Cout(err.insert5);
    if (err.insert6)
        Unicode::Cout(err.insert6);
    if (err.insert7)
        Unicode::Cout(err.insert7);
    if (err.insert8)
        Unicode::Cout(err.insert8);
    if (err.insert9)
        Unicode::Cout(err.insert9);

    return;
}


void SemanticError::PrintINVALID_OPTION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << '\"';
    Unicode::Cout(err.insert1);
    cout << "\" is an invalid option\n"
         << "use: jikes [-g][-O][-debug][-depend][-nowarn][-verbose]"
         << "[-classpath path][-nowrite][-d dir] "
         << "[++][+C][+D][+E][+F][+M][+R][+V]"
         << " file.java ...";

    return;
}


void SemanticError::PrintINVALID_K_OPTION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "No argument specified for +K option. The proper form is \"+Kxxx=xxx\" (with no intervening space)";

    return;
}


void SemanticError::PrintINVALID_K_TARGET(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << '\"';
    Unicode::Cout(err.insert1);
    cout << "\" is not a valid target in a +K option. The target must be a numeric type or boolean";

    return;
}


void SemanticError::PrintINVALID_DIRECTORY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The directory specified in the \"-d\" option, \"";
    Unicode::Cout(err.insert1);
    cout << "\", is either invalid or it could not be expanded";

    return;
}


void SemanticError::PrintUNSUPPORTED_OPTION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "This option is currently unsupported";

    return;
}


void SemanticError::PrintNO_CURRENT_DIRECTORY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Could not open current directory";
    return;
}


void SemanticError::PrintCANNOT_OPEN_ZIP_FILE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "\"";
    Unicode::Cout(err.insert1);
    cout << "\" is either not a valid zip file or it contains a zip file comment. "
            "If it contains a comment, rezip it without the comment...";

    return;
}


void SemanticError::PrintPACKAGE_NOT_FOUND(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Could not find package named: \n";
    for (int i = 1; i < control.classpath.Length(); i++)
    {
        PathSymbol *path_symbol = control.classpath[i];
        wchar_t *path = path_symbol -> Name();

        cout << "                ";
        Unicode::Cout(path);
        if (path_symbol -> IsZip())
        {
            cout << "(";
            Unicode::Cout(err.insert1);
            cout << ")";
        }
        else
        {
            cout << "/";
            Unicode::Cout(err.insert1);
        }

        if (i + 2 < control.classpath.Length())
             cout << ", \n";
        else if (i + 2 == control.classpath.Length())
             cout << " or \n";
    }

    return;
}


void SemanticError::PrintCANNOT_OPEN_DIRECTORY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Unable to open directory \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintBAD_INPUT_FILE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The input file \"";
    Unicode::Cout(err.insert1);
    cout << "\" does not have the \".java\" extension";

    return;
}


void SemanticError::PrintUNREADABLE_INPUT_FILE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The input file \"";
    Unicode::Cout(err.insert1);
    cout << "\" was not found";

    return;
}


void SemanticError::PrintCANNOT_REOPEN_FILE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Unable to reopen file \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintCANNOT_WRITE_FILE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Unable to write file \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintCANNOT_COMPUTE_COLUMNS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Unable to reopen file \"";
    Unicode::Cout(err.insert1);
    cout << "\". Therefore, column positions may be incorrect";

    return;
}


void SemanticError::PrintEMPTY_DECLARATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "An EmptyDeclaration is a deprecated feature that should not be used - \";\" ignored";

    return;
}


void SemanticError::PrintNO_TYPES(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "This compilation unit contains no type declaration";

    return;
}


void SemanticError::PrintTYPE_IN_MULTIPLE_FILES(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The file \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << ".java\" contains type \"";
    Unicode::Cout(err.insert4);
    cout << "\" which conflicts with file \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << ".java\"";

    return;
}


void SemanticError::PrintPACKAGE_TYPE_CONFLICT(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\" contained in file \"";
    Unicode::Cout(err.insert3);
    cout << "\" conflicts with the package \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\"";

    return;
}


void SemanticError::PrintDIRECTORY_FILE_CONFLICT(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type \"";
    Unicode::Cout(err.insert1);
    cout << "\" contained in file \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << ".java\" conflicts with the directory \"";
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintFILE_FILE_CONFLICT(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Cannot write class file \"";
    Unicode::Cout(err.insert1);
    cout << ".class\" because that name conflicts with the name of the class file \"";
    Unicode::Cout(err.insert2);
    cout << "\" in directory \"";
    Unicode::Cout(err.insert3);
    cout << "\". This is illegal because file names are case-insensitive in this system";

    return;
}


void SemanticError::PrintMULTIPLE_PUBLIC_TYPES(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type \"";
    Unicode::Cout(err.insert1);
    cout << "\" is declared public in compilation unit \"";
    Unicode::Cout(lex_stream -> FileName());
    cout<< "\" which also contains the public type, \"";
    Unicode::Cout(err.insert2);
    cout << "\"";

    return;
}


void SemanticError::PrintMISMATCHED_TYPE_AND_FILE_NAMES(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The public type \"";
    Unicode::Cout(err.insert1);
    cout << "\" does not match the name of its containing file \"";
    Unicode::Cout(lex_stream -> FileName());
    cout << "\"";

    return;
}


void SemanticError::PrintREFERENCE_TO_TYPE_IN_MISMATCHED_FILE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type \"";
    Unicode::Cout(err.insert1);
    cout << "\" is defined in the file \"";
    Unicode::Cout(err.insert2);
    cout << ".java\" but referenced in the file \"";
    Unicode::Cout(lex_stream -> FileName());
    cout << "\". It is recommended that it be redefined in \"";
    Unicode::Cout(err.insert1);
    cout << ".java\"";

    return;
}


void SemanticError::PrintDUPLICATE_INNER_TYPE_NAME(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The inner type named \"";
    Unicode::Cout(err.insert1);
    cout << "\" is nested in an outer class of the same name at location ";
    Unicode::Cout(err.insert2);

    return;
}


void SemanticError::PrintDUPLICATE_TYPE_DECLARATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Duplicate declaration of type \"";
    Unicode::Cout(err.insert1);
    cout << "\". The other occurrence is at location ";
    Unicode::Cout(err.insert2);

    return;
}


void SemanticError::PrintUNINITIALIZED_FIELD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The field \"";
    Unicode::Cout(err.insert1);
    cout << "\" is not initialized";

    return;
}


void SemanticError::PrintDUPLICATE_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Duplicate specification of this modifier";

    return;
}


void SemanticError::PrintDUPLICATE_ACCESS_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Duplicate specification of an access modifier. "
            "Only one instance of \"public\", \"private\", or \"protected\" may appear in a declaration";

    return;
}


void SemanticError::PrintINVALID_TOP_LEVEL_CLASS_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is not a valid modifier for a top-level class";

    return;
}


void SemanticError::PrintINVALID_INNER_CLASS_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is not a valid modifier for an inner class";

    return;
}


void SemanticError::PrintINVALID_STATIC_INNER_CLASS_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is not a valid modifier for an inner class that is enclosed in an interface";

    return;
}


void SemanticError::PrintINVALID_LOCAL_CLASS_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is not a valid modifier for a local inner class";

    return;
}


void SemanticError::PrintFINAL_ABSTRACT_CLASS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A class may not be declared both \"final\" and \"abstract\"";

    return;
}


void SemanticError::PrintINVALID_INTERFACE_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is not a valid interface modifier";

    return;
}


void SemanticError::PrintVOLATILE_FINAL(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A \"volatile\" field may not be declared \"final\"";

    return;
}


void SemanticError::PrintFINAL_VOLATILE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A \"final\" field may not be declared \"volatile\"";

    return;
}


void SemanticError::PrintINVALID_FIELD_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is not a valid field modifier";

    return;
}


void SemanticError::PrintINVALID_LOCAL_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is not a valid local variable or parameter modifier";

    return;
}


void SemanticError::PrintINVALID_METHOD_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is not a valid method modifier";

    return;
}


void SemanticError::PrintINVALID_SIGNATURE_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is not a valid signature modifier";

    return;
}


void SemanticError::PrintINVALID_CONSTRUCTOR_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is not a valid constructor modifier";

    return;
}


void SemanticError::PrintINVALID_CONSTANT_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is not a valid constant modifier";

    return;
}


void SemanticError::PrintPARENT_TYPE_IN_UNNAMED_PACKAGE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type associated with this construct is (or depends on) the type ";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << " which is contained in an unnamed package";

    return;
}


void SemanticError::PrintRECOMPILATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type associated with this construct depends on file ";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << ".class which, in turn, depends on file ";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << ".java. All files that depend on this source file, in particular, ";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << ".java should be recompiled";

    return;
}


void SemanticError::PrintTYPE_NOT_FOUND(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Type ";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << " was not found";

    return;
}


void SemanticError::PrintDUPLICATE_ON_DEMAND_IMPORT(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Type ";
    Unicode::Cout(err.insert1);
    cout << " is imported on demand from package ";
    Unicode::Cout(err.insert2);
    cout << " and package ";
    Unicode::Cout(err.insert3);

    return;
}


void SemanticError::PrintNOT_A_TYPE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A type is expected here";

    return;
}


void SemanticError::PrintNOT_A_CLASS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Interface \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\" cannot be used where a class is expected";

    return;
}


void SemanticError::PrintNOT_AN_INTERFACE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Class ";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << " cannot be used where an interface is expected";

    return;
}


void SemanticError::PrintSUPER_IS_FINAL(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The super class \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\" is final. A final class must not have subclasses";

    return;
}


void SemanticError::PrintOBJECT_WITH_SUPER_TYPE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type ";
    Unicode::Cout(err.insert1);
    cout << '/';
    Unicode::Cout(err.insert2);
    cout << " must not have a super type";

    return;
}


void SemanticError::PrintOBJECT_HAS_NO_SUPER_TYPE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type ";
    Unicode::Cout(err.insert1);
    cout << '/';
    Unicode::Cout(err.insert2);
    cout << " does not have a super type";

    return;
}


void SemanticError::PrintDUPLICATE_FIELD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Duplicate declaration of field \"";
    Unicode::Cout(err.insert1);
    cout << "\" in type \"";
    Unicode::Cout(err.insert2);
    cout << "\"";

    return;
}


void SemanticError::PrintDUPLICATE_METHOD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Duplicate declaration of method \"";
    Unicode::Cout(err.insert1);
    cout << "\" in type \"";
    Unicode::Cout(err.insert2);
    cout << "\"";

    return;
}


void SemanticError::PrintMISMATCHED_INHERITED_METHOD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The return type of method \"";
    Unicode::Cout(err.insert1);
    cout << "\" does not match the return type of method \"";
    Unicode::Cout(err.insert2);
    cout << "\" inherited from type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintMISMATCHED_INHERITED_METHOD_EXTERNALLY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "In class \"";
    Unicode::Cout(err.insert1);
    cout << "\", the method \"";
    Unicode::Cout(err.insert2);
    cout << "\", inherited from type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\", does not have the same return type as the overridden method \"";
    Unicode::Cout(err.insert5);
    cout << "\", inherited from type \"";
    if (wcslen(err.insert6) > 0 && wcscmp(err.insert6, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert6);
        cout << "/";
    }
    Unicode::Cout(err.insert7);
    cout << "\"";

    return;
}


void SemanticError::PrintDUPLICATE_CONSTRUCTOR(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Duplicate declaration of this constructor in type \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintMISMATCHED_CONSTRUCTOR_NAME(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The name of the constructor \"";
    Unicode::Cout(err.insert1);
    cout << "\" does not match name of class \"";
    Unicode::Cout(err.insert2);
    cout << "\"";

    return;
}


void SemanticError::PrintMETHOD_WITH_CONSTRUCTOR_NAME(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The name of this method \"";
    Unicode::Cout(err.insert1);
    cout << "\" matches the name of the containing class. "
            "However, the method is not a constructor since it contains a return type";

    return;
}


void SemanticError::PrintDUPLICATE_FORMAL_PARAMETER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Duplicate declaration of formal parameter ";
    Unicode::Cout(err.insert1);

    return;
}


void SemanticError::PrintDUPLICATE_LOCAL_VARIABLE_DECLARATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Duplicate declaration of local variable \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintDUPLICATE_LOCAL_TYPE_DECLARATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Duplicate declaration of local class \"";
    Unicode::Cout(err.insert1);
    cout << "\". The other occurrence is at location ";
    Unicode::Cout(err.insert2);

    return;
}


void SemanticError::PrintMULTIPLE_DEFAULT_LABEL(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Multiple specification of default label in switch statement";

    return;
}


void SemanticError::PrintUNDECLARED_LABEL(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    Unicode::Cout(err.insert1);
    cout << " is an undeclared label";

    return;
}


void SemanticError::PrintDUPLICATE_LABEL(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Duplicate declaration of label \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintTYPE_NOT_THROWABLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\" is not a subclass of \"Throwable\"";

    return;
}


void SemanticError::PrintCATCH_PRIMITIVE_TYPE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A primitive type cannot be used to declare a catch clause parameter - the type Error is assumed";

    return;
}


void SemanticError::PrintCATCH_ARRAY_TYPE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A array type cannot be used to declare a catch clause parameter - the type Error is assumed";

    return;
}


void SemanticError::PrintAMBIGUOUS_NAME(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The name \"";
    Unicode::Cout(err.insert1);
    cout << "\" is an ambiguous name found in the types \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\" and \"";
    if (wcslen(err.insert4) > 0 && wcscmp(err.insert4, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert4);
        cout << "/";
    }
    Unicode::Cout(err.insert5);
    cout << "\"";

    return;
}


void SemanticError::PrintFIELD_IS_TYPE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The name \"";
    Unicode::Cout(err.insert1);
    cout << "\" cannot be dereferenced as it represents a type";

    return;
}


void SemanticError::PrintFIELD_NOT_FOUND(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The field named \"";
    Unicode::Cout(err.insert1);
    cout << "\" was not found";

    return;
}


void SemanticError::PrintFIELD_WITH_PRIVATE_ACCESS_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The field \"";
    Unicode::Cout(err.insert1);
    cout << "\" contained in class \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\" has private access. Therefore, it is not accessible in class \"";
    if (wcslen(err.insert4) > 0 && wcscmp(err.insert4, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert4);
        cout << "/";
    }
    Unicode::Cout(err.insert5);
    cout << "\"";

    return;
}


void SemanticError::PrintFIELD_WITH_DEFAULT_ACCESS_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The field \"";
    Unicode::Cout(err.insert1);
    cout << "\" contained in class \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\" has default access. Therefore, it is not accessible in class \"";
    if (wcslen(err.insert4) > 0 && wcscmp(err.insert4, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert4);
        cout << "/";
    }
    Unicode::Cout(err.insert5);
    cout << "\" which is in a different package";

    return;
}


void SemanticError::PrintNAME_NOT_FOUND(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The name \"";
    Unicode::Cout(err.insert1);
    cout << "\" was not found";

    return;
}


void SemanticError::PrintNAME_NOT_YET_AVAILABLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Illegal use of name \"";
    Unicode::Cout(err.insert1);
    cout << "\" which has not yet been fully declared at this point";

    return;
}


void SemanticError::PrintNAME_NOT_VARIABLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The name \"";
    Unicode::Cout(err.insert1);
    cout << "\" does not denote a valid variable. If \"";
    Unicode::Cout(err.insert1);
    cout << "\" is a method that takes no argument, it must be followed by \"()\"";

    return;
}


void SemanticError::PrintMETHOD_NOT_FOUND(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "No match was found for method \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintMETHOD_WITH_PRIVATE_ACCESS_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Method \"";
    Unicode::Cout(err.insert1);
    cout << "\" in class \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\" has private access. Therefore, it is not accessible in class \"";
    if (wcslen(err.insert4) > 0 && wcscmp(err.insert4, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert4);
        cout << "/";
    }
    Unicode::Cout(err.insert5);
    cout << "\"";

    return;
}


void SemanticError::PrintMETHOD_WITH_DEFAULT_ACCESS_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Method \"";
    Unicode::Cout(err.insert1);
    cout << "\" in class \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\" has default access. Therefore, it is not accessible in class \"";
    if (wcslen(err.insert4) > 0 && wcscmp(err.insert4, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert4);
        cout << "/";
    }
    Unicode::Cout(err.insert5);
    cout << "\" which is in a different package";

    return;
}


void SemanticError::PrintFIELD_NOT_METHOD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The name \"";
    Unicode::Cout(err.insert1);
    cout << "\" is not a method name but the name of a field member of the type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\"";

    return;
}


void SemanticError::PrintTYPE_NOT_METHOD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The keyword \"new\" is expected before this expression as \"";
    Unicode::Cout(err.insert1);
    cout << "\" is not the name of a method but the name of a type";

    return;
}


void SemanticError::PrintTYPE_NOT_FIELD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A type \"";
    Unicode::Cout(err.insert1);
    cout << "\" was found where a field name or method call was expected. Did you mean to write \"";
    Unicode::Cout(err.insert1);
    cout << ".xxx\", or \"new ";
    Unicode::Cout(err.insert1);
    cout << "()\", or ... ?";

    return;
}


void SemanticError::PrintMETHOD_NOT_FIELD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The name \"";
    Unicode::Cout(err.insert1);
    cout << "\" is not a field name but the name of a method declared in the type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\"";

    return;
}


void SemanticError::PrintAMBIGUOUS_CONSTRUCTOR_INVOCATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Ambiguous invocation of constructor \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintAMBIGUOUS_METHOD_INVOCATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Ambiguous invocation of method \"";
    Unicode::Cout(err.insert1);
    cout << "\". At least two methods are accessible from here: Method \"";
    Unicode::Cout(err.insert2);
    cout << "\" declared in type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << '/';
    }
    Unicode::Cout(err.insert4);
    cout << "\" and method \"";
    Unicode::Cout(err.insert5);
    cout << "\" declared in type \"";
    if (wcslen(err.insert6) > 0 && wcscmp(err.insert6, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert6);
        cout << '/';
    }
    Unicode::Cout(err.insert7);
    cout << "\"";

    return;
}


void SemanticError::PrintNAME_NOT_CLASS_VARIABLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The name \"";
    Unicode::Cout(err.insert1);
    cout << "\" does not denote a class (static) variable";

    return;
}


void SemanticError::PrintNOT_A_NUMERIC_VARIABLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Only a variable of numeric type can appear in this context";

    return;
}


void SemanticError::PrintMETHOD_NOT_CLASS_METHOD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The method \"";
    Unicode::Cout(err.insert1);
    cout << "\" does not denote a class method";

    return;
}


void SemanticError::PrintABSTRACT_TYPE_CREATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Attempt to instantiate an abstract class \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintCONSTRUCTOR_NOT_FOUND(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "No match was found for constructor \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
} 


void SemanticError::PrintMETHOD_FOUND_FOR_CONSTRUCTOR(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "No match was found for constructor \"";
    Unicode::Cout(err.insert1);
    cout << "\". However, a method with the same name was found at location ";
    Unicode::Cout(err.insert2);

    return;
} 


void SemanticError::PrintINCOMPATIBLE_TYPE_FOR_INITIALIZATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of the left-hand side (or array type) in this initialization (or array creation expression), \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\", is not compatible with the type of the right-hand side expression, \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << '/';
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintINCOMPATIBLE_TYPE_FOR_ASSIGNMENT(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of the left-hand side in this assignment, \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\", is not compatible with the type of the right-hand side expression, \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << '/';
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintINCOMPATIBLE_TYPE_FOR_CONDITIONAL_EXPRESSION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "In this conditional expression, the type of the false subexpression, \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\", is not compatible with the type of the true subexpression, \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << '/';
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintVOID_TYPE_IN_EQUALITY_EXPRESSION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Subexpressions of type \"void\" may not appear in an EqualityExpression";

    return;
}


void SemanticError::PrintINCOMPATIBLE_TYPE_FOR_BINARY_EXPRESSION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of the left-hand side expression, \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\", is not compatible with the type of the right-hand side expression, \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << '/';
    }
    Unicode::Cout(err.insert4);
    cout << "\" (and vice-versa)";

    return;
}


void SemanticError::PrintINVALID_INSTANCEOF_CONVERSION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of the left-side expression, \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\", cannot possibly be an instance of type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << '/';
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintINVALID_CAST_CONVERSION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "An expression of type \"";
    Unicode::Cout(err.insert1);
    cout << "\" cannot be cast into type \"";
    Unicode::Cout(err.insert2);
    cout << "\"";

    return;
}


void SemanticError::PrintINVALID_CAST_TYPE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Expression found where a type is expected";

    return;
}


void SemanticError::PrintTYPE_NOT_PRIMITIVE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of this expression, \"";
    Unicode::Cout(err.insert1);
    cout << "\", is not a primitive type";

    return;
}


void SemanticError::PrintTYPE_NOT_INTEGRAL(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of this expression,\"";
    Unicode::Cout(err.insert1);
    cout << "\", is not an integral type";

    return;
}


void SemanticError::PrintTYPE_NOT_NUMERIC(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of this expression,\"";
    Unicode::Cout(err.insert1);
    cout << "\", is not numeric";

    return;
}


void SemanticError::PrintTYPE_NOT_INTEGER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of this expression, \"";
    Unicode::Cout(err.insert1);
    cout << "\", cannot be promoted to \"int\" by widening conversion";

    return;
}


void SemanticError::PrintTYPE_NOT_BOOLEAN(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of this expression, \"";
    Unicode::Cout(err.insert1);
    cout << "\", is not boolean";

    return;
}


void SemanticError::PrintTYPE_NOT_ARRAY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of this expression, \"";
    Unicode::Cout(err.insert1);
    cout << "\", is not an array type";

    return;
}


void SemanticError::PrintTYPE_NOT_REFERENCE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of this expression, \"";
    Unicode::Cout(err.insert1);
    cout << "\", is not a valid reference type in this context";

    return;
}


void SemanticError::PrintTYPE_NOT_VALID_FOR_SWITCH(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of a switch statement expression must be either \"int\", \"short\", \"char\" or \"byte\".";
    cout << " The type of this expression is \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\"";

    return;
}


void SemanticError::PrintTYPE_IS_VOID(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "An expression of type \"";
    Unicode::Cout(err.insert1);
    cout << "\" is not valid in this context";

    return;
}


void SemanticError::PrintVALUE_NOT_REPRESENTABLE_IN_TYPE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The value of this expression cannot be represented in type \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintDUPLICATE_CASE_VALUE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The value of this expression, ";
    Unicode::Cout(err.insert1);
    cout << ", has already been used in this switch statement";

    return;
}


void SemanticError::PrintMISPLACED_THIS_EXPRESSION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A \"this\" expression may only be used in the body of an instance method, "
            "constructor (after the explicit constructor invocation, if any), "
            "initializer block, or in the initializer expression of an instance variable";

    return;
}


void SemanticError::PrintMISPLACED_SUPER_EXPRESSION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A \"super\" expression may only appear in the body of a class that has a super class and"
            " it must be enclosed in the body of an instance method or constructor or in the initializer"
            " of an instance variable";

    return;
}


void SemanticError::PrintFINAL_VARIABLE_TARGET_IN_LOOP(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Possible attempt to assign a value to a final variable \"";
    Unicode::Cout(err.insert1);
    cout << "\"";
    cout << ", within the body of a loop that may execute more than once";

    return;
}


void SemanticError::PrintTARGET_VARIABLE_IS_FINAL(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Possible attempt to reassign a value to the final variable \"";
    Unicode::Cout(err.insert1);
    cout << "\"";
    if (err.insert2)
    {
        cout << ". The other assignement was at location ";
        Unicode::Cout(err.insert2);
    }

    return;
}


void SemanticError::PrintUNINITIALIZED_FINAL_VARIABLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A final variable must be initialized";

    return;
}


void SemanticError::PrintUNINITIALIZED_STATIC_FINAL_VARIABLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A blank class final variable must be initialized in a static initializer block. It is assumed to be initialized";

    return;
}


void SemanticError::PrintUNINITIALIZED_FINAL_VARIABLE_IN_CONSTRUCTOR(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The blank final field \"this.";
    cout << err.insert1;
    cout << "\" is not definitely assigned a value in this constructor";

    return;
}


void SemanticError::PrintINIT_SCALAR_WITH_ARRAY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "An array initializer cannot be used to initialize a variable of type \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintINIT_ARRAY_WITH_SCALAR(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A single expression cannot be used to initialize an array variable of type \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintINVALID_BYTE_VALUE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A byte value must be an integer value (note that a character literal is not an integer value) in the range -128..127";

    return;
}


void SemanticError::PrintINVALID_SHORT_VALUE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A short value must be an integer value (note that a character literal is not an integer value) in the range -32768..32767";

    return;
}


void SemanticError::PrintINVALID_CHARACTER_VALUE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A character literal must be a valid unicode value - i.e., a character literal enclosed in single quotes or "
            "an integer value in the range 0..65535 or an escaped 3-digit octal value in the range \\000..\\377";

    return;
}


void SemanticError::PrintINVALID_INT_VALUE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The value of an \"int\" literal must be a decimal value in the range -2147483648..2147483647"
            " or a hexadecimal or octal literal that fits in 32 bits";

    return;
}


void SemanticError::PrintINVALID_LONG_VALUE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The value of a long literal must be a decimal value in the range "
            "-9223372036854775808..9223372036854775807 or a hexadecimal or octal literal that fits in 64 bits";

    return;
}


void SemanticError::PrintINVALID_FLOAT_VALUE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Invalid floating-point constant";

    return;
}


void SemanticError::PrintINVALID_DOUBLE_VALUE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Invalid double constant";

    return;
}


void SemanticError::PrintINVALID_STRING_VALUE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The value of this \"String\" literal is invalid. Perhaps it contains a bad escape sequence?";

    return;
}


void SemanticError::PrintRETURN_STATEMENT_IN_INITIALIZER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A return statement may not appear in an initializer block";

    return;
}


void SemanticError::PrintMISPLACED_RETURN_WITH_EXPRESSION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A return statement with expression must be contained in a method declaration that is "
            "declared to return a value";

    return;
}


void SemanticError::PrintMISPLACED_RETURN_WITH_NO_EXPRESSION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A return statement with no expression may only appear in void method or a constructor";

    return;
}


void SemanticError::PrintMISMATCHED_RETURN_AND_METHOD_TYPE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of this return expression, \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\", does not match the return type of the method, \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << '/';
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintEXPRESSION_NOT_THROWABLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The expression in a throw statement must denote a variable or value of a reference type "
            "which is assignable to the type Throwable";

    return;
}


void SemanticError::PrintBAD_THROWABLE_EXPRESSION_IN_TRY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of the expression in this throw statement, \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\", is not catchable by the enclosing try statement;";
    if (wcslen(err.insert3) > 0)
    {
        cout << " nor is it assignable to an exception in the throws clause of the enclosing method or constructor \"";
        Unicode::Cout(err.insert3);
        cout << "\";";
    }
    cout << " nor is it a subclass of RuntimeException or Error";

    return;
}


void SemanticError::PrintBAD_THROWABLE_EXPRESSION_IN_METHOD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of the expression in this throw statement, \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\", is not assignable to an exception in the throws clause of the enclosing method or constructor \"";
    Unicode::Cout(err.insert3);
    cout << "\"; nor is it a subclass of RuntimeException or Error";

    return;
}


void SemanticError::PrintBAD_THROWABLE_EXPRESSION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type of the expression in this throw statement, \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\", is not a subclass of RuntimeException or Error";

    return;
}


void SemanticError::PrintMISPLACED_BREAK_STATEMENT(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A \"break\" statement must be enclosed in a \"switch\", \"while\", \"do\" or \"for\" statement";

    return;
}


void SemanticError::PrintMISPLACED_CONTINUE_STATEMENT(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A \"continue\" statement must be enclosed in a \"while\", \"do\" or \"for\" statement";

    return;
}


void SemanticError::PrintINVALID_CONTINUE_TARGET(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The statement labeled \"";
    Unicode::Cout(err.insert1);
    cout << "\" cannot be continued since it is not a \"while\", \"do\" or \"for\" statement";

    return;
}


void SemanticError::PrintNON_ABSTRACT_TYPE_CONTAINS_ABSTRACT_METHOD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The abstract method \"";
    Unicode::Cout(err.insert1);
    cout << "\" is enclosed in a class, \"";
    Unicode::Cout(err.insert2);
    cout << "\", that is not abstract";

    return;
}


void SemanticError::PrintNON_ABSTRACT_TYPE_INHERITS_ABSTRACT_METHOD(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The abstract method \"";
    Unicode::Cout(err.insert1);
    cout << "\", inherited from class \"";
    Unicode::Cout(err.insert2);
    cout << "\", is not implemented in the non-abstract type \"";
    Unicode::Cout(err.insert3);
    cout << "\"";

    return;
}


void SemanticError::PrintNO_ABSTRACT_METHOD_IMPLEMENTATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "No implementation of the abstract method \"";
    Unicode::Cout(err.insert1);
    cout << "\" declared in type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << '/';
    }
    Unicode::Cout(err.insert3);
    cout << "\" was found in class \"";
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintDUPLICATE_INTERFACE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Duplicate specification of interface \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\" in definition of type \"";
    Unicode::Cout(err.insert3);
    cout << "\"";

    return;
}


void SemanticError::PrintUNKNOWN_QUALIFIED_NAME_BASE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "\"";
    Unicode::Cout(err.insert1);
    cout << "\" is either a misplaced package name or a non-existent entity.";

    return;
}


void SemanticError::PrintUNKNOWN_AMBIGUOUS_NAME(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "\"";
    Unicode::Cout(err.insert1);
    cout << "\" is either a misplaced package name or a non-existent entity. An expression name is expected in this context";

    return;
}


void SemanticError::PrintCIRCULAR_CLASS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The class \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\" is circularly defined with its super type(s)";

    return;
}


void SemanticError::PrintCIRCULAR_INTERFACE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The interface \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\" is circularly defined with its super type(s)";

    return;
}


void SemanticError::PrintTYPE_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << '/';
    }
    Unicode::Cout(err.insert2);
    cout << "\" with ";
    Unicode::Cout(err.insert3);
    cout << " access is not visible here";

    return;
}


void SemanticError::PrintPRIVATE_FIELD_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The field \"";
    Unicode::Cout(err.insert1);
    cout << "\" in type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << '/';
    }
    Unicode::Cout(err.insert3);
    cout << "\" is private and not accessible here";

    return;
}


void SemanticError::PrintPROTECTED_FIELD_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The field \"";
    Unicode::Cout(err.insert1);
    cout << "\" in type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << '/';
    }
    Unicode::Cout(err.insert3);
    cout << "\" is protected and not accessible here";

    return;
}


void SemanticError::PrintDEFAULT_FIELD_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The field \"";
    Unicode::Cout(err.insert1);
    cout << "\" in type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << '/';
    }
    Unicode::Cout(err.insert3);
    cout << "\" has default access and is not accessible here";

    return;
}


void SemanticError::PrintPRIVATE_METHOD_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Method \"";
    Unicode::Cout(err.insert1);
    cout << "\" in type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << '/';
    }
    Unicode::Cout(err.insert3);
    cout << "\" is private and not accessible here";

    return;
}


void SemanticError::PrintPROTECTED_METHOD_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Method \"";
    Unicode::Cout(err.insert1);
    cout << "\" in type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << '/';
    }
    Unicode::Cout(err.insert3);
    cout << "\" is protected and not accessible here";

    return;
}


void SemanticError::PrintDEFAULT_METHOD_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Method \"";
    Unicode::Cout(err.insert1);
    cout << "\" in type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << '/';
    }
    Unicode::Cout(err.insert3);
    cout << "\" has default access and is not accessible here";

    return;
}


void SemanticError::PrintPRIVATE_CONSTRUCTOR_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The constructor \"";
    Unicode::Cout(err.insert1);
    cout << "\" in type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << '/';
    }
    Unicode::Cout(err.insert3);
    cout << "\" is private. Therefore, it is not accessible here";

    return;
}


void SemanticError::PrintPROTECTED_CONSTRUCTOR_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The constructor \"";
    Unicode::Cout(err.insert1);
    cout << "\" in type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << '/';
    }
    Unicode::Cout(err.insert3);
    cout << "\" is protected. Therefore, it is not accessible here";

    return;
}


void SemanticError::PrintDEFAULT_CONSTRUCTOR_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The constructor \"";
    Unicode::Cout(err.insert1);
    cout << "\" in type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << '/';
    }
    Unicode::Cout(err.insert3);
    cout << "\" has default access. Therefore, it is not accessible here";

    return;
}


void SemanticError::PrintDEFAULT_CONSTRUCTOR_CANNOT_THROW(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "An default constructor cannot be provided for this type because the constructor of its super type can throw \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\"";

    return;
}


void SemanticError::PrintPARAMETER_REDECLARED(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The name of a formal parameter, \"";
    Unicode::Cout(err.insert1);
    cout << "\", may not be used to declare a local variable or an exception parameter";

    return;
}


void SemanticError::PrintBAD_ABSTRACT_METHOD_MODIFIER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A method declaration that contains the keyword \"abstract\" may not also contain one of the keywords: "
            "\"private\", \"static\", \"final\", \"native\" or \"synchronized\"";

    return;
}


void SemanticError::PrintABSTRACT_METHOD_MODIFIER_CONFLICT(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "An abstract method may not also contain the keyword \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintABSTRACT_METHOD_INVOCATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "An abstract method, \"";
    Unicode::Cout(err.insert1);
    cout << "\", cannot be invoked";

    return;
}


void SemanticError::PrintFINAL_METHOD_OVERRIDE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The method \"";
    Unicode::Cout(err.insert1);
    cout << "\" cannot override the final (or private) method \"";
    Unicode::Cout(err.insert2);
    cout << "\" declared in type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintFINAL_METHOD_OVERRIDE_EXTERNALLY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "In class \"";
    Unicode::Cout(err.insert1);
    cout << "\", the method \"";
    Unicode::Cout(err.insert2);
    cout << "\", inherited from type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\", overrides the final (or private) method \"";
    Unicode::Cout(err.insert5);
    cout << "\", inherited from type \"";
    if (wcslen(err.insert6) > 0 && wcscmp(err.insert6, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert6);
        cout << "/";
    }
    Unicode::Cout(err.insert7);
    cout << "\"";

    return;
}


void SemanticError::PrintPRIVATE_METHOD_OVERRIDE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The method \"";
    Unicode::Cout(err.insert1);
    cout << "\" is overriding the private (should be treated as final) method \"";
    Unicode::Cout(err.insert2);
    cout << "\" declared in type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintPRIVATE_METHOD_OVERRIDE_EXTERNALLY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "In class \"";
    Unicode::Cout(err.insert1);
    cout << "\", the method \"";
    Unicode::Cout(err.insert2);
    cout << "\", inherited from type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\", overrides the private (should be treated as final) method \"";
    Unicode::Cout(err.insert5);
    cout << "\", inherited from type \"";
    if (wcslen(err.insert6) > 0 && wcscmp(err.insert6, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert6);
        cout << "/";
    }
    Unicode::Cout(err.insert7);
    cout << "\"";

    return;
}


void SemanticError::PrintCLASS_METHOD_OVERRIDE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The instance method \"";
    Unicode::Cout(err.insert1);
    cout << "\" cannot override the static method \"";
    Unicode::Cout(err.insert2);
    cout << "\" declared in type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintCLASS_METHOD_OVERRIDE_EXTERNALLY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "In class \"";
    Unicode::Cout(err.insert1);
    cout << "\", the instance method \"";
    Unicode::Cout(err.insert2);
    cout << "\", inherited from type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\", cannot override the static method \"";
    Unicode::Cout(err.insert5);
    cout << "\", declared in type \"";
    if (wcslen(err.insert6) > 0 && wcscmp(err.insert6, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert6);
        cout << "/";
    }
    Unicode::Cout(err.insert7);
    cout << "\"";

    return;
}


void SemanticError::PrintINSTANCE_METHOD_OVERRIDE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The static method \"";
    Unicode::Cout(err.insert1);
    cout << "\" cannot hide the instance method \"";
    Unicode::Cout(err.insert2);
    cout << "\" declared in \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintINSTANCE_METHOD_OVERRIDE_EXTERNALLY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "In class \"";
    Unicode::Cout(err.insert1);
    cout << "\", the static method \"";
    Unicode::Cout(err.insert2);
    cout << "\", inherited from type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\", hides the instance method \"";
    Unicode::Cout(err.insert5);
    cout << "\", declared in type \"";
    if (wcslen(err.insert6) > 0 && wcscmp(err.insert6, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert6);
        cout << "/";
    }
    Unicode::Cout(err.insert7);
    cout << "\"";

    return;
}


void SemanticError::PrintBAD_ACCESS_METHOD_OVERRIDE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The method \"";
    Unicode::Cout(err.insert1);
    cout << "\" with ";
    Unicode::Cout(err.insert2);
    cout << " access cannot override the method \"";
    Unicode::Cout(err.insert3);
    cout << "\" with ";
    Unicode::Cout(err.insert4);
    cout << " access declared in type \"";
    if (wcslen(err.insert5) > 0 && wcscmp(err.insert5, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert5);
        cout << "/";
    }
    Unicode::Cout(err.insert6);
    cout << "\"";

    return;
}


void SemanticError::PrintBAD_ACCESS_METHOD_OVERRIDE_EXTERNALLY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "In class \"";
    Unicode::Cout(err.insert1);
    cout << "\", the method \"";
    Unicode::Cout(err.insert2);
    cout << "\" with ";
    Unicode::Cout(err.insert3);
    cout << " access inherited from type \"";
    if (wcslen(err.insert4) > 0 && wcscmp(err.insert4, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert4);
        cout << "/";
    }
    Unicode::Cout(err.insert5);
    cout << "\", cannot override the method \"";
    Unicode::Cout(err.insert6);
    cout << "\" with ";
    Unicode::Cout(err.insert7);
    cout << " access inherited from type \"";
    if (wcslen(err.insert8) > 0 && wcscmp(err.insert8, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert8);
        cout << "/";
    }
    Unicode::Cout(err.insert9);
    cout << "\"";

    return;
}


void SemanticError::PrintMISMATCHED_OVERRIDDEN_EXCEPTION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The exception \"";
    Unicode::Cout(err.insert1);
    cout << "\" is not the same as or a subclass of any exception in the throws clause of the overridden method \"";
    Unicode::Cout(err.insert2);
    cout << "\" declared in type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintMISMATCHED_OVERRIDDEN_EXCEPTION_EXTERNALLY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "In class \"";
    Unicode::Cout(err.insert1);
    cout << "\", the exception \"";
    Unicode::Cout(err.insert2);
    cout << "\" specified in method \"";
    Unicode::Cout(err.insert3);
    cout << "\" inherited from type \"";
    if (wcslen(err.insert4) > 0 && wcscmp(err.insert4, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert4);
        cout << "/";
    }
    Unicode::Cout(err.insert5);
    cout << "\", is not the same as or a subclass of any exception in the throws clause of the overridden method \"";
    Unicode::Cout(err.insert6);
    cout << "\" declared in type \"";
    if (wcslen(err.insert7) > 0 && wcscmp(err.insert7, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert7);
        cout << "/";
    }
    Unicode::Cout(err.insert8);
    cout << "\"";

    return;
}


void SemanticError::PrintABSTRACT_METHOD_WITH_BODY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The declaration of the abstract or native method, \"";
    Unicode::Cout(err.insert1);
    cout << "\", must not contain a method body";

    return;
}


void SemanticError::PrintNON_ABSTRACT_METHOD_WITHOUT_BODY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The declaration of the non-abstract and non-native method, \"";
    Unicode::Cout(err.insert1);
    cout << "\", must contain a method body";

    return;
}


void SemanticError::PrintSTATIC_OVERRIDE_ABSTRACT(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The static method \"";
    Unicode::Cout(err.insert1);
    cout << "\" cannot hide an abstract method of the same name declared in type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\"";

    return;
}


void SemanticError::PrintSTATIC_OVERRIDE_ABSTRACT_EXTERNALLY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "In class \"";
    Unicode::Cout(err.insert1);
    cout << "\", the static method \"";
    Unicode::Cout(err.insert2);
    cout << "\", inherited from type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\", cannot hide the abstract method \"";
    Unicode::Cout(err.insert5);
    cout << "\", inherited from type \"";
    if (wcslen(err.insert6) > 0 && wcscmp(err.insert6, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert6);
        cout << "/";
    }
    Unicode::Cout(err.insert7);
    cout << "\"";

    return;
}


void SemanticError::PrintCIRCULAR_THIS_CALL(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The constructor \"";
    Unicode::Cout(err.insert1);
    cout << "\" may not directly or indirectly invoke itself";

    return;
}


void SemanticError::PrintINSTANCE_VARIABLE_IN_EXPLICIT_CONSTRUCTOR_INVOCATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The instance variable \"";
    Unicode::Cout(err.insert1);
    cout << "\" declared in class \"";
    Unicode::Cout(err.insert2);
    cout << "\" is not accessible in an explicit constructor invocation";

    return;
}


void SemanticError::PrintINSTANCE_METHOD_IN_EXPLICIT_CONSTRUCTOR_INVOCATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The instance method \"";
    Unicode::Cout(err.insert1);
    cout << "\" declared in this class, \"";
    Unicode::Cout(err.insert2);
    cout << "\", or one of its super classes, is not accessible in an explicit constructor invocation";

    return;
}


void SemanticError::PrintTHIS_IN_EXPLICIT_CONSTRUCTOR_INVOCATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The expression \"";
    Unicode::Cout(err.insert1);
    cout << "\" may not be used in an explicit constructor invocation";

    return;
}


void SemanticError::PrintSUPER_IN_EXPLICIT_CONSTRUCTOR_INVOCATION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The expression \"";
    Unicode::Cout(err.insert1);
    cout << "\" may not be used in an explicit constructor invocation";

    return;
}


void SemanticError::PrintEXPRESSION_NOT_CONSTANT(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "A constant expression is expected in this context";

    return;
}


void SemanticError::PrintUNCATCHABLE_METHOD_THROWN_CHECKED_EXCEPTION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The method \"";
    Unicode::Cout(err.insert1);
    cout << "\" can throw the checked exception \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\", but its invocation is neither enclosed in a try statement that can catch "
            "that exception nor in the body of a method or constructor that \"throws\" that exception";

    return;
}


void SemanticError::PrintUNCATCHABLE_CONSTRUCTOR_THROWN_CHECKED_EXCEPTION(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The constructor \"";
    Unicode::Cout(err.insert1);
    cout << "\" can throw the checked exception \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\", but its invocation is neither enclosed in a try statement that can catch "
            "that exception nor in the body of a method or constructor that \"throws\" that exception";

    return;
}


void SemanticError::PrintUNREACHABLE_CATCH_CLAUSE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "This catch block may be unreachable because there is no exception "
            "whose type is assignable to \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\" that can be thrown during execution of the body of the try block";

    return;
}


void SemanticError::PrintUNREACHABLE_STATEMENT(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "This statement is unreachable";

    return;
}


void SemanticError::PrintUNREACHABLE_STATEMENTS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "These statements are unreachable";

    return;
}


void SemanticError::PrintUNREACHABLE_CONSTRUCTOR_BODY(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The body of this constructor is unreachable because the last initializer block in this class does not complete normally";

    return;
}


void SemanticError::PrintBLOCKED_CATCH_CLAUSE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "This catch block is unreachable: the exception \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\" was used in a previous catch clause in this try statement at location ";
    Unicode::Cout(err.insert3);

    return;
}


void SemanticError::PrintVARIABLE_NOT_DEFINITELY_ASSIGNED(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The variable \"";
    Unicode::Cout(err.insert1);
    cout << "\" may be accessed here before having been definitely assigned a value";

    return;
}


void SemanticError::PrintTYPED_METHOD_WITH_NO_RETURN(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The method \"";
    Unicode::Cout(err.insert1);
    cout << "\" must contain a return statement with an expression compatible with type \"";
    Unicode::Cout(err.insert2);
    cout << "\"";

    return;
}


void SemanticError::PrintDEFAULT_METHOD_NOT_OVERRIDDEN(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Method \"";
    Unicode::Cout(err.insert1);
    cout << "\" in class \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\" does not override the corresponding method with default access in class \"";
    if (wcslen(err.insert4) > 0 && wcscmp(err.insert4, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert4);
        cout << "/";
    }
    Unicode::Cout(err.insert5);
    cout << "\"";

    return;
}


void SemanticError::PrintPRIVATE_METHOD_NOT_OVERRIDDEN(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The method \"";
    Unicode::Cout(err.insert1);
    cout << "\" in class \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\" does not override the corresponding method with private access in class \"";
    if (wcslen(err.insert4) > 0 && wcscmp(err.insert4, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert4);
        cout << "/";
    }
    Unicode::Cout(err.insert5);
    cout << "\"";

    return;
}


void SemanticError::PrintTYPE_NOT_IN_UNNAMED_PACKAGE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The file \"";
    Unicode::Cout(err.insert1);
    cout << ".class\" was found in directory \"";
    Unicode::Cout(err.insert2);
    cout << "\" specified in the CLASSPATH. However, that class file specifies a type associated with the named package \"";
    Unicode::Cout(err.insert3);
    cout << "\" instead of the unnamed package";
}


void SemanticError::PrintTYPE_IN_WRONG_PACKAGE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type \"";
    Unicode::Cout(err.insert1);
    cout << "\" was found in package \"";
    Unicode::Cout(err.insert2);
    cout << "\". However, that type is associated with ";
    if (wcslen(err.insert3) == 0)
        cout << "the unnamed package";
    else
    {
        cout << "another named package, \"";
        Unicode::Cout(err.insert3);
        cout << "\"";
    }
}


void SemanticError::PrintTYPE_NAME_MISMATCH(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The name of the type specified, \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\", does not match the name found in the class file: \"";
    Unicode::Cout(err.insert3);
    cout << "\"";
}


void SemanticError::PrintONE_UNNAMED_PACKAGE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type \"";
    Unicode::Cout(err.insert1);
    cout << "\" was found in directory \"";
    Unicode::Cout(err.insert2);
    cout << "\" specified in the CLASSPATH. It is accessible here only because, by default, this compiler uses "
            "one unnamed package. In a compiler that associates an unnamed package with each directory "
            "(as this compiler does with the +P option) this access would be illegal";

    return;
}



void SemanticError::PrintCOMPRESSED_ZIP_FILE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The file ";
    Unicode::Cout(err.insert1);
    cout << "(";
    Unicode::Cout(err.insert2);
    cout << "/";
    Unicode::Cout(err.insert3);
    cout << ") is in an unsupported compressed format. (Unzip and) Rezip \"";
    Unicode::Cout(err.insert1);
    cout << "\"";

    return;
}


void SemanticError::PrintINVALID_CLASS_FILE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The class file \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << ".class\" has an invalid format";

    return;
}


void SemanticError::PrintCANNOT_OPEN_CLASS_FILE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Unable to open file associated with type \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\"";

    return;
}


void SemanticError::PrintONE_ONE_FEATURE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "This is a 1.1 feature";

    return;
}


void SemanticError::PrintSTATIC_NOT_INNER_CLASS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The static class \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\" is not an inner class";

    return;
}


void SemanticError::PrintTYPE_NOT_INNER_CLASS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\", is not an inner class that is immediately enclosed in type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintSUPER_TYPE_NOT_INNER_CLASS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The super type \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\" of this type, \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\", is not an inner class that is immediately enclosed in type \"";
    if (wcslen(err.insert5) > 0 && wcscmp(err.insert5, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert5);
        cout << "/";
    }
    Unicode::Cout(err.insert6);
    cout << "\"";

    return;
}


void SemanticError::PrintSTATIC_FIELD_IN_INNER_CLASS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "An inner class may not contain a static field that is not final";

    return;
}


void SemanticError::PrintSTATIC_METHOD_IN_INNER_CLASS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Inner class may not contain static method";

    return;
}


void SemanticError::PrintSTATIC_TYPE_IN_INNER_CLASS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The static type \"";
    Unicode::Cout(err.insert1);
    cout << "\" is enclosed in an inner class, \"";
    Unicode::Cout(err.insert2);
    cout << "\", located at ";
    Unicode::Cout(err.insert3);

    return;
}


void SemanticError::PrintSTATIC_INITIALIZER_IN_INNER_CLASS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "This static initializer is enclosed in an inner class, \"";
    Unicode::Cout(err.insert1);
    cout << "\", located at ";
    Unicode::Cout(err.insert2);

    return;
}



void SemanticError::PrintINNER_CLASS_REFERENCE_TO_NON_FINAL_LOCAL_VARIABLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Invalid reference in inner class \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\" to a non-final local variable, \"";
    Unicode::Cout(err.insert3);
    cout << "\", declared in method \"";
    Unicode::Cout(err.insert4);
    cout << "\"";

    return;
}


void SemanticError::PrintINHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_LOCAL(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Ambiguous reference to field \"";
    Unicode::Cout(err.insert1);
    cout << "\" declared in (or inherited from) type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\" but also declared in the enclosing method \"";
    Unicode::Cout(err.insert4);
    cout << "\". Explicit qualification is required";

    return;
}

void SemanticError::PrintINHERITANCE_AND_LEXICAL_SCOPING_CONFLICT_WITH_MEMBER(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Ambiguous reference to field \"";
    Unicode::Cout(err.insert1);
    cout << "\" inherited from type \"";
    if (wcslen(err.insert2) > 0 && wcscmp(err.insert2, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert2);
        cout << "/";
    }
    Unicode::Cout(err.insert3);
    cout << "\" but also declared or inherited in the enclosing type \"";
    if (wcslen(err.insert4) > 0 && wcscmp(err.insert4, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert4);
        cout << "/";
    }
    Unicode::Cout(err.insert5);
    cout << "\". Explicit qualification is required";

    return;
}


void SemanticError::PrintILLEGAL_THIS_FIELD_ACCESS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The type \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\" is either not an outer type of type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\" or it is not accessible because this expression appears in a static region";

    return;
}


void SemanticError::PrintENCLOSING_INSTANCE_NOT_ACCESSIBLE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "An instance of \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    Unicode::Cout(StringConstant::US__DO_);
    Unicode::Cout(StringConstant::US_this);
    cout << "\" is not accessible here";
    cout << ". In general, an enclosing instance is accessible only in the body of an instance method, "
            "constructor (after the explicit constructor invocation, if any), "
            "initializer block, or in the initializer expression of an instance variable";
    return;
}


void SemanticError::PrintZERO_DIVIDE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Attempt to divide by zero";
}


void SemanticError::PrintVOID_TO_STRING(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Attempt to convert an expression of type void into a String";
}


void SemanticError::PrintINVALID_ENCLOSING_INSTANCE(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "The super type of this type, \"";
    if (wcslen(err.insert1) > 0 && wcscmp(err.insert1, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert1);
        cout << "/";
    }
    Unicode::Cout(err.insert2);
    cout << "\", is immediately enclosed in type \"";
    if (wcslen(err.insert3) > 0 && wcscmp(err.insert3, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert3);
        cout << "/";
    }
    Unicode::Cout(err.insert4);
    cout << "\" which does not match the type of this primary expression, \"";
    if (wcslen(err.insert5) > 0 && wcscmp(err.insert5, StringConstant::US__DO_) != 0)
    {
        Unicode::Cout(err.insert5);
        cout << "/";
    }
    Unicode::Cout(err.insert6);
    cout << "\"";

    return;
}


void SemanticError::PrintPRIVATE_ENCLOSED_CONSTRUCTOR(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "Access to a private enclosed constructor \"";
    Unicode::Cout(err.insert1);
    cout << "\" is currently unsupported... Awaiting clarification of language spec";

    return;
}


void SemanticError::PrintCONSTRUCTOR_FOUND_IN_ANONYMOUS_CLASS(ErrorInfo &err, LexStream *lex_stream, Control &control)
{
    cout << "An anonymous class cannot have a constructor";

    return;
}
