// $Id$ -*- c++ -*-
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 2004 IBM Corporation and others.  All Rights Reserved.
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
struct CaseElement;

class VariableSymbolArray
{
    typedef VariableSymbol* T;

    T** base;
    size_t base_size;
    unsigned top;
    unsigned size;
    StoragePool* pool;
    unsigned short log_blksize;
    unsigned short base_increment;

    inline size_t Blksize() { return 1 << log_blksize; }

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
    void Reset(unsigned n = 0)
    {
        assert(n <= size);
        top = n;
    }

    //
    // Return length of the VariableSymbol array.
    //
    unsigned Length() { return top; }

    //
    // Return a reference to the ith element of the VariableSymbol array.
    //
    // Note that no check is made here to ensure that 0 <= i < top.
    // Such a check might be useful for debugging and a range exception
    // should be thrown if it yields true.
    //
    T& operator[](unsigned i) { return base[i >> log_blksize][i]; }

    //
    // Add an element to the VariableSymbol array and return the top index.
    //
    unsigned NextIndex()
    {
        unsigned i = top++;
        if (i == size)
            AllocateMoreSpace();
        return i;
    }

    //
    // Add an element to the VariableSymbol array and return a reference to
    // that new element.
    //
    T& Next()
    {
        unsigned i = NextIndex();
        return base[i >> log_blksize][i];
    }

    //
    // Constructor of a VariableSymbol array.
    //
    VariableSymbolArray(StoragePool*, unsigned);

    //
    // Destructor of an VariableSymbol array.
    //
    ~VariableSymbolArray() { assert(false); }
};


//***************************************************************************
//
// TODO: This documentation is a bit out of date...
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
//    All AST tree nodes belong to a StoragePool. When a new node must be
//    added, it is allocated from the same StoragePool. Thus, you should
//    never use operator new to construct an AST object, but instead use New*
//    or Gen* defined in StoragePool. Likewise, AST tree nodes never need
//    destruction - simply delete the pool to reclaim the entire tree.
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
template <typename T> class AstArray;
class AstListNode;
class AstDeclared;
class AstDeclaredType;
class AstStatement;
class AstMemberValue;
class AstExpression;
class AstType;

class AstBlock;
class AstName;
class AstPrimitiveType;
class AstBrackets;
class AstArrayType;
class AstTypeName;
class AstMemberValuePair;
class AstAnnotation;
class AstModifierKeyword;
class AstModifiers;
class AstPackageDeclaration;
class AstImportDeclaration;
class AstCompilationUnit;
class AstEmptyDeclaration;
class AstClassBody;
class AstClassDeclaration;
class AstArrayInitializer;
class AstVariableDeclaratorId;
class AstVariableDeclarator;
class AstFieldDeclaration;
class AstFormalParameter;
class AstMethodDeclarator;
class AstMethodBody;
class AstMethodDeclaration;
class AstInitializerDeclaration;
class AstArguments;
class AstThisCall;
class AstSuperCall;
class AstConstructorDeclaration;
class AstInterfaceDeclaration;
class AstLocalVariableStatement;
class AstLocalClassStatement;
class AstIfStatement;
class AstEmptyStatement;
class AstExpressionStatement;
class AstSwitchLabel;
class AstSwitchBlockStatement;
class AstSwitchStatement;
class AstWhileStatement;
class AstDoStatement;
class AstForStatement;
class AstForeachStatement;
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
class AstClassLiteral;
class AstThisExpression;
class AstSuperExpression;
class AstParenthesizedExpression;
class AstClassCreationExpression;
class AstDimExpr;
class AstArrayCreationExpression;
class AstFieldAccess;
class AstMethodInvocation;
class AstArrayAccess;
class AstPostUnaryExpression;
class AstPreUnaryExpression;
class AstCastExpression;
class AstBinaryExpression;
class AstInstanceofExpression;
class AstConditionalExpression;
class AstAssignmentExpression;

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

        _num_tags = EXPRESSION
    };

    //
    // These are the different kinds for the Ast objects.
    //
    enum AstKind
    {
        AST, // must be first
        // Expressions
        NAME,
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
        CLASS_LITERAL,
        THIS_EXPRESSION,
        SUPER_EXPRESSION,
        PARENTHESIZED_EXPRESSION,
        ARRAY_ACCESS,
        CALL,
        CLASS_CREATION,
        ARRAY_CREATION,
        POST_UNARY,
        PRE_UNARY,
        CAST,
        BINARY,
        INSTANCEOF,
        CONDITIONAL,
        ASSIGNMENT,
        _num_expression_kinds,
        // Statements
        THIS_CALL,
        SUPER_CALL,
        BLOCK,
        IF,
        EMPTY_STATEMENT,
        EXPRESSION_STATEMENT,
        SWITCH,
        SWITCH_BLOCK,
        LOCAL_VARIABLE_DECLARATION,
        LOCAL_CLASS,
        WHILE,
        DO,
        FOR,
        FOREACH,
        BREAK,
        CONTINUE,
        RETURN,
        THROW,
        SYNCHRONIZED_STATEMENT,
        ASSERT,
        TRY,
        _num_expr_or_stmt_kinds,
        // All others
        ARGUMENTS = _num_expr_or_stmt_kinds,
        DIM,
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
        TYPE,
        COMPILATION,
        MEMBER_VALUE_PAIR,
        ANNOTATION,
        MODIFIER_KEYWORD,
        MODIFIERS,
        PACKAGE,
        IMPORT,
        EMPTY_DECLARATION,
        CLASS,
        CLASS_BODY,
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
        INITIALIZER,
        METHOD_BODY,
        SWITCH_LABEL,
        CATCH,
        FINALLY,
        _num_kinds
    };

    //
    // Every node has a unique kind, and class_tag marks groups of similar
    // nodes. The bit-fields allow smaller Ast objects without sacrificing
    // type-safety or debug visibility.
    //
    const AstKind kind : 8;
    const AstTag class_tag : 8;

    //
    // This is a catch-all set of bits free for the use of subclasses.
    // See CompilationTag, FieldDeclarationTag, InitializerDeclarationTag,
    // ClassBodyTag, BlockTag, PreUnaryExpressionTag, PostUnaryExpressionTag,
    // BinaryExpressionTag, AssignmentExpressionTag.
    //
protected:
    unsigned other_tag : 8;

    //
    // "generated" is a boolean value that indicates whether or not a node
    // is associated with a construct in a source file or that is was generated
    // by the compiler. See functions "gen_ ..." and "new_ ..." below.
    //
public:
    bool generated;

#ifdef JIKES_DEBUG
    const unsigned id;
    static unsigned count;
    static bool debug_unparse;
#endif // JIKES_DEBUG

    //
    // Note that ALL fields of an Ast are initialized to 0 unless modified
    // by the constructor, thanks to the 0-initialization guaranteed by
    // operator new.  This allows for more efficiency by not redundantly
    // setting a field to 0, false, or NULL.
    //
    inline Ast(AstKind k, AstTag t = NO_TAG)
        : kind(k)
        , class_tag(t)
#ifdef JIKES_DEBUG
        , id(++count)
#endif // JIKES_DEBUG
    {}

    //
    // Ast nodes should be created from a storage pool. Use the syntax
    // new (pool) AstSubclass(constructor arguments). The resultant Ast
    // will be zero-initialized except for what the constructor explicitly
    // sets, due to the properties of StoragePool. Note that there are
    // no Ast[]; rather, use AstArray<Ast*>.
    //
    inline void* operator new(size_t, StoragePool*);
private:
    void* operator new[](size_t, void* p) { assert(false); return p; }
public:

    //
    // ASTs should not be destructed. Instead, delete the containing
    // StoragePool.
    //
    virtual ~Ast() { assert(false && "Use the associated StoragePool"); }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&) = 0;
    virtual void Unparse(Ostream&, LexStream*) = 0;
#endif // JIKES_DEBUG

    //
    // General queries.
    //
    bool IsLeftHandSide();
    bool IsExplicitConstructorInvocation();
    bool IsGenerated();

    //
    // The Conversion functions below are provided as a convenient way to
    // cast a generic Ast node into a specific node. Note that if one knows
    // the type of a node for sure, it is more efficient to use a specific
    // cast expression. For example, if one knows that a "Ast* p" pointer
    // dereferences a FieldDeclaration then a cast expression should be
    // used to cast p, as follows:
    //
    //       AstFieldDeclaration* fp = (FieldDeclaration*) p;
    //
    // However, if p points to a ClassBodyDeclaration which may be
    // either a FieldDeclaration, MethodDeclaration, ConstructorDeclaration,
    // InitializerDeclaration, ClassDeclaration, or an InterfaceDeclaration,
    // then the following sequence of code may be used:
    //
    //    AstFieldDeclaration* fp;
    //    AstMethodDeclaration* mp;
    //    AstConstructorDeclaration* cp;
    //    AstInitializerDeclaration* sp;
    //    AstClassDeclaration* Cp; // 1.1 only
    //    AstInterfaceDeclaration* Ip; // 1.1 only
    //
    //    if (fp = p -> FieldDeclarationCast())
    //        ...
    //    else if (mp = p -> MethodDeclarationCast())
    //        ...
    //    else if (cp = p -> ConstructorDeclarationCast())
    //        ...
    //    else if (sp = p -> InitializerDeclarationCast())
    //        ...
    //    else if (Cp = p -> ClassDeclarationCast())
    //        ...
    //    else if (Ip = p -> InterfaceDeclarationCast())
    //        ...
    //

    //
    // These cast functions are used for classes that represent more than
    // one kind of nodes.  The functions must be listed after the subclasses
    // have been defined.
    //
    inline AstStatement* StatementCast();
    inline AstMemberValue* MemberValueCast();
    inline AstExpression* ExpressionCast();
    inline AstPrimitiveType* PrimitiveTypeCast();
    inline AstFieldDeclaration* StaticFieldCast();
    inline AstInitializerDeclaration* StaticInitializerCast();
    inline AstClassBody* UnparsedClassBodyCast();
    inline AstCompilationUnit* BadCompilationUnitCast();
    inline AstCompilationUnit* EmptyCompilationUnitCast();

    //
    // These cast functions are used for classes that represent exactly
    // one kind of node.
    //
    inline AstListNode* ListNodeCast();
    inline AstBlock* BlockCast();
    inline AstName* NameCast();
    inline AstBrackets* BracketsCast();
    inline AstArrayType* ArrayTypeCast();
    inline AstTypeName* TypeNameCast();
    inline AstMemberValuePair* MemberValuePairCast();
    inline AstAnnotation* AnnotationCast();
    inline AstModifierKeyword* ModifierKeywordCast();
    inline AstModifiers* ModifiersCast();
    inline AstPackageDeclaration* PackageDeclarationCast();
    inline AstImportDeclaration* ImportDeclarationCast();
    inline AstCompilationUnit* CompilationUnitCast();
    inline AstEmptyDeclaration* EmptyDeclarationCast();
    inline AstClassBody* ClassBodyCast();
    inline AstClassDeclaration* ClassDeclarationCast();
    inline AstArrayInitializer* ArrayInitializerCast();
    inline AstVariableDeclaratorId* VariableDeclaratorIdCast();
    inline AstVariableDeclarator* VariableDeclaratorCast();
    inline AstFieldDeclaration* FieldDeclarationCast();
    inline AstFormalParameter* FormalParameterCast();
    inline AstMethodDeclarator* MethodDeclaratorCast();
    inline AstMethodBody* MethodBodyCast();
    inline AstMethodDeclaration* MethodDeclarationCast();
    inline AstInitializerDeclaration* InitializerDeclarationCast();
    inline AstArguments* ArgumentsCast();
    inline AstThisCall* ThisCallCast();
    inline AstSuperCall* SuperCallCast();
    inline AstConstructorDeclaration* ConstructorDeclarationCast();
    inline AstInterfaceDeclaration* InterfaceDeclarationCast();
    inline AstLocalVariableStatement* LocalVariableStatementCast();
    inline AstLocalClassStatement* LocalClassStatementCast();
    inline AstIfStatement* IfStatementCast();
    inline AstEmptyStatement* EmptyStatementCast();
    inline AstExpressionStatement* ExpressionStatementCast();
    inline AstSwitchLabel* SwitchLabelCast();
    inline AstSwitchBlockStatement* SwitchBlockStatementCast();
    inline AstSwitchStatement* SwitchStatementCast();
    inline AstWhileStatement* WhileStatementCast();
    inline AstDoStatement* DoStatementCast();
    inline AstForStatement* ForStatementCast();
    inline AstForeachStatement* ForeachStatementCast();
    inline AstBreakStatement* BreakStatementCast();
    inline AstContinueStatement* ContinueStatementCast();
    inline AstReturnStatement* ReturnStatementCast();
    inline AstThrowStatement* ThrowStatementCast();
    inline AstSynchronizedStatement* SynchronizedStatementCast();
    inline AstAssertStatement* AssertStatementCast();
    inline AstCatchClause* CatchClauseCast();
    inline AstFinallyClause* FinallyClauseCast();
    inline AstTryStatement* TryStatementCast();
    inline AstIntegerLiteral* IntegerLiteralCast();
    inline AstLongLiteral* LongLiteralCast();
    inline AstFloatLiteral* FloatLiteralCast();
    inline AstDoubleLiteral* DoubleLiteralCast();
    inline AstTrueLiteral* TrueLiteralCast();
    inline AstFalseLiteral* FalseLiteralCast();
    inline AstStringLiteral* StringLiteralCast();
    inline AstCharacterLiteral* CharacterLiteralCast();
    inline AstNullLiteral* NullLiteralCast();
    inline AstClassLiteral* ClassLiteralCast();
    inline AstThisExpression* ThisExpressionCast();
    inline AstSuperExpression* SuperExpressionCast();
    inline AstParenthesizedExpression* ParenthesizedExpressionCast();
    inline AstClassCreationExpression* ClassCreationExpressionCast();
    inline AstDimExpr* DimExprCast();
    inline AstArrayCreationExpression* ArrayCreationExpressionCast();
    inline AstFieldAccess* FieldAccessCast();
    inline AstMethodInvocation* MethodInvocationCast();
    inline AstArrayAccess* ArrayAccessCast();
    inline AstPostUnaryExpression* PostUnaryExpressionCast();
    inline AstPreUnaryExpression* PreUnaryExpressionCast();
    inline AstCastExpression* CastExpressionCast();
    inline AstBinaryExpression* BinaryExpressionCast();
    inline AstInstanceofExpression* InstanceofExpressionCast();
    inline AstConditionalExpression* ConditionalExpressionCast();
    inline AstAssignmentExpression* AssignmentExpressionCast();

    //
    // It would be nice if this could be covariant, as it would allow
    // less casting. But MSVC++ can't yet handle covariant return
    // types at all, and both GCC and HP's aCC croak with covariance during
    // multiple inheritance (bummer). So, there is a bunch of hideous casting
    // in ast.cpp that could otherwise be avoided if standards were followed.
    //
    // Clones are used for various things, such as pre-evaluating final
    // constant values.
    //
    virtual Ast* Clone(StoragePool*) = 0;

    //
    // These functions return the left and right tokens of this tree branch.
    //
    virtual LexStream::TokenIndex LeftToken() = 0;
    virtual LexStream::TokenIndex RightToken() = 0;
};


//
// This AstArray template class can be used to construct a bounds-checking
// array of Ast objects. The size of the array must be known up front, as
// it is allocated contiguously from a StoragePool (preferably the pool that
// also owns the Ast object which contains this array).
//
template <typename T>
class AstArray
{
    const unsigned size;
    unsigned top;
    T* array;

public:
    //
    // Return length of the Ast array.
    //
    unsigned Length() { return top; }

    //
    // Return a reference to the ith element of the Ast array.
    //
    T& operator[](unsigned i)
    {
        assert(i < top);
        return array[i];
    }

    //
    // Add an element to the Ast array and return a reference to
    // that new element.
    //
    T& Next()
    {
        assert(top < size);
        return array[top++];
    }

    //
    // Constructor of an Ast array.
    //
    AstArray(StoragePool*, unsigned);

    //
    // Ast arrays should not be destroyed. Rather, delete the StoragePool
    // that was passed to the constructor.
    //
    ~AstArray() { assert(false && "Use the associated StoragePool"); }

    //
    // Ast arrays must be created via a StoragePool, and there are no
    // AstArray[].
    //
    inline void* operator new(size_t, StoragePool*);
private:
    void* operator new[](size_t, void* p) { assert(false); return p; }
};


//
// The Ast list node. This is a temporary object used in constructing lists
// while parsing the grammar; once constructed, the contents are extracted
// and this list is reclaimed. It is circular to make insertion easy while
// maintaining declaration order.
//
class AstListNode : public Ast
{
public:
    AstListNode* next;
    Ast* element;
    unsigned index;

    inline AstListNode()
        : Ast(LIST_NODE)
    {
#ifdef JIKES_DEBUG
        --count; // don't count these nodes
#endif // JIKES_DEBUG
    }

    ~AstListNode() {}

    //
    // These next three functions should never be called, since list nodes
    // only exist long enough to create the AST tree and then are reclaimed.
    //
    virtual Ast* Clone(StoragePool*) { assert(false); return NULL; }
#ifdef JIKES_DEBUG
    virtual void Print(LexStream&) { assert(false); }
    virtual void Unparse(Ostream&, LexStream*) { assert(false); }
#endif // JIKES_DEBUG

    virtual LexStream::TokenIndex LeftToken()
    {
        return element -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return element -> RightToken();
    }
};


//
// This class adds some type safety. It represents all member declarations
// in types. See AstFieldDeclaration, AstMethodDeclaration,
// AstConstructorDeclaration, AstInitializerDeclaration, DeclaredType.
//
class AstDeclared : public Ast
{
public:
    AstModifiers* modifiers_opt;

    inline AstDeclared(AstKind k)
        : Ast(k)
    {}
    ~AstDeclared() {}
};


//
// This class adds some type safety. It represents all type declarations.
// See AstClassDeclaration, AstInterfaceDeclaration, AstEmptyDeclaration.
//
class AstDeclaredType : public AstDeclared
{
public:
    AstClassBody* class_body;

    inline AstDeclaredType(AstKind k)
        : AstDeclared(k)
    {}
    ~AstDeclaredType() {}

    inline bool IsValid();
};


//
// This class represents statements.
//
class AstStatement : public Ast
{
public:
    bool is_reachable;
    bool can_complete_normally;

    inline AstStatement(AstKind k, bool reachable = false,
                        bool can_complete = false)
        : Ast(k, STATEMENT)
        , is_reachable(reachable)
        , can_complete_normally(can_complete)
    {}
    ~AstStatement() {}
};


//
// This is the superclass of constructs which can appear in an array
// initializer, including annotations added by JSR 175.
//
class AstMemberValue : public Ast
{
public:
    // The field or method this expression resolves to, or the annotation type
    // that the annotation resolves to.
    Symbol* symbol;

    inline AstMemberValue(AstKind k, AstTag t = NO_TAG)
        : Ast(k, t)
    {}
    ~AstMemberValue() {}

    TypeSymbol* Type();
};


//
// This is the superclass of constructs which represent an expression.
//
class AstExpression : public AstMemberValue
{
public:
    LiteralValue* value; // The compile-time constant value of the expression.

    inline AstExpression(AstKind k)
        : AstMemberValue(k, EXPRESSION)
    {}
    ~AstExpression() {}

    inline bool IsConstant() { return value != NULL; }
};


//
// This is the superclass of constructs which represent a type.
//
class AstType : public Ast
{
public:
    TypeSymbol* symbol;

    inline AstType(AstKind k, AstTag t = NO_TAG)
        : Ast(k, t)
    {}
    ~AstType() {}

    virtual LexStream::TokenIndex IdentifierToken() = 0;
};


//
// Blocks represent both method blocks and compound statements. The parser
// creates synthetic blocks around statements where blocks are optional (such
// as if statement branches), and around loops.
//
class AstBlock : public AstStatement
{
protected:
    StoragePool* pool;

private:
    AstArray<AstStatement*>* block_statements;
    VariableSymbolArray* defined_variables;

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

    BlockSymbol* block_symbol;
    unsigned nesting_level;

    LexStream::TokenIndex label_opt;
    LexStream::TokenIndex left_brace_token;
    LexStream::TokenIndex right_brace_token;

    bool no_braces;

    inline AstBlock(StoragePool* p, AstKind k = BLOCK, bool reachable = false)
        : AstStatement(k, reachable)
        , pool(p)
    {}
    ~AstBlock() {}

    inline BlockTag Tag() { return (BlockTag) other_tag; }
    inline void SetTag(BlockTag tag) { other_tag = tag; }

    inline AstStatement*& Statement(unsigned i)
    {
        return (*block_statements)[i];
    }
    inline unsigned NumStatements()
    {
        return block_statements ? block_statements -> Length() : 0;
    }
    inline void AllocateStatements(unsigned estimate = 1);
    inline void AddStatement(AstStatement*);

    inline VariableSymbol*& LocallyDefinedVariable(unsigned i)
    {
        return (*defined_variables)[i];
    }
    inline unsigned NumLocallyDefinedVariables()
    {
        return defined_variables ? defined_variables -> Length() : 0;
    }
    inline void AllocateLocallyDefinedVariables(unsigned estimate = 1);
    inline void AddLocallyDefinedVariable(VariableSymbol*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return left_brace_token; }
    virtual LexStream::TokenIndex RightToken() { return right_brace_token; }

protected:
    void CloneBlock(StoragePool*, AstBlock*);
};


//
// Simple and qualified names.
//
class AstName : public AstExpression
{
public:
    AstName* base_opt;
    LexStream::TokenIndex identifier_token;

    //
    // When a name refers to a member in an enclosing scope, it is mapped
    // into an expression that creates a path to the member in question.
    //
    AstExpression* resolution_opt;

    inline AstName(LexStream::TokenIndex token)
        : AstExpression(NAME)
        , identifier_token(token)
    {}
    ~AstName() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return base_opt ? base_opt -> LeftToken() : identifier_token;
    }
    virtual LexStream::TokenIndex RightToken() { return identifier_token; }
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
class AstPrimitiveType : public AstType
{
public:
    LexStream::TokenIndex primitive_kind_token;

    inline AstPrimitiveType(AstKind k, LexStream::TokenIndex token)
        : AstType(k, PRIMITIVE_TYPE)
        , primitive_kind_token(token)
    {}
    ~AstPrimitiveType() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return primitive_kind_token; }
    virtual LexStream::TokenIndex RightToken() { return primitive_kind_token; }
    virtual LexStream::TokenIndex IdentifierToken()
    {
        return primitive_kind_token;
    }
};


//
// Represents one or more pairs of '[' ']'.
//
class AstBrackets : public Ast
{
public:
    LexStream::TokenIndex left_bracket_token;
    LexStream::TokenIndex right_bracket_token;

    unsigned dims;

    inline AstBrackets(LexStream::TokenIndex l, LexStream::TokenIndex r)
        : Ast(BRACKETS)
        , left_bracket_token(l)
        , right_bracket_token(r)
        , dims(1)
    {}
    ~AstBrackets() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return left_bracket_token; }
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
class AstArrayType : public AstType
{
public:
    AstType* type; // AstPrimitiveType, AstTypeName
    AstBrackets* brackets;

    inline AstArrayType(AstType* t, AstBrackets* b)
        : AstType(ARRAY)
        , type(t)
        , brackets(b)
    {}
    ~AstArrayType() {}

    inline unsigned NumBrackets() { return brackets -> dims; }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return type -> LeftToken(); }
    virtual LexStream::TokenIndex RightToken()
    {
        return brackets -> right_bracket_token;
    }
    virtual LexStream::TokenIndex IdentifierToken()
    {
        return type -> IdentifierToken();
    }
};


//
// Represents a type. Occurs in several contexts - imports; supertypes;
// throws clauses; parameter, field, and method return types; qualified this
// and super; class literals; casts.
//
class AstTypeName : public AstType
{
public:
    AstName* name;

    inline AstTypeName(StoragePool*, AstName* n)
        : AstType(TYPE)
        , name(n)
    {}
    ~AstTypeName() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return name -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return name -> identifier_token;
    }
    virtual LexStream::TokenIndex IdentifierToken()
    {
        return name -> identifier_token;
    }
};


//
// MemberValuePair is added by JSR 175. This covers MemberValuePair and
// SingleMemberAnnotation in the grammar.
//
class AstMemberValuePair : public Ast
{
public:
    LexStream::TokenIndex identifier_token_opt;
    AstMemberValue* member_value;

    MethodSymbol* name_symbol; // The annotation method this value maps to.

    inline AstMemberValuePair()
        : Ast(MEMBER_VALUE_PAIR)
    {}
    ~AstMemberValuePair() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return identifier_token_opt ? identifier_token_opt
            : member_value -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return member_value -> RightToken();
    }
};


//
// Annotation is added by JSR 175. This covers NormalAnnotation,
// MarkerAnnotation, and SingleMemberAnnotation in the grammar.
//
class AstAnnotation : public AstMemberValue
{
    StoragePool* pool;
    AstArray<AstMemberValuePair*>* member_value_pairs;

public:
    LexStream::TokenIndex at_token;
    AstName* name;
    LexStream::TokenIndex right_paren_token_opt;

    inline AstAnnotation(StoragePool* p)
        : AstMemberValue(ANNOTATION)
        , pool(p)
    {}
    ~AstAnnotation() {}

    inline AstMemberValuePair*& MemberValuePair(unsigned i)
    {
        return (*member_value_pairs)[i];
    }
    inline unsigned NumMemberValuePairs()
    {
        return member_value_pairs ? member_value_pairs -> Length() : 0;
    }
    inline void AllocateMemberValuePairs(unsigned estimate = 1);
    inline void AddMemberValuePair(AstMemberValuePair*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return at_token; }
    virtual LexStream::TokenIndex RightToken()
    {
        return right_paren_token_opt ? right_paren_token_opt
            : name -> identifier_token;
    }
};


// Represents a single modifier keyword ('public', 'protected', 'private',
// 'static', 'abstract', 'final', 'native', 'synchronized', 'transient',
// 'volatile', and 'strictfp').
//
class AstModifierKeyword : public Ast
{
public:
    LexStream::TokenIndex modifier_token;

    AstModifierKeyword(LexStream::TokenIndex token)
        : Ast(MODIFIER_KEYWORD)
        , modifier_token(token)
    {}
    ~AstModifierKeyword() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return modifier_token; }
    virtual LexStream::TokenIndex RightToken() { return modifier_token; }
};


//
// Represents one or more modifier keywords, as well as annotations (added in
// JSR 175).
//
class AstModifiers : public Ast
{
    StoragePool* pool;
    AstArray<Ast*>* modifiers; // AstAnnotation, AstModifierKeyword
    
public:
    // Allows sorting between static and non-static declarations.
    LexStream::TokenIndex static_token_opt;

    inline AstModifiers(StoragePool* p)
        : Ast(MODIFIERS)
        , pool(p)
    {}
    ~AstModifiers() {}

    inline Ast*& Modifier(unsigned i)
    {
        return (*modifiers)[i];
    }
    inline unsigned NumModifiers()
    {
        assert(modifiers);
        return modifiers -> Length();
    }
    inline void AllocateModifiers(unsigned estimate = 1);
    inline void AddModifier(AstAnnotation*);
    inline void AddModifier(AstModifierKeyword*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return Modifier(0) -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return Modifier(NumModifiers() - 1) -> RightToken();
    }
};


//
// PackageDeclaration --> <PACKAGE, package_token, Name, ;_token>
//
class AstPackageDeclaration : public Ast
{
public:
    AstModifiers* modifiers_opt;
    LexStream::TokenIndex package_token;
    AstName* name;
    LexStream::TokenIndex semicolon_token;

    inline AstPackageDeclaration()
        : Ast(PACKAGE)
    {}
    ~AstPackageDeclaration() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return modifiers_opt ? modifiers_opt -> LeftToken() : package_token;
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// ImportDeclaration --> <IMPORT, import_token, Name, *_token_opt, ;_token>
//
class AstImportDeclaration : public Ast
{
public:
    LexStream::TokenIndex import_token;
    LexStream::TokenIndex static_token_opt;
    AstName* name;
    LexStream::TokenIndex star_token_opt;
    LexStream::TokenIndex semicolon_token;

    inline AstImportDeclaration()
        : Ast(IMPORT)
    {}
    ~AstImportDeclaration() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return import_token; }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// The root node for compilation.
//
class AstCompilationUnit : public Ast
{
    AstArray<AstImportDeclaration*>* import_declarations;
    AstArray<AstDeclaredType*>* type_declarations;

public:
    enum CompilationTag
    {
        NONE,
        BAD_COMPILATION,
        EMPTY_COMPILATION
    };

    StoragePool* ast_pool;

    AstPackageDeclaration* package_declaration_opt;

    inline AstCompilationUnit(StoragePool* p)
        : Ast(COMPILATION)
        , ast_pool(p)
    {}
    ~AstCompilationUnit() {}

    void FreeAst();

    inline void MarkBad() { other_tag = BAD_COMPILATION; }
    inline void MarkEmpty() { other_tag = EMPTY_COMPILATION; }

    inline AstImportDeclaration*& ImportDeclaration(unsigned i)
    {
        return (*import_declarations)[i];
    }
    inline unsigned NumImportDeclarations()
    {
        return import_declarations ? import_declarations -> Length() : 0;
    }
    inline void AllocateImportDeclarations(unsigned estimate = 1);
    inline void AddImportDeclaration(AstImportDeclaration*);

    inline AstDeclaredType*& TypeDeclaration(unsigned i)
    {
        return (*type_declarations)[i];
    }
    inline unsigned NumTypeDeclarations()
    {
        return type_declarations ? type_declarations -> Length() : 0;
    }
    inline void AllocateTypeDeclarations(unsigned estimate = 1);
    inline void AddTypeDeclaration(AstDeclaredType*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);

    // special forms
    virtual void Unparse(LexStream*, const char* const directory);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        if (package_declaration_opt)
            return package_declaration_opt -> package_token;
        if (NumImportDeclarations())
            return ImportDeclaration(0) -> import_token;
        if (NumTypeDeclarations())
            return TypeDeclaration(0) -> LeftToken();
        return 0;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        if (NumTypeDeclarations())
            return TypeDeclaration(NumTypeDeclarations() - 1) -> RightToken();
        if (NumImportDeclarations())
            return ImportDeclaration(NumImportDeclarations() - 1) ->
                semicolon_token;
        if (package_declaration_opt)
            return package_declaration_opt -> semicolon_token;
        return 0;
    }
};


//
// EmptyDeclaration --> <EMPTY_DECLARATION, ;_token>
//
class AstEmptyDeclaration : public AstDeclaredType
{
public:
    LexStream::TokenIndex semicolon_token;

    inline AstEmptyDeclaration(LexStream::TokenIndex token)
        : AstDeclaredType(EMPTY_DECLARATION)
        , semicolon_token(token)
    {}
    ~AstEmptyDeclaration() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return semicolon_token; }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// ClassBody --> <CLASS_BODY, {_token, ClassBodyDeclarations, }_token>
//
class AstClassBody : public Ast
{
    friend class Parser;

    StoragePool* pool;
    AstArray<AstDeclared*>* class_body_declarations;

    AstArray<AstFieldDeclaration*>* instance_variables;
    AstArray<AstFieldDeclaration*>* class_variables;
    AstArray<AstMethodDeclaration*>* methods;
    AstArray<AstConstructorDeclaration*>* constructors;
    AstArray<AstInitializerDeclaration*>* static_initializers;
    AstArray<AstInitializerDeclaration*>* instance_initializers;
    AstArray<AstClassDeclaration*>* inner_classes;
    AstArray<AstInterfaceDeclaration*>* inner_interfaces;
    AstArray<AstEmptyDeclaration*>* empty_declarations;

public:
    enum ClassBodyTag
    {
        NONE,
        UNPARSED
    };

    SemanticEnvironment* semantic_environment;
    AstConstructorDeclaration* default_constructor;

    //
    // Filled in by the owning AstClassDeclaration, AstInterfaceDeclaration,
    // or AstClassCreationExpression to allow nicer error messages.
    // Note that owner is null for anonymous classes.
    //
    AstDeclaredType* owner;
    LexStream::TokenIndex identifier_token;

    //
    // The actual delimiters of the class body.
    //
    LexStream::TokenIndex left_brace_token;
    LexStream::TokenIndex right_brace_token;

    inline AstClassBody(StoragePool* p)
        : Ast(CLASS_BODY)
        , pool(p)
    {}
    ~AstClassBody() {}

    inline void MarkUnparsed() { other_tag = UNPARSED; }
    inline void MarkParsed() { other_tag = NONE; }

    inline AstDeclared*& ClassBodyDeclaration(unsigned i)
    {
        return (*class_body_declarations)[i];
    }
    inline unsigned NumClassBodyDeclarations()
    {
        return class_body_declarations
            ? class_body_declarations -> Length() : 0;
    }
    inline void AllocateClassBodyDeclarations(unsigned estimate = 1);
    inline void AddClassBodyDeclaration(AstDeclared*);
    inline void AddClassBodyDeclarationNicely(AstDeclared*);

    inline AstFieldDeclaration*& InstanceVariable(unsigned i)
    {
        return (*instance_variables)[i];
    }
    inline unsigned NumInstanceVariables()
    {
        return instance_variables ? instance_variables -> Length() : 0;
    }
    inline void AllocateInstanceVariables(unsigned estimate = 1);
    inline void AddInstanceVariable(AstFieldDeclaration*);

    inline AstFieldDeclaration*& ClassVariable(unsigned i)
    {
        return (*class_variables)[i];
    }
    inline unsigned NumClassVariables()
    {
        return class_variables ? class_variables -> Length() : 0;
    }
    inline void AllocateClassVariables(unsigned estimate = 1);
    inline void AddClassVariable(AstFieldDeclaration*);

    inline AstMethodDeclaration*& Method(unsigned i) { return (*methods)[i]; }
    inline unsigned NumMethods()
    {
        return methods ? methods -> Length() : 0;
    }
    inline void AllocateMethods(unsigned estimate = 1);
    inline void AddMethod(AstMethodDeclaration*);

    inline AstConstructorDeclaration*& Constructor(unsigned i)
    {
        return (*constructors)[i];
    }
    inline unsigned NumConstructors()
    {
        return constructors ? constructors -> Length() : 0;
    }
    inline void AllocateConstructors(unsigned estimate = 1);
    inline void AddConstructor(AstConstructorDeclaration*);

    inline AstInitializerDeclaration*& StaticInitializer(unsigned i)
    {
        return (*static_initializers)[i];
    }
    inline unsigned NumStaticInitializers()
    {
        return static_initializers ? static_initializers -> Length() : 0;
    }
    inline void AllocateStaticInitializers(unsigned estimate = 1);
    inline void AddStaticInitializer(AstInitializerDeclaration*);

    inline AstInitializerDeclaration*& InstanceInitializer(unsigned i)
    {
        return (*instance_initializers)[i];
    }
    inline unsigned NumInstanceInitializers()
    {
        return instance_initializers ? instance_initializers -> Length() : 0;
    }
    inline void AllocateInstanceInitializers(unsigned estimate = 1);
    inline void AddInstanceInitializer(AstInitializerDeclaration*);

    inline AstClassDeclaration*& NestedClass(unsigned i)
    {
        return (*inner_classes)[i];
    }
    inline unsigned NumNestedClasses()
    {
        return inner_classes ? inner_classes -> Length() : 0;
    }
    inline void AllocateNestedClasses(unsigned estimate = 1);
    inline void AddNestedClass(AstClassDeclaration*);

    inline AstInterfaceDeclaration*& NestedInterface(unsigned i)
    {
        return (*inner_interfaces)[i];
    }
    inline unsigned NumNestedInterfaces()
    {
        return inner_interfaces ? inner_interfaces -> Length() : 0;
    }
    inline void AllocateNestedInterfaces(unsigned estimate = 1);
    inline void AddNestedInterface(AstInterfaceDeclaration*);

    inline AstEmptyDeclaration*& EmptyDeclaration(unsigned i)
    {
        return (*empty_declarations)[i];
    }
    inline unsigned NumEmptyDeclarations()
    {
        return empty_declarations ? empty_declarations -> Length() : 0;
    }
    inline void AllocateEmptyDeclarations(unsigned estimate = 1);
    inline void AddEmptyDeclaration(AstEmptyDeclaration*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return left_brace_token; }
    virtual LexStream::TokenIndex RightToken() { return right_brace_token; }
};



//
// Represents a class declaration.
//
class AstClassDeclaration : public AstDeclaredType
{
    StoragePool* pool;
    AstArray<AstTypeName*>* interfaces;

public:
    LexStream::TokenIndex class_token;
    AstTypeName* super_opt;

    inline AstClassDeclaration(StoragePool* p)
        : AstDeclaredType(CLASS)
        , pool(p)
    {}
    ~AstClassDeclaration() {}

    inline AstTypeName*& Interface(unsigned i) { return (*interfaces)[i]; }
    inline unsigned NumInterfaces()
    {
        return interfaces ? interfaces -> Length() : 0;
    }
    inline void AllocateInterfaces(unsigned estimate = 1);
    inline void AddInterface(AstTypeName*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return modifiers_opt ? modifiers_opt -> LeftToken() : class_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return class_body -> right_brace_token;
    }
};


//
// Covers all array initializer expressions, including those added by JSR 175.
//
class AstArrayInitializer : public AstMemberValue
{
    StoragePool* pool;
    AstArray<AstMemberValue*>* variable_initializers;

public:
    LexStream::TokenIndex left_brace_token;
    LexStream::TokenIndex right_brace_token;

    inline AstArrayInitializer(StoragePool* p)
        : AstMemberValue(ARRAY_INITIALIZER)
        , pool(p)
    {}
    ~AstArrayInitializer() {}

    inline AstMemberValue*& VariableInitializer(unsigned i)
    {
        return (*variable_initializers)[i];
    }
    inline unsigned NumVariableInitializers()
    {
        return variable_initializers ? variable_initializers -> Length() : 0;
    }
    inline void AllocateVariableInitializers(unsigned estimate = 1);
    inline void AddVariableInitializer(AstMemberValue*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return left_brace_token; }
    virtual LexStream::TokenIndex RightToken() { return right_brace_token; }
};


//
// VariableDeclaratorId --> <VARIABLE_DECLARATOR_NAME, identifier_token,
// Brackets>
//
class AstVariableDeclaratorId : public Ast
{
public:
    LexStream::TokenIndex identifier_token;
    AstBrackets* brackets_opt;

    inline AstVariableDeclaratorId()
        : Ast(VARIABLE_DECLARATOR_NAME)
    {}
    ~AstVariableDeclaratorId() {}

    inline unsigned NumBrackets()
    {
        return brackets_opt ? brackets_opt -> dims : 0;
    }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return identifier_token; }
    virtual LexStream::TokenIndex RightToken()
    {
        return brackets_opt ? brackets_opt -> right_bracket_token
            : identifier_token;
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
    VariableSymbol* symbol;

    // when true, this variable signals that the variable_initializer_opt
    // for this variable is currently being evaluated
    bool pending;

    AstVariableDeclaratorId* variable_declarator_name;
    Ast* variable_initializer_opt;

    inline AstVariableDeclarator()
        : AstStatement(VARIABLE_DECLARATOR, true, true)
    {}
    ~AstVariableDeclarator() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return variable_declarator_name -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return variable_initializer_opt
            ? variable_initializer_opt -> RightToken()
            : variable_declarator_name -> RightToken();
    }
};


//
// FieldDeclaration --> <FIELD, VariableModifiers, Type, VariableDeclarators,
// ;_token>
//
// FieldModifier --> Modifier (PUBLIC, PROTECTED, PRIVATE, FINAL, STATIC,
// TRANSIENT or VOLATILE)
//
class AstFieldDeclaration : public AstDeclared
{
    StoragePool* pool;
    AstArray<AstVariableDeclarator*>* variable_declarators;

public:
    enum FieldDeclarationTag
    {
        NONE,
        STATIC
    };

    AstType* type;
    LexStream::TokenIndex semicolon_token;

    inline AstFieldDeclaration(StoragePool* p)
        : AstDeclared(FIELD)
        , pool(p)
    {}
    ~AstFieldDeclaration() {}

    inline void MarkStatic() { other_tag = STATIC; }

    inline AstVariableDeclarator*& VariableDeclarator(unsigned i)
    {
        return (*variable_declarators)[i];
    }
    inline unsigned NumVariableDeclarators()
    {
        return variable_declarators ? variable_declarators -> Length() : 0;
    }
    inline void AllocateVariableDeclarators(unsigned estimate = 1);
    inline void AddVariableDeclarator(AstVariableDeclarator*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return modifiers_opt ? modifiers_opt -> LeftToken()
            : type -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// FormalParameter --> <PARAMETER, Type, VariableDeclaratorId>
//
class AstFormalParameter : public Ast
{
public:
    AstModifiers* modifiers_opt;
    AstType* type;
    LexStream::TokenIndex ellipsis_token_opt;
    AstVariableDeclarator* formal_declarator;

    inline AstFormalParameter()
        : Ast(PARAMETER)
    {}
    ~AstFormalParameter() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return modifiers_opt ? modifiers_opt -> LeftToken()
            : type -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return formal_declarator -> RightToken();
    }
};


//
// MethodDeclarator --> <METHOD_DECLARATOR, identifier_token, (_token,
// FormalParameters, )_token, Brackets>
//
class AstMethodDeclarator : public Ast
{
    StoragePool* pool;
    AstArray<AstFormalParameter*>* formal_parameters;

public:
    LexStream::TokenIndex identifier_token;
    LexStream::TokenIndex left_parenthesis_token;
    LexStream::TokenIndex right_parenthesis_token;
    AstBrackets* brackets_opt;

    inline AstMethodDeclarator(StoragePool* p)
        : Ast(METHOD_DECLARATOR)
        , pool(p)
    {}
    ~AstMethodDeclarator() {}

    inline AstFormalParameter*& FormalParameter(unsigned i)
    {
        return (*formal_parameters)[i];
    }
    inline unsigned NumFormalParameters()
    {
        return formal_parameters ? formal_parameters -> Length() : 0;
    }
    inline void AllocateFormalParameters(unsigned estimate = 1);
    inline void AddFormalParameter(AstFormalParameter*);

    inline unsigned NumBrackets()
    {
        return brackets_opt ? brackets_opt -> dims : 0;
    }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return identifier_token; }
    virtual LexStream::TokenIndex RightToken()
    {
        return brackets_opt ? brackets_opt -> right_bracket_token
            : right_parenthesis_token;
    }
};


//
// This class represents a method body, for methods, constructors, and
// initializers. It is basically a block, with the addition of an explicit
// constructor invocation (used only in the context of constructors, NULL
// otherwise).
//
class AstMethodBody : public AstBlock
{
public:
    AstStatement* explicit_constructor_opt;

    inline AstMethodBody(StoragePool* p)
        : AstBlock(p, METHOD_BODY, true)
    {
        no_braces = true;
    }
    ~AstMethodBody() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);
    // Inherited LeftToken(), RightToken() are adequate.
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
class AstMethodDeclaration : public AstDeclared
{
    StoragePool* pool;
    AstArray<AstTypeName*>* throws;

public:
    MethodSymbol* method_symbol;

    AstType* type;
    AstMethodDeclarator* method_declarator;
    AstMethodBody* method_body_opt;

    inline AstMethodDeclaration(StoragePool* p)
        : AstDeclared(METHOD)
        , pool(p)
    {}
    ~AstMethodDeclaration() {}

    bool IsValid() { return method_symbol != NULL; }

    bool IsSignature() { return ! method_body_opt; }

    inline AstTypeName*& Throw(unsigned i) { return (*throws)[i]; }
    inline unsigned NumThrows() { return throws ? throws -> Length() : 0; }
    inline void AllocateThrows(unsigned estimate = 1);
    inline void AddThrow(AstTypeName*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return modifiers_opt ? modifiers_opt -> LeftToken()
            : type -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return method_body_opt ? method_body_opt -> right_brace_token
            : method_declarator -> RightToken() + 1;
    }
};


//
// This class represents static and instance initializers. It also accepts
// other modifiers, to give a nicer error message.
//
class AstInitializerDeclaration : public AstDeclared
{
public:
    enum InitializerDeclarationTag
    {
        NONE,
        STATIC
    };

    AstMethodBody* block;

    inline AstInitializerDeclaration()
        : AstDeclared(INITIALIZER)
    {}
    ~AstInitializerDeclaration() {}

    inline void MarkStatic() { other_tag = STATIC; }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return modifiers_opt ? modifiers_opt -> LeftToken()
            : block -> left_brace_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return block -> right_brace_token;
    }
};


//
// Represents the arguments of AstThisCall, AstSuperCall, AstMethodInvocation,
// and AstClassCreationExpression. For convenience, the need to add null
// argument or pass shadow parameters is contained here, even though not all
// the calling instances can use these features.
//
class AstArguments : public Ast
{
    StoragePool* pool;
    AstArray<AstExpression*>* arguments;
    AstArray<AstName*>* shadow_arguments;

public:
    LexStream::TokenIndex left_parenthesis_token;
    LexStream::TokenIndex right_parenthesis_token;

    inline AstArguments(StoragePool* p,
                        LexStream::TokenIndex l, LexStream::TokenIndex r)
        : Ast(ARGUMENTS)
        , pool(p)
        , left_parenthesis_token(l)
        , right_parenthesis_token(r)
    {}
    ~AstArguments() {}

    inline AstExpression*& Argument(unsigned i) { return (*arguments)[i]; }
    inline unsigned NumArguments()
    {
        return arguments ? arguments -> Length() : 0;
    }
    inline void AllocateArguments(unsigned estimate = 1);
    inline void AddArgument(AstExpression*);

    inline AstName*& LocalArgument(unsigned i)
    {
        return (*shadow_arguments)[i];
    }
    inline unsigned NumLocalArguments()
    {
        return shadow_arguments ? shadow_arguments -> Length() : 0;
    }
    inline void AllocateLocalArguments(unsigned estimate = 1);
    inline void AddLocalArgument(AstName*);

    inline void AddNullArgument() { other_tag = true; }
    inline bool NeedsExtraNullArgument() { return (bool) other_tag; }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

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
// Represents an explicit call to another constructor in this class.
//
class AstThisCall : public AstStatement
{
public:
    MethodSymbol* symbol;

    LexStream::TokenIndex this_token;
    AstArguments* arguments;
    LexStream::TokenIndex semicolon_token;

    inline AstThisCall()
        : AstStatement(THIS_CALL, true, true)
    {}
    ~AstThisCall() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return this_token; }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// Represents an explicit call to a superconstructor.
//
class AstSuperCall : public AstStatement
{
public:
    MethodSymbol* symbol;

    AstExpression* base_opt;
    LexStream::TokenIndex super_token;
    AstArguments* arguments;
    LexStream::TokenIndex semicolon_token;

    inline AstSuperCall()
        : AstStatement(SUPER_CALL, true, true)
    {}
    ~AstSuperCall() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return base_opt ? base_opt -> LeftToken() : super_token;
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
class AstConstructorDeclaration : public AstDeclared
{
    StoragePool* pool;
    AstArray<AstTypeName*>* throws;

public:
    MethodSymbol* constructor_symbol;
    int index; // Used in depend.cpp to detect cycles.

    AstMethodDeclarator* constructor_declarator;
    AstMethodBody* constructor_body;

    inline AstConstructorDeclaration(StoragePool* p)
        : AstDeclared(CONSTRUCTOR)
        , pool(p)
        , index(ConstructorCycleChecker::OMEGA)
    {}
    ~AstConstructorDeclaration() {}

    bool IsValid() { return constructor_symbol != NULL; }

    inline AstTypeName*& Throw(unsigned i) { return (*throws)[i]; }
    inline unsigned NumThrows() { return throws ? throws -> Length() : 0; }
    inline void AllocateThrows(unsigned estimate = 1);
    inline void AddThrow(AstTypeName*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return modifiers_opt ? modifiers_opt -> LeftToken()
            : constructor_declarator -> identifier_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return constructor_body -> right_brace_token;
    }
};


//
// InterfaceDeclaration --> <INTERFACE, Interfacemodifiers, interface_token,
// identifier_token, ExtendsInterfaces, {_token, InterfaceMemberDeclarations,
// }_token>
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
class AstInterfaceDeclaration : public AstDeclaredType
{
    StoragePool* pool;
    AstArray<AstTypeName*>* interfaces;

public:
    LexStream::TokenIndex interface_token;

    inline AstInterfaceDeclaration(StoragePool* p)
        : AstDeclaredType(INTERFACE)
        , pool(p)
    {}
    ~AstInterfaceDeclaration() {}

    inline AstTypeName*& Interface(unsigned i)
    {
        return (*interfaces)[i];
    }
    inline unsigned NumInterfaces()
    {
        return interfaces ? interfaces -> Length() : 0;
    }
    inline void AllocateInterfaces(unsigned estimate = 1);
    inline void AddInterface(AstTypeName*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return modifiers_opt ? modifiers_opt -> LeftToken() : interface_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return class_body -> right_brace_token;
    }
};


//
// Represents a local variable declaration statement.
//
class AstLocalVariableStatement : public AstStatement
{
    StoragePool* pool;
    AstArray<AstVariableDeclarator*>* variable_declarators;

public:
    AstModifiers* modifiers_opt;
    AstType* type;
    LexStream::TokenIndex semicolon_token_opt;

    inline AstLocalVariableStatement(StoragePool* p)
        : AstStatement(LOCAL_VARIABLE_DECLARATION)
        , pool(p)
    {}
    ~AstLocalVariableStatement() {}

    inline AstVariableDeclarator*& VariableDeclarator(unsigned i)
    {
        return (*variable_declarators)[i];
    }
    inline unsigned NumVariableDeclarators()
    {
        return variable_declarators ? variable_declarators -> Length() : 0;
    }
    inline void AllocateVariableDeclarators(unsigned estimate = 1);
    inline void AddVariableDeclarator(AstVariableDeclarator*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return modifiers_opt ? modifiers_opt -> LeftToken()
            : type -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return semicolon_token_opt ? semicolon_token_opt
            : (VariableDeclarator(NumVariableDeclarators() - 1) ->
               RightToken());
    }
};


//
// Represents a local class declaration statement.
//
class AstLocalClassStatement : public AstStatement
{
public:
    AstClassDeclaration* declaration;

    inline AstLocalClassStatement(AstClassDeclaration* decl)
        : AstStatement(LOCAL_CLASS, false, true)
        , declaration(decl)
    {}
    ~AstLocalClassStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return declaration -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return declaration -> class_body -> right_brace_token;
    }
};


//
// The parser always makes blocks for the enclosed statements, so we denote
// that here (even though any statement is legal).
//
class AstIfStatement : public AstStatement
{
public:
    LexStream::TokenIndex if_token;
    AstExpression* expression;
    AstBlock* true_statement;
    AstBlock* false_statement_opt;

    inline AstIfStatement()
        : AstStatement(IF)
    {}
    ~AstIfStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return if_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return false_statement_opt ? false_statement_opt -> RightToken()
            : true_statement -> RightToken();
    }
};


//
// EmptyStatement --> <EMPTY_STATEMENT, Label_opt, ;_token>
//
class AstEmptyStatement : public AstStatement
{
public:
    LexStream::TokenIndex semicolon_token;

    inline AstEmptyStatement(LexStream::TokenIndex token)
        : AstStatement(EMPTY_STATEMENT)
        , semicolon_token(token)
    {}
    ~AstEmptyStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return semicolon_token; }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// ExpressionStatement --> <EXPRESSION_STATEMENT, Label_opt, Expression,
// ;_token_opt>
//
class AstExpressionStatement : public AstStatement
{
public:
    AstExpression* expression;
    LexStream::TokenIndex semicolon_token_opt;

    inline AstExpressionStatement()
        : AstStatement(EXPRESSION_STATEMENT)
    {}
    ~AstExpressionStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return expression -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return semicolon_token_opt ? semicolon_token_opt
            : expression -> RightToken();
    }
};


