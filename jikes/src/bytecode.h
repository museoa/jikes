// $Id$
//
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

//
#ifndef bytecode_INCLUDED
#define bytecode_INCLUDED

#include <stdio.h>
#include "tuple.h"
#include "ast.h"
#include "class.h"
#include "option.h"
#include "long.h"
#include "op.h"

class TypeSymbol;
class Control;
class Semantic;

class LabelUse
{
public:
    int use_length, // length of use (2 or 4 bytes)
        op_offset,  // length of use from opcode starting instruction
        use_offset; // offset in code stream of use

    LabelUse() : use_length(0), op_offset(0), use_offset(0) {}

    LabelUse(int _length, int _op_offset, int _use) : 
        use_length(_length), op_offset(_op_offset), use_offset(_use) {}
};


class Label
{
public:
     bool defined;    // boolean, set when value is known
     int definition; // offset of definition point of label
     Tuple<LabelUse> uses;

     Label() : defined(false), definition(0) {}
};


class ByteCode : public ClassFile, public StringConstant, public Operators
{
    Control& this_control;
    Semantic& this_semantic;

    void CompileClass();
    void CompileInterface();

    int class_id,
        line_number,
        last_label_pc,        // pc for last (closest to end) label
        last_op_pc,           // pc of last operation emitted
        last_op_nop,          // set if last operation was NOP.
        this_block_depth,     // depth of current block
        stack_depth,          // current stack depth;
        max_block_depth,
        last_parameter_index; // set to local variable index of last parameter


    Code_attribute *code_attribute; // code for current method ?
    LineNumberTable_attribute *line_number_table_attribute;
    LocalVariableTable_attribute *local_variable_table_attribute;
    InnerClasses_attribute *inner_classes_attribute;

    void MethodInitialization()
    {
        last_label_pc = 0;
        last_op_pc = 0;
        last_op_nop = 0;
        this_block_depth = 0;

        stack_depth = 0;

        return;
    }

    //
    // name_string_null is used to point to the string "null" in the constant
    // pool, in case the null constant is appended to a string. Not that if
    // the string does not yet exist in the pool, is it added.
    //
    u2 name_string_null;
    u2 NameStringNull()
    {
        if (name_string_null == 0)
            name_string_null = BuildString(RegisterUtf8(U8S_null, strlen(U8S_null)));
        return name_string_null;
    }

    Label *begin_labels,
          *break_labels,
          *continue_labels,
          *final_labels,
          *monitor_labels,
          *test_labels;

    int *has_finally_clause;
    bool *is_synchronized;

    BlockSymbol **block_symbols; // block symbols for current block

    int synchronized_blocks, // number of outstanding synchronized blocks
        finally_blocks,      // number of outstanding synchronized blocks
        block_depth;             // need to reset at start of each method
    TypeSymbol *method_type;     // return type of method being compiled

    void AllocateMethodInfo(int max_block_depth_)
    {
        max_block_depth = max_block_depth_;

        begin_labels = new Label[max_block_depth + 1];
        break_labels = new Label[max_block_depth + 1];
        continue_labels = new Label[max_block_depth + 1];
        final_labels = new Label[max_block_depth + 1];
        monitor_labels = new Label[max_block_depth + 1];
        test_labels = new Label[max_block_depth + 1];

        has_finally_clause = new int[max_block_depth + 1];
        is_synchronized = new bool[max_block_depth + 1];
        for (int i = 0; i < max_block_depth; i++)
        {
            has_finally_clause[i] = 0;  // reset has_finally_clause
            is_synchronized[i] = false; // reset is_synchronized
        }

        block_symbols = new BlockSymbol *[max_block_depth+1];

        return;
    }

    void FreeMethodInfo()
    {
        delete [] begin_labels;
        delete [] break_labels;
        delete [] continue_labels;
        delete [] final_labels;
        delete [] monitor_labels;
        delete [] test_labels;

        delete [] has_finally_clause;
        delete [] is_synchronized;

        delete [] block_symbols;

        return;
    }

