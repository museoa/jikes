// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#include "assert.h"
#include "config.h"
#include "ast.h"
#include "bytecode.h"
#include "class.h"
#include "control.h"
#include "semantic.h"
#include "stream.h"
#include "symbol.h"
#include "table.h"
#include <iostream.h>
#include <string.h>
#include <wchar.h>

#ifdef WIN32_FILE_SYSTEM
#include <windows.h>
#endif



/*
TODO:
 see if actually need call to ChangeStack, marked CHECK_THIS, in AssigmnentExpression
 */


void ByteCode::CompileClass(TypeSymbol * type)
{
    AstClassDeclaration * class_decl = type -> declaration -> ClassDeclarationCast();
    AstClassBody * class_body = (class_decl ? class_decl -> class_body :
                                  ((AstClassInstanceCreationExpression *) type -> declaration) -> class_body_opt);;
    int i;
    int mi,pi;
    AstConstructorDeclaration *constructor;
    AstMethodDeclaration *method;
    int super_init=0;
    u2 name;
    int descriptor;
    int need_init=0; // see if need to generate <init> method
    int need_clinit=0; // set if need <clinit> method
    Tuple<AstVariableDeclarator *> initialized_fields(type -> NumVariableSymbols()); // fields needing code to initialize
    int method_index;
    AstFieldDeclaration * field_decl;
    AstList * declarators;
    AstList * block_statements;
    int fi;
    int si;
    AstConstructorBlock * constructor_block;
    VariableSymbol * vsym;
    MethodSymbol * msym;
    AstStaticInitializer * static_initializer;
    class_literal_method = type -> outermost_type -> class_literal_method;
    
    for (i=0; i < class_body -> NumClassVariables(); i++) {
        field_decl = class_body -> ClassVariable(i);
        for (int vi=0;vi<field_decl -> NumVariableDeclarators();vi++) {
            AstVariableDeclarator * vd = field_decl -> VariableDeclarator(vi);
            vsym = vd -> symbol;
            //
            // We need a static constructor-initializer if we encounter at least one class
            // variable that is declared with an initialization expression that is not a
            // constant expression.
            //
            need_clinit = need_clinit || (vd -> variable_initializer_opt && !(vsym -> ACC_FINAL() && vsym -> initial_value));
            DeclareField(vsym, 0);
        }
    }
    
    // supply needed field declaration for this$0 (if there is one)
    if (type -> NumConstructorParameters()) {
        for (pi=0; pi < type -> NumConstructorParameters(); pi ++) {
            DeclareField(type -> ConstructorParameter(pi), 1);
        }
    }

    // supply needed field declaration for enclosing instances (this$n) if present
    if (type -> NumEnclosingInstances() > 1) {
        for (pi=1; pi < type -> NumEnclosingInstances(); pi ++) {
            DeclareField(type-> EnclosingInstance(pi), 1);
        }
    }

    // supply needed field declarations for "class " identifiers (used for X.class literals) if present
    for (int ri=0; ri < type -> NumClassLiterals(); ri ++) {
        DeclareField(type -> ClassLiteral(ri), 1);
    }

    for (i=0; i < class_body -> NumInstanceVariables(); i++) {
        field_decl  = class_body -> InstanceVariable(i);
        for (int vi=0;vi<field_decl -> NumVariableDeclarators();vi++) {
            AstVariableDeclarator * vd = field_decl -> VariableDeclarator(vi);
            DeclareField(vd -> symbol, 0);
            // must set Constant attribute if initial value
            if (vd -> variable_initializer_opt) { // if initialization needed
                need_init=1;
                initialized_fields.Next() = vd;
            }
        }
    }
    // compile method bodies and constructors
    for (i=0; i < class_body -> NumMethods(); i++) {
        method = class_body -> Method(i);
        if (method -> method_symbol){
            method_index = BeginMethod(METHOD_KIND_ORDINARY, method -> method_symbol); // not constructor
            AstBlock *method_block = method -> method_body -> BlockCast();
            if (method_block) // not an abstract method ?
                (void) EmitStatement(method_block);
            EndMethod(METHOD_KIND_ORDINARY,method_index, method -> method_symbol); // not constructor
        }
    }

    if (type -> ACC_ABSTRACT())
    {
        for (i=0; i < type -> expanded_method_table -> symbol_pool.Length(); i++) {
            MethodShadowSymbol *method_shadow_symbol = type -> expanded_method_table -> symbol_pool[i];
            MethodSymbol *method_symbol = method_shadow_symbol -> method_symbol;
            if (method_symbol -> containing_type != type && method_symbol -> ACC_ABSTRACT()){
                if (! method_symbol -> IsTyped())
                    method_symbol -> ProcessMethodSignature(&this_semantic, class_decl -> identifier_token);
                method_symbol -> ProcessMethodThrows(&this_semantic, class_decl -> identifier_token);

                method_index = BeginMethod(METHOD_KIND_ORDINARY, method_symbol);
                EndMethod(METHOD_KIND_ORDINARY,method_index, method_symbol);
            }
        }
    }

// compile any private access methods
    for (i = 0; i < type -> NumPrivateAccessMethods(); i++) {
        MethodSymbol * method_sym = type -> PrivateAccessMethod(i);
//      AstMethodDeclaration * method_decl = method_sym -> method_or_constructor_declaration -> MethodDeclarationCast();
        method_index = BeginMethod(METHOD_KIND_ACCESS, method_sym);
        GenerateAccessMethod(method_sym);
        EndMethod(METHOD_KIND_ACCESS,method_index, method_sym);
    }

    if (type -> class_literal_method) {
        MethodSymbol * class_literal_sym = type -> class_literal_method;
        // generate the class$ identity method used for class literal-related garbage mumbo-jumbo initialization
        method_index = BeginMethod(METHOD_KIND_ACCESS_CLASS, class_literal_sym);
        GenerateClassAccessMethod(class_literal_sym);
        EndMethod(METHOD_KIND_ACCESS_CLASS,method_index, class_literal_sym);
    }

    if (type -> block_initializer_method) {
        MethodSymbol * block_init_method = type -> block_initializer_method;
        method_index = BeginMethod(METHOD_KIND_ORDINARY, block_init_method);
                int fi=0, bi=0;
                while (fi < initialized_fields.Length() && bi < class_body -> NumBlocks()) {
                    if (initialized_fields[fi] -> LeftToken() < class_body -> Block(bi) -> left_brace_token) {
                        InitializeInstanceVariable(initialized_fields[fi++]);
                    }
                    else {
                        (void) EmitStatement((AstStatement *) (class_body -> Block(bi++)));
                    }
                }
                        
                while (fi<initialized_fields.Length()) {
                    InitializeInstanceVariable(initialized_fields[fi++]);
                }
    
                // compile any initialization blocks
                
                while (bi < class_body -> NumBlocks()) {
                    (void) EmitStatement((AstStatement *) (class_body -> Block(bi++)));
                }
        
    PutOp(OP_RETURN);
        EndMethod(METHOD_KIND_ORDINARY,method_index,block_init_method);  // is constructor
    }
    
    if (type -> NumGeneratedConstructors() == 0) {
        if (class_body -> default_constructor) {
            CompileConstructor(class_body -> default_constructor, initialized_fields);
        }
        else {
            for (i=0; i < class_body -> NumConstructors(); i++) {
                constructor = class_body -> Constructor(i);
                CompileConstructor(constructor, initialized_fields);
            }
            for (i = 0; i < type -> NumPrivateAccessConstructors(); i++) {
                MethodSymbol *constructor_sym = type -> PrivateAccessConstructor(i);
                AstConstructorDeclaration *constructor =
                       constructor_sym -> method_or_constructor_declaration -> ConstructorDeclarationCast();
                CompileConstructor(constructor, initialized_fields);
            }
        }
    }
    else {
        for (i=0; i < type -> NumGeneratedConstructors(); i++) {
            MethodSymbol * this_constructor_symbol = type -> GeneratedConstructor(i);
            AstConstructorDeclaration * constructor =
                    this_constructor_symbol -> method_or_constructor_declaration -> ConstructorDeclarationCast();
            constructor_block = constructor -> constructor_body -> ConstructorBlockCast();
            // compile generated constructor
            method_index = BeginMethod(METHOD_KIND_CONSTRUCTOR, this_constructor_symbol);  // is constructor
            methods[method_index].name_index = RegisterUtf8(U8S_LT_init_GT_, strlen(U8S_LT_init_GT_));

            UpdateBlockInfo(this_constructor_symbol -> block_symbol);
                
            if (! constructor_block -> explicit_constructor_invocation_opt) {
                PutOp(OP_ALOAD_0);
                PutOp(OP_INVOKENONVIRTUAL);   // no args, hence no need to call ChangeStack()
                PutU2(BuildMethodref(super_class,
                    BuildNameAndType(RegisterUtf8(U8S_LT_init_GT_, strlen(U8S_LT_init_GT_)), RegisterUtf8(U8S_LP_RP_V, strlen(U8S_LP_RP_V)))));
            }
            else {
                (void) EmitStatement((AstStatement *) constructor_block -> explicit_constructor_invocation_opt);
            }

            for (si = 0;si < constructor_block -> NumLocalInitStatements();si++) {
                (void) EmitStatement((AstStatement *) constructor_block -> LocalInitStatement(si));
            }

            // supply needed field initialization unless constructor
            // starts with explicit 'this' call to another constructor
            if (! (constructor_block -> explicit_constructor_invocation_opt &&
                   constructor_block -> explicit_constructor_invocation_opt -> ThisCallCast())) {
                if (type -> NumEnclosingInstances()) {
                    VariableSymbol * this0_parameter = type -> EnclosingInstance(0);
                    PutOp(OP_ALOAD_0); // load address of object on which method is to be invoked
                    LoadLocal(1, this0_parameter -> Type());
                    PutOp(OP_PUTFIELD);
                    PutU2(GenerateFieldReference(this0_parameter));
                }

                if (class_body -> this_block) { // compile explicit 'this' call if present
                    AstBlock * block = (AstBlock *) class_body -> this_block;
                    for (si = 0;si < block -> NumStatements();si++) {
                        (void) EmitStatement((AstStatement *) block -> Statement(si));
                    }
                }
                
                if (! type -> block_initializer_method) {
                    int fi=0,bi=0;
                    while (fi < initialized_fields.Length() && bi < class_body -> NumBlocks()) {
                        if (initialized_fields[fi] -> LeftToken() < class_body -> Block(bi) -> left_brace_token) {
                            InitializeInstanceVariable(initialized_fields[fi++]);
                        }
                        else {
                            AstBlock * block = (AstBlock *) (class_body -> Block(bi++));
                            for (si = 0;si < block -> NumStatements();si++) {
                               (void) EmitStatement((AstStatement *) block -> Statement(si));
                            }
                        }
                    }
                        
                    while (fi<initialized_fields.Length()) {
                        InitializeInstanceVariable(initialized_fields[fi++]);
                    }
                    // compile any initialization blocks
                    while (bi < class_body -> NumBlocks()) {
                        AstBlock * block = (AstBlock *) (class_body -> Block(bi++));
                        for (si = 0;si < block -> NumStatements();si++) {
                           (void) EmitStatement((AstStatement *) block -> Statement(si));
                        }
                    }
                }
                else {
                    // generate a call to the parameterless function block_initializer_function
                    PutOp(OP_ALOAD_0); // load address of object on which method is to be invoked
                    PutOp(OP_INVOKENONVIRTUAL);  
                    CompleteCall(type -> block_initializer_method, 0, 0);
                }
            }

            (void) EmitStatement(constructor_block -> original_constructor_invocation);
            PutOp(OP_RETURN);
            EndMethod(METHOD_KIND_CONSTRUCTOR,method_index, this_constructor_symbol);  // is constructor

            // compile method associated with generated constructor
            MethodSymbol * local_constructor_symbol = this_constructor_symbol -> LocalConstructor();
            method_index = BeginMethod(METHOD_KIND_CONSTRUCTOR, local_constructor_symbol);  // is constructor
            methods[method_index].name_index =
                    RegisterUtf8(local_constructor_symbol -> ExternalIdentity() -> Utf8_literal);

            for (si = 0;si < constructor_block -> block -> NumStatements();si++) {
                (void) EmitStatement((AstStatement *) constructor_block -> block -> Statement(si));
            }
            EndMethod(METHOD_KIND_CONSTRUCTOR,method_index,local_constructor_symbol);  // is constructor
        }
    }
    

    if (class_body -> NumStaticInitializers() > 0 || need_clinit) {
        msym = type -> static_initializer_method;
        method_index =  BeginMethod(METHOD_KIND_GENERATED_CONSTRUCTOR, msym);
        code_attribute -> max_locals = 0;
// revisit members that are part of class initialization
        for (mi = 0; mi < class_body -> NumClassBodyDeclarations(); mi++) {
            static_initializer = class_body -> ClassBodyDeclaration(mi) -> StaticInitializerCast();
            if (static_initializer) {
                (void) EmitStatement((AstStatement *) static_initializer -> block);
            }
            else if (class_body -> ClassBodyDeclaration(mi) -> FieldDeclarationCast()) {
                field_decl = class_body -> ClassBodyDeclaration(mi) -> FieldDeclarationCast();
                // field declaration
                for (int vi=0;vi<field_decl -> NumVariableDeclarators();vi++) {
                    AstVariableDeclarator * vd = field_decl -> VariableDeclarator(vi);;
                    vsym = vd -> symbol;
                    if (!vsym -> ACC_STATIC()) continue; // skip non-static fields
                    InitializeClassVariable(vd);
                }
            }
        }
        PutOp(OP_RETURN);
        EndMethod(METHOD_KIND_GENERATED_CONSTRUCTOR,method_index,msym);
    }
    FinishCode(type);

    Write();

#ifdef TEST
    if (this_control.option.debug_dump_class) PrintCode();
#endif
}

//
// initialized_fields is a list of fields needing code to initialize.
//
void ByteCode::CompileConstructor(AstConstructorDeclaration * constructor, Tuple<AstVariableDeclarator *> &initialized_fields)
{
    
    MethodSymbol * method_symbol = constructor -> constructor_symbol;
    TypeSymbol * type = method_symbol -> containing_type;
    AstClassDeclaration * class_decl = type -> declaration -> ClassDeclarationCast();
    AstClassBody * class_body = (class_decl ? class_decl -> class_body :
                                  ((AstClassInstanceCreationExpression *) type -> declaration) -> class_body_opt);;
    AstBlock * block;
    int is_this=0; // set if start with this() call.
    
    int method_index;
    int fi;
    int si;
    int descriptor;
    AstConstructorBlock * constructor_block;
    AstList * block_statements;

    method_index = BeginMethod(METHOD_KIND_CONSTRUCTOR, method_symbol);  // is constructor
    methods[method_index].name_index = RegisterUtf8(U8S_LT_init_GT_, strlen(U8S_LT_init_GT_));

    UpdateBlockInfo(method_symbol -> block_symbol);

    constructor_block = constructor -> constructor_body -> ConstructorBlockCast();
    
    if (! constructor_block -> explicit_constructor_invocation_opt) {
    if (super_class) {
            PutOp(OP_ALOAD_0);
            PutOp(OP_INVOKENONVIRTUAL);   // no args, hence no need to call ChangeStack()
            PutU2(BuildMethodref(super_class,
                BuildNameAndType(RegisterUtf8(U8S_LT_init_GT_, strlen(U8S_LT_init_GT_)), RegisterUtf8(U8S_LP_RP_V, strlen(U8S_LP_RP_V)))));
    }
    }
    else {
        if (constructor_block -> explicit_constructor_invocation_opt -> ThisCallCast()) is_this=1;
        (void) EmitStatement((AstStatement *) constructor_block -> explicit_constructor_invocation_opt);
    }

    // supply needed field initialization unless constructor
    // starts with explicit 'this' call to another constructor
    if (!is_this) {
        if (type -> NumEnclosingInstances()) {
            VariableSymbol * this0_parameter = type -> EnclosingInstance(0);
            PutOp(OP_ALOAD_0); // load address of object on which method is to be invoked
            LoadLocal(1, this0_parameter -> Type());
            PutOp(OP_PUTFIELD);
            PutU2(GenerateFieldReference(this0_parameter));
        }

        if (class_body -> this_block) { // compile explicit 'this' call if present
            block = (AstBlock *) class_body -> this_block;
            for (si = 0;si < block -> NumStatements();si++) {
                (void) EmitStatement((AstStatement *)block -> Statement(si));
            }
        }
        if (!type -> block_initializer_method) {
        int fi=0,bi=0;
        while (fi < initialized_fields.Length() && bi < class_body -> NumBlocks()) {
            if (initialized_fields[fi] -> LeftToken() < class_body -> Block(bi) -> left_brace_token) {
                InitializeInstanceVariable(initialized_fields[fi++]);
            }
            else {
                AstBlock * block = (AstBlock *) (class_body -> Block(bi++));
                for (si = 0;si < block -> NumStatements();si++) {
                   (void) EmitStatement((AstStatement *) block -> Statement(si));
                }

            }
        }
                
        while (fi<initialized_fields.Length()) {
            InitializeInstanceVariable(initialized_fields[fi++]);
        }
    
        // compile any initialization blocks
        
        while (bi < class_body -> NumBlocks()) {
            AstBlock * block = (AstBlock *) (class_body -> Block(bi++));
            for (si = 0;si < block -> NumStatements();si++) {
               (void) EmitStatement((AstStatement *) block -> Statement(si));
            }
        }
        }
        else {
            // generate a call to the parameterless function block_initializer_function
            PutOp(OP_ALOAD_0); // load address of object on which method is to be invoked
            PutOp(OP_INVOKENONVIRTUAL);  
            CompleteCall(type -> block_initializer_method, 0, 0);
        }

    }
    

    (void) EmitStatement(constructor_block -> block);

    EndMethod(METHOD_KIND_CONSTRUCTOR,method_index, method_symbol);
}


void ByteCode::CompileInterface(TypeSymbol * type)
{
    AstInterfaceDeclaration * interface_decl = type -> declaration -> InterfaceDeclarationCast();
    AstMethodDeclaration * method;
    int method_index;
    int i;
    int vi;
    int need_clinit=0; // set if need <clinit> method
    VariableSymbol * vsym;
    AstFieldDeclaration * field_decl;
    AstVariableDeclarator * vd;
    u2 name;
    AstList * declarators;
    for (i=0; i < interface_decl -> NumClassVariables(); i++) {
        field_decl  = interface_decl -> ClassVariable(i);

        for (vi=0;vi<field_decl -> NumVariableDeclarators();vi++) {
            AstVariableDeclarator * vd = field_decl -> VariableDeclarator(vi);
            vsym = vd -> symbol;
            //
            // We need a static constructor-initializer if we encounter at least one class
            // variable that is declared with an initialization expression that is not a
            // constant expression.
            //
            need_clinit = need_clinit || (vd -> variable_initializer_opt && !(vsym -> ACC_FINAL() && vsym -> initial_value));
            DeclareField(vsym, 0);
        }
    }

    for (i=0; i < interface_decl -> NumMethods(); i++) {

        method = interface_decl -> Method(i);
        if (method  ->  method_symbol){
            method_index = BeginMethod(METHOD_KIND_INTERFACE, method -> method_symbol); // not constructor
            EndMethod(METHOD_KIND_INTERFACE,method_index, method -> method_symbol);
        }
    }

    if (need_clinit) {
        method_index =  BeginMethod(METHOD_KIND_GENERATED_CONSTRUCTOR, (MethodSymbol *) 0);
        code_attribute -> max_locals = 0;
        methods[method_index].SetACC_FINAL();
        // revisit members that are part of class initialization
        for (int mi = 0; mi < interface_decl -> NumClassVariables(); mi++) {
                // field declaration
                field_decl = interface_decl -> ClassVariable(mi);
                for (int vi2=0;vi2<field_decl -> NumVariableDeclarators();vi2++) {
                    vd = field_decl -> VariableDeclarator(vi2);
                    vsym = vd -> symbol;
                    if (!vsym -> ACC_STATIC()) continue; // skip non-static fields
                    InitializeClassVariable(vd);
            }
        }
        PutOp(OP_RETURN);
        EndMethod(METHOD_KIND_GENERATED_CONSTRUCTOR,method_index, (MethodSymbol *)0);
    }
    FinishCode(type);

    Write();

#ifdef TEST
    if (this_control.option.debug_dump_class) PrintCode();
#endif

}

void ByteCode::DeclareField(VariableSymbol * symbol, int is_synthetic)
{
    int field_index = fields.NextIndex(); // index for field

    fields[field_index].access_flags = symbol -> access_flags;
    fields[field_index].name_index = RegisterUtf8(symbol -> ExternalIdentity() -> Utf8_literal);
    fields[field_index].descriptor_index = RegisterUtf8(symbol -> Type() -> signature);

    if (symbol -> ACC_FINAL() && symbol -> initial_value) {
        u4 constant_value_attribute_length = 2;
        int lit_index = GetConstant(symbol -> initial_value, symbol -> Type());

        u2 name = RegisterUtf8(U8S_ConstantValue, strlen(U8S_ConstantValue));
        ConstantValue_attribute *constant_value_attribute = new ConstantValue_attribute(name, constant_value_attribute_length);

        constant_value_attribute -> constantvalue_index = lit_index;
        fields[field_index].attributes.Next() = constant_value_attribute;
    }

    if (is_synthetic) {
        fields[field_index].attributes.Next() = CreateSyntheticAttribute();
    }

    return;
}
            
void ByteCode::GenerateAccessMethod(MethodSymbol * method_symbol)
{
    // generate code for access method to private member of containing class

    int stack_words = 0;
    code_attribute -> max_locals = 1; // DS fix this 01 dec 97
    TypeSymbol * parameter_type;


    VariableSymbol * field_sym = method_symbol -> accessed_member -> VariableCast();
    // generate code according to type of method
    if (method_symbol -> accessed_member -> MethodCast()) {
        // if accessing another method
        // copy arguments
        if (! method_symbol -> ACC_STATIC()) {
            PutOp(OP_ALOAD_0); // load address of object on which method is to be invoked
        }
        for (int i = 0; i < method_symbol -> NumFormalParameters(); i++) {
            TypeSymbol * local_type = method_symbol -> FormalParameter(i) -> Type();
            code_attribute -> max_locals += GetTypeWords(local_type);
            stack_words += GetTypeWords(local_type);
            LoadLocal(method_symbol -> ACC_STATIC() ? i: i+1, local_type);
        }
        PutOp(method_symbol -> ACC_STATIC() ? OP_INVOKESTATIC  // must be static or private
                                            : OP_INVOKENONVIRTUAL);  
        CompleteCall(method_symbol -> accessed_member -> MethodCast(), stack_words, 0);
    }
    else {
        // accessing field
        if (method_type == this_control.void_type) {
            // need method to assign value to field
            if (method_symbol -> NumFormalParameters()== 0) {
                chaos("assignment access method requires parameter");
            }  
            parameter_type = method_symbol -> FormalParameter(0) -> Type();
            code_attribute -> max_locals += GetTypeWords(parameter_type);
            
            if (method_symbol -> ACC_STATIC()) {
                LoadLocal(0, parameter_type);
                PutOp(OP_PUTSTATIC);
            }
            else {
                PutOp(OP_ALOAD_0); // get this for field access
                LoadLocal(1, parameter_type);
                PutOp(OP_PUTFIELD);
            }
            ChangeStack(this_control.IsDoubleWordType(parameter_type) ? -2: -1);
            PutU2(GenerateFieldReference(method_symbol -> accessed_member -> VariableCast()));
        }
        else {
            // need method to retrieve value of field
            if (field_sym -> ACC_STATIC()) {
                PutOp(OP_GETSTATIC);
                ChangeStack(this_control.IsDoubleWordType(method_type) ? 2: 1);
            }
            else {
                PutOp(OP_ALOAD_0); // get this for field access
                PutOp(OP_GETFIELD);
                ChangeStack(this_control.IsDoubleWordType(method_type) ? 1: 0);
            }
            PutU2(GenerateFieldReference(field_sym));
        }
    }
    
        // method returns void, generate return unless last statement is return
    if (method_type == this_control.void_type) {
        //  int line_index;
        // line_index = line_number_table_attribute -> line_number_table.NextIndex();
        // line_number_table_attribute -> line_number_table[line_index]
        PutOp(OP_RETURN);// guarantee return at end of body
    }
    else GenerateReturn(method_type);
    if (last_label_pc >= code_attribute -> code.Length()) {
        // here to emit noop if would otherwise EmitBranch past end
        PutNop(0);
    }
}

void ByteCode::GenerateReturn(TypeSymbol * type)
{
    if (this_control.IsSimpleIntegerValueType(type)|| type==this_control.boolean_type) {
        PutOp(OP_IRETURN);
    }
    else if (type==this_control.long_type) {
        PutOp(OP_LRETURN);
    }
    else if (type==this_control.float_type) {
        PutOp(OP_FRETURN);
    }
    else if (type==this_control.double_type) {
        PutOp(OP_DRETURN);
    }
    else  {// must be reference expression
        PutOp(OP_ARETURN);
    }
}

int ByteCode::BeginMethod(int method_kind, MethodSymbol * msym) 
{
    // is_constructor is set if this is constructor, is_generated_constructor is set is need to compile
    // default (no argument) constructor.

    BlockSymbol * block_symbol;
    TypeSymbol * throw_symbol;
    u4 length_code=0;                   // dummy value, will supply real value after code generated.
    int i;
    u4 line_number_table_attribute_length = 0;
    u4 exceptions_attribute_length = 0;
    u4 local_variable_table_attribute_length = 0;
    stack_depth = 0;
    last_label_pc = 0;
    last_op_pc = 0;
    last_op_nop = 0;
    this_block_depth = 0;
    int method_index = methods.NextIndex(); // index for method
    u2 name;
    switch (method_kind) {
        case METHOD_KIND_ORDINARY:
        case METHOD_KIND_ACCESS:
        case METHOD_KIND_ACCESS_CLASS:
        case METHOD_KIND_INTERFACE:
            methods[method_index].name_index = RegisterUtf8(msym -> ExternalIdentity() -> Utf8_literal);
            methods[method_index].descriptor_index = RegisterUtf8(msym -> signature);
            break;
        case METHOD_KIND_CONSTRUCTOR:
            // caller sets name
            methods[method_index].descriptor_index = RegisterUtf8(msym -> signature);
            break;
    case METHOD_KIND_GENERATED_CONSTRUCTOR:
        methods[method_index].name_index = RegisterUtf8(U8S_LT_clinit_GT_, strlen(U8S_LT_clinit_GT_));
        methods[method_index].descriptor_index = RegisterUtf8(U8S_LP_RP_V, strlen(U8S_LP_RP_V));
        methods[method_index].SetACC_STATIC();
        break;
    }

    if (method_kind==METHOD_KIND_CONSTRUCTOR || method_kind == METHOD_KIND_GENERATED_CONSTRUCTOR) {
       method_type = this_control.Object();
       if (msym == (MethodSymbol *) 0)  {
           max_block_depth = 2;
        }
        else {
           max_block_depth = msym -> max_block_depth;
        }
    }
    else { // normal method declaration 
            max_block_depth = msym -> max_block_depth;
        block_symbol = msym -> block_symbol;
        method_type = msym -> Type();
    }
    // set access flags for non-generated constructor. If we have
    // generated the constructore, then the access flags have been
    // set to be the same as those of the containing class.
    if (method_kind != METHOD_KIND_GENERATED_CONSTRUCTOR) {
        methods[method_index].access_flags = msym -> access_flags;
    }
#ifdef MAKE_FINAL_PUBLIC
    if (method_kind==METHOD_KIND_ACCESS) { // DS debug 01 dec 97
      AccessFlags flags;
      flags.access_flags = methods[method_index].access_flags;
      flags.SetACC_PUBLIC();
      flags.SetACC_FINAL();
      methods[method_index].access_flags = flags.access_flags;
     }
#endif
    if (msym && msym -> NumThrows()) {
        // generate throws attribute if method throws any exceptions
        exceptions_attribute_length = 2 * (1 + msym -> NumThrows());
        name = RegisterUtf8(U8S_Exceptions, strlen(U8S_Exceptions));
        Exceptions_attribute * exceptions_attribute = new Exceptions_attribute(name, exceptions_attribute_length);
        for (i=0;i<msym -> NumThrows(); i++) {
            throw_symbol = (TypeSymbol *) msym -> Throws(i);
            exceptions_attribute -> exception_index_table.Next() = 
                RegisterClass(throw_symbol -> fully_qualified_name);
        }
        methods[method_index].attributes.Next() = exceptions_attribute;
    }
    if (method_kind==METHOD_KIND_INTERFACE) return method_index;
    // here if need code and associated attributes.
    if (this_control.option.g) {
        name = RegisterUtf8(U8S_LocalVariableTable, strlen(U8S_LocalVariableTable));
        local_variable_table_attribute= new LocalVariableTable_attribute(name, local_variable_table_attribute_length);
    }
    begin_labels = new Label[max_block_depth + 1];
    break_labels = new Label[max_block_depth + 1];
    continue_labels = new Label[max_block_depth + 1];
    test_labels = new Label[max_block_depth + 1];
    final_labels = new Label[max_block_depth + 1];
    monitor_labels = new Label[max_block_depth + 1];
    has_finally_clause = new int[max_block_depth+1];
    is_synchronized = new int[max_block_depth+1];
    block_symbols = new BlockSymbol *[max_block_depth+1];
    for (i=0;i<max_block_depth;i++) {
        has_finally_clause[i]=0; // reset has_finally_clause
        is_synchronized[i]=0; // reset has_finally_clause
    }

    if (! (msym && (msym -> ACC_ABSTRACT() || msym -> ACC_NATIVE())))
    {
        name = RegisterUtf8(U8S_Code, strlen(U8S_Code));
        code_attribute  = new Code_attribute(name,length_code);
        code_attribute -> max_stack = 0;
        code_attribute -> max_locals = 0;
    
        if (method_kind==METHOD_KIND_CONSTRUCTOR || method_kind==METHOD_KIND_GENERATED_CONSTRUCTOR) {
            if (method_kind!=METHOD_KIND_GENERATED_CONSTRUCTOR) {
                block_symbol = msym -> block_symbol;
                if (block_symbol && block_symbol -> max_variable_index > code_attribute -> max_locals)
                {
                    code_attribute -> max_locals = msym -> block_symbol -> max_variable_index;
                }
            }
        }
        else {
            block_symbol = msym -> block_symbol;
            if(block_symbol && block_symbol -> max_variable_index>code_attribute -> max_locals){
                code_attribute -> max_locals = block_symbol -> max_variable_index;
            }
        }
        code_attribute -> attribute_name_index = RegisterUtf8(U8S_Code, strlen(U8S_Code));

        line_number=0;  // temporary until PC
        name = RegisterUtf8(U8S_LineNumberTable, strlen(U8S_LineNumberTable));
        line_number_table_attribute  = new LineNumberTable_attribute(name, line_number_table_attribute_length);
        line_number_table_attribute -> attribute_name_index = name;
    }

    if (msym && msym -> NumFormalParameters()) {
        VariableSymbol * last_sym = (VariableSymbol *) msym -> FormalParameter(msym -> NumFormalParameters() - 1);
        last_parameter_index = last_sym -> LocalVariableIndex();
    }
    else {
        last_parameter_index = -1;
    }
    return method_index;
}

void ByteCode::EndMethod(int method_kind,int method_index, MethodSymbol *method_sym) 
{
    int i;
    u4 length_line_number_table_attribute = 0;
    this_block_depth=0;
    TypeSymbol * ptype;
    int has_code = 1; // Assume Code Attribute needed
    if (method_kind==METHOD_KIND_INTERFACE) return;
    if (method_kind==METHOD_KIND_CONSTRUCTOR || method_kind==METHOD_KIND_GENERATED_CONSTRUCTOR) {
        ptype= this_control.void_type;
    }
    else {
        ptype = method_sym -> Type();
        if (method_sym -> ACC_ABSTRACT() || method_sym -> ACC_NATIVE()) has_code =  0;
    }
#ifdef NONO    
    if (has_code && ptype==this_control.void_type) {
        // method returns void, generate return unless last statement is return
            //  int line_index;
            // line_index = line_number_table_attribute -> line_number_table.NextIndex();
            // line_number_table_attribute -> line_number_table[line_index]
           PutOp(OP_RETURN);// guarantee return at end of body
    }
#endif
    if (has_code) {
        if (last_label_pc >= code_attribute -> code.Length()) {
            // here to emit noop if would otherwise branch past end
            PutNop(0);
        }
            
        // attribute length:
        // need to review how to make attribute_name and attribute_length
        // only write line number attribute if -O not specified and there
        // are line numbers to write.
        if (!this_control.option.O && line_number_table_attribute -> line_number_table.Length()) {
            line_number_table_attribute -> attribute_length = 
                line_number_table_attribute -> line_number_table.Length()*4 + 2;
            code_attribute -> attributes.Next() = line_number_table_attribute;
        }
        else { // line_number_table_attribute not needed, so delete it now
          delete line_number_table_attribute;
        }
        if (this_control.option.g) {
            if (method_kind==METHOD_KIND_ORDINARY) {
                if (! method_sym -> ACC_STATIC()) {
                    // add 'this' to local variable table
                    AddLocalVariableTableEntry(0, code_attribute -> code.Length(), RegisterUtf8(U8S_this, strlen(U8S_this)),
                        RegisterUtf8(method_sym -> containing_type -> signature), 0);
                }
            }
            else if (method_kind==METHOD_KIND_CONSTRUCTOR && method_sym) {
                AddLocalVariableTableEntry(0, code_attribute -> code.Length(), RegisterUtf8(U8S_this, strlen(U8S_this)),
                    RegisterUtf8(method_sym -> containing_type -> signature), 0);
            }
            if (method_kind==METHOD_KIND_ORDINARY || method_kind == METHOD_KIND_CONSTRUCTOR) {
                for (i = 0; i < method_sym -> NumFormalParameters(); i++) {
                   VariableSymbol * parameter = method_sym -> FormalParameter(i);
                   AddLocalVariableTableEntry(0, code_attribute -> code.Length(),
                   RegisterUtf8(parameter -> ExternalIdentity() -> Utf8_literal),
                   RegisterUtf8(parameter -> Type() -> signature),
                   parameter -> LocalVariableIndex());
                }
            }
            if (local_variable_table_attribute -> local_variable_table.Length()) {
                local_variable_table_attribute -> attribute_length = 
                local_variable_table_attribute -> local_variable_table.Length()*10 + 2;
                code_attribute -> attributes.Next() = local_variable_table_attribute;
            }
        }
         // std. fields of attribute_info
        int attribute_info_length = 0;
        for (i = 0; i < code_attribute -> attributes.Length(); i++) {
            if (code_attribute -> attributes[i] -> attribute_length) attribute_info_length += (code_attribute -> attributes[i] -> attribute_length+6);
        }
        code_attribute -> attribute_length = + 2 // for max_stack
               + 2  // for max_locals
               + 4      // for code_length
               + code_attribute -> code.Length() // for code
               + 2  // for exception_table_length
               + code_attribute -> exception_table.Length() * 8  // for exception table
               + 2  // for attributes_count
               + attribute_info_length
              ;
    
        methods[method_index].attributes.Next() = code_attribute;
        if (method_kind==METHOD_KIND_ACCESS || method_kind==METHOD_KIND_ACCESS_CLASS) {
          methods[method_index].attributes.Next() = CreateSyntheticAttribute();  
        }
    }
    delete [] begin_labels;
    delete [] break_labels;
    delete [] continue_labels;
    delete [] final_labels;
    delete [] monitor_labels;
    delete [] test_labels;

    delete [] has_finally_clause;
    delete [] is_synchronized;
    delete [] block_symbols;
        
}


void ByteCode::InitializeClassVariable(AstVariableDeclarator * vd)
{
    VariableSymbol * symbol = vd -> symbol;
    AstExpression * expression;
    TypeSymbol * expression_type;
    if (vd -> variable_initializer_opt) { // field needs initialization
        expression = (AstExpression *) vd -> variable_initializer_opt;
        if (expression -> ArrayInitializerCast()) {
            InitializeArray(vd -> symbol ->Type(),
                    expression -> ArrayInitializerCast());
        }
        else if (symbol -> ACC_FINAL() && expression -> IsConstant()) return; // if already initialized
        else if (!initialize_statics_in_clinit && expression -> IsConstant()) return; // if already initialized
        else {
            expression_type = expression -> Type();
            EmitExpression(expression);
        }
        PutOp(OP_PUTSTATIC);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? -2: -1);
        PutU2(GenerateFieldReference(symbol));
    }
}

void ByteCode::InitializeInstanceVariable(AstVariableDeclarator * vd)
{
    VariableSymbol * symbol = vd -> symbol;
    TypeSymbol * type = symbol -> Type();
    AstExpression * expression;
    TypeSymbol * expression_type;
    if (vd -> variable_initializer_opt) { // field needs initialization
        expression = (AstExpression *) vd -> variable_initializer_opt;
        if (expression -> ArrayInitializerCast()) {
            PutOp(OP_ALOAD_0); // load 'this'
            InitializeArray(vd -> symbol ->Type(),
                    expression -> ArrayInitializerCast());
        }
        else {
            expression_type = expression -> Type();
            PutOp(OP_ALOAD_0); // load 'this'
            EmitExpression(expression);
        }
        PutOp(OP_PUTFIELD);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? -2: -1);
        PutU2(GenerateFieldReference(symbol));
    }
}

void ByteCode::InitializeArray(TypeSymbol *type, AstArrayInitializer * array_initializer)
{
    int i;
    AstExpression * expr;
    Ast * entry;
    TypeSymbol * subtype = type -> ArraySubtype();  
    int num_ent = array_initializer -> NumVariableInitializers();

    LoadInteger(num_ent);
    EmitNewArray(1,type); // make the array
    for (i=0;i<num_ent;i++) {
        entry =  array_initializer -> VariableInitializer(i);
        PutOp(OP_DUP);
        LoadInteger(i);
        expr=entry -> ExpressionCast();
        if (expr) {
            EmitExpression(expr);
        }
        else if (entry -> ArrayInitializerCast()) {
          InitializeArray(subtype, entry -> ArrayInitializerCast());
        }
        else chaos("wrong array initializer list element type");
      
        StoreArrayElement(subtype);
    }
}     

void ByteCode::DeclareLocalVariable(AstVariableDeclarator * declarator)
{
    AstArrayCreationExpression * ace;
    // generate code for local variable declaration.
    if (this_control.option.g) {
        declarator -> symbol -> local_program_counter = code_attribute -> code.Length();
    }
    if (declarator -> symbol -> initial_value) {
        (void) LoadLiteral(declarator -> symbol -> initial_value,declarator -> symbol -> Type()); // Is LiteralValue *
    }
    else if (declarator -> variable_initializer_opt) {
        ace = declarator -> variable_initializer_opt -> ArrayCreationExpressionCast();
        if (ace) {
            (void) EmitArrayCreationExpression(ace);
        }
        else if (declarator -> variable_initializer_opt -> ArrayInitializerCast()) {
            InitializeArray(declarator -> symbol ->Type(),
                            declarator -> variable_initializer_opt -> ArrayInitializerCast());
        }
        else { // evaluation as expression
            EmitExpression(declarator -> variable_initializer_opt -> ExpressionCast());
        }
    }
    else return; // if nothing to initialize

    StoreLocalVariable(declarator -> symbol);
}

// JLS Chapter 13: Blocks and Statements
//  Statements control the sequence of evaluation of Java programs,
//  are executed for their effects and do not have values.
//
// Processing of loops requires a loop stack, especially to hangle
// break and continue statements.
// Loops have three labels, LABEL_BEGIN for start of loop body,
// LABEL_BREAK to leave the loop, and LABEL_CONTINUE to continue the iteration.
// Each loop requires a break label; other labels are defined and used
// as needed.
// Labels allocated but never used incur no extra cost in the generated
// byte code, only in additional execution expense during compilation.

int ByteCode::EmitStatement(AstStatement *statement) 
{
    // generate code for statement (JLS 13.4). The list of statement kinds
    // are those used in grammar, not precisely those used in JLS..
    // return 1 if execution of statement causes abrupt exit, 0 otherwise.

    LexStream::TokenIndex start;
    int abrupt = 0;
    if (statement -> kind !=Ast::BLOCK) {
        int line_number_index = line_number_table_attribute -> line_number_table.NextIndex();
        start = statement -> LeftToken();
        line_number_table_attribute -> line_number_table[line_number_index].line_number =
            this_semantic.lex_stream -> Line(start);
        line_number_table_attribute -> line_number_table[line_number_index].start_pc =
            code_attribute -> code.Length(); // pc at start of statement
    }
    stack_depth = 0; // stack empty at start of statement

    switch (statement -> kind) {
        case Ast::BLOCK:                        // JLS 13.2
            return EmitBlockStatement((AstBlock *) statement, 0);
            
        case Ast::LOCAL_VARIABLE_DECLARATION:// JLS 13.3
            {
            // generate code for local variable declaration.
                AstLocalVariableDeclarationStatement * lvds = statement -> LocalVariableDeclarationStatementCast();
                for (int i=0; i<lvds -> NumVariableDeclarators();i++) {
                    DeclareLocalVariable(lvds -> VariableDeclarator(i));
                }
            }
            break;
        case Ast::EMPTY_STATEMENT:          // JLS 13.5
            break;
        case Ast::EXPRESSION_STATEMENT:     // JLS 13.7
            EmitStatementExpression(statement -> ExpressionStatementCast() -> expression);
            break;
        case Ast::IF:                       // JLS 13.8
                {
            AstIfStatement * ifStatement = (AstIfStatement *) statement;
            if (ifStatement -> expression -> IsConstant()) {
                IntLiteralValue *if_constant_expr = (IntLiteralValue *) ifStatement -> expression -> value;
                // Open question (DS, 20 Jan 97): Should be we setting abrupt here? If we weren't
                // doing this optimization, then we wouldn't be setting abrupt. Note that if expression
                // is, for example, non-zero and then-block terminates abruptly, then the following
                // statement is unreachable.
                if (if_constant_expr -> value)
                     EmitStatement(ifStatement -> true_statement);
                else if (ifStatement -> false_statement_opt) // if there is false part
                     EmitStatement(ifStatement -> false_statement_opt);
            }
            else {
                if (ifStatement -> false_statement_opt) {   // if true and false parts
                    Label  label1;
                    Label  label2;
                    int true_abrupt; // set if last statement in true block is abrupt exit
                    int false_abrupt; // set if last statement in false block is abrupt exit
    
                    EmitBranchIfExpression(ifStatement -> expression, false, label1);
                    stack_depth = 0;
                    true_abrupt = EmitStatement(ifStatement -> true_statement);
                    if (!true_abrupt) {
                        EmitBranch(OP_GOTO,label2);
                    }
                    DefineLabel(label1);
                    false_abrupt =  EmitStatement(ifStatement -> false_statement_opt);
                    if (!true_abrupt) {
                        DefineLabel(label2);
                    }
                    CompleteLabel(label1);
                    CompleteLabel(label2);
                    // if terminates abruptly only if both clauses terminate abruptly
                    abrupt =  true_abrupt ? false_abrupt : 0;
                }
                else { // if no false part
                    Label label1;
                    EmitBranchIfExpression(ifStatement -> expression, false, label1);
                    stack_depth=0;
                    (void) EmitStatement(ifStatement -> true_statement);
                    DefineLabel(label1);
                    CompleteLabel(label1);
                }
            }
                }
        break;
        case Ast::SWITCH:                   // JLS 13.9

                EmitSwitchStatement(statement -> SwitchStatementCast());
        break;
        case Ast::SWITCH_BLOCK:             // JLS 13.9
        case Ast::CASE:                     // JLS 13.9
        case Ast::DEFAULT:                  // JLS 13.9
            // these nodes handled by SwitchStatement and
            // are not directly visited
            break;
        case Ast::WHILE:
                {               // JLS 13.10
            AstWhileStatement * wp = statement -> WhileStatementCast();
            // branch to continuation test. This test is placed after the
            // body of the loop we can fall through into it after each
            // loop iteration without the need for an additional branch.
            int while_depth = this_block_depth;
            EmitBranch(OP_GOTO,continue_labels[while_depth]);
            DefineLabel(begin_labels[while_depth]);
            (void) EmitStatement(wp -> statement);
            DefineLabel(continue_labels[while_depth]);
            stack_depth=0;
            EmitBranchIfExpression(wp -> expression,true,
                                 begin_labels[while_depth]);
            CompleteLabel(begin_labels[while_depth]);
            CompleteLabel(continue_labels[while_depth]);
                }
            break;
        case Ast::DO:                       // JLS 13.11
                {
            AstDoStatement * sp = statement -> DoStatementCast();
            int do_depth=this_block_depth;
            DefineLabel(begin_labels[do_depth]);
            (void) EmitStatement(sp -> statement);
            DefineLabel(continue_labels[do_depth]);
            stack_depth=0;
            EmitBranchIfExpression(sp -> expression,true,
                                 begin_labels[do_depth]);
            CompleteLabel(begin_labels[do_depth]);
            CompleteLabel(continue_labels[do_depth]);
                }

            break;
        case Ast::FOR:                      // JLS 13.12
                {
            int i;
            int for_depth;
            AstForStatement * forStatement = statement -> ForStatementCast();
            for_depth=this_block_depth;
            for (i=0; i<forStatement -> NumForInitStatements();i++) {
                EmitStatement((AstStatement *) forStatement -> ForInitStatement(i));
            }
            EmitBranch(OP_GOTO,test_labels[for_depth]);
            DefineLabel(begin_labels[for_depth]);
            (void) EmitStatement(forStatement -> statement);
            DefineLabel(continue_labels[for_depth]);
            for (i=0; i<forStatement -> NumForUpdateStatements();i++) {
                (void) EmitStatement((AstStatement *) forStatement -> ForUpdateStatement(i));
            }
            DefineLabel(test_labels[for_depth]);
            if (forStatement -> end_expression_opt) {
                stack_depth=0;
                EmitBranchIfExpression(forStatement -> end_expression_opt,true,
                                     begin_labels[for_depth]);
            }
            else  {
                EmitBranch(OP_GOTO,begin_labels[for_depth]);
            }
            CompleteLabel(begin_labels[for_depth]);
            CompleteLabel(test_labels[for_depth]);
            CompleteLabel(continue_labels[for_depth]);
                }
            break;
        case Ast::BREAK:                        // JLS 13.13
                ProcessAbruptExit(statement -> BreakStatementCast() -> nesting_level);
            EmitBranch(OP_GOTO,break_labels[statement -> BreakStatementCast() -> nesting_level]);
            abrupt = 1;
            break;
        case Ast::CONTINUE:                 // JLS 13.14
            ProcessAbruptExit(statement -> ContinueStatementCast() -> nesting_level);
            EmitBranch(OP_GOTO,continue_labels[statement -> ContinueStatementCast() -> nesting_level]);
            abrupt = 1;
            break;
        case Ast::RETURN:                   // JLS 13.15
            EmitReturnStatement(statement -> ReturnStatementCast());
            abrupt = 1;
            break;
        case Ast::SUPER_CALL: 
            EmitSuperInvocation((AstSuperCall *) statement);
            break;
        case Ast::THIS_CALL: 
            EmitThisInvocation((AstThisCall *) statement);
            break;
        case Ast::THROW:                        // JLS 13.16
            EmitExpression(statement -> ThrowStatementCast() -> expression);
            PutOp(OP_ATHROW);
            abrupt = 1;
            break;
        case Ast::SYNCHRONIZED_STATEMENT:   // JLS 13.17
            EmitSynchronizedStatement(statement -> SynchronizedStatementCast());
            break;
        case Ast::TRY:                      // JLS 13.18
            EmitTryStatement(statement -> TryStatementCast());
            break;
        case Ast::CLASS: // Class Declaration
        case Ast::INTERFACE: // InterfaceDeclaration
                // these are factored out by the front end; and so must be skipped here
            break;
        case Ast::CATCH:                    // JLS 13.18
        case Ast::FINALLY:                  // JLS 13.18
                // handled by TryStatement
        default:
            chaos("unknown statement kind");
            break;
    }
    return abrupt;
}
void ByteCode::EmitReturnStatement(AstReturnStatement * statement)
{
    int var_index=-1;
    int i;
    if (statement -> expression_opt == NULL) { // no return value
        ProcessAbruptExit(0);
        PutOp(OP_RETURN);
        return;
    }
    TypeSymbol * return_type = statement -> expression_opt-> Type();

    EmitExpression(statement -> expression_opt);
    if (method_type != return_type) {
      // no need to cast from reference to Object -- always ok
      if (method_type == this_control.Object() && IsReferenceType(return_type)) {
      }
      else {
          EmitCast(method_type, return_type);
      }
    }
    if (synchronized_blocks) {
    // if any outstanding synchronized blocks
    // find the index of the innermost enclosing block that is
    // synchronized. This block will have the variables allocated
    // for saving synchronization information.
        int synch_block=this_block_depth;
        for (i=this_block_depth;;i--) {
            if (is_synchronized[i]) {
            synch_block = i;
            break;
            }
            else if (i==0) {
              chaos("unable to find synchronization block");
            }
        }

        var_index=block_symbols[synch_block] -> synchronized_variable_index+2;;

//        if (this_control.IsDoubleWordType(return_type)) var_index--;

    }
    else if (finally_blocks) {
        for (i=this_block_depth;;i--) {
            if (has_finally_clause[i]) {
            var_index = has_finally_clause[i] - 1;
        var_index += 2; // move to start of area to save value 
            break;
            }
            else if (i==0) {
              chaos("unable to find finally block");
            }
        }
    }
    if (var_index >= 0) { // if need to save before abrupt exit 
        StoreLocal(var_index, method_type);
        ProcessAbruptExit(0);
        LoadLocal(var_index, method_type);
    }
    if (method_type != this_control.void_type) GenerateReturn(method_type);
}

int ByteCode::EmitBlockStatement(AstBlock *block, int synchronized)
{
    BlockSymbol *block_symbol = block -> block_symbol;
    int nesting_level = block -> nesting_level;

    int length=0;
    int start_pc;
    int save_depth = this_block_depth;
    int this_depth = nesting_level;
#ifdef TBSL 
    AstStatement * last_statement;
#endif  
    int i;
    int last_kind=0;

    this_block_depth =  nesting_level;
    is_synchronized[this_depth] = synchronized;
    synchronized_blocks += synchronized;
    block_symbols[this_depth] = block_symbol;
    start_pc = code_attribute -> code.Length();
    stack_depth = 0; // stack empty at start of statement

    if (this_depth>max_block_depth) {
        cout << "this_depth " << this_depth << "max " << max_block_depth << "\n";
        chaos("loops too deeply nested");
    }

    if (block -> NumStatements() > 0) {
    for (i=0; i<block -> NumStatements();i++){
        last_kind = EmitStatement((AstStatement *) block -> Statement(i));
    }
    if (this_block_depth != this_depth) {
      chaos("block depth out of synch!");
    }
//    this_block_depth = this_depth; // in case altered by one of above blocks
    }

    // if last statement is if_statement, must supply NOP to serve
    // as target for branch from true part
    // TODO - must refine this test so detect other cases where may
    // need following code, such as Switch, For, While; but
    // not if procedure returns void
    // code to end block (former end_block())
    // always define LABEL_BREAK at this point, and complete definition
    // of other labels

    if (IsLabelUsed(break_labels[this_depth])) {
        // need define only if used
        DefineLabel(break_labels[this_depth]);
    }
    CompleteLabel(begin_labels[this_depth]);
    CompleteLabel(break_labels[this_depth]);
    CompleteLabel(continue_labels[this_depth]);
    CompleteLabel(test_labels[this_depth]);
    CompleteLabel(begin_labels[this_depth]);
    if (is_synchronized[this_depth]) synchronized_blocks--;
//    if (this_block_depth) this_block_depth--;
#ifdef TBSL
        if (block_statements) {
    if (block -> NumStatements()) {
        last_statement = (AstStatement *) block -> Statement(block -> NumStatements()-1);
        if (last_statement -> IfStatementCast()) {
            PutNop(1);
        }
    }
    }
#endif  
    // compute local variable table and max variable number
    UpdateBlockInfo(block_symbol);
    this_block_depth = save_depth;
    return last_kind;
}
void ByteCode::EmitStatementExpression(AstExpression * expression)
{
    switch (expression -> kind) {
        case Ast::PARENTHESIZED_EXPRESSION:
                {
            AstParenthesizedExpression * pe =
                                              (AstParenthesizedExpression *) expression;   
            EmitStatementExpression(pe -> expression);
                }
                break;
        case Ast::CALL:
            (void) EmitMethodInvocation((AstMethodInvocation *)expression, 0);
            if (expression-> Type()!=this_control.void_type) {
                if (this_control.IsDoubleWordType(expression-> Type())) {
                    PutOp(OP_POP2);
                }
                else {
                    PutOp(OP_POP); // discard value if used as statement
                }
            }
            break;
        case Ast::POST_UNARY:
            (void) EmitPostUnaryExpression((AstPostUnaryExpression *)expression,0);
            break;
        case Ast::PRE_UNARY:
            (void) EmitPreUnaryExpression((AstPreUnaryExpression *)expression,0);
            break;
        case Ast::ASSIGNMENT:
            EmitAssignmentExpression((AstAssignmentExpression *)expression,0);
            break;
    case Ast::CLASS_CREATION:
        (void) EmitClassInstanceCreationExpression((AstClassInstanceCreationExpression *) expression, 0);
            break;
        default:
        chaos("invalid statement expression kind");
    }
}
void ByteCode::EmitSwitchStatement(AstSwitchStatement * sws)
{
    // generate code for switch statement. Good code generation requires
    // detailed knowledge of the target machine. Lacking this, we simply
    // choose between LOOKUPSWITCH and TABLESWITCH by picking that
    // opcode that takes the least number of bytes in the byte code.
    int use_lookup=0; // set if using LOOKUPSWITCH opcode
    // compare sizes os generated opcodes, ignoring size of common
    // header (opcode through default bytes).
    int high,low,ncases,switch_depth;
    int has_default;
    int op_start;
    int i;
    int nlabels;
    int map_index;
    int label_index;
    int start_pc;
    int ci;
    int di;
    AstBlock *switch_block=sws -> switch_block;
    BlockSymbol * block_symbol = switch_block -> block_symbol;
    switch_depth = sws -> switch_block -> nesting_level;
    EmitBlockStatement(switch_block, 0);
    // We need only worry about the presence of a default case if there
    // is code for it.
    start_pc = code_attribute -> code.Length();
    has_default = sws -> default_case.switch_block_statement!=NULL ? 1 : 0;
    ncases = sws -> map.Length();
    // Use tableswitch if have exact match or size of tableswitch
    // case is no more than 30 bytes more code than lookup case
    use_lookup=1;
    nlabels=ncases;

    if (ncases) {
      low = sws -> map[0] -> Value();
      high = sws -> map[ncases-1] -> Value();
      // want to compute
      //  (2+high-low+1)<(1+ncases*2+30
      // but must guard against overflow, so factor out
      //  high - low < ncases*2 + 28
      // but can't have number of labels < number of cases
      if ((high-low) < (ncases*2+28)) {
        use_lookup = 0;     // use tableswitch
        nlabels = high-low + 1;
      }
      // correct estimate in case above computation gave ridiculous number
      // for the number of labels. (This is problem 096)
      if (nlabels < ncases) {
    use_lookup = 1;
    nlabels = ncases;
      }
    }

    EmitExpression(sws -> expression);
    stack_depth=0;
    PutOp(use_lookup ? OP_LOOKUPSWITCH : OP_TABLESWITCH);
    op_start = last_op_pc;  // pc at start of instruction
    // supply any needed padding
    while(code_attribute -> code.Length() % 4) {
        PutNop(0);
    }  
    // note that if no default clause in switch statement, must allocate
    // one that corresponds to do nothing and branches to start of next
    // statement.
    Label default_label;
    UseLabel(has_default ? default_label :
              break_labels[switch_depth],
              4,code_attribute -> code.Length()-op_start);

    if (use_lookup) PutU4(ncases);
    else {
        PutU4(low);
        PutU4(high);
    }
    // how to allocate for proper length
    Label *case_labels = new Label[(use_lookup ? ncases : nlabels) + 1];

    if (use_lookup == 0) {
        int *has_tag = new int[nlabels + 1];
        for (i = 0; i < nlabels; i++) has_tag[i] = 0;
        // mark cases for which no case tag available, i.e., default cases

        for (i=0;i<sws -> switch_block -> NumStatements();i++) {
            int li;

            AstSwitchBlockStatement * sbs = (AstSwitchBlockStatement *)
                                             sws -> switch_block -> Statement(i);
            // process labels for this block
            for (li=0;li<sbs -> NumSwitchLabels();li++) {
                if (sbs -> SwitchLabel(li) -> CaseLabelCast()) {
                    map_index =sbs -> SwitchLabel(li) -> CaseLabelCast() -> map_index;
                    di =sws -> map[map_index] -> index;
                    //                  label_index = sws -> map[di] -> Value() - low;
                    label_index = sws -> map[map_index] -> Value() - low;
                    has_tag[label_index]=1;

                }
            }
        }
        // now emit labels in instruction, using appropriate index
        for (i=0;i<nlabels;i++) {
            UseLabel(has_tag[i]  ? case_labels[i] :
                      has_default ? default_label 
                                  : break_labels[switch_depth],
                      4,code_attribute -> code.Length()-op_start);
        }

        delete [] has_tag;
    }
    else {
        for (i=0;i<ncases;i++) {
            PutU4(sws -> map[i] -> Value());
            UseLabel(case_labels[sws -> map[i] -> index],4,code_attribute -> code.Length()-op_start);
        }
    }
    // march through switch block statements, compiling blocks in
    // proper order. We must respect order in which blocks seen
    // so that blocks lacking a terminal break fall through to the
    // proper place.
    for (i=0;i<sws -> switch_block -> NumStatements();i++) {
        int li;

        AstSwitchBlockStatement * sbs = (AstSwitchBlockStatement *)
                                         sws -> switch_block -> Statement(i);
        // process labels for this block
        for (li=0;li<sbs -> NumSwitchLabels();li++) {
            if (sbs -> SwitchLabel(li) -> CaseLabelCast()) {
                map_index =sbs -> SwitchLabel(li) -> CaseLabelCast() -> map_index;
                if (use_lookup) {
                    DefineLabel(case_labels[map_index]);
                }
                else {
                    int found=0;
                    // look for case with same index and
                    // use its value to find label index.
                    for (int di2=0;di2<sws -> map.Length();di2++) {
                        if (sws -> map[di2] -> index==map_index) {
                            ci = sws -> map[di2] -> Value()-low;
                            DefineLabel(case_labels[ci]);
                            found=1;
                            break;
                        }
                    }
                    if (found==0) 
                        chaos("unable to find case label");
                }
            }
            else {
                if (sbs -> SwitchLabel(li) -> DefaultLabelCast()) {
                    if (has_default) {
                        DefineLabel(default_label);
                    }
                    else {
                        chaos("error in processing default label");
                    }
                }
            }
        }
        // compile code for this case
        for (li=0;li<sbs -> NumStatements();li++) {
            (void) EmitStatement(sbs -> Statement(li) -> StatementCast());
        }
    }
    UpdateBlockInfo(block_symbol);
    for (i=0;i<nlabels;i++) {
        if (case_labels[i].uses.Length() && case_labels[i].defined==0) {
            case_labels[i].defined=1;
        if (has_default) {
            case_labels[i].definition=default_label.definition;
        }
        else {
            case_labels[i].definition = break_labels[switch_depth].definition;
        }
        }
        CompleteLabel(case_labels[i]);
    }

    if (has_default) {
        CompleteLabel(default_label);
    }
    // define target of break label
    if (IsLabelUsed(break_labels[switch_depth])) {                   // need define only if used
        DefineLabel(break_labels[switch_depth]);
    }
    for (i=0;i<nlabels;i++) {
        if (case_labels[i].uses.Length() && case_labels[i].defined==0) {
            case_labels[i].defined=1;
        if (has_default) {
            case_labels[i].definition=default_label.definition;
        }
        else {
            case_labels[i].definition = break_labels[switch_depth].definition;
        }
        }
        CompleteLabel(case_labels[i]);
    }

    if (has_default) {
        CompleteLabel(default_label);
    }
    // define target of break label
    if (IsLabelUsed(break_labels[switch_depth])) {               // need define only if used
        CompleteLabel(break_labels[switch_depth]);
    }


    // note that if using table, then must provide slot for every
    // entry in the range low..high, even though the user may not
    // have provided an explicit entry, in which case the default
    // action is to be taken. For example
    // switch (e) {
    //  case 1:2:3: act1; break;
    //  case 5:6:   act2; break;
    //  default: defact; break;
    // }
    // translated as
    // switch (e)
    // switch (e) {
    //  case 1:2:3: act1; break;
    //  case 4: goto defa:
    //  case 5:6:   act2; break;
    //  defa:
    //  default: defact; 
    // }

    delete [] case_labels;
}



//  13.18       The try statement
void ByteCode::EmitTryStatement(AstTryStatement * statement)
{
    AstFinallyClause * finally_clause;
    int start_pc,end_pc;
    int exception_index,handler_pc;
    Label end_label;
    AstCatchClause * catch_clause;
    int last_abrupt=0; // set if last statement in try block is abrupt exit
    int try_depth = statement -> block -> nesting_level - 1;;
    int final_depth = try_depth + 1;
    BlockSymbol * block_symbol;
    start_pc = code_attribute -> code.Length(); // start pc
    if (statement -> finally_clause_opt) { 
    // call finally block if have finally handler
assert(block_symbols[try_depth]);
        block_symbol = block_symbols[try_depth] -> BlockCast();
        finally_clause = statement -> finally_clause_opt;
        has_finally_clause[final_depth]=1 + block_symbol -> try_variable_index;
        finally_blocks++;
    }
    last_abrupt = EmitStatement(statement -> block);
    // increment max_stack in case exception thrown while stack at greatest depth
    code_attribute -> max_stack++;
    if (statement -> finally_clause_opt) { // call finally block if have finally handler
        PutOp(OP_JSR);
        UseLabel(final_labels[final_depth], 2, 1);
    }
    
    if (!last_abrupt) {
        EmitBranch(OP_GOTO,end_label);
    }
    PutNop(0); // emit NOP so end_pc will come out right
    end_pc = last_op_pc;
// process catch clauses
    int catch_abrupt=0;
    for (int i = 0; i<statement -> NumCatchClauses();i++) {
        catch_clause = statement -> CatchClause(i);
        VariableSymbol * parameter_symbol = catch_clause -> parameter_symbol;
        handler_pc = code_attribute -> code.Length();
        StoreLocalVariable(parameter_symbol);
        catch_abrupt = EmitStatement(catch_clause -> block);
        
        exception_index = code_attribute -> exception_table.NextIndex();
        code_attribute -> exception_table[exception_index].start_pc = start_pc;
        // add 1 to end_pc since it is exclusive, while start_pc
        // is inclusive. See footnote in 4.4.4 of JVM Specification
        //code_attribute -> exception_table[exception_index].end_pc = end_pc + 1; // DS 11 feb
        code_attribute -> exception_table[exception_index].end_pc = end_pc; // DS 11 feb
        code_attribute -> exception_table[exception_index].handler_pc = handler_pc;
        code_attribute -> exception_table[exception_index].catch_type =
            RegisterClass(parameter_symbol-> Type() -> fully_qualified_name);
        if (statement -> finally_clause_opt) { // call finally block if have finally handler
            PutOp(OP_JSR);
            UseLabel(final_labels[final_depth], 2, 1);
        }
        if (! catch_abrupt) {
                EmitBranch(OP_GOTO,end_label);
        }
    }

    
    if (statement -> finally_clause_opt) {
        handler_pc = code_attribute -> code.Length();
        has_finally_clause[final_depth] = 0; // reset once finally clause processed
        finally_blocks--;

        // handler for finally()

        // push thrown value
        StoreLocal(block_symbol -> try_variable_index, this_control.Object());
        PutOp(OP_JSR);
        UseLabel(final_labels[final_depth], 2, 1);
        LoadLocal(block_symbol -> try_variable_index, this_control.Object());
        PutOp(OP_ATHROW);  // and rethrow the value to the invoker

        DefineLabel(final_labels[final_depth]);
        CompleteLabel(final_labels[final_depth]);
        StoreLocal(block_symbol -> try_variable_index+1, this_control.Object()); // save return address
        (void) EmitStatement(statement -> finally_clause_opt -> block);
        PutOp(OP_RET);
        PutU1(block_symbol -> try_variable_index + 1); // return using saved address
        exception_index = code_attribute -> exception_table.NextIndex();
        code_attribute -> exception_table[exception_index].start_pc = start_pc;
        code_attribute -> exception_table[exception_index].end_pc = handler_pc;
        code_attribute -> exception_table[exception_index].handler_pc = handler_pc;
        code_attribute -> exception_table[exception_index].catch_type = 0;
    }

    if (IsLabelUsed(end_label)) {
        DefineLabel(end_label);
        CompleteLabel(end_label);
        PutNop(1); // to make sure have code after end of try statement
    }
    else {
        CompleteLabel(end_label);
    }
}

void ByteCode::UpdateBlockInfo(BlockSymbol * block_symbol)
{
    int i;

    if (code_attribute -> max_locals<block_symbol -> max_variable_index) {
        code_attribute -> max_locals = block_symbol -> max_variable_index;
    }
    if (block_symbol == (BlockSymbol *) 0) {
      chaos("null block symbol");
    }
      

    if (this_control.option.g) {
    // compute local variable table
        for (i = 0; i < block_symbol -> NumVariableSymbols();i++) {
            VariableSymbol * sym = block_symbol -> VariableSym(i);
        // only make entry if defined within range
        if (last_op_pc > sym -> local_program_counter) {
                AddLocalVariableTableEntry(sym -> local_program_counter, 
            last_op_pc - sym -> local_program_counter,
                    RegisterUtf8(sym -> ExternalIdentity() -> Utf8_literal),
                    RegisterUtf8(sym-> Type() -> signature),
                    sym -> LocalVariableIndex());
        }
        }
    }
}

void ByteCode::ProcessAbruptExit(int to_lev)
{
    // exit to block at level lev, freeing monitor locks and invoking finally clauses as appropriate
    for (int lev=this_block_depth;lev>to_lev;lev--) {
        if (has_finally_clause[lev]) {
            PutOp(OP_JSR);
            UseLabel(final_labels[lev], 2, 1);
        }
        else if (is_synchronized[lev]) {
            PutOp(OP_JSR);
            UseLabel(monitor_labels[lev], 2, 1);
        }
    }

}

void ByteCode::EmitBranch(unsigned int opc, Label& lab) {
    // generate branch
    PutOp(opc);
    UseLabel(lab,2,1);
}


void ByteCode::EmitBranchIfExpression(AstExpression *p, bool cond,Label &lab)
{
    // java provides a variety of conditional branch instructions, so
    // that a number of operators merit special handling:
    //      constant operand
    //      negation (we eliminate it)
    //      equality
    //      && and || (partial evaluation)
    //      comparisons
    // Other expressions are just evaluated and the appropriate
    // branch emitted.
    AstExpression *left,*right,*temp;
    AstPreUnaryExpression *pre;
    TypeSymbol *left_type, *right_type;

    if (p -> ParenthesizedExpressionCast()) {
        p = UnParenthesize(p);
    }
    if (p -> IsConstant()) {
        if (IsZero(p)) {
            if (cond == false)  EmitBranch(OP_GOTO,lab);
        }
        else {
            if (cond == true)  EmitBranch(OP_GOTO,lab);
        }
        return;
    }
    pre = p -> PreUnaryExpressionCast();
    if (pre) { // must be !, though should probably

        // branch_if(!e,c,l) => branch_if(e,!c,l)
        // test opcode
        // call again with complementary control expression to show
        // effect of negation
        if (pre -> pre_unary_tag == AstPreUnaryExpression::NOT) {
            EmitBranchIfExpression(pre -> expression,cond == true ? false : true, lab);
            return;
        }
        else {
            chaos("branch_if expects ! in this context");
        }
    }
    // dispose of non-binary expression case by just evaluating
    // operand and emitting appropiate test.
    if (!(p -> BinaryExpressionCast())) {
        EmitExpression(p);
        EmitBranch(cond == true ? OP_IFNE : OP_IFEQ,lab);
        return;
    }
    // Here if binary expression, so extract operands
    AstBinaryExpression * bp = (AstBinaryExpression *) p;
    left = bp -> left_expression;
    if (left -> ParenthesizedExpressionCast()) {
        left = UnParenthesize(left);
    }
    right = bp -> right_expression;
    if (right -> ParenthesizedExpressionCast()) {
        right = UnParenthesize(right);
    }
    left_type = left-> Type();
    right_type = right-> Type();
    switch (bp -> binary_tag) {
        case AstBinaryExpression::INSTANCEOF:
                {
            EmitExpression(left);
            PutOp(OP_INSTANCEOF);
        TypeSymbol * instanceof_type = bp -> right_expression-> Type();
        if (instanceof_type -> num_dimensions) {
            PutU2(RegisterClass(instanceof_type -> signature));
        }
        else {
            PutU2(RegisterClass(instanceof_type -> fully_qualified_name));
        }
            EmitBranch(cond == true? OP_IFNE : OP_IFEQ,lab);
                }
                return;
        case AstBinaryExpression::AND_AND:
                /*
                branch_if(a&&b, true, lab) =>
                branch_if(a,false,skip);
                branch_if(b,true,lab);
                skip:
                branch_if(a&&b, false, lab) =>
                branch_if(a,false,lab);
                branch_if(b,false,lab);
*/
                if (cond == true) {
            Label skip;
            EmitBranchIfExpression(left, false, skip);
            EmitBranchIfExpression(right, true, lab);
            DefineLabel(skip);
            CompleteLabel(skip);
        }
                else {
                    EmitBranchIfExpression(left, false, lab);
                    EmitBranchIfExpression(right, false, lab);
                }
                return;
        case AstBinaryExpression::OR_OR:
                /*
                branch_if(a||b,true,lab) =>
                branch_if(a,true,lab);
                branch_if(b,true,lab);
                branch_if(a||b,false,lab) =>
                branch_if(a,true,skip);
                branch_if(b,false,lab);
                There is additional possibility of one of the operands being
                constant that should be dealt with at some point.
*/
                if (cond == true) {
            EmitBranchIfExpression(left, true, lab);
            EmitBranchIfExpression(right, true, lab);
        }
                else {
                    Label skip;  
                    EmitBranchIfExpression(left, true, skip);
                    EmitBranchIfExpression(right, false, lab);
                    DefineLabel(skip);
                    CompleteLabel(skip);
                }
                return;
        case AstBinaryExpression::EQUAL_EQUAL:
        case AstBinaryExpression::NOT_EQUAL:
                // see if test against null
                if (left_type == this_control.null_type || right_type==this_control.null_type) {
            // arrange so right operand is null
            if (left_type == this_control.null_type) {
                temp = left;left = right;right = temp;
                left_type = left-> Type(); right_type = right-> Type();
            }
            EmitExpression(left);
            if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL) {
                EmitBranch(cond == true? OP_IFNULL : OP_IFNONNULL,lab);
            }
            else {
                EmitBranch(cond == true ? OP_IFNONNULL : OP_IFNULL,lab);
            }
            return;
        }
                // see if test against integer zero
                if (IsZero(left) || IsZero(right)) {
                    // arrange so right operand is zero
                    if (IsZero(left)) { 
                        temp = left;left = right;right = temp;
                        left_type = left-> Type();right_type = right-> Type();
                    } 
                    EmitExpression(left);
                    if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL) {
                        EmitBranch(cond == true? OP_IFEQ : OP_IFNE,lab);
                        return;
                    }
                    else {
                        EmitBranch(cond == true ? OP_IFNE : OP_IFEQ,lab);
                    }
                    return;
                }
                // see if test of integers
                if ((this_control.IsSimpleIntegerValueType(left_type)||left_type==this_control.boolean_type) && 
                    (this_control.IsSimpleIntegerValueType(right_type)
                     || right_type == this_control.boolean_type) ) {
                    EmitExpression(left);
                    EmitExpression(right);
                    if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL) {
                        EmitBranch(cond ? OP_IF_ICMPEQ : OP_IF_ICMPNE,lab);
                        return;
                    }
                    else {
                        EmitBranch(cond ? OP_IF_ICMPNE : OP_IF_ICMPEQ,lab);
                    }
                    return;
                }
                else if (IsReferenceType(left_type) && IsReferenceType(right_type)) {
                    // else just do the comparison
                    EmitExpression(left);
                    EmitExpression(right);
                    if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL) {
                        EmitBranch(cond ? OP_IF_ACMPEQ : OP_IF_ACMPNE,lab);
                    }
                    else {
                        EmitBranch(cond ? OP_IF_ACMPNE : OP_IF_ACMPEQ,lab);
                    }
                    return;
                }
    }

    // here if not comparison, comparison for non-integral numeric types, or
    // integral comparison for which no special casing needed.
    // Begin by dealing with non-comparisons
    switch(bp -> binary_tag) {
        case AstBinaryExpression::LESS:
        case AstBinaryExpression::LESS_EQUAL:
        case AstBinaryExpression::GREATER:
        case AstBinaryExpression::GREATER_EQUAL:
        case AstBinaryExpression::EQUAL_EQUAL:
        case AstBinaryExpression::NOT_EQUAL:
                break; // break to continue comparison processing
        default:
            // not a comparison, get the (necessarily boolean) value
            // of the expression and branch on the result
            EmitExpression(p);
            EmitBranch(cond ? OP_IFNE : OP_IFEQ, lab);
            return;
    }
    unsigned opcode = 0;
    unsigned int op_true, op_false;
    if (this_control.IsSimpleIntegerValueType(left_type)
        || left_type == this_control.boolean_type) {
        // we have already dealt with EQUAL_EQUAL and NOT_EQUAL for
        // integers, but still need to look for comparisons for which
        // one operand may be zero.
        if (IsZero(left)) {
            EmitExpression(right);
            switch(bp -> binary_tag) {
                case AstBinaryExpression::LESS: // if (0<x) same as  if (x>0)
                    op_true = OP_IFGT; op_false = OP_IFLE; break;
                case AstBinaryExpression::LESS_EQUAL:  // if (0<=x) same as if (x>=0)
                    op_true = OP_IFGE; op_false = OP_IFLT; break;
                case AstBinaryExpression::GREATER:  // if (0>x) same as if (x<0)
                    op_true = OP_IFLT; op_false = OP_IFGE; break;
                case AstBinaryExpression::GREATER_EQUAL: // if (0>=x) same as if (x<=0)
                    op_true = OP_IFLE; op_false = OP_IFGT; break;
            }
        }
        else if (IsZero(right)) {
            EmitExpression(left);
            switch(bp -> binary_tag) {
                case AstBinaryExpression::LESS:
                    op_true = OP_IFLT; op_false = OP_IFGE; break;
                case AstBinaryExpression::LESS_EQUAL:
                    op_true = OP_IFLE; op_false = OP_IFGT; break;
                case AstBinaryExpression::GREATER:
                    op_true = OP_IFGT; op_false = OP_IFLE; break;
                case AstBinaryExpression::GREATER_EQUAL:
                    op_true = OP_IFGE; op_false = OP_IFLT; break;
            }
        }
        else {
            EmitExpression(left);
            EmitExpression(right);
            switch(bp -> binary_tag) {
                case AstBinaryExpression::LESS:
                    op_true = OP_IF_ICMPLT; op_false = OP_IF_ICMPGE; break;
                case AstBinaryExpression::LESS_EQUAL:
                    op_true = OP_IF_ICMPLE; op_false = OP_IF_ICMPGT; break;
                case AstBinaryExpression::GREATER:
                    op_true = OP_IF_ICMPGT; op_false = OP_IF_ICMPLE; break;
                case AstBinaryExpression::GREATER_EQUAL:
                    op_true = OP_IF_ICMPGE; op_false = OP_IF_ICMPLT; break;
            }
        }
    }
    else if (left_type == this_control.long_type) {
        EmitExpression(left);
        EmitExpression(right);
        opcode = OP_LCMP;
        // branch according to result value on stack
        switch (bp -> binary_tag) {
            case AstBinaryExpression::EQUAL_EQUAL:
                op_true = OP_IFEQ; op_false = OP_IFNE; break;
            case AstBinaryExpression::NOT_EQUAL:
                op_true = OP_IFNE; op_false = OP_IFEQ; break;
            case AstBinaryExpression::LESS:
                op_true = OP_IFLT; op_false = OP_IFGE; break;
            case AstBinaryExpression::LESS_EQUAL:
                op_true = OP_IFLE; op_false = OP_IFGT; break;
            case AstBinaryExpression::GREATER:
                op_true = OP_IFGT; op_false = OP_IFLE; break;
            case AstBinaryExpression::GREATER_EQUAL:
                op_true = OP_IFGE; op_false = OP_IFLT; break;
        }
    }
    else if (left_type == this_control.float_type) {
        EmitExpression(left);
        EmitExpression(right);
        switch (bp -> binary_tag) {
            case AstBinaryExpression::EQUAL_EQUAL:
                opcode = OP_FCMPL; op_true = OP_IFEQ; op_false = OP_IFNE; break;
            case AstBinaryExpression::NOT_EQUAL:
                opcode = OP_FCMPL; op_true = OP_IFNE; op_false = OP_IFEQ; break;
            case AstBinaryExpression::LESS:
                opcode = OP_FCMPG; op_true = OP_IFLT; op_false = OP_IFGE; break;
            case AstBinaryExpression::LESS_EQUAL:
                opcode = OP_FCMPG; op_true = OP_IFLE; op_false = OP_IFGT; break;
            case AstBinaryExpression::GREATER:
                opcode = OP_FCMPL; op_true = OP_IFGT; op_false = OP_IFLE; break;
            case AstBinaryExpression::GREATER_EQUAL:
                opcode = OP_FCMPL; op_true = OP_IFGE; op_false = OP_IFLT; break;
        }
    }
    else if (left_type == this_control.double_type) {
        EmitExpression(left);
        EmitExpression(right);
        switch (bp -> binary_tag) {
            case AstBinaryExpression::EQUAL_EQUAL:
                opcode = OP_DCMPL; op_true = OP_IFEQ; op_false = OP_IFNE; break;
            case AstBinaryExpression::NOT_EQUAL:
                opcode = OP_DCMPL; op_true = OP_IFNE; op_false = OP_IFEQ; break;
            case AstBinaryExpression::LESS:
                opcode = OP_DCMPG; op_true = OP_IFLT; op_false = OP_IFGE; break;
            case AstBinaryExpression::LESS_EQUAL:
                opcode = OP_DCMPG; op_true = OP_IFLE; op_false = OP_IFGT; break;
            case AstBinaryExpression::GREATER:
                opcode = OP_DCMPL; op_true = OP_IFGT; op_false = OP_IFLE; break;
            case AstBinaryExpression::GREATER_EQUAL:
                opcode = OP_DCMPL; op_true = OP_IFGE; op_false = OP_IFLT; break;
        }
    }
    else {
        chaos("comparison of unsupported type");
    }
    if (opcode) PutOp(opcode); // if need to emit comparison before branch
    EmitBranch (cond ? op_true: op_false, lab);
}

void ByteCode::EmitSynchronizedStatement(AstSynchronizedStatement * statement)
{
    int var_index; // local variable index to save address of object
    int loc_index; // local variable index to save address
    int start_pc,end_pc;
    int exception_index,handler_pc;
    Label end_label;
    var_index = statement -> block -> block_symbol -> synchronized_variable_index;
    loc_index = var_index+1;
    EmitExpression(statement -> expression);
    StoreLocal(var_index, this_control.Object()); // save address of object
    LoadLocal(var_index, this_control.Object()); // load address of object onto stack

    PutOp(OP_MONITORENTER); // enter monitor associated with object
    start_pc = code_attribute -> code.Length(); // start pc
    (void) EmitBlockStatement(statement -> block, 1);
    LoadLocal(var_index, this_control.Object()); // load address of object onto stack
    PutOp(OP_MONITOREXIT);
    if (statement -> block -> NumStatements() > 0) {
    end_pc = last_op_pc;
    EmitBranch(OP_GOTO,end_label); // branch around exception handler
    // reach here if any
    // increment max_stack in case exception thrown while stack at greatest depth
    code_attribute -> max_stack++;
    handler_pc = code_attribute -> code.Length();
    LoadLocal(var_index, this_control.Object()); // load address of object onto stack
    PutOp(OP_MONITOREXIT); 
    PutOp(OP_ATHROW);
    exception_index = code_attribute -> exception_table.NextIndex();
    code_attribute -> exception_table[exception_index].start_pc = start_pc;
    code_attribute -> exception_table[exception_index].end_pc = handler_pc;
    code_attribute -> exception_table[exception_index].handler_pc = handler_pc;
    code_attribute -> exception_table[exception_index].catch_type = 0;
    DefineLabel(monitor_labels[statement -> block -> nesting_level]);
    CompleteLabel(monitor_labels[statement -> block -> nesting_level]);
    StoreLocal(loc_index, this_control.Object()); // save return address
    LoadLocal(var_index, this_control.Object()); // load address of object onto stack
    PutOp(OP_MONITOREXIT); 
    PutOp(OP_RET);
    PutU1(loc_index);  // return using saved address
    DefineLabel(end_label);
    CompleteLabel(end_label);
//    EmitNop(1); // guarantee some PutOp after block in case
    // the synchronized statement is the last in the procedure.
    }
}

// JLS is Java Language Specification
// JVM is Java Virtual Machine
//
// Expressions: Chapter 14 of JLS

int ByteCode::EmitExpression(AstExpression *expression) 
{
    if (expression -> IsConstant()) {
        return LoadConstant(expression);
    }
    switch (expression -> kind) {
        case Ast::IDENTIFIER:
            if (expression -> SimpleNameCast() && expression -> SimpleNameCast() -> resolution_opt) {
                return EmitExpression(expression -> SimpleNameCast() -> resolution_opt);
            }
            return LoadSimple(GetLHSKind(expression, (MethodSymbol *)0),expression);
        case Ast::INTEGER_LITERAL:
        case Ast::LONG_LITERAL:
        case Ast::FLOATING_POINT_LITERAL:
        case Ast::DOUBLE_LITERAL:
        case Ast::TRUE_LITERAL:
        case Ast::FALSE_LITERAL:
        case Ast::STRING_LITERAL:
        case Ast::CHARACTER_LITERAL:
        return LoadConstant(expression);
        case Ast::THIS_EXPRESSION:
        case Ast::SUPER_EXPRESSION:
            PutOp(OP_ALOAD_0); // must be use
            return 1;
        case Ast::PARENTHESIZED_EXPRESSION:
            {
            AstParenthesizedExpression * pe =
                    (AstParenthesizedExpression *) expression; 
            return EmitExpression(expression -> ParenthesizedExpressionCast() -> expression);
            }
        case Ast::CLASS_CREATION:
            return EmitClassInstanceCreationExpression((AstClassInstanceCreationExpression *)expression, 1);
        case Ast::ARRAY_CREATION:
            return EmitArrayCreationExpression((AstArrayCreationExpression *)expression);
        case Ast::DIM:
            return EmitExpression(expression -> DimExprCast() -> expression);
        case Ast::DOT:
        {
                AstFieldAccess * field_access =(AstFieldAccess *)expression;
                 return ((field_access -> IsClassAccess()) && (field_access -> resolution_opt)) ? GenerateClassAccess(field_access)
                         : EmitFieldAccess(field_access);
        }
        case Ast::CALL:
            return EmitMethodInvocation((AstMethodInvocation *)expression, 0);
        case Ast::ARRAY_ACCESS:             // if seen alone this will be as RHS
            return EmitArrayAccessRHS((AstArrayAccess *) expression);
        case Ast::POST_UNARY:
            return EmitPostUnaryExpression((AstPostUnaryExpression *)expression,1);
        case Ast::PRE_UNARY:
            return EmitPreUnaryExpression((AstPreUnaryExpression *)expression,1);
        case Ast::CAST:
        {
          AstCastExpression * cast_expression = expression -> CastExpressionCast();
            if (cast_expression -> expression-> Type() -> Primitive())
            {
             // primitive types require casting
              return EmitCastExpression(cast_expression);
            }
            else {
              // not need to cast, just evaluate operand
                return EmitExpression(cast_expression -> expression); 
            }
        }

        case Ast::CHECK_AND_CAST:
            return EmitCastExpression((AstCastExpression *)expression);
        case Ast::BINARY:
            return EmitBinaryExpression((AstBinaryExpression *)expression);
        case Ast::CONDITIONAL:
            return EmitConditionalExpression((AstConditionalExpression *)expression);
        case Ast::ASSIGNMENT:
            return EmitAssignmentExpression((AstAssignmentExpression *)expression,1);
        case Ast::NULL_LITERAL:
            PutOp(OP_ACONST_NULL);
            return 1;
        default:
            chaos("unknown expression kind");
            return 0; // even tho will not reach here
    }
}
void ByteCode::EmitArrayAccessLHS(AstArrayAccess *expression) 
{
    LoadReference(expression -> base);
    EmitExpression(expression -> expression);
}
int ByteCode::EmitArrayAccessRHS(AstArrayAccess *expression) 
{
    EmitArrayAccessLHS(expression); // get array address and index
    return LoadArrayElement(expression-> Type());
}

void ByteCode::EmitFieldAccessLHSBase(AstExpression * expression) {
    AstFieldAccess * field;
    AstSimpleName * simple_name;
    field = expression -> FieldAccessCast();
    if (field){
        if (field -> resolution_opt) {
           expression = field -> resolution_opt;
        }
    }
    else if (expression -> SimpleNameCast()) {
        simple_name = expression -> SimpleNameCast();
        if (simple_name -> resolution_opt) {
            expression = simple_name -> resolution_opt;
        }
    }
    VariableSymbol * sym = (VariableSymbol *) expression -> symbol;
    field = expression -> FieldAccessCast();
    if (field){
        EmitExpression(field -> base);
    }
    else if (expression -> SimpleNameCast()) {
        PutOp(OP_ALOAD_0); // get address of "this"
    }
    else {
        chaos("unexpected AssignmentExpressionField operand base type");
    }
}

void ByteCode::EmitFieldAccessLHS(AstExpression * expression)
{
    EmitFieldAccessLHSBase(expression);
    PutOp(OP_DUP);     // save base address of field for later store
    PutOp(OP_GETFIELD);
    ChangeStack(this_control.IsDoubleWordType(expression-> Type()) ? 1: 0);
    PutU2(GenerateFieldReference((VariableSymbol *) expression -> symbol));
}
void ByteCode::GenerateClassAccessMethod(MethodSymbol * msym) {
// generate code for access method used to set class literal fields
    /* The code takes the form
        aload_0          load this
        invokestatic  java/lang/Class.forName(String)java/lang/Class
        areturn         return Class object for the class named by string
        // exception handler if forName fails
        astore_1        save exception
        new java.lang.NoClassDefFoundError
        dup             save so can return
        aload_1         recover exception
        invokevirtual java.lang.Throwable.getMessage() to get error message
        invokenonvirtual <init>     // invoke initializer
        athrow          rethrow the exception
    */
    
    code_attribute -> max_locals = 2;
    PutOp(OP_ALOAD_0);
    PutOp(OP_INVOKESTATIC);
    ChangeStack(-1);
    PutU2(BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_Class, strlen(U8S_java_SL_lang_SL_Class)),
        BuildNameAndType(RegisterUtf8(U8S_forName, strlen(U8S_forName)),
                          RegisterUtf8(U8S_LP_Ljava_SL_lang_SL_String_SC_RP_Ljava_SL_lang_SL_Class_SC,
                                        strlen(U8S_LP_Ljava_SL_lang_SL_String_SC_RP_Ljava_SL_lang_SL_Class_SC)))));
    ChangeStack(1);

    PutOp(OP_ARETURN);
    PutOp(OP_ASTORE_1);
    PutOp(OP_NEW);
    PutU2(RegisterClass(U8S_java_SL_lang_SL_NoClassDefFoundError, strlen(U8S_java_SL_lang_SL_NoClassDefFoundError)));
    PutOp(OP_DUP);
    PutOp(OP_ALOAD_1);
    PutOp(OP_INVOKEVIRTUAL);
    ChangeStack(-1);
    PutU2(BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_Throwable, strlen(U8S_java_SL_lang_SL_Throwable)),
        BuildNameAndType(
            RegisterUtf8(U8S_getMessage, strlen(U8S_getMessage)),
            RegisterUtf8(U8S_LP_RP_Ljava_SL_lang_SL_String_SC, strlen(U8S_LP_RP_Ljava_SL_lang_SL_String_SC)))));
    ChangeStack(1);

    PutOp(OP_INVOKENONVIRTUAL);
    ChangeStack(-1);
    PutU2(BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_NoClassDefFoundError, strlen(U8S_java_SL_lang_SL_NoClassDefFoundError)),
         BuildNameAndType(
             RegisterUtf8(U8S_LT_init_GT_, strlen(U8S_LT_init_GT_)),
             RegisterUtf8(U8S_LP_Ljava_SL_lang_SL_String_SC_RP_V, strlen(U8S_LP_Ljava_SL_lang_SL_String_SC_RP_V)))));
    ChangeStack(1);
    PutOp(OP_ATHROW);
    code_attribute -> max_stack = 3;
        int exception_index = code_attribute -> exception_table.NextIndex();
        code_attribute -> exception_table[exception_index].start_pc = 0;
        code_attribute -> exception_table[exception_index].end_pc = 5; // DS 11 feb
        code_attribute -> exception_table[exception_index].handler_pc = 5;
        code_attribute -> exception_table[exception_index].catch_type =
            RegisterClass(U8S_java_SL_lang_SL_ClassNotFoundException, strlen(U8S_java_SL_lang_SL_ClassNotFoundException));
        
}
int ByteCode::GenerateClassAccess(AstFieldAccess * field_access)
{  // here to generate code to dymanically initialize the field for a class literal and then return its value
    Label lab1,lab2;
    if (field_access -> symbol -> VariableCast()) {
        // simple case in immediate environment, can use field on both left and right

        //(TypeSymbol * type)
    // evaluate X.class literal. If X is a primitive type, this is a predefined field;
    // otherwise, we must create a new synthetic field to hold the desired result and
    // initialize it at runtime.
        /* generate
           getstatic class_field     load class field
           ifnull lab1               branch if not yet set
           get class_field           here if set, return value
           goto lab2
           lab1:                     here to initialize the field
           load class_constant       get name of class
           invokestatic              invoke generated method to get class_field  desired value
           dup                       save value so can return it
           put class_field           initialize the field
           lab2:
           */
        VariableSymbol * sym =  field_access -> symbol -> VariableCast();

        PutOp(OP_GETSTATIC);
        PutU2(GenerateFieldReference(sym));
        ChangeStack(1);
        EmitBranch(OP_IFNULL, lab1);
        PutOp(OP_GETSTATIC);
        PutU2(GenerateFieldReference(sym));
        ChangeStack(1);
        EmitBranch(OP_GOTO, lab2);
        DefineLabel(lab1);
        // generate load of constant naming the class
        LoadLiteral(field_access -> base-> Type() -> class_literal_name, this_control.String());
        PutOp(OP_INVOKESTATIC);
        CompleteCall(class_literal_method, 1, 0);
        PutOp(OP_DUP);
        PutOp(OP_PUTSTATIC);
        PutU2(GenerateFieldReference(sym));
        ChangeStack(-1);
    }
    else {
        // here in nested case, where must invoke access methods for the field
        VariableSymbol * sym =  field_access -> symbol -> VariableCast();
        MethodSymbol * read_symbol = field_access -> symbol -> MethodCast();
        MethodSymbol * write_symbol = field_access -> resolution_opt -> symbol -> MethodCast();

        AstMethodInvocation * resolve = field_access -> resolution_opt -> MethodInvocationCast();
        u2 read_ref, write_ref;
        // need load this for class with method
        // if the next statement read field_access -> resolution_opt -> symbol = read_method, then
        // generating code for that expression tree would give us what we want
        field_access -> resolution_opt -> symbol = read_symbol;
        PutOp(OP_INVOKESTATIC);
        read_ref = BuildMethodref(RegisterClass(read_symbol -> containing_type -> fully_qualified_name),
            BuildNameAndType(
                RegisterUtf8(read_symbol -> ExternalIdentity() -> Utf8_literal),
                RegisterUtf8(read_symbol -> signature)));
        PutU2(read_ref);
        ChangeStack(1);

        EmitBranch(OP_IFNULL, lab1);
        PutOp(OP_INVOKESTATIC);
        PutU2(read_ref);
        ChangeStack(1);
        EmitBranch(OP_GOTO, lab2);
        DefineLabel(lab1);
        // generate load of constant naming the class
        LoadLiteral(field_access -> base-> Type() -> class_literal_name, this_control.String());
        PutOp(OP_INVOKESTATIC);
        CompleteCall(class_literal_method, 1, 0);
        PutOp(OP_DUP);
        PutOp(OP_INVOKESTATIC);
        write_ref = BuildMethodref(RegisterClass(write_symbol -> containing_type -> fully_qualified_name),
            BuildNameAndType(
                RegisterUtf8(write_symbol -> ExternalIdentity() -> Utf8_literal),
                RegisterUtf8(write_symbol -> signature)));
        PutU2(write_ref);
        ChangeStack(-1); // to indicate argument popped
        
    }
    DefineLabel(lab2);
    CompleteLabel(lab1);
    CompleteLabel(lab2);
    return 1; // return one-word (reference) result
}

int ByteCode::EmitArrayCreationExpression(AstArrayCreationExpression *expression) 
{
    // see also OP_MULTINEWARRAY
    int dims;
    if (expression -> array_initializer_opt) {
        InitializeArray(expression-> Type(), expression -> array_initializer_opt);
    }
    else {
        dims = expression -> NumDimExprs();
        // need to push value of dimension(s)
        for (int i = 0;i < dims;i++) {
            AstDimExpr *d_expr = expression -> DimExpr(i);
            EmitExpression(d_expr -> expression);
        }
        EmitNewArray(dims, expression-> Type());
    }
    return 1;
}

int ByteCode::EmitAssignmentExpression(AstAssignmentExpression *expression,int need_value)
{
    // ASSIGNMENT
    int kind;
    int opc;
    int need_cast=0;
    VariableSymbol * sym;
    AstExpression * left_hand_side;
    TypeSymbol * expression_type = expression-> Type();
    TypeSymbol * expression_expression_type = expression -> expression-> Type();
    TypeSymbol * left_type;
    TypeSymbol * dest_type;
    TypeSymbol * cast_type;
    TypeSymbol * op_type;
    int stack_words = 0;
    if (expression -> left_hand_side -> CastExpressionCast()) {
        need_cast = 1;
        cast_type = expression -> left_hand_side -> CastExpressionCast()-> Type();
        left_hand_side = expression -> left_hand_side -> CastExpressionCast() -> expression;
        op_type = cast_type;
        dest_type = expression -> left_hand_side-> Type();;
    }
    else {
        left_hand_side = expression -> left_hand_side;
        op_type = expression_type;
    }
    left_type = left_hand_side-> Type();
    kind = GetLHSKind(expression -> left_hand_side, expression -> write_method);
    if (expression -> assignment_tag == AstAssignmentExpression::EQUAL) {
        switch(kind) {
            case LHS_ARRAY:
                EmitArrayAccessLHS(left_hand_side -> ArrayAccessCast()); // lhs must be array access
                break;
            case LHS_FIELD:
                EmitFieldAccessLHSBase(left_hand_side); // load base for field access
                break;
            case LHS_CLASS_METHOD:
                // need to load address of object, obtained from resolution
                ResolveAccess(left_hand_side, 0); // just get address
                break;
            case LHS_STATIC_METHOD:
                // nothing to do for static method at this point
                break;
        }
        EmitExpression(expression -> expression);
    }
    else {
        // here for compound assignment. Get the left operand, saving any information necessary to
        // update its value on the stack below the value.
        switch(kind) {
            case LHS_ARRAY:
                EmitArrayAccessLHS(left_hand_side -> ArrayAccessCast()); // lhs must be array access
                PutOp(OP_DUP2); // save base and index for later store
                // load current value
                (void) LoadArrayElement(expression_type);
                break;
            case LHS_FIELD:
                EmitFieldAccessLHS(left_hand_side);
                break;
            case LHS_LOCAL:
            case LHS_STATIC:
                (void) LoadSimple(kind,left_hand_side);
                ChangeStack(this_control.IsDoubleWordType(left_type) ? 1: 0); // CHECK_THIS? Is this really necessary
                break;
            case LHS_CLASS_METHOD:
                // need to load address of object, obtained from resolution, saving a copy on the stack
                ResolveAccess(left_hand_side, 1); // get address and value
                break;
            case LHS_STATIC_METHOD:
                // get value by invoking the appropriate resolution
                EmitExpression(left_hand_side -> SimpleNameCast() -> resolution_opt); // get value
                break;
        }

        if (expression -> assignment_tag ==AstAssignmentExpression::PLUS_EQUAL
            && left_type == this_control.String()) {
            // Here for string concatenation.
            EmitStringBuffer();
            PutOp(OP_SWAP);    // swap address if buffer and string to update.
            EmitStringAppendMethod(this_control.String());
            AppendString(expression -> expression);
            EmitCallStringToString();
        }
        else {
            // Here for operation other than string concatenation. Determine the opcode to use.
            if (this_control.IsSimpleIntegerValueType(op_type)||
                op_type == this_control.boolean_type){
                switch (expression -> assignment_tag) {
                    case AstAssignmentExpression::STAR_EQUAL:                  opc = OP_IMUL;  break;
                    case AstAssignmentExpression::SLASH_EQUAL:                 opc = OP_IDIV;  break;
                    case AstAssignmentExpression::MOD_EQUAL:                   opc = OP_IREM;  break;
                    case AstAssignmentExpression::PLUS_EQUAL:                  opc = OP_IADD;  break;
                    case AstAssignmentExpression::MINUS_EQUAL:                 opc = OP_ISUB;  break;
                    case AstAssignmentExpression::LEFT_SHIFT_EQUAL:            opc = OP_ISHL;  break;
                    case AstAssignmentExpression::RIGHT_SHIFT_EQUAL:           opc = OP_ISHR;  break;
                    case AstAssignmentExpression::UNSIGNED_RIGHT_SHIFT_EQUAL:  opc = OP_IUSHR; break;
                    case AstAssignmentExpression::AND_EQUAL:                   opc = OP_IAND;  break;
                    case AstAssignmentExpression::IOR_EQUAL:                   opc = OP_IOR;   break;
                    case AstAssignmentExpression::XOR_EQUAL:                   opc = OP_IXOR;  break;
                }
            }
            else if (op_type == this_control.long_type){
                switch (expression -> assignment_tag) {
                    case AstAssignmentExpression::STAR_EQUAL:                  opc = OP_LMUL;  break;
                    case AstAssignmentExpression::SLASH_EQUAL:                 opc = OP_LDIV;  break;
                    case AstAssignmentExpression::MOD_EQUAL:                   opc = OP_LREM;  break;
                    case AstAssignmentExpression::PLUS_EQUAL:                  opc = OP_LADD;  break;
                    case AstAssignmentExpression::MINUS_EQUAL:                 opc = OP_LSUB;  break;
                    case AstAssignmentExpression::LEFT_SHIFT_EQUAL:            opc = OP_LSHL;  break;
                    case AstAssignmentExpression::RIGHT_SHIFT_EQUAL:           opc = OP_LSHR;  break;
                    case AstAssignmentExpression::UNSIGNED_RIGHT_SHIFT_EQUAL:  opc = OP_LUSHR; break;
                    case AstAssignmentExpression::AND_EQUAL:                   opc = OP_LAND;  break;
                    case AstAssignmentExpression::IOR_EQUAL:                   opc = OP_LOR;   break;
                    case AstAssignmentExpression::XOR_EQUAL:                   opc = OP_LXOR;  break;
                }
            }
            else if (op_type == this_control.float_type) {
                switch (expression -> assignment_tag) {
                    case AstAssignmentExpression::STAR_EQUAL:                  opc = OP_FMUL;  break;
                    case AstAssignmentExpression::SLASH_EQUAL:                 opc = OP_FDIV;  break;
                    case AstAssignmentExpression::MOD_EQUAL:                   opc = OP_FREM;  break;
                    case AstAssignmentExpression::PLUS_EQUAL:                  opc = OP_FADD;  break;
                    case AstAssignmentExpression::MINUS_EQUAL:                 opc = OP_FSUB;  break;
                }
            }
            else if (op_type == this_control.double_type)    {
                switch (expression -> assignment_tag) {
                    case AstAssignmentExpression::STAR_EQUAL:                  opc = OP_DMUL;  break;
                    case AstAssignmentExpression::SLASH_EQUAL:                 opc = OP_DDIV;  break;
                    case AstAssignmentExpression::MOD_EQUAL:                   opc = OP_DREM;  break;
                    case AstAssignmentExpression::PLUS_EQUAL:                  opc = OP_DADD;  break;
                    case AstAssignmentExpression::MINUS_EQUAL:                 opc = OP_DSUB;  break;
                }
            }

            // convert value to desired type if necessary
            if (need_cast) {
                EmitCast(cast_type,left_type);
            }

            EmitExpression(expression -> expression);

            if (expression -> expression-> Type() == this_control.long_type && (opc == OP_LSHL || opc == OP_LSHR || opc == OP_IUSHR)) {
                PutOp(OP_L2I);
            }

            PutOp(opc);  

            if (need_cast) { // now cast result back to type of result
                EmitCast(left_type, cast_type);
            }
        }
    }
    // Update left operand, saving value of right operand if it is needed.
    switch(kind) {
        case LHS_ARRAY:
            if (need_value) {
                if (this_control.IsDoubleWordType(left_type)) {
                    PutOp(OP_DUP2_X2);
                }
                else PutOp(OP_DUP_X2);
            }
            StoreArrayElement(expression_type);
            break;
        case LHS_FIELD:
        case LHS_CLASS_METHOD:
            if (need_value) {
                if (this_control.IsDoubleWordType(left_type)) {
                    PutOp(OP_DUP2_X1);
                }
                else PutOp(OP_DUP_X1);
            }
            if (kind==LHS_CLASS_METHOD) {
                stack_words = this_control.IsDoubleWordType(left_type) ? 2: 1;
                PutOp(OP_INVOKEVIRTUAL);
                CompleteCall(expression -> write_method, stack_words, 0);
            }
            else {
                StoreField(left_hand_side);
            }
            break;
        case LHS_LOCAL:
        case LHS_STATIC:
        case LHS_STATIC_METHOD:
            if (need_value) {
                if (this_control.IsDoubleWordType(left_type)) {
                    PutOp(OP_DUP2);
                }
                else PutOp(OP_DUP);
            }
            if (kind==LHS_STATIC_METHOD) {
                stack_words = this_control.IsDoubleWordType(left_type) ? 2: 1;
                PutOp(OP_INVOKESTATIC);
                CompleteCall(expression -> write_method, stack_words, 0);
            }
            else {
                StoreSimple(kind,left_hand_side);
            }

            break;
    }
    return GetTypeWords(expression_type);
}

// Similar code patterns are used for the ordered comparisons
int ByteCode::EmitBinaryExpression(AstBinaryExpression *expression) 
{
    // BINARY
    switch (expression -> binary_tag) { // process boolean-results first
        case AstBinaryExpression::OR_OR:
        case AstBinaryExpression::AND_AND:
        case AstBinaryExpression::LESS:
        case AstBinaryExpression::LESS_EQUAL:
        case AstBinaryExpression::GREATER:
        case AstBinaryExpression::GREATER_EQUAL:
        case AstBinaryExpression::EQUAL_EQUAL:
        case AstBinaryExpression::NOT_EQUAL: {
            Label lab1,lab2;
            EmitBranchIfExpression(expression,true,lab1);
            PutOp(OP_ICONST_0);                // push false
            EmitBranch(OP_GOTO,lab2);
            DefineLabel(lab1);
            PutOp(OP_ICONST_1);                // push false
            DefineLabel(lab2);
            CompleteLabel(lab1);
            CompleteLabel(lab2);
        }
        return 1;
    }
    if (expression -> binary_tag == AstBinaryExpression::INSTANCEOF) {
        TypeSymbol * instanceof_type = expression -> right_expression-> Type();
        EmitExpression(expression -> left_expression);
        PutOp(OP_INSTANCEOF);
        if (instanceof_type -> num_dimensions) {
            PutU2(RegisterClass(instanceof_type -> signature));
        }
        else {
            PutU2(RegisterClass(instanceof_type -> fully_qualified_name));
        }
        return 1;
    }
    // special case string concatenation
    if (expression -> binary_tag == AstBinaryExpression::PLUS && (IsReferenceType(expression -> left_expression-> Type())) ||
        IsReferenceType(expression -> right_expression-> Type())) {
        ConcatenateString(expression);
        return 1;
    }
#ifdef TBSL
    // debug this later (DS 19 nov 96)
    // try to simplify if one operand known to be zero.
    if (isZero(expression -> left_expression)) {
        TypeSymbol * right_type = expression -> right_expression-> Type();
        switch (expression -> binary_tag) {
            case AstBinaryExpression::PLUS:
            case AstBinaryExpression::IOR:
            case AstBinaryExpression::XOR:
                    // here for cases that simplify to the right operand
                    EmitExpression(expression -> right_expression);
            return;
            case AstBinaryExpression::STAR:
            case AstBinaryExpression::AND:
            case AstBinaryExpression::LEFT_SHIFT:
            case AstBinaryExpression::RIGHT_SHIFT:
            case AstBinaryExpression::UNSIGNED_RIGHT_SHIFT:
                    // here for cases that evaluate to zero
                    if (this_control.IsSimpleIntegerValueType(right_type)) {
                LoadShort(0);
            }
                    else if (right_type == this_control.long_type) {
                        PutOp(OP_LCONST_0);
                    }
                    else if (right_type == this_control.float_type) {
                        PutOp(OP_FCONST_0);
                    }
                    else if (right_type == this_control.double_type) {
                        PutOp(OP_DCONST_0);
                    }
                    else chaos("unexpected type in expression simplification");
                    return GetTypeWords(right_type);

            case AstBinaryExpression::MINUS:
                    // 0 - x is negation of x
                    EmitExpression(expression -> right_expression);
            if (this_control.IsSimpleIntegerValueType(right_type)) {
                PutOp(OP_INEG);    
            }
            else if (right_type == this_control.long_type) {
                PutOp(OP_LNEG);
            }
            else if (right_type == this_control.float_type) {
                PutOp(OP_FNEG);
            }
            else if (right_type == this_control.double_type) {
                PutOp(OP_DNEG);
            }
            else chaos("unexpected type in expression simplification");
            return 1;
        }
    }


    if (isZero(expression -> right_expression)) {
        TypeSymbol * left_type = expression -> left_expression-> Type();
        switch (expression->binary_tag) {
            case AstBinaryExpression::PLUS:
            case AstBinaryExpression::MINUS:
            case AstBinaryExpression::IOR:
            case AstBinaryExpression::XOR:
            case AstBinaryExpression::LEFT_SHIFT:
            case AstBinaryExpression::RIGHT_SHIFT:
            case AstBinaryExpression::UNSIGNED_RIGHT_SHIFT:
                    // here for cases that simplify to the left operand
                    EmitExpression(expression->left_expression);
            return;
            case AstBinaryExpression::STAR:
            case AstBinaryExpression::AND:
                    // here for cases that evaluate to zero
                    if (this_control.IsSimpleIntegerValueType(left_type)) {
                LoadShort(0);
            }
                    else if (left_type == this_control.long_type) {
                        PutOp(OP_LCONST_0);
                    }
                    else if (left_type == this_control.float_type) {
                        PutOp(OP_FCONST_0);
                    }
                    else if (left_type == this_control.double_type) {
                        PutOp(OP_DCONST_0);
                    }
                    else chaos("unexpected type in expression simplification");
                    return;
        }

    }
#endif

    EmitExpression(expression -> left_expression);
    EmitExpression(expression -> right_expression);

    switch (expression -> binary_tag) {
        case AstBinaryExpression::STAR:
                EmitBinaryOp(expression, OP_IMUL, OP_LMUL, OP_FMUL, OP_DMUL);
        break;
        case AstBinaryExpression::SLASH:
                EmitBinaryOp(expression, OP_IDIV, OP_LDIV, OP_FDIV, OP_DDIV);
        break;
        case AstBinaryExpression::MOD:
                EmitBinaryOp(expression, OP_IREM, OP_LREM, OP_FREM, OP_DREM);
        break;
        case AstBinaryExpression::PLUS:
                EmitBinaryOp(expression, OP_IADD, OP_LADD, OP_FADD, OP_DADD);
        break;
        case AstBinaryExpression::MINUS:
                EmitBinaryOp(expression, OP_ISUB, OP_LSUB, OP_FSUB, OP_DSUB);
        break;
        case AstBinaryExpression::LEFT_SHIFT:
                if (expression -> right_expression-> Type() == this_control.long_type) {
            PutOp(OP_L2I);
        }
                EmitBinaryOp(expression, OP_ISHL, OP_LSHL, 0, 0);
                break;
        case AstBinaryExpression::RIGHT_SHIFT:
                if (expression -> right_expression-> Type() == this_control.long_type) {
            PutOp(OP_L2I);
        }
                EmitBinaryOp(expression, OP_ISHR, OP_LSHR, 0, 0);
                break;
        case AstBinaryExpression::UNSIGNED_RIGHT_SHIFT:
                if (expression -> right_expression-> Type() == this_control.long_type) {
            PutOp(OP_L2I);
        }
                EmitBinaryOp(expression, OP_IUSHR, OP_LUSHR, 0, 0);
                break;
                //          case AstBinaryExpression::INSTANCEOF:
                //              EmitInstanceofExpression((AstInstanceofExpression *)expression);
                //              break;
        case AstBinaryExpression::AND:
                EmitBinaryOp(expression, OP_IAND, OP_LAND, 0, 0);
        break;
        case AstBinaryExpression::XOR:
                EmitBinaryOp(expression, OP_IXOR, OP_LXOR, 0, 0);
        break;
        case AstBinaryExpression::IOR:
                EmitBinaryOp(expression, OP_IOR, OP_LOR, 0, 0);
        break;
        default:
            chaos("binary unknown tag");
    }
    return GetTypeWords(expression-> Type());
}


void ByteCode::EmitBinaryOp(AstBinaryExpression *expression, int iop, int lop, int fop, int dop)
{
    int opc = 0;
    TypeSymbol * type = expression -> left_expression-> Type();
    // binary  PutOp
    if (this_control.IsSimpleIntegerValueType(type)
        || type == this_control.boolean_type)        opc = iop;
    else if (type == this_control.long_type)         opc = lop;
    else if (type == this_control.float_type)        opc = fop;
    else if (type == this_control.double_type)       opc = dop;
    if (opc == 0)       chaos(" * undefined on this type");
    PutOp(opc);

}   
int ByteCode::EmitCastExpression(AstCastExpression *expression) {
    TypeSymbol * dest_type = expression-> Type();
    TypeSymbol * source_type = expression -> expression-> Type();
    // convert from numeric type src to destination type dest
    EmitExpression(expression -> expression); 
    EmitCast(dest_type,source_type);
    return GetTypeWords(dest_type);
}
void ByteCode::EmitCast(TypeSymbol * dest_type, TypeSymbol * source_type)
{
    if (dest_type == source_type) return;  // done if nothing to do
    if (this_control.IsSimpleIntegerValueType(source_type)) {
        if (dest_type == this_control.long_type)         PutOp(OP_I2L);
        else if (dest_type == this_control.float_type)   PutOp(OP_I2F);
        else if (dest_type == this_control.double_type)  PutOp(OP_I2D);
        else if (dest_type == this_control.char_type)    PutOp(OP_I2C);
        else if (dest_type == this_control.byte_type)    PutOp(OP_I2B);
        else if (dest_type == this_control.short_type)   PutOp(OP_I2S);
        else if (dest_type == this_control.int_type); // no conversion needed
        else chaos("unsupported conversion");
    }
    else if (source_type == this_control.long_type) {
        if (this_control.IsSimpleIntegerValueType(dest_type)) {
          PutOp(OP_L2I);
          if (dest_type == this_control.char_type)   PutOp(OP_I2C);
          else if (dest_type == this_control.byte_type)  PutOp(OP_I2B);
          else if (dest_type == this_control.short_type) PutOp(OP_I2S);
            }
        else if (dest_type == this_control.float_type)   PutOp(OP_L2F);
        else if (dest_type == this_control.double_type)  PutOp(OP_L2D);
        else chaos("unsupported conversion");
    }
    else if (source_type == this_control.float_type) {
        if (this_control.IsSimpleIntegerValueType(dest_type)) {
          PutOp(OP_F2I);
          if (dest_type == this_control.char_type)   PutOp(OP_I2C);
          else if (dest_type == this_control.byte_type)  PutOp(OP_I2B);
          else if (dest_type == this_control.short_type) PutOp(OP_I2S);
        }

        else if (dest_type == this_control.long_type)    PutOp(OP_F2L);
        else if (dest_type == this_control.double_type)  PutOp(OP_F2D);
        else chaos("unsupported conversion");
    }
    else if (source_type == this_control.double_type) {
        if (this_control.IsSimpleIntegerValueType(dest_type)) {
          PutOp(OP_D2I);
          if (dest_type == this_control.char_type)   PutOp(OP_I2C);
          else if (dest_type == this_control.byte_type)  PutOp(OP_I2B);
          else if (dest_type == this_control.short_type) PutOp(OP_I2S);
        }
        else if (dest_type == this_control.long_type)    PutOp(OP_D2L);
        else if (dest_type == this_control.float_type)   PutOp(OP_D2F);
        else chaos("unsupported conversion");
    }
    else if (source_type == this_control.null_type) {
        //op(OP_ACONST_NULL);
    }
    else { // generate check cast instruction
    // it is possible to evaluate many of these at compile time
    if (dest_type -> num_dimensions) {
    PutOp(OP_CHECKCAST);
      PutU2(RegisterClass(dest_type -> signature)); 
    }
    else {
    PutOp(OP_CHECKCAST);
      PutU2(RegisterClass(dest_type -> fully_qualified_name));
    }
    }
}
    
int ByteCode::EmitClassInstanceCreationExpression(AstClassInstanceCreationExpression *expression, int need_value) 
{
    MethodSymbol * constructor = (MethodSymbol *) expression -> class_type -> symbol;
    TypeSymbol * type =  expression-> Type();
    int stack_words = 0;
    Label lab1;
    PutOp(OP_NEW);
    PutU2(RegisterClass(type -> fully_qualified_name));
    if (need_value) PutOp(OP_DUP); // save address of new object for constructor
    // call constructor
    // pass address of object explicitly passed to new if specified.
    if (expression -> base_opt) {
        stack_words += EmitExpression(expression -> base_opt);
        PutOp(OP_DUP);
        EmitBranch(OP_IFNONNULL, lab1);
        // need to test for null, raising NullPointerException if so. So just do athrow
        PutOp(OP_ACONST_NULL);
        PutOp(OP_ATHROW); 
        DefineLabel(lab1);
        CompleteLabel(lab1);
    }

    for (int i=0; i < expression -> NumLocalArguments();i++) {
        stack_words += EmitExpression((AstExpression *) expression -> LocalArgument(i));
    }
    for (int k=0; k < expression -> NumArguments();k++) {
        stack_words += EmitExpression((AstExpression *) expression -> Argument(k));
    }
    PutOp(OP_INVOKENONVIRTUAL);
    ChangeStack(-stack_words); 
    if (constructor -> constant_pool_index == 0 || constructor -> constant_pool_class!=class_id) { // build method ref for method
        constructor -> constant_pool_index = BuildMethodref(
            RegisterClass(type -> fully_qualified_name),
            BuildNameAndType(
                RegisterUtf8(this_control.init_name_symbol -> Utf8_literal),
                RegisterUtf8(constructor -> signature)));
        constructor -> constant_pool_class = class_id;
        }
    
    PutU2(constructor -> constant_pool_index);
    return 1;
}

int ByteCode::EmitConditionalExpression(AstConditionalExpression *expression)
{
    Label lab1,lab2;
    EmitBranchIfExpression(expression -> test_expression, false, lab1);
    EmitExpression(expression -> true_expression);
    EmitBranch(OP_GOTO,lab2);
    DefineLabel(lab1);
    EmitExpression(expression -> false_expression);
    DefineLabel(lab2);
    CompleteLabel(lab1);
    CompleteLabel(lab2);
    return GetTypeWords(expression -> true_expression-> Type());
}

int ByteCode::EmitFieldAccess(AstFieldAccess *expression) 
{
    AstExpression * p = expression -> base;
    VariableSymbol * sym =  expression -> symbol -> VariableCast();
    TypeSymbol * expression_type = expression-> Type();
    if (expression -> IsConstant()) {
            if (sym -> ACC_STATIC()) {
            if (!( p -> symbol -> TypeCast() || p -> symbol -> VariableCast())) {
              EmitExpression(p);
              PutOp(OP_POP);
            }
            }
        return LoadConstant(expression);
    }
    if (expression -> resolution_opt) {  // resolve reference to private field in parent
        return EmitExpression(expression -> resolution_opt);
    }
    if (p-> Type() -> IsArray() && sym -> ExternalIdentity() == this_control.length_name_symbol){
        EmitExpression(p);
        PutOp(OP_ARRAYLENGTH);
        return 1;
    }
    if (sym -> ACC_STATIC()) {
        if (!( p -> symbol -> TypeCast() || p -> symbol -> VariableCast())) {
          EmitExpression(p);
          PutOp(OP_POP);
        }
        PutOp(OP_GETSTATIC);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? 2: 1);
    }
    else {
        EmitExpression(p); // get base
        PutOp(OP_GETFIELD);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? 1: 0);
    }
    PutU2(GenerateFieldReference(sym));
    return GetTypeWords(expression_type);
}

void ByteCode::EmitCloneArray(AstMethodInvocation *expression)
{
    // generate code corresponding to
    //  try {
    //      evaluate super.clone();
    //  } catch (CloneNotSupportedException e) {
    //      throw new InternalError(e.getMessage());
    //  }
    MethodSymbol * msym = (MethodSymbol *) expression -> symbol;
    TypeSymbol * res_type = expression-> Type(); // result type
    AstFieldAccess * field;
    int start_pc,end_pc;
    Label end_label;
    int exception_index;
    // msym is clone_name_symbol, indicating array clone
    // use clone() in java/lang/Object.
    start_pc = code_attribute -> code.Length(); 
    field = expression -> method -> FieldAccessCast();
    if (field) {
        EmitExpression(field -> base);
    }
    else {
        chaos("field access expected in array clone");
    }
    PutOp(OP_INVOKEVIRTUAL);
    //  ChangeStack(-stack_words);
    PutU2(RegisterMethod(METHOD_CLONE));
    EmitBranch(OP_GOTO, end_label);
    end_pc = code_attribute -> code.Length(); 
    // start handler
    // can't count next StoreLocal as pop since in handler
    ChangeStack(1); 
    StoreLocal(1, this_control.Object());
    PutOp(OP_NEW); 
    PutU2(RegisterClass(U8S_java_SL_lang_SL_InternalError, strlen(U8S_java_SL_lang_SL_InternalError)));
    PutOp(OP_DUP);
    LoadLocal(1, this_control.Object());
    PutOp(OP_INVOKEVIRTUAL);
    PutU2(RegisterMethod(METHOD_CLONE_GETMESSAGE));
    PutOp(OP_INVOKENONVIRTUAL);
    PutU2(RegisterMethod(METHOD_CLONE_INIT));
    PutOp(OP_ATHROW);
    DefineLabel(end_label);
    CompleteLabel(end_label);

    exception_index = code_attribute -> exception_table.NextIndex();
    code_attribute -> exception_table[exception_index].start_pc = start_pc;
    code_attribute -> exception_table[exception_index].end_pc = end_pc;
    code_attribute -> exception_table[exception_index].handler_pc = end_pc;
    // "java/lang/CloneNotSupportedException"
    code_attribute -> exception_table[exception_index].catch_type =
        RegisterClass(U8S_java_SL_lang_SL_CloneNotSupportedException, strlen(U8S_java_SL_lang_SL_CloneNotSupportedException));

}

int ByteCode::EmitMethodInvocation(AstMethodInvocation *expression, int copy_base) 
{
    MethodSymbol * msym = (MethodSymbol *) expression -> symbol;
    AstSimpleName * simple_name;
    TypeSymbol * res_type = expression-> Type(); // result type
    int is_private = msym -> ACC_PRIVATE();
    int is_static = msym -> ACC_STATIC();
    int is_super=0; // set if super call
    int is_interface = msym -> containing_type -> ACC_INTERFACE();
    AstFieldAccess * field;
    int stack_words = 0; // words on stack needed for arguments
    if (msym -> ExternalIdentity() == this_control.clone_name_symbol) {
        if (msym -> containing_type -> IsArray()) {
            EmitCloneArray(expression);
            return GetTypeWords(res_type);
        }
#ifdef TBSL
        else {
           chaos(" clone not yet supported");
        }
#endif
    }
    if (is_static) {
       if (expression -> method -> FieldAccessCast()) {
           field = expression -> method -> FieldAccessCast();
           if (field -> base -> MethodInvocationCast()) {
              (void) EmitMethodInvocation(field -> base -> MethodInvocationCast(), 0);
              PutOp(OP_POP); // discard value (only evaluating for side effect)
           }
    
       }
    }
    else {
       field =expression -> method -> FieldAccessCast();
       if (field) {
     
         if (field -> base -> SuperExpressionCast()) {
           is_super=1;
         }
         if (field -> base -> MethodInvocationCast()) {
             (void) EmitMethodInvocation(field -> base -> MethodInvocationCast(), 0);
         }
         else {
           EmitExpression(field -> base);
         }
       }
       else if (expression -> method -> SimpleNameCast()) {
           simple_name = expression -> method -> SimpleNameCast();
           if (simple_name -> resolution_opt) {  // use resolution if available
               (void) EmitExpression(simple_name -> resolution_opt);
           }
           else {
               // must be field of current object, so load this
               PutOp(OP_ALOAD_0);
           }
       }
       else {
          chaos("unexpected argument to field access");
       }

    }
    if (!is_static && copy_base) { // if need to save object ref for method invocation
        PutOp(OP_DUP);
    }
    for (int i=0; i < expression -> NumArguments();i++) {
        stack_words += EmitExpression((AstExpression *) expression -> Argument(i));
    }
    PutOp(msym -> ACC_STATIC() ? OP_INVOKESTATIC 
         : (is_super | is_private)  ? OP_INVOKENONVIRTUAL
         : is_interface ? OP_INVOKEINTERFACE 
         : OP_INVOKEVIRTUAL);
     CompleteCall(msym,stack_words, is_interface);
    return GetTypeWords(res_type);
}
void ByteCode::CompleteCall(MethodSymbol * msym,int stack_words, int is_interface)
{
    TypeSymbol * res_type = msym-> Type();
    ChangeStack(-stack_words);
    // need to get method index, the constant_pool index for a
    // reference to this method (a Methodref);
    if (msym -> constant_pool_index == 0 || msym -> constant_pool_class != class_id) { // build method ref for method
        if (is_interface) {
        msym -> constant_pool_index = BuildInterfaceMethodref(
            RegisterClass(msym -> containing_type -> fully_qualified_name),
            BuildNameAndType(
                RegisterUtf8(msym -> ExternalIdentity() -> Utf8_literal),
                RegisterUtf8(msym -> signature)));
        msym -> constant_pool_class = class_id;
        }
    else {

        msym -> constant_pool_index = BuildMethodref(
            RegisterClass(msym -> containing_type -> fully_qualified_name),
            BuildNameAndType(
                RegisterUtf8(msym -> ExternalIdentity() -> Utf8_literal),
                RegisterUtf8(msym -> signature)));
        msym -> constant_pool_class = class_id;
        }
    }
    
    PutU2(msym -> constant_pool_index);
    if (is_interface) {
      PutU1(stack_words+1);
      PutU1(0);
    }
    // must account for value returned by method. Assume it places one
    // word on stack and correct this assumption if wrong.
    stack_words=1;
    if (this_control.IsDoubleWordType(res_type)) {
        stack_words = 2;
    }
    else if (res_type == this_control.void_type) { // no return value
        stack_words = 0;
    }
    ChangeStack(stack_words);
}   

void ByteCode::EmitNewArray(int dims,TypeSymbol * type)
{
    int i;
    TypeSymbol * element_type = type -> ArraySubtype();

    if (dims==0 || (dims == 1 && type -> num_dimensions == dims)) {
        if (this_control.IsNumeric(element_type) || element_type == this_control.boolean_type) {
            // if one-dimensional primitive
            if (element_type == this_control.boolean_type) i = 4;    // T_BOOLEAN
            else if (element_type == this_control.char_type)  i = 5; // T_CHAR
            else if (element_type == this_control.float_type) i = 6; // T_FLOAT
            else if (element_type == this_control.double_type) i = 7;// T_DOUBLE
            else if (element_type == this_control.byte_type) i = 8;  // T_BYTE
            else if (element_type == this_control.short_type)  i = 9;// T_SHORT
            else if (element_type == this_control.int_type) i = 10;  // T_INT
            else if (element_type == this_control.long_type) i = 11; // T_LONG
            else            chaos("new array unsupported type");
            PutOp(OP_NEWARRAY);
            PutU1(i);
            return;
        }
        else {
            // must be reference type
            PutOp(OP_ANEWARRAY);
            PutU2(RegisterClass(element_type -> fully_qualified_name));
        }
    }
    else {
        PutOp(OP_MULTIANEWARRAY);
        PutU2(RegisterClass(type -> signature));
        PutU1(dims);  // load dims count
        ChangeStack(dims-1);  // dims -1
    }
}

int ByteCode::EmitPostUnaryExpression(AstPostUnaryExpression *expression,int need_value) 
{
            // POST_UNARY
    int kind;
        switch(kind=GetLHSKind(expression -> expression, expression -> write_method)) {
        case LHS_LOCAL:
        case LHS_STATIC:
        case LHS_STATIC_METHOD:
            EmitPostUnaryExpressionSimple(kind,expression,need_value);
            break;
        case LHS_ARRAY:
            EmitPostUnaryExpressionArray(expression, need_value);
            break;
        case LHS_FIELD:
        case LHS_CLASS_METHOD:
            EmitPostUnaryExpressionField(kind,expression,need_value);
            break;
        default:
            chaos("unknown lhs kind for assignment");
    }
    return GetTypeWords(expression-> Type());
}

void ByteCode::EmitPostUnaryExpressionField(int kind,AstPostUnaryExpression *expression,int need_value) 
{
//    AstExpression *expression;
    // POST_UNARY on instance variable
    // load value of field, duplicate, do increment or decrement, then store back, leaving original value
    // on top of stack.
    VariableSymbol * sym = (VariableSymbol *) expression -> symbol;
    TypeSymbol * type = (TypeSymbol *) sym -> owner;
    TypeSymbol * expression_type = expression-> Type();
    bool plus = (expression -> post_unary_tag  ==  AstPostUnaryExpression::PLUSPLUS) ? true : false;

    if (kind==LHS_FIELD) {
        EmitFieldAccessLHS(expression -> expression);
    }
    else {
        ResolveAccess(expression -> expression, 1); // get address and value
    }

    if (need_value) {
        if (this_control.IsDoubleWordType(expression_type)) {
        PutOp(OP_DUP2_X1);
        }
        else PutOp(OP_DUP_X1);
    }
    if (this_control.IsSimpleIntegerValueType(expression_type)) {   // TBSL: use iinc eventually
            PutOp(OP_ICONST_1);
            PutOp(plus ? OP_IADD : OP_ISUB);
    }
    else if (expression_type == this_control.long_type) {
        PutOp(OP_LCONST_1);
        PutOp(plus ? OP_LADD : OP_LSUB);
    }
    else if (expression_type == this_control.float_type) {
        PutOp(OP_FCONST_1);
        PutOp(plus ? OP_FADD : OP_FSUB);
    }
    else if (expression_type == this_control.double_type) {
        PutOp(OP_DCONST_1);                // load 1.0
        PutOp(plus ? OP_DADD : OP_DSUB);
    }
    if (kind==LHS_FIELD) {
        PutOp(OP_PUTFIELD);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? -3: -2);
        PutU2(GenerateFieldReference(sym));
    }
    else {
        int stack_words = this_control.IsDoubleWordType(expression_type) ? 2: 1;
        PutOp(OP_INVOKEVIRTUAL);
        CompleteCall(expression -> write_method, stack_words, 0);
    }
}

void ByteCode::EmitPostUnaryExpressionSimple(int kind, AstPostUnaryExpression *expression, int need_value) 
{
//    AstExpression *expression;
    // POST_UNARY on local variable
    // load value of variable, duplicate, do increment or decrement, then store back, leaving original value
    // on top of stack.
    bool plus = (expression -> post_unary_tag  ==  AstPostUnaryExpression::PLUSPLUS) ? true : false;
    TypeSymbol * expression_type = expression-> Type();

    if (expression_type == this_control.int_type) { // see if can use IINC
        if (IsLocal(expression)) {
        if (need_value)  (void) LoadSimple(kind, expression);
          PutOp(OP_IINC);
          PutU1(expression -> symbol -> VariableCast() -> LocalVariableIndex());
          PutI1(plus ? 1 : -1);
          return;
        }
    }
    LoadSimple(kind,expression -> expression); // this will also load value needing resolution

    if (need_value) {
        if (this_control.IsDoubleWordType(expression_type)) {
        PutOp(OP_DUP2);
        }
        else PutOp(OP_DUP);
    }
    if (this_control.IsSimpleIntegerValueType(expression_type)) {               // TBSL: use iinc eventually
            PutOp(OP_ICONST_1);
            PutOp(plus ? OP_IADD : OP_ISUB);
            EmitCast(expression_type, this_control.int_type);
    }
    else if (expression_type == this_control.long_type) {
        PutOp(OP_LCONST_1);
        PutOp(plus ? OP_LADD : OP_LSUB);
    }
    else if (expression_type == this_control.float_type) {
        PutOp(OP_FCONST_1);
        PutOp(plus ? OP_FADD : OP_FSUB);
    }
    else if (expression_type == this_control.double_type) {
        PutOp(OP_DCONST_1);                // load 1.0
        PutOp(plus ? OP_DADD : OP_DSUB);
    }
    if (kind==LHS_STATIC_METHOD) {
        int stack_words = this_control.IsDoubleWordType(expression_type) ? 2: 1;
        PutOp(OP_INVOKESTATIC);
        CompleteCall(expression -> write_method, stack_words, 0);
    }
    else {
        StoreSimple(kind,expression -> expression);
    }
}


void ByteCode::EmitPostUnaryExpressionArrayCode(int load_op,
                                                int need_value,
                                                int dup_op,
                                                int const_op,
                                                int plus,
                                                int add_op,
                                                int sub_op,
                                                int store_op,
                                                int conv_op) 
{
    PutOp(load_op);
    if (need_value)PutOp(dup_op);  // save value below saved array base and index
    PutOp(const_op);
    PutOp(plus ? add_op : sub_op);
    if (conv_op) PutOp(conv_op); // if need to convert back to desired type
    PutOp(store_op);
}

void ByteCode::EmitPostUnaryExpressionArray(AstPostUnaryExpression *expression, int need_value) 
{
    // Post Unary for which operand is array element
    // assignment for which lhs is array element
//    AstExpression *expression;
    bool plus = (expression -> post_unary_tag  ==  AstPostUnaryExpression::PLUSPLUS) ? true : false;
    TypeSymbol * expression_type = expression-> Type();
    EmitArrayAccessLHS((AstArrayAccess *)expression -> expression); // lhs must be array access
    PutOp(OP_DUP2); // save array base and index for later store
    if (expression_type == this_control.int_type) {
        EmitPostUnaryExpressionArrayCode(OP_IALOAD, need_value, OP_DUP_X2,
        OP_ICONST_1, plus, OP_IADD, OP_ISUB, OP_IASTORE, 0);
    }
    else if (expression_type == this_control.byte_type ) {
        EmitPostUnaryExpressionArrayCode(OP_BALOAD, need_value, OP_DUP_X2,
        OP_ICONST_1, plus, OP_IADD, OP_ISUB, OP_BASTORE, OP_I2B);
    }
    else if (expression_type == this_control.char_type ) {
        EmitPostUnaryExpressionArrayCode(OP_CALOAD, need_value, OP_DUP_X2,
        OP_ICONST_1, plus, OP_IADD, OP_ISUB, OP_CASTORE, OP_I2C);
    }
    else if (expression_type == this_control.short_type) {
        EmitPostUnaryExpressionArrayCode(OP_SALOAD, need_value, OP_DUP_X2,
        OP_ICONST_1, plus, OP_IADD, OP_ISUB, OP_SASTORE, OP_I2S);
    }
    else if (expression_type == this_control.long_type) {
        EmitPostUnaryExpressionArrayCode(OP_LALOAD, need_value, OP_DUP2_X2,
        OP_LCONST_1, plus, OP_LADD, OP_LSUB, OP_LASTORE, 0);
    }
    else if (expression_type == this_control.float_type) {
        EmitPostUnaryExpressionArrayCode(OP_FALOAD, need_value, OP_DUP_X2,
        OP_FCONST_1, plus, OP_FADD, OP_FSUB, OP_FASTORE, 0);
    }
    else if (expression_type == this_control.double_type) {
        EmitPostUnaryExpressionArrayCode(OP_DALOAD, need_value, OP_DUP2_X2,
        OP_DCONST_1, plus, OP_DADD, OP_DSUB, OP_DASTORE, 0);
    }
    else    chaos("unsupported postunary type");
}

int ByteCode::EmitPreUnaryExpression(AstPreUnaryExpression *expression,int need_value) 
{
            // PRE_UNARY
    TypeSymbol * type = expression-> Type();
    if (expression -> pre_unary_tag  ==  AstPreUnaryExpression::PLUSPLUS ||
        expression -> pre_unary_tag  ==  AstPreUnaryExpression::MINUSMINUS) {
        EmitPreUnaryIncrementExpression(expression,need_value);
    }
    else {
    // here for ordinary unary operator without side effects.
    switch (expression -> pre_unary_tag) {
        case AstPreUnaryExpression::PLUS:
            // nothing to do (front-end will have done any needed conversions)
            EmitExpression(expression -> expression);
            break;
        case AstPreUnaryExpression::MINUS:
            EmitExpression(expression -> expression);
            if (this_control.IsSimpleIntegerValueType(type)) PutOp(OP_INEG);
            else if (type == this_control.long_type) PutOp(OP_LNEG);
            else if (type == this_control.float_type)    PutOp(OP_FNEG);
            else if (type == this_control.double_type)   PutOp(OP_DNEG);
            else chaos("unary minus on unsupported type");
            break;
        case AstPreUnaryExpression::TWIDDLE:
            if (this_control.IsSimpleIntegerValueType(type)) {
            EmitExpression(expression -> expression);
            PutOp(OP_ICONST_M1);           // -1
            PutOp(OP_IXOR);                // exclusive or to get result
            }
            else if (type == this_control.long_type) {
            EmitExpression(expression -> expression);
            PutOp(OP_LCONST_1);            // make -1
            PutOp(OP_LNEG);
            PutOp(OP_LXOR);                // exclusive or to get result
            }
            else chaos("unary ~ on unsupported type");
            break;
        case AstPreUnaryExpression::NOT: 
            if (type == this_control.boolean_type) {
                Label lab1,lab2;
                EmitExpression(expression -> expression);
                EmitBranch(OP_IFEQ,lab1);
                PutOp(OP_ICONST_0);        // turn true into false
                EmitBranch(OP_GOTO,lab2);
                DefineLabel(lab1);
                PutOp(OP_ICONST_1);        // here to turn false into true
                DefineLabel(lab2);
                CompleteLabel(lab1);
                CompleteLabel(lab2);
            }
            else chaos("unary ! on non-boolean not supported");
            break;
        default:
            chaos("unknown preunary tag");
    }
    }
    return GetTypeWords(type);
//    AstExpression *expression;
}
    
void ByteCode::EmitPreUnaryIncrementExpression(AstPreUnaryExpression *expression, int need_value) 
{
            // PRE_UNARY with side effects (++X or --X)
    int kind;
        switch(kind=GetLHSKind(expression, expression -> write_method)) {
        case LHS_LOCAL:
        case LHS_STATIC:
        case LHS_STATIC_METHOD:
            EmitPreUnaryIncrementExpressionSimple(kind,expression,need_value);
            break;
        case LHS_ARRAY:
            EmitPreUnaryIncrementExpressionArray(expression,need_value);
            break;
        case LHS_FIELD:
        case LHS_CLASS_METHOD:
            EmitPreUnaryIncrementExpressionField(kind, expression,need_value);
        break;
        default:
            chaos("unknown lhs kind for assignment");
    }
}

void ByteCode::EmitPreUnaryIncrementExpressionSimple(int kind,AstPreUnaryExpression *expression, int need_value) 
{
//    AstExpression *expression;
    // POST_UNARY on name
    // load value of variable, do increment or decrement, duplicate, then store back, leaving original value
    // on top of stack.
    bool plus = (expression -> pre_unary_tag  ==  AstPreUnaryExpression::PLUSPLUS) ? true : false;
    TypeSymbol * type = expression-> Type();
    if (type == this_control.int_type) {

        if (kind == LHS_LOCAL) {
          PutOp(OP_IINC);
          PutU1(expression -> symbol -> VariableCast() -> LocalVariableIndex());
          PutI1(plus ? 1 : -1);
          if (need_value) (void) LoadSimple(kind, expression);
          return;
        }
    }
    (void) LoadSimple(kind,expression -> expression); // will also load value if resolution needed

    if (this_control.IsSimpleIntegerValueType(type)) {              // TBSL: use iinc eventually
        PutOp(OP_ICONST_1);
        PutOp(plus ? OP_IADD : OP_ISUB);
        EmitCast(type, this_control.int_type);
        if (need_value) PutOp(OP_DUP);
    }
    else if (type == this_control.long_type) {
        PutOp(OP_LCONST_1);
        PutOp(plus ? OP_LADD : OP_LSUB);
        if (need_value) PutOp(OP_DUP2);
    }
    else if (type == this_control.float_type) {
        PutOp(OP_FCONST_1);
        PutOp(plus ? OP_FADD : OP_FSUB);
        if (need_value) PutOp(OP_DUP);
    }
    else if (type == this_control.double_type) {
        PutOp(OP_DCONST_1);                // load 1.0
        PutOp(plus ? OP_DADD : OP_DSUB);
        if (need_value) PutOp(OP_DUP2);
    }
    if (kind==LHS_STATIC_METHOD) {
        int stack_words = this_control.IsDoubleWordType(type) ? 2: 1;
        PutOp(OP_INVOKESTATIC);
        CompleteCall(expression -> write_method, stack_words, 0);
    }
    else {
        StoreSimple(kind,expression);
    }
}

void ByteCode::EmitPreUnaryIncrementExpressionArrayCode(int load_op,
                                                        int const_op,
                                                        int plus,
                                                        int add_op,
                                                        int sub_op,
                                                        int need_value,
                                                        int dup_op,
                                                        int store_op,
                                                        int conv_op) 
{
    PutOp(load_op);
    PutOp(const_op);
    PutOp(plus ? add_op : sub_op);
    if (need_value) PutOp(dup_op); // save value below saved array base and index
    if (conv_op) PutOp(conv_op); // if need to check result in range
    PutOp(store_op);
}

void ByteCode::EmitPreUnaryIncrementExpressionArray(AstPreUnaryExpression *expression, int need_value) 
{
    // Post Unary for which operand is array element
    // assignment for which lhs is array element
//    AstExpression *expression;
    bool plus = (expression -> pre_unary_tag  ==  AstPreUnaryExpression::PLUSPLUS) ? true : false;
    TypeSymbol * type = expression-> Type();
    EmitArrayAccessLHS((AstArrayAccess *)expression -> expression); // lhs must be array access
    PutOp(OP_DUP2); // save array base and index for later store
    if (type == this_control.int_type) {
        EmitPreUnaryIncrementExpressionArrayCode(OP_IALOAD, OP_ICONST_1,
        plus, OP_IADD, OP_ISUB, need_value, OP_DUP_X2, OP_IASTORE,0);   
    }
    else if (type == this_control.byte_type) {
        EmitPreUnaryIncrementExpressionArrayCode(OP_BALOAD, OP_ICONST_1,
        plus, OP_IADD, OP_ISUB, need_value, OP_DUP_X2, OP_BASTORE, OP_I2B); 
    }
    else if (type == this_control.char_type) {
        EmitPreUnaryIncrementExpressionArrayCode(OP_CALOAD, OP_ICONST_1,
        plus, OP_IADD, OP_ISUB, need_value, OP_DUP_X2, OP_CASTORE, OP_I2C); 
    }
    else if (type == this_control.short_type) {
        EmitPreUnaryIncrementExpressionArrayCode(OP_SALOAD, OP_ICONST_1,
        plus, OP_IADD, OP_ISUB, need_value, OP_DUP_X2, OP_SASTORE, OP_I2S); 
    }
    else if (type == this_control.long_type) {
        EmitPreUnaryIncrementExpressionArrayCode(OP_LALOAD, OP_LCONST_1,
        plus, OP_LADD, OP_LSUB, need_value, OP_DUP2_X2, OP_LASTORE, 0); 
    }
    else if (type == this_control.float_type)    {
        EmitPreUnaryIncrementExpressionArrayCode(OP_FALOAD, OP_FCONST_1,
        plus, OP_FADD, OP_FSUB, need_value, OP_DUP_X2, OP_FASTORE, 0);  
    }
    else if (type == this_control.double_type) {
        EmitPreUnaryIncrementExpressionArrayCode(OP_DALOAD, OP_DCONST_1,
        plus, OP_DADD, OP_DSUB, need_value, OP_DUP2_X2, OP_DASTORE, 0); 
    }
    else chaos("unsupported PreUnary type");
}

void ByteCode::EmitPreUnaryIncrementExpressionField(int kind, AstPreUnaryExpression *expression, int need_value) 
{
    // Pre Unary for which operand is field (instance variable)
//    AstExpression *expression;
    bool plus = (expression -> pre_unary_tag  ==  AstPreUnaryExpression::PLUSPLUS) ? true : false;
    VariableSymbol * sym = (VariableSymbol *) expression -> symbol;
    TypeSymbol * type = (TypeSymbol *) sym -> owner;
    TypeSymbol * expression_type = expression-> Type();
    if (kind==LHS_CLASS_METHOD) {
        // need to load address of object, obtained from resolution, saving a copy on the stack
        ResolveAccess(expression -> expression, 1); // get address and value
    }
    else {
        EmitFieldAccessLHS(expression -> expression);
    }
        if (this_control.IsSimpleIntegerValueType(expression_type)) {
        PutOp(OP_ICONST_1);
        PutOp(plus ? OP_IADD : OP_ISUB);
        EmitCast(expression_type, this_control.int_type);
        if (need_value)PutOp(OP_DUP_X1);
    }
    else if (expression_type == this_control.long_type) {
        PutOp(OP_LCONST_1);
        PutOp(plus ? OP_LADD : OP_LSUB);
        if (need_value)PutOp(OP_DUP2_X1);
    }
    else if (expression_type == this_control.float_type) {
        PutOp(OP_FCONST_1);
        PutOp(plus ? OP_FADD : OP_FSUB);
        if (need_value)PutOp(OP_DUP_X1);
    }
    else if (expression_type == this_control.double_type) {
        PutOp(OP_DCONST_1);
        PutOp(plus ? OP_DADD : OP_DSUB);
        if (need_value)PutOp(OP_DUP2_X1);
    }
    else    chaos("unsupported PreUnary type");
    if (kind==LHS_CLASS_METHOD) {
        int stack_words = this_control.IsDoubleWordType(expression_type) ? 2: 1;
        PutOp(OP_INVOKEVIRTUAL);
        CompleteCall(expression -> write_method, stack_words, 0);
    }
    else {
        PutOp(OP_PUTFIELD);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? -3: -2);
        PutU2(GenerateFieldReference(sym));
    }
}

void ByteCode::EmitThisInvocation(AstThisCall *this_call)
{
    MethodSymbol * msym = this_call -> symbol;
    AstExpression *base_opt = this_call -> base_opt;

    // THIS_CALL
    //    AstExpression *method;
    //    AstList *arguments;
    // A call to another constructor (THIS_CALL) or super constructor (SUPER_CALL)
    // result in the same sort of generated code, as the semantic analysis
    // has resolved the proper constructor to be invoked.
    int stack_words = 0; // words on stack needed for arguments
    PutOp(OP_ALOAD_0); // load 'this'
    if (base_opt) {
        stack_words += EmitExpression(base_opt);
    }
    for (int i=0; i < this_call -> NumLocalArguments();i++) {
        stack_words += EmitExpression((AstExpression *) this_call -> LocalArgument(i));
    }
    for (int k=0; k < this_call -> NumArguments();k++) {
        stack_words += EmitExpression((AstExpression *) this_call -> Argument(k));
    }
    PutOp(OP_INVOKENONVIRTUAL);
    ChangeStack(-stack_words);
    // need to get method index, the constant_pool index for a
    // reference to this method (a Methodref);
// caller will supply methodref
    if (msym -> constant_pool_index == 0 || msym -> constant_pool_class != class_id) { // build method ref for method
        msym -> constant_pool_index = BuildMethodref(
            this_class,
            BuildNameAndType(
                RegisterUtf8(msym -> ExternalIdentity() -> Utf8_literal),
                RegisterUtf8(msym -> signature)));
        msym -> constant_pool_class = class_id;

    }
    
    PutU2(msym -> constant_pool_index);
}
 

void ByteCode::EmitSuperInvocation(AstSuperCall *super_call)
{
    MethodSymbol *msym = super_call -> symbol;
    AstExpression *base_opt = super_call -> base_opt;

    int stack_words = 0; // words on stack needed for arguments
    PutOp(OP_ALOAD_0); // load 'this'
    if (base_opt) {
        stack_words += EmitExpression(base_opt);
    }
    for (int i=0; i < super_call -> NumLocalArguments();i++) {
        stack_words += EmitExpression((AstExpression *) super_call -> LocalArgument(i));
    }
    for (int k=0; k < super_call -> NumArguments();k++) {
        stack_words += EmitExpression((AstExpression *) super_call -> Argument(k));
    }
    PutOp(OP_INVOKENONVIRTUAL);
    ChangeStack(-stack_words);
    // need to get method index, the constant_pool index for a
    // reference to this method (a methodref);
    // caller will supply methodref
    if (msym -> constant_pool_index == 0 || msym -> constant_pool_class != class_id) { // build method ref for method
        msym -> constant_pool_index = BuildMethodref(
            super_class,
            BuildNameAndType(
                RegisterUtf8(msym -> ExternalIdentity() -> Utf8_literal),
                RegisterUtf8(msym -> signature)));
        msym -> constant_pool_class = class_id;

    }
    
    PutU2(msym -> constant_pool_index);
}
 

AstExpression * ByteCode::UnParenthesize(AstExpression * p)
{
  // called when expression has been parenthesized to removed
  // parantheses and expose true structure.
  AstParenthesizedExpression * pe;
  while (p -> ParenthesizedExpressionCast()) {
      p = p -> ParenthesizedExpressionCast()  -> expression;
  }
  return p;
}

//  Methods for string concatenation

void ByteCode::ConcatenateString(AstBinaryExpression * expression)
{
    // generate code to concatenate strings, by generating a string buffer and appending the arguments
    // before calling toString, i.e.,
    //  s1+s2 compiles to
    //  new StringBuffer().append(s1).append(s2).toString();
    // look for sequences of concatenation to use a single buffer where possible
        EmitStringBuffer();
    AppendString(expression -> left_expression); 
    AppendString(expression -> right_expression);
    EmitCallStringToString(); // convert string buffer to string
}

void ByteCode::EmitCallStringToString()
{ // generate call to toString on stringbuffer
    PutOp(OP_INVOKEVIRTUAL);
    PutU2(RegisterMethod(METHOD_STRINGBUFFER_TOSTRING));
    ChangeStack(1); // account for return value
}
void ByteCode::EmitStringBuffer()
{
    // generate code to allocate new string buffer and initialize it
    PutOp(OP_NEW);
    PutU2(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, strlen(U8S_java_SL_lang_SL_StringBuffer)));
    PutOp(OP_DUP);
    PutOp(OP_INVOKENONVIRTUAL);
    PutU2(RegisterMethod(METHOD_STRINGBUFFER_INIT));
}

void ByteCode::AppendString(AstExpression * p)
{
    AstBinaryExpression *binexpr;
    TypeSymbol * type = p-> Type();
    if (p -> BinaryExpressionCast()) {
        binexpr = p -> BinaryExpressionCast();
       if ( binexpr -> binary_tag == AstBinaryExpression::PLUS
            && (IsReferenceType(binexpr -> left_expression-> Type()) ||
        IsReferenceType(binexpr -> right_expression-> Type()))) {
            AppendString(binexpr -> left_expression);
            AppendString(binexpr -> right_expression);
            return;
        } 
    }
    if (p -> ParenthesizedExpressionCast()) {
        AppendString(p -> ParenthesizedExpressionCast() -> expression);
        return;
    }
    if (p -> CastExpressionCast()) { // here if cast expression, verify that converting to string
        AstCastExpression *cast = (AstCastExpression *) p;
        if (cast->kind == Ast::CAST && cast-> Type() == this_control.String()) {
          AppendString(cast->expression);
          return;
        }
    }
    if (type == this_control.null_type) {
        // replace explicit reference to "null" by 
        // corresponding string.
        name_StringNull = BuildString(RegisterUtf8(U8S_null, strlen(U8S_null)));
        if (name_StringNull <=255) {
            PutOp(OP_LDC);
            PutU1((unsigned char) name_StringNull);
        }
        else {
            PutOp(OP_LDC_W);
            PutU2(name_StringNull);
        }
        type = this_control.String();
    }
    else {
        EmitExpression(p);
    }
    EmitStringAppendMethod(type);
}

void ByteCode::EmitStringAppendMethod(TypeSymbol * type) 
{
    int method_sig = 0;
    int stack_words = 1; // assume one word put on stack
    // call appropriate append routine to add to string buffer
    if (type -> num_dimensions == 1 && type -> base_type == this_control.char_type) {
        method_sig = RegisterMethod(METHOD_STRINGBUFFER_APPENDCHARARRAY);
    }
    else if (type == this_control.char_type) {
        method_sig = RegisterMethod(METHOD_STRINGBUFFER_APPENDCHAR);
    }
    else if (type == this_control.boolean_type) {
        method_sig = RegisterMethod(METHOD_STRINGBUFFER_APPENDBOOLEAN);
    }
    else if (type == this_control.int_type|| type == this_control.short_type || type == this_control.byte_type) {
        method_sig = RegisterMethod(METHOD_STRINGBUFFER_APPENDINT);
    }
    else if (type == this_control.long_type) {
        method_sig = RegisterMethod(METHOD_STRINGBUFFER_APPENDLONG);
        stack_words++;
    }
    else if (type == this_control.float_type) {
        method_sig = RegisterMethod(METHOD_STRINGBUFFER_APPENDFLOAT);
    }
    else if (type == this_control.double_type) {
        method_sig = RegisterMethod(METHOD_STRINGBUFFER_APPENDDOUBLE);
        stack_words++;
    }
    else if (type == this_control.String()) {
        method_sig = RegisterMethod(METHOD_STRINGBUFFER_APPENDSTRING);
    }
    else if (IsReferenceType(type)) {
        // may need to call toString method on object before appending to stringbuffer
        // TODO: do above
        method_sig = RegisterMethod(METHOD_STRINGBUFFER_APPENDOBJECT);
    }
    else {
        chaos("unexpected string buffer append operand");
    }
    PutOp(OP_INVOKEVIRTUAL);
    ChangeStack(-stack_words);
    PutU2(method_sig);
    ChangeStack(1); // account for return value
    if (method_sig == 0) chaos("unable to find type for string buffer concatenation");
}
void ByteCode::chaos(char *msg) {
 cout << "chaos: " << msg << "\n";
 cerr << "chaos: " << msg << "\n";
 exit(1);
}
#ifdef TEST
static void op_trap()
{
    int i=0;
    // used for debugger trap
}
#endif

ByteCode::ByteCode(TypeSymbol *unit_type) : ClassFile(unit_type),
                                            this_semantic(*unit_type -> semantic_environment -> sem),
                                            this_control(unit_type -> semantic_environment -> sem -> control)
{
    int i;

#ifdef TEST
    if (this_control.option.nowrite == 0) {
        this_control.class_files_written++;
    }
#endif
    this_control.class_file_id++;
    class_id = this_control.class_file_id;
    initialize_statics_in_clinit = 1;
    stack_depth = 0;
    synchronized_blocks = 0;
    finally_blocks = 0;
    
    for (i=0;i<METHOD_NUMBER;i++) registered_methods[i] = 0;
    name_StringNull=0;

    access_flags = unit_type -> access_flags;
    // The flags for 'static' and 'protected' are set only for the inner
    // classes attribute, not for the class, as described in page 25
    // of the inner classes document.
    if (unit_type -> ACC_PROTECTED()) {
        access_flags |= 0x0001; // set PUBLIC if PROTECTED
    }
    access_flags &= (~ 0x0008); // ResetACC_STATIC
    access_flags &= (~ 0x0004); // ResetACC_PROTECTED
    access_flags &= (~ 0x0002); // ResetACC_PRIVATE

    access_flags |= 0x0020; // must be set always set ACC_SUPER for class (cf page 86 of JVM Spec)
    
    magic = 0xcafebabe;
    major_version = 45;             // use Sun JDK 1.0 version numbers
    minor_version = 3;
    constant_pool.Next()=NULL;
    this_class = RegisterClass(unit_type -> fully_qualified_name);

    if (unit_type -> super) {
        super_class = RegisterClass(unit_type -> super -> fully_qualified_name);
    }
    else {
        super_class=0; // primordial beast Object
    }

    for (i=0; i<unit_type -> NumInterfaces();i++) {
        interfaces.Next() = RegisterClass(unit_type -> Interface(i) -> fully_qualified_name);
    }

    return;
}


//  Methods for manipulating labels

void ByteCode::DefineLabel(Label& lab)
{
    if (lab.defined){
        chaos( "duplicate label definition");
    }
    lab.defined = 1;
    lab.definition = code_attribute -> code.Length();
    if (lab.definition > last_label_pc) {
        last_label_pc = lab.definition;
    }
}
void ByteCode::CompleteLabel(Label& lab)
{
    // patch all uses to have proper value. This requires that
    // all labels be freed at some time.
    if (lab.uses.Length()) {
        if (lab.defined == 0) {
            chaos("label used but with no definition");
        }                 
        for (int i=0; i<lab.uses.Length();i++) {
            // patch byte code reference to label to reflect it's definition
            // as 16-bit signed offset.
            unsigned int luse = lab.uses[i].use_offset;;
            int start =  luse - lab.uses[i].op_offset;
            int offset = lab.definition - start;
            if (lab.uses[i].use_length == 2) {
                // here if short offset
                code_attribute -> code[luse] = (offset >> 8) & 0xFF;
                code_attribute -> code[luse+1] = offset & 0xFF;
            }
            else if (lab.uses[i].use_length == 4) {
                //here if 4 byte use
                code_attribute -> code[luse] = (offset >> 24) & 0xFF;
                code_attribute -> code[luse+1] = (offset >> 16) & 0xFF;
                code_attribute -> code[luse+2] = (offset >> 8) & 0xFF;
                code_attribute -> code[luse+3] = offset & 0xFF;
            }
            else {
                chaos(  "label use length not 2 or 4");
                exit(1);
            }

        }
        lab.uses.Reset();
    }
    // reset in case label is used again.
    lab.definition = 0;
    lab.defined=0;
}


int ByteCode::IsLabelUsed(Label& lab)
{
    return (lab.uses.Length()>0);
}

// int ByteCode::IsLabelDefined(Label& lab)
// {
//     return (lab.defined != 0);
// }

void ByteCode::UseLabel(Label & lab,int _length, int _op_offset)
{
    int lab_index = lab.uses.NextIndex();
    lab.uses[lab_index].use_length = _length;
    lab.uses[lab_index].op_offset = _op_offset;
    lab.uses[lab_index].use_offset = code_attribute -> code.Length();

    // fill next length bytes with zero; will be filled in with proper value when label completed
    for (int i=0;i<lab.uses[lab_index].use_length;i++) code_attribute -> code.Next() = 0;
}

//  Methods to query attributes

int ByteCode::IsLocal(AstExpression *p)
{
    // return 1 if p refers to local variable, 0 otherwise
    VariableSymbol *sym = p -> symbol -> VariableCast();
    return (sym && sym -> owner -> MethodCast()) ? 1 : 0;
}

int ByteCode::IsNull(AstExpression *p)
{
    // see if operand is null. The front-end will have inserted a cast
    // of null to the present type
    if (p -> CastExpressionCast()) {
        return p -> CastExpressionCast() -> expression-> Type() == this_control.null_type;
    }
    else return 0;
}   

int ByteCode::IsReferenceType(TypeSymbol *p)
{
    return (! (this_control.IsNumeric(p) 
               || p == this_control.boolean_type || p == this_control.null_type));
}


int ByteCode::IsDefaultValue(AstExpression *p)
{
    // see if operand is default value of its type
    TypeSymbol *type = p-> Type();
    if (!p -> IsConstant()) return 0;
    LiteralValue * litp = p -> value;
    if (this_control.IsSimpleIntegerValueType(type) || type == this_control.boolean_type) {
        IntLiteralValue * vp = (IntLiteralValue *) litp;
        int val = vp -> value;
        return (val == 0);
    }
    else if (type == this_control.long_type) {
        LongLiteralValue * vp = (LongLiteralValue *) litp;
        LongInt val = vp -> value;
        return (val == 0);
    }
    else if (type == this_control.float_type) {
        FloatLiteralValue * vp = (FloatLiteralValue *) litp;
        IEEEfloat val = vp -> value;
        return (val.Word() == 0);
    }

    else if (type == this_control.double_type) {
        DoubleLiteralValue * vp = (DoubleLiteralValue *) litp;
        IEEEdouble val = vp -> value;
        return (val.HighWord() == 0 && val.LowWord() == 0);
    }
    else {
        // the default value for everything else is 'null'
        return (type == this_control.null_type);
    }

    return 0;
}

int ByteCode::IsZero(AstExpression *p)
{
    // see if operand is integer type and is zero
    TypeSymbol *type = p-> Type();
    if (!p -> IsConstant()) return 0;
    if (p-> Type() == this_control.int_type||p-> Type() == this_control.boolean_type) {
        LiteralValue * litp = p -> value;
        IntLiteralValue * vp = (IntLiteralValue *) litp;
        int val = vp -> value;
        return (val == 0);
    }
    return 0;
}
int ByteCode::GetTypeWords(TypeSymbol * type)
{
    return this_control.IsDoubleWordType(type) ? 2: 1;
}

int ByteCode::GetLHSKind(AstExpression * expression, MethodSymbol * msym)
{
    if (msym) { // if has write_method
        if (msym -> ACC_STATIC())
            return LHS_STATIC_METHOD;
        else
            return LHS_CLASS_METHOD;
    }
            
    if (expression -> CastExpressionCast()) {
        expression = expression -> CastExpressionCast() -> expression;
    }
    else if (expression -> PreUnaryExpressionCast()) {
        expression = expression -> PreUnaryExpressionCast() -> expression;
    }
    else if (expression -> PostUnaryExpressionCast()) {
        expression = expression -> PostUnaryExpressionCast() -> expression;
    }

        //
        // A left-hand side is either an array access,
        // a field access or a name. In the case of a FieldAccess
        // or name, the left-hand side is resolved into a variable.
        // In the case of an array access, it is resolved into a type.
        // 
    VariableSymbol * sym = expression -> symbol -> VariableCast();
    if (! sym) return LHS_ARRAY;
    else if (sym -> owner -> MethodCast()) return LHS_LOCAL;
    else if (sym -> ACC_STATIC()) return LHS_STATIC;
    else return LHS_FIELD;
}

//  Methods to load values
int ByteCode::GetConstant(LiteralValue *litp, TypeSymbol *type)
{
    int lit_index;

    if (type == this_control.String()) {
        Utf8LiteralValue *vp = (Utf8LiteralValue *) litp;
        if (vp -> constant_pool_index_String != 0 && vp -> constant_pool_class == class_id)
            lit_index = vp -> constant_pool_index_String;
        else {
            // must be string
            lit_index = RegisterString(vp);
        }
    }
    else {
        if (litp -> constant_pool_index != 0 && litp -> constant_pool_class == class_id)
            lit_index = litp -> constant_pool_index;
        else {
            // load literal using literal value
            if (this_control.IsSimpleIntegerValueType(type) || type == this_control.boolean_type) {
                IntLiteralValue * vp = (IntLiteralValue *) litp;
                lit_index = RegisterInteger(vp);
            }
            else if (type == this_control.float_type) {
                FloatLiteralValue * vp = (FloatLiteralValue *) litp;
                IEEEfloat val = vp -> value;
                lit_index = RegisterFloat(vp);
            }
            else if (type == this_control.long_type) {
                LongLiteralValue * vp = (LongLiteralValue *) litp;
                lit_index = RegisterLong(vp);
            }
            else if (type == this_control.double_type) {
                DoubleLiteralValue * vp = (DoubleLiteralValue *) litp;
                lit_index = RegisterDouble(vp);
            }
            else chaos("unexpected GetConstant kind");
        }
    }

    return lit_index;
}

