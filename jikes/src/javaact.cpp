#line 721 "java.g"
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
#line 764 "java.g"
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
#line 782 "java.g"
//
// Given a rule of the form A ::= x1 x2, inherit the result from x2.
//
void Parser::SetSym1ToSym2() { Sym(1) = Sym(2); }

//
// Rule 3:  Literal ::= IntegerLiteral
//
#line 794 "java.g"
void Parser::Act3()
{
    Sym(1) = ast_pool -> NewIntegerLiteral(Token(1));
}

//
// Rule 4:  Literal ::= LongLiteral
//
#line 803 "java.g"
void Parser::Act4()
{
    Sym(1) = ast_pool -> NewLongLiteral(Token(1));
}

//
// Rule 5:  Literal ::= FloatLiteral
//
#line 812 "java.g"
void Parser::Act5()
{
    Sym(1) = ast_pool -> NewFloatLiteral(Token(1));
}

//
// Rule 6:  Literal ::= DoubleLiteral
//
#line 821 "java.g"
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
#line 834 "java.g"
void Parser::Act8()
{
    Sym(1) = ast_pool -> NewCharacterLiteral(Token(1));
}

//
// Rule 9:  Literal ::= StringLiteral
//
#line 843 "java.g"
void Parser::Act9()
{
    Sym(1) = ast_pool -> NewStringLiteral(Token(1));
}

//
// Rule 10:  Literal ::= null
//
#line 852 "java.g"
void Parser::Act10()
{
    Sym(1) = ast_pool -> NewNullLiteral(Token(1));
}

//
// Rule 11:  BooleanLiteral ::= true
//
#line 861 "java.g"
void Parser::Act11()
{
    Sym(1) = ast_pool -> NewTrueLiteral(Token(1));
}

//
// Rule 12:  BooleanLiteral ::= false
//
#line 870 "java.g"
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
#line 893 "java.g"
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
#line 910 "java.g"
void Parser::Act19()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::BYTE, Token(1));
}

//
// Rule 20:  IntegralType ::= short
//
#line 919 "java.g"
void Parser::Act20()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::SHORT, Token(1));
}

//
// Rule 21:  IntegralType ::= int
//
#line 928 "java.g"
void Parser::Act21()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::INT, Token(1));
}

//
// Rule 22:  IntegralType ::= long
//
#line 937 "java.g"
void Parser::Act22()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::LONG, Token(1));
}

//
// Rule 23:  IntegralType ::= char
//
#line 946 "java.g"
void Parser::Act23()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::CHAR, Token(1));
}

//
// Rule 24:  FloatingPointType ::= float
//
#line 955 "java.g"
void Parser::Act24()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::FLOAT, Token(1));
}

//
// Rule 25:  FloatingPointType ::= double
//
#line 964 "java.g"
void Parser::Act25()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::DOUBLE, Token(1));
}

//
// Rule 26:  VoidType ::= void
//
#line 976 "java.g"
void Parser::Act26()
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::VOID_TYPE, Token(1));
}

//
// Rule 27:  ReferenceType ::= ClassOrInterfaceType
//
// void NoAction();
//

//
// Rule 28:  ReferenceType ::= ArrayType
//
// void NoAction();
//

//
// Rule 29:  ClassOrInterfaceType ::= ClassOrInterface
//
// void NoAction();
//

//
// Rule 30:  ClassOrInterfaceType ::= ClassOrInterface LESS TypeArgumentList1 Marker
//
#line 1008 "java.g"
void Parser::MakeTypeArguments() { Sym(1) = MakeTypeArguments(1); }

//
// Given AstName/AstType '<' TypeArgumentList1 at tokennum, generate the
// AstTypeName that includes the type arguments. There must be a production at
// tokennum + 3 to allow finding the closing '>'.
//
AstTypeName* Parser::MakeTypeArguments(int tokennum)
{
    AstTypeName* p = Sym(tokennum) -> NameCast()
        ? ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(tokennum)))
        : DYNAMIC_CAST<AstTypeName*> (Sym(tokennum));
    AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(tokennum + 2));
    AstTypeArguments* q =
        ast_pool -> NewTypeArguments(Token(tokennum + 1),
                                     Token(tokennum + 3) - 1);
    q -> AllocateTypeArguments(tail -> index + 1);
    AstListNode* root = tail;
    do
    {
        root = root -> next;
        q -> AddTypeArgument(DYNAMIC_CAST<AstType*> (root -> element));
    } while (root != tail);
    FreeCircularList(tail);
    p -> type_arguments_opt = q;
    return p;
}

//
// Rule 31:  ClassOrInterface ::= Name
//
#line 1040 "java.g"
void Parser::Act31()
{
    AstTypeName* p = ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(1)));
    Sym(1) = p;
}

//
// Rule 32:  ClassOrInterface ::= ClassOrInterface LESS TypeArgumentList1 DOT Name
//
#line 1053 "java.g"
void Parser::Act32()
{
    AstTypeName* p = ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(5)));
    p -> base_opt = MakeTypeArguments(1);
    Sym(1) = p;
}

//
// Rule 33:  ArrayType ::= PrimitiveType Dims
//
#line 1075 "java.g"
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
// Rule 34:  ArrayType ::= Name Dims
//
// void MakeArrayType();
//

//
// Rule 35:  ArrayType ::= ClassOrInterface LESS TypeArgumentList1 DOT Name Dims
//
#line 1106 "java.g"
void Parser::Act35()
{
    AstTypeName* p = ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(5)));
    p -> base_opt = MakeTypeArguments(1);
    Sym(5) = p;
    Sym(1) = MakeArrayType(5);
}

//
// Rule 36:  ArrayType ::= ClassOrInterface LESS TypeArgumentList1 Dims
//
#line 1121 "java.g"
void Parser::Act36()
{
    Sym(3) = MakeTypeArguments(1);
    Sym(1) = MakeArrayType(3);
}

//
// Rule 37:  Name ::= Identifier
//
#line 1139 "java.g"
void Parser::Act37() { MakeSimpleName(1); }

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
// Rule 38:  Name ::= Name DOT Marker Identifier
//
#line 1159 "java.g"
void Parser::Act38()
{
    AstName* p = ast_pool -> NewName(Token(4));
    p -> base_opt = DYNAMIC_CAST<AstName*> (Sym(1));
    Sym(1) = p;
}

//
// Rule 39:  CompilationUnit ::= PackageDeclaration ImportDeclarationsopt TypeDeclarationsopt
//
#line 1180 "java.g"
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
// Rule 40:  CompilationUnit ::= Marker ImportDeclarations TypeDeclarationsopt
//
// void MakeCompilationUnit();
//

//
// Rule 41:  CompilationUnit ::= TypeDeclarationsopt
//
#line 1231 "java.g"
void Parser::Act41()
{
    Sym(3) = Sym(1);
    Sym(1) = NULL;
    Sym(2) = NULL;
    MakeCompilationUnit();
}

//
// Rule 42:  ImportDeclarations ::= ImportDeclaration
//
#line 1243 "java.g"
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
// Rule 43:  ImportDeclarations ::= ImportDeclarations ImportDeclaration
//
#line 1260 "java.g"
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
// Rule 44:  TypeDeclarations ::= TypeDeclaration
//
// void StartList();
//

//
// Rule 45:  TypeDeclarations ::= TypeDeclarations TypeDeclaration
//
// void AddList2();
//

//
// Rule 46:  PackageDeclaration ::= Modifiersopt package Name PackageHeaderMarker SEMICOLON
//
#line 1308 "java.g"
void Parser::Act46()
{
    AstPackageDeclaration* p = ast_pool -> NewPackageDeclaration();
    p -> modifiers_opt = MakeModifiers();
    p -> package_token = Token(2);
    p -> name = DYNAMIC_CAST<AstName*> (Sym(3));
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Converts the list at symbol 1 to AstModifiers.
//
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
// Rule 47:  ImportDeclaration ::= SingleTypeImportDeclaration
//
// void NoAction();
//

//
// Rule 48:  ImportDeclaration ::= TypeImportOnDemandDeclaration
//
// void NoAction();
//

//
// Rule 49:  ImportDeclaration ::= SingleStaticImportDeclaration
//
// void NoAction();
//

//
// Rule 50:  ImportDeclaration ::= StaticImportOnDemandDeclaration
//
// void NoAction();
//

//
// Rule 51:  SingleTypeImportDeclaration ::= import Marker Name Marker Marker SEMICOLON
//
#line 1381 "java.g"
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
// Rule 52:  TypeImportOnDemandDeclaration ::= import Marker Name DOT MULTIPLY SEMICOLON
//
// void MakeImportDeclaration();
//

//
// Rule 53:  SingleStaticImportDeclaration ::= import static Name Marker Marker SEMICOLON
//
// void MakeImportDeclaration();
//

//
// Rule 54:  StaticImportOnDemandDeclaration ::= import static Name DOT MULTIPLY SEMICOLON
//
// void MakeImportDeclaration();
//

//
// Rule 55:  TypeDeclaration ::= ClassDeclaration
//
// void NoAction();
//

//
// Rule 56:  TypeDeclaration ::= InterfaceDeclaration
//
// void NoAction();
//

//
// Rule 57:  TypeDeclaration ::= SEMICOLON
//
#line 1441 "java.g"
void Parser::Act57()
{
    Sym(1) = ast_pool -> NewEmptyDeclaration(Token(1));
}

//
// Rule 58:  Modifiers ::= Modifier
//
// void StartList();
//

//
// Rule 59:  Modifiers ::= Modifiers Modifier
//
// void AddList2();
//

//
// Rule 60:  Modifier ::= public
//
#line 1470 "java.g"
void Parser::MakeModifier()
{
    Sym(1) = ast_pool -> NewModifierKeyword(Token(1));
}

//
// Rule 61:  Modifier ::= protected
//
// void MakeModifier();
//

//
// Rule 62:  Modifier ::= private
//
// void MakeModifier();
//

//
// Rule 63:  Modifier ::= static
//
// void MakeModifier();
//

//
// Rule 64:  Modifier ::= abstract
//
// void MakeModifier();
//

//
// Rule 65:  Modifier ::= final
//
// void MakeModifier();
//

//
// Rule 66:  Modifier ::= native
//
// void MakeModifier();
//

//
// Rule 67:  Modifier ::= strictfp
//
// void MakeModifier();
//

//
// Rule 68:  Modifier ::= synchronized
//
// void MakeModifier();
//

//
// Rule 69:  Modifier ::= transient
//
// void MakeModifier();
//

//
// Rule 70:  Modifier ::= volatile
//
// void MakeModifier();
//

//
// Rule 71:  Modifier ::= Annotation
//
// void NoAction();
//

//
// Rule 72:  Annotation ::= NormalAnnotation
//
// void NoAction();
//

//
// Rule 73:  Annotation ::= MarkerAnnotation
//
// void NoAction();
//

//
// Rule 74:  Annotation ::= SingleMemberAnnotation
//
// void NoAction();
//

//
// Rule 75:  NormalAnnotation ::= AT Name LPAREN MemberValuePairsopt RPAREN
//
#line 1582 "java.g"
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
// Rule 76:  MemberValuePairs ::= MemberValuePair
//
// void StartList();
//

//
// Rule 77:  MemberValuePairs ::= MemberValuePairs COMMA MemberValuePair
//
// void AddList3();
//

//
// Rule 78:  MemberValuePair ::= Identifier EQUAL MemberValue
//
#line 1627 "java.g"
void Parser::Act78()
{
    AstMemberValuePair* p = ast_pool -> NewMemberValuePair();
    p -> identifier_token_opt = Token(1);
    p -> member_value = DYNAMIC_CAST<AstMemberValue*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 79:  MemberValue ::= ConditionalExpression
//
// void NoAction();
//

//
// Rule 80:  MemberValue ::= Annotation
//
// void NoAction();
//

//
// Rule 81:  MemberValue ::= MemberValueArrayInitializer
//
// void NoAction();
//

//
// Rule 82:  MemberValueArrayInitializer ::= LBRACE Marker ,opt RBRACE
//
#line 1666 "java.g"
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
// Rule 83:  MemberValueArrayInitializer ::= LBRACE MemberValues ,opt RBRACE
//
// void MakeArrayInitializer();
//

//
// Rule 84:  MemberValues ::= MemberValue
//
// void StartList();
//

//
// Rule 85:  MemberValues ::= MemberValues COMMA MemberValue
//
// void AddList3();
//

//
// Rule 86:  MarkerAnnotation ::= AT Name Marker Marker Marker
//
// void MakeAnnotation();
//

//
// Rule 87:  SingleMemberAnnotation ::= AT Name LPAREN MemberValue RPAREN
//
#line 1731 "java.g"
void Parser::Act87()
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
// Rule 88:  ClassDeclaration ::= Modifiersopt class Identifier TypeParametersopt Superopt Interfacesopt ClassBody
//
#line 1756 "java.g"
void Parser::Act88()
{
    AstClassDeclaration* p = ast_pool -> NewClassDeclaration();
    p -> modifiers_opt = MakeModifiers();
    p -> class_token = Token(2);
    p -> type_parameters_opt = MakeTypeParameters(4);
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
// Creates an AstTypeParameters node for insertion into an AstTypeName.
//
AstTypeParameters* Parser::MakeTypeParameters(int tokennum)
{
    if (! Sym(tokennum))
        return NULL;
    AstTypeParameters* p = ast_pool -> NewTypeParameters();
    AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(tokennum));
    p -> AllocateTypeParameters(tail -> index + 1);
    AstListNode* root = tail;
    do
    {
        root = root -> next;
        p -> AddTypeParameter(DYNAMIC_CAST<AstTypeParameter*>
                              (root -> element));
    } while (root != tail);
    FreeCircularList(tail);
    p -> right_angle_token = Token(tokennum + 1) - 1;
    return p;
}

//
// Rule 89:  Super ::= extends ClassOrInterfaceType
//
// void SetSym1ToSym2();
//

//
// Rule 90:  Interfaces ::= implements TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 91:  TypeList ::= ClassOrInterfaceType
//
// void StartList();
//

//
// Rule 92:  TypeList ::= TypeList COMMA ClassOrInterfaceType
//
// void AddList3();
//

//
// Rule 93:  ClassBody ::= LBRACE ClassBodyDeclarationsopt RBRACE
//
#line 1841 "java.g"
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
// Rule 94:  ClassBodyDeclarations ::= ClassBodyDeclaration
//
// void StartList();
//

//
// Rule 95:  ClassBodyDeclarations ::= ClassBodyDeclarations ClassBodyDeclaration
//
// void AddList2();
//

//
// Rule 96:  ClassBodyDeclaration ::= MemberDeclaration
//
// void NoAction();
//

//
// Rule 97:  ClassBodyDeclaration ::= ConstructorDeclaration
//
// void NoAction();
//

//
// Rule 98:  ClassBodyDeclaration ::= InitializerDeclaration
//
// void NoAction();
//

//
// Rule 99:  MemberDeclaration ::= FieldDeclaration
//
// void NoAction();
//

//
// Rule 100:  MemberDeclaration ::= MethodDeclaration
//
// void NoAction();
//

//
// Rule 101:  MemberDeclaration ::= TypeDeclaration
//
// void NoAction();
//

//
// Rule 102:  FieldDeclaration ::= Modifiersopt Marker Type VariableDeclarators SEMICOLON
//
#line 2035 "java.g"
void Parser::Act102()
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
// Rule 103:  VariableDeclarators ::= VariableDeclarator
//
// void StartList();
//

//
// Rule 104:  VariableDeclarators ::= VariableDeclarators COMMA VariableDeclarator
//
// void AddList3();
//

//
// Rule 105:  VariableDeclarator ::= VariableDeclaratorId
//
#line 2066 "java.g"
void Parser::Act105()
{
    AstVariableDeclarator* p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name =
        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(1));
    Sym(1) = p;
}

//
// Rule 106:  VariableDeclarator ::= VariableDeclaratorId EQUAL VariableInitializer
//
#line 2078 "java.g"
void Parser::Act106()
{
    AstVariableDeclarator* p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name =
        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(1));
    p -> variable_initializer_opt = Sym(3);
    Sym(1) = p;
}

//
// Rule 107:  VariableDeclaratorId ::= Identifier Dimsopt
//
#line 2091 "java.g"
void Parser::Act107()
{
    AstVariableDeclaratorId* p = ast_pool -> NewVariableDeclaratorId();
    p -> identifier_token = Token(1);
    p -> brackets_opt = DYNAMIC_CAST<AstBrackets*> (Sym(2));
    Sym(1) = p;
}

//
// Rule 108:  VariableInitializer ::= Expression
//
// void NoAction();
//

//
// Rule 109:  VariableInitializer ::= ArrayInitializer
//
// void NoAction();
//

//
// Rule 110:  MethodDeclaration ::= MethodHeader MethodHeaderMarker MethodBody
//
#line 2123 "java.g"
void Parser::MakeMethodDeclaration()
{
    DYNAMIC_CAST<AstMethodDeclaration*> (Sym(1)) -> method_body_opt =
        DYNAMIC_CAST<AstMethodBody*> (Sym(3));
}

//
// Rule 111:  MethodDeclaration ::= MethodHeader MethodHeaderMarker Marker SEMICOLON
//
// void MakeMethodDeclaration();
//

//
// Rule 112:  MethodHeader ::= Modifiersopt Marker Type MethodDeclarator Throwsopt
//
#line 2147 "java.g"
void Parser::MakeMethodHeader()
{
    AstMethodDeclaration* p = ast_pool -> NewMethodDeclaration();
    p -> modifiers_opt = MakeModifiers();
    p -> type_parameters_opt = MakeTypeParameters(2);
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
// Rule 113:  MethodHeader ::= Modifiersopt TypeParameters Type MethodDeclarator Throwsopt
//
// void MakeMethodHeader();
//

//
// Rule 114:  MethodHeader ::= Modifiersopt Marker VoidType MethodDeclarator Throwsopt
//
// void MakeMethodHeader();
//

//
// Rule 115:  MethodHeader ::= Modifiersopt TypeParameters VoidType MethodDeclarator Throwsopt
//
// void MakeMethodHeader();
//

//
// Rule 116:  MethodDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN Dimsopt
//
#line 2207 "java.g"
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
// Rule 117:  FormalParameterList ::= LastFormalParameter
//
// void StartList();
//

//
// Rule 118:  FormalParameterList ::= FormalParameters COMMA LastFormalParameter
//
// void AddList3();
//

//
// Rule 119:  FormalParameters ::= FormalParameter
//
// void StartList();
//

//
// Rule 120:  FormalParameters ::= FormalParameters COMMA FormalParameter
//
// void AddList3();
//

//
// Rule 121:  FormalParameter ::= Type Marker Marker VariableDeclaratorId
//
#line 2270 "java.g"
void Parser::MakeFormalParameter()
{
    AstFormalParameter* p = ast_pool -> NewFormalParameter();
    if (Sym(2))
    {
        p -> modifiers_opt = MakeModifiers();
        p -> type = DYNAMIC_CAST<AstType*> (Sym(2));
    }
    else p -> type = DYNAMIC_CAST<AstType*> (Sym(1));
    if (Token(4) > Token(3))
        p -> ellipsis_token_opt = Token(3);
    AstVariableDeclarator* formal_declarator =
        ast_pool -> NewVariableDeclarator();
    formal_declarator -> variable_declarator_name =
        DYNAMIC_CAST<AstVariableDeclaratorId*> (Sym(4));
    p -> formal_declarator = formal_declarator;
    Sym(1) = p;
}

//
// Rule 122:  FormalParameter ::= Modifiers Type Marker VariableDeclaratorId
//
// void MakeFormalParameter();
//

//
// Rule 123:  LastFormalParameter ::= FormalParameter
//
// void NoAction();
//

//
// Rule 124:  LastFormalParameter ::= Type Marker ELLIPSIS VariableDeclaratorId
//
// void MakeFormalParameter();
//

//
// Rule 125:  LastFormalParameter ::= Modifiers Type ELLIPSIS VariableDeclaratorId
//
// void MakeFormalParameter();
//

//
// Rule 126:  Throws ::= throws TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 127:  MethodBody ::= LBRACE BlockStatementsopt RBRACE
//
#line 2355 "java.g"
void Parser::Act127()
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
// Rule 128:  InitializerDeclaration ::= Modifiersopt MethodHeaderMarker MethodBody
//
#line 2400 "java.g"
void Parser::Act128()
{
    AstInitializerDeclaration* p = ast_pool -> NewInitializerDeclaration();
    p -> modifiers_opt = MakeModifiers();
    p -> block = DYNAMIC_CAST<AstMethodBody*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 129:  ConstructorDeclaration ::= Modifiersopt Marker ConstructorDeclarator Throwsopt MethodHeaderMarker MethodBody
//
#line 2421 "java.g"
void Parser::MakeConstructorDeclaration()
{
    AstConstructorDeclaration* p = ast_pool -> NewConstructorDeclaration();
    p -> modifiers_opt = MakeModifiers();
    p -> type_parameters_opt = MakeTypeParameters(2);
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
// Rule 130:  ConstructorDeclaration ::= Modifiersopt TypeParameters ConstructorDeclarator Throwsopt MethodHeaderMarker...
//
// void MakeConstructorDeclaration();
//

//
// Rule 131:  ConstructorDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN Marker
//
// void MakeMethodDeclarator();
//

//
// Rule 132:  ExplicitConstructorInvocation ::= this LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2481 "java.g"
void Parser::Act132()
{
    AstThisCall* p = ast_pool -> NewThisCall();
    p -> this_token = Token(1);
    p -> arguments = MakeArguments(2);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Parses the optional arguments between '(' and ')' at tokennum.
//
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
// Rule 133:  ExplicitConstructorInvocation ::= TypeArguments this LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2520 "java.g"
void Parser::Act133()
{
    AstThisCall* p = ast_pool -> NewThisCall();
    p -> type_arguments_opt = MakeExplicitTypeArguments(1);
    p -> this_token = Token(2);
    p -> arguments = MakeArguments(3);
    p -> semicolon_token = Token(6);
    Sym(1) = p;
}

//
// Given TypeArgumentsopt at tokennum, generate the AstTypeArguments for use
// in a method call's explicit type arguments. There must be a production
// at tokennum + 1 to allow finding the closing '>'.
//
AstTypeArguments* Parser::MakeExplicitTypeArguments(int tokennum)
{
    if (! Sym(tokennum))
        return NULL;
    AstTypeArguments* p =
        ast_pool -> NewTypeArguments(Token(tokennum),
                                     Token(tokennum + 1) - 1);
    AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(tokennum));
    p -> AllocateTypeArguments(tail -> index + 1);
    AstListNode* root = tail;
    do
    {
        root = root -> next;
        p -> AddTypeArgument(DYNAMIC_CAST<AstType*> (root -> element));
    } while (root != tail);
    FreeCircularList(tail);
    return p;
}

//
// Rule 134:  ExplicitConstructorInvocation ::= super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2558 "java.g"
void Parser::Act134()
{
    AstSuperCall* p = ast_pool -> NewSuperCall();
    p -> super_token = Token(1);
    p -> arguments = MakeArguments(2);
    p -> semicolon_token = Token(5);
    Sym(1) = p;
}

//
// Rule 135:  ExplicitConstructorInvocation ::= TypeArguments super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2575 "java.g"
void Parser::Act135()
{
    AstSuperCall* p = ast_pool -> NewSuperCall();
    p -> type_arguments_opt = MakeExplicitTypeArguments(1);
    p -> super_token = Token(2);
    p -> arguments = MakeArguments(3);
    p -> semicolon_token = Token(6);
    Sym(1) = p;
}

//
// Rule 136:  ExplicitConstructorInvocation ::= Primary DOT TypeArgumentsopt super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2596 "java.g"
void Parser::MakeQualifiedSuper()
{
    AstSuperCall* p = ast_pool -> NewSuperCall();
    p -> base_opt = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> type_arguments_opt = MakeExplicitTypeArguments(3);
    p -> super_token = Token(4);
    p -> arguments = MakeArguments(5);
    p -> semicolon_token = Token(8);
    Sym(1) = p;
}

//
// Rule 137:  ExplicitConstructorInvocation ::= Name DOT Marker super LPAREN ArgumentListopt RPAREN SEMICOLON
//
// void MakeQualifiedSuper();
//

//
// Rule 138:  ExplicitConstructorInvocation ::= Name DOT TypeArguments super LPAREN ArgumentListopt RPAREN SEMICOLON
//
// void MakeQualifiedSuper();
//

//
// Rule 139:  InterfaceDeclaration ::= Modifiersopt interface Identifier TypeParametersopt ExtendsInterfacesopt...
//
#line 2647 "java.g"
void Parser::Act139()
{
    AstInterfaceDeclaration* p = ast_pool -> NewInterfaceDeclaration();
    p -> modifiers_opt = MakeModifiers();
    p -> interface_token = Token(2);
    p -> type_parameters_opt = MakeTypeParameters(4);
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
// Rule 140:  ExtendsInterfaces ::= extends TypeList
//
// void SetSym1ToSym2();
//

//
// Rule 141:  InterfaceBody ::= LBRACE InterfaceMemberDeclarationsopt RBRACE
//
// void MakeClassBody();
//

//
// Rule 142:  InterfaceMemberDeclarations ::= MemberDeclaration
//
#line 2699 "java.g"
void Parser::Act142()
{
    AstFieldDeclaration* field = Sym(1) -> FieldDeclarationCast();
    if (field)
        field -> MarkStatic();
    StartList();
}

//
// Rule 143:  InterfaceMemberDeclarations ::= InterfaceMemberDeclarations MemberDeclaration
//
#line 2714 "java.g"
void Parser::Act143()
{
    AstFieldDeclaration* field = Sym(2) -> FieldDeclarationCast();
    if (field)
        field -> MarkStatic();
    AddList2();
}

//
// Rule 144:  ArrayInitializer ::= LBRACE Marker ,opt RBRACE
//
// void MakeArrayInitializer();
//

//
// Rule 145:  ArrayInitializer ::= LBRACE VariableInitializers ,opt RBRACE
//
// void MakeArrayInitializer();
//

//
// Rule 146:  VariableInitializers ::= VariableInitializer
//
// void StartList();
//

//
// Rule 147:  VariableInitializers ::= VariableInitializers COMMA VariableInitializer
//
// void AddList3();
//

//
// Rule 148:  Block ::= LBRACE BlockStatementsopt RBRACE
//
#line 2767 "java.g"
void Parser::Act148()
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
// Rule 149:  BlockStatements ::= BlockStatement
//
// void StartList();
//

//
// Rule 150:  BlockStatements ::= BlockStatements BlockStatement
//
// void AddList2();
//

//
// Rule 151:  BlockStatement ::= LocalVariableDeclarationStatement
//
// void NoAction();
//

//
// Rule 152:  BlockStatement ::= Statement
//
// void NoAction();
//

//
// Rule 153:  BlockStatement ::= ClassDeclaration
//
#line 2808 "java.g"
void Parser::Act153()
{
    Sym(1) = ast_pool ->
        NewLocalClassStatement(DYNAMIC_CAST<AstClassDeclaration*> (Sym(1)));
}

//
// Rule 154:  BlockStatement ::= ExplicitConstructorInvocation
//
// void NoAction();
//

//
// Rule 155:  LocalVariableDeclarationStatement ::= LocalVariableDeclaration SEMICOLON
//
#line 2827 "java.g"
void Parser::Act155()
{
    DYNAMIC_CAST<AstLocalVariableStatement*> (Sym(1)) -> semicolon_token_opt =
        Token(2);
}

//
// Rule 156:  LocalVariableDeclaration ::= Type Marker Marker VariableDeclarators
//
#line 2843 "java.g"
void Parser::MakeLocalVariable()
{
    AstLocalVariableStatement* p = ast_pool -> NewLocalVariableStatement();
    if (Sym(2))
    {
        p -> modifiers_opt = MakeModifiers();
        p -> type = DYNAMIC_CAST<AstType*> (Sym(2));
    }
    else p -> type = DYNAMIC_CAST<AstType*> (Sym(1));
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
    Sym(1) = p;
}

//
// Rule 157:  LocalVariableDeclaration ::= Modifiers Type Marker VariableDeclarators
//
// void MakeLocalVariable();
//

//
// Rule 158:  Statement ::= StatementWithoutTrailingSubstatement
//
// void NoAction();
//

//
// Rule 159:  Statement ::= LabeledStatement
//
// void NoAction();
//

//
// Rule 160:  Statement ::= IfThenStatement
//
// void NoAction();
//

//
// Rule 161:  Statement ::= IfThenElseStatement
//
// void NoAction();
//

//
// Rule 162:  Statement ::= WhileStatement
//
// void NoAction();
//

//
// Rule 163:  Statement ::= ForStatement
//
// void NoAction();
//

//
// Rule 164:  Statement ::= ForeachStatement
//
// void NoAction();
//

//
// Rule 165:  StatementNoShortIf ::= StatementWithoutTrailingSubstatement
//
// void NoAction();
//

//
// Rule 166:  StatementNoShortIf ::= LabeledStatementNoShortIf
//
// void NoAction();
//

//
// Rule 167:  StatementNoShortIf ::= IfThenElseStatementNoShortIf
//
// void NoAction();
//

//
// Rule 168:  StatementNoShortIf ::= WhileStatementNoShortIf
//
// void NoAction();
//

//
// Rule 169:  StatementNoShortIf ::= ForStatementNoShortIf
//
// void NoAction();
//

//
// Rule 170:  StatementNoShortIf ::= ForeachStatementNoShortIf
//
// void NoAction();
//

//
// Rule 171:  StatementWithoutTrailingSubstatement ::= Block
//
// void NoAction();
//

//
// Rule 172:  StatementWithoutTrailingSubstatement ::= EmptyStatement
//
// void NoAction();
//

//
// Rule 173:  StatementWithoutTrailingSubstatement ::= ExpressionStatement
//
// void NoAction();
//

//
// Rule 174:  StatementWithoutTrailingSubstatement ::= SwitchStatement
//
// void NoAction();
//

//
// Rule 175:  StatementWithoutTrailingSubstatement ::= DoStatement
//
// void NoAction();
//

//
// Rule 176:  StatementWithoutTrailingSubstatement ::= BreakStatement
//
// void NoAction();
//

//
// Rule 177:  StatementWithoutTrailingSubstatement ::= ContinueStatement
//
// void NoAction();
//

//
// Rule 178:  StatementWithoutTrailingSubstatement ::= ReturnStatement
//
// void NoAction();
//

//
// Rule 179:  StatementWithoutTrailingSubstatement ::= SynchronizedStatement
//
// void NoAction();
//

//
// Rule 180:  StatementWithoutTrailingSubstatement ::= ThrowStatement
//
// void NoAction();
//

//
// Rule 181:  StatementWithoutTrailingSubstatement ::= TryStatement
//
// void NoAction();
//

//
// Rule 182:  StatementWithoutTrailingSubstatement ::= AssertStatement
//
// void NoAction();
//

//
// Rule 183:  EmptyStatement ::= SEMICOLON
//
#line 2991 "java.g"
void Parser::Act183()
{
    Sym(1) = ast_pool -> NewEmptyStatement(Token(1));
}

//
// Rule 184:  LabeledStatement ::= Identifier COLON Statement
//
#line 3000 "java.g"
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
// Rule 185:  LabeledStatementNoShortIf ::= Identifier COLON StatementNoShortIf
//
// void MakeLabeledStatement();
//

//
// Rule 186:  ExpressionStatement ::= StatementExpression SEMICOLON
//
#line 3034 "java.g"
void Parser::Act186()
{
    DYNAMIC_CAST<AstExpressionStatement*> (Sym(1)) -> semicolon_token_opt =
        Token(2);
}

//
// Rule 187:  StatementExpression ::= Assignment
//
#line 3044 "java.g"
void Parser::MakeExpressionStatement()
{
    AstExpressionStatement* p = ast_pool -> NewExpressionStatement();
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    Sym(1) = p;
}

//
// Rule 188:  StatementExpression ::= PreIncrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 189:  StatementExpression ::= PreDecrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 190:  StatementExpression ::= PostIncrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 191:  StatementExpression ::= PostDecrementExpression
//
// void MakeExpressionStatement();
//

//
// Rule 192:  StatementExpression ::= MethodInvocation
//
// void MakeExpressionStatement();
//

//
// Rule 193:  StatementExpression ::= ClassInstanceCreationExpression
//
// void MakeExpressionStatement();
//

//
// Rule 194:  IfThenStatement ::= if LPAREN Expression RPAREN Statement Marker Marker
//
#line 3101 "java.g"
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

//
// Ensures the symbol at tokennum is an AstBlock (wrapping it in a generated
// block if necessary).
//
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
// Rule 195:  IfThenElseStatement ::= if LPAREN Expression RPAREN StatementNoShortIf else Statement
//
// void MakeIfThenElseStatement();
//

//
// Rule 196:  IfThenElseStatementNoShortIf ::= if LPAREN Expression RPAREN StatementNoShortIf else StatementNoShortIf
//
// void MakeIfThenElseStatement();
//

//
// Rule 197:  SwitchStatement ::= switch LPAREN Expression RPAREN SwitchBlock
//
#line 3153 "java.g"
void Parser::Act197()
{
    AstSwitchStatement* p = DYNAMIC_CAST<AstSwitchStatement*> (Sym(5));
    p -> switch_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 198:  SwitchBlock ::= LBRACE SwitchBlockStatements SwitchLabelsopt RBRACE
//
#line 3171 "java.g"
void Parser::Act198()
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
// Creates an AstSwitchBlockStatement from the given non-null labels, and
// possibly null list of statements.
//
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
// Rule 199:  SwitchBlock ::= LBRACE SwitchLabelsopt RBRACE
//
#line 3239 "java.g"
void Parser::Act199()
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
// Rule 200:  SwitchBlockStatements ::= SwitchBlockStatement
//
// void StartList();
//

//
// Rule 201:  SwitchBlockStatements ::= SwitchBlockStatements SwitchBlockStatement
//
// void AddList2();
//

//
// Rule 202:  SwitchBlockStatement ::= SwitchLabels BlockStatements
//
#line 3268 "java.g"
void Parser::Act202()
{
    Sym(1) = MakeSwitchBlockStatement(DYNAMIC_CAST<AstListNode*> (Sym(1)),
                                      DYNAMIC_CAST<AstListNode*> (Sym(2)));
}

//
// Rule 203:  SwitchLabels ::= SwitchLabel
//
// void StartList();
//

//
// Rule 204:  SwitchLabels ::= SwitchLabels SwitchLabel
//
// void AddList2();
//

//
// Rule 205:  SwitchLabel ::= case Expression COLON
//
#line 3290 "java.g"
void Parser::MakeSwitchLabel()
{
    AstSwitchLabel* p = ast_pool -> NewSwitchLabel();
    p -> case_token = Token(1);
    p -> expression_opt = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> colon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 206:  SwitchLabel ::= default Marker COLON
//
// void MakeSwitchLabel();
//

//
// Rule 207:  WhileStatement ::= while LPAREN Expression RPAREN Statement
//
#line 3314 "java.g"
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
// Rule 208:  WhileStatementNoShortIf ::= while LPAREN Expression RPAREN StatementNoShortIf
//
// void MakeWhileStatement();
//

//
// Rule 209:  DoStatement ::= do Statement while LPAREN Expression RPAREN SEMICOLON
//
#line 3346 "java.g"
void Parser::Act209()
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
// Rule 210:  ForStatement ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN Statement
//
#line 3374 "java.g"
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
// Rule 211:  ForStatementNoShortIf ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN...
//
// void MakeForStatement();
//

//
// Rule 212:  ForInit ::= StatementExpressionList
//
// void NoAction();
//

//
// Rule 213:  ForInit ::= LocalVariableDeclaration
//
// void StartList();
//

//
// Rule 214:  ForUpdate ::= StatementExpressionList
//
// void NoAction();
//

//
// Rule 215:  StatementExpressionList ::= StatementExpression
//
// void StartList();
//

//
// Rule 216:  StatementExpressionList ::= StatementExpressionList COMMA StatementExpression
//
// void AddList3();
//

//
// Rule 217:  ForeachStatement ::= for LPAREN FormalParameter COLON Expression RPAREN Statement
//
#line 3455 "java.g"
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
// Rule 218:  ForeachStatementNoShortIf ::= for LPAREN FormalParameter COLON Expression RPAREN StatementNoShortIf
//
// void MakeForeachStatement();
//

//
// Rule 219:  AssertStatement ::= assert Expression Marker Marker SEMICOLON
//
#line 3497 "java.g"
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
// Rule 220:  AssertStatement ::= assert Expression COLON Expression SEMICOLON
//
// void MakeAssertStatement();
//

//
// Rule 221:  BreakStatement ::= break Identifieropt SEMICOLON
//
#line 3518 "java.g"
void Parser::Act221()
{
    AstBreakStatement* p = ast_pool -> NewBreakStatement();
    p -> break_token = Token(1);
    if (Token(3) > Token(2))
        p -> identifier_token_opt = Token(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 222:  ContinueStatement ::= continue Identifieropt SEMICOLON
//
#line 3532 "java.g"
void Parser::Act222()
{
    AstContinueStatement* p = ast_pool -> NewContinueStatement();
    p -> continue_token = Token(1);
    if (Token(3) > Token(2))
        p -> identifier_token_opt = Token(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 223:  ReturnStatement ::= return Expressionopt SEMICOLON
//
#line 3546 "java.g"
void Parser::Act223()
{
    AstReturnStatement* p = ast_pool -> NewReturnStatement();
    p -> return_token = Token(1);
    p -> expression_opt = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 224:  ThrowStatement ::= throw Expression SEMICOLON
//
#line 3559 "java.g"
void Parser::Act224()
{
    AstThrowStatement* p = ast_pool -> NewThrowStatement();
    p -> throw_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 225:  SynchronizedStatement ::= synchronized LPAREN Expression RPAREN Block
//
#line 3572 "java.g"
void Parser::Act225()
{
    AstSynchronizedStatement* p = ast_pool -> NewSynchronizedStatement();
    p -> synchronized_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(3));
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(5));
    p -> block -> SetTag(AstBlock::SYNCHRONIZED);

    Sym(1) = p;
}

//
// Rule 226:  TryStatement ::= try Block Catches Marker
//
#line 3591 "java.g"
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
// Rule 227:  TryStatement ::= try Block Catchesopt Finally
//
// void MakeTryStatement();
//

//
// Rule 228:  Catches ::= CatchClause
//
// void StartList();
//

//
// Rule 229:  Catches ::= Catches CatchClause
//
// void AddList2();
//

//
// Rule 230:  CatchClause ::= catch LPAREN FormalParameter RPAREN Block
//
#line 3639 "java.g"
void Parser::Act230()
{
    AstCatchClause* p = ast_pool -> NewCatchClause();
    p -> catch_token = Token(1);
    p -> formal_parameter = DYNAMIC_CAST<AstFormalParameter*> (Sym(3));
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(5));

    Sym(1) = p;
}

//
// Rule 231:  Finally ::= finally Block
//
#line 3653 "java.g"
void Parser::Act231()
{
    AstFinallyClause* p = ast_pool -> NewFinallyClause();
    p -> finally_token = Token(1);
    p -> block = DYNAMIC_CAST<AstBlock*> (Sym(2));
    p -> block -> SetTag(AstBlock::FINALLY);

    Sym(1) = p;
}

//
// Rule 232:  Primary ::= PrimaryNoNewArray
//
// void NoAction();
//

//
// Rule 233:  Primary ::= ArrayCreationUninitialized
//
// void NoAction();
//

//
// Rule 234:  Primary ::= ArrayCreationInitialized
//
// void NoAction();
//

//
// Rule 235:  PrimaryNoNewArray ::= Literal
//
// void NoAction();
//

//
// Rule 236:  PrimaryNoNewArray ::= this
//
#line 3692 "java.g"
void Parser::Act236()
{
    Sym(1) = ast_pool -> NewThisExpression(Token(1));
}

//
// Rule 237:  PrimaryNoNewArray ::= LPAREN Name Marker RPAREN
//
#line 3709 "java.g"
void Parser::MakeParenthesizedExpression()
{
    AstParenthesizedExpression* p = ast_pool -> NewParenthesizedExpression();
    p -> left_parenthesis_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> right_parenthesis_token = Token(4);
    Sym(1) = p;
}

//
// Rule 238:  PrimaryNoNewArray ::= LPAREN ExpressionNotName Marker RPAREN
//
// void MakeParenthesizedExpression();
//

//
// Rule 239:  PrimaryNoNewArray ::= ClassInstanceCreationExpression
//
// void NoAction();
//

//
// Rule 240:  PrimaryNoNewArray ::= FieldAccess
//
// void NoAction();
//

//
// Rule 241:  PrimaryNoNewArray ::= Name DOT this
//
#line 3745 "java.g"
void Parser::Act241()
{
    AstThisExpression* p = ast_pool -> NewThisExpression(Token(3));
    p -> base_opt = ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(1)));
    Sym(1) = p;
}

//
// Rule 242:  PrimaryNoNewArray ::= PrimitiveType Dimsopt DOT class
//
#line 3761 "java.g"
void Parser::MakeClassLiteral()
{
    AstClassLiteral* p = ast_pool -> NewClassLiteral(Token(4));
    if (Token(3) == Token(4))
        Sym(2) = NULL;
    p -> type = MakeArrayType(1);
    Sym(1) = p;
}

//
// Rule 243:  PrimaryNoNewArray ::= Name Dims DOT class
//
// void MakeClassLiteral();
//

//
// Rule 244:  PrimaryNoNewArray ::= Name DOT Marker class
//
// void MakeClassLiteral();
//

//
// Rule 245:  PrimaryNoNewArray ::= VoidType DOT Marker class
//
// void MakeClassLiteral();
//

//
// Rule 246:  PrimaryNoNewArray ::= MethodInvocation
//
// void NoAction();
//

//
// Rule 247:  PrimaryNoNewArray ::= ArrayAccess
//
// void NoAction();
//

//
// Rule 248:  ClassInstanceCreationExpression ::= new ClassOrInterfaceType LPAREN ArgumentListopt RPAREN ClassBodyopt
//
#line 3817 "java.g"
void Parser::Act248()
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
// Rule 249:  ClassInstanceCreationExpression ::= new TypeArguments ClassOrInterfaceType LPAREN ArgumentListopt RPAREN...
//
#line 3840 "java.g"
void Parser::Act249()
{
    AstClassCreationExpression* p = ast_pool -> NewClassCreationExpression();
    p -> new_token = Token(1);
    p -> type_arguments_opt = MakeExplicitTypeArguments(2);
    p -> class_type = DYNAMIC_CAST<AstTypeName*> (Sym(3));
    p -> arguments = MakeArguments(4);
    p -> class_body_opt = DYNAMIC_CAST<AstClassBody*> (Sym(7));
    if (p -> class_body_opt)
        p -> class_body_opt -> identifier_token =
            p -> class_type -> IdentifierToken();
    Sym(1) = p;
}

//
// Rule 250:  ClassInstanceCreationExpression ::= Primary DOT new TypeArgumentsopt Identifier TypeArgumentsopt LPAREN...
//
#line 3866 "java.g"
void Parser::MakeQualifiedNew()
{
    AstClassCreationExpression* p = ast_pool -> NewClassCreationExpression();
    p -> base_opt = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> new_token = Token(3);
    p -> type_arguments_opt = MakeExplicitTypeArguments(4);
    p -> class_type = ast_pool -> NewTypeName(MakeSimpleName(5));
    p -> class_type -> type_arguments_opt = MakeExplicitTypeArguments(6);
    p -> arguments = MakeArguments(7);
    p -> class_body_opt = DYNAMIC_CAST<AstClassBody*> (Sym(10));
    if (p -> class_body_opt)
        p -> class_body_opt -> identifier_token = Token(5);
    Sym(1) = p;
}

//
// Rule 251:  ClassInstanceCreationExpression ::= Name DOT new TypeArgumentsopt Identifier TypeArgumentsopt LPAREN...
//
// void MakeQualifiedNew();
//

//
// Rule 252:  ArgumentList ::= Expression
//
// void StartList();
//

//
// Rule 253:  ArgumentList ::= ArgumentList COMMA Expression
//
// void AddList3();
//

//
// Rule 254:  ArrayCreationUninitialized ::= new PrimitiveType DimExprs Dimsopt
//
#line 3914 "java.g"
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
// Rule 255:  ArrayCreationUninitialized ::= new ClassOrInterfaceType DimExprs Dimsopt
//
// void MakeArrayCreationUninitialized();
//

//
// Rule 256:  ArrayCreationInitialized ::= new PrimitiveType Dims ArrayInitializer
//
#line 3947 "java.g"
void Parser::MakeArrayCreationInitialized()
{
    AstArrayCreationExpression* p = ast_pool -> NewArrayCreationExpression();
    p -> new_token = Token(1);
    p -> array_type = MakeArrayType(2);
    p -> array_initializer_opt = DYNAMIC_CAST<AstArrayInitializer*> (Sym(4));
    Sym(1) = p;
}

//
// Rule 257:  ArrayCreationInitialized ::= new ClassOrInterfaceType Dims ArrayInitializer
//
// void MakeArrayCreationInitialized();
//

//
// Rule 258:  DimExprs ::= DimExpr
//
// void StartList();
//

//
// Rule 259:  DimExprs ::= DimExprs DimExpr
//
// void AddList2();
//

//
// Rule 260:  DimExpr ::= LBRACKET Expression RBRACKET
//
#line 3975 "java.g"
void Parser::Act260()
{
    AstDimExpr* p = ast_pool -> NewDimExpr();
    p -> left_bracket_token = Token(1);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(2));
    p -> right_bracket_token = Token(3);
    Sym(1) = p;
}

//
// Rule 261:  Dims ::= LBRACKET RBRACKET
//
#line 3988 "java.g"
void Parser::Act261()
{
    Sym(1) = ast_pool -> NewBrackets(Token(1), Token(2));
}

//
// Rule 262:  Dims ::= Dims LBRACKET RBRACKET
//
#line 3997 "java.g"
void Parser::Act262()
{
    AstBrackets* p = DYNAMIC_CAST<AstBrackets*> (Sym(1));
    p -> right_bracket_token = Token(2);
    p -> dims++;
}

//
// Rule 263:  SuperAccess ::= super
//
#line 4011 "java.g"
void Parser::Act263()
{
    Sym(1) = ast_pool -> NewSuperExpression(Token(1));
}

//
// Rule 264:  SuperAccess ::= Name DOT Marker super
//
#line 4026 "java.g"
void Parser::Act264()
{
    AstSuperExpression* p = ast_pool -> NewSuperExpression(Token(4));
    p -> base_opt = ast_pool -> NewTypeName(DYNAMIC_CAST<AstName*> (Sym(1)));
    Sym(1) = p;
}

//
// Rule 265:  FieldAccess ::= Primary DOT Marker Identifier
//
#line 4041 "java.g"
void Parser::MakeFieldAccess()
{
    AstFieldAccess* p = ast_pool -> NewFieldAccess();
    p -> base = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> identifier_token = Token(4);
    Sym(1) = p;
}

//
// Rule 266:  FieldAccess ::= SuperAccess DOT Marker Identifier
//
// void MakeFieldAccess();
//

//
// Rule 267:  MethodInvocation ::= Identifier LPAREN ArgumentListopt RPAREN
//
#line 4071 "java.g"
void Parser::Act267()
{
    AstMethodInvocation* p = ast_pool -> NewMethodInvocation(Token(1));
    p -> arguments = MakeArguments(2);
    Sym(1) = p;
}

//
// Rule 268:  MethodInvocation ::= Name DOT Marker Identifier LPAREN ArgumentListopt RPAREN
//
#line 4087 "java.g"
void Parser::MakeMethodInvocation()
{
    AstMethodInvocation* p = ast_pool -> NewMethodInvocation(Token(4));
    p -> base_opt = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> type_arguments_opt = MakeExplicitTypeArguments(3);
    p -> arguments = MakeArguments(5);
    Sym(1) = p;
}

//
// Rule 269:  MethodInvocation ::= Name DOT TypeArguments Identifier LPAREN ArgumentListopt RPAREN
//
// void MakeMethodInvocation();
//

//
// Rule 270:  MethodInvocation ::= Primary DOT Marker Identifier LPAREN ArgumentListopt RPAREN
//
// void MakeMethodInvocation();
//

//
// Rule 271:  MethodInvocation ::= Primary DOT TypeArguments Identifier LPAREN ArgumentListopt RPAREN
//
// void MakeMethodInvocation();
//

//
// Rule 272:  MethodInvocation ::= SuperAccess DOT Marker Identifier LPAREN ArgumentListopt RPAREN
//
// void MakeMethodInvocation();
//

//
// Rule 273:  MethodInvocation ::= SuperAccess DOT TypeArguments Identifier LPAREN ArgumentListopt RPAREN
//
// void MakeMethodInvocation();
//

//
// Rule 274:  ArrayAccess ::= Name LBRACKET Expression RBRACKET
//
#line 4164 "java.g"
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
// Rule 275:  ArrayAccess ::= PrimaryNoNewArray LBRACKET Expression RBRACKET
//
// void MakeArrayAccess();
//

//
// Rule 276:  ArrayAccess ::= ArrayCreationInitialized LBRACKET Expression RBRACKET
//
// void MakeArrayAccess();
//

//
// Rule 277:  PostfixExpression ::= Primary
//
// void NoAction();
//

//
// Rule 278:  PostfixExpression ::= Name
//
// void NoAction();
//

//
// Rule 279:  PostfixExpression ::= PostIncrementExpression
//
// void NoAction();
//

//
// Rule 280:  PostfixExpression ::= PostDecrementExpression
//
// void NoAction();
//

//
// Rule 281:  PostfixExpressionNotName ::= Primary
//
// void NoAction();
//

//
// Rule 282:  PostfixExpressionNotName ::= PostIncrementExpression
//
// void NoAction();
//

//
// Rule 283:  PostfixExpressionNotName ::= PostDecrementExpression
//
// void NoAction();
//

//
// Rule 284:  PostIncrementExpression ::= PostfixExpression PLUS_PLUS
//
#line 4224 "java.g"
void Parser::Act284()
{
    AstPostUnaryExpression* p =
        ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::PLUSPLUS);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}

//
// Rule 285:  PostDecrementExpression ::= PostfixExpression MINUS_MINUS
//
#line 4237 "java.g"
void Parser::Act285()
{
    AstPostUnaryExpression* p =
        ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::MINUSMINUS);
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}

//
// Rule 286:  UnaryExpression ::= PreIncrementExpression
//
// void NoAction();
//

//
// Rule 287:  UnaryExpression ::= PreDecrementExpression
//
// void NoAction();
//

//
// Rule 288:  UnaryExpression ::= PLUS UnaryExpression
//
#line 4258 "java.g"
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
// Rule 289:  UnaryExpression ::= MINUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 290:  UnaryExpression ::= UnaryExpressionNotPlusMinus
//
// void NoAction();
//

//
// Rule 291:  UnaryExpressionNotName ::= PreIncrementExpression
//
// void NoAction();
//

//
// Rule 292:  UnaryExpressionNotName ::= PreDecrementExpression
//
// void NoAction();
//

//
// Rule 293:  UnaryExpressionNotName ::= PLUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 294:  UnaryExpressionNotName ::= MINUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 295:  UnaryExpressionNotName ::= UnaryExpressionNotPlusMinusNotName
//
// void NoAction();
//

//
// Rule 296:  PreIncrementExpression ::= PLUS_PLUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 297:  PreDecrementExpression ::= MINUS_MINUS UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 298:  UnaryExpressionNotPlusMinus ::= PostfixExpression
//
// void NoAction();
//

//
// Rule 299:  UnaryExpressionNotPlusMinus ::= TWIDDLE UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 300:  UnaryExpressionNotPlusMinus ::= NOT UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 301:  UnaryExpressionNotPlusMinus ::= CastExpression
//
// void NoAction();
//

//
// Rule 302:  UnaryExpressionNotPlusMinusNotName ::= PostfixExpressionNotName
//
// void NoAction();
//

//
// Rule 303:  UnaryExpressionNotPlusMinusNotName ::= TWIDDLE UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 304:  UnaryExpressionNotPlusMinusNotName ::= NOT UnaryExpression
//
// void MakePreUnaryExpression();
//

//
// Rule 305:  UnaryExpressionNotPlusMinusNotName ::= CastExpression
//
// void NoAction();
//

//
// Rule 306:  CastExpression ::= LPAREN PrimitiveType Dimsopt RPAREN UnaryExpression
//
#line 4364 "java.g"
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
// Rule 307:  CastExpression ::= LPAREN Name Marker RPAREN UnaryExpressionNotPlusMinus
//
// void MakeCastExpression();
//

//
// Rule 308:  CastExpression ::= LPAREN Name Dims RPAREN UnaryExpressionNotPlusMinus
//
// void MakeCastExpression();
//

//
// Rule 309:  CastExpression ::= LPAREN Name LESS TypeArgumentList1 Dimsopt RPAREN UnaryExpressionNotPlusMinus
//
#line 4405 "java.g"
void Parser::Act309()
{
    Sym(4) = MakeTypeArguments(2);
    MakeCastExpression(MakeArrayType(4), 6);
}

//
// Rule 310:  CastExpression ::= LPAREN Name LESS TypeArgumentList1 DOT ClassOrInterfaceType Dimsopt RPAREN...
//
#line 4419 "java.g"
void Parser::Act310()
{
    AstTypeName* p = DYNAMIC_CAST<AstTypeName*> (Sym(6));
    while (p -> base_opt)
        p = p -> base_opt;
    p -> base_opt = MakeTypeArguments(2);
    MakeCastExpression(MakeArrayType(6), 8);
}

//
// Rule 311:  MultiplicativeExpression ::= UnaryExpression
//
// void NoAction();
//

//
// Rule 312:  MultiplicativeExpression ::= MultiplicativeExpression MULTIPLY UnaryExpression
//
#line 4436 "java.g"
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
// Rule 313:  MultiplicativeExpression ::= MultiplicativeExpression DIVIDE UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 314:  MultiplicativeExpression ::= MultiplicativeExpression REMAINDER UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 315:  MultiplicativeExpressionNotName ::= UnaryExpressionNotName
//
// void NoAction();
//

//
// Rule 316:  MultiplicativeExpressionNotName ::= MultiplicativeExpressionNotName MULTIPLY UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 317:  MultiplicativeExpressionNotName ::= Name MULTIPLY UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 318:  MultiplicativeExpressionNotName ::= MultiplicativeExpressionNotName DIVIDE UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 319:  MultiplicativeExpressionNotName ::= Name DIVIDE UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 320:  MultiplicativeExpressionNotName ::= MultiplicativeExpressionNotName REMAINDER UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 321:  MultiplicativeExpressionNotName ::= Name REMAINDER UnaryExpression
//
// void MakeBinaryExpression();
//

//
// Rule 322:  AdditiveExpression ::= MultiplicativeExpression
//
// void NoAction();
//

//
// Rule 323:  AdditiveExpression ::= AdditiveExpression PLUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 324:  AdditiveExpression ::= AdditiveExpression MINUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 325:  AdditiveExpressionNotName ::= MultiplicativeExpressionNotName
//
// void NoAction();
//

//
// Rule 326:  AdditiveExpressionNotName ::= AdditiveExpressionNotName PLUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 327:  AdditiveExpressionNotName ::= Name PLUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 328:  AdditiveExpressionNotName ::= AdditiveExpressionNotName MINUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 329:  AdditiveExpressionNotName ::= Name MINUS MultiplicativeExpression
//
// void MakeBinaryExpression();
//

//
// Rule 330:  ShiftExpression ::= AdditiveExpression
//
// void NoAction();
//

//
// Rule 331:  ShiftExpression ::= ShiftExpression LEFT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 332:  ShiftExpression ::= ShiftExpression RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 333:  ShiftExpression ::= ShiftExpression UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 334:  ShiftExpressionNotName ::= AdditiveExpressionNotName
//
// void NoAction();
//

//
// Rule 335:  ShiftExpressionNotName ::= ShiftExpressionNotName LEFT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 336:  ShiftExpressionNotName ::= Name LEFT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 337:  ShiftExpressionNotName ::= ShiftExpressionNotName RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 338:  ShiftExpressionNotName ::= Name RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 339:  ShiftExpressionNotName ::= ShiftExpressionNotName UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 340:  ShiftExpressionNotName ::= Name UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
// void MakeBinaryExpression();
//

//
// Rule 341:  RelationalExpression ::= ShiftExpression
//
// void NoAction();
//

//
// Rule 342:  RelationalExpression ::= ShiftExpression LESS ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 343:  RelationalExpression ::= RelationalExpression GREATER ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 344:  RelationalExpression ::= RelationalExpression LESS_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 345:  RelationalExpression ::= RelationalExpression GREATER_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 346:  RelationalExpression ::= RelationalExpression instanceof ReferenceType
//
#line 4620 "java.g"
void Parser::MakeInstanceofExpression()
{
    AstInstanceofExpression* p = ast_pool -> NewInstanceofExpression();
    p -> expression = DYNAMIC_CAST<AstExpression*> (Sym(1));
    p -> instanceof_token = Token(2);
    p -> type = DYNAMIC_CAST<AstType*> (Sym(3));
    Sym(1) = p;
}

//
// Rule 347:  RelationalExpressionNotName ::= ShiftExpressionNotName
//
// void NoAction();
//

//
// Rule 348:  RelationalExpressionNotName ::= ShiftExpressionNotName LESS ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 349:  RelationalExpressionNotName ::= Name LESS ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 350:  RelationalExpressionNotName ::= ShiftExpressionNotName GREATER ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 351:  RelationalExpressionNotName ::= Name GREATER ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 352:  RelationalExpressionNotName ::= RelationalExpressionNotName LESS_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 353:  RelationalExpressionNotName ::= Name LESS_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 354:  RelationalExpressionNotName ::= RelationalExpressionNotName GREATER_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 355:  RelationalExpressionNotName ::= Name GREATER_EQUAL ShiftExpression
//
// void MakeBinaryExpression();
//

//
// Rule 356:  RelationalExpressionNotName ::= RelationalExpressionNotName instanceof ReferenceType
//
// void MakeInstanceofExpression();
//

//
// Rule 357:  RelationalExpressionNotName ::= Name instanceof ReferenceType
//
// void MakeInstanceofExpression();
//

//
// Rule 358:  EqualityExpression ::= RelationalExpression
//
// void NoAction();
//

//
// Rule 359:  EqualityExpression ::= EqualityExpression EQUAL_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 360:  EqualityExpression ::= EqualityExpression NOT_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 361:  EqualityExpressionNotName ::= RelationalExpressionNotName
//
// void NoAction();
//

//
// Rule 362:  EqualityExpressionNotName ::= EqualityExpressionNotName EQUAL_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 363:  EqualityExpressionNotName ::= Name EQUAL_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 364:  EqualityExpressionNotName ::= EqualityExpressionNotName NOT_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 365:  EqualityExpressionNotName ::= Name NOT_EQUAL RelationalExpression
//
// void MakeBinaryExpression();
//

//
// Rule 366:  AndExpression ::= EqualityExpression
//
// void NoAction();
//

//
// Rule 367:  AndExpression ::= AndExpression AND EqualityExpression
//
// void MakeBinaryExpression();
//

//
// Rule 368:  AndExpressionNotName ::= EqualityExpressionNotName
//
// void NoAction();
//

//
// Rule 369:  AndExpressionNotName ::= AndExpressionNotName AND EqualityExpression
//
// void MakeBinaryExpression();
//

//
// Rule 370:  AndExpressionNotName ::= Name AND EqualityExpression
//
// void MakeBinaryExpression();
//

//
// Rule 371:  ExclusiveOrExpression ::= AndExpression
//
// void NoAction();
//

//
// Rule 372:  ExclusiveOrExpression ::= ExclusiveOrExpression XOR AndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 373:  ExclusiveOrExpressionNotName ::= AndExpressionNotName
//
// void NoAction();
//

//
// Rule 374:  ExclusiveOrExpressionNotName ::= ExclusiveOrExpressionNotName XOR AndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 375:  ExclusiveOrExpressionNotName ::= Name XOR AndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 376:  InclusiveOrExpression ::= ExclusiveOrExpression
//
// void NoAction();
//

//
// Rule 377:  InclusiveOrExpression ::= InclusiveOrExpression OR ExclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 378:  InclusiveOrExpressionNotName ::= ExclusiveOrExpressionNotName
//
// void NoAction();
//

//
// Rule 379:  InclusiveOrExpressionNotName ::= InclusiveOrExpressionNotName OR ExclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 380:  InclusiveOrExpressionNotName ::= Name OR ExclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 381:  ConditionalAndExpression ::= InclusiveOrExpression
//
// void NoAction();
//

//
// Rule 382:  ConditionalAndExpression ::= ConditionalAndExpression AND_AND InclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 383:  ConditionalAndExpressionNotName ::= InclusiveOrExpressionNotName
//
// void NoAction();
//

//
// Rule 384:  ConditionalAndExpressionNotName ::= ConditionalAndExpressionNotName AND_AND InclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 385:  ConditionalAndExpressionNotName ::= Name AND_AND InclusiveOrExpression
//
// void MakeBinaryExpression();
//

//
// Rule 386:  ConditionalOrExpression ::= ConditionalAndExpression
//
// void NoAction();
//

//
// Rule 387:  ConditionalOrExpression ::= ConditionalOrExpression OR_OR ConditionalAndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 388:  ConditionalOrExpressionNotName ::= ConditionalAndExpressionNotName
//
// void NoAction();
//

//
// Rule 389:  ConditionalOrExpressionNotName ::= ConditionalOrExpressionNotName OR_OR ConditionalAndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 390:  ConditionalOrExpressionNotName ::= Name OR_OR ConditionalAndExpression
//
// void MakeBinaryExpression();
//

//
// Rule 391:  ConditionalExpression ::= ConditionalOrExpression
//
// void NoAction();
//

//
// Rule 392:  ConditionalExpression ::= ConditionalOrExpression QUESTION Expression COLON ConditionalExpression
//
#line 4834 "java.g"
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
// Rule 393:  ConditionalExpressionNotName ::= ConditionalOrExpressionNotName
//
// void NoAction();
//

//
// Rule 394:  ConditionalExpressionNotName ::= ConditionalOrExpressionNotName QUESTION Expression COLON...
//
// void MakeConditionalExpression();
//

//
// Rule 395:  ConditionalExpressionNotName ::= Name QUESTION Expression COLON ConditionalExpression
//
// void MakeConditionalExpression();
//

//
// Rule 396:  AssignmentExpression ::= ConditionalExpression
//
// void NoAction();
//

//
// Rule 397:  AssignmentExpression ::= Assignment
//
// void NoAction();
//

//
// Rule 398:  AssignmentExpressionNotName ::= ConditionalExpressionNotName
//
// void NoAction();
//

//
// Rule 399:  AssignmentExpressionNotName ::= Assignment
//
// void NoAction();
//

//
// Rule 400:  Assignment ::= PostfixExpression AssignmentOperator AssignmentExpression
//
#line 4891 "java.g"
void Parser::Act400()
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
// Rule 401:  AssignmentOperator ::= EQUAL
//
// void NoAction();
//

//
// Rule 402:  AssignmentOperator ::= MULTIPLY_EQUAL
//
// void NoAction();
//

//
// Rule 403:  AssignmentOperator ::= DIVIDE_EQUAL
//
// void NoAction();
//

//
// Rule 404:  AssignmentOperator ::= REMAINDER_EQUAL
//
// void NoAction();
//

//
// Rule 405:  AssignmentOperator ::= PLUS_EQUAL
//
// void NoAction();
//

//
// Rule 406:  AssignmentOperator ::= MINUS_EQUAL
//
// void NoAction();
//

//
// Rule 407:  AssignmentOperator ::= LEFT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 408:  AssignmentOperator ::= RIGHT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 409:  AssignmentOperator ::= UNSIGNED_RIGHT_SHIFT_EQUAL
//
// void NoAction();
//

//
// Rule 410:  AssignmentOperator ::= AND_EQUAL
//
// void NoAction();
//

//
// Rule 411:  AssignmentOperator ::= XOR_EQUAL
//
// void NoAction();
//

//
// Rule 412:  AssignmentOperator ::= OR_EQUAL
//
// void NoAction();
//

//
// Rule 413:  Expression ::= AssignmentExpression
//
// void NoAction();
//

//
// Rule 414:  ExpressionNotName ::= AssignmentExpressionNotName
//
// void NoAction();
//

//
// Rule 415:  ,opt ::=
//
// void NoAction();
//

//
// Rule 416:  ,opt ::= COMMA
//
// void NoAction();
//

//
// Rule 417:  Identifieropt ::=
//
// void NoAction();
//

//
// Rule 418:  Identifieropt ::= Identifier
//
// void NoAction();
//

//
// Rule 419:  Superopt ::=
//
#line 5015 "java.g"
//
// Given a rule of the form A ::= x1 x2 ... xn
//
// Construct a NULL Ast for A.
//
void Parser::NullAction() { Sym(1) = NULL; }

//
// Rule 420:  Superopt ::= Super
//
// void NoAction();
//

//
// Rule 421:  Expressionopt ::=
//
// void NullAction();
//

//
// Rule 422:  Expressionopt ::= Expression
//
// void NoAction();
//

//
// Rule 423:  ClassBodyopt ::=
//
// void NullAction();
//

//
// Rule 424:  ClassBodyopt ::= ClassBody
//
// void NoAction();
//

//
// Rule 425:  ImportDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 426:  ImportDeclarationsopt ::= ImportDeclarations
//
// void NoAction();
//

//
// Rule 427:  TypeDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 428:  TypeDeclarationsopt ::= TypeDeclarations
//
// void NoAction();
//

//
// Rule 429:  ClassBodyDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 430:  ClassBodyDeclarationsopt ::= ClassBodyDeclarations
//
// void NoAction();
//

//
// Rule 431:  Modifiersopt ::=
//
// void NullAction();
//

//
// Rule 432:  Modifiersopt ::= Modifiers
//
// void NoAction();
//

//
// Rule 433:  BlockStatementsopt ::=
//
// void NullAction();
//

//
// Rule 434:  BlockStatementsopt ::= BlockStatements
//
// void NoAction();
//

//
// Rule 435:  Dimsopt ::=
//
// void NullAction();
//

//
// Rule 436:  Dimsopt ::= Dims
//
// void NoAction();
//

//
// Rule 437:  ArgumentListopt ::=
//
// void NullAction();
//

//
// Rule 438:  ArgumentListopt ::= ArgumentList
//
// void NoAction();
//

//
// Rule 439:  SwitchLabelsopt ::=
//
// void NullAction();
//

//
// Rule 440:  SwitchLabelsopt ::= SwitchLabels
//
// void NoAction();
//

//
// Rule 441:  Throwsopt ::=
//
// void NullAction();
//

//
// Rule 442:  Throwsopt ::= Throws
//
// void NoAction();
//

//
// Rule 443:  FormalParameterListopt ::=
//
// void NullAction();
//

//
// Rule 444:  FormalParameterListopt ::= FormalParameterList
//
// void NoAction();
//

//
// Rule 445:  Interfacesopt ::=
//
// void NullAction();
//

//
// Rule 446:  Interfacesopt ::= Interfaces
//
// void NoAction();
//

//
// Rule 447:  InterfaceMemberDeclarationsopt ::=
//
// void NullAction();
//

//
// Rule 448:  InterfaceMemberDeclarationsopt ::= InterfaceMemberDeclarations
//
// void NoAction();
//

//
// Rule 449:  ForInitopt ::=
//
// void NullAction();
//

//
// Rule 450:  ForInitopt ::= ForInit
//
// void NoAction();
//

//
// Rule 451:  ForUpdateopt ::=
//
// void NullAction();
//

//
// Rule 452:  ForUpdateopt ::= ForUpdate
//
// void NoAction();
//

//
// Rule 453:  ExtendsInterfacesopt ::=
//
// void NullAction();
//

//
// Rule 454:  ExtendsInterfacesopt ::= ExtendsInterfaces
//
// void NoAction();
//

//
// Rule 455:  Catchesopt ::=
//
// void NullAction();
//

//
// Rule 456:  Catchesopt ::= Catches
//
// void NoAction();
//

//
// Rule 457:  MemberValuePairsopt ::=
//
// void NullAction();
//

//
// Rule 458:  MemberValuePairsopt ::= MemberValuePairs
//
// void NoAction();
//

//
// Rule 459:  TypeArgumentsopt ::=
//
// void NullAction();
//

//
// Rule 460:  TypeArgumentsopt ::= TypeArguments
//
// void NoAction();
//

//
// Rule 461:  TypeParametersopt ::=
//
// void NullAction();
//

//
// Rule 462:  TypeParametersopt ::= TypeParameters
//
// void NoAction();
//

//
// Rule 463:  TypeBoundopt ::=
//
// void NullAction();
//

//
// Rule 464:  TypeBoundopt ::= TypeBound
//
// void NoAction();
//

//
// Rule 465:  AdditionalBoundListopt ::=
//
// void NullAction();
//

//
// Rule 466:  AdditionalBoundListopt ::= AdditionalBoundList
//
// void NoAction();
//

//
// Rule 467:  PackageHeaderMarker ::=
//
#line 5244 "java.g"
//
// When this function is invoked, if the "parse_package_header_only" flag
// is turned on, we skip to the end-of-file token.
//
void Parser::Act467()
{
    if (parse_package_header_only)
        // point to the EOF token
        lex_stream -> Reset(lex_stream -> NumTokens() - 1);
    Sym(1) = NULL;
}

//
// Rule 468:  MethodHeaderMarker ::=
//
#line 5260 "java.g"
//
// When this function is invoked, if the "parse_header_only" flag
// is turned on, the body of the method being parsed is skipped.
//
void Parser::Act468()
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
// Rule 469:  Marker ::=
//
// void NullAction();
//

//
// Rule 470:  TypeArguments ::= LESS TypeArgumentList1
//
// void SetSym1ToSym2();
//

//
// Rule 471:  Wildcard ::= QUESTION Marker Marker Marker
//
#line 5318 "java.g"
void Parser::MakeWildcard()
{
    AstWildcard* p = ast_pool -> NewWildcard(Token(1));
    if (Token(3) > Token(2))
    {
        p -> extends_token_opt = Token(2);
        p -> bounds_opt = DYNAMIC_CAST<AstType*> (Sym(4));
    }
    else if (Token(4) > Token(3))
    {
        p -> super_token_opt = Token(3);
        p -> bounds_opt = DYNAMIC_CAST<AstType*> (Sym(4));
    }
    Sym(1) = p;
}

//
// Rule 472:  Wildcard ::= QUESTION extends Marker ReferenceType
//
// void MakeWildcard();
//

//
// Rule 473:  Wildcard ::= QUESTION Marker super ReferenceType
//
// void MakeWildcard();
//

//
// Rule 474:  Wildcard1 ::= QUESTION Marker Marker GREATER
//
// void MakeWildcard();
//

//
// Rule 475:  Wildcard1 ::= QUESTION extends Marker ReferenceType1
//
// void MakeWildcard();
//

//
// Rule 476:  Wildcard1 ::= QUESTION Marker super ReferenceType1
//
// void MakeWildcard();
//

//
// Rule 477:  Wildcard2 ::= QUESTION Marker Marker RIGHT_SHIFT
//
// void MakeWildcard();
//

//
// Rule 478:  Wildcard2 ::= QUESTION extends Marker ReferenceType2
//
// void MakeWildcard();
//

//
// Rule 479:  Wildcard2 ::= QUESTION Marker super ReferenceType2
//
// void MakeWildcard();
//

//
// Rule 480:  Wildcard3 ::= QUESTION Marker Marker UNSIGNED_RIGHT_SHIFT
//
// void MakeWildcard();
//

//
// Rule 481:  Wildcard3 ::= QUESTION extends Marker ReferenceType3
//
// void MakeWildcard();
//

//
// Rule 482:  Wildcard3 ::= QUESTION Marker super ReferenceType3
//
// void MakeWildcard();
//

//
// Rule 483:  TypeArgumentList ::= TypeArgument
//
// void StartList();
//

//
// Rule 484:  TypeArgumentList ::= TypeArgumentList COMMA TypeArgument
//
// void AddList3();
//

//
// Rule 485:  TypeArgumentList1 ::= TypeArgument1
//
// void StartList();
//

//
// Rule 486:  TypeArgumentList1 ::= TypeArgumentList COMMA TypeArgument1
//
// void AddList3();
//

//
// Rule 487:  TypeArgumentList2 ::= TypeArgument2
//
// void StartList();
//

//
// Rule 488:  TypeArgumentList2 ::= TypeArgumentList COMMA TypeArgument2
//
// void AddList3();
//

//
// Rule 489:  TypeArgumentList3 ::= TypeArgument3
//
// void StartList();
//

//
// Rule 490:  TypeArgumentList3 ::= TypeArgumentList COMMA TypeArgument3
//
// void AddList3();
//

//
// Rule 491:  TypeArgument ::= ReferenceType
//
// void NoAction();
//

//
// Rule 492:  TypeArgument ::= Wildcard
//
// void NoAction();
//

//
// Rule 493:  TypeArgument1 ::= ReferenceType1
//
// void NoAction();
//

//
// Rule 494:  TypeArgument1 ::= Wildcard1
//
// void NoAction();
//

//
// Rule 495:  TypeArgument2 ::= ReferenceType2
//
// void NoAction();
//

//
// Rule 496:  TypeArgument2 ::= Wildcard2
//
// void NoAction();
//

//
// Rule 497:  TypeArgument3 ::= ReferenceType3
//
// void NoAction();
//

//
// Rule 498:  TypeArgument3 ::= Wildcard3
//
// void NoAction();
//

//
// Rule 499:  ReferenceType1 ::= ReferenceType GREATER
//
// void NoAction();
//

//
// Rule 500:  ReferenceType1 ::= ClassOrInterface LESS TypeArgumentList2 Marker
//
// void MakeTypeArguments();
//

//
// Rule 501:  ReferenceType2 ::= ReferenceType RIGHT_SHIFT
//
// void NoAction();
//

//
// Rule 502:  ReferenceType2 ::= ClassOrInterface LESS TypeArgumentList3 Marker
//
// void MakeTypeArguments();
//

//
// Rule 503:  ReferenceType3 ::= ReferenceType UNSIGNED_RIGHT_SHIFT
//
// void NoAction();
//

//
// Rule 504:  TypeParameters ::= LESS TypeParameterList1
//
// void SetSym1ToSym2();
//

//
// Rule 505:  TypeParameterList ::= TypeParameter
//
// void StartList();
//

//
// Rule 506:  TypeParameterList ::= TypeParameterList COMMA TypeParameter
//
// void AddList3();
//

//
// Rule 507:  TypeParameterList1 ::= TypeParameter1
//
// void StartList();
//

//
// Rule 508:  TypeParameterList1 ::= TypeParameterList COMMA TypeParameter1
//
// void AddList3();
//

//
// Rule 509:  TypeParameter ::= Identifier TypeBoundopt
//
#line 5655 "java.g"
void Parser::MakeTypeParameter()
{
    AstTypeParameter* p = ast_pool -> NewTypeParameter(Token(1));
    if (Sym(2))
    {
        //
        // Remember, we built the circular list with the first element at
        // the tail of the list, because of the grammar of TypeBound.
        //
        AstListNode* tail = DYNAMIC_CAST<AstListNode*> (Sym(2));
        p -> AllocateBounds(tail -> index + 1);
        AstListNode* root = tail;
        do
        {
            p -> AddBound(DYNAMIC_CAST<AstTypeName*> (root -> element));
            root = root -> next;
        } while (root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 510:  TypeParameter1 ::= Identifier Marker GREATER
//
// void MakeTypeParameter();
//

//
// Rule 511:  TypeParameter1 ::= Identifier TypeBound1
//
// void MakeTypeParameter();
//

//
// Rule 512:  TypeBound ::= extends ReferenceType AdditionalBoundListopt
//
#line 5705 "java.g"
void Parser::MakeTypeBound()
{
    //
    // Unlike most AstListNodes, we stick Sym(2) at the end of the list
    // instead of the beginning. MakeTypeParameter expects this unusual
    // ordering, because it is easier than rewriting the grammar to build the
    // list in lexical order.
    //
    if (Sym(3))
    {
        Sym(1) = Sym(3);
        AddList2();
    }
    else
    {
        Sym(1) = Sym(2);
        StartList();
    }
}

//
// Rule 513:  TypeBound1 ::= extends ReferenceType1 Marker
//
// void MakeTypeBound();
//

//
// Rule 514:  TypeBound1 ::= extends ReferenceType AdditionalBoundList1
//
// void MakeTypeBound();
//

//
// Rule 515:  AdditionalBoundList ::= AdditionalBound
//
// void StartList();
//

//
// Rule 516:  AdditionalBoundList ::= AdditionalBoundList AdditionalBound
//
// void AddList2();
//

//
// Rule 517:  AdditionalBoundList1 ::= AdditionalBound1
//
// void StartList();
//

//
// Rule 518:  AdditionalBoundList1 ::= AdditionalBoundList AdditionalBound1
//
// void AddList2();
//

//
// Rule 519:  AdditionalBound ::= AND ClassOrInterfaceType
//
// void SetSym1ToSym2();
//

//
// Rule 520:  AdditionalBound1 ::= AND ClassOrInterfaceType1
//
// void SetSym1ToSym2();
//

//
// Rule 521:  ClassOrInterfaceType1 ::= ClassOrInterfaceType GREATER
//
// void NoAction();
//

//
// Rule 522:  ClassOrInterfaceType1 ::= ClassOrInterface LESS TypeArgumentList2 Marker
//
// void MakeTypeArguments();
//
#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif
