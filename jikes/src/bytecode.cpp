// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
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

void ByteCode::CompileClass()
{
    AstClassDeclaration *class_decl = unit_type -> declaration -> ClassDeclarationCast();
    AstClassBody *class_body = (class_decl
                                     ? class_decl -> class_body
                                     : ((AstClassInstanceCreationExpression *) unit_type -> declaration) -> class_body_opt);

    //
    // Process static variables.
    //
    Tuple<AstVariableDeclarator *> initialized_static_fields(unit_type -> NumVariableSymbols()); // fields needing code to initialize
    {
        for (int i = 0; i < class_body -> NumClassVariables(); i++)
        {
            AstFieldDeclaration *field_decl = class_body -> ClassVariable(i);
            for (int vi = 0; vi < field_decl -> NumVariableDeclarators(); vi++)
            {
                AstVariableDeclarator *vd = field_decl -> VariableDeclarator(vi);
                VariableSymbol *vsym = vd -> symbol;
                DeclareField(vsym);

                //
                // We need a static constructor-initializer if we encounter at least one class
                // variable that is declared with an initialization expression that is not a
                // constant expression.
                //
                if (vd -> variable_initializer_opt && (! (vsym -> ACC_FINAL() && vsym -> initial_value)))
                    initialized_static_fields.Next() = vd;
            }
        }
    }
    
    //
    // Process instance variables.
    //
    Tuple<AstVariableDeclarator *> initialized_instance_fields(unit_type -> NumVariableSymbols()); // fields needing code to init
    {
        for (int i = 0; i < class_body -> NumInstanceVariables(); i++)
        {
            AstFieldDeclaration *field_decl  = class_body -> InstanceVariable(i);
            for (int vi = 0; vi < field_decl -> NumVariableDeclarators(); vi++)
            {
                AstVariableDeclarator *vd = field_decl -> VariableDeclarator(vi);
                DeclareField(vd -> symbol);

                //
                // must set Constant attribute if initial value specified
                //
                if (vd -> variable_initializer_opt)
                    initialized_instance_fields.Next() = vd;
            }
        }
    }

    //
    // supply needed field declaration for this$0 and any additional local shadow parameters
    //
    {
        for (int i = 0; i < unit_type -> NumConstructorParameters(); i++)
            DeclareField(unit_type -> ConstructorParameter(i));
    }

    //
    // supply needed field declaration for enclosing instances (this$n, n > 0) if present
    //
    {
        for (int i = 1; i < unit_type -> NumEnclosingInstances(); i++)
            DeclareField(unit_type -> EnclosingInstance(i));
    }

    //
    // supply needed field declarations for "class " identifiers (used for X.class literals) if present
    //
    {
        for (int i = 0; i < unit_type -> NumClassLiterals(); i++)
            DeclareField(unit_type -> ClassLiteral(i));
    }

    //
    // compile method bodies
    //
    {
        for (int i = 0; i < class_body -> NumMethods(); i++)
        {
            AstMethodDeclaration *method = class_body -> Method(i);
            if (method -> method_symbol)
            {
                int method_index = methods.NextIndex(); // index for method

                BeginMethod(method_index, method -> method_symbol);
                AstBlock *method_block = method -> method_body -> BlockCast();
                if (method_block) // not an abstract method ?
                    EmitStatement(method_block);
                EndMethod(method_index, method -> method_symbol);
            }
        }
    }

    //
    // NOTE that an abstract class that requires this patch may become out-of-date
    // and cause spurious messages to be emitted if any abstract method inherited
    // from an interface is later removed from that interface.
    //
    if (unit_type -> ACC_ABSTRACT())
    {
        for (int i = 0; i < unit_type -> expanded_method_table -> symbol_pool.Length(); i++)
        {
            MethodShadowSymbol *method_shadow_symbol = unit_type -> expanded_method_table -> symbol_pool[i];
            MethodSymbol *method_symbol = method_shadow_symbol -> method_symbol;
            if (method_symbol -> ACC_ABSTRACT() &&
                method_symbol -> containing_type != unit_type &&
                method_symbol -> containing_type -> ACC_INTERFACE())
            {
                if (! method_symbol -> IsTyped())
                    method_symbol -> ProcessMethodSignature(&this_semantic, class_decl -> identifier_token);
                method_symbol -> ProcessMethodThrows(&this_semantic, class_decl -> identifier_token);

                BeginMethod(methods.NextIndex(), method_symbol);
            }
        }
    }

    //
    // compile any private access methods
    //
    {
        for (int i = 0; i < unit_type -> NumPrivateAccessMethods(); i++)
        {
            int method_index = methods.NextIndex(); // index for method

            MethodSymbol *method_sym = unit_type -> PrivateAccessMethod(i);
            BeginMethod(method_index, method_sym);
            GenerateAccessMethod(method_sym);
            EndMethod(method_index, method_sym);
        }
    }

    //
    //
    //
    MethodSymbol *class_literal_sym = unit_type -> ClassLiteralMethod();
    if (class_literal_sym)
    {
        //
        // Generate the class$ identity method used for class literal-related garbage mumbo-jumbo initialization
        //
        int method_index = methods.NextIndex(); // index for method
        BeginMethod(method_index, class_literal_sym);
        GenerateClassAccessMethod(class_literal_sym);
        EndMethod(method_index, class_literal_sym);
    }

    //
    //
    //
    MethodSymbol *block_init_method = unit_type -> block_initializer_method;
    if (block_init_method)
    {
        int method_index = methods.NextIndex(); // index for method
        BeginMethod(method_index, block_init_method);
   
        int fi = 0,
            bi = 0;
        while (fi < initialized_instance_fields.Length() && bi < class_body -> NumBlocks())
        {
            if (initialized_instance_fields[fi] -> LeftToken() < class_body -> Block(bi) -> left_brace_token)
                 InitializeInstanceVariable(initialized_instance_fields[fi++]);
            else EmitStatement(class_body -> Block(bi++));
        }
                       
        while (fi < initialized_instance_fields.Length())
            InitializeInstanceVariable(initialized_instance_fields[fi++]);
    
        //
        // compile any initialization blocks
        //
        while (bi < class_body -> NumBlocks())
            EmitStatement(class_body -> Block(bi++));
        
        PutOp(OP_RETURN);
        EndMethod(method_index, block_init_method);
    }
    
    //
    //
    //
    if (unit_type -> NumGeneratedConstructors() == 0)
    {
        if (class_body -> default_constructor)
            CompileConstructor(class_body -> default_constructor, initialized_instance_fields);
        else
        {
            for (int i = 0; i < class_body -> NumConstructors(); i++)
            {
                AstConstructorDeclaration *constructor = class_body -> Constructor(i);
                CompileConstructor(constructor, initialized_instance_fields);
            }
       
            for (int k = 0; k < unit_type -> NumPrivateAccessConstructors(); k++)
            {
                MethodSymbol *constructor_sym = unit_type -> PrivateAccessConstructor(k);
                AstConstructorDeclaration *constructor =
                       constructor_sym -> method_or_constructor_declaration -> ConstructorDeclarationCast();
                CompileConstructor(constructor, initialized_instance_fields);
            }
        }
    }
    else
    {
        for (int i = 0; i < unit_type -> NumGeneratedConstructors(); i++)
        {
            MethodSymbol *this_constructor_symbol = unit_type -> GeneratedConstructor(i);
            AstConstructorDeclaration *constructor =
                    this_constructor_symbol -> method_or_constructor_declaration -> ConstructorDeclarationCast();
            AstConstructorBlock *constructor_block = constructor -> constructor_body -> ConstructorBlockCast();

            //
            // compile generated constructor
            //
            int method_index = methods.NextIndex(); // index for method
            BeginMethod(method_index, this_constructor_symbol);
            methods[method_index].SetNameIndex(RegisterUtf8(U8S_LT_init_GT, strlen(U8S_LT_init_GT)));

            UpdateBlockInfo(this_constructor_symbol -> block_symbol);
                
            if (! constructor_block -> explicit_constructor_invocation_opt)
            {
                PutOp(OP_ALOAD_0);
                PutOp(OP_INVOKENONVIRTUAL);   // no args, hence no need to call ChangeStack()
                PutU2(BuildMethodref(super_class,
                                     BuildNameAndType(RegisterUtf8(U8S_LT_init_GT, strlen(U8S_LT_init_GT)),
                                                      RegisterUtf8(U8S_LP_RP_V, strlen(U8S_LP_RP_V)))));
            }
            else EmitStatement((AstStatement *) constructor_block -> explicit_constructor_invocation_opt);

            for (int si = 0; si < constructor_block -> NumLocalInitStatements(); si++)
                EmitStatement(constructor_block -> LocalInitStatement(si));

            //
            // supply needed field initialization unless constructor
            // starts with explicit 'this' call to another constructor
            //
            if (! (constructor_block -> explicit_constructor_invocation_opt &&
                   constructor_block -> explicit_constructor_invocation_opt -> ThisCallCast()))
            {
                if (unit_type -> NumEnclosingInstances())
                {
                    VariableSymbol *this0_parameter = unit_type -> EnclosingInstance(0);
                    PutOp(OP_ALOAD_0); // load address of object on which method is to be invoked
                    LoadLocal(1, this0_parameter -> Type());
                    PutOp(OP_PUTFIELD);
                    PutU2(GenerateFieldReference(this0_parameter));
                }

                if (class_body -> this_block) // compile explicit 'this' call if present
                {
                    AstBlock *block = class_body -> this_block;
                    for (int si = 0; si < block -> NumStatements(); si++)
                        EmitStatement((AstStatement *) block -> Statement(si));
                }
                
                if (! unit_type -> block_initializer_method)
                {
                    int fi = 0,
                        bi = 0;
                    while (fi < initialized_instance_fields.Length() && bi < class_body -> NumBlocks())
                    {
                        if (initialized_instance_fields[fi] -> LeftToken() < class_body -> Block(bi) -> left_brace_token)
                            InitializeInstanceVariable(initialized_instance_fields[fi++]);
                        else
                        {
                            AstBlock *block = class_body -> Block(bi++);
                            for (int si = 0; si < block -> NumStatements(); si++)
                                EmitStatement((AstStatement *) block -> Statement(si));
                        }
                    }
                        
                    while (fi < initialized_instance_fields.Length())
                        InitializeInstanceVariable(initialized_instance_fields[fi++]);

                    //
                    // compile any initialization blocks
                    //
                    while (bi < class_body -> NumBlocks())
                    {
                        AstBlock *block = class_body -> Block(bi++);
                        for (int si = 0; si < block -> NumStatements(); si++)
                            EmitStatement((AstStatement *) block -> Statement(si));
                    }
                }
                else
                {
                    //
                    // generate a call to the parameterless function block_initializer_function
                    //
                    PutOp(OP_ALOAD_0); // load address of object on which method is to be invoked
                    PutOp(OP_INVOKENONVIRTUAL);  
                    CompleteCall(unit_type -> block_initializer_method, 0);
                }
            }

            EmitStatement(constructor_block -> original_constructor_invocation);
            PutOp(OP_RETURN);
            EndMethod(method_index, this_constructor_symbol);

            //
            // compile method associated with generated constructor
            //
            MethodSymbol *local_constructor_symbol = this_constructor_symbol -> LocalConstructor();
            method_index = methods.NextIndex(); // index for method
            BeginMethod(method_index, local_constructor_symbol);  // is constructor
            methods[method_index].SetNameIndex(RegisterUtf8(local_constructor_symbol -> ExternalIdentity() -> Utf8_literal));

            for (int k = 0; k < constructor_block -> block -> NumStatements(); k++)
                EmitStatement((AstStatement *) constructor_block -> block -> Statement(k));

            EndMethod(method_index, local_constructor_symbol);
        }
    }
    
    //
    // If we need to generate a static initializer...
    //
    if (unit_type -> static_initializer_method)
    {
        assert(class_body -> NumStaticInitializers() > 0 || initialized_static_fields.Length() > 0);

        int method_index = methods.NextIndex(); // index for method
        BeginMethod(method_index, unit_type -> static_initializer_method);

        //
        // revisit members that are part of class initialization
        //
        int fi = 0,
            bi = 0;
        while (fi < class_body -> NumStaticInitializers() && bi < initialized_static_fields.Length())
        {
            if (class_body -> StaticInitializer(fi) -> static_token < initialized_static_fields[bi] -> LeftToken())
                 EmitStatement(class_body -> StaticInitializer(fi++) -> block);
            else InitializeClassVariable(initialized_static_fields[bi++]);
        }
                       
        while (fi < class_body -> NumStaticInitializers())
            EmitStatement(class_body -> StaticInitializer(fi++) -> block);
    
        while (bi < initialized_static_fields.Length())
            InitializeClassVariable(initialized_static_fields[bi++]);

        PutOp(OP_RETURN);
        EndMethod(method_index, unit_type -> static_initializer_method);
    }
    else assert(class_body -> NumStaticInitializers() == 0 && initialized_static_fields.Length() == 0);

    FinishCode(unit_type);

    Write();

#ifdef TEST
    if (this_control.option.debug_dump_class)
        PrintCode();
#endif
}


void ByteCode::CompileInterface()
{
    AstInterfaceDeclaration *interface_decl = unit_type -> declaration -> InterfaceDeclarationCast();

    Tuple<AstVariableDeclarator *> initialized_fields(unit_type -> NumVariableSymbols()); // fields needing code to initialize
    {
        for (int i = 0; i < interface_decl -> NumClassVariables(); i++)
        {
            AstFieldDeclaration *field_decl = interface_decl -> ClassVariable(i);

            for (int vi = 0; vi < field_decl -> NumVariableDeclarators(); vi++)
            {
                AstVariableDeclarator *vd = field_decl -> VariableDeclarator(vi);
                VariableSymbol *vsym = vd -> symbol;
                //
                // We need a static constructor-initializer if we encounter at least one class
                // variable that is declared with an initialization expression that is not a
                // constant expression.
                //
                if (! vsym -> initial_value)
                    initialized_fields.Next() = vd;

                DeclareField(vsym);
            }
        }
    }

    //
    // Process all method members
    //
    {
        for (int i = 0; i < interface_decl -> NumMethods(); i++)
        {
            AstMethodDeclaration *method = interface_decl -> Method(i);
            if (method -> method_symbol)
                BeginMethod(methods.NextIndex(), method -> method_symbol);
        }
    }

    //
    // If this interface contained field with initial value
    //
    if (unit_type -> static_initializer_method)
    {
        assert(initialized_fields.Length() > 0);

        int method_index = methods.NextIndex(); // index for method
        BeginMethod(method_index, unit_type -> static_initializer_method);
        methods[method_index].SetACC_FINAL();

        for (int i = 0; i < initialized_fields.Length(); i++)
            InitializeClassVariable(initialized_fields[i]);

        PutOp(OP_RETURN);
        EndMethod(method_index, unit_type -> static_initializer_method);
    }
    else assert(initialized_fields.Length() == 0);

    FinishCode(unit_type);

    Write();

#ifdef TEST
    if (this_control.option.debug_dump_class)
        PrintCode();
#endif

    return;
}


//
// initialized_fields is a list of fields needing code to initialize.
//
void ByteCode::CompileConstructor(AstConstructorDeclaration *constructor, Tuple<AstVariableDeclarator *> &initialized_fields)
{
    MethodSymbol *method_symbol = constructor -> constructor_symbol;
    TypeSymbol *type = method_symbol -> containing_type;
    AstClassDeclaration *class_decl = type -> declaration -> ClassDeclarationCast();
    AstClassBody *class_body = (class_decl ? class_decl -> class_body
                                           : ((AstClassInstanceCreationExpression *) type -> declaration) -> class_body_opt);

    int method_index = methods.NextIndex(); // index for method
    BeginMethod(method_index, method_symbol);
    methods[method_index].SetNameIndex(RegisterUtf8(U8S_LT_init_GT, strlen(U8S_LT_init_GT)));

    UpdateBlockInfo(method_symbol -> block_symbol);

    AstConstructorBlock *constructor_block = constructor -> constructor_body -> ConstructorBlockCast();
    if (! constructor_block -> explicit_constructor_invocation_opt)
    {
        if (super_class)
        {
            PutOp(OP_ALOAD_0);
            PutOp(OP_INVOKENONVIRTUAL);   // no args, hence no need to call ChangeStack()
            PutU2(BuildMethodref(super_class,
                                 BuildNameAndType(RegisterUtf8(U8S_LT_init_GT, strlen(U8S_LT_init_GT)),
                                 RegisterUtf8(U8S_LP_RP_V, strlen(U8S_LP_RP_V)))));
        }
    }
    else EmitStatement((AstStatement *) constructor_block -> explicit_constructor_invocation_opt);

    // supply needed field initialization unless constructor
    // starts with explicit 'this' call to another constructor
    if (! (constructor_block -> explicit_constructor_invocation_opt &&
           constructor_block -> explicit_constructor_invocation_opt -> ThisCallCast()))
    {
        if (type -> NumEnclosingInstances())
        {
            VariableSymbol *this0_parameter = type -> EnclosingInstance(0);
            PutOp(OP_ALOAD_0); // load address of object on which method is to be invoked
            LoadLocal(1, this0_parameter -> Type());
            PutOp(OP_PUTFIELD);
            PutU2(GenerateFieldReference(this0_parameter));
        }

        if (class_body -> this_block) // compile explicit 'this' call if present
        {
            AstBlock *block = class_body -> this_block;
            for (int si = 0; si < block -> NumStatements(); si++)
                EmitStatement((AstStatement *) block -> Statement(si));
        }

        if (! type -> block_initializer_method)
        {
            int fi = 0,
                bi = 0;

            while (fi < initialized_fields.Length() && bi < class_body -> NumBlocks())
            {
                if (initialized_fields[fi] -> LeftToken() < class_body -> Block(bi) -> left_brace_token)
                    InitializeInstanceVariable(initialized_fields[fi++]);
                else
                {
                    AstBlock *block = class_body -> Block(bi++);
                    for (int si = 0; si < block -> NumStatements(); si++)
                        EmitStatement((AstStatement *) block -> Statement(si));
                }
            }
                
            while (fi < initialized_fields.Length())
                InitializeInstanceVariable(initialized_fields[fi++]);
    
            // compile any initialization blocks
            while (bi < class_body -> NumBlocks())
            {
                AstBlock *block = class_body -> Block(bi++);
                for (int si = 0; si < block -> NumStatements(); si++)
                    EmitStatement((AstStatement *) block -> Statement(si));
            }
        }
        else // generate a call to the parameterless function block_initializer_function
        {
            PutOp(OP_ALOAD_0); // load address of object on which method is to be invoked
            PutOp(OP_INVOKENONVIRTUAL);  
            CompleteCall(type -> block_initializer_method, 0);
        }
    }

    EmitStatement(constructor_block -> block);

    EndMethod(method_index, method_symbol);

    return;
}


void ByteCode::DeclareField(VariableSymbol *symbol)
{
    int field_index = fields.NextIndex(); // index for field

    fields[field_index].access_flags = symbol -> access_flags;
    fields[field_index].SetNameIndex(RegisterUtf8(symbol -> ExternalIdentity() -> Utf8_literal));
    fields[field_index].SetDescriptorIndex(RegisterUtf8(symbol -> Type() -> signature));

    TypeSymbol *type = symbol -> Type();
    if (symbol -> ACC_FINAL() &&
        symbol -> initial_value &&
        (type -> Primitive() || (type == this_control.String() && symbol -> initial_value != this_control.NullValue())))
    {
        fields[field_index].AddAttribute(new ConstantValue_attribute(RegisterUtf8(U8S_ConstantValue, U8S_ConstantValue_length),
                                                                     GetConstant(symbol -> initial_value, symbol -> Type())));
    }

    if (symbol -> IsSynthetic())
        fields[field_index].AddAttribute(CreateSyntheticAttribute());

    return;
}
            

