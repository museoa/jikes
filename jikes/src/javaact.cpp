#line 699 "java.g"
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
#line 742 "java.g"
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
#line 760 "java.g"
//
// Given a rule of the form A ::= x1 x2, inherit the result from x2.
//
void Parser::SetSym1ToSym2() { Sym(1) = Sym(2); }

//
// Rule 3:  Literal ::= IntegerLiteral
//
#line 772 "java.g"
void Parser::Act3()
{
    Sym(1) = ast_pool -> NewIntegerLiteral(Token(1));
}

//
// Rule 4:  Literal ::= LongLiteral
//
#line 781 "java.g"
void Parser::Act4()
{
    Sym(1) = ast_pool -> NewLongLiteral(Token(1));
}

//
// Rule 5:  Literal ::= FloatLiteral
//
#line 790 "java.g"
void Parser::Act5()
{
    Sym(1) = ast_pool -> NewFloatLiteral(Token(1));
}

//
// Rule 6:  Literal ::= DoubleLiteral
//
#line 799 "java.g"
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
#line 812 "java.g"
void Parser::Act8()
{
    Sym(1) = ast_pool -> NewCharacterLiteral(Token(1));
}

//
// Rule 9:  Literal ::= StringLiteral
//
#line 821 "java.g"
void Parser::Act9()
{
    Sym(1) = ast_pool -> NewStringLiteral(Token(1));
}

//
// Rule 10:  Literal ::= null
//
#line 830 "java.g"
void Parser::Act10()
{
    Sym(1) = ast_pool -> NewNullLiteral(Token(1));
}

//
// Rule 11:  BooleanLiteral ::= true
//
#line 839 "java.g"
void Parser::Act11()
{
    Sym(1) = ast_pool -> NewTrueLiteral(Token(1));
}

//
// Rule 12:  BooleanLiteral ::= false
//
#line 848 "java.g"
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
#line 871 "java.g"
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
#line 888 "java.g"
void Parser::Act19()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::BYTE, Token(1));
}

//
// Rule 20:  IntegralType ::= short
//
#line 897 "java.g"
void Parser::Act20()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::SHORT, Token(1));
}

//
// Rule 21:  IntegralType ::= int
//
#line 906 "java.g"
void Parser::Act21()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::INT, Token(1));
}

//
// Rule 22:  IntegralType ::= long
//
#line 915 "java.g"
void Parser::Act22()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::LONG, Token(1));
}

//
// Rule 23:  IntegralType ::= char
//
#line 924 "java.g"
void Parser::Act23()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::CHAR, Token(1));
}

//
// Rule 24:  FloatingPointType ::= float
//
#line 933 "java.g"
void Parser::Act24()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::FLOAT, Token(1));
}

//
// Rule 25:  FloatingPointType ::= double
//
#line 942 "java.g"
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
#line 966 "java.g"
void Parser::Act28()
{
    Sym(1) = ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(1)));
}

//
// Rule 29:  ArrayType ::= PrimitiveType Dims
//
#line 986 "java.g"
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
#line 1021 "java.g"
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
#line 1037 "java.g"
void Parser::Act32()
{
    AstName* p = ast_pool -> NewName(Token(3));
    p -> base_opt = DYNAMIC_CAST<AstName*> (Sym(1));
    Sym(1) = p;
}

//
// Rule 33:  CompilationUnit ::= PackageDeclaration ImportDeclarationsopt TypeDeclarationsopt
//
#line 1058 "java.g"
void Parser::MakeCompilationUnit()
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
// Rule 34:  CompilationUnit ::= Marker ImportDeclarations TypeDeclarationsopt
//
// void MakeCompilationUnit();
//

//
// Rule 35:  CompilationUnit ::= TypeDeclarationsopt
//
#line 1109 "java.g"
void Parser::Act35()
{
    Sym(3) = Sym(1);
    Sym(1) = NULL;
    Sym(2) = NULL;
    MakeCompilationUnit();
}

//
// Rule 36:  ImportDeclarations ::= ImportDeclaration
//
#line 1121 "java.g"
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
// Rule 37:  ImportDeclarations ::= ImportDeclarations ImportDeclaration
//
#line 1138 "java.g"
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
// Rule 38:  TypeDeclarations ::= TypeDeclaration
//
// void StartList();
//

//
// Rule 39:  TypeDeclarations ::= TypeDeclarations TypeDeclaration
//
// void AddList2();
//