//
// Represents "case <constant> :" and "default :".
//
class AstSwitchLabel : public Ast
{
public:
    LexStream::TokenIndex case_token;
    AstExpression* expression_opt;
    LexStream::TokenIndex colon_token;

    //
    // The sorted index of this label in the overall switch. Default cases
    // are set to NumCases().
    //
    unsigned map_index;

    inline AstSwitchLabel()
        : Ast(SWITCH_LABEL)
    {}
    ~AstSwitchLabel() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return case_token; }
    virtual LexStream::TokenIndex RightToken() { return colon_token; }
};


//
// SwitchBlockStatement --> <SWITCH_BLOCK, SwitchLabels, BlockStatements>
//
class AstSwitchBlockStatement : public AstBlock
{
    AstArray<AstSwitchLabel*>* switch_labels;

public:
    inline AstSwitchBlockStatement(StoragePool* p)
        : AstBlock(p, SWITCH_BLOCK)
    {
        no_braces = true;
    }
    ~AstSwitchBlockStatement() {}

    inline AstSwitchLabel*& SwitchLabel(unsigned i)
    {
        return (*switch_labels)[i];
    }
    inline unsigned NumSwitchLabels()
    {
        return switch_labels ? switch_labels -> Length() : 0;
    }
    inline void AllocateSwitchLabels(unsigned estimate = 1);
    inline void AddSwitchLabel(AstSwitchLabel*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);
    virtual LexStream::TokenIndex LeftToken()
    {
        return SwitchLabel(0) -> case_token;
    }
    // Inherited RightToken() is adequate.
};


//
// This structure allows a switch statement to sort its case labels. It should
// be a plain-old-data type (POD) for efficient copying.
//
struct CaseElement
{
    unsigned block_index; // which SwitchBlockStatement
    unsigned case_index; // which case label within the block
    i4 value; // the value of the case's expression

    //
    // This keeps the sort stable, so that duplicates stay later in the list.
    //
    inline bool operator<(CaseElement& right)
    {
        return value < right.value ||
            (value == right.value &&
             (block_index < right.block_index ||
              (block_index == right.block_index &&
               case_index < right.case_index)));
    }
};


//
// SwitchStatement --> <SWITCH, Label_opt, switch_token, Expression, {_token,
// SwitchBlockStatements, SwitchLabels_opt, }_token>
//
class AstSwitchStatement : public AstStatement
{
    StoragePool* pool;
    //
    // The sorted list of case label values. Index 0 is reserved for the
    // default case. Index 1 - size are for the case labels, and get sorted.
    //
    CaseElement** cases;
    unsigned num_cases;
#ifdef JIKES_DEBUG
    unsigned max_cases; // bounds check only when debugging
#endif // JIKES_DEBUG

public:
    LexStream::TokenIndex switch_token;
    AstExpression* expression;
    AstBlock* switch_block;

    inline AstSwitchStatement(StoragePool* p)
        : AstStatement(SWITCH)
        , pool(p)
    {}
    ~AstSwitchStatement() {}

    inline CaseElement*& Case(unsigned i)
    {
        assert(i < num_cases);
        return cases[i + 1];
    }
    inline CaseElement*& DefaultCase() { return cases[0]; }
    inline unsigned NumCases() { return num_cases; }
    inline void AllocateCases(unsigned estimate = 1);
    inline void AddCase(CaseElement*);

    inline AstSwitchBlockStatement* Block(unsigned i)
    {
        return (AstSwitchBlockStatement*) switch_block -> Statement(i);
    }
    inline unsigned NumBlocks() { return switch_block -> NumStatements(); }

    void SortCases();
    CaseElement* CaseForValue(i4 value);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return switch_token; }
    virtual LexStream::TokenIndex RightToken()
    {
        return switch_block -> right_brace_token;
    }
};


//
// WhileStatement --> <WHILE, Label_opt, while_token, Expression, Statement>
//
class AstWhileStatement : public AstStatement
{
public:
    LexStream::TokenIndex while_token;
    AstExpression* expression;
    AstBlock* statement;

    inline AstWhileStatement()
        : AstStatement(WHILE)
    {}
    ~AstWhileStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return while_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return statement -> right_brace_token;
    }
};


//
// DoStatement --> <DO, Label_opt, do_token, Expression, Statement, ;_token>
//
class AstDoStatement : public AstStatement
{
public:
    LexStream::TokenIndex do_token;
    AstBlock* statement;
    LexStream::TokenIndex while_token;
    AstExpression* expression;
    LexStream::TokenIndex semicolon_token;

    inline AstDoStatement()
        : AstStatement(DO)
    {}
    ~AstDoStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return do_token;
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// ForStatement --> <FOR, Label_opt, for_token, ForInits, Expression_opt,
// ForUpdates, Statement>
//
// ForInit --> ExpressionStatement
//           | LocalVariableStatement
//
// ForUpdate --> ExpressionStatement
//
class AstForStatement : public AstStatement
{
    StoragePool* pool;
    AstArray<AstStatement*>* for_init_statements;
    AstArray<AstExpressionStatement*>* for_update_statements;

public:
    LexStream::TokenIndex for_token;
    AstExpression* end_expression_opt;
    AstBlock* statement;

    inline AstForStatement(StoragePool* p)
        : AstStatement(FOR)
        , pool(p)
    {}
    ~AstForStatement() {}

    inline AstStatement*& ForInitStatement(unsigned i)
    {
        return (*for_init_statements)[i];
    }
    inline unsigned NumForInitStatements()
    {
        return for_init_statements ? for_init_statements -> Length() : 0;
    }
    inline void AllocateForInitStatements(unsigned estimate = 1);
    inline void AddForInitStatement(AstStatement*);

    inline AstExpressionStatement*& ForUpdateStatement(unsigned i)
    {
        return (*for_update_statements)[i];
    }
    inline unsigned NumForUpdateStatements()
    {
        return for_update_statements ? for_update_statements -> Length() : 0;
    }
    inline void AllocateForUpdateStatements(unsigned estimate = 1);
    inline void AddForUpdateStatement(AstExpressionStatement*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return for_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return statement -> right_brace_token;
    }
};


//
// ForeachStatement is added in JDK 1.5 by JSR 201.  It has the syntax
// "for (FormalParameter : expression) statement", where expression must
// be an array type or an instance of java.lang.Iterable. The parser already
// wrapped the statement in a block.
//
class AstForeachStatement : public AstStatement
{
public:
    LexStream::TokenIndex for_token;
    AstFormalParameter* formal_parameter;
    AstExpression* expression;
    AstBlock* statement;

    inline AstForeachStatement()
        : AstStatement(FOREACH)
    {}
    ~AstForeachStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return for_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return statement -> right_brace_token;
    }
};


//
// BreakStatement --> <BREAK, Label_opt, break_token, identifier_token_opt,
// ;_token>
//
class AstBreakStatement : public AstStatement
{
public:
    LexStream::TokenIndex break_token;
    LexStream::TokenIndex identifier_token_opt;
    LexStream::TokenIndex semicolon_token;
    unsigned nesting_level;

    inline AstBreakStatement()
        : AstStatement(BREAK)
    {}
    ~AstBreakStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return break_token;
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// ContinueStatement --> <CONTINUE, Label_opt, continue_token, SimpleName_opt,
// ;_token>
//
class AstContinueStatement : public AstStatement
{
public:
    LexStream::TokenIndex continue_token;
    LexStream::TokenIndex identifier_token_opt;
    LexStream::TokenIndex semicolon_token;
    unsigned nesting_level;

    inline AstContinueStatement()
        : AstStatement(CONTINUE)
    {}
    ~AstContinueStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return continue_token;
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// ReturnStatement --> <RETURN, Label_opt, return_token, Expression_opt,
// ;_token>
//
class AstReturnStatement : public AstStatement
{
public:
    LexStream::TokenIndex return_token;
    AstExpression* expression_opt;
    LexStream::TokenIndex semicolon_token;

    inline AstReturnStatement()
        : AstStatement(RETURN)
    {}
    ~AstReturnStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

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
    AstExpression* expression;
    LexStream::TokenIndex semicolon_token;

    inline AstThrowStatement()
        : AstStatement(THROW)
    {}
    ~AstThrowStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return throw_token;
    }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// SynchronizedStatement --> <SYNCHRONIZED_STATEMENT, Label_opt,
// synchronized_token, Expression, Block>
//
class AstSynchronizedStatement : public AstStatement
{
public:
    LexStream::TokenIndex synchronized_token;
    AstExpression* expression;
    AstBlock* block;

    inline AstSynchronizedStatement()
        : AstStatement(SYNCHRONIZED_STATEMENT)
    {}
    ~AstSynchronizedStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return synchronized_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return block -> right_brace_token;
    }
};


//
// AssertStatement --> <ASSERT, Label_opt, assert_token, Expression, ;_token>
//                 --> <ASSERT, Label_opt, assert_token, Expression, :_token,
// Expression, ;_token>
//
class AstAssertStatement : public AstStatement
{
public:
    LexStream::TokenIndex assert_token;
    LexStream::TokenIndex semicolon_token;
    AstExpression* condition;
    AstExpression* message_opt;

    VariableSymbol* assert_variable;

    inline AstAssertStatement()
        : AstStatement(ASSERT)
    {}
    ~AstAssertStatement() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return assert_token; }
    virtual LexStream::TokenIndex RightToken() { return semicolon_token; }
};


//
// CatchClause --> <CATCH, catch_token, FormalParameter, Block>
//
class AstCatchClause : public Ast
{
public:
    VariableSymbol* parameter_symbol;

    LexStream::TokenIndex catch_token;
    AstFormalParameter* formal_parameter;
    AstBlock* block;

    inline AstCatchClause()
        : Ast(CATCH)
    {}
    ~AstCatchClause() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return catch_token; }
    virtual LexStream::TokenIndex RightToken()
    {
        return block -> right_brace_token;
    }
};


//
// FinallyClause --> <FINALLY, finally_token, Block>
//
class AstFinallyClause : public Ast
{
public:
    LexStream::TokenIndex finally_token;
    AstBlock* block;

    inline AstFinallyClause()
        : Ast(FINALLY)
    {}
    ~AstFinallyClause() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return finally_token; }
    virtual LexStream::TokenIndex RightToken()
    {
        return block -> right_brace_token;
    }
};


//
// TryStatement --> <TRY, Label_opt, try-token, Block CatchClauses,
// FinallyClause_opt>
//
class AstTryStatement : public AstStatement
{
    StoragePool* pool;
    AstArray<AstCatchClause*>* catch_clauses;

public:
    LexStream::TokenIndex try_token;
    AstBlock* block;
    AstFinallyClause* finally_clause_opt;
    bool processing_try_block;

    inline AstTryStatement(StoragePool* p)
        : AstStatement(TRY)
        , pool(p)
    {}
    ~AstTryStatement() {}

    inline AstCatchClause*& CatchClause(unsigned i)
    {
        return (*catch_clauses)[i];
    }
    inline unsigned NumCatchClauses()
    {
        return catch_clauses ? catch_clauses -> Length() : 0;
    }
    inline void AllocateCatchClauses(unsigned estimate = 1);
    inline void AddCatchClause(AstCatchClause*);

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

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
        return finally_clause_opt ? finally_clause_opt -> RightToken()
            : CatchClause(NumCatchClauses() - 1) -> RightToken();
    }
};


//
// IntegerLiteral --> <INTEGER_LITERAL, integer_literal_token, value>
//
class AstIntegerLiteral : public AstExpression
{
public:
    LexStream::TokenIndex integer_literal_token;

    inline AstIntegerLiteral(LexStream::TokenIndex token)
        : AstExpression(INTEGER_LITERAL)
        , integer_literal_token(token)
    {}
    ~AstIntegerLiteral() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return integer_literal_token; }
    virtual LexStream::TokenIndex RightToken()
    {
        return integer_literal_token;
    }
};


//
// LongLiteral --> <LONG_LITERAL, long_literal_token, value>
//
class AstLongLiteral : public AstExpression
{
public:
    LexStream::TokenIndex long_literal_token;

    inline AstLongLiteral(LexStream::TokenIndex token)
        : AstExpression(LONG_LITERAL)
        , long_literal_token(token)
    {}
    ~AstLongLiteral() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return long_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return long_literal_token; }
};


//
// FloatLiteral --> <FLOAT_LITERAL, Literal, value>
//
class AstFloatLiteral : public AstExpression
{
public:
    LexStream::TokenIndex float_literal_token;

    inline AstFloatLiteral(LexStream::TokenIndex token)
        : AstExpression(FLOAT_LITERAL)
        , float_literal_token(token)
    {}
    ~AstFloatLiteral() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return float_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return float_literal_token; }
};


//
// DoubleLiteral --> <DOUBLE_LITERAL, Literal, value>
//
class AstDoubleLiteral : public AstExpression
{
public:
    LexStream::TokenIndex double_literal_token;

    inline AstDoubleLiteral(LexStream::TokenIndex token)
        : AstExpression(DOUBLE_LITERAL)
        , double_literal_token(token)
    {}
    ~AstDoubleLiteral() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return double_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return double_literal_token; }
};


//
// TrueLiteral --> <TRUE_LITERAL, Literal, value>
//
class AstTrueLiteral : public AstExpression
{
public:
    LexStream::TokenIndex true_literal_token;

    inline AstTrueLiteral(LexStream::TokenIndex token)
        : AstExpression(TRUE_LITERAL)
        , true_literal_token(token)
    {}
    ~AstTrueLiteral() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return true_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return true_literal_token; }
};


//
// FalseLiteral --> <FALSE_LITERAL, Literal, value>
//
class AstFalseLiteral : public AstExpression
{
public:
    LexStream::TokenIndex false_literal_token;

    inline AstFalseLiteral(LexStream::TokenIndex token)
        : AstExpression(FALSE_LITERAL)
        , false_literal_token(token)
    {}
    ~AstFalseLiteral() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return false_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return false_literal_token; }
};


//
// StringLiteral --> <STRING_LITERAL, Literal, value>
//
class AstStringLiteral : public AstExpression
{
public:
    LexStream::TokenIndex string_literal_token;

    inline AstStringLiteral(LexStream::TokenIndex token)
        : AstExpression(STRING_LITERAL)
        , string_literal_token(token)
    {}
    ~AstStringLiteral() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return string_literal_token; }
    virtual LexStream::TokenIndex RightToken() { return string_literal_token; }
};


//
// CharacterLiteral --> <CHARACTER_LITERAL, literal_token, value>
//
class AstCharacterLiteral : public AstExpression
{
public:
    LexStream::TokenIndex character_literal_token;

    inline AstCharacterLiteral(LexStream::TokenIndex token)
        : AstExpression(CHARACTER_LITERAL)
        , character_literal_token(token)
    {}
    ~AstCharacterLiteral() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

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

    inline AstNullLiteral(LexStream::TokenIndex token)
        : AstExpression(NULL_LITERAL)
        , null_token(token)
    {}
    ~AstNullLiteral() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return null_token; }
    virtual LexStream::TokenIndex RightToken() { return null_token; }
};


//
// Represents class literals.
//
class AstClassLiteral : public AstExpression
{
public:
    AstType* type;
    LexStream::TokenIndex class_token;

    //
    // If this expression requires a caching variable and a call to class$(),
    // the resolution holds the needed class$xxx or array$xxx cache.
    //
    AstExpression* resolution_opt;

    inline AstClassLiteral(LexStream::TokenIndex token)
        : AstExpression(CLASS_LITERAL)
        , class_token(token)
    {}
    ~AstClassLiteral() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return type -> LeftToken(); }
    virtual LexStream::TokenIndex RightToken() { return class_token; }
};


//
// Represents qualified and simple 'this'.
//
class AstThisExpression : public AstExpression
{
public:
    AstTypeName* base_opt;
    LexStream::TokenIndex this_token;

    //
    // If this expression accesses an enclosing instance, the resolution
    // holds the needed chain of "this$0" traversals.
    //
    AstExpression* resolution_opt;

    inline AstThisExpression(LexStream::TokenIndex token)
        : AstExpression(THIS_EXPRESSION)
        , this_token(token)
    {}
    ~AstThisExpression() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return base_opt ? base_opt -> LeftToken() : this_token;
    }
    virtual LexStream::TokenIndex RightToken() { return this_token; }
};


//
// Represents qualified and simple 'super'.
//
class AstSuperExpression : public AstExpression
{
public:
    AstTypeName* base_opt;
    LexStream::TokenIndex super_token;

    //
    // If this expression accesses an enclosing instance, the resolution
    // holds the needed chain of "this$0" traversals.
    //
    AstExpression* resolution_opt;

    inline AstSuperExpression(LexStream::TokenIndex token)
        : AstExpression(SUPER_EXPRESSION)
        , super_token(token)
    {}
    ~AstSuperExpression() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return base_opt ? base_opt -> LeftToken() : super_token;
    }
    virtual LexStream::TokenIndex RightToken() { return super_token; }
};


//
// ParenthesizedExpression --> <PARENTHESIZED_EXPRESSION, (_token, Expression,
// )_token>
//
class AstParenthesizedExpression : public AstExpression
{
public:
    LexStream::TokenIndex left_parenthesis_token;
    AstExpression* expression;
    LexStream::TokenIndex right_parenthesis_token;

    inline AstParenthesizedExpression()
        : AstExpression(PARENTHESIZED_EXPRESSION)
    {}
    ~AstParenthesizedExpression() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

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
// ClassCreationExpression represents a class instance creation (keyword new,
// including anonymous classes). Also see ArrayCreationExpression. Sometimes,
// during semantic analysis an artificial base_opt expression is constructed.
// In such a case, the user can determine this condition by testing
// base_opt -> generated.
//
class AstClassCreationExpression : public AstExpression
{
public:
    AstExpression* base_opt;
    LexStream::TokenIndex new_token;
    AstTypeName* class_type;
    AstArguments* arguments;
    AstClassBody* class_body_opt;

    //
    // For anonymous classes, we resolve the original statement into a new
    // one that does not have a class_body_opt. This is necessary to get
    // the parameters called in the correct order.
    //
    AstClassCreationExpression* resolution_opt;

    inline AstClassCreationExpression()
        : AstExpression(CLASS_CREATION)
    {}
    ~AstClassCreationExpression() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return base_opt ? base_opt -> LeftToken() : new_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return class_body_opt ? class_body_opt -> right_brace_token
            : arguments -> right_parenthesis_token;
    }
};


//
// DimExpr --> <DIM, [_token, Expression, ]_token>
//
class AstDimExpr : public Ast
{
public:
    LexStream::TokenIndex left_bracket_token;
    AstExpression* expression;
    LexStream::TokenIndex right_bracket_token;

    inline AstDimExpr()
        : Ast(DIM)
    {}
    ~AstDimExpr() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return left_bracket_token; }
    virtual LexStream::TokenIndex RightToken() { return right_bracket_token; }
};


//
// ArrayCreationExpression --> <ARRAY_CREATION, new_token, Type, DimExprs,
// Brackets>
//
class AstArrayCreationExpression : public AstExpression
{
    StoragePool* pool;
    AstArray<AstDimExpr*>* dim_exprs;

public:
    LexStream::TokenIndex new_token;
    AstType* array_type;
    AstBrackets* brackets_opt;
    AstArrayInitializer* array_initializer_opt;

    inline AstArrayCreationExpression(StoragePool* p)
        : AstExpression(ARRAY_CREATION)
        , pool(p)
    {}
    ~AstArrayCreationExpression() {}

    inline AstDimExpr*& DimExpr(unsigned i) { return (*dim_exprs)[i]; }
    inline unsigned NumDimExprs()
    {
        return dim_exprs ? dim_exprs -> Length() : 0;
    }
    inline void AllocateDimExprs(unsigned estimate = 1);
    inline void AddDimExpr(AstDimExpr*);

    inline unsigned NumBrackets()
    {
        return brackets_opt ? brackets_opt -> dims : 0;
    }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return new_token; }
    virtual LexStream::TokenIndex RightToken()
    {
        return array_initializer_opt
            ? array_initializer_opt -> right_brace_token
            : brackets_opt ? brackets_opt -> right_bracket_token
            : DimExpr(NumDimExprs() - 1) -> right_bracket_token;
    }
};


//
// FieldAccess --> <DOT, Primary, ._token, Identifier>
//
class AstFieldAccess : public AstExpression
{
public:
    AstExpression* base; // Not AstName.
    LexStream::TokenIndex identifier_token;

    //
    // If the base expression of FieldAccess expression is of the form
    // type.this.X, where X is a private variable that is a member of an
    // outer class, then we resolve it into a method call to the read_mehod
    // that gives access to X.
    //
    AstExpression* resolution_opt;

    inline AstFieldAccess()
        : AstExpression(DOT)
    {}
    ~AstFieldAccess() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken() { return base -> LeftToken(); }
    virtual LexStream::TokenIndex RightToken() { return identifier_token; }
};


//
// Represents a method call.
//
class AstMethodInvocation : public AstExpression
{
public:
    AstExpression* base_opt;
    LexStream::TokenIndex identifier_token;
    AstArguments* arguments;

    //
    // When a method refers to a member in an enclosing scope,
    // it is mapped into a new expression that creates a path to
    // the member in question.
    //
    AstExpression* resolution_opt;

    inline AstMethodInvocation(LexStream::TokenIndex t)
        : AstExpression(CALL)
        , identifier_token(t)
    {}
    ~AstMethodInvocation() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return base_opt ? base_opt -> LeftToken() : identifier_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return arguments -> right_parenthesis_token;
    }
};


//
// ArrayAccess --> <ARRAY_ACCESS, Base, [_token, Expression, ]_token>
//
class AstArrayAccess : public AstExpression
{
public:
    AstExpression* base;
    LexStream::TokenIndex left_bracket_token;
    AstExpression* expression;
    LexStream::TokenIndex right_bracket_token;

    inline AstArrayAccess()
        : AstExpression(ARRAY_ACCESS)
    {}
    ~AstArrayAccess() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

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

    AstExpression* expression;
    LexStream::TokenIndex post_operator_token;

    //
    // When the left-hand side of an assignment is a name that refers
    // to a private field in an enclosing scope, the access method
    // that gives write-permission to that field is recorded here.
    //
    MethodSymbol* write_method;

    inline AstPostUnaryExpression(PostUnaryExpressionTag tag)
        : AstExpression(POST_UNARY)
    {
        other_tag = tag;
    }
    ~AstPostUnaryExpression() {}

    inline PostUnaryExpressionTag Tag()
    {
        return (PostUnaryExpressionTag) other_tag;
    }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

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

    LexStream::TokenIndex pre_operator_token;
    AstExpression* expression;

    //
    // When the left-hand side of an assignment is a name that refers
    // to a private field in an enclosing scope, the access method
    // that gives write-permission to that field is recorded here.
    //
    MethodSymbol* write_method;

    inline AstPreUnaryExpression(PreUnaryExpressionTag tag)
        : AstExpression(PRE_UNARY)
    {
        other_tag = tag;
    }
    ~AstPreUnaryExpression() {}

    inline PreUnaryExpressionTag Tag()
    {
        return (PreUnaryExpressionTag) other_tag;
    }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

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
// CastExpression --> <castkind, (_token_opt, Type, )_token_opt, Expression>
//
// NOTE that the optional symbols above are absent only when the compiler
// inserts a CAST conversion node into the program.
//
class AstCastExpression : public AstExpression
{
public:
    LexStream::TokenIndex left_parenthesis_token;
    AstType* type;
    LexStream::TokenIndex right_parenthesis_token;
    AstExpression* expression;

    inline AstCastExpression()
        : AstExpression(CAST)
    {}
    ~AstCastExpression() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return left_parenthesis_token;
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return expression -> RightToken();
    }
};


//
// BinaryExpression --> <BINARY, BinaryTag, Expression, BinaryOperator,
//                      Expression>
//
// BinaryTag --> STAR | SLASH | MOD | PLUS | MINUS | LEFT_SHIFT | RIGHT_SHIFT |
//               UNSIGNED_RIGHT_SHIFT | LESS | GREATER |
//               LESS_EQUAL | GREATER_EQUAL | EQUAL_EQUAL | NOT_EQUAL |
//               AND | XOR | IOR | AND_AND | OR_OR
//
// BinaryOperator --> *_token | /_token | %_token | +_token | -_token |
//                    <<_token | >>_token | >>>_token | <_token | >_token |
//                    <=_token | >=_token | ==_token | !=_token | &_token |
//                    ^_token | |_token | &&_token | ||_token
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

    AstExpression* left_expression;
    LexStream::TokenIndex binary_operator_token;
    AstExpression* right_expression;

    inline AstBinaryExpression(BinaryExpressionTag tag)
        : AstExpression(BINARY)
    {
        other_tag = tag;
    }
    ~AstBinaryExpression() {}

    inline BinaryExpressionTag Tag()
    {
        return (BinaryExpressionTag) other_tag;
    }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

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
// Represents instanceof expressions.
//
class AstInstanceofExpression : public AstExpression
{
public:
    AstExpression* expression;
    LexStream::TokenIndex instanceof_token;
    AstType* type; // AstArrayType, AstTypeName

    inline AstInstanceofExpression()
        : AstExpression(INSTANCEOF)
    {}
    ~AstInstanceofExpression() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

    virtual LexStream::TokenIndex LeftToken()
    {
        return expression -> LeftToken();
    }
    virtual LexStream::TokenIndex RightToken()
    {
        return type -> RightToken();
    }
};


//
// ConditionalExpression --> <CONDITIONAL, Expression, ?_token, Expression,
//                            :_token, Expression>
//
class AstConditionalExpression : public AstExpression
{
public:
    AstExpression* test_expression;
    LexStream::TokenIndex question_token;
    AstExpression* true_expression;
    LexStream::TokenIndex colon_token;
    AstExpression* false_expression;

    inline AstConditionalExpression()
        : AstExpression(CONDITIONAL)
    {}
    ~AstConditionalExpression() {}

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

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
// Assignment --> <ASSIGNMENT, AssignmentTag, LeftHandSide, AssignmentOperator,
//                Expression>
//
// AssignmentTag --> EQUAL | STAR_EQUAL | SLASH_EQUAL | MOD_EQUAL |
//                   PLUS_EQUAL | MINUS_EQUAL | LEFT_SHIFT_EQUAL |
//                   RIGHT_SHIFT_EQUAL | UNSIGNED_RIGHT_SHIFT_EQUAL |
//                   AND_EQUAL | XOR_EQUAL | IOR_EQUAL
//
// LeftHandSide --> Name | FieldAccess | ArrayAccess | ParenthesizedExpression
//                  | CastExpression
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
    MethodSymbol* write_method;

    AstExpression* left_hand_side;
    LexStream::TokenIndex assignment_operator_token;
    AstExpression* expression;

    inline AstAssignmentExpression(AssignmentExpressionTag tag,
                                   LexStream::TokenIndex token)
        : AstExpression(ASSIGNMENT)
        , assignment_operator_token(token)
    {
        other_tag = tag;
    }
    ~AstAssignmentExpression() {}

    inline AssignmentExpressionTag Tag()
    {
        return (AssignmentExpressionTag) other_tag;
    }
    inline bool SimpleAssignment() { return other_tag == SIMPLE_EQUAL; }

#ifdef JIKES_DEBUG
    virtual void Print(LexStream&);
    virtual void Unparse(Ostream&, LexStream*);
#endif // JIKES_DEBUG

    virtual Ast* Clone(StoragePool*);

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
// This Storage pool is similar to dynamic arrays (class Tuple). The
// difference is that instead of a Next() function we have an Alloc(size_t)
// function. The value of the size_t argument represents the size of the
// object to allocate. The allocated memory is guaranteed to be
// zero-initialized.
//
// All AST nodes for a given parse should be allocated from the same storage
// pool, so they have a placement new operator that requires a StoragePool.
// You should never delete an AST object, as all resources they allocate come
// from the same pool. Instead, to reclaim memory when processing is complete,
// simply delete the underlying storage pool.
//
class StoragePool
{
public:
    typedef void* Cell;

    inline size_t Blksize() { return 1U << log_blksize; }

private:
    Cell** base;
    unsigned base_size; // number of segment slots in base
    unsigned base_index; // index of current non-full segment
    unsigned offset; // offset to next free pointer in base[base_index]

    unsigned log_blksize; // log2(words per segment)
    unsigned base_increment; // number of segment slots to add when growing

    //
    // Allocate another block of storage for the storage pool. block_size
    // allows the creation of larger than normal segments, which are rare,
    // but are sometimes requested by AstArray.
    //
    void AllocateMoreSpace(size_t block_size = 0)
    {
        //
        // This advances base_index to the next slot unless this is the first
        // allocation. Then it allocates a segment to live in that slot.
        // The offset field should only be 0 after construction or after a
        // reset, when base_index should stay at 0.  All other times, offset
        // is nonzero, so we allocate advance base_index.
        //
        assert(offset ? base != NULL : ! base_index);
        if (offset)
            base_index++;
        if (base_index == base_size)
        {
            unsigned old_base_size = base_size;
            Cell** old_base = base;
            base_size += base_increment;
            base = new Cell*[base_size];
            if (old_base)
            {
                memcpy(base, old_base, old_base_size * sizeof(Cell*));
                delete [] old_base;
            }
            memset(base + old_base_size, 0, base_increment * sizeof(Cell*));
        }
        if (block_size)
        {
            assert(block_size > Blksize());
            delete [] base[base_index];
            base[base_index] = new Cell[block_size];
        }
        else if (! base[base_index])
        {
            block_size = Blksize();
            base[base_index] = new Cell[block_size];
        }
        memset(base[base_index], 0, block_size * sizeof(Cell));
    }

public:
    //
    // Constructor of a storage pool. The parameter is the number of tokens
    // which the AST tree will contain.
    //
    StoragePool(unsigned num_tokens)
        : base(NULL)
        , base_size(0)
        , base_index(0)
        , offset(0)
    {
        //
        // Make a guess on the size that will be required for the ast
        // based on the number of tokens. On average, we have about 1 node
        // to 2 tokens, but about 10 words (40 bytes) per node. We add some
        // fudge factor to avoid reallocations, resulting in num_tokens * 8.
        //
        unsigned estimate = num_tokens << 3;

        //
        // Find a block of size 2**log_blksize that is large enough
        // to satisfy our estimate.
        //
        for (log_blksize = 8;
             (1U << log_blksize) < estimate && log_blksize < 31;
             log_blksize++)
            ;

        if (log_blksize < 13) // estimate is < 2**(13+2) == 32k
        {
            base_increment = 1U << (log_blksize - 8);
            log_blksize = 8; // fragment in 2**(8+2) == 1k chunks
        }
        else if (log_blksize < 17) // estimate is < 2**(17+2) == 512k
        {
            base_increment = 1U << (log_blksize - 10);
            log_blksize = 10; // fragment in 2**(10+2) == 4k chunks
        }
        else // estimate is >= 512k, which is rare
        {
            base_increment = 1U << (log_blksize - 12);
            log_blksize = 12; // fragment in 2**(12+2) == 16k chunks
        }

        //
        // Double the size of the base and add an extra margin to avoid
        // reallocating the base, especially for things like Cloning.
        //
        base_increment += base_increment + 3;
    }

    //
    // Destructor of a storage pool. This frees the memory of all of the AST
    // nodes allocated in this pool.
    //
    ~StoragePool()
    {
        if (base)
            for (unsigned i = 0; i <= base_index; i++)
                delete [] base[i];
        delete [] base;
    }

    //
    // Alloc allocates an object of size n in the pool and returns a pointer
    // to it. The memory will be zero-initialized.
    //
    inline void* Alloc(size_t n)
    {
        unsigned chunk_size = (n + sizeof(Cell) - 1) / sizeof(Cell);
        if (chunk_size > Blksize())
        {
            //
            // Handle large requests separately. These are rare, when an
            // AstArray is requested that is larger than a segment. In this
            // case, we allocate the extra large segment in the next free
            // slot, and swap it with the previous segment if that one still
            // had room.
            //
            AllocateMoreSpace(chunk_size);
            Cell result = base[base_index];
            if (base_index)
            {
                Cell* temp = base[base_index];
                base[base_index] = base[base_index - 1];
                base[base_index - 1] = temp;
            }
            return result;
        }
        if (! base || offset + chunk_size > Blksize())
        {
            //
            // Here, we overflow the current segment, but fit in a normal
            // next segment.
            //
            AllocateMoreSpace();
            offset = 0;
        }
        Cell result = base[base_index] + offset;
        offset += chunk_size;
        return result;
    }

    //
    // This function is used to reset the Storage pool. This action
    // automatically invalidates all objects that had been allocated in the
    // pool. At least, YOU should assume it does!!!
    //
    inline void Reset()
    {
        base_index = 0;
        offset = 0;
    }

    // ********************************************************************

    inline VariableSymbolArray* NewVariableSymbolArray(unsigned size = 0)
    {
        return new (Alloc(sizeof(VariableSymbolArray)))
            VariableSymbolArray(this, size);
    }

    inline AstListNode* NewListNode()
    {
        return new (this) AstListNode();
    }

    inline AstBlock* NewBlock()
    {
        return new (this) AstBlock(this);
    }

    inline AstName* NewName(LexStream::TokenIndex token)
    {
        return new (this) AstName(token);
    }

    inline AstPrimitiveType* NewPrimitiveType(Ast::AstKind kind,
                                              LexStream::TokenIndex token)
    {
        return new (this) AstPrimitiveType(kind, token);
    }

    inline AstBrackets* NewBrackets(LexStream::TokenIndex left,
                                    LexStream::TokenIndex right)
    {
        return new (this) AstBrackets(left, right);
    }

    inline AstArrayType* NewArrayType(AstType* type, AstBrackets* brackets)
    {
        return new (this) AstArrayType(type, brackets);
    }

    inline AstTypeName* NewTypeName(AstName* name)
    {
        return new (this) AstTypeName(this, name);
    }

    inline AstMemberValuePair* NewMemberValuePair()
    {
        return new (this) AstMemberValuePair();
    }

    inline AstAnnotation* NewAnnotation()
    {
        return new (this) AstAnnotation(this);
    }

    inline AstModifierKeyword* NewModifierKeyword(LexStream::TokenIndex token)
    {
        return new (this) AstModifierKeyword(token);
    }

    inline AstModifiers* NewModifiers()
    {
        return new (this) AstModifiers(this);
    }

    inline AstPackageDeclaration* NewPackageDeclaration()
    {
        return new (this) AstPackageDeclaration();
    }

    inline AstImportDeclaration* NewImportDeclaration()
    {
        return new (this) AstImportDeclaration();
    }

    inline AstCompilationUnit* NewCompilationUnit()
    {
        return new (this) AstCompilationUnit(this);
    }

    inline AstEmptyDeclaration* NewEmptyDeclaration(LexStream::TokenIndex t)
    {
        return new (this) AstEmptyDeclaration(t);
    }

    inline AstClassBody* NewClassBody()
    {
        return new (this) AstClassBody(this);
    }

    inline AstClassDeclaration* NewClassDeclaration()
    {
        return new (this) AstClassDeclaration(this);
    }

    inline AstArrayInitializer* NewArrayInitializer()
    {
        return new (this) AstArrayInitializer(this);
    }

    inline AstVariableDeclaratorId* NewVariableDeclaratorId()
    {
        return new (this) AstVariableDeclaratorId();
    }

    inline AstVariableDeclarator* NewVariableDeclarator()
    {
        return new (this) AstVariableDeclarator();
    }

    inline AstFieldDeclaration* NewFieldDeclaration()
    {
        return new (this) AstFieldDeclaration(this);
    }

    inline AstFormalParameter* NewFormalParameter()
    {
        return new (this) AstFormalParameter();
    }

    inline AstMethodDeclarator* NewMethodDeclarator()
    {
        return new (this) AstMethodDeclarator(this);
    }

    inline AstMethodBody* NewMethodBody()
    {
        return new (this) AstMethodBody(this);
    }

    inline AstMethodDeclaration* NewMethodDeclaration()
    {
        return new (this) AstMethodDeclaration(this);
    }

    inline AstInitializerDeclaration* NewInitializerDeclaration()
    {
        return new (this) AstInitializerDeclaration();
    }

    inline AstArguments* NewArguments(LexStream::TokenIndex left,
                                      LexStream::TokenIndex right)
    {
        return new (this) AstArguments(this, left, right);
    }

    inline AstThisCall* NewThisCall()
    {
        return new (this) AstThisCall();
    }

    inline AstSuperCall* NewSuperCall()
    {
        return new (this) AstSuperCall();
    }

    inline AstConstructorDeclaration* NewConstructorDeclaration()
    {
        return new (this) AstConstructorDeclaration(this);
    }

    inline AstInterfaceDeclaration* NewInterfaceDeclaration()
    {
        return new (this) AstInterfaceDeclaration(this);
    }

    inline AstLocalVariableStatement* NewLocalVariableStatement()
    {
        return new (this) AstLocalVariableStatement(this);
    }

    inline AstLocalClassStatement* NewLocalClassStatement(AstClassDeclaration* decl)
    {
        return new (this) AstLocalClassStatement(decl);
    }

    inline AstIfStatement* NewIfStatement()
    {
        return new (this) AstIfStatement();
    }

    inline AstEmptyStatement* NewEmptyStatement(LexStream::TokenIndex token)
    {
        return new (this) AstEmptyStatement(token);
    }

    inline AstExpressionStatement* NewExpressionStatement()
    {
        return new (this) AstExpressionStatement();
    }

    inline AstSwitchLabel* NewSwitchLabel()
    {
        return new (this) AstSwitchLabel();
    }

    inline AstSwitchBlockStatement* NewSwitchBlockStatement()
    {
        return new (this) AstSwitchBlockStatement(this);
    }

    inline AstSwitchStatement* NewSwitchStatement()
    {
        return new (this) AstSwitchStatement(this);
    }

    inline AstWhileStatement* NewWhileStatement()
    {
        return new (this) AstWhileStatement();
    }

    inline AstDoStatement* NewDoStatement()
    {
        return new (this) AstDoStatement();
    }

    inline AstForStatement* NewForStatement()
    {
        return new (this) AstForStatement(this);
    }

    inline AstForeachStatement* NewForeachStatement()
    {
        return new (this) AstForeachStatement();
    }

    inline AstBreakStatement* NewBreakStatement()
    {
        return new (this) AstBreakStatement();
    }

    inline AstContinueStatement* NewContinueStatement()
    {
        return new (this) AstContinueStatement();
    }

    inline AstReturnStatement* NewReturnStatement()
    {
        return new (this) AstReturnStatement();
    }

    inline AstThrowStatement* NewThrowStatement()
    {
        return new (this) AstThrowStatement();
    }

    inline AstSynchronizedStatement* NewSynchronizedStatement()
    {
        return new (this) AstSynchronizedStatement();
    }

    inline AstAssertStatement* NewAssertStatement()
    {
        return new (this) AstAssertStatement();
    }

    inline AstCatchClause* NewCatchClause()
    {
        return new (this) AstCatchClause();
    }

    inline AstFinallyClause* NewFinallyClause()
    {
        return new (this) AstFinallyClause();
    }

    inline AstTryStatement* NewTryStatement()
    {
        return new (this) AstTryStatement(this);
    }

    inline AstIntegerLiteral* NewIntegerLiteral(LexStream::TokenIndex token)
    {
        return new (this) AstIntegerLiteral(token);
    }

    inline AstLongLiteral* NewLongLiteral(LexStream::TokenIndex token)
    {
        return new (this) AstLongLiteral(token);
    }

    inline AstFloatLiteral* NewFloatLiteral(LexStream::TokenIndex token)
    {
        return new (this) AstFloatLiteral(token);
    }

    inline AstDoubleLiteral* NewDoubleLiteral(LexStream::TokenIndex token)
    {
        return new (this) AstDoubleLiteral(token);
    }

    inline AstTrueLiteral* NewTrueLiteral(LexStream::TokenIndex token)
    {
        return new (this) AstTrueLiteral(token);
    }

    inline AstFalseLiteral* NewFalseLiteral(LexStream::TokenIndex token)
    {
        return new (this) AstFalseLiteral(token);
    }

    inline AstStringLiteral* NewStringLiteral(LexStream::TokenIndex token)
    {
        return new (this) AstStringLiteral(token);
    }

    inline AstCharacterLiteral* NewCharacterLiteral(LexStream::TokenIndex token)
    {
        return new (this) AstCharacterLiteral(token);
    }

    inline AstNullLiteral* NewNullLiteral(LexStream::TokenIndex token)
    {
        return new (this) AstNullLiteral(token);
    }

    inline AstClassLiteral* NewClassLiteral(LexStream::TokenIndex token)
    {
        return new (this) AstClassLiteral(token);
    }

    inline AstThisExpression* NewThisExpression(LexStream::TokenIndex token)
    {
        return new (this) AstThisExpression(token);
    }

    inline AstSuperExpression* NewSuperExpression(LexStream::TokenIndex token)
    {
        return new (this) AstSuperExpression(token);
    }

    inline AstParenthesizedExpression* NewParenthesizedExpression()
    {
        return new (this) AstParenthesizedExpression();
    }

    inline AstClassCreationExpression* NewClassCreationExpression()
    {
        return new (this) AstClassCreationExpression();
    }

    inline AstDimExpr* NewDimExpr()
    {
        return new (this) AstDimExpr();
    }

    inline AstArrayCreationExpression* NewArrayCreationExpression()
    {
        return new (this) AstArrayCreationExpression(this);
    }

    inline AstFieldAccess* NewFieldAccess()
    {
        return new (this) AstFieldAccess();
    }

    inline AstMethodInvocation* NewMethodInvocation(LexStream::TokenIndex t)
    {
        return new (this) AstMethodInvocation(t);
    }

    inline AstArrayAccess* NewArrayAccess()
    {
        return new (this) AstArrayAccess();
    }

    inline AstPostUnaryExpression* NewPostUnaryExpression(AstPostUnaryExpression::PostUnaryExpressionTag tag)
    {
        return new (this) AstPostUnaryExpression(tag);
    }

    inline AstPreUnaryExpression* NewPreUnaryExpression(AstPreUnaryExpression::PreUnaryExpressionTag tag)
    {
        return new (this) AstPreUnaryExpression(tag);
    }

    inline AstCastExpression* NewCastExpression()
    {
        return new (this) AstCastExpression();
    }

    inline AstBinaryExpression* NewBinaryExpression(AstBinaryExpression::BinaryExpressionTag tag)
    {
        return new (this) AstBinaryExpression(tag);
    }

    inline AstInstanceofExpression* NewInstanceofExpression()
    {
        return new (this) AstInstanceofExpression();
    }

    inline AstConditionalExpression* NewConditionalExpression()
    {
        return new (this) AstConditionalExpression();
    }

    inline AstAssignmentExpression* NewAssignmentExpression(AstAssignmentExpression::AssignmentExpressionTag tag,
                                                            LexStream::TokenIndex token)
    {
        return new (this) AstAssignmentExpression(tag, token);
    }

    // *********************************************************************

    //
    // Note that CaseElement nodes are always generated. Since they are not
    // Ast nodes they do not need to be marked.
    //
    inline CaseElement* GenCaseElement(unsigned block_index,
                                       unsigned case_index)
    {
        CaseElement* p = new (Alloc(sizeof(CaseElement))) CaseElement();
        p -> block_index = block_index;
        p -> case_index = case_index;
        return p;
    }

    inline AstBlock* GenBlock()
    {
        AstBlock* p = NewBlock();
        p -> generated = true;
        p -> no_braces = true;
        return p;
    }

    inline AstName* GenName(LexStream::TokenIndex token)
    {
        AstName* p = NewName(token);
        p -> generated = true;
        return p;
    }

    inline AstPrimitiveType* GenPrimitiveType(Ast::AstKind kind,
                                              LexStream::TokenIndex token)
    {
        AstPrimitiveType* p = NewPrimitiveType(kind, token);
        p -> generated = true;
        return p;
    }

    inline AstBrackets* GenBrackets(LexStream::TokenIndex left,
                                    LexStream::TokenIndex right)
    {
        AstBrackets* p = NewBrackets(left, right);
        p -> generated = true;
        return p;
    }

    inline AstArrayType* GenArrayType(AstType* type, AstBrackets* brackets)
    {
        AstArrayType* p = NewArrayType(type, brackets);
        p -> generated = true;
        return p;
    }

    inline AstTypeName* GenTypeName(AstName* type)
    {
        AstTypeName* p = NewTypeName(type);
        p -> generated = true;
        return p;
    }

    inline AstMemberValuePair* GenMemberValuePair()
    {
        AstMemberValuePair* p = NewMemberValuePair();
        p -> generated = true;
        return p;
    }

    inline AstAnnotation* GenAnnotation()
    {
        AstAnnotation* p = NewAnnotation();
        p -> generated = true;
        return p;
    }

    inline AstModifierKeyword* GenModifierKeyword(LexStream::TokenIndex token)
    {
        AstModifierKeyword* p = NewModifierKeyword(token);
        p -> generated = true;
        return p;
    }

    inline AstModifiers* GenModifiers()
    {
        AstModifiers* p = NewModifiers();
        p -> generated = true;
        return p;
    }

    inline AstPackageDeclaration* GenPackageDeclaration()
    {
        AstPackageDeclaration* p = NewPackageDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstImportDeclaration* GenImportDeclaration()
    {
        AstImportDeclaration* p = NewImportDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstCompilationUnit* GenCompilationUnit()
    {
        AstCompilationUnit* p = NewCompilationUnit();
        p -> generated = true;
        return p;
    }

    inline AstEmptyDeclaration* GenEmptyDeclaration(LexStream::TokenIndex t)
    {
        AstEmptyDeclaration* p = NewEmptyDeclaration(t);
        p -> generated = true;
        return p;
    }

    inline AstClassBody* GenClassBody()
    {
        AstClassBody* p = NewClassBody();
        p -> generated = true;
        return p;
    }

    inline AstClassDeclaration* GenClassDeclaration()
    {
        AstClassDeclaration* p = NewClassDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstArrayInitializer* GenArrayInitializer()
    {
        AstArrayInitializer* p = NewArrayInitializer();
        p -> generated = true;
        return p;
    }

    inline AstVariableDeclaratorId* GenVariableDeclaratorId()
    {
        AstVariableDeclaratorId* p = NewVariableDeclaratorId();
        p -> generated = true;
        return p;
    }

    inline AstVariableDeclarator* GenVariableDeclarator()
    {
        AstVariableDeclarator* p = NewVariableDeclarator();
        p -> generated = true;
        return p;
    }

    inline AstFieldDeclaration* GenFieldDeclaration()
    {
        AstFieldDeclaration* p = NewFieldDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstFormalParameter* GenFormalParameter()
    {
        AstFormalParameter* p = NewFormalParameter();
        p -> generated = true;
        return p;
    }

    inline AstMethodDeclarator* GenMethodDeclarator()
    {
        AstMethodDeclarator* p = NewMethodDeclarator();
        p -> generated = true;
        return p;
    }

    inline AstMethodBody* GenMethodBody()
    {
        AstMethodBody* p = NewMethodBody();
        p -> generated = true;
        return p;
    }

    inline AstMethodDeclaration* GenMethodDeclaration()
    {
        AstMethodDeclaration* p = NewMethodDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstInitializerDeclaration* GenInitializerDeclaration()
    {
        AstInitializerDeclaration* p = NewInitializerDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstArguments* GenArguments(LexStream::TokenIndex left,
                                      LexStream::TokenIndex right)
    {
        AstArguments* p = NewArguments(left, right);
        p -> generated = true;
        return p;
    }

    inline AstThisCall* GenThisCall()
    {
        AstThisCall* p = NewThisCall();
        p -> generated = true;
        return p;
    }

    inline AstSuperCall* GenSuperCall()
    {
        AstSuperCall* p = NewSuperCall();
        p -> generated = true;
        return p;
    }

    inline AstConstructorDeclaration* GenConstructorDeclaration()
    {
        AstConstructorDeclaration* p = NewConstructorDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstInterfaceDeclaration* GenInterfaceDeclaration()
    {
        AstInterfaceDeclaration* p = NewInterfaceDeclaration();
        p -> generated = true;
        return p;
    }

    inline AstLocalVariableStatement* GenLocalVariableStatement()
    {
        AstLocalVariableStatement* p = NewLocalVariableStatement();
        p -> generated = true;
        return p;
    }

    inline AstLocalClassStatement* GenLocalClassStatement(AstClassDeclaration* decl)
    {
        AstLocalClassStatement* p = NewLocalClassStatement(decl);
        p -> generated = true;
        return p;
    }

    inline AstIfStatement* GenIfStatement()
    {
        AstIfStatement* p = NewIfStatement();
        p -> generated = true;
        return p;
    }

    inline AstEmptyStatement* GenEmptyStatement(LexStream::TokenIndex token)
    {
        AstEmptyStatement* p = NewEmptyStatement(token);
        p -> generated = true;
        return p;
    }

    inline AstExpressionStatement* GenExpressionStatement()
    {
        AstExpressionStatement* p = NewExpressionStatement();
        p -> generated = true;
        return p;
    }

    inline AstSwitchLabel* GenSwitchLabel()
    {
        AstSwitchLabel* p = NewSwitchLabel();
        p -> generated = true;
        return p;
    }

    inline AstSwitchBlockStatement* GenSwitchBlockStatement()
    {
        AstSwitchBlockStatement* p = NewSwitchBlockStatement();
        p -> generated = true;
        return p;
    }

    inline AstSwitchStatement* GenSwitchStatement()
    {
        AstSwitchStatement* p = NewSwitchStatement();
        p -> generated = true;
        return p;
    }

    inline AstWhileStatement* GenWhileStatement()
    {
        AstWhileStatement* p = NewWhileStatement();
        p -> generated = true;
        return p;
    }

    inline AstDoStatement* GenDoStatement()
    {
        AstDoStatement* p = NewDoStatement();
        p -> generated = true;
        return p;
    }

    inline AstForStatement* GenForStatement()
    {
        AstForStatement* p = NewForStatement();
        p -> generated = true;
        return p;
    }

    inline AstForeachStatement* GenForeachStatement()
    {
        AstForeachStatement* p = NewForeachStatement();
        p -> generated = true;
        return p;
    }

    inline AstBreakStatement* GenBreakStatement()
    {
        AstBreakStatement* p = NewBreakStatement();
        p -> generated = true;
        return p;
    }

    inline AstContinueStatement* GenContinueStatement()
    {
        AstContinueStatement* p = NewContinueStatement();
        p -> generated = true;
        return p;
    }

    inline AstReturnStatement* GenReturnStatement()
    {
        AstReturnStatement* p = NewReturnStatement();
        p -> generated = true;
        return p;
    }

    inline AstThrowStatement* GenThrowStatement()
    {
        AstThrowStatement* p = NewThrowStatement();
        p -> generated = true;
        return p;
    }

    inline AstSynchronizedStatement* GenSynchronizedStatement()
    {
        AstSynchronizedStatement* p = NewSynchronizedStatement();
        p -> generated = true;
        return p;
    }

    inline AstAssertStatement* GenAssertStatement()
    {
        AstAssertStatement* p = NewAssertStatement();
        p -> generated = true;
        return p;
    }

    inline AstCatchClause* GenCatchClause()
    {
        AstCatchClause* p = NewCatchClause();
        p -> generated = true;
        return p;
    }

    inline AstFinallyClause* GenFinallyClause()
    {
        AstFinallyClause* p = NewFinallyClause();
        p -> generated = true;
        return p;
    }

    inline AstTryStatement* GenTryStatement()
    {
        AstTryStatement* p = NewTryStatement();
        p -> generated = true;
        return p;
    }

    inline AstIntegerLiteral* GenIntegerLiteral(LexStream::TokenIndex token)
    {
        AstIntegerLiteral* p = NewIntegerLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstLongLiteral* GenLongLiteral(LexStream::TokenIndex token)
    {
        AstLongLiteral* p = NewLongLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstFloatLiteral* GenFloatLiteral(LexStream::TokenIndex token)
    {
        AstFloatLiteral* p = NewFloatLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstDoubleLiteral* GenDoubleLiteral(LexStream::TokenIndex token)
    {
        AstDoubleLiteral* p = NewDoubleLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstTrueLiteral* GenTrueLiteral(LexStream::TokenIndex token)
    {
        AstTrueLiteral* p = NewTrueLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstFalseLiteral* GenFalseLiteral(LexStream::TokenIndex token)
    {
        AstFalseLiteral* p = NewFalseLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstStringLiteral* GenStringLiteral(LexStream::TokenIndex token)
    {
        AstStringLiteral* p = NewStringLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstCharacterLiteral* GenCharacterLiteral(LexStream::TokenIndex token)
    {
        AstCharacterLiteral* p = NewCharacterLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstNullLiteral* GenNullLiteral(LexStream::TokenIndex token)
    {
        AstNullLiteral* p = NewNullLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstClassLiteral* GenClassLiteral(LexStream::TokenIndex token)
    {
        AstClassLiteral* p = NewClassLiteral(token);
        p -> generated = true;
        return p;
    }

    inline AstThisExpression* GenThisExpression(LexStream::TokenIndex token)
    {
        AstThisExpression* p = NewThisExpression(token);
        p -> generated = true;
        return p;
    }

    inline AstSuperExpression* GenSuperExpression(LexStream::TokenIndex token)
    {
        AstSuperExpression* p = NewSuperExpression(token);
        p -> generated = true;
        return p;
    }

    inline AstParenthesizedExpression* GenParenthesizedExpression()
    {
        AstParenthesizedExpression* p = NewParenthesizedExpression();
        p -> generated = true;
        return p;
    }

    inline AstClassCreationExpression* GenClassCreationExpression()
    {
        AstClassCreationExpression* p = NewClassCreationExpression();
        p -> generated = true;
        return p;
    }

    inline AstDimExpr* GenDimExpr()
    {
        AstDimExpr* p = NewDimExpr();
        p -> generated = true;
        return p;
    }

    inline AstArrayCreationExpression* GenArrayCreationExpression()
    {
        AstArrayCreationExpression* p = NewArrayCreationExpression();
        p -> generated = true;
        return p;
    }

    inline AstFieldAccess* GenFieldAccess()
    {
        AstFieldAccess* p = NewFieldAccess();
        p -> generated = true;
        return p;
    }

    inline AstMethodInvocation* GenMethodInvocation(LexStream::TokenIndex t)
    {
        AstMethodInvocation* p = NewMethodInvocation(t);
        p -> generated = true;
        return p;
    }

    inline AstArrayAccess* GenArrayAccess()
    {
        AstArrayAccess* p = NewArrayAccess();
        p -> generated = true;
        return p;
    }

    inline AstPostUnaryExpression* GenPostUnaryExpression(AstPostUnaryExpression::PostUnaryExpressionTag tag)
    {
        AstPostUnaryExpression* p = NewPostUnaryExpression(tag);
        p -> generated = true;
        return p;
    }

    inline AstPreUnaryExpression* GenPreUnaryExpression(AstPreUnaryExpression::PreUnaryExpressionTag tag)
    {
        AstPreUnaryExpression* p = NewPreUnaryExpression(tag);
        p -> generated = true;
        return p;
    }

    inline AstCastExpression* GenCastExpression()
    {
        AstCastExpression* p = NewCastExpression();
        p -> generated = true;
        return p;
    }

    inline AstBinaryExpression* GenBinaryExpression(AstBinaryExpression::BinaryExpressionTag tag)
    {
        AstBinaryExpression* p = NewBinaryExpression(tag);
        p -> generated = true;
        return p;
    }

    inline AstInstanceofExpression* GenInstanceofExpression()
    {
        AstInstanceofExpression* p = NewInstanceofExpression();
        p -> generated = true;
        return p;
    }

    inline AstConditionalExpression* GenConditionalExpression()
    {
        AstConditionalExpression* p = NewConditionalExpression();
        p -> generated = true;
        return p;
    }

    inline AstAssignmentExpression* GenAssignmentExpression(AstAssignmentExpression::AssignmentExpressionTag tag,
                                                            LexStream::TokenIndex token)
    {
        AstAssignmentExpression* p = NewAssignmentExpression(tag, token);
        p -> generated = true;
        return p;
    }
};


//***********************************

//
// Given an Ast tree, check whether it is a variable (not a value).
//
inline bool Ast::IsLeftHandSide()
{
    return kind == NAME || kind == DOT || kind == ARRAY_ACCESS;
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
    return generated;
}

//
// Cast conversions for Ast
//

inline AstStatement* Ast::StatementCast()
{
    return DYNAMIC_CAST<AstStatement*> (class_tag == STATEMENT ? this : NULL);
}

inline AstMemberValue* Ast::MemberValueCast()
{
    return DYNAMIC_CAST<AstMemberValue*>
        ((class_tag == EXPRESSION || kind == ANNOTATION ||
          kind == ARRAY_INITIALIZER) ? this : NULL);
}

inline AstExpression* Ast::ExpressionCast()
{
    return DYNAMIC_CAST<AstExpression*>
        (class_tag == EXPRESSION ? this : NULL);
}

inline AstPrimitiveType* Ast::PrimitiveTypeCast()
{
    return DYNAMIC_CAST<AstPrimitiveType*>
        (class_tag == PRIMITIVE_TYPE ? this : NULL);
}

inline AstFieldDeclaration* Ast::StaticFieldCast()
{
    return DYNAMIC_CAST<AstFieldDeclaration*>
        (kind == FIELD &&
         other_tag == AstFieldDeclaration::STATIC ? this : NULL);
}

inline AstInitializerDeclaration* Ast::StaticInitializerCast()
{
    return DYNAMIC_CAST<AstInitializerDeclaration*>
        (kind == INITIALIZER &&
         other_tag == AstInitializerDeclaration::STATIC ? this : NULL);
}

inline AstClassBody* Ast::UnparsedClassBodyCast()
{
    return DYNAMIC_CAST<AstClassBody*>
        (kind == CLASS_BODY &&
         other_tag == AstClassBody::UNPARSED ? this : NULL);
}

inline AstCompilationUnit* Ast::BadCompilationUnitCast()
{
    return DYNAMIC_CAST<AstCompilationUnit*>
        (kind == COMPILATION &&
         other_tag == AstCompilationUnit::BAD_COMPILATION ? this : NULL);
}

inline AstCompilationUnit* Ast::EmptyCompilationUnitCast()
{
    return DYNAMIC_CAST<AstCompilationUnit*>
        (kind == COMPILATION &&
         other_tag == AstCompilationUnit::EMPTY_COMPILATION ? this : NULL);
}

//
// These cast functions are used for classes that represent exactly
// one kind of node.
//

inline AstListNode* Ast::ListNodeCast()
{
    return DYNAMIC_CAST<AstListNode*> (kind == LIST_NODE ? this : NULL);
}

inline AstBlock* Ast::BlockCast()
{
    return DYNAMIC_CAST<AstBlock*>
        (kind == BLOCK || kind == METHOD_BODY || kind == SWITCH_BLOCK
         ? this : NULL);
}

inline AstName* Ast::NameCast()
{
    return DYNAMIC_CAST<AstName*> (kind == NAME ? this : NULL);
}

inline AstBrackets* Ast::BracketsCast()
{
    return DYNAMIC_CAST<AstBrackets*> (kind == BRACKETS ? this : NULL);
}

inline AstArrayType* Ast::ArrayTypeCast()
{
    return DYNAMIC_CAST<AstArrayType*> (kind == ARRAY ? this : NULL);
}

inline AstTypeName* Ast::TypeNameCast()
{
    return DYNAMIC_CAST<AstTypeName*> (kind == TYPE ? this : NULL);
}

inline AstMemberValuePair* Ast::MemberValuePairCast()
{
    return DYNAMIC_CAST<AstMemberValuePair*>
        (kind == MEMBER_VALUE_PAIR ? this : NULL);
}

inline AstAnnotation* Ast::AnnotationCast()
{
    return DYNAMIC_CAST<AstAnnotation*> (kind == ANNOTATION ? this : NULL);
}

inline AstModifierKeyword* Ast::ModifierKeywordCast()
{
    return DYNAMIC_CAST<AstModifierKeyword*>
        (kind == MODIFIER_KEYWORD ? this : NULL);
}

inline AstModifiers* Ast::ModifiersCast()
{
    return DYNAMIC_CAST<AstModifiers*> (kind == MODIFIERS ? this : NULL);
}

inline AstPackageDeclaration* Ast::PackageDeclarationCast()
{
    return DYNAMIC_CAST<AstPackageDeclaration*>
        (kind == PACKAGE ? this : NULL);
}

inline AstImportDeclaration* Ast::ImportDeclarationCast()
{
    return DYNAMIC_CAST<AstImportDeclaration*> (kind == IMPORT ? this : NULL);
}

inline AstCompilationUnit* Ast::CompilationUnitCast()
{
    return DYNAMIC_CAST<AstCompilationUnit*>
        (kind == COMPILATION ? this : NULL);
}

inline AstEmptyDeclaration* Ast::EmptyDeclarationCast()
{
    return DYNAMIC_CAST<AstEmptyDeclaration*>
        (kind == EMPTY_DECLARATION ? this : NULL);
}

inline AstClassBody* Ast::ClassBodyCast()
{
    return DYNAMIC_CAST<AstClassBody*> (kind == CLASS_BODY ? this : NULL);
}

inline AstClassDeclaration* Ast::ClassDeclarationCast()
{
    return DYNAMIC_CAST<AstClassDeclaration*> (kind == CLASS ? this : NULL);
}

inline AstArrayInitializer* Ast::ArrayInitializerCast()
{
    return DYNAMIC_CAST<AstArrayInitializer*>
        (kind == ARRAY_INITIALIZER ? this : NULL);
}

inline AstVariableDeclaratorId* Ast::VariableDeclaratorIdCast()
{
    return DYNAMIC_CAST<AstVariableDeclaratorId*>
        (kind == VARIABLE_DECLARATOR_NAME ? this : NULL);
}

inline AstVariableDeclarator* Ast::VariableDeclaratorCast()
{
    return DYNAMIC_CAST<AstVariableDeclarator*>
        (kind == VARIABLE_DECLARATOR ? this : NULL);
}

inline AstFieldDeclaration* Ast::FieldDeclarationCast()
{
    return DYNAMIC_CAST<AstFieldDeclaration*> (kind == FIELD ? this : NULL);
}

inline AstFormalParameter* Ast::FormalParameterCast()
{
    return DYNAMIC_CAST<AstFormalParameter*> (kind == PARAMETER ? this : NULL);
}

inline AstMethodDeclarator* Ast::MethodDeclaratorCast()
{
    return DYNAMIC_CAST<AstMethodDeclarator*>
        (kind == METHOD_DECLARATOR ? this : NULL);
}

inline AstMethodBody* Ast::MethodBodyCast()
{
    return DYNAMIC_CAST<AstMethodBody*> (kind == METHOD_BODY ? this : NULL);
}

inline AstMethodDeclaration* Ast::MethodDeclarationCast()
{
    return DYNAMIC_CAST<AstMethodDeclaration*> (kind == METHOD ? this : NULL);
}

inline AstInitializerDeclaration* Ast::InitializerDeclarationCast()
{
    return DYNAMIC_CAST<AstInitializerDeclaration*>
        (kind == INITIALIZER ? this : NULL);
}

inline AstArguments* Ast::ArgumentsCast()
{
    return DYNAMIC_CAST<AstArguments*> (kind == ARGUMENTS ? this : NULL);
}

inline AstThisCall* Ast::ThisCallCast()
{
    return DYNAMIC_CAST<AstThisCall*> (kind == THIS_CALL ? this : NULL);
}

inline AstSuperCall* Ast::SuperCallCast()
{
    return DYNAMIC_CAST<AstSuperCall*> (kind == SUPER_CALL ? this : NULL);
}

inline AstConstructorDeclaration* Ast::ConstructorDeclarationCast()
{
    return DYNAMIC_CAST<AstConstructorDeclaration*>
        (kind == CONSTRUCTOR ? this : NULL);
}

inline AstInterfaceDeclaration* Ast::InterfaceDeclarationCast()
{
    return DYNAMIC_CAST<AstInterfaceDeclaration*>
        (kind == INTERFACE ? this : NULL);
}

inline AstLocalVariableStatement* Ast::LocalVariableStatementCast()
{
    return DYNAMIC_CAST<AstLocalVariableStatement*>
        (kind == LOCAL_VARIABLE_DECLARATION ? this : NULL);
}

inline AstLocalClassStatement* Ast::LocalClassStatementCast()
{
    return DYNAMIC_CAST<AstLocalClassStatement*>
        (kind == LOCAL_CLASS ? this : NULL);
}

inline AstIfStatement* Ast::IfStatementCast()
{
    return DYNAMIC_CAST<AstIfStatement*> (kind == IF ? this : NULL);
}

inline AstEmptyStatement* Ast::EmptyStatementCast()
{
    return DYNAMIC_CAST<AstEmptyStatement*>
        (kind == EMPTY_STATEMENT ? this : NULL);
}

inline AstExpressionStatement* Ast::ExpressionStatementCast()
{
    return DYNAMIC_CAST<AstExpressionStatement*>
        (kind == EXPRESSION_STATEMENT ? this : NULL);
}

inline AstSwitchLabel* Ast::SwitchLabelCast()
{
    return DYNAMIC_CAST<AstSwitchLabel*> (kind == SWITCH_LABEL ? this : NULL);
}

inline AstSwitchBlockStatement* Ast::SwitchBlockStatementCast()
{
    return DYNAMIC_CAST<AstSwitchBlockStatement*>
        (kind == SWITCH_BLOCK ? this : NULL);
}

inline AstSwitchStatement* Ast::SwitchStatementCast()
{
    return DYNAMIC_CAST<AstSwitchStatement*> (kind == SWITCH ? this : NULL);
}

inline AstWhileStatement* Ast::WhileStatementCast()
{
    return DYNAMIC_CAST<AstWhileStatement*> (kind == WHILE ? this : NULL);
}

inline AstDoStatement* Ast::DoStatementCast()
{
    return DYNAMIC_CAST<AstDoStatement*> (kind == DO ? this : NULL);
}

inline AstForStatement* Ast::ForStatementCast()
{
    return DYNAMIC_CAST<AstForStatement*> (kind == FOR ? this : NULL);
}

inline AstForeachStatement* Ast::ForeachStatementCast()
{
    return DYNAMIC_CAST<AstForeachStatement*> (kind == FOREACH ? this : NULL);
}

inline AstBreakStatement* Ast::BreakStatementCast()
{
    return DYNAMIC_CAST<AstBreakStatement*> (kind == BREAK ? this : NULL);
}

inline AstContinueStatement* Ast::ContinueStatementCast()
{
    return DYNAMIC_CAST<AstContinueStatement*>
        (kind == CONTINUE ? this : NULL);
}

inline AstReturnStatement* Ast::ReturnStatementCast()
{
    return DYNAMIC_CAST<AstReturnStatement*> (kind == RETURN ? this : NULL);
}

inline AstThrowStatement* Ast::ThrowStatementCast()
{
    return DYNAMIC_CAST<AstThrowStatement*> (kind == THROW ? this : NULL);
}

inline AstSynchronizedStatement* Ast::SynchronizedStatementCast()
{
    return DYNAMIC_CAST<AstSynchronizedStatement*>
        (kind == SYNCHRONIZED_STATEMENT ? this : NULL);
}

inline AstAssertStatement* Ast::AssertStatementCast()
{
    return DYNAMIC_CAST<AstAssertStatement*> (kind == ASSERT ? this : NULL);
}

inline AstCatchClause* Ast::CatchClauseCast()
{
    return DYNAMIC_CAST<AstCatchClause*> (kind == CATCH ? this : NULL);
}

inline AstFinallyClause* Ast::FinallyClauseCast()
{
    return DYNAMIC_CAST<AstFinallyClause*> (kind == FINALLY ? this : NULL);
}

inline AstTryStatement* Ast::TryStatementCast()
{
    return DYNAMIC_CAST<AstTryStatement*> (kind == TRY ? this : NULL);
}

inline AstIntegerLiteral* Ast::IntegerLiteralCast()
{
    return DYNAMIC_CAST<AstIntegerLiteral*>
        (kind == INTEGER_LITERAL ? this : NULL);
}

inline AstLongLiteral* Ast::LongLiteralCast()
{
    return DYNAMIC_CAST<AstLongLiteral*> (kind == LONG_LITERAL ? this : NULL);
}

inline AstFloatLiteral* Ast::FloatLiteralCast()
{
    return DYNAMIC_CAST<AstFloatLiteral*>
        (kind == FLOAT_LITERAL ? this : NULL);
}

inline AstDoubleLiteral* Ast::DoubleLiteralCast()
{
    return DYNAMIC_CAST<AstDoubleLiteral*>
        (kind == DOUBLE_LITERAL ? this : NULL);
}

inline AstTrueLiteral* Ast::TrueLiteralCast()
{
    return DYNAMIC_CAST<AstTrueLiteral*> (kind == TRUE_LITERAL ? this : NULL);
}

inline AstFalseLiteral* Ast::FalseLiteralCast()
{
    return DYNAMIC_CAST<AstFalseLiteral*>
        (kind == FALSE_LITERAL ? this : NULL);
}

inline AstStringLiteral* Ast::StringLiteralCast()
{
    return DYNAMIC_CAST<AstStringLiteral*>
        (kind == STRING_LITERAL ? this : NULL);
}

inline AstCharacterLiteral* Ast::CharacterLiteralCast()
{
    return DYNAMIC_CAST<AstCharacterLiteral*>
        (kind == CHARACTER_LITERAL ? this : NULL);
}

inline AstNullLiteral* Ast::NullLiteralCast()
{
    return DYNAMIC_CAST<AstNullLiteral*> (kind == NULL_LITERAL ? this : NULL);
}

inline AstClassLiteral* Ast::ClassLiteralCast()
{
    return DYNAMIC_CAST<AstClassLiteral*>
        (kind == CLASS_LITERAL ? this : NULL);
}

inline AstThisExpression* Ast::ThisExpressionCast()
{
    return DYNAMIC_CAST<AstThisExpression*>
        (kind == THIS_EXPRESSION ? this : NULL);
}

inline AstSuperExpression* Ast::SuperExpressionCast()
{
    return DYNAMIC_CAST<AstSuperExpression*>
        (kind == SUPER_EXPRESSION ? this : NULL);
}

inline AstParenthesizedExpression* Ast::ParenthesizedExpressionCast()
{
    return DYNAMIC_CAST<AstParenthesizedExpression*>
        (kind == PARENTHESIZED_EXPRESSION ? this : NULL);
}

inline AstClassCreationExpression* Ast::ClassCreationExpressionCast()
{
    return DYNAMIC_CAST<AstClassCreationExpression*>
        (kind == CLASS_CREATION ? this : NULL);
}

inline AstDimExpr* Ast::DimExprCast()
{
    return DYNAMIC_CAST<AstDimExpr*> (kind == DIM ? this : NULL);
}

inline AstArrayCreationExpression* Ast::ArrayCreationExpressionCast()
{
    return DYNAMIC_CAST<AstArrayCreationExpression*>
        (kind == ARRAY_CREATION ? this : NULL);
}

inline AstFieldAccess* Ast::FieldAccessCast()
{
    return DYNAMIC_CAST<AstFieldAccess*> (kind == DOT ? this : NULL);
}

inline AstMethodInvocation* Ast::MethodInvocationCast()
{
    return DYNAMIC_CAST<AstMethodInvocation*> (kind == CALL ? this : NULL);
}

inline AstArrayAccess* Ast::ArrayAccessCast()
{
    return DYNAMIC_CAST<AstArrayAccess*> (kind == ARRAY_ACCESS ? this : NULL);
}

inline AstPostUnaryExpression* Ast::PostUnaryExpressionCast()
{
    return DYNAMIC_CAST<AstPostUnaryExpression*>
        (kind == POST_UNARY ? this : NULL);
}

inline AstPreUnaryExpression* Ast::PreUnaryExpressionCast()
{
    return DYNAMIC_CAST<AstPreUnaryExpression*>
        (kind == PRE_UNARY ? this : NULL);
}

inline AstCastExpression* Ast::CastExpressionCast()
{
    return DYNAMIC_CAST<AstCastExpression*> (kind == CAST ? this : NULL);
}

inline AstBinaryExpression* Ast::BinaryExpressionCast()
{
    return DYNAMIC_CAST<AstBinaryExpression*> (kind == BINARY ? this : NULL);
}

inline AstInstanceofExpression* Ast::InstanceofExpressionCast()
{
    return DYNAMIC_CAST<AstInstanceofExpression*>
        (kind == INSTANCEOF ? this : NULL);
}

inline AstConditionalExpression* Ast::ConditionalExpressionCast()
{
    return DYNAMIC_CAST<AstConditionalExpression*>
        (kind == CONDITIONAL ? this : NULL);
}

inline AstAssignmentExpression* Ast::AssignmentExpressionCast()
{
    return DYNAMIC_CAST<AstAssignmentExpression*>
        (kind == ASSIGNMENT ? this : NULL);
}

// **********************************************

inline bool AstDeclaredType::IsValid()
{
    return class_body && class_body -> semantic_environment;
}

inline void AstBlock::AllocateStatements(unsigned estimate)
{
    assert(! block_statements);
    block_statements = new (pool) AstArray<AstStatement*> (pool, estimate);
}

inline void AstBlock::AddStatement(AstStatement* statement)
{
    assert(block_statements);
    block_statements -> Next() = statement;
}

inline void AstBlock::AllocateLocallyDefinedVariables(unsigned estimate)
{
    if (! defined_variables)
        defined_variables = pool -> NewVariableSymbolArray(estimate);
}

inline void AstBlock::AddLocallyDefinedVariable(VariableSymbol* variable_symbol)
{
    if (! defined_variables)
        AllocateLocallyDefinedVariables(1);
    defined_variables -> Next() = variable_symbol;
}

inline void AstAnnotation::AllocateMemberValuePairs(unsigned estimate)
{
    assert(! member_value_pairs);
    member_value_pairs =
        new (pool) AstArray<AstMemberValuePair*> (pool, estimate);
}

inline void AstAnnotation::AddMemberValuePair(AstMemberValuePair* pair)
{
    assert(member_value_pairs);
    member_value_pairs -> Next() = pair;
}

inline void AstModifiers::AllocateModifiers(unsigned estimate)
{
    assert(! modifiers && estimate);
    modifiers = new (pool) AstArray<Ast*> (pool, estimate);
}

inline void AstModifiers::AddModifier(AstAnnotation* annotation)
{
    assert(modifiers);
    modifiers -> Next() = annotation;
}

inline void AstModifiers::AddModifier(AstModifierKeyword* keyword)
{
    assert(modifiers);
    modifiers -> Next() = keyword;
}

inline void AstCompilationUnit::AllocateImportDeclarations(unsigned estimate)
{
    assert(! import_declarations);
    import_declarations =
        new (ast_pool) AstArray<AstImportDeclaration*> (ast_pool, estimate);
}

inline void AstCompilationUnit::AddImportDeclaration(AstImportDeclaration* import_declaration)
{
    assert(import_declarations);
    import_declarations -> Next() = import_declaration;
}

inline void AstCompilationUnit::AllocateTypeDeclarations(unsigned estimate)
{
    assert(! type_declarations);
    type_declarations =
        new (ast_pool) AstArray<AstDeclaredType*> (ast_pool, estimate);
}

inline void AstCompilationUnit::AddTypeDeclaration(AstDeclaredType* type_declaration)
{
    assert(type_declarations);
    type_declarations -> Next() = type_declaration;
}

inline void AstClassBody::AllocateClassBodyDeclarations(unsigned estimate)
{
    assert(! class_body_declarations);
    class_body_declarations =
        new (pool) AstArray<AstDeclared*> (pool, estimate);
}

inline void AstClassBody::AddClassBodyDeclaration(AstDeclared* member)
{
    assert(class_body_declarations);
    class_body_declarations -> Next() = member;
}

inline void AstClassBody::AllocateInstanceVariables(unsigned estimate)
{
    assert(! instance_variables);
    instance_variables =
        new (pool) AstArray<AstFieldDeclaration*> (pool, estimate);
}

inline void AstClassBody::AddInstanceVariable(AstFieldDeclaration* field_declaration)
{
    assert(instance_variables);
    instance_variables -> Next() = field_declaration;
}

inline void AstClassBody::AllocateClassVariables(unsigned estimate)
{
    assert(! class_variables);
    class_variables =
        new (pool) AstArray<AstFieldDeclaration*> (pool, estimate);
}

inline void AstClassBody::AddClassVariable(AstFieldDeclaration* field_declaration)
{
    assert(class_variables);
    class_variables -> Next() = field_declaration;
}

inline void AstClassBody::AllocateMethods(unsigned estimate)
{
    assert(! methods);
    methods = new (pool) AstArray<AstMethodDeclaration*> (pool, estimate);
}

inline void AstClassBody::AddMethod(AstMethodDeclaration* method_declaration)
{
    assert(methods);
    methods -> Next() = method_declaration;
}

inline void AstClassBody::AllocateConstructors(unsigned estimate)
{
    assert(! constructors);
    constructors =
        new (pool) AstArray<AstConstructorDeclaration*> (pool, estimate);
}

inline void AstClassBody::AddConstructor(AstConstructorDeclaration* constructor_declaration)
{
    assert(constructors);
    constructors -> Next() = constructor_declaration;
}

inline void AstClassBody::AllocateStaticInitializers(unsigned estimate)
{
    assert(! static_initializers);
    static_initializers =
        new (pool) AstArray<AstInitializerDeclaration*> (pool, estimate);
}

inline void AstClassBody::AddStaticInitializer(AstInitializerDeclaration* initializer)
{
    assert(static_initializers);
    static_initializers -> Next() = initializer;
}

inline void AstClassBody::AllocateInstanceInitializers(unsigned estimate)
{
    assert(! instance_initializers);
    instance_initializers =
        new (pool) AstArray<AstInitializerDeclaration*> (pool, estimate);
}

inline void AstClassBody::AddInstanceInitializer(AstInitializerDeclaration* initializer)
{
    assert(instance_initializers);
    instance_initializers -> Next() = initializer;
}

inline void AstClassBody::AllocateNestedClasses(unsigned estimate)
{
    assert(! inner_classes);
    inner_classes = new (pool) AstArray<AstClassDeclaration*> (pool, estimate);
}

inline void AstClassBody::AddNestedClass(AstClassDeclaration* class_declaration)
{
    assert(inner_classes);
    inner_classes -> Next() = class_declaration;
}

inline void AstClassBody::AllocateNestedInterfaces(unsigned estimate)
{
    assert(! inner_interfaces);
    inner_interfaces =
        new (pool) AstArray<AstInterfaceDeclaration*> (pool, estimate);
}

inline void AstClassBody::AddNestedInterface(AstInterfaceDeclaration* interface_declaration)
{
    assert(inner_interfaces);
    inner_interfaces -> Next() = interface_declaration;
}

inline void AstClassBody::AllocateEmptyDeclarations(unsigned estimate)
{
    assert(! empty_declarations);
    empty_declarations =
        new (pool) AstArray<AstEmptyDeclaration*> (pool, estimate);
}

inline void AstClassBody::AddEmptyDeclaration(AstEmptyDeclaration* empty_declaration)
{
    assert(empty_declarations);
    empty_declarations -> Next() = empty_declaration;
}

inline void AstClassDeclaration::AllocateInterfaces(unsigned estimate)
{
    assert(! interfaces);
    interfaces = new (pool) AstArray<AstTypeName*> (pool, estimate);
}

inline void AstClassDeclaration::AddInterface(AstTypeName* interf)
{
    assert(interfaces);
    interfaces -> Next() = interf;
}

inline void AstArrayInitializer::AllocateVariableInitializers(unsigned estimate)
{
    assert(! variable_initializers);
    variable_initializers =
        new (pool) AstArray<AstMemberValue*> (pool, estimate);
}

inline void AstArrayInitializer::AddVariableInitializer(AstMemberValue* initializer)
{
    assert(variable_initializers);
    variable_initializers -> Next() = initializer;
}

inline void AstFieldDeclaration::AllocateVariableDeclarators(unsigned estimate)
{
    assert(! variable_declarators);
    variable_declarators =
        new (pool) AstArray<AstVariableDeclarator*> (pool, estimate);
}

inline void AstFieldDeclaration::AddVariableDeclarator(AstVariableDeclarator* variable_declarator)
{
    assert(variable_declarators);
    variable_declarators -> Next() = variable_declarator;
}

inline void AstMethodDeclarator::AllocateFormalParameters(unsigned estimate)
{
    assert(! formal_parameters);
    formal_parameters =
        new (pool) AstArray<AstFormalParameter*> (pool, estimate);
}

inline void AstMethodDeclarator::AddFormalParameter(AstFormalParameter* formal_parameter)
{
    assert(formal_parameters);
    formal_parameters -> Next() = formal_parameter;
}

inline void AstMethodDeclaration::AllocateThrows(unsigned estimate)
{
    assert(! throws);
    throws = new (pool) AstArray<AstTypeName*> (pool, estimate);
}

inline void AstMethodDeclaration::AddThrow(AstTypeName* exception)
{
    assert(throws);
    throws -> Next() = exception;
}

inline void AstArguments::AllocateArguments(unsigned estimate)
{
    assert(! arguments);
    arguments = new (pool) AstArray<AstExpression*> (pool, estimate);
}

inline void AstArguments::AddArgument(AstExpression* argument)
{
    assert(arguments);
    arguments -> Next() = argument;
}

inline void AstArguments::AllocateLocalArguments(unsigned estimate)
{
    assert(! shadow_arguments);
    shadow_arguments = new (pool) AstArray<AstName*> (pool, estimate);
}

inline void AstArguments::AddLocalArgument(AstName* argument)
{
    assert(shadow_arguments);
    shadow_arguments -> Next() = argument;
}

inline void AstConstructorDeclaration::AllocateThrows(unsigned estimate)
{
    assert(! throws);
    throws = new (pool) AstArray<AstTypeName*> (pool, estimate);
}

inline void AstConstructorDeclaration::AddThrow(AstTypeName* exception)
{
    assert(throws);
    throws -> Next() = exception;
}

inline void AstInterfaceDeclaration::AllocateInterfaces(unsigned estimate)
{
    assert(! interfaces);
    interfaces = new (pool) AstArray<AstTypeName*> (pool, estimate);
}

inline void AstInterfaceDeclaration::AddInterface(AstTypeName* interf)
{
    assert(interfaces);
    interfaces -> Next() = interf;
}

inline void AstLocalVariableStatement::AllocateVariableDeclarators(unsigned estimate)
{
    assert(! variable_declarators);
    variable_declarators =
        new (pool) AstArray<AstVariableDeclarator*> (pool, estimate);
}

inline void AstLocalVariableStatement::AddVariableDeclarator(AstVariableDeclarator* variable_declarator)
{
    assert(variable_declarators);
    variable_declarators -> Next() = variable_declarator;
}

inline void AstSwitchBlockStatement::AllocateSwitchLabels(unsigned estimate)
{
    assert(! switch_labels);
    switch_labels = new (pool) AstArray<AstSwitchLabel*> (pool, estimate);
}

inline void AstSwitchBlockStatement::AddSwitchLabel(AstSwitchLabel* case_label)
{
    assert(switch_labels);
    switch_labels -> Next() = case_label;
}

inline void AstSwitchStatement::AllocateCases(unsigned estimate)
{
    //
    // Add one to the estimate to save room for the default case in element 0.
    //
    assert(! cases);
    cases = new (pool -> Alloc((estimate + 1) * sizeof(CaseElement*)))
        CaseElement*[estimate + 1];
#ifdef JIKES_DEBUG
    max_cases = estimate + 1;
#endif // JIKES_DEBUG
}

inline void AstSwitchStatement::AddCase(CaseElement* case_element)
{
    assert(cases);
    cases[++num_cases] = case_element;
#ifdef JIKES_DEBUG
    assert(num_cases < max_cases);
#endif // JIKES_DEBUG
}

inline void AstForStatement::AllocateForInitStatements(unsigned estimate)
{
    assert(! for_init_statements);
    for_init_statements = new (pool) AstArray<AstStatement*> (pool, estimate);
}

inline void AstForStatement::AddForInitStatement(AstStatement* statement)
{
    assert(for_init_statements);
    for_init_statements -> Next() = statement;
}

inline void AstForStatement::AllocateForUpdateStatements(unsigned estimate)
{
    assert(! for_update_statements);
    for_update_statements =
        new (pool) AstArray<AstExpressionStatement*> (pool, estimate);
}

inline void AstForStatement::AddForUpdateStatement(AstExpressionStatement* statement)
{
    assert(for_update_statements);
    for_update_statements -> Next() = statement;
}

inline void AstTryStatement::AllocateCatchClauses(unsigned estimate)
{
    assert(! catch_clauses);
    catch_clauses = new (pool) AstArray<AstCatchClause*> (pool, estimate);
}

inline void AstTryStatement::AddCatchClause(AstCatchClause* catch_clause)
{
    assert(catch_clauses);
    catch_clauses -> Next() = catch_clause;
}

inline void AstArrayCreationExpression::AllocateDimExprs(unsigned estimate)
{
    assert(! dim_exprs);
    dim_exprs = new (pool) AstArray<AstDimExpr*> (pool, estimate);
}

inline void AstArrayCreationExpression::AddDimExpr(AstDimExpr* dim_expr)
{
    assert(dim_exprs);
    dim_exprs -> Next() = dim_expr;
}

// ******************************************

//
// Overridden placement new operator allows us to allocate storage from the
// same pool as everything else in the compilation unit.
//
inline void* Ast::operator new(size_t size, StoragePool* pool)
{
    return pool -> Alloc(size);
}

template <typename T>
inline void* AstArray<T>::operator new(size_t size, StoragePool* pool)
{
    return pool -> Alloc(size);
}

//
// Constructor of an Ast array.
//
template <typename T>
AstArray<T>::AstArray(StoragePool* pool, unsigned estimate)
    : size(estimate)
{
    //
    // This bit of code is a compile-time assertion that only Ast* are stuck
    // in an AstArray.
    //
#if defined JIKES_DEBUG && defined HAVE_STATIC_CAST
    assert(true || static_cast<Ast*> (T()));
#endif // JIKES_DEBUG

    if(estimate)
        array = new (pool -> Alloc(size * sizeof(T))) T[size];
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

#endif // ast_INCLUDED
