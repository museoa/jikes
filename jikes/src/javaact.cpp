#line 452 "java.g"
// $Id$
// DO NOT MODIFY THIS FILE - it is generated using jikespg on java.g.
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, 2001, 2002 International Business
// Machines Corporation and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#include "platform.h"
#include "parser.h"
#include "ast.h"

#undef HEADERS
#include "javaact.h"

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif

//**************************************************************************//
//**************************************************************************//
//*                                                                        *//
//* Below, we show each rule of the Java grammar together with the         *//
//* semantic action that is invoked when the parser performs a reduction   *//
//* by that rule.                                                          *//
//**************************************************************************//
//**************************************************************************//


//
// Rule 1:  Goal -> CompilationUnit
//
#line 490 "java.g"
//
// Given a rule of the form A ::= x1 x2 ... xn        n >= 1
//
// Do nothing - Whatever Ast was produced for x1 is inherited by A.
//
void Parser::BadAction() { assert(false); }
void Parser::NoAction() {}

//
// Rule 2:  Goal ::= BodyMarker MethodBody
//
#line 502 "java.g"
//
// This rule was added to allow the parser to recognize the body of a
// funtion (constructor or method, as the definition of the body of a
// method is subsumed by the definition of the body of a constructor)
// out of context. Note that the artificial terminal BodyMarker is
// added here to prevent an ordinary parse from accepting a body as
// a valid input - i.e., to recognize a body out-of-context, the
// BodyMarker terminal must be inserted in front of the input stream
// containing the body in question.
//
void Parser::Act2()
{
    Sym(1) = Sym(2);
}

//
// Rule 3:  Literal ::= IntegerLiteral
//
#line 527 "java.g"
void Parser::Act3()
{
    Sym(1) = ast_pool -> NewIntegerLiteral(Token(1));
}

//
// Rule 4:  Literal ::= LongLiteral
//
#line 536 "java.g"
void Parser::Act4()
{
    Sym(1) = ast_pool -> NewLongLiteral(Token(1));
}

//
// Rule 5:  Literal ::= FloatLiteral
//
#line 545 "java.g"
void Parser::Act5()
{
    Sym(1) = ast_pool -> NewFloatLiteral(Token(1));
}

//
// Rule 6:  Literal ::= DoubleLiteral
//
#line 554 "java.g"
void Parser::Act6()
{
    Sym(1) = ast_pool -> NewDoubleLiteral(Token(1));
}

//
// Rule 7:  Literal -> BooleanLiteral
//
// void NoAction();
//

//
// Rule 8:  Literal ::= CharacterLiteral
//
#line 567 "java.g"
void Parser::Act8()
{
    Sym(1) = ast_pool -> NewCharacterLiteral(Token(1));
}

//
// Rule 9:  Literal ::= StringLiteral
//
#line 576 "java.g"
void Parser::Act9()
{
    Sym(1) = ast_pool -> NewStringLiteral(Token(1));
}

//
// Rule 10:  Literal ::= null
//
#line 585 "java.g"
void Parser::Act10()
{
    Sym(1) = ast_pool -> NewNullLiteral(Token(1));
}

//
// Rule 11:  BooleanLiteral ::= true
//
#line 594 "java.g"
void Parser::Act11()
{
    Sym(1) = ast_pool -> NewTrueLiteral(Token(1));
}

//
// Rule 12:  BooleanLiteral ::= false
//
#line 603 "java.g"
void Parser::Act12()
{
    Sym(1) = ast_pool -> NewFalseLiteral(Token(1));
}

//
// Rule 13:  Type -> PrimitiveType
//
// void NoAction();
//

//
// Rule 14:  Type -> ReferenceType
//
// void NoAction();
//

//
// Rule 15:  PrimitiveType -> NumericType
//
// void NoAction();
//

//
// Rule 16:  PrimitiveType ::= boolean
//
#line 626 "java.g"
void Parser::Act16()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::BOOLEAN, Token(1));
}

//
// Rule 17:  NumericType -> IntegralType
//
// void NoAction();
//

//
// Rule 18:  NumericType -> FloatingPointType
//
// void NoAction();
//

//
// Rule 19:  IntegralType ::= byte
//
#line 643 "java.g"
void Parser::Act19()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::BYTE, Token(1));
}

//
// Rule 20:  IntegralType ::= short
//
#line 652 "java.g"
void Parser::Act20()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::SHORT, Token(1));
}

//
// Rule 21:  IntegralType ::= int
//
#line 661 "java.g"
void Parser::Act21()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::INT, Token(1));
}

//
// Rule 22:  IntegralType ::= long
//
#line 670 "java.g"
void Parser::Act22()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::LONG, Token(1));
}

//
// Rule 23:  IntegralType ::= char
//
#line 679 "java.g"
void Parser::Act23()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::CHAR, Token(1));
}

//
// Rule 24:  FloatingPointType ::= float
//
#line 688 "java.g"
void Parser::Act24()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::FLOAT, Token(1));
}

//
// Rule 25:  FloatingPointType ::= double
//
#line 697 "java.g"
void Parser::Act25()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::DOUBLE, Token(1));
}

//
// Rule 26:  ReferenceType -> ClassOrInterfaceType
//
// void NoAction();
//

//
// Rule 27:  ReferenceType -> ArrayType
//
// void NoAction();
//

//
// Rule 28:  ClassOrInterfaceType -> Name
//
// void NoAction();
//

//
// Rule 29:  ArrayType ::= PrimitiveType Dims
//
#line 726 "java.g"
void Parser::MakeArrayType()
{
    AstArrayType *p = ast_pool -> NewArrayType();
    p -> type = Sym(1);
    //
    // The list of modifiers is guaranteed not empty
    //
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(2));
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets(DYNAMIC_CAST<AstBrackets *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 30:  ArrayType ::= Name Dims
//
// void MakeArrayType();
//

//
// Rule 31:  ClassType -> ClassOrInterfaceType
//
// void NoAction();
//

//
// Rule 32:  InterfaceType -> ClassOrInterfaceType
//
// void NoAction();
//

//
// Rule 33:  Name -> SimpleName
//
// void NoAction();
//

//
// Rule 34:  Name -> QualifiedName
//
// void NoAction();
//

//
// Rule 35:  SimpleName ::= Identifier
//
#line 776 "java.g"
void Parser::MakeSimpleName()
{
    Sym(1) = ast_pool -> NewSimpleName(Token(1));
}

//
// Rule 36:  QualifiedName ::= Name DOT Identifier
//
#line 785 "java.g"
void Parser::MakeFieldAccess()
{
    AstFieldAccess *p = ast_pool -> NewFieldAccess();
    p -> base = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> dot_token = Token(2);
    p -> identifier_token = Token(3);
    Sym(1) = p;
}

//
// Rule 37:  CompilationUnit ::= PackageDeclarationopt ImportDeclarationsopt TypeDeclarationsopt
//
#line 800 "java.g"
void Parser::Act37()
{
    AstCompilationUnit *p = ast_pool -> NewCompilationUnit();
    p -> package_declaration_opt = DYNAMIC_CAST<AstPackageDeclaration *> (Sym(1));
    if (Sym(2) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(2));
        p -> AllocateImportDeclarations(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddImportDeclaration(DYNAMIC_CAST<AstImportDeclaration *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    if (Sym(3) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateTypeDeclarations(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddTypeDeclaration(root -> element);
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 38:  ImportDeclarations ::= ImportDeclaration
//
#line 835 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act38()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 39:  ImportDeclarations ::= ImportDeclarations ImportDeclaration
//
#line 852 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act39()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 40:  TypeDeclarations ::= TypeDeclaration
//
#line 873 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act40()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 41:  TypeDeclarations ::= TypeDeclarations TypeDeclaration
//
#line 890 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act41()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 42:  PackageDeclaration ::= package Name PackageHeaderMarker SEMICOLON
//
#line 911 "java.g"
void Parser::Act42()
{
    AstPackageDeclaration *p = ast_pool -> NewPackageDeclaration();
    p -> package_token = Token(1);
    p -> name = DYNAMIC_CAST<AstExpression *> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 43:  ImportDeclaration -> SingleTypeImportDeclaration
//
// void NoAction();
//

//
// Rule 44:  ImportDeclaration -> TypeImportOnDemandDeclaration
//
// void NoAction();
//

//
// Rule 45:  SingleTypeImportDeclaration ::= import Name SEMICOLON
//
#line 932 "java.g"
void Parser::Act45()
{
    AstImportDeclaration *p = ast_pool -> NewImportDeclaration();
    p -> import_token = Token(1);
    p -> name = DYNAMIC_CAST<AstExpression *> (Sym(2));
    p -> star_token_opt = 0;
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 46:  TypeImportOnDemandDeclaration ::= import Name DOT MULTIPLY SEMICOLON
//
#line 946 "java.g"
void Parser::Act46()
{
    AstImportDeclaration *p = ast_pool -> NewImportDeclaration();
    p -> import_token = Token(1);
    p -> name = DYNAMIC_CAST<AstExpression *> (Sym(2));
    p -> star_token_opt = Token(4);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 47:  TypeDeclaration -> ClassDeclaration
//
// void NoAction();
//

//
// Rule 48:  TypeDeclaration -> InterfaceDeclaration
//
// void NoAction();
//

//
// Rule 49:  TypeDeclaration ::= SEMICOLON
//
#line 968 "java.g"
void Parser::Act49()
{
    Sym(1) = ast_pool -> NewEmptyDeclaration(Token(1));
}

//
// Rule 50:  Modifiers ::= Modifier
//
#line 979 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act50()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 51:  Modifiers ::= Modifiers Modifier
//
#line 996 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act51()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 52:  Modifier ::= public
//
#line 1017 "java.g"
void Parser::Act52()
{
    Sym(1) = ast_pool -> NewModifier(Ast::PUBLIC, Token(1));
}

//
// Rule 53:  Modifier ::= protected
//
#line 1026 "java.g"
void Parser::Act53()
{
    Sym(1) = ast_pool -> NewModifier(Ast::PROTECTED, Token(1));
}

//
// Rule 54:  Modifier ::= private
//
#line 1035 "java.g"
void Parser::Act54()
{
    Sym(1) = ast_pool -> NewModifier(Ast::PRIVATE, Token(1));
}

//
// Rule 55:  Modifier ::= static
//
#line 1044 "java.g"
void Parser::Act55()
{
    Sym(1) = ast_pool -> NewModifier(Ast::STATIC, Token(1));
}

//
// Rule 56:  Modifier ::= abstract
//
#line 1053 "java.g"
void Parser::Act56()
{
    Sym(1) = ast_pool -> NewModifier(Ast::ABSTRACT, Token(1));
}

//
// Rule 57:  Modifier ::= final
//
#line 1062 "java.g"
void Parser::Act57()
{
    Sym(1) = ast_pool -> NewModifier(Ast::FINAL, Token(1));
}

//
// Rule 58:  Modifier ::= native
//
#line 1071 "java.g"
void Parser::Act58()
{
    Sym(1) = ast_pool -> NewModifier(Ast::NATIVE, Token(1));
}

//
// Rule 59:  Modifier ::= strictfp
//
#line 1080 "java.g"
void Parser::Act59()
{
    Sym(1) = ast_pool -> NewModifier(Ast::STRICTFP, Token(1));
}

//
// Rule 60:  Modifier ::= synchronized
//
#line 1089 "java.g"
void Parser::Act60()
{
    Sym(1) = ast_pool -> NewModifier(Ast::SYNCHRONIZED, Token(1));
}

//
// Rule 61:  Modifier ::= transient
//
#line 1098 "java.g"
void Parser::Act61()
{
    Sym(1) = ast_pool -> NewModifier(Ast::TRANSIENT, Token(1));
}

//
// Rule 62:  Modifier ::= volatile
//
#line 1107 "java.g"
void Parser::Act62()
{
    Sym(1) = ast_pool -> NewModifier(Ast::VOLATILE, Token(1));
}

//
// Rule 63:  ClassDeclaration ::= Modifiersopt class Identifier Superopt Interfacesopt ClassBody
//
#line 1123 "java.g"
void Parser::Act63()
{
    AstClassDeclaration *p = ast_pool -> NewClassDeclaration();
    if (Sym(1) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));
        p -> AllocateClassModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddClassModifier(DYNAMIC_CAST<AstModifier *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> class_token = Token(2);
    p -> identifier_token = Token(3);
    p -> super_opt = DYNAMIC_CAST<AstExpression *> (Sym(4));
    if (Sym(5) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(5));
        p -> AllocateInterfaces(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddInterface(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> class_body = DYNAMIC_CAST<AstClassBody *> (Sym(6));
    Sym(1) = p;
}

//
// Rule 64:  Super ::= extends ClassType
//
#line 1161 "java.g"
void Parser::SetSym1ToSym2() { Sym(1) = Sym(2); }

//
// Rule 65:  Interfaces ::= implements InterfaceTypeList
//
// void SetSym1ToSym2();
//

//
// Rule 66:  InterfaceTypeList ::= InterfaceType
//
#line 1174 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act66()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 67:  InterfaceTypeList ::= InterfaceTypeList COMMA InterfaceType
//
#line 1191 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act67()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 68:  ClassBody ::= LBRACE ClassBodyDeclarationsopt RBRACE
//
#line 1212 "java.g"
void Parser::Act68()
{
    AstClassBody *p = ast_pool -> NewClassBody();
    if (parse_header_only)
        p -> mark_unparsed();

    p -> left_brace_token = Token(1);
    if (Sym(2) != NULL)
    {
        int num_instance_variables = 0,
            num_class_variables = 0,
            num_methods = 0,
            num_constructors = 0,
            num_static_initializers = 0,
            num_inner_classes = 0,
            num_inner_interfaces = 0,
            num_blocks = 0,
            num_empty_declarations = 0;

        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(2));
        p -> AllocateClassBodyDeclarations(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddClassBodyDeclaration(root -> element);

            AstFieldDeclaration *field_declaration = root -> element -> FieldDeclarationCast();
            if (field_declaration)
            {
                for (int i = 0; i < field_declaration -> NumVariableModifiers(); i++)
                {
                    if (field_declaration -> VariableModifier(i) -> kind == Ast::STATIC)
                    {
                        field_declaration -> MarkStatic();
                        break;
                    }
                }
                if (field_declaration -> StaticFieldCast())
                     num_class_variables++;
                else num_instance_variables++;
            }
            else if (root -> element -> MethodDeclarationCast())
            {
                num_methods++;
            }
            else if (root -> element -> ConstructorDeclarationCast())
            {
                num_constructors++;
            }
            else if (root -> element -> StaticInitializerCast())
            {
                num_static_initializers++;
            }
            else if (root -> element -> ClassDeclarationCast())
            {
                num_inner_classes++;
            }
            else if (root -> element -> InterfaceDeclarationCast())
            {
                num_inner_interfaces++;
            }
            else if (root -> element -> BlockCast())
            {
                num_blocks++;
            }
            else num_empty_declarations++;
        } while (root != tail);

        p -> AllocateInstanceVariables(num_instance_variables);
        p -> AllocateClassVariables(num_class_variables);
        p -> AllocateMethods(num_methods);
        p -> AllocateConstructors(num_constructors);
        p -> AllocateStaticInitializers(num_static_initializers);
        p -> AllocateNestedClasses(num_inner_classes);
        p -> AllocateNestedInterfaces(num_inner_interfaces);
        p -> AllocateInstanceInitializers(num_blocks);
        p -> AllocateEmptyDeclarations(num_empty_declarations);

        root = tail;
        do
        {
            root = root -> next;

            AstFieldDeclaration *field_declaration;
            AstMethodDeclaration *method_declaration;
            AstConstructorDeclaration *constructor_declaration;
            AstStaticInitializer *static_initializer;
            AstClassDeclaration *class_declaration;
            AstInterfaceDeclaration *interface_declaration;
            AstMethodBody *block;

            if ((field_declaration = root -> element -> FieldDeclarationCast()))
            {
                if (field_declaration -> StaticFieldCast())
                     p -> AddClassVariable(field_declaration);
                else p -> AddInstanceVariable(field_declaration);
            }
            else if ((method_declaration = root -> element -> MethodDeclarationCast()))
            {
                p -> AddMethod(method_declaration);
            }
            else if ((constructor_declaration = root -> element -> ConstructorDeclarationCast()))
            {
                p -> AddConstructor(constructor_declaration);
            }
            else if ((static_initializer = root -> element -> StaticInitializerCast()))
            {
                p -> AddStaticInitializer(static_initializer);
            }
            else if ((class_declaration = root -> element -> ClassDeclarationCast()))
            {
                p -> AddNestedClass(class_declaration);
            }
            else if ((interface_declaration = root -> element -> InterfaceDeclarationCast()))
            {
                p -> AddNestedInterface(interface_declaration);
            }
            else if ((block = root -> element -> MethodBodyCast()))
            {
                p -> AddInstanceInitializer(block);
            }
            else // assert(block = root -> element -> EmptyDeclarationCast())
            {
                p -> AddEmptyDeclaration(DYNAMIC_CAST<AstEmptyDeclaration *> (root -> element));
            }
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(3);
    p -> pool = body_pool; // from now on, this is the storage pool to use for this type
    Sym(1) = p;
}

//
// Rule 69:  ClassBodyDeclarations ::= ClassBodyDeclaration
//
#line 1350 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act69()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 70:  ClassBodyDeclarations ::= ClassBodyDeclarations ClassBodyDeclaration
//
#line 1367 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act70()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 71:  ClassBodyDeclaration -> ClassMemberDeclaration
//
// void NoAction();
//

//
// Rule 72:  ClassBodyDeclaration -> StaticInitializer
//
// void NoAction();
//

//
// Rule 73:  ClassBodyDeclaration -> ConstructorDeclaration
//
// void NoAction();
//

//
// Rule 74:  ClassBodyDeclaration ::= MethodHeaderMarker MethodBody
//
#line 1401 "java.g"
void Parser::Act74()
{
    Sym(1) = Sym(2);
}

//
// Rule 75:  ClassMemberDeclaration -> FieldDeclaration
//
// void NoAction();
//

//
// Rule 76:  ClassMemberDeclaration -> MethodDeclaration
//
// void NoAction();
//

//
// Rule 77:  ClassMemberDeclaration -> ClassDeclaration
//
// void NoAction();
//

//
// Rule 78:  ClassMemberDeclaration -> InterfaceDeclaration
//
// void NoAction();
//

//
// Rule 79:  ClassMemberDeclaration ::= SEMICOLON
//
#line 1428 "java.g"
void Parser::Act79()
{
    Sym(1) = ast_pool -> NewEmptyDeclaration(Token(1));
}

//
// Rule 80:  FieldDeclaration ::= Modifiersopt Type VariableDeclarators SEMICOLON
//
#line 1447 "java.g"
void Parser::Act80()
{
    AstFieldDeclaration *p = ast_pool -> NewFieldDeclaration();
    if (Sym(1) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));
        p -> AllocateVariableModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableModifier(DYNAMIC_CAST<AstModifier *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> type = Sym(2);
    //
    // The list of declarators is guaranteed not empty
    //
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateVariableDeclarators(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableDeclarator(DYNAMIC_CAST<AstVariableDeclarator *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> semicolon_token = Token(4);
    Sym(1) = p;
}

//
// Rule 81:  VariableDeclarators ::= VariableDeclarator
//
#line 1485 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act81()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 82:  VariableDeclarators ::= VariableDeclarators COMMA VariableDeclarator
//
#line 1502 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act82()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 83:  VariableDeclarator ::= VariableDeclaratorId
//
#line 1523 "java.g"
void Parser::Act83()
{
    AstVariableDeclarator *p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name = DYNAMIC_CAST<AstVariableDeclaratorId *> (Sym(1));
    p -> variable_initializer_opt = NULL;
    Sym(1) = p;
}

//
// Rule 84:  VariableDeclarator ::= VariableDeclaratorId EQUAL VariableInitializer
//
#line 1535 "java.g"
void Parser::Act84()
{
    AstVariableDeclarator *p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name = DYNAMIC_CAST<AstVariableDeclaratorId *> (Sym(1));
    p -> variable_initializer_opt = Sym(3);
    Sym(1) = p;
}

//
// Rule 85:  VariableDeclaratorId ::= Identifier Dimsopt
//
#line 1547 "java.g"
void Parser::Act85()
{
    AstVariableDeclaratorId *p = ast_pool -> NewVariableDeclaratorId();
    p -> identifier_token = Token(1);
    if (Sym(2) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(2));
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets(DYNAMIC_CAST<AstBrackets *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 86:  VariableInitializer -> Expression
//
// void NoAction();
//

//
// Rule 87:  VariableInitializer -> ArrayInitializer
//
// void NoAction();
//

//
// Rule 88:  MethodDeclaration ::= MethodHeader MethodHeaderMarker MethodBody
void Parser::MakeMethodDeclaration()
{
    DYNAMIC_CAST<AstMethodDeclaration *> (Sym(1)) -> method_body =
        DYNAMIC_CAST<AstStatement *> (Sym(3));
}

//
// Rule 89:  MethodDeclaration ::= MethodHeader MethodHeaderMarker EmptyStatement
//
// void MakeMethodDeclaration();
//

//
// Rule 90:  MethodHeader ::= Modifiersopt Type MethodDeclarator Throwsopt
//
#line 1614 "java.g"
void Parser::Act90()
{
    AstMethodDeclaration *p = ast_pool -> NewMethodDeclaration();
    if (Sym(1) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));
        p -> AllocateMethodModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddMethodModifier(DYNAMIC_CAST<AstModifier *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> type = Sym(2);
    p -> method_declarator = DYNAMIC_CAST<AstMethodDeclarator *> (Sym(3));
    if (Sym(4) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(4));
        p -> AllocateThrows(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddThrow(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 91:  MethodHeader ::= Modifiersopt void MethodDeclarator Throwsopt
//
#line 1650 "java.g"
void Parser::Act91()
{
    AstMethodDeclaration *p = ast_pool -> NewMethodDeclaration();
    if (Sym(1) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));
        p -> AllocateMethodModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddMethodModifier(DYNAMIC_CAST<AstModifier *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> type = ast_pool -> NewPrimitiveType(Ast::VOID_TYPE, Token(2));
    p -> method_declarator = DYNAMIC_CAST<AstMethodDeclarator *> (Sym(3));
    if (Sym(4) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(4));
        p -> AllocateThrows(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddThrow(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 92:  MethodDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN Dimsopt
//
#line 1686 "java.g"
void Parser::Act92()
{
    AstMethodDeclarator *p = ast_pool -> NewMethodDeclarator();
    p -> identifier_token = Token(1);
    p -> left_parenthesis_token = Token(2);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateFormalParameters(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddFormalParameter(DYNAMIC_CAST<AstFormalParameter *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    if (Sym(5) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(5));
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets(DYNAMIC_CAST<AstBrackets *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 93:  FormalParameterList ::= FormalParameter
//
#line 1723 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act93()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 94:  FormalParameterList ::= FormalParameterList COMMA FormalParameter
//
#line 1740 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act94()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 95:  FormalParameter ::= Type VariableDeclaratorId
//
#line 1761 "java.g"
void Parser::Act95()
{
    AstFormalParameter *p = ast_pool -> NewFormalParameter();
    p -> type = Sym(1);

    AstVariableDeclarator *formal_declarator = ast_pool -> NewVariableDeclarator();
    formal_declarator -> variable_declarator_name = DYNAMIC_CAST<AstVariableDeclaratorId *> (Sym(2));
    formal_declarator -> variable_initializer_opt = NULL;

    p -> formal_declarator = formal_declarator;

    Sym(1) = p;
}

//
// Rule 96:  FormalParameter ::= Modifiers Type VariableDeclaratorId
//
#line 1780 "java.g"
void Parser::Act96()
{
    AstFormalParameter *p = ast_pool -> NewFormalParameter();
    //
    // The list of modifiers is guaranteed not empty
    //
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));
        p -> AllocateParameterModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddParameterModifier(DYNAMIC_CAST<AstModifier *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }

    p -> type = Sym(2);

    AstVariableDeclarator *formal_declarator = ast_pool -> NewVariableDeclarator();
    formal_declarator -> variable_declarator_name = DYNAMIC_CAST<AstVariableDeclaratorId *> (Sym(3));
    formal_declarator -> variable_initializer_opt = NULL;

    p -> formal_declarator = formal_declarator;

    Sym(1) = p;
}

//
// Rule 97:  Throws ::= throws ClassTypeList
//
// void SetSym1ToSym2();
//

//
// Rule 98:  ClassTypeList ::= ClassType
//
#line 1820 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act98()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 99:  ClassTypeList ::= ClassTypeList COMMA ClassType
//
#line 1837 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act99()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 100:  MethodBody ::= LBRACE BlockStatementsopt RBRACE
//
#line 1864 "java.g"
void Parser::Act100()
{
    AstMethodBody *p = ast_pool -> NewMethodBody();
    p -> left_brace_token = Token(1);
    p -> right_brace_token = Token(3);

    if (Sym(2) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(2));
        p -> AllocateStatements(tail -> index + 1);
        AstListNode *root = tail -> next;
        if (root -> element -> IsExplicitConstructorInvocation())
            p -> explicit_constructor_opt = DYNAMIC_CAST<AstStatement *> (root -> element);
        else
            p -> AddStatement(DYNAMIC_CAST<AstStatement *> (root -> element));
        while (root != tail)
        {
            root = root -> next;
            p -> AddStatement(DYNAMIC_CAST<AstStatement *> (root -> element));
        }
        FreeCircularList(tail);
    }

    Sym(1) = p;
}

//
// Rule 101:  StaticInitializer ::= static MethodHeaderMarker MethodBody
//
#line 1902 "java.g"
void Parser::Act101()
{
    AstStaticInitializer *p = ast_pool -> NewStaticInitializer();
    p -> static_token = Token(1);
    p -> block = DYNAMIC_CAST<AstMethodBody *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 102:  ConstructorDeclaration ::= Modifiersopt ConstructorDeclarator Throwsopt MethodHeaderMarker MethodBody
//
#line 1929 "java.g"
void Parser::Act102()
{
    AstConstructorDeclaration *p = ast_pool -> NewConstructorDeclaration();

    if (Sym(1) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));
        p -> AllocateConstructorModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddConstructorModifier(DYNAMIC_CAST<AstModifier *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> constructor_declarator = DYNAMIC_CAST<AstMethodDeclarator *> (Sym(2));
    if (Sym(3) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateThrows(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddThrow(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> constructor_body = DYNAMIC_CAST<AstMethodBody *> (Sym(5));

    Sym(1) = p;
}

//
// Rule 103:  ConstructorDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN
//
#line 1975 "java.g"
void Parser::Act103()
{
    AstMethodDeclarator *p = ast_pool -> NewMethodDeclarator();
    p -> identifier_token = Token(1);
    p -> left_parenthesis_token = Token(2);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateFormalParameters(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddFormalParameter(DYNAMIC_CAST<AstFormalParameter *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    Sym(1) = p;
}

//
// Rule 104:  ExplicitConstructorInvocation ::= this LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2010 "java.g"
void Parser::Act104()
{
    AstThisCall *p = ast_pool -> NewThisCall();
    p -> this_token = Token(1);
    p -> left_parenthesis_token = Token(2);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 105:  ExplicitConstructorInvocation ::= super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2036 "java.g"
void Parser::Act105()
{
    AstSuperCall *p = ast_pool -> NewSuperCall();
    p -> base_opt = NULL;
    p -> dot_token_opt = 0;
    p -> super_token = Token(1);
    p -> left_parenthesis_token = Token(2);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 106:  ExplicitConstructorInvocation ::= Primary DOT super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2065 "java.g"
void Parser::MakeQualifiedSuper()
{
    AstSuperCall *p = ast_pool -> NewSuperCall();
    p -> base_opt = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> dot_token_opt = Token(2);
    p -> super_token = Token(3);
    p -> left_parenthesis_token = Token(4);
    if (Sym(5) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(5));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(6);
    p -> semicolon_token = Token(7);
    Sym(1) = p;
}

//
// Rule 107:  ExplicitConstructorInvocation ::= Name DOT super LPAREN ArgumentListopt RPAREN SEMICOLON
//
// void MakeQualifiedSuper();
//

//
// Rule 108:  InterfaceDeclaration ::= Modifiersopt interface Identifier ExtendsInterfacesopt InterfaceBody
//
#line 2108 "java.g"
void Parser::Act108()
{
    AstInterfaceDeclaration *p = DYNAMIC_CAST<AstInterfaceDeclaration *> (Sym(5));
    if (Sym(1) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));
        p -> AllocateInterfaceModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddInterfaceModifier(DYNAMIC_CAST<AstModifier *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> interface_token = Token(2);
    p -> identifier_token = Token(3);
    if (Sym(4) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(4));
        p -> AllocateExtendsInterfaces(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddExtendsInterface(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 109:  ExtendsInterfaces ::= extends InterfaceTypeList
//
// void SetSym1ToSym2();
//

//
// Rule 110:  InterfaceBody ::= LBRACE InterfaceMemberDeclarationsopt RBRACE
//
#line 2151 "java.g"
void Parser::Act110()
{
    AstInterfaceDeclaration *p = ast_pool -> NewInterfaceDeclaration();
    if (parse_header_only)
        p -> mark_unparsed();

    p -> left_brace_token = Token(1);
    if (Sym(2) != NULL)
    {
        int num_class_variables = 0,
            num_methods = 0,
            num_inner_classes = 0,
            num_inner_interfaces = 0,
            num_empty_declarations = 0;

        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(2));
        p -> AllocateInterfaceMemberDeclarations(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddInterfaceMemberDeclaration(root -> element);

            AstFieldDeclaration *field_declaration = root -> element -> FieldDeclarationCast();
            if (field_declaration)
            {
                field_declaration -> MarkStatic();
                num_class_variables++;
            }
            else if (root -> element -> MethodDeclarationCast())
            {
                num_methods++;
            }
            else if (root -> element -> ClassDeclarationCast())
            {
                num_inner_classes++;
            }
            else if (root -> element -> InterfaceDeclarationCast())
            {
                num_inner_interfaces++;
            }
            else num_empty_declarations++;
        } while (root != tail);

        p -> AllocateClassVariables(num_class_variables);
        p -> AllocateMethods(num_methods);
        p -> AllocateNestedClasses(num_inner_classes);
        p -> AllocateNestedInterfaces(num_inner_interfaces);
        p -> AllocateEmptyDeclarations(num_empty_declarations);

        root = tail;
        do
        {
            root = root -> next;

            AstFieldDeclaration *field_declaration;
            AstMethodDeclaration *method_declaration;
            AstClassDeclaration *class_declaration;
            AstInterfaceDeclaration *interface_declaration;

            if ((field_declaration = root -> element -> FieldDeclarationCast()))
            {
                p -> AddClassVariable(field_declaration);
            }
            else if ((method_declaration = root -> element -> MethodDeclarationCast()))
            {
                p -> AddMethod(method_declaration);
            }
            else if ((class_declaration = root -> element -> ClassDeclarationCast()))
            {
                p -> AddNestedClass(class_declaration);
            }
            else if ((interface_declaration = root -> element -> InterfaceDeclarationCast()))
            {
                p -> AddNestedInterface(interface_declaration);
            }
            else // assert(interface_declaration = root -> element -> EmptyDeclarationCast())
            {
                p -> AddEmptyDeclaration(DYNAMIC_CAST<AstEmptyDeclaration *> (root -> element));
            }
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(3);
    p -> pool = body_pool; // from now on, this is the storage pool to use for this type
    Sym(1) = p;
}

//
// Rule 111:  InterfaceMemberDeclarations ::= InterfaceMemberDeclaration
//
#line 2243 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act111()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 112:  InterfaceMemberDeclarations ::= InterfaceMemberDeclarations InterfaceMemberDeclaration
//
#line 2260 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act112()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 113:  InterfaceMemberDeclaration -> ConstantDeclaration
//
// void NoAction();
//

//
// Rule 114:  InterfaceMemberDeclaration -> AbstractMethodDeclaration
//
// void NoAction();
//

//
// Rule 115:  InterfaceMemberDeclaration -> ClassDeclaration
//
// void NoAction();
//

//
// Rule 116:  InterfaceMemberDeclaration -> InterfaceDeclaration
//
// void NoAction();
//

//
// Rule 117:  InterfaceMemberDeclaration ::= SEMICOLON
//
#line 2299 "java.g"
void Parser::Act117()
{
    Sym(1) = ast_pool -> NewEmptyDeclaration(Token(1));
}

//
// Rule 118:  ConstantDeclaration -> FieldDeclaration
//
// void NoAction();
//

//
// Rule 119:  AbstractMethodDeclaration ::= MethodHeader SEMICOLON
//
#line 2312 "java.g"
void Parser::Act119()
{
    DYNAMIC_CAST<AstMethodDeclaration *> (Sym(1)) -> method_body =
        ast_pool -> NewEmptyStatement(Token(2));
}

//
// Rule 120:  ArrayInitializer ::= LBRACE ,opt RBRACE
//
#line 2330 "java.g"
void Parser::Act120()
{
    AstArrayInitializer *p = ast_pool -> NewArrayInitializer();
    p -> left_brace_token = Token(1);
    p -> right_brace_token = Token(3);
    Sym(1) = p;
}

//
// Rule 121:  ArrayInitializer ::= LBRACE VariableInitializers RBRACE
//
#line 2342 "java.g"
void Parser::Act121()
{
    AstArrayInitializer *p = ast_pool -> NewArrayInitializer();
    p -> left_brace_token = Token(1);
    if (Sym(2) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(2));
        p -> AllocateVariableInitializers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableInitializer(root -> element);
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(3);
    Sym(1) = p;
}

//
// Rule 122:  ArrayInitializer ::= LBRACE VariableInitializers COMMA RBRACE
//
#line 2366 "java.g"
void Parser::Act122()
{
    AstArrayInitializer *p = ast_pool -> NewArrayInitializer();
    p -> left_brace_token = Token(1);
    if (Sym(2) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(2));
        p -> AllocateVariableInitializers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableInitializer(root -> element);
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(4);
    Sym(1) = p;
}

//
// Rule 123:  VariableInitializers ::= VariableInitializer
//
#line 2390 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act123()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 124:  VariableInitializers ::= VariableInitializers COMMA VariableInitializer
//
#line 2407 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act124()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 125:  Block ::= LBRACE BlockStatementsopt RBRACE
//
#line 2430 "java.g"
void Parser::Act125()
{
    AstBlock *p = ast_pool -> NewBlock();
    p -> left_brace_token = Token(1);
    if (Sym(2) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(2));
        p -> AllocateStatements(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddStatement(DYNAMIC_CAST<AstStatement *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(3);
    Sym(1) = p;
}

//
// Rule 126:  BlockStatements ::= BlockStatement
//
#line 2454 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act126()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 127:  BlockStatements ::= BlockStatements BlockStatement
//
#line 2471 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act127()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 128:  BlockStatement -> LocalVariableDeclarationStatement
//
// void NoAction();
//

//
// Rule 129:  BlockStatement -> Statement
//
// void NoAction();
//

//
// Rule 130:  BlockStatement -> ClassDeclaration
//
// void NoAction();
//

//
// Rule 131:  BlockStatement -> ExplicitConstructorInvocation
//
// void NoAction();
//

//
// Rule 132:  LocalVariableDeclarationStatement ::= LocalVariableDeclaration SEMICOLON
//
#line 2514 "java.g"
void Parser::Act132()
{
    DYNAMIC_CAST<AstLocalVariableDeclarationStatement *> (Sym(1)) -> semicolon_token_opt = Token(2);
}

//
// Rule 133:  LocalVariableDeclaration ::= Type VariableDeclarators
//
#line 2523 "java.g"
void Parser::Act133()
{
    AstLocalVariableDeclarationStatement *p = ast_pool -> NewLocalVariableDeclarationStatement();
    p -> type = Sym(1);
    //
    // The list of declarators is guaranteed not empty
    //
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(2));
        p -> AllocateVariableDeclarators(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableDeclarator(DYNAMIC_CAST<AstVariableDeclarator *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> semicolon_token_opt = 0;
    Sym(1) = p;
}

//
// Rule 134:  LocalVariableDeclaration ::= Modifiers Type VariableDeclarators
//
#line 2550 "java.g"
void Parser::Act134()
{
    AstLocalVariableDeclarationStatement *p = ast_pool -> NewLocalVariableDeclarationStatement();
    //
    // The list of modifiers is guaranteed not empty
    //
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));
        p -> AllocateLocalModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddLocalModifier(DYNAMIC_CAST<AstModifier *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> type = Sym(2);
    //
    // The list of declarators is guaranteed not empty
    //
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateVariableDeclarators(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableDeclarator(DYNAMIC_CAST<AstVariableDeclarator *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> semicolon_token_opt = 0;
    Sym(1) = p;
}

//
// Rule 135:  Statement -> StatementWithoutTrailingSubstatement
//
// void NoAction();
//

//
// Rule 136:  Statement -> LabeledStatement
//
// void NoAction();
//

//
// Rule 137:  Statement -> IfThenStatement
//
// void NoAction();
//

//
// Rule 138:  Statement -> IfThenElseStatement
//
// void NoAction();
//

//
// Rule 139:  Statement -> WhileStatement
//
// void NoAction();
//

//
// Rule 140:  Statement -> ForStatement
//
// void NoAction();
//

//
// Rule 141:  StatementNoShortIf -> StatementWithoutTrailingSubstatement
//
// void NoAction();
//

//
// Rule 142:  StatementNoShortIf -> LabeledStatementNoShortIf
//
// void NoAction();
//

//
// Rule 143:  StatementNoShortIf -> IfThenElseStatementNoShortIf
//
// void NoAction();
//

//
// Rule 144:  StatementNoShortIf -> WhileStatementNoShortIf
//
// void NoAction();
//

//
// Rule 145:  StatementNoShortIf -> ForStatementNoShortIf
//
// void NoAction();
//

//
// Rule 146:  StatementWithoutTrailingSubstatement -> Block
//
// void NoAction();
//

//
// Rule 147:  StatementWithoutTrailingSubstatement -> EmptyStatement
//
// void NoAction();
//

//
// Rule 148:  StatementWithoutTrailingSubstatement -> ExpressionStatement
//
// void NoAction();
//

//
// Rule 149:  StatementWithoutTrailingSubstatement -> SwitchStatement
//
// void NoAction();
//

//
// Rule 150:  StatementWithoutTrailingSubstatement -> DoStatement
//
// void NoAction();
//

//
// Rule 151:  StatementWithoutTrailingSubstatement -> BreakStatement
//
// void NoAction();
//

//
// Rule 152:  StatementWithoutTrailingSubstatement -> ContinueStatement
//
// void NoAction();
//

//
// Rule 153:  StatementWithoutTrailingSubstatement -> ReturnStatement
//
// void NoAction();
//

//
// Rule 154:  StatementWithoutTrailingSubstatement -> SynchronizedStatement
//
// void NoAction();
//

//
// Rule 155:  StatementWithoutTrailingSubstatement -> ThrowStatement
//
// void NoAction();
//

//
// Rule 156:  StatementWithoutTrailingSubstatement -> TryStatement
//
// void NoAction();
//

//
// Rule 157:  StatementWithoutTrailingSubstatement -> AssertStatement
//
// void NoAction();
//

//
// Rule 158:  EmptyStatement ::= SEMICOLON
//
#line 2682 "java.g"
void Parser::Act158()
{
    Sym(1) = ast_pool -> NewEmptyStatement(Token(1));
}

//
// Rule 159:  LabeledStatement ::= Identifier COLON Statement
//
#line 2691 "java.g"
void Parser::MakeLabeledStatement()
{
    AstBlock *p = Sym(3) -> BlockCast();

    if (! p || p -> label_opt)
    {
        //
        // When a statement is labeled, it is enclosed in a block.
        // This is necessary in order to allow the same name to be
        // reused to label a subsequent statement at the same nesting
        // level... See ProcessBlock, ProcessStatement,...
        //
        p = ast_pool -> NewBlock();
        p -> AllocateStatements(1); // allocate 1 element
        p -> left_brace_token = Token(1);
        p -> AddStatement(DYNAMIC_CAST<AstStatement *> (Sym(3)));
        p -> right_brace_token = Sym(3) -> RightToken();
        p -> no_braces = true;
    }

    p -> label_opt = Token(1); // add label to statement
    Sym(1) = p; // The final result is a block containing the labeled-statement
}

//
// Rule 160:  LabeledStatementNoShortIf ::= Identifier COLON StatementNoShortIf
//
// void MakeLabeledStatement();
//

//
// Rule 161:  ExpressionStatement ::= StatementExpression SEMICOLON
//
#line 2726 "java.g"
void Parser::Act161()
{
    DYNAMIC_CAST<AstExpressionStatement *> (Sym(1)) -> semicolon_token_opt = Token(2);
}

//
// Rule 162:  StatementExpression ::= Assignment
//
#line 2735 "java.g"
void Parser::MakeExpressionStatement()
{
    AstExpressionStatement *p = ast_pool -> NewExpressionStatement();
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> semicolon_token_opt = 0;
    Sym(1) = p;
}

//
// Rule 163:  StatementExpression ::= PreIncrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 164:  StatementExpression ::= PreDecrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 165:  StatementExpression ::= PostIncrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 166:  StatementExpression ::= PostDecrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 167:  StatementExpression ::= MethodInvocation
//
// void MakeExpressionStatement();
//

//
// Rule 168:  StatementExpression ::= ClassInstanceCreationExpression
//
// void MakeExpressionStatement();
//

//
// Rule 169:  IfThenStatement ::= if LPAREN Expression RPAREN Statement
//
#line 2789 "java.g"
void Parser::Act169()
{
    AstBlock *block = Sym(5) -> BlockCast();
    if (! block)
    {
        block = ast_pool -> NewBlock();
        block -> AllocateStatements(1); // allocate 1 element
        block -> left_brace_token = Token(5);
        block -> AddStatement(DYNAMIC_CAST<AstStatement *> (Sym(5)));
        block -> right_brace_token = Sym(5) -> RightToken();
        block -> no_braces = true;
    }

    AstIfStatement *p = ast_pool -> NewIfStatement();
    p -> if_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    p -> true_statement = block;
    p -> false_statement_opt = NULL;
    Sym(1) = p;
}

//
// Rule 170:  IfThenElseStatement ::= if LPAREN Expression RPAREN StatementNoShortIf else Statement
//
#line 2814 "java.g"
void Parser::MakeIfThenElseStatement()
{
    AstBlock *true_block = Sym(5) -> BlockCast();
    if (! true_block)
    {
        true_block = ast_pool -> NewBlock();
        true_block -> AllocateStatements(1); // allocate 1 element
        true_block -> left_brace_token = Token(5);
        true_block -> AddStatement(DYNAMIC_CAST<AstStatement *> (Sym(5)));
        true_block -> right_brace_token = Sym(5) -> RightToken();
        true_block -> no_braces = true;
    }

    AstBlock *false_block = Sym(7) -> BlockCast();
    if (! false_block)
    {
        false_block = ast_pool -> NewBlock();
        false_block -> AllocateStatements(1); // allocate 1 element
        false_block -> left_brace_token = Token(7);
        false_block -> AddStatement(DYNAMIC_CAST<AstStatement *> (Sym(7)));
        false_block -> right_brace_token = Sym(7) -> RightToken();
        false_block -> no_braces = true;
    }

    AstIfStatement *p = ast_pool -> NewIfStatement();
    p -> if_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    p -> true_statement = true_block;
    p -> false_statement_opt = false_block;
    Sym(1) = p;
}

//
// Rule 171:  IfThenElseStatementNoShortIf ::= if LPAREN Expression RPAREN StatementNoShortIf else StatementNoShortIf
//
// void MakeIfThenElseStatement();
//

//
// Rule 172:  SwitchStatement ::= switch LPAREN Expression RPAREN SwitchBlock
//
#line 2857 "java.g"
void Parser::Act172()
{
    AstSwitchStatement *p = DYNAMIC_CAST<AstSwitchStatement *> (Sym(5));
    p -> switch_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 173:  SwitchBlock ::= LBRACE SwitchBlockStatements SwitchLabelsopt RBRACE
//
#line 2875 "java.g"
void Parser::Act173()
{
    AstSwitchStatement* p = ast_pool -> NewSwitchStatement();
    AstBlock* block = ast_pool -> NewBlock();
    block -> left_brace_token = Token(1);
    AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(2));
    block -> AllocateStatements(tail -> index + (Sym(3) ? 2 : 1));
    AstListNode* root = tail;
    do
    {
        root = root -> next;
        block -> AddStatement(DYNAMIC_CAST<AstStatement*> (root -> element));
    } while (root != tail);
    FreeCircularList(tail);
    if (Sym(3))
        block -> AddStatement
            (MakeSwitchBlockStatement(DYNAMIC_CAST<AstListNode*> (Sym(3))));
    block -> right_brace_token = Token(4);
    block -> block_tag = AstBlock::SWITCH;
    p -> switch_block = block;
    Sym(1) = p;
}

//
// Rule 174:  SwitchBlock ::= LBRACE SwitchLabelsopt RBRACE
//
#line 2902 "java.g"
void Parser::Act174()
{
    AstSwitchStatement* p = ast_pool -> NewSwitchStatement();
    AstBlock* block = ast_pool -> NewBlock();
    block -> AllocateStatements(1); // allocate 1 element
    block -> left_brace_token = Token(1);
    if (Sym(2))
        block -> AddStatement
            (MakeSwitchBlockStatement(DYNAMIC_CAST<AstListNode*> (Sym(2))));
    block -> right_brace_token = Token(3);
    block -> block_tag = AstBlock::SWITCH;

    p -> switch_block = block;

    Sym(1) = p;
}

//
// Rule 175:  SwitchBlockStatements ::= SwitchBlockStatement
//
#line 2923 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act175()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 176:  SwitchBlockStatements ::= SwitchBlockStatements SwitchBlockStatement
//
#line 2940 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act176()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 177:  SwitchBlockStatement ::= SwitchLabels BlockStatements
//
#line 2961 "java.g"
void Parser::Act177()
{
    Sym(1) = MakeSwitchBlockStatement(DYNAMIC_CAST<AstListNode*> (Sym(1)),
                                      DYNAMIC_CAST<AstListNode*> (Sym(2)));
}

AstStatement* Parser::MakeSwitchBlockStatement(AstListNode* labels,
                                               AstListNode* statements)
{
    AstSwitchBlockStatement* p = ast_pool -> NewSwitchBlockStatement();
    assert(labels);
    AstListNode* tail = labels;
    p -> AllocateSwitchLabels(tail -> index + 1);
    AstListNode* root = tail;
    do
    {
        root = root -> next;
        p -> AddSwitchLabel(DYNAMIC_CAST<AstSwitchLabel*> (root -> element));
    } while (root != tail);
    FreeCircularList(tail);
    if (statements)
    {
        tail = statements;
        p -> AllocateStatements(tail -> index + 1);
        root = tail;
        do
        {
            root = root -> next;
            p -> AddStatement(DYNAMIC_CAST<AstStatement*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    else p -> AddStatement(ast_pool ->
                           GenEmptyStatement(labels -> RightToken()));
    p -> right_brace_token =
        p -> Statement(p -> NumStatements() - 1) -> RightToken();
    return p;
}

//
// Rule 178:  SwitchLabels ::= SwitchLabel
//
#line 3004 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act178()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 179:  SwitchLabels ::= SwitchLabels SwitchLabel
//
#line 3021 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act179()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 180:  SwitchLabel ::= case Expression COLON
//
#line 3046 "java.g"
void Parser::MakeSwitchLabel()
{
    AstSwitchLabel* p =
        ast_pool -> NewSwitchLabel(DYNAMIC_CAST<AstExpression*> (Sym(2)));
    p -> case_token = Token(1);
    p -> colon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 181:  SwitchLabel ::= default Marker COLON
//
// void MakeSwitchLabel();
//

//
// Rule 182:  WhileStatement ::= while LPAREN Expression RPAREN Statement
//
#line 3070 "java.g"
void Parser::MakeWhileStatement()
{
    AstWhileStatement *p = ast_pool -> NewWhileStatement();
    p -> while_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    p -> statement = DYNAMIC_CAST<AstStatement *> (Sym(5));

    AstBlock *block = ast_pool -> NewBlock();
    block -> AllocateStatements(1); // allocate 1 element
    block -> left_brace_token = Token(1); // point to 'WHILE' keyword
    block -> AddStatement(p);
    block -> right_brace_token = Sym(5) -> RightToken(); // point to last token in statement
    block -> no_braces = true;

    Sym(1) = block;
}

//
// Rule 183:  WhileStatementNoShortIf ::= while LPAREN Expression RPAREN StatementNoShortIf
//
// void MakeWhileStatement();
//

//
// Rule 184:  DoStatement ::= do Statement while LPAREN Expression RPAREN SEMICOLON
//
#line 3098 "java.g"
void Parser::Act184()
{
    AstDoStatement *p = ast_pool -> NewDoStatement();
    p -> do_token = Token(1);
    p -> statement = DYNAMIC_CAST<AstStatement *> (Sym(2));
    p -> while_token = Token(3);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(5));
    p -> semicolon_token = Token(7);

    AstBlock *block = ast_pool -> NewBlock();
    block -> AllocateStatements(1); // allocate 1 element
    block -> left_brace_token = Token(1);
    block -> AddStatement(p);
    block -> right_brace_token = Token(7);
    block -> no_braces = true;

    Sym(1) = block;
}

//
// Rule 185:  ForStatement ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN Statement
//
#line 3121 "java.g"
void Parser::MakeForStatement()
{
    AstForStatement *p = ast_pool -> NewForStatement();
    p -> for_token = Token(1);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateForInitStatements(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddForInitStatement(DYNAMIC_CAST<AstStatement *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> end_expression_opt = DYNAMIC_CAST<AstExpression *> (Sym(5));
    if (Sym(7) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(7));
        p -> AllocateForUpdateStatements(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddForUpdateStatement(DYNAMIC_CAST<AstExpressionStatement *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> statement = DYNAMIC_CAST<AstStatement *> (Sym(9));

    AstBlock *block = ast_pool -> NewBlock();
    block -> AllocateStatements(1); // allocate 1 element
    block -> left_brace_token = Token(1);
    block -> AddStatement(p);
    block -> right_brace_token = Sym(9) -> RightToken();
    block -> no_braces = true;

    Sym(1) = block;
}

//
// Rule 186:  ForStatementNoShortIf ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN...
//
// void MakeForStatement();
//

//
// Rule 187:  ForInit -> StatementExpressionList
//
// void NoAction();
//

//
// Rule 188:  ForInit ::= LocalVariableDeclaration
//
#line 3177 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act188()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 189:  ForUpdate -> StatementExpressionList
//
// void NoAction();
//

//
// Rule 190:  StatementExpressionList ::= StatementExpression
//
#line 3198 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act190()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 191:  StatementExpressionList ::= StatementExpressionList COMMA StatementExpression
//
#line 3215 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act191()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 192:  AssertStatement ::= assert Expression SEMICOLON
//
#line 3239 "java.g"
void Parser::Act192()
{
    AstAssertStatement *p = ast_pool -> NewAssertStatement();
    p -> assert_token = Token(1);
    p -> condition = DYNAMIC_CAST<AstExpression *> (Sym(2));
    p -> message_opt = NULL;
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 193:  AssertStatement ::= assert Expression COLON Expression SEMICOLON
//
#line 3253 "java.g"
void Parser::Act193()
{
    AstAssertStatement *p = ast_pool -> NewAssertStatement();
    p -> assert_token = Token(1);
    p -> condition = DYNAMIC_CAST<AstExpression *> (Sym(2));
    p -> message_opt = DYNAMIC_CAST<AstExpression *> (Sym(4));
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 194:  BreakStatement ::= break SEMICOLON
//
#line 3274 "java.g"
void Parser::Act194()
{
    AstBreakStatement *p = ast_pool -> NewBreakStatement();
    p -> break_token = Token(1);
    p -> identifier_token_opt = 0;
    p -> semicolon_token = Token(2);
    Sym(1) = p;
}

//
// Rule 195:  BreakStatement ::= break Identifier SEMICOLON
//
#line 3287 "java.g"
void Parser::Act195()
{
    AstBreakStatement *p = ast_pool -> NewBreakStatement();
    p -> break_token = Token(1);
    p -> identifier_token_opt = Token(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 196:  ContinueStatement ::= continue SEMICOLON
//
#line 3300 "java.g"
void Parser::Act196()
{
    AstContinueStatement *p = ast_pool -> NewContinueStatement();
    p -> continue_token = Token(1);
    p -> identifier_token_opt = 0;
    p -> semicolon_token = Token(2);
    Sym(1) = p;
}

//
// Rule 197:  ContinueStatement ::= continue Identifier SEMICOLON
//
#line 3313 "java.g"
void Parser::Act197()
{
    AstContinueStatement *p = ast_pool -> NewContinueStatement();
    p -> continue_token = Token(1);
    p -> identifier_token_opt = Token(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 198:  ReturnStatement ::= return Expressionopt SEMICOLON
//
#line 3326 "java.g"
void Parser::Act198()
{
    AstReturnStatement *p = ast_pool -> NewReturnStatement();
    p -> return_token = Token(1);
    p -> expression_opt = DYNAMIC_CAST<AstExpression *> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 199:  ThrowStatement ::= throw Expression SEMICOLON
//
#line 3339 "java.g"
void Parser::Act199()
{
    AstThrowStatement *p = ast_pool -> NewThrowStatement();
    p -> throw_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 200:  SynchronizedStatement ::= synchronized LPAREN Expression RPAREN Block
//
#line 3352 "java.g"
void Parser::Act200()
{
    AstSynchronizedStatement *p = ast_pool -> NewSynchronizedStatement();
    p -> synchronized_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    p -> block = DYNAMIC_CAST<AstBlock *> (Sym(5));
    p -> block -> block_tag = AstBlock::SYNCHRONIZED;

    Sym(1) = p;
}

//
// Rule 201:  TryStatement ::= try Block Catches
//
#line 3367 "java.g"
void Parser::Act201()
{
    AstTryStatement *p = ast_pool -> NewTryStatement();
    p -> try_token = Token(1);
    p -> block = DYNAMIC_CAST<AstBlock *> (Sym(2));

    //
    // The list of modifiers is guaranteed not empty
    //
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateCatchClauses(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddCatchClause(DYNAMIC_CAST<AstCatchClause *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> finally_clause_opt = NULL;
    Sym(1) = p;
}

//
// Rule 202:  TryStatement ::= try Block Catchesopt Finally
//
#line 3395 "java.g"
void Parser::Act202()
{
    AstTryStatement *p = ast_pool -> NewTryStatement();
    p -> try_token = Token(1);
    p -> block = DYNAMIC_CAST<AstBlock *> (Sym(2));
    p -> block -> block_tag = AstBlock::TRY_CLAUSE_WITH_FINALLY;

    if (Sym(3) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateCatchClauses(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddCatchClause(DYNAMIC_CAST<AstCatchClause *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }

    for (int i = 0; i < p -> NumCatchClauses(); i++)
        p -> CatchClause(i) -> block -> block_tag = AstBlock::TRY_CLAUSE_WITH_FINALLY;

    p -> finally_clause_opt = DYNAMIC_CAST<AstFinallyClause *> (Sym(4));

    Sym(1) = p;
}

//
// Rule 203:  Catches ::= CatchClause
//
#line 3427 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act203()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 204:  Catches ::= Catches CatchClause
//
#line 3444 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act204()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 205:  CatchClause ::= catch LPAREN FormalParameter RPAREN Block
//
#line 3465 "java.g"
void Parser::Act205()
{
    AstCatchClause *p = ast_pool -> NewCatchClause();
    p -> catch_token = Token(1);
    p -> formal_parameter = DYNAMIC_CAST<AstFormalParameter *> (Sym(3));
    p -> block = DYNAMIC_CAST<AstBlock *> (Sym(5));

    Sym(1) = p;
}

//
// Rule 206:  Finally ::= finally Block
//
#line 3479 "java.g"
void Parser::Act206()
{
    AstFinallyClause *p = ast_pool -> NewFinallyClause();
    p -> finally_token = Token(1);
    p -> block = DYNAMIC_CAST<AstBlock *> (Sym(2));
    p -> block -> block_tag = AstBlock::FINALLY;

    Sym(1) = p;
}

//
// Rule 207:  Primary -> PrimaryNoNewArray
//
// void NoAction();
//

//
// Rule 208:  Primary -> ArrayCreationUninitialized
//
// void NoAction();
//

//
// Rule 209:  Primary -> ArrayCreationInitialized
//
// void NoAction();
//

//
// Rule 210:  PrimaryNoNewArray -> Literal
//
// void NoAction();
//

//
// Rule 211:  PrimaryNoNewArray ::= this
//
#line 3520 "java.g"
void Parser::Act211()
{
    Sym(1) = ast_pool -> NewThisExpression(Token(1));
}

//
// Rule 212:  PrimaryNoNewArray ::= LPAREN Expression RPAREN
//
#line 3529 "java.g"
void Parser::Act212()
{
    AstParenthesizedExpression *p = ast_pool -> NewParenthesizedExpression();
    p -> left_parenthesis_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(2));
    p -> right_parenthesis_token = Token(3);
    Sym(1) = p;
}

//
// Rule 213:  PrimaryNoNewArray -> ClassInstanceCreationExpression
//
// void NoAction();
//

//
// Rule 214:  PrimaryNoNewArray -> FieldAccess
//
// void NoAction();
//

//
// Rule 215:  PrimaryNoNewArray ::= Name DOT this
//
#line 3553 "java.g"
void Parser::Act215()
{
    AstFieldAccess *p = ast_pool -> NewFieldAccess(AstFieldAccess::THIS_TAG);
    p -> base = ast_pool -> NewTypeExpression(Sym(1));
    p -> dot_token = Token(2);
    p -> identifier_token = Token(3);
    Sym(1) = p;
}

//
// Rule 216:  PrimaryNoNewArray ::= Type DOT class
//
#line 3567 "java.g"
void Parser::Act216()
{
    AstFieldAccess *p = ast_pool -> NewFieldAccess(AstFieldAccess::CLASS_TAG);
    p -> base = ast_pool -> NewTypeExpression(Sym(1));
    p -> dot_token = Token(2);
    p -> identifier_token = Token(3);
    Sym(1) = p;
}

//
// Rule 217:  PrimaryNoNewArray ::= void DOT class
//
#line 3581 "java.g"
void Parser::Act217()
{
    AstFieldAccess *p = ast_pool -> NewFieldAccess(AstFieldAccess::CLASS_TAG);
    p -> base = ast_pool -> NewTypeExpression(ast_pool -> NewPrimitiveType(Ast::VOID_TYPE, Token(1)));
    p -> dot_token = Token(2);
    p -> identifier_token = Token(3);
    Sym(1) = p;
}

//
// Rule 218:  PrimaryNoNewArray -> MethodInvocation
//
// void NoAction();
//

//
// Rule 219:  PrimaryNoNewArray -> ArrayAccess
//
// void NoAction();
//

//
// Rule 220:  ClassInstanceCreationExpression ::= new ClassType LPAREN ArgumentListopt RPAREN ClassBodyopt
//
#line 3607 "java.g"
void Parser::Act220()
{
    AstClassInstanceCreationExpression *p = ast_pool -> NewClassInstanceCreationExpression();
    p -> base_opt = NULL;
    p -> dot_token_opt = 0;
    p -> new_token = Token(1);
    p -> class_type = ast_pool -> NewTypeExpression(Sym(2));
    p -> left_parenthesis_token = Token(3);
    if (Sym(4) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(4));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(5);
    p -> class_body_opt = DYNAMIC_CAST<AstClassBody *> (Sym(6));
    Sym(1) = p;
}

//
// Rule 221:  ClassInstanceCreationExpression ::= Primary DOT new SimpleName LPAREN ArgumentListopt RPAREN ClassBodyopt
//
#line 3637 "java.g"
void Parser::MakeQualifiedNew()
{
    AstClassInstanceCreationExpression *p = ast_pool -> NewClassInstanceCreationExpression();
    p -> base_opt = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> dot_token_opt = Token(2);
    p -> new_token = Token(3);
    p -> class_type = ast_pool -> NewTypeExpression(Sym(4));
    p -> left_parenthesis_token = Token(5);
    if (Sym(6) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(6));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(7);
    p -> class_body_opt = DYNAMIC_CAST<AstClassBody *> (Sym(8));
    Sym(1) = p;
}

//
// Rule 222:  ClassInstanceCreationExpression ::= Name DOT new SimpleName LPAREN ArgumentListopt RPAREN ClassBodyopt
//
// void MakeQualifiedNew();
//

//
// Rule 223:  ArgumentList ::= Expression
//
#line 3674 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act223()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 224:  ArgumentList ::= ArgumentList COMMA Expression
//
#line 3691 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act224()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 225:  ArrayCreationUninitialized ::= new PrimitiveType DimExprs Dimsopt
//
#line 3720 "java.g"
void Parser::MakeArrayCreationExpression()
{
    AstArrayCreationExpression *p = ast_pool -> NewArrayCreationExpression();
    p -> new_token = Token(1);
    p -> array_type = Sym(2);
    //
    // The list of DimExprs is never null
    //
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateDimExprs(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddDimExpr(DYNAMIC_CAST<AstDimExpr *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }

    if (Sym(4) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(4));
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets(DYNAMIC_CAST<AstBrackets *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> array_initializer_opt = NULL;
    Sym(1) = p;
}

//
// Rule 226:  ArrayCreationUninitialized ::= new ClassOrInterfaceType DimExprs Dimsopt
//
// void MakeArrayCreationExpression();
//

//
// Rule 227:  ArrayCreationInitialized ::= new ArrayType ArrayInitializer
//
#line 3774 "java.g"
void Parser::Act227()
{
    AstArrayCreationExpression *p = ast_pool -> NewArrayCreationExpression();
    p -> new_token = Token(1);
    p -> array_type = Sym(2);
    p -> array_initializer_opt = DYNAMIC_CAST<AstArrayInitializer *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 228:  DimExprs ::= DimExpr
//
#line 3787 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act228()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 229:  DimExprs ::= DimExprs DimExpr
//
#line 3804 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act229()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 230:  DimExpr ::= LBRACKET Expression RBRACKET
//
#line 3825 "java.g"
void Parser::Act230()
{
    AstDimExpr *p = ast_pool -> NewDimExpr();
    p -> left_bracket_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(2));
    p -> right_bracket_token = Token(3);
    Sym(1) = p;
}

//
// Rule 231:  Dims ::= LBRACKET RBRACKET
//
#line 3838 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act231()
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = ast_pool -> NewBrackets(Token(1), Token(2));
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 232:  Dims ::= Dims LBRACKET RBRACKET
//
#line 3855 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act232()
{
    AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(1));

    AstListNode *p = AllocateListNode();
    p -> element = ast_pool -> NewBrackets(Token(2), Token(3));
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 233:  FieldAccess ::= Primary DOT Identifier
//
// void MakeFieldAccess();
//

//
// Rule 234:  FieldAccess ::= super DOT Identifier
//
#line 3883 "java.g"
void Parser::MakeSuperFieldAccess()
{
    Sym(1) = ast_pool -> NewSuperExpression(Token(1));

    MakeFieldAccess();
}

//
// Rule 235:  FieldAccess ::= Name DOT super DOT Identifier
//
#line 3897 "java.g"
void Parser::MakeSuperDoubleFieldAccess()
{
    AstFieldAccess *p = ast_pool -> NewFieldAccess();

         AstFieldAccess *q = ast_pool -> NewFieldAccess(AstFieldAccess::SUPER_TAG);
         q -> base = ast_pool -> NewTypeExpression(Sym(1));
         q -> dot_token = Token(2);
         q -> identifier_token = Token(3);

    p -> base = q;
    p -> dot_token = Token(4);
    p -> identifier_token = Token(5);

    Sym(1) = p;
}

//
// Rule 236:  MethodInvocation ::= Name LPAREN ArgumentListopt RPAREN
//
#line 3917 "java.g"
void Parser::Act236()
{
    AstMethodInvocation *p = ast_pool -> NewMethodInvocation();
    p -> method = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> left_parenthesis_token = Token(2);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    Sym(1) = p;
}

//
// Rule 237:  MethodInvocation ::= Primary DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3942 "java.g"
void Parser::Act237()
{
    MakeFieldAccess();

    AstMethodInvocation *p = ast_pool -> NewMethodInvocation();
    p -> method = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> left_parenthesis_token = Token(4);
    if (Sym(5) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(5));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(6);
    Sym(1) = p;
}

//
// Rule 238:  MethodInvocation ::= super DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3969 "java.g"
void Parser::Act238()
{
    MakeSuperFieldAccess();

    AstMethodInvocation *p = ast_pool -> NewMethodInvocation();
    p -> method = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> left_parenthesis_token = Token(4);
    if (Sym(5) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(5));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(6);
    Sym(1) = p;
}

//
// Rule 239:  MethodInvocation ::= Name DOT super DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3999 "java.g"
void Parser::Act239()
{
    MakeSuperDoubleFieldAccess();

    AstMethodInvocation *p = ast_pool -> NewMethodInvocation();
    p -> method = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> left_parenthesis_token = Token(6);
    if (Sym(7) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(7));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(8);
    Sym(1) = p;
}

//
// Rule 240:  ArrayAccess ::= Name LBRACKET Expression RBRACKET
//
#line 4026 "java.g"
void Parser::MakeArrayAccess()
{
    AstArrayAccess *p = ast_pool -> NewArrayAccess();
    p -> base = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> left_bracket_token = Token(2);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    p -> right_bracket_token = Token(4);
    Sym(1) = p;
}

//
// Rule 241:  ArrayAccess ::= PrimaryNoNewArray LBRACKET Expression RBRACKET
//
// void MakeArrayAccess();
//

//
// Rule 242:  ArrayAccess ::= ArrayCreationInitialized LBRACKET Expression RBRACKET
//
// void MakeArrayAccess();
//

//
// Rule 243:  PostfixExpression -> Primary
//
// void NoAction();
//

//
// Rule 244:  PostfixExpression -> Name
//
// void NoAction();
//

//
// Rule 245:  PostfixExpression -> PostIncrementExpression
//
// void NoAction();
//

//
// Rule 246:  PostfixExpression -> PostDecrementExpression
//
// void NoAction();
//

//
// Rule 247:  PostIncrementExpression ::= PostfixExpression PLUS_PLUS
//
#line 4074 "java.g"
void Parser::Act247()
{
    AstPostUnaryExpression *p = ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::PLUSPLUS);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}

//
// Rule 248:  PostDecrementExpression ::= PostfixExpression MINUS_MINUS
//
#line 4086 "java.g"
void Parser::Act248()
{
    AstPostUnaryExpression *p = ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::MINUSMINUS);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}

//
// Rule 249:  UnaryExpression -> PreIncrementExpression
//
// void NoAction();
//

//
// Rule 250:  UnaryExpression -> PreDecrementExpression
//
// void NoAction();
//

//
// Rule 251:  UnaryExpression ::= PLUS UnaryExpression
//
#line 4106 "java.g"
void Parser::Act251()
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::PLUS);
    p -> pre_operator_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(2));
    Sym(1) = p;
}

//
// Rule 252:  UnaryExpression ::= MINUS UnaryExpression
//
#line 4118 "java.g"
void Parser::Act252()
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::MINUS);
    p -> pre_operator_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(2));
    Sym(1) = p;
}

//
// Rule 253:  UnaryExpression -> UnaryExpressionNotPlusMinus
//
// void NoAction();
//

//
// Rule 254:  PreIncrementExpression ::= PLUS_PLUS UnaryExpression
//
#line 4134 "java.g"
void Parser::Act254()
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::PLUSPLUS);
    p -> pre_operator_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(2));
    Sym(1) = p;
}

//
// Rule 255:  PreDecrementExpression ::= MINUS_MINUS UnaryExpression
//
#line 4146 "java.g"
void Parser::Act255()
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::MINUSMINUS);
    p -> pre_operator_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(2));
    Sym(1) = p;
}

//
// Rule 256:  UnaryExpressionNotPlusMinus -> PostfixExpression
//
// void NoAction();
//

//
// Rule 257:  UnaryExpressionNotPlusMinus ::= TWIDDLE UnaryExpression
//
#line 4162 "java.g"
void Parser::Act257()
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::TWIDDLE);
    p -> pre_operator_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(2));
    Sym(1) = p;
}

//
// Rule 258:  UnaryExpressionNotPlusMinus ::= NOT UnaryExpression
//
#line 4174 "java.g"
void Parser::Act258()
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::NOT);
    p -> pre_operator_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(2));
    Sym(1) = p;
}

//
// Rule 259:  UnaryExpressionNotPlusMinus -> CastExpression
//
// void NoAction();
//

//
// Rule 260:  CastExpression ::= LPAREN PrimitiveType Dimsopt RPAREN UnaryExpression
//
#line 4190 "java.g"
void Parser::MakeCastExpression()
{
    AstCastExpression *p = ast_pool -> NewCastExpression();
    p -> left_parenthesis_token_opt = Token(1);
    p -> type_opt = Sym(2);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = DYNAMIC_CAST<AstListNode *> (Sym(3));
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets(DYNAMIC_CAST<AstBrackets *> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token_opt = Token(4);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(5));
    Sym(1) = p;
}

//
// Rule 261:  CastExpression ::= LPAREN Expression RPAREN UnaryExpressionNotPlusMinus
//
#line 4216 "java.g"
void Parser::Act261()
{
    //
    // Note that Expression must be a name - i.e., Sym(2) -> isName() == true
    // This check is not performed here and should be performed during
    // semantic processing.
    //
    AstCastExpression *p = ast_pool -> NewCastExpression();
    p -> left_parenthesis_token_opt = Token(1);
    p -> type_opt = Sym(2);
    p -> right_parenthesis_token_opt = Token(3);
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(4));
    Sym(1) = p;
}

//
// Rule 262:  CastExpression ::= LPAREN Name Dims RPAREN UnaryExpressionNotPlusMinus
//
// void MakeCastExpression();
//

//
// Rule 263:  MultiplicativeExpression -> UnaryExpression
//
// void NoAction();
//

//
// Rule 264:  MultiplicativeExpression ::= MultiplicativeExpression MULTIPLY UnaryExpression
//
#line 4246 "java.g"
void Parser::Act264()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::STAR);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 265:  MultiplicativeExpression ::= MultiplicativeExpression DIVIDE UnaryExpression
//
#line 4259 "java.g"
void Parser::Act265()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::SLASH);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 266:  MultiplicativeExpression ::= MultiplicativeExpression REMAINDER UnaryExpression
//
#line 4272 "java.g"
void Parser::Act266()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::MOD);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 267:  AdditiveExpression -> MultiplicativeExpression
//
// void NoAction();
//

//
// Rule 268:  AdditiveExpression ::= AdditiveExpression PLUS MultiplicativeExpression
//
#line 4289 "java.g"
void Parser::Act268()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::PLUS);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 269:  AdditiveExpression ::= AdditiveExpression MINUS MultiplicativeExpression
//
#line 4302 "java.g"
void Parser::Act269()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::MINUS);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 270:  ShiftExpression -> AdditiveExpression
//
// void NoAction();
//

//
// Rule 271:  ShiftExpression ::= ShiftExpression LEFT_SHIFT AdditiveExpression
//
#line 4319 "java.g"
void Parser::Act271()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::LEFT_SHIFT);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 272:  ShiftExpression ::= ShiftExpression RIGHT_SHIFT AdditiveExpression
//
#line 4332 "java.g"
void Parser::Act272()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::RIGHT_SHIFT);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 273:  ShiftExpression ::= ShiftExpression UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
#line 4345 "java.g"
void Parser::Act273()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::UNSIGNED_RIGHT_SHIFT);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 274:  RelationalExpression -> ShiftExpression
//
// void NoAction();
//

//
// Rule 275:  RelationalExpression ::= RelationalExpression LESS ShiftExpression
//
#line 4362 "java.g"
void Parser::Act275()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::LESS);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 276:  RelationalExpression ::= RelationalExpression GREATER ShiftExpression
//
#line 4375 "java.g"
void Parser::Act276()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::GREATER);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 277:  RelationalExpression ::= RelationalExpression LESS_EQUAL ShiftExpression
//
#line 4388 "java.g"
void Parser::Act277()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::LESS_EQUAL);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 278:  RelationalExpression ::= RelationalExpression GREATER_EQUAL ShiftExpression
//
#line 4401 "java.g"
void Parser::Act278()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::GREATER_EQUAL);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 279:  RelationalExpression ::= RelationalExpression instanceof ReferenceType
//
#line 4414 "java.g"
void Parser::Act279()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::INSTANCEOF);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = ast_pool -> NewTypeExpression(Sym(3));
    Sym(1) = p;
}

//
// Rule 280:  EqualityExpression -> RelationalExpression
//
// void NoAction();
//

//
// Rule 281:  EqualityExpression ::= EqualityExpression EQUAL_EQUAL RelationalExpression
//
#line 4431 "java.g"
void Parser::Act281()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::EQUAL_EQUAL);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 282:  EqualityExpression ::= EqualityExpression NOT_EQUAL RelationalExpression
//
#line 4444 "java.g"
void Parser::Act282()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::NOT_EQUAL);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 283:  AndExpression -> EqualityExpression
//
// void NoAction();
//

//
// Rule 284:  AndExpression ::= AndExpression AND EqualityExpression
//
#line 4462 "java.g"
void Parser::Act284()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::AND);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 285:  ExclusiveOrExpression -> AndExpression
//
// void NoAction();
//

//
// Rule 286:  ExclusiveOrExpression ::= ExclusiveOrExpression XOR AndExpression
//
#line 4479 "java.g"
void Parser::Act286()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::XOR);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 287:  InclusiveOrExpression -> ExclusiveOrExpression
//
// void NoAction();
//

//
// Rule 288:  InclusiveOrExpression ::= InclusiveOrExpression OR ExclusiveOrExpression
//
#line 4496 "java.g"
void Parser::Act288()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::IOR);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 289:  ConditionalAndExpression -> InclusiveOrExpression
//
// void NoAction();
//

//
// Rule 290:  ConditionalAndExpression ::= ConditionalAndExpression AND_AND InclusiveOrExpression
//
#line 4513 "java.g"
void Parser::Act290()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::AND_AND);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 291:  ConditionalOrExpression -> ConditionalAndExpression
//
// void NoAction();
//

//
// Rule 292:  ConditionalOrExpression ::= ConditionalOrExpression OR_OR ConditionalAndExpression
//
#line 4530 "java.g"
void Parser::Act292()
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::OR_OR);
    p -> left_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 293:  ConditionalExpression -> ConditionalOrExpression
//
// void NoAction();
//

//
// Rule 294:  ConditionalExpression ::= ConditionalOrExpression QUESTION Expression COLON ConditionalExpression
//
#line 4547 "java.g"
void Parser::Act294()
{
    AstConditionalExpression *p = ast_pool -> NewConditionalExpression();
    p -> test_expression = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> question_token = Token(2);
    p -> true_expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    p -> colon_token = Token(4);
    p -> false_expression = DYNAMIC_CAST<AstExpression *> (Sym(5));
    Sym(1) = p;
}

//
// Rule 295:  AssignmentExpression -> ConditionalExpression
//
// void NoAction();
//

//
// Rule 296:  AssignmentExpression -> Assignment
//
// void NoAction();
//

//
// Rule 297:  Assignment ::= PostfixExpression AssignmentOperator AssignmentExpression
//
#line 4576 "java.g"
void Parser::Act297()
{
    AstAssignmentExpression *p = DYNAMIC_CAST<AstAssignmentExpression *> (Sym(2));
    p -> left_hand_side = DYNAMIC_CAST<AstExpression *> (Sym(1));
    p -> expression = DYNAMIC_CAST<AstExpression *> (Sym(3));
    Sym(1) = p;
}

//
// Rule 298:  AssignmentOperator ::= EQUAL
//
#line 4603 "java.g"
void Parser::Act298()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::SIMPLE_EQUAL, Token(1));
}

//
// Rule 299:  AssignmentOperator ::= MULTIPLY_EQUAL
//
#line 4612 "java.g"
void Parser::Act299()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::STAR_EQUAL, Token(1));
}

//
// Rule 300:  AssignmentOperator ::= DIVIDE_EQUAL
//
#line 4621 "java.g"
void Parser::Act300()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::SLASH_EQUAL, Token(1));
}

//
// Rule 301:  AssignmentOperator ::= REMAINDER_EQUAL
//
#line 4630 "java.g"
void Parser::Act301()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::MOD_EQUAL, Token(1));
}

//
// Rule 302:  AssignmentOperator ::= PLUS_EQUAL
//
#line 4639 "java.g"
void Parser::Act302()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::PLUS_EQUAL, Token(1));
}

//
// Rule 303:  AssignmentOperator ::= MINUS_EQUAL
//
#line 4648 "java.g"
void Parser::Act303()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::MINUS_EQUAL, Token(1));
}

//
// Rule 304:  AssignmentOperator ::= LEFT_SHIFT_EQUAL
//
#line 4657 "java.g"
void Parser::Act304()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::LEFT_SHIFT_EQUAL, Token(1));
}

//
// Rule 305:  AssignmentOperator ::= RIGHT_SHIFT_EQUAL
//
#line 4666 "java.g"
void Parser::Act305()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::RIGHT_SHIFT_EQUAL, Token(1));
}

//
// Rule 306:  AssignmentOperator ::= UNSIGNED_RIGHT_SHIFT_EQUAL
//
#line 4675 "java.g"
void Parser::Act306()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::UNSIGNED_RIGHT_SHIFT_EQUAL, Token(1));
}

//
// Rule 307:  AssignmentOperator ::= AND_EQUAL
//
#line 4684 "java.g"
void Parser::Act307()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::AND_EQUAL, Token(1));
}

//
// Rule 308:  AssignmentOperator ::= XOR_EQUAL
//
#line 4693 "java.g"
void Parser::Act308()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::XOR_EQUAL, Token(1));
}

//
// Rule 309:  AssignmentOperator ::= OR_EQUAL
//
#line 4702 "java.g"
void Parser::Act309()
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::IOR_EQUAL, Token(1));
}

//
// Rule 310:  Expression -> AssignmentExpression
//
// void NoAction();
//

//
// Rule 311:  PackageDeclarationopt ::=
//
#line 4726 "java.g"
//
// Given a rule of the form A ::= x1 x2 ... xn
//
// Construct a NULL Ast for A.
//
void Parser::NullAction() { Sym(1) = NULL; }

//
// Rule 312:  PackageDeclarationopt -> PackageDeclaration
//
// void NoAction();
//

//
// Rule 313:  Superopt ::=
//
// void NullAction();
//

//
// Rule 314:  Superopt -> Super
//
// void NoAction();
//

//
// Rule 315:  Expressionopt ::=
//
// void NullAction();
//

//
// Rule 316:  Expressionopt -> Expression
//
// void NoAction();
//

//
// Rule 317:  ClassBodyopt ::=
//
// void NullAction();
//

//
// Rule 318:  ClassBodyopt -> ClassBody
//
// void NoAction();
//

//
// Rule 319:  ,opt ::=
//
// void NullAction();
//

//
// Rule 320:  ,opt -> COMMA
//
// void NoAction();
//

//
// Rule 321:  ImportDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 322:  ImportDeclarationsopt -> ImportDeclarations
//
// void NoAction();
//

//
// Rule 323:  TypeDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 324:  TypeDeclarationsopt -> TypeDeclarations
//
// void NoAction();
//

//
// Rule 325:  ClassBodyDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 326:  ClassBodyDeclarationsopt -> ClassBodyDeclarations
//
// void NoAction();
//

//
// Rule 327:  Modifiersopt ::=
//
// void NullAction();
//

//
// Rule 328:  Modifiersopt -> Modifiers
//
// void NoAction();
//

//
// Rule 329:  BlockStatementsopt ::=
//
// void NullAction();
//

//
// Rule 330:  BlockStatementsopt -> BlockStatements
//
// void NoAction();
//

//
// Rule 331:  Dimsopt ::=
//
// void NullAction();
//

//
// Rule 332:  Dimsopt -> Dims
//
// void NoAction();
//

//
// Rule 333:  ArgumentListopt ::=
//
// void NullAction();
//

//
// Rule 334:  ArgumentListopt -> ArgumentList
//
// void NoAction();
//

//
// Rule 335:  SwitchLabelsopt ::=
//
// void NullAction();
//

//
// Rule 336:  SwitchLabelsopt ::= SwitchLabels
//
// void NoAction();
//

//
// Rule 337:  Throwsopt ::=
//
// void NullAction();
//

//
// Rule 338:  Throwsopt -> Throws
//
// void NoAction();
//

//
// Rule 339:  FormalParameterListopt ::=
//
// void NullAction();
//

//
// Rule 340:  FormalParameterListopt -> FormalParameterList
//
// void NoAction();
//

//
// Rule 341:  Interfacesopt ::=
//
// void NullAction();
//

//
// Rule 342:  Interfacesopt -> Interfaces
//
// void NoAction();
//

//
// Rule 343:  InterfaceMemberDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 344:  InterfaceMemberDeclarationsopt -> InterfaceMemberDeclarations
//
// void NoAction();
//

//
// Rule 345:  ForInitopt ::=
//
// void NullAction();
//

//
// Rule 346:  ForInitopt -> ForInit
//
// void NoAction();
//

//
// Rule 347:  ForUpdateopt ::=
//
// void NullAction();
//

//
// Rule 348:  ForUpdateopt -> ForUpdate
//
// void NoAction();
//

//
// Rule 349:  ExtendsInterfacesopt ::=
//
// void NullAction();
//

//
// Rule 350:  ExtendsInterfacesopt -> ExtendsInterfaces
//
// void NoAction();
//

//
// Rule 351:  Catchesopt ::=
//
// void NullAction();
//

//
// Rule 352:  Catchesopt -> Catches
//
// void NoAction();
//

//
// Rule 353:  PackageHeaderMarker ::=
//
#line 4915 "java.g"
//
// When this function is invoked, if the "parse_package_header_only" flag
// is turned on, we skip to the end-of-file token.
//
void Parser::Act353()
{
    if (parse_package_header_only)
        lex_stream -> Reset(lex_stream -> NumTokens() - 1); // point to the EOF token
    Sym(1) = NULL;
}

//
// Rule 354:  MethodHeaderMarker ::=
//
#line 4930 "java.g"
//
// When this function is invoked, if the "parse_header_only" flag
// is turned on, the body of the method being parsed is skipped.
//
void Parser::Act354()
{
    if (parse_header_only)
    {
        TokenObject token = Token(1);

        //
        // If the first token immediately following the method header
        // is not an open brace, then we have a syntactic error. Do
        // nothing and let the error recovery take care of it.
        //
        if (lex_stream -> Kind(token) == TK_LBRACE)
            lex_stream -> Reset(lex_stream -> MatchingBrace(token));
    }

    Sym(1) = NULL;
}

//
// Rule 355:  Marker ::=
//
// void NullAction();
//
#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif
