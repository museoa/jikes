// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, 1999, 2000, 2001 International Business
// Machines Corporation and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#include "bytecode.h"
#include "ast.h"
#include "class.h"
#include "control.h"
#include "semantic.h"
#include "stream.h"
#include "symbol.h"
#include "table.h"

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif

void ByteCode::CompileClass()
{
    AstClassDeclaration *class_decl = unit_type -> declaration -> ClassDeclarationCast();
    AstClassBody *class_body = (class_decl ? class_decl -> class_body
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
                AstVariableDeclarator *variable_declarator = field_decl -> VariableDeclarator(vi);
                VariableSymbol *vsym = variable_declarator -> symbol;
                DeclareField(vsym);

                //
                // We need a static initializer if we encounter at least one
                // class variable that is not a constant. Remember, according
                // to JLS 15.28, a constant is a final field initialized with
                // a constant expression.
                //
                if (variable_declarator -> variable_initializer_opt)
                {
                    if (vsym -> initial_value)
                    {
                        AstExpression *init;
                        assert(init = variable_declarator -> variable_initializer_opt -> ExpressionCast());
                        assert(init -> IsConstant() &&
                               variable_declarator -> symbol -> ACC_FINAL());
                    }
                    else
                        initialized_static_fields.Next() = variable_declarator;
                }
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
                // We must initialize all instance fields, even the constants.
                //
                if (vd -> variable_initializer_opt)
                    initialized_instance_fields.Next() = vd;
            }
        }
    }

    //
    // Supply needed field declaration for this$0 and any additional local
    // shadow parameters.
    //
    {
        for (int i = 0; i < unit_type -> NumConstructorParameters(); i++)
            DeclareField(unit_type -> ConstructorParameter(i));
    }

    //
    // Supply needed field declaration for enclosing instances (this$n, n > 0)
    // if present.
    //
    {
        for (int i = 1; i < unit_type -> NumEnclosingInstances(); i++)
            DeclareField(unit_type -> EnclosingInstance(i));
    }

    //
    // Supply needed field declarations for "class" identifiers (used for
    // X.class literals) if present.
    //
    {
        for (int i = 0; i < unit_type -> NumClassLiterals(); i++)
            DeclareField(unit_type -> ClassLiteral(i));
    }

    //
    // Supply needed field declaration for "$noassert" flag (used in assert
    // statements if present).
    //
    VariableSymbol *assert_variable = unit_type -> AssertVariable();
    if (assert_variable)
        DeclareField(assert_variable);

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
    // compile any private access methods
    //
    {
        for (int i = 0; i < unit_type -> NumPrivateAccessMethods(); i++)
        {
            int method_index = methods.NextIndex(); // index for method

            MethodSymbol *method_sym = unit_type -> PrivateAccessMethod(i);
            AstMethodDeclaration *method = (method_sym -> method_or_constructor_declaration
                                            ? method_sym -> method_or_constructor_declaration -> MethodDeclarationCast()
                                            : (AstMethodDeclaration *) NULL);
            BeginMethod(method_index, method_sym);
            if (method)
                EmitStatement(method -> method_body);
            else
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
        // Generate the class$() method used for class literal initialization.
        //
        int method_index = methods.NextIndex(); // index for method
        BeginMethod(method_index, class_literal_sym);
        GenerateClassAccessMethod(class_literal_sym);
        EndMethod(method_index, class_literal_sym);
    }

    //
    //
    // Compile the instance initializer blocks.
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
    // Compile generated constructors.
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

            assert(constructor_block -> explicit_constructor_invocation_opt);

            EmitStatement((AstStatement *) constructor_block -> explicit_constructor_invocation_opt);

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
                    PutU2(RegisterFieldref(this0_parameter));
                }

                if (class_body -> this_block)
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
                    // generate a call to the parameterless function
                    // block_initializer_function
                    //
                    PutOp(OP_ALOAD_0); // load address of object on which method is to be invoked
                    PutOp(OP_INVOKESPECIAL);
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

            EmitStatement(constructor_block -> block);

            EndMethod(method_index, local_constructor_symbol);
        }
    }

    //
    // If we need to generate a static initializer...
    //
    if (unit_type -> static_initializer_method)
    {
        assert(class_body -> NumStaticInitializers() > 0 ||
               initialized_static_fields.Length() > 0 ||
               assert_variable);

        int method_index = methods.NextIndex(); // index for method
        BeginMethod(method_index, unit_type -> static_initializer_method);

        if (assert_variable)
            GenerateAssertVariableInitializer(unit_type -> outermost_type, assert_variable);

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
    else assert(class_body -> NumStaticInitializers() == 0 &&
                initialized_static_fields.Length() == 0 &&
                ! assert_variable);

    FinishCode(unit_type);

    if (constant_pool.Length() > 65535)
    {
         semantic.ReportSemError(SemanticError::CONSTANT_POOL_OVERFLOW,
                                 unit_type -> declaration -> LeftToken(),
                                 unit_type -> declaration -> RightToken(),
                                 unit_type -> ContainingPackage() -> PackageName(),
                                 unit_type -> ExternalName());
    }

    if (interfaces.Length() > 65535)
    {
         AstClassDeclaration *class_declaration = unit_type -> declaration -> ClassDeclarationCast();
         AstInterfaceDeclaration *interface_declaration = unit_type -> declaration -> InterfaceDeclarationCast();
         int n = (class_declaration ? class_declaration -> NumInterfaces()
                                    : interface_declaration -> NumInterfaceMemberDeclarations());
         Ast *left = (class_declaration ? (Ast *) class_declaration -> Interface(0)
                                        : interface_declaration -> InterfaceMemberDeclaration(0)),
             *right = (class_declaration ? (Ast *) class_declaration -> Interface(n - 1)
                                         : interface_declaration -> InterfaceMemberDeclaration(n - 1));

         semantic.ReportSemError(SemanticError::INTERFACES_OVERFLOW,
                                 left -> LeftToken(),
                                 right -> RightToken(),
                                 unit_type -> ContainingPackage() -> PackageName(),
                                 unit_type -> ExternalName());
    }

    if (fields.Length() > 65535)
    {
         semantic.ReportSemError(SemanticError::FIELDS_OVERFLOW,
                                 unit_type -> declaration -> LeftToken(),
                                 unit_type -> declaration -> RightToken(),
                                 unit_type -> ContainingPackage() -> PackageName(),
                                 unit_type -> ExternalName());
    }

    if (methods.Length() > 65535)
    {
         semantic.ReportSemError(SemanticError::METHODS_OVERFLOW,
                                 unit_type -> declaration -> LeftToken(),
                                 unit_type -> declaration -> RightToken(),
                                 unit_type -> ContainingPackage() -> PackageName(),
                                 unit_type -> ExternalName());
    }

    if (string_overflow)
    {
         semantic.ReportSemError(SemanticError::STRING_OVERFLOW,
                                 unit_type -> declaration -> LeftToken(),
                                 unit_type -> declaration -> RightToken(),
                                 unit_type -> ContainingPackage() -> PackageName(),
                                 unit_type -> ExternalName());
    }

    if (library_method_not_found)
    {
         semantic.ReportSemError(SemanticError::LIBRARY_METHOD_NOT_FOUND,
                                 unit_type -> declaration -> LeftToken(),
                                 unit_type -> declaration -> RightToken(),
                                 unit_type -> ContainingPackage() -> PackageName(),
                                 unit_type -> ExternalName());
    }

    if (semantic.NumErrors() == 0)
        Write();
#ifdef JIKES_DEBUG
    if (control.option.debug_dump_class)
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
                AstVariableDeclarator *variable_declarator = field_decl -> VariableDeclarator(vi);
                VariableSymbol *vsym = variable_declarator -> symbol;
                DeclareField(vsym);

                //
                // We need a static constructor-initializer if we encounter at
                // least one variable (all variable declared in an interface
                // are implicitly static) that is declared with an
                // initialization expression that is not a constant expression.
                //
                assert(variable_declarator -> variable_initializer_opt);
                if (vsym -> initial_value)
                {
                    AstExpression *init;
                    assert(init = variable_declarator -> variable_initializer_opt -> ExpressionCast());
                    assert(init -> IsConstant());
                }
                else
                    initialized_fields.Next() = variable_declarator;
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
            {
                int method_index = methods.NextIndex();

                BeginMethod(method_index, method -> method_symbol);
                EndMethod(method_index, method -> method_symbol);
            }
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

        for (int i = 0; i < initialized_fields.Length(); i++)
            InitializeClassVariable(initialized_fields[i]);

        PutOp(OP_RETURN);
        EndMethod(method_index, unit_type -> static_initializer_method);
    }
    else assert(initialized_fields.Length() == 0);

    FinishCode(unit_type);

    if (constant_pool.Length() > 65535)
    {
         semantic.ReportSemError(SemanticError::CONSTANT_POOL_OVERFLOW,
                                 unit_type -> declaration -> LeftToken(),
                                 unit_type -> declaration -> RightToken(),
                                 unit_type -> ContainingPackage() -> PackageName(),
                                 unit_type -> ExternalName());
    }

    if (semantic.NumErrors() == 0)
        Write();
#ifdef JIKES_DEBUG
    if (control.option.debug_dump_class)
        PrintCode();
#endif
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

    AstConstructorBlock *constructor_block = constructor -> constructor_body -> ConstructorBlockCast();
    if (constructor_block -> explicit_constructor_invocation_opt)
        EmitStatement((AstStatement *) constructor_block -> explicit_constructor_invocation_opt);
    else
    {
        assert(unit_type == control.Object() && "A constructor block without an explicit constructor_invocation");
    }

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
            PutU2(RegisterFieldref(this0_parameter));
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
            PutOp(OP_INVOKESPECIAL);
            CompleteCall(type -> block_initializer_method, 0);
        }
    }

    EmitStatement(constructor_block -> block);

    EndMethod(method_index, method_symbol);
}


void ByteCode::DeclareField(VariableSymbol *symbol)
{
    int field_index = fields.NextIndex(); // index for field
    TypeSymbol *type = symbol -> Type();

    fields[field_index].SetFlags(symbol -> Flags());
    fields[field_index].SetNameIndex(RegisterUtf8(symbol -> ExternalIdentity() -> Utf8_literal));
    fields[field_index].SetDescriptorIndex(RegisterUtf8(type -> signature));

    //
    // Any final field initialized with a constant must have a ConstantValue
    // attribute.  However, the VM only reads this value for static fields.
    //
    if (symbol -> initial_value)
    {
        assert(symbol -> ACC_FINAL());
        assert(type -> Primitive() || type == control.String());
        u2 index = ((control.IsSimpleIntegerValueType(type) ||
                     type == control.boolean_type) ? RegisterInteger((IntLiteralValue *) symbol -> initial_value)
                    : type == control.String() ? RegisterString((Utf8LiteralValue *) symbol -> initial_value)
                    : type == control.float_type ? RegisterFloat((FloatLiteralValue *) symbol -> initial_value)
                    : type == control.long_type ? RegisterLong((LongLiteralValue *) symbol -> initial_value)
                    : RegisterDouble((DoubleLiteralValue *) symbol -> initial_value));
        u2 attribute_index = RegisterUtf8(control.ConstantValue_literal);
        fields[field_index].AddAttribute(new ConstantValue_attribute(attribute_index, index));
    }

    if (symbol -> IsSynthetic())
        fields[field_index].AddAttribute(CreateSyntheticAttribute());

    if (symbol -> IsDeprecated())
        fields[field_index].AddAttribute(CreateDeprecatedAttribute());
}


//
// Generate code for access method to private member of containing class
//
void ByteCode::GenerateAccessMethod(MethodSymbol *method_symbol)
{
    assert(method_symbol -> ACC_STATIC());

    // Here, we add a line-number attribute entry for this method.
    // Even though this is a generated method, JPDA debuggers will
    // still fail setting breakpoints if methods don't have line numbers.
    // Sun's javac compiler generates a single line number entry
    // with start_pc set to zero and line number set to the first line of
    // code in the source.  
    // In testing, it appears that setting the start_pc and line_number
    // to zero as we do here, also works.  
    line_number_table_attribute -> AddLineNumber(0, 0);


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
        if (method_sym -> ACC_STATIC())
            PutOp(OP_INVOKESTATIC);
        else
        {
            stack_words--;
            PutOp(method_sym -> ACC_PROTECTED() ? OP_INVOKEVIRTUAL
                  : OP_INVOKESPECIAL);
        }
        CompleteCall(method_sym, stack_words, unit_type);
    }
    else
    {
        VariableSymbol *field_sym = method_symbol -> accessed_member -> VariableCast();

        if (method_symbol -> Type() == control.void_type) // writing to a field
        {
            TypeSymbol *parameter_type = method_symbol -> FormalParameter(field_sym -> ACC_STATIC() ? 0 : 1) -> Type();
            PutOp(field_sym -> ACC_STATIC() ? OP_PUTSTATIC : OP_PUTFIELD);
            PutU2(RegisterFieldref(unit_type, field_sym));
            if (control.IsDoubleWordType(parameter_type))
                ChangeStack(-1);
        }
        else // reading a field: need method to retrieve value of field
        {
            PutOp(field_sym -> ACC_STATIC() ? OP_GETSTATIC : OP_GETFIELD);
            PutU2(RegisterFieldref(unit_type, field_sym));
            if (control.IsDoubleWordType(method_symbol -> Type()))
                ChangeStack(1);
        }
    }

    //
    // Method returns void, generate code for expression-less return statement.
    // Otherwise, call GenerateReturn to generate proper code.
    //
    if (method_symbol -> Type() == control.void_type)
         PutOp(OP_RETURN);
    else GenerateReturn(method_symbol -> Type());
}


void ByteCode::BeginMethod(int method_index, MethodSymbol *msym)
{
    assert(msym);

#ifdef DUMP
    if (control.option.g)
        Coutput << "(51) Generating code for method \"" << msym -> Name()
                << "\" in "
                << unit_type -> ContainingPackage() -> PackageName() << "/"
                << unit_type -> ExternalName() << endl;
#endif
    MethodInitialization();

    methods[method_index].SetNameIndex(RegisterUtf8(msym -> ExternalIdentity() -> Utf8_literal));
    methods[method_index].SetDescriptorIndex(RegisterUtf8(msym -> signature));
    methods[method_index].SetFlags(msym -> Flags());

    if (msym -> IsSynthetic())
        methods[method_index].AddAttribute(CreateSyntheticAttribute());

    if (msym -> IsDeprecated())
        methods[method_index].AddAttribute(CreateDeprecatedAttribute());

    //
    // Generate throws attribute if method throws any exceptions
    //
    if (msym -> NumThrows())
    {
        Exceptions_attribute *exceptions_attribute = new Exceptions_attribute(RegisterUtf8(control.Exceptions_literal));
        for (int i = 0; i < msym -> NumThrows(); i++)
            exceptions_attribute -> AddExceptionIndex(RegisterClass(msym -> Throws(i) -> fully_qualified_name));
        methods[method_index].AddAttribute(exceptions_attribute);
    }

    //
    // If the method is contained in an interface and it is not a generated
    // static initializer, no further processing ins needed
    //
    if (msym -> containing_type -> ACC_INTERFACE() &&
        msym -> Identity() != control.clinit_name_symbol)
    {
        return;
    }

    //
    // here if need code and associated attributes.
    //
    if (! (msym -> ACC_ABSTRACT() || msym -> ACC_NATIVE()))
    {
        method_stack = new MethodStack(msym -> max_block_depth, msym -> block_symbol -> max_variable_index);

        code_attribute = new Code_attribute(RegisterUtf8(control.Code_literal), msym -> block_symbol -> max_variable_index);

        line_number = 0;
        line_number_table_attribute = new LineNumberTable_attribute(RegisterUtf8(control.LineNumberTable_literal));

        local_variable_table_attribute = ((control.option.g & JikesOption::VARS)
                                          ? new LocalVariableTable_attribute(RegisterUtf8(control.LocalVariableTable_literal))
                                          : (LocalVariableTable_attribute *) NULL);
    }

    VariableSymbol *last_parameter = (msym -> NumFormalParameters() ? msym -> FormalParameter(msym -> NumFormalParameters() - 1)
                                                                    : (VariableSymbol *) NULL);

    last_parameter_index = (last_parameter ? last_parameter -> LocalVariableIndex() : -1);

    int num_parameter_slots = (last_parameter && control.IsDoubleWordType(last_parameter -> Type())
                                               ? last_parameter_index + 1
                                               : last_parameter_index);
    if (num_parameter_slots > 255)
    {
        assert(msym -> method_or_constructor_declaration);

        AstMethodDeclaration *method_declaration = msym -> method_or_constructor_declaration -> MethodDeclarationCast();
        AstConstructorDeclaration *constructor_declaration = msym -> method_or_constructor_declaration -> ConstructorDeclarationCast();
        AstMethodDeclarator *declarator = (method_declaration ? method_declaration -> method_declarator
                                                              : constructor_declaration -> constructor_declarator);

        semantic.ReportSemError(SemanticError::PARAMETER_OVERFLOW,
                                declarator -> left_parenthesis_token,
                                declarator -> right_parenthesis_token,
                                msym -> Header(),
                                unit_type -> ContainingPackage() -> PackageName(),
                                unit_type -> ExternalName());
    }
}


void ByteCode::EndMethod(int method_index, MethodSymbol *msym)
{
    assert(msym);

    if (! (msym -> ACC_ABSTRACT() || msym -> ACC_NATIVE()))
    {
        //
        // Make sure that no component in the code attribute exceeded its
        // limit.
        //
        if (msym -> block_symbol -> max_variable_index > 65535)
        {
            semantic.ReportSemError(SemanticError::LOCAL_VARIABLES_OVERFLOW,
                                    msym -> method_or_constructor_declaration -> LeftToken(),
                                    msym -> method_or_constructor_declaration -> RightToken(),
                                    msym -> Header(),
                                    unit_type -> ContainingPackage() -> PackageName(),
                                    unit_type -> ExternalName());
        }

        if (max_stack > 65535)
        {
            semantic.ReportSemError(SemanticError::STACK_OVERFLOW,
                                    msym -> method_or_constructor_declaration -> LeftToken(),
                                    msym -> method_or_constructor_declaration -> RightToken(),
                                    msym -> Header(),
                                    unit_type -> ContainingPackage() -> PackageName(),
                                    unit_type -> ExternalName());
        }

        if (code_attribute -> CodeLength() > 65535)
        {
            semantic.ReportSemError(SemanticError::CODE_OVERFLOW,
                                    msym -> method_or_constructor_declaration -> LeftToken(),
                                    msym -> method_or_constructor_declaration -> RightToken(),
                                    msym -> Header(),
                                    unit_type -> ContainingPackage() -> PackageName(),
                                    unit_type -> ExternalName());
        }

        //
        //
        //
        code_attribute -> SetMaxStack(max_stack);

        //
        // Sanity check - make sure nothing jumped past here
        //
        assert(last_label_pc < code_attribute -> CodeLength());

        //
        // attribute length:
        // Need to review how to make attribute_name and attribute_length.
        // Only write line number attribute if there are line numbers to
        // write, and -g:lines is enabled.
        //
        if ((control.option.g & JikesOption::LINES) &&
            line_number_table_attribute -> LineNumberTableLength() > 0)
        {
             code_attribute -> AddAttribute(line_number_table_attribute);
        }
        else delete line_number_table_attribute; // line_number_table_attribute not needed, so delete it now

        //
        // Debug level -g:vars & not dealing with generated accessed method
        //
        if ((control.option.g & JikesOption::VARS)
            && (! msym -> accessed_member)
            && (msym -> Identity() != control.class_name_symbol))
        {
            if (! msym -> ACC_STATIC()) // add 'this' to local variable table
            {
                local_variable_table_attribute -> AddLocalVariable(0,
                                                                   code_attribute -> CodeLength(),
                                                                   RegisterUtf8(control.this_literal),
                                                                   RegisterUtf8(msym -> containing_type -> signature),
                                                                   0);
            }

            //
            // For a normal constructor or method.
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
        else delete local_variable_table_attribute;

        methods[method_index].AddAttribute(code_attribute);

        delete method_stack;
    }
}


//
// This should only be called on non-constant static fields.
//
void ByteCode::InitializeClassVariable(AstVariableDeclarator *vd)
{
    assert(vd -> variable_initializer_opt);

    AstExpression *expression = vd -> variable_initializer_opt -> ExpressionCast();
    if (expression)
    {
        assert(vd -> symbol && ! vd -> symbol -> initial_value);

        //
        // Here, we add a line number attribute for this initializer for this
        // expression.  It seems that some debuggers (notably Sun's JDB) will
        // not allow setting breakpoints at a specific line of code if a class
        // contains initialized class variables but _no_ static initializer
        // block.  We now add a line number attribute to appease that debugger.
        //
        line_number_table_attribute -> AddLineNumber(code_attribute -> CodeLength(),
                                                     semantic.lex_stream -> Line(expression -> LeftToken()));

        EmitExpression(expression);
    }
    else
    {
        AstArrayInitializer *array_initializer = vd -> variable_initializer_opt -> ArrayInitializerCast();

        assert(array_initializer);

        //
        // Like the case above, we add a line number attribute for this
        // initializer. In this case, the initializer is an array. It seems
        // that some debuggers (notably Sun's JDB) will not allow setting
        // breakpoints at a specific line of code if a class contains
        // initialized class variables but _no_ static initializer block.
        // We now add a line number attribute to appease that debugger.
        //
        line_number_table_attribute -> AddLineNumber(code_attribute -> CodeLength(),
                                                     semantic.lex_stream -> Line(array_initializer->LeftToken()));

        InitializeArray(vd -> symbol -> Type(), array_initializer);
    }

    PutOp(OP_PUTSTATIC);
    if (expression && control.IsDoubleWordType(expression -> Type()))
        ChangeStack(-1);
    PutU2(RegisterFieldref(vd -> symbol));
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
    if (expression && control.IsDoubleWordType(expression -> Type()))
        ChangeStack(-1);
    PutU2(RegisterFieldref(vd -> symbol));
}


void ByteCode::InitializeArray(TypeSymbol *type, AstArrayInitializer *array_initializer)
{
    TypeSymbol *subtype = type -> ArraySubtype();

    LoadImmediateInteger(array_initializer -> NumVariableInitializers());
    EmitNewArray(1, type); // make the array
    for (int i = 0; i < array_initializer -> NumVariableInitializers(); i++)
    {
        Ast *entry = array_initializer -> VariableInitializer(i);
        PutOp(OP_DUP);
        LoadImmediateInteger(i);
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
}


//
// Generate code for local variable declaration.
//
void ByteCode::DeclareLocalVariable(AstVariableDeclarator *declarator)
{
    if (control.option.g & JikesOption::VARS)
    {
#ifdef JIKES_DEBUG
        assert(method_stack -> StartPc(declarator -> symbol) == 0xFFFF); // must be uninitialized
#endif
#ifdef DUMP
        Coutput << "(53) Variable \"" << declarator -> symbol -> Name()
                << "\" numbered "
                << declarator -> symbol -> LocalVariableIndex()
                << " was processed" << endl;
#endif
        method_stack -> StartPc(declarator -> symbol) = code_attribute -> CodeLength();
    }

    if (declarator -> symbol -> initial_value)
    {
        //
        // Optimization: If we are not tracking local variable names, we do
        // not need to waste space on a constant as it is always inlined.
        //
        if (! (control.option.g & JikesOption::VARS))
            return;
        LoadLiteral(declarator -> symbol -> initial_value, declarator -> symbol -> Type());
    }
    else if (declarator -> variable_initializer_opt)
    {
        AstArrayCreationExpression *ace = declarator -> variable_initializer_opt -> ArrayCreationExpressionCast();
        if (ace)
            EmitArrayCreationExpression(ace);
        else if (declarator -> variable_initializer_opt -> ArrayInitializerCast())
            InitializeArray(declarator -> symbol -> Type(), declarator -> variable_initializer_opt -> ArrayInitializerCast());
        else // evaluation as expression
            EmitExpression(declarator -> variable_initializer_opt -> ExpressionCast());
    }
    else return; // if nothing to initialize

    StoreLocal(declarator -> symbol -> LocalVariableIndex(), declarator -> symbol -> Type());
}


//
// JLS Chapter 13: Blocks and Statements
//  Statements control the sequence of evaluation of Java programs,
//  are executed for their effects and do not have values.
//
// Processing of loops requires a loop stack, especially to handle
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
                                                     semantic.lex_stream -> Line(statement -> LeftToken()));
    }

    assert(stack_depth == 0); // stack empty at start of statement

    switch (statement -> kind)
    {
    case Ast::BLOCK: // JLS 14.2
        EmitBlockStatement((AstBlock *) statement);
        break;
    case Ast::LOCAL_VARIABLE_DECLARATION: // JLS 14.3
        {
            AstLocalVariableDeclarationStatement *lvds = statement -> LocalVariableDeclarationStatementCast();
            for (int i = 0; i < lvds -> NumVariableDeclarators(); i++)
                DeclareLocalVariable(lvds -> VariableDeclarator(i));
        }
        break;
    case Ast::EMPTY_STATEMENT: // JLS 14.5
        break;
    case Ast::EXPRESSION_STATEMENT: // JLS 14.7
        EmitStatementExpression(statement -> ExpressionStatementCast() -> expression);
        break;
    case Ast::IF: // JLS 14.8
        {
            AstIfStatement *if_statement = (AstIfStatement *) statement;
            if (if_statement -> expression -> IsConstant())
            {
                if (! IsZero(if_statement -> expression))
                    EmitStatement(if_statement -> true_statement);
                else if (if_statement -> false_statement_opt) // if there is false part
                    EmitStatement(if_statement -> false_statement_opt);
            }
            else if (if_statement -> false_statement_opt) // if true and false parts
            {
                Label label1,
                      label2;
                AstStatement *true_statement = if_statement -> true_statement;
                EmitBranchIfExpression(if_statement -> expression,
                                       false, label1, true_statement);
                assert(stack_depth == 0);

                EmitStatement(true_statement);
                if (true_statement -> can_complete_normally)
                    EmitBranch(OP_GOTO, label2,
                               if_statement -> false_statement_opt);

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
                EmitBranchIfExpression(if_statement -> expression,
                                       false, label1,
                                       if_statement -> true_statement);
                assert(stack_depth == 0);
                EmitStatement(if_statement -> true_statement);
                DefineLabel(label1);
                CompleteLabel(label1);
            }
        }
        break;
    case Ast::SWITCH: // JLS 14.9
        EmitSwitchStatement(statement -> SwitchStatementCast());
        break;
    case Ast::SWITCH_BLOCK: // JLS 14.9
    case Ast::CASE:
    case Ast::DEFAULT:
        //
        // These nodes are handled by SwitchStatement and
        // are not directly visited
        //
        assert(false && "faulty logic encountered");
        break;
    case Ast::WHILE: // JLS 14.10
        {
            AstWhileStatement *wp = statement -> WhileStatementCast();
            //
            // Branch to continuation test. This test is placed after the
            // body of the loop we can fall through into it after each
            // loop iteration without the need for an additional branch,
            // unless the loop body always completes abruptly.
            //
            if (! wp -> statement -> can_complete_normally)
            {
                if (wp -> expression -> IsConstant())
                {
                    // must be true, or internal statement would be
                    // unreachable
                    assert(semantic.IsConstantTrue(wp -> expression));
                }
                else
                {
                    line_number_table_attribute -> AddLineNumber(code_attribute -> CodeLength(),
                                                                 semantic.lex_stream -> Line(wp -> expression -> LeftToken()));
                    EmitExpression(wp -> expression); // for side effects only
                    PutOp(OP_POP);
                }
                EmitStatement(wp -> statement);
                DefineLabel(method_stack -> TopContinueLabel());
                assert(stack_depth == 0);
                CompleteLabel(method_stack -> TopContinueLabel());
                break;
            }
            if (wp -> expression -> IsConstant())
                // must be true, or internal statement would be
                // unreachable
                assert(semantic.IsConstantTrue(wp -> expression));
            else
                EmitBranch(OP_GOTO, method_stack -> TopContinueLabel(),
                           wp -> statement);
            Label begin_label;
            DefineLabel(begin_label);
            EmitStatement(wp -> statement);
            DefineLabel(method_stack -> TopContinueLabel());
            assert(stack_depth == 0);

            //
            // Reset the line number before evaluating the expression
            //
            line_number_table_attribute -> AddLineNumber(code_attribute -> CodeLength(),
                                                         semantic.lex_stream -> Line(wp -> expression -> LeftToken()));

            EmitBranchIfExpression(wp -> expression, true, begin_label,
                                   wp -> statement);
            CompleteLabel(begin_label);
            CompleteLabel(method_stack -> TopContinueLabel());
        }
        break;
    case Ast::DO: // JLS 14.11
        {
            AstDoStatement *sp = statement -> DoStatementCast();
            Label begin_label;
            DefineLabel(begin_label);
            EmitStatement(sp -> statement);
            DefineLabel(method_stack -> TopContinueLabel());
            assert(stack_depth == 0);

            if (sp -> statement -> can_complete_normally)
            {
                //
                // Reset the line number before evaluating the expression
                //
                line_number_table_attribute -> AddLineNumber(code_attribute -> CodeLength(),
                                                             semantic.lex_stream -> Line(sp -> expression -> LeftToken()));
                EmitBranchIfExpression(sp -> expression, true,
                                       begin_label, sp -> statement);
            }
            CompleteLabel(begin_label);
            CompleteLabel(method_stack -> TopContinueLabel());
        }
        break;
    case Ast::FOR: // JLS 14.12
        {
            AstForStatement *for_statement = statement -> ForStatementCast();
            for (int i = 0; i < for_statement -> NumForInitStatements(); i++)
                EmitStatement(for_statement -> ForInitStatement(i));
            Label begin_label,
                  test_label;
            //
            // The loop test is placed after the body, unless the body
            // always completes abruptly, to save an additional jump.
            //
            if (! for_statement -> statement -> can_complete_normally)
            {
                if (for_statement -> end_expression_opt)
                {
                    if (for_statement -> end_expression_opt -> IsConstant())
                    {
                        // must be true, or internal statement would be
                        // unreachable
                        assert(semantic.IsConstantTrue(for_statement -> end_expression_opt));
                    }
                    else
                    {
                        line_number_table_attribute -> AddLineNumber(code_attribute -> CodeLength(),
                                                                     semantic.lex_stream -> Line(for_statement -> end_expression_opt -> LeftToken()));
                        EmitExpression(for_statement -> end_expression_opt); // for side effects only
                        PutOp(OP_POP);
                    }
                }
                EmitStatement(for_statement -> statement);
                DefineLabel(method_stack -> TopContinueLabel());
                assert(stack_depth == 0);
                CompleteLabel(method_stack -> TopContinueLabel());
                break;
            }
            if (for_statement -> end_expression_opt &&
                ! for_statement -> end_expression_opt -> IsConstant())
            {
                EmitBranch(OP_GOTO, test_label,
                           for_statement -> statement);
            }
            DefineLabel(begin_label);
            EmitStatement(for_statement -> statement);
            DefineLabel(method_stack -> TopContinueLabel());
            for (int j = 0; j < for_statement -> NumForUpdateStatements(); j++)
                EmitStatement(for_statement -> ForUpdateStatement(j));
            DefineLabel(test_label);

            AstExpression *end_expr = for_statement -> end_expression_opt;
            if (end_expr)
            {
                assert(stack_depth == 0);

                //
                // Reset the line number before evaluating the expression
                //
                line_number_table_attribute -> AddLineNumber(code_attribute -> CodeLength(),
                                                             semantic.lex_stream -> Line(end_expr -> LeftToken()));

                EmitBranchIfExpression(end_expr, true, begin_label,
                                       for_statement -> statement);
            }
            else EmitBranch(OP_GOTO, begin_label,
                            for_statement -> statement);

            CompleteLabel(begin_label);
            CompleteLabel(test_label);
            CompleteLabel(method_stack -> TopContinueLabel());
        }
        break;
    case Ast::BREAK: // JLS 14.13
        ProcessAbruptExit(statement -> BreakStatementCast() -> nesting_level);
        EmitBranch(OP_GOTO, method_stack -> BreakLabel(statement -> BreakStatementCast() -> nesting_level));
        break;
    case Ast::CONTINUE: // JLS 14.14
        ProcessAbruptExit(statement -> ContinueStatementCast() -> nesting_level);
        EmitBranch(OP_GOTO, method_stack -> ContinueLabel(statement -> ContinueStatementCast() -> nesting_level));
        break;
    case Ast::RETURN: // JLS 14.15
        EmitReturnStatement(statement -> ReturnStatementCast());
        break;
    case Ast::SUPER_CALL:
        EmitSuperInvocation((AstSuperCall *) statement);
        break;
    case Ast::THIS_CALL:
        EmitThisInvocation((AstThisCall *) statement);
        break;
    case Ast::THROW: // JLS 14.16
        EmitExpression(statement -> ThrowStatementCast() -> expression);
        PutOp(OP_ATHROW);
        break;
    case Ast::SYNCHRONIZED_STATEMENT: // JLS 14.17
        EmitSynchronizedStatement((AstSynchronizedStatement *) statement);
        break;
    case Ast::TRY: // JLS 14.18
        EmitTryStatement((AstTryStatement *) statement);
        break;
    case Ast::ASSERT: // JDK 1.4 (JSR 41)
        EmitAssertStatement((AstAssertStatement *) statement);
        break;
    case Ast::CLASS: // Class Declaration
    case Ast::INTERFACE: // InterfaceDeclaration
        //
        // these are factored out by the front end; and so must be
        // skipped here
        //
        break;
    case Ast::CATCH:   // JLS 14.18
    case Ast::FINALLY: // JLS 14.18
        // handled by TryStatement
    default:
        assert(false && "unknown statement kind");
        break;
    }
}


void ByteCode::EmitReturnStatement(AstReturnStatement *statement)
{
    AstExpression *expression = statement -> expression_opt;

    if (! expression)
    {
        ProcessAbruptExit(method_stack -> NestingLevel(0));
        PutOp(OP_RETURN);
    }
    else
    {
        TypeSymbol *type = expression -> Type();
        assert(type != control.void_type);

        EmitExpression(expression);

        ProcessAbruptExit(method_stack -> NestingLevel(0), type);

        GenerateReturn(type);
    }
}


