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
    int use_length; // length of use (2 or 4 bytes)
    int op_offset;  // length of use from opcode starting instruction
    int use_offset; // offset in code stream of use

    LabelUse() : use_length(0),op_offset(0), use_offset(0) {}

    LabelUse(int _length, int _op_offset, int _use) : 
        use_length(_length), op_offset(_op_offset), use_offset(_use) {}
};

class Label
{
public:
     int defined; // boolean, set when value is known
     int definition; // offset of definition point of label
     Tuple<LabelUse> uses;

     Label() : defined(0), definition(0) {}
};

class ByteCode : public ClassFile, public StringConstant
{
public:
    Control& this_control;
    Semantic& this_semantic;
    int class_id;

    //
    //
    //
    enum 
    {
        METHOD_CLONE = 0,
        METHOD_CLONE_GETMESSAGE = 1,
        METHOD_CLONE_INIT = 2,
        METHOD_STRINGBUFFER_TOSTRING = 3,
        METHOD_STRINGBUFFER_INIT = 4,
        METHOD_STRINGBUFFER_APPENDCHARARRAY = 5,
        METHOD_STRINGBUFFER_APPENDCHAR = 6,
        METHOD_STRINGBUFFER_APPENDBOOLEAN = 7,
        METHOD_STRINGBUFFER_APPENDINT = 8,
        METHOD_STRINGBUFFER_APPENDLONG = 9,
        METHOD_STRINGBUFFER_APPENDFLOAT = 10,
        METHOD_STRINGBUFFER_APPENDDOUBLE = 11,
        METHOD_STRINGBUFFER_APPENDSTRING = 12,
        METHOD_STRINGBUFFER_APPENDOBJECT = 13,
        METHOD_NUMBER = 14
    };
    
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

    //
    //
    //
    enum
    {
        METHOD_KIND_ORDINARY = 0,
        METHOD_KIND_CONSTRUCTOR = 1,
        METHOD_KIND_GENERATED_CONSTRUCTOR = 2,
        METHOD_KIND_INTERFACE = 3,
        METHOD_KIND_ACCESS = 4,
        METHOD_KIND_ACCESS_CLASS = 5
    };

    int line_number;
    int last_label_pc;    // pc for last (closest to end) label
    int last_op_pc;       // pc of last operation emitted
    int last_op_nop;      // set if last operation was NOP.
    int this_block_depth; // depth of current block

    Code_attribute *code_attribute; // code for current method ?
    int stack_depth; // current stack depth;
    LineNumberTable_attribute *line_number_table_attribute;
    LocalVariableTable_attribute *local_variable_table_attribute;

    void MethodInitialization()
    {
        last_label_pc = 0;
        last_op_pc = 0;
        last_op_nop = 0;
        this_block_depth = 0;

        stack_depth = 0;

        return;
    }

    InnerClasses_attribute *inner_classes_attribute;
    MethodSymbol *class_literal_method;

    int registered_methods[METHOD_NUMBER];
    
    u2 name_StringNull;

    int max_block_depth,
        last_parameter_index; // set to local variable index of last parameter

    Label *begin_labels;
    Label *break_labels;
    Label *continue_labels;
    Label *final_labels;
    Label *monitor_labels;
    Label *test_labels;

    bool *has_finally_clause;
    bool *is_synchronized;

    BlockSymbol **block_symbols; // block symbols for current block

    int synchronized_blocks; // number of outstanding synchronized blocks
    int finally_blocks;      // number of outstanding synchronized blocks
    int block_depth;             // need to reset at start of each method
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

        has_finally_clause = new bool[max_block_depth+1];
        is_synchronized = new bool[max_block_depth+1];
        for (int i = 0; i < max_block_depth; i++)
        {
            has_finally_clause[i] = false; // reset has_finally_clause
            is_synchronized[i] = false;    // reset is_synchronized
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

    void chaos(char *msg);   // called when can't proceed

    void    ProcessAbruptExit(int);
    void    CompleteLabel(Label& lab);
    void    DefineLabel(Label& lab);
    int     IsLabelUsed(Label& lab);
    void    UseLabel(Label & lab, int length, int op_offset);

    //
    // methods to determine type
    //
    int IsLocal(AstExpression *);
    int IsNull(AstExpression *p);
    int IsReferenceType(TypeSymbol *p);
    int IsDefaultValue(AstExpression *p);
    int IsZero(AstExpression *p);
    int GetLHSKind(AstExpression *);
    int GetTypeWords(TypeSymbol *);

    //
    // methods to load and store values
    //
    void LoadLocalVariable(VariableSymbol *);
    void LoadLocal(int varno, TypeSymbol *);
    void StoreLocalVariable(VariableSymbol *);
    void StoreLocal(int varno, TypeSymbol *);
    int  GetConstant(LiteralValue *, TypeSymbol *);
    int  LoadConstant(AstExpression *);
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
    // These methods build entries in the constant pool.
    //
    u2 BuildDouble(IEEEdouble val);
    u2 BuildFieldref(u2 cl_index, u2 nt_index);
    u2 BuildFloat(IEEEfloat val);
    u2 BuildInteger(int val);
    u2 BuildInterfaceMethodref(u2 cl_index, u2 nt_index);
    u2 BuildLong(LongInt val);
    u2 BuildMethodref(u2 cl_index, u2 nt_index);
    u2 BuildNameAndType(u2 name, u2 type);

    //
    // make entry in constant pool from wchar string and return its index.
    //
    u2 BuildString(u2 si);
    u2 BuildUtf8(char *name, int length);

    //
    // unlike most methods, which always build a new entry, the
    // 'register' methods only build a new entry for a literal if one has net
    // yet been built.
    //
    u2 RegisterClass(Utf8LiteralValue *);
    u2 RegisterClass(char *, int);
    u2 RegisterDouble(DoubleLiteralValue *);
    u2 RegisterFloat(FloatLiteralValue *);
    u2 RegisterInteger(IntLiteralValue *);
    u2 RegisterLong(LongLiteralValue *);
    u2 RegisterMethod(int);
    u2 RegisterString(Utf8LiteralValue *);
    u2 RegisterUtf8(Utf8LiteralValue *);
    u2 RegisterUtf8(char *, int);

    void AddLocalVariableTableEntry(u2, u2, u2, u2, u2); // make entry in local variable table
    void SetInnerAttribute(TypeSymbol *); // make entry for InnerClasses attribute
    Synthetic_attribute *CreateSyntheticAttribute();
    Deprecated_attribute *CreateDeprecatedAttribute();

    //
    // Methods to generate expressions.
    //
    int  EmitExpression(AstExpression *);
    void EmitArrayAccessLHS(AstArrayAccess *);
    int  EmitArrayAccessRHS(AstArrayAccess *);
    int  EmitArrayCreationExpression(AstArrayCreationExpression *);
    int  EmitAssignmentExpression(AstAssignmentExpression *, int);
    int  EmitBinaryExpression(AstBinaryExpression *);
    void EmitBinaryOp(AstBinaryExpression *, int iop, int lop, int fop, int dop);
    int  EmitCastExpression(AstCastExpression *);
    void EmitCast(TypeSymbol *, TypeSymbol *);
    int  EmitClassInstanceCreationExpression(AstClassInstanceCreationExpression *, int);
    int  EmitConditionalExpression(AstConditionalExpression *);
    int  EmitFieldAccess(AstFieldAccess *);
    void EmitFieldAccessLHS(AstExpression *);
    void EmitFieldAccessLHSBase(AstExpression *);
    void EmitMethodInvocation(AstMethodInvocation *);
    void EmitNewArray(int, TypeSymbol *);
    void EmitCloneArray(AstMethodInvocation *);
    int  EmitPostUnaryExpression(AstPostUnaryExpression *, int);
    void EmitPostUnaryExpressionArray(AstPostUnaryExpression *expression, int);
    void EmitPostUnaryExpressionArrayCode(int, int, int, int, int, int, int, int, int);
    void EmitPostUnaryExpressionField(int, AstPostUnaryExpression *expression, int);
    void EmitPostUnaryExpressionSimple(int, AstPostUnaryExpression *, int);
    int  EmitPreUnaryExpression(AstPreUnaryExpression *, int);
    void EmitPreUnaryIncrementExpression(AstPreUnaryExpression *expression, int);
    void EmitPreUnaryIncrementExpressionArray(AstPreUnaryExpression *expression, int);
    void EmitPreUnaryIncrementExpressionArrayCode(int, int, int, int, int, int, int, int, int);
    void EmitPreUnaryIncrementExpressionField(int, AstPreUnaryExpression *expression, int);
    void EmitPreUnaryIncrementExpressionSimple(int, AstPreUnaryExpression *expression, int);
    void EmitThisInvocation(AstThisCall *);
    void EmitSuperInvocation(AstSuperCall *);
    AstExpression *UnParenthesize(AstExpression *);
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
    void CompileClass(TypeSymbol *);
    void CompileInterface(TypeSymbol *);
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
    void EmitBlockStatement(AstBlock *, int);
    void EmitStatementExpression(AstExpression *);
    void EmitSwitchStatement(AstSwitchStatement *);
    void UpdateBlockInfo(BlockSymbol *);
    void EmitTryStatement(AstTryStatement *);
    void EmitBranch(unsigned int opc, Label &);
    void EmitBranchIfExpression(AstExpression *, bool, Label &);
    void GenerateReturn(TypeSymbol *);
    void CompleteCall(MethodSymbol *, int, int);

#ifdef TEST
    void PrintCode();
#endif

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
    
    void PutOp(unsigned char opc);
    void PutNop(int);
    void PutI1(i1);
    void PutI2(i2);
    void PutU1(u1);
    void PutU2(u2);
    void PutU4(u4); 
};

#endif
