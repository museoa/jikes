// $Id$ -*- c++ -*-
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, 1999, 2000, 2001, 2002 International Business
// Machines Corporation and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#ifndef ast_INCLUDED
#define ast_INCLUDED

#include "platform.h"
#include "stream.h"
#include "depend.h"

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif


class Parser;
class SemanticEnvironment;
class LiteralValue;
class Symbol;
class BlockSymbol;
class VariableSymbol;
class MethodSymbol;
class TypeSymbol;
class StoragePool;

class VariableSymbolArray
{
    typedef VariableSymbol * T;

    T **base;
    size_t base_size;
    int top,
        size;
    StoragePool *pool;
    unsigned short log_blksize,
                   base_increment;

    inline size_t Blksize() { return (1 << log_blksize); }

    //
    // Allocate another block of storage for the VariableSymbol array.
    //
    void AllocateMoreSpace();

public:

    //
    // This function is used to reset the size of a VariableSymbol array
    // without allocating or deallocting space. It may be invoked with an
    // integer argument n which indicates the new size or with no argument
    // which indicates that the size should be reset to 0.
    //
    void Reset(const int n = 0)
    {
        if (n < 0 || n > size)
            assert(false);
        top = n;
    }

    //
    // Return length of the VariableSymbol array.
    //
    int Length() { return top; }

    //
    // Return a reference to the ith element of the VariableSymbol array.
    //
    // Note that no check is made here to ensure that 0 <= i < top.
    // Such a check might be useful for debugging and a range exception
    // should be thrown if it yields true.
    //
    T& operator[](const int i) { return base[i >> log_blksize][i]; }

    //
    // Add an element to the VariableSymbol array and return the top index.
    //
    int NextIndex()
    {
        int i = top++;
        if (i == size)
            AllocateMoreSpace();
        return i;
    }

    //
    // Add an element to the VariableSymbol array and return a reference to
    // that new element.
    //
    T& Next() { int i = NextIndex(); return base[i >> log_blksize][i]; }

    //
    // Constructor of a VariableSymbol array.
    //
    VariableSymbolArray(StoragePool *, unsigned);

    //
    // Destructor of an VariableSymbol array.
    //
    ~VariableSymbolArray() { assert(false); }
};


//***************************************************************************
//
// This file contains the definitions of the classes used to construct the
// AST representation of a Java program.
//
// The node Ast is a base class of all other classes. (The name of the other
// classes start with the prefix "Ast".) The nodes associated with executable
// statements (e.g., AstIfStatement) are subclasses of AstStatement and nodes
// associated with expressions (e.g., AstBinaryExpression) are subclasses of
// AstExpression.
//
// The information contained in the AST nodes is described by a grammar where
// each rule consists of a left-hand side nonterminal followed by "-->"
// followed by a right-hand side symbol or a sequence enclosed in the pair of
// symbols "<" and ">". In defining the symbols, the following notation is
// used:
//
// Symbols that are capitalized (e.g., Type) are nonterminals. Symbols that are
// in all upper case (e.g., PACKAGE) represent node kinds. Symbols that contain
// the substring "_token" represents tokens in the source file. The suffix
// "_opt" indicates that a symbol is optional. For example, if Super_opt
// appears in a rule, it indicates that either Super or null can be expected.
// When a symbol is plural (e.g., Modifiers), it indicates zero or more
// instances of such a symbol (a list to be precise) can be expected. Thus,
// when "Modifiers" is specified in the right-hand side of a rule either no
// Modifier or a sequence of them may appear.
//
// Implementation Notes:
//
//    A complete AST tree for a Java program always contains an
//    AstCompilationUnit root node. The kind of that node is
//    Ast::EMPTY_COMPILATION for a tree with no type declaration,
//    Ast::COMPILATION for a tree constructed from an otherwise valid program
//    and Ast::BAD_COMPILATION for a tree constructed from an invalid program.
//
//    All AST tree nodes belong to a StoragePool. When a new node must be added,
//    it is allocated from the same StoragePool. Thus, you should never use
//    operator new to construct an AST object, but instead use New* or Gen*
//    defined in StoragePool. Likewise, AST tree nodes never need destruction -
//    simply delete the pool to reclaim the entire tree.
//
//    When the preprocessor variable JIKES_DEBUG is defined the user may print
//    out an AST tree to standard output by calling the virtual function
//    "Print" for the root node of the tree.
//
//    DynamicArrays are used to implement lists. This representation has the
//    advantage of being very flexible and easy to use. However, it may be
//    slightly less time-efficient than a straightforward linked list. My
//    guess is no more than 10% which justifies this use, but that should be
//    checked at some point...
//
//***************************************************************************
//
// This is a complete list of all Ast nodes declared here to allow
// forward references.
//
class Ast;
class AstListNode;
class AstStatement;
class AstExpression;
class AstPrimitiveType;
class AstArrayType;
class AstSimpleName;
class AstPackageDeclaration;
class AstImportDeclaration;
class AstCompilationUnit;
class AstModifier;
class AstEmptyDeclaration;
class AstClassDeclaration;
class AstClassBody;
class AstArrayInitializer;
class AstBrackets;
class AstVariableDeclaratorId;
class AstVariableDeclarator;
class AstFieldDeclaration;
class AstFormalParameter;
class AstMethodDeclarator;
class AstMethodDeclaration;
class AstStaticInitializer;
class AstThisCall;
class AstSuperCall;
class AstMethodBody;
class AstConstructorDeclaration;
class AstInterfaceDeclaration;
class AstBlock;
class AstLocalVariableDeclarationStatement;
class AstIfStatement;
class AstEmptyStatement;
class AstExpressionStatement;
class AstCaseLabel;
class AstDefaultLabel;
class AstSwitchBlockStatement;
class AstSwitchStatement;
class AstWhileStatement;
class AstDoStatement;
class AstForStatement;
class AstBreakStatement;
class AstContinueStatement;
class AstReturnStatement;
class AstThrowStatement;
class AstSynchronizedStatement;
class AstAssertStatement;
class AstCatchClause;
class AstFinallyClause;
class AstTryStatement;
class AstIntegerLiteral;
class AstLongLiteral;
class AstFloatLiteral;
class AstDoubleLiteral;
class AstTrueLiteral;
class AstFalseLiteral;
class AstStringLiteral;
class AstCharacterLiteral;
class AstNullLiteral;
class AstThisExpression;
class AstSuperExpression;
class AstParenthesizedExpression;
class AstClassInstanceCreationExpression;
class AstDimExpr;
class AstArrayCreationExpression;
class AstFieldAccess;
class AstMethodInvocation;
class AstArrayAccess;
class AstPostUnaryExpression;
class AstPreUnaryExpression;
class AstCastExpression;
class AstBinaryExpression;
class AstTypeExpression;
class AstConditionalExpression;
class AstAssignmentExpression;

class CaseElement;

//
// The Ast base node.
//
class Ast
{
public:
    //
    // These tags are used to identify nodes that can represent more than
    // one kind of objects.
    //
    enum AstTag
    {
        NO_TAG,
        PRIMITIVE_TYPE,
        STATEMENT,
        EXPRESSION,
        MODIFIER,
        STATIC_FIELD,
        UNPARSED,

        _num_tags = MODIFIER
    };

    //
    // These are the different kinds for the Ast objects.
    //
    enum AstKind
    {
        AST,
        IDENTIFIER,
        DOT,
        INTEGER_LITERAL,
        LONG_LITERAL,
        FLOAT_LITERAL,
        DOUBLE_LITERAL,
        TRUE_LITERAL,
        FALSE_LITERAL,
        STRING_LITERAL,
        CHARACTER_LITERAL,
        NULL_LITERAL,
        ARRAY_ACCESS,
        CALL,
        THIS_EXPRESSION,
        SUPER_EXPRESSION,
        PARENTHESIZED_EXPRESSION,
        CLASS_CREATION,
        ARRAY_CREATION,
        POST_UNARY,
        PRE_UNARY,
        CAST,
        BINARY,
        TYPE,
        CONDITIONAL,
        ASSIGNMENT,

        _num_expression_kinds,

        DIM = _num_expression_kinds,
        LIST_NODE,
        INT,
        DOUBLE,
        CHAR,
        LONG,
        FLOAT,
        BYTE,
        SHORT,
        BOOLEAN,
        VOID_TYPE,
        ARRAY,
        COMPILATION,
        BAD_COMPILATION,
        EMPTY_COMPILATION,
        PACKAGE_COMPONENT,
        PACKAGE_NAME,
        PACKAGE,
        IMPORT,
        EMPTY_DECLARATION,
        CLASS,
        CLASS_BODY,
        PUBLIC,
        PROTECTED,
        PRIVATE,
        STATIC,
        ABSTRACT,
        FINAL,
        NATIVE,
        STRICTFP,
        SYNCHRONIZED,
        TRANSIENT,
        VOLATILE,
        FIELD,
        VARIABLE_DECLARATOR,
        VARIABLE_DECLARATOR_NAME,
        BRACKETS,
        METHOD,
        METHOD_DECLARATOR,
        PARAMETER,
        CONSTRUCTOR,
        INTERFACE,
        ARRAY_INITIALIZER,
        STATIC_INITIALIZER,
        THIS_CALL,
        SUPER_CALL,
        BLOCK,
        METHOD_BODY,
        LOCAL_VARIABLE_DECLARATION,
        IF,
        EMPTY_STATEMENT,
        EXPRESSION_STATEMENT,
        SWITCH,
        SWITCH_BLOCK,
        CASE,
        DEFAULT,
        WHILE,
        DO,
        FOR,
        BREAK,
        CONTINUE,
        RETURN,
        THROW,
        SYNCHRONIZED_STATEMENT,
        ASSERT,
        TRY,
        CATCH,
        FINALLY,

        _num_kinds
    };

#ifdef JIKES_DEBUG
    typedef AstKind Kind;
    typedef AstTag Tag;
#else
    typedef unsigned short Kind;
    typedef unsigned char Tag;
#endif

    Kind kind; // every node has a unique kind...
    // Some subsets of nodes are grouped together to form a class of nodes.
    Tag class_tag;

    //
    // "generated" is a boolean value that indicates whether or not a node
    // is associated with a construct in a source file or that is was generated
    // by the compiler. See functions "gen_ ..." and "new_ ..." below.
    //
    bool generated;

#ifdef JIKES_DEBUG
    unsigned id;
    static unsigned count;
    static bool debug_unparse;

    Ast() : id(++count)
    {}
#endif

    virtual ~Ast();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    //
    // Given an Ast tree, check whether or not it is a Name - simple or
    // qualified.
    //
    bool IsName();
    bool IsSimpleNameOrFieldAccess();
    bool IsSuperExpression();
    bool IsThisExpression();
    bool IsLeftHandSide();
    bool IsExplicitConstructorInvocation();
    bool IsGenerated();

    //
    // The Conversion functions below are provided as a convenient way to
    // cast a generic Ast node into a specific node. Note that if one knows
    // the type of a node for sure, it is more efficient to use a specific
    // cast expression. For example, if one knows that a "Ast *p" pointer
    // dereferences a FieldDeclaration then a cast expression should be
    // used to cast p, as follows:
    //
    //       AstFieldDeclaration *fp = (FieldDeclaration *) p;
    //
    // However, if p points to a ClassBodyDeclaration which may be
    // either a FieldDeclaration, MethodDeclaration, ConstructorDeclaration,
    // StaticInitializer, ClassDeclaration, InterfaceDeclaration or a block
    // then the following sequence of code may be used:
    //
    //    AstFieldDeclaration       *fp;
    //    AstMethodDeclaration      *mp;
    //    AstConstructorDeclaration *cp;
    //    AstStaticInitializer      *sp;
    //    AstClassDeclaration       *Cp; // 1.1 only
    //    AstInterfaceDeclaration   *Ip; // 1.1 only
    //    AstBlock                  *Bp; // 1.1 only
    //
    //    if (fp = p -> FieldDeclaration())
    //        ...
    //    else if (mp = p -> MethodDeclaration())
    //        ...
    //    else if (cp = p -> ConstructorDeclaration())
    //        ...
    //    else if (sp = p -> StaticInitializer())
    //        ...
    //    else if (Cp = p -> ClassDeclaration())
    //        ...
    //    else if (Ip = p -> InterfaceDeclaration())
    //        ...
    //    else if (Bp = p -> Block())
    //        ...
    //

    //
    // These cast functions are used for classes that represent more than
    // one kind of nodes.  The functions must be listed after the subclasses
    // have been defined.
    //
    inline AstStatement *StatementCast();
    inline AstExpression *ExpressionCast();
    inline AstPrimitiveType *PrimitiveTypeCast();
    inline AstModifier *ModifierCast();
    inline AstFieldDeclaration *StaticFieldCast();
    inline AstClassBody *UnparsedClassBodyCast();
    inline AstInterfaceDeclaration *UnparsedInterfaceBodyCast();

    //
    // These cast functions are used for classes that represent exactly
    // one kind of node.
    //
    inline AstListNode *ListNodeCast();
    inline AstArrayType *ArrayTypeCast();
    inline AstSimpleName *SimpleNameCast();
    inline AstPackageDeclaration *PackageDeclarationCast();
    inline AstImportDeclaration *ImportDeclarationCast();
    inline AstCompilationUnit *CompilationUnitCast();
    inline AstCompilationUnit *BadCompilationUnitCast();
    inline AstCompilationUnit *EmptyCompilationUnitCast();
    inline AstEmptyDeclaration *EmptyDeclarationCast();
    inline AstClassDeclaration *ClassDeclarationCast();
    inline AstArrayInitializer *ArrayInitializerCast();
    inline AstBrackets *BracketsCast();
    inline AstVariableDeclaratorId *VariableDeclaratorIdCast();
    inline AstVariableDeclarator *VariableDeclaratorCast();
    inline AstFieldDeclaration *FieldDeclarationCast();
    inline AstFormalParameter *FormalParameterCast();
    inline AstMethodDeclarator *MethodDeclaratorCast();
    inline AstMethodDeclaration *MethodDeclarationCast();
    inline AstStaticInitializer *StaticInitializerCast();
    inline AstThisCall *ThisCallCast();
    inline AstSuperCall *SuperCallCast();
    inline AstMethodBody *MethodBodyCast();
    inline AstConstructorDeclaration *ConstructorDeclarationCast();
    inline AstInterfaceDeclaration *InterfaceDeclarationCast();
    inline AstBlock *BlockCast();
    inline AstLocalVariableDeclarationStatement *LocalVariableDeclarationStatementCast();
    inline AstIfStatement *IfStatementCast();
    inline AstEmptyStatement *EmptyStatementCast();
    inline AstExpressionStatement *ExpressionStatementCast();
    inline AstCaseLabel *CaseLabelCast();
    inline AstDefaultLabel *DefaultLabelCast();
    inline AstSwitchBlockStatement *SwitchBlockStatementCast();
    inline AstSwitchStatement *SwitchStatementCast();
    inline AstWhileStatement *WhileStatementCast();
    inline AstDoStatement *DoStatementCast();
    inline AstForStatement *ForStatementCast();
    inline AstBreakStatement *BreakStatementCast();
    inline AstContinueStatement *ContinueStatementCast();
    inline AstReturnStatement *ReturnStatementCast();
    inline AstThrowStatement *ThrowStatementCast();
    inline AstSynchronizedStatement *SynchronizedStatementCast();
    inline AstAssertStatement *AssertStatementCast();
    inline AstCatchClause *CatchClauseCast();
    inline AstFinallyClause *FinallyClauseCast();
    inline AstTryStatement *TryStatementCast();
    inline AstIntegerLiteral *IntegerLiteralCast();
    inline AstLongLiteral *LongLiteralCast();
    inline AstFloatLiteral *FloatLiteralCast();
    inline AstDoubleLiteral *DoubleLiteralCast();
    inline AstTrueLiteral *TrueLiteralCast();
    inline AstFalseLiteral *FalseLiteralCast();
    inline AstStringLiteral *StringLiteralCast();
    inline AstCharacterLiteral *CharacterLiteralCast();
    inline AstNullLiteral *NullLiteralCast();
    inline AstThisExpression *ThisExpressionCast();
    inline AstSuperExpression *SuperExpressionCast();
    inline AstParenthesizedExpression *ParenthesizedExpressionCast();
    inline AstClassInstanceCreationExpression *ClassInstanceCreationExpressionCast();
    inline AstDimExpr *DimExprCast();
    inline AstArrayCreationExpression *ArrayCreationExpressionCast();
    inline AstFieldAccess *FieldAccessCast();
    inline AstMethodInvocation *MethodInvocationCast();
    inline AstArrayAccess *ArrayAccessCast();
    inline AstPostUnaryExpression *PostUnaryExpressionCast();
    inline AstPreUnaryExpression *PreUnaryExpressionCast();
    inline AstCastExpression *CastExpressionCast();
    inline AstBinaryExpression *BinaryExpressionCast();
    inline AstTypeExpression *TypeExpressionCast();
    inline AstConditionalExpression *ConditionalExpressionCast();
    inline AstAssignmentExpression *AssignmentExpressionCast();

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { assert(0); return 0; }
    virtual LexStream::TokenIndex RightToken() { assert(0); return 0; }
};


//
// This AstArray template class can be used to construct a dynamic
// array of arbitrary objects. The space for the array is allocated in
// blocks of size 2**LOG_BLKSIZE. In declaring a Ast array the user
// may specify a value for LOG_BLKSIZE which by default is 6. Also,
// as the array is implemented using a base+offset strategy, the user
// may also specify the number of "slots" to add to the base when the
// current base runs out of space. Each slot points to a block.
//
template <typename T>
class AstArray
{
    T **base;
    size_t base_size;
    int top,
        size;
    StoragePool *pool;
    unsigned short log_blksize,
                   base_increment;

    inline size_t Blksize() { return (1 << log_blksize); }

    //
    // Allocate another block of storage for the Ast array.
    //
    void AllocateMoreSpace();

public:

    //
    // This function is used to reset the size of a Ast array without
    // allocating or deallocting space. It may be invoked with an integer
    // argument n which indicates the new size or with no argument which
    // indicates that the size should be reset to 0.
    //
    void Reset(const int n = 0)
    {
        if (n < 0 || n > size)
            assert(false);
        top = n;
    }

    //
    // Return length of the Ast array.
    //
    int Length() { return top; }

    //
    // Return a reference to the ith element of the Ast array.
    //
    // Note that no check is made here to ensure that 0 <= i < top.
    // Such a check might be useful for debugging and a range exception
    // should be thrown if it yields true.
    //
    T& operator[](const int i) { return base[i >> log_blksize][i]; }

    //
    // Add an element to the Ast array and return the top index.
    //
    int NextIndex()
    {
        int i = top++;
        if (i == size)
            AllocateMoreSpace();
        return i;
    }

    //
    // Add an element to the Ast array and return a reference to
    // that new element.
    //
    T& Next() { int i = NextIndex(); return base[i >> log_blksize][i]; }

    inline void Push(T elt) { Next() = elt; }
    // Not "return (*this)[--top]" because that may violate an invariant
    // in operator[].
    inline T Pop()
    {
        assert(top!=0);
        top--;
        return base[top >> log_blksize][top];
    }
    inline T Top() { assert(top!=0); return (*this)[top-1]; }

    //
    // Constructor of a ast array.
    //
    AstArray(StoragePool *, unsigned);

    //
    // Destructor of an Ast array.
    //
    ~AstArray() { assert(false); }
};


//
// The Ast base node.
//
class AstListNode : public Ast
{
public:
    AstListNode *next;
    Ast *element;
    unsigned index;

    AstListNode()
    {
        Ast::kind = Ast::LIST_NODE;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
#ifdef JIKES_DEBUG
        --count; // don't count these nodes
#endif
    }

    ~AstListNode() {}

    virtual LexStream::TokenIndex LeftToken()
    {
        return element -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return element -> RightToken();
    }
};


class AstStatement : public Ast
{
protected:

    StoragePool *pool;
    VariableSymbolArray *defined_variables;

public:

    bool is_reachable,
         can_complete_normally;

    //
    // Note that for efficiency reasons AstStatement does not have a
    // constructor. Therefore, subclasses that are derived from AstStatement
    // are expected to initialize the fields is_reachable and
    // can_complete_normally appropriately.
    //
    // Note also that an AstStatement is never constructed directly!
    //
    virtual ~AstStatement();

    virtual Ast *Clone(StoragePool *) { return (Ast *) NULL; }

    inline VariableSymbol *&DefinedVariable(int i)
    {
        return (*defined_variables)[i];
    }
    inline int NumDefinedVariables()
    {
        return (defined_variables ? defined_variables -> Length() : 0);
    }
    inline void AllocateDefinedVariables(int estimate = 0);
    inline void AddDefinedVariable(VariableSymbol *);

    virtual LexStream::TokenIndex LeftToken()  { assert(0); return 0; }
    virtual LexStream::TokenIndex RightToken() { assert(0); return 0; }
};


class AstExpression : public Ast
{
public:
    LiteralValue *value;
    Symbol *symbol;

    //
    // Note that for efficiency reasons AstExpression does not have a
    // constructor. However, subclasses that are derived from AstExpression
    // are expected to initialize the fields value and symbol to NULL as
    // indicated below:
    //
    // AstExpression() : value(NULL),
    //                   symbol(NULL)
    // {}
    //

    virtual ~AstExpression();

    bool IsConstant() { return (value != NULL); }

    TypeSymbol *Type();

    virtual Ast *Clone(StoragePool *) { return (Ast *) NULL; }

    virtual LexStream::TokenIndex LeftToken()  { assert(0); return 0; }
    virtual LexStream::TokenIndex RightToken() { assert(0); return 0; }
};


//
// Block --> <BLOCK, {_token, BlockStatements, }_token>
//
// BlockStatement --> LocalVariableDeclarationStatement
//                  | Statement
//                  | ClassDeclaration
//
class AstBlock : public AstStatement
{
private:
    AstArray<AstStatement *> *block_statements;
    VariableSymbolArray *locally_defined_variables;

public:
    enum BlockTag
    {
        NONE,
        TRY_CLAUSE_WITH_FINALLY,
        TRY_CLAUSE_WITH_CATCH,
        ABRUPT_TRY_FINALLY,
        FINALLY,
        SYNCHRONIZED,
        SWITCH
    };
    BlockTag block_tag;

    BlockSymbol *block_symbol;

    int nesting_level;
    LexStream::TokenIndex left_brace_token;
    LexStream::TokenIndex right_brace_token;
    LexStream::TokenIndex label_opt;

    bool no_braces;

    AstBlock(StoragePool *pool_) : block_statements(NULL),
                                   locally_defined_variables(NULL),
                                   block_tag(NONE),
                                   block_symbol(NULL),
                                   nesting_level(0),
                                   label_opt(LexStream::BadToken()),
                                   no_braces(false)
    {
        Ast::kind = Ast::BLOCK;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstBlock();

    inline AstStatement *&Statement(int i) { return (*block_statements)[i]; }
    inline int NumStatements()
    {
        return (block_statements ? block_statements -> Length() : 0);
    }
    inline void AllocateBlockStatements(int estimate = 0);
    inline void AddStatement(AstStatement *);

    inline VariableSymbol *&LocallyDefinedVariable(int i)
    {
        return (*locally_defined_variables)[i];
    }
    inline int NumLocallyDefinedVariables()
    {
        return (locally_defined_variables
                ? locally_defined_variables -> Length() : 0);
    }
    inline void AllocateLocallyDefinedVariables(int estimate = 0);
    inline void AddLocallyDefinedVariable(VariableSymbol *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return left_brace_token; }
    virtual LexStream::TokenIndex RightToken() { return right_brace_token; }

protected:
    void CloneInto(AstBlock *, StoragePool *);
};

//
// Type --> PrimitiveType
//        | ReferenceType
//
// PrimitiveType --> <PrimitiveKind, PrimitiveName>
//
// PrimitiveKind --> BYTE | SHORT | INT | LONG | CHAR | FLOAT | DOUBLE |
//                   BOOLEAN | VOID
//
// PrimitiveName --> byte_token | short_token | int_token | long_token |
//                   char_token | float_token | double_token | boolean_token |
//                   void_token
//
class AstPrimitiveType : public Ast
{
public:
    LexStream::TokenIndex primitive_kind_token;

    AstPrimitiveType(Ast::Kind kind_, LexStream::TokenIndex token_)
        : primitive_kind_token(token_)
    {
        Ast::kind = kind_;
        Ast::class_tag = Ast::PRIMITIVE_TYPE;
        Ast::generated = false;
    }

    virtual ~AstPrimitiveType();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return primitive_kind_token; }
    virtual LexStream::TokenIndex RightToken() { return primitive_kind_token; }
};


//
// Brackets --> <BRACKETS, [_token, ]_token>
//
class AstBrackets : public Ast
{
public:
    LexStream::TokenIndex left_bracket_token;
    LexStream::TokenIndex right_bracket_token;

    AstBrackets(LexStream::TokenIndex left_, LexStream::TokenIndex right_)
        : left_bracket_token(left_),
          right_bracket_token(right_)
    {
        Ast::kind = Ast::BRACKETS;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstBrackets();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return left_bracket_token; }
    virtual LexStream::TokenIndex RightToken() { return right_bracket_token; }
};


//
// ReferenceType --> ClassType
//                 | ArrayType
//
// ClassType --> Name
//
// ArrayType --> <ARRAY, ArrayKind, [_token, ]_token>
//
// ArrayKind --> PrimitiveType
//             | Name
//             | ArrayType
//
class AstArrayType : public Ast
{
private:

    StoragePool *pool;
    AstArray<AstBrackets *> *brackets;

public:
    Ast *type;

    AstArrayType(StoragePool *pool_) : pool(pool_),
                                       brackets(NULL)
    {
        Ast::kind = Ast::ARRAY;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstArrayType();

    inline AstBrackets *&Brackets(int i) { return (*brackets)[i]; }
    inline int NumBrackets() { return (brackets ? brackets -> Length() : 0); }
    inline void AllocateBrackets(int estimate = 0);
    inline void AddBrackets(AstBrackets *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return type -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return Brackets(NumBrackets() - 1) -> RightToken();
    }
};


//
// Name --> SimpleName
//        | FieldAccess
//
// SimpleName --> <IDENTIFIER, identifier_token>
//
class AstSimpleName : public AstExpression
{
public:
    LexStream::TokenIndex identifier_token;

    //
    // When a simple_name refers to a member in an enclosing scope,
    // it is mapped into a new expression that creates a path to
    // the member in question.
    //
    AstExpression *resolution_opt;

    AstSimpleName(LexStream::TokenIndex token_) : identifier_token(token_),
                                                  resolution_opt(NULL)
    {
        Ast::kind = Ast::IDENTIFIER;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstSimpleName();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return identifier_token; }
    virtual LexStream::TokenIndex RightToken() { return identifier_token; }
};

//
// PackageDeclaration --> <PACKAGE, package_token, Name, ;_token>
//
class AstPackageDeclaration : public Ast
{
public:
    LexStream::TokenIndex package_token;
    AstExpression *name;
    LexStream::TokenIndex semicolon_token;

    AstPackageDeclaration()
    {
        Ast::kind = Ast::PACKAGE;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstPackageDeclaration();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return package_token; }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};

//
// ImportDeclaration --> <IMPORT, import_token, Name, *_token_opt, ;_token>
//
class AstImportDeclaration : public Ast
{
public:
    LexStream::TokenIndex import_token;
    AstExpression *name;
    LexStream::TokenIndex star_token_opt;       // import on demand
    LexStream::TokenIndex semicolon_token;

    AstImportDeclaration()
    {
        Ast::kind = Ast::IMPORT;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstImportDeclaration();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return import_token; }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};

//
// CompilationUnit --> <COMPILATION, PackageDeclaration_opt, ImportDeclarations, TypeDeclarations>
//                   | <BAD_COMPILATION, PackageDeclaration_opt, ImportDeclarations, TypeDeclarations>
//                   | <EMPTY_COMPILATION, PackageDeclaration_opt, ImportDeclarations, TypeDeclarations>
//
class AstCompilationUnit : public Ast
{
private:

    StoragePool *pool;
    AstArray<AstImportDeclaration *> *import_declarations;
    AstArray<Ast *> *type_declarations;

public:
    StoragePool *ast_pool;

    AstPackageDeclaration *package_declaration_opt;

    AstCompilationUnit(StoragePool *pool_) : pool(pool_),
                                             import_declarations(NULL),
                                             type_declarations(NULL),
                                             package_declaration_opt(NULL)
    {
        Ast::kind = Ast::COMPILATION;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstCompilationUnit();

    void FreeAst();

    inline AstImportDeclaration *&ImportDeclaration(int i)
    {
        return (*import_declarations)[i];
    }
    inline int NumImportDeclarations()
    {
        return (import_declarations ? import_declarations -> Length() : 0);
    }
    inline void AllocateImportDeclarations(int estimate = 0);
    inline void AddImportDeclaration(AstImportDeclaration *);

    inline void ResetTypeDeclarations(int n)
    {
        if (type_declarations) type_declarations -> Reset(n);
    }
    inline Ast *&TypeDeclaration(int i) { return (*type_declarations)[i]; }
    inline int NumTypeDeclarations()
    {
        return (type_declarations ? type_declarations -> Length() : 0);
    }
    inline void AllocateTypeDeclarations(int estimate = 0);
    inline void AddTypeDeclaration(Ast *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);

    // special forms
    virtual void Unparse(LexStream *, const char * const directory);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        if (package_declaration_opt)
             return package_declaration_opt -> LeftToken();
        else if (NumImportDeclarations() > 0)
             return ImportDeclaration(0) -> LeftToken();
        else if (NumTypeDeclarations() > 0)
             return TypeDeclaration(0) -> LeftToken();

        return 0;
    }

    virtual LexStream::TokenIndex RightToken()
    {
        if (NumTypeDeclarations() > 0)
            return TypeDeclaration(NumTypeDeclarations() - 1) -> RightToken();
        else if (NumImportDeclarations() > 0)
            return ImportDeclaration(NumImportDeclarations() - 1) ->
                RightToken();
        else if (package_declaration_opt)
            return package_declaration_opt -> RightToken();

        return 0;
    }
};


//
// Modifier --> <ModifierKind, ModifierName>
//
// ModifierKind --> PUBLIC | PROTECTED | PRIVATE | STATIC | ABSTRACT | FINAL |
//                  NATIVE | SYNCHRONIZED | TRANSIENT | VOLATILE | STRICTFP
//
// ModifierName --> public_token | protected_token | private_token |
//                  static_token | abstract_token | final_token |
//                  native_token | synchronized_token | transient_token |
//                  volatile_token | strictfp_token
//
class AstModifier : public Ast
{
public:
    LexStream::TokenIndex modifier_kind_token;

    AstModifier(Ast::Kind kind_, LexStream::TokenIndex token_)
        : modifier_kind_token(token_)
    {
        Ast::kind = kind_;
        Ast::class_tag = Ast::MODIFIER;
        Ast::generated = false;
    }

    virtual ~AstModifier();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return modifier_kind_token; }
    virtual LexStream::TokenIndex RightToken() { return modifier_kind_token; }
};


//
// EmptyDeclaration --> <EMPTY_DECLARATION, ;_token>
//
class AstEmptyDeclaration : public Ast
{
public:
    LexStream::TokenIndex semicolon_token;

    AstEmptyDeclaration(LexStream::TokenIndex token_) : semicolon_token(token_)
    {
        Ast::kind = Ast::EMPTY_DECLARATION;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstEmptyDeclaration();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return semicolon_token; }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};

//
// ClassBody --> <CLASS_BODY, {_token, ClassBodyDeclarations, }_token>
//
class AstClassBody : public Ast
{
private:
    friend class Parser;

    StoragePool *pool;
    AstArray<Ast *> *class_body_declarations;

    AstArray<AstFieldDeclaration *> *instance_variables;
    AstArray<AstFieldDeclaration *> *class_variables;
    AstArray<AstMethodDeclaration *> *methods;
    AstArray<AstConstructorDeclaration *> *constructors;
    AstArray<AstStaticInitializer *> *static_initializers;
    AstArray<AstClassDeclaration *> *inner_classes;
    AstArray<AstInterfaceDeclaration *> *inner_interfaces;
    AstArray<AstMethodBody *> *instance_initializers;
    AstArray<AstEmptyDeclaration *> *empty_declarations;

public:

    AstConstructorDeclaration *default_constructor;

    LexStream::TokenIndex left_brace_token;
    LexStream::TokenIndex right_brace_token;

    inline void mark_unparsed() { Ast::class_tag = Ast::UNPARSED; }
    inline void mark_parsed()   { Ast::class_tag = Ast::NO_TAG; }

    AstClassBody(StoragePool *pool_) : pool(pool_),
                                       class_body_declarations(NULL),
                                       instance_variables(NULL),
                                       class_variables(NULL),
                                       methods(NULL),
                                       constructors(NULL),
                                       static_initializers(NULL),
                                       inner_classes(NULL),
                                       inner_interfaces(NULL),
                                       instance_initializers(NULL),
                                       empty_declarations(NULL),
                                       default_constructor(NULL)
    {
        Ast::kind = Ast::CLASS_BODY;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstClassBody();

    inline Ast *&ClassBodyDeclaration(int i)
    {
        return (*class_body_declarations)[i];
    }
    inline int NumClassBodyDeclarations()
    {
        return (class_body_declarations
                ? class_body_declarations -> Length() : 0);
    }
    inline void AllocateClassBodyDeclarations(int estimate = 0);
    inline void AddClassBodyDeclaration(Ast *);
    inline void AddClassBodyDeclarationNicely(Ast *);

    inline AstFieldDeclaration *&InstanceVariable(int i)
    {
        return (*instance_variables)[i];
    }
    inline int NumInstanceVariables()
    {
        return (instance_variables ? instance_variables -> Length() : 0);
    }
    inline void AllocateInstanceVariables(int estimate = 0);
    inline void AddInstanceVariable(AstFieldDeclaration *);

    inline AstFieldDeclaration *&ClassVariable(int i)
    {
        return (*class_variables)[i];
    }
    inline int NumClassVariables()
    {
        return (class_variables ? class_variables -> Length() : 0);
    }
    inline void AllocateClassVariables(int estimate = 0);
    inline void AddClassVariable(AstFieldDeclaration *);

    inline AstMethodDeclaration *&Method(int i) { return (*methods)[i]; }
    inline int NumMethods() { return (methods ? methods -> Length() : 0); }
    inline void AllocateMethods(int estimate = 0);
    inline void AddMethod(AstMethodDeclaration *);

    inline AstConstructorDeclaration *&Constructor(int i)
    {
        return (*constructors)[i];
    }
    inline int NumConstructors()
    {
        return (constructors ? constructors -> Length() : 0);
    }
    inline void AllocateConstructors(int estimate = 0);
    inline void AddConstructor(AstConstructorDeclaration *);

    inline AstStaticInitializer *&StaticInitializer(int i)
    {
        return (*static_initializers)[i];
    }
    inline int NumStaticInitializers()
    {
        return (static_initializers ? static_initializers -> Length() : 0);
    }
    inline void AllocateStaticInitializers(int estimate = 0);
    inline void AddStaticInitializer(AstStaticInitializer *);

    inline AstClassDeclaration *&NestedClass(int i)
    {
        return (*inner_classes)[i];
    }
    inline int NumNestedClasses()
    {
        return (inner_classes ? inner_classes -> Length() : 0);
    }
    inline void AllocateNestedClasses(int estimate = 0);
    inline void AddNestedClass(AstClassDeclaration *);

    inline AstInterfaceDeclaration *&NestedInterface(int i)
    {
        return (*inner_interfaces)[i];
    }
    inline int NumNestedInterfaces()
    {
        return (inner_interfaces ? inner_interfaces -> Length() : 0);
    }
    inline void AllocateNestedInterfaces(int estimate = 0);
    inline void AddNestedInterface(AstInterfaceDeclaration *);

    inline AstMethodBody *&InstanceInitializer(int i)
    {
        return (*instance_initializers)[i];
    }
    inline int NumInstanceInitializers()
    {
        return (instance_initializers ? instance_initializers -> Length() : 0);
    }
    inline void AllocateInstanceInitializers(int estimate = 0);
    inline void AddInstanceInitializer(AstMethodBody *);

    inline AstEmptyDeclaration *&EmptyDeclaration(int i)
    {
        return (*empty_declarations)[i];
    }
    inline int NumEmptyDeclarations()
    {
        return (empty_declarations ? empty_declarations -> Length() : 0);
    }
    inline void AllocateEmptyDeclarations(int estimate = 0);
    inline void AddEmptyDeclaration(AstEmptyDeclaration *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return left_brace_token; }
    virtual LexStream::TokenIndex RightToken() { return right_brace_token; }
};



//
// TypeDeclaration --> ClassDeclaration
//                   | InterfaceDeclaration
//                   | EmptyDeclaration
//
// ClassDeclaration --> <CLASS, ClassModifiers, class_token, identifier_token, Super_opt, Interfaces, ClassBody>
//
// Super --> Name
//
// Interface --> Name
//
// ClassModifier --> Modifier  (ABSTRACT, FINAL or PUBLIC)
//
// ClassBodyDeclaration --> FieldDeclaration
//                        | MethodDeclaration
//                        | ConstructorDeclaration
//                        | StaticInitializer
//
class AstClassDeclaration : public AstStatement
{
    AstArray<AstModifier *> *class_modifiers;
    AstArray<AstExpression *> *interfaces;

public:
    SemanticEnvironment *semantic_environment;

    LexStream::TokenIndex class_token;
    LexStream::TokenIndex identifier_token;
    Ast *super_opt;
    AstClassBody *class_body;

    AstClassDeclaration(StoragePool *pool_) : class_modifiers(NULL),
                                              interfaces(NULL),
                                              semantic_environment(NULL),
                                              super_opt(NULL)
    {
        Ast::kind = Ast::CLASS;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
        AstStatement::pool = pool_;
    }

    virtual ~AstClassDeclaration();

    bool IsValid() { return semantic_environment != NULL; }

    inline void MarkLocal()
    {
        Ast::class_tag = Ast::STATEMENT;
        AstStatement::is_reachable = true;
        AstStatement::can_complete_normally = true;
        AstStatement::defined_variables = NULL;
    }

    inline AstModifier *&ClassModifier(int i) { return (*class_modifiers)[i]; }
    inline int NumClassModifiers()
    {
        return (class_modifiers ? class_modifiers -> Length() : 0);
    }
    inline void AllocateClassModifiers(int estimate = 0);
    inline void AddClassModifier(AstModifier *);

    inline AstExpression *&Interface(int i) { return (*interfaces)[i]; }
    inline int NumInterfaces()
    {
        return (interfaces ? interfaces -> Length() : 0);
    }
    inline void AllocateInterfaces(int estimate = 0);
    inline void AddInterface(AstExpression *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return (NumClassModifiers() > 0
                ? (*class_modifiers)[0] -> LeftToken() : class_token);
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return class_body -> RightToken();
    }
};


//
// VariableInitializer --> Expression
//                       | ArrayInitializer
//
// ArrayInitializer --> <ARRAY_INITIALIZER, {_token, VariableInitializers, }_token>
//
class AstArrayInitializer : public Ast
{
private:

    StoragePool *pool;
    AstArray<Ast *> *variable_initializers;

public:
    LexStream::TokenIndex left_brace_token;
    LexStream::TokenIndex right_brace_token;

    AstArrayInitializer(StoragePool *pool_) : pool(pool_),
                                              variable_initializers(NULL)
    {
        Ast::kind = Ast::ARRAY_INITIALIZER;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstArrayInitializer();

    inline Ast *&VariableInitializer(int i)
    {
        return (*variable_initializers)[i];
    }
    inline int NumVariableInitializers()
    {
        return (variable_initializers ? variable_initializers -> Length() : 0);
    }
    inline void AllocateVariableInitializers(int estimate = 0);
    inline void AddVariableInitializer(Ast *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return left_brace_token; }
    virtual LexStream::TokenIndex RightToken() { return right_brace_token; }
};


//
// VariableDeclaratorId --> <VARIABLE_DECLARATOR_NAME, identifier_token, Brackets>
//
class AstVariableDeclaratorId : public Ast
{
private:

    StoragePool *pool;
    AstArray<AstBrackets *> *brackets;

public:

    LexStream::TokenIndex identifier_token;

    AstVariableDeclaratorId(StoragePool *pool_) : pool(pool_),
                                                  brackets(NULL)
    {
        Ast::kind = Ast::VARIABLE_DECLARATOR_NAME;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstVariableDeclaratorId();

    inline AstBrackets *&Brackets(int i) { return (*brackets)[i]; }
    inline int NumBrackets() { return (brackets ? brackets -> Length() : 0); }
    inline void AllocateBrackets(int estimate = 0);
    inline void AddBrackets(AstBrackets *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return identifier_token; }
    virtual LexStream::TokenIndex RightToken()
    {
        return (NumBrackets() > 0
                ? (*brackets)[NumBrackets() - 1] -> RightToken()
                : identifier_token);
    }
};


//
// VariableDeclarator --> <VARIABLE_DECLARATOR, VariableDeclaratorId,
//     VariableInitializer_opt>
//
// Technically, this is not a statement. But it is similar to local variable
// declarations, which are, and treating it as a statement makes compiling
// initializer blocks more uniform.
//
class AstVariableDeclarator : public AstStatement
{
public:
    VariableSymbol *symbol;

    // when true, this variable signals that the variable_initializer_opt
    // for this variable is currently being evaluated
    bool pending;

    AstVariableDeclaratorId *variable_declarator_name;
    Ast *variable_initializer_opt;

    AstVariableDeclarator() : symbol(NULL),
                              pending(false),
                              variable_declarator_name(NULL),
                              variable_initializer_opt(NULL)
    {
        Ast::kind = Ast::VARIABLE_DECLARATOR;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = NULL;
        AstStatement::defined_variables = NULL;
        AstStatement:: is_reachable = true;
        AstStatement:: can_complete_normally = true;
    }

    virtual ~AstVariableDeclarator();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return variable_declarator_name -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return (variable_initializer_opt ?
                variable_initializer_opt -> RightToken() :
                variable_declarator_name -> RightToken());
    }
};


//
// FieldDeclaration --> <FIELD, VariableModifiers, Type, VariableDeclarators, ;_token>
//
// FieldModifier --> Modifier (PUBLIC, PROTECTED, PRIVATE, FINAL, STATIC, TRANSIENT or VOLATILE)
//
class AstFieldDeclaration : public Ast
{
    StoragePool *pool;
    AstArray<AstModifier *> *variable_modifiers;
    AstArray<AstVariableDeclarator *> *variable_declarators;

public:

    Ast *type;
    LexStream::TokenIndex semicolon_token;

    AstFieldDeclaration(StoragePool *pool_) : pool(pool_),
                                              variable_modifiers(NULL),
                                              variable_declarators(NULL)
    {
        Ast::kind = Ast::FIELD;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstFieldDeclaration();

    inline void MarkStatic() { Ast::class_tag = Ast::STATIC_FIELD; }

    inline AstModifier *&VariableModifier(int i)
    {
        return (*variable_modifiers)[i];
    }
    inline int NumVariableModifiers()
    {
        return (variable_modifiers ? variable_modifiers -> Length() : 0);
    }
    inline void AllocateVariableModifiers(int estimate = 0);
    inline void AddVariableModifier(AstModifier *);

    inline AstVariableDeclarator *&VariableDeclarator(int i)
    {
        return (*variable_declarators)[i];
    }
    inline int NumVariableDeclarators()
    {
        return (variable_declarators ? variable_declarators -> Length() : 0);
    }
    inline void AllocateVariableDeclarators(int estimate = 0);
    inline void AddVariableDeclarator(AstVariableDeclarator *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return (NumVariableModifiers() > 0
                ? (*variable_modifiers)[0] -> LeftToken()
                : type -> LeftToken());
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// FormalParameter --> <PARAMETER, Type, VariableDeclaratorId>
//
class AstFormalParameter : public Ast
{
    StoragePool *pool;
    AstArray<AstModifier *> *parameter_modifiers;

public:

    Ast *type;
    AstVariableDeclarator *formal_declarator;

    AstFormalParameter(StoragePool *pool_) : pool(pool_),
                                             parameter_modifiers(NULL)
    {
        Ast::kind = Ast::PARAMETER;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstFormalParameter();

    inline AstModifier *&ParameterModifier(int i)
    {
        return (*parameter_modifiers)[i];
    }
    inline int NumParameterModifiers()
    {
        return (parameter_modifiers ? parameter_modifiers -> Length() : 0);
    }
    inline void AllocateParameterModifiers(int estimate = 0);
    inline void AddParameterModifier(AstModifier *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
       return (NumParameterModifiers() > 0
               ? (*parameter_modifiers)[0] -> LeftToken()
               : type -> LeftToken());
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return formal_declarator -> RightToken();
    }
};


//
// MethodDeclarator --> <METHOD_DECLARATOR, identifier_token, (_token, FormalParameters, )_token, Brackets>
//
class AstMethodDeclarator : public Ast
{
private:

    StoragePool *pool;
    AstArray<AstBrackets *> *brackets;
    AstArray<AstFormalParameter *> *formal_parameters;

public:
    LexStream::TokenIndex identifier_token;
    LexStream::TokenIndex left_parenthesis_token;
    LexStream::TokenIndex right_parenthesis_token;

    AstMethodDeclarator(StoragePool *pool_) : pool(pool_),
                                              brackets(NULL),
                                              formal_parameters(NULL)
    {
        Ast::kind = Ast::METHOD_DECLARATOR;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstMethodDeclarator();

    inline AstBrackets *&Brackets(int i) { return (*brackets)[i]; }
    inline int NumBrackets() { return (brackets ? brackets -> Length() : 0); }
    inline void AllocateBrackets(int estimate = 0);
    inline void AddBrackets(AstBrackets *);

    inline AstFormalParameter *&FormalParameter(int i)
    {
        return (*formal_parameters)[i];
    }
    inline int NumFormalParameters()
    {
        return (formal_parameters ? formal_parameters -> Length() : 0);
    }
    inline void AllocateFormalParameters(int estimate = 0);
    inline void AddFormalParameter(AstFormalParameter *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return identifier_token; }

    virtual LexStream::TokenIndex RightToken()
    {
        return (NumBrackets()
                ? Brackets(NumBrackets() - 1) -> RightToken()
                : right_parenthesis_token);
    }
};


//
// MethodDeclaration --> <METHOD, MethodModifiers, Type, MethodDeclarator,
//    Throws, MethodBody>
//
// MethodModifier --> Modifier (PUBLIC, PROTECTED, PRIVATE, STATIC, ABSTRACT,
//    FINAL, NATIVE or SYNCHRONIZED)
//
// Throws --> Names
//
// MethodBody --> Block
//              | EmptyStatement
//
class AstMethodDeclaration : public Ast
{
    StoragePool *pool;
    AstArray<AstModifier *> *method_modifiers;
    AstArray<AstExpression *> *throws;

public:
    MethodSymbol *method_symbol;

    Ast *type;
    AstMethodDeclarator *method_declarator;
    AstStatement *method_body; // AstMethodBody or AstEmptyStatement

    AstMethodDeclaration(StoragePool *pool_) : pool(pool_),
                                               method_modifiers(NULL),
                                               throws(NULL),
                                               method_symbol(NULL)
    {
        Ast::kind = Ast::METHOD;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstMethodDeclaration();

    bool IsValid() { return method_symbol != NULL; }

    bool IsSignature() { return (method_body -> EmptyStatementCast() != NULL); }

    inline AstModifier *&MethodModifier(int i)
    {
        return (*method_modifiers)[i];
    }
    inline int NumMethodModifiers()
    {
        return (method_modifiers ? method_modifiers -> Length() : 0);
    }
    inline void AllocateMethodModifiers(int estimate = 0);
    inline void AddMethodModifier(AstModifier *);

    inline AstExpression *&Throw(int i) { return (*throws)[i]; }
    inline int NumThrows() { return (throws ? throws -> Length() : 0); }
    inline void AllocateThrows(int estimate = 0);
    inline void AddThrow(AstExpression *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return (NumMethodModifiers() > 0
                ? (*method_modifiers)[0] -> LeftToken() : type -> LeftToken());
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return method_body -> RightToken();
    }
};

//
// This class represents a method body, for methods and constructors. It is
// basically a block, with the addition of an explicit constructor invocation
// (used only in the context of constructors, NULL otherwise).
//
class AstMethodBody : public AstBlock
{
public:
    AstStatement *explicit_constructor_opt;

    AstMethodBody(StoragePool *pool_)
        : AstBlock(pool_),
          explicit_constructor_opt(NULL)
    {
        Ast::kind = Ast::METHOD_BODY;
        Ast::class_tag = Ast::STATEMENT;
        AstStatement::is_reachable = true;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
        no_braces = true;
    }

    virtual ~AstMethodBody();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);
};


//
// StaticInitializer --> <STATIC_INITIALIZER, static_token, Block>
//
class AstStaticInitializer : public Ast
{
public:
    LexStream::TokenIndex static_token;
    AstMethodBody *block;

    AstStaticInitializer()
    {
        Ast::kind = Ast::STATIC_INITIALIZER;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstStaticInitializer();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return static_token; }
    virtual LexStream::TokenIndex RightToken() { return block -> RightToken(); }
};


//
// ThisCall --> <THIS_CALL, this_token, (_token, Arguments, )_token, ;_token>
//
// Argument --> Expression
//
class AstThisCall : public AstStatement
{
private:
    AstArray<AstExpression *> *arguments;

public:
    MethodSymbol *symbol;

    LexStream::TokenIndex this_token;
    LexStream::TokenIndex left_parenthesis_token;
    LexStream::TokenIndex right_parenthesis_token;
    LexStream::TokenIndex semicolon_token;

    AstThisCall(StoragePool *pool_) : arguments(NULL),
                                      symbol(NULL)
    {
        Ast::kind = Ast::THIS_CALL;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = true;
        AstStatement::can_complete_normally = true;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstThisCall();

    inline AstExpression *&Argument(int i) { return (*arguments)[i]; }
    inline int NumArguments()
    {
        return (arguments ? arguments -> Length() : 0);
    }
    inline void AllocateArguments(int estimate = 0);
    inline void AddArgument(AstExpression *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return this_token; }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// SuperCall --> <SUPER_CALL, super_token, (_token, Arguments, )_token, ;_token>
//             | <SUPER_CALL, SuperField, (_token, Arguments, )_token, ;_token>
//
class AstSuperCall : public AstStatement
{
private:

    AstArray<AstExpression *> *arguments;
    // used only for local classes that use enclosed local variables
    AstArray<AstExpression *> *local_arguments_opt;

    bool add_null_argument;

public:
    MethodSymbol *symbol;

    AstExpression *base_opt;
    LexStream::TokenIndex dot_token_opt;
    LexStream::TokenIndex super_token;
    LexStream::TokenIndex left_parenthesis_token;
    LexStream::TokenIndex right_parenthesis_token;
    LexStream::TokenIndex semicolon_token;

    AstSuperCall(StoragePool *pool_) : arguments(NULL),
                                       local_arguments_opt(NULL),
                                       add_null_argument(false),
                                       symbol(NULL),
                                       base_opt(NULL)
    {
        Ast::kind = Ast::SUPER_CALL;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = true;
        AstStatement::can_complete_normally = true;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstSuperCall();

    inline AstExpression *&Argument(int i) { return (*arguments)[i]; }
    inline int NumArguments()
    {
        return (arguments ? arguments -> Length() : 0);
    }
    inline void AllocateArguments(int estimate = 0);
    inline void AddArgument(AstExpression *);

    inline AstExpression *&LocalArgument(int i)
    {
        return (*local_arguments_opt)[i];
    }
    inline int NumLocalArguments()
    {
        return (local_arguments_opt ? local_arguments_opt -> Length() : 0);
    }
    inline void AllocateLocalArguments(int estimate = 0);
    inline void AddLocalArgument(AstExpression *);

    inline void AddNullArgument() { add_null_argument = true; }
    inline bool NeedsExtraNullArgument() { return add_null_argument; }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return (base_opt ? base_opt -> LeftToken() : super_token);
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// ConstructorDeclaration --> <CONSTRUCTOR, ConstructorModifiers,
//     MethodDeclarator, Throws, ConstructorBody>
//
// ConstructorBody --> <METHOD_BODY, {_token,
//     ExplicitConstructorInvocation, BlockStatements, }_token>
//
// ConstructorModifier --> Modifier (PUBLIC, PROTECTED or PRIVATE)
//
// ExplicitConstructorInvocation --> ThisCall
//                                 | SuperCall
//
// NOTE: We do not actually build ConstructorBodies. Instead, we have
// overloaded MethodBody to store the necessary information. This is
// because this() and super() are treated as Statements in the grammar;
// and in the bytecode, constructors are just methods with a special
// name.
//
class AstConstructorDeclaration : public Ast
{
    StoragePool *pool;
    AstArray<AstModifier *> *constructor_modifiers;
    AstArray<AstExpression *> *throws;

public:
    MethodSymbol *constructor_symbol;
    int index;

    AstMethodDeclarator *constructor_declarator;
    AstMethodBody *constructor_body;

    AstConstructorDeclaration(StoragePool *pool_)
        : pool(pool_),
          constructor_modifiers(NULL),
          throws(NULL),
          constructor_symbol(NULL),
          index(ConstructorCycleChecker::OMEGA)
    {
        Ast::kind = Ast::CONSTRUCTOR;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstConstructorDeclaration();

    bool IsValid() { return constructor_symbol != NULL; }

    inline AstModifier *&ConstructorModifier(int i)
    {
        return (*constructor_modifiers)[i];
    }
    inline int NumConstructorModifiers()
    {
        return (constructor_modifiers ? constructor_modifiers -> Length() : 0);
    }
    inline void AllocateConstructorModifiers(int estimate = 0);
    inline void AddConstructorModifier(AstModifier *);

    inline AstExpression *&Throw(int i) { return (*throws)[i]; }
    inline int NumThrows() { return (throws ? throws -> Length() : 0); }
    inline void AllocateThrows(int estimate = 0);
    inline void AddThrow(AstExpression *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return (NumConstructorModifiers() > 0
                ? (*constructor_modifiers)[0] -> LeftToken()
                : constructor_declarator -> LeftToken());
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return constructor_body -> RightToken();
    }
};


//
// InterfaceDeclaration --> <INTERFACE, Interfacemodifiers, interface_token, identifier_token, ExtendsInterfaces, {_token, InterfaceMemberDeclarations, }_token>
//
// InterfaceModifier --> Modifier (PUBLIC, ABSTRACT)
//
// ExtendsInterfaces --> Names
//
//
// InterfaceMemberDeclaration --> ConstantDeclaration
//                              | AbstractMethodDeclaration
//
// ConstantDeclaration --> FieldDeclaration (where the FieldModifierList is a
//         Constantmodifiers)
//
// ConstantModifier --> Modifier (PUBLIC, STATIC or FINAL)
//
// AbstractMethodDeclaration --> MethodDeclaration (where MethodModifierList
//         is a SignatureModifierList and the MethodBody is an EmptyStatement)
//
// SignatureModifier --> Modifier (PUBLIC or ABSTRACT)
//
class AstInterfaceDeclaration : public Ast
{
private:
    friend class Parser;

    StoragePool *pool;
    AstArray<AstModifier *> *interface_modifiers;
    AstArray<AstExpression *> *extends_interfaces;
    AstArray<Ast *> *interface_member_declarations;

    AstArray<AstFieldDeclaration *> *class_variables;
    AstArray<AstMethodDeclaration *> *methods;
    AstArray<AstClassDeclaration *> *inner_classes;
    AstArray<AstInterfaceDeclaration *> *inner_interfaces;
    AstArray<AstEmptyDeclaration *> *empty_declarations;

public:

    SemanticEnvironment *semantic_environment;

    LexStream::TokenIndex interface_token;
    LexStream::TokenIndex identifier_token;
    LexStream::TokenIndex left_brace_token;
    LexStream::TokenIndex right_brace_token;

    inline void mark_unparsed() { Ast::class_tag = Ast::UNPARSED; }
    inline void mark_parsed()   { Ast::class_tag = Ast::NO_TAG; }

    AstInterfaceDeclaration(StoragePool *pool_)
        : pool(pool_),
          interface_modifiers(NULL),
          extends_interfaces(NULL),
          interface_member_declarations(NULL),
          class_variables(NULL),
          methods(NULL),
          inner_classes(NULL),
          inner_interfaces(NULL),
          empty_declarations(NULL),
          semantic_environment(NULL)
    {
        Ast::kind = Ast::INTERFACE;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstInterfaceDeclaration();

    bool IsValid() { return semantic_environment != NULL; }

    inline AstModifier *&InterfaceModifier(int i)
    {
        return (*interface_modifiers)[i];
    }
    inline int NumInterfaceModifiers()
    {
        return (interface_modifiers ? interface_modifiers -> Length() : 0);
    }
    inline void AllocateInterfaceModifiers(int estimate = 0);
    inline void AddInterfaceModifier(AstModifier *);

    inline AstExpression *&ExtendsInterface(int i)
    {
        return (*extends_interfaces)[i];
    }
    inline int NumExtendsInterfaces()
    {
        return (extends_interfaces ? extends_interfaces -> Length() : 0);
    }
    inline void AllocateExtendsInterfaces(int estimate = 0);
    inline void AddExtendsInterface(AstExpression *);

    inline Ast *&InterfaceMemberDeclaration(int i)
    {
        return (*interface_member_declarations)[i];
    }
    inline int NumInterfaceMemberDeclarations()
    {
        return (interface_member_declarations
                ? interface_member_declarations -> Length() : 0);
    }
    inline void AllocateInterfaceMemberDeclarations(int estimate = 0);
    inline void AddInterfaceMemberDeclaration(Ast *);

    inline AstFieldDeclaration *&ClassVariable(int i)
    {
        return (*class_variables)[i];
    }
    inline int NumClassVariables()
    {
        return (class_variables ? class_variables -> Length() : 0);
    }
    inline void AllocateClassVariables(int estimate = 0);
    inline void AddClassVariable(AstFieldDeclaration *);

    inline AstMethodDeclaration *&Method(int i) { return (*methods)[i]; }
    inline int NumMethods() { return (methods ? methods -> Length() : 0); }
    inline void AllocateMethods(int estimate = 0);
    inline void AddMethod(AstMethodDeclaration *);

    inline AstClassDeclaration *&NestedClass(int i)
    {
        return (*inner_classes)[i];
    }
    inline int NumNestedClasses()
    {
        return (inner_classes ? inner_classes -> Length() : 0);
    }
    inline void AllocateNestedClasses(int estimate = 0);
    inline void AddNestedClass(AstClassDeclaration *);

    inline AstInterfaceDeclaration *&NestedInterface(int i)
    {
        return (*inner_interfaces)[i];
    }
    inline int NumNestedInterfaces()
    {
        return (inner_interfaces ? inner_interfaces -> Length() : 0);
    }
    inline void AllocateNestedInterfaces(int estimate = 0);
    inline void AddNestedInterface(AstInterfaceDeclaration *);

    inline AstEmptyDeclaration *&EmptyDeclaration(int i)
    {
        return (*empty_declarations)[i];
    }
    inline int NumEmptyDeclarations()
    {
        return (empty_declarations ? empty_declarations -> Length() : 0);
    }
    inline void AllocateEmptyDeclarations(int estimate = 0);
    inline void AddEmptyDeclaration(AstEmptyDeclaration *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return (NumInterfaceModifiers() > 0
                ? (*interface_modifiers)[0] -> LeftToken() : interface_token);
    }
    virtual LexStream::TokenIndex RightToken() { return right_brace_token; }
};


//
// LocalVariableDeclarationStatement --> <LOCAL_VARIABLE_DECLARATION, Type, VariableDeclarators, ;_token_opt>
//
class AstLocalVariableDeclarationStatement : public AstStatement
{
    AstArray<AstModifier *> *local_modifiers;
    AstArray<AstVariableDeclarator *> *variable_declarators;

public:
    Ast *type;
    LexStream::TokenIndex semicolon_token_opt;

    AstLocalVariableDeclarationStatement(StoragePool *pool_)
        : local_modifiers(NULL),
          variable_declarators(NULL)
    {
        Ast::kind = Ast::LOCAL_VARIABLE_DECLARATION;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstLocalVariableDeclarationStatement();

    inline AstModifier *&LocalModifier(int i) { return (*local_modifiers)[i]; }
    inline int NumLocalModifiers()
    {
        return (local_modifiers ? local_modifiers -> Length() : 0);
    }
    inline void AllocateLocalModifiers(int estimate = 0);
    inline void AddLocalModifier(AstModifier *);

    inline AstVariableDeclarator *&VariableDeclarator(int i)
    {
        return (*variable_declarators)[i];
    }
    inline int NumVariableDeclarators()
    {
        return (variable_declarators ? variable_declarators -> Length() : 0);
    }
    inline void AllocateVariableDeclarators(int estimate = 0);
    inline void AddVariableDeclarator(AstVariableDeclarator *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return (NumLocalModifiers() > 0
                ? (*local_modifiers)[0] -> LeftToken() : type -> LeftToken());
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return (semicolon_token_opt ? semicolon_token_opt
                : (VariableDeclarator(NumVariableDeclarators() - 1) ->
                   RightToken()));
    }
};

//
// Statement --> IfStatement
//             | WhileStatement
//             | ForStatement
//             | Block
//             | EmptyStatement
//             | ExpressionStatement
//             | SwitchStatement
//             | DoStatement
//             | BreakStatement
//             | ContinueStatement
//             | ReturnStatement
//             | SynchronizedStatement
//             | ThrowStatement
//             | TryStatement
//
// Label --> identifier_token
//
// IfStatement --> <IF, Label_opt, if_token, Expression, TrueStatement, FalseStatement_opt>
//
// TrueStatement --> Statement
//
// FalseStatement --> Statement
//
class AstIfStatement : public AstStatement
{
public:
    LexStream::TokenIndex if_token;
    AstExpression *expression;
    AstStatement *true_statement;
    AstStatement *false_statement_opt;

    AstIfStatement(StoragePool *pool_) : false_statement_opt(NULL)
    {
        Ast::kind = Ast::IF;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstIfStatement();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return if_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return (false_statement_opt ? false_statement_opt -> RightToken()
                                    : true_statement -> RightToken());
    }
};


//
// EmptyStatement --> <EMPTY_STATEMENT, Label_opt, ;_token>
//
class AstEmptyStatement : public AstStatement
{
public:
    LexStream::TokenIndex semicolon_token;

    AstEmptyStatement(StoragePool *pool_, LexStream::TokenIndex token_)
        : semicolon_token(token_)
    {
        Ast::kind = Ast::EMPTY_STATEMENT;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstEmptyStatement();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return semicolon_token;
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// ExpressionStatement --> <EXPRESSION_STATEMENT, Label_opt, Expression, ;_token_opt>
//
class AstExpressionStatement : public AstStatement
{
public:
    AstExpression *expression;
    LexStream::TokenIndex semicolon_token_opt;

    AstExpressionStatement(StoragePool *pool_)
    {
        Ast::kind = Ast::EXPRESSION_STATEMENT;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstExpressionStatement();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return expression -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return (semicolon_token_opt ? semicolon_token_opt
                : expression -> RightToken());
    }
};


//
// SwitchLabel --> CaseLabel
//               | DefaultLabel
//
// CaseLabel --> <CASE, case_token, Expression, :_token>
//
class AstCaseLabel : public Ast
{
public:
    LexStream::TokenIndex case_token;
    AstExpression *expression;
    LexStream::TokenIndex colon_token;
    int map_index;

    AstCaseLabel()
    {
        Ast::kind = Ast::CASE;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstCaseLabel();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return case_token; }
    virtual LexStream::TokenIndex RightToken() { return colon_token; }
};


//
// DefaultLabel --> <DEFAULT, default_token, :_token>
//
class AstDefaultLabel : public Ast
{
public:
    LexStream::TokenIndex default_token;
    LexStream::TokenIndex colon_token;

    AstDefaultLabel()
    {
        Ast::kind = Ast::DEFAULT;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstDefaultLabel();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return default_token; }
    virtual LexStream::TokenIndex RightToken() { return colon_token; }
};


//
// SwitchBlockStatement --> <SWITCH_BLOCK, SwitchLabels, BlockStatements>
//
class AstSwitchBlockStatement : public AstBlock
{
private:
    AstArray<Ast *> *switch_labels;

public:
    AstSwitchBlockStatement(StoragePool *pool_)
        : AstBlock(pool_),
          switch_labels(NULL)
    {
        Ast::kind = Ast::SWITCH_BLOCK;
        no_braces = true;
    }

    virtual ~AstSwitchBlockStatement();

    inline Ast *&SwitchLabel(int i) { return (*switch_labels)[i]; }
    inline int NumSwitchLabels()
    {
        return (switch_labels ? switch_labels -> Length() : 0);
    }
    inline void AllocateSwitchLabels(int estimate = 0);
    inline void AddSwitchLabel(Ast *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);
};


class CaseElement
{
public:
    AstSwitchBlockStatement *switch_block_statement;
    AstExpression *expression;
    int index;

    int Value();
};

//
// SwitchStatement --> <SWITCH, Label_opt, switch_token, Expression, {_token, SwitchBlockStatements, SwitchLabels_opt, }_token>
//
class AstSwitchStatement : public AstStatement
{
    AstArray<CaseElement *> *cases;

public:
    CaseElement default_case;

    LexStream::TokenIndex switch_token;
    AstExpression *expression;
    AstBlock *switch_block;

    AstSwitchStatement(StoragePool *pool_) : cases(NULL)
    {
        Ast::kind = Ast::SWITCH;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstSwitchStatement();

    inline CaseElement *&Case(int i) { return (*cases)[i]; }
    inline int NumCases() { return (cases ? cases -> Length() : 0); }
    inline void AllocateCases(int estimate = 0);
    inline void AddCase(CaseElement *);

    void SortCases();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return switch_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return switch_block -> RightToken();
    }
};


//
// WhileStatement --> <WHILE, Label_opt, while_token, Expression, Statement>
//
class AstWhileStatement : public AstStatement
{
public:
    LexStream::TokenIndex while_token;
    AstExpression *expression;
    AstStatement *statement;

    AstWhileStatement(StoragePool *pool_)
    {
        Ast::kind = Ast::WHILE;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstWhileStatement();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return while_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return statement -> RightToken();
    }
};


//
// DoStatement --> <DO, Label_opt, do_token, Expression, Statement, ;_token>
//
class AstDoStatement : public AstStatement
{
public:
    LexStream::TokenIndex do_token;
    AstStatement *statement;
    LexStream::TokenIndex while_token;
    AstExpression *expression;
    LexStream::TokenIndex semicolon_token;

    AstDoStatement(StoragePool *pool_)
    {
        Ast::kind = Ast::DO;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstDoStatement();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return do_token;
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// ForStatement --> <FOR, Label_opt, for_token, ForInits, Expression_opt, ForUpdates, Statement>
//
// ForInit --> ExpressionStatement
//           | LocalVariableDeclarationStatement
//
// ForUpdate --> ExpressionStatement
//
class AstForStatement : public AstStatement
{
private:

    AstArray<AstStatement *> *for_init_statements;
    AstArray<AstExpressionStatement *> *for_update_statements;

public:
    LexStream::TokenIndex for_token;
    AstExpression *end_expression_opt;
    AstStatement *statement;

    AstForStatement(StoragePool *pool_) : for_init_statements(NULL),
                                          for_update_statements(NULL),
                                          end_expression_opt(NULL)
    {
        Ast::kind = Ast::FOR;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstForStatement();

    inline AstStatement *&ForInitStatement(int i)
    {
        return (*for_init_statements)[i];
    }
    inline int NumForInitStatements()
    {
        return (for_init_statements ? for_init_statements -> Length() : 0);
    }
    inline void AllocateForInitStatements(int estimate = 0);
    inline void AddForInitStatement(AstStatement *);

    inline AstExpressionStatement *&ForUpdateStatement(int i)
    {
        return (*for_update_statements)[i];
    }
    inline int NumForUpdateStatements()
    {
        return (for_update_statements ? for_update_statements -> Length() : 0);
    }
    inline void AllocateForUpdateStatements(int estimate = 0);
    inline void AddForUpdateStatement(AstExpressionStatement *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return for_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return statement -> RightToken();
    }
};


//
// BreakStatement --> <BREAK, Label_opt, break_token, identifier_token_opt, ;_token>
//
class AstBreakStatement : public AstStatement
{
public:
    LexStream::TokenIndex break_token;
    LexStream::TokenIndex identifier_token_opt;
    LexStream::TokenIndex semicolon_token;
    int nesting_level;

    AstBreakStatement(StoragePool *pool_)
        : nesting_level(0)
    {
        Ast::kind = Ast::BREAK;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstBreakStatement();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return break_token;
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};

//
// ContinueStatement --> <CONTINUE, Label_opt, continue_token, SimpleName_opt, ;_token>
//
class AstContinueStatement : public AstStatement
{
public:
    LexStream::TokenIndex continue_token;
    LexStream::TokenIndex identifier_token_opt;
    LexStream::TokenIndex semicolon_token;
    int nesting_level;

    AstContinueStatement(StoragePool *pool_)
        : nesting_level(0)
    {
        Ast::kind = Ast::CONTINUE;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstContinueStatement();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return continue_token;
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// ReturnStatement --> <RETURN, Label_opt, return_token, Expression_opt, ;_token>
//
class AstReturnStatement : public AstStatement
{
public:
    LexStream::TokenIndex return_token;
    AstExpression *expression_opt;
    LexStream::TokenIndex semicolon_token;

    AstReturnStatement(StoragePool *pool_) : expression_opt(NULL)
    {
        Ast::kind = Ast::RETURN;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstReturnStatement();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return return_token;
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// ThrowStatement --> <THROW, Label_opt, throw_token, Expression, ;_token>
//
class AstThrowStatement : public AstStatement
{
public:
    LexStream::TokenIndex throw_token;
    AstExpression *expression;
    LexStream::TokenIndex semicolon_token;

    AstThrowStatement(StoragePool *pool_)
    {
        Ast::kind = Ast::THROW;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstThrowStatement();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return throw_token;
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// SynchronizedStatement --> <SYNCHRONIZED_STATEMENT, Label_opt, synchronized_token, Expression, Block>
//
class AstSynchronizedStatement : public AstStatement
{
public:
    LexStream::TokenIndex synchronized_token;
    AstExpression *expression;
    AstBlock *block;

    AstSynchronizedStatement(StoragePool *pool_)
    {
        Ast::kind = Ast::SYNCHRONIZED_STATEMENT;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstSynchronizedStatement();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return synchronized_token;
    }
    virtual LexStream::TokenIndex RightToken() { return block -> RightToken(); }
};


//
// AssertStatement --> <ASSERT, Label_opt, assert_token, Expression, ;_token>
//                 --> <ASSERT, Label_opt, assert_token, Expression, :_token, Expression, ;_token>
//
class AstAssertStatement : public AstStatement
{
public:
    LexStream::TokenIndex assert_token,
                          semicolon_token;
    AstExpression *condition,
                  *message_opt;
    VariableSymbol *assert_variable;

    AstAssertStatement(StoragePool *pool_) : assert_variable(NULL)
    {
        Ast::kind = Ast::ASSERT;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstAssertStatement();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return assert_token; }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// CatchClause --> <CATCH, catch_token, FormalParameter, Block>
//
class AstCatchClause : public Ast
{
public:
    VariableSymbol *parameter_symbol;

    LexStream::TokenIndex catch_token;
    AstFormalParameter *formal_parameter;
    AstBlock *block;

    AstCatchClause() : parameter_symbol(NULL)
    {
        Ast::kind = Ast::CATCH;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstCatchClause();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return catch_token; }
    virtual LexStream::TokenIndex RightToken() { return block -> RightToken(); }
};


//
// FinallyClause --> <FINALLY, finally_token, Block>
//
class AstFinallyClause : public Ast
{
public:
    LexStream::TokenIndex finally_token;
    AstBlock *block;

    AstFinallyClause()
    {
        Ast::kind = Ast::FINALLY;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstFinallyClause();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return finally_token; }
    virtual LexStream::TokenIndex RightToken() { return block -> RightToken(); }
};


//
// TryStatement --> <TRY, Label_opt, try-token, Block CatchClauses, FinallyClause_opt>
//
class AstTryStatement : public AstStatement
{
private:

    AstArray<AstCatchClause *> *catch_clauses;

public:
    LexStream::TokenIndex try_token;
    AstBlock *block;
    AstFinallyClause *finally_clause_opt;
    bool processing_try_block;

    AstTryStatement(StoragePool *pool_) : catch_clauses(NULL),
                                          finally_clause_opt(NULL),
                                          processing_try_block(false)
    {
        Ast::kind = Ast::TRY;
        Ast::class_tag = Ast::STATEMENT;
        Ast::generated = false;
        AstStatement::pool = pool_;
        AstStatement::is_reachable = false;
        AstStatement::can_complete_normally = false;
        AstStatement::defined_variables = NULL;
    }

    virtual ~AstTryStatement();

    inline AstCatchClause *&CatchClause(int i) { return (*catch_clauses)[i]; }
    inline int NumCatchClauses()
    {
        return (catch_clauses ? catch_clauses -> Length() : 0);
    }
    inline void AllocateCatchClauses(int estimate = 0);
    inline void AddCatchClause(AstCatchClause *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return try_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        //
        // when the Finally clause is null, there must be one or more catch
        // clauses
        //
        return (finally_clause_opt ? finally_clause_opt -> RightToken()
                : CatchClause(NumCatchClauses() - 1) -> RightToken());
    }
};

//
// Expression --> Primary
//              | UnaryExpression
//              | BinaryExpression
//              | ConditionalExpression
//              | AssignmentExpression
//
// Primary --> Literal
//           | NullLiteral
//           | ThisExpression
//           | SuperExpression
//           | ParenthesizedExpression
//           | ClassInstanceCreationExpression
//           | ArrayCreationExpression
//           | FieldAccess
//           | MethodInvocation
//           | ArrayAccess
//
// Literal --> IntegerLiteral
//           | LongLiteral
//           | FloatLiteral
//           | DoubleLiteral
//           | BooleanLiteral
//           | StringLiteral
//           | CharacterLiteral
//
// BooleanLiteral --> TrueLiteral
//                  | FalseLiteral
//

//
// IntegerLiteral --> <INTEGER_LITERAL, integer_literal_token, value>
//
class AstIntegerLiteral : public AstExpression
{
public:
    LexStream::TokenIndex integer_literal_token;

    AstIntegerLiteral(LexStream::TokenIndex token_)
        : integer_literal_token(token_)
    {
        Ast::kind = Ast::INTEGER_LITERAL;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstIntegerLiteral();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return integer_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return integer_literal_token; }
};


//
// LongLiteral --> <LONG_LITERAL, long_literal_token, value>
//
class AstLongLiteral : public AstExpression
{
public:
    LexStream::TokenIndex long_literal_token;

    AstLongLiteral(LexStream::TokenIndex token_) : long_literal_token(token_)
    {
        Ast::kind = Ast::LONG_LITERAL;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstLongLiteral();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return long_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return long_literal_token; }
};


//
// FloatLiteral --> <FLOAT_LITERAL, Literal, value>
//
class AstFloatLiteral : public AstExpression
{
public:
    LexStream::TokenIndex float_literal_token;

    AstFloatLiteral(LexStream::TokenIndex token_) : float_literal_token(token_)
    {
        Ast::kind = Ast::FLOAT_LITERAL;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstFloatLiteral();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return float_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return float_literal_token; }
};

//
// DoubleLiteral --> <DOUBLE_LITERAL, Literal, value>
//
class AstDoubleLiteral : public AstExpression
{
public:
    LexStream::TokenIndex double_literal_token;

    AstDoubleLiteral(LexStream::TokenIndex token_)
        : double_literal_token(token_)
    {
        Ast::kind = Ast::DOUBLE_LITERAL;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstDoubleLiteral();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return double_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return double_literal_token; }
};

//
// TrueLiteral --> <TRUE_LITERAL, Literal, value>
//
class AstTrueLiteral : public AstExpression
{
public:
    LexStream::TokenIndex true_literal_token;

    AstTrueLiteral(LexStream::TokenIndex token_) : true_literal_token(token_)
    {
        Ast::kind = Ast::TRUE_LITERAL;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstTrueLiteral();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return true_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return true_literal_token; }
};

//
// FalseLiteral --> <FALSE_LITERAL, Literal, value>
//
class AstFalseLiteral : public AstExpression
{
public:
    LexStream::TokenIndex false_literal_token;

    AstFalseLiteral(LexStream::TokenIndex token_) : false_literal_token(token_)
    {
        Ast::kind = Ast::FALSE_LITERAL;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstFalseLiteral();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return false_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return false_literal_token; }
};

//
// StringLiteral --> <STRING_LITERAL, Literal, value>
//
class AstStringLiteral : public AstExpression
{
public:
    LexStream::TokenIndex string_literal_token;

    AstStringLiteral(LexStream::TokenIndex token_)
        : string_literal_token(token_)
    {
        Ast::kind = Ast::STRING_LITERAL;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstStringLiteral();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return string_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return string_literal_token; }
};

//
// CharacterLiteral --> <CHARACTER_LITERAL, literal_token, value>
//
class AstCharacterLiteral : public AstExpression
{
public:
    LexStream::TokenIndex character_literal_token;

    AstCharacterLiteral(LexStream::TokenIndex token_)
        : character_literal_token(token_)
    {
        Ast::kind = Ast::CHARACTER_LITERAL;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstCharacterLiteral();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return character_literal_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return character_literal_token;
    }
};

//
// NullLiteral --> <NULL_EXPRESSION, null_token>
//
class AstNullLiteral : public AstExpression
{
public:
    LexStream::TokenIndex null_token;

    AstNullLiteral(LexStream::TokenIndex token_) : null_token(token_)
    {
        Ast::kind = Ast::NULL_LITERAL;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstNullLiteral();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return null_token; }
    virtual LexStream::TokenIndex RightToken() { return null_token; }
};

//
// ThisExpression --> <THIS, this_token>
//
class AstThisExpression : public AstExpression
{
public:
    LexStream::TokenIndex this_token;

    AstThisExpression(LexStream::TokenIndex token_) : this_token(token_)
    {
        Ast::kind = Ast::THIS_EXPRESSION;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstThisExpression();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return this_token; }
    virtual LexStream::TokenIndex RightToken() { return this_token; }
};


//
// SuperExpression --> <SUPER, super_token>
//
class AstSuperExpression : public AstExpression
{
public:
    LexStream::TokenIndex super_token;

    AstSuperExpression(LexStream::TokenIndex token_) : super_token(token_)
    {
        Ast::kind = Ast::SUPER_EXPRESSION;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstSuperExpression();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return super_token; }
    virtual LexStream::TokenIndex RightToken() { return super_token; }
};


//
// ParenthesizedExpression --> <PARENTHESIZED_EXPRESSION, (_token, Expression, )_token>
//
class AstParenthesizedExpression : public AstExpression
{
public:
    LexStream::TokenIndex left_parenthesis_token;
    AstExpression *expression;
    LexStream::TokenIndex right_parenthesis_token;

    AstParenthesizedExpression()
    {
        Ast::kind = Ast::PARENTHESIZED_EXPRESSION;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstParenthesizedExpression();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return left_parenthesis_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return right_parenthesis_token;
    }
};


//
// TypeExpression --> <TYPE, Type>
//
class AstTypeExpression : public AstExpression
{
public:
    Ast *type;

    AstTypeExpression(Ast *type_) : type(type_)
    {
        Ast::kind = Ast::TYPE;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstTypeExpression();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return type -> LeftToken(); }
    virtual LexStream::TokenIndex RightToken() { return type -> RightToken(); }
};


//
// ClassInstanceCreationExpression --> <CLASS_CREATION, new_token, TypeExpression, (_token, Arguments, )_token>
//
// Sometimes, during semantic analysis an artificial base_opt expression is
// constructed. In such a case, the user can determine this condition by
// testing whether or not dot_token_opt is 0;
//
class AstClassInstanceCreationExpression : public AstExpression
{
private:

    StoragePool *pool;
    AstArray<AstExpression *> *arguments;
    // used only for local classes that use enclosed local variables
    AstArray<AstExpression *> *local_arguments_opt;

    bool add_null_argument;

public:
    AstExpression *base_opt;
    LexStream::TokenIndex dot_token_opt;
    LexStream::TokenIndex new_token;
    AstTypeExpression *class_type;
    LexStream::TokenIndex left_parenthesis_token;
    LexStream::TokenIndex right_parenthesis_token;
    AstClassBody *class_body_opt;

    AstClassDeclaration *anonymous_declaration;
    //
    // For anonymous classes, we resolve the original statement into a new
    // one that does not have a class_body_opt. This is necessary to get
    // the parameters called in the correct order.
    //
    AstClassInstanceCreationExpression *resolution_opt;

    AstClassInstanceCreationExpression(StoragePool *pool_)
        : pool(pool_),
          arguments(NULL),
          local_arguments_opt(NULL),
          add_null_argument(false),
          base_opt(NULL),
          class_body_opt(NULL),
          anonymous_declaration(NULL),
          resolution_opt(NULL)
    {
        Ast::kind = Ast::CLASS_CREATION;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstClassInstanceCreationExpression();

    inline AstExpression *&Argument(int i) { return (*arguments)[i]; }
    inline int NumArguments()
    {
        return (arguments ? arguments -> Length() : 0);
    }
    inline void AllocateArguments(int estimate = 0);
    inline void AddArgument(AstExpression *);

    inline AstExpression *&LocalArgument(int i)
    {
        return (*local_arguments_opt)[i];
    }
    inline int NumLocalArguments()
    {
        return (local_arguments_opt ? local_arguments_opt -> Length() : 0);
    }
    inline void AllocateLocalArguments(int estimate = 0);
    inline void AddLocalArgument(AstExpression *);

    inline void AddNullArgument() { add_null_argument = true; }
    inline bool NeedsExtraNullArgument() { return add_null_argument; }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return (base_opt ? base_opt -> LeftToken() : new_token);
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return (class_body_opt ? class_body_opt -> RightToken()
                : right_parenthesis_token);
    }
};


//
// DimExpr --> <DIM, [_token, Expression, ]_token>
//
class AstDimExpr : public Ast
{
public:
    LexStream::TokenIndex left_bracket_token;
    AstExpression *expression;
    LexStream::TokenIndex right_bracket_token;

    AstDimExpr()
    {
        Ast::kind = Ast::DIM;
        Ast::class_tag = Ast::NO_TAG;
        Ast::generated = false;
    }

    virtual ~AstDimExpr();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return left_bracket_token; }
    virtual LexStream::TokenIndex RightToken() { return right_bracket_token; }
};


//
// ArrayCreationExpression --> <ARRAY_CREATION, new_token, Type, DimExprs, Brackets>
//
class AstArrayCreationExpression : public AstExpression
{
private:

    StoragePool *pool;
    AstArray<AstBrackets *> *brackets;
    AstArray<AstDimExpr *> *dim_exprs;

public:
    LexStream::TokenIndex new_token;
    Ast *array_type;
    AstArrayInitializer *array_initializer_opt;

    AstArrayCreationExpression(StoragePool *pool_) : pool(pool_),
                                                     brackets(NULL),
                                                     dim_exprs(NULL),
                                                     array_initializer_opt(NULL)
    {
        Ast::kind = Ast::ARRAY_CREATION;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstArrayCreationExpression();

    inline AstBrackets *&Brackets(int i) { return (*brackets)[i]; }
    inline int NumBrackets() { return (brackets ? brackets -> Length() : 0); }
    inline void AllocateBrackets(int estimate = 0);
    inline void AddBrackets(AstBrackets *);

    inline AstDimExpr *&DimExpr(int i) { return (*dim_exprs)[i]; }
    inline int NumDimExprs() { return (dim_exprs ? dim_exprs -> Length() : 0); }
    inline void AllocateDimExprs(int estimate = 0);
    inline void AddDimExpr(AstDimExpr *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return new_token; }
    virtual LexStream::TokenIndex RightToken()
    {
        return (array_initializer_opt ? array_initializer_opt -> RightToken()
                : (NumBrackets() > 0
                   ? Brackets(NumBrackets() - 1) -> RightToken()
                   : DimExpr(NumDimExprs() - 1) -> RightToken()));
    }
};


//
// FieldAccess --> <DOT, Base, ._token, SimpleName>
//               | <DOT, TypeExpression, ._token, class_token>
//               | <DOT, TypeExpression, ._token, this_token>
//
// SuperField --> <DOT, TypeExpression, ._token, super_token>
//
// Base --> Primary
//        | Name
//
class AstFieldAccess : public AstExpression
{
public:
    enum FieldAccessTag
    {
        NONE,
        CLASS_TAG,
        THIS_TAG,
        SUPER_TAG,

        _num_kinds
    };

    AstExpression *base;
    LexStream::TokenIndex dot_token;
    LexStream::TokenIndex identifier_token;

    //
    // When the right-side of a field access consists of the keyword this,
    // we resolve it into either "this" or a chain of "this$0" traversals.
    //
    // If the base expression of FieldAccess expression is of the form
    // type.this.X, where X is a private variable that is a member of an
    // outer class, then we resolve it into a method call to the read_mehod
    // that gives access to X.
    //
    AstExpression *resolution_opt;

    AstFieldAccess(FieldAccessTag tag = NONE) : resolution_opt(NULL),
                                                field_access_tag(tag)
    {
        Ast::kind = Ast::DOT;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstFieldAccess();

    bool IsNameAccess()  { return field_access_tag == NONE; }
    bool IsThisAccess()  { return field_access_tag == THIS_TAG; }
    bool IsSuperAccess() { return field_access_tag == SUPER_TAG; }
    bool IsClassAccess() { return field_access_tag == CLASS_TAG; }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()  { return base -> LeftToken(); }
    virtual LexStream::TokenIndex RightToken() { return identifier_token; }

private:
    FieldAccessTag field_access_tag;
};


//
// MethodInvocation --> <CALL, Method, (_token, Arguments, )_token>
//
// Method --> SimpleName
//          | FieldAccess
//
class AstMethodInvocation : public AstExpression
{
private:

    StoragePool *pool;
    AstArray<AstExpression *> *arguments;

public:
    AstExpression *method;
    LexStream::TokenIndex left_parenthesis_token;
    LexStream::TokenIndex right_parenthesis_token;

    //
    // When a method refers to a member in an enclosing scope,
    // it is mapped into a new expression that creates a path to
    // the member in question.
    //
    AstExpression *resolution_opt;

    AstMethodInvocation(StoragePool *pool_) : pool(pool_),
                                              arguments(NULL),
                                              resolution_opt(NULL)
    {
        Ast::kind = Ast::CALL;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstMethodInvocation();

    inline AstExpression *&Argument(int i) { return (*arguments)[i]; }
    inline int NumArguments()
    {
        return (arguments ? arguments -> Length() : 0);
    }
    inline void AllocateArguments(int estimate = 0);
    inline void AddArgument(AstExpression *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return method -> LeftToken(); }
    virtual LexStream::TokenIndex RightToken()
    {
        return right_parenthesis_token;
    }
};


//
// ArrayAccess --> <ARRAY_ACCESS, Base, [_token, Expression, ]_token>
//
class AstArrayAccess : public AstExpression
{
public:
    AstExpression *base;
    LexStream::TokenIndex left_bracket_token;
    AstExpression *expression;
    LexStream::TokenIndex right_bracket_token;

    AstArrayAccess()
    {
        Ast::kind = Ast::ARRAY_ACCESS;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstArrayAccess();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken() { return base -> LeftToken(); }
    virtual LexStream::TokenIndex RightToken() { return right_bracket_token; }
};


//
// UnaryExpression --> PreUnaryExpression
//                   | PostUnaryExpression
//                   | CastExpression
//
// PostUnaryExpression --> <POST_UNARY, PostUnaryTag, Expression, PostOperator>
//
// PostUnaryTag --> PLUSPLUS | MINUSMINUS
//
// PostOperator --> ++_token | --_token
//
class AstPostUnaryExpression : public AstExpression
{
public:
    enum PostUnaryExpressionTag
    {
        NONE,
        PLUSPLUS,
        MINUSMINUS,

        _num_kinds
    };

    PostUnaryExpressionTag post_unary_tag;
    AstExpression *expression;
    LexStream::TokenIndex post_operator_token;

    //
    // When the left-hand side of an assignment is a name that refers
    // to a private field in an enclosing scope, the access method
    // that gives write-permission to that field is recorded here.
    //
    MethodSymbol *write_method;

    AstPostUnaryExpression(PostUnaryExpressionTag tag_) : post_unary_tag(tag_),
                                                          write_method(NULL)
    {
        Ast::kind = Ast::POST_UNARY;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstPostUnaryExpression();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return expression -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return post_operator_token;
    }
};


//
// PreUnaryExpression -->  <PRE_UNARY, PreUnaryTag, PreOperator, Expression>
//
// PreUnaryTag --> PLUS | MINUS | TWIDDLE | NOT | PLUSPLUS | MINUSMINUS
//
// PreOperator --> +_token | -_token | ~_token | !_token | ++_token | --_token
//
class AstPreUnaryExpression : public AstExpression
{
public:
    enum PreUnaryExpressionTag
    {
        NONE,
        PLUSPLUS,
        MINUSMINUS,
        PLUS,
        MINUS,
        TWIDDLE,
        NOT,

        _num_kinds
    };

    PreUnaryExpressionTag pre_unary_tag;
    LexStream::TokenIndex pre_operator_token;
    AstExpression *expression;

    //
    // When the left-hand side of an assignment is a name that refers
    // to a private field in an enclosing scope, the access method
    // that gives write-permission to that field is recorded here.
    //
    MethodSymbol *write_method;

    AstPreUnaryExpression(PreUnaryExpressionTag tag_) : pre_unary_tag(tag_),
                                                        write_method(NULL)
    {
        Ast::kind = Ast::PRE_UNARY;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstPreUnaryExpression();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return pre_operator_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return expression -> RightToken();
    }
};


//
// CastExpression --> <cAstkind, (_token_opt, Type_opt, Brackets )_token_opt, Expression>
//
// cAstkind --> CAST
//
// NOTE that the optional symbols above are absent only when the compiler
// inserts a CAST conversion node into the program.
//
class AstCastExpression : public AstExpression
{
private:

    StoragePool *pool;
    AstArray<AstBrackets *> *brackets;

public:
    LexStream::TokenIndex left_parenthesis_token_opt;
    Ast *type_opt;
    LexStream::TokenIndex right_parenthesis_token_opt;
    AstExpression *expression;

    AstCastExpression(StoragePool *pool_) : pool(pool_),
                                            brackets(NULL),
                                            type_opt(NULL)
    {
        Ast::kind = Ast::CAST;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstCastExpression();

    inline AstBrackets *&Brackets(int i) { return (*brackets)[i]; }
    inline int NumBrackets() { return (brackets ? brackets -> Length() : 0); }
    inline void AllocateBrackets(int estimate = 0);
    inline void AddBrackets(AstBrackets *);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return (left_parenthesis_token_opt
                ? left_parenthesis_token_opt : expression -> LeftToken());
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return expression -> RightToken();
    }
};


//
// BinaryExpression --> <BINARY, BinaryTag, LeftExpression, BinaryOperator, RightExpression>
//
// LeftExpression --> Expression
//
// RightExpression --> Expression
//                   | type
//
// BinaryTag --> STAR | SLASH | MOD | PLUS | MINUS | LEFT_SHIFT | RIGHT_SHIFT |
//               UNSIGNED_RIGHT_SHIFT | INSTANCEOF | LESS | GREATER |
//               LESS_EQUAL | GREATER_EQUAL | EQUAL_EQUAL | NOT_EQUAL |
//               AND | XOR | IOR | AND_AND | OR_OR
//
// BinaryOperator --> *_token | /_token | %_token | +_token | -_token |
//                    <<_token | >>_token | >>>_token | instanceof_token |
//                    <_token | >_token | <=_token | >=_token | ==_token |
//                    !=_token | &_token | ^_token | |_token | &&_token |
//                    ||_token
//
class AstBinaryExpression : public AstExpression
{
public:
    enum BinaryExpressionTag
    {
        NONE,
        STAR,
        SLASH,
        MOD,
        PLUS,
        MINUS,
        LEFT_SHIFT,
        RIGHT_SHIFT,
        UNSIGNED_RIGHT_SHIFT,
        INSTANCEOF,
        LESS,
        GREATER,
        AND,
        XOR,
        IOR,
        AND_AND,
        OR_OR,

        LESS_EQUAL,
        GREATER_EQUAL,
        EQUAL_EQUAL,
        NOT_EQUAL,

        _num_kinds
    };

    BinaryExpressionTag binary_tag;
    AstExpression *left_expression;
    LexStream::TokenIndex binary_operator_token;
    AstExpression *right_expression;

    AstBinaryExpression(BinaryExpressionTag tag_) : binary_tag(tag_)
    {
        Ast::kind = Ast::BINARY;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstBinaryExpression();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return left_expression -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return right_expression -> RightToken();
    }
};


//
// ConditionalExpression --> <CONDITIONAL, Expression, ?_token, Expression, :_token, Expression>
//
class AstConditionalExpression : public AstExpression
{
public:
    AstExpression *test_expression;
    LexStream::TokenIndex question_token;
    AstExpression *true_expression;
    LexStream::TokenIndex colon_token;
    AstExpression *false_expression;

    AstConditionalExpression()
    {
        Ast::kind = Ast::CONDITIONAL;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstConditionalExpression();

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return test_expression -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return false_expression -> RightToken();
    }
};


//
// Assignment --> <ASSIGNMENT, AssignmentTag, LeftHandSide, AssignmentOperator, Expression>
//
// AssignmentTag --> EQUAL | STAR_EQUAL | SLASH_EQUAL | MOD_EQUAL |
//                   PLUS_EQUAL | MINUS_EQUAL | LEFT_SHIFT_EQUAL |
//                   RIGHT_SHIFT_EQUAL | UNSIGNED_RIGHT_SHIFT_EQUAL |
//                   AND_EQUAL | XOR_EQUAL | IOR_EQUAL
//
// LeftHandSide --> Name | FieldAccess | ArrayAccess
//                | <cAstkind, (_token_opt, Type_opt, Brackets )_token_opt, Name>
//                | <cAstkind, (_token_opt, Type_opt, Brackets )_token_opt, FieldAccess>
//                | <cAstkind, (_token_opt, Type_opt, Brackets )_token_opt, ArrayAccess>
//
// NOTE: that a LeftHandSide appears as a cast node only when the
// assignment_operator in question is of the form "op=" and the application
// of the operator requires a casting of the value of the left-hand side.
//
// AssignmentOperator --> =_token | *=_token | /=_token | %=_token | +=_token |
//                        -=_token | <<=_token | >>=_token | >>>=_token |
//                        &=_token | ^=_token | |=_token
//
class AstAssignmentExpression : public AstExpression
{
public:
    enum AssignmentExpressionTag
    {
        NONE,
        SIMPLE_EQUAL,
        STAR_EQUAL,
        SLASH_EQUAL,
        MOD_EQUAL,
        PLUS_EQUAL,
        MINUS_EQUAL,
        LEFT_SHIFT_EQUAL,
        RIGHT_SHIFT_EQUAL,
        UNSIGNED_RIGHT_SHIFT_EQUAL,

        AND_EQUAL,
        XOR_EQUAL,
        IOR_EQUAL,

        _num_kinds
    };

    //
    // When the left-hand side of an assignment is a name that refers
    // to a private field in an enclosing scope, the access method
    // that gives write-permission to that field is recorded here.
    //
    MethodSymbol *write_method;

    AssignmentExpressionTag assignment_tag;
    AstExpression *left_hand_side;
    LexStream::TokenIndex assignment_operator_token;
    AstExpression *expression;

    AstAssignmentExpression(AssignmentExpressionTag tag_,
                            LexStream::TokenIndex token_)
        : write_method(NULL),
          assignment_tag(tag_),
          assignment_operator_token(token_)
    {
        Ast::kind = Ast::ASSIGNMENT;
        Ast::class_tag = Ast::EXPRESSION;
        Ast::generated = false;
        AstExpression::value = NULL;
        AstExpression::symbol = NULL;
    }

    virtual ~AstAssignmentExpression();

    inline bool SimpleAssignment() { return assignment_tag == SIMPLE_EQUAL; }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream &);
    virtual void Unparse(Ostream &, LexStream *);
#endif

    virtual Ast *Clone(StoragePool *);

    virtual LexStream::TokenIndex LeftToken()
    {
        return left_hand_side -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return expression -> RightToken();
    }
};


//
// Given an Ast tree, check whether or not it is a Name - simple or qualified.
//
inline bool Ast::IsName()
{
    Ast *name = this;
    for (AstFieldAccess *field_access = name -> FieldAccessCast();
         field_access && field_access -> IsNameAccess();
         field_access = name -> FieldAccessCast())
    {
        name = field_access -> base;
    }
    return name -> kind == IDENTIFIER;
}


//
// Given an Ast tree, check whether or not it is a simple name or
// a field access consisting only of simple names or keywords.
//
inline bool Ast::IsSimpleNameOrFieldAccess()
{
    Ast *name = this;
    for (AstFieldAccess *field_access = name -> FieldAccessCast();
         field_access; field_access = name -> FieldAccessCast())
    {
        name = field_access -> base;
    }
    return name -> kind == IDENTIFIER || name -> kind == TYPE;
}


//
// Do we have a simple 'super' expression or a field of the form XXX.super
//
inline bool Ast::IsSuperExpression()
{
    return kind == SUPER_EXPRESSION ||
        (kind == DOT && FieldAccessCast() -> IsSuperAccess());
}

//
// Do we have a simple 'this' expression or a field of the form XXX.this
//
inline bool Ast::IsThisExpression()
{
    return kind == THIS_EXPRESSION ||
        (kind == DOT && FieldAccessCast() -> IsThisAccess());
}

//
// Given an Ast tree, check whether or not it is a Name - simple or qualified.
//
inline bool Ast::IsLeftHandSide()
{
    return kind == IDENTIFIER || kind == DOT || kind == ARRAY_ACCESS;
}


//
// Given an Ast tree, check whether it is an explicit constructor invocation.
//
inline bool Ast::IsExplicitConstructorInvocation()
{
    return kind == THIS_CALL || kind == SUPER_CALL;
}


//
// Given an Ast tree, check whether or not it is generated.
//
inline bool Ast::IsGenerated()
{
    return generated == 1;
}


//
// This Storage pool is modeled after the Dynamic arrays. The difference is
// that instead of a Next() function we have an alloc(size_t) function. The
// value of the size_t argument represents the size of the object to allocate.
//
// Since all AST nodes for a given parse are allocated from the same storage
// pool, you should never need to directly allocate an AST object (with
// operator new), and you should never need to destroy an AST object (with
// operator delete). To reclaim memory when processing is complete, simply
// delete the underlying storage pool.
//
class StoragePool
{
public:

    typedef void *Cell;

    inline size_t Blksize() { return (1 << log_blksize); }

private:

    Cell **base;
    size_t base_size;
    int top,
        size;

    size_t log_blksize,
           base_increment;

    //
    // Allocate another block of storage for the storage pool
    //
    void AllocateMoreSpace()
    {
        //
        // The variable size always indicates the maximum number of
        // cells that has been allocated for the storage pool.
        // Initially, it is set to 0 to indicate that the pool is empty.
        // The pool of available elements is divided into segments of size
        // 2**log_blksize each. Each segment is pointed to by a slot in
        // the array base.
        //
        // By dividing size by the size of the segment we obtain the
        // index for the next segment in base. If base is full, it is
        // reallocated.
        //
        //
        size_t k = size >> log_blksize; /* which segment? */

        //
        // If the base is overflowed, reallocate it and initialize the new
        // elements to NULL.
        //
        if (k == base_size)
        {
            int old_base_size = base_size;
            Cell **old_base = base;

            base_size += base_increment;
            base = new Cell*[base_size];

            if (old_base != NULL)
            {
                memmove(base, old_base, old_base_size * sizeof(Cell *));
                delete [] old_base;
            }
            memset(&base[old_base_size], 0,
                   (base_size - old_base_size) * sizeof(Cell *));
        }

        //
        // If the slot "k" does not already contain a segment,
        // we allocate a new segment and place its adjusted address in
        // base[k]. The adjustment allows us to index the segment directly,
        // instead of having to perform a subtraction for each reference.
        // See operator[] below.
        //
        if (base[k] == NULL)
        {
            base[k] = new Cell[Blksize()];
            base[k] -= size;
        }

        //
        // Finally, we update SIZE.
        //
        size += Blksize();
    }

public:

    //
    // Constructor of a storage pool
    //
    StoragePool(size_t num_tokens)
    {
        //
        // Make a guess on the size that will be required for the ast
        // based on the number of tokens. The ratio for the bodies is
        // usually 40 to 1. We will double the base to add to account
        // for the headers
        // recall that each cell is a-byte word. So, 10 * 4 = 40
        //
        size_t estimate = num_tokens * 10;

        //
        // Find a block of size 2**log_blksize that is large enough
        // to satisfy our estimate.
        //
        for (log_blksize = 8;
             (((unsigned) 1 << log_blksize) < estimate) && (log_blksize < 31);
             log_blksize++)
            ;

        //
        // If the size of the block found is < 1k, allocate a block of size 1k
        // with one slot to spare, just in case.
        // If the size is less than 16k, then break it up into equal blocks
        // of size 1k;
        // Otherwise, fragment it into pieces of size 16k.
        //
        if (log_blksize < 8)
        {
            base_increment = 1;
            log_blksize = 8;
        }
        else if (log_blksize < 13)
        {
            base_increment = (unsigned) 1 << (log_blksize - 8);
            log_blksize = 8;
        }
        else if (log_blksize < 17)
        {
            base_increment = (unsigned) 1 << (log_blksize - 10);
            log_blksize = 10;
        }
        else
        {
            // assume we won't be allocating more than this many blocks.
            base_increment = (unsigned) 1 << (log_blksize - 12);
            log_blksize = 12;
        }

        //
        // Double the size of the base in order to allocate extra space for
        // the headers and add a little margin for stuff like extra Cast node
        // and computation of static expressions that require cloning.
        //
        base_increment = (base_increment << 1) + 3;

        base_size = 0;
        size = 0;
        top = 0;
        base = NULL;
    }

    //
    // Destructor of a storage pool
    //
    ~StoragePool()
    {
        for (int k = (size >> log_blksize) - 1; k >= 0; k--)
        {
            size -= Blksize();
            base[k] += size;
            delete [] base[k];
        }

        delete [] base;
    }

    //
    // alloc allocates an object of size n in the pool and
    // returns a pointer to it.
    //
    inline void *Alloc(size_t n)
    {
        size_t i = top,
               chunk_size = ((n + sizeof(Cell) - 1) / sizeof(Cell));
        top += chunk_size;
        if (top > size)
        {
            assert(chunk_size <= Blksize() &&
                   "allocating a chunk of storage larger than the block !");

            i = size;
            top = size + chunk_size;
            AllocateMoreSpace();
        }

        return ((void *) &(base[i >> log_blksize] [i]));
    }

    //
    // Return length of the amount of storage that has been allocated so far.
    //
    inline size_t Length() { return top; }

    //
    // This function is used to reset the Storage pool. This action
    // automatically invalidates all objects that had been allocated in the
    // pool. At least, YOU should assume it does!!!
    //
    inline void Reset(const int n = 0)
    {
        if (n < 0 || n > size)
            assert(false);
        top = n;
    }

    //
    // This function frees up all dynamic space that
    // was allocated for this storage pool.
    //
    inline void Destroy()
    {
        for (int k = (size >> log_blksize) - 1; k >= 0; k--)
        {
            size -= Blksize();
            base[k] += size;
            delete [] base[k];
            base[k] = NULL;
        }

        delete [] base;
        base = NULL;
        base_size = 0;

        Reset();
    }

    // ********************************************************************

    inline VariableSymbolArray *NewVariableSymbolArray(unsigned size = 0)
    {
        return new (Alloc(sizeof(VariableSymbolArray)))
            VariableSymbolArray(this, size);
    }

    //
    // Older compilers do not support templatized member methods, hence we
    // have moved this to be a global method (yuck).
    //
    // template <typename T>
    // inline AstArray<T> *NewAstArray(unsigned size = 0)
    // {
    //     return new (Alloc(sizeof(AstArray<T>))) AstArray<T>(this, size);
    // }

    inline AstListNode *NewListNode()
    {
        return new (Alloc(sizeof(AstListNode))) AstListNode();
    }

    inline AstBlock *NewBlock()
    {
        return new (Alloc(sizeof(AstBlock))) AstBlock(this);
    }

    inline AstPrimitiveType *NewPrimitiveType(Ast::Kind kind,
                                              LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstPrimitiveType)))
            AstPrimitiveType(kind, token);
    }

    inline AstArrayType *NewArrayType()
    {
        return new (Alloc(sizeof(AstArrayType))) AstArrayType(this);
    }

    inline AstSimpleName *NewSimpleName(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstSimpleName))) AstSimpleName(token);
    }

    inline AstPackageDeclaration *NewPackageDeclaration()
    {
        return new (Alloc(sizeof(AstPackageDeclaration)))
            AstPackageDeclaration();
    }

    inline AstImportDeclaration *NewImportDeclaration()
    {
        return new (Alloc(sizeof(AstImportDeclaration))) AstImportDeclaration();
    }

    inline AstCompilationUnit *NewCompilationUnit()
    {
        return new (Alloc(sizeof(AstCompilationUnit))) AstCompilationUnit(this);
    }

    inline AstModifier *NewModifier(Ast::Kind kind, LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstModifier))) AstModifier(kind, token);
    }

    inline AstEmptyDeclaration *NewEmptyDeclaration(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstEmptyDeclaration)))
            AstEmptyDeclaration(token);
    }

    inline AstClassBody *NewClassBody()
    {
        return new (Alloc(sizeof(AstClassBody))) AstClassBody(this);
    }

    inline AstClassDeclaration *NewClassDeclaration()
    {
        return new (Alloc(sizeof(AstClassDeclaration)))
            AstClassDeclaration(this);
    }

    inline AstArrayInitializer *NewArrayInitializer()
    {
        return new (Alloc(sizeof(AstArrayInitializer)))
            AstArrayInitializer(this);
    }

    inline AstBrackets *NewBrackets(LexStream::TokenIndex left,
                                    LexStream::TokenIndex right)
    {
        return new (Alloc(sizeof(AstBrackets))) AstBrackets(left, right);
    }

    inline AstVariableDeclaratorId *NewVariableDeclaratorId()
    {
        return new (Alloc(sizeof(AstVariableDeclaratorId)))
            AstVariableDeclaratorId(this);
    }

    inline AstVariableDeclarator *NewVariableDeclarator()
    {
        return new (Alloc(sizeof(AstVariableDeclarator)))
            AstVariableDeclarator();
    }

    inline AstFieldDeclaration *NewFieldDeclaration()
    {
        return new (Alloc(sizeof(AstFieldDeclaration)))
            AstFieldDeclaration(this);
    }

    inline AstFormalParameter *NewFormalParameter()
    {
        return new (Alloc(sizeof(AstFormalParameter))) AstFormalParameter(this);
    }

    inline AstMethodDeclarator *NewMethodDeclarator()
    {
        return new (Alloc(sizeof(AstMethodDeclarator)))
            AstMethodDeclarator(this);
    }

    inline AstMethodDeclaration *NewMethodDeclaration()
    {
        return new (Alloc(sizeof(AstMethodDeclaration)))
            AstMethodDeclaration(this);
    }

    inline AstStaticInitializer *NewStaticInitializer()
    {
        return new (Alloc(sizeof(AstStaticInitializer))) AstStaticInitializer();
    }

    inline AstThisCall *NewThisCall()
    {
        return new (Alloc(sizeof(AstThisCall))) AstThisCall(this);
    }

    inline AstSuperCall *NewSuperCall()
    {
        return new (Alloc(sizeof(AstSuperCall))) AstSuperCall(this);
    }

    inline AstMethodBody *NewMethodBody()
    {
        return new (Alloc(sizeof(AstMethodBody))) AstMethodBody(this);
    }

    inline AstConstructorDeclaration *NewConstructorDeclaration()
    {
        return new (Alloc(sizeof(AstConstructorDeclaration)))
            AstConstructorDeclaration(this);
    }

    inline AstInterfaceDeclaration *NewInterfaceDeclaration()
    {
        return new (Alloc(sizeof(AstInterfaceDeclaration)))
            AstInterfaceDeclaration(this);
    }

    inline AstLocalVariableDeclarationStatement *NewLocalVariableDeclarationStatement()
    {
        return new (Alloc(sizeof(AstLocalVariableDeclarationStatement)))
            AstLocalVariableDeclarationStatement(this);
    }

    inline AstIfStatement *NewIfStatement()
    {
        return new (Alloc(sizeof(AstIfStatement))) AstIfStatement(this);
    }

    inline AstEmptyStatement *NewEmptyStatement(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstEmptyStatement)))
            AstEmptyStatement(this, token);
    }

    inline AstExpressionStatement *NewExpressionStatement()
    {
        return new (Alloc(sizeof(AstExpressionStatement)))
            AstExpressionStatement(this);
    }

    inline AstCaseLabel *NewCaseLabel()
    {
        return new (Alloc(sizeof(AstCaseLabel))) AstCaseLabel();
    }

    inline AstDefaultLabel *NewDefaultLabel()
    {
        return new (Alloc(sizeof(AstDefaultLabel))) AstDefaultLabel();
    }

    inline AstSwitchBlockStatement *NewSwitchBlockStatement()
    {
        return new (Alloc(sizeof(AstSwitchBlockStatement)))
            AstSwitchBlockStatement(this);
    }

    inline AstSwitchStatement *NewSwitchStatement()
    {
        return new (Alloc(sizeof(AstSwitchStatement))) AstSwitchStatement(this);
    }

    inline AstWhileStatement *NewWhileStatement()
    {
        return new (Alloc(sizeof(AstWhileStatement))) AstWhileStatement(this);
    }

    inline AstDoStatement *NewDoStatement()
    {
        return new (Alloc(sizeof(AstDoStatement))) AstDoStatement(this);
    }

    inline AstForStatement *NewForStatement()
    {
        return new (Alloc(sizeof(AstForStatement))) AstForStatement(this);
    }

    inline AstBreakStatement *NewBreakStatement()
    {
        return new (Alloc(sizeof(AstBreakStatement))) AstBreakStatement(this);
    }

    inline AstContinueStatement *NewContinueStatement()
    {
        return new (Alloc(sizeof(AstContinueStatement)))
            AstContinueStatement(this);
    }

    inline AstReturnStatement *NewReturnStatement()
    {
        return new (Alloc(sizeof(AstReturnStatement))) AstReturnStatement(this);
    }

    inline AstThrowStatement *NewThrowStatement()
    {
        return new (Alloc(sizeof(AstThrowStatement))) AstThrowStatement(this);
    }

    inline AstSynchronizedStatement *NewSynchronizedStatement()
    {
        return new (Alloc(sizeof(AstSynchronizedStatement)))
            AstSynchronizedStatement(this);
    }

    inline AstAssertStatement *NewAssertStatement()
    {
        return new (Alloc(sizeof(AstAssertStatement))) AstAssertStatement(this);
    }

    inline AstCatchClause *NewCatchClause()
    {
        return new (Alloc(sizeof(AstCatchClause))) AstCatchClause();
    }

    inline AstFinallyClause *NewFinallyClause()
    {
        return new (Alloc(sizeof(AstFinallyClause))) AstFinallyClause();
    }

    inline AstTryStatement *NewTryStatement()
    {
        return new (Alloc(sizeof(AstTryStatement))) AstTryStatement(this);
    }

    inline AstIntegerLiteral *NewIntegerLiteral(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstIntegerLiteral))) AstIntegerLiteral(token);
    }

    inline AstLongLiteral *NewLongLiteral(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstLongLiteral))) AstLongLiteral(token);
    }

    inline AstFloatLiteral *NewFloatLiteral(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstFloatLiteral))) AstFloatLiteral(token);
    }

    inline AstDoubleLiteral *NewDoubleLiteral(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstDoubleLiteral))) AstDoubleLiteral(token);
    }

    inline AstTrueLiteral *NewTrueLiteral(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstTrueLiteral))) AstTrueLiteral(token);
    }

    inline AstFalseLiteral *NewFalseLiteral(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstFalseLiteral))) AstFalseLiteral(token);
    }

    inline AstStringLiteral *NewStringLiteral(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstStringLiteral))) AstStringLiteral(token);
    }

    inline AstCharacterLiteral *NewCharacterLiteral(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstCharacterLiteral)))
            AstCharacterLiteral(token);
    }

    inline AstNullLiteral *NewNullLiteral(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstNullLiteral))) AstNullLiteral(token);
    }

    inline AstThisExpression *NewThisExpression(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstThisExpression))) AstThisExpression(token);
    }

    inline AstSuperExpression *NewSuperExpression(LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstSuperExpression)))
            AstSuperExpression(token);
    }

    inline AstParenthesizedExpression *NewParenthesizedExpression()
    {
        return new (Alloc(sizeof(AstParenthesizedExpression)))
            AstParenthesizedExpression();
    }

    inline AstTypeExpression *NewTypeExpression(Ast *type)
    {
        return new (Alloc(sizeof(AstTypeExpression))) AstTypeExpression(type);
    }

    inline AstClassInstanceCreationExpression *NewClassInstanceCreationExpression()
    {
        return new (Alloc(sizeof(AstClassInstanceCreationExpression)))
            AstClassInstanceCreationExpression(this);
    }

    inline AstDimExpr *NewDimExpr()
    {
        return new (Alloc(sizeof(AstDimExpr))) AstDimExpr();
    }

    inline AstArrayCreationExpression *NewArrayCreationExpression()
    {
        return new (Alloc(sizeof(AstArrayCreationExpression)))
            AstArrayCreationExpression(this);
    }

    inline AstFieldAccess *NewFieldAccess(AstFieldAccess::FieldAccessTag tag = AstFieldAccess::NONE)
    {
        return new (Alloc(sizeof(AstFieldAccess))) AstFieldAccess(tag);
    }

    inline AstMethodInvocation *NewMethodInvocation()
    {
        return new (Alloc(sizeof(AstMethodInvocation)))
            AstMethodInvocation(this);
    }

    inline AstArrayAccess *NewArrayAccess()
    {
        return new (Alloc(sizeof(AstArrayAccess))) AstArrayAccess();
    }

    inline AstPostUnaryExpression *NewPostUnaryExpression(AstPostUnaryExpression::PostUnaryExpressionTag tag)
    {
        return new (Alloc(sizeof(AstPostUnaryExpression)))
            AstPostUnaryExpression(tag);
    }

    inline AstPreUnaryExpression *NewPreUnaryExpression(AstPreUnaryExpression::PreUnaryExpressionTag tag)
    {
        return new (Alloc(sizeof(AstPreUnaryExpression)))
            AstPreUnaryExpression(tag);
    }

    inline AstCastExpression *NewCastExpression()
    {
        return new (Alloc(sizeof(AstCastExpression))) AstCastExpression(this);
    }

    inline AstBinaryExpression *NewBinaryExpression(AstBinaryExpression::BinaryExpressionTag tag)
    {
        return new (Alloc(sizeof(AstBinaryExpression)))
            AstBinaryExpression(tag);
    }

    inline AstConditionalExpression *NewConditionalExpression()
    {
        return new (Alloc(sizeof(AstConditionalExpression)))
            AstConditionalExpression();
    }

    inline AstAssignmentExpression *NewAssignmentExpression(AstAssignmentExpression::AssignmentExpressionTag tag,
                                                            LexStream::TokenIndex token)
    {
        return new (Alloc(sizeof(AstAssignmentExpression)))
            AstAssignmentExpression(tag, token);
    }

    // *********************************************************************

    //
    // Note that CaseElement nodes are always generated.
    // Since they are not Ast nodes they do not need to
    // be marked.
    //
    inline CaseElement *GenCaseElement()
    {
        return new (Alloc(sizeof(CaseElement))) CaseElement();
    }

    inline AstBlock *GenBlock()
    {
        AstBlock *p = NewBlock();
        p -> generated = true;
        return p;
    }

    inline AstPrimitiveType *GenPrimitiveType(Ast::Kind kind,
                                              LexStream::TokenIndex token)
    {
        AstPrimitiveType *p = NewPrimitiveType(kind, token);
        p -> generated = true;
        return p;
    }

    inline AstArrayType *GenArrayType()
    {
        AstArrayType *p = NewArrayType();
        p -> generated = true;
        return p;
    }

    inline AstSimpleName *GenSimpleName(LexStream::TokenIndex token)
    {
        AstSimpleName *p = NewSimpleName(token);
        p -> generated = true;
        return p;
    }

    inline AstPackageDeclaration *GenPackageDeclaration()
    {
        AstPackageDeclaration *p = NewPackageDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstImportDeclaration *GenImportDeclaration()
    {
        AstImportDeclaration *p = NewImportDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstCompilationUnit *GenCompilationUnit()
    {
        AstCompilationUnit *p = NewCompilationUnit();
        p -> generated = true;
        return p;
    }

    inline AstModifier *GenModifier(Ast::Kind kind, LexStream::TokenIndex token)
    {
        AstModifier *p = NewModifier(kind, token);
        p -> generated = true;
        return p;
    }

    inline AstEmptyDeclaration *GenEmptyDeclaration(LexStream::TokenIndex token)
    {
        AstEmptyDeclaration *p = NewEmptyDeclaration(token);
        p -> generated = true;
        return p;
    }

    inline AstClassBody *GenClassBody()
    {
        AstClassBody *p = NewClassBody();
        p -> generated = true;
        return p;
    }

    inline AstClassDeclaration *GenClassDeclaration()
    {
        AstClassDeclaration *p = NewClassDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstArrayInitializer *GenArrayInitializer()
    {
        AstArrayInitializer *p = NewArrayInitializer();
        p -> generated = true;
        return p;
    }

    inline AstBrackets *GenBrackets(LexStream::TokenIndex left,
                                    LexStream::TokenIndex right)
    {
        AstBrackets *p = NewBrackets(left, right);
        p -> generated = true;
        return p;
    }

    inline AstVariableDeclaratorId *GenVariableDeclaratorId()
    {
        AstVariableDeclaratorId *p = NewVariableDeclaratorId();
        p -> generated = true;
        return p;
    }

    inline AstVariableDeclarator *GenVariableDeclarator()
    {
        AstVariableDeclarator *p = NewVariableDeclarator();
        p -> generated = true;
        return p;
    }

    inline AstFieldDeclaration *GenFieldDeclaration()
    {
        AstFieldDeclaration *p = NewFieldDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstFormalParameter *GenFormalParameter()
    {
        AstFormalParameter *p = NewFormalParameter();
        p -> generated = true;
        return p;
    }

    inline AstMethodDeclarator *GenMethodDeclarator()
    {
        AstMethodDeclarator *p = NewMethodDeclarator();
        p -> generated = true;
        return p;
    }

    inline AstMethodDeclaration *GenMethodDeclaration()
    {
        AstMethodDeclaration *p = NewMethodDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstStaticInitializer *GenStaticInitializer()
    {
        AstStaticInitializer *p = NewStaticInitializer();
        p -> generated = true;
        return p;
    }

    inline AstThisCall *GenThisCall()
    {
        AstThisCall *p = NewThisCall();
        p -> generated = true;
        return p;
    }

    inline AstSuperCall *GenSuperCall()
    {
        AstSuperCall *p = NewSuperCall();
        p -> generated = true;
        return p;
    }

    inline AstMethodBody *GenMethodBody()
    {
        AstMethodBody *p = NewMethodBody();
        p -> generated = true;
        return p;
    }

    inline AstConstructorDeclaration *GenConstructorDeclaration()
    {
        AstConstructorDeclaration *p = NewConstructorDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstInterfaceDeclaration *GenInterfaceDeclaration()
    {
        AstInterfaceDeclaration *p = NewInterfaceDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstLocalVariableDeclarationStatement *GenLocalVariableDeclarationStatement()
    {
        AstLocalVariableDeclarationStatement *p =
            NewLocalVariableDeclarationStatement();
        p -> generated = true;
        return p;
    }

    inline AstIfStatement *GenIfStatement()
    {
        AstIfStatement *p = NewIfStatement();
        p -> generated = true;
        return p;
    }

    inline AstEmptyStatement *GenEmptyStatement(LexStream::TokenIndex token)
    {
        AstEmptyStatement *p = NewEmptyStatement(token);
        p -> generated = true;
        return p;
    }

    inline AstExpressionStatement *GenExpressionStatement()
    {
        AstExpressionStatement *p = NewExpressionStatement();
        p -> generated = true;
        return p;
    }

    inline AstCaseLabel *GenCaseLabel()
    {
        AstCaseLabel *p = NewCaseLabel();
        p -> generated = true;
        return p;
    }

    inline AstDefaultLabel *GenDefaultLabel()
    {
        AstDefaultLabel *p = NewDefaultLabel();
        p -> generated = true;
        return p;
    }

    inline AstSwitchBlockStatement *GenSwitchBlockStatement()
    {
        AstSwitchBlockStatement *p = NewSwitchBlockStatement();
        p -> generated = true;
        return p;
    }

    inline AstSwitchStatement *GenSwitchStatement()
    {
        AstSwitchStatement *p = NewSwitchStatement();
        p -> generated = true;
        return p;
    }

    inline AstWhileStatement *GenWhileStatement()
    {
        AstWhileStatement *p = NewWhileStatement();
        p -> generated = true;
        return p;
    }

    inline AstDoStatement *GenDoStatement()
    {
        AstDoStatement *p = NewDoStatement();
        p -> generated = true;
        return p;
    }

    inline AstForStatement *GenForStatement()
    {
        AstForStatement *p = NewForStatement();
        p -> generated = true;
        return p;
    }

    inline AstBreakStatement *GenBreakStatement()
    {
        AstBreakStatement *p = NewBreakStatement();
        p -> generated = true;
        return p;
    }

    inline AstContinueStatement *GenContinueStatement()
    {
        AstContinueStatement *p = NewContinueStatement();
        p -> generated = true;
        return p;
    }

    inline AstReturnStatement *GenReturnStatement()
    {
        AstReturnStatement *p = NewReturnStatement();
        p -> generated = true;
        return p;
    }

    inline AstThrowStatement *GenThrowStatement()
    {
        AstThrowStatement *p = NewThrowStatement();
        p -> generated = true;
        return p;
    }

    inline AstSynchronizedStatement *GenSynchronizedStatement()
    {
        AstSynchronizedStatement *p = NewSynchronizedStatement();
        p -> generated = true;
        return p;
    }

    inline AstAssertStatement *GenAssertStatement()
    {
        AstAssertStatement *p = NewAssertStatement();
        p -> generated = true;
        return p;
    }

    inline AstCatchClause *GenCatchClause()
    {
        AstCatchClause *p = NewCatchClause();
        p -> generated = true;
        return p;
    }

    inline AstFinallyClause *GenFinallyClause()
    {
        AstFinallyClause *p = NewFinallyClause();
        p -> generated = true;
        return p;
    }

    inline AstTryStatement *GenTryStatement()
    {
        AstTryStatement *p = NewTryStatement();
        p -> generated = true;
        return p;
    }

    inline AstIntegerLiteral *GenIntegerLiteral(LexStream::TokenIndex token)
    {
        AstIntegerLiteral *p = NewIntegerLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstLongLiteral *GenLongLiteral(LexStream::TokenIndex token)
    {
        AstLongLiteral *p = NewLongLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstFloatLiteral *GenFloatLiteral(LexStream::TokenIndex token)
    {
        AstFloatLiteral *p = NewFloatLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstDoubleLiteral *GenDoubleLiteral(LexStream::TokenIndex token)
    {
        AstDoubleLiteral *p = NewDoubleLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstTrueLiteral *GenTrueLiteral(LexStream::TokenIndex token)
    {
        AstTrueLiteral *p = NewTrueLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstFalseLiteral *GenFalseLiteral(LexStream::TokenIndex token)
    {
        AstFalseLiteral *p = NewFalseLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstStringLiteral *GenStringLiteral(LexStream::TokenIndex token)
    {
        AstStringLiteral *p = NewStringLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstCharacterLiteral *GenCharacterLiteral(LexStream::TokenIndex token)
    {
        AstCharacterLiteral *p = NewCharacterLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstNullLiteral *GenNullLiteral(LexStream::TokenIndex token)
    {
        AstNullLiteral *p = NewNullLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstThisExpression *GenThisExpression(LexStream::TokenIndex token)
    {
        AstThisExpression *p = NewThisExpression(token);
        p -> generated = true;
        return p;
    }

    inline AstSuperExpression *GenSuperExpression(LexStream::TokenIndex token)
    {
        AstSuperExpression *p = NewSuperExpression(token);
        p -> generated = true;
        return p;
    }

    inline AstParenthesizedExpression *GenParenthesizedExpression()
    {
        AstParenthesizedExpression *p = NewParenthesizedExpression();
        p -> generated = true;
        return p;
    }

    inline AstTypeExpression *GenTypeExpression(Ast *type)
    {
        AstTypeExpression *p = NewTypeExpression(type);
        p -> generated = true;
        return p;
    }

    inline AstClassInstanceCreationExpression *GenClassInstanceCreationExpression()
    {
        AstClassInstanceCreationExpression *p =
            NewClassInstanceCreationExpression();
        p -> generated = true;
        return p;
    }

    inline AstDimExpr *GenDimExpr()
    {
        AstDimExpr *p = NewDimExpr();
        p -> generated = true;
        return p;
    }

    inline AstArrayCreationExpression *GenArrayCreationExpression()
    {
        AstArrayCreationExpression *p = NewArrayCreationExpression();
        p -> generated = true;
        return p;
    }

    inline AstFieldAccess *GenFieldAccess(AstFieldAccess::FieldAccessTag tag = AstFieldAccess::NONE)
    {
        AstFieldAccess *p = NewFieldAccess(tag);
        p -> generated = true;
        return p;
    }

    inline AstMethodInvocation *GenMethodInvocation()
    {
        AstMethodInvocation *p = NewMethodInvocation();
        p -> generated = true;
        return p;
    }

    inline AstArrayAccess *GenArrayAccess()
    {
        AstArrayAccess *p = NewArrayAccess();
        p -> generated = true;
        return p;
    }

    inline AstPostUnaryExpression *GenPostUnaryExpression(AstPostUnaryExpression::PostUnaryExpressionTag tag)
    {
        AstPostUnaryExpression *p = NewPostUnaryExpression(tag);
        p -> generated = true;
        return p;
    }

    inline AstPreUnaryExpression *GenPreUnaryExpression(AstPreUnaryExpression::PreUnaryExpressionTag tag)
    {
        AstPreUnaryExpression *p = NewPreUnaryExpression(tag);
        p -> generated = true;
        return p;
    }

    inline AstCastExpression *GenCastExpression()
    {
        AstCastExpression *p = NewCastExpression();
        p -> generated = true;
        return p;
    }

    inline AstBinaryExpression *GenBinaryExpression(AstBinaryExpression::BinaryExpressionTag tag)
    {
        AstBinaryExpression *p = NewBinaryExpression(tag);
        p -> generated = true;
        return p;
    }

    inline AstConditionalExpression *GenConditionalExpression()
    {
        AstConditionalExpression *p = NewConditionalExpression();
        p -> generated = true;
        return p;
    }

    inline AstAssignmentExpression *GenAssignmentExpression(AstAssignmentExpression::AssignmentExpressionTag tag, LexStream::TokenIndex token)
    {
        AstAssignmentExpression *p = NewAssignmentExpression(tag, token);
        p -> generated = true;
        return p;
    }

    // *********************************************************************

    //
    // Return the total size of temporary space allocated.
    //
    size_t SpaceAllocated(void)
    {
        return ((base_size * sizeof(Cell **)) + (size * sizeof(Cell)));
    }

    //
    // Return the total size of temporary space used.
    //
    size_t SpaceUsed(void)
    {
        return (((size >> log_blksize) * sizeof(Cell **)) + (top * sizeof(Cell)));
    }
};


//
// Older compilers do not support templatized member methods. If it were not
// for that fact, this method should be a member of StoragePool.
//
template <typename T>
inline AstArray<T> *NewAstArray(StoragePool *pool, unsigned size = 0)
{
    return new (pool -> Alloc(sizeof(AstArray<T>))) AstArray<T>(pool, size);
}

//
// Cast conversions for Ast
//

inline AstStatement *Ast::StatementCast()
{
    return DYNAMIC_CAST<AstStatement *>
        (class_tag == STATEMENT ? this : NULL);
}

inline AstExpression *Ast::ExpressionCast()
{
    return DYNAMIC_CAST<AstExpression *>
        (class_tag == EXPRESSION ? this : NULL);
}

inline AstPrimitiveType *Ast::PrimitiveTypeCast()
{
    return DYNAMIC_CAST<AstPrimitiveType *>
        (class_tag == PRIMITIVE_TYPE ? this : NULL);
}

inline AstModifier *Ast::ModifierCast()
{
    return DYNAMIC_CAST<AstModifier *>
        (class_tag == MODIFIER ? this : NULL);
}

inline AstFieldDeclaration *Ast::StaticFieldCast()
{
    return DYNAMIC_CAST<AstFieldDeclaration *>
        (class_tag == STATIC_FIELD ? this : NULL);
}

inline AstClassBody *Ast::UnparsedClassBodyCast()
{
    return DYNAMIC_CAST<AstClassBody *>
        (class_tag == UNPARSED ? this : NULL);
}

inline AstInterfaceDeclaration *Ast::UnparsedInterfaceBodyCast()
{
    return DYNAMIC_CAST<AstInterfaceDeclaration *>
        (class_tag == UNPARSED ? this : NULL);
}


//
// These cast functions are used for classes that represent exactly
// one kind of node.
//

inline AstListNode *Ast::ListNodeCast()
{
    return DYNAMIC_CAST<AstListNode *>
        (kind == LIST_NODE ? this : NULL);
}

inline AstArrayType *Ast::ArrayTypeCast()
{
    return DYNAMIC_CAST<AstArrayType *>
        (kind == ARRAY ? this : NULL);
}

inline AstSimpleName *Ast::SimpleNameCast()
{
    return DYNAMIC_CAST<AstSimpleName *>
        (kind == IDENTIFIER ? this : NULL);
}

inline AstPackageDeclaration *Ast::PackageDeclarationCast()
{
    return DYNAMIC_CAST<AstPackageDeclaration *>
        (kind == PACKAGE ? this : NULL);
}

inline AstImportDeclaration *Ast::ImportDeclarationCast()
{
    return DYNAMIC_CAST<AstImportDeclaration *>
        (kind == IMPORT ? this : NULL);
}

inline AstCompilationUnit *Ast::CompilationUnitCast()
{
    return DYNAMIC_CAST<AstCompilationUnit *>
        (kind == COMPILATION || kind == BAD_COMPILATION
            || kind == EMPTY_COMPILATION ? this : NULL);
}

inline AstCompilationUnit *Ast::BadCompilationUnitCast()
{
    return DYNAMIC_CAST<AstCompilationUnit *>
        (kind == BAD_COMPILATION ? this : NULL);
}

inline AstCompilationUnit *Ast::EmptyCompilationUnitCast()
{
    return DYNAMIC_CAST<AstCompilationUnit *>
        (kind == EMPTY_COMPILATION ? this : NULL);
}

inline AstEmptyDeclaration *Ast::EmptyDeclarationCast()
{
    return DYNAMIC_CAST<AstEmptyDeclaration *>
        (kind == EMPTY_DECLARATION ? this : NULL);
}

inline AstClassDeclaration *Ast::ClassDeclarationCast()
{
    return DYNAMIC_CAST<AstClassDeclaration *>
        (kind == CLASS ? this : NULL);
}

inline AstArrayInitializer *Ast::ArrayInitializerCast()
{
    return DYNAMIC_CAST<AstArrayInitializer *>
        (kind == ARRAY_INITIALIZER ? this : NULL);
}

inline AstBrackets *Ast::BracketsCast()
{
    return DYNAMIC_CAST<AstBrackets *>
        (kind == BRACKETS ? this : NULL);
}

inline AstVariableDeclaratorId *Ast::VariableDeclaratorIdCast()
{
    return DYNAMIC_CAST<AstVariableDeclaratorId *>
        (kind == VARIABLE_DECLARATOR_NAME ? this : NULL);
}

inline AstVariableDeclarator *Ast::VariableDeclaratorCast()
{
    return DYNAMIC_CAST<AstVariableDeclarator *>
        (kind == VARIABLE_DECLARATOR ? this : NULL);
}

inline AstFieldDeclaration *Ast::FieldDeclarationCast()
{
    return DYNAMIC_CAST<AstFieldDeclaration *>
        (kind == FIELD ? this : NULL);
}

inline AstFormalParameter *Ast::FormalParameterCast()
{
    return DYNAMIC_CAST<AstFormalParameter *>
        (kind == PARAMETER ? this : NULL);
}

inline AstMethodDeclarator *Ast::MethodDeclaratorCast()
{
    return DYNAMIC_CAST<AstMethodDeclarator *>
        (kind == METHOD_DECLARATOR ? this : NULL);
}

inline AstMethodDeclaration *Ast::MethodDeclarationCast()
{
    return DYNAMIC_CAST<AstMethodDeclaration *>
        (kind == METHOD ? this : NULL);
}

inline AstStaticInitializer *Ast::StaticInitializerCast()
{
    return DYNAMIC_CAST<AstStaticInitializer *>
        (kind == STATIC_INITIALIZER ? this : NULL);
}

inline AstThisCall *Ast::ThisCallCast()
{
    return DYNAMIC_CAST<AstThisCall *>
        (kind == THIS_CALL ? this : NULL);
}

inline AstSuperCall *Ast::SuperCallCast()
{
    return DYNAMIC_CAST<AstSuperCall *>
        (kind == SUPER_CALL ? this : NULL);
}

inline AstMethodBody *Ast::MethodBodyCast()
{
    return DYNAMIC_CAST<AstMethodBody *>
        (kind == METHOD_BODY ? this : NULL);
}

inline AstConstructorDeclaration *Ast::ConstructorDeclarationCast()
{
    return DYNAMIC_CAST<AstConstructorDeclaration *>
        (kind == CONSTRUCTOR ? this : NULL);
}

inline AstInterfaceDeclaration *Ast::InterfaceDeclarationCast()
{
    return DYNAMIC_CAST<AstInterfaceDeclaration *>
        (kind == INTERFACE ? this : NULL);
}

inline AstBlock *Ast::BlockCast()
{
    return DYNAMIC_CAST<AstBlock *>
        (kind == BLOCK || kind == METHOD_BODY || kind == SWITCH_BLOCK
         ? this : NULL);
}

inline AstLocalVariableDeclarationStatement *Ast::LocalVariableDeclarationStatementCast()
{
    return DYNAMIC_CAST<AstLocalVariableDeclarationStatement *>
        (kind == LOCAL_VARIABLE_DECLARATION ? this : NULL);
}

inline AstIfStatement *Ast::IfStatementCast()
{
    return DYNAMIC_CAST<AstIfStatement *>
        (kind == IF ? this : NULL);
}

inline AstEmptyStatement *Ast::EmptyStatementCast()
{
    return DYNAMIC_CAST<AstEmptyStatement *>
        (kind == EMPTY_STATEMENT ? this : NULL);
}

inline AstExpressionStatement *Ast::ExpressionStatementCast()
{
    return DYNAMIC_CAST<AstExpressionStatement *>
        (kind == EXPRESSION_STATEMENT ? this : NULL);
}

inline AstCaseLabel *Ast::CaseLabelCast()
{
    return DYNAMIC_CAST<AstCaseLabel *>
        (kind == CASE ? this : NULL);
}

inline AstDefaultLabel *Ast::DefaultLabelCast()
{
    return DYNAMIC_CAST<AstDefaultLabel *>
        (kind == DEFAULT ? this : NULL);
}

inline AstSwitchBlockStatement *Ast::SwitchBlockStatementCast()
{
    return DYNAMIC_CAST<AstSwitchBlockStatement *>
        (kind == SWITCH_BLOCK ? this : NULL);
}

inline AstSwitchStatement *Ast::SwitchStatementCast()
{
    return DYNAMIC_CAST<AstSwitchStatement *>
        (kind == SWITCH ? this : NULL);
}

inline AstWhileStatement *Ast::WhileStatementCast()
{
    return DYNAMIC_CAST<AstWhileStatement *>
        (kind == WHILE ? this : NULL);
}

inline AstDoStatement *Ast::DoStatementCast()
{
    return DYNAMIC_CAST<AstDoStatement *>
        (kind == DO ? this : NULL);
}

inline AstForStatement *Ast::ForStatementCast()
{
    return DYNAMIC_CAST<AstForStatement *>
        (kind == FOR ? this : NULL);
}

inline AstBreakStatement *Ast::BreakStatementCast()
{
    return DYNAMIC_CAST<AstBreakStatement *>
        (kind == BREAK ? this : NULL);
}

inline AstContinueStatement *Ast::ContinueStatementCast()
{
    return DYNAMIC_CAST<AstContinueStatement *>
        (kind == CONTINUE ? this : NULL);
}

inline AstReturnStatement *Ast::ReturnStatementCast()
{
    return DYNAMIC_CAST<AstReturnStatement *>
        (kind == RETURN ? this : NULL);
}

inline AstThrowStatement *Ast::ThrowStatementCast()
{
    return DYNAMIC_CAST<AstThrowStatement *>
        (kind == THROW ? this : NULL);
}

inline AstSynchronizedStatement *Ast::SynchronizedStatementCast()
{
    return DYNAMIC_CAST<AstSynchronizedStatement *>
        (kind == SYNCHRONIZED_STATEMENT ? this : NULL);
}

inline AstAssertStatement *Ast::AssertStatementCast()
{
    return DYNAMIC_CAST<AstAssertStatement *>
        (kind == ASSERT ? this : NULL);
}

inline AstCatchClause *Ast::CatchClauseCast()
{
    return DYNAMIC_CAST<AstCatchClause *>
        (kind == CATCH ? this : NULL);
}

inline AstFinallyClause *Ast::FinallyClauseCast()
{
    return DYNAMIC_CAST<AstFinallyClause *>
        (kind == FINALLY ? this : NULL);
}

inline AstTryStatement *Ast::TryStatementCast()
{
    return DYNAMIC_CAST<AstTryStatement *>
        (kind == TRY ? this : NULL);
}

inline AstIntegerLiteral *Ast::IntegerLiteralCast()
{
    return DYNAMIC_CAST<AstIntegerLiteral *>
        (kind == INTEGER_LITERAL ? this : NULL);
}

inline AstLongLiteral *Ast::LongLiteralCast()
{
    return DYNAMIC_CAST<AstLongLiteral *>
        (kind == LONG_LITERAL ? this : NULL);
}

inline AstFloatLiteral *Ast::FloatLiteralCast()
{
    return DYNAMIC_CAST<AstFloatLiteral *>
        (kind == FLOAT_LITERAL ? this : NULL);
}

inline AstDoubleLiteral *Ast::DoubleLiteralCast()
{
    return DYNAMIC_CAST<AstDoubleLiteral *>
        (kind == DOUBLE_LITERAL ? this : NULL);
}

inline AstTrueLiteral *Ast::TrueLiteralCast()
{
    return DYNAMIC_CAST<AstTrueLiteral *>
        (kind == TRUE_LITERAL ? this : NULL);
}

inline AstFalseLiteral *Ast::FalseLiteralCast()
{
    return DYNAMIC_CAST<AstFalseLiteral *>
        (kind == FALSE_LITERAL ? this : NULL);
}

inline AstStringLiteral *Ast::StringLiteralCast()
{
    return DYNAMIC_CAST<AstStringLiteral *>
        (kind == STRING_LITERAL ? this : NULL);
}

inline AstCharacterLiteral *Ast::CharacterLiteralCast()
{
    return DYNAMIC_CAST<AstCharacterLiteral *>
        (kind == CHARACTER_LITERAL ? this : NULL);
}

inline AstNullLiteral *Ast::NullLiteralCast()
{
    return DYNAMIC_CAST<AstNullLiteral *>
        (kind == NULL_LITERAL ? this : NULL);
}

inline AstThisExpression *Ast::ThisExpressionCast()
{
    return DYNAMIC_CAST<AstThisExpression *>
        (kind == THIS_EXPRESSION ? this : NULL);
}

inline AstSuperExpression *Ast::SuperExpressionCast()
{
    return DYNAMIC_CAST<AstSuperExpression *>
        (kind == SUPER_EXPRESSION ? this : NULL);
}

inline AstParenthesizedExpression *Ast::ParenthesizedExpressionCast()
{
    return DYNAMIC_CAST<AstParenthesizedExpression *>
        (kind == PARENTHESIZED_EXPRESSION ? this : NULL);
}

inline AstClassInstanceCreationExpression *Ast::ClassInstanceCreationExpressionCast()
{
    return DYNAMIC_CAST<AstClassInstanceCreationExpression *>
        (kind == CLASS_CREATION ? this : NULL);
}

inline AstDimExpr *Ast::DimExprCast()
{
    return DYNAMIC_CAST<AstDimExpr *>
        (kind == DIM ? this : NULL);
}

inline AstArrayCreationExpression *Ast::ArrayCreationExpressionCast()
{
    return DYNAMIC_CAST<AstArrayCreationExpression *>
        (kind == ARRAY_CREATION ? this : NULL);
}

inline AstFieldAccess *Ast::FieldAccessCast()
{
    return DYNAMIC_CAST<AstFieldAccess *>
        (kind == DOT ? this : NULL);
}

inline AstMethodInvocation *Ast::MethodInvocationCast()
{
    return DYNAMIC_CAST<AstMethodInvocation *>
        (kind == CALL ? this : NULL);
}

inline AstArrayAccess *Ast::ArrayAccessCast()
{
    return DYNAMIC_CAST<AstArrayAccess *>
        (kind == ARRAY_ACCESS ? this : NULL);
}

inline AstPostUnaryExpression *Ast::PostUnaryExpressionCast()
{
    return DYNAMIC_CAST<AstPostUnaryExpression *>
        (kind == POST_UNARY ? this : NULL);
}

inline AstPreUnaryExpression *Ast::PreUnaryExpressionCast()
{
    return DYNAMIC_CAST<AstPreUnaryExpression *>
        (kind == PRE_UNARY ? this : NULL);
}

inline AstCastExpression *Ast::CastExpressionCast()
{
    return DYNAMIC_CAST<AstCastExpression *>
        (kind == CAST ? this : NULL);
}

inline AstBinaryExpression *Ast::BinaryExpressionCast()
{
    return DYNAMIC_CAST<AstBinaryExpression *>
        (kind == BINARY ? this : NULL);
}

inline AstTypeExpression *Ast::TypeExpressionCast()
{
    return DYNAMIC_CAST<AstTypeExpression *>
        (kind == TYPE ? this : NULL);
}

inline AstConditionalExpression *Ast::ConditionalExpressionCast()
{
    return DYNAMIC_CAST<AstConditionalExpression *>
        (kind == CONDITIONAL ? this : NULL);
}

inline AstAssignmentExpression *Ast::AssignmentExpressionCast()
{
    return DYNAMIC_CAST<AstAssignmentExpression *>
        (kind == ASSIGNMENT ? this : NULL);
}

inline void AstClassBody::AllocateInstanceVariables(int estimate)
{
    if (! instance_variables)
        instance_variables =
            NewAstArray<AstFieldDeclaration *> (pool, estimate);
}

inline void AstClassBody::AddInstanceVariable(AstFieldDeclaration *field_declaration)
{
    if (! instance_variables)
        AllocateInstanceVariables(1);
    instance_variables -> Next() = field_declaration;
}

inline void AstClassBody::AllocateClassVariables(int estimate)
{
    if (! class_variables)
        class_variables =
            NewAstArray<AstFieldDeclaration *> (pool, estimate);
}

inline void AstClassBody::AddClassVariable(AstFieldDeclaration *field_declaration)
{
    if (! class_variables)
        AllocateClassVariables(1);
    class_variables -> Next() = field_declaration;
}

inline void AstClassBody::AllocateMethods(int estimate)
{
    if (! methods)
        methods =
            NewAstArray<AstMethodDeclaration *> (pool, estimate);
}

inline void AstClassBody::AddMethod(AstMethodDeclaration *method_declaration)
{
    if (! methods)
        AllocateMethods(1);
    methods -> Next() = method_declaration;
}

inline void AstClassBody::AllocateInstanceInitializers(int estimate)
{
    if (! instance_initializers)
        instance_initializers =
            NewAstArray<AstMethodBody *> (pool, estimate);
}

inline void AstClassBody::AddInstanceInitializer(AstMethodBody *block)
{
    if (! instance_initializers)
        AllocateInstanceInitializers(1);
    instance_initializers -> Next() = block;
}

inline void AstClassBody::AllocateNestedInterfaces(int estimate)
{
    if (! inner_interfaces)
        inner_interfaces =
            NewAstArray<AstInterfaceDeclaration *> (pool, estimate);
}

inline void AstClassBody::AddNestedInterface(AstInterfaceDeclaration *interface_declaration)
{
    if (! inner_interfaces)
        AllocateNestedInterfaces(1);
    inner_interfaces -> Next() = interface_declaration;
}

inline void AstClassBody::AllocateNestedClasses(int estimate)
{
    if (! inner_classes)
        inner_classes =
            NewAstArray<AstClassDeclaration *> (pool, estimate);
}

inline void AstClassBody::AddNestedClass(AstClassDeclaration *class_declaration)
{
    if (! inner_classes)
        AllocateNestedClasses(1);
    inner_classes -> Next() = class_declaration;
}

inline void AstClassBody::AllocateStaticInitializers(int estimate)
{
    if (! static_initializers)
        static_initializers =
            NewAstArray<AstStaticInitializer *> (pool, estimate);
}

inline void AstClassBody::AddStaticInitializer(AstStaticInitializer *static_initializer)
{
    if (! static_initializers)
        AllocateStaticInitializers(1);
    static_initializers -> Next() = static_initializer;
}

inline void AstClassBody::AllocateConstructors(int estimate)
{
    if (! constructors)
        constructors =
            NewAstArray<AstConstructorDeclaration *> (pool, estimate);
}

inline void AstClassBody::AddConstructor(AstConstructorDeclaration *constructor_declaration)
{
    if (! constructors)
        AllocateConstructors(1);
    constructors -> Next() = constructor_declaration;
}

inline void AstClassBody::AllocateEmptyDeclarations(int estimate)
{
    if (! empty_declarations)
        empty_declarations =
            NewAstArray<AstEmptyDeclaration *> (pool, estimate);
}

inline void AstClassBody::AddEmptyDeclaration(AstEmptyDeclaration *empty_declaration)
{
    if (! empty_declarations)
        AllocateEmptyDeclarations(1);
    empty_declarations -> Next() = empty_declaration;
}

inline void AstInterfaceDeclaration::AllocateNestedInterfaces(int estimate)
{
    if (! inner_interfaces)
        inner_interfaces =
            NewAstArray<AstInterfaceDeclaration *> (pool, estimate);
}

inline void AstInterfaceDeclaration::AddNestedInterface(AstInterfaceDeclaration *interface_declaration)
{
    if (! inner_interfaces)
        AllocateNestedInterfaces(1);
    inner_interfaces -> Next() = interface_declaration;
}

inline void AstInterfaceDeclaration::AllocateNestedClasses(int estimate)
{
    if (! inner_classes)
        inner_classes =
            NewAstArray<AstClassDeclaration *> (pool, estimate);
}

inline void AstInterfaceDeclaration::AddNestedClass(AstClassDeclaration *class_declaration)
{
    if (! inner_classes)
        AllocateNestedClasses(1);
    inner_classes -> Next() = class_declaration;
}

inline void AstInterfaceDeclaration::AllocateMethods(int estimate)
{
    if (! methods)
        methods =
            NewAstArray<AstMethodDeclaration *> (pool, estimate);
}

inline void AstInterfaceDeclaration::AddMethod(AstMethodDeclaration *method_declaration)
{
    if (! methods)
        AllocateMethods(1);
    methods -> Next() = method_declaration;
}

inline void AstInterfaceDeclaration::AllocateClassVariables(int estimate)
{
    if (! class_variables)
        class_variables =
            NewAstArray<AstFieldDeclaration *> (pool, estimate);
}

inline void AstInterfaceDeclaration::AddClassVariable(AstFieldDeclaration *field_declaration)
{
    if (! class_variables)
        AllocateClassVariables(1);
    class_variables -> Next() = field_declaration;
}

inline void AstInterfaceDeclaration::AllocateEmptyDeclarations(int estimate)
{
    if (! empty_declarations)
        empty_declarations =
            NewAstArray<AstEmptyDeclaration *> (pool, estimate);
}

inline void AstInterfaceDeclaration::AddEmptyDeclaration(AstEmptyDeclaration *empty_declaration)
{
    if (! empty_declarations)
        AllocateEmptyDeclarations(1);
    empty_declarations -> Next() = empty_declaration;
}

inline void AstClassDeclaration::AllocateClassModifiers(int estimate)
{
    if (! class_modifiers)
        class_modifiers =
            NewAstArray<AstModifier *> (pool, estimate);
}

inline void AstClassDeclaration::AddClassModifier(AstModifier *class_modifier)
{
    if (! class_modifiers)
        AllocateClassModifiers(4); // there are only 10 modifiers.
    class_modifiers -> Next() = class_modifier;
}

inline void AstFieldDeclaration::AllocateVariableModifiers(int estimate)
{
    if (! variable_modifiers)
        variable_modifiers =
            NewAstArray<AstModifier *> (pool, estimate);
}

inline void AstFieldDeclaration::AddVariableModifier(AstModifier *variable_modifier)
{
    if (! variable_modifiers)
        AllocateVariableModifiers(4); // there are only 10 modifiers.
    variable_modifiers -> Next() = variable_modifier;
}

inline void AstFormalParameter::AllocateParameterModifiers(int estimate)
{
    if (! parameter_modifiers)
        parameter_modifiers =
            NewAstArray<AstModifier *> (pool, estimate);
}

inline void AstFormalParameter::AddParameterModifier(AstModifier *parameter_modifier)
{
    if (! parameter_modifiers)
        AllocateParameterModifiers(4); // there are only 10 modifiers.
    parameter_modifiers -> Next() = parameter_modifier;
}

inline void AstMethodDeclaration::AllocateMethodModifiers(int estimate)
{
    if (! method_modifiers)
        method_modifiers =
            NewAstArray<AstModifier *> (pool, estimate);
}

inline void AstMethodDeclaration::AddMethodModifier(AstModifier *method_modifier)
{
    if (! method_modifiers)
        AllocateMethodModifiers(4); // there are only 10 modifiers.
    method_modifiers -> Next() = method_modifier;
}

inline void AstConstructorDeclaration::AllocateConstructorModifiers(int estimate)
{
    if (! constructor_modifiers)
        constructor_modifiers =
            NewAstArray<AstModifier *> (pool, estimate);
}

inline void AstConstructorDeclaration::AddConstructorModifier(AstModifier *constructor_modifier)
{
    if (! constructor_modifiers)
        AllocateConstructorModifiers(4); // there are only 10 modifiers.
    constructor_modifiers -> Next() = constructor_modifier;
}

inline void AstInterfaceDeclaration::AllocateInterfaceModifiers(int estimate)
{
    if (! interface_modifiers)
        interface_modifiers =
            NewAstArray<AstModifier *> (pool, estimate);
}

inline void AstInterfaceDeclaration::AddInterfaceModifier(AstModifier *interface_modifier)
{
    if (! interface_modifiers)
        AllocateInterfaceModifiers(4); // there are only 10 modifiers.
    interface_modifiers -> Next() = interface_modifier;
}

inline void AstLocalVariableDeclarationStatement::AllocateLocalModifiers(int estimate)
{
    if (! local_modifiers)
        local_modifiers =
            NewAstArray<AstModifier *> (pool, estimate);
}

inline void AstLocalVariableDeclarationStatement::AddLocalModifier(AstModifier *local_modifier)
{
    if (! local_modifiers)
        AllocateLocalModifiers(4); // there are only 10 modifiers.
    local_modifiers -> Next() = local_modifier;
}

inline void AstBlock::AllocateBlockStatements(int estimate)
{
    if (! block_statements)
        block_statements =
            NewAstArray<AstStatement *> (pool, estimate);
}

inline void AstBlock::AddStatement(AstStatement *statement)
{
    if (! block_statements)
        AllocateBlockStatements();
    block_statements -> Next() = statement;
}

inline void AstBlock::AllocateLocallyDefinedVariables(int estimate)
{
    if (! locally_defined_variables)
        locally_defined_variables = pool -> NewVariableSymbolArray(estimate);
}

inline void AstBlock::AddLocallyDefinedVariable(VariableSymbol *variable_symbol)
{
    if (! locally_defined_variables)
        AllocateLocallyDefinedVariables();
    locally_defined_variables -> Next() = variable_symbol;
}

inline void AstStatement::AllocateDefinedVariables(int estimate)
{
    if (! defined_variables)
        defined_variables = pool -> NewVariableSymbolArray(estimate);
}

inline void AstStatement::AddDefinedVariable(VariableSymbol *variable_symbol)
{
    if (! defined_variables)
        AllocateDefinedVariables();
    defined_variables -> Next() = variable_symbol;
}

inline void AstSwitchBlockStatement::AllocateSwitchLabels(int estimate)
{
    if (! switch_labels)
        switch_labels = NewAstArray<Ast *> (pool, estimate);
}

inline void AstSwitchBlockStatement::AddSwitchLabel(Ast *case_or_default_label)
{
    assert(case_or_default_label -> kind == CASE ||
           case_or_default_label -> kind == DEFAULT);
    if (! switch_labels)
        AllocateSwitchLabels();
    switch_labels -> Next() = case_or_default_label;
}

inline void AstSwitchStatement::AllocateCases(int estimate)
{
    if (! cases)
        cases = NewAstArray<CaseElement *> (pool, estimate);
}

inline void AstSwitchStatement::AddCase(CaseElement *case_element)
{
    if (! cases)
        AllocateCases();
    cases -> Next() = case_element;
}

inline void AstVariableDeclaratorId::AllocateBrackets(int estimate)
{
    if (! brackets)
        brackets =
            NewAstArray<AstBrackets *> (pool, estimate);
}

inline void AstVariableDeclaratorId::AddBrackets(AstBrackets *bracket)
{
    if (! brackets)
        AllocateBrackets();
    brackets -> Next() = bracket;
}

inline void AstArrayType::AllocateBrackets(int estimate)
{
    if (! brackets)
        brackets =
            NewAstArray<AstBrackets *> (pool, estimate);
}

inline void AstArrayType::AddBrackets(AstBrackets *bracket)
{
    if (! brackets)
        AllocateBrackets();
    brackets -> Next() = bracket;
}

inline void AstMethodDeclarator::AllocateBrackets(int estimate)
{
    if (! brackets)
        brackets =
            NewAstArray<AstBrackets *> (pool, estimate);
}

inline void AstMethodDeclarator::AddBrackets(AstBrackets *bracket)
{
    if (! brackets)
        AllocateBrackets();
    brackets -> Next() = bracket;
}

inline void AstArrayCreationExpression::AllocateBrackets(int estimate)
{
    if (! brackets)
        brackets =
            NewAstArray<AstBrackets *> (pool, estimate);
}

inline void AstArrayCreationExpression::AddBrackets(AstBrackets *bracket)
{
    if (! brackets)
        AllocateBrackets();
    brackets -> Next() = bracket;
}

inline void AstCastExpression::AllocateBrackets(int estimate)
{
    if (! brackets)
        brackets =
            NewAstArray<AstBrackets *> (pool, estimate);
}

inline void AstCastExpression::AddBrackets(AstBrackets *bracket)
{
    if (! brackets)
        AllocateBrackets();
    brackets -> Next() = bracket;
}

inline void AstArrayCreationExpression::AllocateDimExprs(int estimate)
{
    if (! dim_exprs)
        dim_exprs =
            NewAstArray<AstDimExpr *> (pool, estimate);
}

inline void AstArrayCreationExpression::AddDimExpr(AstDimExpr *dim_expr)
{
    if (! dim_exprs)
        // will not be executed, as assume dim_exprs has already been allocated
        AllocateDimExprs();
    dim_exprs -> Next() = dim_expr;
}

inline void AstThisCall::AllocateArguments(int estimate)
{
    if (! arguments)
        arguments =
            NewAstArray<AstExpression *> (pool, estimate);
}

inline void AstThisCall::AddArgument(AstExpression *argument)
{
    if (! arguments)
        // will not be executed, as assume arguments has already been allocated
        AllocateArguments();
    arguments -> Next() = argument;
}

inline void AstSuperCall::AllocateArguments(int estimate)
{
    if (! arguments)
        arguments =
            NewAstArray<AstExpression *> (pool, estimate);
}

inline void AstSuperCall::AddArgument(AstExpression *argument)
{
    if (! arguments)
        // will not be executed, as we assume arguments is already allocated
        AllocateArguments();
    arguments -> Next() = argument;
}

inline void AstClassInstanceCreationExpression::AllocateArguments(int estimate)
{
    if (! arguments)
        arguments =
            NewAstArray<AstExpression *> (pool, estimate);
}

inline void AstClassInstanceCreationExpression::AddArgument(AstExpression *argument)
{
    if (! arguments)
    // will not be executed, as we assume arguments has already been allocated
        AllocateArguments();
    arguments -> Next() = argument;
}

inline void AstMethodInvocation::AllocateArguments(int estimate)
{
    if (! arguments)
        arguments =
            NewAstArray<AstExpression *> (pool, estimate);
}

inline void AstMethodInvocation::AddArgument(AstExpression *argument)
{
    if (! arguments)
        // will not be executed, as we assume arguments is already allocated
        AllocateArguments();
    arguments -> Next() = argument;
}

inline void AstSuperCall::AllocateLocalArguments(int estimate)
{
    if (! local_arguments_opt)
        local_arguments_opt =
            NewAstArray<AstExpression *> (pool, estimate);
}

inline void AstSuperCall::AddLocalArgument(AstExpression *argument)
{
    if (! local_arguments_opt)
        AllocateLocalArguments();
    local_arguments_opt -> Next() = argument;
}

inline void AstClassInstanceCreationExpression::AllocateLocalArguments(int estimate)
{
    if (! local_arguments_opt)
        local_arguments_opt =
            NewAstArray<AstExpression *> (pool, estimate);
}

inline void AstClassInstanceCreationExpression::AddLocalArgument(AstExpression *argument)
{
    if (! local_arguments_opt)
        AllocateLocalArguments();
    local_arguments_opt -> Next() = argument;
}

inline void AstMethodDeclaration::AllocateThrows(int estimate)
{
    if (! throws)
        throws =
            NewAstArray<AstExpression *> (pool, estimate);
}

inline void AstMethodDeclaration::AddThrow(AstExpression *exception)
{
    if (! throws)
        AllocateThrows();
    throws -> Next() = exception;
}

inline void AstConstructorDeclaration::AllocateThrows(int estimate)
{
    if (! throws)
        throws =
            NewAstArray<AstExpression *> (pool, estimate);
}

inline void AstConstructorDeclaration::AddThrow(AstExpression *exception)
{
    if (! throws)
        AllocateThrows();
    throws -> Next() = exception;
}

inline void AstMethodDeclarator::AllocateFormalParameters(int estimate)
{
    if (! formal_parameters)
        formal_parameters =
            NewAstArray<AstFormalParameter *> (pool, estimate);
}

inline void AstMethodDeclarator::AddFormalParameter(AstFormalParameter *formal_parameter)
{
    if (! formal_parameters)
        AllocateFormalParameters();
    formal_parameters -> Next() = formal_parameter;
}

inline void AstLocalVariableDeclarationStatement::AllocateVariableDeclarators(int estimate)
{
    if (! variable_declarators)
        variable_declarators =
            NewAstArray<AstVariableDeclarator *> (pool, estimate);
}

inline void AstLocalVariableDeclarationStatement::AddVariableDeclarator(AstVariableDeclarator *variable_declarator)
{
    if (! variable_declarators)
        AllocateVariableDeclarators();
    variable_declarators -> Next() = variable_declarator;
}

inline void AstFieldDeclaration::AllocateVariableDeclarators(int estimate)
{
    if (! variable_declarators)
        variable_declarators =
            NewAstArray<AstVariableDeclarator *> (pool, estimate);
}

inline void AstFieldDeclaration::AddVariableDeclarator(AstVariableDeclarator *variable_declarator)
{
    if (! variable_declarators)
        AllocateVariableDeclarators();
    variable_declarators -> Next() = variable_declarator;
}

inline void AstClassDeclaration::AllocateInterfaces(int estimate)
{
    if (! interfaces)
        interfaces =
            NewAstArray<AstExpression *> (pool, estimate);
}

inline void AstClassDeclaration::AddInterface(AstExpression *interf)
{
    if (! interfaces)
        AllocateInterfaces();
    interfaces -> Next() = interf;
}

inline void AstInterfaceDeclaration::AllocateExtendsInterfaces(int estimate)
{
    if (! extends_interfaces)
        extends_interfaces =
            NewAstArray<AstExpression *> (pool, estimate);
}

inline void AstInterfaceDeclaration::AddExtendsInterface(AstExpression *interf)
{
    if (! extends_interfaces)
        AllocateExtendsInterfaces();
    extends_interfaces -> Next() = interf;
}

inline void AstInterfaceDeclaration::AllocateInterfaceMemberDeclarations(int estimate)
{
    if (! interface_member_declarations)
        interface_member_declarations = NewAstArray<Ast *> (pool, estimate);
}

inline void AstInterfaceDeclaration::AddInterfaceMemberDeclaration(Ast *member)
{
    if (! interface_member_declarations)
        AllocateInterfaceMemberDeclarations();
    interface_member_declarations -> Next() = member;
}

inline void AstClassBody::AllocateClassBodyDeclarations(int estimate)
{
    if (! class_body_declarations)
        class_body_declarations = NewAstArray<Ast *> (pool, estimate);
}

inline void AstClassBody::AddClassBodyDeclaration(Ast *member)
{
    if (! class_body_declarations)
        AllocateClassBodyDeclarations();
    class_body_declarations -> Next() = member;
}

// not inline
void AstClassBody::AddClassBodyDeclarationNicely(Ast *member)
{
    AstFieldDeclaration *field_declaration = member -> FieldDeclarationCast();
    AstMethodDeclaration *method_declaration =
        member -> MethodDeclarationCast();
    AstConstructorDeclaration *constructor_declaration =
        member -> ConstructorDeclarationCast();
    AstStaticInitializer *static_initializer =
        member -> StaticInitializerCast();
    AstClassDeclaration *class_declaration = member -> ClassDeclarationCast();
    AstInterfaceDeclaration *interface_declaration =
        member -> InterfaceDeclarationCast();
    AstMethodBody *block = member -> MethodBodyCast();

    AddClassBodyDeclaration(member);


    // This is lifted from Parser::Act68.

    if (field_declaration)
    {
        if (field_declaration -> StaticFieldCast())
            AddClassVariable(field_declaration);
        else AddInstanceVariable(field_declaration);
    }
    else if (method_declaration)
    {
        AddMethod(method_declaration);
    }
    else if (constructor_declaration)
    {
        AddConstructor(constructor_declaration);
    }
    else if (static_initializer)
    {
        AddStaticInitializer(static_initializer);
    }
    else if (class_declaration)
    {
        AddNestedClass(class_declaration);
    }
    else if (interface_declaration)
    {
        AddNestedInterface(interface_declaration);
    }
    else if (block)
    {
        AddInstanceInitializer(block);
    }
    else // assert(block = member -> EmptyDeclarationCast())
    {
        AddEmptyDeclaration((AstEmptyDeclaration *) member);
    }
}

inline void AstForStatement::AllocateForInitStatements(int estimate)
{
    if (! for_init_statements)
        for_init_statements =
            NewAstArray<AstStatement *> (pool, estimate);
}

inline void AstForStatement::AddForInitStatement(AstStatement *statement)
{
    if (! for_init_statements)
        AllocateForInitStatements();
    for_init_statements -> Next() = statement;
}

inline void AstForStatement::AllocateForUpdateStatements(int estimate)
{
    if (! for_update_statements)
        for_update_statements =
            NewAstArray<AstExpressionStatement *> (pool, estimate);
}

inline void AstForStatement::AddForUpdateStatement(AstExpressionStatement *statement)
{
    if (! for_update_statements)
        AllocateForUpdateStatements();
    for_update_statements -> Next() = statement;
}

inline void AstArrayInitializer::AllocateVariableInitializers(int estimate)
{
    if (! variable_initializers)
        variable_initializers = NewAstArray<Ast *> (pool, estimate);
}

inline void AstArrayInitializer::AddVariableInitializer(Ast *initializer)
{
    if (! variable_initializers)
        AllocateVariableInitializers();
    variable_initializers -> Next() = initializer;
}

inline void AstTryStatement::AllocateCatchClauses(int estimate)
{
    if (! catch_clauses)
        catch_clauses =
            NewAstArray<AstCatchClause *> (pool, estimate);
}

inline void AstTryStatement::AddCatchClause(AstCatchClause *catch_clause)
{
    if (! catch_clauses)
        AllocateCatchClauses();
    catch_clauses -> Next() = catch_clause;
}

inline void AstCompilationUnit::AllocateImportDeclarations(int estimate)
{
    if (! import_declarations)
        import_declarations =
            NewAstArray<AstImportDeclaration *> (pool, estimate);
}

inline void AstCompilationUnit::AddImportDeclaration(AstImportDeclaration *import_declaration)
{
    if (! import_declarations)
        AllocateImportDeclarations();
    import_declarations -> Next() = import_declaration;
}

inline void AstCompilationUnit::AllocateTypeDeclarations(int estimate)
{
    if (! type_declarations)
        type_declarations = NewAstArray<Ast *> (pool, estimate);
}

inline void AstCompilationUnit::AddTypeDeclaration(Ast *type_declaration)
{
    if (! type_declarations)
        AllocateTypeDeclarations();
    type_declarations -> Next() = type_declaration;
}


//
// Allocate another block of storage for the ast array.
//
template <typename T>
    void AstArray<T>::AllocateMoreSpace()
    {
        //
        //
        // The variable size always indicates the maximum number of
        // elements that has been allocated for the array.
        // Initially, it is set to 0 to indicate that the array is empty.
        // The pool of available elements is divided into segments of size
        // 2**log_blksize each. Each segment is pointed to by a slot in
        // the array base.
        //
        // By dividing size by the size of the segment we obtain the
        // index for the next segment in base. If base is full, it is
        // reallocated.
        //
        //
        size_t k = size >> log_blksize; /* which segment? */

        //
        // If the base is overflowed, reallocate it and initialize the new
        // elements to NULL.
        //
        if (k == base_size)
        {
            int old_base_size = base_size;
            T **old_base = base;

            base_size += base_increment;

            // There must be enough room to allocate base
            assert(base_size <= pool -> Blksize());

            base = (T **) pool -> Alloc(sizeof(T *) * base_size);

            if (old_base != NULL)
            {
                memmove(base, old_base, old_base_size * sizeof(T *));
            }
            memset(&base[old_base_size], 0,
                   (base_size - old_base_size) * sizeof(T *));
        }

        //
        // We allocate a new segment and place its adjusted address in
        // base[k]. The adjustment allows us to index the segment directly,
        // instead of having to perform a subtraction for each reference.
        // See operator[] below.
        //
        // There must be enough room to allocate block
        assert(Blksize() <= pool -> Blksize());

        base[k] = (T *) pool -> Alloc(sizeof(T) * Blksize());
        base[k] -= size;

        //
        // Finally, we update size.
        //
        size += Blksize();
    }


template <typename T>
    //
    // Constructor of a ast array.
    //
    AstArray<T>::AstArray(StoragePool *pool_, unsigned estimate) : pool(pool_)
    {
        // AstArray should only be used for arrays of pointers.
        assert(sizeof(T) == sizeof(StoragePool::Cell));
        // There must be enough space in the storage pool to move !!!
        assert(pool -> Blksize() >= 256);

        if (estimate == 0)
            log_blksize = 6; // take a guess
        else
        {
            for (log_blksize = 1;
                 (((unsigned) 1 << log_blksize) < estimate) &&
                     (log_blksize < 31);
                 log_blksize++)
                ;
        }

        //
        // Increment a base_increment size that is big enough not to have to
        // be reallocated. Find a block size that is smaller that the block
        // size of the pool.
        //
        base_increment = (Blksize() > pool -> Blksize()
                          ? Blksize() / pool -> Blksize() : 1) * 2;
        while (Blksize() >= pool -> Blksize())
            log_blksize--;

        base_size = 0;
        size = 0;
        top = 0;
        base = NULL;
    }

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

#endif // ast_INCLUDED

