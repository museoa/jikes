#line 674 "java.g"
// $Id$
// DO NOT MODIFY THIS FILE - it is generated using jikespg on java.g.
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 2004 IBM Corporation and others.  All Rights Reserved.
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
#line 717 "java.g"
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
#line 735 "java.g"
//
// Given a rule of the form A ::= x1 x2, inherit the result from x2.
//
void Parser::SetSym1ToSym2() { Sym(1) = Sym(2); }

//
// Rule 3:  Literal ::= IntegerLiteral
//
#line 747 "java.g"
void Parser::Act3()
{
    Sym(1) = ast_pool -> NewIntegerLiteral(Token(1));
}

//
// Rule 4:  Literal ::= LongLiteral
//
#line 756 "java.g"
void Parser::Act4()
{
    Sym(1) = ast_pool -> NewLongLiteral(Token(1));
}

//
// Rule 5:  Literal ::= FloatLiteral
//
#line 765 "java.g"
void Parser::Act5()
{
    Sym(1) = ast_pool -> NewFloatLiteral(Token(1));
}

//
// Rule 6:  Literal ::= DoubleLiteral
//
#line 774 "java.g"
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
#line 787 "java.g"
void Parser::Act8()
{
    Sym(1) = ast_pool -> NewCharacterLiteral(Token(1));
}

//
// Rule 9:  Literal ::= StringLiteral
//
#line 796 "java.g"
void Parser::Act9()
{
    Sym(1) = ast_pool -> NewStringLiteral(Token(1));
}

//
// Rule 10:  Literal ::= null
//
#line 805 "java.g"
void Parser::Act10()
{
    Sym(1) = ast_pool -> NewNullLiteral(Token(1));
}

//
// Rule 11:  BooleanLiteral ::= true
//
#line 814 "java.g"
void Parser::Act11()
{
    Sym(1) = ast_pool -> NewTrueLiteral(Token(1));
}

//
// Rule 12:  BooleanLiteral ::= false
//
#line 823 "java.g"
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
#line 846 "java.g"
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
#line 863 "java.g"
void Parser::Act19()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::BYTE, Token(1));
}

//
// Rule 20:  IntegralType ::= short
//
#line 872 "java.g"
void Parser::Act20()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::SHORT, Token(1));
}

//
// Rule 21:  IntegralType ::= int
//
#line 881 "java.g"
void Parser::Act21()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::INT, Token(1));
}

//
// Rule 22:  IntegralType ::= long
//
#line 890 "java.g"
void Parser::Act22()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::LONG, Token(1));
}

//
// Rule 23:  IntegralType ::= char
//
#line 899 "java.g"
void Parser::Act23()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::CHAR, Token(1));
}

//
// Rule 24:  FloatingPointType ::= float
//
#line 908 "java.g"
void Parser::Act24()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::FLOAT, Token(1));
}

//
// Rule 25:  FloatingPointType ::= double
//
#line 917 "java.g"
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
#line 941 "java.g"
void Parser::Act28()
{
    Sym(1) = ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(1)));
}

//
// Rule 29:  ArrayType ::= PrimitiveType Dims
//
#line 961 "java.g"
void Parser::MakeArrayType() { Sym(1) = MakeArrayType(1); }

//
// Used on variants of "Type Dimsopt". If this type has dimensions, make an
// array type; otherwise return the type name.
//
AstType* Parser::MakeArrayType(int tokennum)
{
    AstType* p = Sym(tokennum) -> NameCast()
        ? ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(tokennum)))
        : DYNAMIC_CAST<AstType*> (Sym(tokennum));
    return ! Sym(tokennum + 1) ? p
        : ast_pool -> NewArrayType(p, (DYNAMIC_CAST<AstBrackets*>
                                       (Sym(tokennum + 1))));
}

//
// Rule 30:  ArrayType ::= ClassOrInterfaceType Dims
//
// void MakeArrayType();
//

//
// Rule 31:  Name ::= Identifier
//
#line 996 "java.g"
void Parser::Act31() { MakeSimpleName(1); }

//
// Used on "Identifier", and sets the corresponding symbol to a simple name.
//
AstName* Parser::MakeSimpleName(int tokennum)
{
    AstName* name = ast_pool -> NewName(Token(tokennum));
    Sym(tokennum) = name;
    return name;
}

//
// Rule 32:  Name ::= Name DOT Identifier
//
#line 1012 "java.g"
void Parser::Act32()
{
    AstName* p = ast_pool -> NewName(Token(3));
    p -> base_opt = DYNAMIC_CAST<AstName*> (Sym(1));
    Sym(1) = p;
}

//
// Rule 33:  CompilationUnit ::= PackageDeclarationopt ImportDeclarationsopt TypeDeclarationsopt
//
#line 1026 "java.g"
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
            p -> AddTypeDeclaration(DYNAMIC_CAST<AstDeclaredType*>
                                    (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 34:  ImportDeclarations ::= ImportDeclaration
//
#line 1064 "java.g"
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
#line 1081 "java.g"
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
#line 1123 "java.g"
void Parser::Act38()
{
    AstPackageDeclaration* p = ast_pool -> NewPackageDeclaration();
    p -> package_token = Token(1);
    p -> name = DYNAMIC_CAST<AstName*> (Sym(2));
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
// Rule 41:  ImportDeclaration ::= SingleStaticImportDeclaration
//
// void NoAction();
//

//
// Rule 42:  ImportDeclaration ::= StaticImportOnDemandDeclaration
//
// void NoAction();
//

//
// Rule 43:  SingleTypeImportDeclaration ::= import Marker Name Marker Marker SEMICOLON
//
#line 1164 "java.g"
void Parser::MakeImportDeclaration()
{
    AstImportDeclaration* p = ast_pool -> NewImportDeclaration();
    p -> import_token = Token(1);
    p -> static_token_opt = Token(2) == Token(3) ? 0 : Token(2);
    p -> name = DYNAMIC_CAST<AstName*> (Sym(3));
    p -> star_token_opt = Token(4) == Token(5) ? 0 : Token(5);
    p -> semicolon_token = Token(6);
    Sym(1) = p;
}

//
// Rule 44:  TypeImportOnDemandDeclaration ::= import Marker Name DOT MULTIPLY SEMICOLON
//
// void MakeImportDeclaration();
//

//
// Rule 45:  SingleStaticImportDeclaration ::= import static Name Marker Marker SEMICOLON
//
// void MakeImportDeclaration();
//

//
// Rule 46:  StaticImportOnDemandDeclaration ::= import static Name DOT MULTIPLY SEMICOLON
//
// void MakeImportDeclaration();
//

//
// Rule 47:  TypeDeclaration ::= ClassDeclaration
//
// void NoAction();
//

//
// Rule 48:  TypeDeclaration ::= InterfaceDeclaration
//
// void NoAction();
//

//
// Rule 49:  TypeDeclaration ::= SEMICOLON
//
#line 1223 "java.g"
void Parser::Act49()
{
    Sym(1) = ast_pool -> NewEmptyDeclaration(Token(1));
}

//
// Rule 50:  Modifiers ::= Modifier
//
#line 1244 "java.g"
void Parser::Act50()
{
    AstModifiers* p = ast_pool -> NewModifier(Token(1));
    if (lex_stream -> Kind(Token(1)) == TK_static)
        p -> static_token_opt = Token(1);
    Sym(1) = p;
}

//
// Rule 51:  Modifiers ::= Modifiers Modifier
//
#line 1256 "java.g"
void Parser::Act51()
{
    AstModifiers* p = DYNAMIC_CAST<AstModifiers*> (Sym(1));
    p -> right_modifier_token = Token(2);
    if (! p -> static_token_opt && lex_stream -> Kind(Token(2)) == TK_static)
        p -> static_token_opt = Token(1);
}

//
// Rule 52:  Modifier ::= public
//
// void NoAction();
//

//
// Rule 53:  Modifier ::= protected
//
// void NoAction();
//

//
// Rule 54:  Modifier ::= private
//
// void NoAction();
//

//
// Rule 55:  Modifier ::= static
//
// void NoAction();
//

//
// Rule 56:  Modifier ::= abstract
//
// void NoAction();
//

//
// Rule 57:  Modifier ::= final
//
// void NoAction();
//

//
// Rule 58:  Modifier ::= native
//
// void NoAction();
//

//
// Rule 59:  Modifier ::= strictfp
//
// void NoAction();
//

//
// Rule 60:  Modifier ::= synchronized
//
// void NoAction();
//

//
// Rule 61:  Modifier ::= transient
//
// void NoAction();
//

//
// Rule 62:  Modifier ::= volatile
//
// void NoAction();
//

//
// Rule 63:  ClassDeclaration ::= Modifiersopt class Identifier Marker Superopt Interfacesopt ClassBody
//
#line 1321 "java.g"
void Parser::Act63()
{
    AstClassDeclaration* p = ast_pool -> NewClassDeclaration();
    p -> modifiers_opt = DYNAMIC_CAST<AstModifiers*> (Sym(1));
    p -> class_token = Token(2);
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
    p -> class_body -> identifier_token = Token(3);
    p -> class_body -> owner = p;
    Sym(1) = p;
}

//
// Rule 64:  Super ::= extends ClassOrInterfaceType
//
// void SetSym1ToSym2();
//

//
// Rule 65:  Interfaces ::= implements TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 66:  TypeList ::= ClassOrInterfaceType
//
// void StartList();
//

//
// Rule 67:  TypeList ::= TypeList COMMA ClassOrInterfaceType
//
// void AddList3();
//

//
// Rule 68:  ClassBody ::= LBRACE ClassBodyDeclarationsopt RBRACE
//
#line 1383 "java.g"
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
            num_instance_initializers = 0,
            num_inner_classes = 0,
            num_inner_interfaces = 0,
            num_empty_declarations = 0;

        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(2));
        p -> AllocateClassBodyDeclarations(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            AstDeclared* declaration =
                DYNAMIC_CAST<AstDeclared*> (root -> element);
            p -> AddClassBodyDeclaration(declaration);
            AstFieldDeclaration* field_declaration =
                declaration -> FieldDeclarationCast();
            AstInitializerDeclaration* initializer =
                declaration -> InitializerDeclarationCast();
            if (field_declaration)
            {
                if (field_declaration -> modifiers_opt &&
                    field_declaration -> modifiers_opt -> static_token_opt)
                {
                    field_declaration -> MarkStatic();
                }
                //
                // Interface fields were already marked static.
                //
                if (field_declaration -> StaticFieldCast())
                    num_class_variables++;
                else num_instance_variables++;
            }
            else if (declaration -> MethodDeclarationCast())
                num_methods++;
            else if (declaration -> ConstructorDeclarationCast())
                num_constructors++;
            else if (initializer)
            {
                if (initializer -> modifiers_opt &&
                    initializer -> modifiers_opt -> static_token_opt)
                {
                    initializer -> MarkStatic();
                    num_static_initializers++;
                }
                else num_instance_initializers++;
            }
            else if (declaration -> ClassDeclarationCast())
                num_inner_classes++;
            else if (declaration -> InterfaceDeclarationCast())
                num_inner_interfaces++;
            else num_empty_declarations++;
        } while (root != tail);

        p -> AllocateInstanceVariables(num_instance_variables);
        p -> AllocateClassVariables(num_class_variables);
        p -> AllocateMethods(num_methods);
        p -> AllocateConstructors(num_constructors);
        p -> AllocateStaticInitializers(num_static_initializers);
        p -> AllocateInstanceInitializers(num_instance_initializers);
        p -> AllocateNestedClasses(num_inner_classes);
        p -> AllocateNestedInterfaces(num_inner_interfaces);
        p -> AllocateEmptyDeclarations(num_empty_declarations);

        root = tail;
        do
        {
            root = root -> next;
            AstDeclared* declaration =
                DYNAMIC_CAST<AstDeclared*> (root -> element);
            AstFieldDeclaration* field_declaration =
                declaration -> FieldDeclarationCast();
            AstMethodDeclaration* method_declaration =
                declaration -> MethodDeclarationCast();
            AstConstructorDeclaration* constructor_declaration =
                declaration -> ConstructorDeclarationCast();
            AstInitializerDeclaration* initializer =
                declaration -> InitializerDeclarationCast();
            AstClassDeclaration* class_declaration =
                declaration -> ClassDeclarationCast();
            AstInterfaceDeclaration* interface_declaration =
                declaration -> InterfaceDeclarationCast();

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
            else if (initializer)
            {
                if (initializer -> StaticInitializerCast())
                     p -> AddStaticInitializer(initializer);
                else p -> AddInstanceInitializer(initializer);
            }
            else if (class_declaration)
                p -> AddNestedClass(class_declaration);
            else if (interface_declaration)
                p -> AddNestedInterface(interface_declaration);
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
// Rule 69:  ClassBodyDeclarations ::= ClassBodyDeclaration
//
// void StartList();
//

//
// Rule 70:  ClassBodyDeclarations ::= ClassBodyDeclarations ClassBodyDeclaration
//
// void AddList2();
//

//
// Rule 71:  ClassBodyDeclaration ::= MemberDeclaration
//
// void NoAction();
//

//
// Rule 72:  ClassBodyDeclaration ::= ConstructorDeclaration
//
// void NoAction();
//

//
// Rule 73:  ClassBodyDeclaration ::= InitializerDeclaration
//
// void NoAction();
//

//
// Rule 74:  MemberDeclaration ::= FieldDeclaration
//
// void NoAction();
//

//
// Rule 75:  MemberDeclaration ::= MethodDeclaration
//
// void NoAction();
//

//
// Rule 76:  MemberDeclaration ::= TypeDeclaration
//
// void NoAction();
//

//
// Rule 77:  FieldDeclaration ::= Modifiersopt Marker Type VariableDeclarators SEMICOLON
//
#line 1577 "java.g"
void Parser::Act77()
{
    AstFieldDeclaration* p = ast_pool -> NewFieldDeclaration();
    p -> modifiers_opt = DYNAMIC_CAST<AstModifiers*> (Sym(1));
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
// Rule 78:  VariableDeclarators ::= VariableDeclarator
//
// void StartList();
//

//
// Rule 79:  VariableDeclarators ::= VariableDeclarators COMMA VariableDeclarator
//
// void AddList3();
//

//
// Rule 80:  VariableDeclarator ::= VariableDeclaratorId
//
#line 1608 "java.g"
void Parser::Act80()
{
    AstVariableDeclarator* p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name =
        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(1));
    Sym(1) = p;
}

//
// Rule 81:  VariableDeclarator ::= VariableDeclaratorId EQUAL VariableInitializer
//
#line 1620 "java.g"
void Parser::Act81()
{
    AstVariableDeclarator* p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name =
        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(1));
    p -> variable_initializer_opt = Sym(3);
    Sym(1) = p;
}

//
// Rule 82:  VariableDeclaratorId ::= Identifier Dimsopt
//
#line 1633 "java.g"
void Parser::Act82()
{
    AstVariableDeclaratorId* p = ast_pool -> NewVariableDeclaratorId();
    p -> identifier_token = Token(1);
    p -> brackets_opt = DYNAMIC_CAST<AstBrackets*> (Sym(2));
    Sym(1) = p;
}

//
// Rule 83:  VariableInitializer ::= Expression
//
// void NoAction();
//

//
// Rule 84:  VariableInitializer ::= ArrayInitializer
//
// void NoAction();
//

//
// Rule 85:  MethodDeclaration ::= MethodHeader MethodHeaderMarker MethodBody
//
#line 1665 "java.g"
void Parser::MakeMethodDeclaration()
{
    DYNAMIC_CAST<AstMethodDeclaration*> (Sym(1)) -> method_body_opt =
        DYNAMIC_CAST<AstMethodBody*> (Sym(3));
}

//
// Rule 86:  MethodDeclaration ::= MethodHeader MethodHeaderMarker Marker SEMICOLON
//
// void MakeMethodDeclaration();
//

//
// Rule 87:  MethodHeader ::= Modifiersopt Marker Type MethodDeclarator Throwsopt
//
#line 1689 "java.g"
void Parser::MakeMethodHeader()
{
    AstMethodDeclaration* p = ast_pool -> NewMethodDeclaration();
    p -> modifiers_opt = DYNAMIC_CAST<AstModifiers*> (Sym(1));
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
// Rule 88:  MethodHeader ::= Modifiersopt Marker void MethodDeclarator Throwsopt
//
#line 1718 "java.g"
void Parser::Act88()
{
    Sym(3) = ast_pool -> NewPrimitiveType(Ast::VOID_TYPE, Token(3));
    MakeMethodHeader();
}

//
// Rule 89:  MethodDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN Dimsopt
//
#line 1728 "java.g"
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
    p -> brackets_opt = DYNAMIC_CAST<AstBrackets*> (Sym(5));
    Sym(1) = p;
}

//
// Rule 90:  FormalParameterList ::= LastFormalParameter
//
// void StartList();
//

//
// Rule 91:  FormalParameterList ::= FormalParameters COMMA LastFormalParameter
//
// void AddList3();
//

//
// Rule 92:  FormalParameters ::= FormalParameter
//
// void StartList();
//

//
// Rule 93:  FormalParameters ::= FormalParameters COMMA FormalParameter
//
// void AddList3();
//

//
// Rule 94:  FormalParameter ::= PrimitiveType Dimsopt Marker VariableDeclaratorId
//
#line 1791 "java.g"
void Parser::MakeFormalParameterA()
{
    MakeFormalParameter(NULL, MakeArrayType(1),
                        Token(3) == Token(4) ? 0 : Token(3),
                        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(4)));
}

//
// Creates a formal parameter declaration and places it in Sym(1).
//
void Parser::MakeFormalParameter(AstModifiers* modifiers, AstType* type,
                                 int ellipsis_token,
                                 AstVariableDeclaratorId* variable)
{
    AstFormalParameter* p = ast_pool -> NewFormalParameter();
    p -> modifiers_opt = modifiers;
    p -> type = type;
    p -> ellipsis_token_opt = ellipsis_token;
    AstVariableDeclarator* formal_declarator =
        ast_pool -> NewVariableDeclarator();
    formal_declarator -> variable_declarator_name = variable;
    p -> formal_declarator = formal_declarator;
    Sym(1) = p;
}

//
// Rule 95:  FormalParameter ::= Name Marker Marker VariableDeclaratorId
//
// void MakeFormalParameterA();
//

//
// Rule 96:  FormalParameter ::= Name Dims Marker VariableDeclaratorId
//
// void MakeFormalParameterA();
//

//
// Rule 97:  FormalParameter ::= Modifiers Type Marker VariableDeclaratorId
//
#line 1849 "java.g"
void Parser::MakeFormalParameterB()
{
    MakeFormalParameter(DYNAMIC_CAST<AstModifiers*> (Sym(1)),
                        DYNAMIC_CAST<AstType*> (Sym(2)),
                        Token(3) == Token(4) ? 0 : Token(3),
                        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(4)));
}

//
// Rule 98:  LastFormalParameter ::= FormalParameter
//
// void NoAction();
//

//
// Rule 99:  LastFormalParameter ::= PrimitiveType Dimsopt ELLIPSIS VariableDeclaratorId
//
// void MakeFormalParameterA();
//

//
// Rule 100:  LastFormalParameter ::= Name Marker ELLIPSIS VariableDeclaratorId
//
// void MakeFormalParameterA();
//

//
// Rule 101:  LastFormalParameter ::= Name Dims ELLIPSIS VariableDeclaratorId
//
// void MakeFormalParameterA();
//

//
// Rule 102:  LastFormalParameter ::= Modifiers Type ELLIPSIS VariableDeclaratorId
//
// void MakeFormalParameterB();
//

//
// Rule 103:  Throws ::= throws TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 104:  MethodBody ::= LBRACE BlockStatementsopt RBRACE
//
#line 1931 "java.g"
void Parser::Act104()
{
    AstMethodBody* p = ast_pool -> NewMethodBody();
    p -> left_brace_token = Token(1);
    p -> right_brace_token = Token(3);

    if (Sym(2))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(2));
        // Allocate 1 extra for possible generated return statement.
        p -> AllocateStatements(tail -> index + 2);
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
    else p -> AllocateStatements(1);
    Sym(1) = p;
}

//
// Rule 105:  InitializerDeclaration ::= Modifiersopt Marker MethodHeaderMarker MethodBody
//
#line 1977 "java.g"
void Parser::Act105()
{
    AstInitializerDeclaration* p = ast_pool -> NewInitializerDeclaration();
    p -> modifiers_opt = DYNAMIC_CAST<AstModifiers*> (Sym(1));
    p -> block = DYNAMIC_CAST<AstMethodBody*> (Sym(4));
    Sym(1) = p;
}

//
// Rule 106:  ConstructorDeclaration ::= Modifiersopt Marker ConstructorDeclarator Throwsopt MethodHeaderMarker MethodBody
//
#line 1998 "java.g"
void Parser::MakeConstructorDeclaration()
{
    AstConstructorDeclaration* p = ast_pool -> NewConstructorDeclaration();
    p -> modifiers_opt = DYNAMIC_CAST<AstModifiers*> (Sym(1));
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
// Rule 107:  ConstructorDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN Marker
//
// void MakeMethodDeclarator();
//

//
// Rule 108:  ExplicitConstructorInvocation ::= this LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2045 "java.g"
void Parser::Act108()
{
    AstThisCall* p = ast_pool -> NewThisCall();
    p -> this_token = Token(1);
    p -> arguments = MakeArguments(2);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

AstArguments* Parser::MakeArguments(int tokennum)
{
    AstArguments* p = ast_pool -> NewArguments(Token(tokennum),
                                               Token(tokennum + 2));
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
    return p;
}

//
// Rule 109:  ExplicitConstructorInvocation ::= super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2077 "java.g"
void Parser::Act109()
{
    AstSuperCall* p = ast_pool -> NewSuperCall();
    p -> super_token = Token(1);
    p -> arguments = MakeArguments(2);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 110:  ExplicitConstructorInvocation ::= Primary DOT super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2092 "java.g"
void Parser::MakeQualifiedSuper()
{
    AstSuperCall* p = ast_pool -> NewSuperCall();
    p -> base_opt = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> super_token = Token(3);
    p -> arguments = MakeArguments(4);
    p -> semicolon_token = Token(7);
    Sym(1) = p;
}

//
// Rule 111:  ExplicitConstructorInvocation ::= Name DOT super LPAREN ArgumentListopt RPAREN SEMICOLON
//
// void MakeQualifiedSuper();
//

//
// Rule 112:  InterfaceDeclaration ::= Modifiersopt interface Identifier Marker ExtendsInterfacesopt InterfaceBody
//
#line 2122 "java.g"
void Parser::Act112()
{
    AstInterfaceDeclaration* p = ast_pool -> NewInterfaceDeclaration();
    p -> modifiers_opt = DYNAMIC_CAST<AstModifiers*> (Sym(1));
    p -> interface_token = Token(2);
    if (Sym(5))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(5));
        p -> AllocateInterfaces(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddInterface(DYNAMIC_CAST<AstTypeName*> (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> class_body = DYNAMIC_CAST<AstClassBody*> (Sym(6));
    p -> class_body -> identifier_token = Token(3);
    p -> class_body -> owner = p;
    Sym(1) = p;
}

//
// Rule 113:  ExtendsInterfaces ::= extends TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 114:  InterfaceBody ::= LBRACE InterfaceMemberDeclarationsopt RBRACE
//
// void MakeClassBody();
//

//
// Rule 115:  InterfaceMemberDeclarations ::= MemberDeclaration
//
#line 2173 "java.g"
void Parser::Act115()
{
    AstFieldDeclaration* field = Sym(1) -> FieldDeclarationCast();
    if (field)
        field -> MarkStatic();
    StartList();
}

//
// Rule 116:  InterfaceMemberDeclarations ::= InterfaceMemberDeclarations MemberDeclaration
//
#line 2188 "java.g"
void Parser::Act116()
{
    AstFieldDeclaration* field = Sym(2) -> FieldDeclarationCast();
    if (field)
        field -> MarkStatic();
    AddList2();
}

//
// Rule 117:  ArrayInitializer ::= LBRACE Marker ,opt RBRACE
//
#line 2217 "java.g"
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
// Rule 118:  ArrayInitializer ::= LBRACE VariableInitializers ,opt RBRACE
//
// void MakeArrayInitializer();
//

//
// Rule 119:  VariableInitializers ::= VariableInitializer
//
// void StartList();
//

//
// Rule 120:  VariableInitializers ::= VariableInitializers COMMA VariableInitializer
//
// void AddList3();
//

//
// Rule 121:  Block ::= LBRACE BlockStatementsopt RBRACE
//
#line 2258 "java.g"
void Parser::Act121()
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
// Rule 122:  BlockStatements ::= BlockStatement
//
// void StartList();
//

//
// Rule 123:  BlockStatements ::= BlockStatements BlockStatement
//
// void AddList2();
//

//
// Rule 124:  BlockStatement ::= LocalVariableDeclarationStatement
//
// void NoAction();
//

//
// Rule 125:  BlockStatement ::= Statement
//
// void NoAction();
//

//
// Rule 126:  BlockStatement ::= ClassDeclaration
//
#line 2299 "java.g"
void Parser::Act126()
{
    Sym(1) = ast_pool ->
        NewLocalClassDeclarationStatement(DYNAMIC_CAST<AstClassDeclaration*>
                                          (Sym(1)));
}

//
// Rule 127:  BlockStatement ::= ExplicitConstructorInvocation
//
// void NoAction();
//

//
// Rule 128:  LocalVariableDeclarationStatement ::= LocalVariableDeclaration SEMICOLON
//
#line 2319 "java.g"
void Parser::Act128()
{
    DYNAMIC_CAST<AstLocalVariableDeclarationStatement*> (Sym(1)) ->
        semicolon_token_opt = Token(2);
}

//
// Rule 129:  LocalVariableDeclaration ::= PrimitiveType Dimsopt Marker VariableDeclarators
//
#line 2335 "java.g"
void Parser::MakeLocalVariable()
{
    MakeLocalVariable(NULL, MakeArrayType(1),
                      DYNAMIC_CAST<AstListNode*> (Sym(4)));
}

//
// Creates a local variable declaration and places it in Sym(1).
//
void Parser::MakeLocalVariable(AstModifiers* modifiers, AstType* type,
                               AstListNode* variables)
{
    AstLocalVariableDeclarationStatement* p =
        ast_pool -> NewLocalVariableDeclarationStatement();
    p -> modifiers_opt = modifiers;
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
// Rule 130:  LocalVariableDeclaration ::= Name Marker Marker VariableDeclarators
//
// void MakeLocalVariable();
//

//
// Rule 131:  LocalVariableDeclaration ::= Name Dims Marker VariableDeclarators
//
// void MakeLocalVariable();
//

//
// Rule 132:  LocalVariableDeclaration ::= Modifiers Type Marker VariableDeclarators
//
#line 2397 "java.g"
void Parser::Act132()
{
    MakeLocalVariable(DYNAMIC_CAST<AstModifiers*> (Sym(1)),
                      DYNAMIC_CAST<AstType*> (Sym(2)),
                      DYNAMIC_CAST<AstListNode*> (Sym(4)));
}

//
// Rule 133:  Statement ::= StatementWithoutTrailingSubstatement
//
// void NoAction();
//

//
// Rule 134:  Statement ::= LabeledStatement
//
// void NoAction();
//

//
// Rule 135:  Statement ::= IfThenStatement
//
// void NoAction();
//

//
// Rule 136:  Statement ::= IfThenElseStatement
//
// void NoAction();
//

//
// Rule 137:  Statement ::= WhileStatement
//
// void NoAction();
//

//
// Rule 138:  Statement ::= ForStatement
//
// void NoAction();
//

//
// Rule 139:  Statement ::= ForeachStatement
//
// void NoAction();
//

//
// Rule 140:  StatementNoShortIf ::= StatementWithoutTrailingSubstatement
//
// void NoAction();
//

//
// Rule 141:  StatementNoShortIf ::= LabeledStatementNoShortIf
//
// void NoAction();
//

//
// Rule 142:  StatementNoShortIf ::= IfThenElseStatementNoShortIf
//
// void NoAction();
//

//
// Rule 143:  StatementNoShortIf ::= WhileStatementNoShortIf
//
// void NoAction();
//

//
// Rule 144:  StatementNoShortIf ::= ForStatementNoShortIf
//
// void NoAction();
//

//
// Rule 145:  StatementNoShortIf ::= ForeachStatementNoShortIf
//
// void NoAction();
//

//
// Rule 146:  StatementWithoutTrailingSubstatement ::= Block
//
// void NoAction();
//

//
// Rule 147:  StatementWithoutTrailingSubstatement ::= EmptyStatement
//
// void NoAction();
//

//
// Rule 148:  StatementWithoutTrailingSubstatement ::= ExpressionStatement
//
// void NoAction();
//

//
// Rule 149:  StatementWithoutTrailingSubstatement ::= SwitchStatement
//
// void NoAction();
//

//
// Rule 150:  StatementWithoutTrailingSubstatement ::= DoStatement
//
// void NoAction();
//

//
// Rule 151:  StatementWithoutTrailingSubstatement ::= BreakStatement
//
// void NoAction();
//

//
// Rule 152:  StatementWithoutTrailingSubstatement ::= ContinueStatement
//
// void NoAction();
//

//
// Rule 153:  StatementWithoutTrailingSubstatement ::= ReturnStatement
//
// void NoAction();
//

//
// Rule 154:  StatementWithoutTrailingSubstatement ::= SynchronizedStatement
//
// void NoAction();
//

//
// Rule 155:  StatementWithoutTrailingSubstatement ::= ThrowStatement
//
// void NoAction();
//

//
// Rule 156:  StatementWithoutTrailingSubstatement ::= TryStatement
//
// void NoAction();
//

//
// Rule 157:  StatementWithoutTrailingSubstatement ::= AssertStatement
//
// void NoAction();
//

//
// Rule 158:  EmptyStatement ::= SEMICOLON
//
#line 2516 "java.g"
void Parser::Act158()
{
    Sym(1) = ast_pool -> NewEmptyStatement(Token(1));
}

//
// Rule 159:  LabeledStatement ::= Identifier COLON Statement
//
#line 2525 "java.g"
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
// Rule 160:  LabeledStatementNoShortIf ::= Identifier COLON StatementNoShortIf
//
// void MakeLabeledStatement();
//

//
// Rule 161:  ExpressionStatement ::= StatementExpression SEMICOLON
//
#line 2559 "java.g"
void Parser::Act161()
{
    DYNAMIC_CAST<AstExpressionStatement*> (Sym(1)) -> semicolon_token_opt =
        Token(2);
}

//
// Rule 162:  StatementExpression ::= Assignment
//
#line 2569 "java.g"
void Parser::MakeExpressionStatement()
{
    AstExpressionStatement* p = ast_pool -> NewExpressionStatement();
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
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
// Rule 169:  IfThenStatement ::= if LPAREN Expression RPAREN Statement Marker Marker
//
#line 2626 "java.g"
void Parser::MakeIfThenElseStatement()
{
    //
    // We wrap the true and false statements in a block, to make the semantic
    // pass easier.
    //
    AstIfStatement* p = ast_pool -> NewIfStatement();
    p -> if_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    p -> true_statement = MakeBlock(5);
    p -> false_statement_opt = Sym(7) ? MakeBlock(7) : NULL;
    Sym(1) = p;
}

AstBlock* Parser::MakeBlock(int tokennum)
{
    AstBlock* block = Sym(tokennum) -> BlockCast();
    if (! block)
    {
        block = ast_pool -> GenBlock();
        block -> AllocateStatements(1); // allocate 1 element
        block -> left_brace_token = Token(tokennum);
        block -> AddStatement(DYNAMIC_CAST<AstStatement*> (Sym(tokennum)));
        block -> right_brace_token = Sym(tokennum) -> RightToken();
    }
    return block;
}

//
// Rule 170:  IfThenElseStatement ::= if LPAREN Expression RPAREN StatementNoShortIf else Statement
//
// void MakeIfThenElseStatement();
//

//
// Rule 171:  IfThenElseStatementNoShortIf ::= if LPAREN Expression RPAREN StatementNoShortIf else StatementNoShortIf
//
// void MakeIfThenElseStatement();
//

//
// Rule 172:  SwitchStatement ::= switch LPAREN Expression RPAREN SwitchBlock
//
#line 2674 "java.g"
void Parser::Act172()
{
    AstSwitchStatement* p = DYNAMIC_CAST<AstSwitchStatement*> (Sym(5));
    p -> switch_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 173:  SwitchBlock ::= LBRACE SwitchBlockStatements SwitchLabelsopt RBRACE
//
#line 2692 "java.g"
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
#line 2719 "java.g"
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
// void StartList();
//

//
// Rule 176:  SwitchBlockStatements ::= SwitchBlockStatements SwitchBlockStatement
//
// void AddList2();
//

//
// Rule 177:  SwitchBlockStatement ::= SwitchLabels BlockStatements
//
#line 2748 "java.g"
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
    else
    {
        p -> AllocateStatements(1);
        p -> AddStatement(ast_pool -> GenEmptyStatement(labels ->
                                                        RightToken()));
    }
    p -> right_brace_token =
        p -> Statement(p -> NumStatements() - 1) -> RightToken();
    return p;
}

//
// Rule 178:  SwitchLabels ::= SwitchLabel
//
// void StartList();
//

//
// Rule 179:  SwitchLabels ::= SwitchLabels SwitchLabel
//
// void AddList2();
//

//
// Rule 180:  SwitchLabel ::= case Expression COLON
//
#line 2807 "java.g"
void Parser::MakeSwitchLabel()
{
    AstSwitchLabel* p = ast_pool -> NewSwitchLabel();
    p -> case_token = Token(1);
    p -> expression_opt = DYNAMIC_CAST<AstExpression*> (Sym(2));
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
#line 2831 "java.g"
void Parser::MakeWhileStatement()
{
    //
    // We wrap the loop statement in a block, to make the semantic pass easier.
    //
    AstWhileStatement* p = ast_pool -> NewWhileStatement();
    p -> while_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    p -> statement = MakeBlock(5);

    //
    // We also wrap the loop in a block, to make the semantic pass easier.
    //
    AstBlock* block = ast_pool -> GenBlock();
    block -> AllocateStatements(1); // allocate 1 element
    block -> left_brace_token = Token(1);
    block -> AddStatement(p);
    block -> right_brace_token = Sym(5) -> RightToken();
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
#line 2863 "java.g"
void Parser::Act184()
{
    //
    // We wrap the loop statement in a block, to make the semantic pass easier.
    //
    AstDoStatement* p = ast_pool -> NewDoStatement();
    p -> do_token = Token(1);
    p -> statement = MakeBlock(2);
    p -> while_token = Token(3);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(5));
    p -> semicolon_token = Token(7);

    //
    // We also wrap the loop in a block, to make the semantic pass easier.
    //
    AstBlock* block = ast_pool -> GenBlock();
    block -> AllocateStatements(1); // allocate 1 element
    block -> left_brace_token = Token(1);
    block -> AddStatement(p);
    block -> right_brace_token = Token(7);
    Sym(1) = block;
}

//
// Rule 185:  ForStatement ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN Statement
//
#line 2891 "java.g"
void Parser::MakeForStatement()
{
    //
    // We wrap the loop statement in a block, to make the semantic pass easier.
    //
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
    p -> statement = MakeBlock(9);

    //
    // We also wrap the loop in a block, to make the semantic pass easier. In
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
// Rule 186:  ForStatementNoShortIf ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN...
//
// void MakeForStatement();
//

//
// Rule 187:  ForInit ::= StatementExpressionList
//
// void NoAction();
//

//
// Rule 188:  ForInit ::= LocalVariableDeclaration
//
// void StartList();
//

//
// Rule 189:  ForUpdate ::= StatementExpressionList
//
// void NoAction();
//

//
// Rule 190:  StatementExpressionList ::= StatementExpression
//
// void StartList();
//

//
// Rule 191:  StatementExpressionList ::= StatementExpressionList COMMA StatementExpression
//
// void AddList3();
//

//
// Rule 192:  ForeachStatement ::= for LPAREN FormalParameter COLON Expression RPAREN Statement
//
#line 2972 "java.g"
void Parser::MakeForeachStatement()
{
    //
    // We wrap the loop statement in a block, to make the semantic pass easier.
    //
    AstForeachStatement* p = ast_pool -> NewForeachStatement();
    p -> for_token = Token(1);
    p -> formal_parameter = DYNAMIC_CAST<AstFormalParameter*> (Sym(3));
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(5));
    p -> statement = MakeBlock(7);

    //
    // We also wrap the loop in a block, to make the semantic pass easier. In
    // particular, this lets us correctly handle
    // "for(int i:new int[0]);for(int i::new int[0]);".
    //
    AstBlock* block = ast_pool -> NewBlock();
    block -> AllocateStatements(1); // allocate 1 element
    block -> left_brace_token = Token(1);
    block -> AddStatement(p);
    block -> right_brace_token = Sym(7) -> RightToken();
    block -> no_braces = true;
    Sym(1) = block;
}

//
// Rule 193:  ForeachStatementNoShortIf ::= for LPAREN FormalParameter COLON Expression RPAREN StatementNoShortIf
//
// void MakeForeachStatement();
//

//
// Rule 194:  AssertStatement ::= assert Expression Marker Marker SEMICOLON
//
#line 3014 "java.g"
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
// Rule 195:  AssertStatement ::= assert Expression COLON Expression SEMICOLON
//
// void MakeAssertStatement();
//

//
// Rule 196:  BreakStatement ::= break Identifieropt SEMICOLON
//
#line 3035 "java.g"
void Parser::Act196()
{
    AstBreakStatement* p = ast_pool -> NewBreakStatement();
    p -> break_token = Token(1);
    p -> identifier_token_opt = Token(2) == Token(3) ? 0 : Token(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 197:  ContinueStatement ::= continue Identifieropt SEMICOLON
//
#line 3048 "java.g"
void Parser::Act197()
{
    AstContinueStatement* p = ast_pool -> NewContinueStatement();
    p -> continue_token = Token(1);
    p -> identifier_token_opt = Token(2) == Token(3) ? 0 : Token(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 198:  ReturnStatement ::= return Expressionopt SEMICOLON
//
#line 3061 "java.g"
void Parser::Act198()
{
    AstReturnStatement* p = ast_pool -> NewReturnStatement();
    p -> return_token = Token(1);
    p -> expression_opt = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 199:  ThrowStatement ::= throw Expression SEMICOLON
//
#line 3074 "java.g"
void Parser::Act199()
{
    AstThrowStatement* p = ast_pool -> NewThrowStatement();
    p -> throw_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 200:  SynchronizedStatement ::= synchronized LPAREN Expression RPAREN Block
//
#line 3087 "java.g"
void Parser::Act200()
{
    AstSynchronizedStatement* p = ast_pool -> NewSynchronizedStatement();
    p -> synchronized_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(5));
    p -> block -> block_tag = AstBlock::SYNCHRONIZED;

    Sym(1) = p;
}

//
// Rule 201:  TryStatement ::= try Block Catches Marker
//
#line 3106 "java.g"
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
// Rule 202:  TryStatement ::= try Block Catchesopt Finally
//
// void MakeTryStatement();
//

//
// Rule 203:  Catches ::= CatchClause
//
// void StartList();
//

//
// Rule 204:  Catches ::= Catches CatchClause
//
// void AddList2();
//

//
// Rule 205:  CatchClause ::= catch LPAREN FormalParameter RPAREN Block
//
#line 3154 "java.g"
void Parser::Act205()
{
    AstCatchClause* p = ast_pool -> NewCatchClause();
    p -> catch_token = Token(1);
    p -> formal_parameter = DYNAMIC_CAST<AstFormalParameter*> (Sym(3));
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(5));

    Sym(1) = p;
}

//
// Rule 206:  Finally ::= finally Block
//
#line 3168 "java.g"
void Parser::Act206()
{
    AstFinallyClause* p = ast_pool -> NewFinallyClause();
    p -> finally_token = Token(1);
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(2));
    p -> block -> block_tag = AstBlock::FINALLY;

    Sym(1) = p;
}

//
// Rule 207:  Primary ::= PrimaryNoNewArray
//
// void NoAction();
//

//
// Rule 208:  Primary ::= ArrayCreationUninitialized
//
// void NoAction();
//

//
// Rule 209:  Primary ::= ArrayCreationInitialized
//
// void NoAction();
//

//
// Rule 210:  PrimaryNoNewArray ::= Literal
//
// void NoAction();
//

//
// Rule 211:  PrimaryNoNewArray ::= this
//
#line 3207 "java.g"
void Parser::Act211()
{
    Sym(1) = ast_pool -> NewThisExpression(Token(1));
}

//
// Rule 212:  PrimaryNoNewArray ::= LPAREN Name Marker RPAREN
//
#line 3224 "java.g"
void Parser::MakeParenthesizedExpression()
{
    AstParenthesizedExpression* p = ast_pool -> NewParenthesizedExpression();
    p -> left_parenthesis_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> right_parenthesis_token = Token(4);
    Sym(1) = p;
}

//
// Rule 213:  PrimaryNoNewArray ::= LPAREN ExpressionNotName Marker RPAREN
//
// void MakeParenthesizedExpression();
//

//
// Rule 214:  PrimaryNoNewArray ::= ClassInstanceCreationExpression
//
// void NoAction();
//

//
// Rule 215:  PrimaryNoNewArray ::= FieldAccess
//
// void NoAction();
//

//
// Rule 216:  PrimaryNoNewArray ::= Name DOT this
//
#line 3260 "java.g"
void Parser::Act216()
{
    AstThisExpression* p = ast_pool -> NewThisExpression(Token(3));
    p -> base_opt = ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(1)));
    Sym(1) = p;
}

//
// Rule 217:  PrimaryNoNewArray ::= PrimitiveType Dimsopt DOT class
//
#line 3276 "java.g"
void Parser::MakeClassLiteral()
{
    AstClassLiteral* p = ast_pool -> NewClassLiteral(Token(4));
    p -> type = MakeArrayType(1);
    Sym(1) = p;
}

//
// Rule 218:  PrimaryNoNewArray ::= Name Dims DOT class
//
// void MakeClassLiteral();
//

//
// Rule 219:  PrimaryNoNewArray ::= Name DOT Marker class
//
#line 3297 "java.g"
void Parser::Act219()
{
    Sym(2) = NULL;
    MakeClassLiteral();
}

//
// Rule 220:  PrimaryNoNewArray ::= void DOT Marker class
//
#line 3311 "java.g"
void Parser::Act220()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::VOID_TYPE, Token(1));
    Sym(2) = NULL;
    MakeClassLiteral();
}

//
// Rule 221:  PrimaryNoNewArray ::= MethodInvocation
//
// void NoAction();
//

//
// Rule 222:  PrimaryNoNewArray ::= ArrayAccess
//
// void NoAction();
//

//
// Rule 223:  ClassInstanceCreationExpression ::= new ClassOrInterfaceType LPAREN ArgumentListopt RPAREN ClassBodyopt
//
#line 3336 "java.g"
void Parser::Act223()
{
    AstClassInstanceCreationExpression* p =
        ast_pool -> NewClassInstanceCreationExpression();
    p -> new_token = Token(1);
    p -> class_type = DYNAMIC_CAST<AstTypeName*> (Sym(2));
    p -> arguments = MakeArguments(3);
    p -> class_body_opt = DYNAMIC_CAST<AstClassBody*> (Sym(6));
    if (p -> class_body_opt)
        p -> class_body_opt -> identifier_token =
            p -> class_type -> IdentifierToken();
    Sym(1) = p;
}

//
// Rule 224:  ClassInstanceCreationExpression ::= Primary DOT new Identifier Marker LPAREN ArgumentListopt RPAREN...
//
#line 3362 "java.g"
void Parser::MakeQualifiedNew()
{
    AstClassInstanceCreationExpression* p =
        ast_pool -> NewClassInstanceCreationExpression();
    p -> base_opt = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> new_token = Token(3);
    p -> class_type = ast_pool -> NewTypeName(MakeSimpleName(4));
    p -> arguments = MakeArguments(6);
    p -> class_body_opt = DYNAMIC_CAST<AstClassBody*> (Sym(9));
    if (p -> class_body_opt)
        p -> class_body_opt -> identifier_token = Token(4);
    Sym(1) = p;
}

//
// Rule 225:  ClassInstanceCreationExpression ::= Name DOT new Identifier Marker LPAREN ArgumentListopt RPAREN ClassBodyopt
//
// void MakeQualifiedNew();
//

//
// Rule 226:  ArgumentList ::= Expression
//
// void StartList();
//

//
// Rule 227:  ArgumentList ::= ArgumentList COMMA Expression
//
// void AddList3();
//

//
// Rule 228:  ArrayCreationUninitialized ::= new PrimitiveType DimExprs Dimsopt
//
#line 3409 "java.g"
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
    p -> brackets_opt = DYNAMIC_CAST<AstBrackets*> (Sym(4));
    Sym(1) = p;
}

//
// Rule 229:  ArrayCreationUninitialized ::= new ClassOrInterfaceType DimExprs Dimsopt
//
// void MakeArrayCreationUninitialized();
//

//
// Rule 230:  ArrayCreationInitialized ::= new ArrayType ArrayInitializer
//
#line 3442 "java.g"
void Parser::Act230()
{
    AstArrayCreationExpression* p = ast_pool -> NewArrayCreationExpression();
    p -> new_token = Token(1);
    p -> array_type = DYNAMIC_CAST<AstType*> (Sym(2));
    p -> array_initializer_opt = DYNAMIC_CAST<AstArrayInitializer*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 231:  DimExprs ::= DimExpr
//
// void StartList();
//

//
// Rule 232:  DimExprs ::= DimExprs DimExpr
//
// void AddList2();
//

//
// Rule 233:  DimExpr ::= LBRACKET Expression RBRACKET
//
#line 3463 "java.g"
void Parser::Act233()
{
    AstDimExpr* p = ast_pool -> NewDimExpr();
    p -> left_bracket_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> right_bracket_token = Token(3);
    Sym(1) = p;
}

//
// Rule 234:  Dims ::= LBRACKET RBRACKET
//
#line 3476 "java.g"
void Parser::Act234()
{
    Sym(1) = ast_pool -> NewBrackets(Token(1), Token(2));
}

//
// Rule 235:  Dims ::= Dims LBRACKET RBRACKET
//
#line 3485 "java.g"
void Parser::Act235()
{
    AstBrackets* p = DYNAMIC_CAST<AstBrackets*> (Sym(1));
    p -> right_bracket_token = Token(2);
    p -> dims++;
}

//
// Rule 236:  FieldAccess ::= Primary DOT Identifier
//
#line 3496 "java.g"
void Parser::MakeFieldAccess()
{
    AstFieldAccess* p = ast_pool -> NewFieldAccess();
    p -> base = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> identifier_token = Token(3);
    Sym(1) = p;
}

//
// Rule 237:  FieldAccess ::= super DOT Identifier
//
#line 3508 "java.g"
void Parser::MakeSuperFieldAccess()
{
    Sym(1) = ast_pool -> NewSuperExpression(Token(1));
    MakeFieldAccess();
}

//
// Rule 238:  FieldAccess ::= Name DOT super DOT Identifier
//
#line 3524 "java.g"
void Parser::MakeQualifiedSuperFieldAccess()
{
    AstSuperExpression* q = ast_pool -> NewSuperExpression(Token(3));
    q -> base_opt = ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(1)));
    AstFieldAccess* p = ast_pool -> NewFieldAccess();
    p -> base = q;
    p -> identifier_token = Token(5);
    Sym(1) = p;
}

//
// Rule 239:  MethodInvocation ::= Name LPAREN ArgumentListopt RPAREN
//
#line 3538 "java.g"
void Parser::Act239() { MakeMethodInvocation(2); }

//
// This function treats Sym(1) as a method name, and builds the method
// invocation starting with the '('.
//
void Parser::MakeMethodInvocation(int tokennum)
{
    AstMethodInvocation* p = ast_pool -> NewMethodInvocation();
    p -> method = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> arguments = MakeArguments(tokennum);
    Sym(1) = p;
}

//
// Rule 240:  MethodInvocation ::= Primary DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3556 "java.g"
void Parser::Act240()
{
    MakeFieldAccess();
    MakeMethodInvocation(4);
}

//
// Rule 241:  MethodInvocation ::= super DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3566 "java.g"
void Parser::Act241()
{
    MakeSuperFieldAccess();
    MakeMethodInvocation(4);
}

//
// Rule 242:  MethodInvocation ::= Name DOT super DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3583 "java.g"
void Parser::Act242()
{
    MakeQualifiedSuperFieldAccess();
    MakeMethodInvocation(6);
}

//
// Rule 243:  ArrayAccess ::= Name LBRACKET Expression RBRACKET
//
#line 3593 "java.g"
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
// Rule 244:  ArrayAccess ::= PrimaryNoNewArray LBRACKET Expression RBRACKET
//
// void MakeArrayAccess();
//

//
// Rule 245:  ArrayAccess ::= ArrayCreationInitialized LBRACKET Expression RBRACKET
//
// void MakeArrayAccess();
//

//
// Rule 246:  PostfixExpression ::= Primary
//
// void NoAction();
//

//
// Rule 247:  PostfixExpression ::= Name
//
// void NoAction();
//

//
// Rule 248:  PostfixExpression ::= PostIncrementExpression
//
// void NoAction();
//

//
// Rule 249:  PostfixExpression ::= PostDecrementExpression
//
// void NoAction();
//

//
// Rule 250:  PostfixExpressionNotName ::= Primary
//
// void NoAction();
//

//
// Rule 251:  PostfixExpressionNotName ::= PostIncrementExpression
//
// void NoAction();
//

//
// Rule 252:  PostfixExpressionNotName ::= PostDecrementExpression
//
// void NoAction();
//

//
// Rule 253:  PostIncrementExpression ::= PostfixExpression PLUS_PLUS
//
#line 3653 "java.g"
void Parser::Act253()
{
    AstPostUnaryExpression* p =
        ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::PLUSPLUS);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}

//
// Rule 254:  PostDecrementExpression ::= PostfixExpression MINUS_MINUS
//
#line 3666 "java.g"
void Parser::Act254()
{
    AstPostUnaryExpression* p =
        ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::MINUSMINUS);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}

//
// Rule 255:  UnaryExpression ::= PreIncrementExpression
//
// void NoAction();
//

//
// Rule 256:  UnaryExpression ::= PreDecrementExpression
//
// void NoAction();
//

//
// Rule 257:  UnaryExpression ::= PLUS UnaryExpression
//
#line 3687 "java.g"
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
// Rule 258:  UnaryExpression ::= MINUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 259:  UnaryExpression ::= UnaryExpressionNotPlusMinus
//
// void NoAction();
//

//
// Rule 260:  UnaryExpressionNotName ::= PreIncrementExpression
//
// void NoAction();
//

//
// Rule 261:  UnaryExpressionNotName ::= PreDecrementExpression
//
// void NoAction();
//

//
// Rule 262:  UnaryExpressionNotName ::= PLUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 263:  UnaryExpressionNotName ::= MINUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 264:  UnaryExpressionNotName ::= UnaryExpressionNotPlusMinusNotName
//
// void NoAction();
//

//
// Rule 265:  PreIncrementExpression ::= PLUS_PLUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 266:  PreDecrementExpression ::= MINUS_MINUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 267:  UnaryExpressionNotPlusMinus ::= PostfixExpression
//
// void NoAction();
//

//
// Rule 268:  UnaryExpressionNotPlusMinus ::= TWIDDLE UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 269:  UnaryExpressionNotPlusMinus ::= NOT UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 270:  UnaryExpressionNotPlusMinus ::= CastExpression
//
// void NoAction();
//

//
// Rule 271:  UnaryExpressionNotPlusMinusNotName ::= PostfixExpressionNotName
//
// void NoAction();
//

//
// Rule 272:  UnaryExpressionNotPlusMinusNotName ::= TWIDDLE UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 273:  UnaryExpressionNotPlusMinusNotName ::= NOT UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 274:  UnaryExpressionNotPlusMinusNotName ::= CastExpression
//
// void NoAction();
//

//
// Rule 275:  CastExpression ::= LPAREN PrimitiveType Dimsopt RPAREN UnaryExpression
//
#line 3794 "java.g"
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
// Rule 276:  CastExpression ::= LPAREN Name Marker RPAREN UnaryExpressionNotPlusMinus
//
// void MakeCastExpression();
//

//
// Rule 277:  CastExpression ::= LPAREN Name Dims RPAREN UnaryExpressionNotPlusMinus
//
// void MakeCastExpression();
//

//
// Rule 278:  MultiplicativeExpression ::= UnaryExpression
//
// void NoAction();
//

//
// Rule 279:  MultiplicativeExpression ::= MultiplicativeExpression MULTIPLY UnaryExpression
//
#line 3835 "java.g"
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
// Rule 280:  MultiplicativeExpression ::= MultiplicativeExpression DIVIDE UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 281:  MultiplicativeExpression ::= MultiplicativeExpression REMAINDER UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 282:  MultiplicativeExpressionNotName ::= UnaryExpressionNotName
//
// void NoAction();
//

//
// Rule 283:  MultiplicativeExpressionNotName ::= MultiplicativeExpressionNotName MULTIPLY UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 284:  MultiplicativeExpressionNotName ::= Name MULTIPLY UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 285:  MultiplicativeExpressionNotName ::= MultiplicativeExpressionNotName DIVIDE UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 286:  MultiplicativeExpressionNotName ::= Name DIVIDE UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 287:  MultiplicativeExpressionNotName ::= MultiplicativeExpressionNotName REMAINDER UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 288:  MultiplicativeExpressionNotName ::= Name REMAINDER UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 289:  AdditiveExpression ::= MultiplicativeExpression
//
// void NoAction();
//

//
// Rule 290:  AdditiveExpression ::= AdditiveExpression PLUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 291:  AdditiveExpression ::= AdditiveExpression MINUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 292:  AdditiveExpressionNotName ::= MultiplicativeExpressionNotName
//
// void NoAction();
//

//
// Rule 293:  AdditiveExpressionNotName ::= AdditiveExpressionNotName PLUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 294:  AdditiveExpressionNotName ::= Name PLUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 295:  AdditiveExpressionNotName ::= AdditiveExpressionNotName MINUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 296:  AdditiveExpressionNotName ::= Name MINUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 297:  ShiftExpression ::= AdditiveExpression
//
// void NoAction();
//

//
// Rule 298:  ShiftExpression ::= ShiftExpression LEFT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 299:  ShiftExpression ::= ShiftExpression RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 300:  ShiftExpression ::= ShiftExpression UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 301:  ShiftExpressionNotName ::= AdditiveExpressionNotName
//
// void NoAction();
//

//
// Rule 302:  ShiftExpressionNotName ::= ShiftExpressionNotName LEFT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 303:  ShiftExpressionNotName ::= Name LEFT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 304:  ShiftExpressionNotName ::= ShiftExpressionNotName RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 305:  ShiftExpressionNotName ::= Name RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 306:  ShiftExpressionNotName ::= ShiftExpressionNotName UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 307:  ShiftExpressionNotName ::= Name UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 308:  RelationalExpression ::= ShiftExpression
//
// void NoAction();
//

//
// Rule 309:  RelationalExpression ::= RelationalExpression LESS ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 310:  RelationalExpression ::= RelationalExpression GREATER ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 311:  RelationalExpression ::= RelationalExpression LESS_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 312:  RelationalExpression ::= RelationalExpression GREATER_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 313:  RelationalExpression ::= RelationalExpression instanceof ReferenceType
//
#line 4013 "java.g"
void Parser::MakeInstanceofExpression()
{
    AstInstanceofExpression* p = ast_pool -> NewInstanceofExpression();
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> instanceof_token = Token(2);
    p -> type = DYNAMIC_CAST<AstType*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 314:  RelationalExpressionNotName ::= ShiftExpressionNotName
//
// void NoAction();
//

//
// Rule 315:  RelationalExpressionNotName ::= RelationalExpressionNotName LESS ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 316:  RelationalExpressionNotName ::= Name LESS ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 317:  RelationalExpressionNotName ::= RelationalExpressionNotName GREATER ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 318:  RelationalExpressionNotName ::= Name GREATER ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 319:  RelationalExpressionNotName ::= RelationalExpressionNotName LESS_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 320:  RelationalExpressionNotName ::= Name LESS_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 321:  RelationalExpressionNotName ::= RelationalExpressionNotName GREATER_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 322:  RelationalExpressionNotName ::= Name GREATER_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 323:  RelationalExpressionNotName ::= RelationalExpressionNotName instanceof ReferenceType
//
// void MakeInstanceofExpression();
//

//
// Rule 324:  RelationalExpressionNotName ::= Name instanceof ReferenceType
//
// void MakeInstanceofExpression();
//

//
// Rule 325:  EqualityExpression ::= RelationalExpression
//
// void NoAction();
//

//
// Rule 326:  EqualityExpression ::= EqualityExpression EQUAL_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 327:  EqualityExpression ::= EqualityExpression NOT_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 328:  EqualityExpressionNotName ::= RelationalExpressionNotName
//
// void NoAction();
//

//
// Rule 329:  EqualityExpressionNotName ::= EqualityExpressionNotName EQUAL_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 330:  EqualityExpressionNotName ::= Name EQUAL_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 331:  EqualityExpressionNotName ::= EqualityExpressionNotName NOT_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 332:  EqualityExpressionNotName ::= Name NOT_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 333:  AndExpression ::= EqualityExpression
//
// void NoAction();
//

//
// Rule 334:  AndExpression ::= AndExpression AND EqualityExpression
//
// void MakeBinaryExpression();
//

//
// Rule 335:  AndExpressionNotName ::= EqualityExpressionNotName
//
// void NoAction();
//

//
// Rule 336:  AndExpressionNotName ::= AndExpressionNotName AND EqualityExpression
//
// void MakeBinaryExpression();
//

//
// Rule 337:  AndExpressionNotName ::= Name AND EqualityExpression
//
// void MakeBinaryExpression();
//

//
// Rule 338:  ExclusiveOrExpression ::= AndExpression
//
// void NoAction();
//

//
// Rule 339:  ExclusiveOrExpression ::= ExclusiveOrExpression XOR AndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 340:  ExclusiveOrExpressionNotName ::= AndExpressionNotName
//
// void NoAction();
//

//
// Rule 341:  ExclusiveOrExpressionNotName ::= ExclusiveOrExpressionNotName XOR AndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 342:  ExclusiveOrExpressionNotName ::= Name XOR AndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 343:  InclusiveOrExpression ::= ExclusiveOrExpression
//
// void NoAction();
//

//
// Rule 344:  InclusiveOrExpression ::= InclusiveOrExpression OR ExclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 345:  InclusiveOrExpressionNotName ::= ExclusiveOrExpressionNotName
//
// void NoAction();
//

//
// Rule 346:  InclusiveOrExpressionNotName ::= InclusiveOrExpressionNotName OR ExclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 347:  InclusiveOrExpressionNotName ::= Name OR ExclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 348:  ConditionalAndExpression ::= InclusiveOrExpression
//
// void NoAction();
//

//
// Rule 349:  ConditionalAndExpression ::= ConditionalAndExpression AND_AND InclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 350:  ConditionalAndExpressionNotName ::= InclusiveOrExpressionNotName
//
// void NoAction();
//

//
// Rule 351:  ConditionalAndExpressionNotName ::= ConditionalAndExpressionNotName AND_AND InclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 352:  ConditionalAndExpressionNotName ::= Name AND_AND InclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 353:  ConditionalOrExpression ::= ConditionalAndExpression
//
// void NoAction();
//

//
// Rule 354:  ConditionalOrExpression ::= ConditionalOrExpression OR_OR ConditionalAndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 355:  ConditionalOrExpressionNotName ::= ConditionalAndExpressionNotName
//
// void NoAction();
//

//
// Rule 356:  ConditionalOrExpressionNotName ::= ConditionalOrExpressionNotName OR_OR ConditionalAndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 357:  ConditionalOrExpressionNotName ::= Name OR_OR ConditionalAndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 358:  ConditionalExpression ::= ConditionalOrExpression
//
// void NoAction();
//

//
// Rule 359:  ConditionalExpression ::= ConditionalOrExpression QUESTION Expression COLON ConditionalExpression
//
#line 4223 "java.g"
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
// Rule 360:  ConditionalExpressionNotName ::= ConditionalOrExpressionNotName
//
// void NoAction();
//

//
// Rule 361:  ConditionalExpressionNotName ::= ConditionalOrExpressionNotName QUESTION Expression COLON...
//
// void MakeConditionalExpression();
//

//
// Rule 362:  ConditionalExpressionNotName ::= Name QUESTION Expression COLON ConditionalExpression
//
// void MakeConditionalExpression();
//

//
// Rule 363:  AssignmentExpression ::= ConditionalExpression
//
// void NoAction();
//

//
// Rule 364:  AssignmentExpression ::= Assignment
//
// void NoAction();
//

//
// Rule 365:  AssignmentExpressionNotName ::= ConditionalExpressionNotName
//
// void NoAction();
//

//
// Rule 366:  AssignmentExpressionNotName ::= Assignment
//
// void NoAction();
//

//
// Rule 367:  Assignment ::= PostfixExpression AssignmentOperator AssignmentExpression
//
#line 4280 "java.g"
void Parser::Act367()
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
// Rule 368:  AssignmentOperator ::= EQUAL
//
// void NoAction();
//

//
// Rule 369:  AssignmentOperator ::= MULTIPLY_EQUAL
//
// void NoAction();
//

//
// Rule 370:  AssignmentOperator ::= DIVIDE_EQUAL
//
// void NoAction();
//

//
// Rule 371:  AssignmentOperator ::= REMAINDER_EQUAL
//
// void NoAction();
//

//
// Rule 372:  AssignmentOperator ::= PLUS_EQUAL
//
// void NoAction();
//

//
// Rule 373:  AssignmentOperator ::= MINUS_EQUAL
//
// void NoAction();
//

//
// Rule 374:  AssignmentOperator ::= LEFT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 375:  AssignmentOperator ::= RIGHT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 376:  AssignmentOperator ::= UNSIGNED_RIGHT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 377:  AssignmentOperator ::= AND_EQUAL
//
// void NoAction();
//

//
// Rule 378:  AssignmentOperator ::= XOR_EQUAL
//
// void NoAction();
//

//
// Rule 379:  AssignmentOperator ::= OR_EQUAL
//
// void NoAction();
//

//
// Rule 380:  Expression ::= AssignmentExpression
//
// void NoAction();
//

//
// Rule 381:  ExpressionNotName ::= AssignmentExpressionNotName
//
// void NoAction();
//

//
// Rule 382:  ,opt ::=
//
// void NoAction();
//

//
// Rule 383:  ,opt ::= COMMA
//
// void NoAction();
//

//
// Rule 384:  Identifieropt ::=
//
// void NoAction();
//

//
// Rule 385:  Identifieropt ::= Identifier
//
// void NoAction();
//

//
// Rule 386:  PackageDeclarationopt ::=
//
#line 4404 "java.g"
//
// Given a rule of the form A ::= x1 x2 ... xn
//
// Construct a NULL Ast for A.
//
void Parser::NullAction() { Sym(1) = NULL; }

//
// Rule 387:  PackageDeclarationopt ::= PackageDeclaration
//
// void NoAction();
//

//
// Rule 388:  Superopt ::=
//
// void NullAction();
//

//
// Rule 389:  Superopt ::= Super
//
// void NoAction();
//

//
// Rule 390:  Expressionopt ::=
//
// void NullAction();
//

//
// Rule 391:  Expressionopt ::= Expression
//
// void NoAction();
//

//
// Rule 392:  ClassBodyopt ::=
//
// void NullAction();
//

//
// Rule 393:  ClassBodyopt ::= ClassBody
//
// void NoAction();
//

//
// Rule 394:  ImportDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 395:  ImportDeclarationsopt ::= ImportDeclarations
//
// void NoAction();
//

//
// Rule 396:  TypeDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 397:  TypeDeclarationsopt ::= TypeDeclarations
//
// void NoAction();
//

//
// Rule 398:  ClassBodyDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 399:  ClassBodyDeclarationsopt ::= ClassBodyDeclarations
//
// void NoAction();
//

//
// Rule 400:  Modifiersopt ::=
//
// void NullAction();
//

//
// Rule 401:  Modifiersopt ::= Modifiers
//
// void NoAction();
//

//
// Rule 402:  BlockStatementsopt ::=
//
// void NullAction();
//

//
// Rule 403:  BlockStatementsopt ::= BlockStatements
//
// void NoAction();
//

//
// Rule 404:  Dimsopt ::=
//
// void NullAction();
//

//
// Rule 405:  Dimsopt ::= Dims
//
// void NoAction();
//

//
// Rule 406:  ArgumentListopt ::=
//
// void NullAction();
//

//
// Rule 407:  ArgumentListopt ::= ArgumentList
//
// void NoAction();
//

//
// Rule 408:  SwitchLabelsopt ::=
//
// void NullAction();
//

//
// Rule 409:  SwitchLabelsopt ::= SwitchLabels
//
// void NoAction();
//

//
// Rule 410:  Throwsopt ::=
//
// void NullAction();
//

//
// Rule 411:  Throwsopt ::= Throws
//
// void NoAction();
//

//
// Rule 412:  FormalParameterListopt ::=
//
// void NullAction();
//

//
// Rule 413:  FormalParameterListopt ::= FormalParameterList
//
// void NoAction();
//

//
// Rule 414:  Interfacesopt ::=
//
// void NullAction();
//

//
// Rule 415:  Interfacesopt ::= Interfaces
//
// void NoAction();
//

//
// Rule 416:  InterfaceMemberDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 417:  InterfaceMemberDeclarationsopt ::= InterfaceMemberDeclarations
//
// void NoAction();
//

//
// Rule 418:  ForInitopt ::=
//
// void NullAction();
//

//
// Rule 419:  ForInitopt ::= ForInit
//
// void NoAction();
//

//
// Rule 420:  ForUpdateopt ::=
//
// void NullAction();
//

//
// Rule 421:  ForUpdateopt ::= ForUpdate
//
// void NoAction();
//

//
// Rule 422:  ExtendsInterfacesopt ::=
//
// void NullAction();
//

//
// Rule 423:  ExtendsInterfacesopt ::= ExtendsInterfaces
//
// void NoAction();
//

//
// Rule 424:  Catchesopt ::=
//
// void NullAction();
//

//
// Rule 425:  Catchesopt ::= Catches
//
// void NoAction();
//

//
// Rule 426:  PackageHeaderMarker ::=
//
#line 4571 "java.g"
//
// When this function is invoked, if the "parse_package_header_only" flag
// is turned on, we skip to the end-of-file token.
//
void Parser::Act426()
{
    if (parse_package_header_only)
        // point to the EOF token
        lex_stream -> Reset(lex_stream -> NumTokens() - 1);
    Sym(1) = NULL;
}

//
// Rule 427:  MethodHeaderMarker ::=
//
#line 4587 "java.g"
//
// When this function is invoked, if the "parse_header_only" flag
// is turned on, the body of the method being parsed is skipped.
//
void Parser::Act427()
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
// Rule 428:  Marker ::=
//
// void NullAction();
//
#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif
