#line 419 "java.g"
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#include "config.h"
#include "parser.h"
#include "ast.h"

#undef HEADERS
#include "javaact.h"

//****************************************************************************//
//****************************************************************************//
//*                                                                          *//
//* Below, we show each rule of the Java grammar together with the semantic  *//
//* action that is invoked when the parser performs a reduction by that rule.*//
//*                                                                          *//
//****************************************************************************//
//****************************************************************************//

 
// 
// Rule 1:  Goal -> CompilationUnit
//
#line 451 "java.g"
//
// Given a rule of the form A ::= x1 x2 ... xn        n >= 1
//
// Do nothing - Whatever Ast was produced for x1 is inherited by A.
//
void Parser::BadAction(void) { assert(0); }
void Parser::NoAction(void) {}
 
// 
// Rule 2:  Goal ::= BodyMarker ConstructorBody
//
#line 463 "java.g"
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
void Parser::Act2(void)
{
    Sym(1) = Sym(2);
}
 
// 
// Rule 3:  Literal ::= IntegerLiteral
//
#line 488 "java.g"
void Parser::Act3(void)
{
    Sym(1) = ast_pool -> NewIntegerLiteral(Token(1));
}
 
// 
// Rule 4:  Literal ::= LongLiteral
//
#line 497 "java.g"
void Parser::Act4(void)
{
    Sym(1) = ast_pool -> NewLongLiteral(Token(1));
}
 
// 
// Rule 5:  Literal ::= FloatingPointLiteral
//
#line 506 "java.g"
void Parser::Act5(void)
{
    Sym(1) = ast_pool -> NewFloatingPointLiteral(Token(1));
}
 
// 
// Rule 6:  Literal ::= DoubleLiteral
//
#line 515 "java.g"
void Parser::Act6(void)
{
    Sym(1) = ast_pool -> NewDoubleLiteral(Token(1));
}

//
// Rule 7:  Literal -> BooleanLiteral
//
// void NoAction(void);
//
 
// 
// Rule 8:  Literal ::= CharacterLiteral
//
#line 528 "java.g"
void Parser::Act8(void)
{
    Sym(1) = ast_pool -> NewCharacterLiteral(Token(1));
}
 
// 
// Rule 9:  Literal ::= StringLiteral
//
#line 537 "java.g"
void Parser::Act9(void)
{
    Sym(1) = ast_pool -> NewStringLiteral(Token(1));
}
 
// 
// Rule 10:  Literal ::= null
//
#line 546 "java.g"
void Parser::Act10(void)
{
    Sym(1) = ast_pool -> NewNullLiteral(Token(1));
}
 
// 
// Rule 11:  BooleanLiteral ::= true
//
#line 555 "java.g"
void Parser::Act11(void)
{
    Sym(1) = ast_pool -> NewTrueLiteral(Token(1));
}
 
// 
// Rule 12:  BooleanLiteral ::= false
//
#line 564 "java.g"
void Parser::Act12(void)
{
    Sym(1) = ast_pool -> NewFalseLiteral(Token(1));
}

//
// Rule 13:  Type -> PrimitiveType
//
// void NoAction(void);
//

//
// Rule 14:  Type -> ReferenceType
//
// void NoAction(void);
//

//
// Rule 15:  PrimitiveType -> NumericType
//
// void NoAction(void);
//
 
// 
// Rule 16:  PrimitiveType ::= boolean
//
#line 587 "java.g"
void Parser::Act16(void)
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::BOOLEAN, Token(1));
}

//
// Rule 17:  NumericType -> IntegralType
//
// void NoAction(void);
//

//
// Rule 18:  NumericType -> FloatingPointType
//
// void NoAction(void);
//
 
// 
// Rule 19:  IntegralType ::= byte
//
#line 604 "java.g"
void Parser::Act19(void)
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::BYTE, Token(1));
}
 
// 
// Rule 20:  IntegralType ::= short
//
#line 613 "java.g"
void Parser::Act20(void)
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::SHORT, Token(1));
}
 
// 
// Rule 21:  IntegralType ::= int
//
#line 622 "java.g"
void Parser::Act21(void)
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::INT, Token(1));
}
 
// 
// Rule 22:  IntegralType ::= long
//
#line 631 "java.g"
void Parser::Act22(void)
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::LONG, Token(1));
}
 
// 
// Rule 23:  IntegralType ::= char
//
#line 640 "java.g"
void Parser::Act23(void)
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::CHAR, Token(1));
}
 
// 
// Rule 24:  FloatingPointType ::= float
//
#line 649 "java.g"
void Parser::Act24(void)
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::FLOAT, Token(1));
}
 
// 
// Rule 25:  FloatingPointType ::= double
//
#line 658 "java.g"
void Parser::Act25(void)
{
    Sym(1) = ast_pool -> NewPrimitiveType(Ast::DOUBLE, Token(1));
}

//
// Rule 26:  ReferenceType -> ClassOrInterfaceType
//
// void NoAction(void);
//

//
// Rule 27:  ReferenceType -> ArrayType
//
// void NoAction(void);
//

//
// Rule 28:  ClassOrInterfaceType -> Name
//
// void NoAction(void);
//
 
// 
// Rule 29:  ArrayType ::= PrimitiveType Dims
//
#line 687 "java.g"
void Parser::MakeArrayType(void)
{
    AstArrayType *p = ast_pool -> NewArrayType();
    p -> type     = Sym(1);
    //
    // The list of modifiers is guaranteed not empty
    //
    {   
        AstListNode *tail = (AstListNode *) Sym(2);
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets((AstBrackets *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 30:  ArrayType ::= Name Dims
//
// void MakeArrayType(void);
//

//
// Rule 31:  ClassType -> ClassOrInterfaceType
//
// void NoAction(void);
//

//
// Rule 32:  InterfaceType -> ClassOrInterfaceType
//
// void NoAction(void);
//

//
// Rule 33:  Name -> SimpleName
//
// void NoAction(void);
//

//
// Rule 34:  Name -> QualifiedName
//
// void NoAction(void);
//
 
// 
// Rule 35:  SimpleName ::= Identifier
//
#line 737 "java.g"
void Parser::MakeSimpleName(void)
{
    Sym(1) = ast_pool -> NewSimpleName(Token(1));
}
 
// 
// Rule 36:  QualifiedName ::= Name DOT Identifier
//
#line 746 "java.g"
void Parser::MakeFieldAccess(void)
{ 
    AstFieldAccess *p = ast_pool -> NewFieldAccess();
    p -> base = (AstExpression *) Sym(1);
    p -> dot_token = Token(2);
    p -> identifier_token = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 37:  CompilationUnit ::= PackageDeclarationopt ImportDeclarationsopt TypeDeclarationsopt
//
#line 761 "java.g"
void Parser::Act37(void)
{
    AstCompilationUnit *p = ast_pool -> NewCompilationUnit();
    p -> package_declaration_opt = (AstPackageDeclaration *) Sym(1);
    if (Sym(2) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(2);
        p -> AllocateImportDeclarations(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddImportDeclaration((AstImportDeclaration *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    if (Sym(3) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateTypeDeclarations(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddTypeDeclaration(root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}
 
// 
// Rule 38:  ImportDeclarations ::= ImportDeclaration
//
#line 796 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act38(void)
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
#line 813 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act39(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

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
#line 834 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act40(void)
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
#line 851 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act41(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

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
#line 872 "java.g"
void Parser::Act42(void)
{
    AstPackageDeclaration *p = ast_pool -> NewPackageDeclaration();
    p -> package_token   = Token(1);
    p -> name            = (AstExpression *) Sym(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}

//
// Rule 43:  ImportDeclaration -> SingleTypeImportDeclaration
//
// void NoAction(void);
//

//
// Rule 44:  ImportDeclaration -> TypeImportOnDemandDeclaration
//
// void NoAction(void);
//
 
// 
// Rule 45:  SingleTypeImportDeclaration ::= import Name SEMICOLON
//
#line 893 "java.g"
void Parser::Act45(void)
{
    AstImportDeclaration *p = ast_pool -> NewImportDeclaration();
    p -> import_token    = Token(1);
    p -> name            = (AstExpression *) Sym(2);
    p -> star_token_opt  = 0;
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 46:  TypeImportOnDemandDeclaration ::= import Name DOT MULTIPLY SEMICOLON
//
#line 907 "java.g"
void Parser::Act46(void)
{
    AstImportDeclaration *p = ast_pool -> NewImportDeclaration();
    p -> import_token         = Token(1);
    p -> name                 = (AstExpression *) Sym(2);
    p -> star_token_opt       = Token(4);
    p -> semicolon_token      = Token(5);
    Sym(1) = p;
}

//
// Rule 47:  TypeDeclaration -> ClassDeclaration
//
// void NoAction(void);
//

//
// Rule 48:  TypeDeclaration -> InterfaceDeclaration
//
// void NoAction(void);
//
 
// 
// Rule 49:  TypeDeclaration ::= SEMICOLON
//
#line 929 "java.g"
void Parser::Act49(void)
{
    Sym(1) = ast_pool -> NewEmptyDeclaration(Token(1));
}
 
// 
// Rule 50:  Modifiers ::= Modifier
//
#line 940 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act50(void)
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
#line 957 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act51(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

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
#line 978 "java.g"
void Parser::Act52(void)
{
    Sym(1) = ast_pool -> NewModifier(Ast::PUBLIC, Token(1));
}
 
// 
// Rule 53:  Modifier ::= protected
//
#line 987 "java.g"
void Parser::Act53(void)
{
    Sym(1) = ast_pool -> NewModifier(Ast::PROTECTED, Token(1));
}
 
// 
// Rule 54:  Modifier ::= private
//
#line 996 "java.g"
void Parser::Act54(void)
{
    Sym(1) = ast_pool -> NewModifier(Ast::PRIVATE, Token(1));
}
 
// 
// Rule 55:  Modifier ::= static
//
#line 1005 "java.g"
void Parser::Act55(void)
{
    Sym(1) = ast_pool -> NewModifier(Ast::STATIC, Token(1));
}
 
// 
// Rule 56:  Modifier ::= abstract
//
#line 1014 "java.g"
void Parser::Act56(void)
{
    Sym(1) = ast_pool -> NewModifier(Ast::ABSTRACT, Token(1));
}
 
// 
// Rule 57:  Modifier ::= final
//
#line 1023 "java.g"
void Parser::Act57(void)
{
    Sym(1) = ast_pool -> NewModifier(Ast::FINAL, Token(1));
}
 
// 
// Rule 58:  Modifier ::= native
//
#line 1032 "java.g"
void Parser::Act58(void)
{
    Sym(1) = ast_pool -> NewModifier(Ast::NATIVE, Token(1));
}
 
// 
// Rule 59:  Modifier ::= synchronized
//
#line 1041 "java.g"
void Parser::Act59(void)
{
    Sym(1) = ast_pool -> NewModifier(Ast::SYNCHRONIZED, Token(1));
}
 
// 
// Rule 60:  Modifier ::= transient
//
#line 1050 "java.g"
void Parser::Act60(void)
{
    Sym(1) = ast_pool -> NewModifier(Ast::TRANSIENT, Token(1));
}
 
// 
// Rule 61:  Modifier ::= volatile
//
#line 1059 "java.g"
void Parser::Act61(void)
{
    Sym(1) = ast_pool -> NewModifier(Ast::VOLATILE, Token(1));
}
 
// 
// Rule 62:  ClassDeclaration ::= Modifiersopt class Identifier Superopt Interfacesopt ClassBody
//
#line 1075 "java.g"
void Parser::Act62(void)
{
    AstClassDeclaration *p = ast_pool -> NewClassDeclaration();
    if (Sym(1) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(1);
        p -> AllocateClassModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddClassModifier((AstModifier *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> class_token          = Token(2);
    p -> identifier_token     = Token(3);
    p -> super_opt            = (AstExpression *) Sym(4);
    if (Sym(5) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(5);
        p -> AllocateInterfaces(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddInterface((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> class_body = (AstClassBody *) Sym(6);
    Sym(1) = p;
}
 
// 
// Rule 63:  Super ::= extends ClassType
//
#line 1113 "java.g"
void Parser::SetSym1ToSym2(void) { Sym(1) = Sym(2); }

//
// Rule 64:  Interfaces ::= implements InterfaceTypeList
//
// void SetSym1ToSym2(void);
//
 
// 
// Rule 65:  InterfaceTypeList ::= InterfaceType
//
#line 1126 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act65(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 66:  InterfaceTypeList ::= InterfaceTypeList COMMA InterfaceType
//
#line 1143 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act66(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}
 
// 
// Rule 67:  ClassBody ::= LBRACE ClassBodyDeclarationsopt RBRACE
//
#line 1164 "java.g"
void Parser::Act67(void)
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

        AstListNode *tail = (AstListNode *) Sym(2);
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
        } while(root != tail);

        p -> AllocateInstanceVariables(num_instance_variables);
        p -> AllocateClassVariables(num_class_variables);
        p -> AllocateMethods(num_methods);
        p -> AllocateConstructors(num_constructors);
        p -> AllocateStaticInitializers(num_static_initializers);
        p -> AllocateNestedClasses(num_inner_classes);
        p -> AllocateNestedInterfaces(num_inner_interfaces);
        p -> AllocateBlocks(num_blocks);
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
            AstBlock *block;

            if (field_declaration = root -> element -> FieldDeclarationCast())
            {
                if (field_declaration -> StaticFieldCast())
                     p -> AddClassVariable(field_declaration);
                else p -> AddInstanceVariable(field_declaration);
            }
            else if (method_declaration = root -> element -> MethodDeclarationCast())
            {
                p -> AddMethod(method_declaration);
            }
            else if (constructor_declaration = root -> element -> ConstructorDeclarationCast())
            {
                p -> AddConstructor(constructor_declaration);
            }
            else if (static_initializer = root -> element -> StaticInitializerCast())
            {
                p -> AddStaticInitializer(static_initializer);
            }
            else if (class_declaration = root -> element -> ClassDeclarationCast())
            {
                p -> AddNestedClass(class_declaration);
            }
            else if (interface_declaration = root -> element -> InterfaceDeclarationCast())
            {
                p -> AddNestedInterface(interface_declaration);
            }
            else if (block = root -> element -> BlockCast())
            {
                p -> AddBlock(block);
            }
            else // assert(block = root -> element -> EmptyDeclarationCast())
            {
                p -> AddEmptyDeclaration((AstEmptyDeclaration *) root -> element);
            }
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(3);
    p -> pool = body_pool; // from now on, this is the storage pool to use for this type
    Sym(1) = p;
}
 
// 
// Rule 68:  ClassBodyDeclarations ::= ClassBodyDeclaration
//
#line 1302 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act68(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 69:  ClassBodyDeclarations ::= ClassBodyDeclarations ClassBodyDeclaration
//
#line 1319 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act69(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 70:  ClassBodyDeclaration -> ClassMemberDeclaration
//
// void NoAction(void);
//

//
// Rule 71:  ClassBodyDeclaration -> StaticInitializer
//
// void NoAction(void);
//

//
// Rule 72:  ClassBodyDeclaration -> ConstructorDeclaration
//
// void NoAction(void);
//
 
// 
// Rule 73:  ClassBodyDeclaration ::= MethodHeaderMarker Block
//
#line 1353 "java.g"
void Parser::Act73(void)
{
    Sym(1) = Sym(2);
}

//
// Rule 74:  ClassMemberDeclaration -> FieldDeclaration
//
// void NoAction(void);
//

//
// Rule 75:  ClassMemberDeclaration -> MethodDeclaration
//
// void NoAction(void);
//

//
// Rule 76:  ClassMemberDeclaration -> ClassDeclaration
//
// void NoAction(void);
//

//
// Rule 77:  ClassMemberDeclaration -> InterfaceDeclaration
//
// void NoAction(void);
//
 
// 
// Rule 78:  ClassMemberDeclaration ::= SEMICOLON
//
#line 1387 "java.g"
void Parser::Act78(void)
{
    Sym(1) = ast_pool -> NewEmptyDeclaration(Token(1));
}
 
// 
// Rule 79:  FieldDeclaration ::= Modifiersopt Type VariableDeclarators SEMICOLON
//
#line 1406 "java.g"
void Parser::Act79(void)
{
    AstFieldDeclaration *p = ast_pool -> NewFieldDeclaration();
    if (Sym(1) != NULL)
    {   
        AstListNode *tail = (AstListNode *) Sym(1);
        p -> AllocateVariableModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableModifier((AstModifier *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> type = Sym(2);
    //
    // The list of declarators is guaranteed not empty
    //
    {   
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateVariableDeclarators(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableDeclarator((AstVariableDeclarator *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> semicolon_token      = Token(4);
    Sym(1) = p;
}
 
// 
// Rule 80:  VariableDeclarators ::= VariableDeclarator
//
#line 1444 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act80(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 81:  VariableDeclarators ::= VariableDeclarators COMMA VariableDeclarator
//
#line 1461 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act81(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}
 
// 
// Rule 82:  VariableDeclarator ::= VariableDeclaratorId
//
#line 1482 "java.g"
void Parser::Act82(void)
{
    AstVariableDeclarator *p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name = (AstVariableDeclaratorId *) Sym(1);
    p -> variable_initializer_opt = NULL;
    Sym(1) = p;
}
 
// 
// Rule 83:  VariableDeclarator ::= VariableDeclaratorId EQUAL VariableInitializer
//
#line 1494 "java.g"
void Parser::Act83(void)
{
    AstVariableDeclarator *p = ast_pool -> NewVariableDeclarator();
    p -> variable_declarator_name = (AstVariableDeclaratorId *) Sym(1);
    p -> variable_initializer_opt = Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 84:  VariableDeclaratorId ::= Identifier Dimsopt
//
#line 1506 "java.g"
void Parser::Act84(void)
{
    AstVariableDeclaratorId *p = ast_pool -> NewVariableDeclaratorId();
    p -> identifier_token = Token(1);
    if (Sym(2) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(2);
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets((AstBrackets *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 85:  VariableInitializer -> Expression
//
// void NoAction(void);
//

//
// Rule 86:  VariableInitializer -> ArrayInitializer
//
// void NoAction(void);
//
 
// 
// Rule 87:  MethodDeclaration ::= MethodHeader MethodHeaderMarker MethodBody
//
#line 1554 "java.g"
void Parser::Act87(void)
{
    ((AstMethodDeclaration *) Sym(1)) -> method_body = (AstStatement *) Sym(3);
}
 
// 
// Rule 88:  MethodHeader ::= Modifiersopt Type MethodDeclarator Throwsopt
//
#line 1563 "java.g"
void Parser::Act88(void)
{
    AstMethodDeclaration *p = ast_pool -> NewMethodDeclaration();
    if (Sym(1) != NULL)
    {   
        AstListNode *tail = (AstListNode *) Sym(1);
        p -> AllocateMethodModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddMethodModifier((AstModifier *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> type              = Sym(2);
    p -> method_declarator = (AstMethodDeclarator *) Sym(3);
    if (Sym(4) != NULL)
    {   
        AstListNode *tail = (AstListNode *) Sym(4);
        p -> AllocateThrows(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddThrow((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}
 
// 
// Rule 89:  MethodHeader ::= Modifiersopt void MethodDeclarator Throwsopt
//
#line 1599 "java.g"
void Parser::Act89(void)
{
    AstMethodDeclaration *p = ast_pool -> NewMethodDeclaration();
    if (Sym(1) != NULL)
    {   
        AstListNode *tail = (AstListNode *) Sym(1);
        p -> AllocateMethodModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddMethodModifier((AstModifier *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> type              = ast_pool -> NewPrimitiveType(Ast::VOID_TYPE, Token(2));
    p -> method_declarator = (AstMethodDeclarator *) Sym(3);
    if (Sym(4) != NULL)
    {   
        AstListNode *tail = (AstListNode *) Sym(4);
        p -> AllocateThrows(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddThrow((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}
 
// 
// Rule 90:  MethodDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN Dimsopt
//
#line 1635 "java.g"
void Parser::Act90(void)
{
    AstMethodDeclarator *p = ast_pool -> NewMethodDeclarator();
    p -> identifier_token        = Token(1);
    p -> left_parenthesis_token  = Token(2);
    if (Sym(3) != NULL)
    {   
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateFormalParameters(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddFormalParameter((AstFormalParameter *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    if (Sym(5) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(5);
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets((AstBrackets *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}
 
// 
// Rule 91:  FormalParameterList ::= FormalParameter
//
#line 1672 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act91(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 92:  FormalParameterList ::= FormalParameterList COMMA FormalParameter
//
#line 1689 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act92(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}
 
// 
// Rule 93:  FormalParameter ::= Type VariableDeclaratorId
//
#line 1710 "java.g"
void Parser::Act93(void)
{
    AstFormalParameter *p = ast_pool -> NewFormalParameter();
    p -> type                     = Sym(1);
    p -> variable_declarator_name = (AstVariableDeclaratorId *) Sym(2);
    Sym(1) = p;
}
 
// 
// Rule 94:  FormalParameter ::= Modifiers Type VariableDeclaratorId
//
#line 1723 "java.g"
void Parser::Act94(void)
{
    AstFormalParameter *p = ast_pool -> NewFormalParameter();
    //
    // The list of modifiers is guaranteed not empty
    //
    {   
        AstListNode *tail = (AstListNode *) Sym(1);
        p -> AllocateParameterModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddParameterModifier((AstModifier *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> type                     = Sym(2);
    p -> variable_declarator_name = (AstVariableDeclaratorId *) Sym(3);
    Sym(1) = p;
}

//
// Rule 95:  Throws ::= throws ClassTypeList
//
// void SetSym1ToSym2(void);
//
 
// 
// Rule 96:  ClassTypeList ::= ClassType
//
#line 1756 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act96(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 97:  ClassTypeList ::= ClassTypeList COMMA ClassType
//
#line 1773 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act97(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 98:  MethodBody -> Block
//
// void NoAction(void);
//
 
// 
// Rule 99:  MethodBody ::= SEMICOLON
//
#line 1798 "java.g"
void Parser::MakeEmptyStatement(void)
{
    Sym(1) = ast_pool -> NewEmptyStatement(Token(1));
}
 
// 
// Rule 100:  StaticInitializer ::= static MethodHeaderMarker Block
//
#line 1809 "java.g"
void Parser::Act100(void)
{
    AstStaticInitializer *p = ast_pool -> NewStaticInitializer();
    p -> static_token = Token(1);
    p -> block        = (AstBlock *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 101:  ConstructorDeclaration ::= Modifiersopt ConstructorDeclarator Throwsopt MethodHeaderMarker ConstructorBody
//
#line 1834 "java.g"
void Parser::Act101(void)
{
    AstConstructorBlock *block = Sym(5) -> ConstructorBlockCast();
    if (! block)
    {
        block = ast_pool -> NewConstructorBlock();
        block -> left_brace_token                    = Sym(5) -> LeftToken();
        block -> explicit_constructor_invocation_opt = NULL;
        block -> block                               = (AstBlock *) Sym(5);
        block -> right_brace_token                   = Sym(5) -> RightToken();
    }

    AstConstructorDeclaration *p = ast_pool -> NewConstructorDeclaration();

    if (Sym(1) != NULL)
    {   
        AstListNode *tail = (AstListNode *) Sym(1);
        p -> AllocateConstructorModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddConstructorModifier((AstModifier *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> constructor_declarator = (AstMethodDeclarator *) Sym(2);
    if (Sym(3) != NULL)
    {   
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateThrows(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddThrow((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> constructor_body       = block;

    Sym(1) = p;
}
 
// 
// Rule 102:  ConstructorDeclarator ::= Identifier LPAREN FormalParameterListopt RPAREN
//
#line 1890 "java.g"
void Parser::Act102(void)
{
    AstMethodDeclarator *p = ast_pool -> NewMethodDeclarator();
    p -> identifier_token        = Token(1);
    p -> left_parenthesis_token  = Token(2);
    if (Sym(3) != NULL)
    {   
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateFormalParameters(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddFormalParameter((AstFormalParameter *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    Sym(1) = p;
}

//
// Rule 103:  ConstructorBody -> Block
//
// void NoAction(void);
//
 
// 
// Rule 104:  ConstructorBody ::= LBRACE ExplicitConstructorInvocation BlockStatementsopt RBRACE
//
#line 1925 "java.g"
void Parser::Act104(void)
{
    AstBlock *block = ast_pool -> NewBlock();
    if (Sym(3) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(3);
        block -> AllocateBlockStatements(tail -> index + 1);
        AstListNode *root = tail;
        block -> left_brace_token  = root -> element -> LeftToken();
        block -> right_brace_token = tail -> element -> RightToken();
        do
        {
            root = root -> next;
            block -> AddStatement((AstStatement *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    else
    {
        block -> left_brace_token  = Token(4);
        block -> right_brace_token = Token(4);
    }

    AstConstructorBlock *p = ast_pool -> NewConstructorBlock();
    p -> left_brace_token                    = Token(1);
    p -> explicit_constructor_invocation_opt = Sym(2);
    p -> block                               = block;
    p -> right_brace_token                   = Token(4);
    Sym(1) = p;
}
 
// 
// Rule 105:  ExplicitConstructorInvocation ::= this LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 1960 "java.g"
void Parser::Act105(void)
{
    AstThisCall *p = ast_pool -> NewThisCall();
    p -> this_token              = Token(1);
    p -> left_parenthesis_token  = Token(2);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    p -> semicolon_token         = Token(5);
    Sym(1) = p;
}
 
// 
// Rule 106:  ExplicitConstructorInvocation ::= super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 1986 "java.g"
void Parser::Act106(void)
{
    AstSuperCall *p = ast_pool -> NewSuperCall();
    p -> base_opt                = NULL;
    p -> dot_token_opt           = 0;
    p -> super_token             = Token(1);
    p -> left_parenthesis_token  = Token(2);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    p -> semicolon_token         = Token(5);
    Sym(1) = p;
}
 
// 
// Rule 107:  ExplicitConstructorInvocation ::= Primary DOT super LPAREN ArgumentListopt RPAREN SEMICOLON
//
#line 2015 "java.g"
void Parser::MakeQualifiedSuper(void)
{
    AstSuperCall *p = ast_pool -> NewSuperCall();
    p -> base_opt                = (AstExpression *) Sym(1);
    p -> dot_token_opt           = Token(2);
    p -> super_token             = Token(3);
    p -> left_parenthesis_token  = Token(4);
    if (Sym(5) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(5);
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(6);
    p -> semicolon_token         = Token(7);
    Sym(1) = p;
}

//
// Rule 108:  ExplicitConstructorInvocation ::= Name DOT super LPAREN ArgumentListopt RPAREN SEMICOLON
//
// void MakeQualifiedSuper(void);
//
 
// 
// Rule 109:  InterfaceDeclaration ::= Modifiersopt interface Identifier ExtendsInterfacesopt InterfaceBody
//
#line 2058 "java.g"
void Parser::Act109(void)
{
    AstInterfaceDeclaration *p = (AstInterfaceDeclaration *) Sym(5);
    if (Sym(1) != NULL)
    {   
        AstListNode *tail = (AstListNode *) Sym(1);
        p -> AllocateInterfaceModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddInterfaceModifier((AstModifier *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> interface_token  = Token(2);
    p -> identifier_token = Token(3);
    if (Sym(4) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(4);
        p -> AllocateExtendsInterfaces(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddExtendsInterface((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}

//
// Rule 110:  ExtendsInterfaces ::= extends InterfaceTypeList
//
// void SetSym1ToSym2(void);
//
 
// 
// Rule 111:  InterfaceBody ::= LBRACE InterfaceMemberDeclarationsopt RBRACE
//
#line 2101 "java.g"
void Parser::Act111(void)
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

        AstListNode *tail = (AstListNode *) Sym(2);
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
        } while(root != tail);

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

            if (field_declaration = root -> element -> FieldDeclarationCast())
            {
                p -> AddClassVariable(field_declaration);
            }
            else if (method_declaration = root -> element -> MethodDeclarationCast())
            {
                p -> AddMethod(method_declaration);
            }
            else if (class_declaration = root -> element -> ClassDeclarationCast())
            {
                p -> AddNestedClass(class_declaration);
            }
            else if (interface_declaration = root -> element -> InterfaceDeclarationCast())
            {
                p -> AddNestedInterface(interface_declaration);
            }
            else // assert(interface_declaration = root -> element -> EmptyDeclarationCast())
            {
                p -> AddEmptyDeclaration((AstEmptyDeclaration *) root -> element);
            }
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(3);
    p -> pool = body_pool; // from now on, this is the storage pool to use for this type
    Sym(1) = p;
}
 
// 
// Rule 112:  InterfaceMemberDeclarations ::= InterfaceMemberDeclaration
//
#line 2193 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act112(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 113:  InterfaceMemberDeclarations ::= InterfaceMemberDeclarations InterfaceMemberDeclaration
//
#line 2210 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act113(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 114:  InterfaceMemberDeclaration -> ConstantDeclaration
//
// void NoAction(void);
//

//
// Rule 115:  InterfaceMemberDeclaration -> AbstractMethodDeclaration
//
// void NoAction(void);
//

//
// Rule 116:  InterfaceMemberDeclaration -> ClassDeclaration
//
// void NoAction(void);
//

//
// Rule 117:  InterfaceMemberDeclaration -> InterfaceDeclaration
//
// void NoAction(void);
//
 
// 
// Rule 118:  InterfaceMemberDeclaration ::= SEMICOLON
//
#line 2256 "java.g"
void Parser::Act118(void)
{
    Sym(1) = ast_pool -> NewEmptyDeclaration(Token(1));
}

//
// Rule 119:  ConstantDeclaration -> FieldDeclaration
//
// void NoAction(void);
//
 
// 
// Rule 120:  AbstractMethodDeclaration ::= MethodHeader SEMICOLON
//
#line 2269 "java.g"
void Parser::Act120(void)
{
    ((AstMethodDeclaration *) Sym(1)) -> method_body = ast_pool -> NewEmptyStatement(Token(2));
}
 
// 
// Rule 121:  ArrayInitializer ::= LBRACE ,opt RBRACE
//
#line 2286 "java.g"
void Parser::Act121(void)
{
    AstArrayInitializer *p = ast_pool -> NewArrayInitializer();
    p -> left_brace_token      = Token(1);
    p -> right_brace_token     = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 122:  ArrayInitializer ::= LBRACE VariableInitializers RBRACE
//
#line 2298 "java.g"
void Parser::Act122(void)
{
    AstArrayInitializer *p = ast_pool -> NewArrayInitializer();
    p -> left_brace_token      = Token(1);
    if (Sym(2) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(2);
        p -> AllocateVariableInitializers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableInitializer(root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token     = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 123:  ArrayInitializer ::= LBRACE VariableInitializers COMMA RBRACE
//
#line 2322 "java.g"
void Parser::Act123(void)
{
    AstArrayInitializer *p = ast_pool -> NewArrayInitializer();
    p -> left_brace_token      = Token(1);
    if (Sym(2) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(2);
        p -> AllocateVariableInitializers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableInitializer(root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token     = Token(4);
    Sym(1) = p;
}
 
// 
// Rule 124:  VariableInitializers ::= VariableInitializer
//
#line 2346 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act124(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 125:  VariableInitializers ::= VariableInitializers COMMA VariableInitializer
//
#line 2363 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act125(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}
 
// 
// Rule 126:  Block ::= LBRACE BlockStatementsopt RBRACE
//
#line 2386 "java.g"
void Parser::Act126(void)
{
    AstBlock *p = ast_pool -> NewBlock();
    p -> left_brace_token  = Token(1);
    if (Sym(2) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(2);
        p -> AllocateBlockStatements(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddStatement((AstStatement *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_brace_token = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 127:  BlockStatements ::= BlockStatement
//
#line 2410 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act127(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 128:  BlockStatements ::= BlockStatements BlockStatement
//
#line 2427 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act128(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 129:  BlockStatement -> LocalVariableDeclarationStatement
//
// void NoAction(void);
//

//
// Rule 130:  BlockStatement -> Statement
//
// void NoAction(void);
//

//
// Rule 131:  BlockStatement -> ClassDeclaration
//
// void NoAction(void);
//
 
// 
// Rule 132:  LocalVariableDeclarationStatement ::= LocalVariableDeclaration SEMICOLON
//
#line 2461 "java.g"
void Parser::Act132(void)
{
    ((AstLocalVariableDeclarationStatement *) Sym(1)) -> semicolon_token_opt = Token(2);
}
 
// 
// Rule 133:  LocalVariableDeclaration ::= Type VariableDeclarators
//
#line 2470 "java.g"
void Parser::Act133(void)
{
    AstLocalVariableDeclarationStatement *p = ast_pool -> NewLocalVariableDeclarationStatement();
    p -> type                 = Sym(1);
    //
    // The list of declarators is guaranteed not empty
    //
    {   
        AstListNode *tail = (AstListNode *) Sym(2);
        p -> AllocateVariableDeclarators(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableDeclarator((AstVariableDeclarator *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> semicolon_token_opt  = 0;
    Sym(1) = p;
}
 
// 
// Rule 134:  LocalVariableDeclaration ::= Modifiers Type VariableDeclarators
//
#line 2497 "java.g"
void Parser::Act134(void)
{
    AstLocalVariableDeclarationStatement *p = ast_pool -> NewLocalVariableDeclarationStatement();
    //
    // The list of modifiers is guaranteed not empty
    //
    {   
        AstListNode *tail = (AstListNode *) Sym(1);
        p -> AllocateLocalModifiers(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddLocalModifier((AstModifier *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> type = Sym(2);
    //
    // The list of declarators is guaranteed not empty
    //
    {   
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateVariableDeclarators(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddVariableDeclarator((AstVariableDeclarator *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> semicolon_token_opt  = 0;
    Sym(1) = p;
}

//
// Rule 135:  Statement -> StatementWithoutTrailingSubstatement
//
// void NoAction(void);
//

//
// Rule 136:  Statement -> LabeledStatement
//
// void NoAction(void);
//

//
// Rule 137:  Statement -> IfThenStatement
//
// void NoAction(void);
//

//
// Rule 138:  Statement -> IfThenElseStatement
//
// void NoAction(void);
//

//
// Rule 139:  Statement -> WhileStatement
//
// void NoAction(void);
//

//
// Rule 140:  Statement -> ForStatement
//
// void NoAction(void);
//

//
// Rule 141:  StatementNoShortIf -> StatementWithoutTrailingSubstatement
//
// void NoAction(void);
//

//
// Rule 142:  StatementNoShortIf -> LabeledStatementNoShortIf
//
// void NoAction(void);
//

//
// Rule 143:  StatementNoShortIf -> IfThenElseStatementNoShortIf
//
// void NoAction(void);
//

//
// Rule 144:  StatementNoShortIf -> WhileStatementNoShortIf
//
// void NoAction(void);
//

//
// Rule 145:  StatementNoShortIf -> ForStatementNoShortIf
//
// void NoAction(void);
//

//
// Rule 146:  StatementWithoutTrailingSubstatement -> Block
//
// void NoAction(void);
//

//
// Rule 147:  StatementWithoutTrailingSubstatement -> EmptyStatement
//
// void NoAction(void);
//

//
// Rule 148:  StatementWithoutTrailingSubstatement -> ExpressionStatement
//
// void NoAction(void);
//

//
// Rule 149:  StatementWithoutTrailingSubstatement -> SwitchStatement
//
// void NoAction(void);
//

//
// Rule 150:  StatementWithoutTrailingSubstatement -> DoStatement
//
// void NoAction(void);
//

//
// Rule 151:  StatementWithoutTrailingSubstatement -> BreakStatement
//
// void NoAction(void);
//

//
// Rule 152:  StatementWithoutTrailingSubstatement -> ContinueStatement
//
// void NoAction(void);
//

//
// Rule 153:  StatementWithoutTrailingSubstatement -> ReturnStatement
//
// void NoAction(void);
//

//
// Rule 154:  StatementWithoutTrailingSubstatement -> SynchronizedStatement
//
// void NoAction(void);
//

//
// Rule 155:  StatementWithoutTrailingSubstatement -> ThrowStatement
//
// void NoAction(void);
//

//
// Rule 156:  StatementWithoutTrailingSubstatement -> TryStatement
//
// void NoAction(void);
//

//
// Rule 157:  EmptyStatement ::= SEMICOLON
//
// void MakeEmptyStatement(void);
//
 
// 
// Rule 158:  LabeledStatement ::= Identifier COLON Statement
//
#line 2632 "java.g"
void Parser::MakeLabeledStatement(void)
{
    AstBlock *p = Sym(3) -> BlockCast();

    if (! (p && p -> NumStatements() == 1 &&
           (p -> Statement(0) -> kind == Ast::FOR   ||
            p -> Statement(0) -> kind == Ast::WHILE ||
            p -> Statement(0) -> kind == Ast::DO)))
    {
        //
        // When a statement is labeled, it is enclosed in a block.
        // This is necessary in order to allow the same name to be
        // reused to label a subsequent statement at the same nesting
        // level... See ProcessBlock, ProcessStatement,...
        //
        p = ast_pool -> NewBlock();
        p -> AllocateBlockStatements(1); // allocate 1 element
        p -> left_brace_token  = Token(1);
        p -> AddStatement((AstStatement *) Sym(3));
        p -> right_brace_token = Sym(3) -> RightToken();
    }

    p -> label_token_opt = Token(1); // add label to statement
    Sym(1) = p; // The final result is a block containing the labeled-statement
}

//
// Rule 159:  LabeledStatementNoShortIf ::= Identifier COLON StatementNoShortIf
//
// void MakeLabeledStatement(void);
//
 
// 
// Rule 160:  ExpressionStatement ::= StatementExpression SEMICOLON
//
#line 2669 "java.g"
void Parser::Act160(void)
{
    ((AstExpressionStatement *) Sym(1)) -> semicolon_token_opt = Token(2);
}
 
// 
// Rule 161:  StatementExpression ::= Assignment
//
#line 2678 "java.g"
void Parser::MakeExpressionStatement(void)
{
    AstExpressionStatement *p = ast_pool -> NewExpressionStatement();
    p -> expression          = (AstExpression *) Sym(1);
    p -> semicolon_token_opt = 0;
    Sym(1) = p;
}

//
// Rule 162:  StatementExpression ::= PreIncrementExpression
//
// void MakeExpressionStatement(void);
//

//
// Rule 163:  StatementExpression ::= PreDecrementExpression
//
// void MakeExpressionStatement(void);
//

//
// Rule 164:  StatementExpression ::= PostIncrementExpression
//
// void MakeExpressionStatement(void);
//

//
// Rule 165:  StatementExpression ::= PostDecrementExpression
//
// void MakeExpressionStatement(void);
//

//
// Rule 166:  StatementExpression ::= MethodInvocation
//
// void MakeExpressionStatement(void);
//

//
// Rule 167:  StatementExpression ::= ClassInstanceCreationExpression
//
// void MakeExpressionStatement(void);
//
 
// 
// Rule 168:  IfThenStatement ::= if LPAREN Expression RPAREN Statement
//
#line 2732 "java.g"
void Parser::Act168(void)
{
    AstBlock *block = Sym(5) -> BlockCast();
    if (! block)
    {
        block = ast_pool -> NewBlock();
        block -> AllocateBlockStatements(1); // allocate 1 element
        block -> left_brace_token  = Token(5);
        block -> AddStatement((AstStatement *) Sym(5));
        block -> right_brace_token = Sym(5) -> RightToken();
    }

    AstIfStatement *p = ast_pool -> NewIfStatement();
    p -> if_token            = Token(1);
    p -> expression          = (AstExpression *) Sym(3);
    p -> true_statement      = block;
    p -> false_statement_opt = NULL;
    Sym(1) = p;
}
 
// 
// Rule 169:  IfThenElseStatement ::= if LPAREN Expression RPAREN StatementNoShortIf else Statement
//
#line 2756 "java.g"
void Parser::MakeIfThenElseStatement(void)
{
    AstBlock *true_block = Sym(5) -> BlockCast();
    if (! true_block)
    {
        true_block = ast_pool -> NewBlock();
        true_block -> AllocateBlockStatements(1); // allocate 1 element
        true_block -> left_brace_token  = Token(5);
        true_block -> AddStatement((AstStatement *) Sym(5));
        true_block -> right_brace_token = Sym(5) -> RightToken();
    }

    AstBlock *false_block = Sym(7) -> BlockCast();
    if (! false_block)
    {
        false_block = ast_pool -> NewBlock();
        false_block -> AllocateBlockStatements(1); // allocate 1 element
        false_block -> left_brace_token  = Token(7);
        false_block -> AddStatement((AstStatement *) Sym(7));
        false_block -> right_brace_token = Sym(7) -> RightToken();
    }

    AstIfStatement *p = ast_pool -> NewIfStatement();
    p -> if_token            = Token(1);
    p -> expression          = (AstExpression *) Sym(3);
    p -> true_statement      = true_block;
    p -> false_statement_opt = false_block;
    Sym(1) = p;
}

//
// Rule 170:  IfThenElseStatementNoShortIf ::= if LPAREN Expression RPAREN StatementNoShortIf else StatementNoShortIf
//
// void MakeIfThenElseStatement(void);
//
 
// 
// Rule 171:  SwitchStatement ::= switch LPAREN Expression RPAREN SwitchBlock
//
#line 2797 "java.g"
void Parser::Act171(void)
{
    AstSwitchStatement *p = (AstSwitchStatement *) Sym(5);
    p -> switch_token = Token(1);
    p -> expression   = (AstExpression *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 172:  SwitchBlock ::= LBRACE RBRACE
//
#line 2809 "java.g"
void Parser::Act172(void)
{
    AstSwitchStatement *p = ast_pool -> NewSwitchStatement();

    AstBlock *block = ast_pool -> NewBlock();
    block -> left_brace_token  = Token(1);
    block -> right_brace_token = Token(2);

    p -> switch_block = block;

    Sym(1) = p;
}
 
// 
// Rule 173:  SwitchBlock ::= LBRACE SwitchBlockStatements RBRACE
//
#line 2826 "java.g"
void Parser::Act173(void)
{
    AstSwitchStatement *p = ast_pool -> NewSwitchStatement();

    AstBlock *block = ast_pool -> NewBlock();
    block -> left_brace_token  = Token(1);
    if (Sym(2) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(2);
        block -> AllocateBlockStatements(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            block -> AddStatement((AstStatement *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    block -> right_brace_token = Token(3);

    p -> switch_block  = block;

    Sym(1) = p;
}
 
// 
// Rule 174:  SwitchBlock ::= LBRACE SwitchLabels RBRACE
//
#line 2855 "java.g"
void Parser::Act174(void)
{
    AstSwitchStatement *p = ast_pool -> NewSwitchStatement();

    AstSwitchBlockStatement *q = ast_pool -> NewSwitchBlockStatement();
    //
    // The list of SwitchBlockStatements is never null
    //
    {
        AstListNode *tail = (AstListNode *) Sym(2);
        q -> AllocateSwitchLabels(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            q -> AddSwitchLabel((AstStatement *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }

    AstBlock *block = ast_pool -> NewBlock();
    block -> AllocateBlockStatements(1); // allocate 1 element
    block -> left_brace_token  = Token(1);
    block -> AddStatement(q);
    block -> right_brace_token = Token(3);

    p -> switch_block  = block;

    Sym(1) = p;
}
 
// 
// Rule 175:  SwitchBlock ::= LBRACE SwitchBlockStatements SwitchLabels RBRACE
//
#line 2890 "java.g"
void Parser::Act175(void)
{
    AstSwitchStatement *p = ast_pool -> NewSwitchStatement();

    AstBlock *block = ast_pool -> NewBlock();
    block -> left_brace_token  = Token(1);
    //
    // The list of SwitchBlockStatements is never null
    //
    {
        AstListNode *tail = (AstListNode *) Sym(2);
        block -> AllocateBlockStatements(tail -> index + 2); // +1 because of extra statement for additional SwithLabels
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            block -> AddStatement((AstStatement *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }

    AstSwitchBlockStatement *q = ast_pool -> NewSwitchBlockStatement();
    //
    // The list of SwitchLabels is never null
    //
    {
        AstListNode *tail = (AstListNode *) Sym(3);
        q -> AllocateSwitchLabels(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            q -> AddSwitchLabel(root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }

    block -> AddStatement(q);
    block -> right_brace_token = Token(4);

    p -> switch_block  = block;

    Sym(1) = p;
}
 
// 
// Rule 176:  SwitchBlockStatements ::= SwitchBlockStatement
//
#line 2939 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act176(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 177:  SwitchBlockStatements ::= SwitchBlockStatements SwitchBlockStatement
//
#line 2956 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act177(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}
 
// 
// Rule 178:  SwitchBlockStatement ::= SwitchLabels BlockStatements
//
#line 2977 "java.g"
void Parser::Act178(void)
{
    AstSwitchBlockStatement *p = ast_pool -> NewSwitchBlockStatement();
    //
    // The list of SwitchLabels is never null
    //
    {
        AstListNode *tail = (AstListNode *) Sym(1);
        p -> AllocateSwitchLabels(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddSwitchLabel(root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }

    //
    // The list of SwitchBlockStatements is never null
    //
    {
        AstListNode *tail = (AstListNode *) Sym(2);
        p -> AllocateBlockStatements(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddStatement((AstStatement *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    Sym(1) = p;
}
 
// 
// Rule 179:  SwitchLabels ::= SwitchLabel
//
#line 3016 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act179(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 180:  SwitchLabels ::= SwitchLabels SwitchLabel
//
#line 3033 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act180(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}
 
// 
// Rule 181:  SwitchLabel ::= case ConstantExpression COLON
//
#line 3054 "java.g"
void Parser::Act181(void)
{
    AstCaseLabel *p = ast_pool -> NewCaseLabel();
    p -> case_token  = Token(1);
    p -> expression  = (AstExpression *) Sym(2);
    p -> colon_token = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 182:  SwitchLabel ::= default COLON
//
#line 3067 "java.g"
void Parser::Act182(void)
{
    AstDefaultLabel *p = ast_pool -> NewDefaultLabel();
    p -> default_token = Token(1);
    p -> colon_token   = Token(2);
    Sym(1) = p;
}
 
// 
// Rule 183:  WhileStatement ::= while LPAREN Expression RPAREN Statement
//
#line 3079 "java.g"
void Parser::MakeWhileStatement(void)
{
    AstWhileStatement *p = ast_pool -> NewWhileStatement();
    p -> while_token = Token(1);
    p -> expression  = (AstExpression *) Sym(3);
    p -> statement   = (AstStatement *) Sym(5);

    AstBlock *block = ast_pool -> NewBlock();
    block -> AllocateBlockStatements(1); // allocate 1 element
    block -> left_brace_token  = Token(1); // point to 'FOR' keyword
    block -> AddStatement(p);
    block -> right_brace_token = Sym(5) -> RightToken(); // point to last token in statement

    Sym(1) = block;
}

//
// Rule 184:  WhileStatementNoShortIf ::= while LPAREN Expression RPAREN StatementNoShortIf
//
// void MakeWhileStatement(void);
//
 
// 
// Rule 185:  DoStatement ::= do Statement while LPAREN Expression RPAREN SEMICOLON
//
#line 3106 "java.g"
void Parser::Act185(void)
{
    AstDoStatement *p = ast_pool -> NewDoStatement();
    p -> do_token        = Token(1);
    p -> statement       = (AstStatement *) Sym(2);
    p -> while_token     = Token(3);
    p -> expression      = (AstExpression *) Sym(5);
    p -> semicolon_token = Token(7);

    AstBlock *block = ast_pool -> NewBlock();
    block -> AllocateBlockStatements(1); // allocate 1 element
    block -> left_brace_token  = Token(1);
    block -> AddStatement(p);
    block -> right_brace_token = Token(7);

    Sym(1) = block;
}
 
// 
// Rule 186:  ForStatement ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN Statement
//
#line 3128 "java.g"
void Parser::MakeForStatement(void)
{
    AstForStatement *p = ast_pool -> NewForStatement();
    p -> for_token = Token(1);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateForInitStatements(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddForInitStatement((AstStatement *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> end_expression_opt = (AstExpression *) Sym(5);
    if (Sym(7) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(7);
        p -> AllocateForUpdateStatements(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddForUpdateStatement((AstExpressionStatement *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> statement = (AstStatement *) Sym(9);

    AstBlock *block = ast_pool -> NewBlock();
    block -> AllocateBlockStatements(1); // allocate 1 element
    block -> left_brace_token  = Token(1);
    block -> AddStatement(p);
    block -> right_brace_token = Sym(9) -> RightToken();

    Sym(1) = block;
}

//
// Rule 187:  ForStatementNoShortIf ::= for LPAREN ForInitopt SEMICOLON Expressionopt SEMICOLON ForUpdateopt RPAREN...
//
// void MakeForStatement(void);
//

//
// Rule 188:  ForInit -> StatementExpressionList
//
// void NoAction(void);
//
 
// 
// Rule 189:  ForInit ::= LocalVariableDeclaration
//
#line 3183 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act189(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}

//
// Rule 190:  ForUpdate -> StatementExpressionList
//
// void NoAction(void);
//
 
// 
// Rule 191:  StatementExpressionList ::= StatementExpression
//
#line 3204 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act191(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 192:  StatementExpressionList ::= StatementExpressionList COMMA StatementExpression
//
#line 3221 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act192(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}
 
// 
// Rule 193:  BreakStatement ::= break SEMICOLON
//
#line 3248 "java.g"
void Parser::Act193(void)
{
    AstBreakStatement *p = ast_pool -> NewBreakStatement();
    p -> break_token          = Token(1);
    p -> identifier_token_opt = 0;
    p -> semicolon_token      = Token(2);
    Sym(1) = p;
}
 
// 
// Rule 194:  BreakStatement ::= break Identifier SEMICOLON
//
#line 3261 "java.g"
void Parser::Act194(void)
{
    AstBreakStatement *p = ast_pool -> NewBreakStatement();
    p -> break_token          = Token(1);
    p -> identifier_token_opt = Token(2);
    p -> semicolon_token      = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 195:  ContinueStatement ::= continue SEMICOLON
//
#line 3274 "java.g"
void Parser::Act195(void)
{
    AstContinueStatement *p = ast_pool -> NewContinueStatement();
    p -> continue_token       = Token(1);
    p -> identifier_token_opt = 0;
    p -> semicolon_token      = Token(2);
    Sym(1) = p;
}
 
// 
// Rule 196:  ContinueStatement ::= continue Identifier SEMICOLON
//
#line 3287 "java.g"
void Parser::Act196(void)
{
    AstContinueStatement *p = ast_pool -> NewContinueStatement();
    p -> continue_token       = Token(1);
    p -> identifier_token_opt = Token(2);
    p -> semicolon_token      = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 197:  ReturnStatement ::= return Expressionopt SEMICOLON
//
#line 3300 "java.g"
void Parser::Act197(void)
{
    AstReturnStatement *p = ast_pool -> NewReturnStatement();
    p -> return_token    = Token(1);
    p -> expression_opt  = (AstExpression *) Sym(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 198:  ThrowStatement ::= throw Expression SEMICOLON
//
#line 3313 "java.g"
void Parser::Act198(void)
{
    AstThrowStatement *p = ast_pool -> NewThrowStatement();
    p -> throw_token     = Token(1);
    p -> expression      = (AstExpression *) Sym(2);
    p -> semicolon_token = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 199:  SynchronizedStatement ::= synchronized LPAREN Expression RPAREN Block
//
#line 3326 "java.g"
void Parser::Act199(void)
{
    AstSynchronizedStatement *p = ast_pool -> NewSynchronizedStatement();
    p -> synchronized_token = Token(1);
    p -> expression         = (AstExpression *) Sym(3);
    p -> block              = (AstBlock *) Sym(5);
    Sym(1) = p;
}
 
// 
// Rule 200:  TryStatement ::= try Block Catches
//
#line 3339 "java.g"
void Parser::Act200(void)
{
    AstTryStatement *p = ast_pool -> NewTryStatement();
    p -> try_token          = Token(1);
    p -> block              = (AstBlock *) Sym(2);
    //
    // The list of modifiers is guaranteed not empty
    //
    {
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateCatchClauses(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddCatchClause((AstCatchClause *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> finally_clause_opt = NULL;
    Sym(1) = p;
}
 
// 
// Rule 201:  TryStatement ::= try Block Catchesopt Finally
//
#line 3366 "java.g"
void Parser::Act201(void)
{
    AstTryStatement *p = ast_pool -> NewTryStatement();
    p -> try_token      = Token(1);
    p -> block          = (AstBlock *) Sym(2);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateCatchClauses(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddCatchClause((AstCatchClause *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> finally_clause_opt = (AstFinallyClause *) Sym(4);
    Sym(1) = p;
}
 
// 
// Rule 202:  Catches ::= CatchClause
//
#line 3391 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act202(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 203:  Catches ::= Catches CatchClause
//
#line 3408 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act203(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}
 
// 
// Rule 204:  CatchClause ::= catch LPAREN FormalParameter RPAREN Block
//
#line 3429 "java.g"
void Parser::Act204(void)
{
    AstCatchClause *p = ast_pool -> NewCatchClause();
    p -> catch_token      = Token(1);
    p -> formal_parameter = (AstFormalParameter *) Sym(3);
    p -> block            = (AstBlock *) Sym(5);
    Sym(1) = p;
}
 
// 
// Rule 205:  Finally ::= finally Block
//
#line 3442 "java.g"
void Parser::Act205(void)
{
    AstFinallyClause *p = ast_pool -> NewFinallyClause();
    p -> finally_token = Token(1);
    p -> block         = (AstBlock *) Sym(2);
    Sym(1) = p;
}

//
// Rule 206:  Primary -> PrimaryNoNewArray
//
// void NoAction(void);
//

//
// Rule 207:  Primary -> ArrayCreationExpression
//
// void NoAction(void);
//

//
// Rule 208:  PrimaryNoNewArray -> Literal
//
// void NoAction(void);
//
 
// 
// Rule 209:  PrimaryNoNewArray ::= this
//
#line 3468 "java.g"
void Parser::Act209(void)
{
    Sym(1) = ast_pool -> NewThisExpression(Token(1));
}
 
// 
// Rule 210:  PrimaryNoNewArray ::= LPAREN Expression RPAREN
//
#line 3477 "java.g"
void Parser::Act210(void)
{
    AstParenthesizedExpression *p = ast_pool -> NewParenthesizedExpression();
    p -> left_parenthesis_token = Token(1);
    p -> expression = (AstExpression *) Sym(2);
    p -> right_parenthesis_token = Token(3);
    Sym(1) = p;
}

//
// Rule 211:  PrimaryNoNewArray -> ClassInstanceCreationExpression
//
// void NoAction(void);
//

//
// Rule 212:  PrimaryNoNewArray -> FieldAccess
//
// void NoAction(void);
//
 
// 
// Rule 213:  PrimaryNoNewArray ::= Name DOT this
//
#line 3499 "java.g"
void Parser::Act213(void)
{
    AstFieldAccess *p = ast_pool -> NewFieldAccess(AstFieldAccess::THIS_TAG);
    p -> base = (AstExpression *) Sym(1);
    p -> dot_token = Token(2);
    p -> identifier_token = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 214:  PrimaryNoNewArray ::= Type DOT class
//
#line 3513 "java.g"
void Parser::Act214(void)
{
    AstFieldAccess *p = ast_pool -> NewFieldAccess(AstFieldAccess::CLASS_TAG);
    p -> base = ast_pool -> NewTypeExpression(Sym(1));
    p -> dot_token = Token(2);
    p -> identifier_token = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 215:  PrimaryNoNewArray ::= void DOT class
//
#line 3527 "java.g"
void Parser::Act215(void)
{
    AstFieldAccess *p = ast_pool -> NewFieldAccess(AstFieldAccess::CLASS_TAG);
    p -> base = ast_pool -> NewTypeExpression(ast_pool -> NewPrimitiveType(Ast::VOID_TYPE, Token(1)));
    p -> dot_token = Token(2);
    p -> identifier_token = Token(3);
    Sym(1) = p;
}

//
// Rule 216:  PrimaryNoNewArray -> MethodInvocation
//
// void NoAction(void);
//

//
// Rule 217:  PrimaryNoNewArray -> ArrayAccess
//
// void NoAction(void);
//
 
// 
// Rule 218:  ClassInstanceCreationExpression ::= new ClassType LPAREN ArgumentListopt RPAREN ClassBodyopt
//
#line 3553 "java.g"
void Parser::Act218(void)
{
    AstClassInstanceCreationExpression *p = ast_pool -> NewClassInstanceCreationExpression();
    p -> base_opt                = NULL;
    p -> dot_token_opt           = 0;
    p -> new_token               = Token(1);
    p -> class_type              = ast_pool -> NewTypeExpression(Sym(2));
    p -> left_parenthesis_token  = Token(3);
    if (Sym(4) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(4);
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(5);
    p -> class_body_opt          = (AstClassBody *) Sym(6);
    Sym(1) = p;
}
 
// 
// Rule 219:  ClassInstanceCreationExpression ::= Primary DOT new SimpleName LPAREN ArgumentListopt RPAREN ClassBodyopt
//
#line 3583 "java.g"
void Parser::MakeQualifiedNew(void)
{
    AstClassInstanceCreationExpression *p = ast_pool -> NewClassInstanceCreationExpression();
    p -> base_opt                = (AstExpression *) Sym(1);
    p -> dot_token_opt           = Token(2);
    p -> new_token               = Token(3);
    p -> class_type              = ast_pool -> NewTypeExpression(Sym(4));
    p -> left_parenthesis_token  = Token(5);
    if (Sym(6) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(6);
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(7);
    p -> class_body_opt          = (AstClassBody *) Sym(8);
    Sym(1) = p;
}

//
// Rule 220:  ClassInstanceCreationExpression ::= Name DOT new SimpleName LPAREN ArgumentListopt RPAREN ClassBodyopt
//
// void MakeQualifiedNew(void);
//
 
// 
// Rule 221:  ArgumentList ::= Expression
//
#line 3620 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act221(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 222:  ArgumentList ::= ArgumentList COMMA Expression
//
#line 3637 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act222(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(3);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}
 
// 
// Rule 223:  ArrayCreationExpression ::= new PrimitiveType DimExprs Dimsopt
//
#line 3658 "java.g"
void Parser::MakeArrayCreationExpression(void)
{
    AstArrayCreationExpression *p = ast_pool -> NewArrayCreationExpression();
    p -> new_token             = Token(1);
    p -> array_type            = Sym(2);
    //
    // The list of DimExprs is never null
    //
    {
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateDimExprs(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddDimExpr((AstDimExpr *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }

    if (Sym(4) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(4);
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets((AstBrackets *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> array_initializer_opt = NULL;
    Sym(1) = p;
}

//
// Rule 224:  ArrayCreationExpression ::= new ClassOrInterfaceType DimExprs Dimsopt
//
// void MakeArrayCreationExpression(void);
//
 
// 
// Rule 225:  ArrayCreationExpression ::= new ArrayType ArrayInitializer
//
#line 3706 "java.g"
void Parser::Act225(void)
{
    AstArrayCreationExpression *p = ast_pool -> NewArrayCreationExpression();
    p -> new_token             = Token(1);
    p -> array_type            = Sym(2);
    p -> array_initializer_opt = (AstArrayInitializer *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 226:  DimExprs ::= DimExpr
//
#line 3719 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act226(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = Sym(1);
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 227:  DimExprs ::= DimExprs DimExpr
//
#line 3736 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act227(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = Sym(2);
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}
 
// 
// Rule 228:  DimExpr ::= LBRACKET Expression RBRACKET
//
#line 3757 "java.g"
void Parser::Act228(void)
{
    AstDimExpr *p = ast_pool -> NewDimExpr();
    p -> left_bracket_token  = Token(1);
    p -> expression          = (AstExpression *) Sym(2);
    p -> right_bracket_token = Token(3);
    Sym(1) = p;
}
 
// 
// Rule 229:  Dims ::= LBRACKET RBRACKET
//
#line 3770 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act229(void)
{
    AstListNode *p = AllocateListNode();
    p -> next = p;
    p -> element = ast_pool -> NewBrackets(Token(1), Token(2));
    p -> index = 0;

    Sym(1) = p;
}
 
// 
// Rule 230:  Dims ::= Dims LBRACKET RBRACKET
//
#line 3787 "java.g"
//
// Note that the list is circular so as to preserve the order of the elements
//
void Parser::Act230(void)
{
    AstListNode *tail = (AstListNode *) Sym(1);

    AstListNode *p = AllocateListNode();
    p -> element = ast_pool -> NewBrackets(Token(2), Token(3));
    p -> index = tail -> index + 1;

    p -> next = tail -> next;
    tail -> next = p;

    Sym(1) = p;
}

//
// Rule 231:  FieldAccess ::= Primary DOT Identifier
//
// void MakeFieldAccess(void);
//
 
// 
// Rule 232:  FieldAccess ::= super DOT Identifier
//
#line 3815 "java.g"
void Parser::MakeSuperFieldAccess(void)
{
    Sym(1) = ast_pool -> NewSuperExpression(Token(1));

    MakeFieldAccess();
}
 
// 
// Rule 233:  MethodInvocation ::= Name LPAREN ArgumentListopt RPAREN
//
#line 3826 "java.g"
void Parser::Act233(void)
{
    AstMethodInvocation *p = ast_pool -> NewMethodInvocation();
    p -> method                  = (AstExpression *) Sym(1);
    p -> left_parenthesis_token  = Token(2);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(4);
    Sym(1) = p;
}
 
// 
// Rule 234:  MethodInvocation ::= Primary DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3851 "java.g"
void Parser::Act234(void)
{
    MakeFieldAccess();

    AstMethodInvocation *p = ast_pool -> NewMethodInvocation();
    p -> method                  = (AstExpression *) Sym(1);
    p -> left_parenthesis_token  = Token(4);
    if (Sym(5) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(5);
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(6);
    Sym(1) = p;
}
 
// 
// Rule 235:  MethodInvocation ::= super DOT Identifier LPAREN ArgumentListopt RPAREN
//
#line 3878 "java.g"
void Parser::Act235(void)
{
    MakeSuperFieldAccess();

    AstMethodInvocation *p = ast_pool -> NewMethodInvocation();
    p -> method                  = (AstExpression *) Sym(1);
    p -> left_parenthesis_token  = Token(4);
    if (Sym(5) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(5);
        p -> AllocateArguments(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddArgument((AstExpression *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token = Token(6);
    Sym(1) = p;
}
 
// 
// Rule 236:  ArrayAccess ::= Name LBRACKET Expression RBRACKET
//
#line 3905 "java.g"
void Parser::MakeArrayAccess(void)
{
    AstArrayAccess *p = ast_pool -> NewArrayAccess();
    p -> base                = (AstExpression *) Sym(1);
    p -> left_bracket_token  = Token(2);
    p -> expression          = (AstExpression *) Sym(3);
    p -> right_bracket_token = Token(4);
    Sym(1) = p;
}

//
// Rule 237:  ArrayAccess ::= PrimaryNoNewArray LBRACKET Expression RBRACKET
//
// void MakeArrayAccess(void);
//

//
// Rule 238:  PostfixExpression -> Primary
//
// void NoAction(void);
//

//
// Rule 239:  PostfixExpression -> Name
//
// void NoAction(void);
//

//
// Rule 240:  PostfixExpression -> PostIncrementExpression
//
// void NoAction(void);
//

//
// Rule 241:  PostfixExpression -> PostDecrementExpression
//
// void NoAction(void);
//
 
// 
// Rule 242:  PostIncrementExpression ::= PostfixExpression PLUS_PLUS
//
#line 3942 "java.g"
void Parser::Act242(void)
{
    AstPostUnaryExpression *p = ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::PLUSPLUS);
    p -> expression          = (AstExpression *) Sym(1);
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}
 
// 
// Rule 243:  PostDecrementExpression ::= PostfixExpression MINUS_MINUS
//
#line 3954 "java.g"
void Parser::Act243(void)
{
    AstPostUnaryExpression *p = ast_pool -> NewPostUnaryExpression(AstPostUnaryExpression::MINUSMINUS);
    p -> expression          = (AstExpression *) Sym(1);
    p -> post_operator_token = Token(2);
    Sym(1) = p;
}

//
// Rule 244:  UnaryExpression -> PreIncrementExpression
//
// void NoAction(void);
//

//
// Rule 245:  UnaryExpression -> PreDecrementExpression
//
// void NoAction(void);
//
 
// 
// Rule 246:  UnaryExpression ::= PLUS UnaryExpression
//
#line 3974 "java.g"
void Parser::Act246(void)
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::PLUS);
    p -> pre_operator_token = Token(1);
    p -> expression         = (AstExpression *) Sym(2);
    Sym(1) = p;
}
 
// 
// Rule 247:  UnaryExpression ::= MINUS UnaryExpression
//
#line 3986 "java.g"
void Parser::Act247(void)
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::MINUS);
    p -> pre_operator_token = Token(1);
    p -> expression         = (AstExpression *) Sym(2);
    Sym(1) = p;
}

//
// Rule 248:  UnaryExpression -> UnaryExpressionNotPlusMinus
//
// void NoAction(void);
//
 
// 
// Rule 249:  PreIncrementExpression ::= PLUS_PLUS UnaryExpression
//
#line 4002 "java.g"
void Parser::Act249(void)
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::PLUSPLUS);
    p -> pre_operator_token = Token(1);
    p -> expression         = (AstExpression *) Sym(2);
    Sym(1) = p;
}
 
// 
// Rule 250:  PreDecrementExpression ::= MINUS_MINUS UnaryExpression
//
#line 4014 "java.g"
void Parser::Act250(void)
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::MINUSMINUS);
    p -> pre_operator_token = Token(1);
    p -> expression         = (AstExpression *) Sym(2);
    Sym(1) = p;
}

//
// Rule 251:  UnaryExpressionNotPlusMinus -> PostfixExpression
//
// void NoAction(void);
//
 
// 
// Rule 252:  UnaryExpressionNotPlusMinus ::= TWIDDLE UnaryExpression
//
#line 4030 "java.g"
void Parser::Act252(void)
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::TWIDDLE);
    p -> pre_operator_token = Token(1);
    p -> expression         = (AstExpression *) Sym(2);
    Sym(1) = p;
}
 
// 
// Rule 253:  UnaryExpressionNotPlusMinus ::= NOT UnaryExpression
//
#line 4042 "java.g"
void Parser::Act253(void)
{
    AstPreUnaryExpression *p = ast_pool -> NewPreUnaryExpression(AstPreUnaryExpression::NOT);
    p -> pre_operator_token = Token(1);
    p -> expression         = (AstExpression *) Sym(2);
    Sym(1) = p;
}

//
// Rule 254:  UnaryExpressionNotPlusMinus -> CastExpression
//
// void NoAction(void);
//
 
// 
// Rule 255:  CastExpression ::= LPAREN PrimitiveType Dimsopt RPAREN UnaryExpression
//
#line 4058 "java.g"
void Parser::MakeCastExpression(void)
{
    AstCastExpression *p = ast_pool -> NewCastExpression();
    p -> left_parenthesis_token_opt  = Token(1);
    p -> type_opt                    = Sym(2);
    if (Sym(3) != NULL)
    {
        AstListNode *tail = (AstListNode *) Sym(3);
        p -> AllocateBrackets(tail -> index + 1);
        AstListNode *root = tail;
        do
        {
            root = root -> next;
            p -> AddBrackets((AstBrackets *) root -> element);
        } while(root != tail);
        FreeCircularList(tail);
    }
    p -> right_parenthesis_token_opt = Token(4);
    p -> expression                  = (AstExpression *) Sym(5);
    Sym(1) = p;
}
 
// 
// Rule 256:  CastExpression ::= LPAREN Expression RPAREN UnaryExpressionNotPlusMinus
//
#line 4084 "java.g"
void Parser::Act256(void)
{ 
    //
    // Note that Expression must be a name - i.e., Sym(2) -> isName() == true
    // This check is not performed here and should be performed during
    // semantic processing.
    //
    AstCastExpression *p = ast_pool -> NewCastExpression();
    p -> left_parenthesis_token_opt  = Token(1);
    p -> type_opt                    = Sym(2);
    p -> right_parenthesis_token_opt = Token(3);
    p -> expression                  = (AstExpression *) Sym(4);
    Sym(1) = p;
}

//
// Rule 257:  CastExpression ::= LPAREN Name Dims RPAREN UnaryExpressionNotPlusMinus
//
// void MakeCastExpression(void);
//

//
// Rule 258:  MultiplicativeExpression -> UnaryExpression
//
// void NoAction(void);
//
 
// 
// Rule 259:  MultiplicativeExpression ::= MultiplicativeExpression MULTIPLY UnaryExpression
//
#line 4114 "java.g"
void Parser::Act259(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::STAR);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 260:  MultiplicativeExpression ::= MultiplicativeExpression DIVIDE UnaryExpression
//
#line 4127 "java.g"
void Parser::Act260(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::SLASH);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 261:  MultiplicativeExpression ::= MultiplicativeExpression REMAINDER UnaryExpression
//
#line 4140 "java.g"
void Parser::Act261(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::MOD);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}

//
// Rule 262:  AdditiveExpression -> MultiplicativeExpression
//
// void NoAction(void);
//
 
// 
// Rule 263:  AdditiveExpression ::= AdditiveExpression PLUS MultiplicativeExpression
//
#line 4157 "java.g"
void Parser::Act263(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::PLUS);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 264:  AdditiveExpression ::= AdditiveExpression MINUS MultiplicativeExpression
//
#line 4170 "java.g"
void Parser::Act264(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::MINUS);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}

//
// Rule 265:  ShiftExpression -> AdditiveExpression
//
// void NoAction(void);
//
 
// 
// Rule 266:  ShiftExpression ::= ShiftExpression LEFT_SHIFT AdditiveExpression
//
#line 4187 "java.g"
void Parser::Act266(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::LEFT_SHIFT);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 267:  ShiftExpression ::= ShiftExpression RIGHT_SHIFT AdditiveExpression
//
#line 4200 "java.g"
void Parser::Act267(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::RIGHT_SHIFT);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 268:  ShiftExpression ::= ShiftExpression UNSIGNED_RIGHT_SHIFT AdditiveExpression
//
#line 4213 "java.g"
void Parser::Act268(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::UNSIGNED_RIGHT_SHIFT);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}

//
// Rule 269:  RelationalExpression -> ShiftExpression
//
// void NoAction(void);
//
 
// 
// Rule 270:  RelationalExpression ::= RelationalExpression LESS ShiftExpression
//
#line 4230 "java.g"
void Parser::Act270(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::LESS);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 271:  RelationalExpression ::= RelationalExpression GREATER ShiftExpression
//
#line 4243 "java.g"
void Parser::Act271(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::GREATER);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 272:  RelationalExpression ::= RelationalExpression LESS_EQUAL ShiftExpression
//
#line 4256 "java.g"
void Parser::Act272(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::LESS_EQUAL);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 273:  RelationalExpression ::= RelationalExpression GREATER_EQUAL ShiftExpression
//
#line 4269 "java.g"
void Parser::Act273(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::GREATER_EQUAL);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 274:  RelationalExpression ::= RelationalExpression instanceof ReferenceType
//
#line 4282 "java.g"
void Parser::Act274(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::INSTANCEOF);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = ast_pool -> NewTypeExpression(Sym(3));
    Sym(1) = p;
}

//
// Rule 275:  EqualityExpression -> RelationalExpression
//
// void NoAction(void);
//
 
// 
// Rule 276:  EqualityExpression ::= EqualityExpression EQUAL_EQUAL RelationalExpression
//
#line 4299 "java.g"
void Parser::Act276(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::EQUAL_EQUAL);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}
 
// 
// Rule 277:  EqualityExpression ::= EqualityExpression NOT_EQUAL RelationalExpression
//
#line 4312 "java.g"
void Parser::Act277(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::NOT_EQUAL);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}

//
// Rule 278:  AndExpression -> EqualityExpression
//
// void NoAction(void);
//
 
// 
// Rule 279:  AndExpression ::= AndExpression AND EqualityExpression
//
#line 4330 "java.g"
void Parser::Act279(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::AND);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}

//
// Rule 280:  ExclusiveOrExpression -> AndExpression
//
// void NoAction(void);
//
 
// 
// Rule 281:  ExclusiveOrExpression ::= ExclusiveOrExpression XOR AndExpression
//
#line 4347 "java.g"
void Parser::Act281(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::XOR);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}

//
// Rule 282:  InclusiveOrExpression -> ExclusiveOrExpression
//
// void NoAction(void);
//
 
// 
// Rule 283:  InclusiveOrExpression ::= InclusiveOrExpression OR ExclusiveOrExpression
//
#line 4364 "java.g"
void Parser::Act283(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::IOR);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}

//
// Rule 284:  ConditionalAndExpression -> InclusiveOrExpression
//
// void NoAction(void);
//
 
// 
// Rule 285:  ConditionalAndExpression ::= ConditionalAndExpression AND_AND InclusiveOrExpression
//
#line 4381 "java.g"
void Parser::Act285(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::AND_AND);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}

//
// Rule 286:  ConditionalOrExpression -> ConditionalAndExpression
//
// void NoAction(void);
//
 
// 
// Rule 287:  ConditionalOrExpression ::= ConditionalOrExpression OR_OR ConditionalAndExpression
//
#line 4398 "java.g"
void Parser::Act287(void)
{
    AstBinaryExpression *p = ast_pool -> NewBinaryExpression(AstBinaryExpression::OR_OR);
    p -> left_expression       = (AstExpression *) Sym(1);
    p -> binary_operator_token = Token(2);
    p -> right_expression      = (AstExpression *) Sym(3);
    Sym(1) = p;
}

//
// Rule 288:  ConditionalExpression -> ConditionalOrExpression
//
// void NoAction(void);
//
 
// 
// Rule 289:  ConditionalExpression ::= ConditionalOrExpression QUESTION Expression COLON ConditionalExpression
//
#line 4415 "java.g"
void Parser::Act289(void)
{
    AstConditionalExpression *p = ast_pool -> NewConditionalExpression();
    p -> test_expression  = (AstExpression *) Sym(1);
    p -> question_token   = Token(2);
    p -> true_expression  = (AstExpression *) Sym(3);
    p -> colon_token      = Token(4);
    p -> false_expression = (AstExpression *) Sym(5);
    Sym(1) = p;
}

//
// Rule 290:  AssignmentExpression -> ConditionalExpression
//
// void NoAction(void);
//

//
// Rule 291:  AssignmentExpression -> Assignment
//
// void NoAction(void);
//
 
// 
// Rule 292:  Assignment ::= LeftHandSide AssignmentOperator AssignmentExpression
//
#line 4438 "java.g"
void Parser::Act292(void)
{
    AstAssignmentExpression *p = (AstAssignmentExpression *) Sym(2);
    p -> left_hand_side = (AstExpression *) Sym(1);
    p -> expression     = (AstExpression *) Sym(3);
    Sym(1) = p;
}

//
// Rule 293:  LeftHandSide -> Name
//
// void NoAction(void);
//

//
// Rule 294:  LeftHandSide -> FieldAccess
//
// void NoAction(void);
//

//
// Rule 295:  LeftHandSide -> ArrayAccess
//
// void NoAction(void);
//
 
// 
// Rule 296:  AssignmentOperator ::= EQUAL
//
#line 4462 "java.g"
void Parser::Act296(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::EQUAL, Token(1));
}
 
// 
// Rule 297:  AssignmentOperator ::= MULTIPLY_EQUAL
//
#line 4471 "java.g"
void Parser::Act297(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::STAR_EQUAL, Token(1));
}
 
// 
// Rule 298:  AssignmentOperator ::= DIVIDE_EQUAL
//
#line 4480 "java.g"
void Parser::Act298(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::SLASH_EQUAL, Token(1));
}
 
// 
// Rule 299:  AssignmentOperator ::= REMAINDER_EQUAL
//
#line 4489 "java.g"
void Parser::Act299(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::MOD_EQUAL, Token(1));
}
 
// 
// Rule 300:  AssignmentOperator ::= PLUS_EQUAL
//
#line 4498 "java.g"
void Parser::Act300(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::PLUS_EQUAL, Token(1));
}
 
// 
// Rule 301:  AssignmentOperator ::= MINUS_EQUAL
//
#line 4507 "java.g"
void Parser::Act301(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::MINUS_EQUAL, Token(1));
}
 
// 
// Rule 302:  AssignmentOperator ::= LEFT_SHIFT_EQUAL
//
#line 4516 "java.g"
void Parser::Act302(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::LEFT_SHIFT_EQUAL, Token(1));
}
 
// 
// Rule 303:  AssignmentOperator ::= RIGHT_SHIFT_EQUAL
//
#line 4525 "java.g"
void Parser::Act303(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::RIGHT_SHIFT_EQUAL, Token(1));
}
 
// 
// Rule 304:  AssignmentOperator ::= UNSIGNED_RIGHT_SHIFT_EQUAL
//
#line 4534 "java.g"
void Parser::Act304(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::UNSIGNED_RIGHT_SHIFT_EQUAL, Token(1));
}
 
// 
// Rule 305:  AssignmentOperator ::= AND_EQUAL
//
#line 4543 "java.g"
void Parser::Act305(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::AND_EQUAL, Token(1));
}
 
// 
// Rule 306:  AssignmentOperator ::= XOR_EQUAL
//
#line 4552 "java.g"
void Parser::Act306(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::XOR_EQUAL, Token(1));
}
 
// 
// Rule 307:  AssignmentOperator ::= OR_EQUAL
//
#line 4561 "java.g"
void Parser::Act307(void)
{
    Sym(1) = ast_pool -> NewAssignmentExpression(AstAssignmentExpression::IOR_EQUAL, Token(1));
}

//
// Rule 308:  Expression -> AssignmentExpression
//
// void NoAction(void);
//

//
// Rule 309:  ConstantExpression -> Expression
//
// void NoAction(void);
//
 
// 
// Rule 310:  PackageDeclarationopt ::=
//
#line 4584 "java.g"
//
// Given a rule of the form A ::= x1 x2 ... xn
//
// Construct a NULL Ast for A.
//
void Parser::NullAction(void) { Sym(1) = NULL; }

//
// Rule 311:  PackageDeclarationopt -> PackageDeclaration
//
// void NoAction(void);
//

//
// Rule 312:  Superopt ::=
//
// void NullAction(void);
//

//
// Rule 313:  Superopt -> Super
//
// void NoAction(void);
//

//
// Rule 314:  Expressionopt ::=
//
// void NullAction(void);
//

//
// Rule 315:  Expressionopt -> Expression
//
// void NoAction(void);
//

//
// Rule 316:  ClassBodyopt ::=
//
// void NullAction(void);
//

//
// Rule 317:  ClassBodyopt -> ClassBody
//
// void NoAction(void);
//

//
// Rule 318:  ,opt ::=
//
// void NullAction(void);
//

//
// Rule 319:  ,opt -> COMMA
//
// void NoAction(void);
//

//
// Rule 320:  ImportDeclarationsopt ::=
//
// void NullAction(void);
//

//
// Rule 321:  ImportDeclarationsopt -> ImportDeclarations
//
// void NoAction(void);
//

//
// Rule 322:  TypeDeclarationsopt ::=
//
// void NullAction(void);
//

//
// Rule 323:  TypeDeclarationsopt -> TypeDeclarations
//
// void NoAction(void);
//

//
// Rule 324:  ClassBodyDeclarationsopt ::=
//
// void NullAction(void);
//

//
// Rule 325:  ClassBodyDeclarationsopt -> ClassBodyDeclarations
//
// void NoAction(void);
//

//
// Rule 326:  Modifiersopt ::=
//
// void NullAction(void);
//

//
// Rule 327:  Modifiersopt -> Modifiers
//
// void NoAction(void);
//

//
// Rule 328:  BlockStatementsopt ::=
//
// void NullAction(void);
//

//
// Rule 329:  BlockStatementsopt -> BlockStatements
//
// void NoAction(void);
//

//
// Rule 330:  Dimsopt ::=
//
// void NullAction(void);
//

//
// Rule 331:  Dimsopt -> Dims
//
// void NoAction(void);
//

//
// Rule 332:  ArgumentListopt ::=
//
// void NullAction(void);
//

//
// Rule 333:  ArgumentListopt -> ArgumentList
//
// void NoAction(void);
//

//
// Rule 334:  Throwsopt ::=
//
// void NullAction(void);
//

//
// Rule 335:  Throwsopt -> Throws
//
// void NoAction(void);
//

//
// Rule 336:  FormalParameterListopt ::=
//
// void NullAction(void);
//

//
// Rule 337:  FormalParameterListopt -> FormalParameterList
//
// void NoAction(void);
//

//
// Rule 338:  Interfacesopt ::=
//
// void NullAction(void);
//

//
// Rule 339:  Interfacesopt -> Interfaces
//
// void NoAction(void);
//

//
// Rule 340:  InterfaceMemberDeclarationsopt ::=
//
// void NullAction(void);
//

//
// Rule 341:  InterfaceMemberDeclarationsopt -> InterfaceMemberDeclarations
//
// void NoAction(void);
//

//
// Rule 342:  ForInitopt ::=
//
// void NullAction(void);
//

//
// Rule 343:  ForInitopt -> ForInit
//
// void NoAction(void);
//

//
// Rule 344:  ForUpdateopt ::=
//
// void NullAction(void);
//

//
// Rule 345:  ForUpdateopt -> ForUpdate
//
// void NoAction(void);
//

//
// Rule 346:  ExtendsInterfacesopt ::=
//
// void NullAction(void);
//

//
// Rule 347:  ExtendsInterfacesopt -> ExtendsInterfaces
//
// void NoAction(void);
//

//
// Rule 348:  Catchesopt ::=
//
// void NullAction(void);
//

//
// Rule 349:  Catchesopt -> Catches
//
// void NoAction(void);
//
 
// 
// Rule 350:  PackageHeaderMarker ::=
//
#line 4765 "java.g"
//
// When this function is invoked, if the "parse_package_header_only" flag
// is turned on, we skip to the end-of-file token.
//
void Parser::Act350(void)
{
    if (parse_package_header_only)
        lex_stream -> Reset(lex_stream -> NumTokens() - 1); // point to the EOF token
    Sym(1) = NULL;
}
 
// 
// Rule 351:  MethodHeaderMarker ::=
//
#line 4781 "java.g"
//
// When this function is invoked, if the "parse_header_only" flag
// is turned on, the body of the method being parsed is skipped.
//
void Parser::Act351(void)
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