//
// Generate code for access method to private member of containing class
//
void ByteCode::GenerateAccessMethod(MethodSymbol *method_symbol)
{
    assert(method_symbol -> ACC_STATIC());

    int stack_words = 0,
        argument_offset = 0; // offset to start of argument

    //
    // Load the parameters
    //
    for (int i = 0; i < method_symbol -> NumFormalParameters(); i++)
    {
        TypeSymbol *local_type = method_symbol -> FormalParameter(i) -> Type();
        stack_words += GetTypeWords(local_type);
        LoadLocal(argument_offset, local_type);
        argument_offset += GetTypeWords(local_type); // update position in stack
    }

    MethodSymbol *method_sym = method_symbol -> accessed_member -> MethodCast();
    if (method_sym)
    {
        PutOp(method_sym -> ACC_STATIC() ? OP_INVOKESTATIC : OP_INVOKENONVIRTUAL);  
        CompleteCall(method_sym, stack_words);
    }
    else
    {
        VariableSymbol *field_sym = method_symbol -> accessed_member -> VariableCast();

        if (method_symbol -> Type() == this_control.void_type) // writing to a field
        {
            TypeSymbol *parameter_type = method_symbol -> FormalParameter(field_sym -> ACC_STATIC() ? 0 : 1) -> Type();
            PutOp(field_sym -> ACC_STATIC() ? OP_PUTSTATIC : OP_PUTFIELD);
            PutU2(GenerateFieldReference(field_sym));
            ChangeStack(this_control.IsDoubleWordType(parameter_type) ? -2 : -1);
        }
        else // reading a field: need method to retrieve value of field
        {
            PutOp(field_sym -> ACC_STATIC() ? OP_GETSTATIC : OP_GETFIELD);
            PutU2(GenerateFieldReference(field_sym));
            ChangeStack(this_control.IsDoubleWordType(method_symbol -> Type()) ? 2 : 1);
        }
    }

    //
    // Method returns void, generate code for expression-less return statement.
    // Otherwise, call GenerateReturn to generate proper code.
    //
    if (method_symbol -> Type() == this_control.void_type)
         PutOp(OP_RETURN);
    else GenerateReturn(method_symbol -> Type());

    //
    // here to emit noop if would otherwise EmitBranch past end
    //
    if (last_label_pc >= code_attribute -> CodeLength())
        PutNop(0);

    return;
}


void ByteCode::BeginMethod(int method_index, MethodSymbol *msym) 
{
    assert(msym);

    MethodInitialization();
    method_type = msym -> Type(); // save the type of the method being compiled

    if (msym -> Identity() == this_control.clinit_name_symbol) // Generated constructor for static init.
    {
         methods[method_index].SetNameIndex(RegisterUtf8(U8S_LT_clinit_GT, strlen(U8S_LT_clinit_GT)));
         methods[method_index].SetDescriptorIndex(RegisterUtf8(U8S_LP_RP_V, strlen(U8S_LP_RP_V)));
    }
    else if (msym -> Identity() == this_control.init_name_symbol) // a constructor, caller sets name
         methods[method_index].SetDescriptorIndex(RegisterUtf8(msym -> signature));
    else
    {
         methods[method_index].SetNameIndex(RegisterUtf8(msym -> ExternalIdentity() -> Utf8_literal));
         methods[method_index].SetDescriptorIndex(RegisterUtf8(msym -> signature));
    }

    //
    //
    //
    methods[method_index].access_flags = msym -> access_flags;

    if (msym -> IsSynthetic())
        methods[method_index].AddAttribute(CreateSyntheticAttribute());

    //
    // Generate throws attribute if method throws any exceptions
    //
    if (msym -> NumThrows())
    {
        Exceptions_attribute *exceptions_attribute = new Exceptions_attribute(RegisterUtf8(U8S_Exceptions, U8S_Exceptions_length));
        for (int i = 0; i < msym -> NumThrows(); i++)
            exceptions_attribute -> AddExceptionIndex(RegisterClass(msym -> Throws(i) -> fully_qualified_name));
        methods[method_index].AddAttribute(exceptions_attribute);
    }

    //
    // If the method is contained in an interface and it is not a generated static initializer,
    // no further processing ins needed
    //
    if (msym -> containing_type -> ACC_INTERFACE() && msym -> Identity() != this_control.clinit_name_symbol)
        return;

    //
    // here if need code and associated attributes.
    //
    if (this_control.option.g)
        local_variable_table_attribute = new LocalVariableTable_attribute(RegisterUtf8(U8S_LocalVariableTable,
                                                                                       U8S_LocalVariableTable_length));

    if (! (msym -> ACC_ABSTRACT() || msym -> ACC_NATIVE()))
    {
        AllocateMethodInfo(msym -> max_block_depth);

        code_attribute = new Code_attribute(RegisterUtf8(U8S_Code, U8S_Code_length), msym -> block_symbol -> max_variable_index);

        line_number = 0;
        line_number_table_attribute = new LineNumberTable_attribute(RegisterUtf8(U8S_LineNumberTable, U8S_LineNumberTable_length));
    }

    last_parameter_index = (msym -> NumFormalParameters()
                                  ? msym -> FormalParameter(msym -> NumFormalParameters() - 1) -> LocalVariableIndex()
                                  : -1);

    return;
}


void ByteCode::EndMethod(int method_index, MethodSymbol *msym) 
{
    assert(msym);

    if (! (msym -> ACC_ABSTRACT() || msym -> ACC_NATIVE()))
    {
        if (last_label_pc >= code_attribute -> CodeLength()) // here to emit noop if would otherwise branch past end
            PutNop(0);

        //            
        // attribute length:
        // need to review how to make attribute_name and attribute_length
        // only write line number attribute if -O not specified and there
        // are line numbers to write.
        //            
        if ((! this_control.option.O) && line_number_table_attribute -> LineNumberTableLength() > 0)
             code_attribute -> AddAttribute(line_number_table_attribute);
        else delete line_number_table_attribute; // line_number_table_attribute not needed, so delete it now

        //
        // debug & not dealing with generated accessed method 
        //
        if (this_control.option.g && (! msym -> accessed_member) && (msym -> Identity() != this_control.class_name_symbol))
        {
            if (! msym -> ACC_STATIC()) // add 'this' to local variable table
            {
                local_variable_table_attribute -> AddLocalVariable(0,
                                                                   code_attribute -> CodeLength(),
                                                                   RegisterUtf8(U8S_this, strlen(U8S_this)),
                                                                   RegisterUtf8(msym -> containing_type -> signature),
                                                                   0);
            }

            //
            // For an ordinary method or constructor...
            //
            for (int i = 0; i < msym -> NumFormalParameters(); i++)
            {
                VariableSymbol *parameter = msym -> FormalParameter(i);
                local_variable_table_attribute -> AddLocalVariable(0,
                                                                   code_attribute -> CodeLength(),
                                                                   RegisterUtf8(parameter -> ExternalIdentity() -> Utf8_literal),
                                                                   RegisterUtf8(parameter -> Type() -> signature),
                                                                   parameter -> LocalVariableIndex());
            }

            if (local_variable_table_attribute -> LocalVariableTableLength() > 0)
                 code_attribute -> AddAttribute(local_variable_table_attribute);
            else delete local_variable_table_attribute; // local_variable_table_attribute not needed, so delete it now
        }

        methods[method_index].AddAttribute(code_attribute);

        FreeMethodInfo();
    }

    return;
}


void ByteCode::InitializeClassVariable(AstVariableDeclarator *vd)
{
    assert(vd -> variable_initializer_opt);

    AstExpression *expression = vd -> variable_initializer_opt -> ExpressionCast();
    if (expression)
    {
        //
        // if already initialized
        //
        if (expression -> IsConstant() && vd -> symbol -> ACC_FINAL())
            return;
        EmitExpression(expression);
    }
    else
    {
        AstArrayInitializer *array_initializer = vd -> variable_initializer_opt -> ArrayInitializerCast();

        assert(array_initializer);

        InitializeArray(vd -> symbol -> Type(), array_initializer);
    }

    PutOp(OP_PUTSTATIC);
    ChangeStack(expression && this_control.IsDoubleWordType(expression -> Type()) ? -2 : -1);
    PutU2(GenerateFieldReference(vd -> symbol));

    return;
}


void ByteCode::InitializeInstanceVariable(AstVariableDeclarator *vd)
{
    assert(vd -> variable_initializer_opt); // field needs initialization

    AstExpression *expression = vd -> variable_initializer_opt -> ExpressionCast();
    if (expression)
    {
        PutOp(OP_ALOAD_0); // load 'this'
        EmitExpression(expression);
    }
    else
    {
        AstArrayInitializer *array_initializer = vd -> variable_initializer_opt -> ArrayInitializerCast();

        assert(array_initializer);

        PutOp(OP_ALOAD_0); // load 'this'
        InitializeArray(vd -> symbol -> Type(), array_initializer);
    }

    PutOp(OP_PUTFIELD);
    ChangeStack(expression && this_control.IsDoubleWordType(expression -> Type()) ? -2 : -1);
    PutU2(GenerateFieldReference(vd -> symbol));

    return;
}


void ByteCode::InitializeArray(TypeSymbol *type, AstArrayInitializer *array_initializer)
{
    TypeSymbol *subtype = type -> ArraySubtype();

    LoadInteger(array_initializer -> NumVariableInitializers());
    EmitNewArray(1, type); // make the array
    for (int i = 0; i < array_initializer -> NumVariableInitializers(); i++)
    {
        Ast *entry = array_initializer -> VariableInitializer(i);
        PutOp(OP_DUP);
        LoadInteger(i);
        AstExpression *expr = entry -> ExpressionCast();
        if (expr)
             EmitExpression(expr);
        else
        {
            assert(entry -> ArrayInitializerCast());

            InitializeArray(subtype, entry -> ArrayInitializerCast());
        }
      
        StoreArrayElement(subtype);
    }

    return;
}     


//
// Generate code for local variable declaration.
//
void ByteCode::DeclareLocalVariable(AstVariableDeclarator *declarator)
{
    if (this_control.option.g)
        declarator -> symbol -> local_program_counter = code_attribute -> CodeLength();

    if (declarator -> symbol -> initial_value)
        (void) LoadLiteral(declarator -> symbol -> initial_value, declarator -> symbol -> Type());
    else if (declarator -> variable_initializer_opt)
    {
        AstArrayCreationExpression *ace = declarator -> variable_initializer_opt -> ArrayCreationExpressionCast();
        if (ace)
            (void) EmitArrayCreationExpression(ace);
        else if (declarator -> variable_initializer_opt -> ArrayInitializerCast())
            InitializeArray(declarator -> symbol -> Type(), declarator -> variable_initializer_opt -> ArrayInitializerCast());
        else // evaluation as expression
            EmitExpression(declarator -> variable_initializer_opt -> ExpressionCast());
    }
    else return; // if nothing to initialize

    StoreLocalVariable(declarator -> symbol);

    return;
}


//
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
//
void ByteCode::EmitStatement(AstStatement *statement) 
{
    if (! statement -> BlockCast())
    {
        line_number_table_attribute -> AddLineNumber(code_attribute -> CodeLength(), 
                                                     this_semantic.lex_stream -> Line(statement -> LeftToken()));
    }

    stack_depth = 0; // stack empty at start of statement

    switch (statement -> kind)
    {
        case Ast::BLOCK: // JLS 13.2
             EmitBlockStatement((AstBlock *) statement, false);
             break;
        case Ast::LOCAL_VARIABLE_DECLARATION: // JLS 13.3
             {
                 AstLocalVariableDeclarationStatement *lvds = statement -> LocalVariableDeclarationStatementCast();
                 for (int i = 0; i < lvds -> NumVariableDeclarators(); i++)
                     DeclareLocalVariable(lvds -> VariableDeclarator(i));
             }
             break;
        case Ast::EMPTY_STATEMENT: // JLS 13.5
             break;
        case Ast::EXPRESSION_STATEMENT: // JLS 13.7
             EmitStatementExpression(statement -> ExpressionStatementCast() -> expression);
             break;
        case Ast::IF: // JLS 13.8
             {
                 AstIfStatement *if_statement = (AstIfStatement *) statement;
                 if (if_statement -> expression -> IsConstant())
                 {
                     IntLiteralValue *if_constant_expr = (IntLiteralValue *) if_statement -> expression -> value;

                     if (if_constant_expr -> value)
                          EmitStatement(if_statement -> true_statement);
                     else if (if_statement -> false_statement_opt) // if there is false part
                          EmitStatement(if_statement -> false_statement_opt);
                 }
                 else if (if_statement -> false_statement_opt) // if true and false parts
                 {
                     Label label1,
                           label2;
                     EmitBranchIfExpression(if_statement -> expression, false, label1);
                     stack_depth = 0;

                     AstStatement *true_statement = if_statement -> true_statement;
                     EmitStatement(true_statement);
                     if (true_statement -> can_complete_normally)
                         EmitBranch(OP_GOTO, label2);

                     DefineLabel(label1);
                     EmitStatement(if_statement -> false_statement_opt);

                     if (true_statement -> can_complete_normally)
                         DefineLabel(label2);

                     CompleteLabel(label1);
                     CompleteLabel(label2);
                 }
                 else // if no false part
                 {
                     Label label1;
                     EmitBranchIfExpression(if_statement -> expression, false, label1);
                     stack_depth = 0;
                     EmitStatement(if_statement -> true_statement);
                     DefineLabel(label1);
                     CompleteLabel(label1);
                 }
             }
             break;
        case Ast::SWITCH: // JLS 13.9
             EmitSwitchStatement(statement -> SwitchStatementCast());
             break;
        case Ast::SWITCH_BLOCK: // JLS 13.9
        case Ast::CASE:
        case Ast::DEFAULT:
            //
            // These nodes are handled by SwitchStatement and
            // are not directly visited
            //
            break;
        case Ast::WHILE: // JLS 13.10
             {
                 AstWhileStatement *wp = statement -> WhileStatementCast();
                 int while_depth = this_block_depth;

                 //
                 // branch to continuation test. This test is placed after the
                 // body of the loop we can fall through into it after each
                 // loop iteration without the need for an additional branch.
                 //
                 EmitBranch(OP_GOTO, continue_labels[while_depth]);
                 DefineLabel(begin_labels[while_depth]);
                 EmitStatement(wp -> statement);
                 DefineLabel(continue_labels[while_depth]);
                 stack_depth = 0;
                 EmitBranchIfExpression(wp -> expression, true, begin_labels[while_depth]);
                 CompleteLabel(begin_labels[while_depth]);
                 CompleteLabel(continue_labels[while_depth]);
             }
             break;
        case Ast::DO: // JLS 13.11
             {
                 AstDoStatement *sp = statement -> DoStatementCast();
                 int do_depth = this_block_depth;
                 DefineLabel(begin_labels[do_depth]);
                 EmitStatement(sp -> statement);
                 DefineLabel(continue_labels[do_depth]);
                 stack_depth = 0;
                 EmitBranchIfExpression(sp -> expression, true, begin_labels[do_depth]);
                 CompleteLabel(begin_labels[do_depth]);
                 CompleteLabel(continue_labels[do_depth]);
             }
             break;
        case Ast::FOR: // JLS 13.12
             {
                 AstForStatement *for_statement = statement -> ForStatementCast();
                 int for_depth = this_block_depth;
                 for (int i = 0; i < for_statement -> NumForInitStatements(); i++)
                     EmitStatement(for_statement -> ForInitStatement(i));
                 EmitBranch(OP_GOTO, test_labels[for_depth]);
                 DefineLabel(begin_labels[for_depth]);
                 EmitStatement(for_statement -> statement);
                 DefineLabel(continue_labels[for_depth]);
                 for (int j = 0; j < for_statement -> NumForUpdateStatements(); j++)
                     EmitStatement(for_statement -> ForUpdateStatement(j));
                 DefineLabel(test_labels[for_depth]);
                 if (for_statement -> end_expression_opt)
                 {
                     stack_depth = 0;
                     EmitBranchIfExpression(for_statement -> end_expression_opt, true, begin_labels[for_depth]);
                 }
                 else EmitBranch(OP_GOTO, begin_labels[for_depth]);

                 CompleteLabel(begin_labels[for_depth]);
                 CompleteLabel(test_labels[for_depth]);
                 CompleteLabel(continue_labels[for_depth]);
             }
             break;
        case Ast::BREAK: // JLS 13.13
             ProcessAbruptExit(statement -> BreakStatementCast() -> nesting_level);
             EmitBranch(OP_GOTO, break_labels[statement -> BreakStatementCast() -> nesting_level]);
             break;
        case Ast::CONTINUE: // JLS 13.14
             ProcessAbruptExit(statement -> ContinueStatementCast() -> nesting_level);
             EmitBranch(OP_GOTO, continue_labels[statement -> ContinueStatementCast() -> nesting_level]);
             break;
        case Ast::RETURN: // JLS 13.15
             EmitReturnStatement(statement -> ReturnStatementCast());
             break;
        case Ast::SUPER_CALL: 
             EmitSuperInvocation((AstSuperCall *) statement);
             break;
        case Ast::THIS_CALL: 
             EmitThisInvocation((AstThisCall *) statement);
             break;
        case Ast::THROW: // JLS 13.16
             EmitExpression(statement -> ThrowStatementCast() -> expression);
             PutOp(OP_ATHROW);
             break;
        case Ast::SYNCHRONIZED_STATEMENT: // JLS 13.17
             EmitSynchronizedStatement(statement -> SynchronizedStatementCast());
             break;
        case Ast::TRY: // JLS 13.18
             EmitTryStatement(statement -> TryStatementCast());
             break;
        case Ast::CLASS: // Class Declaration
        case Ast::INTERFACE: // InterfaceDeclaration
             //
             // these are factored out by the front end; and so must be skipped here
             //
             break;
        case Ast::CATCH:   // JLS 13.18
        case Ast::FINALLY: // JLS 13.18
             // handled by TryStatement
        default:
            assert(false && "unknown statement kind");
            break;
    }

    return;
}


void ByteCode::EmitReturnStatement(AstReturnStatement *statement)
{
    if (! statement -> expression_opt)
    {
        ProcessAbruptExit(0);
        PutOp(OP_RETURN);
        return;
    }

    EmitExpression(statement -> expression_opt);

    //
    // if any outstanding synchronized blocks,
    // find the index of the innermost enclosing block that is
    // synchronized. This block will have the variables allocated
    // for saving synchronization information.
    //
    int var_index = -1;
    if (synchronized_blocks)
    {
        int synch_index = 0;
        for (int i = this_block_depth; i >= 0; i--)
        {
            if (is_synchronized[i])
            {
                synch_index = i;
                break;
            }
        }

        assert(synch_index > 0); // unable to find synchronization block: block #0 is method block. 

        var_index = block_symbols[synch_index] -> synchronized_variable_index + 2;
    }
    else if (finally_blocks)
    {
        int finally_index = 0;
        for (int i = this_block_depth; i >= 0; i--)
        {
            if (has_finally_clause[i] > 0)
            {
                finally_index = i;
                break;
            }
        }

        assert(finally_index > 0); // unable to find finally block: block #0 is method block

        var_index = (has_finally_clause[finally_index] - 1) + 2; // +2 to move to start of area to save value 
    }

    if (var_index >= 0) // if need to save before abrupt exit 
    {
        StoreLocal(var_index, method_type);
        ProcessAbruptExit(0);
        LoadLocal(var_index, method_type);
    }

    if (method_type != this_control.void_type)
        GenerateReturn(method_type);

    return;
}


void ByteCode::EmitBlockStatement(AstBlock *block, bool synchronized)
{
    int save_depth = this_block_depth; // save the depth level upon entry...

    BlockSymbol *block_symbol = block -> block_symbol;
    int nesting_level = block -> nesting_level;

    this_block_depth = nesting_level;
    is_synchronized[nesting_level] = synchronized;
    synchronized_blocks += (synchronized ? 1 : 0);
    block_symbols[nesting_level] = block_symbol;

    stack_depth = 0; // stack empty at start of statement

    if (nesting_level > max_block_depth)
    {
        cout << "nesting_level " << nesting_level << "max " << max_block_depth << "\n";
        assert(false && "loops too deeply nested");
    }

    for (int i = 0; i < block -> NumStatements(); i++)
        EmitStatement((AstStatement *) block -> Statement(i));

    assert(this_block_depth == nesting_level); // block depth out of synch!

    //
    // Always define LABEL_BREAK at this point, and complete definition
    // of other labels
    //
    if (IsLabelUsed(break_labels[nesting_level])) // need define only if used
        DefineLabel(break_labels[nesting_level]);

    CompleteLabel(begin_labels[nesting_level]);
    CompleteLabel(break_labels[nesting_level]);
    CompleteLabel(continue_labels[nesting_level]);
    CompleteLabel(test_labels[nesting_level]);
    CompleteLabel(begin_labels[nesting_level]);

    if (is_synchronized[nesting_level])
        synchronized_blocks--;

    UpdateBlockInfo(block_symbol);

    this_block_depth = save_depth; // restore the original depth level upon entry.

    return;
}