void ByteCode::EmitBlockStatement(AstBlock *block)
{
    assert(stack_depth == 0); // stack empty at start of statement

    method_stack -> Push(block);

    for (int i = 0; i < block -> NumStatements(); i++)
        EmitStatement((AstStatement *) block -> Statement(i));

    //
    // Always define LABEL_BREAK at this point, and complete its definition.
    //
    if (IsLabelUsed(method_stack -> TopBreakLabel())) // need define only if used
        DefineLabel(method_stack -> TopBreakLabel());
    CompleteLabel(method_stack -> TopBreakLabel());

    if (control.option.g & JikesOption::VARS)
    {
        for (int i = 0; i < block -> NumLocallyDefinedVariables(); i++)
        {
            VariableSymbol *variable = block -> LocallyDefinedVariable(i);
#ifdef JIKES_DEBUG
            assert(method_stack -> StartPc(variable) != 0xFFFF);
#endif
#ifdef DUMP
            Coutput << "(56) The symbol \"" << variable -> Name()
                    << "\" numbered " << variable -> LocalVariableIndex()
                    << " was released" << endl;
#endif
            local_variable_table_attribute -> AddLocalVariable(method_stack -> StartPc(variable),
                                                               code_attribute -> CodeLength(),
                                                               RegisterUtf8(variable -> ExternalIdentity() -> Utf8_literal),
                                                               RegisterUtf8(variable -> Type() -> signature),
                                                               variable -> LocalVariableIndex());
        }
    }

    method_stack -> Pop();
}


void ByteCode::EmitStatementExpression(AstExpression *expression)
{
    switch (expression -> kind)
    {
    case Ast::CALL:
        {
            AstMethodInvocation *method_call = (AstMethodInvocation *) expression;
            EmitMethodInvocation(method_call);
            if (method_call -> Type() != control.void_type)
                PutOp(control.IsDoubleWordType(method_call -> Type())
                      ? OP_POP2 : OP_POP); // discard value
        }
        break;
    case Ast::POST_UNARY:
        EmitPostUnaryExpression((AstPostUnaryExpression *) expression, false);
        break;
    case Ast::PRE_UNARY:
        EmitPreUnaryExpression((AstPreUnaryExpression *) expression, false);
        break;
    case Ast::ASSIGNMENT:
        EmitAssignmentExpression((AstAssignmentExpression *) expression, false);
        break;
    case Ast::CLASS_CREATION:
        EmitClassInstanceCreationExpression((AstClassInstanceCreationExpression *) expression, false);
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
void ByteCode::EmitSwitchStatement(AstSwitchStatement *switch_statement)
{
    AstBlock *switch_block = switch_statement -> switch_block;

    assert(stack_depth == 0); // stack empty at start of statement

    //
    // Use tableswitch if have exact match or size of tableswitch
    // case is no more than 30 bytes more code than lookup case
    //
    bool use_lookup = true; // set if using LOOKUPSWITCH opcode
    int ncases = switch_statement -> NumCases(),
        nlabels = ncases,
        high = 0,
        low = 0;
    if (ncases > 0)
    {
        low = switch_statement -> Case(0) -> Value();
        high = switch_statement -> Case(ncases - 1) -> Value();

        // Workaround for Sun JVM TABLESWITCH bug in JDK 1.2, 1.3
        // when case values of 0x7ffffff0 through 0x7fffffff are used.
        // Force the generation of a LOOKUPSWITCH in these circumstances.

        assert(low <= high);

        if ((unsigned long)high < 0x7ffffff0UL)
        { 
            // want to compute
            //  (2 + high-low + 1) < (1 + ncases * 2 + 30)
            // but must guard against overflow, so factor out
            //  high - low < ncases * 2 + 28
            // but can't have number of labels < number of cases

            LongInt range = LongInt(high) - low + 1;
            if (range < (ncases * 2 + 28))
            {
                use_lookup = false; // use tableswitch
                nlabels = range.LowWord();
    
                assert(range.HighWord() == 0);
                assert(nlabels >= ncases);
            }
        } 
    }

    //
    // Set up the environment for the switch block.  This must be done before
    // emitting the expression, in case the expression is an assignment.
    //
    method_stack -> Push(switch_block);

    //
    // Reset the line number before evaluating the expression
    //
    line_number_table_attribute -> AddLineNumber(code_attribute -> CodeLength(),
                                                 semantic.lex_stream -> Line(switch_statement -> expression -> LeftToken()));
    EmitExpression(switch_statement -> expression);

    PutOp(use_lookup ? OP_LOOKUPSWITCH : OP_TABLESWITCH);
    int op_start = last_op_pc; // pc at start of instruction

    //
    // supply any needed padding
    //
    while (code_attribute -> CodeLength() % 4 != 0)
        PutNop(0);

    //
    // Note that if no default clause in switch statement, must allocate
    // one that corresponds to do nothing and branches to start of next
    // statement.
    //
    Label *case_labels = new Label[(use_lookup ? ncases : nlabels) + 1],
          default_label;
    UseLabel((switch_statement -> default_case.switch_block_statement
              ? default_label : method_stack -> TopBreakLabel()),
             4,
             code_attribute -> CodeLength() - op_start);

    //
    //
    //
    if (use_lookup)
    {
        PutU4(ncases);

        for (int i = 0; i < ncases; i++)
        {
            PutU4(switch_statement -> Case(i) -> Value());
            UseLabel(case_labels[switch_statement -> Case(i) -> index],
                     4, code_attribute -> CodeLength() - op_start);
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
        for (int j = 0; j < switch_block -> NumStatements(); j++)
        {
            AstSwitchBlockStatement *switch_block_statement = (AstSwitchBlockStatement *) switch_block -> Statement(j);

            //
            // process labels for this block
            //
            for (int li = 0; li < switch_block_statement -> NumSwitchLabels(); li++)
            {
                AstCaseLabel *case_label = switch_block_statement -> SwitchLabel(li) -> CaseLabelCast();
                if (case_label)
                {
                    int label_index = switch_statement -> Case(case_label -> map_index) -> Value() - low;
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
                     : (switch_statement -> default_case.switch_block_statement
                        ? default_label : method_stack -> TopBreakLabel()),
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
    for (int i = 0; i < switch_block -> NumStatements(); i++)
    {
        AstSwitchBlockStatement *switch_block_statement = (AstSwitchBlockStatement *) switch_block -> Statement(i);

        //
        // process labels for this block
        //
        for (int li = 0; li < switch_block_statement -> NumSwitchLabels(); li++)
        {
            AstCaseLabel *case_label = switch_block_statement -> SwitchLabel(li) -> CaseLabelCast();
            if (case_label)
            {
                int map_index = case_label -> map_index;

                if (use_lookup)
                    DefineLabel(case_labels[map_index]);
                else
                {
                    //
                    // TODO: Do this more efficiently ??? !!!
                    //
                    for (int di = 0; di < switch_statement -> NumCases(); di++)
                    {
                        if (switch_statement -> Case(di) -> index == map_index)
                        {
                            int ci = switch_statement -> Case(di) -> Value() - low;
                            DefineLabel(case_labels[ci]);
                            break;
                        }
                    }
                }
            }
            else
            {
                assert(switch_block_statement -> SwitchLabel(li) -> DefaultLabelCast());
                assert(switch_statement -> default_case.switch_block_statement);

                DefineLabel(default_label);
            }
        }

        //
        // compile code for this case
        //
        for (int si = 0; si < switch_block_statement -> NumStatements(); si++)
            EmitStatement(switch_block_statement -> Statement(si) -> StatementCast());
    }

    //
    // Close the range of the locally defined variables.
    //
    if (control.option.g & JikesOption::VARS)
    {
        for (int i = 0; i < switch_block -> NumLocallyDefinedVariables(); i++)
        {
            VariableSymbol *variable = switch_block -> LocallyDefinedVariable(i);
            if (method_stack -> StartPc(variable) > op_start)
            {
#ifdef JIKES_DEBUG
                assert(method_stack -> StartPc(variable) != 0xFFFF);
#endif
#ifdef DUMP
                Coutput << "(58) The symbol \"" << variable -> Name()
                        << "\" numbered " << variable -> LocalVariableIndex()
                        << " was released" << endl;
#endif
                local_variable_table_attribute -> AddLocalVariable(method_stack -> StartPc(variable),
                                                                   code_attribute -> CodeLength(),
                                                                   RegisterUtf8(variable -> ExternalIdentity() -> Utf8_literal),
                                                                   RegisterUtf8(variable -> Type() -> signature),
                                                                   variable -> LocalVariableIndex());
            }
        }
    }

    //
    //
    //
    for (int j = 0; j < nlabels; j++)
    {
        if ((case_labels[j].uses.Length() > 0) && (! case_labels[j].defined))
        {
            case_labels[j].defined = true;
            case_labels[j].definition = (switch_statement -> default_case.switch_block_statement
                                         ? default_label.definition
                                         : method_stack -> TopBreakLabel().definition);
        }

        CompleteLabel(case_labels[j]);
    }

    //
    // If the switch statement contains a default case, we clean up
    // the default label here.
    //
    if (switch_statement -> default_case.switch_block_statement)
        CompleteLabel(default_label);

    //
    // If this switch statement can be "broken", we define the break label
    // here.
    //
    if (IsLabelUsed(method_stack -> TopBreakLabel())) // need define only if used
    {
        DefineLabel(method_stack -> TopBreakLabel());
        CompleteLabel(method_stack -> TopBreakLabel());
    }

    delete [] case_labels;

    method_stack -> Pop();
}


//
//  13.18       The try statement
//
void ByteCode::EmitTryStatement(AstTryStatement *statement)
{
    //
    // If the finally label in the surrounding block is used by a try
    // statement, it is cleared after the finally block associated with the
    // try statement has been processed.
    //
    assert(method_stack -> TopFinallyLabel().uses.Length() == 0);
    assert(method_stack -> TopFinallyLabel().defined == false);
    assert(method_stack -> TopFinallyLabel().definition == 0);

    int start_try_block_pc = code_attribute -> CodeLength(); // start pc

    EmitBlockStatement(statement -> block);

    //
    // The computation of end_try_block_pc, the instruction following the last
    // instruction in the body of the try block, does not include the code, if
    // any, needed to call a finally block or skip to the end of the try
    // statement.
    //
    int end_try_block_pc = code_attribute -> CodeLength(),
        special_end_pc = end_try_block_pc; // end_pc for "special" handler

    Label &finally_label = method_stack -> TopFinallyLabel(), // use the label in the block immediately enclosing try statement.
          end_label;
    if (statement -> block -> can_complete_normally)
    {
        if (statement -> finally_clause_opt)
        {
            //
            // Call finally block if have finally handler.
            //
            EmitBranch(OP_JSR, finally_label, statement);
        }

        EmitBranch(OP_GOTO, end_label);
    }

    //
    // Process catch clauses, but only if try block is not empty.
    //
    if (start_try_block_pc != end_try_block_pc)
    {
        for (int i = 0; i < statement -> NumCatchClauses(); i++)
        {
            int handler_pc = code_attribute -> CodeLength();

            AstCatchClause *catch_clause = statement -> CatchClause(i);
            VariableSymbol *parameter_symbol = catch_clause -> parameter_symbol;

            assert(stack_depth == 0);
            stack_depth = 1; // account for the exception already on the stack
            StoreLocal(parameter_symbol -> LocalVariableIndex(), parameter_symbol -> Type());

            EmitBlockStatement(catch_clause -> block);

            if (control.option.g & JikesOption::VARS)
            {
                local_variable_table_attribute -> AddLocalVariable(handler_pc,
                                                                   code_attribute -> CodeLength(),
                                                                   RegisterUtf8(parameter_symbol -> ExternalIdentity() -> Utf8_literal),
                                                                   RegisterUtf8(parameter_symbol -> Type() -> signature),
                                                                   parameter_symbol -> LocalVariableIndex());
            }

            code_attribute -> AddException(start_try_block_pc,
                                           end_try_block_pc,
                                           handler_pc,
                                           RegisterClass(parameter_symbol -> Type() -> fully_qualified_name));

            special_end_pc = code_attribute -> CodeLength();

            if (catch_clause -> block -> can_complete_normally)
            {
                if (statement -> finally_clause_opt)
                {
                    //
                    // Call finally block if have finally handler.
                    //
                    EmitBranch(OP_JSR, finally_label, statement);
                }

                //
                // If there are more catch clauses, or a finally clause, then
                // emit branch to skip over their code and on to the next
                // statement.
                //
                if (statement -> finally_clause_opt || i < (statement -> NumCatchClauses() - 1))
                    EmitBranch(OP_GOTO, end_label);
            }
        }
    }

    //
    // If this try statement contains a finally clause, then ...
    //
    if (statement -> finally_clause_opt)
    {
        int variable_index = method_stack -> TopBlock() -> block_symbol -> try_or_synchronized_variable_index;

        //
        // Emit code for "special" handler to make sure finally clause is
        // invoked in case an otherwise uncaught exception is thrown in the
        // try block, or an exception is thrown from within a catch block.
        //
        // No special handler is needed if the try block is empty.
        if (start_try_block_pc != end_try_block_pc) // If try-block not empty
        {
            code_attribute -> AddException(start_try_block_pc,
                                           special_end_pc,
                                           code_attribute -> CodeLength(),
                                           0);
            assert(stack_depth == 0);
            stack_depth = 1; // account for the exception already on stack
            StoreLocal(variable_index, control.Object()); // Save exception
            EmitBranch(OP_JSR, finally_label, statement);
            LoadLocal(variable_index, control.Object()); // Reload exception,
            PutOp(OP_ATHROW); // and rethrow it.
        }

        //
        // Generate code for finally clause.
        //
        DefineLabel(finally_label);
        CompleteLabel(finally_label);

        //
        // If the finally block can complete normally, save the return address.
        // Otherwise, we pop the return address from the stack.
        //
        assert(stack_depth == 0);
        stack_depth = 1; // account for the return location already on stack
        if (statement -> finally_clause_opt -> block -> can_complete_normally)
             StoreLocal(variable_index + 1, control.Object());
        else PutOp(OP_POP);

        EmitBlockStatement(statement -> finally_clause_opt -> block);

        //
        // If a finally block can complete normally, after executing itsbody,
        // we return to the caller using the return address saved earlier.
        //
        if (statement -> finally_clause_opt -> block -> can_complete_normally)
            PutOpWide(OP_RET, variable_index + 1);
    }

    if (IsLabelUsed(end_label))
        DefineLabel(end_label);
    CompleteLabel(end_label);
}


//
// Exit to block at level lev, freeing monitor locks and invoking finally
// clauses as appropriate
//
void ByteCode::ProcessAbruptExit(int to_lev, TypeSymbol *return_type)
{
    for (int i = method_stack -> Size() - 1; i > 0 && method_stack -> NestingLevel(i) != to_lev; i--)
    {
        int nesting_level = method_stack -> NestingLevel(i),
            enclosing_level = method_stack -> NestingLevel(i - 1);
        AstBlock *block = method_stack -> Block(nesting_level);
        if (block -> block_tag == AstBlock::TRY_CLAUSE_WITH_FINALLY)
        {
            if (return_type)
            {
                Label &finally_label = method_stack -> FinallyLabel(enclosing_level);
                int variable_index = method_stack -> Block(enclosing_level) -> block_symbol -> try_or_synchronized_variable_index + 2;

                StoreLocal(variable_index, return_type);

                EmitBranch(OP_JSR, finally_label, 
                           method_stack -> Block(enclosing_level));

                LoadLocal(variable_index, return_type);
            }
            else
            {
                EmitBranch(OP_JSR, 
                           method_stack -> FinallyLabel(enclosing_level),
                           method_stack -> Block(enclosing_level));
            }
        }
        else if (block -> block_tag == AstBlock::SYNCHRONIZED)
        {
            int variable_index = method_stack -> Block(enclosing_level) ->
                block_symbol -> try_or_synchronized_variable_index;
            
            LoadLocal(variable_index, control.Object());
            PutOp(OP_MONITOREXIT);
        }
    }
}

void ByteCode::EmitBranch(unsigned int opc, Label& lab, AstStatement *over)
{
    // Use the number of tokens as a heuristic for the size of the statement
    // we're jumping over. If the statement is large enough, either change
    // to the 4-byte branch opcode or write out a branch around a goto_w for
    // branch opcodes that don't have a long form.
    int sizeHeuristic = ! over ? 0 : over -> RightToken() - over -> LeftToken();
    if (sizeHeuristic < TOKEN_WIDTH_REQUIRING_GOTOW) {
        EmitBranch(opc, lab);
        return;
    }
    if (opc == OP_GOTO) {
        PutOp(OP_GOTO_W);
        UseLabel(lab, 4, 1);
        return;
    }
    if (opc == OP_JSR) {
        PutOp(OP_JSR_W);
        UseLabel(lab, 4, 1);
        return;
    }
    // if op lab
    //  =>
    // if !op label2
    // goto_w lab
    // label2:
    PutOp(InvertIfOpCode(opc));
    Label label2;
    UseLabel(label2, 2, 1);
    PutOp(OP_GOTO_W);
    UseLabel(lab, 4, 1);
    DefineLabel(label2);
    CompleteLabel(label2);
}

//
// java provides a variety of conditional branch instructions, so
// that a number of operators merit special handling:
//      constant operand
//      negation (we eliminate it)
//      equality
//      ?: && and || (partial evaluation)
//      comparisons
// Other expressions are just evaluated and the appropriate
// branch emitted.
//
// TODO: return a bool that is true if the statement being branched over is
// even needed (if statements and other places might have a constant false
// expression, allowing the next block of code to be skipped entirely).
//
void ByteCode::EmitBranchIfExpression(AstExpression *p, bool cond, Label &lab,
                                      AstStatement *over)
{
    p = StripNops(p);

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

        EmitBranchIfExpression(pre -> expression, (! cond), lab, over);
        return;
    }

    AstConditionalExpression *conditional = p -> ConditionalExpressionCast();
    if (conditional)
    {
        //
        // branch_if(true?a:b, cond, lab) =>
        // branch_if(a, cond, lab);
        // branch_if(false?a:b, cond, lab) =>
        // branch_if(b, cond, lab);
        //
        if (conditional -> test_expression -> IsConstant())
        {
            if (IsZero(conditional -> test_expression))
                EmitBranchIfExpression(conditional -> false_expression,
                                       cond, lab, over);
            else
                EmitBranchIfExpression(conditional -> true_expression,
                                       cond, lab, over);
            return;
        }
    }


    //
    // dispose of non-binary expression case by just evaluating
    // operand and emitting appropiate test.
    //
    AstBinaryExpression *bp = p -> BinaryExpressionCast();
    if (! bp)
    {
        EmitExpression(p);
        EmitBranch((cond ? OP_IFNE : OP_IFEQ), lab, over);
        return;
    }

    //
    // Here if binary expression, so extract operands
    //
    AstExpression *left = StripNops(bp -> left_expression);
    AstExpression *right = StripNops(bp -> right_expression);

    TypeSymbol *left_type = left -> Type(),
               *right_type = right -> Type();
    switch (bp -> binary_tag)
    {
    case AstBinaryExpression::INSTANCEOF:
        {
            EmitExpression(left);
            PutOp(OP_INSTANCEOF);
            TypeSymbol *instanceof_type = bp -> right_expression -> Type();
            PutU2(instanceof_type -> num_dimensions > 0
                  ? RegisterClass(instanceof_type -> signature)
                  : RegisterClass(instanceof_type -> fully_qualified_name));

            EmitBranch((cond ? OP_IFNE : OP_IFEQ), lab, over);
        }
        return;
    case AstBinaryExpression::AND_AND:
        //
        // branch_if(true&&b, cond, lab) =>
        // branch_if(b, cond, lab);
        // branch_if(false&&b, cond, lab) =>
        // branch_if(false, cond, lab);
        //
        if (left -> IsConstant())
        {
            if (! IsZero(left))
                EmitBranchIfExpression(right, cond, lab, over);
            else if (! cond)
                EmitBranch(OP_GOTO, lab, over);
        }
        //
        // branch_if(a&&true, cond, lab) =>
        // branch_if(a, cond, lab);
        // branch_if(a&&false, cond, lab) =>
        // emit(a), pop; for side effects
        //
        else if (right -> IsConstant())
        {
            if (! IsZero(right))
                EmitBranchIfExpression(left, cond, lab, over);
            else
            {
                EmitExpression(left);
                PutOp(OP_POP);
                if (! cond)
                    EmitBranch(OP_GOTO, lab, over);
            }
        }
        //
        // branch_if(a&&b, true, lab) =>
        // branch_if(a,false,skip);
        // branch_if(b,true,lab);
        // skip:
        // branch_if(a&&b, false, lab) =>
        // branch_if(a,false,lab);
        // branch_if(b,false,lab);
        //
        else if (cond)
        {
            Label skip;
            EmitBranchIfExpression(left, false, skip, over);
            EmitBranchIfExpression(right, true, lab, over);
            DefineLabel(skip);
            CompleteLabel(skip);
        }
        else
        {
            EmitBranchIfExpression(left, false, lab, over);
            EmitBranchIfExpression(right, false, lab, over);
        }
        return;
    case AstBinaryExpression::OR_OR:
        //
        // branch_if(false||b, cond, lab) =>
        // branch_if(b, cond, lab);
        // branch_if(true||b, cond, lab) =>
        // branch_if(true, cond, lab);
        //
        if (left -> IsConstant())
        {
            if (IsZero(left))
                EmitBranchIfExpression(right, cond, lab, over);
            else if (cond)
                EmitBranch(OP_GOTO, lab, over);
        }
        //
        // branch_if(a||false, cond, lab) =>
        // branch_if(a, cond, lab);
        // branch_if(a||true, cond, lab) =>
        // emit(a), pop; for side effects
        //
        else if (right -> IsConstant())
        {
            if (IsZero(right))
                EmitBranchIfExpression(left, cond, lab, over);
            else
            {
                EmitExpression(left);
                PutOp(OP_POP);
                if (cond)
                    EmitBranch(OP_GOTO, lab, over);
            }
        }
        //
        // branch_if(a||b,true,lab) =>
        // branch_if(a,true,lab);
        // branch_if(b,true,lab);
        // branch_if(a||b,false,lab) =>
        // branch_if(a,true,skip);
        // branch_if(b,false,lab);
        // skip:
        //
        else if (cond)
        {
            EmitBranchIfExpression(left, true, lab, over);
            EmitBranchIfExpression(right, true, lab, over);
        }
        else
        {
            Label skip;
            EmitBranchIfExpression(left, true, skip, over);
            EmitBranchIfExpression(right, false, lab, over);
            DefineLabel(skip);
            CompleteLabel(skip);
        }
        return;
    case AstBinaryExpression::EQUAL_EQUAL:
    case AstBinaryExpression::NOT_EQUAL:
        //
        // One of the operands is null.
        //
        if (left_type == control.null_type || right_type == control.null_type)
        {
            if (left_type == control.null_type)
            {
                // arrange so right operand is null
                AstExpression *temp = left;
                left = right;
                right = temp;

                left_type = left -> Type();
                right_type = right -> Type();
            }

            EmitExpression(left);

            if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL)
                EmitBranch(cond ? OP_IFNULL : OP_IFNONNULL, lab, over);
            else EmitBranch(cond ? OP_IFNONNULL : OP_IFNULL, lab, over);

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
                EmitBranch((cond ? OP_IFEQ : OP_IFNE), lab, over);
            else EmitBranch((cond ? OP_IFNE : OP_IFEQ), lab, over);

            return;
        }

        //
        // both operands are integer
        //
        if ((control.IsSimpleIntegerValueType(left_type) ||
             left_type == control.boolean_type) &&
            (control.IsSimpleIntegerValueType(right_type) ||
             right_type == control.boolean_type))
        {
            EmitExpression(left);
            EmitExpression(right);

            if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL)
                EmitBranch((cond ? OP_IF_ICMPEQ : OP_IF_ICMPNE), lab, over);
            else EmitBranch((cond ? OP_IF_ICMPNE : OP_IF_ICMPEQ), lab, over);

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
                EmitBranch((cond ? OP_IF_ACMPEQ : OP_IF_ACMPNE), lab, over);
            else EmitBranch((cond ? OP_IF_ACMPNE : OP_IF_ACMPEQ), lab, over);

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
    switch (bp -> binary_tag)
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
        EmitBranch(cond ? OP_IFNE : OP_IFEQ, lab, over);
        return;
    }

    //
    //
    //
    unsigned opcode = 0,
             op_true,
             op_false;
    if (control.IsSimpleIntegerValueType(left_type) || left_type == control.boolean_type)
    {
        //
        // we have already dealt with EQUAL_EQUAL and NOT_EQUAL for the case
        // of two integers, but still need to look for comparisons for which
        // one operand may be zero.
        //
        if (IsZero(left))
        {
            EmitExpression(right);
            switch (bp -> binary_tag)
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
            default:
                assert(false);
                break;
            }
        }
        else if (IsZero(right))
        {
            EmitExpression(left);

            switch (bp -> binary_tag)
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
            default:
                assert(false);
                break;
            }
        }
        else
        {
            EmitExpression(left);
            EmitExpression(right);

            switch (bp -> binary_tag)
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
            default:
                assert(false);
                break;
            }
        }
    }
    else if (left_type == control.long_type)
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
        default:
            assert(false);
            break;
        }
    }
    else if (left_type == control.float_type)
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
        default:
            assert(false);
            break;
        }
    }
    else if (left_type == control.double_type)
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
        default:
            assert(false);
            break;
        }
    }
    else assert(false && "comparison of unsupported type");

    if (opcode)
        PutOp(opcode); // if need to emit comparison before branch

    EmitBranch (cond ? op_true : op_false, lab, over);
}


void ByteCode::EmitSynchronizedStatement(AstSynchronizedStatement *statement)
{
    EmitExpression(statement -> expression);

    int variable_index = method_stack -> TopBlock() -> block_symbol -> try_or_synchronized_variable_index;

    PutOp(OP_DUP); // duplicate for saving, entering monitor
    StoreLocal(variable_index, control.Object()); // save address of object
    PutOp(OP_MONITORENTER); // enter monitor associated with object

    int start_synchronized_pc = code_attribute -> CodeLength(); // start pc

    EmitBlockStatement(statement -> block);

    int end_synchronized_pc = code_attribute -> CodeLength(); // end pc

    if (statement -> block -> can_complete_normally)
    {
        LoadLocal(variable_index, control.Object()); // load address of object onto stack
        PutOp(OP_MONITOREXIT);
    }

    if (start_synchronized_pc != end_synchronized_pc) // if the synchronized block is not empty.
    {
        Label end_label;
        if (statement -> block -> can_complete_normally)
            EmitBranch(OP_GOTO, end_label); // branch around exception handler

        //
        // Reach here if any exception thrown.
        //
        int handler_pc = code_attribute -> CodeLength();
        assert(stack_depth == 0);
        stack_depth = 1; // account for the exception already on the stack
        LoadLocal(variable_index, control.Object()); // load address of object onto stack
        PutOp(OP_MONITOREXIT);
        PutOp(OP_ATHROW);

        code_attribute -> AddException(start_synchronized_pc, handler_pc, handler_pc, 0);

        if (IsLabelUsed(end_label))
            DefineLabel(end_label);
        CompleteLabel(end_label);
    }
}


void ByteCode::EmitAssertStatement(AstAssertStatement *assertion)
{
    //
    // When constant true, the assert statement is a no-op.
    // Otherwise, assert a : b; is syntactic sugar for:
    //
    // while (! ($noassert && (a)))
    //     throw new java.lang.AssertionError(b);
    //
    if (! semantic.IsConstantTrue(assertion -> condition))
    {
        PutOp(OP_GETSTATIC);
        PutU2(RegisterFieldref(assertion -> assert_variable));
        Label label;
        EmitBranch(OP_IFNE, label);
        EmitBranchIfExpression(assertion -> condition, true, label, NULL);

        PutOp(OP_NEW);
        PutU2(RegisterClass(control.AssertionError() -> fully_qualified_name));
        PutOp(OP_DUP);

        MethodSymbol *constructor = NULL;
        if (assertion -> message_opt)
        {
            EmitExpression(assertion -> message_opt);
            TypeSymbol *type = assertion -> message_opt -> Type();
            constructor = (type == control.char_type ? control.AssertionError_InitWithCharMethod()
                           : type == control.boolean_type ? control.AssertionError_InitWithBooleanMethod()
                           : (type == control.int_type ||
                              type == control.short_type ||
                              type == control.byte_type) ? control.AssertionError_InitWithIntMethod()
                           : type == control.long_type ? control.AssertionError_InitWithLongMethod()
                           : type == control.float_type ? control.AssertionError_InitWithFloatMethod()
                           : type == control.double_type ? control.AssertionError_InitWithDoubleMethod()
                           : (type == control.null_type ||
                              IsReferenceType(type)) ? control.AssertionError_InitWithObjectMethod()
                           : control.AssertionError_InitMethod()); // for assertion

            assert(constructor != control.AssertionError_InitMethod() &&
                   "unable to find right constructor for assertion error");
            ChangeStack(- GetTypeWords(type));
        }
        else constructor = control.AssertionError_InitMethod();
    
        PutOp(OP_INVOKESPECIAL);
        PutU2(RegisterLibraryMethodref(constructor));
        PutOp(OP_ATHROW);

        DefineLabel(label);
        CompleteLabel(label);
    }
}


//
// JLS is Java Language Specification
// JVM is Java Virtual Machine
//
// Expressions: Chapter 14 of JLS
//
int ByteCode::EmitExpression(AstExpression *expression, bool need_value)
{
    //
    // The only time when need_value should be false is for an expression
    // qualifying a static member access.  Hence, it must be a reference,
    // and if one is a constant, it must be a String expression.
    //
    expression = StripNops(expression);
    if (expression -> IsConstant())
    {
        if (need_value)
        {
            LoadLiteral(expression -> value, expression -> Type());
            return (GetTypeWords(expression -> Type()));
        }
        else
        {
            assert(expression -> Type() == control.String());
            return 0;
        }
    }

    switch (expression -> kind)
    {
    case Ast::IDENTIFIER:
        if (need_value)
        {
            AstSimpleName *simple_name = expression -> SimpleNameCast();
            return (simple_name -> resolution_opt
                    ? EmitExpression(simple_name -> resolution_opt)
                    : LoadVariable(GetLhsKind(expression), expression));
        }
        else
            return 0;
    case Ast::THIS_EXPRESSION:
    case Ast::SUPER_EXPRESSION:
        if (need_value)
        {
            PutOp(OP_ALOAD_0); // will be used
            return 1;
        }
        else
            return 0;
    case Ast::PARENTHESIZED_EXPRESSION:
        return EmitExpression(((AstParenthesizedExpression *) expression) -> expression, need_value);
    case Ast::CLASS_CREATION:
        return EmitClassInstanceCreationExpression((AstClassInstanceCreationExpression *) expression, need_value);
    case Ast::ARRAY_CREATION:
        assert(need_value && "array can't qualify static member");
        return EmitArrayCreationExpression((AstArrayCreationExpression *) expression);
    case Ast::DIM:
        assert(need_value && "dimension can't qualify static member");
        return EmitExpression(expression -> DimExprCast() -> expression);
    case Ast::DOT:
        {
            AstFieldAccess *field_access = (AstFieldAccess *) expression;
            return ((field_access -> IsClassAccess() &&
                     field_access -> resolution_opt)
                    ? (unit_type -> outermost_type -> ACC_INTERFACE()
                       ? EmitExpression(field_access -> resolution_opt, need_value)
                       : GenerateClassAccess(field_access, need_value))
                    : EmitFieldAccess(field_access, need_value));
        }
    case Ast::CALL:
        {
            AstMethodInvocation *method_call = expression -> MethodInvocationCast();
            // must evaluate for side effects
            EmitMethodInvocation(method_call);
            if (need_value)
                return GetTypeWords(method_call -> Type());
            else
            {
                assert(! method_call -> Type() -> Primitive());
                PutOp(OP_POP);
                return 0;
            }
        }
    case Ast::ARRAY_ACCESS:
        {
            // must evaluate, for potential Exception side effects
            int words = EmitArrayAccessRhs((AstArrayAccess *) expression);
            if (need_value)
                return words;
            else
            {
                assert(words == 1); // must be reference type
                PutOp(OP_POP);
                return 0;
            }
        }
    case Ast::POST_UNARY:
        assert(need_value && "increment can't qualify static member");
        return EmitPostUnaryExpression((AstPostUnaryExpression *) expression, true);
    case Ast::PRE_UNARY:
        assert(need_value && "increment can't qualify static member");
        return EmitPreUnaryExpression((AstPreUnaryExpression *) expression, true);
    case Ast::CAST:
        {
            AstCastExpression *cast_expression = (AstCastExpression *) expression;

            assert(need_value || ! cast_expression -> expression -> Type() -> Primitive());

            //
            // only primitive types require casting
            //
            return (cast_expression -> expression -> Type() -> Primitive()
                    ? EmitCastExpression(cast_expression)
                    : EmitExpression(cast_expression -> expression, need_value));
        }
    case Ast::CHECK_AND_CAST:
        {
            // must evaluate, for ClassCastException side effect
            int words = EmitCastExpression((AstCastExpression *) expression);
            if (need_value)
                return words;
            else
            {
                assert(words == 1); // must be reference type
                PutOp(OP_POP);
                return 0;
            }
        }
    case Ast::BINARY:
        {
            // must evaluate for potential side effects
            int words = EmitBinaryExpression((AstBinaryExpression *) expression);
            if (need_value)
                return words;
            else
            {
                assert(((AstBinaryExpression *) expression) -> binary_tag == AstBinaryExpression::PLUS && words == 1); // must be string concat
                PutOp(OP_POP);
                return 0;
            }
        }
    case Ast::CONDITIONAL:
        return EmitConditionalExpression((AstConditionalExpression *) expression, need_value);
    case Ast::ASSIGNMENT:
        return EmitAssignmentExpression((AstAssignmentExpression *) expression, need_value);
    case Ast::NULL_LITERAL:
        if (need_value)
        {
            PutOp(OP_ACONST_NULL);
            return 1;
        }
        else
            return 0;
    default:
        assert(false && "unknown expression kind");
        break;
    }

    return 0; // even though we will not reach here
}


AstExpression *ByteCode::VariableExpressionResolution(AstExpression *expression)
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

    return expression;
}


TypeSymbol *ByteCode::VariableTypeResolution(AstExpression *expression, VariableSymbol *sym)
{
    expression = VariableExpressionResolution(expression);
    AstFieldAccess *field = expression -> FieldAccessCast();
    AstSimpleName *simple_name = expression -> SimpleNameCast();
    assert(field || simple_name);

    //
    // JLS2 13.1 Use the type of the base expression for qualified reference
    // (this even works for super expressions), and the innermost type that
    // contains the (possibly inherited) field for simple name reference.
    // However, for class literals, use the owner of the field.
    //
    return (field ? (field -> IsClassAccess()
                     ? (TypeSymbol *) field -> symbol -> VariableCast() -> owner
                     : field -> base -> Type())
            : (simple_name -> resolution_opt
               ? simple_name -> resolution_opt -> Type()
               : unit_type));
}


TypeSymbol *ByteCode::MethodTypeResolution(AstExpression *method_name,
                                           MethodSymbol *msym)
{
    //
    // JLS 13.1 If the method is declared in Object, use Object. Otherwise,
    // use the type of the base expression for qualified reference (this even
    // works for super expressions), and the innermost type that contains the
    // (possibly inherited) method for simple name reference.  However, if
    // this is an accessor method, use the owner_type (since the base type
    // relates to the accessed expression, not the accessor method).
    //
    AstFieldAccess *field = method_name -> FieldAccessCast();
    AstSimpleName *simple_name = method_name -> SimpleNameCast();
    assert(field || simple_name);

    TypeSymbol *owner_type = msym -> containing_type,
               *base_type = (msym -> IsSynthetic() ? owner_type
                             : field ? field -> base -> Type()
                             : simple_name -> resolution_opt ? simple_name -> resolution_opt -> Type()
                             : unit_type);

    return owner_type == control.Object() ? owner_type : base_type;
}


void ByteCode::EmitFieldAccessLhsBase(AstExpression *expression)
{
    expression = VariableExpressionResolution(expression);
    AstFieldAccess *field = expression -> FieldAccessCast();

    //
    // We now have the right expression. Check if it's a field. If so, process
    // base Otherwise, it must be a simple name...
    //
    if (field)
        EmitExpression(field -> base);
    else
    {
        assert(expression -> SimpleNameCast() && "unexpected AssignmentExpressionField operand base type");

        PutOp(OP_ALOAD_0); // get address of "this"
    }
}


void ByteCode::EmitFieldAccessLhs(AstExpression *expression)
{
    EmitFieldAccessLhsBase(expression);
    PutOp(OP_DUP);     // save base address of field for later store
    PutOp(OP_GETFIELD);
    if (control.IsDoubleWordType(expression -> Type()))
        ChangeStack(1);

    VariableSymbol *sym = (VariableSymbol *) expression -> symbol;
    PutU2(RegisterFieldref(VariableTypeResolution(expression, sym), sym));
}


//
// Generate code for access method used to set class literal fields
//
void ByteCode::GenerateClassAccessMethod(MethodSymbol *msym)
{
    //
    // Here, we add a line-number attribute entry for this method.
    // Even though this is a generated method, JPDA debuggers will
    // still fail setting breakpoints if methods don't have line numbers.
    // Sun's javac compiler generates a single line number entry
    // with start_pc set to zero and line number set to the first line of
    // code in the source. In testing, it appears that setting the start_pc
    // and line_number to zero as we do here, also works.
    //
    line_number_table_attribute -> AddLineNumber(0, 0);

    //
    // The code takes the form:
    //
    // Since the VM does not have a nice way of finding a class without a
    // runtime object, we use this approach.  Notice that getClass can throw
    // a checked exception, but JLS semantics do not allow this, so we must
    // add a catch block to convert the problem to an unchecked Error.
    //
    // The generated code is equivalent to:
    //
    // /*synthetic*/ static java.lang.Class class$(java.lang.String name) {
    //     try {
    //         return java.lang.Class.forName(name);
    //     } catch (ClassNotFoundException e) {
    //         throw new NoClassDefFoundError(((Throwable) e).getMessage());
    //     }
    // }
    //
    // TODO: Obey option.target >= SDK1_4, where we want to use
    //   throw new NoClassDefFoundError(e);
    //
    // Since ClassNotFoundException inherits, rather than declares, getMessage,
    // we link to Throwable, and use the cast to Throwable in the code above to
    // show that we are still obeying JLS 13.1, which requires that .class
    // files must link to the type of the qualifying expression.
    //
    //  aload_0        load this
    //  invokestatic   java/lang/Class.forName(Ljava/lang/String;)Ljava/lang/Class;
    //  areturn        return Class object for the class named by string
    //
    // exception handler if forName fails (optimization: the
    // ClassNotFoundException will already be on the stack):
    //
    //  invokevirtual  java/lang/Throwable.getMessage()Ljava/lang/String;
    //  new            java/lang/NoClassDefFoundError
    //  dup_x1         save copy to throw, but below string arg to constructor
    //  swap           swap string and new object to correct order
    //  invokespecial  java/lang/NoClassDefFoundError.<init>(Ljava/lang/String;)V
    //  athrow         throw the correct exception
    PutOp(OP_ALOAD_0);
    PutOp(OP_INVOKESTATIC);
    PutU2(RegisterLibraryMethodref(control.Class_forNameMethod()));
    PutOp(OP_ARETURN);

    ChangeStack(1); // account for the exception on the stack
    PutOp(OP_INVOKEVIRTUAL);
    PutU2(RegisterLibraryMethodref(control.Throwable_getMessageMethod()));
    ChangeStack(1); // account for the returned string
    PutOp(OP_NEW);
    PutU2(RegisterClass(control.NoClassDefFoundError() -> fully_qualified_name));
    PutOp(OP_DUP_X1);
    PutOp(OP_SWAP);
    PutOp(OP_INVOKESPECIAL);
    PutU2(RegisterLibraryMethodref(control.NoClassDefFoundError_InitMethod()));
    ChangeStack(-1); // account for the argument to the constructor
    PutOp(OP_ATHROW);

    code_attribute -> AddException(0,
                                   5,
                                   5,
                                   RegisterClass(control.ClassNotFoundException() -> fully_qualified_name));
}


//
// here to generate code to dymanically initialize the field for a class
// literal and then return its value
//
int ByteCode::GenerateClassAccess(AstFieldAccess *field_access, bool need_value)
{
    if (! need_value)
        return 0;
    //
    // Evaluate X.class literal. If X is a primitive type, this is a
    // predefined field; otherwise, we must create a new synthetic field to
    // hold the desired result and initialize it at runtime. Within a class,
    // this cannot be done in the static initializer, because it is possible
    // to access a class literal before a class is initialized. We cache the
    // result, to avoid a runtime hit.
    //
    // Foo.Bar.class becomes
    // (class$Foo$Bar == null ? class$Foo$Bar = class$("Foo.Bar")
    //                        : class$Foo$Bar)
    //
    // getstatic class_field     load class field
    // dup                       optimize: common case is non-null
    // ifnonnull label           branch if it exists, otherwise initialize
    // pop                       pop the null we just duplicated
    // load class_constant       get name of class
    // invokestatic              invoke synthetic class$ method
    // dup                       save value so can return it
    // put class_field           initialize the field
    // label:
    //
    Label label;
    assert(field_access -> symbol -> VariableCast());

    u2 field_index = RegisterFieldref(field_access -> symbol -> VariableCast());

    PutOp(OP_GETSTATIC);
    PutU2(field_index);
    PutOp(OP_DUP);
    EmitBranch(OP_IFNONNULL, label);

    PutOp(OP_POP);
    LoadLiteral(field_access -> base -> Type() -> ClassLiteralName(),
                control.String());
    PutOp(OP_INVOKESTATIC);
    CompleteCall(unit_type -> outermost_type -> ClassLiteralMethod(), 1);
    PutOp(OP_DUP);
    PutOp(OP_PUTSTATIC);
    PutU2(field_index);

    DefineLabel(label);
    CompleteLabel(label);

    return 1; // return one-word (reference) result
}


//
// Generate code for initializing assert variable
//
void ByteCode::GenerateAssertVariableInitializer(TypeSymbol *tsym,
                                                 VariableSymbol *vsym)
{
    //
    // Create the field initializer. This approach avoids using a class
    // literal, for two reasons:
    //   - we use fewer bytecodes if the rest of the class does not use class
    //     literals (and we need no try-catch block)
    //   - determining assertion status will not initialize an enclosing class.
    //
    // Unfortunately, until the VM supports easier determination of classes
    // from a static context, we must create an empty garbage array.
    // We initialize to the opposite of desiredAssertionStatus to obey the
    // semantics of assert - until class initialization starts, the default
    // value of false in this variable will enable asserts anywhere in the
    // class.
    //
    // private static final boolean $noassert = ! new <outermostClass>[0]
    //     .getClass().getComponentType().desiredAssertionStatus();
    //
    //  iconst_0
    //  anewarray        <outermostClass>
    //  invokevirtual    java/lang/Object.getClass()java/lang/Class
    //  invokevirtual    java/lang/Class.getComponentType()java/lang/Class
    //  invokevirtual    java/lang/Class.desiredAssertionStatus()Z
    //  iconst_1
    //  ixor             result ^ true <=> !result
    //  putstatic        <thisClass>.$noassert
    //
    PutOp(OP_ICONST_0);
    PutOp(OP_ANEWARRAY);
    PutU2(RegisterClass(tsym -> fully_qualified_name));
    PutOp(OP_INVOKEVIRTUAL);
    ChangeStack(1); // for returned value
    PutU2(RegisterLibraryMethodref(control.Object_getClassMethod()));
    PutOp(OP_INVOKEVIRTUAL);
    ChangeStack(1); // for returned value
    PutU2(RegisterLibraryMethodref(control.Class_getComponentTypeMethod()));
    PutOp(OP_INVOKEVIRTUAL);
    ChangeStack(1); // for returned value
    PutU2(RegisterLibraryMethodref(control.Class_desiredAssertionStatusMethod()));
    PutOp(OP_ICONST_1);
    PutOp(OP_IXOR);
    PutOp(OP_PUTSTATIC);
    PutU2(RegisterFieldref(vsym));
}


//
// see also OP_MULTIANEWARRAY
//
int ByteCode::EmitArrayCreationExpression(AstArrayCreationExpression *expression)
{
    int num_dims = expression -> NumDimExprs();

    if (num_dims > 255)
    {
        semantic.ReportSemError(SemanticError::ARRAY_OVERFLOW,
                                expression -> LeftToken(),
                                expression -> RightToken());
    }

    if (expression -> array_initializer_opt)
        InitializeArray(expression -> Type(), expression -> array_initializer_opt);
    else
    {
        //
        // need to push value of dimension(s)
        //
        for (int i = 0; i < num_dims; i++)
            EmitExpression(expression -> DimExpr(i) -> expression);

        EmitNewArray(num_dims, expression -> Type());
    }

    return 1;
}


//
// ASSIGNMENT
//
int ByteCode::EmitAssignmentExpression(AstAssignmentExpression *assignment_expression, bool need_value)
{
    AstCastExpression *casted_left_hand_side = assignment_expression -> left_hand_side -> CastExpressionCast();
    AstExpression *left_hand_side = (casted_left_hand_side ? casted_left_hand_side -> expression : assignment_expression -> left_hand_side);

    TypeSymbol *left_type = left_hand_side -> Type();

    int kind = GetLhsKind(assignment_expression);
    VariableSymbol *accessed_member = (assignment_expression -> write_method
                                                   ? assignment_expression -> write_method -> accessed_member -> VariableCast()
                                                   : (VariableSymbol *) NULL);

    if (assignment_expression -> SimpleAssignment())
    {
        switch (kind)
        {
        case LHS_ARRAY:
            EmitArrayAccessLhs(left_hand_side -> ArrayAccessCast()); // lhs must be array access
            break;
        case LHS_FIELD:
            EmitFieldAccessLhsBase(left_hand_side); // load base for field access
            break;
        case LHS_STATIC:
            //
            // If the access is qualified by an arbitrary base
            // expression, evaluate it for side effects.
            //
            if (left_hand_side -> FieldAccessCast())
                EmitExpression(((AstFieldAccess *) left_hand_side) -> base, false);
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
            else if (left_hand_side -> FieldAccessCast())
                //
                // If the access is qualified by an arbitrary base
                // expression, evaluate it for side effects.
                //
                EmitExpression(((AstFieldAccess *) left_hand_side) -> base, false);
            break;
        case LHS_LOCAL:
            break;
        default:
            assert(false && "bad kind in EmitAssignmentExpression");
        }

        EmitExpression(assignment_expression -> expression);
    }
    //
    // Here for compound assignment. Get the left operand, saving any
    // information necessary to update its value on the stack below the value.
    //
    else
    {
        switch (kind)
        {
        case LHS_ARRAY:
            EmitArrayAccessLhs(left_hand_side -> ArrayAccessCast()); // lhs must be array access
            PutOp(OP_DUP2); // save base and index for later store

            //
            // load current value
            //
            LoadArrayElement(assignment_expression -> Type());
            break;
        case LHS_FIELD:
            EmitFieldAccessLhs(left_hand_side);
            break;
        case LHS_LOCAL:
            if (! casted_left_hand_side &&
                assignment_expression -> Type() == control.int_type &&
                assignment_expression -> expression -> IsConstant() &&
                (assignment_expression -> assignment_tag == AstAssignmentExpression::PLUS_EQUAL ||
                 assignment_expression -> assignment_tag == AstAssignmentExpression::MINUS_EQUAL))
            {
                IntLiteralValue *vp = (IntLiteralValue *) assignment_expression -> expression -> value;
                int val = (assignment_expression -> assignment_tag == AstAssignmentExpression::MINUS_EQUAL
                           ? -(vp -> value) // we treat "a -= x" as "a += (-x)"
                           : vp -> value);
                if (val >= -32768 && val < 32768) // if value in range
                {
                    VariableSymbol *sym = (VariableSymbol *) left_hand_side -> symbol;
                    PutOpIINC(sym -> LocalVariableIndex(), val);
                    if (need_value)
                        LoadVariable(LHS_LOCAL, left_hand_side);
                    return GetTypeWords(assignment_expression -> Type());
                }
            }

            LoadVariable(kind, left_hand_side);
            break;
        case LHS_STATIC:
            LoadVariable(kind, left_hand_side);
            break;
        case LHS_METHOD:
            //
            // If we are accessing a static member, get value by invoking
            // appropriate resolution. Otherwise, in addition to getting
            // the value, we need to load address of the object,
            // obtained from the resolution, saving a copy on the stack.
            //
            if (accessed_member -> ACC_STATIC())
                EmitExpression(left_hand_side);
            else ResolveAccess(left_hand_side);
            break;
        default:
            assert(false && "bad kind in EmitAssignmentExpression");
        }

        //
        // Here for string concatenation.
        //
        if (assignment_expression -> assignment_tag == AstAssignmentExpression::PLUS_EQUAL && left_type == control.String())
        {
            PutOp(OP_NEW);
            PutU2(RegisterClass(control.StringBuffer() -> fully_qualified_name));
            PutOp(OP_DUP_X1);
            PutOp(OP_INVOKESPECIAL);
            PutU2(RegisterLibraryMethodref(control.StringBuffer_InitMethod()));
            EmitStringAppendMethod(control.String());
            AppendString(assignment_expression -> expression);
            PutOp(OP_INVOKEVIRTUAL);
            PutU2(RegisterLibraryMethodref(control.StringBuffer_toStringMethod()));
            ChangeStack(1); // account for return value
        }
        //
        // Here for operation other than string concatenation. Determine the
        // opcode to use.
        //
        else
        {
            int opc;

            TypeSymbol *op_type = (casted_left_hand_side
                                   ? casted_left_hand_side -> Type()
                                   : assignment_expression -> Type());

            if (control.IsSimpleIntegerValueType(op_type) ||
                op_type == control.boolean_type)
            {
                switch (assignment_expression -> assignment_tag)
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
                    assert(false && "bad op_type in EmitAssignmentExpression");
                }
            }
            else if (op_type == control.long_type)
            {
                switch (assignment_expression -> assignment_tag)
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
                    assert(false && "bad op_type in EmitAssignmentExpression");
                }
            }
            else if (op_type == control.float_type)
            {
                switch (assignment_expression -> assignment_tag)
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
                    assert(false && "bad op_type in EmitAssignmentExpression");
                }
            }
            else if (op_type == control.double_type)
            {
                switch (assignment_expression -> assignment_tag)
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
                    assert(false && "bad op_type in EmitAssignmentExpression");
                }
            }
            else
            {
                assert(false && "unrecognized op_type in EmitAssignmentExpression");
            }

            //
            // convert value to desired type if necessary
            //
            if (casted_left_hand_side)
                EmitCast(casted_left_hand_side -> Type(), left_type);

            EmitExpression(assignment_expression -> expression);

            PutOp(opc);

            if (casted_left_hand_side) // now cast result back to type of result
                EmitCast(left_type, casted_left_hand_side -> Type());
        }
    }

    //
    // Update left operand, saving value of right operand if it is needed.
    //
    switch (kind)
    {
    case LHS_ARRAY:
        if (need_value)
            PutOp(control.IsDoubleWordType(left_type) ? OP_DUP2_X2 : OP_DUP_X2);
        StoreArrayElement(assignment_expression -> Type());
        break;
    case LHS_FIELD:
        if (need_value)
            PutOp(control.IsDoubleWordType(left_type) ? OP_DUP2_X1 : OP_DUP_X1);
        StoreField(left_hand_side);
        break;
    case LHS_METHOD:
        {
            if (need_value)
            {
                if (accessed_member -> ACC_STATIC())
                    PutOp(control.IsDoubleWordType(left_type) ? OP_DUP2 : OP_DUP);
                else PutOp(control.IsDoubleWordType(left_type) ? OP_DUP2_X1 : OP_DUP_X1);
            }

            int stack_words = (GetTypeWords(left_type) +
                               accessed_member -> ACC_STATIC() ? 0 : 1);
            PutOp(OP_INVOKESTATIC);
            CompleteCall(assignment_expression -> write_method, stack_words);
        }
        break;
    case LHS_LOCAL:
    case LHS_STATIC:
        if (need_value)
            PutOp(control.IsDoubleWordType(left_type) ? OP_DUP2 : OP_DUP);
        StoreVariable(kind, left_hand_side);
        break;
    default:
        assert(false && "bad kind in EmitAssignmentExpression");
    }

    return GetTypeWords(assignment_expression -> Type());
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
            // Assume false, and update if true.
            Label label;
            PutOp(OP_ICONST_0); // push false
            EmitBranchIfExpression(expression, false, label, NULL);
            PutOp(OP_POP); // pop the false
            PutOp(OP_ICONST_1); // push true 
            DefineLabel(label);
            CompleteLabel(label);
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
    if (expression -> Type() == control.String())
    {
        assert(expression -> binary_tag == AstBinaryExpression::PLUS);
        ConcatenateString(expression);
        PutOp(OP_INVOKEVIRTUAL);
        PutU2(RegisterLibraryMethodref(control.StringBuffer_toStringMethod()));
        ChangeStack(1); // account for return value
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
            if (control.IsSimpleIntegerValueType(right_type) || right_type == control.boolean_type)
                PutOp(OP_ICONST_0);
            else
            {
                assert((right_type == control.long_type ||
                        right_type == control.float_type ||
                        right_type == control.double_type) && "unexpected type in expression simplification");

                PutOp(right_type == control.long_type ? OP_LCONST_0
                      : right_type == control.float_type ? OP_FCONST_0
                      : OP_DCONST_0); // double_type
            }
            return GetTypeWords(right_type);
        case AstBinaryExpression::MINUS: // 0 - x is negation of x
            EmitExpression(expression -> right_expression);

            assert((control.IsSimpleIntegerValueType(right_type) ||
                    right_type == control.long_type ||
                    right_type == control.float_type ||
                    right_type == control.double_type) && "unexpected type in expression simplification");

            PutOp(control.IsSimpleIntegerValueType(right_type) ? OP_INEG
                  : right_type == control.long_type ? OP_LNEG
                  : right_type == control.float_type ? OP_FNEG
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
            if (control.IsSimpleIntegerValueType(left_type) || left_type == control.boolean_type)
                PutOp(OP_ICONST_0);
            else
            {
                assert((left_type == control.long_type ||
                        left_type == control.float_type ||
                        left_type == control.double_type) && "unexpected type in expression simplification");

                PutOp(left_type == control.long_type ? OP_LCONST_0
                      : left_type == control.float_type ? OP_FCONST_0
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
    bool integer_type = (control.IsSimpleIntegerValueType(type) || type == control.boolean_type);
    switch (expression -> binary_tag)
    {
    case AstBinaryExpression::STAR:
        PutOp(integer_type ? OP_IMUL
              : type == control.long_type ? OP_LMUL
              : type == control.float_type ? OP_FMUL
              : OP_DMUL); // double_type
        break;
    case AstBinaryExpression::SLASH:
        PutOp(integer_type ? OP_IDIV
              : type == control.long_type ? OP_LDIV
              : type == control.float_type ? OP_FDIV
              : OP_DDIV); // double_type
        break;
    case AstBinaryExpression::MOD:
        PutOp(integer_type ? OP_IREM
              : type == control.long_type ? OP_LREM
              : type == control.float_type ? OP_FREM
              : OP_DREM); // double_type
        break;
    case AstBinaryExpression::PLUS:
        PutOp(integer_type ? OP_IADD
              : type == control.long_type ? OP_LADD
              : type == control.float_type ? OP_FADD
              : OP_DADD); // double_type
        break;
    case AstBinaryExpression::MINUS:
        PutOp(integer_type ? OP_ISUB
              : type == control.long_type ? OP_LSUB
              : type == control.float_type ? OP_FSUB
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
    if (source_type -> IsSubclass(dest_type) ||
        source_type == control.null_type)
    {
        return; // done if nothing to do
    }

    if (control.IsSimpleIntegerValueType(source_type))
    {
        if (dest_type == control.int_type ||
            (source_type == control.byte_type &&
             dest_type == control.short_type))
        {
            return; // no conversion needed
        }
        Operators::operators op_kind = (dest_type == control.long_type ? OP_I2L
                                        : dest_type == control.float_type ? OP_I2F
                                        : dest_type == control.double_type ? OP_I2D
                                        : dest_type == control.char_type ? OP_I2C
                                        : dest_type == control.byte_type ? OP_I2B
                                        : OP_I2S); // short_type
        // If the type we wanted to cast to could not be matched then
        // the cast is invalid. For example, one might be trying
        // to cast an int to a Object.
        assert(op_kind != OP_I2S || dest_type == control.short_type);

        PutOp(op_kind);
    }
    else if (source_type == control.long_type)
    {
        Operators::operators op_kind = (dest_type == control.float_type ? OP_L2F
                                        : dest_type == control.double_type ? OP_L2D
                                        : OP_L2I);
        PutOp(op_kind);

        if (op_kind == OP_L2I && dest_type != control.int_type)
        {
            assert(control.IsSimpleIntegerValueType(dest_type) && "unsupported conversion");

            PutOp(dest_type == control.char_type ? OP_I2C
                  : dest_type == control.byte_type ? OP_I2B
                  : OP_I2S); // short_type
        }
    }
    else if (source_type == control.float_type)
    {
        Operators::operators op_kind = (dest_type == control.long_type ? OP_F2L
                                        : dest_type == control.double_type ? OP_F2D
                                        : OP_F2I);
        PutOp(op_kind);

        if (op_kind == OP_F2I && dest_type != control.int_type)
        {
            assert(control.IsSimpleIntegerValueType(dest_type) && "unsupported conversion");

            PutOp(dest_type == control.char_type ? OP_I2C
                  : dest_type == control.byte_type ? OP_I2B
                  : OP_I2S); // short_type
        }
    }
    else if (source_type == control.double_type)
    {
        Operators::operators op_kind = (dest_type == control.long_type ? OP_D2L
                                        : dest_type == control.float_type ? OP_D2F
                                        : OP_D2I);

        PutOp(op_kind);

        if (op_kind == OP_D2I && dest_type != control.int_type)
        {
            assert(control.IsSimpleIntegerValueType(dest_type) && "unsupported conversion");

            PutOp(dest_type == control.char_type ? OP_I2C
                  : dest_type == control.byte_type ? OP_I2B
                  : OP_I2S); // short_type
        }
    }
    else
    {
        //
        // Generate check cast instruction.
        //
        PutOp(OP_CHECKCAST);
        PutU2(dest_type -> num_dimensions > 0
              ? RegisterClass(dest_type -> signature)
              : RegisterClass(dest_type -> fully_qualified_name));
    }
}

//
// Emits the required check for null in a qualified instance creation or
// super constructor call if the base expression can possibly be null.
//
void ByteCode::EmitCheckForNull(AstExpression *expression)
{
    expression = StripNops(expression);

    if (! expression -> ClassInstanceCreationExpressionCast() &&
        ! expression -> ThisExpressionCast())
    {
        PutOp(OP_DUP);
        Label lab1;
        EmitBranch(OP_IFNONNULL, lab1);

        //
        // The base was null, so we need to raise a NullPointerException. But
        // the fastest way to do that is throwing null. Notice that the
        // verifier remembers what type the existing null on the stack has, so
        // we must use a fresh one.
        //
        PutOp(OP_ACONST_NULL);
        PutOp(OP_ATHROW);
        DefineLabel(lab1);
        CompleteLabel(lab1);
    }
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
        EmitCheckForNull(expression -> base_opt);
    }

    //
    // Pass all local arguments, if any
    //
    for (int i = 0; i < expression -> NumLocalArguments(); i++)
        stack_words += EmitExpression((AstExpression *) expression -> LocalArgument(i));

    //
    // If we are calling a private constructor, pass the extra null argument
    // to the access constructor.
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

    PutOp(OP_INVOKESPECIAL);
    ChangeStack(-stack_words);
    PutU2(RegisterMethodref(expression -> Type() -> fully_qualified_name,
                            control.init_name_symbol -> Utf8_literal,
                            constructor -> signature));

    return 1;
}


int ByteCode::EmitConditionalExpression(AstConditionalExpression *expression,
                                        bool need_value)
{
    if (expression -> test_expression -> IsConstant())
    {
        if (IsZero(expression -> test_expression))
            EmitExpression(expression -> false_expression);
        else EmitExpression(expression -> true_expression);
    }
    else
    {
        Label lab1,
              lab2;
        EmitBranchIfExpression(expression -> test_expression, false,
                               lab1, NULL);
        EmitExpression(expression -> true_expression, need_value);
        EmitBranch(OP_GOTO, lab2);
        if (need_value) // restore the stack size
            ChangeStack(- GetTypeWords(expression -> Type()));
        DefineLabel(lab1);
        EmitExpression(expression -> false_expression, need_value);
        DefineLabel(lab2);
        CompleteLabel(lab1);
        CompleteLabel(lab2);
    }
    return GetTypeWords(expression -> true_expression -> Type());
}


int ByteCode::EmitFieldAccess(AstFieldAccess *expression, bool need_value)
{
    assert(! expression -> IsConstant());

    AstExpression *base = expression -> base;
    VariableSymbol *sym = expression -> symbol -> VariableCast();

    if (expression -> resolution_opt) // resolve reference through enclosing class
    {
        //
        // If the access is qualified by an arbitrary base
        // expression, evaluate it for side effects.
        // Normally, this will be done when evaluating the accessor method.
        // However, if this is a static field, and need_value is false,
        // the access will not have a side effect, so we can bypass it.
        //
        MethodSymbol *method = expression -> symbol -> MethodCast();
        if (! need_value && method && ! method -> AccessesInstanceMember())
            return EmitExpression(base, false);
        else return EmitExpression(expression -> resolution_opt, need_value);
    }

    if (! sym) // not a variable, so it must be a class or package name
        return 0;

    if (base -> Type() -> IsArray() && sym -> ExternalIdentity() == control.length_name_symbol)
    {
        assert(need_value && "array.length cannot qualify static member");

        EmitExpression(base);
        PutOp(OP_ARRAYLENGTH);

        return 1;
    }

    TypeSymbol *expression_type = expression -> Type();
    if (sym -> ACC_STATIC())
    {
        //
        // If the access is qualified by an arbitrary base
        // expression, evaluate it for side effects.
        //
        if (! expression -> IsClassAccess())
            EmitExpression(base, false);
        if (need_value)
        {
            PutOp(OP_GETSTATIC);
            if (control.IsDoubleWordType(expression_type))
                ChangeStack(1);
        }
        else return 0;
    }
    else
    {
        EmitExpression(base); // get base
        PutOp(OP_GETFIELD); // must evaluate, in case of NullPointerException
        if (control.IsDoubleWordType(expression_type))
            ChangeStack(1);
    }

    PutU2(RegisterFieldref(VariableTypeResolution(expression, sym), sym));

    if (! need_value)
    {
        assert(! expression_type -> Primitive());
        PutOp(OP_POP);
        return 0;
    }

    return GetTypeWords(expression_type);
}


void ByteCode::EmitMethodInvocation(AstMethodInvocation *expression)
{
    //
    // If the method call was resolved into a call to another method, use the
    // resolution expression.
    //
    AstMethodInvocation *method_call = (expression -> resolution_opt
                                        ? expression -> resolution_opt -> MethodInvocationCast()
                                        : expression);
    assert(method_call);

    MethodSymbol *msym = (MethodSymbol *) method_call -> symbol;

    bool is_super = false; // set if super call

    if (msym -> ACC_STATIC())
    {
        //
        // If the access is qualified by an arbitrary base
        // expression, evaluate it for side effects.
        // Notice that accessor methods, which are always static, might
        // access an instance method, in which case the base expression
        // will already be evaluated as the first parameter.
        //
        AstFieldAccess *field = (msym -> AccessesInstanceMember() ? NULL
                                 : method_call -> method -> FieldAccessCast());
        if (field)
            EmitExpression(field -> base, false);
    }
    else
    {
        AstFieldAccess *field = method_call -> method -> FieldAccessCast();
        AstSimpleName *simple_name = method_call -> method -> SimpleNameCast();
        if (field)
        {
            //
            // Note that field will be marked IsSuperAccess only in synthetic
            // accessor methods.  Code that calls Foo.super.bar() in a nested
            // class creates an accessor method:
            // Foo.access$<num>(Foo $1) { $1.bar(); }
            // but must use invokespecial instead of the regular invokevirtual.
            //
            is_super = (field -> base -> IsSuperExpression() ||
                        field -> IsSuperAccess());

            EmitExpression(field -> base);
        }
        else if (simple_name)
        {
            if (simple_name -> resolution_opt) // use resolution if available
                EmitExpression(simple_name -> resolution_opt);
            else // must be field of current object, so load this
                PutOp(OP_ALOAD_0);
        }
        else assert(false && "unexpected argument to field access");
    }

    int stack_words = 0; // words on stack needed for arguments
    for (int i = 0; i < method_call -> NumArguments(); i++)
        stack_words += EmitExpression((AstExpression *) method_call -> Argument(i));

    TypeSymbol *type = MethodTypeResolution(method_call -> method, msym);
    PutOp(msym -> ACC_STATIC() ? OP_INVOKESTATIC
          : (is_super || msym -> ACC_PRIVATE()) ? OP_INVOKESPECIAL
          : type -> ACC_INTERFACE() ? OP_INVOKEINTERFACE
          : OP_INVOKEVIRTUAL);
    CompleteCall(msym, stack_words, type);
}


void ByteCode::CompleteCall(MethodSymbol *msym, int stack_words, TypeSymbol *base_type)
{
    ChangeStack(-stack_words);

    TypeSymbol *type = (base_type ? base_type : msym -> containing_type);

    PutU2(type -> ACC_INTERFACE() ? RegisterInterfaceMethodref(type -> fully_qualified_name,
                                                               msym -> ExternalIdentity() -> Utf8_literal,
                                                               msym -> signature)
                                  : RegisterMethodref(type -> fully_qualified_name,
                                                      msym -> ExternalIdentity() -> Utf8_literal,
                                                      msym -> signature));

    if (type -> ACC_INTERFACE())
    {
        PutU1(stack_words + 1);
        PutU1(0);
    }

    //
    // must account for value returned by method.
    //
    ChangeStack(control.IsDoubleWordType(msym -> Type()) ? 2
                : msym -> Type() == control.void_type ? 0 : 1);
}


//
// Called when expression has been parenthesized to removed parantheses and
// widening casts to expose true structure.
//
AstExpression *ByteCode::StripNops(AstExpression *expr)
{
    while (! expr -> IsConstant())
    {
        if (expr -> ParenthesizedExpressionCast())
            expr = ((AstParenthesizedExpression *) expr) -> expression;
        else if (expr -> CastExpressionCast())
        {
            AstCastExpression *cast_expr = (AstCastExpression *) expr;
            AstExpression *sub_expr = StripNops(cast_expr -> expression);
            if (sub_expr -> Type() -> IsSubclass(expr -> Type()))
                expr = sub_expr;
            else return expr;
        }
        else return expr;
    }

    return expr;
}


void ByteCode::EmitNewArray(int num_dims, TypeSymbol *type)
{
    assert(num_dims > 0);
    if (num_dims == 1)
    {
        TypeSymbol *element_type = type -> ArraySubtype();

        if (control.IsNumeric(element_type) ||
            element_type == control.boolean_type)
        {
            PutOp(OP_NEWARRAY);
            PutU1(element_type == control.boolean_type ? 4
                  : element_type == control.char_type ? 5
                  : element_type == control.float_type ? 6
                  : element_type == control.double_type ? 7
                  : element_type == control.byte_type ? 8
                  : element_type == control.short_type ? 9
                  : element_type == control.int_type ? 10
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
        ChangeStack(1 - num_dims);
    }
}


//
// POST_UNARY
//
int ByteCode::EmitPostUnaryExpression(AstPostUnaryExpression *expression, bool need_value)
{
    int kind = GetLhsKind(expression);

    switch (kind)
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
// load value of field, duplicate, do increment or decrement, then store
// back, leaving original value on top of stack.
//
void ByteCode::EmitPostUnaryExpressionField(int kind, AstPostUnaryExpression *expression, bool need_value)
{
    if (kind == LHS_METHOD)
         ResolveAccess(expression -> expression); // get address and value
    else EmitFieldAccessLhs(expression -> expression);

    TypeSymbol *expression_type = expression -> Type();
    if (need_value)
        PutOp(control.IsDoubleWordType(expression_type) ? OP_DUP2_X1 : OP_DUP_X1);

    if (control.IsSimpleIntegerValueType(expression_type))
    {
        PutOp(OP_ICONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
              ? OP_IADD : OP_ISUB);
        EmitCast(expression_type, control.int_type);
    }
    else if (expression_type == control.long_type)
    {
        PutOp(OP_LCONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
              ? OP_LADD : OP_LSUB);
    }
    else if (expression_type == control.float_type)
    {
        PutOp(OP_FCONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
              ? OP_FADD : OP_FSUB);
    }
    else if (expression_type == control.double_type)
    {
        PutOp(OP_DCONST_1); // load 1.0
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
              ? OP_DADD : OP_DSUB);
    }

    if (kind == LHS_METHOD)
    {
        int stack_words = GetTypeWords(expression_type) + 1;
        PutOp(OP_INVOKESTATIC);
        CompleteCall(expression -> write_method, stack_words);
    }
    else // assert(kind == LHS_FIELD)
    {
        PutOp(OP_PUTFIELD);
        if (control.IsDoubleWordType(expression_type))
            ChangeStack(-1);

        VariableSymbol *sym = (VariableSymbol *) expression -> symbol;
        PutU2(RegisterFieldref(VariableTypeResolution(expression -> expression, sym), sym));
    }
}


//
// AstExpression *expression;
// POST_UNARY on local variable
// load value of variable, duplicate, do increment or decrement, then store
// back, leaving original value on top of stack.
//
void ByteCode::EmitPostUnaryExpressionSimple(int kind, AstPostUnaryExpression *expression, bool need_value)
{
    TypeSymbol *expression_type = expression -> Type();
    if (kind == LHS_LOCAL && expression_type == control.int_type) // can we use IINC ??
    {
        if (need_value)
            LoadVariable(kind, expression);
        PutOpIINC(expression -> symbol -> VariableCast() -> LocalVariableIndex(),
                  expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? 1 : -1);
        return;
    }

    LoadVariable(kind, expression -> expression); // this will also load value needing resolution

    if (need_value)
        PutOp(control.IsDoubleWordType(expression_type) ? OP_DUP2 : OP_DUP);

    if (control.IsSimpleIntegerValueType(expression_type))
    {
        PutOp(OP_ICONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
              ? OP_IADD : OP_ISUB);
        EmitCast(expression_type, control.int_type);
    }
    else if (expression_type == control.long_type)
    {
        PutOp(OP_LCONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
              ? OP_LADD : OP_LSUB);
    }
    else if (expression_type == control.float_type)
    {
        PutOp(OP_FCONST_1);
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
              ? OP_FADD : OP_FSUB);
    }
    else if (expression_type == control.double_type)
    {
        PutOp(OP_DCONST_1); // load 1.0
        PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
              ? OP_DADD : OP_DSUB);
    }

    if (kind == LHS_METHOD)
    {
         int stack_words = GetTypeWords(expression_type);
         PutOp(OP_INVOKESTATIC);
         CompleteCall(expression -> write_method, stack_words);
    }
    else StoreVariable(kind, expression -> expression);
}


//
// Post Unary for which operand is array element
// assignment for which lhs is array element
//    AstExpression *expression;
//
void ByteCode::EmitPostUnaryExpressionArray(AstPostUnaryExpression *expression, bool need_value)
{
    EmitArrayAccessLhs((AstArrayAccess *) expression -> expression); // lhs must be array access
    PutOp(OP_DUP2); // save array base and index for later store

    TypeSymbol *expression_type = expression -> Type();
    if (expression_type == control.int_type)
    {
         PutOp(OP_IALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP_X2);
         PutOp(OP_ICONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
               ? OP_IADD : OP_ISUB);
         PutOp(OP_IASTORE);
    }
    else if (expression_type == control.byte_type )
    {
         PutOp(OP_BALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP_X2);
         PutOp(OP_ICONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
               ? OP_IADD : OP_ISUB);
         PutOp(OP_I2B);
         PutOp(OP_BASTORE);
    }
    else if (expression_type == control.char_type )
    {
         PutOp(OP_CALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP_X2);
         PutOp(OP_ICONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
               ? OP_IADD : OP_ISUB);
         PutOp(OP_I2C);
         PutOp(OP_CASTORE);
    }
    else if (expression_type == control.short_type)
    {
         PutOp(OP_SALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP_X2);
         PutOp(OP_ICONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
               ? OP_IADD : OP_ISUB);
         PutOp(OP_I2S);
         PutOp(OP_SASTORE);
    }
    else if (expression_type == control.long_type)
    {
         PutOp(OP_LALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP2_X2);
         PutOp(OP_LCONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
               ? OP_LADD : OP_LSUB);
         PutOp(OP_LASTORE);
    }
    else if (expression_type == control.float_type)
    {
         PutOp(OP_FALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP_X2);
         PutOp(OP_FCONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
               ? OP_FADD : OP_FSUB);
         PutOp(OP_FASTORE);
    }
    else if (expression_type == control.double_type)
    {
         PutOp(OP_DALOAD);
         if (need_value) // save value below saved array base and index
             PutOp(OP_DUP2_X2);
         PutOp(OP_DCONST_1);
         PutOp(expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS
               ? OP_DADD : OP_DSUB);
         PutOp(OP_DASTORE);
    }
    else assert(false && "unsupported postunary type");
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

            assert((control.IsSimpleIntegerValueType(type) ||
                    type == control.long_type ||
                    type == control.float_type ||
                    type == control.double_type) && "unary minus on unsupported type");

            PutOp(control.IsSimpleIntegerValueType(type) ? OP_INEG
                  : type == control.long_type ? OP_LNEG
                  : type == control.float_type ? OP_FNEG
                  : OP_DNEG); // double_type
            break;
        case AstPreUnaryExpression::TWIDDLE:
            if (control.IsSimpleIntegerValueType(type))
            {
                EmitExpression(expression -> expression);
                PutOp(OP_ICONST_M1); // -1
                PutOp(OP_IXOR);      // exclusive or to get result
            }
            else if (type == control.long_type)
            {
                EmitExpression(expression -> expression);
                PutOp(OP_LCONST_1); // make -1
                PutOp(OP_LNEG);
                PutOp(OP_LXOR);     // exclusive or to get result
            }
            else assert(false && "unary ~ on unsupported type");
            break;
        case AstPreUnaryExpression::NOT:
            assert(type == control.boolean_type);

            EmitExpression(expression -> expression);
            PutOp(OP_ICONST_1);
            PutOp(OP_IXOR); // !(e) <=> (e)^true
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
    int kind = GetLhsKind(expression);

    switch (kind)
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
}


//
// AstExpression *expression;
// PRE_UNARY on name
// load value of variable, do increment or decrement, duplicate, then store
// back, leaving new value on top of stack.
//
void ByteCode::EmitPreUnaryIncrementExpressionSimple(int kind, AstPreUnaryExpression *expression, bool need_value)
{
    TypeSymbol *type = expression -> Type();
    if (kind == LHS_LOCAL && type == control.int_type)
    {
        PutOpIINC(expression -> symbol -> VariableCast() -> LocalVariableIndex(),
                  expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? 1 : -1);
        if (need_value)
            LoadVariable(kind, expression);
        return;
    }

    LoadVariable(kind, expression -> expression); // will also load value if resolution needed

    if (control.IsSimpleIntegerValueType(type))
    {
        PutOp(OP_ICONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
              ? OP_IADD : OP_ISUB);
        EmitCast(type, control.int_type);
        if (need_value)
            PutOp(OP_DUP);
    }
    else if (type == control.long_type)
    {
        PutOp(OP_LCONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
              ? OP_LADD : OP_LSUB);
        if (need_value)
            PutOp(OP_DUP2);
    }
    else if (type == control.float_type)
    {
        PutOp(OP_FCONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
              ? OP_FADD : OP_FSUB);
        if (need_value)
            PutOp(OP_DUP);
    }
    else if (type == control.double_type)
    {
        PutOp(OP_DCONST_1); // load 1.0
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
              ? OP_DADD : OP_DSUB);
        if (need_value)
            PutOp(OP_DUP2);
    }

    if (kind == LHS_METHOD)
    {
        int stack_words = GetTypeWords(type);
        PutOp(OP_INVOKESTATIC);
        CompleteCall(expression -> write_method, stack_words);
    }
    else StoreVariable(kind, expression -> expression);
}


//
// Post Unary for which operand is array element
// assignment for which lhs is array element
//    AstExpression *expression;
//
void ByteCode::EmitPreUnaryIncrementExpressionArray(AstPreUnaryExpression *expression, bool need_value)
{
    EmitArrayAccessLhs((AstArrayAccess *) expression -> expression); // lhs must be array access

    PutOp(OP_DUP2); // save array base and index for later store

    TypeSymbol *type = expression -> Type();
    if (type == control.int_type)
    {
         PutOp(OP_IALOAD);
         PutOp(OP_ICONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
               ? OP_IADD : OP_ISUB);
         if (need_value)
             PutOp(OP_DUP_X2);
         PutOp(OP_IASTORE);
    }
    else if (type == control.byte_type)
    {
         PutOp(OP_BALOAD);
         PutOp(OP_ICONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
               ? OP_IADD : OP_ISUB);
         PutOp(OP_I2B);
         if (need_value)
             PutOp(OP_DUP_X2);
         PutOp(OP_BASTORE);
    }
    else if (type == control.char_type)
    {
         PutOp(OP_CALOAD);
         PutOp(OP_ICONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
               ? OP_IADD : OP_ISUB);
         PutOp(OP_I2C);
         if (need_value)
             PutOp(OP_DUP_X2);
         PutOp(OP_CASTORE);
    }
    else if (type == control.short_type)
    {
         PutOp(OP_SALOAD);
         PutOp(OP_ICONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
               ? OP_IADD : OP_ISUB);
         PutOp(OP_I2S);
         if (need_value)
             PutOp(OP_DUP_X2);
         PutOp(OP_SASTORE);
    }
    else if (type == control.long_type)
    {
         PutOp(OP_LALOAD);
         PutOp(OP_LCONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
               ? OP_LADD : OP_LSUB);
         if (need_value)
             PutOp(OP_DUP2_X2);
         PutOp(OP_LASTORE);
    }
    else if (type == control.float_type)
    {
         PutOp(OP_FALOAD);
         PutOp(OP_FCONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
               ? OP_FADD : OP_FSUB);
         if (need_value)
             PutOp(OP_DUP_X2);
         PutOp(OP_FASTORE);
    }
    else if (type == control.double_type)
    {
         PutOp(OP_DALOAD);
         PutOp(OP_DCONST_1);
         PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
               ? OP_DADD : OP_DSUB);
         if (need_value)
             PutOp(OP_DUP2_X2);
         PutOp(OP_DASTORE);
    }
    else assert(false && "unsupported PreUnary type");
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
        EmitFieldAccessLhs(expression -> expression);

    TypeSymbol *expression_type = expression -> Type();
    if (control.IsSimpleIntegerValueType(expression_type))
    {
        PutOp(OP_ICONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
              ? OP_IADD : OP_ISUB);
        EmitCast(expression_type, control.int_type);
        if (need_value)
            PutOp(OP_DUP_X1);
    }
    else if (expression_type == control.long_type)
    {
        PutOp(OP_LCONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
              ? OP_LADD : OP_LSUB);
        if (need_value)
            PutOp(OP_DUP2_X1);
    }
    else if (expression_type == control.float_type)
    {
        PutOp(OP_FCONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
              ? OP_FADD : OP_FSUB);
        if (need_value)
            PutOp(OP_DUP_X1);
    }
    else if (expression_type == control.double_type)
    {
        PutOp(OP_DCONST_1);
        PutOp(expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS
              ? OP_DADD : OP_DSUB);
        if (need_value)
            PutOp(OP_DUP2_X1);
    }
    else assert(false && "unsupported PreUnary type");

    if (kind == LHS_METHOD)
    {
        int stack_words = GetTypeWords(expression_type) + 1;
        PutOp(OP_INVOKESTATIC);
        CompleteCall(expression -> write_method, stack_words);
    }
    else
    {
        PutOp(OP_PUTFIELD);
        if (control.IsDoubleWordType(expression_type))
            ChangeStack(-1);

        VariableSymbol *sym = (VariableSymbol *) expression -> symbol;
        PutU2(RegisterFieldref(VariableTypeResolution(expression -> expression, sym), sym));
    }
}


void ByteCode::EmitThisInvocation(AstThisCall *this_call)
{
    //
    // THIS_CALL
    //    AstExpression *method;
    //    AstList *arguments;
    // A call to another constructor (THIS_CALL) or super constructor
    // (SUPER_CALL) results in the same sort of generated code, as the
    // semantic analysis has resolved the proper constructor to be invoked.
    //
    PutOp(OP_ALOAD_0); // load 'this'

    int stack_words = 0; // words on stack needed for arguments

    for (int i = 0; i < this_call -> NumLocalArguments(); i++)
        stack_words += EmitExpression((AstExpression *) this_call -> LocalArgument(i));

    for (int k = 0; k < this_call -> NumArguments(); k++)
        stack_words += EmitExpression((AstExpression *) this_call -> Argument(k));

    PutOp(OP_INVOKESPECIAL);
    ChangeStack(-stack_words);

    PutU2(RegisterMethodref(unit_type -> fully_qualified_name,
                            this_call -> symbol -> ExternalIdentity() -> Utf8_literal,
                            this_call -> symbol -> signature));
}


void ByteCode::EmitSuperInvocation(AstSuperCall *super_call)
{
    PutOp(OP_ALOAD_0); // load 'this'

    int stack_words = 0; // words on stack needed for arguments
    if (super_call -> base_opt)
    {
        stack_words += EmitExpression(super_call -> base_opt);
        EmitCheckForNull(super_call -> base_opt);
    }

    for (int i = 0; i < super_call -> NumLocalArguments(); i++)
        stack_words += EmitExpression((AstExpression *) super_call -> LocalArgument(i));

    if (super_call -> NeedsExtraNullArgument())
    {
        PutOp(OP_ACONST_NULL);
        stack_words += 1;
    }

    for (int k = 0; k < super_call -> NumArguments(); k++)
        stack_words += EmitExpression((AstExpression *) super_call -> Argument(k));

    PutOp(OP_INVOKESPECIAL);
    ChangeStack(-stack_words);

    PutU2(RegisterMethodref(unit_type -> super -> fully_qualified_name,
                            super_call -> symbol -> ExternalIdentity() -> Utf8_literal,
                            super_call -> symbol -> signature));
}


//
//  Methods for string concatenation
//
void ByteCode::ConcatenateString(AstBinaryExpression *expression)
{
    //
    // Generate code to concatenate strings, by generating a string buffer
    // and appending the arguments before calling toString, i.e.,
    //  s1+s2
    // compiles to
    //  new StringBuffer().append(s1).append(s2).toString();
    // Use recursion to share a single buffer where possible.
    //
    AstExpression *left_expr = StripNops(expression -> left_expression);
    if (left_expr -> Type() == control.String() &&
	left_expr -> BinaryExpressionCast())
    {
        ConcatenateString((AstBinaryExpression *) left_expr);
    }
    else
    {
        PutOp(OP_NEW);
        PutU2(RegisterClass(control.StringBuffer() -> fully_qualified_name));
        PutOp(OP_DUP);
        if (left_expr -> IsConstant())
        {
            //
            // Optimizations: if the left term is "", just append the right
            // term. Otherwise, use new StringBuffer(String) on the left term.
            // This is not normally safe, as new StringBuffer(null) throws
            // a NullPointerException, but constants are non-null!
            //
            assert(left_expr -> Type() == control.String() &&
                   ! expression -> right_expression -> IsConstant());
            if (((Utf8LiteralValue *) left_expr -> value) -> length == 0)
            {
                PutOp(OP_INVOKESPECIAL);
                PutU2(RegisterLibraryMethodref(control.StringBuffer_InitMethod()));
            }
            else
            {
                EmitExpression(left_expr);
                PutOp(OP_INVOKESPECIAL);
                PutU2(RegisterLibraryMethodref(control.StringBuffer_InitWithStringMethod()));
                ChangeStack(-1); // account for the argument
            }
        }
        else
        {
            PutOp(OP_INVOKESPECIAL);
            PutU2(RegisterLibraryMethodref(control.StringBuffer_InitMethod()));
            AppendString(left_expr);
        }
    }

    AppendString(expression -> right_expression);
}


void ByteCode::AppendString(AstExpression *expression)
{
    expression = StripNops(expression);
    TypeSymbol *type = expression -> Type();

    if (expression -> IsConstant())
    {
        assert(type == control.String());
        Utf8LiteralValue *value = (Utf8LiteralValue *) expression -> value;

        if (value -> length == 0)
            return;  // Optimization: do nothing when appending "".
        LoadConstantAtIndex(RegisterString(value));
    }
    else
    {
        AstBinaryExpression *binary_expression = expression -> BinaryExpressionCast();
        if (binary_expression && type == control.String())
        {
            assert(binary_expression -> binary_tag == AstBinaryExpression::PLUS);
            AppendString(binary_expression -> left_expression);
            AppendString(binary_expression -> right_expression);
            return;
        }

        EmitExpression(expression);
    }

    EmitStringAppendMethod(type);
}


void ByteCode::EmitStringAppendMethod(TypeSymbol *type)
{
    //
    // Find appropriate append routine to add to string buffer. Do not use
    // append(char[]), because that inserts the contents instead of the
    // correct char[].toString(). Treating null as a String is slightly more
    // efficient than as an Object.
    //
    MethodSymbol *append_method =
        (type == control.char_type ? control.StringBuffer_append_charMethod()
         : type == control.boolean_type ? control.StringBuffer_append_booleanMethod()
         : (type == control.int_type || type == control.short_type ||
            type == control.byte_type) ? control.StringBuffer_append_intMethod()
         : type == control.long_type ? control.StringBuffer_append_longMethod()
         : type == control.float_type ? control.StringBuffer_append_floatMethod()
         : type == control.double_type ? control.StringBuffer_append_doubleMethod()
         : (type == control.String() ||
            type == control.null_type) ? control.StringBuffer_append_stringMethod()
         : IsReferenceType(type) ? control.StringBuffer_append_objectMethod()
         : control.StringBuffer_InitMethod()); // for assertion

    assert(append_method != control.StringBuffer_InitMethod() && "unable to find method for string buffer concatenation");

    PutOp(OP_INVOKEVIRTUAL);
    if (control.IsDoubleWordType(type))
        ChangeStack(-1);
    PutU2(RegisterLibraryMethodref(append_method));
}


#ifdef JIKES_DEBUG
static void op_trap()
{
    int i = 0; // used for debugger trap
    i++;       // avoid compiler warnings about unused variable
}
#endif


ByteCode::ByteCode(TypeSymbol *unit_type) : ClassFile(unit_type),
                                            control(unit_type -> semantic_environment -> sem -> control),
                                            semantic(*unit_type -> semantic_environment -> sem),

                                            string_overflow(false),
                                            library_method_not_found(false),

                                            double_constant_pool_index(NULL),
                                            integer_constant_pool_index(NULL),
                                            long_constant_pool_index(NULL),
                                            float_constant_pool_index(NULL),
                                            string_constant_pool_index(NULL),

                                            utf8_constant_pool_index(segment_pool, control.Utf8_pool.symbol_pool.Length()),
                                            class_constant_pool_index(segment_pool, control.Utf8_pool.symbol_pool.Length()),

                                            name_and_type_constant_pool_index(NULL),
                                            fieldref_constant_pool_index(NULL),
                                            methodref_constant_pool_index(NULL)
{
#ifdef JIKES_DEBUG
    if (! control.option.nowrite)
        control.class_files_written++;
#endif

    SetFlags(unit_type -> Flags());

    //
    // TODO: Are these appropriately taken care of earlier?
    // The flags for 'static' and 'protected' are set only for the inner
    // classes attribute, not for the class, as described in page 25
    // of the inner classes document.
    //
    if (unit_type -> ACC_PROTECTED())
    {
        this -> ResetACC_PROTECTED();
        this -> SetACC_PUBLIC();
    }
    this -> ResetACC_STATIC();
    this -> ResetACC_PRIVATE();
    this -> SetACC_SUPER(); // must always set ACC_SUPER for class (cf page 96 of revised JVM Spec)

    magic = 0xcafebabe;
    major_version = 45;             // use Sun JDK 1.0 version numbers
    minor_version = 3;
    constant_pool.Next() = NULL;
    this_class = RegisterClass(unit_type -> fully_qualified_name);

    super_class = (unit_type -> super ? RegisterClass(unit_type -> super -> fully_qualified_name) : 0);

    for (int k = 0; k < unit_type -> NumInterfaces(); k++)
        interfaces.Next() = RegisterClass(unit_type -> Interface(k) -> fully_qualified_name);
}


//
//  Methods for manipulating labels
//
void ByteCode::DefineLabel(Label& lab)
{
    assert((! lab.defined) && "duplicate label definition");

    lab.defined = true;
    lab.definition = code_attribute -> CodeLength();
}


//
// patch all uses to have proper value. This requires that
// all labels be freed at some time.
//
void ByteCode::CompleteLabel(Label& lab)
{
    //
    // TODO: when the offset is to the next instruction, the goto is useless
    // and can be optimized away (but what if it is at the end of a method...?)
    //
    if (lab.uses.Length() > 0)
    {
        assert((lab.defined) && "label used but with no definition");

        //
        // Sanity check - when completing method, make sure nothing jumps out
        // of the method
        //
        if (lab.definition > last_label_pc)
            last_label_pc = lab.definition;

        //
        // patch byte code reference to label to reflect its definition
        // as 16-bit signed offset.
        //
        for (int i = 0; i < lab.uses.Length(); i++)
        {
            unsigned int luse = lab.uses[i].use_offset;
            int start = luse - lab.uses[i].op_offset,
                offset = lab.definition - start;
            if (lab.uses[i].use_length == 2) // here if short offset
            {
                assert(offset < 32768 && "needed longer branch offset");
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
    }

    //
    // reset in case label is used again.
    //
    lab.Reset();
}


void ByteCode::UseLabel(Label &lab, int _length, int _op_offset)
{
    int lab_index = lab.uses.NextIndex();
    lab.uses[lab_index].use_length = _length;
    lab.uses[lab_index].op_offset = _op_offset;
    lab.uses[lab_index].use_offset = code_attribute -> CodeLength();

    //
    // fill next length bytes with zero; will be filled in with proper value
    // when label completed
    //
    for (int i = 0; i < lab.uses[lab_index].use_length; i++)
        code_attribute -> AddCode(0);
}


void ByteCode::LoadLocal(int varno, TypeSymbol *type)
{
    if (control.IsSimpleIntegerValueType(type) || type == control.boolean_type)
    {
         if (varno <= 3)
              PutOp(OP_ILOAD_0 + varno);
         else PutOpWide(OP_ILOAD, varno);
    }
    else if (type == control.long_type)
    {
         if (varno <= 3)
              PutOp(OP_LLOAD_0 + varno);
         else PutOpWide(OP_LLOAD, varno);
    }
    else if (type == control.float_type)
    {
         if (varno <= 3)
              PutOp(OP_FLOAD_0 + varno);
         else PutOpWide(OP_FLOAD, varno);
    }
    else if (type == control.double_type)
    {
         if (varno <= 3)
              PutOp(OP_DLOAD_0 + varno);
         else PutOpWide(OP_DLOAD, varno);
    }
    else // assume reference
    {
         if (varno <= 3)
              PutOp(OP_ALOAD_0 + varno);
         else PutOpWide(OP_ALOAD, varno);
    }
}


//
// see if can load without using LDC even if have literal index; otherwise
// generate constant pool entry if one has not yet been generated.
//
//
void ByteCode::LoadLiteral(LiteralValue *litp, TypeSymbol *type)
{
    if (control.IsSimpleIntegerValueType(type) || type == control.boolean_type) // load literal using literal value
    {
        IntLiteralValue *vp = (IntLiteralValue *) litp;
        LoadImmediateInteger(vp -> value);
    }
    else if (type == control.String()) // register index as string if this has not yet been done
    {
        LoadConstantAtIndex(RegisterString((Utf8LiteralValue *) litp));
    }
    else if (type == control.long_type)
    {
        LongLiteralValue *vp = (LongLiteralValue *) litp;
        if (vp -> value == 0)
             PutOp(OP_LCONST_0);
        else if (vp -> value == 1)
             PutOp(OP_LCONST_1);
        else
        {
             PutOp(OP_LDC2_W);
             PutU2(RegisterLong(vp));
        }
    }
    else if (type == control.float_type)
    {
        FloatLiteralValue *vp = (FloatLiteralValue *) litp;
        IEEEfloat val = vp -> value;
        if (val.Word() == 0) // if float 0.0
             PutOp(OP_FCONST_0);
        else if (val.Word() == 0x3f800000) // if float 1.0
             PutOp(OP_FCONST_1);
        else if (val.Word() == 0x40000000) // if float 2.0
             PutOp(OP_FCONST_2);
        else LoadConstantAtIndex(RegisterFloat(vp));
    }
    else if (type == control.double_type)
    {
        DoubleLiteralValue *vp = (DoubleLiteralValue *) litp;
        IEEEdouble val = vp -> value;
        if (val.HighWord() == 0 && val.LowWord() == 0)
             PutOp(OP_DCONST_0);
        else if (val.HighWord() == 0x3ff00000 && val.LowWord() == 0x00000000) // if double 1.0
             PutOp(OP_DCONST_1);
        else
        {
             PutOp(OP_LDC2_W);
             PutU2(RegisterDouble(vp));
        }
    }
    else assert(false && "unsupported constant kind");
}


void ByteCode::LoadImmediateInteger(int val)
{
    if (val >= -1 && val <= 5)
        PutOp(OP_ICONST_0 + val); // exploit opcode encoding
    else if (val >= -128 && val < 128)
    {
        PutOp(OP_BIPUSH);
        PutU1(val);
    }
    else if (val >= -32768 && val < 32768)
    {
        //
        // For a short value, look to see if it is already in the constant
        // pool. In such a case, ldc is two bytes, while sipush is three, so
        // we emit a smaller classfile with no penalty to a good JIT. Note
        // that ldc_w does not buy us anything, however.
        //
        u2 index = FindInteger(control.int_pool.Find(val));
        if (index == 0 || index > 255) 
        {
            PutOp(OP_SIPUSH);
            PutU2(val);
        }
        else LoadConstantAtIndex(index);
    }
    // Outside the range of sipush, we must use the constant pool.
    else LoadConstantAtIndex(RegisterInteger(control.int_pool.FindOrInsert(val)));
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
}


int ByteCode::LoadVariable(int kind, AstExpression *expr)
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
        {
            if (sym -> ACC_STATIC())
            {
                //
                // If the access is qualified by an arbitrary base
                // expression, evaluate it for side effects.
                //
                if (expr -> FieldAccessCast())
                    EmitExpression(((AstFieldAccess *) expr) -> base, false);

                PutOp(OP_GETSTATIC);
                ChangeStack(GetTypeWords(expression_type) - 1);
            }
            else
            {
                PutOp(OP_ALOAD_0); // get address of "this"
                PutOp(OP_GETFIELD);
                ChangeStack(GetTypeWords(expression_type) - 1);
            }

            PutU2(RegisterFieldref(VariableTypeResolution(expr, sym), sym));
        }
        break;
    default:
        assert(false && "LoadVariable bad kind");
    }

    return GetTypeWords(expression_type);
}


int ByteCode::LoadArrayElement(TypeSymbol *type)
{
    PutOp((type == control.byte_type ||
           type == control.boolean_type) ? OP_BALOAD
          : type == control.short_type ? OP_SALOAD
          : type == control.int_type ? OP_IALOAD
          : type == control.long_type ? OP_LALOAD
          : type == control.char_type ? OP_CALOAD
          : type == control.float_type ? OP_FALOAD
          : type == control.double_type ? OP_DALOAD
          : OP_AALOAD); // assume reference

    return GetTypeWords(type);
}


void ByteCode::StoreArrayElement(TypeSymbol *type)
{
    PutOp((type == control.byte_type ||
           type == control.boolean_type) ? OP_BASTORE
          : type == control.short_type ? OP_SASTORE
          : type == control.int_type ? OP_IASTORE
          : type == control.long_type ? OP_LASTORE
          : type == control.char_type ? OP_CASTORE
          : type == control.float_type ? OP_FASTORE
          : type == control.double_type ? OP_DASTORE
          : OP_AASTORE); // assume reference
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
        ChangeStack(1 - GetTypeWords(expression_type));
    }
    else
    {
        PutOp(OP_PUTFIELD);
        ChangeStack(1 - GetTypeWords(expression_type));
    }

    PutU2(RegisterFieldref(VariableTypeResolution(expression, sym), sym));
}


void ByteCode::StoreLocal(int varno, TypeSymbol *type)
{
    if (control.IsSimpleIntegerValueType(type) || type == control.boolean_type)
    {
         if (varno <= 3)
              PutOp(OP_ISTORE_0 + varno);
         else PutOpWide(OP_ISTORE, varno);
    }
    else if (type == control.long_type)
    {
         if (varno <= 3)
              PutOp(OP_LSTORE_0 + varno);
         else PutOpWide(OP_LSTORE, varno);
    }
    else if (type == control.float_type)
    {
         if (varno <= 3)
              PutOp(OP_FSTORE_0 + varno);
         else PutOpWide(OP_FSTORE, varno);
    }
    else if (type == control.double_type)
    {
         if (varno <= 3)
              PutOp(OP_DSTORE_0 + varno);
         else PutOpWide(OP_DSTORE, varno);
    }
    else // assume reference
    {
         if (varno <= 3)
              PutOp(OP_ASTORE_0 + varno);
         else PutOpWide(OP_ASTORE, varno);
    }
}


void ByteCode::StoreVariable(int kind, AstExpression *expr)
{
    VariableSymbol *sym = (VariableSymbol *) expr -> symbol;
    switch (kind)
    {
    case LHS_LOCAL:
        StoreLocal(sym -> LocalVariableIndex(), sym -> Type());
        break;
    case LHS_FIELD:
    case LHS_STATIC:
        {
            if (sym -> ACC_STATIC())
            {
                PutOp(OP_PUTSTATIC);
                ChangeStack(1 - GetTypeWords(expr -> Type()));
            }
            else
            {
                PutOp(OP_ALOAD_0); // get address of "this"
                PutOp(OP_PUTFIELD);
                ChangeStack(1 - GetTypeWords(expr -> Type()));
            }

            PutU2(RegisterFieldref(VariableTypeResolution(expr, sym), sym));
        }
        break;
    default:
        assert(false && "StoreVariable bad kind");
    }
}


//
// finish off code by writing SourceFile attribute
// and InnerClasses attribute (if appropriate)
//
void ByteCode::FinishCode(TypeSymbol *type)
{
    //
    // Only output SourceFile attribute if -g:source is enabled.
    //
    if (control.option.g & JikesOption::SOURCE)
        attributes.Next() = new SourceFile_attribute(RegisterUtf8(control.SourceFile_literal),
                                                     RegisterUtf8(type -> file_symbol -> FileNameLiteral()));

    if (type == NULL)
        return; // return if interface type

    if (type -> IsLocal() || type -> IsNested() ||
        type -> NumNestedTypes() > 0) // here to generate InnerClasses attribute
    {
        inner_classes_attribute = new InnerClasses_attribute(RegisterUtf8(control.InnerClasses_literal));

        //
        // need to build chain from this type to its owner all the way to the
        // containing type and then write that out in reverse order (so
        // containing type comes first), and then write out an entry for each
        // immediately contained type
        //
        Tuple<TypeSymbol *> owners;
        for (TypeSymbol *t = type;
             t && t != type -> outermost_type;
             t = t -> ContainingType())
        {
            owners.Next() = t;
        }

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
                                                     outer -> Flags());
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
                                                     nested -> Flags());
        }

        attributes.Next() = inner_classes_attribute;
    }

    if (type -> IsDeprecated())
        attributes.Next() = CreateDeprecatedAttribute();
}


void ByteCode::PutOp(unsigned char opc)
{
#ifdef JIKES_DEBUG
    if (control.option.debug_trap_op > 0 &&
        code_attribute -> CodeLength() == control.option.debug_trap_op)
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
    case OP_INVOKESPECIAL: break;
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
}

void ByteCode::PutOpWide(unsigned char opc, u2 var)
{
    if (var <= 255)  // if can use standard form
    {
        PutOp(opc);
        PutU1(var);
    }
    else // need wide form
    {
        PutOp(OP_WIDE);
        PutOp(opc);
        PutU2(var);
    }
}

void ByteCode::PutOpIINC(u2 var, int val)
{
    if (var <= 255 && (val >= -128 && val <= 127))  // if can use standard form
    {
        PutOp(OP_IINC);
        PutU1(var);
        PutU1(val);
    }
    else // else need wide form
    {
        PutOp(OP_WIDE);
        PutOp(OP_IINC);
        PutU2(var);
        PutU2(val);
    }
}

void ByteCode::ChangeStack(int i)
{
    stack_depth += i;
    assert(stack_depth >= 0);

    if (i > 0 && stack_depth > max_stack)
        max_stack = stack_depth;

#ifdef TRACE_STACK_CHANGE
    Coutput << "stack change: pc " << last_op_pc << " change " << i
            << "  stack_depth " << stack_depth << "  max_stack: " << max_stack
            << endl;
#endif
}


#ifdef JIKES_DEBUG
void ByteCode::PrintCode()
{
    Coutput << "magic " << hex << magic << dec
            << " major_version " << (unsigned) major_version
            << " minor_version " << (unsigned) minor_version << endl;
    AccessFlags::Print();
    Coutput << endl
            << " this_class " << (unsigned) this_class << "  super_class "
            << (unsigned) super_class << endl
            << " constant_pool: " << constant_pool.Length() << endl;

    {
        for (int i = 1; i < constant_pool.Length(); i++)
        {
            Coutput << "  " << i << "  ";
            constant_pool[i] -> Print(constant_pool);
            if (constant_pool[i] -> Tag() == CONSTANT_Long || constant_pool[i] -> Tag() == CONSTANT_Double)
                i++; // skip the next entry for eight-byte constants
        }
    }

    Coutput << "  interfaces " << interfaces.Length() <<": ";
    {
        for (int i = 0; i < interfaces.Length(); i++)
             Coutput << "  " << (int) interfaces[i];
        Coutput << endl;
    }

    Coutput << "  fields " << fields.Length() << ": ";
    {
        for (int i = 0; i < fields.Length(); i++)
        {
            Coutput << "field " << i << endl;
            fields[i].Print(constant_pool);
        }
    }

    Coutput << " methods length " << methods.Length() << endl;
    {
        for (int i = 0; i < methods.Length(); i++)
        {
            Coutput << "method " << i << endl;
            methods[i].Print(constant_pool);
        }
    }

    Coutput << " attributes length " << attributes.Length() << endl;
    {
        for (int i = 0; i < attributes.Length(); i++)
        {
            Coutput << "attribute " << i << endl;
            attributes[i] -> Print(constant_pool);
        }
    }
    Coutput << endl;
}
#endif

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

