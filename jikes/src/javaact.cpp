#line 639 "java.g"
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
// The parse was bad. Give up now.
//
void Parser::BadAction() { assert(false); }


//
// Rule 1:  Goal ::= CompilationUnit
//
#line 683 "java.g"
//
// Given a rule of the form A ::= x1 x2 ... xn        n >= 1
// Do nothing - Whatever Ast was produced for x1 is inherited by A.
//
void Parser::NoAction() {}
//
// This next rule was added to allow the parser to recognize the body of a
// funtion (constructor, method, or initializer) out of context. Note that
// the artificial terminal BodyMarker is added here to prevent an ordinary
// parse from accepting a body as a valid input - i.e., to recognize a body
// out-of-context, the BodyMarker terminal must be inserted in front of the
// input stream containing the body in question.

//
// Rule 2:  Goal ::= BodyMarker MethodBody
//
#line 701 "java.g"
//
// Given a rule of the form A ::= x1 x2, inherit the result from x2.
//
void Parser::SetSym1ToSym2() { Sym(1) = Sym(2); }

//
// Rule 3:  Literal ::= IntegerLiteral
//
#line 713 "java.g"
void Parser::Act3()
{
    Sym(1) = ast_pool -> NewIntegerLiteral(Token(1));
}

//
// Rule 4:  Literal ::= LongLiteral
//
#line 722 "java.g"
void Parser::Act4()
{
    Sym(1) = ast_pool -> NewLongLiteral(Token(1));
}

//
// Rule 5:  Literal ::= FloatLiteral
//
#line 731 "java.g"
void Parser::Act5()
{
    Sym(1) = ast_pool -> NewFloatLiteral(Token(1));
}

//
// Rule 6:  Literal ::= DoubleLiteral
//
#line 740 "java.g"
void Parser::Act6()
{
    Sym(1) = ast_pool -> NewDoubleLiteral(Token(1));
}

//
// Rule 7:  Literal ::= BooleanLiteral
//
// void NoAction();
//

//
// Rule 8:  Literal ::= CharacterLiteral
//
#line 753 "java.g"
void Parser::Act8()
{
    Sym(1) = ast_pool -> NewCharacterLiteral(Token(1));
}

//
// Rule 9:  Literal ::= StringLiteral
//
#line 762 "java.g"
void Parser::Act9()
{
    Sym(1) = ast_pool -> NewStringLiteral(Token(1));
}

//
// Rule 10:  Literal ::= null
//
#line 771 "java.g"
void Parser::Act10()
{
    Sym(1) = ast_pool -> NewNullLiteral(Token(1));
}

//
// Rule 11:  BooleanLiteral ::= true
//
#line 780 "java.g"
void Parser::Act11()
{
    Sym(1) = ast_pool -> NewTrueLiteral(Token(1));
}

//
// Rule 12:  BooleanLiteral ::= false
//
#line 789 "java.g"
void Parser::Act12()
{
    Sym(1) = ast_pool -> NewFalseLiteral(Token(1));
}

//
// Rule 13:  Type ::= PrimitiveType
//
// void NoAction();
//

//
// Rule 14:  Type ::= ReferenceType
//
// void NoAction();
//

//
// Rule 15:  PrimitiveType ::= NumericType
//
// void NoAction();
//

//
// Rule 16:  PrimitiveType ::= boolean
//
#line 812 "java.g"
void Parser::Act16()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::BOOLEAN, Token(1));
}

//
// Rule 17:  NumericType ::= IntegralType
//
// void NoAction();
//

//
// Rule 18:  NumericType ::= FloatingPointType
//
// void NoAction();
//

//
// Rule 19:  IntegralType ::= byte
//
#line 829 "java.g"
void Parser::Act19()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::BYTE, Token(1));
}

//
// Rule 20:  IntegralType ::= short
//
#line 838 "java.g"
void Parser::Act20()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::SHORT, Token(1));
}

//
// Rule 21:  IntegralType ::= int
//
#line 847 "java.g"
void Parser::Act21()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::INT, Token(1));
}

//
// Rule 22:  IntegralType ::= long
//
#line 856 "java.g"
void Parser::Act22()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::LONG, Token(1));
}

//
// Rule 23:  IntegralType ::= char
//
#line 865 "java.g"
void Parser::Act23()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::CHAR, Token(1));
}

//
// Rule 24:  FloatingPointType ::= float
//
#line 874 "java.g"
void Parser::Act24()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::FLOAT, Token(1));
}

//
// Rule 25:  FloatingPointType ::= double
//
#line 883 "java.g"
void Parser::Act25()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::DOUBLE, Token(1));
}

//
// Rule 26:  ReferenceType ::= ClassOrInterfaceType
//
// void NoAction();
//

//
// Rule 27:  ReferenceType ::= ArrayType
//
// void NoAction();
//

//
// Rule 28:  ClassOrInterfaceType ::= Name
//
#line 907 "java.g"
void Parser::Act28()
{
    Sym(1) = ast_pool -> NewTypeName(DYNAMIC_CAST<AstExpression*> (Sym(1)));
}

//
// Rule 29:  ArrayType ::= PrimitiveType Dims
//
#line 927 "java.g"
void Parser::MakeArrayType() { Sym(1) = MakeArrayType(1); }

//
// Used on variants of "Type Dimsopt". If this type has dimensions, make an
// array type; otherwise return the type name.
//
AstType* Parser::MakeArrayType(int tokennum)
{
    AstType* t = Sym(tokennum) -> IsName()
        ? ast_pool -> NewTypeName(DYNAMIC_CAST<AstExpression*> (Sym(tokennum)))
        : DYNAMIC_CAST<AstType*> (Sym(tokennum));
    if (! Sym(tokennum + 1))
        return t;
    AstArrayType* p = ast_pool -> NewArrayType(t);
    AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(tokennum + 1));
    p -> AllocateBrackets(tail -> index + 1);
    AstListNode* root = tail;
    do
    {
        root = root -> next;
        p -> AddBrackets(DYNAMIC_CAST<AstBrackets*> (root -> element));
    } while (root != tail);
    FreeCircularList(tail);
    return p;
}

//
// Rule 30:  ArrayType ::= ClassOrInterfaceType Dims
//
// void MakeArrayType();
//

//
// Rule 31:  Name ::= Identifier
//
#line 972 "java.g"
void Parser::Act31() { MakeSimpleName(1); }

//
// Used on "Identifier", and sets the corresponding symbol to a simple name.
//
AstSimpleName* Parser::MakeSimpleName(int tokennum)
{
    AstSimpleName* name = ast_pool -> NewSimpleName(Token(tokennum));
    Sym(tokennum) = name;
    return name;
}

//
// Rule 32:  Name ::= Name DOT Identifier
//
// void MakeFieldAccess();
//

//
// Rule 33:  CompilationUnit ::= PackageDeclarationopt ImportDeclarationsopt TypeDeclarationsopt
//
#line 998 "java.g"
void Parser::Act33()
{
    AstCompilationUnit* p = ast_pool -> NewCompilationUnit();
    p -> package_declaration_opt =
        DYNAMIC_CAST<AstPackageDeclaration*> (Sym(1));
    if (Sym(2))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(2));
        p -> AllocateImportDeclarations(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddImportDeclaration(DYNAMIC_CAST<AstImportDeclaration*>
                                      (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    if (Sym(3))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(3));
        p -> AllocateTypeDeclarations(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddTypeDeclaration(DYNAMIC_CAST<Ast*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 34:  ImportDeclarations ::= ImportDeclaration
//
#line 1035 "java.g"
//
// This starts a list containing a single element.
// Note that the list is circular so as to preserve the order of the elements.
//
void Parser::StartList()
{
    AstListNode* p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;
    Sym(1) = p;
}

//
// Rule 35:  ImportDeclarations ::= ImportDeclarations ImportDeclaration
//
#line 1052 "java.g"
//
// This adds token 2 to an existing list.
// Note that the list is circular so as to preserve the order of the elements.
//
void Parser::AddList2()
{
    AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(1));
    AstListNode* p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;
    p -> next = tail -> next;
    tail -> next = p;
    Sym(1) = p;
}

//
// This adds token 3 to an existing list (thus, token 2 was a delimiter).
// Note that the list is circular so as to preserve the order of the elements.
//
void Parser::AddList3()
{
    AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(1));
    AstListNode* p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;
    p -> next = tail -> next;
    tail -> next = p;
    Sym(1) = p;
}

//
// Rule 36:  TypeDeclarations ::= TypeDeclaration
//
// void StartList();
//

//
// Rule 37:  TypeDeclarations ::= TypeDeclarations TypeDeclaration
//
// void AddList2();
//

//
// Rule 38:  PackageDeclaration ::= package Name PackageHeaderMarker SEMICOLON
//
#line 1094 "java.g"
void Parser::Act38()
{
    AstPackageDeclaration* p = ast_pool -> NewPackageDeclaration();
    p -> package_token = Token(1);
    p -> name = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 39:  ImportDeclaration ::= SingleTypeImportDeclaration
//
// void NoAction();
//

//
// Rule 40:  ImportDeclaration ::= TypeImportOnDemandDeclaration
//
// void NoAction();
//

//
// Rule 41:  SingleTypeImportDeclaration ::= import Name SEMICOLON
//
#line 1119 "java.g"
void Parser::Act41()
{
    AstImportDeclaration* p = ast_pool -> NewImportDeclaration();
    p -> import_token = Token(1);
    p -> name = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 42:  TypeImportOnDemandDeclaration ::= import Name DOT MULTIPLY SEMICOLON
//
#line 1132 "java.g"
void Parser::Act42()
{
    AstImportDeclaration* p = ast_pool -> NewImportDeclaration();
    p -> import_token = Token(1);
    p -> name = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> star_token_opt = Token(4);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 43:  TypeDeclaration ::= ClassDeclaration
//
// void NoAction();
//

//
// Rule 44:  TypeDeclaration ::= InterfaceDeclaration
//
// void NoAction();
//

//
// Rule 45:  TypeDeclaration ::= SEMICOLON
//
#line 1154 "java.g"
void Parser::Act45()
{
    Sym(1) = ast_pool -> NewEmptyDeclaration(Token(1));
}

//
// Rule 46:  Modifiers ::= Modifier
//
// void StartList();
//

//
// Rule 47:  Modifiers ::= Modifiers Modifier
//
// void AddList2();
//

//
// Rule 48:  Modifier ::= public
//
#line 1183 "java.g"
void Parser::MakeModifier()
{
    Sym(1) = ast_pool -> NewModifier(Token(1));
}

//
// Rule 49:  Modifier ::= protected
//
// void MakeModifier()
//

//
// Rule 50:  Modifier ::= private
//
// void MakeModifier()
//

//
// Rule 51:  Modifier ::= static
//
#line 1206 "java.g"
void Parser::Act51()
{
    MakeModifier();
    Sym(1) -> class_tag = Ast::STATIC;
}

//
// Rule 52:  Modifier ::= abstract
//
// void MakeModifier()
//

//
// Rule 53:  Modifier ::= final
//
// void MakeModifier()
//

//
// Rule 54:  Modifier ::= native
//
// void MakeModifier()
//

//
// Rule 55:  Modifier ::= strictfp
//
// void MakeModifier()
//

//
// Rule 56:  Modifier ::= synchronized
//
// void MakeModifier()
//

//
// Rule 57:  Modifier ::= transient
//
// void MakeModifier()
//

//
// Rule 58:  Modifier ::= volatile
//
// void MakeModifier()
//

//
// Rule 59:  ClassDeclaration ::= Modifiersopt class Identifier Marker Superopt Interfacesopt ClassBody
//
#line 1274 "java.g"
void Parser::Act59()
{
    AstClassDeclaration* p = ast_pool -> NewClassDeclaration();
    if (Sym(1))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(1));
        p -> AllocateClassModifiers(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddClassModifier(DYNAMIC_CAST<AstModifier*>
                                  (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> class_token = Token(2);
    p -> identifier_token = Token(3);
    p -> super_opt = DYNAMIC_CAST<AstTypeName*> (Sym(5));
    if (Sym(6))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(6));
        p -> AllocateInterfaces(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddInterface(DYNAMIC_CAST<AstTypeName*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> class_body = DYNAMIC_CAST<AstClassBody*> (Sym(7));
    Sym(1) = p;
}

//
// Rule 60:  Super ::= extends ClassOrInterfaceType
//
// void SetSym1ToSym2();
//

//
// Rule 61:  Interfaces ::= implements TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 62:  TypeList ::= ClassOrInterfaceType
//
// void StartList();
//

//
// Rule 63:  TypeList ::= TypeList COMMA ClassOrInterfaceType
//
// void AddList3();
//

//
// Rule 64:  ClassBody ::= LBRACE ClassBodyDeclarationsopt RBRACE
//
#line 1347 "java.g"
void Parser::MakeClassBody()
{
    AstClassBody* p = ast_pool -> NewClassBody();
    if (parse_header_only)
        p -> MarkUnparsed();

    p -> left_brace_token = Token(1);
    if (Sym(2))
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

        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(2));
        p -> AllocateClassBodyDeclarations(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddClassBodyDeclaration(root -> element);

            AstFieldDeclaration* field_declaration =
                root -> element -> FieldDeclarationCast();
            if (field_declaration)
            {
                for (int i = 0; i < field_declaration -> NumVariableModifiers(); i++)
                {
                    if (field_declaration -> VariableModifier(i) -> class_tag == Ast::STATIC)
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
                num_methods++;
            else if (root -> element -> ConstructorDeclarationCast())
                num_constructors++;
            else if (root -> element -> StaticInitializerCast())
                num_static_initializers++;
            else if (root -> element -> ClassDeclarationCast())
                num_inner_classes++;
            else if (root -> element -> InterfaceDeclarationCast())
                num_inner_interfaces++;
            else if (root -> element -> BlockCast())
                num_blocks++;
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
            AstFieldDeclaration* field_declaration =
                root -> element -> FieldDeclarationCast();
            AstMethodDeclaration* method_declaration =
                root -> element -> MethodDeclarationCast();
            AstConstructorDeclaration* constructor_declaration =
                root -> element -> ConstructorDeclarationCast();
            AstStaticInitializer* static_initializer =
                root -> element -> StaticInitializerCast();
            AstClassDeclaration* class_declaration =
                root -> element -> ClassDeclarationCast();
            AstInterfaceDeclaration* interface_declaration =
                root -> element -> InterfaceDeclarationCast();
            AstMethodBody* block = root -> element -> MethodBodyCast();

            if (field_declaration)
            {
                if (field_declaration -> StaticFieldCast())
                    p -> AddClassVariable(field_declaration);
                else p -> AddInstanceVariable(field_declaration);
            }
            else if (method_declaration)
                p -> AddMethod(method_declaration);
            else if (constructor_declaration)
                p -> AddConstructor(constructor_declaration);
            else if (static_initializer)
                p -> AddStaticInitializer(static_initializer);
            else if (class_declaration)
                p -> AddNestedClass(class_declaration);
            else if (interface_declaration)
                p -> AddNestedInterface(interface_declaration);
            else if (block)
                p -> AddInstanceInitializer(block);
            else
            {
                p -> AddEmptyDeclaration(DYNAMIC_CAST<AstEmptyDeclaration*>
                                         (root -> element));
            }
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(3);
    // from now on, this is the storage pool to use for this type
    p -> pool = body_pool;
    Sym(1) = p;
}

//
// Rule 65:  ClassBodyDeclarations ::= ClassBodyDeclaration
//
// void StartList();
//

//
// Rule 66:  ClassBodyDeclarations ::= ClassBodyDeclarations ClassBodyDeclaration
//
// void AddList2();
//

//
// Rule 67:  ClassBodyDeclaration ::= ClassMemberDeclaration
//
// void NoAction();
//

//
// Rule 68:  ClassBodyDeclaration ::= ConstructorDeclaration
//
// void NoAction();
//

//
// Rule 69:  ClassBodyDeclaration ::= StaticInitializer
//
// void NoAction();
//

//
// Rule 70:  ClassBodyDeclaration ::= MethodHeaderMarker MethodBody
//
// void SetSym1ToSym2();
//

//
// Rule 71:  ClassMemberDeclaration ::= FieldDeclaration
//
// void NoAction();
//

//
// Rule 72:  ClassMemberDeclaration ::= MethodDeclaration
//
// void NoAction();
//

//
// Rule 73:  ClassMemberDeclaration ::= TypeDeclaration
//
// void NoAction();
//

//
// Rule 74:  FieldDeclaration ::= Modifiersopt Marker Type VariableDeclarators SEMICOLON
//
#line 1522 "java.g"
void Parser::Act74()
{
    AstFieldDeclaration* p = ast_pool -> NewFieldDeclaration();
    if (Sym(1))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(1));
        p -> AllocateVariableModifiers(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableModifier(DYNAMIC_CAST<AstModifier*>
                                     (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> type = DYNAMIC_CAST<AstType*> (Sym(3));
    AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(4));
    p -> AllocateVariableDeclarators(tail -> index + 1);
    AstListNode* root = tail;
    do
    {
        root = root -> next;
        p -> AddVariableDeclarator(DYNAMIC_CAST<AstVariableDeclarator*>
                                   (root -> element));
    } while (root != tail);
    FreeCircularList(tail);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 75:  VariableDeclarators ::= VariableDeclarator
//
// void StartList();
//

//
// Rule 76:  VariableDeclarators ::= VariableDeclarators COMMA VariableDeclarator
//
// void AddList3();
//

//
// Rule 77:  VariableDeclarator ::= VariableDeclaratorId
//
#line 1565 "java.g"
void Parser::Act77()
{
    AstVariableDeclarator* p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name =
        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(1));
    Sym(1) = p;
}

//
// Rule 78:  VariableDeclarator ::= VariableDeclaratorId EQUAL VariableInitializer
//
#line 1577 "java.g"
void Parser::Act78()
{
    AstVariableDeclarator* p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name =
        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(1));
    p -> variable_initializer_opt = Sym(3);
    Sym(1) = p;
}

//
// Rule 79:  VariableDeclaratorId ::= Identifier Dimsopt
//
#line 1590 "java.g"
void Parser::Act79()
{
    AstVariableDeclaratorId* p = ast_pool -> NewVariableDeclaratorId();
    p -> identifier_token = Token(1);
    if (Sym(2))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(2));
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets(DYNAMIC_CAST<AstBrackets*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 80:  VariableInitializer ::= Expression
//
// void NoAction();
//

//
// Rule 81:  VariableInitializer ::= ArrayInitializer
//
// void NoAction();
//

//
// Rule 82:  MethodDeclaration ::= MethodHeader MethodHeaderMarker MethodBody
//
#line 1633 "java.g"
void Parser::MakeMethodDeclaration()
{
    DYNAMIC_CAST<AstMethodDeclaration*> (Sym(1)) -> method_body =
        DYNAMIC_CAST<AstStatement*> (Sym(3));
}

//
// Rule 83:  MethodDeclaration ::= MethodHeader MethodHeaderMarker EmptyStatement
//
// void MakeMethodDeclaration();
//

//
// Rule 84:  MethodHeader ::= Modifiersopt Marker Type MethodDeclarator Throwsopt
//
#line 1654 "java.g"
void Parser::MakeMethodHeader()
{
    AstMethodDeclaration* p = ast_pool -> NewMethodDeclaration();
    if (Sym(1))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(1));
        p -> AllocateMethodModifiers(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddMethodModifier(DYNAMIC_CAST<AstModifier*>
                                   (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> type = DYNAMIC_CAST<AstType*> (Sym(3));
    p -> method_declarator = DYNAMIC_CAST<AstMethodDeclarator*> (Sym(4));
    if (Sym(5))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(5));
        p -> AllocateThrows(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddThrow(DYNAMIC_CAST<AstTypeName*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 85:  MethodHeader ::= Modifiersopt Marker void MethodDeclarator Throwsopt
//
#line 1695 "java.g"
void Parser::Act85()
{
    Sym(3) = ast_pool -> NewPrimitiveType(Ast::VOID_TYPE, Token(3));
    MakeMethodHeader();
}

//
// Rule 86:  MethodDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN Dimsopt
//
#line 1705 "java.g"
void Parser::MakeMethodDeclarator()
{
    AstMethodDeclarator* p = ast_pool -> NewMethodDeclarator();
    p -> identifier_token = Token(1);
    p -> left_parenthesis_token = Token(2);
    if (Sym(3))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(3));
        p -> AllocateFormalParameters(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddFormalParameter(DYNAMIC_CAST<AstFormalParameter*>
                                    (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    if (Sym(5))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(5));
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets(DYNAMIC_CAST<AstBrackets*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 87:  FormalParameterList ::= FormalParameter
//
// void StartList();
//

//
// Rule 88:  FormalParameterList ::= FormalParameterList COMMA FormalParameter
//
// void AddList3();
//

//
// Rule 89:  FormalParameter ::= Modifiersopt Type VariableDeclaratorId
//
#line 1756 "java.g"
void Parser::Act89()
{
    AstFormalParameter* p = ast_pool -> NewFormalParameter();
    if (Sym(1))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(1));
        p -> AllocateParameterModifiers(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddParameterModifier(DYNAMIC_CAST<AstModifier*>
                                      (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> type = DYNAMIC_CAST<AstType*> (Sym(2));

    AstVariableDeclarator* formal_declarator =
        ast_pool -> NewVariableDeclarator();
    formal_declarator -> variable_declarator_name =
        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(3));

    p -> formal_declarator = formal_declarator;
    Sym(1) = p;
}

//
// Rule 90:  Throws ::= throws TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 91:  MethodBody ::= LBRACE BlockStatementsopt RBRACE
//
#line 1804 "java.g"
void Parser::Act91()
{
    AstMethodBody* p = ast_pool -> NewMethodBody();
    p -> left_brace_token = Token(1);
    p -> right_brace_token = Token(3);

    if (Sym(2))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(2));
        p -> AllocateStatements(tail -> index + 1);
        AstListNode* root = tail -> next;
        if (root -> element -> IsExplicitConstructorInvocation())
            p -> explicit_constructor_opt =
                DYNAMIC_CAST<AstStatement*> (root -> element);
        else
            p -> AddStatement(DYNAMIC_CAST<AstStatement*> (root -> element));
        while (root != tail)
        {
            root = root -> next;
            p -> AddStatement(DYNAMIC_CAST<AstStatement*> (root -> element));
        }
        FreeCircularList(tail);
    }

    Sym(1) = p;
}

//
// Rule 92:  StaticInitializer ::= static MethodHeaderMarker MethodBody
//
#line 1843 "java.g"
void Parser::Act92()
{
    AstStaticInitializer* p = ast_pool -> NewStaticInitializer();
    p -> static_token = Token(1);
    p -> block = DYNAMIC_CAST<AstMethodBody*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 93:  ConstructorDeclaration ::= Modifiersopt Marker ConstructorDeclarator Throwsopt MethodHeaderMarker MethodBody
//
#line 1864 "java.g"
void Parser::MakeConstructorDeclaration()
{
    AstConstructorDeclaration* p = ast_pool -> NewConstructorDeclaration();
    if (Sym(1))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(1));
        p -> AllocateConstructorModifiers(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddConstructorModifier(DYNAMIC_CAST<AstModifier*>
                                        (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> constructor_declarator = DYNAMIC_CAST<AstMethodDeclarator*> (Sym(3));
    if (Sym(4))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(4));
        p -> AllocateThrows(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddThrow(DYNAMIC_CAST<AstTypeName*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> constructor_body = DYNAMIC_CAST<AstMethodBody*> (Sym(6));

    Sym(1) = p;
}

//
// Rule 94:  ConstructorDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN Marker
//
// void MakeMethodDeclarator();
//

//
// Rule 95:  ExplicitConstructorInvocation ::= this LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 1923 "java.g"
void Parser::Act95()
{
    AstThisCall* p = ast_pool -> NewThisCall();
    p -> this_token = Token(1);
    p -> left_parenthesis_token = Token(2);
    if (Sym(3))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(3));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 96:  ExplicitConstructorInvocation ::= super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 1949 "java.g"
void Parser::Act96()
{
    AstSuperCall* p = ast_pool -> NewSuperCall();
    p -> super_token = Token(1);
    p -> left_parenthesis_token = Token(2);
    if (Sym(3))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(3));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 97:  ExplicitConstructorInvocation ::= Primary DOT super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 1977 "java.g"
void Parser::MakeQualifiedSuper()
{
    AstSuperCall* p = ast_pool -> NewSuperCall();
    p -> base_opt = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> super_token = Token(3);
    p -> left_parenthesis_token = Token(4);
    if (Sym(5))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(5));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(6);
    p -> semicolon_token = Token(7);
    Sym(1) = p;
}

//
// Rule 98:  ExplicitConstructorInvocation ::= Name DOT super LPAREN ArgumentListopt RPAREN SEMICOLON
//
// void MakeQualifiedSuper();
//

//
// Rule 99:  InterfaceDeclaration ::= Modifiersopt interface Identifier Marker ExtendsInterfacesopt InterfaceBody
//
#line 2020 "java.g"
void Parser::Act99()
{
    AstInterfaceDeclaration* p =
        DYNAMIC_CAST<AstInterfaceDeclaration*> (Sym(6));
    if (Sym(1))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(1));
        p -> AllocateInterfaceModifiers(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddInterfaceModifier(DYNAMIC_CAST<AstModifier*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> interface_token = Token(2);
    p -> identifier_token = Token(3);
    if (Sym(5))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(5));
        p -> AllocateExtendsInterfaces(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddExtendsInterface(DYNAMIC_CAST<AstTypeName*>
                                     (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 100:  ExtendsInterfaces ::= extends TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 101:  InterfaceBody ::= LBRACE InterfaceMemberDeclarationsopt RBRACE
//
#line 2069 "java.g"
void Parser::Act101()
{
    AstInterfaceDeclaration* p = ast_pool -> NewInterfaceDeclaration();
    if (parse_header_only)
        p -> MarkUnparsed();

    p -> left_brace_token = Token(1);
    if (Sym(2))
    {
        int num_class_variables = 0,
            num_methods = 0,
            num_inner_classes = 0,
            num_inner_interfaces = 0,
            num_empty_declarations = 0;

        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(2));
        p -> AllocateInterfaceMemberDeclarations(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddInterfaceMemberDeclaration(root -> element);

            AstFieldDeclaration* field_declaration = root -> element -> FieldDeclarationCast();
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

            AstFieldDeclaration* field_declaration;
            AstMethodDeclaration* method_declaration;
            AstClassDeclaration* class_declaration;
            AstInterfaceDeclaration* interface_declaration;

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
                p -> AddEmptyDeclaration(DYNAMIC_CAST<AstEmptyDeclaration*> (root -> element));
            }
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(3);
    p -> pool = body_pool; // from now on, this is the storage pool to use for this type
    Sym(1) = p;
}

//
// Rule 102:  InterfaceMemberDeclarations ::= InterfaceMemberDeclaration
//
#line 2161 "java.g"
void Parser::Act102()
{
    AstFieldDeclaration* field = Sym(1) -> FieldDeclarationCast();
    if (field)
        field -> MarkStatic();
    StartList();
}

//
// Rule 103:  InterfaceMemberDeclarations ::= InterfaceMemberDeclarations InterfaceMemberDeclaration
//
#line 2174 "java.g"
void Parser::Act103()
{
    AstFieldDeclaration* field = Sym(2) -> FieldDeclarationCast();
    if (field)
        field -> MarkStatic();
    AddList2();
}

//
// Rule 104:  InterfaceMemberDeclaration ::= ConstantDeclaration
//
// void NoAction();
//

//
// Rule 105:  InterfaceMemberDeclaration ::= AbstractMethodDeclaration
//
// void NoAction();
//

//
// Rule 106:  InterfaceMemberDeclaration ::= ClassDeclaration
//
// void NoAction();
//

//
// Rule 107:  InterfaceMemberDeclaration ::= InterfaceDeclaration
//
// void NoAction();
//

//
// Rule 108:  InterfaceMemberDeclaration ::= SEMICOLON
//
#line 2204 "java.g"
void Parser::Act108()
{
    Sym(1) = ast_pool -> NewEmptyDeclaration(Token(1));
}

//
// Rule 109:  ConstantDeclaration ::= FieldDeclaration
//
// void NoAction();
//

//
// Rule 110:  AbstractMethodDeclaration ::= MethodHeader SEMICOLON
//
#line 2217 "java.g"
void Parser::Act110()
{
    DYNAMIC_CAST<AstMethodDeclaration*> (Sym(1)) -> method_body =
        ast_pool -> NewEmptyStatement(Token(2));
}

//
// Rule 111:  ArrayInitializer ::= LBRACE Marker ,opt RBRACE
//
#line 2233 "java.g"
void Parser::MakeArrayInitializer()
{
    AstArrayInitializer* p = ast_pool -> NewArrayInitializer();
    p -> left_brace_token = Token(1);
    if (Sym(2))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(2));
        p -> AllocateVariableInitializers(tail -> index + 1);
        AstListNode* root = tail;
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
// Rule 112:  ArrayInitializer ::= LBRACE VariableInitializers ,opt RBRACE
//
// void MakeArrayInitializer();
//

//
// Rule 113:  VariableInitializers ::= VariableInitializer
//
// void StartList();
//

//
// Rule 114:  VariableInitializers ::= VariableInitializers COMMA VariableInitializer
//
// void AddList3();
//

//
// Rule 115:  Block ::= LBRACE BlockStatementsopt RBRACE
//
#line 2274 "java.g"
void Parser::Act115()
{
    AstBlock* p = ast_pool -> NewBlock();
    p -> left_brace_token = Token(1);
    if (Sym(2))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(2));
        p -> AllocateStatements(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddStatement(DYNAMIC_CAST<AstStatement*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(3);
    Sym(1) = p;
}

//
// Rule 116:  BlockStatements ::= BlockStatement
//
// void StartList();
//

//
// Rule 117:  BlockStatements ::= BlockStatements BlockStatement
//
// void AddList2();
//

//
// Rule 118:  BlockStatement ::= LocalVariableDeclarationStatement
//
// void NoAction();
//

//
// Rule 119:  BlockStatement ::= Statement
//
// void NoAction();
//

//
// Rule 120:  BlockStatement ::= ClassDeclaration
//
#line 2315 "java.g"
void Parser::Act120()
{
    Sym(1) = ast_pool ->
        NewLocalClassDeclarationStatement(DYNAMIC_CAST<AstClassDeclaration*>
                                          (Sym(1)));
}

//
// Rule 121:  BlockStatement ::= ExplicitConstructorInvocation
//
// void NoAction();
//

//
// Rule 122:  LocalVariableDeclarationStatement ::= LocalVariableDeclaration SEMICOLON
//
#line 2335 "java.g"
void Parser::Act122()
{
    DYNAMIC_CAST<AstLocalVariableDeclarationStatement*> (Sym(1)) ->
        semicolon_token_opt = Token(2);
}

//
// Rule 123:  LocalVariableDeclaration ::= PrimitiveType Dimsopt VariableDeclarators
//
#line 2349 "java.g"
void Parser::MakeLocalVariable()
{
    MakeLocalVariable(NULL, MakeArrayType(1),
                      DYNAMIC_CAST<AstListNode*> (Sym(3)));
}

//
// Creates a local variable declaration and places it in Sym(1).
//
void Parser::MakeLocalVariable(AstListNode* modifiers, AstType* type,
                               AstListNode* variables)
{
    AstLocalVariableDeclarationStatement* p =
        ast_pool -> NewLocalVariableDeclarationStatement();
    if (modifiers)
    {
        AstListNode* tail = modifiers;
        p -> AllocateLocalModifiers(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddLocalModifier(DYNAMIC_CAST<AstModifier*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> type = type;
    AstListNode* tail = variables;
    p -> AllocateVariableDeclarators(tail -> index + 1);
    AstListNode* root = tail;
    do
    {
        root = root -> next;
        p -> AddVariableDeclarator(DYNAMIC_CAST<AstVariableDeclarator*>
                                   (root -> element));
    } while (root != tail);
    FreeCircularList(tail);
    Sym(1) = p;
}

//
// Rule 124:  LocalVariableDeclaration ::= Name Marker VariableDeclarators
//
// void MakeLocalVariable();
//

//
// Rule 125:  LocalVariableDeclaration ::= Name Dims VariableDeclarators
//
// void MakeLocalVariable();
//

//
// Rule 126:  LocalVariableDeclaration ::= Modifiers Type VariableDeclarators
//
#line 2417 "java.g"
void Parser::Act126()
{
    MakeLocalVariable(DYNAMIC_CAST<AstListNode*> (Sym(1)),
                      DYNAMIC_CAST<AstType*> (Sym(2)),
                      DYNAMIC_CAST<AstListNode*> (Sym(3)));
}

//
// Rule 127:  Statement ::= StatementWithoutTrailingSubstatement
//
// void NoAction();
//

//
// Rule 128:  Statement ::= LabeledStatement
//
// void NoAction();
//

//
// Rule 129:  Statement ::= IfThenStatement
//
// void NoAction();
//

//
// Rule 130:  Statement ::= IfThenElseStatement
//
// void NoAction();
//

//
// Rule 131:  Statement ::= WhileStatement
//
// void NoAction();
//

//
// Rule 132:  Statement ::= ForStatement
//
// void NoAction();
//

//
// Rule 133:  StatementNoShortIf ::= StatementWithoutTrailingSubstatement
//
// void NoAction();
//

//
// Rule 134:  StatementNoShortIf ::= LabeledStatementNoShortIf
//
// void NoAction();
//

//
// Rule 135:  StatementNoShortIf ::= IfThenElseStatementNoShortIf
//
// void NoAction();
//

//
// Rule 136:  StatementNoShortIf ::= WhileStatementNoShortIf
//
// void NoAction();
//

//
// Rule 137:  StatementNoShortIf ::= ForStatementNoShortIf
//
// void NoAction();
//

//
// Rule 138:  StatementWithoutTrailingSubstatement ::= Block
//
// void NoAction();
//

//
// Rule 139:  StatementWithoutTrailingSubstatement ::= EmptyStatement
//
// void NoAction();
//

//
// Rule 140:  StatementWithoutTrailingSubstatement ::= ExpressionStatement
//
// void NoAction();
//

//
// Rule 141:  StatementWithoutTrailingSubstatement ::= SwitchStatement
//
// void NoAction();
//

//
// Rule 142:  StatementWithoutTrailingSubstatement ::= DoStatement
//
// void NoAction();
//

//
// Rule 143:  StatementWithoutTrailingSubstatement ::= BreakStatement
//
// void NoAction();
//

//
// Rule 144:  StatementWithoutTrailingSubstatement ::= ContinueStatement
//
// void NoAction();
//

//
// Rule 145:  StatementWithoutTrailingSubstatement ::= ReturnStatement
//
// void NoAction();
//

//
// Rule 146:  StatementWithoutTrailingSubstatement ::= SynchronizedStatement
//
// void NoAction();
//

//
// Rule 147:  StatementWithoutTrailingSubstatement ::= ThrowStatement
//
// void NoAction();
//

//
// Rule 148:  StatementWithoutTrailingSubstatement ::= TryStatement
//
// void NoAction();
//

//
// Rule 149:  StatementWithoutTrailingSubstatement ::= AssertStatement
//
// void NoAction();
//

//
// Rule 150:  EmptyStatement ::= SEMICOLON
//
#line 2520 "java.g"
void Parser::Act150()
{
    Sym(1) = ast_pool -> NewEmptyStatement(Token(1));
}

//
// Rule 151:  LabeledStatement ::= Identifier COLON Statement
//
#line 2529 "java.g"
void Parser::MakeLabeledStatement()
{
    AstBlock* p = Sym(3) -> BlockCast();

    if (! p || p -> label_opt)
    {
        //
        // When a statement is labeled, it is enclosed in a block.
        // This is necessary in order to allow the same name to be
        // reused to label a subsequent statement at the same nesting
        // level... See ProcessBlock, ProcessStatement,...
        //
        p = ast_pool -> GenBlock();
        p -> AllocateStatements(1); // allocate 1 element
        p -> left_brace_token = Token(1);
        p -> AddStatement(DYNAMIC_CAST<AstStatement*> (Sym(3)));
        p -> right_brace_token = Sym(3) -> RightToken();
    }

    p -> label_opt = Token(1); // add label to statement
    Sym(1) = p; // The final result is a block containing the labeled-statement
}

//
// Rule 152:  LabeledStatementNoShortIf ::= Identifier COLON StatementNoShortIf
//
// void MakeLabeledStatement();
//

//
// Rule 153:  ExpressionStatement ::= StatementExpression SEMICOLON
//
#line 2563 "java.g"
void Parser::Act153()
{
    DYNAMIC_CAST<AstExpressionStatement*> (Sym(1)) -> semicolon_token_opt = Token(2);
}

//
// Rule 154:  StatementExpression ::= Assignment
//
#line 2572 "java.g"
void Parser::MakeExpressionStatement()
{
    AstExpressionStatement* p = ast_pool -> NewExpressionStatement();
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    Sym(1) = p;
}

//
// Rule 155:  StatementExpression ::= PreIncrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 156:  StatementExpression ::= PreDecrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 157:  StatementExpression ::= PostIncrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 158:  StatementExpression ::= PostDecrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 159:  StatementExpression ::= MethodInvocation
//
// void MakeExpressionStatement();
//

//
// Rule 160:  StatementExpression ::= ClassInstanceCreationExpression
//
// void MakeExpressionStatement();
//

//
// Rule 161:  IfThenStatement ::= if LPAREN Expression RPAREN Statement Marker Marker
//
#line 2629 "java.g"
void Parser::MakeIfThenElseStatement()
{
    //
    // We wrap the true and false statements in a block, to make the semantic
    // pass easier.
    //
    AstBlock* true_block = Sym(5) -> BlockCast();
    if (! true_block)
    {
        true_block = ast_pool -> GenBlock();
        true_block -> AllocateStatements(1); // allocate 1 element
        true_block -> left_brace_token = Token(5);
        true_block -> AddStatement(DYNAMIC_CAST<AstStatement*> (Sym(5)));
        true_block -> right_brace_token = Sym(5) -> RightToken();
    }

    AstBlock* false_block = NULL;
    if (Sym(7))
    {
        false_block = Sym(7) -> BlockCast();
        if (! false_block)
        {
            false_block = ast_pool -> GenBlock();
            false_block -> AllocateStatements(1); // allocate 1 element
            false_block -> left_brace_token = Token(7);
            false_block -> AddStatement(DYNAMIC_CAST<AstStatement*> (Sym(7)));
            false_block -> right_brace_token = Sym(7) -> RightToken();
        }
    }

    AstIfStatement* p = ast_pool -> NewIfStatement();
    p -> if_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    p -> true_statement = true_block;
    p -> false_statement_opt = false_block;
    Sym(1) = p;
}

//
// Rule 162:  IfThenElseStatement ::= if LPAREN Expression RPAREN StatementNoShortIf else Statement
//
// void MakeIfThenElseStatement();
//

//
// Rule 163:  IfThenElseStatementNoShortIf ::= if LPAREN Expression RPAREN StatementNoShortIf else StatementNoShortIf
//
// void MakeIfThenElseStatement();
//

//
// Rule 164:  SwitchStatement ::= switch LPAREN Expression RPAREN SwitchBlock
//
#line 2687 "java.g"
void Parser::Act164()
{
    AstSwitchStatement* p = DYNAMIC_CAST<AstSwitchStatement*> (Sym(5));
    p -> switch_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 165:  SwitchBlock ::= LBRACE SwitchBlockStatements SwitchLabelsopt RBRACE
//
#line 2705 "java.g"
void Parser::Act165()
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
// Rule 166:  SwitchBlock ::= LBRACE SwitchLabelsopt RBRACE
//
#line 2732 "java.g"
void Parser::Act166()
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
// Rule 167:  SwitchBlockStatements ::= SwitchBlockStatement
//
// void StartList();
//

//
// Rule 168:  SwitchBlockStatements ::= SwitchBlockStatements SwitchBlockStatement
//
// void AddList2();
//

//
// Rule 169:  SwitchBlockStatement ::= SwitchLabels BlockStatements
//
#line 2761 "java.g"
void Parser::Act169()
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
// Rule 170:  SwitchLabels ::= SwitchLabel
//
// void StartList();
//

//
// Rule 171:  SwitchLabels ::= SwitchLabels SwitchLabel
//
// void AddList2();
//

//
// Rule 172:  SwitchLabel ::= case Expression COLON
//
#line 2816 "java.g"
void Parser::MakeSwitchLabel()
{
    AstSwitchLabel* p = ast_pool -> NewSwitchLabel();
    p -> case_token = Token(1);
    p -> expression_opt = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> colon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 173:  SwitchLabel ::= default Marker COLON
//
// void MakeSwitchLabel();
//

//
// Rule 174:  WhileStatement ::= while LPAREN Expression RPAREN Statement
//
#line 2840 "java.g"
void Parser::MakeWhileStatement()
{
    AstWhileStatement* p = ast_pool -> NewWhileStatement();
    p -> while_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    p -> statement = DYNAMIC_CAST<AstStatement*> (Sym(5));

    //
    // We wrap the loop in a block, to make the semantic pass easier.
    //
    AstBlock* block = ast_pool -> GenBlock();
    block -> AllocateStatements(1); // allocate 1 element
    block -> left_brace_token = Token(1);
    block -> AddStatement(p);
    block -> right_brace_token = Sym(5) -> RightToken();
    Sym(1) = block;
}

//
// Rule 175:  WhileStatementNoShortIf ::= while LPAREN Expression RPAREN StatementNoShortIf
//
// void MakeWhileStatement();
//

//
// Rule 176:  DoStatement ::= do Statement while LPAREN Expression RPAREN SEMICOLON
//
#line 2869 "java.g"
void Parser::Act176()
{
    AstDoStatement* p = ast_pool -> NewDoStatement();
    p -> do_token = Token(1);
    p -> statement = DYNAMIC_CAST<AstStatement*> (Sym(2));
    p -> while_token = Token(3);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(5));
    p -> semicolon_token = Token(7);

    //
    // We wrap the loop in a block, to make the semantic pass easier.
    //
    AstBlock* block = ast_pool -> GenBlock();
    block -> AllocateStatements(1); // allocate 1 element
    block -> left_brace_token = Token(1);
    block -> AddStatement(p);
    block -> right_brace_token = Token(7);
    Sym(1) = block;
}

//
// Rule 177:  ForStatement ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN Statement
//
#line 2894 "java.g"
void Parser::MakeForStatement()
{
    AstForStatement* p = ast_pool -> NewForStatement();
    p -> for_token = Token(1);
    if (Sym(3))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(3));
        p -> AllocateForInitStatements(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddForInitStatement(DYNAMIC_CAST<AstStatement*>
                                     (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> end_expression_opt = DYNAMIC_CAST<AstExpression*> (Sym(5));
    if (Sym(7))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(7));
        p -> AllocateForUpdateStatements(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddForUpdateStatement(DYNAMIC_CAST<AstExpressionStatement*>
                                       (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> statement = DYNAMIC_CAST<AstStatement*> (Sym(9));

    //
    // We wrap the loop in a block, to make the semantic pass easier. In
    // particular, this lets us correctly handle "for(int i;;);for(int i;;);".
    //
    AstBlock* block = ast_pool -> NewBlock();
    block -> AllocateStatements(1); // allocate 1 element
    block -> left_brace_token = Token(1);
    block -> AddStatement(p);
    block -> right_brace_token = Sym(9) -> RightToken();
    block -> no_braces = true;
    Sym(1) = block;
}

//
// Rule 178:  ForStatementNoShortIf ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN...
//
// void MakeForStatement();
//

//
// Rule 179:  ForInit ::= StatementExpressionList
//
// void NoAction();
//

//
// Rule 180:  ForInit ::= LocalVariableDeclaration
//
// void StartList();
//

//
// Rule 181:  ForUpdate ::= StatementExpressionList
//
// void NoAction();
//

//
// Rule 182:  StatementExpressionList ::= StatementExpression
//
// void StartList();
//

//
// Rule 183:  StatementExpressionList ::= StatementExpressionList COMMA StatementExpression
//
// void AddList3();
//

//
// Rule 184:  AssertStatement ::= assert Expression Marker Marker SEMICOLON
//
#line 2976 "java.g"
void Parser::MakeAssertStatement()
{
    AstAssertStatement* p = ast_pool -> NewAssertStatement();
    p -> assert_token = Token(1);
    p -> condition = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> message_opt = DYNAMIC_CAST<AstExpression*> (Sym(4));
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 185:  AssertStatement ::= assert Expression COLON Expression SEMICOLON
//
// void MakeAssertStatement();
//

//
// Rule 186:  BreakStatement ::= break Identifieropt SEMICOLON
//
#line 2997 "java.g"
void Parser::Act186()
{
    AstBreakStatement* p = ast_pool -> NewBreakStatement();
    p -> break_token = Token(1);
    p -> identifier_token_opt = Token(2) == Token(3) ? 0 : Token(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 187:  ContinueStatement ::= continue Identifieropt SEMICOLON
//
#line 3010 "java.g"
void Parser::Act187()
{
    AstContinueStatement* p = ast_pool -> NewContinueStatement();
    p -> continue_token = Token(1);
    p -> identifier_token_opt = Token(2) == Token(3) ? 0 : Token(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 188:  ReturnStatement ::= return Expressionopt SEMICOLON
//
#line 3023 "java.g"
void Parser::Act188()
{
    AstReturnStatement* p = ast_pool -> NewReturnStatement();
    p -> return_token = Token(1);
    p -> expression_opt = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 189:  ThrowStatement ::= throw Expression SEMICOLON
//
#line 3036 "java.g"
void Parser::Act189()
{
    AstThrowStatement* p = ast_pool -> NewThrowStatement();
    p -> throw_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 190:  SynchronizedStatement ::= synchronized LPAREN Expression RPAREN Block
//
#line 3049 "java.g"
void Parser::Act190()
{
    AstSynchronizedStatement* p = ast_pool -> NewSynchronizedStatement();
    p -> synchronized_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(5));
    p -> block -> block_tag = AstBlock::SYNCHRONIZED;

    Sym(1) = p;
}

//
// Rule 191:  TryStatement ::= try Block Catches Marker
//
#line 3068 "java.g"
void Parser::MakeTryStatement()
{
    AstTryStatement* p = ast_pool -> NewTryStatement();
    p -> try_token = Token(1);
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(2));
    if (Sym(3))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(3));
        p -> AllocateCatchClauses(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddCatchClause(DYNAMIC_CAST<AstCatchClause*>
                                (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    if (Sym(4))
    {
        p -> block -> block_tag = AstBlock::TRY_CLAUSE_WITH_FINALLY;
        for (unsigned i = 0; i < p -> NumCatchClauses(); i++)
            p -> CatchClause(i) -> block -> block_tag =
                AstBlock::TRY_CLAUSE_WITH_FINALLY;
        p -> finally_clause_opt = DYNAMIC_CAST<AstFinallyClause*> (Sym(4));
    }
    Sym(1) = p;
}

//
// Rule 192:  TryStatement ::= try Block Catchesopt Finally
//
// void MakeTryStatement();
//

//
// Rule 193:  Catches ::= CatchClause
//
// void StartList();
//

//
// Rule 194:  Catches ::= Catches CatchClause
//
// void AddList2();
//

//
// Rule 195:  CatchClause ::= catch LPAREN FormalParameter RPAREN Block
//
#line 3116 "java.g"
void Parser::Act195()
{
    AstCatchClause* p = ast_pool -> NewCatchClause();
    p -> catch_token = Token(1);
    p -> formal_parameter = DYNAMIC_CAST<AstFormalParameter*> (Sym(3));
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(5));

    Sym(1) = p;
}

//
// Rule 196:  Finally ::= finally Block
//
#line 3130 "java.g"
void Parser::Act196()
{
    AstFinallyClause* p = ast_pool -> NewFinallyClause();
    p -> finally_token = Token(1);
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(2));
    p -> block -> block_tag = AstBlock::FINALLY;

    Sym(1) = p;
}

//
// Rule 197:  Primary ::= PrimaryNoNewArray
//
// void NoAction();
//

//
// Rule 198:  Primary ::= ArrayCreationUninitialized
//
// void NoAction();
//

//
// Rule 199:  Primary ::= ArrayCreationInitialized
//
// void NoAction();
//

//
// Rule 200:  PrimaryNoNewArray ::= Literal
//
// void NoAction();
//

//
// Rule 201:  PrimaryNoNewArray ::= this
//
#line 3169 "java.g"
void Parser::Act201()
{
    Sym(1) = ast_pool -> NewThisExpression(Token(1));
}

//
// Rule 202:  PrimaryNoNewArray ::= LPAREN Expression RPAREN
//
#line 3178 "java.g"
void Parser::MakeParenthesizedExpression()
{
    AstParenthesizedExpression* p = ast_pool -> NewParenthesizedExpression();
    p -> left_parenthesis_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> right_parenthesis_token = Token(3);
    Sym(1) = p;
}

//
// Rule 203:  PrimaryNoNewArray ::= ClassInstanceCreationExpression
//
// void NoAction();
//

//
// Rule 204:  PrimaryNoNewArray ::= FieldAccess
//
// void NoAction();
//

//
// Rule 205:  PrimaryNoNewArray ::= Name DOT this
//
#line 3203 "java.g"
void Parser::Act205()
{
    AstThisExpression* p = ast_pool -> NewThisExpression(Token(3));
    p -> base_opt = ast_pool -> NewTypeName(DYNAMIC_CAST<AstExpression*> (Sym(1)));
    Sym(1) = p;
}

//
// Rule 206:  PrimaryNoNewArray ::= PrimitiveType Dimsopt DOT class
//
#line 3218 "java.g"
void Parser::MakeClassLiteral()
{
    AstClassLiteral* p = ast_pool -> NewClassLiteral(Token(4));
    p -> type = MakeArrayType(1);
    Sym(1) = p;
}

//
// Rule 207:  PrimaryNoNewArray ::= Name Dims DOT class
//
// void MakeClassLiteral();
//

//
// Rule 208:  PrimaryNoNewArray ::= Name DOT Marker class
//
#line 3239 "java.g"
void Parser::Act208()
{
    Sym(2) = NULL;
    MakeClassLiteral();
}

//
// Rule 209:  PrimaryNoNewArray ::= void DOT Marker class
//
#line 3253 "java.g"
void Parser::Act209()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::VOID_TYPE, Token(1));
    Sym(2) = NULL;
    MakeClassLiteral();
}

//
// Rule 210:  PrimaryNoNewArray ::= MethodInvocation
//
// void NoAction();
//

//
// Rule 211:  PrimaryNoNewArray ::= ArrayAccess
//
// void NoAction();
//

//
// Rule 212:  ClassInstanceCreationExpression ::= new ClassOrInterfaceType LPAREN ArgumentListopt RPAREN ClassBodyopt
//
#line 3278 "java.g"
void Parser::Act212()
{
    AstClassInstanceCreationExpression* p =
        ast_pool -> NewClassInstanceCreationExpression();
    p -> new_token = Token(1);
    p -> class_type = DYNAMIC_CAST<AstTypeName*> (Sym(2));
    p -> left_parenthesis_token = Token(3);
    if (Sym(4))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(4));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(5);
    p -> class_body_opt = DYNAMIC_CAST<AstClassBody*> (Sym(6));
    Sym(1) = p;
}

//
// Rule 213:  ClassInstanceCreationExpression ::= Primary DOT new Identifier Marker LPAREN ArgumentListopt RPAREN...
//
#line 3314 "java.g"
void Parser::MakeQualifiedNew()
{
    AstClassInstanceCreationExpression* p =
        ast_pool -> NewClassInstanceCreationExpression();
    p -> base_opt = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> new_token = Token(3);
    p -> class_type = ast_pool -> NewTypeName(MakeSimpleName(4));
    p -> left_parenthesis_token = Token(6);
    if (Sym(7))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(7));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(8);
    p -> class_body_opt = DYNAMIC_CAST<AstClassBody*> (Sym(9));
    Sym(1) = p;
}

//
// Rule 214:  ClassInstanceCreationExpression ::= Name DOT new Identifier Marker LPAREN ArgumentListopt RPAREN ClassBodyopt
//
// void MakeQualifiedNew();
//

//
// Rule 215:  ArgumentList ::= Expression
//
// void StartList();
//

//
// Rule 216:  ArgumentList ::= ArgumentList COMMA Expression
//
// void AddList3();
//

//
// Rule 217:  ArrayCreationUninitialized ::= new PrimitiveType DimExprs Dimsopt
//
#line 3372 "java.g"
void Parser::MakeArrayCreationUninitialized()
{
    AstArrayCreationExpression* p = ast_pool -> NewArrayCreationExpression();
    p -> new_token = Token(1);
    p -> array_type = DYNAMIC_CAST<AstType*> (Sym(2));
    AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(3));
    p -> AllocateDimExprs(tail -> index + 1);
    AstListNode* root = tail;
    do
    {
        root = root -> next;
        p -> AddDimExpr(DYNAMIC_CAST<AstDimExpr*> (root -> element));
    } while (root != tail);
    FreeCircularList(tail);
    if (Sym(4))
    {
        tail = DYNAMIC_CAST<AstListNode*> (Sym(4));
        p -> AllocateBrackets(tail -> index + 1);
        root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets(DYNAMIC_CAST<AstBrackets*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 218:  ArrayCreationUninitialized ::= new ClassOrInterfaceType DimExprs Dimsopt
//
// void MakeArrayCreationUninitialized();
//

//
// Rule 219:  ArrayCreationInitialized ::= new ArrayType ArrayInitializer
//
#line 3416 "java.g"
void Parser::Act219()
{
    AstArrayCreationExpression* p = ast_pool -> NewArrayCreationExpression();
    p -> new_token = Token(1);
    p -> array_type = DYNAMIC_CAST<AstType*> (Sym(2));
    p -> array_initializer_opt = DYNAMIC_CAST<AstArrayInitializer*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 220:  DimExprs ::= DimExpr
//
// void StartList();
//

//
// Rule 221:  DimExprs ::= DimExprs DimExpr
//
// void AddList2();
//

//
// Rule 222:  DimExpr ::= LBRACKET Expression RBRACKET
//
#line 3437 "java.g"
void Parser::Act222()
{
    AstDimExpr* p = ast_pool -> NewDimExpr();
    p -> left_bracket_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> right_bracket_token = Token(3);
    Sym(1) = p;
}

//
// Rule 223:  Dims ::= LBRACKET RBRACKET
//
#line 3450 "java.g"
void Parser::Act223()
{
    Sym(1) = ast_pool -> NewBrackets(Token(1), Token(2));
    StartList();
}

//
// Rule 224:  Dims ::= Dims LBRACKET RBRACKET
//
#line 3460 "java.g"
void Parser::Act224()
{
    Sym(2) = ast_pool -> NewBrackets(Token(2), Token(3));
    AddList2();
}

//
// Rule 225:  FieldAccess ::= Primary DOT Identifier
//
#line 3470 "java.g"
void Parser::MakeFieldAccess()
{
    AstFieldAccess* p = ast_pool -> NewFieldAccess();
    p -> base = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> identifier_token = Token(3);
    Sym(1) = p;
}

//
// Rule 226:  FieldAccess ::= super DOT Identifier
//
#line 3482 "java.g"
void Parser::MakeSuperFieldAccess()
{
    Sym(1) = ast_pool -> NewSuperExpression(Token(1));
    MakeFieldAccess();
}

//
// Rule 227:  FieldAccess ::= Name DOT super DOT Identifier
//
#line 3497 "java.g"
void Parser::MakeQualifiedSuperFieldAccess()
{
    AstSuperExpression* q = ast_pool -> NewSuperExpression(Token(3));
    q -> base_opt = ast_pool -> NewTypeName(DYNAMIC_CAST<AstExpression*> (Sym(1)));
    AstFieldAccess* p = ast_pool -> NewFieldAccess();
    p -> base = q;
    p -> identifier_token = Token(5);
    Sym(1) = p;
}

//
// Rule 228:  MethodInvocation ::= Name LPAREN ArgumentListopt RPAREN
//
#line 3511 "java.g"
void Parser::Act228() { MakeMethodInvocation(2); }

//
// This function treats Sym(1) as a method name, and builds the method
// invocation starting with the '('.
//
void Parser::MakeMethodInvocation(int tokennum)
{
    AstMethodInvocation* p = ast_pool -> NewMethodInvocation();
    p -> method = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> left_parenthesis_token = Token(tokennum);
    if (Sym(tokennum + 1))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(tokennum + 1));
        p -> AllocateArguments(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument(DYNAMIC_CAST<AstExpression*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(tokennum + 2);
    Sym(1) = p;
}

//
// Rule 229:  MethodInvocation ::= Primary DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3542 "java.g"
void Parser::Act229()
{
    MakeFieldAccess();
    MakeMethodInvocation(4);
}

//
// Rule 230:  MethodInvocation ::= super DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3552 "java.g"
void Parser::Act230()
{
    MakeSuperFieldAccess();
    MakeMethodInvocation(4);
}

//
// Rule 231:  MethodInvocation ::= Name DOT super DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3568 "java.g"
void Parser::Act231()
{
    MakeQualifiedSuperFieldAccess();
    MakeMethodInvocation(6);
}

//
// Rule 232:  ArrayAccess ::= Name LBRACKET Expression RBRACKET
//
#line 3578 "java.g"
void Parser::MakeArrayAccess()
{
    AstArrayAccess* p = ast_pool -> NewArrayAccess();
    p -> base = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> left_bracket_token = Token(2);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    p -> right_bracket_token = Token(4);
    Sym(1) = p;
}

//
// Rule 233:  ArrayAccess ::= PrimaryNoNewArray LBRACKET Expression RBRACKET
//
// void MakeArrayAccess();
//

//
// Rule 234:  ArrayAccess ::= ArrayCreationInitialized LBRACKET Expression RBRACKET
//
// void MakeArrayAccess();
//

//
// Rule 235:  PostfixExpression ::= Primary
//
// void NoAction();
//

//
// Rule 236:  PostfixExpression ::= Name
//
// void NoAction();
//

//
// Rule 237:  PostfixExpression ::= PostIncrementExpression
//
// void NoAction();
//

//
// Rule 238:  PostfixExpression ::= PostDecrementExpression
//
// void NoAction();
//

//
// Rule 239:  PostIncrementExpression ::= PostfixExpression PLUS_PLUS
//
#line 3626 "java.g"
void Parser::Act239()
{
    AstPostUnaryExpression* p =
        ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::PLUSPLUS);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}

//
// Rule 240:  PostDecrementExpression ::= PostfixExpression MINUS_MINUS
//
#line 3639 "java.g"
void Parser::Act240()
{
    AstPostUnaryExpression* p =
        ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::MINUSMINUS);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}

//
// Rule 241:  UnaryExpression ::= PreIncrementExpression
//
// void NoAction();
//

//
// Rule 242:  UnaryExpression ::= PreDecrementExpression
//
// void NoAction();
//

//
// Rule 243:  UnaryExpression ::= PLUS UnaryExpression
//
#line 3660 "java.g"
void Parser::MakePreUnaryExpression()
{
    AstPreUnaryExpression::PreUnaryExpressionTag tag;
    switch (lex_stream -> Kind(Token(1)))
    {
    case TK_PLUS_PLUS: tag = AstPreUnaryExpression::PLUSPLUS; break;
    case TK_MINUS_MINUS: tag = AstPreUnaryExpression::MINUSMINUS; break;
    case TK_PLUS: tag = AstPreUnaryExpression::PLUS; break;
    case TK_MINUS: tag = AstPreUnaryExpression::MINUS; break;
    case TK_TWIDDLE: tag = AstPreUnaryExpression::TWIDDLE; break;
    case TK_NOT: tag = AstPreUnaryExpression::NOT; break;
    default: tag = AstPreUnaryExpression::NONE;
    }
    AstPreUnaryExpression* p = ast_pool -> NewPreUnaryExpression(tag);
    p -> pre_operator_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    Sym(1) = p;
}

//
// Rule 244:  UnaryExpression ::= MINUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 245:  UnaryExpression ::= UnaryExpressionNotPlusMinus
//
// void NoAction();
//

//
// Rule 246:  PreIncrementExpression ::= PLUS_PLUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 247:  PreDecrementExpression ::= MINUS_MINUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 248:  UnaryExpressionNotPlusMinus ::= PostfixExpression
//
// void NoAction();
//

//
// Rule 249:  UnaryExpressionNotPlusMinus ::= TWIDDLE UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 250:  UnaryExpressionNotPlusMinus ::= NOT UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 251:  UnaryExpressionNotPlusMinus ::= CastExpression
//
// void NoAction();
//

//
// Rule 252:  CastExpression ::= LPAREN PrimitiveType Dimsopt RPAREN UnaryExpression
//
#line 3715 "java.g"
void Parser::MakeCastExpression() { MakeCastExpression(MakeArrayType(2), 4); }

//
// Builds a cast expression. type must be the target AstType, and tokennum
// should point to the ')'.
//
void Parser::MakeCastExpression(AstType* type, int tokennum)
{
    AstCastExpression* p = ast_pool -> NewCastExpression();
    p -> left_parenthesis_token = Token(1);
    p -> type = type;
    p -> right_parenthesis_token = Token(tokennum);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(tokennum + 1));
    Sym(1) = p;
}

//
// Rule 253:  CastExpression ::= LPAREN Expression RPAREN UnaryExpressionNotPlusMinus
//
#line 3735 "java.g"
void Parser::Act253()
{
    //
    // Note that Expression must be a name - i.e., Sym(2) -> isName() == true
    // This check is not performed here and should be performed during
    // semantic processing.
    //
    AstCastExpression* p = ast_pool -> NewCastExpression();
    p -> left_parenthesis_token = Token(1);
    p -> type = ast_pool -> NewTypeName(DYNAMIC_CAST<AstExpression*> (Sym(2)));
    p -> right_parenthesis_token = Token(3);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(4));
    Sym(1) = p;
}

//
// Rule 254:  CastExpression ::= LPAREN Name Dims RPAREN UnaryExpressionNotPlusMinus
//
// void MakeCastExpression();
//

//
// Rule 255:  MultiplicativeExpression ::= UnaryExpression
//
// void NoAction();
//

//
// Rule 256:  MultiplicativeExpression ::= MultiplicativeExpression MULTIPLY UnaryExpression
//
#line 3765 "java.g"
//
// This creates a binary expression of the named type.
//
void Parser::MakeBinaryExpression()
{
    AstBinaryExpression::BinaryExpressionTag tag;
    switch (lex_stream -> Kind(Token(2)))
    {
    case TK_MULTIPLY: tag = AstBinaryExpression::STAR; break;
    case TK_DIVIDE: tag = AstBinaryExpression::SLASH; break;
    case TK_REMAINDER: tag = AstBinaryExpression::MOD; break;
    case TK_PLUS: tag = AstBinaryExpression::PLUS; break;
    case TK_MINUS: tag = AstBinaryExpression::MINUS; break;
    case TK_LEFT_SHIFT: tag = AstBinaryExpression::LEFT_SHIFT; break;
    case TK_RIGHT_SHIFT: tag = AstBinaryExpression::RIGHT_SHIFT; break;
    case TK_UNSIGNED_RIGHT_SHIFT:
        tag = AstBinaryExpression::UNSIGNED_RIGHT_SHIFT; break;
    case TK_LESS: tag = AstBinaryExpression::LESS; break;
    case TK_GREATER: tag = AstBinaryExpression::GREATER; break;
    case TK_LESS_EQUAL: tag = AstBinaryExpression::LESS_EQUAL; break;
    case TK_GREATER_EQUAL: tag = AstBinaryExpression::GREATER_EQUAL; break;
    case TK_EQUAL_EQUAL: tag = AstBinaryExpression::EQUAL_EQUAL; break;
    case TK_NOT_EQUAL: tag = AstBinaryExpression::NOT_EQUAL; break;
    case TK_AND: tag = AstBinaryExpression::AND; break;
    case TK_XOR: tag = AstBinaryExpression::XOR; break;
    case TK_OR: tag = AstBinaryExpression::IOR; break;
    case TK_AND_AND: tag = AstBinaryExpression::AND_AND; break;
    case TK_OR_OR: tag = AstBinaryExpression::OR_OR; break;
    default: tag = AstBinaryExpression::NONE;
    }
    AstBinaryExpression* p = ast_pool -> NewBinaryExpression(tag);
    p -> left_expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> binary_operator_token = Token(2);
    p -> right_expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 257:  MultiplicativeExpression ::= MultiplicativeExpression DIVIDE UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 258:  MultiplicativeExpression ::= MultiplicativeExpression REMAINDER UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 259:  AdditiveExpression ::= MultiplicativeExpression
//
// void NoAction();
//

//
// Rule 260:  AdditiveExpression ::= AdditiveExpression PLUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 261:  AdditiveExpression ::= AdditiveExpression MINUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 262:  ShiftExpression ::= AdditiveExpression
//
// void NoAction();
//

//
// Rule 263:  ShiftExpression ::= ShiftExpression LEFT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 264:  ShiftExpression ::= ShiftExpression RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 265:  ShiftExpression ::= ShiftExpression UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 266:  RelationalExpression ::= ShiftExpression
//
// void NoAction();
//

//
// Rule 267:  RelationalExpression ::= RelationalExpression LESS ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 268:  RelationalExpression ::= RelationalExpression GREATER ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 269:  RelationalExpression ::= RelationalExpression LESS_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 270:  RelationalExpression ::= RelationalExpression GREATER_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 271:  RelationalExpression ::= RelationalExpression instanceof ReferenceType
//
#line 3862 "java.g"
void Parser::MakeInstanceofExpression()
{
    AstInstanceofExpression* p = ast_pool -> NewInstanceofExpression();
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> instanceof_token = Token(2);
    p -> type = DYNAMIC_CAST<AstType*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 272:  EqualityExpression ::= RelationalExpression
//
// void NoAction();
//

//
// Rule 273:  EqualityExpression ::= EqualityExpression EQUAL_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 274:  EqualityExpression ::= EqualityExpression NOT_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 275:  AndExpression ::= EqualityExpression
//
// void NoAction();
//

//
// Rule 276:  AndExpression ::= AndExpression AND EqualityExpression
//
// void MakeBinaryExpression();
//

//
// Rule 277:  ExclusiveOrExpression ::= AndExpression
//
// void NoAction();
//

//
// Rule 278:  ExclusiveOrExpression ::= ExclusiveOrExpression XOR AndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 279:  InclusiveOrExpression ::= ExclusiveOrExpression
//
// void NoAction();
//

//
// Rule 280:  InclusiveOrExpression ::= InclusiveOrExpression OR ExclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 281:  ConditionalAndExpression ::= InclusiveOrExpression
//
// void NoAction();
//

//
// Rule 282:  ConditionalAndExpression ::= ConditionalAndExpression AND_AND InclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 283:  ConditionalOrExpression ::= ConditionalAndExpression
//
// void NoAction();
//

//
// Rule 284:  ConditionalOrExpression ::= ConditionalOrExpression OR_OR ConditionalAndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 285:  ConditionalExpression ::= ConditionalOrExpression
//
// void NoAction();
//

//
// Rule 286:  ConditionalExpression ::= ConditionalOrExpression QUESTION Expression COLON ConditionalExpression
//
#line 3934 "java.g"
void Parser::MakeConditionalExpression()
{
    AstConditionalExpression* p = ast_pool -> NewConditionalExpression();
    p -> test_expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> question_token = Token(2);
    p -> true_expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    p -> colon_token = Token(4);
    p -> false_expression = DYNAMIC_CAST<AstExpression*> (Sym(5));
    Sym(1) = p;
}

//
// Rule 287:  AssignmentExpression ::= ConditionalExpression
//
// void NoAction();
//

//
// Rule 288:  AssignmentExpression ::= Assignment
//
// void NoAction();
//

//
// Rule 289:  Assignment ::= PostfixExpression AssignmentOperator AssignmentExpression
//
#line 3964 "java.g"
void Parser::Act289()
{
    AstAssignmentExpression::AssignmentExpressionTag tag;
    switch (lex_stream -> Kind(Token(2)))
    {
    case TK_EQUAL: tag = AstAssignmentExpression::SIMPLE_EQUAL; break;
    case TK_MULTIPLY_EQUAL: tag = AstAssignmentExpression::STAR_EQUAL; break;
    case TK_DIVIDE_EQUAL: tag = AstAssignmentExpression::SLASH_EQUAL; break;
    case TK_REMAINDER_EQUAL: tag = AstAssignmentExpression::MOD_EQUAL; break;
    case TK_PLUS_EQUAL: tag = AstAssignmentExpression::PLUS_EQUAL; break;
    case TK_MINUS_EQUAL: tag = AstAssignmentExpression::MINUS_EQUAL; break;
    case TK_LEFT_SHIFT_EQUAL:
        tag = AstAssignmentExpression::LEFT_SHIFT_EQUAL; break;
    case TK_RIGHT_SHIFT_EQUAL:
        tag = AstAssignmentExpression::RIGHT_SHIFT_EQUAL; break;
    case TK_UNSIGNED_RIGHT_SHIFT_EQUAL:
        tag = AstAssignmentExpression::UNSIGNED_RIGHT_SHIFT_EQUAL; break;
    case TK_AND_EQUAL: tag = AstAssignmentExpression::AND_EQUAL; break;
    case TK_XOR_EQUAL: tag = AstAssignmentExpression::XOR_EQUAL; break;
    case TK_OR_EQUAL: tag = AstAssignmentExpression::IOR_EQUAL; break;
    default: tag = AstAssignmentExpression::NONE;
    }
    AstAssignmentExpression* p =
        ast_pool -> NewAssignmentExpression(tag, Token(2));
    p -> left_hand_side = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 290:  AssignmentOperator ::= EQUAL
//
// void NoAction();
//

//
// Rule 291:  AssignmentOperator ::= MULTIPLY_EQUAL
//
// void NoAction();
//

//
// Rule 292:  AssignmentOperator ::= DIVIDE_EQUAL
//
// void NoAction();
//

//
// Rule 293:  AssignmentOperator ::= REMAINDER_EQUAL
//
// void NoAction();
//

//
// Rule 294:  AssignmentOperator ::= PLUS_EQUAL
//
// void NoAction();
//

//
// Rule 295:  AssignmentOperator ::= MINUS_EQUAL
//
// void NoAction();
//

//
// Rule 296:  AssignmentOperator ::= LEFT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 297:  AssignmentOperator ::= RIGHT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 298:  AssignmentOperator ::= UNSIGNED_RIGHT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 299:  AssignmentOperator ::= AND_EQUAL
//
// void NoAction();
//

//
// Rule 300:  AssignmentOperator ::= XOR_EQUAL
//
// void NoAction();
//

//
// Rule 301:  AssignmentOperator ::= OR_EQUAL
//
// void NoAction();
//

//
// Rule 302:  Expression ::= AssignmentExpression
//
// void NoAction();
//

//
// Rule 303:  ,opt ::=
//
// void NoAction();
//

//
// Rule 304:  ,opt ::= COMMA
//
// void NoAction();
//

//
// Rule 305:  Identifieropt ::=
//
// void NoAction();
//

//
// Rule 306:  Identifieropt -> Identifier
//
// void NoAction();
//

//
// Rule 307:  PackageDeclarationopt ::=
//
#line 4084 "java.g"
//
// Given a rule of the form A ::= x1 x2 ... xn
//
// Construct a NULL Ast for A.
//
void Parser::NullAction() { Sym(1) = NULL; }

//
// Rule 308:  PackageDeclarationopt ::= PackageDeclaration
//
// void NoAction();
//

//
// Rule 309:  Superopt ::=
//
// void NullAction();
//

//
// Rule 310:  Superopt ::= Super
//
// void NoAction();
//

//
// Rule 311:  Expressionopt ::=
//
// void NullAction();
//

//
// Rule 312:  Expressionopt ::= Expression
//
// void NoAction();
//

//
// Rule 313:  ClassBodyopt ::=
//
// void NullAction();
//

//
// Rule 314:  ClassBodyopt ::= ClassBody
//
// void NoAction();
//

//
// Rule 315:  ImportDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 316:  ImportDeclarationsopt ::= ImportDeclarations
//
// void NoAction();
//

//
// Rule 317:  TypeDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 318:  TypeDeclarationsopt ::= TypeDeclarations
//
// void NoAction();
//

//
// Rule 319:  ClassBodyDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 320:  ClassBodyDeclarationsopt ::= ClassBodyDeclarations
//
// void NoAction();
//

//
// Rule 321:  Modifiersopt ::=
//
// void NullAction();
//

//
// Rule 322:  Modifiersopt ::= Modifiers
//
// void NoAction();
//

//
// Rule 323:  BlockStatementsopt ::=
//
// void NullAction();
//

//
// Rule 324:  BlockStatementsopt ::= BlockStatements
//
// void NoAction();
//

//
// Rule 325:  Dimsopt ::=
//
// void NullAction();
//

//
// Rule 326:  Dimsopt ::= Dims
//
// void NoAction();
//

//
// Rule 327:  ArgumentListopt ::=
//
// void NullAction();
//

//
// Rule 328:  ArgumentListopt ::= ArgumentList
//
// void NoAction();
//

//
// Rule 329:  SwitchLabelsopt ::=
//
// void NullAction();
//

//
// Rule 330:  SwitchLabelsopt ::= SwitchLabels
//
// void NoAction();
//

//
// Rule 331:  Throwsopt ::=
//
// void NullAction();
//

//
// Rule 332:  Throwsopt ::= Throws
//
// void NoAction();
//

//
// Rule 333:  FormalParameterListopt ::=
//
// void NullAction();
//

//
// Rule 334:  FormalParameterListopt ::= FormalParameterList
//
// void NoAction();
//

//
// Rule 335:  Interfacesopt ::=
//
// void NullAction();
//

//
// Rule 336:  Interfacesopt ::= Interfaces
//
// void NoAction();
//

//
// Rule 337:  InterfaceMemberDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 338:  InterfaceMemberDeclarationsopt ::= InterfaceMemberDeclarations
//
// void NoAction();
//

//
// Rule 339:  ForInitopt ::=
//
// void NullAction();
//

//
// Rule 340:  ForInitopt ::= ForInit
//
// void NoAction();
//

//
// Rule 341:  ForUpdateopt ::=
//
// void NullAction();
//

//
// Rule 342:  ForUpdateopt ::= ForUpdate
//
// void NoAction();
//

//
// Rule 343:  ExtendsInterfacesopt ::=
//
// void NullAction();
//

//
// Rule 344:  ExtendsInterfacesopt ::= ExtendsInterfaces
//
// void NoAction();
//

//
// Rule 345:  Catchesopt ::=
//
// void NullAction();
//

//
// Rule 346:  Catchesopt ::= Catches
//
// void NoAction();
//

//
// Rule 347:  PackageHeaderMarker ::=
//
#line 4251 "java.g"
//
// When this function is invoked, if the "parse_package_header_only" flag
// is turned on, we skip to the end-of-file token.
//
void Parser::Act347()
{
    if (parse_package_header_only)
        // point to the EOF token
        lex_stream -> Reset(lex_stream -> NumTokens() - 1);
    Sym(1) = NULL;
}

//
// Rule 348:  MethodHeaderMarker ::=
//
#line 4267 "java.g"
//
// When this function is invoked, if the "parse_header_only" flag
// is turned on, the body of the method being parsed is skipped.
//
void Parser::Act348()
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
// Rule 349:  Marker ::=
//
// void NullAction();
//
#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif
