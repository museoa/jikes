// $Id$ -*- c++ -*-
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, 1999, 2000, 2001, 2002 International Business
// Machines Corporation and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#ifndef semantic_INCLUDED
#define semantic_INCLUDED

#include "platform.h"
#include "ast.h"
#include "diagnose.h"
#include "error.h"
#include "symbol.h"
#include "control.h"
#include "tuple.h"
#include "set.h"

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif


class cp_info;
class TypeShadowSymbol;

//
//
//
class SymbolTableStack
{
public:
    void Push(SymbolTable *symtab) { table.Next() = symtab; }
    void Pop() { if (table.Length() > 0) table.Reset(table.Length() - 1); }
    int Size() { return table.Length(); }
    SymbolTable *Top()
    {
        return (SymbolTable *) (table.Length() > 0
                                ? table[table.Length() - 1] : NULL);
    }

    SymbolTable *operator[](const int i) { return table[i]; } /*  */

    //
    // Search for a variable in a stack of symbol tables starting at the
    // current symbol table and ending with the symbol table of the method
    // from which this call originates.
    //
    VariableSymbol *FindVariableSymbol(NameSymbol *name_symbol)
    {
        for (int i = table.Length() - 1; i >= 0; i--)
        {
            VariableSymbol *symbol =
                table[i] -> FindVariableSymbol(name_symbol);
            if (symbol)
                return symbol;
        }
        return (VariableSymbol *) NULL;
    }

    //
    // Search for a type in a stack of symbol tables starting at the current
    // symbol table and ending with the symbol table of the method from which
    // this call originates.
    //
    TypeSymbol* FindTypeSymbol(NameSymbol *name_symbol)
    {
        for (int i = table.Length() - 1; i >= 0; i--)
        {
            TypeSymbol *symbol = table[i] -> FindTypeSymbol(name_symbol);
            if (symbol)
                return symbol;
        }
        return (TypeSymbol *) NULL;
    }

    //
    // Search for a label in a stack of symbol tables starting at the current
    // symbol table and ending with the symbol table of the method from which
    // this call originates.
    //
    LabelSymbol* FindLabelSymbol(NameSymbol *name_symbol)
    {
        for (int i = table.Length() - 1; i >= 0; i--)
        {
            LabelSymbol *label = table[i] -> FindLabelSymbol(name_symbol);
            if (label)
                return label;
        }
        return (LabelSymbol *) NULL;
    }

private:
    Tuple<SymbolTable *> table;
};


//
//
//
class ExceptionTableStack
{
public:
    void Push(SymbolSet *set) { table.Next() = set; }
    void Pop() { if (table.Length() > 0) table.Reset(table.Length() - 1); }
    int Size() { return table.Length(); }
    SymbolSet *Top()
    {
        return (SymbolSet *) (table.Length() > 0
                              ? table[table.Length() - 1] : NULL);
    }

private:
    Tuple<SymbolSet *> table;
};


//
//
//
class StatementStack
{
public:
    void Push(Ast *stmt) { info.Next() = stmt; }
    void Pop() { if (info.Length() > 0) info.Reset(info.Length() - 1); }
    int Size() { return info.Length(); }
    Ast *Top()
    {
        return (Ast *) (info.Length() > 0 ? info[info.Length() - 1] : NULL);
    }

    Ast *operator[](const int i) { return info[i]; }

private:
    Tuple<Ast *> info;
};


//
//
//
class NestingLevelStack
{
public:
    void Push(int nesting) { info.Next() = nesting; }
    void Pop() { if (info.Length() > 0) info.Reset(info.Length() - 1); }
    int Size() { return info.Length(); }
    // Top returns 0 if the stack is empty, a valid value for no nesting depth
    int Top()
    {
        return (int) (info.Length() > 0 ? info[info.Length() - 1] : 0);
    }

    int operator[](const int i) { return info[i]; }

private:
    Tuple<int> info;
};


//
//
//
class BlockStack
{
public:
    int max_size;

    void Push(AstBlock *block_)
    {
        block.Next() = block_;
        index.Next() = 0;
        if (block.Length() > max_size)
            max_size = block.Length();
    }

    void Pop()
    {
        int len = block.Length() - 1;
        if (len >= 0)
        {
            block.Reset(len);
            index.Reset(len);
        }
    }

    int Size() { return block.Length(); }
    AstBlock *TopBlock()
    {
        return (AstBlock *) (block.Length() > 0
                             ? block[block.Length() - 1] : NULL);
    }

    AstBlock *operator[](const int i) { return block[i]; }

    int &TopMaxEnclosedVariableIndex()
    {
        if (index.Length() <= 0)
            assert(false);
        return index[index.Length() - 1];
    }

    BlockStack() : max_size(0) {}

private:
    Tuple<AstBlock *> block;
    Tuple<int> index;
};


//
//
//
class DefiniteFinalAssignmentStack
{
public:
    void Push() { info.Next().Reset(); }
    void Pop()  { if (info.Length() > 0) info.Reset(info.Length() - 1); }
    int Size()  { return info.Length(); }
    Tuple <AstExpression *> &Top()
    {
        if (info.Length() == 0) assert(false); return info[info.Length() - 1];
    }

private:
    Tuple< Tuple<AstExpression *> > info;
};


//
//
//
class DefinitePairs
{
public:
    DefinitePairs(int set_size) : break_pair(set_size),
                                  continue_pair(set_size),
                                  return_pair(set_size),
                                  throw_pair(set_size)
    {}

    DefinitePair break_pair,
                 continue_pair,
                 return_pair,
                 throw_pair;

    void UniverseInit()
    {
        break_pair.SetUniverse();
        continue_pair.SetUniverse();
        return_pair.SetUniverse();
        throw_pair.SetUniverse();
    }

    void EmptyInit()
    {
        break_pair.SetEmpty();
        continue_pair.SetEmpty();
        return_pair.SetEmpty();
        throw_pair.SetEmpty();
    }
};


//
//
//
class DefiniteBlockStack
{
public:

    void Push(AstBlock *block_)
    {
        definite_pairs[top_index] -> UniverseInit();

        if (locally_defined_variables)
        {
            if (top_index == 0)
            {
                memset(local_variables[top_index], 0,
                       (locally_defined_variables[top_index] -> Size() *
                        sizeof(VariableSymbol *)));
                locally_defined_variables[top_index] -> SetEmpty();
            }
            else
            {
                memmove(local_variables[top_index],
                        local_variables[top_index - 1],
                        (locally_defined_variables[top_index] -> Size() *
                         sizeof(VariableSymbol *)));
                *locally_defined_variables[top_index] =
                    *locally_defined_variables[top_index - 1];
            }
        }

        block[top_index] = block_;
        top_index++;
    }

    void Pop()
    {
        if (top_index > 0)
             top_index--;
        else assert(false);
    }

    int Size() { return top_index; }
    AstBlock *Block(int i) { return block[i]; }
    AstBlock *TopBlock()
    {
        assert(top_index > 0);
        return block[top_index - 1];
    }

    VariableSymbol **TopLocalVariables()
    {
        assert(top_index > 0 && local_variables);
        return local_variables[top_index - 1];
    }
    BitSet *TopLocallyDefinedVariables()
    {
        assert(top_index > 0 && locally_defined_variables);
        return locally_defined_variables[top_index - 1];
    }

    DefinitePair &BreakPair(int i)
    {
        return definite_pairs[i] -> break_pair;
    }
    DefinitePair &ContinuePair(int i)
    {
        return definite_pairs[i] -> continue_pair;
    }
    DefinitePair &ReturnPair(int i)
    {
        return definite_pairs[i] -> return_pair;
    }
    DefinitePair &ThrowPair(int i)
    {
        return definite_pairs[i] -> throw_pair;
    }

    DefinitePair &TopBreakPair()
    {
        assert(top_index > 0);
        return definite_pairs[top_index - 1] -> break_pair;
    }
    DefinitePair &TopContinuePair()
    {
        assert(top_index > 0);
        return definite_pairs[top_index - 1] -> continue_pair;
    }
    DefinitePair &TopReturnPair()
    {
        assert(top_index > 0);
        return definite_pairs[top_index - 1] -> return_pair;
    }
    DefinitePair &TopThrowPair()
    {
        assert(top_index > 0);
        return definite_pairs[top_index - 1] -> throw_pair;
    }

    DefinitePair &TopExitPair(DefinitePair &start_pair)
    {
        assert(top_index > 0);

        exit_pair  = start_pair;
        exit_pair *= TopBreakPair();
        exit_pair *= TopContinuePair();
        exit_pair *= TopReturnPair();
        exit_pair *= TopThrowPair();

        return exit_pair;
    }

    DefiniteBlockStack(Control &control, int stack_size_, int set_size)
        : stack_size(stack_size_),
          top_index(0),
          exit_pair(set_size)
    {
        block = new AstBlock*[stack_size];
        definite_pairs = new DefinitePairs*[stack_size];
        local_variables =
            (VariableSymbol ***) ((control.option.g & JikesOption::VARS)
                                  ? new VariableSymbol**[stack_size] : NULL);
        locally_defined_variables =
            (BitSet **) ((control.option.g & JikesOption::VARS)
                         ? new BitSet*[stack_size] : NULL);

        for (int i = 0; i < stack_size; i++)
        {
            definite_pairs[i] = new DefinitePairs(set_size);
            if (local_variables)
            {
                local_variables[i] = new VariableSymbol*[set_size];
                locally_defined_variables[i] = new BitSet(set_size);
            }
        }
    }

    ~DefiniteBlockStack()
    {
        for (int i = 0; i < stack_size; i++)
        {
            delete definite_pairs[i];
            if (local_variables)
            {
                delete [] local_variables[i];
                delete locally_defined_variables[i];
            }
        }

        delete [] block;
        delete [] definite_pairs;
        delete [] local_variables;
        delete [] locally_defined_variables;
    }

private:

    int stack_size,
        top_index;
    AstBlock **block;

    DefinitePairs **definite_pairs;

    BitSet **locally_defined_variables;
    VariableSymbol ***local_variables;

    DefinitePair exit_pair;
};


//
//
//
class DefiniteTryStack
{
public:

    void Push(AstTryStatement *try_statement_)
    {
        this -> try_statement[top_index] = try_statement_;
        top_index++;
    }

    void Pop()
    {
        if (top_index > 0)
             top_index--;
        else assert(false);
    }

    int Size() { return top_index; }
    AstTryStatement *TryStatement(int i) { return try_statement[i]; }
    AstBlock *Block(int i) { return block[i]; }
    AstBlock *TopBlock()
    {
        assert(top_index > 0);
        return block[top_index - 1];
    }
    void SetTopBlock(AstBlock *block_)
    {
        assert(top_index > 0);
        block[top_index - 1] = block_;
    }

    DefiniteTryStack(int stack_size_) : stack_size(stack_size_),
                                        top_index(0)
    {
        block = new AstBlock*[stack_size];
        try_statement = new AstTryStatement*[stack_size];
    }

    ~DefiniteTryStack()
    {
        delete [] block;
        delete [] try_statement;
    }

private:

    int stack_size,
        top_index;
    AstBlock **block;
    AstTryStatement **try_statement;
};


//
//
//
class SemanticEnvironment
{
public:

    Semantic *sem;
    SemanticEnvironment *previous;

    MethodSymbol *this_method;
    VariableSymbol *this_variable;
    AstStatement *explicit_constructor;
    Ast *ast_construct;

    SymbolTableStack symbol_table; // Points to symbol table on top of stack
    ExceptionTableStack try_exception_table_stack;
    StatementStack try_statement_stack,
                   breakable_statement_stack,
                   continuable_statement_stack;
    NestingLevelStack abrupt_finally_stack;
    BlockStack block_stack;

    SemanticEnvironment(Semantic *sem_, TypeSymbol *type_,
                        SemanticEnvironment *previous_ = NULL)
        : sem(sem_),
          previous(previous_),
          this_method(NULL),
          this_variable(NULL),
          explicit_constructor(NULL),
          ast_construct(NULL),
          _type(type_),
          next(NULL)
    {}


    ~SemanticEnvironment()
    {
        delete next; // if there was any clone, get rid of it
    }

    //
    // Clone the immediate environment of "this" Semantic environment. The
    // immediate environment consists primarily of the stack of symbol tables
    // that are necessary for looking up local variables in the immediate
    // environment.
    //
    SemanticEnvironment *GetEnvironment(Ast *ast)
    {
        SemanticEnvironment *clone = new SemanticEnvironment(sem, _type);
        clone -> this_method = this_method;
        clone -> this_variable = this_variable;
        clone -> explicit_constructor = explicit_constructor;
        clone -> ast_construct = ast;
        for (int i = 0; i < symbol_table.Size(); i++)
            clone -> symbol_table.Push(symbol_table[i]);
        clone -> next = next;
        next = clone;

        return clone;
    }

    TypeSymbol *Type() { return _type; }

    //
    // Are we in a region with no current instance? This applies to all
    // static initializers, static methods, and explicit constructor
    // invocations, including initializers in interfaces.
    //
    inline bool StaticRegion()
    {
        return ((this_variable && this_variable -> ACC_STATIC()) ||
                (this_method && this_method -> ACC_STATIC()) ||
                explicit_constructor);
    }

private:

    TypeSymbol *_type;
    SemanticEnvironment *next; // use to link an environment to its clones.
};


//
//
//
class SemanticEnvironmentStack
{
public:
    void Push(SemanticEnvironment *env) { info.Next() = env; }

    void Pop()
    {
        if (info.Length() > 0)
            info.Reset(info.Length() - 1);
    }

    int Size() { return info.Length(); }

    SemanticEnvironment *Top()
    {
        return (SemanticEnvironment *) (info.Length() > 0
                                        ? info[info.Length() - 1] : NULL);
    }

    SemanticEnvironment *operator[](const int i) { return info[i]; }

private:
    Tuple<SemanticEnvironment *> info;
};


class Semantic
{
public:
    //
    //
    //
    Control &control;
    FileSymbol *source_file_symbol;
    LexStream *lex_stream;
    AstCompilationUnit *compilation_unit;
    DirectorySymbol *directory_symbol;

    SymbolSet types_to_be_processed;

    int return_code;

    PackageSymbol *Package() { return this_package; }

    void CheckPackage();
    void ProcessTypeNames();
    void ProcessImports();
    void ProcessSuperTypeDependences(AstClassDeclaration *);
    void ProcessSuperTypeDependences(AstInterfaceDeclaration *);

    LiteralValue *ComputeFinalValue(AstVariableDeclarator *);

    Semantic(Control &control_, FileSymbol *file_symbol_)
        : control(control_),
          source_file_symbol(file_symbol_),
          lex_stream(file_symbol_ -> lex_stream),
          compilation_unit(file_symbol_ -> compilation_unit),
          directory_symbol(file_symbol_ -> directory_symbol),
          return_code(0),
          error(NULL),
          this_package(file_symbol_ -> package),
          definitely_assigned_variables(NULL),
          universe(NULL),
          blank_finals(NULL),
          definite_block_stack(NULL),
          definite_try_stack(NULL),
          definite_final_assignment_stack(NULL),
          definite_visible_variables(NULL),
          processing_simple_assignment(false)
    {
#ifdef JIKES_DEBUG
        for (int i = 0; i < Ast::_num_kinds; i++)
            ProcessExprOrStmt[i] = &Semantic::ProcessInvalidKind;
#endif
        // Semantic processing of expressions.
        ProcessExprOrStmt[Ast::IDENTIFIER] = &Semantic::ProcessSimpleName;
        ProcessExprOrStmt[Ast::DOT] = &Semantic::ProcessFieldAccess;
        ProcessExprOrStmt[Ast::INTEGER_LITERAL] =
            &Semantic::ProcessIntegerLiteral;
        ProcessExprOrStmt[Ast::LONG_LITERAL] = &Semantic::ProcessLongLiteral;
        ProcessExprOrStmt[Ast::FLOAT_LITERAL] = &Semantic::ProcessFloatLiteral;
        ProcessExprOrStmt[Ast::DOUBLE_LITERAL] =
            &Semantic::ProcessDoubleLiteral;
        ProcessExprOrStmt[Ast::TRUE_LITERAL] = &Semantic::ProcessTrueLiteral;
        ProcessExprOrStmt[Ast::FALSE_LITERAL] = &Semantic::ProcessFalseLiteral;
        ProcessExprOrStmt[Ast::STRING_LITERAL] =
            &Semantic::ProcessStringLiteral;
        ProcessExprOrStmt[Ast::CHARACTER_LITERAL] =
            &Semantic::ProcessCharacterLiteral;
        ProcessExprOrStmt[Ast::NULL_LITERAL] = &Semantic::ProcessNullLiteral;
        ProcessExprOrStmt[Ast::ARRAY_ACCESS] = &Semantic::ProcessArrayAccess;
        ProcessExprOrStmt[Ast::CALL] = &Semantic::ProcessMethodInvocation;
        ProcessExprOrStmt[Ast::THIS_EXPRESSION] =
            &Semantic::ProcessThisExpression;
        ProcessExprOrStmt[Ast::SUPER_EXPRESSION] =
            &Semantic::ProcessSuperExpression;
        ProcessExprOrStmt[Ast::PARENTHESIZED_EXPRESSION] =
            &Semantic::ProcessParenthesizedExpression;
        ProcessExprOrStmt[Ast::CLASS_CREATION] =
            &Semantic::ProcessClassInstanceCreationExpression;
        ProcessExprOrStmt[Ast::ARRAY_CREATION] =
            &Semantic::ProcessArrayCreationExpression;
        ProcessExprOrStmt[Ast::POST_UNARY] =
            &Semantic::ProcessPostUnaryExpression;
        ProcessExprOrStmt[Ast::PRE_UNARY] =
            &Semantic::ProcessPreUnaryExpression;
        ProcessExprOrStmt[Ast::CAST] = &Semantic::ProcessCastExpression;
        ProcessExprOrStmt[Ast::CHECK_AND_CAST] =
            &Semantic::ProcessCastExpression;
        ProcessExprOrStmt[Ast::BINARY] = &Semantic::ProcessBinaryExpression;
        ProcessExprOrStmt[Ast::TYPE] = &Semantic::ProcessTypeExpression;
        ProcessExprOrStmt[Ast::CONDITIONAL] =
            &Semantic::ProcessConditionalExpression;
        ProcessExprOrStmt[Ast::ASSIGNMENT] =
            &Semantic::ProcessAssignmentExpression;
        // Semantic processing of statements.
        ProcessExprOrStmt[Ast::CLASS] = &Semantic::ProcessClassDeclaration;
        ProcessExprOrStmt[Ast::THIS_CALL] = &Semantic::ProcessInvalidKind;
        ProcessExprOrStmt[Ast::SUPER_CALL] = &Semantic::ProcessInvalidKind;
        ProcessExprOrStmt[Ast::BLOCK] = &Semantic::ProcessBlock;
        ProcessExprOrStmt[Ast::LOCAL_VARIABLE_DECLARATION] =
            &Semantic::ProcessLocalVariableDeclarationStatement;
        ProcessExprOrStmt[Ast::IF] = &Semantic::ProcessIfStatement;
        ProcessExprOrStmt[Ast::EMPTY_STATEMENT] =
            &Semantic::ProcessEmptyStatement;
        ProcessExprOrStmt[Ast::EXPRESSION_STATEMENT] =
            &Semantic::ProcessExpressionStatement;
        ProcessExprOrStmt[Ast::SWITCH] = &Semantic::ProcessSwitchStatement;
        ProcessExprOrStmt[Ast::WHILE] = &Semantic::ProcessWhileStatement;
        ProcessExprOrStmt[Ast::DO] = &Semantic::ProcessDoStatement;
        ProcessExprOrStmt[Ast::FOR] = &Semantic::ProcessForStatement;
        ProcessExprOrStmt[Ast::BREAK] = &Semantic::ProcessBreakStatement;
        ProcessExprOrStmt[Ast::CONTINUE] = &Semantic::ProcessContinueStatement;
        ProcessExprOrStmt[Ast::RETURN] = &Semantic::ProcessReturnStatement;
        ProcessExprOrStmt[Ast::THROW] = &Semantic::ProcessThrowStatement;
        ProcessExprOrStmt[Ast::SYNCHRONIZED_STATEMENT] =
            &Semantic::ProcessSynchronizedStatement;
        ProcessExprOrStmt[Ast::ASSERT] = &Semantic::ProcessAssertStatement;
        ProcessExprOrStmt[Ast::TRY] = &Semantic::ProcessTryStatement;

        // Check statements for definite assignment.
        DefiniteStmt[Ast::CLASS] = &Semantic::DefiniteEmptyStatement;
        DefiniteStmt[Ast::THIS_CALL] = &Semantic::DefiniteEmptyStatement;
        DefiniteStmt[Ast::SUPER_CALL] = &Semantic::DefiniteEmptyStatement;
        DefiniteStmt[Ast::BLOCK] = &Semantic::DefiniteBlock;
        DefiniteStmt[Ast::LOCAL_VARIABLE_DECLARATION] =
            &Semantic::DefiniteLocalVariableDeclarationStatement;
        DefiniteStmt[Ast::IF] = &Semantic::DefiniteIfStatement;
        DefiniteStmt[Ast::EMPTY_STATEMENT] = &Semantic::DefiniteEmptyStatement;
        DefiniteStmt[Ast::EXPRESSION_STATEMENT] =
            &Semantic::DefiniteExpressionStatement;
        DefiniteStmt[Ast::SWITCH] = &Semantic::DefiniteSwitchStatement;
        DefiniteStmt[Ast::WHILE] = &Semantic::DefiniteWhileStatement;
        DefiniteStmt[Ast::DO] = &Semantic::DefiniteDoStatement;
        DefiniteStmt[Ast::FOR] = &Semantic::DefiniteForStatement;
        DefiniteStmt[Ast::BREAK] = &Semantic::DefiniteBreakStatement;
        DefiniteStmt[Ast::CONTINUE] = &Semantic::DefiniteContinueStatement;
        DefiniteStmt[Ast::RETURN] = &Semantic::DefiniteReturnStatement;
        DefiniteStmt[Ast::THROW] = &Semantic::DefiniteThrowStatement;
        DefiniteStmt[Ast::SYNCHRONIZED_STATEMENT] =
            &Semantic::DefiniteSynchronizedStatement;
        DefiniteStmt[Ast::ASSERT] = &Semantic::DefiniteAssertStatement;
        DefiniteStmt[Ast::TRY] = &Semantic::DefiniteTryStatement;

        DefiniteExpr[Ast::IDENTIFIER] = &Semantic::DefiniteSimpleName;
        DefiniteExpr[Ast::DOT] = &Semantic::DefiniteFieldAccess;
        DefiniteExpr[Ast::ARRAY_ACCESS] = &Semantic::DefiniteArrayAccess;
        DefiniteExpr[Ast::CALL] = &Semantic::DefiniteMethodInvocation;
        DefiniteExpr[Ast::PARENTHESIZED_EXPRESSION] =
            &Semantic::DefiniteParenthesizedExpression;
        DefiniteExpr[Ast::CLASS_CREATION] =
            &Semantic::DefiniteClassInstanceCreationExpression;
        DefiniteExpr[Ast::ARRAY_CREATION] =
            &Semantic::DefiniteArrayCreationExpression;
        DefiniteExpr[Ast::POST_UNARY] = &Semantic::DefinitePostUnaryExpression;
        DefiniteExpr[Ast::PRE_UNARY] = &Semantic::DefinitePreUnaryExpression;
        DefiniteExpr[Ast::CAST] = &Semantic::DefiniteCastExpression;
        DefiniteExpr[Ast::CHECK_AND_CAST] = &Semantic::DefiniteCastExpression;
        DefiniteExpr[Ast::BINARY] = &Semantic::DefiniteBinaryExpression;
        DefiniteExpr[Ast::CONDITIONAL] =
            &Semantic::DefiniteConditionalExpression;
        DefiniteExpr[Ast::ASSIGNMENT] =
            &Semantic::DefiniteAssignmentExpression;
        DefiniteExpr[Ast::INTEGER_LITERAL] =
            &Semantic::DefiniteDefaultExpression;
        DefiniteExpr[Ast::LONG_LITERAL] = &Semantic::DefiniteDefaultExpression;
        DefiniteExpr[Ast::FLOAT_LITERAL] =
            &Semantic::DefiniteDefaultExpression;
        DefiniteExpr[Ast::DOUBLE_LITERAL] =
            &Semantic::DefiniteDefaultExpression;
        DefiniteExpr[Ast::TRUE_LITERAL] = &Semantic::DefiniteDefaultExpression;
        DefiniteExpr[Ast::FALSE_LITERAL] =
            &Semantic::DefiniteDefaultExpression;
        DefiniteExpr[Ast::STRING_LITERAL] =
            &Semantic::DefiniteDefaultExpression;
        DefiniteExpr[Ast::CHARACTER_LITERAL] =
            &Semantic::DefiniteDefaultExpression;
        DefiniteExpr[Ast::NULL_LITERAL] = &Semantic::DefiniteDefaultExpression;
        DefiniteExpr[Ast::THIS_EXPRESSION] =
            &Semantic::DefiniteDefaultExpression;
        DefiniteExpr[Ast::SUPER_EXPRESSION] =
            &Semantic::DefiniteDefaultExpression;
        DefiniteExpr[Ast::TYPE] = &Semantic::DefiniteDefaultExpression;

        DefiniteBinaryExpr[AstBinaryExpression::PLUS] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::LEFT_SHIFT] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::RIGHT_SHIFT] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::UNSIGNED_RIGHT_SHIFT] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::LESS] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::GREATER] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::LESS_EQUAL] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::GREATER_EQUAL] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::AND] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::XOR] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::IOR] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::EQUAL_EQUAL] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::NOT_EQUAL] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::AND_AND] =
            &Semantic::DefiniteAND_AND;
        DefiniteBinaryExpr[AstBinaryExpression::OR_OR] =
            &Semantic::DefiniteOR_OR;
        DefiniteBinaryExpr[AstBinaryExpression::STAR] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::MINUS] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::SLASH] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::MOD] =
            &Semantic::DefiniteDefaultBinaryExpression;
        DefiniteBinaryExpr[AstBinaryExpression::INSTANCEOF] =
            &Semantic::DefiniteDefaultBinaryExpression;

        DefinitePreUnaryExpr[AstPreUnaryExpression::PLUS] =
            &Semantic::DefiniteDefaultPreUnaryExpression;
        DefinitePreUnaryExpr[AstPreUnaryExpression::MINUS] =
            &Semantic::DefiniteDefaultPreUnaryExpression;
        DefinitePreUnaryExpr[AstPreUnaryExpression::TWIDDLE] =
            &Semantic::DefiniteDefaultPreUnaryExpression;
        DefinitePreUnaryExpr[AstPreUnaryExpression::NOT] =
            &Semantic::DefiniteNOT;
        DefinitePreUnaryExpr[AstPreUnaryExpression::PLUSPLUS] =
            &Semantic::DefinitePLUSPLUSOrMINUSMINUS;
        DefinitePreUnaryExpr[AstPreUnaryExpression::MINUSMINUS] =
            &Semantic::DefinitePLUSPLUSOrMINUSMINUS;

        ProcessBinaryExpr[AstBinaryExpression::PLUS] = &Semantic::ProcessPLUS;
        ProcessBinaryExpr[AstBinaryExpression::LEFT_SHIFT] =
            &Semantic::ProcessLEFT_SHIFT;
        ProcessBinaryExpr[AstBinaryExpression::RIGHT_SHIFT] =
            &Semantic::ProcessRIGHT_SHIFT;
        ProcessBinaryExpr[AstBinaryExpression::UNSIGNED_RIGHT_SHIFT] =
            &Semantic::ProcessUNSIGNED_RIGHT_SHIFT;
        ProcessBinaryExpr[AstBinaryExpression::LESS] = &Semantic::ProcessLESS;
        ProcessBinaryExpr[AstBinaryExpression::GREATER] =
            &Semantic::ProcessGREATER;
        ProcessBinaryExpr[AstBinaryExpression::LESS_EQUAL] =
            &Semantic::ProcessLESS_EQUAL;
        ProcessBinaryExpr[AstBinaryExpression::GREATER_EQUAL] =
            &Semantic::ProcessGREATER_EQUAL;
        ProcessBinaryExpr[AstBinaryExpression::AND] = &Semantic::ProcessAND;
        ProcessBinaryExpr[AstBinaryExpression::XOR] = &Semantic::ProcessXOR;
        ProcessBinaryExpr[AstBinaryExpression::IOR] = &Semantic::ProcessIOR;
        ProcessBinaryExpr[AstBinaryExpression::AND_AND] =
            &Semantic::ProcessAND_AND;
        ProcessBinaryExpr[AstBinaryExpression::OR_OR] =
            &Semantic::ProcessOR_OR;
        ProcessBinaryExpr[AstBinaryExpression::EQUAL_EQUAL] =
            &Semantic::ProcessEQUAL_EQUAL;
        ProcessBinaryExpr[AstBinaryExpression::NOT_EQUAL] =
            &Semantic::ProcessNOT_EQUAL;
        ProcessBinaryExpr[AstBinaryExpression::STAR] = &Semantic::ProcessSTAR;
        ProcessBinaryExpr[AstBinaryExpression::MINUS] =
            &Semantic::ProcessMINUS;
        ProcessBinaryExpr[AstBinaryExpression::SLASH] =
            &Semantic::ProcessSLASH;
        ProcessBinaryExpr[AstBinaryExpression::MOD] = &Semantic::ProcessMOD;
        ProcessBinaryExpr[AstBinaryExpression::INSTANCEOF] =
            &Semantic::ProcessINSTANCEOF;

        ProcessPreUnaryExpr[AstPreUnaryExpression::PLUS] =
            &Semantic::ProcessPLUS;
        ProcessPreUnaryExpr[AstPreUnaryExpression::MINUS] =
            &Semantic::ProcessMINUS;
        ProcessPreUnaryExpr[AstPreUnaryExpression::TWIDDLE] =
            &Semantic::ProcessTWIDDLE;
        ProcessPreUnaryExpr[AstPreUnaryExpression::NOT] =
            &Semantic::ProcessNOT;
        ProcessPreUnaryExpr[AstPreUnaryExpression::PLUSPLUS] =
            &Semantic::ProcessPLUSPLUSOrMINUSMINUS;
        ProcessPreUnaryExpr[AstPreUnaryExpression::MINUSMINUS] =
            &Semantic::ProcessPLUSPLUSOrMINUSMINUS;
    }

    ~Semantic() { delete error; }

    void ReportSemError(SemanticError::SemanticErrorKind kind,
                        LexStream::TokenIndex ltok,
                        LexStream::TokenIndex rtok,
                        wchar_t *s1 = NULL,
                        wchar_t *s2 = NULL,
                        wchar_t *s3 = NULL,
                        wchar_t *s4 = NULL,
                        wchar_t *s5 = NULL,
                        wchar_t *s6 = NULL,
                        wchar_t *s7 = NULL,
                        wchar_t *s8 = NULL,
                        wchar_t *s9 = NULL)
    {
        if (! error)
            error = new SemanticError(control, source_file_symbol);
        error -> Report(kind, ltok, rtok, s1, s2, s3, s4, s5, s6, s7, s8, s9);
    }

    int NumErrors() { return (error ? error -> num_errors : 0); }

    //
    // If we had a bad compilation unit, print the parser messages.
    // If semantic errors were detected print them too.
    // Set the return code.
    //
    void PrintMessages()
    {
        if (this != control.system_semantic)
        {
            if (lex_stream -> NumBadTokens() > 0)
            {
                lex_stream -> PrintMessages();
                return_code = 1;
            }
            else if (lex_stream -> NumWarnTokens() > 0)
                lex_stream -> PrintMessages();

            if (! compilation_unit ||
                compilation_unit -> BadCompilationUnitCast())
            {
                DiagnoseParser *diagnose_parser =
                    new DiagnoseParser(control, lex_stream);
                return_code = 1;
                delete diagnose_parser;
            }

            if (! control.option.nocleanup && compilation_unit)
                CleanUp();
        }

        if (error && error -> error.Length() > 0 &&
            error -> PrintMessages() > return_code)
        {
            return_code = 1;
        }

        //
        // Once we have processed the errors, reset the error object
        //
        delete error;
        error = NULL;
    }

    TypeSymbol *ProcessSignature(TypeSymbol *, const char *,
                                 LexStream::TokenIndex);
    TypeSymbol *ReadType(FileSymbol *, PackageSymbol *, NameSymbol *,
                         LexStream::TokenIndex);
    TypeSymbol *ReadTypeFromSignature(TypeSymbol *, const char *, int,
                                      LexStream::TokenIndex);
    TypeSymbol *ProcessNestedType(TypeSymbol *, NameSymbol *,
                                  LexStream::TokenIndex);

    inline bool IsConstantTrue(AstExpression *expr)
    {
        return expr -> IsConstant() &&
            expr -> Type() == control.boolean_type &&
            DYNAMIC_CAST<IntLiteralValue *> (expr -> value) -> value;
    }
    inline bool IsConstantFalse(AstExpression *expr)
    {
        return expr -> IsConstant() &&
            expr -> Type() == control.boolean_type &&
            ! DYNAMIC_CAST<IntLiteralValue *> (expr -> value) -> value;
    }

private:
    enum
    {
        INT_SHIFT_MASK = 0x1f,
        LONG_SHIFT_MASK = 0x3f
    };

    SemanticError *error;

    void CleanUp();
    void CleanUpType(TypeSymbol *);

    void SetDefaultSuperType(AstClassDeclaration *);
    void SetDefaultSuperType(AstInterfaceDeclaration *);
    void ProcessTypeHeader(AstClassDeclaration *);
    void MarkCircularNest(TypeSymbol *);
    void ProcessSuperTypesOfOuterType(TypeSymbol *);
    void ProcessSuperTypesOfInnerType(TypeSymbol *, Tuple<TypeSymbol *> &);
    void ProcessTypeHeaders(AstClassDeclaration *);
    TypeSymbol *FindTypeInLayer(Ast *, SymbolSet &);
    void ProcessNestedSuperTypes(TypeSymbol *);
    void ProcessNestedTypeHeaders(TypeSymbol *, AstClassBody *);
    void ProcessTypeHeader(AstInterfaceDeclaration *);
    void ProcessTypeHeaders(AstInterfaceDeclaration *);
    void ProcessNestedTypeHeaders(AstInterfaceDeclaration *);
    void ProcessConstructorMembers(AstClassBody *);
    void ProcessMethodMembers(AstClassBody *);
    void ProcessFieldMembers(AstClassBody *);
    void ProcessMembers(SemanticEnvironment *, AstClassBody *);
    void CompleteSymbolTable(SemanticEnvironment *, LexStream::TokenIndex,
                             AstClassBody *);
    void ProcessExecutableBodies(SemanticEnvironment *, AstClassBody *);
    void ProcessExecutableBodies(AstInterfaceDeclaration *);

    void ProcessMethodMembers(AstInterfaceDeclaration *);
    void ProcessFieldMembers(AstInterfaceDeclaration *);
    void ProcessMembers(AstInterfaceDeclaration *);
    void CompleteSymbolTable(AstInterfaceDeclaration *);

    friend class TypeSymbol;

    Tuple<Symbol *> import_on_demand_packages;
    Tuple<TypeSymbol *> single_type_imports;

    //
    // Where am I?
    //
    PackageSymbol  *this_package;

    TypeSymbol *ThisType()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> Type();
    }
    MethodSymbol *&ThisMethod()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> this_method;
    }
    VariableSymbol *&ThisVariable()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> this_variable;
    }
    AstStatement *&ExplicitConstructorInvocation()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> explicit_constructor;
    }
    SymbolTableStack &LocalSymbolTable()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> symbol_table;
    }
    ExceptionTableStack &TryExceptionTableStack()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> try_exception_table_stack;
    }
    StatementStack &TryStatementStack()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> try_statement_stack;
    }
    StatementStack &BreakableStatementStack()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> breakable_statement_stack;
    }
    StatementStack &ContinuableStatementStack()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> continuable_statement_stack;
    }
    NestingLevelStack &AbruptFinallyStack()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> abrupt_finally_stack;
    }
    BlockStack &LocalBlockStack()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> block_stack;
    }
    SemanticEnvironment *GetEnvironment(Ast *ast)
    {
        assert(state_stack.Size());
        return state_stack.Top() -> GetEnvironment(ast);
    }
    bool StaticRegion()
    {
        assert(state_stack.Size());
        return state_stack.Top() -> StaticRegion();
    }

    SemanticEnvironmentStack state_stack;

    DefinitePair *definitely_assigned_variables,
                 *universe;
    BitSet *blank_finals;
    DefiniteBlockStack *definite_block_stack;
    DefiniteTryStack *definite_try_stack;
    DefiniteFinalAssignmentStack *definite_final_assignment_stack;
    SymbolSet *definite_visible_variables;

    bool IsIntValueRepresentableInType(AstExpression *, TypeSymbol *);

    void CheckClassMembers(TypeSymbol *, AstClassBody *);
    void CheckNestedTypeDuplication(SemanticEnvironment *,
                                    LexStream::TokenIndex);
    TypeSymbol *ProcessNestedClassName(TypeSymbol *, AstClassDeclaration *);
    void CheckInterfaceMembers(TypeSymbol *, AstInterfaceDeclaration *);
    TypeSymbol *ProcessNestedInterfaceName(TypeSymbol *,
                                           AstInterfaceDeclaration *);
    TypeSymbol *FindTypeInShadow(TypeShadowSymbol *, LexStream::TokenIndex);
    void ReportTypeInaccessible(LexStream::TokenIndex, LexStream::TokenIndex,
                                TypeSymbol *);
    void ReportTypeInaccessible(Ast *ast, TypeSymbol *type)
    {
        ReportTypeInaccessible(ast -> LeftToken(), ast -> RightToken(), type);
    }
    TypeSymbol *GetBadNestedType(TypeSymbol *, LexStream::TokenIndex);
    TypeSymbol *FindNestedType(TypeSymbol *, LexStream::TokenIndex);
    TypeSymbol *MustFindNestedType(TypeSymbol *, Ast *);
    void ProcessImportQualifiedName(AstExpression *);
    void ProcessPackageOrType(AstExpression *);
    void ProcessTypeImportOnDemandDeclaration(AstImportDeclaration *);
    AstExpression *FindFirstType(Ast *);
    TypeSymbol *FindSimpleNameType(PackageSymbol *, LexStream::TokenIndex);
    void ProcessSingleTypeImportDeclaration(AstImportDeclaration *);
    AccessFlags ProcessClassModifiers(AstClassDeclaration *);
    AccessFlags ProcessLocalClassModifiers(AstClassDeclaration *);
    AccessFlags ProcessNestedClassModifiers(AstClassDeclaration *);
    AccessFlags ProcessStaticNestedClassModifiers(AstClassDeclaration *);
    AccessFlags ProcessInterfaceModifiers(AstInterfaceDeclaration *);
    AccessFlags ProcessNestedInterfaceModifiers(AstInterfaceDeclaration *);
    AccessFlags ProcessStaticNestedInterfaceModifiers(AstInterfaceDeclaration *);
    AccessFlags ProcessFieldModifiers(AstFieldDeclaration *);
    AccessFlags ProcessLocalModifiers(AstLocalVariableDeclarationStatement *);
    AccessFlags ProcessFormalModifiers(AstFormalParameter *);
    AccessFlags ProcessMethodModifiers(AstMethodDeclaration *);
    AccessFlags ProcessConstructorModifiers(AstConstructorDeclaration *);
    AccessFlags ProcessInterfaceFieldModifiers(AstFieldDeclaration *);
    AccessFlags ProcessInterfaceMethodModifiers(AstMethodDeclaration *);
    void AddDefaultConstructor(TypeSymbol *);
    void ProcessConstructorDeclaration(AstConstructorDeclaration *);
    void ProcessMethodDeclaration(AstMethodDeclaration *);
    void ProcessFieldDeclaration(AstFieldDeclaration *);
    void ProcessFormalParameters(BlockSymbol *, AstMethodDeclarator *);
    TypeSymbol *ImportType(LexStream::TokenIndex, NameSymbol *);
    TypeSymbol *FindPrimitiveType(AstPrimitiveType *);
    TypeSymbol *FindTypeInEnvironment(SemanticEnvironment *, NameSymbol *);
    TypeSymbol *FindType(LexStream::TokenIndex);
    TypeSymbol *MustFindType(Ast *);
    void ProcessInterface(TypeSymbol *, AstExpression *);

    void InitializeVariable(AstFieldDeclaration *, MethodSymbol *,
                            Tuple<VariableSymbol *> &);
    void ProcessInitializer(AstMethodBody *, MethodSymbol *,
                            Tuple<VariableSymbol *> &);
    void ProcessStaticInitializers(AstClassBody *);
    void ProcessInstanceInitializers(AstClassBody *);
    MethodSymbol *GetStaticInitializerMethod(int estimate = 0);

    bool CanWideningPrimitiveConvert(TypeSymbol *, TypeSymbol *);
    bool CanNarrowingPrimitiveConvert(TypeSymbol *, TypeSymbol *);
    bool CanCastConvert(TypeSymbol *, TypeSymbol *,
                        LexStream::TokenIndex = LexStream::BadToken());
    bool CanMethodInvocationConvert(TypeSymbol *, TypeSymbol *);
    bool CanAssignmentConvert(TypeSymbol *, AstExpression *);
    bool CanAssignmentConvertReference(TypeSymbol *, TypeSymbol *);
    LiteralValue *CastValue(TypeSymbol *, AstExpression *);
    AstExpression *ConvertToType(AstExpression *, TypeSymbol *);
    AstExpression *PromoteUnaryNumericExpression(AstExpression *);
    void BinaryNumericPromotion(AstAssignmentExpression *);
    void BinaryNumericPromotion(AstBinaryExpression *);
    void BinaryNumericPromotion(AstConditionalExpression *);

    void (Semantic::*DefiniteStmt[Ast::_num_kinds])(Ast *);
    inline void DefiniteStatement(Ast *);

    void DefiniteLoopBody(BitSet *);

    void DefiniteBlock(Ast *);
    void DefiniteLocalVariableDeclarationStatement(Ast *);
    void DefiniteExpressionStatement(Ast *);
    void DefiniteSynchronizedStatement(Ast *);
    void DefiniteIfStatement(Ast *);
    void DefiniteWhileStatement(Ast *);
    void DefiniteForStatement(Ast *);
    void DefiniteSwitchStatement(Ast *);
    void DefiniteDoStatement(Ast *);
    void DefiniteBreakStatement(Ast *);
    void DefiniteContinueStatement(Ast *);
    void DefiniteReturnStatement(Ast *);
    void DefiniteThrowStatement(Ast *);
    void DefiniteTryStatement(Ast *);
    void DefiniteAssertStatement(Ast *);
    void DefiniteEmptyStatement(Ast *);
    void DefiniteClassDeclaration(Ast *);

    VariableSymbol *DefiniteFinal(AstFieldAccess *);

    DefiniteAssignmentSet *(Semantic::*DefiniteExpr[Ast::_num_expression_kinds])(AstExpression *, DefinitePair &);
    DefiniteAssignmentSet *DefiniteSimpleName(AstExpression *, DefinitePair &);
    DefiniteAssignmentSet *DefiniteArrayAccess(AstExpression *,
                                               DefinitePair &);
    DefiniteAssignmentSet *DefiniteMethodInvocation(AstExpression *,
                                                    DefinitePair &);
    DefiniteAssignmentSet *DefiniteClassInstanceCreationExpression(AstExpression *,
                                                                   DefinitePair &);
    DefiniteAssignmentSet *DefiniteArrayCreationExpression(AstExpression *,
                                                           DefinitePair &);
    DefiniteAssignmentSet *DefinitePreUnaryExpression(AstExpression *,
                                                      DefinitePair &);
    DefiniteAssignmentSet *DefinitePostUnaryExpression(AstExpression *,
                                                       DefinitePair &);
    DefiniteAssignmentSet *DefiniteBinaryExpression(AstExpression *,
                                                    DefinitePair &);
    DefiniteAssignmentSet *DefiniteConditionalExpression(AstExpression *,
                                                         DefinitePair &);
    DefiniteAssignmentSet *DefiniteAssignmentExpression(AstExpression *,
                                                        DefinitePair &);
    DefiniteAssignmentSet *DefiniteDefaultExpression(AstExpression *,
                                                     DefinitePair &);
    DefiniteAssignmentSet *DefiniteFieldAccess(AstExpression *,
                                               DefinitePair &);
    DefiniteAssignmentSet *DefiniteParenthesizedExpression(AstExpression *,
                                                           DefinitePair &);
    DefiniteAssignmentSet *DefiniteCastExpression(AstExpression *,
                                                  DefinitePair &);
    DefiniteAssignmentSet *DefiniteBooleanExpression(AstExpression *,
                                                     DefinitePair &);
    void DefiniteExpression(AstExpression *, DefinitePair &);

    DefiniteAssignmentSet *(Semantic::*DefinitePreUnaryExpr[AstPreUnaryExpression::_num_kinds])(AstExpression *, DefinitePair &);
    DefiniteAssignmentSet *DefiniteDefaultPreUnaryExpression(AstExpression *,
                                                             DefinitePair &);
    DefiniteAssignmentSet *DefiniteNOT(AstExpression *, DefinitePair &);
    DefiniteAssignmentSet *DefinitePLUSPLUSOrMINUSMINUS(AstExpression *,
                                                        DefinitePair &);

    DefiniteAssignmentSet *(Semantic::*DefiniteBinaryExpr[AstBinaryExpression::_num_kinds])(AstBinaryExpression *, DefinitePair &);
    DefiniteAssignmentSet *DefiniteDefaultBinaryExpression(AstBinaryExpression *,
                                                           DefinitePair &);
    DefiniteAssignmentSet *DefiniteAND_AND(AstBinaryExpression *,
                                           DefinitePair &);
    DefiniteAssignmentSet *DefiniteOR_OR(AstBinaryExpression *,
                                         DefinitePair &);

    void DefiniteArrayInitializer(AstArrayInitializer *, DefinitePair &);
    void DefiniteArrayInitializer(AstArrayInitializer *);
    void DefiniteVariableInitializer(AstVariableDeclarator *);
    void DefiniteBlockStatements(AstBlock *);
    void DefiniteMethodBody(AstMethodDeclaration *, Tuple<VariableSymbol *> &);
    void DefiniteConstructorBody(AstConstructorDeclaration *,
                                 Tuple<VariableSymbol *> &);
    void DefiniteBlockInitializer(AstBlock *, int, Tuple<VariableSymbol *> &);
    void DefiniteVariableInitializer(AstVariableDeclarator *,
                                     Tuple<VariableSymbol *> &);

    void ProcessBlockStatements(AstBlock *);
    void ProcessThisCall(AstThisCall *);
    void ProcessSuperCall(AstSuperCall *);
    void CheckThrow(AstExpression *);
    void ProcessMethodBody(AstMethodDeclaration *);
    void ProcessConstructorBody(AstConstructorDeclaration *);
    bool UncaughtException(TypeSymbol *);
    wchar_t *UncaughtExceptionContext();
    void ReportConstructorNotFound(Ast *, TypeSymbol *);
    void ReportMethodNotFound(AstMethodInvocation *, TypeSymbol *);
    MethodSymbol *FindConstructor(TypeSymbol *, Ast *,
                                  LexStream::TokenIndex,
                                  LexStream::TokenIndex);
    bool MoreSpecific(MethodSymbol *, MethodSymbol *);
    bool MoreSpecific(MethodSymbol *, Tuple<MethodSymbol *> &);
    bool NoMethodMoreSpecific(Tuple<MethodSymbol *> &, MethodSymbol *);
    void FindMethodInEnvironment(Tuple<MethodSymbol *> &,
                                 SemanticEnvironment *&,
                                 SemanticEnvironment *, AstMethodInvocation *);
    MethodSymbol *FindMisspelledMethodName(TypeSymbol *,
                                           AstMethodInvocation *,
                                           NameSymbol *);
    MethodSymbol *FindMethodInEnvironment(SemanticEnvironment *&,
                                          SemanticEnvironment *,
                                          AstMethodInvocation *);
    MethodSymbol *FindMethodInType(TypeSymbol *, AstMethodInvocation *,
                                   NameSymbol * = NULL);

    void ReportVariableNotFound(AstExpression *, TypeSymbol *);
    void FindVariableInEnvironment(Tuple<VariableSymbol *> &,
                                   SemanticEnvironment *&,
                                   SemanticEnvironment *, NameSymbol *,
                                   LexStream::TokenIndex);
    VariableSymbol *FindMisspelledVariableName(TypeSymbol *,
                                               LexStream::TokenIndex);
    VariableSymbol *FindVariableInEnvironment(SemanticEnvironment *&,
                                              SemanticEnvironment *,
                                              LexStream::TokenIndex);
    VariableSymbol *FindVariableInType(TypeSymbol *, AstFieldAccess *,
                                       NameSymbol * = NULL);
    VariableSymbol *FindLocalVariable(VariableSymbol *, TypeSymbol *);
    AstExpression *FindEnclosingInstance(AstExpression *, TypeSymbol *, bool);
    AstExpression *CreateAccessToType(Ast *, TypeSymbol *);
    void CreateAccessToScopedVariable(AstSimpleName *, TypeSymbol *);
    void CreateAccessToScopedMethod(AstMethodInvocation *, TypeSymbol *);

    bool TypeAccessCheck(Ast *, TypeSymbol *);
    void TypeNestAccessCheck(AstExpression *);
    bool ConstructorAccessCheck(Ast *, MethodSymbol *);
    bool MemberAccessCheck(AstFieldAccess *, TypeSymbol *, Symbol *);
    void SimpleNameAccessCheck(AstSimpleName *, TypeSymbol *, Symbol *);
    bool ProtectedAccessCheck(TypeSymbol *);

    void (Semantic::*ProcessPreUnaryExpr[AstPreUnaryExpression::_num_kinds])(AstPreUnaryExpression *);
    void ProcessPLUS(AstPreUnaryExpression *);
    void ProcessMINUS(AstPreUnaryExpression *);
    void ProcessTWIDDLE(AstPreUnaryExpression *);
    void ProcessNOT(AstPreUnaryExpression *);
    void ProcessPLUSPLUSOrMINUSMINUS(AstPreUnaryExpression *);

    void (Semantic::*ProcessBinaryExpr[AstBinaryExpression::_num_kinds])(AstBinaryExpression *);
    void ProcessPLUS(AstBinaryExpression *);
    void ProcessLEFT_SHIFT(AstBinaryExpression *);
    void ProcessRIGHT_SHIFT(AstBinaryExpression *);
    void ProcessUNSIGNED_RIGHT_SHIFT(AstBinaryExpression *);
    void ProcessLESS(AstBinaryExpression *);
    void ProcessGREATER(AstBinaryExpression *);
    void ProcessLESS_EQUAL(AstBinaryExpression *);
    void ProcessGREATER_EQUAL(AstBinaryExpression *);
    void ProcessAND(AstBinaryExpression *);
    void ProcessXOR(AstBinaryExpression *);
    void ProcessIOR(AstBinaryExpression *);
    void ProcessAND_AND(AstBinaryExpression *);
    void ProcessOR_OR(AstBinaryExpression *);
    void ProcessEQUAL_EQUAL(AstBinaryExpression *);
    void ProcessNOT_EQUAL(AstBinaryExpression *);
    void ProcessSTAR(AstBinaryExpression *);
    void ProcessMINUS(AstBinaryExpression *);
    void ProcessSLASH(AstBinaryExpression *);
    void ProcessMOD(AstBinaryExpression *);
    void ProcessINSTANCEOF(AstBinaryExpression *);

    void FindMethodMember(TypeSymbol *, AstMethodInvocation *);
    void ProcessMethodName(AstMethodInvocation *);

    //
    // An array of member methods, to dispatch the various expressions and
    // statements for processing.
    //
    void (Semantic::*ProcessExprOrStmt[Ast::_num_kinds])(Ast *);

    inline void ProcessStatement(AstStatement *stmt)
    {
        (this ->* ProcessExprOrStmt[stmt -> kind])(stmt);
    }

    inline void ProcessExpression(AstExpression *expr)
    {
        if (expr -> symbol)
            // already processed, make sure it was compiler-generated
            assert(expr -> generated);
        else (this ->* ProcessExprOrStmt[expr -> kind])(expr);
    }

    inline void ProcessExpressionOrStringConstant(AstExpression *expr)
    {
        ProcessExpression(expr);
        //
        // If the expression is of type String, check whether or not it is
        // constant, and if so, compute the result.
        //
        if (expr -> symbol == control.String() && (! expr -> IsConstant()))
            control.Utf8_pool.CheckStringConstant(expr);
    }

    void ProcessLocalVariableDeclarationStatement(Ast *);
    void ProcessBlock(Ast *);
    void ProcessForStatement(Ast *);
    void ProcessSwitchStatement(Ast *);
    void ProcessThrowStatement(Ast *);
    void ProcessTryStatement(Ast *);
    void ProcessExpressionStatement(Ast *);
    void ProcessSynchronizedStatement(Ast *);
    void ProcessIfStatement(Ast *);
    void ProcessWhileStatement(Ast *);
    void ProcessDoStatement(Ast *);
    void ProcessBreakStatement(Ast *);
    void ProcessContinueStatement(Ast *);
    void ProcessReturnStatement(Ast *);
    void ProcessAssertStatement(Ast *);
    void ProcessEmptyStatement(Ast *);
    void ProcessInvalidKind(Ast *ast)
    {
        assert(ast -> IsExplicitConstructorInvocation());
        AstStatement *statement = (AstStatement *) ast;
        statement -> can_complete_normally = statement -> is_reachable;
        ReportSemError(SemanticError::MISPLACED_EXPLICIT_CONSTRUCTOR,
                       statement -> LeftToken(),
                       statement -> RightToken());
    }

    TypeSymbol *GetLocalType(AstClassDeclaration *);
    void ProcessClassDeclaration(Ast *);

    //
    // CheckSimpleName must behave differently if we are in the middle
    // of a simple assignment. See expr.cpp.
    //
    bool processing_simple_assignment;

    void CheckSimpleName(AstSimpleName *, SemanticEnvironment *where_found);
    void ProcessSimpleName(Ast *);
    void FindVariableMember(TypeSymbol *, AstFieldAccess *);
    void ProcessAmbiguousName(Ast *);
    void ProcessFieldAccess(Ast *);
    void ProcessIntegerLiteral(Ast *);
    void ProcessLongLiteral(Ast *);
    void ProcessFloatLiteral(Ast *);
    void ProcessDoubleLiteral(Ast *);
    void ProcessTrueLiteral(Ast *);
    void ProcessFalseLiteral(Ast *);
    void ProcessStringLiteral(Ast *);
    void ProcessCharacterLiteral(Ast *);
    void ProcessArrayAccess(Ast *);
    void ProcessMethodInvocation(Ast *);
    void ProcessNullLiteral(Ast *);
    void ProcessThisExpression(Ast *);
    void ProcessSuperExpression(Ast *);
    void ProcessParenthesizedExpression(Ast *);
    void UpdateLocalConstructors(TypeSymbol *);
    void GetAnonymousConstructor(AstClassInstanceCreationExpression *,
                                 TypeSymbol *);
    TypeSymbol *GetAnonymousType(AstClassInstanceCreationExpression *,
                                 TypeSymbol *);
    void ProcessClassInstanceCreationExpression(Ast *);
    void ProcessArrayCreationExpression(Ast *);
    void ProcessPostUnaryExpression(Ast *);
    void ProcessPreUnaryExpression(Ast *);
    void ProcessCastExpression(Ast *);
    void ProcessBinaryExpression(Ast *);
    void ProcessTypeExpression(Ast *);
    void ProcessConditionalExpression(Ast *);
    void ProcessAssignmentExpression(Ast *);

    void ProcessVariableInitializer(AstVariableDeclarator *);
    void ProcessArrayInitializer(AstArrayInitializer *, TypeSymbol *);

    void CheckMethodOverride(MethodSymbol *, MethodSymbol *, TypeSymbol *);
    void AddInheritedTypes(TypeSymbol *, TypeSymbol *);
    void AddInheritedFields(TypeSymbol *, TypeSymbol *);
    void AddInheritedMethods(TypeSymbol *, TypeSymbol *,
                             LexStream::TokenIndex);
    void ComputeTypesClosure(TypeSymbol *, LexStream::TokenIndex);
    void ComputeFieldsClosure(TypeSymbol *, LexStream::TokenIndex);
    void ComputeMethodsClosure(TypeSymbol *, LexStream::TokenIndex);

    inline bool InRange(const char *buffer_ptr, const char *buffer_tail,
                        int size)
    {
        return ((buffer_ptr + size) <= buffer_tail);
    }
    TypeSymbol *RetrieveNestedTypes(TypeSymbol *, wchar_t *,
                                    LexStream::TokenIndex);
    TypeSymbol *GetClassPool(TypeSymbol *, TypeSymbol **, const char **, int,
                             LexStream::TokenIndex);
    void ProcessBadClass(TypeSymbol *, LexStream::TokenIndex);
    bool ProcessClassFile(TypeSymbol *, const char *, int,
                          LexStream::TokenIndex);
    void ReadClassFile(TypeSymbol *, LexStream::TokenIndex);

    //
    // Any exception that is neither RuntimeException or one of its subclasses
    // nor Error or one of its subclasses is a checked exception. This also
    // ignores invalid types.
    //
    inline bool CheckedException(TypeSymbol *exception)
    {
        return (! exception -> IsSubclass(control.RuntimeException()) &&
                ! exception -> IsSubclass(control.Error()) &&
                exception != control.no_type);
    }

public:

    static inline u1 GetU1(const char *);
    static inline u2 GetU2(const char *);
    static inline u4 GetU4(const char *);

    static inline u1 GetAndSkipU1(const char *&);
    static inline u2 GetAndSkipU2(const char *&);
    static inline u4 GetAndSkipU4(const char *&);
    static inline void Skip(const char *&, int);

    inline void AddDependence(TypeSymbol *, TypeSymbol *,
                              LexStream::TokenIndex, bool = false);
    inline void SetObjectSuperType(TypeSymbol *, LexStream::TokenIndex);
    inline void AddStringConversionDependence(TypeSymbol *,
                                              LexStream::TokenIndex);
};


inline void Semantic::AddDependence(TypeSymbol *base_type,
                                    TypeSymbol *parent_type,
                                    LexStream::TokenIndex tok,
                                    bool static_access)
{
    if (base_type != control.no_type)
    {
        base_type = base_type -> outermost_type;
        parent_type = parent_type -> outermost_type;

        parent_type -> dependents -> AddElement(base_type);
        if (static_access)
             base_type -> static_parents -> AddElement(parent_type);
        else base_type -> parents -> AddElement(parent_type);

        //
        // It is not possible to import from the unnamed package, and without
        // imports, it is impossible to reference a class in the unnamed
        // package from a package.
        //
        assert(parent_type -> ContainingPackage() != control.unnamed_package ||
               base_type -> ContainingPackage() == control.unnamed_package);
    }
}

inline void Semantic::SetObjectSuperType(TypeSymbol *type,
                                         LexStream::TokenIndex tok)
{
    type -> super = control.Object();
    AddDependence(type, type -> super, tok);
}

inline void Semantic::AddStringConversionDependence(TypeSymbol *type,
                                                    LexStream::TokenIndex tok)
{
    if (type == control.null_type)
         ;
    else if (type == control.boolean_type)
         AddDependence(ThisType(), control.Boolean(), tok);
    else if (type == control.char_type)
         AddDependence(ThisType(), control.Character(), tok);
    else if (type == control.int_type)
         AddDependence(ThisType(), control.Integer(), tok);
    else if (type == control.long_type)
         AddDependence(ThisType(), control.Long(), tok);
    else if (type == control.float_type)
         AddDependence(ThisType(), control.Float(), tok);
    else // (type == control.double_type)
         AddDependence(ThisType(), control.Double(), tok);
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

#endif // semantic_INCLUDED