//
// Rule 40:  PackageDeclaration ::= Modifiersopt package Name PackageHeaderMarker SEMICOLON
//
#line 1186 "java.g"
void Parser::Act40()
{
    AstPackageDeclaration* p = ast_pool -> NewPackageDeclaration();
    p -> modifiers_opt = MakeModifiers();
    p -> package_token = Token(2);
    p -> name = DYNAMIC_CAST<AstName*> (Sym(3));
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

AstModifiers* Parser::MakeModifiers()
{
    AstModifiers* p = NULL;
    if (Sym(1))
    {
        p = ast_pool -> NewModifiers();
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(1));
        p -> AllocateModifiers(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            if (root -> element -> ModifierKeywordCast())
            {
                AstModifierKeyword* mod =
                    (AstModifierKeyword*) root -> element;
                p -> AddModifier(mod);
                if (lex_stream -> Kind(mod -> modifier_token) == TK_static)
                    p -> static_token_opt = mod -> modifier_token;
            }
            else p -> AddModifier(DYNAMIC_CAST<AstAnnotation*>
                                  (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    return p;
}

//
// Rule 41:  ImportDeclaration ::= SingleTypeImportDeclaration
//
// void NoAction();
//

//
// Rule 42:  ImportDeclaration ::= TypeImportOnDemandDeclaration
//
// void NoAction();
//

//
// Rule 43:  ImportDeclaration ::= SingleStaticImportDeclaration
//
// void NoAction();
//

//
// Rule 44:  ImportDeclaration ::= StaticImportOnDemandDeclaration
//
// void NoAction();
//

//
// Rule 45:  SingleTypeImportDeclaration ::= import Marker Name Marker Marker SEMICOLON
//
#line 1256 "java.g"
void Parser::MakeImportDeclaration()
{
    AstImportDeclaration* p = ast_pool -> NewImportDeclaration();
    p -> import_token = Token(1);
    if (Token(3) > Token(2))
        p -> static_token_opt = Token(2);
    p -> name = DYNAMIC_CAST<AstName*> (Sym(3));
    if (Token(6) > Token(5))
        p -> star_token_opt = Token(5);
    p -> semicolon_token = Token(6);
    Sym(1) = p;
}

//
// Rule 46:  TypeImportOnDemandDeclaration ::= import Marker Name DOT MULTIPLY SEMICOLON
//
// void MakeImportDeclaration();
//

//
// Rule 47:  SingleStaticImportDeclaration ::= import static Name Marker Marker SEMICOLON
//
// void MakeImportDeclaration();
//

//
// Rule 48:  StaticImportOnDemandDeclaration ::= import static Name DOT MULTIPLY SEMICOLON
//
// void MakeImportDeclaration();
//

//
// Rule 49:  TypeDeclaration ::= ClassDeclaration
//
// void NoAction();
//

//
// Rule 50:  TypeDeclaration ::= InterfaceDeclaration
//
// void NoAction();
//

//
// Rule 51:  TypeDeclaration ::= SEMICOLON
//
#line 1317 "java.g"
void Parser::Act51()
{
    Sym(1) = ast_pool -> NewEmptyDeclaration(Token(1));
}

//
// Rule 52:  Modifiers ::= Modifier
//
// void StartList();
//

//
// Rule 53:  Modifiers ::= Modifiers Modifier
//
// void AddList2();
//

//
// Rule 54:  Modifier ::= public
//
#line 1346 "java.g"
void Parser::MakeModifier()
{
    Sym(1) = ast_pool -> NewModifierKeyword(Token(1));
}

//
// Rule 55:  Modifier ::= protected
//
// void MakeModifier();
//

//
// Rule 56:  Modifier ::= private
//
// void MakeModifier();
//

//
// Rule 57:  Modifier ::= static
//
// void MakeModifier();
//

//
// Rule 58:  Modifier ::= abstract
//
// void MakeModifier();
//

//
// Rule 59:  Modifier ::= final
//
// void MakeModifier();
//

//
// Rule 60:  Modifier ::= native
//
// void MakeModifier();
//

//
// Rule 61:  Modifier ::= strictfp
//
// void MakeModifier();
//

//
// Rule 62:  Modifier ::= synchronized
//
// void MakeModifier();
//

//
// Rule 63:  Modifier ::= transient
//
// void MakeModifier();
//

//
// Rule 64:  Modifier ::= volatile
//
// void MakeModifier();
//

//
// Rule 65:  Modifier ::= Annotation
//
// void NoAction();
//

//
// Rule 66:  Annotation ::= NormalAnnotation
//
// void NoAction();
//

//
// Rule 67:  Annotation ::= MarkerAnnotation
//
// void NoAction();
//

//
// Rule 68:  Annotation ::= SingleMemberAnnotation
//
// void NoAction();
//

//
// Rule 69:  NormalAnnotation ::= AT Name LPAREN MemberValuePairsopt RPAREN
//
#line 1458 "java.g"
void Parser::MakeAnnotation()
{
    AstAnnotation* p = ast_pool -> NewAnnotation();
    p -> at_token = Token(1);
    p -> name = DYNAMIC_CAST<AstName*> (Sym(2));
    if (Sym(4))
    {
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(4));
        p -> AllocateMemberValuePairs(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            root = root -> next;
            p -> AddMemberValuePair(DYNAMIC_CAST<AstMemberValuePair*>
                                    (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    if (Token(5) > Token(3))
        p -> right_paren_token_opt = Token(5);
    Sym(1) = p;
}

//
// Rule 70:  MemberValuePairs ::= MemberValuePair
//
// void StartList();
//

//
// Rule 71:  MemberValuePairs ::= MemberValuePairs COMMA MemberValuePair
//
// void AddList3();
//

//
// Rule 72:  MemberValuePair ::= Identifier EQUAL MemberValue
//
#line 1503 "java.g"
void Parser::Act72()
{
    AstMemberValuePair* p = ast_pool -> NewMemberValuePair();
    p -> identifier_token_opt = Token(1);
    p -> member_value = DYNAMIC_CAST<AstMemberValue*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 73:  MemberValue ::= ConditionalExpression
//
// void NoAction();
//

//
// Rule 74:  MemberValue ::= Annotation
//
// void NoAction();
//

//
// Rule 75:  MemberValue ::= MemberValueArrayInitializer
//
// void NoAction();
//

//
// Rule 76:  MemberValueArrayInitializer ::= LBRACE Marker ,opt RBRACE
//
#line 1542 "java.g"
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
            p -> AddVariableInitializer(DYNAMIC_CAST<AstMemberValue*>
                                        (root -> element));
        } while (root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(4);
    Sym(1) = p;
}

//
// Rule 77:  MemberValueArrayInitializer ::= LBRACE MemberValues ,opt RBRACE
//
// void MakeArrayInitializer();
//

//
// Rule 78:  MemberValues ::= MemberValue
//
// void StartList();
//

//
// Rule 79:  MemberValues ::= MemberValues COMMA MemberValue
//
// void AddList3();
//

//
// Rule 80:  MarkerAnnotation ::= AT Name Marker Marker Marker
//
// void MakeAnnotation();
//

//
// Rule 81:  SingleMemberAnnotation ::= AT Name LPAREN MemberValue RPAREN
//
#line 1607 "java.g"
void Parser::Act81()
{
    AstMemberValuePair* mvp = ast_pool -> NewMemberValuePair();
    mvp -> member_value = DYNAMIC_CAST<AstMemberValue*> (Sym(4));
    AstListNode* p = AllocateListNode();
    p -> next = p;
    p -> element = mvp;
    p -> index = 0;
    Sym(4) = p;
    MakeAnnotation();
}

//
// Rule 82:  ClassDeclaration ::= Modifiersopt class Identifier Marker Superopt Interfacesopt ClassBody
//
#line 1632 "java.g"
void Parser::Act82()
{
    AstClassDeclaration* p = ast_pool -> NewClassDeclaration();
    p -> modifiers_opt = MakeModifiers();
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
// Rule 83:  Super ::= extends ClassOrInterfaceType
//
// void SetSym1ToSym2();
//

//
// Rule 84:  Interfaces ::= implements TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 85:  TypeList ::= ClassOrInterfaceType
//
// void StartList();
//

//
// Rule 86:  TypeList ::= TypeList COMMA ClassOrInterfaceType
//
// void AddList3();
//

//
// Rule 87:  ClassBody ::= LBRACE ClassBodyDeclarationsopt RBRACE
//
#line 1694 "java.g"
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
// Rule 88:  ClassBodyDeclarations ::= ClassBodyDeclaration
//
// void StartList();
//

//
// Rule 89:  ClassBodyDeclarations ::= ClassBodyDeclarations ClassBodyDeclaration
//
// void AddList2();
//

//
// Rule 90:  ClassBodyDeclaration ::= MemberDeclaration
//
// void NoAction();
//

//
// Rule 91:  ClassBodyDeclaration ::= ConstructorDeclaration
//
// void NoAction();
//

//
// Rule 92:  ClassBodyDeclaration ::= InitializerDeclaration
//
// void NoAction();
//

//
// Rule 93:  MemberDeclaration ::= FieldDeclaration
//
// void NoAction();
//

//
// Rule 94:  MemberDeclaration ::= MethodDeclaration
//
// void NoAction();
//

//
// Rule 95:  MemberDeclaration ::= TypeDeclaration
//
// void NoAction();
//

//
// Rule 96:  FieldDeclaration ::= Modifiersopt Marker Type VariableDeclarators SEMICOLON
//
#line 1888 "java.g"
void Parser::Act96()
{
    AstFieldDeclaration* p = ast_pool -> NewFieldDeclaration();
    p -> modifiers_opt = MakeModifiers();
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
// Rule 97:  VariableDeclarators ::= VariableDeclarator
//
// void StartList();
//

//
// Rule 98:  VariableDeclarators ::= VariableDeclarators COMMA VariableDeclarator
//
// void AddList3();
//

//
// Rule 99:  VariableDeclarator ::= VariableDeclaratorId
//
#line 1919 "java.g"
void Parser::Act99()
{
    AstVariableDeclarator* p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name =
        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(1));
    Sym(1) = p;
}

//
// Rule 100:  VariableDeclarator ::= VariableDeclaratorId EQUAL VariableInitializer
//
#line 1931 "java.g"
void Parser::Act100()
{
    AstVariableDeclarator* p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name =
        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(1));
    p -> variable_initializer_opt = Sym(3);
    Sym(1) = p;
}

//
// Rule 101:  VariableDeclaratorId ::= Identifier Dimsopt
//
#line 1944 "java.g"
void Parser::Act101()
{
    AstVariableDeclaratorId* p = ast_pool -> NewVariableDeclaratorId();
    p -> identifier_token = Token(1);
    p -> brackets_opt = DYNAMIC_CAST<AstBrackets*> (Sym(2));
    Sym(1) = p;
}

//
// Rule 102:  VariableInitializer ::= Expression
//
// void NoAction();
//

//
// Rule 103:  VariableInitializer ::= ArrayInitializer
//
// void NoAction();
//

//
// Rule 104:  MethodDeclaration ::= MethodHeader MethodHeaderMarker MethodBody
//
#line 1976 "java.g"
void Parser::MakeMethodDeclaration()
{
    DYNAMIC_CAST<AstMethodDeclaration*> (Sym(1)) -> method_body_opt =
        DYNAMIC_CAST<AstMethodBody*> (Sym(3));
}

//
// Rule 105:  MethodDeclaration ::= MethodHeader MethodHeaderMarker Marker SEMICOLON
//
// void MakeMethodDeclaration();
//

//
// Rule 106:  MethodHeader ::= Modifiersopt Marker Type MethodDeclarator Throwsopt
//
#line 2000 "java.g"
void Parser::MakeMethodHeader()
{
    AstMethodDeclaration* p = ast_pool -> NewMethodDeclaration();
    p -> modifiers_opt = MakeModifiers();
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
// Rule 107:  MethodHeader ::= Modifiersopt Marker void MethodDeclarator Throwsopt
//
#line 2029 "java.g"
void Parser::Act107()
{
    Sym(3) = ast_pool -> NewPrimitiveType(Ast::VOID_TYPE, Token(3));
    MakeMethodHeader();
}

//
// Rule 108:  MethodDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN Dimsopt
//
#line 2039 "java.g"
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
// Rule 109:  FormalParameterList ::= LastFormalParameter
//
// void StartList();
//

//
// Rule 110:  FormalParameterList ::= FormalParameters COMMA LastFormalParameter
//
// void AddList3();
//

//
// Rule 111:  FormalParameters ::= FormalParameter
//
// void StartList();
//

//
// Rule 112:  FormalParameters ::= FormalParameters COMMA FormalParameter
//
// void AddList3();
//

//
// Rule 113:  FormalParameter ::= PrimitiveType Dimsopt Marker VariableDeclaratorId
//
#line 2102 "java.g"
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
// Rule 114:  FormalParameter ::= Name Marker Marker VariableDeclaratorId
//
// void MakeFormalParameterA();
//

//
// Rule 115:  FormalParameter ::= Name Dims Marker VariableDeclaratorId
//
// void MakeFormalParameterA();
//

//
// Rule 116:  FormalParameter ::= Modifiers Type Marker VariableDeclaratorId
//
#line 2160 "java.g"
void Parser::MakeFormalParameterB()
{
    MakeFormalParameter(MakeModifiers(), DYNAMIC_CAST<AstType*> (Sym(2)),
                        Token(3) == Token(4) ? 0 : Token(3),
                        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(4)));
}

//
// Rule 117:  LastFormalParameter ::= FormalParameter
//
// void NoAction();
//

//
// Rule 118:  LastFormalParameter ::= PrimitiveType Dimsopt ELLIPSIS VariableDeclaratorId
//
// void MakeFormalParameterA();
//

//
// Rule 119:  LastFormalParameter ::= Name Marker ELLIPSIS VariableDeclaratorId
//
// void MakeFormalParameterA();
//

//
// Rule 120:  LastFormalParameter ::= Name Dims ELLIPSIS VariableDeclaratorId
//
// void MakeFormalParameterA();
//

//
// Rule 121:  LastFormalParameter ::= Modifiers Type ELLIPSIS VariableDeclaratorId
//
// void MakeFormalParameterB();
//

//
// Rule 122:  Throws ::= throws TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 123:  MethodBody ::= LBRACE BlockStatementsopt RBRACE
//
#line 2241 "java.g"
void Parser::Act123()
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
// Rule 124:  InitializerDeclaration ::= Modifiersopt Marker MethodHeaderMarker MethodBody
//
#line 2287 "java.g"
void Parser::Act124()
{
    AstInitializerDeclaration* p = ast_pool -> NewInitializerDeclaration();
    p -> modifiers_opt = MakeModifiers();
    p -> block = DYNAMIC_CAST<AstMethodBody*> (Sym(4));
    Sym(1) = p;
}

//
// Rule 125:  ConstructorDeclaration ::= Modifiersopt Marker ConstructorDeclarator Throwsopt MethodHeaderMarker MethodBody
//
#line 2308 "java.g"
void Parser::MakeConstructorDeclaration()
{
    AstConstructorDeclaration* p = ast_pool -> NewConstructorDeclaration();
    p -> modifiers_opt = MakeModifiers();
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
// Rule 126:  ConstructorDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN Marker
//
// void MakeMethodDeclarator();
//

//
// Rule 127:  ExplicitConstructorInvocation ::= this LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2355 "java.g"
void Parser::Act127()
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
// Rule 128:  ExplicitConstructorInvocation ::= super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2387 "java.g"
void Parser::Act128()
{
    AstSuperCall* p = ast_pool -> NewSuperCall();
    p -> super_token = Token(1);
    p -> arguments = MakeArguments(2);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 129:  ExplicitConstructorInvocation ::= Primary DOT super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2402 "java.g"
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
// Rule 130:  ExplicitConstructorInvocation ::= Name DOT super LPAREN ArgumentListopt RPAREN SEMICOLON
//
// void MakeQualifiedSuper();
//

//
// Rule 131:  InterfaceDeclaration ::= Modifiersopt interface Identifier Marker ExtendsInterfacesopt InterfaceBody
//
#line 2432 "java.g"
void Parser::Act131()
{
    AstInterfaceDeclaration* p = ast_pool -> NewInterfaceDeclaration();
    p -> modifiers_opt = MakeModifiers();
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
// Rule 132:  ExtendsInterfaces ::= extends TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 133:  InterfaceBody ::= LBRACE InterfaceMemberDeclarationsopt RBRACE
//
// void MakeClassBody();
//

//
// Rule 134:  InterfaceMemberDeclarations ::= MemberDeclaration
//
#line 2483 "java.g"
void Parser::Act134()
{
    AstFieldDeclaration* field = Sym(1) -> FieldDeclarationCast();
    if (field)
        field -> MarkStatic();
    StartList();
}

//
// Rule 135:  InterfaceMemberDeclarations ::= InterfaceMemberDeclarations MemberDeclaration
//
#line 2498 "java.g"
void Parser::Act135()
{
    AstFieldDeclaration* field = Sym(2) -> FieldDeclarationCast();
    if (field)
        field -> MarkStatic();
    AddList2();
}

//
// Rule 136:  ArrayInitializer ::= LBRACE Marker ,opt RBRACE
//
// void MakeArrayInitializer();
//

//
// Rule 137:  ArrayInitializer ::= LBRACE VariableInitializers ,opt RBRACE
//
// void MakeArrayInitializer();
//

//
// Rule 138:  VariableInitializers ::= VariableInitializer
//
// void StartList();
//

//
// Rule 139:  VariableInitializers ::= VariableInitializers COMMA VariableInitializer
//
// void AddList3();
//

//
// Rule 140:  Block ::= LBRACE BlockStatementsopt RBRACE
//
#line 2551 "java.g"
void Parser::Act140()
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
// Rule 141:  BlockStatements ::= BlockStatement
//
// void StartList();
//

//
// Rule 142:  BlockStatements ::= BlockStatements BlockStatement
//
// void AddList2();
//

//
// Rule 143:  BlockStatement ::= LocalVariableDeclarationStatement
//
// void NoAction();
//

//
// Rule 144:  BlockStatement ::= Statement
//
// void NoAction();
//

//
// Rule 145:  BlockStatement ::= ClassDeclaration
//
#line 2592 "java.g"
void Parser::Act145()
{
    Sym(1) = ast_pool ->
        NewLocalClassStatement(DYNAMIC_CAST<AstClassDeclaration*> (Sym(1)));
}

//
// Rule 146:  BlockStatement ::= ExplicitConstructorInvocation
//
// void NoAction();
//

//
// Rule 147:  LocalVariableDeclarationStatement ::= LocalVariableDeclaration SEMICOLON
//
#line 2611 "java.g"
void Parser::Act147()
{
    DYNAMIC_CAST<AstLocalVariableStatement*> (Sym(1)) ->
        semicolon_token_opt = Token(2);
}

//
// Rule 148:  LocalVariableDeclaration ::= PrimitiveType Dimsopt Marker VariableDeclarators
//
#line 2627 "java.g"
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
    AstLocalVariableStatement* p = ast_pool -> NewLocalVariableStatement();
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
// Rule 149:  LocalVariableDeclaration ::= Name Marker Marker VariableDeclarators
//
// void MakeLocalVariable();
//

//
// Rule 150:  LocalVariableDeclaration ::= Name Dims Marker VariableDeclarators
//
// void MakeLocalVariable();
//

//
// Rule 151:  LocalVariableDeclaration ::= Modifiers Type Marker VariableDeclarators
//
#line 2688 "java.g"
void Parser::Act151()
{
    MakeLocalVariable(MakeModifiers(), DYNAMIC_CAST<AstType*> (Sym(2)),
                      DYNAMIC_CAST<AstListNode*> (Sym(4)));
}

//
// Rule 152:  Statement ::= StatementWithoutTrailingSubstatement
//
// void NoAction();
//

//
// Rule 153:  Statement ::= LabeledStatement
//
// void NoAction();
//

//
// Rule 154:  Statement ::= IfThenStatement
//
// void NoAction();
//

//
// Rule 155:  Statement ::= IfThenElseStatement
//
// void NoAction();
//

//
// Rule 156:  Statement ::= WhileStatement
//
// void NoAction();
//

//
// Rule 157:  Statement ::= ForStatement
//
// void NoAction();
//

//
// Rule 158:  Statement ::= ForeachStatement
//
// void NoAction();
//

//
// Rule 159:  StatementNoShortIf ::= StatementWithoutTrailingSubstatement
//
// void NoAction();
//

//
// Rule 160:  StatementNoShortIf ::= LabeledStatementNoShortIf
//
// void NoAction();
//

//
// Rule 161:  StatementNoShortIf ::= IfThenElseStatementNoShortIf
//
// void NoAction();
//

//
// Rule 162:  StatementNoShortIf ::= WhileStatementNoShortIf
//
// void NoAction();
//

//
// Rule 163:  StatementNoShortIf ::= ForStatementNoShortIf
//
// void NoAction();
//

//
// Rule 164:  StatementNoShortIf ::= ForeachStatementNoShortIf
//
// void NoAction();
//

//
// Rule 165:  StatementWithoutTrailingSubstatement ::= Block
//
// void NoAction();
//

//
// Rule 166:  StatementWithoutTrailingSubstatement ::= EmptyStatement
//
// void NoAction();
//

//
// Rule 167:  StatementWithoutTrailingSubstatement ::= ExpressionStatement
//
// void NoAction();
//

//
// Rule 168:  StatementWithoutTrailingSubstatement ::= SwitchStatement
//
// void NoAction();
//

//
// Rule 169:  StatementWithoutTrailingSubstatement ::= DoStatement
//
// void NoAction();
//

//
// Rule 170:  StatementWithoutTrailingSubstatement ::= BreakStatement
//
// void NoAction();
//

//
// Rule 171:  StatementWithoutTrailingSubstatement ::= ContinueStatement
//
// void NoAction();
//

//
// Rule 172:  StatementWithoutTrailingSubstatement ::= ReturnStatement
//
// void NoAction();
//

//
// Rule 173:  StatementWithoutTrailingSubstatement ::= SynchronizedStatement
//
// void NoAction();
//

//
// Rule 174:  StatementWithoutTrailingSubstatement ::= ThrowStatement
//
// void NoAction();
//

//
// Rule 175:  StatementWithoutTrailingSubstatement ::= TryStatement
//
// void NoAction();
//

//
// Rule 176:  StatementWithoutTrailingSubstatement ::= AssertStatement
//
// void NoAction();
//

//
// Rule 177:  EmptyStatement ::= SEMICOLON
//
#line 2806 "java.g"
void Parser::Act177()
{
    Sym(1) = ast_pool -> NewEmptyStatement(Token(1));
}

//
// Rule 178:  LabeledStatement ::= Identifier COLON Statement
//
#line 2815 "java.g"
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
// Rule 179:  LabeledStatementNoShortIf ::= Identifier COLON StatementNoShortIf
//
// void MakeLabeledStatement();
//

//
// Rule 180:  ExpressionStatement ::= StatementExpression SEMICOLON
//
#line 2849 "java.g"
void Parser::Act180()
{
    DYNAMIC_CAST<AstExpressionStatement*> (Sym(1)) -> semicolon_token_opt =
        Token(2);
}

//
// Rule 181:  StatementExpression ::= Assignment
//
#line 2859 "java.g"
void Parser::MakeExpressionStatement()
{
    AstExpressionStatement* p = ast_pool -> NewExpressionStatement();
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    Sym(1) = p;
}

//
// Rule 182:  StatementExpression ::= PreIncrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 183:  StatementExpression ::= PreDecrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 184:  StatementExpression ::= PostIncrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 185:  StatementExpression ::= PostDecrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 186:  StatementExpression ::= MethodInvocation
//
// void MakeExpressionStatement();
//

//
// Rule 187:  StatementExpression ::= ClassInstanceCreationExpression
//
// void MakeExpressionStatement();
//

//
// Rule 188:  IfThenStatement ::= if LPAREN Expression RPAREN Statement Marker Marker
//
#line 2916 "java.g"
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
// Rule 189:  IfThenElseStatement ::= if LPAREN Expression RPAREN StatementNoShortIf else Statement
//
// void MakeIfThenElseStatement();
//

//
// Rule 190:  IfThenElseStatementNoShortIf ::= if LPAREN Expression RPAREN StatementNoShortIf else StatementNoShortIf
//
// void MakeIfThenElseStatement();
//

//
// Rule 191:  SwitchStatement ::= switch LPAREN Expression RPAREN SwitchBlock
//
#line 2964 "java.g"
void Parser::Act191()
{
    AstSwitchStatement* p = DYNAMIC_CAST<AstSwitchStatement*> (Sym(5));
    p -> switch_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 192:  SwitchBlock ::= LBRACE SwitchBlockStatements SwitchLabelsopt RBRACE
//
#line 2982 "java.g"
void Parser::Act192()
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
    block -> SetTag(AstBlock::SWITCH);
    p -> switch_block = block;
    Sym(1) = p;
}

//
// Rule 193:  SwitchBlock ::= LBRACE SwitchLabelsopt RBRACE
//
#line 3009 "java.g"
void Parser::Act193()
{
    AstSwitchStatement* p = ast_pool -> NewSwitchStatement();
    AstBlock* block = ast_pool -> NewBlock();
    block -> AllocateStatements(1); // allocate 1 element
    block -> left_brace_token = Token(1);
    if (Sym(2))
        block -> AddStatement
            (MakeSwitchBlockStatement(DYNAMIC_CAST<AstListNode*> (Sym(2))));
    block -> right_brace_token = Token(3);
    block -> SetTag(AstBlock::SWITCH);

    p -> switch_block = block;

    Sym(1) = p;
}

//
// Rule 194:  SwitchBlockStatements ::= SwitchBlockStatement
//
// void StartList();
//

//
// Rule 195:  SwitchBlockStatements ::= SwitchBlockStatements SwitchBlockStatement
//
// void AddList2();
//

//
// Rule 196:  SwitchBlockStatement ::= SwitchLabels BlockStatements
//
#line 3038 "java.g"
void Parser::Act196()
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
// Rule 197:  SwitchLabels ::= SwitchLabel
//
// void StartList();
//

//
// Rule 198:  SwitchLabels ::= SwitchLabels SwitchLabel
//
// void AddList2();
//

//
// Rule 199:  SwitchLabel ::= case Expression COLON
//
#line 3097 "java.g"
void Parser::MakeSwitchLabel()
{
    AstSwitchLabel* p = ast_pool -> NewSwitchLabel();
    p -> case_token = Token(1);
    p -> expression_opt = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> colon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 200:  SwitchLabel ::= default Marker COLON
//
// void MakeSwitchLabel();
//

//
// Rule 201:  WhileStatement ::= while LPAREN Expression RPAREN Statement
//
#line 3121 "java.g"
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
// Rule 202:  WhileStatementNoShortIf ::= while LPAREN Expression RPAREN StatementNoShortIf
//
// void MakeWhileStatement();
//

//
// Rule 203:  DoStatement ::= do Statement while LPAREN Expression RPAREN SEMICOLON
//
#line 3153 "java.g"
void Parser::Act203()
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
// Rule 204:  ForStatement ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN Statement
//
#line 3181 "java.g"
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
// Rule 205:  ForStatementNoShortIf ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN...
//
// void MakeForStatement();
//

//
// Rule 206:  ForInit ::= StatementExpressionList
//
// void NoAction();
//

//
// Rule 207:  ForInit ::= LocalVariableDeclaration
//
// void StartList();
//

//
// Rule 208:  ForUpdate ::= StatementExpressionList
//
// void NoAction();
//

//
// Rule 209:  StatementExpressionList ::= StatementExpression
//
// void StartList();
//

//
// Rule 210:  StatementExpressionList ::= StatementExpressionList COMMA StatementExpression
//
// void AddList3();
//

//
// Rule 211:  ForeachStatement ::= for LPAREN FormalParameter COLON Expression RPAREN Statement
//
#line 3262 "java.g"
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
// Rule 212:  ForeachStatementNoShortIf ::= for LPAREN FormalParameter COLON Expression RPAREN StatementNoShortIf
//
// void MakeForeachStatement();
//

//
// Rule 213:  AssertStatement ::= assert Expression Marker Marker SEMICOLON
//
#line 3304 "java.g"
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
// Rule 214:  AssertStatement ::= assert Expression COLON Expression SEMICOLON
//
// void MakeAssertStatement();
//

//
// Rule 215:  BreakStatement ::= break Identifieropt SEMICOLON
//
#line 3325 "java.g"
void Parser::Act215()
{
    AstBreakStatement* p = ast_pool -> NewBreakStatement();
    p -> break_token = Token(1);
    if (Token(3) > Token(2))
        p -> identifier_token_opt = Token(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 216:  ContinueStatement ::= continue Identifieropt SEMICOLON
//
#line 3339 "java.g"
void Parser::Act216()
{
    AstContinueStatement* p = ast_pool -> NewContinueStatement();
    p -> continue_token = Token(1);
    if (Token(3) > Token(2))
        p -> identifier_token_opt = Token(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 217:  ReturnStatement ::= return Expressionopt SEMICOLON
//
#line 3353 "java.g"
void Parser::Act217()
{
    AstReturnStatement* p = ast_pool -> NewReturnStatement();
    p -> return_token = Token(1);
    p -> expression_opt = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 218:  ThrowStatement ::= throw Expression SEMICOLON
//
#line 3366 "java.g"
void Parser::Act218()
{
    AstThrowStatement* p = ast_pool -> NewThrowStatement();
    p -> throw_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 219:  SynchronizedStatement ::= synchronized LPAREN Expression RPAREN Block
//
#line 3379 "java.g"
void Parser::Act219()
{
    AstSynchronizedStatement* p = ast_pool -> NewSynchronizedStatement();
    p -> synchronized_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(5));
    p -> block -> SetTag(AstBlock::SYNCHRONIZED);

    Sym(1) = p;
}

//
// Rule 220:  TryStatement ::= try Block Catches Marker
//
#line 3398 "java.g"
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
        p -> block -> SetTag(AstBlock::TRY_CLAUSE_WITH_FINALLY);
        for (unsigned i = 0; i < p -> NumCatchClauses(); i++)
            p -> CatchClause(i) -> block ->
                SetTag(AstBlock::TRY_CLAUSE_WITH_FINALLY);
        p -> finally_clause_opt = DYNAMIC_CAST<AstFinallyClause*> (Sym(4));
    }
    Sym(1) = p;
}

//
// Rule 221:  TryStatement ::= try Block Catchesopt Finally
//
// void MakeTryStatement();
//

//
// Rule 222:  Catches ::= CatchClause
//
// void StartList();
//

//
// Rule 223:  Catches ::= Catches CatchClause
//
// void AddList2();
//

//
// Rule 224:  CatchClause ::= catch LPAREN FormalParameter RPAREN Block
//
#line 3446 "java.g"
void Parser::Act224()
{
    AstCatchClause* p = ast_pool -> NewCatchClause();
    p -> catch_token = Token(1);
    p -> formal_parameter = DYNAMIC_CAST<AstFormalParameter*> (Sym(3));
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(5));

    Sym(1) = p;
}

//
// Rule 225:  Finally ::= finally Block
//
#line 3460 "java.g"
void Parser::Act225()
{
    AstFinallyClause* p = ast_pool -> NewFinallyClause();
    p -> finally_token = Token(1);
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(2));
    p -> block -> SetTag(AstBlock::FINALLY);

    Sym(1) = p;
}

//
// Rule 226:  Primary ::= PrimaryNoNewArray
//
// void NoAction();
//

//
// Rule 227:  Primary ::= ArrayCreationUninitialized
//
// void NoAction();
//

//
// Rule 228:  Primary ::= ArrayCreationInitialized
//
// void NoAction();
//

//
// Rule 229:  PrimaryNoNewArray ::= Literal
//
// void NoAction();
//

//
// Rule 230:  PrimaryNoNewArray ::= this
//
#line 3499 "java.g"
void Parser::Act230()
{
    Sym(1) = ast_pool -> NewThisExpression(Token(1));
}

//
// Rule 231:  PrimaryNoNewArray ::= LPAREN Name Marker RPAREN
//
#line 3516 "java.g"
void Parser::MakeParenthesizedExpression()
{
    AstParenthesizedExpression* p = ast_pool -> NewParenthesizedExpression();
    p -> left_parenthesis_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> right_parenthesis_token = Token(4);
    Sym(1) = p;
}

//
// Rule 232:  PrimaryNoNewArray ::= LPAREN ExpressionNotName Marker RPAREN
//
// void MakeParenthesizedExpression();
//

//
// Rule 233:  PrimaryNoNewArray ::= ClassInstanceCreationExpression
//
// void NoAction();
//

//
// Rule 234:  PrimaryNoNewArray ::= FieldAccess
//
// void NoAction();
//

//
// Rule 235:  PrimaryNoNewArray ::= Name DOT this
//
#line 3552 "java.g"
void Parser::Act235()
{
    AstThisExpression* p = ast_pool -> NewThisExpression(Token(3));
    p -> base_opt = ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(1)));
    Sym(1) = p;
}

//
// Rule 236:  PrimaryNoNewArray ::= PrimitiveType Dimsopt DOT class
//
#line 3568 "java.g"
void Parser::MakeClassLiteral()
{
    AstClassLiteral* p = ast_pool -> NewClassLiteral(Token(4));
    p -> type = MakeArrayType(1);
    Sym(1) = p;
}

//
// Rule 237:  PrimaryNoNewArray ::= Name Dims DOT class
//
// void MakeClassLiteral();
//

//
// Rule 238:  PrimaryNoNewArray ::= Name DOT Marker class
//
#line 3589 "java.g"
void Parser::Act238()
{
    Sym(2) = NULL;
    MakeClassLiteral();
}

//
// Rule 239:  PrimaryNoNewArray ::= void DOT Marker class
//
#line 3603 "java.g"
void Parser::Act239()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::VOID_TYPE, Token(1));
    Sym(2) = NULL;
    MakeClassLiteral();
}

//
// Rule 240:  PrimaryNoNewArray ::= MethodInvocation
//
// void NoAction();
//

//
// Rule 241:  PrimaryNoNewArray ::= ArrayAccess
//
// void NoAction();
//

//
// Rule 242:  ClassInstanceCreationExpression ::= new ClassOrInterfaceType LPAREN ArgumentListopt RPAREN ClassBodyopt
//
#line 3628 "java.g"
void Parser::Act242()
{
    AstClassCreationExpression* p = ast_pool -> NewClassCreationExpression();
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
// Rule 243:  ClassInstanceCreationExpression ::= Primary DOT new Identifier Marker LPAREN ArgumentListopt RPAREN...
//
#line 3653 "java.g"
void Parser::MakeQualifiedNew()
{
    AstClassCreationExpression* p = ast_pool -> NewClassCreationExpression();
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
// Rule 244:  ClassInstanceCreationExpression ::= Name DOT new Identifier Marker LPAREN ArgumentListopt RPAREN ClassBodyopt
//
// void MakeQualifiedNew();
//

//
// Rule 245:  ArgumentList ::= Expression
//
// void StartList();
//

//
// Rule 246:  ArgumentList ::= ArgumentList COMMA Expression
//
// void AddList3();
//

//
// Rule 247:  ArrayCreationUninitialized ::= new PrimitiveType DimExprs Dimsopt
//
#line 3699 "java.g"
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
// Rule 248:  ArrayCreationUninitialized ::= new ClassOrInterfaceType DimExprs Dimsopt
//
// void MakeArrayCreationUninitialized();
//

//
// Rule 249:  ArrayCreationInitialized ::= new ArrayType ArrayInitializer
//
#line 3732 "java.g"
void Parser::Act249()
{
    AstArrayCreationExpression* p = ast_pool -> NewArrayCreationExpression();
    p -> new_token = Token(1);
    p -> array_type = DYNAMIC_CAST<AstType*> (Sym(2));
    p -> array_initializer_opt = DYNAMIC_CAST<AstArrayInitializer*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 250:  DimExprs ::= DimExpr
//
// void StartList();
//

//
// Rule 251:  DimExprs ::= DimExprs DimExpr
//
// void AddList2();
//

//
// Rule 252:  DimExpr ::= LBRACKET Expression RBRACKET
//
#line 3753 "java.g"
void Parser::Act252()
{
    AstDimExpr* p = ast_pool -> NewDimExpr();
    p -> left_bracket_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> right_bracket_token = Token(3);
    Sym(1) = p;
}

//
// Rule 253:  Dims ::= LBRACKET RBRACKET
//
#line 3766 "java.g"
void Parser::Act253()
{
    Sym(1) = ast_pool -> NewBrackets(Token(1), Token(2));
}

//
// Rule 254:  Dims ::= Dims LBRACKET RBRACKET
//
#line 3775 "java.g"
void Parser::Act254()
{
    AstBrackets* p = DYNAMIC_CAST<AstBrackets*> (Sym(1));
    p -> right_bracket_token = Token(2);
    p -> dims++;
}

//
// Rule 255:  FieldAccess ::= Primary DOT Identifier
//
#line 3786 "java.g"
void Parser::MakeFieldAccess()
{
    AstFieldAccess* p = ast_pool -> NewFieldAccess();
    p -> base = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> identifier_token = Token(3);
    Sym(1) = p;
}

//
// Rule 256:  FieldAccess ::= super DOT Identifier
//
#line 3798 "java.g"
void Parser::MakeSuperFieldAccess()
{
    Sym(1) = ast_pool -> NewSuperExpression(Token(1));
    MakeFieldAccess();
}

//
// Rule 257:  FieldAccess ::= Name DOT super DOT Identifier
//
#line 3814 "java.g"
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
// Rule 258:  MethodInvocation ::= Identifier LPAREN ArgumentListopt RPAREN
//
#line 3833 "java.g"
void Parser::Act258()
{
    Sym(1) = NULL;
    Sym(1) = MakeMethodInvocation(1);
}

//
// This function treats Sym(1) as base_opt, and builds the method invocation
// starting with the 'Identifier' at tokennum.
//
AstMethodInvocation* Parser::MakeMethodInvocation(int tokennum)
{
    AstMethodInvocation* p = ast_pool -> NewMethodInvocation(Token(tokennum));
    p -> base_opt = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> arguments = MakeArguments(tokennum + 1);
    Sym(1) = p;
    return p;
}

//
// Rule 259:  MethodInvocation ::= Name DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3861 "java.g"
void Parser::Act259()
{
    MakeMethodInvocation(3);
}

//
// Rule 260:  MethodInvocation ::= Primary DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3870 "java.g"
void Parser::Act260()
{
    MakeMethodInvocation(3);
}

//
// Rule 261:  MethodInvocation ::= super DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3879 "java.g"
void Parser::Act261()
{
    Sym(1) = ast_pool -> NewSuperExpression(Token(1));
    MakeMethodInvocation(3);
}

//
// Rule 262:  MethodInvocation ::= Name DOT super DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3896 "java.g"
void Parser::Act262()
{
    AstSuperExpression* p = ast_pool -> NewSuperExpression(Token(3));
    p -> base_opt = ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(1)));
    Sym(1) = p;
    MakeMethodInvocation(5);
}

//
// Rule 263:  ArrayAccess ::= Name LBRACKET Expression RBRACKET
//
#line 3908 "java.g"
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
// Rule 264:  ArrayAccess ::= PrimaryNoNewArray LBRACKET Expression RBRACKET
//
// void MakeArrayAccess();
//

//
// Rule 265:  ArrayAccess ::= ArrayCreationInitialized LBRACKET Expression RBRACKET
//
// void MakeArrayAccess();
//

//
// Rule 266:  PostfixExpression ::= Primary
//
// void NoAction();
//

//
// Rule 267:  PostfixExpression ::= Name
//
// void NoAction();
//

//
// Rule 268:  PostfixExpression ::= PostIncrementExpression
//
// void NoAction();
//

//
// Rule 269:  PostfixExpression ::= PostDecrementExpression
//
// void NoAction();
//

//
// Rule 270:  PostfixExpressionNotName ::= Primary
//
// void NoAction();
//

//
// Rule 271:  PostfixExpressionNotName ::= PostIncrementExpression
//
// void NoAction();
//

//
// Rule 272:  PostfixExpressionNotName ::= PostDecrementExpression
//
// void NoAction();
//

//
// Rule 273:  PostIncrementExpression ::= PostfixExpression PLUS_PLUS
//
#line 3968 "java.g"
void Parser::Act273()
{
    AstPostUnaryExpression* p =
        ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::PLUSPLUS);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}

//
// Rule 274:  PostDecrementExpression ::= PostfixExpression MINUS_MINUS
//
#line 3981 "java.g"
void Parser::Act274()
{
    AstPostUnaryExpression* p =
        ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::MINUSMINUS);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}

//
// Rule 275:  UnaryExpression ::= PreIncrementExpression
//
// void NoAction();
//

//
// Rule 276:  UnaryExpression ::= PreDecrementExpression
//
// void NoAction();
//

//
// Rule 277:  UnaryExpression ::= PLUS UnaryExpression
//
#line 4002 "java.g"
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
// Rule 278:  UnaryExpression ::= MINUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 279:  UnaryExpression ::= UnaryExpressionNotPlusMinus
//
// void NoAction();
//

//
// Rule 280:  UnaryExpressionNotName ::= PreIncrementExpression
//
// void NoAction();
//

//
// Rule 281:  UnaryExpressionNotName ::= PreDecrementExpression
//
// void NoAction();
//

//
// Rule 282:  UnaryExpressionNotName ::= PLUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 283:  UnaryExpressionNotName ::= MINUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 284:  UnaryExpressionNotName ::= UnaryExpressionNotPlusMinusNotName
//
// void NoAction();
//

//
// Rule 285:  PreIncrementExpression ::= PLUS_PLUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 286:  PreDecrementExpression ::= MINUS_MINUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 287:  UnaryExpressionNotPlusMinus ::= PostfixExpression
//
// void NoAction();
//

//
// Rule 288:  UnaryExpressionNotPlusMinus ::= TWIDDLE UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 289:  UnaryExpressionNotPlusMinus ::= NOT UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 290:  UnaryExpressionNotPlusMinus ::= CastExpression
//
// void NoAction();
//

//
// Rule 291:  UnaryExpressionNotPlusMinusNotName ::= PostfixExpressionNotName
//
// void NoAction();
//

//
// Rule 292:  UnaryExpressionNotPlusMinusNotName ::= TWIDDLE UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 293:  UnaryExpressionNotPlusMinusNotName ::= NOT UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 294:  UnaryExpressionNotPlusMinusNotName ::= CastExpression
//
// void NoAction();
//

//
// Rule 295:  CastExpression ::= LPAREN PrimitiveType Dimsopt RPAREN UnaryExpression
//
#line 4109 "java.g"
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
// Rule 296:  CastExpression ::= LPAREN Name Marker RPAREN UnaryExpressionNotPlusMinus
//
// void MakeCastExpression();
//

//
// Rule 297:  CastExpression ::= LPAREN Name Dims RPAREN UnaryExpressionNotPlusMinus
//
// void MakeCastExpression();
//

//
// Rule 298:  MultiplicativeExpression ::= UnaryExpression
//
// void NoAction();
//

//
// Rule 299:  MultiplicativeExpression ::= MultiplicativeExpression MULTIPLY UnaryExpression
//
#line 4150 "java.g"
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
// Rule 300:  MultiplicativeExpression ::= MultiplicativeExpression DIVIDE UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 301:  MultiplicativeExpression ::= MultiplicativeExpression REMAINDER UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 302:  MultiplicativeExpressionNotName ::= UnaryExpressionNotName
//
// void NoAction();
//

//
// Rule 303:  MultiplicativeExpressionNotName ::= MultiplicativeExpressionNotName MULTIPLY UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 304:  MultiplicativeExpressionNotName ::= Name MULTIPLY UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 305:  MultiplicativeExpressionNotName ::= MultiplicativeExpressionNotName DIVIDE UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 306:  MultiplicativeExpressionNotName ::= Name DIVIDE UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 307:  MultiplicativeExpressionNotName ::= MultiplicativeExpressionNotName REMAINDER UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 308:  MultiplicativeExpressionNotName ::= Name REMAINDER UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 309:  AdditiveExpression ::= MultiplicativeExpression
//
// void NoAction();
//

//
// Rule 310:  AdditiveExpression ::= AdditiveExpression PLUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 311:  AdditiveExpression ::= AdditiveExpression MINUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 312:  AdditiveExpressionNotName ::= MultiplicativeExpressionNotName
//
// void NoAction();
//

//
// Rule 313:  AdditiveExpressionNotName ::= AdditiveExpressionNotName PLUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 314:  AdditiveExpressionNotName ::= Name PLUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 315:  AdditiveExpressionNotName ::= AdditiveExpressionNotName MINUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 316:  AdditiveExpressionNotName ::= Name MINUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 317:  ShiftExpression ::= AdditiveExpression
//
// void NoAction();
//

//
// Rule 318:  ShiftExpression ::= ShiftExpression LEFT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 319:  ShiftExpression ::= ShiftExpression RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 320:  ShiftExpression ::= ShiftExpression UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 321:  ShiftExpressionNotName ::= AdditiveExpressionNotName
//
// void NoAction();
//

//
// Rule 322:  ShiftExpressionNotName ::= ShiftExpressionNotName LEFT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 323:  ShiftExpressionNotName ::= Name LEFT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 324:  ShiftExpressionNotName ::= ShiftExpressionNotName RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 325:  ShiftExpressionNotName ::= Name RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 326:  ShiftExpressionNotName ::= ShiftExpressionNotName UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 327:  ShiftExpressionNotName ::= Name UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 328:  RelationalExpression ::= ShiftExpression
//
// void NoAction();
//

//
// Rule 329:  RelationalExpression ::= RelationalExpression LESS ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 330:  RelationalExpression ::= RelationalExpression GREATER ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 331:  RelationalExpression ::= RelationalExpression LESS_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 332:  RelationalExpression ::= RelationalExpression GREATER_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 333:  RelationalExpression ::= RelationalExpression instanceof ReferenceType
//
#line 4328 "java.g"
void Parser::MakeInstanceofExpression()
{
    AstInstanceofExpression* p = ast_pool -> NewInstanceofExpression();
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> instanceof_token = Token(2);
    p -> type = DYNAMIC_CAST<AstType*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 334:  RelationalExpressionNotName ::= ShiftExpressionNotName
//
// void NoAction();
//

//
// Rule 335:  RelationalExpressionNotName ::= RelationalExpressionNotName LESS ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 336:  RelationalExpressionNotName ::= Name LESS ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 337:  RelationalExpressionNotName ::= RelationalExpressionNotName GREATER ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 338:  RelationalExpressionNotName ::= Name GREATER ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 339:  RelationalExpressionNotName ::= RelationalExpressionNotName LESS_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 340:  RelationalExpressionNotName ::= Name LESS_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 341:  RelationalExpressionNotName ::= RelationalExpressionNotName GREATER_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 342:  RelationalExpressionNotName ::= Name GREATER_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 343:  RelationalExpressionNotName ::= RelationalExpressionNotName instanceof ReferenceType
//
// void MakeInstanceofExpression();
//

//
// Rule 344:  RelationalExpressionNotName ::= Name instanceof ReferenceType
//
// void MakeInstanceofExpression();
//

//
// Rule 345:  EqualityExpression ::= RelationalExpression
//
// void NoAction();
//

//
// Rule 346:  EqualityExpression ::= EqualityExpression EQUAL_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 347:  EqualityExpression ::= EqualityExpression NOT_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 348:  EqualityExpressionNotName ::= RelationalExpressionNotName
//
// void NoAction();
//

//
// Rule 349:  EqualityExpressionNotName ::= EqualityExpressionNotName EQUAL_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 350:  EqualityExpressionNotName ::= Name EQUAL_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 351:  EqualityExpressionNotName ::= EqualityExpressionNotName NOT_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 352:  EqualityExpressionNotName ::= Name NOT_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 353:  AndExpression ::= EqualityExpression
//
// void NoAction();
//

//
// Rule 354:  AndExpression ::= AndExpression AND EqualityExpression
//
// void MakeBinaryExpression();
//

//
// Rule 355:  AndExpressionNotName ::= EqualityExpressionNotName
//
// void NoAction();
//

//
// Rule 356:  AndExpressionNotName ::= AndExpressionNotName AND EqualityExpression
//
// void MakeBinaryExpression();
//

//
// Rule 357:  AndExpressionNotName ::= Name AND EqualityExpression
//
// void MakeBinaryExpression();
//

//
// Rule 358:  ExclusiveOrExpression ::= AndExpression
//
// void NoAction();
//

//
// Rule 359:  ExclusiveOrExpression ::= ExclusiveOrExpression XOR AndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 360:  ExclusiveOrExpressionNotName ::= AndExpressionNotName
//
// void NoAction();
//

//
// Rule 361:  ExclusiveOrExpressionNotName ::= ExclusiveOrExpressionNotName XOR AndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 362:  ExclusiveOrExpressionNotName ::= Name XOR AndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 363:  InclusiveOrExpression ::= ExclusiveOrExpression
//
// void NoAction();
//

//
// Rule 364:  InclusiveOrExpression ::= InclusiveOrExpression OR ExclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 365:  InclusiveOrExpressionNotName ::= ExclusiveOrExpressionNotName
//
// void NoAction();
//

//
// Rule 366:  InclusiveOrExpressionNotName ::= InclusiveOrExpressionNotName OR ExclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 367:  InclusiveOrExpressionNotName ::= Name OR ExclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 368:  ConditionalAndExpression ::= InclusiveOrExpression
//
// void NoAction();
//

//
// Rule 369:  ConditionalAndExpression ::= ConditionalAndExpression AND_AND InclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 370:  ConditionalAndExpressionNotName ::= InclusiveOrExpressionNotName
//
// void NoAction();
//

//
// Rule 371:  ConditionalAndExpressionNotName ::= ConditionalAndExpressionNotName AND_AND InclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 372:  ConditionalAndExpressionNotName ::= Name AND_AND InclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 373:  ConditionalOrExpression ::= ConditionalAndExpression
//
// void NoAction();
//

//
// Rule 374:  ConditionalOrExpression ::= ConditionalOrExpression OR_OR ConditionalAndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 375:  ConditionalOrExpressionNotName ::= ConditionalAndExpressionNotName
//
// void NoAction();
//

//
// Rule 376:  ConditionalOrExpressionNotName ::= ConditionalOrExpressionNotName OR_OR ConditionalAndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 377:  ConditionalOrExpressionNotName ::= Name OR_OR ConditionalAndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 378:  ConditionalExpression ::= ConditionalOrExpression
//
// void NoAction();
//

//
// Rule 379:  ConditionalExpression ::= ConditionalOrExpression QUESTION Expression COLON ConditionalExpression
//
#line 4538 "java.g"
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
// Rule 380:  ConditionalExpressionNotName ::= ConditionalOrExpressionNotName
//
// void NoAction();
//

//
// Rule 381:  ConditionalExpressionNotName ::= ConditionalOrExpressionNotName QUESTION Expression COLON...
//
// void MakeConditionalExpression();
//

//
// Rule 382:  ConditionalExpressionNotName ::= Name QUESTION Expression COLON ConditionalExpression
//
// void MakeConditionalExpression();
//

//
// Rule 383:  AssignmentExpression ::= ConditionalExpression
//
// void NoAction();
//

//
// Rule 384:  AssignmentExpression ::= Assignment
//
// void NoAction();
//

//
// Rule 385:  AssignmentExpressionNotName ::= ConditionalExpressionNotName
//
// void NoAction();
//

//
// Rule 386:  AssignmentExpressionNotName ::= Assignment
//
// void NoAction();
//

//
// Rule 387:  Assignment ::= PostfixExpression AssignmentOperator AssignmentExpression
//
#line 4595 "java.g"
void Parser::Act387()
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
// Rule 388:  AssignmentOperator ::= EQUAL
//
// void NoAction();
//

//
// Rule 389:  AssignmentOperator ::= MULTIPLY_EQUAL
//
// void NoAction();
//

//
// Rule 390:  AssignmentOperator ::= DIVIDE_EQUAL
//
// void NoAction();
//

//
// Rule 391:  AssignmentOperator ::= REMAINDER_EQUAL
//
// void NoAction();
//

//
// Rule 392:  AssignmentOperator ::= PLUS_EQUAL
//
// void NoAction();
//

//
// Rule 393:  AssignmentOperator ::= MINUS_EQUAL
//
// void NoAction();
//

//
// Rule 394:  AssignmentOperator ::= LEFT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 395:  AssignmentOperator ::= RIGHT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 396:  AssignmentOperator ::= UNSIGNED_RIGHT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 397:  AssignmentOperator ::= AND_EQUAL
//
// void NoAction();
//

//
// Rule 398:  AssignmentOperator ::= XOR_EQUAL
//
// void NoAction();
//

//
// Rule 399:  AssignmentOperator ::= OR_EQUAL
//
// void NoAction();
//

//
// Rule 400:  Expression ::= AssignmentExpression
//
// void NoAction();
//

//
// Rule 401:  ExpressionNotName ::= AssignmentExpressionNotName
//
// void NoAction();
//

//
// Rule 402:  ,opt ::=
//
// void NoAction();
//

//
// Rule 403:  ,opt ::= COMMA
//
// void NoAction();
//

//
// Rule 404:  Identifieropt ::=
//
// void NoAction();
//

//
// Rule 405:  Identifieropt ::= Identifier
//
// void NoAction();
//

//
// Rule 406:  Superopt ::=
//
#line 4719 "java.g"
//
// Given a rule of the form A ::= x1 x2 ... xn
//
// Construct a NULL Ast for A.
//
void Parser::NullAction() { Sym(1) = NULL; }

//
// Rule 407:  Superopt ::= Super
//
// void NoAction();
//

//
// Rule 408:  Expressionopt ::=
//
// void NullAction();
//

//
// Rule 409:  Expressionopt ::= Expression
//
// void NoAction();
//

//
// Rule 410:  ClassBodyopt ::=
//
// void NullAction();
//

//
// Rule 411:  ClassBodyopt ::= ClassBody
//
// void NoAction();
//

//
// Rule 412:  ImportDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 413:  ImportDeclarationsopt ::= ImportDeclarations
//
// void NoAction();
//

//
// Rule 414:  TypeDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 415:  TypeDeclarationsopt ::= TypeDeclarations
//
// void NoAction();
//

//
// Rule 416:  ClassBodyDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 417:  ClassBodyDeclarationsopt ::= ClassBodyDeclarations
//
// void NoAction();
//

//
// Rule 418:  Modifiersopt ::=
//
// void NullAction();
//

//
// Rule 419:  Modifiersopt ::= Modifiers
//
// void NoAction();
//

//
// Rule 420:  BlockStatementsopt ::=
//
// void NullAction();
//

//
// Rule 421:  BlockStatementsopt ::= BlockStatements
//
// void NoAction();
//

//
// Rule 422:  Dimsopt ::=
//
// void NullAction();
//

//
// Rule 423:  Dimsopt ::= Dims
//
// void NoAction();
//

//
// Rule 424:  ArgumentListopt ::=
//
// void NullAction();
//

//
// Rule 425:  ArgumentListopt ::= ArgumentList
//
// void NoAction();
//

//
// Rule 426:  SwitchLabelsopt ::=
//
// void NullAction();
//

//
// Rule 427:  SwitchLabelsopt ::= SwitchLabels
//
// void NoAction();
//

//
// Rule 428:  Throwsopt ::=
//
// void NullAction();
//

//
// Rule 429:  Throwsopt ::= Throws
//
// void NoAction();
//

//
// Rule 430:  FormalParameterListopt ::=
//
// void NullAction();
//

//
// Rule 431:  FormalParameterListopt ::= FormalParameterList
//
// void NoAction();
//

//
// Rule 432:  Interfacesopt ::=
//
// void NullAction();
//

//
// Rule 433:  Interfacesopt ::= Interfaces
//
// void NoAction();
//

//
// Rule 434:  InterfaceMemberDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 435:  InterfaceMemberDeclarationsopt ::= InterfaceMemberDeclarations
//
// void NoAction();
//

//
// Rule 436:  ForInitopt ::=
//
// void NullAction();
//

//
// Rule 437:  ForInitopt ::= ForInit
//
// void NoAction();
//

//
// Rule 438:  ForUpdateopt ::=
//
// void NullAction();
//

//
// Rule 439:  ForUpdateopt ::= ForUpdate
//
// void NoAction();
//

//
// Rule 440:  ExtendsInterfacesopt ::=
//
// void NullAction();
//

//
// Rule 441:  ExtendsInterfacesopt ::= ExtendsInterfaces
//
// void NoAction();
//

//
// Rule 442:  Catchesopt ::=
//
// void NullAction();
//

//
// Rule 443:  Catchesopt ::= Catches
//
// void NoAction();
//

//
// Rule 444:  MemberValuePairsopt ::=
//
// void NullAction();
//

//
// Rule 445:  MemberValuePairsopt ::= MemberValuePairs
//
// void NoAction();
//

//
// Rule 446:  PackageHeaderMarker ::=
//
#line 4892 "java.g"
//
// When this function is invoked, if the "parse_package_header_only" flag
// is turned on, we skip to the end-of-file token.
//
void Parser::Act446()
{
    if (parse_package_header_only)
        // point to the EOF token
        lex_stream -> Reset(lex_stream -> NumTokens() - 1);
    Sym(1) = NULL;
}

//
// Rule 447:  MethodHeaderMarker ::=
//
#line 4908 "java.g"
//
// When this function is invoked, if the "parse_header_only" flag
// is turned on, the body of the method being parsed is skipped.
//
void Parser::Act447()
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
// Rule 448:  Marker ::=
//
// void NullAction();
//
#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif
