// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, 1999, 2000, 2001, 2002 International Business
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
    AstClassDeclaration *class_decl =
        unit_type -> declaration -> ClassDeclarationCast();
    AstClassBody *class_body =
        class_decl ? class_decl -> class_body
        : (((AstClassInstanceCreationExpression *) unit_type -> declaration) ->
           class_body_opt);
    int i;

    //
    // Process static variables.
    //
    Tuple<AstVariableDeclarator *> initialized_static_fields(unit_type -> NumVariableSymbols());
    for (i = 0; i < class_body -> NumClassVariables(); i++)
    {
        AstFieldDeclaration *field_decl = class_body -> ClassVariable(i);
        for (int vi = 0; vi < field_decl -> NumVariableDeclarators(); vi++)
        {
            AstVariableDeclarator *variable_declarator =
                field_decl -> VariableDeclarator(vi);
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

    //
    // Process instance variables.
    //
    Tuple<AstVariableDeclarator *> initialized_instance_fields(unit_type -> NumVariableSymbols());
    for (i = 0; i < class_body -> NumInstanceVariables(); i++)
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

    //
    // Supply needed field declaration for this$0 and any additional local
    // shadow parameters.
    //
    if (unit_type -> EnclosingType())
        DeclareField(unit_type -> EnclosingInstance());
    for (i = 0; i < unit_type -> NumConstructorParameters(); i++)
        DeclareField(unit_type -> ConstructorParameter(i));

    //
    // Supply needed field declarations for "class" identifiers (used for
    // X.class literals) if present.
    //
    for (i = 0; i < unit_type -> NumClassLiterals(); i++)
        DeclareField(unit_type -> ClassLiteral(i));

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
    for (i = 0; i < class_body -> NumMethods(); i++)
    {
        AstMethodDeclaration *method = class_body -> Method(i);
        if (method -> method_symbol)
        {
            int method_index = methods.NextIndex(); // index for method

            BeginMethod(method_index, method -> method_symbol);
            AstBlock *method_block = method -> method_body -> BlockCast();
            if (method_block) // not an abstract method ?
                EmitBlockStatement(method_block);
            EndMethod(method_index, method -> method_symbol);
        }
    }

    //
    // compile any private access methods
    //
    for (i = 0; i < unit_type -> NumPrivateAccessMethods(); i++)
    {
        int method_index = methods.NextIndex(); // index for method

        MethodSymbol *method_sym = unit_type -> PrivateAccessMethod(i);
        AstMethodDeclaration *method = method_sym -> declaration ->
            MethodDeclarationCast();
        assert(method);
        BeginMethod(method_index, method_sym);
        EmitBlockStatement((AstBlock *) method -> method_body);
        EndMethod(method_index, method_sym);
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
    // Compile the instance initializer.
    //
    bool has_instance_initializer = false;
    if (unit_type -> instance_initializer_method)
    {
        AstMethodDeclaration *declaration = (AstMethodDeclaration *)
            unit_type -> instance_initializer_method -> declaration;
        AstBlock *init_block = (AstBlock *) declaration -> method_body;
        if (! IsNop(init_block))
        {
            int method_index = methods.NextIndex(); // index for method
            BeginMethod(method_index,
                        unit_type -> instance_initializer_method);
            bool abrupt = EmitBlockStatement(init_block);
            if (! abrupt)
                PutOp(OP_RETURN);
            EndMethod(method_index, unit_type -> instance_initializer_method);
            has_instance_initializer = true;
        }
    }

    //
    // Compile all constructors.
    //
    if (class_body -> default_constructor)
        CompileConstructor(class_body -> default_constructor,
                           initialized_instance_fields,
                           has_instance_initializer);
    else
        for (i = 0; i < class_body -> NumConstructors(); i++)
            CompileConstructor(class_body -> Constructor(i),
                               initialized_instance_fields,
                               has_instance_initializer);

    for (i = 0; i < unit_type -> NumPrivateAccessConstructors(); i++)
    {
        MethodSymbol *constructor_sym =
            unit_type -> PrivateAccessConstructor(i);
        AstConstructorDeclaration *constructor =
            constructor_sym -> declaration -> ConstructorDeclarationCast();
        CompileConstructor(constructor, initialized_instance_fields,
                           has_instance_initializer);
    }

    //
    // Compile the static initializer.
    //
    if (unit_type -> static_initializer_method)
    {
        AstMethodDeclaration *declaration = (AstMethodDeclaration *)
            unit_type -> static_initializer_method -> declaration;
        AstBlock *init_block = (AstBlock *) declaration -> method_body;
        if (assert_variable || ! IsNop(init_block))
        {
            int method_index = methods.NextIndex(); // index for method
            BeginMethod(method_index, unit_type -> static_initializer_method);
            if (assert_variable)
                GenerateAssertVariableInitializer(unit_type -> outermost_type,
                                                  assert_variable);
            bool abrupt = EmitBlockStatement(init_block);
            if (! abrupt)
                PutOp(OP_RETURN);
            EndMethod(method_index, unit_type -> static_initializer_method);
        }
    }

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
        AstClassDeclaration *class_declaration =
            unit_type -> declaration -> ClassDeclarationCast();
        AstInterfaceDeclaration *interface_declaration =
            unit_type -> declaration -> InterfaceDeclarationCast();
        int n = (class_declaration ? class_declaration -> NumInterfaces()
                 : interface_declaration -> NumInterfaceMemberDeclarations());
        Ast *left = (class_declaration
                     ? (Ast *) class_declaration -> Interface(0)
                     : interface_declaration -> InterfaceMemberDeclaration(0)),
            *right = (class_declaration
                      ? (Ast *) class_declaration -> Interface(n - 1)
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
#endif // JIKES_DEBUG
}


void ByteCode::CompileInterface()
{
    AstInterfaceDeclaration *interface_decl =
        unit_type -> declaration -> InterfaceDeclarationCast();
    int i;

    Tuple<AstVariableDeclarator *> initialized_fields(unit_type -> NumVariableSymbols());
    for (i = 0; i < interface_decl -> NumClassVariables(); i++)
    {
        AstFieldDeclaration *field_decl = interface_decl -> ClassVariable(i);

        for (int vi = 0; vi < field_decl -> NumVariableDeclarators(); vi++)
        {
            AstVariableDeclarator *variable_declarator =
                field_decl -> VariableDeclarator(vi);
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

    //
    // Process all method members
    //
    for (i = 0; i < interface_decl -> NumMethods(); i++)
    {
        AstMethodDeclaration *method = interface_decl -> Method(i);
        if (method -> method_symbol)
        {
            int method_index = methods.NextIndex();

            BeginMethod(method_index, method -> method_symbol);
            EndMethod(method_index, method -> method_symbol);
        }
    }

    //
    // If this interface contained field with initial value
    //
    if (unit_type -> static_initializer_method)
    {
        AstMethodDeclaration *declaration = (AstMethodDeclaration *)
            unit_type -> static_initializer_method -> declaration;
        AstBlock *init_block = (AstBlock *) declaration -> method_body;

        int method_index = methods.NextIndex(); // index for method
        BeginMethod(method_index, unit_type -> static_initializer_method);
        bool abrupt = EmitBlockStatement(init_block);
        assert(! abrupt);
        PutOp(OP_RETURN);
        EndMethod(method_index, unit_type -> static_initializer_method);
    }

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
#endif // JIKES_DEBUG
}


//
// initialized_fields is a list of fields needing code to initialize.
//
void ByteCode::CompileConstructor(AstConstructorDeclaration *constructor,
                                  Tuple<AstVariableDeclarator *> &initialized_fields,
                                  bool has_instance_initializer)
{
    MethodSymbol *method_symbol = constructor -> constructor_symbol;
    AstMethodBody *constructor_block = constructor -> constructor_body;

    int method_index = methods.NextIndex(); // index for method
    BeginMethod(method_index, method_symbol);

    //
    // Set up the index to account for this, this$0, and normal parameters,
    // so we know where the local variable shadows begin.
    //
    shadow_parameter_offset = unit_type -> EnclosingType() ? 2 : 1;
    if (unit_type -> NumConstructorParameters() > 0)
    {
        for (int j = 0; j < method_symbol -> NumFormalParameters(); j++)
            shadow_parameter_offset +=
                GetTypeWords(method_symbol -> FormalParameter(j) -> Type());
    }

    if (control.option.target < JikesOption::SDK1_4)
    {
        //
        // Prior to JDK 1.4, VMs incorrectly complained if shadow
        // initialization happened before the superconstructor, even though
        // the JVMS permits it.
        //
        if (constructor_block -> explicit_constructor_opt)
            EmitStatement(constructor_block -> explicit_constructor_opt);
        else
            assert(unit_type == control.Object() &&
                   "A constructor without an explicit constructor invocation");
    }

    //
    // Supply synthetic field initialization unless constructor calls this().
    //
    if (constructor_block -> explicit_constructor_opt &&
        ! constructor_block -> explicit_constructor_opt -> ThisCallCast())
    {
        if (unit_type -> EnclosingType())
        {
            //
            // Initialize this$0
            //
            VariableSymbol *this0_parameter = unit_type -> EnclosingInstance();
            PutOp(OP_ALOAD_0);
            LoadLocal(1, this0_parameter -> Type());
            PutOp(OP_PUTFIELD);
            PutU2(RegisterFieldref(this0_parameter));
        }

        for (int i = 0, index = shadow_parameter_offset;
             i < unit_type -> NumConstructorParameters(); i++)
        {
            VariableSymbol *shadow = unit_type -> ConstructorParameter(i);
            PutOp(OP_ALOAD_0);
            LoadLocal(index, shadow -> Type());
            PutOp(OP_PUTFIELD);
            if (control.IsDoubleWordType(shadow -> Type()))
                ChangeStack(-1);
            PutU2(RegisterFieldref(shadow));
            index += GetTypeWords(shadow -> Type());
        }
    }

    if (control.option.target >= JikesOption::SDK1_4)
    {
        //
        // Since JDK 1.4, VMs correctly allow shadow initialization before
        // the superconstructor, which is necessary to avoid null pointer
        // exceptions with polymorphic calls from the superconstructor.
        //
        if (constructor_block -> explicit_constructor_opt)
            EmitStatement(constructor_block -> explicit_constructor_opt);
        else
            assert(unit_type == control.Object() &&
                   "A constructor without an explicit constructor invocation");
    }

    //
    // Compile instance initializers unless the constructor calls this().
    //
    shadow_parameter_offset = 0;
    if (has_instance_initializer &&
        constructor_block -> explicit_constructor_opt &&
        ! constructor_block -> explicit_constructor_opt -> ThisCallCast())
    {
        PutOp(OP_ALOAD_0);
        PutOp(OP_INVOKESPECIAL);
        CompleteCall(unit_type -> instance_initializer_method, 0);
    }

    EmitBlockStatement(constructor_block);
    EndMethod(method_index, method_symbol);
}


void ByteCode::DeclareField(VariableSymbol *symbol)
{
    int field_index = fields.NextIndex(); // index for field
    TypeSymbol *type = symbol -> Type();
    if (type -> num_dimensions > 255)
    {
        semantic.ReportSemError(SemanticError::ARRAY_OVERFLOW,
                                symbol -> declarator -> LeftToken(),
                                symbol -> declarator -> RightToken());
    }

    fields[field_index].SetFlags(symbol -> Flags());
    fields[field_index].SetNameIndex(RegisterName(symbol ->
                                                  ExternalIdentity()));
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
                     type == control.boolean_type)
                    ? RegisterInteger(DYNAMIC_CAST<IntLiteralValue *>
                                      (symbol -> initial_value))
                    : type == control.String()
                    ? RegisterString(DYNAMIC_CAST<Utf8LiteralValue *>
                                     (symbol -> initial_value))
                    : type == control.float_type
                    ? RegisterFloat(DYNAMIC_CAST<FloatLiteralValue *>
                                    (symbol -> initial_value))
                    : type == control.long_type
                    ? RegisterLong(DYNAMIC_CAST<LongLiteralValue *>
                                   (symbol -> initial_value))
                    : RegisterDouble(DYNAMIC_CAST<DoubleLiteralValue *>
                                     (symbol -> initial_value)));
        u2 attribute_index = RegisterUtf8(control.ConstantValue_literal);
        fields[field_index].AddAttribute(new ConstantValue_attribute(attribute_index, index));
    }

    if (symbol -> IsSynthetic())
        fields[field_index].AddAttribute(CreateSyntheticAttribute());

    if (symbol -> IsDeprecated())
        fields[field_index].AddAttribute(CreateDeprecatedAttribute());
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
#endif // DUMP
#ifdef JIKES_DEBUG
    if (control.option.debug_trace_stack_change)
        Coutput << endl << "Generating method "
                << unit_type -> ContainingPackage() -> PackageName() << '.'
                << unit_type -> ExternalName() << '.' << msym -> Name()
                << msym -> signature -> value << endl;
#endif // JIKES_DEBUG
    MethodInitialization();

    methods[method_index].SetNameIndex(RegisterName(msym ->
                                                    ExternalIdentity()));
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
        Exceptions_attribute *exceptions_attribute =
            new Exceptions_attribute(RegisterUtf8(control.Exceptions_literal));
        for (int i = 0; i < msym -> NumThrows(); i++)
            exceptions_attribute ->
                AddExceptionIndex(RegisterClass(msym -> Throws(i)));
        methods[method_index].AddAttribute(exceptions_attribute);
    }

    //
    // here if need code and associated attributes.
    //
    if (! (msym -> ACC_ABSTRACT() || msym -> ACC_NATIVE()))
    {
        method_stack =
            new MethodStack(msym -> max_block_depth,
                            msym -> block_symbol -> max_variable_index);

        code_attribute =
            new Code_attribute(RegisterUtf8(control.Code_literal),
                               msym -> block_symbol -> max_variable_index);

        line_number = 0;
        line_number_table_attribute = new LineNumberTable_attribute
            (RegisterUtf8(control.LineNumberTable_literal));

        local_variable_table_attribute = (control.option.g & JikesOption::VARS)
            ? (new LocalVariableTable_attribute
               (RegisterUtf8(control.LocalVariableTable_literal)))
            : (LocalVariableTable_attribute *) NULL;
    }

    if (msym -> Type() -> num_dimensions > 255)
    {
        assert(msym -> declaration -> MethodDeclarationCast());
        Ast *type = ((AstMethodDeclaration *) msym -> declaration) -> type;

        semantic.ReportSemError(SemanticError::ARRAY_OVERFLOW,
                                type -> LeftToken(),
                                type -> RightToken());
    }

    VariableSymbol *parameter = NULL;
    for (int i = 0; i < msym -> NumFormalParameters(); i++)
    {
        parameter = msym -> FormalParameter(i);
        if (parameter -> Type() -> num_dimensions > 255)
        {
            semantic.ReportSemError(SemanticError::ARRAY_OVERFLOW,
                                    parameter -> declarator -> LeftToken(),
                                    parameter -> declarator -> RightToken());
        }
    }
    if (parameter)
    {
        int last_parameter_index = parameter -> LocalVariableIndex();
        if (control.IsDoubleWordType(parameter -> Type()))
            last_parameter_index++;
        if (last_parameter_index >= 255)
        {
            assert(msym -> declaration);

            AstMethodDeclaration *method_declaration =
                msym -> declaration -> MethodDeclarationCast();
            AstConstructorDeclaration *constructor_declaration =
                msym -> declaration -> ConstructorDeclarationCast();
            AstMethodDeclarator *declarator = method_declaration
                ? method_declaration -> method_declarator
                : constructor_declaration -> constructor_declarator;

            semantic.ReportSemError(SemanticError::PARAMETER_OVERFLOW,
                                    declarator -> left_parenthesis_token,
                                    declarator -> right_parenthesis_token,
                                    msym -> Header(),
                                    unit_type -> ContainingPackage() -> PackageName(),
                                    unit_type -> ExternalName());
        }
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
                                    msym -> declaration -> LeftToken(),
                                    msym -> declaration -> RightToken(),
                                    msym -> Header(),
                                    unit_type -> ContainingPackage() -> PackageName(),
                                    unit_type -> ExternalName());
        }

        if (max_stack > 65535)
        {
            semantic.ReportSemError(SemanticError::STACK_OVERFLOW,
                                    msym -> declaration -> LeftToken(),
                                    msym -> declaration -> RightToken(),
                                    msym -> Header(),
                                    unit_type -> ContainingPackage() -> PackageName(),
                                    unit_type -> ExternalName());
        }

        if (code_attribute -> CodeLengthExceeded())
        {
            semantic.ReportSemError(SemanticError::CODE_OVERFLOW,
                                    msym -> declaration -> LeftToken(),
                                    msym -> declaration -> RightToken(),
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
        assert((u2) last_label_pc < code_attribute -> CodeLength() ||
               code_attribute -> CodeLength() == 0x0ffff);
        assert(stack_depth == 0);

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
        else
        {
            // line_number_table_attribute not needed, so delete it now
            delete line_number_table_attribute;
        }

        //
        // Debug level -g:vars & not dealing with generated accessed method
        //
        if ((control.option.g & JikesOption::VARS)
            && (! msym -> accessed_member)
            && (msym -> Identity() != control.class_name_symbol))
        {
            if (! msym -> ACC_STATIC()) // add 'this' to local variable table
            {
                local_variable_table_attribute ->
                    AddLocalVariable(0, code_attribute -> CodeLength(),
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
                local_variable_table_attribute ->
                    AddLocalVariable(0, code_attribute -> CodeLength(),
                                     RegisterName(parameter -> ExternalIdentity()),
                                     RegisterUtf8(parameter -> Type() -> signature),
                                     parameter -> LocalVariableIndex());
            }

            if (local_variable_table_attribute -> LocalVariableTableLength() > 0)
                 code_attribute -> AddAttribute(local_variable_table_attribute);
            else
                // local_variable_table_attribute not needed, so delete it now
                delete local_variable_table_attribute;
        }
        else delete local_variable_table_attribute;

        methods[method_index].AddAttribute(code_attribute);

        delete method_stack;
    }
}


//
// This is called to initialize non-constant static fields, and all instance
// fields, that were declared with optional initializers.
//
void ByteCode::InitializeVariable(AstVariableDeclarator *vd)
{
    assert(vd -> variable_initializer_opt && vd -> symbol);

    AstExpression *expression =
        vd -> variable_initializer_opt -> ExpressionCast();
    if (expression)
    {
        if (vd -> symbol -> ACC_STATIC())
            assert(! vd -> symbol -> initial_value);
        else
            PutOp(OP_ALOAD_0); // load 'this' for instance variables
        EmitExpression(expression);
    }
    else
    {
        AstArrayInitializer *array_initializer =
            vd -> variable_initializer_opt -> ArrayInitializerCast();
        assert(array_initializer);
        if (! vd -> symbol -> ACC_STATIC())
            PutOp(OP_ALOAD_0); // load 'this' for instance variables
        InitializeArray(vd -> symbol -> Type(), array_initializer);
    }

    PutOp(vd -> symbol -> ACC_STATIC() ? OP_PUTSTATIC : OP_PUTFIELD);
    if (expression && control.IsDoubleWordType(expression -> Type()))
        ChangeStack(-1);
    PutU2(RegisterFieldref(vd -> symbol));
}


void ByteCode::InitializeArray(TypeSymbol *type,
                               AstArrayInitializer *array_initializer)
{
    TypeSymbol *subtype = type -> ArraySubtype();

    LoadImmediateInteger(array_initializer -> NumVariableInitializers());
    EmitNewArray(1, type); // make the array
    for (int i = 0; i < array_initializer -> NumVariableInitializers(); i++)
    {
        Ast *entry = array_initializer -> VariableInitializer(i);
        AstExpression *expr = entry -> ExpressionCast();
        if (expr && (IsZero(expr) || expr -> Type() == control.null_type))
        {
            EmitExpression(expr, false);
            continue;
        }

        PutOp(OP_DUP);
        LoadImmediateInteger(i);
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
        // Must be uninitialized.
        assert(method_stack -> StartPc(declarator -> symbol) == 0xFFFF);
#endif // JIKES_DEBUG
#ifdef DUMP
        Coutput << "(53) Variable \"" << declarator -> symbol -> Name()
                << "\" numbered "
                << declarator -> symbol -> LocalVariableIndex()
                << " was processed" << endl;
#endif // DUMP
        method_stack -> StartPc(declarator -> symbol) =
            code_attribute -> CodeLength();
    }

    if (declarator -> symbol -> Type() -> num_dimensions > 255)
    {
        semantic.ReportSemError(SemanticError::ARRAY_OVERFLOW,
                                declarator -> LeftToken(),
                                declarator -> RightToken());
    }

    if (declarator -> symbol -> initial_value)
    {
        //
        // Optimization: If we are not tracking local variable names, we do
        // not need to waste space on a constant as it is always inlined.
        //
        if (! (control.option.g & JikesOption::VARS))
            return;
        LoadLiteral(declarator -> symbol -> initial_value,
                    declarator -> symbol -> Type());
    }
    else if (declarator -> variable_initializer_opt)
    {
        AstArrayCreationExpression *ace = declarator ->
            variable_initializer_opt -> ArrayCreationExpressionCast();
        if (ace)
            EmitArrayCreationExpression(ace);
        else if (declarator -> variable_initializer_opt ->
                 ArrayInitializerCast())
        {
            InitializeArray(declarator -> symbol -> Type(),
                            declarator -> variable_initializer_opt -> ArrayInitializerCast());
        }
        else // evaluation as expression
            EmitExpression(declarator -> variable_initializer_opt -> ExpressionCast());
    }
    else return; // if nothing to initialize

    StoreLocal(declarator -> symbol -> LocalVariableIndex(),
               declarator -> symbol -> Type());
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
// This method returns true if the statement is guaranteed to complete
// abruptly (break, continue, throw, return, and special cases of if); it
// allows some dead code elimination.
//
bool ByteCode::EmitStatement(AstStatement *statement)
{
    if (! statement -> BlockCast())
    {
        line_number_table_attribute ->
            AddLineNumber(code_attribute -> CodeLength(),
                          semantic.lex_stream -> Line(statement -> LeftToken()));
    }

    assert(stack_depth == 0); // stack empty at start of statement

    switch (statement -> kind)
    {
    case Ast::METHOD_BODY:
    case Ast::BLOCK: // JLS 14.2
        return EmitBlockStatement((AstBlock *) statement);
    case Ast::LOCAL_VARIABLE_DECLARATION: // JLS 14.3
        {
            AstLocalVariableDeclarationStatement *lvds =
                statement -> LocalVariableDeclarationStatementCast();
            for (int i = 0; i < lvds -> NumVariableDeclarators(); i++)
                DeclareLocalVariable(lvds -> VariableDeclarator(i));
        }
        return false;
    case Ast::EMPTY_STATEMENT: // JLS 14.5
        return false;
    case Ast::EXPRESSION_STATEMENT: // JLS 14.7
        EmitStatementExpression(statement -> ExpressionStatementCast() ->
                                expression);
        return false;
    case Ast::IF: // JLS 14.8
        {
            AstIfStatement *if_statement = (AstIfStatement *) statement;
            // Constant condition.
            if (IsOne(if_statement -> expression))
                return EmitStatement(if_statement -> true_statement);
            if (IsZero(if_statement -> expression))
            {
                if (if_statement -> false_statement_opt)
                    return EmitStatement(if_statement -> false_statement_opt);
                return false;
            }
            // True and false parts.
            if (if_statement -> false_statement_opt &&
                ! IsNop(if_statement -> false_statement_opt -> BlockCast()))
            {
                if (IsNop(if_statement -> true_statement -> BlockCast()))
                {
                    Label label;
                    EmitBranchIfExpression(if_statement -> expression,
                                           true, label,
                                           if_statement -> false_statement_opt);
                    assert(stack_depth == 0);
                    EmitStatement(if_statement -> false_statement_opt);
                    DefineLabel(label);
                    CompleteLabel(label);
                    return false;
                }
                Label label1,
                      label2;
                bool abrupt;
                AstStatement *true_statement = if_statement -> true_statement;
                EmitBranchIfExpression(if_statement -> expression,
                                       false, label1, true_statement);
                assert(stack_depth == 0);

                abrupt = EmitStatement(true_statement);
                if (! abrupt)
                    EmitBranch(OP_GOTO, label2,
                               if_statement -> false_statement_opt);

                DefineLabel(label1);
                abrupt &= EmitStatement(if_statement -> false_statement_opt);

                if (! abrupt)
                {
                    DefineLabel(label2);
                    CompleteLabel(label2);
                }
                CompleteLabel(label1);
                return abrupt;
            }
            // No false part.
            if (IsNop(if_statement -> true_statement -> BlockCast()))
            {
                EmitExpression(if_statement -> expression, false);
                return false;
            }
            Label label1;
            EmitBranchIfExpression(if_statement -> expression,
                                   false, label1,
                                   if_statement -> true_statement);
            assert(stack_depth == 0);
            EmitStatement(if_statement -> true_statement);
            DefineLabel(label1);
            CompleteLabel(label1);
            return false;
        }
    case Ast::SWITCH: // JLS 14.9
        EmitSwitchStatement(statement -> SwitchStatementCast());
        return ! statement -> can_complete_normally;
    case Ast::SWITCH_BLOCK: // JLS 14.9
    case Ast::CASE:
    case Ast::DEFAULT:
        //
        // These nodes are handled by SwitchStatement and
        // are not directly visited
        //
        assert(false && "faulty logic encountered");
        return false;
    case Ast::WHILE: // JLS 14.10
        {
            AstWhileStatement *wp = statement -> WhileStatementCast();
            bool abrupt = false;
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
                    abrupt = true;
                }
                else
                {
                    line_number_table_attribute ->
                        AddLineNumber(code_attribute -> CodeLength(),
                                      semantic.lex_stream -> Line(wp -> expression -> LeftToken()));
                    EmitBranchIfExpression(wp -> expression, false,
                                           method_stack -> TopBreakLabel(),
                                           wp -> statement);
                }
                EmitStatement(wp -> statement);
                assert(stack_depth == 0);
                return abrupt;
            }
            Label &continue_label = method_stack -> TopContinueLabel();
            if (wp -> expression -> IsConstant())
            {
                // must be true, or internal statement would be
                // unreachable
                assert(semantic.IsConstantTrue(wp -> expression));
                abrupt = true;
            }
            else
                EmitBranch(OP_GOTO, continue_label, wp -> statement);
            Label begin_label;
            DefineLabel(begin_label);
            u2 begin_pc = code_attribute -> CodeLength();
            abrupt |= EmitStatement(wp -> statement);
            bool empty = (begin_pc == code_attribute -> CodeLength());
            DefineLabel(continue_label);
            assert(stack_depth == 0);

            //
            // Reset the line number before evaluating the expression
            //
            line_number_table_attribute ->
                AddLineNumber(code_attribute -> CodeLength(),
                              semantic.lex_stream -> Line(wp -> expression -> LeftToken()));

            EmitBranchIfExpression(wp -> expression, true,
                                   empty ? continue_label : begin_label,
                                   wp -> statement);
            CompleteLabel(begin_label);
            CompleteLabel(continue_label);
            return abrupt && ! wp -> can_complete_normally;
        }
    case Ast::DO: // JLS 14.11
        {
            AstDoStatement *sp = statement -> DoStatementCast();
            Label begin_label;
            DefineLabel(begin_label);
            bool abrupt = EmitStatement(sp -> statement);
            if (IsLabelUsed(method_stack -> TopContinueLabel()))
            {
                DefineLabel(method_stack -> TopContinueLabel());
                CompleteLabel(method_stack -> TopContinueLabel());
                abrupt = false;
            }
            assert(stack_depth == 0);

            if (! abrupt)
            {
                //
                // Reset the line number before evaluating the expression
                //
                line_number_table_attribute ->
                    AddLineNumber(code_attribute -> CodeLength(),
                                  semantic.lex_stream -> Line(sp -> expression -> LeftToken()));
                EmitBranchIfExpression(sp -> expression, true,
                                       begin_label, sp -> statement);
            }
            CompleteLabel(begin_label);
            return (abrupt || IsOne(sp -> expression)) &&
                ! sp -> can_complete_normally;
        }
    case Ast::FOR: // JLS 14.12
        {
            AstForStatement *for_statement = statement -> ForStatementCast();
            bool abrupt = false;
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
                abrupt = true;
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
                        abrupt = false;
                        line_number_table_attribute ->
                            AddLineNumber(code_attribute -> CodeLength(),
                                          semantic.lex_stream -> Line(for_statement -> end_expression_opt -> LeftToken()));
                        EmitBranchIfExpression(for_statement -> end_expression_opt,
                                               false,
                                               method_stack -> TopBreakLabel(),
                                               for_statement -> statement);
                    }
                }
                EmitStatement(for_statement -> statement);
                assert(stack_depth == 0);
                return abrupt;
            }
            Label &continue_label = method_stack -> TopContinueLabel();
            if (for_statement -> end_expression_opt &&
                ! for_statement -> end_expression_opt -> IsConstant())
            {
                EmitBranch(OP_GOTO,
                           (for_statement -> NumForUpdateStatements()
                            ? test_label : continue_label),
                           for_statement -> statement);
            }
            else
                abrupt = true;
            DefineLabel(begin_label);
            u2 begin_pc = code_attribute -> CodeLength();
            abrupt |= EmitStatement(for_statement -> statement);
            bool empty = (begin_pc == code_attribute -> CodeLength());
            DefineLabel(continue_label);
            for (int j = 0; j < for_statement -> NumForUpdateStatements(); j++)
                EmitStatement(for_statement -> ForUpdateStatement(j));
            DefineLabel(test_label);
            CompleteLabel(test_label);

            AstExpression *end_expr = for_statement -> end_expression_opt;
            if (end_expr)
            {
                assert(stack_depth == 0);

                //
                // Reset the line number before evaluating the expression
                //
                line_number_table_attribute ->
                    AddLineNumber(code_attribute -> CodeLength(),
                                  semantic.lex_stream -> Line(end_expr -> LeftToken()));

                EmitBranchIfExpression(end_expr, true,
                                       empty ? continue_label : begin_label,
                                       for_statement -> statement);
            }
            else EmitBranch(OP_GOTO, empty ? continue_label : begin_label,
                            for_statement -> statement);
            CompleteLabel(continue_label);
            CompleteLabel(begin_label);
            return abrupt && ! for_statement -> can_complete_normally;
        }
    case Ast::BREAK: // JLS 14.13
        {
            int nesting_level = statement -> BreakStatementCast() ->
                nesting_level;
            AstBlock *over = method_stack -> Block(nesting_level);
            u2 jump_size = (over -> RightToken() - over -> LeftToken() <
                            TOKEN_WIDTH_REQUIRING_GOTOW) ? 3 : 5;
            if (ProcessAbruptExit(nesting_level, jump_size))
            {
                EmitBranch(OP_GOTO, method_stack -> BreakLabel(nesting_level),
                           over);
            }
            return true;
        }
    case Ast::CONTINUE: // JLS 14.14
        {
            int nesting_level = statement -> ContinueStatementCast() ->
                nesting_level;
            AstBlock *over = method_stack -> Block(nesting_level);
            u2 jump_size = (over -> RightToken() - over -> LeftToken() <
                            TOKEN_WIDTH_REQUIRING_GOTOW) ? 3 : 5;
            if (ProcessAbruptExit(nesting_level, jump_size))
            {
                EmitBranch(OP_GOTO,
                           method_stack -> ContinueLabel(nesting_level),
                           over);
            }
            return true;
        }
    case Ast::RETURN: // JLS 14.15
        EmitReturnStatement(statement -> ReturnStatementCast());
        return true;
    case Ast::SUPER_CALL:
        EmitSuperInvocation((AstSuperCall *) statement);
        return false;
    case Ast::THIS_CALL:
        EmitThisInvocation((AstThisCall *) statement);
        return false;
    case Ast::THROW: // JLS 14.16
        EmitExpression(statement -> ThrowStatementCast() -> expression);
        PutOp(OP_ATHROW);
        return true;
    case Ast::SYNCHRONIZED_STATEMENT: // JLS 14.17
        return EmitSynchronizedStatement((AstSynchronizedStatement *) statement);
    case Ast::TRY: // JLS 14.18
        EmitTryStatement((AstTryStatement *) statement);
        return ! statement -> can_complete_normally;
    case Ast::CATCH:   // JLS 14.18
    case Ast::FINALLY: // JLS 14.18
        // handled by TryStatement
        assert(false && "should not get here");
        return false;
    case Ast::ASSERT: // JDK 1.4 (JSR 41)
        EmitAssertStatement((AstAssertStatement *) statement);
        return false;
    case Ast::CLASS: // Class Declaration
        //
        // This is factored out by the front end; and so must be
        // skipped here (remember, interfaces cannot be declared locally).
        //
        return false;
    case Ast::VARIABLE_DECLARATOR:
        //
        // This is not really a statement, but we treat it as one to make
        // initializer blocks easier to intermix with variable declarations.
        //
        InitializeVariable((AstVariableDeclarator *) statement);
        return false;
    default:
        assert(false && "unknown statement kind");
        return false;
    }
}


void ByteCode::EmitReturnStatement(AstReturnStatement *statement)
{
    AstExpression *expression = statement -> expression_opt;

    if (! expression)
    {
        if (ProcessAbruptExit(method_stack -> NestingLevel(0), 1))
            PutOp(OP_RETURN);
    }
    else
    {
        TypeSymbol *type = expression -> Type();
        assert(type != control.void_type);

        EmitExpression(expression);

        if (ProcessAbruptExit(method_stack -> NestingLevel(0), 1, type))
            GenerateReturn(type);
    }
}


bool ByteCode::EmitBlockStatement(AstBlock *block)
{
    assert(stack_depth == 0); // stack empty at start of statement

    method_stack -> Push(block);
    bool abrupt = false;
    for (int i = 0; i < block -> NumStatements() && ! abrupt; i++)
        abrupt = EmitStatement(block -> Statement(i));

    //
    // If contained break statements jump out of this block, define the label.
    //
    if (IsLabelUsed(method_stack -> TopBreakLabel()))
    {
        DefineLabel(method_stack -> TopBreakLabel());
        CompleteLabel(method_stack -> TopBreakLabel());
        abrupt = false;
    }

    if (control.option.g & JikesOption::VARS)
    {
        for (int i = 0; i < block -> NumLocallyDefinedVariables(); i++)
        {
            VariableSymbol *variable = block -> LocallyDefinedVariable(i);
            if (method_stack -> StartPc(variable) == 0xFFFF) // never used
                continue;
#ifdef DUMP
            Coutput << "(56) The symbol \"" << variable -> Name()
                    << "\" numbered " << variable -> LocalVariableIndex()
                    << " was released" << endl;
#endif // DUMP
            local_variable_table_attribute ->
                AddLocalVariable(method_stack -> StartPc(variable),
                                 code_attribute -> CodeLength(),
                                 RegisterName(variable -> ExternalIdentity()),
                                 RegisterUtf8(variable -> Type() -> signature),
                                 variable -> LocalVariableIndex());
        }
    }

    method_stack -> Pop();
    return abrupt;
}


void ByteCode::EmitStatementExpression(AstExpression *expression)
{
    switch (expression -> kind)
    {
    case Ast::CALL:
        {
            AstMethodInvocation *method_call =
                (AstMethodInvocation *) expression;
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
        EmitInstanceCreationExpression((AstClassInstanceCreationExpression *) expression, false);
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
        nlabels = ncases;
    i4 high = 0,
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
    line_number_table_attribute ->
        AddLineNumber(code_attribute -> CodeLength(),
                      semantic.lex_stream -> Line(switch_statement -> expression -> LeftToken()));
    EmitExpression(switch_statement -> expression);

    PutOp(use_lookup ? OP_LOOKUPSWITCH : OP_TABLESWITCH);
    u2 op_start = last_op_pc; // pc at start of instruction

    //
    // supply any needed padding
    //
    while (code_attribute -> CodeLength() % 4 != 0)
        PutU1(0);

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
            AstSwitchBlockStatement *switch_block_statement =
                (AstSwitchBlockStatement *) switch_block -> Statement(j);

            //
            // process labels for this block
            //
            for (int li = 0;
                 li < switch_block_statement -> NumSwitchLabels(); li++)
            {
                AstCaseLabel *case_label = switch_block_statement ->
                    SwitchLabel(li) -> CaseLabelCast();
                if (case_label)
                {
                    int label_index = switch_statement ->
                        Case(case_label -> map_index) -> Value() - low;
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
    // March through switch block statements, compiling blocks in
    // proper order. We must respect order in which blocks are seen
    // so that blocks lacking a terminal break fall through to the
    // proper place.
    //
    for (int i = 0; i < switch_block -> NumStatements(); i++)
    {
        AstSwitchBlockStatement *switch_block_statement =
            (AstSwitchBlockStatement *) switch_block -> Statement(i);

        //
        // process labels for this block
        //
        for (int li = 0; li < switch_block_statement -> NumSwitchLabels(); li++)
        {
            AstCaseLabel *case_label =
                switch_block_statement -> SwitchLabel(li) -> CaseLabelCast();
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
                            int ci =
                                switch_statement -> Case(di) -> Value() - low;
                            DefineLabel(case_labels[ci]);
                            break;
                        }
                    }
                }
            }
            else
            {
                assert(switch_block_statement -> SwitchLabel(li) ->
                       DefaultLabelCast());
                assert(switch_statement -> default_case.switch_block_statement);
                DefineLabel(default_label);
            }
        }

        //
        // Compile code for this case.
        //
        bool abrupt = false;
        for (int si = 0; si < switch_block_statement -> NumStatements(); si++)
        {
            if (! abrupt)
                abrupt = EmitStatement(switch_block_statement ->
                                       Statement(si) -> StatementCast());
            else if (switch_block_statement -> Statement(si) ->
                     LocalVariableDeclarationStatementCast())
            {
                //
                // In a switch statement, local variable declarations are
                // accessible in other case labels even if the declaration
                // itself is unreachable.
                //
                AstLocalVariableDeclarationStatement *lvds =
                    (AstLocalVariableDeclarationStatement *)
                    switch_block_statement -> Statement(si);
                for (int j = 0; j < lvds -> NumVariableDeclarators(); j++)
                {
                    AstVariableDeclarator *declarator =
                        lvds -> VariableDeclarator(j);
                    if (control.option.g & JikesOption::VARS)
                    {
                        method_stack -> StartPc(declarator -> symbol) =
                            code_attribute -> CodeLength();
                    }
                    if (declarator -> symbol -> Type() -> num_dimensions > 255)
                    {
                        semantic.ReportSemError(SemanticError::ARRAY_OVERFLOW,
                                                declarator -> LeftToken(),
                                                declarator -> RightToken());
                    }
                }
            }
        }
    }

    //
    // Close the range of the locally defined variables.
    //
    if (control.option.g & JikesOption::VARS)
    {
        for (int i = 0; i < switch_block -> NumLocallyDefinedVariables(); i++)
        {
            VariableSymbol *variable =
                switch_block -> LocallyDefinedVariable(i);
            if (method_stack -> StartPc(variable) > op_start)
            {
                if (method_stack -> StartPc(variable) == 0xFFFF) // never used
                    continue;
#ifdef DUMP
                Coutput << "(58) The symbol \"" << variable -> Name()
                        << "\" numbered " << variable -> LocalVariableIndex()
                        << " was released" << endl;
#endif // DUMP
                local_variable_table_attribute ->
                    AddLocalVariable(method_stack -> StartPc(variable),
                                     code_attribute -> CodeLength(),
                                     RegisterName(variable -> ExternalIdentity()),
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
            case_labels[j].definition =
                (switch_statement -> default_case.switch_block_statement
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
    if (IsLabelUsed(method_stack -> TopBreakLabel()))
    {
        // need define only if used
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

    u2 start_try_block_pc = code_attribute -> CodeLength(); // start pc
    assert(method_stack -> TopHandlerRangeStart().Length() == 0 &&
           method_stack -> TopHandlerRangeEnd().Length() == 0);
    method_stack -> TopHandlerRangeStart().Push(start_try_block_pc);
    bool emit_finally_clause = statement -> finally_clause_opt &&
        ! IsNop(statement -> finally_clause_opt -> block);

    //
    // If we determined the finally clause is a nop, remove the tag
    // TRY_CLAUSE_WITH_FINALLY so that abrupt completions do not emit JSR.
    // On the other hand, if the finally clause cannot complete normally,
    // change the tag to ABRUPT_TRY_FINALLY so that abrupt completions emit
    // a GOTO instead of a JSR. Also, mark a try block which has a catch
    // clause but no finally clause, in case an abrupt exit forces a split
    // in the range of protected code.
    //
    if (statement -> finally_clause_opt)
        if (! emit_finally_clause)
            statement -> block -> block_tag = AstBlock::NONE;
        else if (! statement -> finally_clause_opt -> block ->
                 can_complete_normally)
        {
            statement -> block -> block_tag = AstBlock::ABRUPT_TRY_FINALLY;
        }
    if (statement -> block -> block_tag == AstBlock::NONE &&
        statement -> NumCatchClauses())
    {
        statement -> block -> block_tag = AstBlock::TRY_CLAUSE_WITH_CATCH;
    }
    bool abrupt = EmitBlockStatement(statement -> block);

    //
    // The computation of end_try_block_pc, the instruction following the last
    // instruction in the body of the try block, does not include the code, if
    // any, needed to call a finally block or skip to the end of the try
    // statement.
    //
    u2 end_try_block_pc = code_attribute -> CodeLength();
    Tuple<u2> handler_starts(method_stack -> TopHandlerRangeStart());
    Tuple<u2> handler_ends(method_stack -> TopHandlerRangeEnd());
    handler_ends.Push(end_try_block_pc);
    assert(handler_starts.Length() == handler_ends.Length());

    //
    // If try block is not empty, process catch clauses, including "special"
    // clause for finally.
    //
    if (start_try_block_pc != end_try_block_pc)
    {
        // Use the label in the block immediately enclosing try statement.
        Label &finally_label = method_stack -> TopFinallyLabel(),
              end_label;

        //
        // If try block completes normally, skip code for catch blocks.
        //
        if (! abrupt &&
            (emit_finally_clause || statement -> NumCatchClauses()))
        {
            EmitBranch(OP_GOTO, end_label, statement);
        }

        for (int i = 0; i < statement -> NumCatchClauses(); i++)
        {
            u2 handler_pc = code_attribute -> CodeLength();

            AstCatchClause *catch_clause = statement -> CatchClause(i);
            VariableSymbol *parameter_symbol = catch_clause -> parameter_symbol;

            assert(stack_depth == 0);
            stack_depth = 1; // account for the exception already on the stack
            StoreLocal(parameter_symbol -> LocalVariableIndex(),
                       parameter_symbol -> Type());
            u2 handler_type = RegisterClass(parameter_symbol -> Type());
            for (int j = handler_starts.Length(); --j >= 0; )
            {
                code_attribute ->
                    AddException(handler_starts[j], handler_ends[j],
                                 handler_pc, handler_type);
            }

            //
            // If we determined the finally clause is a nop, remove the tag
            // TRY_CLAUSE_WITH_FINALLY so that abrupt completions do not emit
            // JSR. On the other hand, if the finally clause cannot complete
            // normally, change the tag to ABRUPT_TRY_FINALLY so that abrupt
            // completions emit a GOTO instead of a JSR.
            //
            if (statement -> finally_clause_opt)
                if (! emit_finally_clause)
                    catch_clause -> block -> block_tag = AstBlock::NONE;
                else if (! statement -> finally_clause_opt -> block ->
                         can_complete_normally)
                {
                    catch_clause -> block -> block_tag =
                        AstBlock::ABRUPT_TRY_FINALLY;
                }
            abrupt = EmitBlockStatement(catch_clause -> block);

            if (control.option.g & JikesOption::VARS)
            {
                local_variable_table_attribute ->
                    AddLocalVariable(handler_pc,
                                     code_attribute -> CodeLength(),
                                     RegisterName(parameter_symbol -> ExternalIdentity()),
                                     RegisterUtf8(parameter_symbol -> Type() -> signature),
                                     parameter_symbol -> LocalVariableIndex());
            }

            //
            // If catch block completes normally, skip further catch blocks.
            //
            if (! abrupt && (emit_finally_clause ||
                             i < (statement -> NumCatchClauses() - 1)))
            {
                EmitBranch(OP_GOTO, end_label, statement);
            }
        }
        //
        // If this try statement contains a finally clause, then ...
        //
        if (emit_finally_clause)
        {
            int variable_index = method_stack -> TopBlock() ->
                block_symbol -> try_or_synchronized_variable_index;
            u2 finally_start_pc = code_attribute -> CodeLength();
            u2 special_end_pc = finally_start_pc;

            //
            // Emit code for "special" handler to make sure finally clause is
            // invoked in case an otherwise uncaught exception is thrown in the
            // try block, or an exception is thrown from within a catch block.
            // This must cover all instructions through the jsr, in case of
            // asynchronous exceptions.
            //
            assert(stack_depth == 0);
            stack_depth = 1; // account for the exception already on stack
            if (statement -> finally_clause_opt -> block ->
                can_complete_normally)
            {
                StoreLocal(variable_index, control.Object()); // Save exception
                EmitBranch(OP_JSR, finally_label, statement);
                special_end_pc = code_attribute -> CodeLength();
                LoadLocal(variable_index, control.Object()); // Reload and
                PutOp(OP_ATHROW); // rethrow exception.
            }
            else
            {
                //
                // Ignore the exception already on the stack, since we know
                // the finally clause overrides it.
                //
                PutOp(OP_POP);
            }
            method_stack -> TopHandlerRangeEnd().Push(special_end_pc);
            int count = method_stack -> TopHandlerRangeStart().Length();
            assert(count == method_stack -> TopHandlerRangeEnd().Length());
            while (count--)
            {
                code_attribute ->
                    AddException(method_stack -> TopHandlerRangeStart().Pop(),
                                 method_stack -> TopHandlerRangeEnd().Pop(),
                                 finally_start_pc, 0);
            }

            //
            // Generate code for finally clause. If the finally block can
            // complete normally, this is reached from a JSR, so save the
            // return address. Otherwise, this is reached from a GOTO.
            //
            DefineLabel(finally_label);
            assert(stack_depth == 0);
            if (statement -> finally_clause_opt -> block ->
                can_complete_normally)
            {
                stack_depth = 1; // account for the return location on stack
                StoreLocal(variable_index + 1, control.Object());
            }
            else if (IsLabelUsed(end_label))
            {
                DefineLabel(end_label);
                CompleteLabel(end_label);
            }
            EmitBlockStatement(statement -> finally_clause_opt -> block);

            //
            // If a finally block can complete normally, return to the saved
            // address of the caller.
            //
            if (statement -> finally_clause_opt -> block ->
                can_complete_normally)
            {
                PutOpWide(OP_RET, variable_index + 1);
                //
                // Now, if the try or catch blocks complete normally, execute
                // the finally block before advancing to next statement. We
                // need to trap one more possibility of an asynchronous
                // exception before the jsr has started.
                //
                if (IsLabelUsed(end_label))
                {
                    DefineLabel(end_label);
                    CompleteLabel(end_label);
                    EmitBranch(OP_JSR, finally_label,
                               statement -> finally_clause_opt -> block);
                    special_end_pc = code_attribute -> CodeLength();
                    code_attribute -> AddException(special_end_pc - 3,
                                                   special_end_pc,
                                                   finally_start_pc, 0);
                }
            }
            CompleteLabel(finally_label);
        }
        else
        {
            //
            // Finally block is not present, advance to next statement, and
            // clean up the handler start/end ranges.
            //
            assert(! IsLabelUsed(finally_label));
            DefineLabel(end_label);
            CompleteLabel(end_label);
            method_stack -> TopHandlerRangeStart().Reset();
            method_stack -> TopHandlerRangeEnd().Reset();
        }
    }
    else
    {
        //
        // Try block was empty; skip all catch blocks, and a finally block
        // is treated normally.
        //
        method_stack -> TopHandlerRangeStart().Reset();
        if (emit_finally_clause)
            EmitBlockStatement(statement -> finally_clause_opt -> block);
    }
}


//
// Exit to block at level, freeing monitor locks and invoking finally
// clauses as appropriate. The width is 1 for return, 3 for normal a normal
// GOTO (from a break or continue), or 5 for a GOTO_W. The return is true
// unless some intervening finally block cannot complete normally.
//
bool ByteCode::ProcessAbruptExit(int level, u2 width, TypeSymbol *return_type)
{
    int variable_index = -1;
    //
    // We must store the return value in a variable, rather than on the
    // stack, in case a finally block contains an embedded try-catch which
    // wipes out the stack.
    //
    if (return_type)
    {
        for (int i = method_stack -> Size() - 1;
             i > 0 && method_stack -> NestingLevel(i) != level; i--)
        {
            int nesting_level = method_stack -> NestingLevel(i),
                enclosing_level = method_stack -> NestingLevel(i - 1);
            AstBlock *block = method_stack -> Block(nesting_level);
            if (block -> block_tag == AstBlock::TRY_CLAUSE_WITH_FINALLY)
            {
                variable_index = method_stack -> Block(enclosing_level) ->
                    block_symbol -> try_or_synchronized_variable_index + 2;
            }
            else if (block -> block_tag == AstBlock::ABRUPT_TRY_FINALLY)
            {
                variable_index = -1;
                PutOp(control.IsDoubleWordType(return_type) ? OP_POP2 : OP_POP);
                break;
            }
        }
    }
    if (variable_index >= 0)
        StoreLocal(variable_index, return_type);

    for (int i = method_stack -> Size() - 1;
         i > 0 && method_stack -> NestingLevel(i) != level; i--)
    {
        int nesting_level = method_stack -> NestingLevel(i),
            enclosing_level = method_stack -> NestingLevel(i - 1);
        AstBlock *block = method_stack -> Block(nesting_level);
        if (block -> block_tag == AstBlock::TRY_CLAUSE_WITH_FINALLY)
        {
            EmitBranch(OP_JSR, method_stack -> FinallyLabel(enclosing_level),
                       method_stack -> Block(enclosing_level));
            method_stack -> HandlerRangeEnd(enclosing_level).
                Push(code_attribute -> CodeLength());
        }
        else if (block -> block_tag == AstBlock::ABRUPT_TRY_FINALLY)
        {
            //
            // Ignore the width of the abrupt instruction, because the abrupt
            // finally preempts it.
            //
            width = 0;
            EmitBranch(OP_GOTO, method_stack -> FinallyLabel(enclosing_level),
                       method_stack -> Block(enclosing_level));
            method_stack -> HandlerRangeEnd(enclosing_level).
                Push(code_attribute -> CodeLength());
            break;
        }
        else if (block -> block_tag == AstBlock::SYNCHRONIZED)
        {
            //
            // This code must be safe for asynchronous exceptions.  Note that
            // we are splitting the range of instructions covered by the
            // synchronized statement catchall handler.
            //
            int variable_index = method_stack -> Block(enclosing_level) ->
                block_symbol -> try_or_synchronized_variable_index;
            LoadLocal(variable_index, control.Object());
            PutOp(OP_MONITOREXIT);
            method_stack -> HandlerRangeEnd(enclosing_level).
                Push(code_attribute -> CodeLength());
        }
        else if (block -> block_tag == AstBlock::TRY_CLAUSE_WITH_CATCH)
        {
            method_stack -> HandlerRangeEnd(enclosing_level).
                Push(code_attribute -> CodeLength());
        }
    }

    if (variable_index >= 0)
        LoadLocal(variable_index, return_type);
    for (int j = method_stack -> Size() - 1;
         j > 0 && method_stack -> NestingLevel(j) != level; j--)
    {
        int nesting_level = method_stack -> NestingLevel(j),
            enclosing_level = method_stack -> NestingLevel(j - 1);
        AstBlock *block = method_stack -> Block(nesting_level);
        if (block -> block_tag == AstBlock::SYNCHRONIZED ||
            block -> block_tag == AstBlock::TRY_CLAUSE_WITH_CATCH ||
            block -> block_tag == AstBlock::TRY_CLAUSE_WITH_FINALLY)
        {
            method_stack -> HandlerRangeStart(enclosing_level).
                Push(code_attribute -> CodeLength() + width);
        }
        else if (block -> block_tag == AstBlock::ABRUPT_TRY_FINALLY)
        {
            method_stack -> HandlerRangeStart(enclosing_level).
                Push(code_attribute -> CodeLength());
            return false;
        }
    }
    return true;
}

void ByteCode::EmitBranch(Opcode opc, Label& lab, AstStatement *over)
{
    // Use the number of tokens as a heuristic for the size of the statement
    // we're jumping over. If the statement is large enough, either change
    // to the 4-byte branch opcode or write out a branch around a goto_w for
    // branch opcodes that don't have a long form.
    int sizeHeuristic = over ? over -> RightToken() - over -> LeftToken() : 0;
    if (sizeHeuristic < TOKEN_WIDTH_REQUIRING_GOTOW) {
        PutOp(opc);
        UseLabel(lab, 2, 1);
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
    assert(p -> Type() == control.boolean_type);

    if (p -> IsConstant())
    {
        if (IsZero(p) != cond)
            EmitBranch(OP_GOTO, lab, over);
        return;
    }

    AstPreUnaryExpression *pre = p -> PreUnaryExpressionCast();
    if (pre) // must be !
    {
        // branch_if(!e,c,l) => branch_if(e,!c,l)
        // test opcode
        // call again with complementary control expression to show
        // effect of negation
        assert(pre -> pre_unary_tag == AstPreUnaryExpression::NOT);
        EmitBranchIfExpression(pre -> expression, ! cond, lab, over);
        return;
    }

    AstConditionalExpression *conditional = p -> ConditionalExpressionCast();
    if (conditional)
    {
        //
        // branch_if(true?a:b, cond, lab) => branch_if(a, cond, lab);
        // branch_if(false?a:b, cond, lab) => branch_if(b, cond, lab);
        //
        if (conditional -> test_expression -> IsConstant())
        {
            EmitBranchIfExpression((IsZero(conditional -> test_expression)
                                    ? conditional -> false_expression
                                    : conditional -> true_expression),
                                   cond, lab, over);
            return;
        }
        //
        // branch_if(expr?true:false, c, l) => branch_if(expr, c, l);
        // branch_if(expr?false:true, c, l) => branch_if(expr, ! c, l);
        // branch_if(expr?true:true, c, l) => expr, branch if c
        // branch_if(expr?false:false, c, l) => expr, branch if ! c
        //
        if (conditional -> true_expression -> IsConstant() &&
            conditional -> false_expression -> IsConstant())
        {
            if (IsOne(conditional -> true_expression))
            {
                if (IsOne(conditional -> false_expression))
                {
                    EmitExpression(conditional -> test_expression, false);
                    if (cond)
                        EmitBranch(OP_GOTO, lab, over);
                }
                else
                {
                    EmitBranchIfExpression(conditional -> test_expression,
                                           cond, lab, over);
                }
            }
            else
            {
                if (IsOne(conditional -> false_expression))
                {
                    EmitBranchIfExpression(conditional -> test_expression,
                                           ! cond, lab, over);
                }
                else
                {
                    EmitExpression(conditional -> test_expression, false);
                    if (! cond)
                        EmitBranch(OP_GOTO, lab, over);
                }
            }
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
            if (right_type -> num_dimensions > 255)
            {
                semantic.ReportSemError(SemanticError::ARRAY_OVERFLOW,
                                        right -> LeftToken(),
                                        right -> RightToken());
            }
            if (left_type == control.null_type)
            {
                //
                // We know the result: false. But emit the left expression,
                // in case of side effects in (expr ? null : null).
                //
                EmitExpression(left, false);
                if (! cond)
                    EmitBranch(OP_GOTO, lab, over);
            }
            else if (left -> IsConstant() || // a String constant
                     left -> BinaryExpressionCast()) // a String concat
            {
                //
                // We know the result: true, since the expression is non-null
                // and String is a final class.
                //
                assert(left_type == control.String());
                EmitExpression(left, false);
                if (cond)
                    EmitBranch(OP_GOTO, lab, over);
            }
            else if ((left -> IsThisExpression() ||
                      left -> IsSuperExpression() ||
                      left -> ClassInstanceCreationExpressionCast() ||
                      left -> ArrayCreationExpressionCast()) &&
                     (left_type -> IsSubclass(right_type) ||
                      left_type -> Implements(right_type)))
            {
                //
                // We know the result: true, since the expression is non-null.
                //
                EmitExpression(left, false);
                if (cond)
                    EmitBranch(OP_GOTO, lab, over);
            }
            else
            {
                EmitExpression(left);
                PutOp(OP_INSTANCEOF);
                PutU2(right_type -> num_dimensions > 0
                      ? RegisterClass(right_type -> signature)
                      : RegisterClass(right_type));

                EmitBranch((cond ? OP_IFNE : OP_IFEQ), lab, over);
            }
        }
        return;
    case AstBinaryExpression::AND_AND:
        //
        // branch_if(true&&b, cond, lab) => branch_if(b, cond, lab);
        // branch_if(false&&b, cond, lab) => branch_if(false, cond, lab);
        //
        if (left -> IsConstant())
        {
            if (IsOne(left))
                EmitBranchIfExpression(right, cond, lab, over);
            else if (! cond)
                EmitBranch(OP_GOTO, lab, over);
        }
        //
        // branch_if(a&&true, cond, lab) => branch_if(a, cond, lab);
        // branch_if(a&&false, cond, lab) => emit(a), pop; for side effects
        //
        else if (right -> IsConstant())
        {
            if (IsOne(right))
                EmitBranchIfExpression(left, cond, lab, over);
            else
            {
                EmitExpression(left, false);
                if (! cond)
                    EmitBranch(OP_GOTO, lab, over);
            }
        }
        //
        // branch_if(a&&b, true, lab) =>
        //   branch_if(a,false,skip);
        //   branch_if(b,true,lab);
        //   skip:
        // branch_if(a&&b, false, lab) =>
        //   branch_if(a,false,lab);
        //   branch_if(b,false,lab);
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
        // branch_if(false||b, cond, lab) => branch_if(b, cond, lab);
        // branch_if(true||b, cond, lab) => branch_if(true, cond, lab);
        //
        if (left -> IsConstant())
        {
            if (IsZero(left))
                EmitBranchIfExpression(right, cond, lab, over);
            else if (cond)
                EmitBranch(OP_GOTO, lab, over);
        }
        //
        // branch_if(a||false, cond, lab) => branch_if(a, cond, lab);
        // branch_if(a||true, cond, lab) => emit(a), pop; for side effects
        //
        else if (right -> IsConstant())
        {
            if (IsZero(right))
                EmitBranchIfExpression(left, cond, lab, over);
            else
            {
                EmitExpression(left, false);
                if (cond)
                    EmitBranch(OP_GOTO, lab, over);
            }
        }
        //
        // branch_if(a||b,true,lab) =>
        //   branch_if(a,true,lab);
        //   branch_if(b,true,lab);
        // branch_if(a||b,false,lab) =>
        //   branch_if(a,true,skip);
        //   branch_if(b,false,lab);
        //   skip:
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
    case AstBinaryExpression::XOR: // ^ on booleans is equavalent to !=
        assert(left_type == control.boolean_type);
        // Fallthrough!
    case AstBinaryExpression::EQUAL_EQUAL:
    case AstBinaryExpression::NOT_EQUAL:
        //
        // One of the operands is null. We must evaluate both operands, to get
        // any side effects in (expr ? null : null).
        //
        if (left_type == control.null_type || right_type == control.null_type)
        {
            EmitExpression(left, left_type != control.null_type);
            EmitExpression(right, right_type != control.null_type);
            if (left_type == right_type)
            {
                if (cond ==
                    (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL))
                {
                    EmitBranch(OP_GOTO, lab, over);
                }
            }
            else
            {
                if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL)
                    EmitBranch(cond ? OP_IFNULL : OP_IFNONNULL, lab, over);
                else EmitBranch(cond ? OP_IFNONNULL : OP_IFNULL, lab, over);
            }
            return;
        }

        //
        // One of the operands is true. Branch on the other.
        //
        if (left_type == control.boolean_type &&
            (IsOne(left) || IsOne(right)))
        {
            EmitBranchIfExpression(IsOne(left) ? right : left,
                                   cond == (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL),
                                   lab, over);
            return;
        }

        //
        // Both operands are integer.
        //
        if (control.IsSimpleIntegerValueType(left_type) ||
             left_type == control.boolean_type)
        {
            assert(control.IsSimpleIntegerValueType(right_type) ||
                   right_type == control.boolean_type);

            if (IsZero(left) || IsZero(right))
            {
                if (left_type == control.boolean_type)
                {
                    //
                    // One of the operands is false. Branch on the other.
                    //
                    EmitBranchIfExpression(IsZero(left) ? right : left,
                                           cond == (bp -> binary_tag != AstBinaryExpression::EQUAL_EQUAL),
                                           lab, over);
                }
                else
                {
                    //
                    // One of the operands is zero. Only emit the other.
                    //
                    EmitExpression(IsZero(left) ? right : left);

                    if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL)
                        EmitBranch((cond ? OP_IFEQ : OP_IFNE), lab, over);
                    else EmitBranch((cond ? OP_IFNE : OP_IFEQ), lab, over);
                }
            }
            else
            {
                EmitExpression(left);
                EmitExpression(right);

                if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL)
                    EmitBranch((cond ? OP_IF_ICMPEQ : OP_IF_ICMPNE), lab, over);
                else
                    EmitBranch((cond ? OP_IF_ICMPNE : OP_IF_ICMPEQ), lab, over);
            }

            return;
        }

        //
        // Both operands are reference types: just do the comparison.
        //
        if (IsReferenceType(left_type))
        {
            assert(IsReferenceType(right_type));
            EmitExpression(left);
            EmitExpression(right);

            if (bp -> binary_tag == AstBinaryExpression::EQUAL_EQUAL)
                EmitBranch((cond ? OP_IF_ACMPEQ : OP_IF_ACMPNE), lab, over);
            else EmitBranch((cond ? OP_IF_ACMPNE : OP_IF_ACMPEQ), lab, over);

            return;
        }

        break;
    case AstBinaryExpression::IOR:
        //
        // One argument is false. Branch on other.
        //
        if (IsZero(left) || IsZero(right))
        {
            EmitBranchIfExpression(IsZero(left) ? right : left,
                                   cond, lab, over);
            return;
        }

        //
        // One argument is true. Emit the other, and result is true.
        //
        if (IsOne(left) || IsOne(right))
        {
            EmitExpression(IsOne(left) ? right : left, false);
            if (cond)
                EmitBranch(OP_GOTO, lab, over);
            return;
        }
        break;
    case AstBinaryExpression::AND:
        //
        // One argument is true. Branch on other.
        //
        if (IsOne(left) || IsOne(right))
        {
            EmitBranchIfExpression(IsOne(left) ? right : left,
                                   cond, lab, over);
            return;
        }

        //
        // One argument is false. Emit the other, and result is false.
        //
        if (IsZero(left) || IsZero(right))
        {
            EmitExpression(IsZero(left) ? right : left, false);
            if (! cond)
                EmitBranch(OP_GOTO, lab, over);
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
    Opcode opcode = OP_NOP,
           op_true,
           op_false;
    assert(left_type != control.boolean_type);
    if (control.IsSimpleIntegerValueType(left_type))
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
            case AstBinaryExpression::LESS:
                // if (0 < x) same as  if (x > 0)
                op_true = OP_IFGT;
                op_false = OP_IFLE;
                break;
            case AstBinaryExpression::LESS_EQUAL:
                // if (0 <= x) same as if (x >= 0)
                op_true = OP_IFGE;
                op_false = OP_IFLT;
                break;
            case AstBinaryExpression::GREATER:
                // if (0 > x) same as if (x < 0)
                op_true = OP_IFLT;
                op_false = OP_IFGE;
                break;
            case AstBinaryExpression::GREATER_EQUAL:
                // if (0 >= x) same as if (x <= 0)
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

    if (opcode != OP_NOP)
        PutOp(opcode); // if need to emit comparison before branch

    EmitBranch (cond ? op_true : op_false, lab, over);
}


//
// Emits a synchronized statement, including monitor cleanup. The return
// value is true if the contained statement is abrupt.
//
bool ByteCode::EmitSynchronizedStatement(AstSynchronizedStatement *statement)
{
    int variable_index = method_stack -> TopBlock() -> block_symbol ->
        try_or_synchronized_variable_index;

    Label start_label;
    //
    // This code must be careful of asynchronous exceptions. Even if the
    // synchronized block is empty, user code can use Thread.stop(Throwable),
    // so we must ensure the monitor exits. We make sure that all instructions
    // after the monitorenter are covered.  By sticking the catchall code
    // before the synchronized block, we can even make abrupt exits inside the
    // statement be asynch-exception safe.  Note that the user can cause
    // deadlock (ie. an infinite loop), by releasing the monitor (via JNI or
    // some other means) in the block statement, so that the monitorexit fails
    // synchronously with an IllegalMonitorStateException and tries again; but
    // JLS 17.13 states that the compiler need not worry about such user
    // stupidity.
    //
    EmitBranch(OP_GOTO, start_label, NULL);
    u2 handler_pc = code_attribute -> CodeLength();
    assert(stack_depth == 0);
    stack_depth = 1; // account for the exception already on the stack
    LoadLocal(variable_index, control.Object()); // reload monitor
    PutOp(OP_MONITOREXIT);
    u2 throw_pc = code_attribute -> CodeLength();
    PutOp(OP_ATHROW);
    code_attribute -> AddException(handler_pc, throw_pc, handler_pc, 0);

    //
    // Even if enclosed statement is a nop, we must enter the monitor, because
    // of memory flushing side effects of synchronization.
    //
    DefineLabel(start_label);
    CompleteLabel(start_label);
    EmitExpression(statement -> expression);
    PutOp(OP_DUP); // duplicate for saving, entering monitor
    StoreLocal(variable_index, control.Object()); // save address of object
    PutOp(OP_MONITORENTER); // enter monitor associated with object

    assert(method_stack -> TopHandlerRangeStart().Length() == 0 &&
           method_stack -> TopHandlerRangeEnd().Length() == 0);
    method_stack -> TopHandlerRangeStart().Push(code_attribute -> CodeLength());
    bool abrupt = EmitBlockStatement(statement -> block);

    if (! abrupt)
    {
        LoadLocal(variable_index, control.Object()); // reload monitor
        PutOp(OP_MONITOREXIT);
    }
    u2 end_pc = code_attribute -> CodeLength();
    method_stack -> TopHandlerRangeEnd().Push(end_pc);
    int count = method_stack -> TopHandlerRangeStart().Length();
    assert(count == method_stack -> TopHandlerRangeEnd().Length());
    while (count--)
    {
        code_attribute ->
            AddException(method_stack -> TopHandlerRangeStart().Pop(),
                         method_stack -> TopHandlerRangeEnd().Pop(),
                         handler_pc, 0);
    }
    return abrupt;
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
        EmitBranchIfExpression(assertion -> condition, true, label);

        PutOp(OP_NEW);
        PutU2(RegisterClass(control.AssertionError()));
        PutOp(OP_DUP);

        MethodSymbol *constructor = NULL;
        if (assertion -> message_opt)
        {
            EmitExpression(assertion -> message_opt);
            TypeSymbol *type = assertion -> message_opt -> Type();

            if (! control.AssertionError() -> Bad())
            {
                // We found the class, now can we find the method?
                if (type == control.char_type)
                    constructor = control.AssertionError_InitWithCharMethod();
                else if (type == control.boolean_type)
                {
                    constructor =
                        control.AssertionError_InitWithBooleanMethod();
                }
                else if (type == control.int_type ||
                         type == control.short_type ||
                         type == control.byte_type)
                {
                    constructor = control.AssertionError_InitWithIntMethod();
                }
                else if (type == control.long_type)
                    constructor = control.AssertionError_InitWithLongMethod();
                else if (type == control.float_type)
                    constructor = control.AssertionError_InitWithFloatMethod();
                else if (type == control.double_type)
                    constructor = control.AssertionError_InitWithDoubleMethod();
                else if (type == control.null_type || IsReferenceType(type))
                    constructor = control.AssertionError_InitWithObjectMethod();
                else
                {
                    assert (false && "Missing AssertionError constructor!");
                }
                if (! constructor) // We didn't find it; suckage....
                    // TODO: error ought to include what we were looking for
                    semantic.ReportSemError(SemanticError::LIBRARY_METHOD_NOT_FOUND,
                                            assertion -> LeftToken(),
                                            assertion -> RightToken(),
                                            unit_type -> ContainingPackage() -> PackageName(),
                                            unit_type -> ExternalName());

            }
            else
            {
                // The type for AssertionError is BAD, that means it wasn't
                // found! but the calls to control.AssertionError() above will
                // file a semantic error for us, no need to here.
            }
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
    expression = StripNops(expression);
    if (expression -> IsConstant())
    {
        if (need_value)
        {
            LoadLiteral(expression -> value, expression -> Type());
            return GetTypeWords(expression -> Type());
        }
        return 0;
    }

    switch (expression -> kind)
    {
    case Ast::IDENTIFIER:
        {
            if (expression -> symbol && expression -> symbol -> TypeCast())
                return 0;
            AstSimpleName *simple_name = (AstSimpleName *) expression;
            return (simple_name -> resolution_opt
                    ? EmitExpression(simple_name -> resolution_opt, need_value)
                    : LoadVariable(GetLhsKind(expression), expression,
                                   need_value));
        }
    case Ast::THIS_EXPRESSION:
    case Ast::SUPER_EXPRESSION:
        if (need_value)
        {
            PutOp(OP_ALOAD_0); // will be used
            return 1;
        }
        return 0;
    case Ast::CLASS_CREATION:
        return EmitInstanceCreationExpression((AstClassInstanceCreationExpression *) expression, need_value);
    case Ast::ARRAY_CREATION:
        assert(need_value && "array can't qualify static member");
        return EmitArrayCreationExpression((AstArrayCreationExpression *) expression);
    case Ast::DOT:
        {
            AstFieldAccess *field_access = (AstFieldAccess *) expression;
            if (field_access -> IsClassAccess() &&
                field_access -> resolution_opt)
            {
                return GenerateClassAccess(field_access, need_value);
            }
            return EmitFieldAccess(field_access, need_value);
        }
    case Ast::CALL:
        {
            AstMethodInvocation *method_call =
                (AstMethodInvocation *) expression;
            // must evaluate for side effects
            EmitMethodInvocation(method_call);
            if (need_value)
                return GetTypeWords(method_call -> Type());
            PutOp(GetTypeWords(method_call -> Type()) == 1 ? OP_POP : OP_POP2);
            return 0;
        }
    case Ast::ARRAY_ACCESS:
        {
            // must evaluate, for potential Exception side effects
            int words = EmitArrayAccessRhs((AstArrayAccess *) expression);
            if (need_value)
                return words;
            PutOp(words == 1 ? OP_POP : OP_POP2);
            return 0;
        }
    case Ast::POST_UNARY:
        return EmitPostUnaryExpression((AstPostUnaryExpression *) expression,
                                       need_value);
    case Ast::PRE_UNARY:
        return EmitPreUnaryExpression((AstPreUnaryExpression *) expression,
                                      need_value);
    case Ast::CAST:
        return EmitCastExpression((AstCastExpression *) expression, need_value);
    case Ast::BINARY:
        {
            // Must evaluate for potential side effects.
            int words = EmitBinaryExpression((AstBinaryExpression *) expression);
            if (need_value)
                return words;
            PutOp(words == 1 ? OP_POP : OP_POP2);
            return 0;
        }
    case Ast::CONDITIONAL:
        return EmitConditionalExpression((AstConditionalExpression *) expression,
                                         need_value);
    case Ast::ASSIGNMENT:
        return EmitAssignmentExpression((AstAssignmentExpression *) expression,
                                        need_value);
    case Ast::NULL_LITERAL:
        if (need_value)
        {
            PutOp(OP_ACONST_NULL);
            return 1;
        }
        return 0;
    default:
        assert(false && "unknown expression kind");
        break;
    }
    return 0; // even though we will not reach here
}


AstExpression *ByteCode::VariableExpressionResolution(AstExpression *expression)
{
    //
    // JLS2 added ability for parenthesized variable to remain a variable.
    //
    expression = StripNops(expression);

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


TypeSymbol *ByteCode::VariableTypeResolution(AstExpression *expression,
                                             VariableSymbol *sym)
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
                             : (simple_name -> resolution_opt
                                ? simple_name -> resolution_opt -> Type()
                                : unit_type));

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
        assert(expression -> SimpleNameCast() &&
               "unexpected AssignmentExpressionField operand base type");

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
    // Since the VM does not have a nice way of finding a class without a
    // runtime object, we use this approach.  Notice that forName can throw
    // a checked exception, but JLS semantics do not allow this, so we must
    // add a catch block to convert the problem to an unchecked Error.
    // Likewise, note that we must not initialize the class in question,
    // hence the use of forName on array types in all cases.
    //
    // The generated code is semantically equivalent to:
    //
    // /*synthetic*/ static java.lang.Class class$(java.lang.String name,
    //                                             boolean array) {
    //     try {
    //         Class result = java.lang.Class.forName(name);
    //         return array ? result : result.getComponentType();
    //     } catch (ClassNotFoundException e) {
    //         throw new NoClassDefFoundError(((Throwable) e).getMessage());
    //     }
    // }
    //
    // When option.target >= SDK1_4, we use the new exception chaining,
    // and the catch clause becomes
    //   throw (Error) ((Throwable) new NoClassDefFoundError()).initCause(e);
    //
    // Since ClassNotFoundException inherits, rather than declares, getMessage,
    // we link to Throwable, and use the cast to Throwable in the code above to
    // show that we are still obeying JLS 13.1, which requires that .class
    // files must link to the type of the qualifying expression.
    //
    //  aload_0        load class name in array form
    //  invokestatic   java/lang/Class.forName(Ljava/lang/String;)Ljava/lang/Class;
    //  iload_1        load array
    //  ifne label
    //  invokevirtual  java/lang/Class.getComponentType()Ljava/lang/Class;
    //  label:
    //  areturn        return Class object
    //
    // pre-SDK1_4 exception handler if forName fails (optimization: the
    // ClassNotFoundException will already be on the stack):
    //
    //  invokevirtual  java/lang/Throwable.getMessage()Ljava/lang/String;
    //  new            java/lang/NoClassDefFoundError
    //  dup_x1         save copy to throw, but below string arg to constructor
    //  swap           swap string and new object to correct order
    //  invokespecial  java/lang/NoClassDefFoundError.<init>(Ljava/lang/String;)V
    //  athrow         throw the correct exception
    //
    // post-SDK1_4 exception handler if forName fails (optimization: the
    // ClassNotFoundException will already be on the stack):
    //
    //  new            java/lang/NoClassDefFoundError
    //  dup_x1         save copy, but below cause
    //  invokespecial  java/lang/NoClassDefFoundError.<init>()V
    //  invokevirtual  java/lang/Throwable.initCause(Ljava/lang/Throwable;)Ljava/lang/Throwable;
    //  athrow         throw the correct exception
    //
    Label label;
    PutOp(OP_ALOAD_0);
    PutOp(OP_INVOKESTATIC);
    PutU2(RegisterLibraryMethodref(control.Class_forNameMethod()));
    PutOp(OP_ILOAD_1);
    EmitBranch(OP_IFNE, label);
    PutOp(OP_INVOKEVIRTUAL);
    PutU2(RegisterLibraryMethodref(control.Class_getComponentTypeMethod()));
    ChangeStack(1); // account for the return
    DefineLabel(label);
    CompleteLabel(label);
    PutOp(OP_ARETURN);
    code_attribute ->
      AddException(0, 12, 12, RegisterClass(control.ClassNotFoundException()));

    ChangeStack(1); // account for the exception on the stack
    if (control.option.target < JikesOption::SDK1_4)
    {
        PutOp(OP_INVOKEVIRTUAL);
        PutU2(RegisterLibraryMethodref(control.Throwable_getMessageMethod()));
        ChangeStack(1); // account for the returned string
        PutOp(OP_NEW);
        PutU2(RegisterClass(control.NoClassDefFoundError()));
        PutOp(OP_DUP_X1);
        PutOp(OP_SWAP);
        PutOp(OP_INVOKESPECIAL);
        PutU2(RegisterLibraryMethodref(control.NoClassDefFoundError_InitStringMethod()));
        ChangeStack(-1); // account for the argument to the constructor
    }
    else
    {
        PutOp(OP_NEW);
        PutU2(RegisterClass(control.NoClassDefFoundError()));
        PutOp(OP_DUP_X1);
        PutOp(OP_INVOKESPECIAL);
        PutU2(RegisterLibraryMethodref(control.NoClassDefFoundError_InitMethod()));
        PutOp(OP_INVOKEVIRTUAL);
        PutU2(RegisterLibraryMethodref(control.Throwable_initCauseMethod()));
    }
    PutOp(OP_ATHROW);
}


//
// here to generate code to dymanically initialize the field for a class
// literal and then return its value
//
int ByteCode::GenerateClassAccess(AstFieldAccess *field_access,
                                  bool need_value)
{
    //
    // Evaluate X.class literal. If X is a primitive type, this is a
    // predefined field, and we emitted it directly rather than relying on
    // this method. Otherwise, we have created a synthetic field to cache
    // the desired result, and we initialize it at runtime. Within a class,
    // this cannot be done in the static initializer, because it is possible
    // to access a class literal before a class is initialized.
    //
    // Foo.Bar.class becomes
    // (class$Foo$Bar == null ? class$Foo$Bar = class$("[LFoo.Bar;", false)
    //                        : class$Foo$Bar)
    // int[].class becomes
    // (array$I == null ? array$I = class$("[I", true) : array$I)
    //
    // getstatic class_field     load class field
    // dup                       optimize: common case is non-null
    // ifnonnull label           branch if it exists, otherwise initialize
    // pop                       pop the null we just duplicated
    // load class_constant       get name of class
    // iconst_x                  true iff array
    // invokestatic              invoke synthetic class$ method
    // dup                       save value so can return it
    // put class_field           initialize the field
    // label:
    //
    Label label;
    assert(field_access -> symbol -> VariableCast());
    VariableSymbol *cache = (VariableSymbol *) field_access -> symbol;

    u2 field_index = RegisterFieldref(cache);

    PutOp(OP_GETSTATIC);
    PutU2(field_index);
    if (need_value)
        PutOp(OP_DUP);
    EmitBranch(OP_IFNONNULL, label);

    if (need_value)
        PutOp(OP_POP);
    TypeSymbol *type = field_access -> base -> Type();
    if (type -> num_dimensions > 255)
    {
        semantic.ReportSemError(SemanticError::ARRAY_OVERFLOW,
                                field_access -> LeftToken(),
                                field_access -> RightToken());
    }
    bool is_array = type -> IsArray();
    if (! is_array)
        type = type -> GetArrayType(control.system_semantic, 1);
    LoadLiteral(type -> FindOrInsertClassLiteralName(control),
                control.String());
    PutOp(is_array ? OP_ICONST_1 : OP_ICONST_0);
    PutOp(OP_INVOKESTATIC);
    CompleteCall(cache -> ContainingType() -> ClassLiteralMethod(), 2);
    if (need_value)
        PutOp(OP_DUP);
    PutOp(OP_PUTSTATIC);
    PutU2(field_index);

    DefineLabel(label);
    CompleteLabel(label);

    return need_value ? 1 : 0;
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
    // private static final boolean $noassert
    //     = ! Class.forName("[L<outermostClass>;").getComponentType()
    //     .desiredAssertionStatus();
    //
    //  ldc              "L[<outermostClass>;"
    //  invokevirtual    java/lang/Class.forName(Ljava/lang/String;)java/lang/Class
    //  invokevirtual    java/lang/Class.getComponentType()Ljava/lang/Class;
    //  invokevirtual    java/lang/Class.desiredAssertionStatus()Z
    //  iconst_1
    //  ixor             result ^ true <=> !result
    //  putstatic        <thisClass>.$noassert
    //
    tsym = tsym -> GetArrayType(control.system_semantic, 1);
    LoadLiteral(tsym -> FindOrInsertClassLiteralName(control),
                control.String());
    PutOp(OP_INVOKESTATIC);
    PutU2(RegisterLibraryMethodref(control.Class_forNameMethod()));
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

    if (expression -> Type() -> num_dimensions > 255)
    {
        semantic.ReportSemError(SemanticError::ARRAY_OVERFLOW,
                                expression -> LeftToken(),
                                expression -> RightToken());
    }

    if (expression -> array_initializer_opt)
    {
        InitializeArray(expression -> Type(),
                        expression -> array_initializer_opt);
    }
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
int ByteCode::EmitAssignmentExpression(AstAssignmentExpression *assignment_expression,
                                       bool need_value)
{
    //
    // JLS2 added ability for parenthesized variable to remain a variable.
    //
    AstCastExpression *casted_left_hand_side =
        assignment_expression -> left_hand_side -> CastExpressionCast();
    AstExpression *left_hand_side
        = StripNops(casted_left_hand_side
                    ? casted_left_hand_side -> expression
                    : assignment_expression -> left_hand_side);

    TypeSymbol *left_type = left_hand_side -> Type();

    VariableCategory kind = GetLhsKind(assignment_expression);
    VariableSymbol *accessed_member = assignment_expression -> write_method
        ? assignment_expression -> write_method -> accessed_member -> VariableCast()
        : (VariableSymbol *) NULL;

    if (assignment_expression -> SimpleAssignment())
    {
        switch (kind)
        {
        case LHS_ARRAY:
            // lhs must be array access
            EmitArrayAccessLhs(left_hand_side -> ArrayAccessCast());
            break;
        case LHS_FIELD:
            // load base for field access
            EmitFieldAccessLhsBase(left_hand_side);
            break;
        case LHS_STATIC:
            //
            // If the access is qualified by an arbitrary base
            // expression, evaluate it for side effects.
            //
            if (left_hand_side -> FieldAccessCast())
            {
                AstExpression *base =
                    ((AstFieldAccess *) left_hand_side) -> base;
                EmitExpression(base, false);
            }
            break;
        case LHS_METHOD:
            // need to load address of object, obtained from resolution
            if (! accessed_member -> ACC_STATIC())
            {
                AstExpression *resolve = left_hand_side -> FieldAccessCast()
                    ? left_hand_side -> FieldAccessCast() -> resolution_opt
                    : left_hand_side -> SimpleNameCast() -> resolution_opt;

                assert(resolve);

                AstFieldAccess *field_expression = resolve ->
                    MethodInvocationCast() -> method -> FieldAccessCast();

                assert(field_expression);

                EmitExpression(field_expression -> base);
            }
            else if (left_hand_side -> FieldAccessCast())
                //
                // If the access is qualified by an arbitrary base
                // expression, evaluate it for side effects.
                //
                EmitExpression(((AstFieldAccess *) left_hand_side) -> base,
                               false);
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
            // lhs must be array access
            EmitArrayAccessLhs(left_hand_side -> ArrayAccessCast());
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
                ((assignment_expression -> assignment_tag ==
                  AstAssignmentExpression::PLUS_EQUAL) ||
                 (assignment_expression -> assignment_tag ==
                  AstAssignmentExpression::MINUS_EQUAL)))
            {
                IntLiteralValue *vp = DYNAMIC_CAST<IntLiteralValue *>
                    (assignment_expression -> expression -> value);
                int val = ((assignment_expression -> assignment_tag ==
                            AstAssignmentExpression::MINUS_EQUAL)
                           ? -(vp -> value) // we treat "a -= x" as "a += (-x)"
                           : vp -> value);
                if (val >= -32768 && val < 32768) // if value in range
                {
                    VariableSymbol *sym =
                        (VariableSymbol *) left_hand_side -> symbol;
                    PutOpIINC(sym -> LocalVariableIndex(), val);
                    LoadVariable(LHS_LOCAL, left_hand_side, need_value);
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
        if ((assignment_expression -> assignment_tag ==
             AstAssignmentExpression::PLUS_EQUAL) &&
            left_type == control.String())
        {
            PutOp(OP_NEW);
            PutU2(RegisterClass(control.StringBuffer()));
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
            Opcode opc;

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
                    PutOp(control.IsDoubleWordType(left_type)
                          ? OP_DUP2 : OP_DUP);
                else PutOp(control.IsDoubleWordType(left_type)
                           ? OP_DUP2_X1 : OP_DUP_X1);
            }

            int stack_words = (GetTypeWords(left_type) +
                               (accessed_member -> ACC_STATIC() ? 0 : 1));
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
// BINARY: Similar code patterns are used for the ordered comparisons. This
// method relies on the compiler having already inserted numeric promotion
// casts, so that the type of the left and right expressions match.
//
int ByteCode::EmitBinaryExpression(AstBinaryExpression *expression)
{
    TypeSymbol *type = expression -> Type();

    //
    // First, special case instanceof and string concatenation.
    //
    if (expression -> binary_tag == AstBinaryExpression::INSTANCEOF)
    {
        TypeSymbol *left_type = expression -> left_expression -> Type();
        TypeSymbol *right_type = expression -> right_expression -> Type();
        if (right_type -> num_dimensions > 255)
        {
            semantic.ReportSemError(SemanticError::ARRAY_OVERFLOW,
                                    expression -> right_expression -> LeftToken(),
                                    expression -> right_expression -> RightToken());
        }
        if (left_type == control.null_type)
        {
            //
            // We know the result: false. But emit the left expression,
            // in case of side effects in (expr ? null : null).
            //
            EmitExpression(expression -> left_expression, false);
            PutOp(OP_ICONST_0);
        }
        else if (expression -> left_expression -> IsConstant() ||
                 expression -> left_expression -> BinaryExpressionCast())
        {
            //
            // We know the result: true, since the expression is non-null
            // and String is a final class.
            //
            assert(left_type == control.String());
            EmitExpression(expression -> left_expression, false);
            PutOp(OP_ICONST_1);
        }
        else if ((expression -> left_expression -> IsThisExpression() ||
                  expression -> left_expression -> IsSuperExpression() ||
                  expression -> left_expression -> ClassInstanceCreationExpressionCast() ||
                  expression -> left_expression -> ArrayCreationExpressionCast()) &&
                 (left_type -> IsSubclass(right_type) ||
                  left_type -> Implements(right_type)))
        {
            //
            // We know the result: true, since the expression is non-null.
            //
            EmitExpression(expression -> left_expression, false);
            PutOp(OP_ICONST_1);
        }
        else
        {
            EmitExpression(expression -> left_expression);
            PutOp(OP_INSTANCEOF);
            PutU2(right_type -> num_dimensions > 0
                  ? RegisterClass(right_type -> signature)
                  : RegisterClass(right_type));
        }
        return 1;
    }

    if (type == control.String())
    {
        assert(expression -> binary_tag == AstBinaryExpression::PLUS);
        ConcatenateString(expression);
        PutOp(OP_INVOKEVIRTUAL);
        PutU2(RegisterLibraryMethodref(control.StringBuffer_toStringMethod()));
        ChangeStack(1); // account for return value
        return 1;
    }

    //
    // Next, try to simplify if one operand known to be zero or one.
    //
    if (IsZero(expression -> left_expression))
    {
        //
        // Undo compiler-inserted numeric promotion, as well as narrowing from
        // long to int in shifts, to avoid unnecessary type conversions.
        //
        AstExpression *right_expr = expression -> right_expression;
        if (right_expr -> CastExpressionCast() && right_expr -> generated)
            right_expr = ((AstCastExpression *) right_expr) -> expression;
        TypeSymbol *right_type = right_expr -> Type();

        switch (expression -> binary_tag)
        {
        case AstBinaryExpression::AND_AND:
            PutOp(OP_ICONST_0);
            return 1;
        case AstBinaryExpression::EQUAL_EQUAL:
            if (right_type != control.boolean_type)
                break;
            EmitExpression(right_expr);
            PutOp(OP_ICONST_1);
            PutOp(OP_IXOR);
            return 1;
        case AstBinaryExpression::NOT_EQUAL:
            if (right_type != control.boolean_type)
                break;
            // Fallthrough on boolean case!
        case AstBinaryExpression::PLUS:
        case AstBinaryExpression::IOR:
        case AstBinaryExpression::XOR:
        case AstBinaryExpression::OR_OR:
            //
            // Note that +0.0 + expr cannot be simplified if expr is floating
            // point, because of -0.0 rules.
            //
            if (control.IsFloatingPoint(right_type))
            {
                if (expression -> left_expression -> Type() ==
                    control.float_type)
                {
                    FloatLiteralValue *value = DYNAMIC_CAST<FloatLiteralValue *>
                        (expression -> left_expression -> value);
                    if (value -> value.IsPositiveZero())
                        break;
                }
                else if (expression -> left_expression -> Type() ==
                         control.double_type)
                {
                    DoubleLiteralValue *value = DYNAMIC_CAST<DoubleLiteralValue *>
                        (expression -> left_expression -> value);
                    if (value -> value.IsPositiveZero())
                        break;
                }
            }
            // Use promoted version, not the stripped right_expr.
            EmitExpression(expression -> right_expression);
            return GetTypeWords(type);
        case AstBinaryExpression::STAR:
        case AstBinaryExpression::AND:
        case AstBinaryExpression::LEFT_SHIFT:
        case AstBinaryExpression::RIGHT_SHIFT:
        case AstBinaryExpression::UNSIGNED_RIGHT_SHIFT:
            //
            // Floating point multiplication by 0 cannot be simplified, because
            // of NaN, infinity, and -0.0 rules. And in general, division
            // cannot be simplified because of divide by 0 for integers and
            // corner cases for floating point.
            //
            if (control.IsFloatingPoint(type))
                break;

            EmitExpression(right_expr, false);
            PutOp(type == control.long_type ? OP_LCONST_0 : OP_ICONST_0);
            return GetTypeWords(type);
        case AstBinaryExpression::MINUS:
            //
            // 0 - x is negation, but note that +0.0 - expr cannot be
            // simplified if expr is floating point, because of -0.0 rules.
            //
            if (control.IsFloatingPoint(right_type))
            {
                if (expression -> left_expression -> Type() ==
                    control.float_type)
                {
                    FloatLiteralValue *value = DYNAMIC_CAST<FloatLiteralValue *>
                        (expression -> left_expression -> value);
                    if (value -> value.IsPositiveZero())
                        break;
                }
                else if (expression -> left_expression -> Type() ==
                         control.double_type)
                {
                    DoubleLiteralValue *value = DYNAMIC_CAST<DoubleLiteralValue *>
                        (expression -> left_expression -> value);
                    if (value -> value.IsPositiveZero())
                        break;
                }
            }
            // Use promoted version, not the stripped right_expr.
            EmitExpression(expression -> right_expression);

            PutOp(control.IsSimpleIntegerValueType(type) ? OP_INEG
                  : type == control.long_type ? OP_LNEG
                  : type == control.float_type ? OP_FNEG
                  : OP_DNEG); // double_type
            return GetTypeWords(type);
        default:
            break;
        }
    }

    if (IsOne(expression -> left_expression))
    {
        if (expression -> binary_tag == AstBinaryExpression::STAR)
        {
            EmitExpression(expression -> right_expression);
            return GetTypeWords(type);
        }
        if (expression -> left_expression -> Type() == control.boolean_type)
        {
            switch (expression -> binary_tag)
            {
            case AstBinaryExpression::EQUAL_EQUAL:
            case AstBinaryExpression::AND_AND:
            case AstBinaryExpression::AND:
                EmitExpression(expression -> right_expression);
                break;
            case AstBinaryExpression::IOR:
                EmitExpression(expression -> right_expression, false);
                // Fallthrough
            case AstBinaryExpression::OR_OR:
                PutOp(OP_ICONST_1);
                break;
            case AstBinaryExpression::NOT_EQUAL:
            case AstBinaryExpression::XOR:
                EmitExpression(expression -> right_expression);
                PutOp(OP_ICONST_1);
                PutOp(OP_IXOR);
                break;
            default:
                assert(false && "Invalid operator on boolean");
            }
            return 1;
        }
    }

    if (IsZero(expression -> right_expression))
    {
        //
        // Undo compiler-inserted numeric promotion to avoid unnecessary type
        // conversions.
        //
        AstExpression *left_expr = expression -> left_expression;
        if (left_expr -> CastExpressionCast() && left_expr -> generated)
            left_expr = ((AstCastExpression *) left_expr) -> expression;
        TypeSymbol *left_type = left_expr -> Type();

        switch (expression -> binary_tag)
        {
        case AstBinaryExpression::AND_AND:
            EmitExpression(left_expr, false);
            PutOp(OP_ICONST_0);
            return 1;
        case AstBinaryExpression::EQUAL_EQUAL:
            if (left_type != control.boolean_type)
                break;
            EmitExpression(left_expr);
            PutOp(OP_ICONST_1);
            PutOp(OP_IXOR);
            return 1;
        case AstBinaryExpression::NOT_EQUAL:
            if (left_type != control.boolean_type)
                break;
            // Fallthrough on boolean case!
        case AstBinaryExpression::PLUS:
        case AstBinaryExpression::MINUS:
        case AstBinaryExpression::IOR:
        case AstBinaryExpression::XOR:
        case AstBinaryExpression::OR_OR:
        case AstBinaryExpression::LEFT_SHIFT:
        case AstBinaryExpression::RIGHT_SHIFT:
        case AstBinaryExpression::UNSIGNED_RIGHT_SHIFT:
            //
            // Here for cases that simplify to the left operand. Note that
            // (expr + +0.0) and (expr - -0.0) cannot be simplified if expr
            // is floating point, because of -0.0 rules.
            //
            if (control.IsFloatingPoint(left_type))
            {
                if (expression -> right_expression -> Type() ==
                    control.float_type)
                {
                    FloatLiteralValue *value = DYNAMIC_CAST<FloatLiteralValue *>
                        (expression -> right_expression -> value);
                    if (value -> value.IsPositiveZero() ==
                        (expression -> binary_tag == AstBinaryExpression::PLUS))
                        break;
                }
                else if (expression -> right_expression -> Type() ==
                         control.double_type)
                {
                    DoubleLiteralValue *value = DYNAMIC_CAST<DoubleLiteralValue *>
                        (expression -> right_expression -> value);
                    if (value -> value.IsPositiveZero() ==
                        (expression -> binary_tag == AstBinaryExpression::PLUS))
                        break;
                }
            }
            // Use promoted version, not the stripped left_expr.
            EmitExpression(expression -> left_expression);
            return GetTypeWords(type);
        case AstBinaryExpression::STAR:
        case AstBinaryExpression::AND:
            //
            // Floating point multiplication by 0 cannot be simplified, because
            // of NaN, infinity, and -0.0 rules. And in general, division
            // cannot be simplified because of divide by 0 for integers and
            // corner cases for floating point.
            //
            if (control.IsFloatingPoint(type))
                break;

            EmitExpression(left_expr, false);
            PutOp(type == control.long_type ? OP_LCONST_0 : OP_ICONST_0);
            return GetTypeWords(type);
        default:
            break;
        }
    }

    if (IsOne(expression -> right_expression))
    {
        if (expression -> binary_tag == AstBinaryExpression::STAR ||
            expression -> binary_tag == AstBinaryExpression::SLASH)
        {
            EmitExpression(expression -> left_expression);
            return GetTypeWords(type);
        }
        if (expression -> right_expression -> Type() == control.boolean_type)
        {
            switch (expression -> binary_tag)
            {
            case AstBinaryExpression::EQUAL_EQUAL:
            case AstBinaryExpression::AND_AND:
            case AstBinaryExpression::AND:
                EmitExpression(expression -> left_expression);
                break;
            case AstBinaryExpression::IOR:
                EmitExpression(expression -> left_expression, false);
                // Fallthrough
            case AstBinaryExpression::OR_OR:
                PutOp(OP_ICONST_1);
                break;
            case AstBinaryExpression::NOT_EQUAL:
            case AstBinaryExpression::XOR:
                EmitExpression(expression -> left_expression);
                PutOp(OP_ICONST_1);
                PutOp(OP_IXOR);
                break;
            default:
                assert(false && "Invalid operator on boolean");
            }
            return 1;
        }
    }

    //
    // Next, simplify all remaining boolean result expressions.
    //
    if (expression -> left_expression -> Type() == control.boolean_type &&
        (expression -> binary_tag == AstBinaryExpression::EQUAL_EQUAL ||
         expression -> binary_tag == AstBinaryExpression::NOT_EQUAL))
    {
        EmitExpression(expression -> left_expression);
        EmitExpression(expression -> right_expression);
        PutOp(OP_IXOR);
        if (expression -> binary_tag == AstBinaryExpression::EQUAL_EQUAL)
        {
            PutOp(OP_ICONST_1);
            PutOp(OP_IXOR);
        }
        return 1;
    }

    switch (expression -> binary_tag)
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
            EmitBranchIfExpression(expression, false, label);
            PutOp(OP_POP); // pop the false
            PutOp(OP_ICONST_1); // push true
            DefineLabel(label);
            CompleteLabel(label);
        }
        return 1;
    default:
        break;
    }

    //
    // Finally, if we get here, the expression is not boolean, and cannot be
    // optimized.
    //
    EmitExpression(expression -> left_expression);
    EmitExpression(expression -> right_expression);

    bool integer_type = (control.IsSimpleIntegerValueType(type) ||
                         type == control.boolean_type);
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


int ByteCode::EmitCastExpression(AstCastExpression *expression,
                                 bool need_value)
{
    TypeSymbol *dest_type = expression -> Type();
    TypeSymbol *source_type = expression -> expression -> Type();
    if (dest_type -> num_dimensions > 255)
    {
        semantic.ReportSemError(SemanticError::ARRAY_OVERFLOW,
                                expression -> type_opt -> LeftToken(),
                                expression -> right_parenthesis_token_opt - 1);
    }

    //
    // Object downcasts must be emitted, in case of a ClassCastException.
    //
    EmitExpression(expression -> expression,
                   need_value || dest_type -> IsSubclass(source_type));

    if (need_value || dest_type -> IsSubclass(source_type))
    {
        EmitCast(dest_type, source_type);
        if (! need_value)
        {
            assert(source_type -> IsSubclass(control.Object()));
            PutOp(OP_POP);
        }
    }

    return need_value ? GetTypeWords(dest_type) : 0;
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
        Opcode op_kind = (dest_type == control.long_type ? OP_I2L
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
        Opcode op_kind = (dest_type == control.float_type ? OP_L2F
                          : dest_type == control.double_type ? OP_L2D
                          : OP_L2I);
        PutOp(op_kind);

        if (op_kind == OP_L2I && dest_type != control.int_type)
        {
            assert(control.IsSimpleIntegerValueType(dest_type) &&
                   "unsupported conversion");

            PutOp(dest_type == control.char_type ? OP_I2C
                  : dest_type == control.byte_type ? OP_I2B
                  : OP_I2S); // short_type
        }
    }
    else if (source_type == control.float_type)
    {
        Opcode op_kind = (dest_type == control.long_type ? OP_F2L
                          : dest_type == control.double_type ? OP_F2D
                          : OP_F2I);
        PutOp(op_kind);

        if (op_kind == OP_F2I && dest_type != control.int_type)
        {
            assert(control.IsSimpleIntegerValueType(dest_type) &&
                   "unsupported conversion");

            PutOp(dest_type == control.char_type ? OP_I2C
                  : dest_type == control.byte_type ? OP_I2B
                  : OP_I2S); // short_type
        }
    }
    else if (source_type == control.double_type)
    {
        Opcode op_kind = (dest_type == control.long_type ? OP_D2L
                          : dest_type == control.float_type ? OP_D2F
                          : OP_D2I);

        PutOp(op_kind);

        if (op_kind == OP_D2I && dest_type != control.int_type)
        {
            assert(control.IsSimpleIntegerValueType(dest_type) &&
                   "unsupported conversion");

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
              : RegisterClass(dest_type));
    }
}

//
// Emits the required check for null in a qualified instance creation,
// super constructor call, or constant instance variable reference, if the
// base expression can possibly be null. It also emits the base expression.
// In the case of anonymous classes, we emit an alternate expression (the
// constructor parameter), after performing the null check on the qualifier
// of the anonymous class instance creation expression.
//
void ByteCode::EmitCheckForNull(AstExpression *expression, bool need_value)
{
    expression = StripNops(expression);

    if (expression -> Type() == control.null_type)
    {
        //
        // It's guaranteed to be null, so cause any side effects, then throw
        // the null already on the stack (which will make the VM correctly
        // create and throw a NullPointerException). Adjust the stack if
        // necessary, since the calling context does not realize that this
        // will always complete abruptly.
        //
        EmitExpression(expression, true);
        PutOp(OP_ATHROW);
        if (need_value)
            ChangeStack(1);
        return;
    }
    VariableSymbol *variable = expression -> symbol -> VariableCast();
    if (expression -> ClassInstanceCreationExpressionCast() ||
        expression -> IsThisExpression() ||
        expression -> IsSuperExpression() ||
        (variable && variable -> IsSynthetic() &&
         variable -> Identity() == control.this0_name_symbol))
    {
        EmitExpression(expression, need_value);
        return;
    }
    //
    // We did not bother checking for other guaranteed non-null conditions:
    // IsConstant(), string concats, and ArrayCreationExpressionCast(), since
    // none of these can qualify a constructor invocation or a constant
    // instance field reference. If we get here, it is uncertain whether the
    // expression can be null, so check, using:
    //
    // ((Object) ref).getClass();
    //
    // This discarded instance method call will cause the necessary
    // NullPointerException if invoked on null; and since it is final in
    // Object, we can be certain it has no side-effects.
    //
    EmitExpression(expression, true);
    if (need_value)
        PutOp(OP_DUP);
    PutOp(OP_INVOKEVIRTUAL);
    ChangeStack(1); // for returned value
    PutU2(RegisterLibraryMethodref(control.Object_getClassMethod()));
    PutOp(OP_POP);
}

int ByteCode::EmitInstanceCreationExpression(AstClassInstanceCreationExpression *expression,
                                             bool need_value)
{
    if (expression -> resolution_opt)
        expression = expression -> resolution_opt;
    MethodSymbol *constructor =
        (MethodSymbol *) expression -> class_type -> symbol;

    PutOp(OP_NEW);
    PutU2(RegisterClass(expression -> Type()));
    if (need_value) // save address of new object for constructor
        PutOp(OP_DUP);

    //
    // Pass enclosing instance along, then real arguments, then shadow
    // variables, and finally an extra null argument, as needed.
    //
    int stack_words = 0;
    int i = 0;
    if (expression -> base_opt)
    {
        stack_words++;
        EmitCheckForNull(expression -> base_opt);
    }
    if (expression -> Type() -> Anonymous() &&
        expression -> Type() -> super -> EnclosingInstance())
    {
        stack_words++;
        EmitCheckForNull(expression -> Argument(i++));
    }
    for ( ; i < expression -> NumArguments(); i++)
        stack_words += EmitExpression(expression -> Argument(i));
    for (i = 0; i < expression -> NumLocalArguments(); i++)
        stack_words += EmitExpression(expression -> LocalArgument(i));
    if (expression -> NeedsExtraNullArgument())
    {
        PutOp(OP_ACONST_NULL);
        stack_words += 1;
    }

    PutOp(OP_INVOKESPECIAL);
    ChangeStack(-stack_words);
    PutU2(RegisterMethodref(expression -> Type(), control.init_name_symbol,
                            constructor));

    return 1;
}


int ByteCode::EmitConditionalExpression(AstConditionalExpression *expression,
                                        bool need_value)
{
    //
    // Optimize (true ? a : b) to (a).
    // Optimize (false ? a : b) (b).
    //
    if (expression -> test_expression -> IsConstant())
        return EmitExpression((IsZero(expression -> test_expression)
                               ? expression -> false_expression
                               : expression -> true_expression),
                              need_value);
    if (expression -> Type() == control.null_type &&
        expression -> false_expression -> NullLiteralCast() &&
        expression -> true_expression -> NullLiteralCast())
    {
        //
        // Optimize (cond ? null : null) to (cond, null).
        //
        EmitExpression(expression -> test_expression, false);
        if (need_value)
            PutOp(OP_ACONST_NULL);
        return need_value ? 1 : 0;
    }
    if (expression -> false_expression -> IsConstant() &&
        expression -> true_expression -> IsConstant())
    {
        if (! need_value)
            return EmitExpression(expression -> test_expression, false);
        //
        // Optimize (cond ? expr : expr) to (cond, expr).
        //
        if (expression -> Type() == control.String())
        {
            Utf8LiteralValue *left = DYNAMIC_CAST<Utf8LiteralValue *>
                (expression -> true_expression -> value);
            Utf8LiteralValue *right = DYNAMIC_CAST<Utf8LiteralValue *>
                (expression -> false_expression -> value);
            if (! strcmp(left -> value, right -> value))
            {
                EmitExpression(expression -> test_expression, false);
                return EmitExpression(expression -> true_expression);
            }
        }
        else if (expression -> Type() == control.double_type)
        {
            DoubleLiteralValue *left = DYNAMIC_CAST<DoubleLiteralValue *>
                (expression -> true_expression -> value);
            DoubleLiteralValue *right = DYNAMIC_CAST<DoubleLiteralValue *>
                (expression -> false_expression -> value);
            if (left -> value == right -> value)
            {
                EmitExpression(expression -> test_expression, false);
                return EmitExpression(expression -> true_expression);
            }
        }
        else if (expression -> Type() == control.float_type)
        {
            FloatLiteralValue *left = DYNAMIC_CAST<FloatLiteralValue *>
                (expression -> true_expression -> value);
            FloatLiteralValue *right = DYNAMIC_CAST<FloatLiteralValue *>
                (expression -> false_expression -> value);
            if (left -> value == right -> value)
            {
                EmitExpression(expression -> test_expression, false);
                return EmitExpression(expression -> true_expression);
            }
        }
        else if (expression -> Type() == control.long_type)
        {
            LongLiteralValue *left = DYNAMIC_CAST<LongLiteralValue *>
                (expression -> true_expression -> value);
            LongLiteralValue *right = DYNAMIC_CAST<LongLiteralValue *>
                (expression -> false_expression -> value);
            if (left -> value == right -> value)
            {
                EmitExpression(expression -> test_expression, false);
                return EmitExpression(expression -> true_expression);
            }
        }
        else
        {
            assert(control.IsSimpleIntegerValueType(expression -> Type()) ||
                   expression -> Type() == control.boolean_type);
            IntLiteralValue *left = DYNAMIC_CAST<IntLiteralValue *>
                (expression -> true_expression -> value);
            IntLiteralValue *right = DYNAMIC_CAST<IntLiteralValue *>
                (expression -> false_expression -> value);
            if (left -> value == right -> value)
            {
                EmitExpression(expression -> test_expression, false);
                return EmitExpression(expression -> true_expression);
            }
            //
            // Optimize (expr ? 1 : 0) to (expr).
            // Optimize (expr ? value + 1 : value) to (expr + value).
            // Optimize (expr ? value - 1 : value) to (value - expr).
            //
            if (left -> value == 1 && right -> value == 0)
                return EmitExpression(expression -> test_expression);
            if (left -> value == right -> value + 1)
            {
                EmitExpression(expression -> test_expression);
                EmitExpression(expression -> false_expression);
                PutOp(OP_IADD);
                return 1;
            }
            if (left -> value == right -> value - 1)
            {
                EmitExpression(expression -> false_expression);
                EmitExpression(expression -> test_expression);
                PutOp(OP_ISUB);
                return 1;
            }
        }
    }
    Label lab1,
        lab2;
    EmitBranchIfExpression(expression -> test_expression, false, lab1);
    EmitExpression(expression -> true_expression, need_value);
    EmitBranch(OP_GOTO, lab2);
    if (need_value) // restore the stack size
        ChangeStack(- GetTypeWords(expression -> Type()));
    DefineLabel(lab1);
    EmitExpression(expression -> false_expression, need_value);
    DefineLabel(lab2);
    CompleteLabel(lab2);
    CompleteLabel(lab1);
    return GetTypeWords(expression -> true_expression -> Type());
}


int ByteCode::EmitFieldAccess(AstFieldAccess *expression, bool need_value)
{
    assert(! expression -> IsConstant());

    AstExpression *base = expression -> base;
    VariableSymbol *sym = expression -> symbol -> VariableCast();

    if (expression -> resolution_opt)
    {
        //
        // A resolution exists if the field belongs to an enclosing class and
        // has an accessor method. If the access is qualified by an arbitrary
        // base expression, evaluate it for side effects. Normally, this will
        // be done when evaluating the accessor method. However, if this is a
        // static field, and need_value is false, the access will not have a
        // side effect, so we can bypass it.
        //
        MethodSymbol *method = expression -> symbol -> MethodCast();
        if (! need_value && method && ! method -> AccessesInstanceMember())
            return EmitExpression(base, false);
        return EmitExpression(expression -> resolution_opt, need_value);
    }

    if (! sym) // not a variable, so it must be a class or package name
        return 0;
    return LoadVariable(sym -> ACC_STATIC() ? LHS_STATIC : LHS_FIELD,
                        expression, need_value);
}


void ByteCode::EmitMethodInvocation(AstMethodInvocation *expression)
{
    //
    // If the method call was resolved into a call to another method, use the
    // resolution expression.
    //
    AstMethodInvocation *method_call = expression -> resolution_opt
        ? expression -> resolution_opt -> MethodInvocationCast() : expression;
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
        stack_words +=
            EmitExpression((AstExpression *) method_call -> Argument(i));

    TypeSymbol *type = MethodTypeResolution(method_call -> method, msym);
    PutOp(msym -> ACC_STATIC() ? OP_INVOKESTATIC
          : (is_super || msym -> ACC_PRIVATE()) ? OP_INVOKESPECIAL
          : type -> ACC_INTERFACE() ? OP_INVOKEINTERFACE
          : OP_INVOKEVIRTUAL);
    CompleteCall(msym, stack_words, type);
}


void ByteCode::CompleteCall(MethodSymbol *msym,
                            int stack_words,
                            TypeSymbol *base_type)
{
    ChangeStack(-stack_words);

    TypeSymbol *type = (base_type ? base_type : msym -> containing_type);

    PutU2(type -> ACC_INTERFACE()
          ? RegisterInterfaceMethodref(type, msym -> ExternalIdentity(), msym)
          : RegisterMethodref(type, msym -> ExternalIdentity(), msym));

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
            if (sub_expr -> Type() -> IsSubclass(expr -> Type()) ||
                (sub_expr -> Type() == control.null_type &&
                 expr -> Type() -> num_dimensions <= 255))
            {
                return sub_expr;
            }
            else return expr;
        }
        else return expr;
    }

    return expr;
}


bool ByteCode::IsNop(AstBlock *block)
{
    for (int i = block -> NumStatements() - 1; i >= 0; i--)
    {
        Ast *statement = block -> Statement(i);
        if (statement -> EmptyStatementCast() ||
            (statement -> BlockCast() && IsNop((AstBlock *) statement)))
            continue;
        if (statement -> kind == Ast::IF)
        {
            AstIfStatement *ifstat = (AstIfStatement *) statement;
            assert(ifstat -> true_statement -> BlockCast() &&
                   (! ifstat -> false_statement_opt ||
                    ifstat -> false_statement_opt -> BlockCast()));
            if ((IsOne(ifstat -> expression) &&
                 IsNop((AstBlock *) ifstat -> true_statement)) ||
                (IsZero(ifstat -> expression) &&
                 (! ifstat -> false_statement_opt ||
                  IsNop((AstBlock *) ifstat -> false_statement_opt))))
            {
                continue;
            }
        }
        //
        // TODO: Is it worth adding more checks for bypassed code?
        //
        return false;
    }
    return true;
}


void ByteCode::EmitNewArray(int num_dims, TypeSymbol *type)
{
    assert(num_dims > 0);
    if (num_dims == 1)
    {
        TypeSymbol *element_type = type -> ArraySubtype();

        if (control.IsPrimitive(element_type))
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
            PutU2(RegisterClass(element_type));
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
int ByteCode::EmitPostUnaryExpression(AstPostUnaryExpression *expression,
                                      bool need_value)
{
    VariableCategory kind = GetLhsKind(expression);

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
            VariableSymbol *accessed_member =
                expression -> write_method -> accessed_member -> VariableCast();
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
void ByteCode::EmitPostUnaryExpressionField(VariableCategory kind,
                                            AstPostUnaryExpression *expression,
                                            bool need_value)
{
    if (kind == LHS_METHOD)
         ResolveAccess(expression -> expression); // get address and value
    else EmitFieldAccessLhs(expression -> expression);

    TypeSymbol *expression_type = expression -> Type();
    if (need_value)
        PutOp(control.IsDoubleWordType(expression_type)
              ? OP_DUP2_X1 : OP_DUP_X1);

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
        PutU2(RegisterFieldref(VariableTypeResolution(expression ->
                                                      expression, sym), sym));
    }
}


//
// AstExpression *expression;
// POST_UNARY on local variable
// load value of variable, duplicate, do increment or decrement, then store
// back, leaving original value on top of stack.
//
void ByteCode::EmitPostUnaryExpressionSimple(VariableCategory kind,
                                             AstPostUnaryExpression *expression,
                                             bool need_value)
{
    TypeSymbol *expression_type = expression -> Type();
    if (kind == LHS_LOCAL && expression_type == control.int_type)
    {
        // can we use IINC ??
        LoadVariable(kind, expression, need_value);
        PutOpIINC(expression -> symbol -> VariableCast() -> LocalVariableIndex(),
                  expression -> post_unary_tag == AstPostUnaryExpression::PLUSPLUS ? 1 : -1);
        return;
    }

    // this will also load value needing resolution
    LoadVariable(kind, expression -> expression);

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
void ByteCode::EmitPostUnaryExpressionArray(AstPostUnaryExpression *expression,
                                            bool need_value)
{
    //
    // JLS2 added ability for parenthesized variable to remain a variable.
    //
    EmitArrayAccessLhs((AstArrayAccess *) StripNops(expression -> expression));
    // lhs must be array access
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
int ByteCode::EmitPreUnaryExpression(AstPreUnaryExpression *expression,
                                     bool need_value)
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
            assert(need_value);
            EmitExpression(expression -> expression);
            break;
        case AstPreUnaryExpression::MINUS:
            assert(need_value);
            assert(control.IsNumeric(type) && "unary minus on bad type");

            EmitExpression(expression -> expression);
            PutOp(control.IsSimpleIntegerValueType(type) ? OP_INEG
                  : type == control.long_type ? OP_LNEG
                  : type == control.float_type ? OP_FNEG
                  : OP_DNEG); // double_type
            break;
        case AstPreUnaryExpression::TWIDDLE:
            assert(need_value);
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

            EmitExpression(expression -> expression, need_value);
            if (! need_value)
                return 0;
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
void ByteCode::EmitPreUnaryIncrementExpression(AstPreUnaryExpression *expression,
                                               bool need_value)
{
    VariableCategory kind = GetLhsKind(expression);

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
            VariableSymbol *accessed_member =
                expression -> write_method -> accessed_member -> VariableCast();
            if (accessed_member -> ACC_STATIC())
                EmitPreUnaryIncrementExpressionSimple(kind, expression,
                                                      need_value);
            else EmitPreUnaryIncrementExpressionField(kind, expression,
                                                      need_value);
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
void ByteCode::EmitPreUnaryIncrementExpressionSimple(VariableCategory kind,
                                                     AstPreUnaryExpression *expression,
                                                     bool need_value)
{
    TypeSymbol *type = expression -> Type();
    if (kind == LHS_LOCAL && type == control.int_type)
    {
        PutOpIINC(expression -> symbol -> VariableCast() -> LocalVariableIndex(),
                  expression -> pre_unary_tag == AstPreUnaryExpression::PLUSPLUS ? 1 : -1);
        LoadVariable(kind, expression, need_value);
        return;
    }

    // will also load value if resolution needed
    LoadVariable(kind, expression -> expression);

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
void ByteCode::EmitPreUnaryIncrementExpressionArray(AstPreUnaryExpression *expression,
                                                    bool need_value)
{
    //
    // JLS2 added ability for parenthesized variable to remain a variable.
    //
    // lhs must be array access
    EmitArrayAccessLhs((AstArrayAccess *) StripNops(expression -> expression));

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
void ByteCode::EmitPreUnaryIncrementExpressionField(VariableCategory kind,
                                                    AstPreUnaryExpression *expression,
                                                    bool need_value)
{
    if (kind == LHS_METHOD)
        ResolveAccess(expression -> expression); // get address and value
    else
        // need to load address of object, obtained from resolution, saving
        // a copy on the stack
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
        PutU2(RegisterFieldref(VariableTypeResolution(expression ->
                                                      expression, sym), sym));
    }
}


void ByteCode::EmitThisInvocation(AstThisCall *this_call)
{
    //
    // Pass enclosing instance along, then real arguments.
    //
    PutOp(OP_ALOAD_0); // load 'this'
    int stack_words = 0; // words on stack needed for arguments
    if (unit_type -> EnclosingType())
        LoadLocal(++stack_words, unit_type -> EnclosingType());
    for (int k = 0; k < this_call -> NumArguments(); k++)
        stack_words += EmitExpression(this_call -> Argument(k));

    //
    // Now do a transfer of the shadow variables. We do not need to worry
    // about an extra null argument, as there are no accessibility issues
    // when invoking this().
    //
    if (shadow_parameter_offset)
    {
        int offset = shadow_parameter_offset;
        for (int i = 0; i < unit_type -> NumConstructorParameters(); i++)
        {
            VariableSymbol *shadow = unit_type -> ConstructorParameter(i);
            LoadLocal(offset, shadow -> Type());
            int words = GetTypeWords(shadow -> Type());
            offset += words;
            stack_words += words;
        }
    }

    PutOp(OP_INVOKESPECIAL);
    ChangeStack(-stack_words);

    PutU2(RegisterMethodref(unit_type,
                            this_call -> symbol -> ExternalIdentity(),
                            this_call -> symbol));
}


void ByteCode::EmitSuperInvocation(AstSuperCall *super_call)
{
    //
    // Pass enclosing instance along, then real arguments, then shadow
    // variables, and finally any extra null argument for accessibility
    // issues.
    //
    PutOp(OP_ALOAD_0); // load 'this'
    int stack_words = 0; // words on stack needed for arguments
    if (super_call -> base_opt)
    {
        stack_words++;
        if (unit_type -> Anonymous())
        {
            //
            // Special case - the null check was done during the class instance
            // creation, so we skip it here.
            //
            EmitExpression(super_call -> base_opt);
        }
        else EmitCheckForNull(super_call -> base_opt);
    }
    for (int k = 0; k < super_call -> NumArguments(); k++)
        stack_words += EmitExpression(super_call -> Argument(k));
    for (int i = 0; i < super_call -> NumLocalArguments(); i++)
        stack_words += EmitExpression(super_call -> LocalArgument(i));
    if (super_call -> NeedsExtraNullArgument())
    {
        PutOp(OP_ACONST_NULL);
        stack_words += 1;
    }

    PutOp(OP_INVOKESPECIAL);
    ChangeStack(-stack_words);
    PutU2(RegisterMethodref(unit_type -> super,
                            super_call -> symbol -> ExternalIdentity(),
                            super_call -> symbol));
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
        left_expr -> BinaryExpressionCast() &&
        ! left_expr -> IsConstant())
    {
        ConcatenateString((AstBinaryExpression *) left_expr);
    }
    else
    {
        PutOp(OP_NEW);
        PutU2(RegisterClass(control.StringBuffer()));
        PutOp(OP_DUP);
        if (left_expr -> IsConstant())
        {
            //
            // Optimizations: if the left term is "", just append the right
            // term to an empty StringBuffer. If the left term is not "",
            // use new StringBuffer(String) to create a StringBuffer
            // that includes the left term. No need to worry about
            // new StringBuffer(null) raising a NullPointerException
            // since string constants are never null.
            //
            Utf8LiteralValue *value =
                DYNAMIC_CAST<Utf8LiteralValue *> (left_expr -> value);
            if (value -> length == 0)
            {
                PutOp(OP_INVOKESPECIAL);
                PutU2(RegisterLibraryMethodref(control.StringBuffer_InitMethod()));
            }
            else
            {
                LoadConstantAtIndex(RegisterString(value));
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
        Utf8LiteralValue *value =
            DYNAMIC_CAST<Utf8LiteralValue *> (expression -> value);
        if (value -> length == 0)
            return;  // Optimization: do nothing when appending "".
        assert(type == control.String());
        if (value -> length == 1)
        {
            // Optimization: append(char) more efficient than append(String)
            LoadImmediateInteger(value -> value[0]);
            type = control.char_type;
        }
        else if (value -> length == 2 &&
                 (value -> value[0] & 0x00E0) == 0x00C0)
        {
            // 2-byte string in UTF-8, but still single character.
            LoadImmediateInteger(((value -> value[0] & 0x001F) << 6) |
                                 (value -> value[1] & 0x003F));
            type = control.char_type;
        }
        else if (value -> length == 3 &&
                 (value -> value[0] & 0x00E0) == 0x00E0)
        {
            // 3-byte string in UTF-8, but still single character.
            LoadImmediateInteger(((value -> value[0] & 0x000F) << 12) |
                                 ((value -> value[1] & 0x003F) << 6) |
                                 (value -> value[2] & 0x003F));
            type = control.char_type;
        }
        else
            LoadConstantAtIndex(RegisterString(value));
    }
    else
    {
        AstBinaryExpression *binary_expression =
            expression -> BinaryExpressionCast();
        if (binary_expression && type == control.String())
        {
            assert(binary_expression -> binary_tag ==
                   AstBinaryExpression::PLUS);
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
        (type == control.char_type
         ? control.StringBuffer_append_charMethod()
         : type == control.boolean_type
         ? control.StringBuffer_append_booleanMethod()
         : (type == control.int_type || type == control.short_type ||
            type == control.byte_type)
         ? control.StringBuffer_append_intMethod()
         : type == control.long_type
         ? control.StringBuffer_append_longMethod()
         : type == control.float_type
         ? control.StringBuffer_append_floatMethod()
         : type == control.double_type
         ? control.StringBuffer_append_doubleMethod()
         : (type == control.String() || type == control.null_type)
         ? control.StringBuffer_append_stringMethod()
         : IsReferenceType(type)
         ? control.StringBuffer_append_objectMethod()
         : control.StringBuffer_InitMethod()); // for assertion

    assert(append_method != control.StringBuffer_InitMethod() &&
           "unable to find method for string buffer concatenation");

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
#endif // JIKES_DEBUG


ByteCode::ByteCode(TypeSymbol *unit_type)
    : ClassFile(unit_type),
      control(unit_type -> semantic_environment -> sem -> control),
      semantic(*unit_type -> semantic_environment -> sem),
      string_overflow(false),
      library_method_not_found(false),
      last_op_goto(false),
      shadow_parameter_offset(0),
      double_constant_pool_index(NULL),
      integer_constant_pool_index(NULL),
      long_constant_pool_index(NULL),
      float_constant_pool_index(NULL),
      string_constant_pool_index(NULL),

      utf8_constant_pool_index(segment_pool,
                               control.Utf8_pool.symbol_pool.Length()),
      class_constant_pool_index(segment_pool,
                                control.Utf8_pool.symbol_pool.Length()),

      name_and_type_constant_pool_index(NULL),
      fieldref_constant_pool_index(NULL),
      methodref_constant_pool_index(NULL)
{
#ifdef JIKES_DEBUG
    if (! control.option.nowrite)
        control.class_files_written++;

    if (control.option.verbose)
        Coutput << "[generating code for class "
                << unit_type -> fully_qualified_name -> value << "]" << endl;
#endif // JIKES_DEBUG

    SetFlags(unit_type -> Flags());

    magic = 0xcafebabe;
    major_version = 45;             // use Sun JDK 1.0 version numbers
    minor_version = 3;
    constant_pool.Next() = NULL;
    this_class = RegisterClass(unit_type);

    super_class = (unit_type -> super ? RegisterClass(unit_type -> super) : 0);

    for (int k = 0; k < unit_type -> NumInterfaces(); k++)
        interfaces.Next() = RegisterClass(unit_type -> Interface(k));
}


//
//  Methods for manipulating labels
//
void ByteCode::DefineLabel(Label& lab)
{
    assert(! lab.defined && "duplicate label definition");

    //
    // Optimize if previous instruction was unconditional jump to this label.
    // However, we cannot perform the optimization if another label was also
    // defined at this location.
    //
    int index = lab.uses.Length() - 1;
    if (last_op_goto && index >= 0)
    {
        unsigned int luse = lab.uses[index].use_offset;
        int start = luse - lab.uses[index].op_offset;
        if (start == last_op_pc &&
            code_attribute -> CodeLength() != last_label_pc)
        {
#ifdef JIKES_DEBUG
            if (control.option.debug_trace_stack_change)
                Coutput << "removing dead jump: pc " << start << endl;
#endif
            code_attribute -> DeleteCode(lab.uses[index].op_offset +
                                         lab.uses[index].use_length);
            lab.uses.Reset(index);
            line_number_table_attribute -> SetMax(start);
            last_op_goto = false;
        }
    }
    lab.defined = true;
    lab.definition = code_attribute -> CodeLength();
    if (lab.uses.Length())
        last_label_pc = lab.definition;
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
        // Sanity check - when completing method, make sure nothing jumps out
        // of the method. This also collapses two labels that begin on
        // the same location, before one is optimized away, as in
        // "if (b) <statement> else {}".
        //
        if (lab.definition > code_attribute -> CodeLength())
            lab.definition = code_attribute -> CodeLength();

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
                assert(offset < 32768 && offset >= -32768 &&
                       "needed longer branch offset");
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
             PutOp((Opcode) (OP_ILOAD_0 + varno)); // Exploit opcode encodings
         else PutOpWide(OP_ILOAD, varno);
    }
    else if (type == control.long_type)
    {
         if (varno <= 3)
             PutOp((Opcode) (OP_LLOAD_0 + varno)); // Exploit opcode encodings
         else PutOpWide(OP_LLOAD, varno);
    }
    else if (type == control.float_type)
    {
         if (varno <= 3)
             PutOp((Opcode) (OP_FLOAD_0 + varno)); // Exploit opcode encodings
         else PutOpWide(OP_FLOAD, varno);
    }
    else if (type == control.double_type)
    {
         if (varno <= 3)
             PutOp((Opcode) (OP_DLOAD_0 + varno)); // Exploit opcode encodings
         else PutOpWide(OP_DLOAD, varno);
    }
    else // assume reference
    {
         if (varno <= 3)
             PutOp((Opcode) (OP_ALOAD_0 + varno)); // Exploit opcode encodings
         else PutOpWide(OP_ALOAD, varno);
    }
}


//
// See if we can load without using LDC; otherwise generate constant pool
// entry if one has not yet been generated.
//
void ByteCode::LoadLiteral(LiteralValue *litp, TypeSymbol *type)
{
    if (control.IsSimpleIntegerValueType(type) || type == control.boolean_type)
    {
        // load literal using literal value
        IntLiteralValue *vp = DYNAMIC_CAST<IntLiteralValue *> (litp);
        LoadImmediateInteger(vp -> value);
    }
    else if (type == control.String() || type == control.null_type)
    {
        // register index as string if this has not yet been done
        Utf8LiteralValue *vp = DYNAMIC_CAST<Utf8LiteralValue *> (litp);
        LoadConstantAtIndex(RegisterString(vp));
    }
    else if (type == control.long_type)
    {
        LongLiteralValue *vp = DYNAMIC_CAST<LongLiteralValue *> (litp);
        if (vp -> value == 0)
            PutOp(OP_LCONST_0);
        else if (vp -> value == 1)
            PutOp(OP_LCONST_1);
        else if (vp -> value >= -1 && vp -> value <= 5)
        {
            LoadImmediateInteger(vp -> value.LowWord());
            PutOp(OP_I2L);
        }
        else
        {
            PutOp(OP_LDC2_W);
            PutU2(RegisterLong(vp));
        }
    }
    else if (type == control.float_type)
    {
        FloatLiteralValue *vp = DYNAMIC_CAST<FloatLiteralValue *> (litp);
        IEEEfloat val = vp -> value;
        if (val.IsZero())
        {
            PutOp(OP_FCONST_0);
            if (val.IsNegative())
                PutOp(OP_FNEG);
        }
        else if (val == 1.0f)
            PutOp(OP_FCONST_1);
        else if (val == 2.0f)
            PutOp(OP_FCONST_2);
        else if (val == -1.0f)
        {
            PutOp(OP_FCONST_1);
            PutOp(OP_FNEG);
        }
        else if (val == 3.0f || val == 4.0f || val == 5.0f)
        {
            LoadImmediateInteger(val.IntValue());
            PutOp(OP_I2F);
        }
        else LoadConstantAtIndex(RegisterFloat(vp));
    }
    else if (type == control.double_type)
    {
        DoubleLiteralValue *vp = DYNAMIC_CAST<DoubleLiteralValue *> (litp);
        IEEEdouble val = vp -> value;
        if (val.IsZero())
        {
            PutOp(OP_DCONST_0);
            if (val.IsNegative())
                PutOp(OP_DNEG);
        }
        else if (val == 1.0)
            PutOp(OP_DCONST_1);
        else if (val == -1.0)
        {
            PutOp(OP_DCONST_1);
            PutOp(OP_DNEG);
        }
        else if (val == 2.0 || val == 3.0 || val == 4.0 || val == 5.0)
        {
            LoadImmediateInteger(val.IntValue());
            PutOp(OP_I2D);
        }
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
        PutOp((Opcode) (OP_ICONST_0 + val)); // exploit opcode encoding
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
    else if (val == 65535)
    {
        PutOp(OP_ICONST_M1);
        PutOp(OP_I2C);
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
    //
    // JLS2 added ability for parenthesized variable to remain a variable.
    //
    p = StripNops(p);

    AstFieldAccess *field = p -> FieldAccessCast();
    AstExpression *resolve_expression = field ? field -> resolution_opt
        : p -> SimpleNameCast() -> resolution_opt;
    AstMethodInvocation *read_method =
        resolve_expression -> MethodInvocationCast();

    // a read method has exactly one argument: the object in question.
    assert(read_method && read_method -> NumArguments() == 1);

    int stack_words =
        EmitExpression((AstExpression *) read_method -> Argument(0));
    PutOp(OP_DUP);
    PutOp(OP_INVOKESTATIC);
    CompleteCall(read_method -> symbol -> MethodCast(), stack_words);
}


int ByteCode::LoadVariable(VariableCategory kind, AstExpression *expr,
                           bool need_value)
{
    expr = StripNops(expr);
    VariableSymbol *sym = (VariableSymbol *) expr -> symbol;
    TypeSymbol *expression_type = expr -> Type();
    AstFieldAccess *field_access = expr -> FieldAccessCast();
    switch (kind)
    {
    case LHS_LOCAL:
        if (! need_value)
            return 0;
        LoadLocal(sym -> LocalVariableIndex(), expression_type);
        break;
    case LHS_METHOD:
        {
            AstFieldAccess *field_access = expr -> FieldAccessCast();
            AstSimpleName *simple_name = expr -> SimpleNameCast();
            assert(field_access || simple_name);
            expr = (field_access ? field_access -> resolution_opt
                    : simple_name -> resolution_opt);
            assert(expr);
            return EmitExpression(expr, need_value);
        }
    case LHS_FIELD:
    case LHS_STATIC:
        assert(sym -> IsInitialized() || ! sym -> ACC_FINAL());
        if (shadow_parameter_offset && sym -> owner == unit_type &&
            (sym -> accessed_local ||
             sym -> Identity() == control.this0_name_symbol))
        {
            //
            // In a constructor, use the parameter that was passed to the
            // constructor rather than the val$ or this$0 field, because the
            // field is not yet initialized.
            //
            if (! sym -> accessed_local)
            {
                LoadLocal(1, expression_type);
                return 1;
            }
            int offset = shadow_parameter_offset;
            for (int i = 0; i < unit_type -> NumConstructorParameters(); i++)
            {
                VariableSymbol *shadow = unit_type -> ConstructorParameter(i);
                if (sym == shadow)
                {
                    LoadLocal(offset, expression_type);
                    return GetTypeWords(expression_type);
                }
                offset += GetTypeWords(shadow -> Type());
            }
            assert(false && "local variable shadowing is messed up");
        }
        if (field_access && field_access -> base -> Type() -> IsArray())
        {
            assert(sym -> name_symbol == control.length_name_symbol &&
                   need_value);
            EmitExpression(field_access -> base);
            PutOp(OP_ARRAYLENGTH);
            return 1;
        }

        if (sym -> ACC_STATIC())
        {
            //
            // If the access is qualified by an arbitrary base expression,
            // evaluate it for side effects. Likewise, volatile fields must be
            // loaded because of the memory barrier side effect.
            //
            if (field_access && ! field_access -> IsClassAccess())
                EmitExpression(field_access -> base, false);
            if (need_value || sym -> ACC_VOLATILE())
            {
                if (sym -> initial_value)
                {
                    //
                    // Inline any constant. Note that volatile variables can't
                    // be final, so they are not constant.
                    //
                    LoadLiteral(sym -> initial_value, expression_type);
                    return GetTypeWords(expression_type);
                }
                PutOp(OP_GETSTATIC);
            }
            else return 0;
        }
        else
        {
            if (sym -> initial_value)
            {
                //
                // Inline constants without referring to the field. However, we
                // must still check for null.
                //
                if (field_access)
                    EmitCheckForNull(field_access -> base, false);
                if (need_value)
                {
                    LoadLiteral(sym -> initial_value, expression_type);
                    return GetTypeWords(expression_type);
                }
                return 0;
            }
            if (field_access)
                EmitExpression(field_access -> base);
            else PutOp(OP_ALOAD_0);
            PutOp(OP_GETFIELD);
        }
        if (control.IsDoubleWordType(expression_type))
            ChangeStack(1);
        PutU2(RegisterFieldref(VariableTypeResolution(expr, sym), sym));

        if (! need_value)
        {
            PutOp(control.IsDoubleWordType(expression_type) ? OP_POP2 : OP_POP);
            return 0;
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
             PutOp((Opcode) (OP_ISTORE_0 + varno)); // Exploit opcode encodings
         else PutOpWide(OP_ISTORE, varno);
    }
    else if (type == control.long_type)
    {
         if (varno <= 3)
             PutOp((Opcode) (OP_LSTORE_0 + varno)); // Exploit opcode encodings
         else PutOpWide(OP_LSTORE, varno);
    }
    else if (type == control.float_type)
    {
         if (varno <= 3)
             PutOp((Opcode) (OP_FSTORE_0 + varno)); // Exploit opcode encodings
         else PutOpWide(OP_FSTORE, varno);
    }
    else if (type == control.double_type)
    {
         if (varno <= 3)
             PutOp((Opcode) (OP_DSTORE_0 + varno)); // Exploit opcode encodings
         else PutOpWide(OP_DSTORE, varno);
    }
    else // assume reference
    {
         if (varno <= 3)
             PutOp((Opcode) (OP_ASTORE_0 + varno)); // Exploit opcode encodings
         else PutOpWide(OP_ASTORE, varno);
    }
}


void ByteCode::StoreVariable(VariableCategory kind, AstExpression *expr)
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
        attributes.Next() =
            new SourceFile_attribute(RegisterUtf8(control.SourceFile_literal),
                                     RegisterUtf8(type -> file_symbol -> FileNameLiteral()));

    //
    // Generate InnerClasses attribute for every CONSTANT_Class_info in the
    // pool that is nested in another type.
    //
    if (type -> IsLocal() || type -> IsNested() ||
        type -> NumNestedTypes() > 0)
    {
        inner_classes_attribute =
            new InnerClasses_attribute(RegisterUtf8(control.InnerClasses_literal));

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
            inner_classes_attribute ->
                AddInnerClass(RegisterClass(outer),
                              (outer -> IsLocal() ? 0
                               : RegisterClass(outer -> ContainingType())),
                              (outer -> Anonymous() ? 0
                               : RegisterName(outer -> name_symbol)),
                              outer -> Flags());
        }

        for (int k = 0; k < type -> NumNestedTypes(); k++)
        {
            TypeSymbol *nested = type -> NestedType(k);
            inner_classes_attribute ->
                AddInnerClass(RegisterClass(nested),
                              (nested -> IsLocal() ? 0
                               : RegisterClass(nested -> ContainingType())),
                              (nested -> Anonymous() ? 0
                               : RegisterName(nested -> name_symbol)),
                              nested -> Flags());
        }

        attributes.Next() = inner_classes_attribute;
    }

    if (type -> IsDeprecated())
        attributes.Next() = CreateDeprecatedAttribute();
}


void ByteCode::PutOp(Opcode opc)
{
#ifdef JIKES_DEBUG
    if (control.option.debug_trap_op > 0 &&
        code_attribute -> CodeLength() == (u2) control.option.debug_trap_op)
    {
        op_trap();
    }

    if (control.option.debug_trace_stack_change)
    {
        const char *opname;
        OpDesc(opc, &opname, NULL);
        Coutput << "opcode: " << opname << endl;
    }
#endif // JIKES_DEBUG

    // save pc at start of operation
    last_op_pc = code_attribute -> CodeLength();
    code_attribute -> AddCode(opc);
    ChangeStack(stack_effect[opc]);
    last_op_goto = (opc == OP_GOTO || opc == OP_GOTO_W);
}

void ByteCode::PutOpWide(Opcode opc, u2 var)
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

#ifdef JIKES_DEBUG
    if (control.option.debug_trace_stack_change)
        Coutput << "stack change: pc " << last_op_pc << " change " << i
                << "  stack_depth " << stack_depth << "  max_stack: "
                << max_stack << endl;
#endif // JIKES_DEBUG
}


#ifdef JIKES_DEBUG
void ByteCode::PrintCode()
{
    // This explicit casting works around a bug in g++ 3.1 library.
    Coutput << "magic " << (ios&(*)(ios&)) hex << magic << (ios&(*)(ios&)) dec
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
            if (constant_pool[i] -> Tag() == CONSTANT_Long ||
                constant_pool[i] -> Tag() == CONSTANT_Double)
            {
                i++; // skip the next entry for eight-byte constants
            }
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
#endif // JIKES_DEBUG

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