    void    ProcessAbruptExit(int);
    void    CompleteLabel(Label &lab);
    void    DefineLabel(Label &lab);
    void    UseLabel(Label &lab, int length, int op_offset);

    bool IsLabelUsed(Label &lab)
    {
        return (lab.uses.Length() > 0);
    }


    //
    // methods to determine type
    //
    bool IsDefaultValue(AstExpression *);

    //
    //  Methods to query attributes
    // return true if p refers to local variable, false otherwise
    //
    bool IsLocal(AstExpression *p)
    {
        VariableSymbol *sym = p -> symbol -> VariableCast();
        return (sym && sym -> owner -> MethodCast());
    }


    //
    // see if operand is null. The front-end will have inserted a cast
    // of null to the present type
    //
    bool IsNull(AstExpression *p)
    {
        return (p -> CastExpressionCast() ? (p -> CastExpressionCast() -> expression -> Type() == this_control.null_type) : false);
    }   


    //
    // Does p refer to a non-null reference type?
    //
    bool IsReferenceType(TypeSymbol *p)
    {
        return (! (p -> Primitive() || p == this_control.null_type));
    }


    //
    // see if operand is integer type and is zero
    //
    bool IsZero(AstExpression *p)
    {
        if (p -> IsConstant() && (p -> Type() == this_control.int_type || p -> Type() == this_control.boolean_type))
        {
            IntLiteralValue *vp = (IntLiteralValue *) (p -> value);
            return (vp -> value == 0);
        }

        return false;
    }


    //
    // memory access: reference either
    // constant (literal)
    // name (includes local varable, or class variable, or field access)
    // array
    //
    enum
    {
        LHS_LOCAL =  0, // local variable
        LHS_ARRAY =  1, // array (of any kind)
        LHS_FIELD =  2, // instance variable
        LHS_STATIC = 3, // class variable
        LHS_METHOD = 4 // access to private variable
    };

    int GetLHSKind(AstExpression *expression)
    {
        AstAssignmentExpression *assignment = expression -> AssignmentExpressionCast();
        AstPreUnaryExpression *pre = expression -> PreUnaryExpressionCast();
        AstPostUnaryExpression *post = expression -> PostUnaryExpressionCast();

        AstExpression *lhs = assignment ? (assignment -> write_method ? (AstExpression *) NULL : assignment -> left_hand_side)
                                        : pre ? (pre -> write_method ? (AstExpression *) NULL : pre -> expression)
                                              : post ? (post -> write_method ? (AstExpression *) NULL : post -> expression)
                                                     : expression;

        //
        // Find symbol that is associated with expression. If the subexpression has
        // to be referenced via an access method then the symbol is null
        //
        AstCastExpression *cast = (lhs ? lhs -> CastExpressionCast() : (AstCastExpression *) NULL);
        Symbol *sym = cast ? cast -> expression -> symbol : (lhs ? lhs -> symbol : (Symbol *) NULL);

        //
        // If the expression associated with the left-hand side is null,
        // then we have an access method.
        // Otherwise, a left-hand side is either an array access,
        // a field access or a name. In the case of a FieldAccess
        // or name, the left-hand side is resolved into a variable.
        // In the case of an array access, it is resolved into a type.
        // 
        VariableSymbol *var = (sym ? sym -> VariableCast() : (VariableSymbol *) NULL);
        return ((! lhs) ? LHS_METHOD
                        : (! var) ? LHS_ARRAY
                                  : var -> owner -> MethodCast() ? LHS_LOCAL
                                                                 : var -> ACC_STATIC() ? LHS_STATIC
                                                                                       : LHS_FIELD);
    }


    int GetTypeWords(TypeSymbol *type)
    {
        return this_control.IsDoubleWordType(type) ? 2 : 1;
    }


    //
    // methods to load and store values
    //
    void LoadLocal(int varno, TypeSymbol *);
    void StoreLocalVariable(VariableSymbol *);
    void StoreLocal(int varno, TypeSymbol *);
    int  GetConstant(LiteralValue *, TypeSymbol *);
    int  LoadLiteral(LiteralValue *, TypeSymbol *);
    void LoadReference(AstExpression *);
    void LoadShort(int val);
    void LoadInteger(int val);
    int  LoadSimple(int, AstExpression *);
    int  LoadArrayElement(TypeSymbol *);
    void StoreArrayElement(TypeSymbol *);
    void StoreField(AstExpression *);
    void StoreSimple(int, AstExpression *);

    //
    // here to load a constant when the LiteralValue is set.
    //
    int LoadConstant(AstExpression *p)
    {
        assert (p -> IsConstant());

        return LoadLiteral(p -> value, p -> Type());
    }

    //
    // These methods build entries in the constant pool.
    //
    u2 BuildDouble(IEEEdouble d)
    {
        int index = constant_pool.NextIndex();
        constant_pool[index] = new CONSTANT_Double_info(CONSTANT_Double, d.HighWord(), d.LowWord());
        constant_pool.Next() = 0; // extra slop for double-word entry

        return index;
    }


    u2 BuildFieldref(u2 cl_index, u2 nt_index)
    {
        int index = constant_pool.NextIndex();
        constant_pool[index] = new CONSTANT_Fieldref_info(CONSTANT_Fieldref, cl_index, nt_index);

        return index;
    }


    u2 BuildFloat(IEEEfloat val)
    {
        int index = constant_pool.NextIndex();
        constant_pool[index] = new CONSTANT_Float_info(CONSTANT_Float, val.Word());

        return index;
    }


    u2 BuildInteger(int val)
    {
        int index = constant_pool.NextIndex();
        u4 bytes = (((unsigned) (val >> 24)) << 24) | ((val >> 16 & 0xff) << 16) | ((val >> 8 & 0xff) ) << 8 | (val & 0xff);
        constant_pool[index] = new CONSTANT_Integer_info(CONSTANT_Integer, bytes);

        return index;
    }


    u2 BuildInterfaceMethodref(u2 cl_index, u2 nt_index)
    {
        int index = constant_pool.NextIndex();
        constant_pool[index] = new CONSTANT_InterfaceMethodref_info(CONSTANT_InterfaceMethodref, cl_index, nt_index);

        return index;
    }


    u2  BuildLong(LongInt  val)
    {
        int index = constant_pool.NextIndex();
        constant_pool[index] = new CONSTANT_Long_info(CONSTANT_Long, val.HighWord(), val.LowWord());
        constant_pool.Next() = 0; // extra slop for double-word entry

        return index;
    }


    u2 BuildMethodref(u2 cl_index, u2 nt_index)
    {
        int index = constant_pool.NextIndex();
        constant_pool[index] = new CONSTANT_Methodref_info(CONSTANT_Methodref, cl_index, nt_index);

        return index;
    }


    u2 BuildNameAndType(u2 name, u2 type)
    {
        int index = constant_pool.NextIndex();
        constant_pool[index] = new CONSTANT_NameAndType_info(CONSTANT_NameAndType, name, type);

        return index;
    }


    u2 BuildString(u2 si)
    {
        int index = constant_pool.NextIndex();
        constant_pool[index] = new CONSTANT_String_info(CONSTANT_String, si);

        return index;
    }


    u2 BuildUtf8(char *s, int len)
    {
        int index = constant_pool.NextIndex();
        constant_pool[index] = new CONSTANT_Utf8_info(CONSTANT_Utf8, s, len);

        return index;
    }


    //
    // unlike most methods, which always build a new entry, the
    // 'register' methods only build a new entry for a literal if one has net
    // yet been built.
    //
    int method_clone_index,
        method_clone_getmessage_index,
        method_clone_init_index,
        method_stringbuffer_tostring_index,
        method_stringbuffer_init_index,
        method_stringbuffer_appendchararray_index,
        method_stringbuffer_appendchar_index,
        method_stringbuffer_appendboolean_index,
        method_stringbuffer_appendint_index,
        method_stringbuffer_appendlong_index,
        method_stringbuffer_appendfloat_index,
        method_stringbuffer_appenddouble_index,
        method_stringbuffer_appendstring_index,
        method_stringbuffer_appendobject_index;

    u2 RegisterMethodClone()
    {
        if (method_clone_index == 0)
        {
            method_clone_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_Object, U8S_java_SL_lang_SL_Object_length),
                               BuildNameAndType(RegisterUtf8(U8S_clone, U8S_clone_length),
                                                RegisterUtf8(U8S_LP_RP_Ljava_SL_lang_SL_Object_SC,
                                                             U8S_LP_RP_Ljava_SL_lang_SL_Object_SC_length)));
        }

        return method_clone_index;
    }


    u2 RegisterMethodCloneGetmessage()
    {
        if (method_clone_getmessage_index == 0)
        {
            method_clone_getmessage_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_Throwable, U8S_java_SL_lang_SL_Throwable_length),
                               BuildNameAndType(RegisterUtf8(U8S_getMessage, U8S_getMessage_length),
                                                RegisterUtf8(U8S_LP_RP_Ljava_SL_lang_SL_String_SC,
                                                             U8S_LP_RP_Ljava_SL_lang_SL_String_SC_length)));
        }

        return method_clone_getmessage_index;
    }


    u2 RegisterMethodCloneInit()
    {
        if (method_clone_init_index == 0)
        {
            method_clone_init_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_InternalError, U8S_java_SL_lang_SL_InternalError_length),
                               BuildNameAndType(RegisterUtf8(U8S_LT_init_GT, U8S_LT_init_GT_length),
                                                RegisterUtf8(U8S_LP_Ljava_SL_lang_SL_String_SC_RP_V,
                                                             U8S_LP_Ljava_SL_lang_SL_String_SC_RP_V_length)));
        }

        return method_clone_init_index;
    }


    u2 RegisterMethodStringbufferTostring()
    {
        if (method_stringbuffer_tostring_index == 0)
        {
            method_stringbuffer_tostring_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, U8S_java_SL_lang_SL_StringBuffer_length),
                               BuildNameAndType(RegisterUtf8(U8S_toString, U8S_toString_length),
                                                RegisterUtf8(U8S_LP_RP_Ljava_SL_lang_SL_String_SC,
                                                             U8S_LP_RP_Ljava_SL_lang_SL_String_SC_length)));
        }

        return method_stringbuffer_tostring_index;
    }


    u2 RegisterMethodStringbufferInit()
    {
        if (method_stringbuffer_init_index == 0)
        {
            method_stringbuffer_init_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, U8S_java_SL_lang_SL_StringBuffer_length),
                               BuildNameAndType(RegisterUtf8(U8S_LT_init_GT, U8S_LT_init_GT_length),
                                                RegisterUtf8(U8S_LP_RP_V, U8S_LP_RP_V_length)));
        }

        return method_stringbuffer_init_index;
    }


    u2 RegisterMethodStringbufferAppendchararray()
    {
        if (method_stringbuffer_appendchararray_index == 0)
        {
            method_stringbuffer_appendchararray_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, U8S_java_SL_lang_SL_StringBuffer_length),
                               BuildNameAndType(RegisterUtf8(U8S_append, U8S_append_length),
                                                RegisterUtf8(U8S_LP_LB_C_RP_Ljava_SL_lang_SL_StringBuffer_SC,
                                                             U8S_LP_LB_C_RP_Ljava_SL_lang_SL_StringBuffer_SC_length)));
        }

        return method_stringbuffer_appendchararray_index;
    }


    u2 RegisterMethodStringbufferAppendchar()
    {
        if (method_stringbuffer_appendchar_index == 0)
        {
            method_stringbuffer_appendchar_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, U8S_java_SL_lang_SL_StringBuffer_length),
                               BuildNameAndType(RegisterUtf8(U8S_append, U8S_append_length),
                                                RegisterUtf8(U8S_LP_C_RP_Ljava_SL_lang_SL_StringBuffer_SC,
                                                             U8S_LP_C_RP_Ljava_SL_lang_SL_StringBuffer_SC_length)));
        }

        return method_stringbuffer_appendchar_index;
    }


    u2 RegisterMethodStringbufferAppendboolean()
    {
        if (method_stringbuffer_appendboolean_index == 0)
        {
            method_stringbuffer_appendboolean_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, U8S_java_SL_lang_SL_StringBuffer_length),
                               BuildNameAndType(RegisterUtf8(U8S_append, U8S_append_length),
                                                RegisterUtf8(U8S_LP_Z_RP_Ljava_SL_lang_SL_StringBuffer_SC,
                                                             U8S_LP_Z_RP_Ljava_SL_lang_SL_StringBuffer_SC_length)));
        }

        return method_stringbuffer_appendboolean_index;
    }


    u2 RegisterMethodStringbufferAppendint()
    {
        if (method_stringbuffer_appendint_index == 0)
        {
            method_stringbuffer_appendint_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, U8S_java_SL_lang_SL_StringBuffer_length),
                               BuildNameAndType(RegisterUtf8(U8S_append, U8S_append_length),
                                                RegisterUtf8(U8S_LP_I_RP_Ljava_SL_lang_SL_StringBuffer_SC,
                                                             U8S_LP_I_RP_Ljava_SL_lang_SL_StringBuffer_SC_length)));
        }

        return method_stringbuffer_appendint_index;
    }


    u2 RegisterMethodStringbufferAppendlong()
    {
        if (method_stringbuffer_appendlong_index == 0)
        {
            method_stringbuffer_appendlong_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, U8S_java_SL_lang_SL_StringBuffer_length),
                               BuildNameAndType(RegisterUtf8(U8S_append, U8S_append_length),
                                                RegisterUtf8(U8S_LP_J_RP_Ljava_SL_lang_SL_StringBuffer_SC,
                                                             U8S_LP_J_RP_Ljava_SL_lang_SL_StringBuffer_SC_length)));
        }

        return method_stringbuffer_appendlong_index;
    }


    u2 RegisterMethodStringbufferAppendfloat()
    {
        if (method_stringbuffer_appendfloat_index == 0)
        {
            method_stringbuffer_appendfloat_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, U8S_java_SL_lang_SL_StringBuffer_length),
                               BuildNameAndType(RegisterUtf8(U8S_append, U8S_append_length),
                                                RegisterUtf8(U8S_LP_F_RP_Ljava_SL_lang_SL_StringBuffer_SC,
                                                             U8S_LP_F_RP_Ljava_SL_lang_SL_StringBuffer_SC_length)));
        }

        return method_stringbuffer_appendfloat_index;
    }


    u2 RegisterMethodStringbufferAppenddouble()
    {
        if (method_stringbuffer_appenddouble_index == 0)
        {
            method_stringbuffer_appenddouble_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, U8S_java_SL_lang_SL_StringBuffer_length),
                               BuildNameAndType(RegisterUtf8(U8S_append, U8S_append_length),
                                                RegisterUtf8(U8S_LP_D_RP_Ljava_SL_lang_SL_StringBuffer_SC,
                                                             U8S_LP_D_RP_Ljava_SL_lang_SL_StringBuffer_SC_length)));
        }

        return method_stringbuffer_appenddouble_index;
    }


    u2 RegisterMethodStringbufferAppendstring()
    {
        if (method_stringbuffer_appendstring_index == 0)
        {
            method_stringbuffer_appendstring_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, U8S_java_SL_lang_SL_StringBuffer_length),
                               BuildNameAndType(RegisterUtf8(U8S_append, U8S_append_length),
                                                RegisterUtf8(U8S_LP_Ljava_SL_lang_SL_String_SC_RP_Ljava_SL_lang_SL_StringBuffer_SC,
                                                             U8S_LP_Ljava_SL_lang_SL_String_SC_RP_Ljava_SL_lang_SL_StringBuffer_SC_length)));
        }

        return method_stringbuffer_appendstring_index;
    }


    u2 RegisterMethodStringbufferAppendobject()
    {
        if (method_stringbuffer_appendobject_index == 0)
        {
            method_stringbuffer_appendobject_index =
                BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, U8S_java_SL_lang_SL_StringBuffer_length),
                               BuildNameAndType(RegisterUtf8(U8S_append, U8S_append_length),
                                                RegisterUtf8(U8S_LP_Ljava_SL_lang_SL_Object_SC_RP_Ljava_SL_lang_SL_StringBuffer_SC,
                                                             U8S_LP_Ljava_SL_lang_SL_Object_SC_RP_Ljava_SL_lang_SL_StringBuffer_SC_length)));
        }

        return method_stringbuffer_appendobject_index;
    }


    u2 RegisterClass(Utf8LiteralValue *lit)
    {
        assert((lit != NULL) && "null argument to RegisterClass");

        if (lit -> constant_pool_class != class_id) // kill values assigned in prior class
        {
            lit -> constant_pool_index_Class = 0;
            lit -> constant_pool_index_String = 0;
            lit -> constant_pool_index = 0;
        }

        if (lit -> constant_pool_index_Class == 0)
        {
            lit -> constant_pool_class = class_id;
            lit -> constant_pool_index_Class = constant_pool.NextIndex();
            constant_pool[lit -> constant_pool_index_Class] = new CONSTANT_Class_info(CONSTANT_Class, RegisterUtf8(lit));
        }

        return lit -> constant_pool_index_Class;
    }


    u2 RegisterClass(char *str, int len)
    {
        return RegisterClass(this_control.Utf8_pool.FindOrInsert(str, len));
    }

    u2 RegisterDouble(DoubleLiteralValue *lit)
    {
        assert((lit != NULL) && "null argument to RegisterDouble");

        if (lit -> constant_pool_index == 0 || lit -> constant_pool_class != class_id)
        {
            lit -> constant_pool_index = BuildDouble(lit -> value);
            lit -> constant_pool_class = class_id;
        }

        return lit -> constant_pool_index;
    }


    u2 RegisterInteger(IntLiteralValue *lit)
    {
        assert((lit != NULL) && "null argument to RegisterInteger");

        if (lit -> constant_pool_index == 0 || lit -> constant_pool_class != class_id)
        {
            lit -> constant_pool_index = BuildInteger(lit -> value);
            lit -> constant_pool_class = class_id;
        }

        return lit -> constant_pool_index;
    }


    u2 RegisterLong(LongLiteralValue *lit)
    {
        assert((lit != NULL) && "null argument to RegisterLong");

        if (lit -> constant_pool_index == 0|| lit -> constant_pool_class != class_id)
        {
            lit -> constant_pool_index = BuildLong(lit -> value);
            lit -> constant_pool_class = class_id;
        }

        return lit -> constant_pool_index;
    }


    u2 RegisterFloat(FloatLiteralValue *lit)
    {
        assert((lit != NULL) && "null argument to RegisterFloat");

        if (lit -> constant_pool_index == 0 || lit -> constant_pool_class != class_id)
        {
            lit -> constant_pool_index = BuildFloat(lit -> value);
            lit -> constant_pool_class = class_id;
        }

        return lit -> constant_pool_index;
    }


    u2 RegisterString(Utf8LiteralValue *lit)
    {
        assert((lit != NULL) && "null argument to RegisterString");

        if (lit -> constant_pool_class != class_id) // kill values assigned in prior class
        {
            lit -> constant_pool_index_Class = 0;
            lit -> constant_pool_index_String = 0;
            lit -> constant_pool_index = 0;
        }

        if (lit -> constant_pool_index_String == 0)
            lit -> constant_pool_index_String = BuildString(RegisterUtf8(lit));

        return lit -> constant_pool_index_String;
    }


    u2 RegisterUtf8(Utf8LiteralValue *lit)
    {
        assert((lit != NULL) && "null argument to RegisterUtf8");

        if (lit -> constant_pool_class != class_id) // kill values assigned in prior class
        {
            lit -> constant_pool_index_Class = 0;
            lit -> constant_pool_index_String = 0;
            lit -> constant_pool_index = 0;
        }

        if (lit -> constant_pool_index  == 0)
        {
            lit -> constant_pool_index = BuildUtf8(lit -> value,lit -> length);
            lit -> constant_pool_class = class_id;
        }

        return lit -> constant_pool_index;
    }


    u2 RegisterUtf8(char *str, int len)
    {
        return RegisterUtf8(this_control.Utf8_pool.FindOrInsert(str,len));
    }


    //
    //  Methods to write out the byte code
    //
    Deprecated_attribute *CreateDeprecatedAttribute()
    {
        return new Deprecated_attribute(RegisterUtf8(U8S_Deprecated, strlen(U8S_Deprecated)));
    }
    

    Synthetic_attribute *CreateSyntheticAttribute()
    {
        return new Synthetic_attribute(RegisterUtf8(U8S_Synthetic, strlen(U8S_Synthetic)));
    }
    

    //
    // Methods to generate expressions.
    //
    int  EmitExpression(AstExpression *);
    int  EmitArrayCreationExpression(AstArrayCreationExpression *);
    int  EmitAssignmentExpression(AstAssignmentExpression *, bool);
    int  EmitBinaryExpression(AstBinaryExpression *);
    int  EmitCastExpression(AstCastExpression *);
    void EmitCast(TypeSymbol *, TypeSymbol *);
    int  EmitClassInstanceCreationExpression(AstClassInstanceCreationExpression *, bool);
    int  EmitConditionalExpression(AstConditionalExpression *);
    int  EmitFieldAccess(AstFieldAccess *);
    void EmitFieldAccessLHSBase(AstExpression *);
    void EmitMethodInvocation(AstMethodInvocation *);
    void EmitNewArray(int, TypeSymbol *);
    void EmitCloneArray(AstMethodInvocation *);
    int  EmitPostUnaryExpression(AstPostUnaryExpression *, bool);
    void EmitPostUnaryExpressionArray(AstPostUnaryExpression *, bool);
    void EmitPostUnaryExpressionField(int, AstPostUnaryExpression *, bool);
    void EmitPostUnaryExpressionSimple(int, AstPostUnaryExpression *, bool);
    int  EmitPreUnaryExpression(AstPreUnaryExpression *, bool);
    void EmitPreUnaryIncrementExpression(AstPreUnaryExpression *expression, bool);
    void EmitPreUnaryIncrementExpressionArray(AstPreUnaryExpression *expression, bool);
    void EmitPreUnaryIncrementExpressionField(int, AstPreUnaryExpression *expression, bool);
    void EmitPreUnaryIncrementExpressionSimple(int, AstPreUnaryExpression *expression, bool);
    void EmitThisInvocation(AstThisCall *);
    void EmitSuperInvocation(AstSuperCall *);
    void ConcatenateString(AstBinaryExpression *);
    void EmitStringBuffer();
    void EmitCallStringToString();
    void AppendString(AstExpression *);
    void EmitStringAppendMethod(TypeSymbol *);
    void GenerateAccessMethod(MethodSymbol *);
    int  GenerateFieldReference(VariableSymbol *);
    void ChangeStack (int);
    void ResolveAccess(AstExpression *);
    int  GenerateClassAccess(AstFieldAccess *);
    void GenerateClassAccessMethod(MethodSymbol *);

    //
    // Methods to process statements
    //
    void CompileConstructor(AstConstructorDeclaration *, Tuple<AstVariableDeclarator *> &);

    void BeginMethod(int, MethodSymbol *);
    void EndMethod(int, MethodSymbol *);
    void DeclareField(VariableSymbol *);
    void InitializeClassVariable(AstVariableDeclarator *);
    void InitializeInstanceVariable(AstVariableDeclarator *);
    void InitializeArray(TypeSymbol *, AstArrayInitializer *);
    void DeclareLocalVariable(AstVariableDeclarator *);
    void EmitStatement(AstStatement *);
    void EmitReturnStatement(AstReturnStatement *);
    void EmitSynchronizedStatement(AstSynchronizedStatement *);
    void EmitBlockStatement(AstBlock *, bool);
    void EmitStatementExpression(AstExpression *);
    void EmitSwitchStatement(AstSwitchStatement *);
    void UpdateBlockInfo(BlockSymbol *);
    void EmitTryStatement(AstTryStatement *);
    void EmitBranchIfExpression(AstExpression *, bool, Label &);
    void CompleteCall(MethodSymbol *, int);


    //
    // called when expression has been parenthesized to removed
    // parantheses and expose true structure.
    //
    AstExpression *UnParenthesize(AstExpression *expr)
    {
        while(expr -> ParenthesizedExpressionCast())
            expr = expr -> ParenthesizedExpressionCast() -> expression;

        return expr;
    }


    void EmitArrayAccessLHS(AstArrayAccess *expression)
    {
        LoadReference(expression -> base);
        EmitExpression(expression -> expression);

        return;
    }


    int EmitArrayAccessRHS(AstArrayAccess *expression) 
    {
        EmitArrayAccessLHS(expression); // get array address and index
        return LoadArrayElement(expression -> Type());
    }


    void EmitBranch(unsigned int opc, Label& lab)
    {
        PutOp(opc);
        UseLabel(lab, 2, 1);

        return;
    }


    void EmitFieldAccessLHS(AstExpression *expression)
    {
        EmitFieldAccessLHSBase(expression);
        PutOp(OP_DUP);     // save base address of field for later store
        PutOp(OP_GETFIELD);
        ChangeStack(this_control.IsDoubleWordType(expression -> Type()) ? 1 : 0);
        PutU2(GenerateFieldReference((VariableSymbol *) expression -> symbol));

        return;
    }


    void GenerateReturn(TypeSymbol *type)
    {
        PutOp(this_control.IsSimpleIntegerValueType(type) || type == this_control.boolean_type
                  ? OP_IRETURN
                  : type == this_control.long_type
                          ? OP_LRETURN
                          : type == this_control.float_type
                                  ? OP_FRETURN
                                  : type == this_control.double_type
                                          ? OP_DRETURN
                                          : OP_ARETURN);
        return;
    }


#ifdef TEST
    void PrintCode();
#endif

    void PutOp(unsigned char opc);

    //
    //  Methods to insert values into byte code
    //
    void PutI1(i1 i)
    {
        code_attribute -> AddCode(i & 0xff);

        return;
    }


    void PutI2(i2 i)
    {
        code_attribute -> AddCode((i >> 8) & 0xff);
        code_attribute -> AddCode(i & 0xff);

        return;
    }


    void PutU1(u1 u)
    {
        code_attribute -> AddCode(u & 0xff);

        return;
    }


    void PutU2(u2 u)
    {
        code_attribute -> AddCode((u >> 8) & 0xff);
        code_attribute -> AddCode(u & 0xff);

        return;
    }


    void PutU4(u4 u)
    {
        code_attribute -> AddCode((u >> 24));
        code_attribute -> AddCode((u >> 16) & 0xff);
        code_attribute -> AddCode((u >>  8) & 0xff);
        code_attribute -> AddCode(u & 0xff);

        return;
    }


    //
    // emit NOP. The NOP can be replaced by the next instruction if 
    // optional is set; otherwise it must be kept.
    //
    void PutNop(int optional)
    {
        PutOp(OP_NOP);

        //
        // this optimization is causing more trouble than it's worth.
        // latest problem (27 jan 97) was reported by Derek, in that
        // nop just before label definition, resulted in operation generated
        // after label def. being moved before the def! Since it's such a sin
        // to generate junk code, disable the "nop" optimization.
        //  if (optional) last_op_nop = 1;
        //

        return;
    }

    void FinishCode(TypeSymbol *);

    void Reset()
    {
        constant_pool.Reset();
        fields.Reset();
        methods.Reset();
        attributes.Reset();
        this_class = super_class = 0;
    }

public:
    ByteCode(TypeSymbol *);

    inline void GenerateCode()
    {
        if (unit_type -> ACC_INTERFACE())
             CompileInterface();
        else CompileClass();
    }
};

#endif