int ByteCode::LoadConstant(AstExpression *p)
{
    // here to load a constant when the LiteralValue is set.

    LiteralValue * litp = p -> value;
    if (!p -> IsConstant()) {
        chaos("constant expected by LoadConstant");
    }
    return LoadLiteral(p -> value,p-> Type());
}


int ByteCode::LoadLiteral(LiteralValue* litp, TypeSymbol *type) 
{
    //    int lit_index = litp -> constant_pool_index;
    int lit_index;
    int is_long_or_double=0; // set if need lcd2_w
    if (litp -> constant_pool_index >0 && litp -> constant_pool_class == class_id) lit_index= litp -> constant_pool_index;
    else lit_index = 0;
    // see if can load without using LDC even if have literal index; otherwise generate constant pool entry
    // if one has not yet been generated.
    if (type == this_control.null_type) {
        PutOp(OP_ACONST_NULL);
        return 1;
    }
    if (type == this_control.String()) {
        // register index as string if this has not yet been done
        Utf8LiteralValue * lv = (Utf8LiteralValue *) litp;
        lit_index = RegisterString(lv);
    }
    // load literal using literal value
    // note that boolean literal values stored as int literals
    else if (this_control.IsSimpleIntegerValueType(type) || type == this_control.boolean_type) {
        IntLiteralValue * vp = (IntLiteralValue *) litp;
        int val = vp -> value;
        if (val >= -32768 && val <32768) {
            LoadShort(val);
            return 1;
        }
        lit_index = RegisterInteger(vp);
    }
    else if (type == this_control.float_type) {
        FloatLiteralValue * vp = (FloatLiteralValue *) litp;
        IEEEfloat val = vp -> value;
        if (val.Word() == 0) { // if float 0.0
            PutOp(OP_FCONST_0);
            return 1;
        }
        else if (val.Word() == 0x3f800000) { // if float 1.0
            PutOp(OP_FCONST_1);
            return 1;
        }
        else if (val.Word() == 0x40000000) { // if float 2.0
            PutOp(OP_FCONST_2);
            return 1;
        }
        lit_index = RegisterFloat(vp);
    }
    else if (type == this_control.long_type) {
        LongLiteralValue * vp = (LongLiteralValue *) litp;
        LongInt val = vp -> value;
        if (val == 0) {
            PutOp(OP_LCONST_0);        // long 0
            return 2;
        }
        else if (val == 1) {
            PutOp(OP_LCONST_1);        // long 1
            return 2;
        }
        lit_index = RegisterLong(vp);
        is_long_or_double=1;
    }
    else if (type == this_control.double_type) {
    //
        DoubleLiteralValue * vp = (DoubleLiteralValue *) litp;
        IEEEdouble val = vp -> value;
        if(val.HighWord() == 0 && val.LowWord() == 0) {
            PutOp(OP_DCONST_0);
            return 2;
        }
        else if (val.HighWord() == 0x3ff00000 && val.LowWord() == 0x00000000) { // if double 1.0
            PutOp(OP_DCONST_1);
            return 2;
        }
        else { // if need ldc2_w
            lit_index = RegisterDouble(vp);
            is_long_or_double=1;
        }
    }
    else {
        chaos("unsupported constant kind");
    }
    if (lit_index == 0) chaos("lit_index zero");
    if(!is_long_or_double && lit_index <=255) {
        PutOp(OP_LDC);
        PutU1(lit_index);
    }
    else {
        PutOp(is_long_or_double ? OP_LDC2_W : OP_LDC_W);
        PutU2(lit_index);
    }
    return is_long_or_double + 1;
}
void ByteCode::LoadLocalVariable(VariableSymbol * var)
{
    LoadLocal(var -> LocalVariableIndex(), var-> Type());
}
void ByteCode::LoadLocal(int varno, TypeSymbol * type)
{
    int opc0, opc;
    if (this_control.IsSimpleIntegerValueType(type)|| type == this_control.boolean_type) {
        opc0 = OP_ILOAD_0; opc = OP_ILOAD;
    }
    else if (type == this_control.long_type) {
        opc0 = OP_LLOAD_0; opc = OP_LLOAD;
    }
    else if (type == this_control.float_type) {
        opc0 = OP_FLOAD_0; opc = OP_FLOAD;
    }
    else if (type == this_control.double_type) {
        opc0 = OP_DLOAD_0; opc = OP_DLOAD;
    }
    else { // assume reference
        opc0 = OP_ALOAD_0; opc = OP_ALOAD;
    }
    if (varno<=3) PutOp(opc0+varno);
    else if (varno<256) {
        PutOp(opc); PutU1(varno);
    }
    else {
        PutOp(OP_WIDE); PutOp(opc); PutU2(varno);
    }
}
void ByteCode::LoadInteger(int val)
{
    
    if (val >= -32768 && val <32768) { // if short
        LoadShort(val);
    }
    else {
        u2 index = BuildInteger(val);
        if (index <=255) {
            PutOp(OP_LDC);
            PutU1((unsigned char) index);
        }
        else {
            PutOp(OP_LDC_W);
            PutU2(index);
        }
    }
}

void ByteCode::LoadShort(int val)
{ // load short (signed) value onto stack
    if (val >= -128 && val <128) {
        switch (val) {
            case -1:
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                PutOp(OP_ICONST_0 + val); // exploit opcode encoding
                break;
            default:        // else put byte value
                PutOp(OP_BIPUSH);
                PutU1(val);
        }
    }
    else if (val >= -32768 && val <32768) { // if short
        PutOp(OP_SIPUSH);
        PutU1((val >> 8)); 
        PutU1(val);
    }
    else {
        chaos("bcShort operand not short!");
    }
}
void ByteCode::ResolveAccess(AstExpression *p, int need_value)
{
    // if need_value zero, then just get address for access of private member
    // else get value with an extra copy of the needed address below the value
    AstExpression * resolve;
    if (p -> FieldAccessCast()) {
        resolve = p -> FieldAccessCast() -> resolution_opt;
    }
    else if (p -> SimpleNameCast()) {
        resolve = p -> SimpleNameCast() -> resolution_opt;
    }
    if (resolve -> MethodInvocationCast()) {
        if (need_value) {
            EmitMethodInvocation(resolve -> MethodInvocationCast(), 1);
        }
        else {
    // next does too much, getting base and field value; just want base
            //Expression(resolve -> MethodInvocationCast() -> method);
        if (resolve -> MethodInvocationCast() -> method -> FieldAccessCast()) {
            AstFieldAccess * field_expression = 
             resolve -> MethodInvocationCast() -> method -> FieldAccessCast();
    //   VariableSymbol * sym = (VariableSymbol *) field_expression -> owner;
         EmitExpression (field_expression -> base);
        }
        else {
          chaos("field access expected in method resolution");
        }
        }
    }
    else {
        chaos("method invocation expected here");
    }
}
    
int ByteCode::LoadSimple (int kind,AstExpression *p)
{
    VariableSymbol * sym = (VariableSymbol *) p -> symbol;
    TypeSymbol * type = (TypeSymbol *) sym -> owner;
    TypeSymbol * expression_type = p-> Type();
    switch (kind) {
        case LHS_LOCAL:
            LoadLocal(sym -> LocalVariableIndex(), expression_type);
            break;
        case LHS_STATIC_METHOD:
            EmitExpression(p); // will do resolution
            break;
        case LHS_FIELD:
        case LHS_STATIC:
        {
            if (sym -> ACC_STATIC()) {
                PutOp(OP_GETSTATIC);
                ChangeStack(GetTypeWords(expression_type));
            }
            else {
                PutOp(OP_ALOAD_0); // get address of "this"
                PutOp(OP_GETFIELD);
                ChangeStack(GetTypeWords(expression_type)-1);
            }
            PutU2(GenerateFieldReference(sym));
            break;
            default: chaos("LoadSimple bad kind");
        }
    }
    return GetTypeWords(expression_type);
}

void ByteCode::LoadReference(AstExpression *expression) 
{
    //load reference from local variable.
    // otherwise will use getstatic or getfield.
    TypeSymbol * type;
    int is_local=0,varno;
    if (expression -> ParenthesizedExpressionCast()) {
        expression = UnParenthesize(expression);
    }
    VariableSymbol * sym = expression -> symbol -> VariableCast();
    if (sym && sym -> owner -> MethodCast()) {
        is_local=1;
        varno = sym -> LocalVariableIndex();
        LoadLocal(varno,expression-> Type());
        return;
    }
    if (expression -> ArrayAccessCast()) { // nested array reference
        EmitArrayAccessLHS(expression -> ArrayAccessCast());
        PutOp(OP_AALOAD);
    }
    else if (expression -> FieldAccessCast() && expression -> FieldAccessCast() -> resolution_opt) {
        EmitExpression(expression -> FieldAccessCast() -> resolution_opt);
        return;
    }
    else if (expression -> FieldAccessCast() && (type=sym -> owner -> TypeCast())) {
        // TypeSymbol * expression_type = expression-> Type();
        // here if field 
        if (sym -> ACC_STATIC()) {
            PutOp(OP_GETSTATIC);
            ChangeStack(this_control.IsDoubleWordType(type) ? 2: 1);
        }
        else {
            AstFieldAccess *field = expression -> FieldAccessCast();
            if (field){
                EmitExpression(field -> base);
            }
            else if (expression -> SimpleNameCast()) {
                PutOp(OP_ALOAD_0); // get address of "this"
            }
            else {
                chaos("LoadReference unexpected base type");
            }
            PutOp(OP_GETFIELD);
            ChangeStack(this_control.IsDoubleWordType(type) ? 1: 0);
        }
        PutU2(GenerateFieldReference(sym));
    }
    else { // must have expression, the value of which is reference
      EmitExpression(expression);
    }
}

int  ByteCode::LoadArrayElement(TypeSymbol * type)
{
    int opc;
    if (type == this_control.byte_type
        || type == this_control.boolean_type) opc=OP_BALOAD;
    else if (type == this_control.short_type)    opc = OP_SALOAD;
    else if (type == this_control.int_type)  opc = OP_IALOAD;
    else if (type == this_control.long_type)  opc = OP_LALOAD;
    else if (type == this_control.char_type)  opc = OP_CALOAD;
    else if (type == this_control.float_type)  opc = OP_FALOAD;
    else if (type == this_control.double_type)  opc = OP_DALOAD;
    else        opc = OP_AALOAD; // assume reference
    PutOp(opc);
    return GetTypeWords(type);
}

void ByteCode::StoreArrayElement(TypeSymbol * type)
{
    int opc;
    if (type == this_control.byte_type
        || type == this_control.boolean_type) opc=OP_BASTORE;
    else if (type == this_control.short_type)    opc = OP_SASTORE;
    else if (type == this_control.int_type)  opc = OP_IASTORE;
    else if (type == this_control.long_type)  opc = OP_LASTORE;
    else if (type == this_control.char_type)  opc = OP_CASTORE;
    else if (type == this_control.float_type)  opc = OP_FASTORE;
    else if (type == this_control.double_type)  opc = OP_DASTORE;
    else        opc = OP_AASTORE; // assume reference
    PutOp(opc);
}


//  Method to generate field reference
void ByteCode::StoreField(AstExpression *expression) 
{
    // DOT
    VariableSymbol * sym = (VariableSymbol *) expression -> symbol;
    TypeSymbol * type = (TypeSymbol *) sym -> owner;
    TypeSymbol * expression_type=expression-> Type();
    if (sym -> ACC_STATIC()) {
        PutOp(OP_PUTSTATIC);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? -2: -1);
    }
    else {
        PutOp(OP_PUTFIELD);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? -3: -2);
    }
    PutU2(GenerateFieldReference(sym));
}

void ByteCode::StoreLocalVariable(VariableSymbol * var)
{
    StoreLocal(var -> LocalVariableIndex(), var-> Type());
    if (this_control.option.g && var -> LocalVariableIndex() > last_parameter_index) {
        if (var -> local_program_counter == 0) {
            // here to update point of first assignment, marking point at which value is
            // available to be displayed by debugger.
            var -> local_program_counter = code_attribute -> code.Length();
        }
    }
}

void ByteCode::StoreLocal(int varno, TypeSymbol * type)
{
    int opc0, opc;
    if (this_control.IsSimpleIntegerValueType(type)|| type == this_control.boolean_type) {
        opc0 = OP_ISTORE_0; opc = OP_ISTORE;
    }
    else if (type == this_control.long_type) {
        opc0 = OP_LSTORE_0; opc = OP_LSTORE;
    }
    else if (type == this_control.float_type) {
        opc0 = OP_FSTORE_0; opc = OP_FSTORE;
    }
    else if (type == this_control.double_type) {
        opc0 = OP_DSTORE_0; opc = OP_DSTORE;
    }
    else { // assume reference
        opc0 = OP_ASTORE_0; opc = OP_ASTORE;
    }
    if (varno<=3) PutOp(opc0+varno);
    else if (varno<256) {
        PutOp(opc); PutU1(varno);
    }
    else {
        PutOp(OP_WIDE); PutOp(opc); PutU2(varno);
    }
}


int ByteCode::GenerateFieldReference(VariableSymbol * sym)
{
    // generate a field reg from symbol and class literal
    // build field ref for field
    // the field ref requires Utf8 entries for the containing
    // class, the field name and the field signature, the latter
    // two expressed as a NameAndTypeEntry
    if (sym -> constant_pool_index == 0 || sym -> constant_pool_class != class_id) {
        TypeSymbol * owner = (TypeSymbol *) sym -> owner;
        sym -> constant_pool_index = BuildFieldref(
            RegisterClass(owner -> fully_qualified_name),
            BuildNameAndType(
                              RegisterUtf8(sym -> ExternalIdentity()-> Utf8_literal),
                              RegisterUtf8(sym-> Type() -> signature)));
        sym -> constant_pool_class = class_id;
    }
    return sym -> constant_pool_index;
}


void ByteCode::StoreSimple (int kind,AstExpression *p)
{
    VariableSymbol * sym = (VariableSymbol *) p -> symbol;
    TypeSymbol * type = (TypeSymbol *) sym -> owner;
    TypeSymbol * expression_type = p-> Type();
    switch (kind) {
        case LHS_LOCAL:
            StoreLocalVariable(sym);
            break;
        case LHS_FIELD:
        case LHS_STATIC:
        {
            if (sym -> ACC_STATIC()) {
                PutOp(OP_PUTSTATIC);
                ChangeStack(this_control.IsDoubleWordType(expression_type) ? -2: -1);
            }
            else {
                PutOp(OP_ALOAD_0); // get address of "this"
                PutOp(OP_PUTFIELD);
                ChangeStack(this_control.IsDoubleWordType(expression_type) ? -3: -2);
            }
            PutU2(GenerateFieldReference(sym));
            break;
            default: chaos("StoreSimple bad kind");
        }
    }
}

//  Methods to locate and build entries in constant pool.
u2 ByteCode::BuildDouble(IEEEdouble d)
{
    CONSTANT_Double_info *p = new CONSTANT_Double_info(CONSTANT_Double);

    p -> high_bytes = d.HighWord();             
    p -> low_bytes = d.LowWord();               
    constant_pool.Next() = p;
    constant_pool.Next() = 0; // extra slop for double-word entry
    return constant_pool.Length()-2;
}


u2 ByteCode::BuildFieldref(u2 cl_index, u2 nt_index)
{
    CONSTANT_Fieldref_info *p = new CONSTANT_Fieldref_info(CONSTANT_Fieldref);

    p -> class_index = cl_index;
    p -> name_and_type_index = nt_index;
    constant_pool.Next() = p;
    return constant_pool.Length()-1;
}

u2 ByteCode::BuildFloat(IEEEfloat val)
{
    CONSTANT_Float_info *p = new CONSTANT_Float_info(CONSTANT_Float);
    p -> bytes = val.Word();
    constant_pool.Next() = p;
    return constant_pool.Length()-1;
}

u2 ByteCode::BuildInteger(int val)
{
    CONSTANT_Integer_info *p = new CONSTANT_Integer_info(CONSTANT_Integer);
    p -> bytes =    ((val>>24 & 0xff) << 24) | ((val>>16 & 0xff) << 16)
                  | ((val>>8 & 0xff) )<< 8 | (val&0xff);
    constant_pool.Next() = p;
    return constant_pool.Length()-1;
}

u2 ByteCode::BuildInterfaceMethodref(u2 cl_index, u2 nt_index)
{
    CONSTANT_InterfaceMethodref_info *p = new CONSTANT_InterfaceMethodref_info(CONSTANT_InterfaceMethodref);

    p -> class_index = cl_index;
    p -> name_and_type_index = nt_index;
    constant_pool.Next() = p;
    return constant_pool.Length()-1;
}

u2  ByteCode::BuildLong(LongInt  val)
{
    CONSTANT_Long_info *p = new CONSTANT_Long_info(CONSTANT_Long);

    p -> high_bytes = val.HighWord();
    p -> low_bytes = val.LowWord();
    constant_pool.Next() = p;
    constant_pool.Next() = 0; // extra slop for double-word entry
    return constant_pool.Length()-2;
}
static char   registered_methods_data[] = {
//
// This comment describes the strings below.
//
// "java/lang/Object","clone","()Ljava/lang/Object;",                  // Clone
// "java/lang/Throwable","getMessage","()Ljava/lang/String;",          // Clone_getMessage
// "java/lang/InternalError","<init>","(Ljava/lang/String;)V",         // Clone_init
// "java/lang/StringBuffer","toString","()Ljava/lang/String;",         // StringBuffer_toString
// "java/lang/StringBuffer","<init>","()V",                            // StringBuffer_init
// "java/lang/StringBuffer","append","([C)Ljava/lang/StringBuffer;",   // StringBuffer_appendCharArray
// "java/lang/StringBuffer","append","(C)Ljava/lang/StringBuffer;",    // StringBuffer_appendChar
// "java/lang/StringBuffer","append","(Z)Ljava/lang/StringBuffer;",    // StringBuffer_appendBoolean
// "java/lang/StringBuffer","append","(I)Ljava/lang/StringBuffer;",    // StringBuffer_appendInt
// "java/lang/StringBuffer","append","(J)Ljava/lang/StringBuffer;",    // StringBuffer_appendLong
// "java/lang/StringBuffer","append","(F)Ljava/lang/StringBuffer;",    // StringBuffer_appendFloat
// "java/lang/StringBuffer","append","(D)Ljava/lang/StringBuffer;",    // StringBuffer_appendDouble
// "java/lang/StringBuffer","append","(Ljava/lang/String;)Ljava/lang/StringBuffer;", // StringBuffer_appendString
// "java/lang/StringBuffer","append","(Ljava/lang/Object;)Ljava/lang/StringBuffer;" // StringBuffer_appendObject
//
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_O,U_b,U_j,U_e,U_c,U_t,U_NU,
   U_c,U_l,U_o,U_n,U_e,U_NU,
   U_LP,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_O,U_b,U_j,U_e,U_c,U_t,U_SC,U_NU,                  // Clone

   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_T,U_h,U_r,U_o,U_w,U_a,U_b,U_l,U_e,U_NU,
   U_g,U_e,U_t,U_M,U_e,U_s,U_s,U_a,U_g,U_e,U_NU,
   U_LP,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_SC,U_NU,          // Clone_getMessage
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_I,U_n,U_t,U_e,U_r,U_n,U_a,U_l,U_E,U_r,U_r,U_o,U_r,U_NU,
   U_LT,U_i,U_n,U_i,U_t,U_GT,U_NU,
   U_LP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_SC,U_RP,U_V,U_NU,
   // Clone_init
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_NU,
   U_t,U_o,U_S,U_t,U_r,U_i,U_n,U_g,U_NU,
   U_LP,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_SC,U_NU,
   // StringBuffer_toString
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_NU,
   U_LT,U_i,U_n,U_i,U_t,U_GT,U_NU,
   U_LP,U_RP,U_V,U_NU,
   // StringBuffer_init
//"java/lang/StringBuffer",
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_NU,
//"append",
U_a,U_p,U_p,U_e,U_n,U_d,U_NU,

//"([C)Ljava/lang/StringBuffer;",
U_LP,U_LB,U_C,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_SC,U_NU,
// StringBuffer_appendCharArray
//"java/lang/StringBuffer",
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_NU,
//"append",
U_a,U_p,U_p,U_e,U_n,U_d,U_NU,
//"(C)Ljava/lang/StringBuffer;",
U_LP,U_C,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_SC,U_NU,
// StringBuffer_appendChar
//"java/lang/StringBuffer",
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_NU,
//"append",
U_a,U_p,U_p,U_e,U_n,U_d,U_NU,
//"(Z)Ljava/lang/StringBuffer;",
U_LP,U_Z,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_SC,U_NU,
// StringBuffer_appendBoolean
//"java/lang/StringBuffer",
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_NU,
//"append",
U_a,U_p,U_p,U_e,U_n,U_d,U_NU,
//"(I)Ljava/lang/StringBuffer;",
U_LP,U_I,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_SC,U_NU,
// StringBuffer_appendInt
//"java/lang/StringBuffer",
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_NU,
//"append",
U_a,U_p,U_p,U_e,U_n,U_d,U_NU,
//"(J)Ljava/lang/StringBuffer;",
U_LP,U_J,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_SC,U_NU,
// StringBuffer_appendLong
//"java/lang/StringBuffer",
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_NU,
//"append",
U_a,U_p,U_p,U_e,U_n,U_d,U_NU,
//"(F)Ljava/lang/StringBuffer;",
U_LP,U_F,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_SC,U_NU,
// StringBuffer_appendFloat
//"java/lang/StringBuffer",
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_NU,
//"append",
U_a,U_p,U_p,U_e,U_n,U_d,U_NU,
//"(D)Ljava/lang/StringBuffer;",
U_LP,U_D,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_SC,U_NU,
// StringBuffer_appendDouble
//"java/lang/StringBuffer",
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_NU,
//"append",
U_a,U_p,U_p,U_e,U_n,U_d,U_NU,
//"(Ljava/lang/String;)Ljava/lang/StringBuffer;",
U_LP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_SC,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_SC,U_NU,
// StringBuffer_appendString
//"java/lang/StringBuffer",
   U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_NU,
//"append",
U_a,U_p,U_p,U_e,U_n,U_d,U_NU,
//"(Ljava/lang/Object;)Ljava/lang/StringBuffer;"
U_LP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_O,U_b,U_j,U_e,U_c,U_t,U_SC,U_RP,U_L,U_j,U_a,U_v,U_a,U_SL,U_l,U_a,U_n,U_g,U_SL,U_S,U_t,U_r,U_i,U_n,U_g,U_B,U_u,U_f,U_f,U_e,U_r,U_SC,U_NU
   // StringBuffer_appendObject
#ifdef DD
#endif
   ,U_a // to mark end of last string
};

u2 ByteCode::RegisterMethod(int num)
{
    // return index for use in method reference, building an entry if necessary.
    int pos = num * 3;  // starting index of method descriptor
    // skip precdeing strings
    int i=0;
    char *p1, *p2, *p3, *p4;
    p1 = &registered_methods_data[0];
    for (i=0;i<num;i++) {
        while (*p1++); // skip first string
        while (*p1++); // skip second string
        while (*p1++); // skip third string
    }
    p2=p1;
    while (*p2++); // move p2 to stard of second string
    p3=p2;
    while (*p3++); // move p3 to stard of third string
    p4=p3;
    while(*p4++); // move past end of third string
    if (registered_methods[num] == 0) {
          registered_methods[num] = BuildMethodref(
                RegisterClass(p1, p2-p1-1),
                BuildNameAndType(
                    RegisterUtf8(p2, p3-p2-1),
                    RegisterUtf8(p3, p4-p3-1)));

    }
    return registered_methods[num];
}
u2 ByteCode::BuildMethodref(u2 cl_index, u2 nt_index)
{
    CONSTANT_Methodref_info *p = new CONSTANT_Methodref_info(CONSTANT_Methodref);

    p -> class_index = cl_index;
    p -> name_and_type_index = nt_index;
    constant_pool.Next() = p;
    return constant_pool.Length()-1;
}

u2 ByteCode::BuildNameAndType(u2 name, u2 type)
{
    CONSTANT_NameAndType_info *p = new CONSTANT_NameAndType_info(CONSTANT_NameAndType);

    p -> name_index = name;
    p -> descriptor_index = type;
    constant_pool.Next() = p;
    return constant_pool.Length()-1;
}


u2 ByteCode::BuildString(u2 si)
{
    CONSTANT_String_info *p = new CONSTANT_String_info(CONSTANT_String);

    p -> string_index = si;
    constant_pool.Next() = p;
    return constant_pool.Length()-1;
}

u2 ByteCode::BuildUtf8(char * s,int len)
{
    CONSTANT_Utf8_info *p = new CONSTANT_Utf8_info(CONSTANT_Utf8);
    // build Utf8 from ASCII string (assume no embedded nulls), so just do straight copy
    p -> bytes = new char[len];
    // compute number of bytes in Utf8 form
    for (int i=0;i<len;i++) {
        p -> bytes[i] = s[i];
    }
    p -> length_ = len;
    constant_pool.Next() = p;
    return constant_pool.Length()-1;
}
u2 ByteCode::RegisterClass(Utf8LiteralValue * lit) {
    if (lit == (Utf8LiteralValue *) 0) chaos("null argument to RegisterClass");
    if (lit -> constant_pool_class != class_id) {
        // kill values assigned in prior class
        lit -> constant_pool_index_Class = 0;
        lit -> constant_pool_index_String = 0;
        lit -> constant_pool_index = 0;
    }
    if (lit -> constant_pool_index_Class == 0){
        CONSTANT_Class_info *ci = new CONSTANT_Class_info(CONSTANT_Class);
        ci -> name_index = (RegisterUtf8(lit));
        constant_pool.Next() = ci;
        lit -> constant_pool_index_Class = constant_pool.Length()-1;
        lit -> constant_pool_class = class_id;
    }
    return lit -> constant_pool_index_Class;
}

u2 ByteCode::RegisterClass(char * str, int len) {
    return RegisterClass(this_control.Utf8_pool.FindOrInsert(str,len));
}

u2 ByteCode::RegisterDouble(DoubleLiteralValue * lit) {
    if (lit == (DoubleLiteralValue *) 0) chaos("null argument to RegisterDouble");
    if (lit -> constant_pool_index == 0 || lit -> constant_pool_class != class_id){
        lit -> constant_pool_index = BuildDouble(lit -> value);
        lit -> constant_pool_class = class_id;
    }
    return lit -> constant_pool_index;
}

u2 ByteCode::RegisterInteger(IntLiteralValue * lit) {
    if (lit == (IntLiteralValue *) 0) chaos("null argument to RegisterInteger");
    if (lit -> constant_pool_index == 0 || lit -> constant_pool_class != class_id){
        lit -> constant_pool_index = BuildInteger(lit -> value);
        lit -> constant_pool_class = class_id;
    }
    return lit -> constant_pool_index;
}

u2 ByteCode::RegisterLong(LongLiteralValue * lit) {
    if (lit == (LongLiteralValue *) 0) chaos("null argument to RegisterLong");
    if (lit -> constant_pool_index == 0|| lit -> constant_pool_class != class_id){
        lit -> constant_pool_index = BuildLong(lit -> value);
        lit -> constant_pool_class = class_id;
    }
    return lit -> constant_pool_index;
}
u2 ByteCode::RegisterFloat(FloatLiteralValue * lit) {
    if (lit == (FloatLiteralValue *) 0) chaos("null argument to RegisterFloat");
    if (lit -> constant_pool_index == 0 || lit -> constant_pool_class != class_id){
        lit -> constant_pool_index = BuildFloat(lit -> value);
        lit -> constant_pool_class = class_id;
    }
    return lit -> constant_pool_index;
}


u2 ByteCode::RegisterString(Utf8LiteralValue * lit) {
    if (lit == (Utf8LiteralValue *) 0) chaos("null argument to RegisterString");
    if (lit -> constant_pool_class != class_id) {
        // kill values assigned in prior class
        lit -> constant_pool_index_Class = 0;
        lit -> constant_pool_index_String = 0;
        lit -> constant_pool_index = 0;
    }
    if (lit -> constant_pool_index_String == 0){
        lit -> constant_pool_index_String = BuildString(RegisterUtf8(lit));
    }
    return lit -> constant_pool_index_String;
}

u2 ByteCode::RegisterUtf8(Utf8LiteralValue * lit) {
    if (lit == (Utf8LiteralValue *) 0) chaos("null argument to RegisterUtf8");
    if (lit -> constant_pool_class != class_id) {
        // kill values assigned in prior class
        lit -> constant_pool_index_Class = 0;
        lit -> constant_pool_index_String = 0;
        lit -> constant_pool_index = 0;
    }
    if (lit -> constant_pool_index  == 0){
        lit -> constant_pool_index = BuildUtf8(lit -> value,lit -> length);
        lit -> constant_pool_class = class_id;
    }
    return lit -> constant_pool_index;
}

u2 ByteCode::RegisterUtf8(char * str, int len) {
    return RegisterUtf8(this_control.Utf8_pool.FindOrInsert(str,len));
}



//  Methods to write out the byte code
Deprecated_attribute *  ByteCode::CreateDeprecatedAttribute()
{

    Deprecated_attribute * deprecated_attribute;
    u2 deprecated_attribute_name;
    u4 deprecated_attribute_length = 0;
    deprecated_attribute_name = RegisterUtf8(U8S_Deprecated, strlen(U8S_Deprecated));
    deprecated_attribute =  new Deprecated_attribute(deprecated_attribute_name, deprecated_attribute_length);
    return deprecated_attribute;
}
    
Synthetic_attribute *  ByteCode::CreateSyntheticAttribute()
{

    Synthetic_attribute * synthetic_attribute;
    u2 synthetic_attribute_name;
    u4 synthetic_attribute_length = 0;
    synthetic_attribute_name = RegisterUtf8(U8S_Synthetic, strlen(U8S_Synthetic));
    synthetic_attribute =  new Synthetic_attribute(synthetic_attribute_name, synthetic_attribute_length);
    return synthetic_attribute;
}
    
void ByteCode::FinishCode(TypeSymbol * type)
{
    // finish off code by writing SourceFile attribute
    // and InnerClasses attribute (if appropriate)
    u4 sourcefile_attribute_length = 2;
    int  file_name_start=0;
    int delim=-1; // location of last delimiter in file name
       TypeSymbol * class_type;
    int i;
    u2 name;
    char *file_name = this_semantic.lex_stream -> FileName();
    int file_name_length = this_semantic.lex_stream -> FileNameLength();
    for (i = file_name_length; i >= 0; i--)
    {
        if (file_name[i] == U_SLASH)
        {
            delim = i;
            break;
        }
    }
    // define Source Attribute

    if (delim != -1) {
        file_name_start =  delim+1;
        file_name_length -= (delim + 1);
    }
    name = RegisterUtf8(U8S_Sourcefile, strlen(U8S_Sourcefile));
    SourceFile_attribute * sourcefile_attribute = 
        new SourceFile_attribute(name, sourcefile_attribute_length);
    sourcefile_attribute -> sourcefile_index = 
                                             BuildUtf8(file_name+file_name_start, file_name_length);
    attributes.Next() = sourcefile_attribute;
    if (type==(TypeSymbol *) 0) return; // return if interface type
    if (type -> IsLocal() || type -> IsNested() || type -> NumNestedTypes() > 0) {

        // here to generate InnerClasses attribute
        u4 inner_classes_attribute_length = 0;
        name = RegisterUtf8(U8S_InnerClasses, strlen(U8S_InnerClasses));
        inner_classes_attribute =
            new InnerClasses_attribute(name, inner_classes_attribute_length);
        inner_classes_attribute -> attribute_name_index = name;
        Tuple<TypeSymbol *> owners;
        // need to build chain from this type to its owner all the way to the containing type
        // and then write that out in reverse order (so containing type comes first),
        // and then write out an entry for each immediately contained type
        TypeSymbol * this_type = type;
        if (this_type != type -> outermost_type) {
            for (this_type = type;this_type != ((TypeSymbol *) 0)  && this_type  != type -> outermost_type;this_type = this_type -> ContainingType()) {
                owners.Next() = this_type;
            }
        }

        for (i = owners.Length();i > 0; i--) {
            SetInnerAttribute(owners[i - 1]);
        }
          
        for (i = 0; i < type -> NumNestedTypes(); i++) {
            SetInnerAttribute(type -> NestedType(i));
        }

        inner_classes_attribute -> attribute_length = 
            inner_classes_attribute -> inner_classes.Length() * 8 + 2;
        attributes.Next() = inner_classes_attribute;
    }
}
void ByteCode::AddLocalVariableTableEntry(u2 start,u2 length,u2 name,u2 descriptor,u2 index)
{
    // make entry in local variable table
            int li = local_variable_table_attribute -> local_variable_table.NextIndex();
            local_variable_table_attribute -> local_variable_table[li].start_pc = start;
            local_variable_table_attribute -> local_variable_table[li].length = length;
            local_variable_table_attribute -> local_variable_table[li].name_index = name;
            local_variable_table_attribute -> local_variable_table[li].descriptor_index = descriptor;
            local_variable_table_attribute -> local_variable_table[li].index = index;
}

void ByteCode::SetInnerAttribute(TypeSymbol * itype)
{
    int i = inner_classes_attribute -> inner_classes.NextIndex();
    inner_classes_attribute -> inner_classes[i].inner_class_info_index = RegisterClass(itype -> fully_qualified_name);
    if (itype -> IsLocal()) {
        inner_classes_attribute -> inner_classes[i].outer_class_info_index = 0;
    }
    else {
        TypeSymbol * otype = itype -> ContainingType();
        inner_classes_attribute -> inner_classes[i].outer_class_info_index = RegisterClass(otype -> fully_qualified_name);
    }
    if (itype -> Anonymous()) {
        inner_classes_attribute -> inner_classes[i].inner_name_index = 0;
    }
    else {
        inner_classes_attribute -> inner_classes[i].inner_name_index = RegisterUtf8(itype -> name_symbol -> Utf8_literal);
    }
    AccessFlags flags;
    flags.access_flags = itype -> access_flags;
    inner_classes_attribute -> inner_classes[i].inner_class_access_flags = flags.access_flags;
}
//  Methods to insert values into byte code

void ByteCode::PutI1(i1 i)
{
    code_attribute -> code.Next() = i&0xff;
}

void ByteCode::PutI2(i2 i)
{
    code_attribute -> code.Next() = (i>>8) & 0xff;
    code_attribute -> code.Next() = i & 0xff;
}

void ByteCode::PutU1(u1 u)
{
    code_attribute -> code.Next() = u & 0xff;
}

void ByteCode::PutU2(u2 u)
{
    code_attribute -> code.Next() = (u>>8) & 0xff;
    code_attribute -> code.Next() = u & 0xff;
}

void ByteCode::PutU4(u4 u)
{
    code_attribute -> code.Next() = (u>>24) & 0xff;
    code_attribute -> code.Next() = (u>>16) & 0xff;
    code_attribute -> code.Next() = (u>>8) & 0xff;
    code_attribute -> code.Next() = u & 0xff;
}

//  stack_effect gives effect on stack of executing an opcode

static int stack_effect[] =
{
    0,  // OP_NOP 
    1,  // OP_ACONST_NULL 
    1,  // OP_ICONST_M1 
    1,  // OP_ICONST_0 
    1,  // OP_ICONST_1 
    1,  // OP_ICONST_2 
    1,  // OP_ICONST_3 
    1,  // OP_ICONST_4 
    1,  // OP_ICONST_5 
    2,  // OP_LCONST_0 
    2,  // OP_LCONST_1 
    1,  // OP_FCONST_0 
    1,  // OP_FCONST_1 
    1, // OP_FCONST_2 
    2,  // OP_DCONST_0 
    2,  // OP_DCONST_1 
    1,  // OP_BIPUSH 
    1,  // OP_SIPUSH 
    1,  // OP_LDC 
    1,  // OP_LDC_W 
    2,  // OP_LDC2_W 
    1,  // OP_ILOAD 
    2,  // OP_LLOAD 
    1,  // OP_FLOAD 
    2,  // OP_DLOAD 
    1,  // OP_ALOAD 
    1,  // OP_ILOAD_0 
    1,  // OP_ILOAD_1 
    1,  // OP_ILOAD_2 
    1,  // OP_ILOAD_3 
    2,  // OP_LLOAD_0 
    2,  // OP_LLOAD_1 
    2,  // OP_LLOAD_2 
    2,  // OP_LLOAD_3 
    1,  // OP_FLOAD_0 
    1,  // OP_FLOAD_1 
    1,  // OP_FLOAD_2 
    1,  // OP_FLOAD_3 
    2,  // OP_DLOAD_0 
    2,  // OP_DLOAD_1 
    2,  // OP_DLOAD_2 
    2,  // OP_DLOAD_3 
    1,  // OP_ALOAD_0 
    1,  // OP_ALOAD_1 
    1,  // OP_ALOAD_2 
    1,  // OP_ALOAD_3 
    -1,  // OP_IALOAD 
    0,  // OP_LALOAD 
    -1,  // OP_FALOAD 
    0,  // OP_DALOAD 
    -1,  // OP_AALOAD 
    -1,  // OP_BALOAD 
    -1,  // OP_CALOAD 
    -1,  // OP_SALOAD 
    -1, // OP_ISTORE 
    -2,  // OP_LSTORE 
    -1, // OP_FSTORE 
    -2,  // OP_DSTORE 
    -1,  // OP_ASTORE 
    -1,  // OP_ISTORE_0 
    -1,  // OP_ISTORE_1 
    -1,  // OP_ISTORE_2 
    -1,  // OP_ISTORE_3 
    -2,  // OP_LSTORE_0 
    -2,  // OP_LSTORE_1 
    -2,  // OP_LSTORE_2 
    -2,  // OP_LSTORE_3 
    -1,  // OP_FSTORE_0 
    -1,  // OP_FSTORE_1 
    -1,  // OP_FSTORE_2 
    -1,  // OP_FSTORE_3 
    -2,  // OP_DSTORE_0 
    -2,  // OP_DSTORE_1 
    -2,  // OP_DSTORE_2 
    -2,  // OP_DSTORE_3 
    -1,  // OP_ASTORE_0 
    -1,  // OP_ASTORE_1 
    -1,  // OP_ASTORE_2 
    -1,  // OP_ASTORE_3 
    -3,  // OP_IASTORE 
    -4,  // OP_LASTORE 
    -3,  // OP_FASTORE 
    -4,  // OP_DASTORE 
    -3,  // OP_AASTORE 
    -3,  // OP_BASTORE 
    -3,  // OP_CASTORE 
    -3,  // OP_SASTORE 
    -1,  // OP_POP 
    -2,  // OP_POP2 
    1,  // OP_DUP 
    1,  // OP_DUP_X1 
    1,  // OP_DUP_X2 
    2,  // OP_DUP2 
    2,  // OP_DUP2_X1 
    2,  // OP_DUP2_X2 
    0,  // OP_SWAP 
    -1,  // OP_IADD 
    -2,  // OP_LADD 
    -1,  // OP_FADD 
    -2,  // OP_DADD 
    -1,  // OP_ISUB 
    -2,  // OP_LSUB 
    -1,  // OP_FSUB 
    -2,  // OP_DSUB 
    -1,  // OP_IMUL 
    -2,  // OP_LMUL 
    -1,  // OP_FMUL 
    -2,  // OP_DMUL 
    -1,  // OP_IDIV 
    -2,  // OP_LDIV 
    -1,  // OP_FDIV 
    -2,  // OP_DDIV 
    -1,  // OP_IREM 
    -2,  // OP_LREM 
    -1,  // OP_FREM 
    -2,  // OP_DREM 
    0,  // OP_INEG 
    0,  // OP_LNEG 
    0,  // OP_FNEG 
    0,  // OP_DNEG 
    -1,  // OP_ISHL 
    -1,  // OP_LSHL 
    -1,  // OP_ISHR 
    -1,  // OP_LSHR 
    -1,  // OP_IUSHR 
    -1,  // OP_LUSHR 
    -1,  // OP_IAND 
    -2,  // OP_LAND 
    -1,  // OP_IOR 
    -2,  // OP_LOR 
    -1,  // OP_IXOR 
    -2,  // OP_LXOR 
    0,  // OP_IINC 
    1,  // OP_I2L 
    0,  // OP_I2F 
    1,  // OP_I2D 
    -1,  // OP_L2I 
    -1,  // OP_L2F 
    0,  // OP_L2D 
    0,  // OP_F2I 
    1,  // OP_F2L 
    1,  // OP_F2D 
    -1,  // OP_D2I 
    0,  // OP_D2L 
    -1,  // OP_D2F 
    0,  // OP_I2B 
    0,  // OP_I2C 
    0,  // OP_I2S 
    -3,  // OP_LCMP 
    -1,  // OP_FCMPL 
    -1,  // OP_FCMPG 
    -3,  // OP_DCMPL 
    -3,  // OP_DCMPG 
    -1,  // OP_IFEQ 
    -1,  // OP_IFNE 
    -1,  // OP_IFLT 
    -1,  // OP_IFGE 
    -1,  // OP_IFGT 
    -1,  // OP_IFLE 
    -2,  // OP_IF_ICMPEQ 
    -2,  // OP_IF_ICMPNE 
    -2,  // OP_IF_ICMPLT 
    -2,  // OP_IF_ICMPGE 
    -2,  // OP_IF_ICMPGT 
    -2,  // OP_IF_ICMPLE 
    -2,  // OP_IF_ACMPEQ 
    -2,  // OP_IF_ACMPNE 
    0,  // OP_GOTO 
    1,  // OP_JSR 
    0,  // OP_RET 
    -1,  // OP_TABLESWITCH 
    -1,  // OP_LOOKUPSWITCH 
    -1,  // OP_IRETURN 
    -2,  // OP_LRETURN 
    -1,  // OP_FRETURN 
    -2,  // OP_DRETURN 
    -1,  // OP_ARETURN 
    0,  // OP_RETURN 
    0,  // OP_GETSTATIC, caller must adjust if long or double
    0,  // OP_PUTSTATIC, caller must adjust if long or double
    0,  // OP_GETFIELD, caller must adjust if long or double
    0,  // OP_PUTFIELD, caller must adjust if long or double
    -1,  // OP_INVOKEVIRTUAL,   actually  -1-args_length
    -1,  // OP_INVOKENONVIRTUAL,    actually  -1-args_length
    0,  // OP_INVOKESTATIC, actually -args_length
    -1,  // OP_INVOKEINTERFACE, actually -1 -args_length
    0,  // OP_XXXUNUSEDXXX 
    1,  // OP_NEW 
    0,  // OP_NEWARRAY 
    0,  // OP_ANEWARRAY 
    0,  // OP_ARRAYLENGTH 
    0,  // OP_ATHROW 
    0,  // OP_CHECKCAST 
    0,  // OP_INSTANCEOF 
    -1,  // OP_MONITORENTER 
    -1,  // OP_MONITOREXIT 
    0,  // OP_WIDE 
    0,  // OP_MULTIANEWARRAY, actually dims-1
    -1,  // OP_IFNULL 
    -1,  // OP_IFNONNULL 
    0,  // OP_GOTO_W 
    1,  // OP_JSR_W 
    0,  // OP_SOFTWARE 
    0  // OP_HARDWARE
};

void ByteCode::PutNop(int optional)
{
    // emit NOP. The NOP can be replaced by the next instruction if 
    // optional is set; otherwise it must be kept.
    PutOp(OP_NOP);
// this optimization is causing more trouble than it's worth.
// latest problem (27 jan 97) was reported by Derek, in that
// nop just before label definition, resulted in operation generated
// after label def. being moved before the def! Since it's such a sin
// to generate junk code, disable the "nop" optimization.
//  if (optional) last_op_nop = 1;
}

void ByteCode::PutOp(unsigned char opc)
{
#ifdef TEST
    int len = code_attribute -> code.Length(); // show current position
    if (this_control.option.debug_trap_op >0 && code_attribute -> code.Length() == this_control.option.debug_trap_op) {
        op_trap();
    }
    // debug trick - force branch on opcode to see what opcode we are compiling
    switch (opc) {
        case OP_NOP: break;
        case OP_ACONST_NULL: break;
        case OP_ICONST_M1: break;
        case OP_ICONST_0: break;
        case OP_ICONST_1: break;
        case OP_ICONST_2: break;
        case OP_ICONST_3: break;
        case OP_ICONST_4: break;
        case OP_ICONST_5: break;
        case OP_LCONST_0: break;
        case OP_LCONST_1: break;
        case OP_FCONST_0: break;
        case OP_FCONST_1: break;
        case OP_FCONST_2: break;
        case OP_DCONST_0: break;
        case OP_DCONST_1: break;
        case OP_BIPUSH: break;
        case OP_SIPUSH: break;
        case OP_LDC: break;
        case OP_LDC_W: break;
        case OP_LDC2_W: break;
        case OP_ILOAD: break;
        case OP_LLOAD: break;
        case OP_FLOAD: break;
        case OP_DLOAD: break;
        case OP_ALOAD: break;
        case OP_ILOAD_0: break;
        case OP_ILOAD_1: break;
        case OP_ILOAD_2: break;
        case OP_ILOAD_3: break;
        case OP_LLOAD_0: break;
        case OP_LLOAD_1: break;
        case OP_LLOAD_2: break;
        case OP_LLOAD_3: break;
        case OP_FLOAD_0: break;
        case OP_FLOAD_1: break;
        case OP_FLOAD_2: break;
        case OP_FLOAD_3: break;
        case OP_DLOAD_0: break;
        case OP_DLOAD_1: break;
        case OP_DLOAD_2: break;
        case OP_DLOAD_3: break;
        case OP_ALOAD_0: break;
        case OP_ALOAD_1: break;
        case OP_ALOAD_2: break;
        case OP_ALOAD_3: break;
        case OP_IALOAD: break;
        case OP_LALOAD: break;
        case OP_FALOAD: break;
        case OP_DALOAD: break;
        case OP_AALOAD: break;
        case OP_BALOAD: break;
        case OP_CALOAD: break;
        case OP_SALOAD: break;
        case OP_ISTORE: break;
        case OP_LSTORE: break;
        case OP_FSTORE: break;
        case OP_DSTORE: break;
        case OP_ASTORE: break;
        case OP_ISTORE_0: break;
        case OP_ISTORE_1: break;
        case OP_ISTORE_2: break;
        case OP_ISTORE_3: break;
        case OP_LSTORE_0: break;
        case OP_LSTORE_1: break;
        case OP_LSTORE_2: break;
        case OP_LSTORE_3: break;
        case OP_FSTORE_0: break;
        case OP_FSTORE_1: break;
        case OP_FSTORE_2: break;
        case OP_FSTORE_3: break;
        case OP_DSTORE_0: break;
        case OP_DSTORE_1: break;
        case OP_DSTORE_2: break;
        case OP_DSTORE_3: break;
        case OP_ASTORE_0: break;
        case OP_ASTORE_1: break;
        case OP_ASTORE_2: break;
        case OP_ASTORE_3: break;
        case OP_IASTORE: break;
        case OP_LASTORE: break;
        case OP_FASTORE: break;
        case OP_DASTORE: break;
        case OP_AASTORE: break;
        case OP_BASTORE: break;
        case OP_CASTORE: break;
        case OP_SASTORE: break;
        case OP_POP: break;
        case OP_POP2: break;
        case OP_DUP: break;
        case OP_DUP_X1: break;
        case OP_DUP_X2: break;
        case OP_DUP2: break;
        case OP_DUP2_X1: break;
        case OP_DUP2_X2: break;
        case OP_SWAP: break;
        case OP_IADD: break;
        case OP_LADD: break;
        case OP_FADD: break;
        case OP_DADD: break;
        case OP_ISUB: break;
        case OP_LSUB: break;
        case OP_FSUB: break;
        case OP_DSUB: break;
        case OP_IMUL: break;
        case OP_LMUL: break;
        case OP_FMUL: break;
        case OP_DMUL: break;
        case OP_IDIV: break;
        case OP_LDIV: break;
        case OP_FDIV: break;
        case OP_DDIV: break;
        case OP_IREM: break;
        case OP_LREM: break;
        case OP_FREM: break;
        case OP_DREM: break;
        case OP_INEG: break;
        case OP_LNEG: break;
        case OP_FNEG: break;
        case OP_DNEG: break;
        case OP_ISHL: break;
        case OP_LSHL: break;
        case OP_ISHR: break;
        case OP_LSHR: break;
        case OP_IUSHR: break;
        case OP_LUSHR: break;
        case OP_IAND: break;
        case OP_LAND: break;
        case OP_IOR: break;
        case OP_LOR: break;
        case OP_IXOR: break;
        case OP_LXOR: break;
        case OP_IINC: break;
        case OP_I2L: break;
        case OP_I2F: break;
        case OP_I2D: break;
        case OP_L2I: break;
        case OP_L2F: break;
        case OP_L2D: break;
        case OP_F2I: break;
        case OP_F2L: break;
        case OP_F2D: break;
        case OP_D2I: break;
        case OP_D2L: break;
        case OP_D2F: break;
        case OP_I2B: break;
        case OP_I2C: break;
        case OP_I2S: break;
        case OP_LCMP: break;
        case OP_FCMPL: break;
        case OP_FCMPG: break;
        case OP_DCMPL: break;
        case OP_DCMPG: break;
        case OP_IFEQ: break;
        case OP_IFNE: break;
        case OP_IFLT: break;
        case OP_IFGE: break;
        case OP_IFGT: break;
        case OP_IFLE: break;
        case OP_IF_ICMPEQ: break;
        case OP_IF_ICMPNE: break;
        case OP_IF_ICMPLT: break;
        case OP_IF_ICMPGE: break;
        case OP_IF_ICMPGT: break;
        case OP_IF_ICMPLE: break;
        case OP_IF_ACMPEQ: break;
        case OP_IF_ACMPNE: break;
        case OP_GOTO: break;
        case OP_JSR: break;
        case OP_RET: break;
        case OP_TABLESWITCH: break;
        case OP_LOOKUPSWITCH: break;
        case OP_IRETURN: break;
        case OP_LRETURN: break;
        case OP_FRETURN: break;
        case OP_DRETURN: break;
        case OP_ARETURN: break;
        case OP_RETURN: break;
        case OP_GETSTATIC: break;
        case OP_PUTSTATIC: break;
        case OP_GETFIELD: break;
        case OP_PUTFIELD: break;
        case OP_INVOKEVIRTUAL: break;
        case OP_INVOKENONVIRTUAL: break;
        case OP_INVOKESTATIC: break;
        case OP_INVOKEINTERFACE: break;
        case OP_XXXUNUSEDXXX: break;
        case OP_NEW: break;
        case OP_NEWARRAY: break;
        case OP_ANEWARRAY: break;
        case OP_ARRAYLENGTH: break;
        case OP_ATHROW: break;
        case OP_CHECKCAST: break;
        case OP_INSTANCEOF: break;
        case OP_MONITORENTER: break;
        case OP_MONITOREXIT: break;
        case OP_WIDE: break;
        case OP_MULTIANEWARRAY: break;
        case OP_IFNULL: break;
        case OP_IFNONNULL: break;
        case OP_GOTO_W: break;
        case OP_JSR_W: break;
        case OP_SOFTWARE: break;
        case OP_HARDWARE: break;
    }
#endif
    last_op_pc = code_attribute -> code.Length(); // save pc at start of operation
#ifdef NOP_OPT
// this optimization doesn't work - disable for now
//  if (last_op_nop) {
//      last_op_nop = 0;
//      code_attribute -> code[last_op_pc - 1] =  opc;
//  }
//  else {
//      code_attribute -> code.Next() =  opc;
//  }
#else
        code_attribute -> code.Next() =  opc;
#endif
    ChangeStack(stack_effect[opc]);
}
void ByteCode::ChangeStack(int i)
{
    stack_depth += i;
    if (stack_depth < 0) stack_depth = 0;
    if (i>0 && stack_depth > code_attribute -> max_stack) {
        code_attribute -> max_stack = stack_depth;
    }
#ifdef TRACE_STACK_CHANGE
    cout << "stack change: pc " << last_op_pc << " change " << i <<
            "  stack_depth " << stack_depth << "  max_stack: "<< code_attribute -> max_stack << "\n";
#endif
}

#ifdef TEST
void ByteCode::PrintCode()
{
    int i;
    cout << "magic " << hex << magic << dec
            << " major_version " << major_version
            << " minor_version " << minor_version << "\n";
    AccessFlags::Print();
    cout << "\n";
    cout << " this_class " << this_class << "  super_class " << super_class <<"\n";
    cout << " constant_pool: " << constant_pool.Length() << "\n";
    for (i=1;i<constant_pool.Length();i++) {
        cout << "  " << i << "  ";
        constant_pool[i] -> print(constant_pool);
        if (constant_pool[i] -> tag == CONSTANT_Long ||
            constant_pool[i] -> tag == CONSTANT_Double) {
            i++;; // skip the next entry for eight-byte constants
        }
    }
    cout << "  interfaces " << interfaces.Length() <<": ";
    for (i=0;i<interfaces.Length();i++) cout << "  " << (int) interfaces[i];
    cout <<"\n";
    for (i=0;i<fields.Length();i++) {
        cout << "field " << i << "\n";
        fields[i].print(constant_pool);
    }
    cout << " methods length " << methods.Length() << "\n";
    for (i=0;i<methods.Length();i++) {
        cout << "method " << i << "\n";
        methods[i].print(constant_pool);
    }
    for (i=0;i<attributes.Length();i++) {
        cout << "attribute " << i << "\n";
        attributes[i] -> print(constant_pool);
    }
    cout << "\n";
} 
#endif