void ByteCode::EmitStatementExpression(AstExpression *expression)
{
    switch (expression -> kind)
    {
        case Ast::PARENTHESIZED_EXPRESSION:
             (void) EmitStatementExpression(expression -> ParenthesizedExpressionCast() -> expression);
             break;
        case Ast::CALL:
             {
                 AstMethodInvocation *method_call = (AstMethodInvocation *) expression;
                 method_call = (method_call -> resolution_opt
                                             ? method_call -> resolution_opt -> MethodInvocationCast()
                                             : method_call);
                 (void) EmitMethodInvocation(method_call);
                 if (method_call -> Type() != this_control.void_type)
                     PutOp(this_control.IsDoubleWordType(method_call -> Type()) ? OP_POP2 : OP_POP); // discard value
            }
            break;
        case Ast::POST_UNARY:
             (void) EmitPostUnaryExpression((AstPostUnaryExpression *) expression, false);
             break;
        case Ast::PRE_UNARY:
             (void) EmitPreUnaryExpression((AstPreUnaryExpression *) expression, false);
             break;
        case Ast::ASSIGNMENT:
             EmitAssignmentExpression((AstAssignmentExpression *) expression, false);
             break;
        case Ast::CLASS_CREATION:
             (void) EmitClassInstanceCreationExpression((AstClassInstanceCreationExpression *) expression, false);
             break;
        default:
             assert(false && "invalid statement expression kind");
    }
}


//
// Generate code for switch statement. Good code generation requires
// detailed knowledge of the target machine. Lacking this, we simply
// choose between LOOKUPSWITCH and TABLESWITCH by picking that
// opcode that takes the least number of bytes in the byte code.
//
//
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
//
void ByteCode::EmitSwitchStatement(AstSwitchStatement *sws)
{
    bool use_lookup = true; // set if using LOOKUPSWITCH opcode

    AstBlock *switch_block = sws -> switch_block;

    int switch_depth = sws -> switch_block -> nesting_level;

    EmitBlockStatement(switch_block, false);

    //
    // Use tableswitch if have exact match or size of tableswitch
    // case is no more than 30 bytes more code than lookup case
    //
    int ncases = sws -> NumCases(),
        nlabels = ncases,
        high = 0,
        low = 0;
    if (ncases > 0)
    {
        low = sws -> Case(0) -> Value();
        high = sws -> Case(ncases - 1) -> Value();

        //
        // want to compute
        //  (2 + high-low + 1) < (1 + ncases * 2 + 30)
        // but must guard against overflow, so factor out
        //  high - low < ncases * 2 + 28
        // but can't have number of labels < number of cases
        //
        if ((high - low) < (ncases * 2 + 28))
        {
            use_lookup = false; // use tableswitch
            nlabels = high - low + 1;

            assert(nlabels >= ncases);
        }
    }

    EmitExpression(sws -> expression);

    stack_depth = 0;
    PutOp(use_lookup ? OP_LOOKUPSWITCH : OP_TABLESWITCH);
    int op_start = last_op_pc; // pc at start of instruction

    //
    // supply any needed padding
    //
    while(code_attribute -> CodeLength() % 4 != 0)
        PutNop(0);

    //
    // Note that if no default clause in switch statement, must allocate
    // one that corresponds to do nothing and branches to start of next
    // statement.
    //
    Label default_label;
    UseLabel(sws -> default_case.switch_block_statement ? default_label : break_labels[switch_depth],
             4,
             code_attribute -> CodeLength() - op_start);

    //
    // 
    //
    Label *case_labels = new Label[(use_lookup ? ncases : nlabels) + 1];
    if (use_lookup)
    {
        PutU4(ncases);

        for (int i = 0; i < ncases; i++)
        {
            PutU4(sws -> Case(i) -> Value());
            UseLabel(case_labels[sws -> Case(i) -> index], 4, code_attribute -> CodeLength() - op_start);
        }
    }
    else
    {
        bool *has_tag = new bool[nlabels + 1];

        for (int i = 0; i < nlabels; i++)
            has_tag[i] = false;

        PutU4(low);
        PutU4(high);

        //
        // mark cases for which no case tag available, i.e., default cases
        //
        for (int j = 0; j < sws -> switch_block -> NumStatements(); j++)
        {
            AstSwitchBlockStatement *sbs = (AstSwitchBlockStatement *) sws -> switch_block -> Statement(j);

            //
            // process labels for this block
            //
            for (int li = 0; li < sbs -> NumSwitchLabels(); li++)
            {
                AstCaseLabel *case_label = sbs -> SwitchLabel(li) -> CaseLabelCast();
                if (case_label)
                {
                    int label_index = sws -> Case(case_label -> map_index) -> Value() - low;
                    has_tag[label_index] = true;
                }
            }
        }

        //
        // Now emit labels in instruction, using appropriate index
        //
        for (int k = 0; k < nlabels; k++)
        {
            UseLabel(has_tag[k] ? case_labels[k]
                                : sws -> default_case.switch_block_statement
                                       ? default_label
                                       : break_labels[switch_depth],
                     4,
                     code_attribute -> CodeLength() - op_start);
        }

        delete [] has_tag;
    }

    //
    // march through switch block statements, compiling blocks in
    // proper order. We must respect order in which blocks seen
    // so that blocks lacking a terminal break fall through to the
    // proper place.
    //
    for (int i = 0; i < sws -> switch_block -> NumStatements(); i++)
    {
        AstSwitchBlockStatement *sbs = (AstSwitchBlockStatement *) sws -> switch_block -> Statement(i);

        //
        // process labels for this block
        //
        for (int li = 0; li < sbs -> NumSwitchLabels(); li++)
        {
            AstCaseLabel *case_label = sbs -> SwitchLabel(li) -> CaseLabelCast();
            if (case_label)
            {
                int map_index = case_label -> map_index;

                if (use_lookup)
                    DefineLabel(case_labels[map_index]);
                else
                {
                    //
                    // TODO: Do this more efficiently ???!!!
                    //
                    for (int di = 0; di < sws -> NumCases(); di++)
                    {
                        if (sws -> Case(di) -> index == map_index)
                        {
                            int ci = sws -> Case(di) -> Value() - low;
                            DefineLabel(case_labels[ci]);
                            break;
                        }
                    }
                }
            }
            else
            {
                assert(sbs -> SwitchLabel(li) -> DefaultLabelCast());
                assert(sws -> default_case.switch_block_statement);

                DefineLabel(default_label);
            }
        }

        //
        // compile code for this case
        //
        for (int si = 0; si < sbs -> NumStatements(); si++)
            EmitStatement(sbs -> Statement(si) -> StatementCast());
    }

    //
    //
    //
    UpdateBlockInfo(switch_block -> block_symbol);

    for (int j = 0; j < nlabels; j++)
    {
        if ((case_labels[j].uses.Length() > 0) && (! case_labels[j].defined))
        {
            case_labels[j].defined = true;
            case_labels[j].definition = (sws -> default_case.switch_block_statement ? default_label.definition
                                                                                    : break_labels[switch_depth].definition);
        }

        CompleteLabel(case_labels[j]);
    }

    if (sws -> default_case.switch_block_statement)
        CompleteLabel(default_label);

    // define target of break label
    if (IsLabelUsed(break_labels[switch_depth])) // need define only if used
        DefineLabel(break_labels[switch_depth]);

    if (sws -> default_case.switch_block_statement)
        CompleteLabel(default_label);

    //
    // define target of break label
    //
    if (IsLabelUsed(break_labels[switch_depth])) // need define only if used
        CompleteLabel(break_labels[switch_depth]);

    delete [] case_labels;

    return;
}


//
//  13.18       The try statement
//
void ByteCode::EmitTryStatement(AstTryStatement *statement)
{
    int final_depth = statement -> block -> nesting_level,
        start_pc = code_attribute -> CodeLength(); // start pc

    //
    // call finally block if have finally handler
    //
    if (statement -> finally_clause_opt)
    { 
        assert(block_symbols[final_depth - 1]);

        BlockSymbol *block_symbol = block_symbols[final_depth - 1] -> BlockCast();
        AstFinallyClause *finally_clause = statement -> finally_clause_opt;
        has_finally_clause[final_depth] = 1 + block_symbol -> try_variable_index;
        finally_blocks++;
    }
    
    EmitStatement(statement -> block);

    //
    // increment max_stack in case exception thrown while stack at greatest depth
    //
    code_attribute -> max_stack++;
    if (statement -> finally_clause_opt) // call finally block if have finally handler
    {
        PutOp(OP_JSR);
        UseLabel(final_labels[final_depth], 2, 1);
    }
    
    Label end_label;
    if (statement -> block -> can_complete_normally)
        EmitBranch(OP_GOTO, end_label);

    PutNop(0); // emit NOP so end_pc will come out right

    int end_pc = last_op_pc;

    //
    // process catch clauses
    //
    for (int i = 0; i < statement -> NumCatchClauses(); i++)
    {
        int handler_pc = code_attribute -> CodeLength();

        AstCatchClause *catch_clause = statement -> CatchClause(i);
        VariableSymbol *parameter_symbol = catch_clause -> parameter_symbol;
        StoreLocalVariable(parameter_symbol);
        EmitStatement(catch_clause -> block);

        
        code_attribute -> AddException(start_pc,
                                       end_pc,
                                       handler_pc,
                                       RegisterClass(parameter_symbol -> Type() -> fully_qualified_name));

        if (statement -> finally_clause_opt) // call finally block if have finally handler
        {
            PutOp(OP_JSR);
            UseLabel(final_labels[final_depth], 2, 1);
        }

        if (catch_clause -> block -> can_complete_normally)
            EmitBranch(OP_GOTO, end_label);
    }

    if (statement -> finally_clause_opt)
    {
        int handler_pc = code_attribute -> CodeLength();
        has_finally_clause[final_depth] = 0; // reset once finally clause processed
        finally_blocks--;

        BlockSymbol *block_symbol = block_symbols[final_depth - 1] -> BlockCast();

        //
        // handler for finally()
        // push thrown value
        //
        StoreLocal(block_symbol -> try_variable_index, this_control.Object());
        PutOp(OP_JSR);
        UseLabel(final_labels[final_depth], 2, 1);
        LoadLocal(block_symbol -> try_variable_index, this_control.Object());
        PutOp(OP_ATHROW); // and rethrow the value to the invoker

        DefineLabel(final_labels[final_depth]);
        CompleteLabel(final_labels[final_depth]);
        StoreLocal(block_symbol -> try_variable_index+1, this_control.Object()); // save return address
        EmitStatement(statement -> finally_clause_opt -> block);
        PutOp(OP_RET);
        PutU1(block_symbol -> try_variable_index + 1); // return using saved address

        code_attribute -> AddException(start_pc, handler_pc, handler_pc, 0);
    }

    if (IsLabelUsed(end_label))
    {
        DefineLabel(end_label);
        CompleteLabel(end_label);
        PutNop(1); // to make sure have code after end of try statement
    }
    else CompleteLabel(end_label);

    return;
}


void ByteCode::UpdateBlockInfo(BlockSymbol *block_symbol)
{
    assert(block_symbol);

    //
    // TODO: Assert that this is always the case
    //
    //    if (code_attribute -> MaxLocals() < block_symbol -> max_variable_index)
    //        code_attribute -> ResetMaxLocals(block_symbol -> max_variable_index);
    assert(code_attribute -> MaxLocals() >= block_symbol -> max_variable_index);

    if (this_control.option.g) // compute local variable table
    {
        for (int i = 0; i < block_symbol -> NumVariableSymbols(); i++)
        {
            VariableSymbol *sym = block_symbol -> VariableSym(i);

            if (last_op_pc > sym -> local_program_counter) // only make entry if defined within range
            {
                local_variable_table_attribute -> AddLocalVariable(sym -> local_program_counter, 
                                                                   last_op_pc - sym -> local_program_counter,
                                                                   RegisterUtf8(sym -> ExternalIdentity() -> Utf8_literal),
                                                                   RegisterUtf8(sym -> Type() -> signature),
                                                                   sym -> LocalVariableIndex());
            }
        }
    }

    return;
}


//
// Exit to block at level lev, freeing monitor locks and invoking finally clauses as appropriate
//
void ByteCode::ProcessAbruptExit(int to_lev)
{
    for (int lev = this_block_depth; lev > to_lev; lev--)
    {
        if (has_finally_clause[lev] > 0)
        {
            PutOp(OP_JSR);
            UseLabel(final_labels[lev], 2, 1);
        }
        else if (is_synchronized[lev])
        {
            PutOp(OP_JSR);
            UseLabel(monitor_labels[lev], 2, 1);
        }
    }

    return;
}


//
// java provides a variety of conditional branch instructions, so
// that a number of operators merit special handling:
//      constant operand
//      negation (we eliminate it)
//      equality
//      && and || (partial evaluation)
//      comparisons
// Other expressions are just evaluated and the appropriate
// branch emitted.
//
void ByteCode::EmitBranchIfExpression(AstExpression *p, bool cond, Label &lab)
{
    if (p -> ParenthesizedExpressionCast())
        p = UnParenthesize(p);

    if (p -> IsConstant())
    {
        if (IsZero(p) != cond)
            EmitBranch(OP_GOTO, lab);
        return;
    }

    AstPreUnaryExpression *pre = p -> PreUnaryExpressionCast();
    if (pre) // must be !, though should probably
    {
        // branch_if(!e,c,l) => branch_if(e,!c,l)
        // test opcode
        // call again with complementary control expression to show
        // effect of negation
        assert(pre -> pre_unary_tag == AstPreUnaryExpression::NOT);

        EmitBranchIfExpression(pre -> expression, (! cond), lab);
        return;
    }

    //
    // dispose of non-binary expression case by just evaluating
    // operand and emitting appropiate test.
    //
    AstBinaryExpression *bp = p -> BinaryExpressionCast();
    if (! bp)
    {
        EmitExpression(p);
        EmitBranch((cond ? OP_IFNE : OP_IFEQ), lab);
        return;
    }

    //
    // Here if binary expression, so extract operands
    //
    AstExpression *left = bp -> left_expression;
    if (left -> ParenthesizedExpressionCast())
        left = UnParenthesize(left);

    AstExpression *right = bp -> right_expression;
    if (right -> ParenthesizedExpressionCast())
        right = UnParenthesize(right);

    TypeSymbol *left_type = left -> Type(),
               *right_type = right -> Type();
    switch (bp -> binary_tag)
    {
        case AstBinaryExpression::INSTANCEOF:
             {
                 EmitExpression(left);
                 PutOp(OP_INSTANCEOF);
                 TypeSymbol *instanceof_type = bp -> right_expression -> Type();
                 PutU2(instanceof_type -> num_dimensions > 0 ? RegisterClass(instanceof_type -> signature)
                                                             : RegisterClass(instanceof_type -> fully_qualified_name));

                 EmitBranch((cond ? OP_IFNE : OP_IFEQ), lab);
             }
             return;
        case AstBinaryExpression::AND_AND:
             //
             // branch_if(a&&b, true, lab) =>
             // branch_if(a,false,skip);
             // branch_if(b,true,lab);
             // skip:
             // branch_if(a&&b, false, lab) =>
             // branch_if(a,false,lab);
             // branch_if(b,false,lab);
             //
             if (cond)
             {
                 Label skip;
                 EmitBranchIfExpression(left, false, skip);
                 EmitBranchIfExpression(right, true, lab);
                 DefineLabel(skip);
                 CompleteLabel(skip);
             }
             else
             {
                 EmitBranchIfExpression(left, false, lab);
                 EmitBranchIfExpression(right, false, lab);
             }
             return;
        case AstBinaryExpression::OR_OR:
             //
             // branch_if(a||b,true,lab) =>
             // branch_if(a,true,lab);
             // branch_if(b,true,lab);
             // branch_if(a||b,false,lab) =>
             // branch_if(a,true,skip);
             // branch_if(b,false,lab);
             // There is additional possibility of one of the operands being
             // constant that should be dealt with at some point.
             //
             if (cond)
             {
                 EmitBranchIfExpression(left, true, lab);
                 EmitBranchIfExpression(right, true, lab);
             }
             else
             {
                 Label skip;  
                 EmitBranchIfExpression(left, true, skip);
                 EmitBranchIfExpression(right, false, lab);
                 DefineLabel(skip);
                 CompleteLabel(skip);
             }
             return;
        case AstBinaryExpression::EQUAL_EQUAL:
        case AstBinaryExpression::NOT_EQUAL:
             //
             // One of the operands is null.
             //
             if (left_type == this_control.null_type || right_type == this_control.null_type)
             {
                 if (left_type == this_control.null_type)  // arrange so right operand is null
                 {
                     AstExpression *temp = left;
                     left = right;
                     right = temp;

                     left_type = left -> Type();
                     right_type = right -> Type();
                 }

                 EmitExpression(left);

                 if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL)
                      EmitBranch(cond ? OP_IFNULL : OP_IFNONNULL, lab);
                 else EmitBranch(cond ? OP_IFNONNULL : OP_IFNULL, lab);

                 return;
             }

             //
             // One of the operands is zero.
             //
             if (IsZero(left) || IsZero(right))
             {
                 if (IsZero(left)) // arrange so right operand is zero
                 {
                     AstExpression *temp = left;
                     left = right;
                     right = temp;

                     left_type = left -> Type();
                     right_type = right -> Type();
                 } 

                 EmitExpression(left);

                 if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL)
                      EmitBranch((cond ? OP_IFEQ : OP_IFNE), lab);
                 else EmitBranch((cond ? OP_IFNE : OP_IFEQ), lab);

                 return;
             }

             //
             // both operands are integer
             //
             if ((this_control.IsSimpleIntegerValueType(left_type)  || left_type == this_control.boolean_type) && 
                 (this_control.IsSimpleIntegerValueType(right_type) || right_type == this_control.boolean_type))
             {
                 EmitExpression(left);
                 EmitExpression(right);

                 if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL)
                      EmitBranch((cond ? OP_IF_ICMPEQ : OP_IF_ICMPNE), lab);
                 else EmitBranch((cond ? OP_IF_ICMPNE : OP_IF_ICMPEQ), lab);

                 return;
             }

             //
             // Both operands are reference types: just do the comparison
             //
             if (IsReferenceType(left_type) && IsReferenceType(right_type))
             {
                 EmitExpression(left);
                 EmitExpression(right);

                 if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL)
                      EmitBranch((cond ? OP_IF_ACMPEQ : OP_IF_ACMPNE), lab);
                 else EmitBranch((cond ? OP_IF_ACMPNE : OP_IF_ACMPEQ), lab);

                 return;
             }

             break;

        default:
             break;
    }

    //
    // here if not comparison, comparison for non-integral numeric types, or
    // integral comparison for which no special casing needed.
    // Begin by dealing with non-comparisons
    //
    switch(bp -> binary_tag)
    {
        case AstBinaryExpression::LESS:
        case AstBinaryExpression::LESS_EQUAL:
        case AstBinaryExpression::GREATER:
        case AstBinaryExpression::GREATER_EQUAL:
        case AstBinaryExpression::EQUAL_EQUAL:
        case AstBinaryExpression::NOT_EQUAL:
             break; // break to continue comparison processing
        default:
             //
             // not a comparison, get the (necessarily boolean) value
             // of the expression and branch on the result
             //
             EmitExpression(p);
             EmitBranch(cond ? OP_IFNE : OP_IFEQ, lab);
             return;
    }

    // 
    // 
    // 
    unsigned opcode = 0,
             op_true,
             op_false;
    if (this_control.IsSimpleIntegerValueType(left_type) || left_type == this_control.boolean_type)
    {
        //
        // we have already dealt with EQUAL_EQUAL and NOT_EQUAL for the case
        // of two integers, but still need to look for comparisons for which
        // one operand may be zero.
        //
        if (IsZero(left))
        {
            EmitExpression(right);
            switch(bp -> binary_tag)
            {
                case AstBinaryExpression::LESS: // if (0 < x) same as  if (x > 0)
                     op_true = OP_IFGT;
                     op_false = OP_IFLE;
                     break;
                case AstBinaryExpression::LESS_EQUAL:  // if (0 <= x) same as if (x >= 0)
                     op_true = OP_IFGE;
                     op_false = OP_IFLT;
                     break;
                case AstBinaryExpression::GREATER:  // if (0 > x) same as if (x < 0)
                     op_true = OP_IFLT;
                     op_false = OP_IFGE;
                     break;
                case AstBinaryExpression::GREATER_EQUAL: // if (0 >= x) same as if (x <= 0)
                     op_true = OP_IFLE;
                     op_false = OP_IFGT;
                     break;
            }
        }
        else if (IsZero(right))
        {
            EmitExpression(left);

            switch(bp -> binary_tag)
            {
                case AstBinaryExpression::LESS:
                     op_true = OP_IFLT;
                     op_false = OP_IFGE;
                     break;
                case AstBinaryExpression::LESS_EQUAL:
                     op_true = OP_IFLE;
                     op_false = OP_IFGT;
                     break;
                case AstBinaryExpression::GREATER:
                     op_true = OP_IFGT;
                     op_false = OP_IFLE;
                     break;
                case AstBinaryExpression::GREATER_EQUAL:
                     op_true = OP_IFGE;
                     op_false = OP_IFLT;
                     break;
            }
        }
        else
        {
            EmitExpression(left);
            EmitExpression(right);

            switch(bp -> binary_tag)
            {
                case AstBinaryExpression::LESS:
                     op_true = OP_IF_ICMPLT;
                     op_false = OP_IF_ICMPGE;
                     break;
                case AstBinaryExpression::LESS_EQUAL:
                     op_true = OP_IF_ICMPLE;
                     op_false = OP_IF_ICMPGT;
                     break;
                case AstBinaryExpression::GREATER:
                     op_true = OP_IF_ICMPGT;
                     op_false = OP_IF_ICMPLE;
                     break;
                case AstBinaryExpression::GREATER_EQUAL:
                     op_true = OP_IF_ICMPGE;
                     op_false = OP_IF_ICMPLT;
                     break;
            }
        }
    }
    else if (left_type == this_control.long_type)
    {
        EmitExpression(left);
        EmitExpression(right);

        opcode = OP_LCMP;

        //
        // branch according to result value on stack
        //
        switch (bp -> binary_tag)
        {
            case AstBinaryExpression::EQUAL_EQUAL:
                 op_true = OP_IFEQ;
                 op_false = OP_IFNE;
                 break;
            case AstBinaryExpression::NOT_EQUAL:
                 op_true = OP_IFNE;
                 op_false = OP_IFEQ;
                 break;
            case AstBinaryExpression::LESS:
                 op_true = OP_IFLT;
                 op_false = OP_IFGE;
                 break;
            case AstBinaryExpression::LESS_EQUAL:
                 op_true = OP_IFLE;
                 op_false = OP_IFGT;
                 break;
            case AstBinaryExpression::GREATER:
                 op_true = OP_IFGT;
                 op_false = OP_IFLE;
                 break;
            case AstBinaryExpression::GREATER_EQUAL:
                 op_true = OP_IFGE;
                 op_false = OP_IFLT;
                 break;
        }
    }
    else if (left_type == this_control.float_type)
    {
        EmitExpression(left);
        EmitExpression(right);
   
        switch (bp -> binary_tag)
        {
            case AstBinaryExpression::EQUAL_EQUAL:
                 opcode = OP_FCMPL;
                 op_true = OP_IFEQ;
                 op_false = OP_IFNE;
                 break;
            case AstBinaryExpression::NOT_EQUAL:
                 opcode = OP_FCMPL;
                 op_true = OP_IFNE;
                 op_false = OP_IFEQ;
                 break;
            case AstBinaryExpression::LESS:
                 opcode = OP_FCMPG;
                 op_true = OP_IFLT;
                 op_false = OP_IFGE;
                 break;
            case AstBinaryExpression::LESS_EQUAL:
                 opcode = OP_FCMPG;
                 op_true = OP_IFLE;
                 op_false = OP_IFGT;
                 break;
            case AstBinaryExpression::GREATER:
                 opcode = OP_FCMPL;
                 op_true = OP_IFGT;
                 op_false = OP_IFLE;
                 break;
            case AstBinaryExpression::GREATER_EQUAL:
                 opcode = OP_FCMPL;
                 op_true = OP_IFGE;
                 op_false = OP_IFLT;
                 break;
        }
    }
    else if (left_type == this_control.double_type)
    {
        EmitExpression(left);
        EmitExpression(right);
        switch (bp -> binary_tag)
        {
            case AstBinaryExpression::EQUAL_EQUAL:
                 opcode = OP_DCMPL;
                 op_true = OP_IFEQ;
                 op_false = OP_IFNE;
                 break;
            case AstBinaryExpression::NOT_EQUAL:
                 opcode = OP_DCMPL;
                 op_true = OP_IFNE;
                 op_false = OP_IFEQ;
                 break;
            case AstBinaryExpression::LESS:
                 opcode = OP_DCMPG;
                 op_true = OP_IFLT;
                 op_false = OP_IFGE;
                 break;
            case AstBinaryExpression::LESS_EQUAL:
                 opcode = OP_DCMPG;
                 op_true = OP_IFLE;
                 op_false = OP_IFGT;
                 break;
            case AstBinaryExpression::GREATER:
                 opcode = OP_DCMPL;
                 op_true = OP_IFGT;
                 op_false = OP_IFLE;
                 break;
            case AstBinaryExpression::GREATER_EQUAL:
                 opcode = OP_DCMPL;
                 op_true = OP_IFGE;
                 op_false = OP_IFLT;
                 break;
        }
    }
    else assert(false && "comparison of unsupported type");

    if (opcode)
        PutOp(opcode); // if need to emit comparison before branch

    EmitBranch (cond ? op_true : op_false, lab);

    return;
}


void ByteCode::EmitSynchronizedStatement(AstSynchronizedStatement *statement)
{
    EmitExpression(statement -> expression);

    int var_index = statement -> block -> block_symbol -> synchronized_variable_index; // variable index to save address of object

    StoreLocal(var_index, this_control.Object()); // save address of object
    LoadLocal(var_index, this_control.Object()); // load address of object onto stack

    PutOp(OP_MONITORENTER); // enter monitor associated with object

    int start_pc = code_attribute -> CodeLength(); // start pc

    EmitBlockStatement(statement -> block, true);
    LoadLocal(var_index, this_control.Object()); // load address of object onto stack
    PutOp(OP_MONITOREXIT);

    if (statement -> block -> NumStatements() > 0)
    {
        int end_pc = last_op_pc;

        Label end_label;
        EmitBranch(OP_GOTO, end_label); // branch around exception handler

        //
        // Reach here if any increment. max_stack in case exception thrown while stack at greatest depth
        //
        code_attribute -> max_stack++;
        int handler_pc = code_attribute -> CodeLength();
        LoadLocal(var_index, this_control.Object()); // load address of object onto stack
        PutOp(OP_MONITOREXIT); 
        PutOp(OP_ATHROW);

        code_attribute -> AddException(start_pc, handler_pc, handler_pc, 0);

        DefineLabel(monitor_labels[statement -> block -> nesting_level]);
        CompleteLabel(monitor_labels[statement -> block -> nesting_level]);

        int loc_index = var_index + 1; // local variable index to save address
        StoreLocal(loc_index, this_control.Object()); // save return address
        LoadLocal(var_index, this_control.Object()); // load address of object onto stack
        PutOp(OP_MONITOREXIT); 
        PutOp(OP_RET);
        PutU1(loc_index);  // return using saved address

        DefineLabel(end_label);
        CompleteLabel(end_label);
    }

    return;
}


//
// JLS is Java Language Specification
// JVM is Java Virtual Machine
//
// Expressions: Chapter 14 of JLS
//
int ByteCode::EmitExpression(AstExpression *expression) 
{
    if (expression -> IsConstant())
        return LoadConstant(expression);

    switch (expression -> kind)
    {
        case Ast::IDENTIFIER:
             return (expression -> SimpleNameCast() && expression -> SimpleNameCast() -> resolution_opt)
                                 ? EmitExpression(expression -> SimpleNameCast() -> resolution_opt)
                                 : LoadSimple(GetLHSKind(expression), expression);
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
             return EmitExpression(((AstParenthesizedExpression *) expression) -> expression);
        case Ast::CLASS_CREATION:
             return EmitClassInstanceCreationExpression((AstClassInstanceCreationExpression *) expression, true);
        case Ast::ARRAY_CREATION:
             return EmitArrayCreationExpression((AstArrayCreationExpression *) expression);
        case Ast::DIM:
             return EmitExpression(expression -> DimExprCast() -> expression);
        case Ast::DOT:
             {
                 AstFieldAccess *field_access = (AstFieldAccess *) expression;
                 return ((field_access -> IsClassAccess()) && (field_access -> resolution_opt))
                                                            ? (unit_type -> outermost_type -> ACC_INTERFACE()
                                                                          ? EmitExpression(field_access -> resolution_opt)
                                                                          : GenerateClassAccess(field_access))
                                                            : EmitFieldAccess(field_access);
             }
        case Ast::CALL:
             {
                 AstMethodInvocation *method_call = expression -> MethodInvocationCast();
                 method_call = (method_call -> resolution_opt
                                             ? method_call -> resolution_opt -> MethodInvocationCast()
                                             : method_call);
                 EmitMethodInvocation(method_call);
                 return GetTypeWords(method_call -> Type());
             }
        case Ast::ARRAY_ACCESS:             // if seen alone this will be as RHS
             return EmitArrayAccessRHS((AstArrayAccess *) expression);
        case Ast::POST_UNARY:
             return EmitPostUnaryExpression((AstPostUnaryExpression *) expression, true);
        case Ast::PRE_UNARY:
             return EmitPreUnaryExpression((AstPreUnaryExpression *) expression, true);
        case Ast::CAST:
             {
                 AstCastExpression *cast_expression = (AstCastExpression *) expression;

                 //
                 // only primitive types require casting
                 //
                 return (cast_expression -> expression -> Type() -> Primitive()
                                          ? EmitCastExpression(cast_expression)
                                          : EmitExpression(cast_expression -> expression));
             }
        case Ast::CHECK_AND_CAST:
             return EmitCastExpression((AstCastExpression *) expression);
        case Ast::BINARY:
             return EmitBinaryExpression((AstBinaryExpression *) expression);
        case Ast::CONDITIONAL:
             return EmitConditionalExpression((AstConditionalExpression *) expression);
        case Ast::ASSIGNMENT:
             return EmitAssignmentExpression((AstAssignmentExpression *) expression, true);
        case Ast::NULL_LITERAL:
             PutOp(OP_ACONST_NULL);
             return 1;
        default:
             assert(false && "unknown expression kind");
             break;
     }

     return 0; // even though we will not reach here
}


void ByteCode::EmitFieldAccessLHSBase(AstExpression *expression)
{
    AstFieldAccess *field = expression -> FieldAccessCast();
    AstSimpleName *simple_name = expression -> SimpleNameCast();

    //
    // If the expression was resolved, get the resolution
    //
    if (field)
    {
        if (field -> resolution_opt)
           expression = field -> resolution_opt;
    }
    else if (simple_name)
    {
        if (simple_name -> resolution_opt)
            expression = simple_name -> resolution_opt;
    }

    //
    // We now have the right expression. Check if it's a field. If so, process base
    // Otherwise, it must be a simple name...
    //
    VariableSymbol *sym = (VariableSymbol *) expression -> symbol;
    field = expression -> FieldAccessCast();
    if (field)
        EmitExpression(field -> base);
    else
    {
        assert(expression -> SimpleNameCast() && "unexpected AssignmentExpressionField operand base type");

        PutOp(OP_ALOAD_0); // get address of "this"
    }

    return;
}


//
// Generate code for access method used to set class literal fields
//
void ByteCode::GenerateClassAccessMethod(MethodSymbol *msym)
{
    //
    // The code takes the form:
    //
    //  aload_0          load this
    //  invokestatic     java/lang/Class.forName(String)java/lang/Class
    //  areturn          return Class object for the class named by string
    //
    //  exception handler if forName fails:
    //
    //  astore_1         save exception
    //  new              java.lang.NoClassDefFoundError
    //  dup              save so can return
    //  aload_1          recover exception
    //  invokevirtual    java.lang.Throwable.getMessage() to get error message
    //  invokenonvirtual <init>     // invoke initializer
    //  athrow           rethrow the exception
    //
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
                         BuildNameAndType(RegisterUtf8(U8S_getMessage, strlen(U8S_getMessage)),
                                          RegisterUtf8(U8S_LP_RP_Ljava_SL_lang_SL_String_SC,
                                                       strlen(U8S_LP_RP_Ljava_SL_lang_SL_String_SC)))));
    ChangeStack(1);

    PutOp(OP_INVOKENONVIRTUAL);
    ChangeStack(-1);
    PutU2(BuildMethodref(RegisterClass(U8S_java_SL_lang_SL_NoClassDefFoundError, strlen(U8S_java_SL_lang_SL_NoClassDefFoundError)),
                         BuildNameAndType(RegisterUtf8(U8S_LT_init_GT, strlen(U8S_LT_init_GT)),
                                          RegisterUtf8(U8S_LP_Ljava_SL_lang_SL_String_SC_RP_V,
                                                       strlen(U8S_LP_Ljava_SL_lang_SL_String_SC_RP_V)))));
    ChangeStack(1);
    PutOp(OP_ATHROW);
    code_attribute -> max_stack = 3;

    code_attribute -> AddException(0,
                                   5,
                                   5,
                                   RegisterClass(U8S_java_SL_lang_SL_ClassNotFoundException,
                                                 strlen(U8S_java_SL_lang_SL_ClassNotFoundException)));

    return;
}


//
// here to generate code to dymanically initialize the field for a class literal and then return its value
//
int ByteCode::GenerateClassAccess(AstFieldAccess *field_access)
{
    //
    // simple case in immediate environment, can use field on both left and right
    // (TypeSymbol *type)
    // evaluate X.class literal. If X is a primitive type, this is a predefined field;
    // otherwise, we must create a new synthetic field to hold the desired result and
    // initialize it at runtime.
    // generate
    // getstatic class_field     load class field
    // ifnull lab1               branch if not yet set
    // get class_field           here if set, return value
    // goto lab2
    // lab1:                     here to initialize the field
    // load class_constant       get name of class
    // invokestatic              invoke generated method to get class_field  desired value
    // dup                       save value so can return it
    // put class_field           initialize the field
    // lab2:
    //
    Label lab1,
          lab2;
    if (field_access -> symbol -> VariableCast())
    {
        VariableSymbol *sym = field_access -> symbol -> VariableCast();

        PutOp(OP_GETSTATIC);
        PutU2(GenerateFieldReference(sym));
        ChangeStack(1);
        EmitBranch(OP_IFNULL, lab1);
        PutOp(OP_GETSTATIC);
        PutU2(GenerateFieldReference(sym));
        ChangeStack(1);
        EmitBranch(OP_GOTO, lab2);
        DefineLabel(lab1);

        //
        // generate load of constant naming the class
        //
        LoadLiteral(field_access -> base -> Type() -> ClassLiteralName(), this_control.String());
        PutOp(OP_INVOKESTATIC);
        CompleteCall(unit_type -> outermost_type -> ClassLiteralMethod(), 1);
        PutOp(OP_DUP);
        PutOp(OP_PUTSTATIC);
        PutU2(GenerateFieldReference(sym));
        ChangeStack(-1);
    }
    else // here in nested case, where must invoke access methods for the field
    {
        VariableSymbol *sym = field_access -> symbol -> VariableCast();
        MethodSymbol *read_symbol = field_access -> symbol -> MethodCast(),
                     *write_symbol = field_access -> resolution_opt -> symbol -> MethodCast();
        AstMethodInvocation *resolve = field_access -> resolution_opt -> MethodInvocationCast();

        //
        // need load this for class with method
        // if the next statement read field_access -> resolution_opt -> symbol = read_method, then
        // generating code for that expression tree would give us what we want
        //
        // TODO: THIS DOES NOT SEEM TO HAVE ANY PURPOSE. BESIDES, IT CHANGES THE INTERMEDIATE REPRESENTATION !!!
        //
        //        field_access -> resolution_opt -> symbol = read_symbol;
        //

        PutOp(OP_INVOKESTATIC);
        u2 read_ref = BuildMethodref(RegisterClass(read_symbol -> containing_type -> fully_qualified_name),
                                     BuildNameAndType(RegisterUtf8(read_symbol -> ExternalIdentity() -> Utf8_literal),
                                                      RegisterUtf8(read_symbol -> signature)));
        PutU2(read_ref);
        ChangeStack(1);

        EmitBranch(OP_IFNULL, lab1);
        PutOp(OP_INVOKESTATIC);
        PutU2(read_ref);
        ChangeStack(1);
        EmitBranch(OP_GOTO, lab2);
        DefineLabel(lab1);

        //
        // generate load of constant naming the class
        //
        LoadLiteral(field_access -> base -> Type() -> ClassLiteralName(), this_control.String());
        PutOp(OP_INVOKESTATIC);
        CompleteCall(unit_type -> outermost_type -> ClassLiteralMethod(), 1);
        PutOp(OP_DUP);
        PutOp(OP_INVOKESTATIC);

        u2 write_ref = BuildMethodref(RegisterClass(write_symbol -> containing_type -> fully_qualified_name),
                                      BuildNameAndType(RegisterUtf8(write_symbol -> ExternalIdentity() -> Utf8_literal),
                                                       RegisterUtf8(write_symbol -> signature)));
        PutU2(write_ref);
        ChangeStack(-1); // to indicate argument popped
    }

    DefineLabel(lab2);
    CompleteLabel(lab1);
    CompleteLabel(lab2);

    return 1; // return one-word (reference) result
}


//
// see also OP_MULTINEWARRAY
//
int ByteCode::EmitArrayCreationExpression(AstArrayCreationExpression *expression) 
{
    if (expression -> array_initializer_opt)
        InitializeArray(expression -> Type(), expression -> array_initializer_opt);
    else
    {
        //
        // need to push value of dimension(s)
        //
        for (int i = 0; i < expression -> NumDimExprs(); i++)
            EmitExpression(expression -> DimExpr(i) -> expression);

        EmitNewArray(expression -> NumDimExprs(), expression -> Type());
    }

    return 1;
}


//
// ASSIGNMENT
//
int ByteCode::EmitAssignmentExpression(AstAssignmentExpression *expression, bool need_value)
{
    AstCastExpression *casted_left_hand_side = expression -> left_hand_side -> CastExpressionCast();
    AstExpression *left_hand_side = (casted_left_hand_side ? casted_left_hand_side -> expression : expression -> left_hand_side);

    TypeSymbol *left_type = left_hand_side -> Type();

    int kind = GetLHSKind(expression);
    VariableSymbol *accessed_member = (expression -> write_method
                                                   ? expression -> write_method -> accessed_member -> VariableCast()
                                                   : (VariableSymbol *) NULL);

    if (expression -> assignment_tag == AstAssignmentExpression::EQUAL)
    {
        switch(kind)
        {
            case LHS_ARRAY:
                 EmitArrayAccessLHS(left_hand_side -> ArrayAccessCast()); // lhs must be array access
                 break;
            case LHS_FIELD:
                 EmitFieldAccessLHSBase(left_hand_side); // load base for field access
                 break;
            case LHS_METHOD:
                 if (! accessed_member -> ACC_STATIC()) // need to load address of object, obtained from resolution
                 {
                     AstExpression *resolve = (left_hand_side -> FieldAccessCast()
                                                               ? left_hand_side -> FieldAccessCast() -> resolution_opt
                                                               : left_hand_side -> SimpleNameCast() -> resolution_opt);

                     assert(resolve);

                     AstFieldAccess *field_expression = resolve -> MethodInvocationCast() -> method -> FieldAccessCast();

                     assert(field_expression);

                     EmitExpression(field_expression -> base);
                 }
                 break;
            default:
                 break;
        }

        EmitExpression(expression -> expression);
    }
    //
    // Here for compound assignment. Get the left operand, saving any information necessary to
    // update its value on the stack below the value.
    //
    else
    {
        switch(kind)
        {
            case LHS_ARRAY:
                 EmitArrayAccessLHS(left_hand_side -> ArrayAccessCast()); // lhs must be array access
                 PutOp(OP_DUP2); // save base and index for later store

                 //
                 // load current value
                 //
                 (void) LoadArrayElement(expression -> Type());
                 break;
            case LHS_FIELD:
                 EmitFieldAccessLHS(left_hand_side);
                 break;
            case LHS_LOCAL:
            case LHS_STATIC:
                 (void) LoadSimple(kind, left_hand_side);
                 //
                 // TODO:
                 // see if actually need call to ChangeStack, marked CHECK_THIS, in AssigmnentExpression
                 //
                 // ChangeStack(this_control.IsDoubleWordType(left_type) ? 1: 0); // CHECK_THIS? Is this really necessary
                 //
                 break;
            case LHS_METHOD:
                 //
                 // If we are accessing a static member, get value by invoking appropriate resolution.
                 // Otherwise, in addition to getting the value, we need to load address of the object,
                 // obtained from the resolution, saving a copy on the stack.
                 //
                 if (accessed_member -> ACC_STATIC()) 
                      EmitExpression(left_hand_side);
                 else ResolveAccess(left_hand_side);
                 break;
            default:
                 break;
        }

        //
        // Here for string concatenation.
        //
        if (expression -> assignment_tag == AstAssignmentExpression::PLUS_EQUAL && left_type == this_control.String())
        {
            EmitStringBuffer();
            PutOp(OP_SWAP); // swap address if buffer and string to update.
            EmitStringAppendMethod(this_control.String());
            AppendString(expression -> expression);
            EmitCallStringToString();
        }
        //
        // Here for operation other than string concatenation. Determine the opcode to use.
        //
        else
        {
            int opc;

            TypeSymbol *op_type = (casted_left_hand_side ? casted_left_hand_side -> Type() : expression -> Type());

            if (this_control.IsSimpleIntegerValueType(op_type) || op_type == this_control.boolean_type)
            {
                switch (expression -> assignment_tag)
                {
                    case AstAssignmentExpression::STAR_EQUAL:
                         opc = OP_IMUL;
                         break;
                    case AstAssignmentExpression::SLASH_EQUAL:
                         opc = OP_IDIV;
                         break;
                    case AstAssignmentExpression::MOD_EQUAL:
                         opc = OP_IREM;
                         break;
                    case AstAssignmentExpression::PLUS_EQUAL:
                         opc = OP_IADD;
                         break;
                    case AstAssignmentExpression::MINUS_EQUAL:
                         opc = OP_ISUB;
                         break;
                    case AstAssignmentExpression::LEFT_SHIFT_EQUAL:
                         opc = OP_ISHL;
                         break;
                    case AstAssignmentExpression::RIGHT_SHIFT_EQUAL:
                         opc = OP_ISHR;
                         break;
                    case AstAssignmentExpression::UNSIGNED_RIGHT_SHIFT_EQUAL:
                         opc = OP_IUSHR;
                         break;
                    case AstAssignmentExpression::AND_EQUAL:
                         opc = OP_IAND;
                         break;
                    case AstAssignmentExpression::IOR_EQUAL:
                         opc = OP_IOR;
                         break;
                    case AstAssignmentExpression::XOR_EQUAL:
                         opc = OP_IXOR;
                         break;
                    default:
                         break;
                }
            }
            else if (op_type == this_control.long_type)
            {
                switch (expression -> assignment_tag)
                {
                    case AstAssignmentExpression::STAR_EQUAL:
                         opc = OP_LMUL;
                         break;
                    case AstAssignmentExpression::SLASH_EQUAL:
                         opc = OP_LDIV;
                         break;
                    case AstAssignmentExpression::MOD_EQUAL:
                         opc = OP_LREM;
                         break;
                    case AstAssignmentExpression::PLUS_EQUAL:
                         opc = OP_LADD;
                         break;
                    case AstAssignmentExpression::MINUS_EQUAL:
                         opc = OP_LSUB;
                         break;
                    case AstAssignmentExpression::LEFT_SHIFT_EQUAL:
                         opc = OP_LSHL;
                         break;
                    case AstAssignmentExpression::RIGHT_SHIFT_EQUAL:
                         opc = OP_LSHR;
                         break;
                    case AstAssignmentExpression::UNSIGNED_RIGHT_SHIFT_EQUAL:
                         opc = OP_LUSHR;
                         break;
                    case AstAssignmentExpression::AND_EQUAL:
                         opc = OP_LAND;
                         break;
                    case AstAssignmentExpression::IOR_EQUAL:
                         opc = OP_LOR;
                         break;
                    case AstAssignmentExpression::XOR_EQUAL:
                         opc = OP_LXOR;
                         break;
                    default:
                         break;
                }
            }
            else if (op_type == this_control.float_type)
            {
                switch (expression -> assignment_tag)
                {
                    case AstAssignmentExpression::STAR_EQUAL:
                         opc = OP_FMUL;
                         break;
                    case AstAssignmentExpression::SLASH_EQUAL:
                         opc = OP_FDIV;
                         break;
                    case AstAssignmentExpression::MOD_EQUAL:
                         opc = OP_FREM;
                         break;
                    case AstAssignmentExpression::PLUS_EQUAL:
                         opc = OP_FADD;
                         break;
                    case AstAssignmentExpression::MINUS_EQUAL:
                         opc = OP_FSUB;
                         break;
                    default:
                         break;
                }
            }
            else if (op_type == this_control.double_type)
            {
                switch (expression -> assignment_tag)
                {
                    case AstAssignmentExpression::STAR_EQUAL:
                         opc = OP_DMUL;
                         break;
                    case AstAssignmentExpression::SLASH_EQUAL:
                         opc = OP_DDIV;
                         break;
                    case AstAssignmentExpression::MOD_EQUAL:
                         opc = OP_DREM;
                         break;
                    case AstAssignmentExpression::PLUS_EQUAL:
                         opc = OP_DADD;
                         break;
                    case AstAssignmentExpression::MINUS_EQUAL:
                         opc = OP_DSUB;
                         break;
                    default:
                         break;
                }
            }

            //
            // convert value to desired type if necessary
            //
            if (casted_left_hand_side)
                EmitCast(casted_left_hand_side -> Type(), left_type);

            EmitExpression(expression -> expression);

            PutOp(opc);  

            if (casted_left_hand_side) // now cast result back to type of result
                EmitCast(left_type, casted_left_hand_side -> Type());
        }
    }

    //
    // Update left operand, saving value of right operand if it is needed.
    //
    switch(kind)
    {
        case LHS_ARRAY:
             if (need_value)
                 PutOp(this_control.IsDoubleWordType(left_type) ? OP_DUP2_X2 : OP_DUP_X2);
             StoreArrayElement(expression -> Type());
             break;
        case LHS_FIELD:
             if (need_value)
                 PutOp(this_control.IsDoubleWordType(left_type) ? OP_DUP2_X1 : OP_DUP_X1);
             StoreField(left_hand_side);
             break;
        case LHS_METHOD:
             {
                 if (need_value)
                 {
                     if (accessed_member -> ACC_STATIC())
                          PutOp(this_control.IsDoubleWordType(left_type) ? OP_DUP2 : OP_DUP);
                     else PutOp(this_control.IsDoubleWordType(left_type) ? OP_DUP2_X1 : OP_DUP_X1);
                 }

                 int stack_words = (this_control.IsDoubleWordType(left_type) ? 2 : 1) + (accessed_member -> ACC_STATIC() ? 0 : 1);
                 PutOp(OP_INVOKESTATIC);
                 CompleteCall(expression -> write_method, stack_words);
             }
             break;
        case LHS_LOCAL:
        case LHS_STATIC:
            if (need_value)
                PutOp(this_control.IsDoubleWordType(left_type) ? OP_DUP2 : OP_DUP);
            StoreSimple(kind, left_hand_side);
            break;
        default:
            break;
    }

    return GetTypeWords(expression -> Type());
}


//
// BINARY: Similar code patterns are used for the ordered comparisons
//
int ByteCode::EmitBinaryExpression(AstBinaryExpression *expression) 
{
    switch (expression -> binary_tag) // process boolean-results first
    {
        case AstBinaryExpression::OR_OR:
        case AstBinaryExpression::AND_AND:
        case AstBinaryExpression::LESS:
        case AstBinaryExpression::LESS_EQUAL:
        case AstBinaryExpression::GREATER:
        case AstBinaryExpression::GREATER_EQUAL:
        case AstBinaryExpression::EQUAL_EQUAL:
        case AstBinaryExpression::NOT_EQUAL:
             {
                 Label lab1,
                       lab2;
                 EmitBranchIfExpression(expression, true, lab1);
                 PutOp(OP_ICONST_0);                // push false
                 EmitBranch(OP_GOTO, lab2);
                 DefineLabel(lab1);
                 PutOp(OP_ICONST_1);                // push false
                 DefineLabel(lab2);
                 CompleteLabel(lab1);
                 CompleteLabel(lab2);
             }
             return 1;
        default:
             break;
    }

    if (expression -> binary_tag == AstBinaryExpression::INSTANCEOF)
    {
        TypeSymbol *instanceof_type = expression -> right_expression -> Type();
        EmitExpression(expression -> left_expression);
        PutOp(OP_INSTANCEOF);
        PutU2(instanceof_type -> num_dimensions > 0 ? RegisterClass(instanceof_type -> signature)
                                                    : RegisterClass(instanceof_type -> fully_qualified_name));
        return 1;
    }

    //
    // special case string concatenation
    //
    if (expression -> binary_tag == AstBinaryExpression::PLUS &&
        (IsReferenceType(expression -> left_expression -> Type()) || IsReferenceType(expression -> right_expression -> Type())))
    {
        ConcatenateString(expression);
        return 1;
    }

    //
    // Try to simplify if one operand known to be zero.
    //
    if (IsZero(expression -> left_expression))
    {
        TypeSymbol *right_type = expression -> right_expression -> Type();
        switch (expression -> binary_tag)
        {
            case AstBinaryExpression::PLUS:
            case AstBinaryExpression::IOR:
            case AstBinaryExpression::XOR:
                 EmitExpression(expression -> right_expression);
                 return GetTypeWords(expression -> Type());
            case AstBinaryExpression::STAR:
            case AstBinaryExpression::AND:
            case AstBinaryExpression::LEFT_SHIFT:
            case AstBinaryExpression::RIGHT_SHIFT:
            case AstBinaryExpression::UNSIGNED_RIGHT_SHIFT:
                 if (this_control.IsSimpleIntegerValueType(right_type) || right_type == this_control.boolean_type)
                     LoadShort(0);
                 else
                 {
                     assert((right_type == this_control.long_type ||
                             right_type == this_control.float_type ||
                             right_type == this_control.double_type) && "unexpected type in expression simplification");

                     PutOp(right_type == this_control.long_type
                                       ? OP_LCONST_0
                                       : right_type == this_control.float_type
                                                     ? OP_FCONST_0
                                                     : OP_DCONST_0); // double_type
                 }
                 return GetTypeWords(right_type);
            case AstBinaryExpression::MINUS: // 0 - x is negation of x
                 EmitExpression(expression -> right_expression);

                 assert((this_control.IsSimpleIntegerValueType(right_type) ||
                         right_type == this_control.long_type ||
                         right_type == this_control.float_type ||
                         right_type == this_control.double_type) && "unexpected type in expression simplification");

                 PutOp(this_control.IsSimpleIntegerValueType(right_type)
                           ? OP_INEG
                           : right_type == this_control.long_type
                                         ? OP_LNEG
                                         : right_type == this_control.float_type
                                                       ? OP_FNEG
                                                       : OP_DNEG); // double_type
                 return GetTypeWords(expression -> Type());
            default:
                 break;
        }
    }

    if (IsZero(expression -> right_expression))
    {
        TypeSymbol *left_type = expression -> left_expression -> Type();
        switch (expression -> binary_tag)
        {
            case AstBinaryExpression::PLUS:
            case AstBinaryExpression::MINUS:
            case AstBinaryExpression::IOR:
            case AstBinaryExpression::XOR:
            case AstBinaryExpression::LEFT_SHIFT:
            case AstBinaryExpression::RIGHT_SHIFT:
            case AstBinaryExpression::UNSIGNED_RIGHT_SHIFT: // here for cases that simplify to the left operand
                 EmitExpression(expression -> left_expression);
                 return GetTypeWords(expression -> Type());
            case AstBinaryExpression::STAR:
            case AstBinaryExpression::AND: // here for cases that evaluate to zero
                 if (this_control.IsSimpleIntegerValueType(left_type) || left_type == this_control.boolean_type)
                      LoadShort(0);
                 else
                 {
                     assert((left_type == this_control.long_type ||
                             left_type == this_control.float_type ||
                             left_type == this_control.double_type) && "unexpected type in expression simplification");

                     PutOp(left_type == this_control.long_type
                                      ? OP_LCONST_0
                                      : left_type == this_control.float_type
                                                   ? OP_FCONST_0
                                                   : OP_DCONST_0); // double_type
                 }
                 return GetTypeWords(expression -> Type());
            default:
                 break;
        }
    }

    EmitExpression(expression -> left_expression);
    EmitExpression(expression -> right_expression);

    TypeSymbol *type = expression -> left_expression -> Type();
    bool integer_type = (this_control.IsSimpleIntegerValueType(type) || type == this_control.boolean_type);
    switch (expression -> binary_tag)
    {
        case AstBinaryExpression::STAR:
             PutOp(integer_type ? OP_IMUL
                                : type == this_control.long_type
                                        ? OP_LMUL
                                        : type == this_control.float_type
                                                ? OP_FMUL
                                                : OP_DMUL); // double_type
             break;
        case AstBinaryExpression::SLASH:
             PutOp(integer_type ? OP_IDIV
                                : type == this_control.long_type
                                        ? OP_LDIV
                                        : type == this_control.float_type
                                                ? OP_FDIV
                                                : OP_DDIV); // double_type
             break;
        case AstBinaryExpression::MOD:
             PutOp(integer_type ? OP_IREM
                                : type == this_control.long_type
                                        ? OP_LREM
                                        : type == this_control.float_type
                                                ? OP_FREM
                                                : OP_DREM); // double_type
             break;
        case AstBinaryExpression::PLUS:
             PutOp(integer_type ? OP_IADD
                                : type == this_control.long_type
                                        ? OP_LADD
                                        : type == this_control.float_type
                                                ? OP_FADD
                                                : OP_DADD); // double_type
             break;
        case AstBinaryExpression::MINUS:
             PutOp(integer_type ? OP_ISUB
                                : type == this_control.long_type
                                        ? OP_LSUB
                                        : type == this_control.float_type
                                                ? OP_FSUB
                                                : OP_DSUB); // double_type
             break;
        case AstBinaryExpression::LEFT_SHIFT:
             PutOp(integer_type ? OP_ISHL : OP_LSHL);
             break;
        case AstBinaryExpression::RIGHT_SHIFT:
             PutOp(integer_type ? OP_ISHR : OP_LSHR);
             break;
        case AstBinaryExpression::UNSIGNED_RIGHT_SHIFT:
             PutOp(integer_type ? OP_IUSHR : OP_LUSHR);
             break;
        case AstBinaryExpression::AND:
             PutOp(integer_type ? OP_IAND : OP_LAND);
             break;
        case AstBinaryExpression::XOR:
             PutOp(integer_type ? OP_IXOR : OP_LXOR);
             break;
        case AstBinaryExpression::IOR:
             PutOp(integer_type ? OP_IOR : OP_LOR);
             break;
        default:
             assert(false && "binary unknown tag");
    }

    return GetTypeWords(expression -> Type());
}


int ByteCode::EmitCastExpression(AstCastExpression *expression)
{
    //
    // convert from numeric type src to destination type dest
    //
    EmitExpression(expression -> expression); 

    TypeSymbol *dest_type = expression -> Type(),
               *source_type = expression -> expression -> Type();
    EmitCast(dest_type, source_type);

    return GetTypeWords(dest_type);
}


void ByteCode::EmitCast(TypeSymbol *dest_type, TypeSymbol *source_type)
{
    if (dest_type == source_type) // done if nothing to do
        return;

    if (this_control.IsSimpleIntegerValueType(source_type))
    {
        if (dest_type != this_control.int_type) // no conversion needed
        {
            assert((dest_type == this_control.long_type ||
                    dest_type == this_control.float_type ||
                    dest_type == this_control.double_type ||
                    dest_type == this_control.char_type ||
                    dest_type == this_control.byte_type ||
                    dest_type == this_control.short_type) && "unsupported conversion");

            PutOp(dest_type == this_control.long_type
                             ? OP_I2L
                             : dest_type == this_control.float_type
                                          ? OP_I2F
                                          : dest_type == this_control.double_type
                                                       ? OP_I2D
                                                       : dest_type == this_control.char_type
                                                                    ? OP_I2C
                                                                    : dest_type == this_control.byte_type
                                                                                 ? OP_I2B
                                                                                 : OP_I2S); // short_type
        }
    }
    else if (source_type == this_control.long_type)
    {
        if (this_control.IsSimpleIntegerValueType(dest_type))
        {
            PutOp(OP_L2I);

            if (dest_type != this_control.int_type)
            {
                PutOp(dest_type == this_control.char_type
                                 ? OP_I2C
                                 : dest_type == this_control.byte_type
                                              ? OP_I2B
                                              : OP_I2S); // short_type
            }
        }
        else
        {
            assert((dest_type == this_control.float_type ||
                    dest_type == this_control.double_type) && "unsupported conversion");

            PutOp(dest_type == this_control.float_type
                             ? OP_L2F
                             : OP_L2D); // double_type
        }
    }
    else if (source_type == this_control.float_type)
    {
        if (this_control.IsSimpleIntegerValueType(dest_type))
        {
            PutOp(OP_F2I);

            if (dest_type != this_control.int_type)
            {
                PutOp(dest_type == this_control.char_type
                                 ? OP_I2C
                                 : dest_type == this_control.byte_type
                                              ? OP_I2B
                                              : OP_I2S); // short_type
            }
        }
        else
        {
            assert((dest_type == this_control.long_type ||
                    dest_type == this_control.double_type) && "unsupported conversion");

            PutOp(dest_type == this_control.long_type
                             ? OP_F2L
                             : OP_F2D);
        }
    }
    else if (source_type == this_control.double_type)
    {
        if (this_control.IsSimpleIntegerValueType(dest_type))
        {
            PutOp(OP_D2I);

            if (dest_type != this_control.int_type)
            {
                PutOp(dest_type == this_control.char_type
                                 ? OP_I2C
                                 : dest_type == this_control.byte_type
                                              ? OP_I2B
                                              : OP_I2S); // short_type
            }
        }
        else
        {
            assert((dest_type == this_control.long_type ||
                    dest_type == this_control.float_type) && "unsupported conversion");

            PutOp(dest_type == this_control.long_type
                             ? OP_D2L
                             : OP_D2F); // float_type
        }
    }
    else if (source_type == this_control.null_type)
         ; // Nothing to do
    else
    {
        //
        // Generate check cast instruction.
        //
        PutOp(OP_CHECKCAST);
        PutU2(dest_type -> num_dimensions > 0 ? RegisterClass(dest_type -> signature)
                                              : RegisterClass(dest_type -> fully_qualified_name));
    }

    return;
}
    

int ByteCode::EmitClassInstanceCreationExpression(AstClassInstanceCreationExpression *expression, bool need_value) 
{
    MethodSymbol *constructor = (MethodSymbol *) expression -> class_type -> symbol;

    PutOp(OP_NEW);
    PutU2(RegisterClass(expression -> Type() -> fully_qualified_name));
    if (need_value) // save address of new object for constructor
        PutOp(OP_DUP);

    //
    // call constructor
    // pass address of object explicitly passed to new if specified.
    //
    int stack_words = 0;
    if (expression -> base_opt)
    {
        stack_words += EmitExpression(expression -> base_opt);
        PutOp(OP_DUP);

        Label lab1;
        EmitBranch(OP_IFNONNULL, lab1);
        PutOp(OP_ACONST_NULL); // need to test for null, raising NullPointerException if so. So just do athrow
        PutOp(OP_ATHROW); 
        DefineLabel(lab1);
        CompleteLabel(lab1);
    }

    //
    // Pass all local arguments, if any
    //
    for (int i = 0; i < expression -> NumLocalArguments(); i++)
        stack_words += EmitExpression((AstExpression *) expression -> LocalArgument(i));

    //
    // If we are calling a private constructor, pass the extra null argument to the access constructor.
    //
    if (expression -> NeedsExtraNullArgument())
    {
        PutOp(OP_ACONST_NULL);
        stack_words += 1;
    }

    //
    // Now, process the real arguments specified in the source.
    //
    for (int k = 0; k < expression -> NumArguments(); k++)
        stack_words += EmitExpression((AstExpression *) expression -> Argument(k));

    PutOp(OP_INVOKENONVIRTUAL);
    ChangeStack(-stack_words); 
    if (constructor -> constant_pool_index == 0 || constructor -> constant_pool_class != class_id) // build method ref for method
    {
        constructor -> constant_pool_index =
                       BuildMethodref(RegisterClass(expression -> Type() -> fully_qualified_name),
                                      BuildNameAndType(RegisterUtf8(this_control.init_name_symbol -> Utf8_literal),
                                                       RegisterUtf8(constructor -> signature)));
        constructor -> constant_pool_class = class_id;
    }
    
    PutU2(constructor -> constant_pool_index);

    return 1;
}


int ByteCode::EmitConditionalExpression(AstConditionalExpression *expression)
{
    Label lab1,
          lab2;
    EmitBranchIfExpression(expression -> test_expression, false, lab1);
    EmitExpression(expression -> true_expression);
    EmitBranch(OP_GOTO, lab2);
    DefineLabel(lab1);
    EmitExpression(expression -> false_expression);
    DefineLabel(lab2);
    CompleteLabel(lab1);
    CompleteLabel(lab2);

    return GetTypeWords(expression -> true_expression -> Type());
}


int ByteCode::EmitFieldAccess(AstFieldAccess *expression) 
{
    AstExpression *base = expression -> base;
    VariableSymbol *sym = expression -> symbol -> VariableCast();
    if (expression -> IsConstant())
    {
        if (sym -> ACC_STATIC())
        {
            if (! (base -> symbol -> TypeCast() || base -> symbol -> VariableCast()))
            {
                EmitExpression(base);
                PutOp(OP_POP);
            }
        }

        return LoadConstant(expression);
    }

    if (expression -> resolution_opt) // resolve reference to private field in parent
        return EmitExpression(expression -> resolution_opt);

    if (base -> Type() -> IsArray() && sym -> ExternalIdentity() == this_control.length_name_symbol)
    {
        EmitExpression(base);
        PutOp(OP_ARRAYLENGTH);

        return 1;
    }

    TypeSymbol *expression_type = expression -> Type();
    if (sym -> ACC_STATIC())
    {
        if (! (base -> symbol -> TypeCast() || base -> symbol -> VariableCast()))
        {
          EmitExpression(base);
          PutOp(OP_POP);
        }
        PutOp(OP_GETSTATIC);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? 2 : 1);
    }
    else
    {
        EmitExpression(base); // get base
        PutOp(OP_GETFIELD);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? 1 : 0);
    }

    PutU2(GenerateFieldReference(sym));

    return GetTypeWords(expression_type);
}


//
// generate code corresponding to
//  try {
//      evaluate super.clone();
//  } catch (CloneNotSupportedException e) {
//      throw new InternalError(e.getMessage());
//  }
//
void ByteCode::EmitCloneArray(AstMethodInvocation *expression)
{
    int start_pc = code_attribute -> CodeLength(); 

    assert(expression -> method -> FieldAccessCast());
    EmitExpression(expression -> method -> FieldAccessCast() -> base);

    PutOp(OP_INVOKEVIRTUAL);

    PutU2(RegisterMethodClone());
    Label end_label;
    EmitBranch(OP_GOTO, end_label);
    int end_pc = code_attribute -> CodeLength(); 

    //
    // indicate that we use at least 2 local variables (this and the exception)
    //
    if (code_attribute -> MaxLocals() < 2)
        code_attribute -> ResetMaxLocals(2); // CSA fix 08-dec-1998 for PR 294

    //
    // start handler
    // can't count next StoreLocal as pop since in handler
    //
    ChangeStack(1); 
    StoreLocal(1, this_control.Object());
    PutOp(OP_NEW); 
    PutU2(RegisterClass(U8S_java_SL_lang_SL_InternalError, strlen(U8S_java_SL_lang_SL_InternalError)));
    PutOp(OP_DUP);
    LoadLocal(1, this_control.Object());
    PutOp(OP_INVOKEVIRTUAL);
    PutU2(RegisterMethodCloneGetmessage());
    PutOp(OP_INVOKENONVIRTUAL);
    PutU2(RegisterMethodCloneInit());
    PutOp(OP_ATHROW);
    DefineLabel(end_label);
    CompleteLabel(end_label);

    //
    // "java/lang/CloneNotSupportedException"
    //
    code_attribute -> AddException(start_pc,
                                   end_pc,
                                   end_pc,
                                   RegisterClass(U8S_java_SL_lang_SL_CloneNotSupportedException,
                                                 strlen(U8S_java_SL_lang_SL_CloneNotSupportedException)));

    return;
}


void ByteCode::EmitMethodInvocation(AstMethodInvocation *expression) 
{
    MethodSymbol *msym = (MethodSymbol *) expression -> symbol;

    if (msym -> ExternalIdentity() == this_control.clone_name_symbol && msym -> containing_type -> IsArray())
    {
        EmitCloneArray(expression);
        return;
    }

    bool is_super = false; // set if super call

    if (msym -> ACC_STATIC())
    {
        if (expression -> method -> FieldAccessCast())
        {
            AstFieldAccess *field = expression -> method -> FieldAccessCast();
            if (field -> base -> MethodInvocationCast())
            {
                EmitMethodInvocation(field -> base -> MethodInvocationCast());
                PutOp(OP_POP); // discard value (only evaluating for side effect)
            }
        }
    }
    else
    {
        AstFieldAccess *field = expression -> method -> FieldAccessCast();
        if (field)
        {
            AstFieldAccess *sub_field_access = field -> base -> FieldAccessCast();
            is_super = field -> base -> SuperExpressionCast() || (sub_field_access && sub_field_access -> IsSuperAccess());

            if (field -> base -> MethodInvocationCast())
                 EmitMethodInvocation(field -> base -> MethodInvocationCast());
            else EmitExpression(field -> base);
        }
        else if (expression -> method -> SimpleNameCast())
        {
            AstSimpleName *simple_name = expression -> method -> SimpleNameCast();
            if (simple_name -> resolution_opt) // use resolution if available
                EmitExpression(simple_name -> resolution_opt);
            else // must be field of current object, so load this
                PutOp(OP_ALOAD_0);
        }
        else assert(false && "unexpected argument to field access");
    }

    int stack_words = 0; // words on stack needed for arguments
    for (int i = 0; i < expression -> NumArguments(); i++)
        stack_words += EmitExpression((AstExpression *) expression -> Argument(i));

    PutOp(msym -> ACC_STATIC()
                ? OP_INVOKESTATIC 
                : (is_super || msym -> ACC_PRIVATE())
                             ? OP_INVOKENONVIRTUAL
                             : msym -> containing_type -> ACC_INTERFACE() ? OP_INVOKEINTERFACE 
                                                                          : OP_INVOKEVIRTUAL);
    CompleteCall(msym, stack_words);

    return;
}


void ByteCode::CompleteCall(MethodSymbol *msym, int stack_words)
{
    ChangeStack(-stack_words);

    //
    // need to get method index, the constant_pool index for a
    // reference to this method (a Methodref);
    //
    if (msym -> constant_pool_index == 0 || msym -> constant_pool_class != class_id) // build method ref for method
    {
        msym -> constant_pool_index =
                (msym -> containing_type -> ACC_INTERFACE()
                       ? BuildInterfaceMethodref(RegisterClass(msym -> containing_type -> fully_qualified_name),
                                                 BuildNameAndType(RegisterUtf8(msym -> ExternalIdentity() -> Utf8_literal),
                                                                  RegisterUtf8(msym -> signature)))
                       : BuildMethodref(RegisterClass(msym -> containing_type -> fully_qualified_name),
                                        BuildNameAndType(RegisterUtf8(msym -> ExternalIdentity() -> Utf8_literal),
                                                         RegisterUtf8(msym -> signature))));
        msym -> constant_pool_class = class_id;
    }
    
    PutU2(msym -> constant_pool_index);
    if (msym -> containing_type -> ACC_INTERFACE())
    {
        PutU1(stack_words + 1);
        PutU1(0);
    }

    //
    // must account for value returned by method.
    //
    ChangeStack(this_control.IsDoubleWordType(msym -> Type()) ? 2 : msym -> Type() == this_control.void_type ? 0 : 1);

    return;
}   


void ByteCode::EmitNewArray(int num_dims, TypeSymbol *type)
{
    if (num_dims == 0 || (num_dims == 1 && type -> num_dimensions == num_dims))
    {
        TypeSymbol *element_type = type -> ArraySubtype();

        if (this_control.IsNumeric(element_type) || element_type == this_control.boolean_type) // one-dimensional primitive?
        {
            PutOp(OP_NEWARRAY);
            PutU1(element_type == this_control.boolean_type
                         ? 4
                         : element_type == this_control.char_type
                                  ? 5
                                  : element_type == this_control.float_type
                                           ? 6
                                           : element_type == this_control.double_type
                                                    ? 7
                                                    : element_type == this_control.byte_type
                                                             ? 8
                                                             : element_type == this_control.short_type
                                                                      ? 9
                                                                      : element_type == this_control.int_type
                                                                               ? 10
                                                                               : 11); // control.long_type
        }
        else // must be reference type
        {
            PutOp(OP_ANEWARRAY);
            PutU2(RegisterClass(element_type -> fully_qualified_name));
        }
    }
    else
    {
        PutOp(OP_MULTIANEWARRAY);
        PutU2(RegisterClass(type -> signature));
        PutU1(num_dims); // load dims count
        ChangeStack(num_dims - 1);
    }

    return;
}


//
// POST_UNARY
//
int ByteCode::EmitPostUnaryExpression(AstPostUnaryExpression *expression, bool need_value) 
{
    int kind = GetLHSKind(expression);

    switch(kind)
    {
        case LHS_LOCAL:
        case LHS_STATIC:
             EmitPostUnaryExpressionSimple(kind, expression, need_value);
             break;
        case LHS_ARRAY:
             EmitPostUnaryExpressionArray(expression, need_value);
             break;
        case LHS_FIELD:
             EmitPostUnaryExpressionField(kind, expression, need_value);
             break;
        case LHS_METHOD:
             {
                 VariableSymbol *accessed_member = expression -> write_method -> accessed_member -> VariableCast();
                 if (accessed_member -> ACC_STATIC())
                      EmitPostUnaryExpressionSimple(kind, expression, need_value);
                 else EmitPostUnaryExpressionField(kind, expression, need_value);
             }
             break;
        default:
             assert(false && "unknown lhs kind for assignment");
    }

    return GetTypeWords(expression -> Type());
}


//
// AstExpression *expression;
// POST_UNARY on instance variable
// load value of field, duplicate, do increment or decrement, then store back, leaving original value
// on top of stack.
//
void ByteCode::EmitPostUnaryExpressionField(int kind, AstPostUnaryExpression *expression, bool need_value) 
{
    if (kind == LHS_METHOD)
         ResolveAccess(expression -> expression); // get address and value
    else EmitFieldAccessLHS(expression -> expression);

    TypeSymbol *expression_type = expression -> Type();
    if (need_value)
        PutOp(this_control.IsDoubleWordType(expression_type) ? OP_DUP2_X1 : OP_DUP_X1);

    if (this_control.IsSimpleIntegerValueType(expression_type)) // TBSL: use iinc eventually
    {
        PutOp(OP_ICONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
    }
    else if (expression_type == this_control.long_type)
    {
        PutOp(OP_LCONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_LADD : OP_LSUB);
    }
    else if (expression_type == this_control.float_type)
    {
        PutOp(OP_FCONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_FADD : OP_FSUB);
    }
    else if (expression_type == this_control.double_type)
    {
        PutOp(OP_DCONST_1); // load 1.0
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_DADD : OP_DSUB);
    }

    if (kind == LHS_METHOD)
    {
        int stack_words = (this_control.IsDoubleWordType(expression_type) ? 2 : 1) + 1;
        PutOp(OP_INVOKESTATIC);
        CompleteCall(expression -> write_method, stack_words);
    }
    else // assert(kind == LHS_FIELD)
    {
        PutOp(OP_PUTFIELD);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? -3 : -2);
        PutU2(GenerateFieldReference((VariableSymbol *) expression -> symbol));
    }

    return;
}


//
// AstExpression *expression;
// POST_UNARY on local variable
// load value of variable, duplicate, do increment or decrement, then store back, leaving original value
// on top of stack.
//
void ByteCode::EmitPostUnaryExpressionSimple(int kind, AstPostUnaryExpression *expression, bool need_value) 
{
    TypeSymbol *expression_type = expression -> Type();
    if (expression_type == this_control.int_type) // see if can use IINC
    {
        if (IsLocal(expression))
        {
            if (need_value)
                (void) LoadSimple(kind, expression);
            PutOp(OP_IINC);
            PutU1(expression -> symbol -> VariableCast() -> LocalVariableIndex());
            PutI1(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? 1 : -1);

            return;
        }
    }

    (void) LoadSimple(kind, expression -> expression); // this will also load value needing resolution

    if (need_value)
        PutOp(this_control.IsDoubleWordType(expression_type) ? OP_DUP2 : OP_DUP);

    if (this_control.IsSimpleIntegerValueType(expression_type)) // TBSL: use iinc eventually
    {
        PutOp(OP_ICONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
        EmitCast(expression_type, this_control.int_type);
    }
    else if (expression_type == this_control.long_type)
    {
        PutOp(OP_LCONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_LADD : OP_LSUB);
    }
    else if (expression_type == this_control.float_type)
    {
        PutOp(OP_FCONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_FADD : OP_FSUB);
    }
    else if (expression_type == this_control.double_type)
    {
        PutOp(OP_DCONST_1); // load 1.0
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_DADD : OP_DSUB);
    }

    if (kind == LHS_METHOD)
    {
         int stack_words = this_control.IsDoubleWordType(expression_type) ? 2 : 1;
         PutOp(OP_INVOKESTATIC);
         CompleteCall(expression -> write_method, stack_words);
    }
    else StoreSimple(kind, expression -> expression);

    return;
}


//
// Post Unary for which operand is array element
// assignment for which lhs is array element
//    AstExpression *expression;
//
void ByteCode::EmitPostUnaryExpressionArray(AstPostUnaryExpression *expression, bool need_value) 
{
    EmitArrayAccessLHS((AstArrayAccess *) expression -> expression); // lhs must be array access
    PutOp(OP_DUP2); // save array base and index for later store

    TypeSymbol *expression_type = expression -> Type();
    if (expression_type == this_control.int_type)
    {
         PutOp(OP_IALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP_X2);
         PutOp(OP_ICONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
         PutOp(OP_IASTORE);
    }
    else if (expression_type == this_control.byte_type )
    {
         PutOp(OP_BALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP_X2);
         PutOp(OP_ICONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
         PutOp(OP_I2B);
         PutOp(OP_BASTORE);
    }
    else if (expression_type == this_control.char_type )
    {
         PutOp(OP_CALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP_X2);
         PutOp(OP_ICONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
         PutOp(OP_I2C);
         PutOp(OP_CASTORE);
    }
    else if (expression_type == this_control.short_type)
    {
         PutOp(OP_SALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP_X2);
         PutOp(OP_ICONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
         PutOp(OP_I2S);
         PutOp(OP_SASTORE);
    }
    else if (expression_type == this_control.long_type)
    {
         PutOp(OP_LALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP2_X2);
         PutOp(OP_LCONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_LADD : OP_LSUB);
         PutOp(OP_LASTORE);
    }
    else if (expression_type == this_control.float_type)
    {
         PutOp(OP_FALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP_X2);
         PutOp(OP_FCONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_FADD : OP_FSUB);
         PutOp(OP_FASTORE);
    }
    else if (expression_type == this_control.double_type)
    {
         PutOp(OP_DALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP2_X2);
         PutOp(OP_DCONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? OP_DADD : OP_DSUB);
         PutOp(OP_DASTORE);
    }
    else assert(false && "unsupported postunary type");

    return;
}


//
// PRE_UNARY
//
int ByteCode::EmitPreUnaryExpression(AstPreUnaryExpression *expression, bool need_value) 
{
    TypeSymbol *type = expression -> Type();
    if (expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ||
        expression -> pre_unary_tag == AstPreUnaryExpression::MINUSMINUS)
    {
        EmitPreUnaryIncrementExpression(expression, need_value);
    }
    else // here for ordinary unary operator without side effects.
    {
        switch (expression -> pre_unary_tag)
        {
            case AstPreUnaryExpression::PLUS:
                 // nothing to do (front-end will have done any needed conversions)
                 EmitExpression(expression -> expression);
                 break;
            case AstPreUnaryExpression::MINUS:
                 EmitExpression(expression -> expression);

                 assert((this_control.IsSimpleIntegerValueType(type) ||
                         type == this_control.long_type ||
                         type == this_control.float_type ||
                         type == this_control.double_type) && "unary minus on unsupported type");

                 PutOp(this_control.IsSimpleIntegerValueType(type) 
                           ? OP_INEG
                           : type == this_control.long_type
                                   ? OP_LNEG
                                   : type == this_control.float_type
                                           ? OP_FNEG
                                           : OP_DNEG); // double_type
                 break;
            case AstPreUnaryExpression::TWIDDLE:
                 if (this_control.IsSimpleIntegerValueType(type))
                 {
                     EmitExpression(expression -> expression);
                     PutOp(OP_ICONST_M1); // -1
                     PutOp(OP_IXOR);      // exclusive or to get result
                 }
                 else if (type == this_control.long_type)
                 {
                     EmitExpression(expression -> expression);
                     PutOp(OP_LCONST_1); // make -1
                     PutOp(OP_LNEG);
                     PutOp(OP_LXOR);     // exclusive or to get result
                 }
                 else assert(false && "unary ~ on unsupported type");
                 break;
            case AstPreUnaryExpression::NOT: 
                assert(type == this_control.boolean_type);

                {
                    Label lab1,
                          lab2;
                    EmitExpression(expression -> expression);
                    EmitBranch(OP_IFEQ, lab1);
                    PutOp(OP_ICONST_0);       // turn true into false
                    EmitBranch(OP_GOTO, lab2);
                    DefineLabel(lab1);
                    PutOp(OP_ICONST_1);       // here to turn false into true
                    DefineLabel(lab2);
                    CompleteLabel(lab1);
                    CompleteLabel(lab2);
                }
                break;
            default:
                assert(false && "unknown preunary tag");
        }
    }

    return GetTypeWords(type);
}
    

//
// PRE_UNARY with side effects (++X or --X)
//
void ByteCode::EmitPreUnaryIncrementExpression(AstPreUnaryExpression *expression, bool need_value) 
{
    int kind = GetLHSKind(expression);

    switch(kind)
    {
        case LHS_LOCAL:
        case LHS_STATIC:
             EmitPreUnaryIncrementExpressionSimple(kind, expression, need_value);
             break;
        case LHS_ARRAY:
             EmitPreUnaryIncrementExpressionArray(expression, need_value);
             break;
        case LHS_FIELD:
             EmitPreUnaryIncrementExpressionField(kind, expression, need_value);
             break;
        case LHS_METHOD:
             {
                 VariableSymbol *accessed_member = expression -> write_method -> accessed_member -> VariableCast();
                 if (accessed_member -> ACC_STATIC())
                      EmitPreUnaryIncrementExpressionSimple(kind, expression, need_value);
                 else EmitPreUnaryIncrementExpressionField(kind, expression, need_value);
             }
             break;
        default:
             assert(false && "unknown lhs kind for assignment");
    }

    return;
}


//
//    AstExpression *expression;
// POST_UNARY on name
// load value of variable, do increment or decrement, duplicate, then store back, leaving original value
// on top of stack.
//
void ByteCode::EmitPreUnaryIncrementExpressionSimple(int kind, AstPreUnaryExpression *expression, bool need_value) 
{
    TypeSymbol *type = expression -> Type();
    if (type == this_control.int_type)
    {
        if (kind == LHS_LOCAL)
        {
            PutOp(OP_IINC);
            PutU1(expression -> symbol -> VariableCast() -> LocalVariableIndex());
            PutI1(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? 1 : -1);
            if (need_value)
                (void) LoadSimple(kind, expression);
            return;
        }
    }

    (void) LoadSimple(kind, expression -> expression); // will also load value if resolution needed

    if (this_control.IsSimpleIntegerValueType(type)) // TBSL: use iinc eventually
    {
        PutOp(OP_ICONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
        EmitCast(type, this_control.int_type);
        if (need_value)
            PutOp(OP_DUP);
    }
    else if (type == this_control.long_type)
    {
        PutOp(OP_LCONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_LADD : OP_LSUB);
        if (need_value)
            PutOp(OP_DUP2);
    }
    else if (type == this_control.float_type)
    {
        PutOp(OP_FCONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_FADD : OP_FSUB);
        if (need_value)
            PutOp(OP_DUP);
    }
    else if (type == this_control.double_type)
    {
        PutOp(OP_DCONST_1); // load 1.0
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_DADD : OP_DSUB);
        if (need_value)
            PutOp(OP_DUP2);
    }

    if (kind == LHS_METHOD)
    {
        int stack_words = this_control.IsDoubleWordType(type) ? 2 : 1;
        PutOp(OP_INVOKESTATIC);
        CompleteCall(expression -> write_method, stack_words);
    }
    else StoreSimple(kind, expression);

    return;
}


//
// Post Unary for which operand is array element
// assignment for which lhs is array element
//    AstExpression *expression;
//
void ByteCode::EmitPreUnaryIncrementExpressionArray(AstPreUnaryExpression *expression, bool need_value) 
{
    EmitArrayAccessLHS((AstArrayAccess *) expression -> expression); // lhs must be array access

    PutOp(OP_DUP2); // save array base and index for later store

    TypeSymbol *type = expression -> Type();
    if (type == this_control.int_type)
    {
         PutOp(OP_IALOAD);
         PutOp(OP_ICONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
         if (need_value)
             PutOp(OP_DUP_X2);
         PutOp(OP_IASTORE);
    }
    else if (type == this_control.byte_type)
    {
         PutOp(OP_BALOAD);
         PutOp(OP_ICONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
         if (need_value)
             PutOp(OP_DUP_X2);
         PutOp(OP_I2B);
         PutOp(OP_BASTORE);
    }
    else if (type == this_control.char_type)
    {
         PutOp(OP_CALOAD);
         PutOp(OP_ICONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
         if (need_value)
             PutOp(OP_DUP_X2);
         PutOp(OP_I2C);
         PutOp(OP_CASTORE);
    }
    else if (type == this_control.short_type)
    {
         PutOp(OP_SALOAD);
         PutOp(OP_ICONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
         if (need_value)
             PutOp(OP_DUP_X2);
         PutOp(OP_I2S);
         PutOp(OP_SASTORE);
    }
    else if (type == this_control.long_type)
    {
         PutOp(OP_LALOAD);
         PutOp(OP_LCONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_LADD : OP_LSUB);
         if (need_value)
             PutOp(OP_DUP2_X2);
         PutOp(OP_LASTORE);
    }
    else if (type == this_control.float_type)
    {
         PutOp(OP_FALOAD);
         PutOp(OP_FCONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_FADD : OP_FSUB);
         if (need_value)
             PutOp(OP_DUP_X2);
         PutOp(OP_FASTORE);
    }
    else if (type == this_control.double_type)
    {
         PutOp(OP_DALOAD);
         PutOp(OP_DCONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_DADD : OP_DSUB);
         if (need_value)
             PutOp(OP_DUP2_X2);
         PutOp(OP_DASTORE);
    }
    else assert(false && "unsupported PreUnary type");

    return;
}


//
// Pre Unary for which operand is field (instance variable)
// AstExpression *expression;
//
void ByteCode::EmitPreUnaryIncrementExpressionField(int kind, AstPreUnaryExpression *expression, bool need_value) 
{
    if (kind == LHS_METHOD)
        ResolveAccess(expression -> expression); // get address and value
    else // need to load address of object, obtained from resolution, saving a copy on the stack
        EmitFieldAccessLHS(expression -> expression);

    TypeSymbol *expression_type = expression -> Type();
    if (this_control.IsSimpleIntegerValueType(expression_type))
    {
        PutOp(OP_ICONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_IADD : OP_ISUB);
        EmitCast(expression_type, this_control.int_type);
        if (need_value)
            PutOp(OP_DUP_X1);
    }
    else if (expression_type == this_control.long_type)
    {
        PutOp(OP_LCONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_LADD : OP_LSUB);
        if (need_value)
            PutOp(OP_DUP2_X1);
    }
    else if (expression_type == this_control.float_type)
    {
        PutOp(OP_FCONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_FADD : OP_FSUB);
        if (need_value)
            PutOp(OP_DUP_X1);
    }
    else if (expression_type == this_control.double_type)
    {
        PutOp(OP_DCONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? OP_DADD : OP_DSUB);
        if (need_value)
            PutOp(OP_DUP2_X1);
    }
    else assert(false && "unsupported PreUnary type");

    if (kind == LHS_METHOD)
    {
        int stack_words = (this_control.IsDoubleWordType(expression_type) ? 2 : 1) + 1;
        PutOp(OP_INVOKESTATIC);
        CompleteCall(expression -> write_method, stack_words);
    }
    else
    {
        PutOp(OP_PUTFIELD);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? -3 : -2);
        PutU2(GenerateFieldReference((VariableSymbol *) expression -> symbol));
    }

    return;
}


void ByteCode::EmitThisInvocation(AstThisCall *this_call)
{
    //
    // THIS_CALL
    //    AstExpression *method;
    //    AstList *arguments;
    // A call to another constructor (THIS_CALL) or super constructor (SUPER_CALL)
    // result in the same sort of generated code, as the semantic analysis
    // has resolved the proper constructor to be invoked.
    //
    PutOp(OP_ALOAD_0); // load 'this'

    int stack_words = 0; // words on stack needed for arguments
    if (this_call -> base_opt)
        stack_words += EmitExpression(this_call -> base_opt);

    for (int i = 0; i < this_call -> NumLocalArguments(); i++)
        stack_words += EmitExpression((AstExpression *) this_call -> LocalArgument(i));

    for (int k = 0; k < this_call -> NumArguments(); k++)
        stack_words += EmitExpression((AstExpression *) this_call -> Argument(k));

    PutOp(OP_INVOKENONVIRTUAL);
    ChangeStack(-stack_words);

    //
    // need to get method index, the constant_pool index for a
    // reference to this method (a Methodref);
    // caller will supply methodref
    //
    MethodSymbol *msym = this_call -> symbol;
    if (msym -> constant_pool_index == 0 || msym -> constant_pool_class != class_id) // build method ref for method
    {
        msym -> constant_pool_index = BuildMethodref(this_class,
                                                     BuildNameAndType(RegisterUtf8(msym -> ExternalIdentity() -> Utf8_literal),
                                                                      RegisterUtf8(msym -> signature)));
        msym -> constant_pool_class = class_id;
    }
    
    PutU2(msym -> constant_pool_index);

    return;
}
 

void ByteCode::EmitSuperInvocation(AstSuperCall *super_call)
{
    PutOp(OP_ALOAD_0); // load 'this'

    int stack_words = 0; // words on stack needed for arguments
    if (super_call -> base_opt)
        stack_words += EmitExpression(super_call -> base_opt);

    for (int i = 0; i < super_call -> NumLocalArguments(); i++)
        stack_words += EmitExpression((AstExpression *) super_call -> LocalArgument(i));

    if (super_call -> NeedsExtraNullArgument())
    {
        PutOp(OP_ACONST_NULL);
        stack_words += 1;
    }

    for (int k = 0; k < super_call -> NumArguments(); k++)
        stack_words += EmitExpression((AstExpression *) super_call -> Argument(k));

    PutOp(OP_INVOKENONVIRTUAL);
    ChangeStack(-stack_words);

    //
    // need to get method index, the constant_pool index for a
    // reference to this method (a methodref);
    // caller will supply methodref
    //
    MethodSymbol *msym = super_call -> symbol;
    if (msym -> constant_pool_index == 0 || msym -> constant_pool_class != class_id) // build method ref for method
    {
        msym -> constant_pool_index = BuildMethodref(super_class,
                                                     BuildNameAndType(RegisterUtf8(msym -> ExternalIdentity() -> Utf8_literal),
                                                                      RegisterUtf8(msym -> signature)));
        msym -> constant_pool_class = class_id;
    }
    
    PutU2(msym -> constant_pool_index);

    return;
}
 

//
//  Methods for string concatenation
//
void ByteCode::ConcatenateString(AstBinaryExpression *expression)
{
    //
    // generate code to concatenate strings, by generating a string buffer and appending the arguments
    // before calling toString, i.e.,
    //  s1+s2 compiles to
    //  new StringBuffer().append(s1).append(s2).toString();
    // look for sequences of concatenation to use a single buffer where possible
    //
    EmitStringBuffer();
    AppendString(expression -> left_expression); 
    AppendString(expression -> right_expression);
    EmitCallStringToString(); // convert string buffer to string

    return;
}


//
// generate call to toString on stringbuffer
//
void ByteCode::EmitCallStringToString()
{
    PutOp(OP_INVOKEVIRTUAL);
    PutU2(RegisterMethodStringbufferTostring());
    ChangeStack(1); // account for return value

    return;
}


//
// generate code to allocate new string buffer and initialize it
//
void ByteCode::EmitStringBuffer()
{
    PutOp(OP_NEW);
    PutU2(RegisterClass(U8S_java_SL_lang_SL_StringBuffer, strlen(U8S_java_SL_lang_SL_StringBuffer)));
    PutOp(OP_DUP);
    PutOp(OP_INVOKENONVIRTUAL);
    PutU2(RegisterMethodStringbufferInit());

    return;
}


void ByteCode::AppendString(AstExpression *p)
{
    AstBinaryExpression *binexpr;
    if (p -> BinaryExpressionCast())
    {
        binexpr = p -> BinaryExpressionCast();
        if (binexpr -> binary_tag == AstBinaryExpression::PLUS &&
            (IsReferenceType(binexpr -> left_expression -> Type()) || IsReferenceType(binexpr -> right_expression -> Type())))
        {
            AppendString(binexpr -> left_expression);
            AppendString(binexpr -> right_expression);

            return;
        } 
    }

    if (p -> ParenthesizedExpressionCast())
    {
        AppendString(p -> ParenthesizedExpressionCast() -> expression);
        return;
    }

    AstCastExpression *cast = p -> CastExpressionCast();
    if (cast) // here if cast expression, verify that converting to string
    {
        if (cast -> kind == Ast::CAST && cast -> Type() == this_control.String())
        {
            AppendString(cast -> expression);
            return;
        }
    }

    //
    // replace explicit reference to "null" by 
    // corresponding string.
    //
    TypeSymbol *type = p -> Type();
    if (type == this_control.null_type)
    {
        if (NameStringNull() <= 255)
        {
            PutOp(OP_LDC);
            PutU1((u1) NameStringNull());
        }
        else
        {
            PutOp(OP_LDC_W);
            PutU2(NameStringNull());
        }
        type = this_control.String();
    }
    else EmitExpression(p);

    EmitStringAppendMethod(type);

    return;
}


void ByteCode::EmitStringAppendMethod(TypeSymbol *type) 
{
    int method_sig = 0,
        stack_words = 1; // assume one word put on stack

    //
    // call appropriate append routine to add to string buffer
    //
    if (type -> num_dimensions == 1 && type -> base_type == this_control.char_type)
         method_sig = RegisterMethodStringbufferAppendchararray();
    else if (type == this_control.char_type)
         method_sig = RegisterMethodStringbufferAppendchar();
    else if (type == this_control.boolean_type)
         method_sig = RegisterMethodStringbufferAppendboolean();
    else if (type == this_control.int_type|| type == this_control.short_type || type == this_control.byte_type)
         method_sig = RegisterMethodStringbufferAppendint();
    else if (type == this_control.long_type)
    {
         method_sig = RegisterMethodStringbufferAppendlong();
         stack_words += 1;
    }
    else if (type == this_control.float_type)
         method_sig = RegisterMethodStringbufferAppendfloat();
    else if (type == this_control.double_type)
    {
         method_sig = RegisterMethodStringbufferAppenddouble();
         stack_words += 1;
    }
    else if (type == this_control.String())
         method_sig = RegisterMethodStringbufferAppendstring();
    else if (IsReferenceType(type))
         method_sig = RegisterMethodStringbufferAppendobject();
    else assert(false && "unexpected string buffer append operand");

    PutOp(OP_INVOKEVIRTUAL);
    ChangeStack(-stack_words);
    PutU2(method_sig);
    ChangeStack(1); // account for return value
    assert((method_sig != 0) && "unable to find type for string buffer concatenation");

    return;
}


#ifdef TEST
static void op_trap()
{
    int i = 0; // used for debugger trap
}
#endif


ByteCode::ByteCode(TypeSymbol *unit_type) : ClassFile(unit_type),
                                            this_semantic(*unit_type -> semantic_environment -> sem),
                                            this_control(unit_type -> semantic_environment -> sem -> control),
                                            name_string_null(0),
                                            method_clone_index(0),
                                            method_clone_getmessage_index(0),
                                            method_clone_init_index(0),
                                            method_stringbuffer_tostring_index(0),
                                            method_stringbuffer_init_index(0),
                                            method_stringbuffer_appendchararray_index(0),
                                            method_stringbuffer_appendchar_index(0),
                                            method_stringbuffer_appendboolean_index(0),
                                            method_stringbuffer_appendint_index(0),
                                            method_stringbuffer_appendlong_index(0),
                                            method_stringbuffer_appendfloat_index(0),
                                            method_stringbuffer_appenddouble_index(0),
                                            method_stringbuffer_appendstring_index(0),
                                            method_stringbuffer_appendobject_index(0)
{
#ifdef TEST
    if (this_control.option.nowrite == 0)
        this_control.class_files_written++;
#endif

    this_control.class_file_id++;
    class_id = this_control.class_file_id;
    stack_depth = 0;
    synchronized_blocks = 0;
    finally_blocks = 0;
    
    access_flags = unit_type -> access_flags;

    //
    // The flags for 'static' and 'protected' are set only for the inner
    // classes attribute, not for the class, as described in page 25
    // of the inner classes document.
    //
    if (unit_type -> ACC_PROTECTED())
        access_flags |= 0x0001; // set PUBLIC if PROTECTED
    access_flags &= (~ 0x0008); // ResetACC_STATIC
    access_flags &= (~ 0x0004); // ResetACC_PROTECTED
    access_flags &= (~ 0x0002); // ResetACC_PRIVATE
    access_flags |= 0x0020; // must be set always set ACC_SUPER for class (cf page 86 of JVM Spec)
    
    magic = 0xcafebabe;
    major_version = 45;             // use Sun JDK 1.0 version numbers
    minor_version = 3;
    constant_pool.Next() = NULL;
    this_class = RegisterClass(unit_type -> fully_qualified_name);

    super_class = (unit_type -> super ? RegisterClass(unit_type -> super -> fully_qualified_name) : 0);

    for (int k = 0; k < unit_type -> NumInterfaces(); k++)
        interfaces.Next() = RegisterClass(unit_type -> Interface(k) -> fully_qualified_name);

    return;
}


//
//  Methods for manipulating labels
//
void ByteCode::DefineLabel(Label& lab)
{
    assert((! lab.defined) && "duplicate label definition");

    lab.defined = true;
    lab.definition = code_attribute -> CodeLength();
    if (lab.definition > last_label_pc)
        last_label_pc = lab.definition;

    return;
}


//
// patch all uses to have proper value. This requires that
// all labels be freed at some time.
//
void ByteCode::CompleteLabel(Label& lab)
{
    if (lab.uses.Length() > 0)
    {
        assert((lab.defined) && "label used but with no definition");
                
        //
        // patch byte code reference to label to reflect it's definition
        // as 16-bit signed offset.
        //
        for (int i = 0; i < lab.uses.Length(); i++)
        {
            unsigned int luse = lab.uses[i].use_offset;
            int start = luse - lab.uses[i].op_offset,
                offset = lab.definition - start;
            if (lab.uses[i].use_length == 2) // here if short offset
            {
                code_attribute -> ResetCode(luse, (offset >> 8) & 0xFF);
                code_attribute -> ResetCode(luse + 1, offset & 0xFF);
            }
            else if (lab.uses[i].use_length == 4) // here if 4 byte use
            {
                code_attribute -> ResetCode(luse, (offset >> 24) & 0xFF);
                code_attribute -> ResetCode(luse + 1, (offset >> 16) & 0xFF);
                code_attribute -> ResetCode(luse + 2, (offset >>  8) & 0xFF);
                code_attribute -> ResetCode(luse + 3, offset & 0xFF);
            }
            else assert(false &&  "label use length not 2 or 4");
        }

        lab.uses.Reset();
    }

    //
    // reset in case label is used again.
    //
    lab.definition = 0;
    lab.defined = false;

    return;
}


void ByteCode::UseLabel(Label &lab, int _length, int _op_offset)
{
    int lab_index = lab.uses.NextIndex();
    lab.uses[lab_index].use_length = _length;
    lab.uses[lab_index].op_offset = _op_offset;
    lab.uses[lab_index].use_offset = code_attribute -> CodeLength();

    //
    // fill next length bytes with zero; will be filled in with proper value when label completed
    //
    for (int i = 0; i < lab.uses[lab_index].use_length; i++)
        code_attribute -> AddCode(0);

    return;
}


//
// see if operand is default value of its type
//
bool ByteCode::IsDefaultValue(AstExpression *p)
{
    if (! p -> IsConstant())
         return false;

    LiteralValue *litp = p -> value;
    TypeSymbol *type = p -> Type();
    if (this_control.IsSimpleIntegerValueType(type) || type == this_control.boolean_type)
    {
        IntLiteralValue *vp = (IntLiteralValue *) litp;
        return (vp -> value == 0);
    }
    else if (type == this_control.long_type)
    {
        LongLiteralValue *vp = (LongLiteralValue *) litp;
        return (vp -> value == 0);
    }
    else if (type == this_control.float_type)
    {
        FloatLiteralValue *vp = (FloatLiteralValue *) litp;
        return (vp -> value.FloatValue() == 0.0F);
    }
    else if (type == this_control.double_type)
    {
        DoubleLiteralValue *vp = (DoubleLiteralValue *) litp;
        return (vp -> value.DoubleValue() == 0.0L);
    }

    //
    // the default value for everything else is null
    //
    return (type == this_control.null_type);
}


//
//  Methods to load values
//
int ByteCode::GetConstant(LiteralValue *litp, TypeSymbol *type)
{
    int lit_index;

    if (type == this_control.String())
    {
        Utf8LiteralValue *vp = (Utf8LiteralValue *) litp;
        lit_index = (vp -> constant_pool_index_String != 0 && vp -> constant_pool_class == class_id
                                                       ? vp -> constant_pool_index_String
                                                       : RegisterString(vp));
    }
    else
    {
        if (litp -> constant_pool_index != 0 && litp -> constant_pool_class == class_id)
            lit_index = litp -> constant_pool_index;
        else // load literal using literal value
        {
            if (this_control.IsSimpleIntegerValueType(type) || type == this_control.boolean_type)
            {
                IntLiteralValue *vp = (IntLiteralValue *) litp;
                lit_index = RegisterInteger(vp);
            }
            else if (type == this_control.float_type)
            {
                FloatLiteralValue *vp = (FloatLiteralValue *) litp;
                IEEEfloat val = vp -> value;
                lit_index = RegisterFloat(vp);
            }
            else if (type == this_control.long_type)
            {
                LongLiteralValue *vp = (LongLiteralValue *) litp;
                lit_index = RegisterLong(vp);
            }
            else if (type == this_control.double_type)
            {
                DoubleLiteralValue *vp = (DoubleLiteralValue *) litp;
                lit_index = RegisterDouble(vp);
            }
            else assert(false && "unexpected GetConstant kind");
        }
    }

    return lit_index;
}


//
// see if can load without using LDC even if have literal index; otherwise generate constant pool entry
// if one has not yet been generated.
//
//
int ByteCode::LoadLiteral(LiteralValue* litp, TypeSymbol *type) 
{
    int lit_index = (litp -> constant_pool_index > 0 && litp -> constant_pool_class == class_id
                                                      ? litp -> constant_pool_index
                                                      : 0);
    if (litp == this_control.NullValue())
    {
        PutOp(OP_ACONST_NULL);
        return 1;
    }

    if (type == this_control.String()) // register index as string if this has not yet been done
    {
        lit_index = RegisterString((Utf8LiteralValue *) litp);
    }
    else if (this_control.IsSimpleIntegerValueType(type) || type == this_control.boolean_type) // load literal using literal value
    {
        IntLiteralValue *vp = (IntLiteralValue *) litp;
        int val = vp -> value;
        if (val >= -32768 && val < 32768)
        {
            LoadShort(val);
            return 1;
        }

        lit_index = RegisterInteger(vp);
    }
    else if (type == this_control.float_type)
    {
        FloatLiteralValue *vp = (FloatLiteralValue *) litp;
        IEEEfloat val = vp -> value;
        if (val.Word() == 0) // if float 0.0
        {
            PutOp(OP_FCONST_0);
            return 1;
        }
        else if (val.Word() == 0x3f800000) // if float 1.0
        {
            PutOp(OP_FCONST_1);
            return 1;
        }
        else if (val.Word() == 0x40000000) // if float 2.0
        {
            PutOp(OP_FCONST_2);
            return 1;
        }

        lit_index = RegisterFloat(vp);
    }
    else if (type == this_control.long_type)
    {
        LongLiteralValue *vp = (LongLiteralValue *) litp;
        LongInt val = vp -> value;
        if (val == 0)
        {
            PutOp(OP_LCONST_0); // long 0
            return 2;
        }
        else if (val == 1)
        {
            PutOp(OP_LCONST_1); // long 1
            return 2;
        }

        lit_index = RegisterLong(vp);
    }
    else if (type == this_control.double_type)
    {
        DoubleLiteralValue *vp = (DoubleLiteralValue *) litp;
        IEEEdouble val = vp -> value;
        if (val.HighWord() == 0 && val.LowWord() == 0)
        {
            PutOp(OP_DCONST_0);
            return 2;
        }
        else if (val.HighWord() == 0x3ff00000 && val.LowWord() == 0x00000000) // if double 1.0
        {
            PutOp(OP_DCONST_1);
            return 2;
        }

        lit_index = RegisterDouble(vp);
    }
    else assert(false && "unsupported constant kind");

    assert(lit_index != 0);

    if (type == this_control.long_type || type == this_control.double_type)
    {
        PutOp(OP_LDC2_W);
        PutU2(lit_index);
        return 2;
    }

    if (lit_index <= 255)
    {
        PutOp(OP_LDC);
        PutU1(lit_index);
    }
    else
    {
        PutOp(OP_LDC_W);
        PutU2(lit_index);
    }

    return 1;
}


void ByteCode::LoadLocal(int varno, TypeSymbol *type)
{
    if (this_control.IsSimpleIntegerValueType(type) || type == this_control.boolean_type)
    {
         if (varno <= 3)
              PutOp(OP_ILOAD_0 + varno);
         else if (varno < 256)
         {
              PutOp(OP_ILOAD);
              PutU1(varno);
         }
         else
         {
              PutOp(OP_WIDE);
              PutOp(OP_ILOAD);
              PutU2(varno);
         }
    }
    else if (type == this_control.long_type)
    {
         if (varno <= 3)
              PutOp(OP_LLOAD_0 + varno);
         else if (varno < 256)
         {
              PutOp(OP_LLOAD);
              PutU1(varno);
         }
         else
         {
              PutOp(OP_WIDE);
              PutOp(OP_LLOAD);
              PutU2(varno);
         }
    }
    else if (type == this_control.float_type)
    {
         if (varno <= 3)
              PutOp(OP_FLOAD_0 + varno);
         else if (varno < 256)
         {
              PutOp(OP_FLOAD);
              PutU1(varno);
         }
         else
         {
              PutOp(OP_WIDE);
              PutOp(OP_FLOAD);
              PutU2(varno);
         }
    }
    else if (type == this_control.double_type)
    {
         if (varno <= 3)
              PutOp(OP_DLOAD_0 + varno);
         else if (varno < 256)
         {
              PutOp(OP_DLOAD);
              PutU1(varno);
         }
         else
         {
              PutOp(OP_WIDE);
              PutOp(OP_DLOAD);
              PutU2(varno);
         }
    }
    else // assume reference
    {
         if (varno <= 3)
              PutOp(OP_ALOAD_0 + varno);
         else if (varno < 256)
         {
              PutOp(OP_ALOAD);
              PutU1(varno);
         }
         else
         {
              PutOp(OP_WIDE);
              PutOp(OP_ALOAD);
              PutU2(varno);
         }
    }

    return;
}


void ByteCode::LoadInteger(int val)
{
    
    if (val >= -32768 && val < 32768) // if short
        LoadShort(val);
    else
    {
        u2 index = BuildInteger(val);
        if (index <= 255)
        {
            PutOp(OP_LDC);
            PutU1((u1) index);
        }
        else
        {
            PutOp(OP_LDC_W);
            PutU2(index);
        }
    }

    return;
}


//
// load short (signed) value onto stack
//
void ByteCode::LoadShort(int val)
{
    if (val >= -128 && val < 128)
    {
        switch (val)
        {
            case -1:
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                PutOp(OP_ICONST_0 + val); // exploit opcode encoding
                break;
            default: // else put byte value
                PutOp(OP_BIPUSH);
                PutU1(val);
        }
    }
    else if (val >= -32768 && val < 32768) // if short
    {
        PutOp(OP_SIPUSH);
        PutU1((val >> 8)); 
        PutU1(val);
    }
    else assert(false && "bcShort operand not short!");

    return;
}


//
// Call to an access method for a compound operator such as ++, --,
// or "op=".
//
void ByteCode::ResolveAccess(AstExpression *p)
{
    AstFieldAccess *field = p -> FieldAccessCast();
    AstExpression *resolve_expression = (field ? field -> resolution_opt : p -> SimpleNameCast() -> resolution_opt);
    AstMethodInvocation *read_method = resolve_expression -> MethodInvocationCast();

    assert(read_method && read_method -> NumArguments() == 1); // a read method has exactly one argument: the object in question.

    int stack_words = EmitExpression((AstExpression *) read_method -> Argument(0));
    PutOp(OP_DUP);
    PutOp(OP_INVOKESTATIC);
    CompleteCall(read_method -> symbol -> MethodCast(), stack_words);

    return;
}
    

int ByteCode::LoadSimple(int kind, AstExpression *expr)
{
    VariableSymbol *sym = (VariableSymbol *) expr -> symbol;
    TypeSymbol *expression_type = expr -> Type();
    switch (kind)
    {
        case LHS_LOCAL:
             LoadLocal(sym -> LocalVariableIndex(), expression_type);
             break;
        case LHS_METHOD:
             EmitExpression(expr); // will do resolution
             break;
        case LHS_FIELD:
        case LHS_STATIC:
             if (sym -> ACC_STATIC())
             {
                 PutOp(OP_GETSTATIC);
                 ChangeStack(GetTypeWords(expression_type));
             }
             else
             {
                 PutOp(OP_ALOAD_0); // get address of "this"
                 PutOp(OP_GETFIELD);
                 ChangeStack(GetTypeWords(expression_type) - 1);
             }

             PutU2(GenerateFieldReference(sym));
             break;
        default:
             assert(false && "LoadSimple bad kind");
    }

    return GetTypeWords(expression_type);
}


//
// load reference from local variable.
// otherwise will use getstatic or getfield.
//
void ByteCode::LoadReference(AstExpression *expression) 
{
    if (expression -> ParenthesizedExpressionCast())
        expression = UnParenthesize(expression);

    VariableSymbol *sym = expression -> symbol -> VariableCast();
    if (sym && sym -> owner -> MethodCast()) // a local variable ?
    {
        int varno = sym -> LocalVariableIndex();
        LoadLocal(varno, expression -> Type());
        return;
    }

    AstFieldAccess *field_access = expression -> FieldAccessCast();
    if (field_access)
    {
        if (field_access -> resolution_opt) // This field access was resolved... Process the resolution
        {
            EmitExpression(field_access -> resolution_opt);
            return;
        }

        if (sym -> ACC_STATIC())
        {
            PutOp(OP_GETSTATIC);
            ChangeStack(1);
        }
        else
        {
            EmitExpression(field_access -> base);
            PutOp(OP_GETFIELD);
            ChangeStack(0);
        }

        PutU2(GenerateFieldReference(sym));
    }
    else if (expression -> ArrayAccessCast()) // nested array reference
    {
        EmitArrayAccessLHS(expression -> ArrayAccessCast());
        PutOp(OP_AALOAD);
    }
    else // must have expression, the value of which is reference
        EmitExpression(expression);

    return;
}


int  ByteCode::LoadArrayElement(TypeSymbol *type)
{
    PutOp(type == this_control.byte_type || type == this_control.boolean_type
                ? OP_BALOAD
                : type == this_control.short_type
                        ? OP_SALOAD
                        : type == this_control.int_type
                                ? OP_IALOAD
                                : type == this_control.long_type
                                        ? OP_LALOAD
                                        : type == this_control.char_type
                                                ? OP_CALOAD
                                                : type == this_control.float_type
                                                        ? OP_FALOAD
                                                        : type == this_control.double_type
                                                                ? OP_DALOAD
                                                                : OP_AALOAD); // assume reference

    return GetTypeWords(type);
}


void ByteCode::StoreArrayElement(TypeSymbol *type)
{
    PutOp(type == this_control.byte_type || type == this_control.boolean_type
                ? OP_BASTORE
                : type == this_control.short_type
                        ? OP_SASTORE
                        : type == this_control.int_type
                                ? OP_IASTORE
                                : type == this_control.long_type
                                        ? OP_LASTORE
                                        : type == this_control.char_type
                                                ? OP_CASTORE
                                                : type == this_control.float_type
                                                        ? OP_FASTORE
                                                        : type == this_control.double_type
                                                                ? OP_DASTORE
                                                                : OP_AASTORE); // assume reference

    return;
}


//
//  Method to generate field reference
//
void ByteCode::StoreField(AstExpression *expression) 
{
    VariableSymbol *sym = (VariableSymbol *) expression -> symbol;
    TypeSymbol *expression_type = expression -> Type();
    if (sym -> ACC_STATIC())
    {
        PutOp(OP_PUTSTATIC);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? -2 : -1);
    }
    else
    {
        PutOp(OP_PUTFIELD);
        ChangeStack(this_control.IsDoubleWordType(expression_type) ? -3 : -2);
    }

    PutU2(GenerateFieldReference(sym));

    return;
}


void ByteCode::StoreLocalVariable(VariableSymbol *var)
{
    StoreLocal(var -> LocalVariableIndex(), var -> Type());
    if (this_control.option.g && var -> LocalVariableIndex() > last_parameter_index)
    {
        //
        // here to update point of first assignment, marking point at which value is
        // available to be displayed by debugger.
        //
        if (var -> local_program_counter == 0)
            var -> local_program_counter = code_attribute -> CodeLength();
    }

    return;
}

void ByteCode::StoreLocal(int varno, TypeSymbol *type)
{
    if (this_control.IsSimpleIntegerValueType(type) || type == this_control.boolean_type)
    {
         if (varno <= 3)
              PutOp(OP_ISTORE_0 + varno);
         else if (varno < 256)
         {
              PutOp(OP_ISTORE);
              PutU1(varno);
         }
         else
         {
              PutOp(OP_WIDE);
              PutOp(OP_ISTORE);
              PutU2(varno);
         }
    }
    else if (type == this_control.long_type)
    {
         if (varno <= 3)
              PutOp(OP_LSTORE_0 + varno);
         else if (varno < 256)
         {
              PutOp(OP_LSTORE);
              PutU1(varno);
         }
         else
         {
              PutOp(OP_WIDE);
              PutOp(OP_LSTORE);
              PutU2(varno);
         }
    }
    else if (type == this_control.float_type)
    {
         if (varno <= 3)
              PutOp(OP_FSTORE_0 + varno);
         else if (varno < 256)
         {
              PutOp(OP_FSTORE);
              PutU1(varno);
         }
         else
         {
              PutOp(OP_WIDE);
              PutOp(OP_FSTORE);
              PutU2(varno);
         }
    }
    else if (type == this_control.double_type)
    {
         if (varno <= 3)
              PutOp(OP_DSTORE_0 + varno);
         else if (varno < 256)
         {
              PutOp(OP_DSTORE);
              PutU1(varno);
         }
         else
         {
              PutOp(OP_WIDE);
              PutOp(OP_DSTORE);
              PutU2(varno);
         }
    }
    else // assume reference
    {
         if (varno <= 3)
              PutOp(OP_ASTORE_0 + varno);
         else if (varno < 256)
         {
              PutOp(OP_ASTORE);
              PutU1(varno);
         }
         else
         {
              PutOp(OP_WIDE);
              PutOp(OP_ASTORE);
              PutU2(varno);
         }
    }

    return;
}


//
// generate a field reg from symbol and class literal
// build field ref for field
// the field ref requires Utf8 entries for the containing
// class, the field name and the field signature, the latter
// two expressed as a NameAndTypeEntry
//
int ByteCode::GenerateFieldReference(VariableSymbol *sym)
{
    if (sym -> constant_pool_index == 0 || sym -> constant_pool_class != class_id)
    {
        TypeSymbol *owner = (TypeSymbol *) sym -> owner;
        sym -> constant_pool_index = BuildFieldref(RegisterClass(owner -> fully_qualified_name),
                                                   BuildNameAndType(RegisterUtf8(sym -> ExternalIdentity()-> Utf8_literal),
                                                                    RegisterUtf8(sym -> Type() -> signature)));
        sym -> constant_pool_class = class_id;
    }

    return sym -> constant_pool_index;
}


void ByteCode::StoreSimple(int kind, AstExpression *p)
{
    VariableSymbol *sym = (VariableSymbol *) p -> symbol;
    TypeSymbol *expression_type = p -> Type();
    switch (kind)
    {
        case LHS_LOCAL:
             StoreLocalVariable(sym);
             break;
        case LHS_FIELD:
        case LHS_STATIC:
             if (sym -> ACC_STATIC())
             {
                 PutOp(OP_PUTSTATIC);
                 ChangeStack(this_control.IsDoubleWordType(expression_type) ? -2 : -1);
             }
             else
             {
                 PutOp(OP_ALOAD_0); // get address of "this"
                 PutOp(OP_PUTFIELD);
                 ChangeStack(this_control.IsDoubleWordType(expression_type) ? -3 : -2);
             }

             PutU2(GenerateFieldReference(sym));
             break;
        default:
            assert(false && "StoreSimple bad kind");
    }

    return;
}


//
// finish off code by writing SourceFile attribute
// and InnerClasses attribute (if appropriate)
//
void ByteCode::FinishCode(TypeSymbol *type)
{
    char *file_name = this_semantic.lex_stream -> FileName();

    int i;
    for (i = this_semantic.lex_stream -> FileNameLength() - 1; i >= 0; i--)
    {
        if (file_name[i] == U_SLASH)
            break;
    }

    int file_name_start = i + 1,
        file_name_length = this_semantic.lex_stream -> FileNameLength() - file_name_start;

    attributes.Next() = new SourceFile_attribute(RegisterUtf8(U8S_Sourcefile, U8S_Sourcefile_length),
                                                 BuildUtf8(file_name + file_name_start, file_name_length));

    if (type == NULL)
        return; // return if interface type

    if (type -> IsLocal() || type -> IsNested() || type -> NumNestedTypes() > 0) // here to generate InnerClasses attribute
    {
        inner_classes_attribute = new InnerClasses_attribute(RegisterUtf8(U8S_InnerClasses, U8S_InnerClasses_length));

        //
        // need to build chain from this type to its owner all the way to the containing type
        // and then write that out in reverse order (so containing type comes first),
        // and then write out an entry for each immediately contained type
        //
        Tuple<TypeSymbol *> owners;
        for (TypeSymbol *t = type; t && t != type -> outermost_type; t = t -> ContainingType())
            owners.Next() = t;

        for (int j = owners.Length() - 1; j >= 0; j--)
        {
            TypeSymbol *outer = owners[j];
            inner_classes_attribute -> AddInnerClass(RegisterClass(outer -> fully_qualified_name),
                                                     outer -> IsLocal()
                                                            ? 0
                                                            : RegisterClass(outer -> ContainingType() -> fully_qualified_name),
                                                     outer -> Anonymous()
                                                            ? 0
                                                            : RegisterUtf8(outer -> name_symbol -> Utf8_literal),
                                                     outer -> access_flags);
        }
          
        for (int k = 0; k < type -> NumNestedTypes(); k++)
        {
            TypeSymbol *nested = type -> NestedType(k);
            inner_classes_attribute -> AddInnerClass(RegisterClass(nested -> fully_qualified_name),
                                                     nested -> IsLocal()
                                                             ? 0
                                                             : RegisterClass(nested -> ContainingType() -> fully_qualified_name),
                                                     nested -> Anonymous()
                                                             ? 0
                                                             : RegisterUtf8(nested -> name_symbol -> Utf8_literal),
                                                     nested -> access_flags);
        }

        attributes.Next() = inner_classes_attribute;
    }

    return;
}


void ByteCode::PutOp(unsigned char opc)
{
#ifdef TEST
    int len = code_attribute -> CodeLength(); // show current position
    if (this_control.option.debug_trap_op > 0 && code_attribute -> CodeLength() == this_control.option.debug_trap_op)
        op_trap();

    //
    // debug trick - force branch on opcode to see what opcode we are compiling
    //
    switch (opc)
    {
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

    last_op_pc = code_attribute -> CodeLength(); // save pc at start of operation
    code_attribute -> AddCode(opc);
    ChangeStack(stack_effect[opc]);

    return;
}


void ByteCode::ChangeStack(int i)
{
    stack_depth += i;
    if (stack_depth < 0)
        stack_depth = 0;

    if (i > 0 && stack_depth > code_attribute -> max_stack)
        code_attribute -> max_stack = stack_depth;

#ifdef TRACE_STACK_CHANGE
    cout << "stack change: pc " << last_op_pc << " change " << i <<
            "  stack_depth " << stack_depth << "  max_stack: "<< code_attribute -> max_stack << "\n";
#endif

    return;
}


#ifdef TEST
void ByteCode::PrintCode()
{
    cout << "magic " << hex << magic << dec
         << " major_version " << major_version
         << " minor_version " << minor_version << "\n";
    AccessFlags::Print();
    cout << "\n";
    cout << " this_class " << this_class << "  super_class " << super_class <<"\n";
    cout << " constant_pool: " << constant_pool.Length() << "\n";
    
    {
        for (int i = 1; i < constant_pool.Length(); i++)
        {
            cout << "  " << i << "  ";
            constant_pool[i] -> Print(constant_pool);
            if (constant_pool[i] -> Tag() == CONSTANT_Long || constant_pool[i] -> Tag() == CONSTANT_Double)
                i++; // skip the next entry for eight-byte constants
        }
    }

    cout << "  interfaces " << interfaces.Length() <<": ";
    {
        for (int i = 0; i < interfaces.Length(); i++)
             cout << "  " << (int) interfaces[i];
        cout <<"\n";
    }

    cout << "  fields " << fields.Length() <<": ";
    {
        for (int i = 0; i < fields.Length(); i++)
        {
            cout << "field " << i << "\n";
            fields[i].Print(constant_pool);
        }
    }

    cout << " methods length " << methods.Length() << "\n";
    {
        for (int i = 0; i < methods.Length(); i++)
        {
            cout << "method " << i << "\n";
            methods[i].Print(constant_pool);
        }
    }

    cout << " attributes length " << attributes.Length() << "\n";
    {
        for (int i = 0; i < attributes.Length(); i++)
        {
            cout << "attribute " << i << "\n";
            attributes[i] -> Print(constant_pool);
        }
    }
    cout << "\n";

    return;
} 
#endif
