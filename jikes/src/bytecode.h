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
    int use_length;    // length of use (2 or 4 bytes)
    int op_offset;    // length of use from opcode starting instruction
    int use_offset;    // offset in code stream of use

    LabelUse() : use_length(0),op_offset(0), use_offset(0) {}

    LabelUse(int _length, int _op_offset, int _use) : 
        use_length(_length),op_offset(_op_offset),use_offset(_use) {}
};

class Label
{
public:
     int    defined; // boolean, set when value is known
     int    definition; // offset of definition point of label
     Tuple<LabelUse> uses;

     Label() : defined(0), definition(0) {}
};

class ByteCode : public ClassFile, public StringConstant
{
    public:
    Control& this_control;
    Semantic& this_semantic;
    int class_id;
//    int bytes;                          // number of bytes written to file (so far)

// Many java virtual machines, notably Sun's JDK 1.*, don't properly
// handle the case where non-final static fields are initialized
// using the Constant attribute, even though the documentation says
// that this should be done. Define initialize_statics_in_clinit
// to get this behavior to initialize such fields in clinit; otherwise
// fields are initialized using the Constant attribute.
    int initialize_statics_in_clinit;
    
    int null_constructor_seen;    // set if user supplied null constructor
        // if not set, we have to supply null constructor
    int line_number;
    int last_label_pc;  // pc for last (closest to end) label
    int last_op_pc;    // pc of last operation emitted
    int last_op_nop; // set if last operation was NOP.
    int this_block_depth; // depth of current block
    Code_attribute * code_attribute;     // code for current method/??
    int stack_depth;        // current stack depth;
    LineNumberTable_attribute * line_number_table_attribute;
    LocalVariableTable_attribute * local_variable_table_attribute;
//    Synthetic_attribute * synthetic_attribute;
    InnerClasses_attribute * inner_classes_attribute;
    MethodSymbol * class_literal_method;

#define METHOD_CLONE 0
#define METHOD_CLONE_GETMESSAGE 1
#define METHOD_CLONE_INIT    2
#define METHOD_STRINGBUFFER_TOSTRING     3
#define METHOD_STRINGBUFFER_INIT    4
#define METHOD_STRINGBUFFER_APPENDCHARARRAY 5    
#define METHOD_STRINGBUFFER_APPENDCHAR 6        
#define METHOD_STRINGBUFFER_APPENDBOOLEAN 7        
#define METHOD_STRINGBUFFER_APPENDINT    8        
#define METHOD_STRINGBUFFER_APPENDLONG    9        
#define METHOD_STRINGBUFFER_APPENDFLOAT    10        
#define METHOD_STRINGBUFFER_APPENDDOUBLE    11        
#define METHOD_STRINGBUFFER_APPENDSTRING    12    
#define METHOD_STRINGBUFFER_APPENDOBJECT    13
#define METHOD_NUMBER 14  
    
    int registered_methods[METHOD_NUMBER];
    
    u2 name_StringNull;

    int max_block_depth;
    int last_parameter_index; // set to local variable index of last parameter
    Label * begin_labels;
    Label * break_labels;
    Label * continue_labels;
    Label * final_labels;
    Label * monitor_labels;
    Label * test_labels;
    int * has_finally_clause;
    int * is_synchronized;
    int   synchronized_blocks; // number of outstanding synchronized blocks
    int   finally_blocks; // number of outstanding synchronized blocks
    BlockSymbol ** block_symbols; // block symbols for current block
    int block_depth;                  // need to reset at start of each method
    TypeSymbol * method_type;    // return type of method being compiled

void chaos(char *msg);   // called when can't proceed


void    ProcessAbruptExit(int);
void    CompleteLabel(Label& lab);
void    DefineLabel(Label& lab);
int     IsLabelUsed(Label& lab);
void    UseLabel(Label & lab,int length, int op_offset);

// methods to determine type
int     IsLocal(AstExpression *);
int     IsNull(AstExpression *p);
int     IsReferenceType(TypeSymbol *p);
int     IsDefaultValue(AstExpression *p);
int     IsZero(AstExpression *p);
int     GetLHSKind(AstExpression *, MethodSymbol *);
int     GetTypeWords(TypeSymbol *);

// methods to load and store values
void    LoadLocalVariable(VariableSymbol *);
void    LoadLocal(int varno, TypeSymbol * type);
void    StoreLocalVariable(VariableSymbol *);
void    StoreLocal(int varno, TypeSymbol * type);
int     GetConstant(LiteralValue *, TypeSymbol *);
int     LoadConstant(AstExpression *);
int     LoadLiteral(LiteralValue *,TypeSymbol *);
void    LoadReference(AstExpression *);
void    LoadShort(int val);
void    LoadInteger(int val);
int     LoadSimple(int,AstExpression *);
int     LoadArrayElement(TypeSymbol * type);
void    StoreArrayElement(TypeSymbol * type);
void    StoreField(AstExpression *);
void    StoreSimple(int,AstExpression *);

// These methods build entries in the constant pool.
u2      BuildDouble(IEEEdouble val);
u2      BuildFieldref(u2 cl_index, u2 nt_index);
u2      BuildFloat(IEEEfloat val);
u2      BuildInteger(int val);
u2      BuildInterfaceMethodref(u2 cl_index, u2 nt_index);
u2      BuildLong(LongInt val);
u2      BuildMethodref(u2 cl_index, u2 nt_index);
u2      BuildNameAndType(u2 name, u2 type);

// make entry in constant pool from wchar string and return its index.
u2      BuildString(u2 si);
u2      BuildUtf8(char *name,int length);

// unlike most methods, which always build a new entry, the
// 'register' methods only build a new entry for a literal if one has net
// yet been built.
u2      RegisterClass(Utf8LiteralValue *);
u2      RegisterClass(char *, int );
u2      RegisterDouble(DoubleLiteralValue *);
u2      RegisterFloat(FloatLiteralValue *);
u2      RegisterInteger(IntLiteralValue *);
u2      RegisterLong(LongLiteralValue *);
u2      RegisterMethod(int);
u2      RegisterString(Utf8LiteralValue *);
u2      RegisterUtf8(Utf8LiteralValue *);
u2      RegisterUtf8(char *, int);

void    AddLocalVariableTableEntry(u2,u2,u2,u2,u2); // make entry in local variable table
void    SetInnerAttribute(TypeSymbol *); // make entry for InnerClasses attribute
Synthetic_attribute * CreateSyntheticAttribute();
Deprecated_attribute * CreateDeprecatedAttribute();

//
// memory access: reference either
        // constant (literal)
        // name (includes local varable, or class variable, or field access)
        // array
#define LHS_LOCAL  0    // local variable
#define LHS_ARRAY  1    // array (of any kind)
#define LHS_FIELD  2    // instance variable
#define LHS_STATIC 3    // class variable
#define LHS_CLASS_METHOD 4  // access to private variable via class method call
#define LHS_STATIC_METHOD 5 // access to private variable via static method call
    // Methods in bc_expr.cpp        
int     EmitExpression(AstExpression * ast);
void    EmitArrayAccessLHS(AstArrayAccess *);
int     EmitArrayAccessRHS(AstArrayAccess *);
int     EmitArrayCreationExpression(AstArrayCreationExpression *);
int     EmitAssignmentExpression(AstAssignmentExpression *, int);
int     EmitBinaryExpression(AstBinaryExpression *);
void    EmitBinaryOp(AstBinaryExpression *, int iop, int lop, int fop, int dop);
int     EmitCastExpression(AstCastExpression *);
void    EmitCast(TypeSymbol *, TypeSymbol *);
int     EmitClassInstanceCreationExpression(AstClassInstanceCreationExpression *, int);
int     EmitConditionalExpression(AstConditionalExpression *);
int     EmitFieldAccess(AstFieldAccess *);
void    EmitFieldAccessLHS(AstExpression *);
void    EmitFieldAccessLHSBase(AstExpression *);
int     EmitMethodInvocation(AstMethodInvocation *, int);
void    EmitNewArray(int,TypeSymbol *);
void    EmitCloneArray(AstMethodInvocation *);
int     EmitPostUnaryExpression(AstPostUnaryExpression *,int);
void    EmitPostUnaryExpressionArray(AstPostUnaryExpression *expression, int);
void    EmitPostUnaryExpressionArrayCode(int, int, int, int, int, int, int, int, int);
void    EmitPostUnaryExpressionField(int,AstPostUnaryExpression *expression, int);
void    EmitPostUnaryExpressionSimple(int,AstPostUnaryExpression *, int);
int     EmitPreUnaryExpression(AstPreUnaryExpression *,int);
void    EmitPreUnaryIncrementExpression(AstPreUnaryExpression *expression, int);
void    EmitPreUnaryIncrementExpressionArray(AstPreUnaryExpression *expression, int);
void    EmitPreUnaryIncrementExpressionArrayCode(int, int, int, int, int, int, int,int,int);
void    EmitPreUnaryIncrementExpressionField(int,AstPreUnaryExpression *expression, int);
void    EmitPreUnaryIncrementExpressionSimple(int,AstPreUnaryExpression *expression, int);
void    EmitThisInvocation(AstThisCall *);
void    EmitSuperInvocation(AstSuperCall *);
AstExpression * UnParenthesize(AstExpression *);
void    ConcatenateString(AstBinaryExpression *);
void    EmitStringBuffer();
void    EmitCallStringToString();
void    AppendString(AstExpression *);
void    EmitStringAppendMethod(TypeSymbol *);
void    GenerateAccessMethod(MethodSymbol *);
int     GenerateFieldReference(VariableSymbol *);
void    ChangeStack (int);
void    ResolveAccess(AstExpression *, int);
int     GenerateClassAccess(AstFieldAccess *);
void    GenerateClassAccessMethod(MethodSymbol *);

    // Methods in bc_stmt.cpp

void    CompileClass(TypeSymbol *);
//void    SetConstructorParameter(VariableSymbol *);
void    CompileInterface(TypeSymbol *);
void    CompileConstructor(AstConstructorDeclaration *, Tuple<AstVariableDeclarator *> &);
#define METHOD_KIND_ORDINARY 0
#define METHOD_KIND_CONSTRUCTOR 1
#define METHOD_KIND_GENERATED_CONSTRUCTOR 2
#define METHOD_KIND_INTERFACE 3
#define METHOD_KIND_ACCESS 4
#define METHOD_KIND_ACCESS_CLASS 5
int     BeginMethod(int, MethodSymbol *);
void    EndMethod(int,int, MethodSymbol *);
void    DeclareField(VariableSymbol *);
void    InitializeClassVariable(AstVariableDeclarator *);
void    InitializeInstanceVariable(AstVariableDeclarator *);
void    InitializeArray(TypeSymbol *,AstArrayInitializer *);
void    DeclareLocalVariable(AstVariableDeclarator *);
int     EmitStatement(AstStatement *);
void    EmitReturnStatement(AstReturnStatement *);
void    EmitSynchronizedStatement(AstSynchronizedStatement *);
int     EmitBlockStatement(AstBlock *, int);
void    EmitStatementExpression(AstExpression * ast);
void    EmitSwitchStatement(AstSwitchStatement *);
void    UpdateBlockInfo(BlockSymbol *);
void    EmitTryStatement(AstTryStatement *);
void    EmitBranch(unsigned int opc, Label &);
void    EmitBranchIfExpression(AstExpression *, bool, Label &);
void    GenerateReturn(TypeSymbol * type);
void    CompleteCall(MethodSymbol *, int, int);

#ifdef TEST
void PrintCode();
#endif
//void    ProcessMethodDeclaration(AstMethodDeclaration *method);
void FinishCode(TypeSymbol *);
void Reset(){
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
