// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, 1999, 2000, 2001 International Business
// Machines Corporation and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#include "platform.h"
#include "semantic.h"

#ifdef HAVE_JIKES_NAMESPACE
namespace Jikes { // Open namespace Jikes block
#endif

//
// NOTE: This file is used to determine definite assignment and definite
// unassignment rules, per JLS chapter 16.  Often, these are abbreviated
// da and du.  The BitSet type holds a status bit for every variable in
// scope.  Since many rules operate identically on da and du, the DefinitePair
// type is a wrapper for two BitSets.  Finally, boolean expressions can
// cause different definite assignment status during speculation, so
// the DefiniteAssignmentSet is a wrapper for two DefinitePair objects.
//
// It is a compile-time error if a local variable is accessed that is not
// da, and a compile-time error if a blank final is assigned when it is
// not du.  This code also handles the compile-time error when an assignment
// is attempted to an initialized final.
//
// There are two contexts: expression and statement.  In expression context,
// we must pass in the current assignment state and return the resultant
// state (if different) - this is to allow speculative decisions when
// evaluating loop bodies.  In statement context, rather than pass the
// information around, we store it in the instance variable
// *definitely_assigned_variables for efficiency.
//

inline DefinitePair::DefinitePair(const DefiniteAssignmentSet &set) : da_set(set.DASet()),
                                                                      du_set(set.DUSet())
{}

inline DefinitePair& DefinitePair::operator=(const DefiniteAssignmentSet& rhs)
{
    da_set = rhs.DASet();
    du_set = rhs.DUSet();
    return *this;
}


//
// There are two versions of DefiniteExpression.  Call this version if the
// expression can be boolean, but there is no need to track the when true
// and when false cases separately; or if the expression cannot be boolean.
// If the expression can be (or always is) boolean, and the when true and
// when false cases matter, call DefiniteBooleanExpression.
//
void Semantic::DefiniteExpression(AstExpression *expr, DefinitePair &pair)
{
    DefiniteAssignmentSet *definite = DefiniteBooleanExpression(expr, pair);
    if (definite)
    {
        pair = *definite;
        delete definite;
    }
}

//
// See the comments for DefiniteExpression above.  If the when true and when
// false status differ after this expression, the calling function MUST delete
// the returned object to avoid a memory leak.
//
DefiniteAssignmentSet *Semantic::DefiniteBooleanExpression(AstExpression *expr, DefinitePair &pair)
{
    DefiniteAssignmentSet *definite = NULL;

    //
    // Is the expression a constant expression of type boolean?
    // Recall that a constant expression does not contain an
    // assignment statement.
    //
    if (IsConstantTrue(expr))
        return new DefiniteAssignmentSet(pair, *universe);
    else if (IsConstantFalse(expr))
        return new DefiniteAssignmentSet(*universe, pair);
    else if (expr -> symbol != control.no_type)
        definite = (this ->* DefiniteExpr[expr -> kind])(expr, pair);

    assert(! definite || expr -> Type() == control.boolean_type);
    return definite;
}


DefiniteAssignmentSet *Semantic::DefiniteSimpleName(AstExpression *expression, DefinitePair &pair)
{
    AstSimpleName *simple_name = (AstSimpleName *) expression;

    if (simple_name -> resolution_opt)
        return DefiniteBooleanExpression(simple_name -> resolution_opt, pair);

    //
    // Some simple names are undefined. e.g., the simple name in a method call.
    //
    VariableSymbol *variable = (simple_name -> symbol ? simple_name -> symbol -> VariableCast() : (VariableSymbol *) NULL);
    if (variable && (variable -> IsLocal(ThisMethod()) || variable -> IsFinal(ThisType())))
    {
        int index = variable -> LocalVariableIndex();
        //
        // Compile time constants are always da; this matters in switch
        // blocks, where we might have bypassed the initializer.
        //
        if (! pair.da_set[index] && ! simple_name -> IsConstant())
        {
            ReportSemError(SemanticError::VARIABLE_NOT_DEFINITELY_ASSIGNED,
                           simple_name -> identifier_token,
                           simple_name -> identifier_token,
                           variable -> Name());

            if (variable -> IsLocal(ThisMethod())) // to avoid cascading errors!
                pair.da_set.AddElement(index);
        }
    }

    return (DefiniteAssignmentSet *) NULL;
}


DefiniteAssignmentSet *Semantic::DefiniteArrayAccess(AstExpression *expression, DefinitePair &pair)
{
    AstArrayAccess *array_access = (AstArrayAccess *) expression;

    DefiniteExpression(array_access -> base, pair);
    DefiniteExpression(array_access -> expression, pair);

    return (DefiniteAssignmentSet *) NULL;
}


DefiniteAssignmentSet *Semantic::DefiniteMethodInvocation(AstExpression *expression, DefinitePair &pair)
{
    AstMethodInvocation *method_call = (AstMethodInvocation *) expression;

    DefiniteExpression(method_call -> method, pair);

    for (int i = 0; i < method_call -> NumArguments(); i++)
    {
        AstExpression *expr = method_call -> Argument(i);
        DefiniteExpression(expr, pair);
    }

    return (DefiniteAssignmentSet *) NULL;
}


DefiniteAssignmentSet *Semantic::DefiniteClassInstanceCreationExpression(AstExpression *expression, DefinitePair &pair)
{
    AstClassInstanceCreationExpression *class_creation = (AstClassInstanceCreationExpression *) expression;

    if (class_creation -> base_opt)
        DefiniteExpression(class_creation -> base_opt, pair);

    for (int i = 0; i < class_creation -> NumLocalArguments(); i++)
    {
        AstExpression *expr = class_creation -> LocalArgument(i);
        DefiniteExpression(expr, pair);
    }

    for (int k = 0; k < class_creation -> NumArguments(); k++)
    {
        AstExpression *expr = class_creation -> Argument(k);
        DefiniteExpression(expr, pair);
    }

    return (DefiniteAssignmentSet *) NULL;
}


DefiniteAssignmentSet *Semantic::DefiniteArrayCreationExpression(AstExpression *expression, DefinitePair &pair)
{
    AstArrayCreationExpression *array_creation = (AstArrayCreationExpression *) expression;

    for (int i = 0; i < array_creation -> NumDimExprs(); i++)
    {
        AstDimExpr *dim_expr = array_creation -> DimExpr(i);
        DefiniteExpression(dim_expr -> expression, pair);
    }

    if (array_creation -> array_initializer_opt)
        DefiniteArrayInitializer(array_creation -> array_initializer_opt, pair);

    return (DefiniteAssignmentSet *) NULL;
}


inline VariableSymbol *Semantic::DefiniteFinal(AstFieldAccess *field_access)
{
    if (field_access -> resolution_opt)
        field_access = field_access -> resolution_opt -> FieldAccessCast();

    if (field_access)
    {
        VariableSymbol *variable = (field_access -> symbol ? field_access -> symbol -> VariableCast() : (VariableSymbol *) NULL);
        if (variable && variable -> IsFinal(ThisType()))
        {
            if (variable -> ACC_STATIC()) // there is exactly one copy of a static variable, so, it's always the right one.
                return variable;

            AstFieldAccess *sub_field_access = field_access -> base -> FieldAccessCast();
            if (field_access -> base -> ThisExpressionCast() || (sub_field_access && sub_field_access -> IsThisAccess()))
                return variable;
        }
    }

    return NULL;
}


DefiniteAssignmentSet *Semantic::DefinitePLUSPLUSOrMINUSMINUS(AstExpression *expr, DefinitePair &pair)
{
    DefiniteExpression(expr, pair);

    VariableSymbol *variable = NULL;
    if (! expr -> ArrayAccessCast()) // some kind of name
    {
        MethodSymbol *read_method = NULL;
        AstSimpleName *simple_name = expr -> SimpleNameCast();
        if (simple_name)
        {
            if (simple_name -> resolution_opt)
               read_method = simple_name -> resolution_opt -> symbol -> MethodCast();
        }
        else
        {
            AstFieldAccess *field_access = expr -> FieldAccessCast();

            assert(field_access);

            if (field_access -> resolution_opt)
                read_method = field_access -> resolution_opt -> symbol -> MethodCast();
        }

        variable = (read_method ? (VariableSymbol *) read_method -> accessed_member : expr -> symbol -> VariableCast());
    }

    //
    // If we have a variable and it is final then...
    //
    if (variable && variable -> ACC_FINAL())
    {
        ReportSemError(SemanticError::TARGET_VARIABLE_IS_FINAL,
                       expr -> LeftToken(),
                       expr -> RightToken(),
                       variable -> Name());

        if (variable -> IsFinal(ThisType())) // mark it assigned, to catch further errors
            pair.du_set.RemoveElement(variable -> LocalVariableIndex());
    }

    return (DefiniteAssignmentSet *) NULL;
}


DefiniteAssignmentSet *Semantic::DefinitePostUnaryExpression(AstExpression *expression, DefinitePair &pair)
{
    AstPostUnaryExpression *postfix_expression = (AstPostUnaryExpression *) expression;
    return DefinitePLUSPLUSOrMINUSMINUS(postfix_expression -> expression, pair);
}


DefiniteAssignmentSet *Semantic::DefiniteNOT(AstExpression *expr, DefinitePair &pair)
{
    DefiniteAssignmentSet *after_expr = DefiniteBooleanExpression(expr, pair);
    if (after_expr) // is the expression is a complex boolean expression?
    {
        DefinitePair temp(after_expr -> true_pair);
        after_expr -> true_pair = after_expr -> false_pair;
        after_expr -> false_pair = temp;
    }

    return after_expr;
}


//
// The default pre unary operators are +, -, and ~.
// As these operators are not applicable to boolean expressions,
// we do not need to invoke DefiniteExpression to process them.
//
DefiniteAssignmentSet *Semantic::DefiniteDefaultPreUnaryExpression(AstExpression *expr, DefinitePair &pair)
{
    return (this ->* DefiniteExpr[expr -> kind])(expr, pair);
}


DefiniteAssignmentSet *Semantic::DefinitePreUnaryExpression(AstExpression *expression, DefinitePair &pair)
{
    AstPreUnaryExpression *prefix_expression = (AstPreUnaryExpression *) expression;
    return (this ->* DefinitePreUnaryExpr[prefix_expression -> pre_unary_tag])(prefix_expression -> expression, pair);
}


DefiniteAssignmentSet *Semantic::DefiniteAND_AND(AstBinaryExpression *expr, DefinitePair &pair)
{
    DefiniteAssignmentSet *after_left = DefiniteBooleanExpression(expr -> left_expression, pair);
    DefinitePair *before_right = NULL;
    if (after_left)
        pair = after_left -> true_pair;
    else
        before_right = new DefinitePair(pair);

    DefiniteAssignmentSet *after_right = DefiniteBooleanExpression(expr -> right_expression, pair);

    if (after_left)
    {
        if (after_right)
        {
            after_right -> false_pair *= after_left -> false_pair;
            delete after_left;
        }
        else
        {
            after_right = after_left;
            after_right -> true_pair = pair;
            after_right -> false_pair *= pair;
        }
    }
    else
    {
        if (! after_right)
            after_right = new DefiniteAssignmentSet(pair);

        after_right -> false_pair *= *before_right;
    }

    // harmless if NULL
    delete before_right;

    return after_right;
}


DefiniteAssignmentSet *Semantic::DefiniteOR_OR(AstBinaryExpression *expr, DefinitePair &pair)
{
    DefiniteAssignmentSet *after_left = DefiniteBooleanExpression(expr -> left_expression, pair);
    DefinitePair *before_right = NULL;
    if (after_left)
        pair = after_left -> false_pair;
    else
        before_right = new DefinitePair(pair);

    DefiniteAssignmentSet *after_right = DefiniteBooleanExpression(expr -> right_expression, pair);

    if (after_left)
    {
        if (after_right)
        {
            after_right -> true_pair *= after_left -> true_pair;
            delete after_left;
        }
        else
        {
            after_right = after_left;
            after_right -> true_pair *= pair;
            after_right -> false_pair = pair;
        }
    }
    else
    {
        if (! after_right)
            after_right = new DefiniteAssignmentSet(pair);

        after_right -> true_pair *= *before_right;
    }

    // harmless if NULL
    delete before_right;

    return after_right;
}


DefiniteAssignmentSet *Semantic::DefiniteDefaultBinaryExpression(AstBinaryExpression *expr, DefinitePair &pair)
{
    DefiniteExpression(expr -> left_expression, pair);
    DefiniteExpression(expr -> right_expression, pair);

    return (DefiniteAssignmentSet *) NULL;
}


DefiniteAssignmentSet *Semantic::DefiniteBinaryExpression(AstExpression *expression, DefinitePair &pair)
{
    AstBinaryExpression *binary_expression = (AstBinaryExpression *) expression;
    return (this ->* DefiniteBinaryExpr[binary_expression -> binary_tag])(binary_expression, pair);
}


DefiniteAssignmentSet *Semantic::DefiniteConditionalExpression(AstExpression *expression, DefinitePair &pair)
{
    AstConditionalExpression *conditional_expression = (AstConditionalExpression *) expression;

    DefiniteAssignmentSet *after_condition = DefiniteBooleanExpression(conditional_expression -> test_expression, pair);
    DefinitePair *before_expressions = NULL;

    if (after_condition)
        pair = after_condition -> true_pair;
    else before_expressions = new DefinitePair(pair);
    DefiniteAssignmentSet *after_true = DefiniteBooleanExpression(conditional_expression -> true_expression, pair);
    DefinitePair *after_true_pair = (after_true ? (DefinitePair *) NULL : new DefinitePair(pair));

    if (after_condition)
         pair = after_condition -> false_pair;
    else pair = *before_expressions;
    DefiniteAssignmentSet *after_false = DefiniteBooleanExpression(conditional_expression -> false_expression, pair);

    if (conditional_expression -> Type() == control.boolean_type)
    {
        if (! after_true)
            after_true = new DefiniteAssignmentSet(*after_true_pair);

        if (after_false)
        {
            after_true -> true_pair *= after_false -> true_pair;
            after_true -> false_pair *= after_false -> false_pair;
        }
        else
        {
            after_true -> true_pair *= pair;
            after_true -> false_pair *= pair;
        }
    }
    else
    {
        assert(! after_true && ! after_false);
        pair *= *after_true_pair;
    }

    // harmless if NULL
    delete after_condition;
    delete before_expressions;
    delete after_false;

    return after_true;
}


DefiniteAssignmentSet *Semantic::DefiniteAssignmentExpression(AstExpression *expression, DefinitePair &pair)
{
    AstAssignmentExpression *assignment_expression = (AstAssignmentExpression *) expression;

    AstCastExpression *casted_left_hand_side = assignment_expression -> left_hand_side -> CastExpressionCast();
    AstExpression *left_hand_side = (casted_left_hand_side ? casted_left_hand_side -> expression : assignment_expression -> left_hand_side);
    AstSimpleName *simple_name = left_hand_side -> SimpleNameCast();
    if (simple_name)
    {
        if (simple_name -> resolution_opt)
        {
            left_hand_side = simple_name -> resolution_opt;
            simple_name = left_hand_side -> SimpleNameCast();
        }
    }
    else
    {
        AstFieldAccess *field_access = left_hand_side -> FieldAccessCast();
        if (field_access && field_access -> resolution_opt)
            left_hand_side = field_access -> resolution_opt;
    }

    VariableSymbol *variable = (left_hand_side -> symbol ? left_hand_side -> symbol -> VariableCast() : (VariableSymbol *) NULL);

    //
    // An array access is never considered to be final. Since no variable
    // is associated with the array access, the testing for the presence of
    // variable takes care of that possibility.
    //
    if (variable)
    {
        if (variable -> IsLocal(ThisMethod()) || variable -> IsFinal(ThisType()))
        {
            int index = variable -> LocalVariableIndex();

            //
            // If the debug option "g" is set and we have a simple assignment
            // statement whose left-hand side is a local variable that has not
            // yet been defined, mark this assignment as a definition
            // assignment.
            //
            if (control.option.g &&
                assignment_expression -> assignment_tag == AstAssignmentExpression::SIMPLE_EQUAL &&
                variable -> IsLocal(ThisMethod()) &&
                ! (*definite_block_stack -> TopLocallyDefinedVariables())[index])
            {
                assignment_expression -> assignment_tag = AstAssignmentExpression::DEFINITE_EQUAL;
                definite_block_stack -> TopLocallyDefinedVariables() -> AddElement(index);
                AstBlock *block = definite_block_stack -> TopBlock();
                block -> AddLocallyDefinedVariable(variable);
#ifdef DUMP
Coutput << "(1) Variable \"" << variable -> Name() << " #" << index
        << "\" is defined at line " << lex_stream -> Line(assignment_expression -> LeftToken())
        << endl;
#endif
            }

            //
            // If we have a compound assignment then the variable must have
            // been set prior to such an assignment. otherwise, an error
            // occurs.
            //
            if (! (assignment_expression -> SimpleAssignment() || pair.da_set[index]))
            {
                ReportSemError(SemanticError::VARIABLE_NOT_DEFINITELY_ASSIGNED,
                               assignment_expression -> left_hand_side -> LeftToken(),
                               assignment_expression -> left_hand_side -> RightToken(),
                               variable -> Name());
            }
        }
        else if (variable -> ACC_FINAL()) // attempt to assign a value to a final field member!
        {
            ReportSemError(SemanticError::TARGET_VARIABLE_IS_FINAL,
                           assignment_expression -> left_hand_side -> LeftToken(),
                           assignment_expression -> left_hand_side -> RightToken(),
                           variable -> Name());
        }
    }

    //
    // The left-hand-side of an assignment expression is either a simple name,
    // a field access or an array access.  A simple name does not need further
    // action, a field access needs to descend into all qualifying expressions,
    // and an array access needs to descend into the entire expression. 
    //
    if (! simple_name)
    {
        AstFieldAccess *field_access = left_hand_side -> FieldAccessCast();
        DefiniteExpression((field_access ? field_access -> base : left_hand_side), pair);
    }

    //
    // JLS2 16.1.7 - The rules for definite assignment of boolean valued
    // assignments are stricter than they were in JLS1; hence we no longer
    // consider the when true and when false values separately.
    //
    DefiniteExpression(assignment_expression -> expression, pair);

    //
    // Finally, we mark the variable as assigned.
    //
    if (variable && (variable -> IsLocal(ThisMethod()) || variable -> IsFinal(ThisType())))
    {
        if (variable -> ACC_FINAL())
        {
            //
            // It is an error to assign any final except a DU blank final
            //
            if (! ((*blank_finals)[variable -> LocalVariableIndex()] &&
                   pair.du_set[variable -> LocalVariableIndex()]))
            {
                ReportSemError(SemanticError::TARGET_VARIABLE_IS_FINAL,
                               assignment_expression -> left_hand_side -> LeftToken(),
                               assignment_expression -> left_hand_side -> RightToken(),
                               variable -> Name());
            }
            else if (definite_final_assignment_stack -> Size() > 0) // are we processing the body of a loop ?
                definite_final_assignment_stack -> Top().Next() = left_hand_side;
        }

        pair.AssignElement(variable -> LocalVariableIndex());
    }

    return (DefiniteAssignmentSet *) NULL;
}

DefiniteAssignmentSet *Semantic::DefiniteDefaultExpression(AstExpression *expr, DefinitePair &pair)
{
    return (DefiniteAssignmentSet *) NULL;
}

DefiniteAssignmentSet *Semantic::DefiniteParenthesizedExpression(AstExpression *expression, DefinitePair &pair)
{
    AstParenthesizedExpression *expr = (AstParenthesizedExpression *) expression;

    return DefiniteBooleanExpression(expr -> expression, pair);
}

DefiniteAssignmentSet *Semantic::DefiniteFieldAccess(AstExpression *expression, DefinitePair &pair)
{
    AstFieldAccess *expr = (AstFieldAccess *) expression;

    //
    // TODO: Sun bug 4395322 mentions that DA is underspecified for field
    // references.  In other words, it is legal to read an uninitialized
    // static value through Classname.fieldname, or an instance value
    // through this.fieldname. If Sun specifies that this behavior is correct,
    // this commented code can be removed.
    //
    //      VariableSymbol *variable = DefiniteFinal(expr);
    //      if (variable)
    //      {
    //          if (! pair.da_set[variable -> LocalVariableIndex()])
    //          {
    //              ReportSemError(SemanticError::VARIABLE_NOT_DEFINITELY_ASSIGNED,
    //                             expr -> LeftToken(),
    //                             expr -> RightToken(),
    //                             variable -> Name());
    //              // supress further warnings
    //              pair.da_set.AddElement(variable -> LocalVariableIndex());
    //          }
    //      }

    return DefiniteBooleanExpression((expr -> resolution_opt ? expr -> resolution_opt : expr -> base), pair);
}

DefiniteAssignmentSet *Semantic::DefiniteCastExpression(AstExpression *expression, DefinitePair &pair)
{
    AstCastExpression *expr = (AstCastExpression *) expression;

    return DefiniteBooleanExpression(expr -> expression, pair);
}


//
// Must have two versions, since this can be called in both expression and
// statement context.
//
void Semantic::DefiniteArrayInitializer(AstArrayInitializer *array_initializer, DefinitePair &pair)
{
    for (int i = 0; i < array_initializer -> NumVariableInitializers(); i++)
    {
        AstArrayInitializer *sub_array_initializer = array_initializer -> VariableInitializer(i) -> ArrayInitializerCast();

        if (sub_array_initializer)
            DefiniteArrayInitializer(sub_array_initializer, pair);
        else
        {
            AstExpression *init = (AstExpression *) array_initializer -> VariableInitializer(i);
            DefiniteExpression(init, pair);
        }
    }

    return;
}

inline void Semantic::DefiniteArrayInitializer(AstArrayInitializer *array_initializer)
{
    DefiniteArrayInitializer(array_initializer, *definitely_assigned_variables);
}


inline void Semantic::DefiniteVariableInitializer(AstVariableDeclarator *variable_declarator)
{
    AstExpression *init = variable_declarator -> variable_initializer_opt -> ExpressionCast();
    if (! init)
        DefiniteArrayInitializer((AstArrayInitializer *) variable_declarator -> variable_initializer_opt, *definitely_assigned_variables);
    else
        DefiniteExpression(init, *definitely_assigned_variables);

    //
    // Even when initialized by a non-constant, variables declared in a
    // switch are necessarily blank finals.
    //
    if (definite_block_stack &&
        definite_block_stack -> TopBlock() -> block_tag == AstBlock::SWITCH &&
        ((! init) || (! init -> IsConstant())))
    {
        blank_finals -> AddElement(variable_declarator -> symbol -> LocalVariableIndex());
    }

    return;
}


inline void Semantic::DefiniteStatement(Ast *ast)
{
    (this ->* DefiniteStmt[ast -> kind])(ast);

    return;
}

inline void Semantic::DefiniteBlockStatements(AstBlock *block_body)
{
    if (control.option.g && block_body -> NumStatements() > 0)
    {
        AstStatement *statement = (AstStatement *) block_body -> Statement(0);
        DefiniteStatement(statement);
        for (int i = 1; i < block_body -> NumStatements(); i++)
        {
            statement = (AstStatement *) block_body -> Statement(i);
            //
            // As unreachable statements already cause an error, we avoid
            // them here
            //
            if (statement -> is_reachable)
            {
                //
                // All variables that were assigned a value in the previous
                // statement must be defined
                BitSet &locally_defined_variables = *definite_block_stack -> TopLocallyDefinedVariables();
                for (int k = 0; k < definitely_assigned_variables -> Size(); k++)
                {
                    VariableSymbol *variable = definite_block_stack -> TopLocalVariables()[k];
                    if (variable) // a variable that is visible in this block? (i.e. not one declare in a non-enclosing block)
                    {
                        if ((*definitely_assigned_variables).da_set[k] && (! locally_defined_variables[k]))
                        {
#ifdef DUMP
Coutput << "(2) Variable \"" << variable -> Name() << " #" << variable -> LocalVariableIndex()
        << "\" is defined at line " << lex_stream -> Line(statement -> LeftToken())
        << endl;
#endif
                            statement -> AddDefinedVariable(variable);
                            locally_defined_variables.AddElement(k);
                            block_body -> AddLocallyDefinedVariable(variable);
                        }
                    }
                }

                DefiniteStatement(statement);
            }
            else break;
        }
    }
    else
    {
        for (int i = 0; i < block_body -> NumStatements(); i++)
        {
            AstStatement *statement = (AstStatement *) block_body -> Statement(i);
            //
            // As unreachable statements already cause an error, we avoid
            // them here
            //
            if (statement -> is_reachable)
                 DefiniteStatement(statement);
            else break;
        }
    }

    return;
}


void Semantic::DefiniteBlock(Ast *stmt)
{
    AstBlock *block_body = (AstBlock *) stmt;

    definite_block_stack -> Push(block_body);

    for (int i = 0; i < block_body -> block_symbol -> NumVariableSymbols(); i++)
        definite_visible_variables -> AddElement(block_body -> block_symbol -> VariableSym(i));

    DefiniteBlockStatements(block_body);

#ifdef DUMP
if (control.option.g && block_body -> NumLocallyDefinedVariables() > 0)
{
Coutput << "(3) At Line " << lex_stream -> Line(block_body -> RightToken())
        << " the range for the following variables end:" << endl << endl;
for (int j = 0; j < block_body -> NumLocallyDefinedVariables(); j++)
Coutput << "    \"" << block_body -> LocallyDefinedVariable(j) -> Name()
        << "\"" << endl;
}
#endif
    //
    // Remove all variables that just went out of scope
    //
    for (int k = 0; k < block_body -> block_symbol -> NumVariableSymbols(); k++)
    {
        VariableSymbol *variable = block_body -> block_symbol -> VariableSym(k);

        blank_finals -> RemoveElement(variable -> LocalVariableIndex());
        definitely_assigned_variables -> ReclaimElement(variable -> LocalVariableIndex());

        definite_visible_variables -> RemoveElement(variable);
    }

    //
    // Note that in constructing the Ast, the parser encloses each
    // labeled statement in its own block... Therefore, only blocks
    // are labeled.
    //
    if (block_body -> NumLabels() > 0)
        *definitely_assigned_variables *= definite_block_stack -> TopBreakPair();

    definite_block_stack -> Pop();

    return;
}


void Semantic::DefiniteLocalVariableDeclarationStatement(Ast *stmt)
{
    AstLocalVariableDeclarationStatement *local_decl = (AstLocalVariableDeclarationStatement *) stmt;

    for (int i = 0; i < local_decl -> NumVariableDeclarators(); i++)
    {
        AstVariableDeclarator *variable_declarator = local_decl -> VariableDeclarator(i);
        VariableSymbol *variable_symbol = variable_declarator -> symbol;
        if (variable_symbol)
        {
            if (control.option.g)
            {
                assert(definite_block_stack -> TopLocalVariables()[variable_symbol -> LocalVariableIndex()] == NULL);
#ifdef DUMP
Coutput << "(3.5) Local Variable \"" << variable_symbol -> Name() << " #" << variable_symbol -> LocalVariableIndex()
        << "\" is declared at line " << lex_stream -> Line(variable_declarator -> LeftToken())
        << endl;
#endif
                definite_block_stack -> TopLocalVariables()[variable_symbol -> LocalVariableIndex()] = variable_symbol;
            }

            if (variable_declarator -> variable_initializer_opt)
            {
                DefiniteVariableInitializer(variable_declarator);
                definitely_assigned_variables -> AssignElement(variable_symbol -> LocalVariableIndex());

                if (control.option.g)
                {
                    definite_block_stack -> TopLocallyDefinedVariables() -> AddElement(variable_symbol -> LocalVariableIndex());
                    AstBlock *block = definite_block_stack -> TopBlock();
                    block -> AddLocallyDefinedVariable(variable_symbol);
#ifdef DUMP
Coutput << "(4) Variable \"" << variable_symbol -> Name() << " #" << variable_symbol -> LocalVariableIndex()
        << "\" is defined at line " << lex_stream -> Line(variable_declarator -> LeftToken())
        << endl;
#endif
                }
            }
            else
            {
                definitely_assigned_variables -> ReclaimElement(variable_symbol -> LocalVariableIndex());
                if (variable_symbol -> ACC_FINAL())
                    blank_finals -> AddElement(variable_symbol -> LocalVariableIndex());
            }
        }
    }

    return;
}


void Semantic::DefiniteExpressionStatement(Ast *stmt)
{
    AstExpressionStatement *expression_statement = (AstExpressionStatement *) stmt;

    DefiniteExpression(expression_statement -> expression, *definitely_assigned_variables);

   return;
}


void Semantic::DefiniteSynchronizedStatement(Ast *stmt)
{
    AstSynchronizedStatement *synchronized_statement = (AstSynchronizedStatement *) stmt;

    DefiniteExpression(synchronized_statement -> expression, *definitely_assigned_variables);

    DefiniteBlock(synchronized_statement -> block);

    return;
}


void Semantic::DefiniteIfStatement(Ast *stmt)
{
    AstIfStatement *if_statement = (AstIfStatement *) stmt;

    DefiniteAssignmentSet *after_expr = DefiniteBooleanExpression(if_statement -> expression, *definitely_assigned_variables);
    DefinitePair *starting_pair = new DefinitePair(*definitely_assigned_variables);
    if (after_expr)
        *definitely_assigned_variables = after_expr -> true_pair;

    //
    // Recall that the parser ensures that the statements that appear in an
    // if-statement (both the true and false statement) are enclosed in a
    // block.
    //
    DefiniteBlock(if_statement -> true_statement);

    if (! if_statement -> false_statement_opt) // no else part ?
    {
        *definitely_assigned_variables *= (after_expr ? after_expr -> false_pair : *starting_pair);
    }
    else
    {
        DefinitePair true_set(*definitely_assigned_variables);
        *definitely_assigned_variables = (after_expr ? after_expr -> false_pair : *starting_pair);

        DefiniteBlock(if_statement -> false_statement_opt);

        *definitely_assigned_variables *= true_set;
    }

    // harmless if NULL
    delete starting_pair;

    return;
}


void Semantic::DefiniteLoopBody(AstStatement *statement)
{
    definite_final_assignment_stack -> Push();

    BitSet starting_set(definitely_assigned_variables -> du_set);

    DefiniteStatement(statement);

    BitSet exit_set(definitely_assigned_variables -> du_set);
    exit_set *= definite_block_stack -> TopContinuePair().du_set;

    //
    // Find the set of variables that were DU before the loop, but not DU
    // before a continue or at the loop end.
    //
    BitSet new_set(starting_set);
    new_set -= exit_set;

    for (int k = 0; k < definite_final_assignment_stack -> Top().Length(); k++)
    {
        AstExpression *name = definite_final_assignment_stack -> Top()[k];
        VariableSymbol *variable = (VariableSymbol *) name -> symbol;

        if (definite_visible_variables -> IsElement(variable) && new_set[variable -> LocalVariableIndex()])
        {
            ReportSemError(SemanticError::FINAL_VARIABLE_TARGET_IN_LOOP,
                           name -> LeftToken(),
                           name -> RightToken(),
                           variable -> Name());
        }
    }

    definitely_assigned_variables -> du_set = exit_set * definite_block_stack -> TopBreakPair().du_set;
    definite_final_assignment_stack -> Pop();

    return;
}


void Semantic::DefiniteWhileStatement(Ast *stmt)
{
    AstWhileStatement *while_statement = (AstWhileStatement *) stmt;

    BreakableStatementStack().Push(definite_block_stack -> TopBlock());
    ContinuableStatementStack().Push(stmt);

    DefiniteAssignmentSet *after_expr = DefiniteBooleanExpression(while_statement -> expression, *definitely_assigned_variables);

    if (after_expr)
    {
        *definitely_assigned_variables = after_expr -> true_pair;
        //
        // We have already given a warning if the statement is unreachable
        //
        if (while_statement -> statement -> is_reachable)
            DefiniteLoopBody(while_statement -> statement);
        *definitely_assigned_variables = after_expr -> false_pair * definite_block_stack -> TopBreakPair();
        delete after_expr;
    }
    else
    {
        DefinitePair starting_pair(*definitely_assigned_variables);
        //
        // We have already given a warning if the statement is unreachable
        //
        if (while_statement -> statement -> is_reachable)
            DefiniteLoopBody(while_statement -> statement);
        *definitely_assigned_variables = starting_pair * definite_block_stack -> TopBreakPair();
    }

    ContinuableStatementStack().Pop();
    BreakableStatementStack().Pop();

    return;
}


void Semantic::DefiniteForStatement(Ast *stmt)
{
    AstForStatement *for_statement = (AstForStatement *) stmt;

    //
    // Note that in constructing the Ast, the parser encloses each
    // for-statement whose for-init-statements starts with a local
    // variable declaration in its own block. Therefore a redeclaration
    // of another local variable with the same name in a different loop
    // at the same nesting level will not cause any conflict.
    //
    // For example, the following sequence of statements is legal:
    //
    //     for (int i = 0; i < 10; i++);
    //     for (int i = 10; i < 20; i++);
    //
    if (control.option.g && for_statement -> NumForInitStatements() > 0)
    {
        AstStatement *statement = (AstStatement *) for_statement -> ForInitStatement(0);
        DefiniteStatement(statement);
        for (int i = 1; i < for_statement -> NumForInitStatements(); i++)
        {
            statement = (AstStatement *) for_statement -> ForInitStatement(i);

            //
            // All variables that were assigned a value in the previous
            // statement must be defined
            //
            BitSet &locally_defined_variables = *definite_block_stack -> TopLocallyDefinedVariables();
            for (int k = 0; k < definitely_assigned_variables -> Size(); k++)
            {
                VariableSymbol *variable = definite_block_stack -> TopLocalVariables()[k];
                if (variable) // a variable that is visible in this block? (i.e. not one declare in a non-enclosing block)
                {
                    if ((definitely_assigned_variables -> da_set)[k] && (! locally_defined_variables[k]))
                    {
#ifdef DUMP
Coutput << "(5) Variable \"" << variable -> Name() << " #" << variable -> LocalVariableIndex()
        << "\" is defined at line " << lex_stream -> Line(statement -> LeftToken())
        << endl;
#endif
                        statement -> AddDefinedVariable(variable);
                        locally_defined_variables.AddElement(k);
                        AstBlock *block = definite_block_stack -> TopBlock();
                        block -> AddLocallyDefinedVariable(variable);
                    }
                }
            }

            DefiniteStatement(statement);
        }
    }
    else
    {
        for (int i = 0; i < for_statement -> NumForInitStatements(); i++)
        {
            DefiniteStatement(for_statement -> ForInitStatement(i));
        }
    }

    BreakableStatementStack().Push(definite_block_stack -> TopBlock());
    ContinuableStatementStack().Push(stmt);

    DefiniteAssignmentSet *after_end_expression = NULL;
    DefinitePair before_statement(universe -> Size());

    if (for_statement -> end_expression_opt)
        after_end_expression = DefiniteBooleanExpression(for_statement -> end_expression_opt, *definitely_assigned_variables);

    if (after_end_expression)
         *definitely_assigned_variables = after_end_expression -> true_pair;
    else before_statement = *definitely_assigned_variables;

    //
    // We have already given a warning if the statement is unreachable
    //
    if (for_statement -> statement -> is_reachable)
        DefiniteLoopBody(for_statement -> statement);

    //
    // Compute the set of variables that are definitely assigned after the
    // contained statement and after every continue statement that may exit
    // the body of the for statement.
    //
    *definitely_assigned_variables *= definite_block_stack -> TopContinuePair();
    for (int j = 0; j < for_statement -> NumForUpdateStatements(); j++)
        DefiniteExpressionStatement(for_statement -> ForUpdateStatement(j));

    //
    // Compute the set of variables that belongs to both sets below:
    //
    //    . the universe if no condition expression is present;
    //      otherwise, the set of variables that are DA when
    //      the condition expression is false.
    //
    //    . the set of variables that are DA before every
    //      break statement that may exit the for statement.
    //
    *definitely_assigned_variables = (for_statement -> end_expression_opt
                                                     ? (after_end_expression ? after_end_expression -> false_pair : before_statement)
                                                     : *universe); // set of variables that depend on the condition

    //
    // The replacement
    //
    *definitely_assigned_variables *= definite_block_stack -> TopBreakPair();

    delete after_end_expression; // nothing happens if after_end_expression is NULL

    ContinuableStatementStack().Pop();
    BreakableStatementStack().Pop();

    return;
}


void Semantic::DefiniteDoStatement(Ast *stmt)
{
    AstDoStatement *do_statement = (AstDoStatement *) stmt;

    BreakableStatementStack().Push(definite_block_stack -> TopBlock());
    ContinuableStatementStack().Push(stmt);

    if (! IsConstantFalse(do_statement -> expression))
         DefiniteLoopBody(do_statement -> statement);
    else DefiniteStatement(do_statement -> statement); // The expression is always false therefore, loop will execute exactly once.

    DefinitePair after_stmt(*definitely_assigned_variables);
    *definitely_assigned_variables *= definite_block_stack -> TopContinuePair();
    DefiniteAssignmentSet *after_expr = DefiniteBooleanExpression(do_statement -> expression, *definitely_assigned_variables);
    if (after_expr)
    {
        *definitely_assigned_variables = after_expr -> false_pair * definite_block_stack -> TopBreakPair();
        delete after_expr;
    }
    else *definitely_assigned_variables *= definite_block_stack -> TopBreakPair();

    ContinuableStatementStack().Pop();
    BreakableStatementStack().Pop();

    return;
}


void Semantic::DefiniteSwitchStatement(Ast *stmt)
{
    AstSwitchStatement *switch_statement = (AstSwitchStatement *) stmt;

    AstBlock *block_body = switch_statement -> switch_block;
    definite_block_stack -> Push(block_body);
    BreakableStatementStack().Push(block_body);

    DefiniteExpression(switch_statement -> expression, *definitely_assigned_variables);

    DefinitePair starting_pair(*definitely_assigned_variables);

    //
    // Recall that the parser inserts an empty statement if necessary after
    // the last label, so that all SwitchBlockStatementGroups have statements
    //
    int i;
    for (i = 0; i < block_body -> NumStatements(); i++)
    {
        AstSwitchBlockStatement *switch_block_statement = (AstSwitchBlockStatement *) block_body -> Statement(i);

        *definitely_assigned_variables *= starting_pair;

        if (control.option.g && switch_block_statement -> NumStatements() > 0)
        {
            BitSet &locally_defined_variables = *definite_block_stack -> TopLocallyDefinedVariables();
            AstStatement *statement = (AstStatement *) switch_block_statement -> Statement(0);
            DefiniteStatement(statement);
            for (int j = 1; j < switch_block_statement -> NumStatements(); j++)
            {
                statement = (AstStatement *) switch_block_statement -> Statement(j);
                //
                // As unreachable statements already cause an error, we avoid
                // them here
                //
                if (statement -> is_reachable)
                {
                    //
                    // All variables that were assigned a value in the previous
                    // statement must be defined
                    //
                    for (int k = 0; k < definitely_assigned_variables -> Size(); k++)
                    {
                        VariableSymbol *variable = definite_block_stack -> TopLocalVariables()[k];
                        if (variable) // a variable that is visible in this block? (i.e. not one declare in a non-enclosing block)
                        {
                            if (definitely_assigned_variables -> da_set[k] && (! locally_defined_variables[k]))
                            {
#ifdef DUMP
Coutput << "Variable \"" << variable -> Name() << " #" << variable -> LocalVariableIndex()
        << "\" is defined at line " << lex_stream -> Line(statement -> LeftToken())
        << endl;
#endif
                                statement -> AddDefinedVariable(variable);
                                locally_defined_variables.AddElement(k);
                                block_body -> AddLocallyDefinedVariable(variable);
                            }
                        }
                    }

                    DefiniteStatement(statement);
                }
                else break;
            }

#ifdef DUMP
if (control.option.g && block_body -> NumLocallyDefinedVariables() > 0)
{
Coutput << "(5.5) At Line " << lex_stream -> Line(statement -> RightToken())
        << " the range for the following variables end:" << endl << endl;
for (int j = 0; j < block_body -> NumLocallyDefinedVariables(); j++)
Coutput << "    \"" << block_body -> LocallyDefinedVariable(j) -> Name()
        << "\"" << endl;
}
#endif
            //
            // At the end of a switch block statement, we always close the
            // range of all local variables that have been defined. That's
            // because even if this switch block statement extends into the
            // next block, it is not the only entry point into that block.
            // Therefore, in order for a variable to be definitely defined at
            // the starting point of a switch block statement, it must have
            // been defined prior to the switch statement.
            // Notice that this clears out compile-time constants as well,
            // which are always DA, so we have a special case in
            // DefiniteSimpleName to get around that.
            //
            for (int k = 0; k < block_body -> NumLocallyDefinedVariables(); k++)
                locally_defined_variables.RemoveElement(block_body -> LocallyDefinedVariable(k) -> LocalVariableIndex());
            block_body -> TransferLocallyDefinedVariablesTo(switch_block_statement);
        }
        else
        {
            for (int j = 0; j < switch_block_statement -> NumStatements(); j++)
            {
                AstStatement *statement = (AstStatement *) switch_block_statement -> Statement(j);
                //
                // As unreachable statements already cause an error, we avoid
                // them here
                //
                if (statement -> is_reachable)
                     DefiniteStatement(statement);
                else break;
            }
        }
    }

    //
    // TODO: What if the switch enumerates all 256 byte cases without a
    // default label?
    //
    if (switch_statement -> default_case.switch_block_statement) // Is there a default case?
        *definitely_assigned_variables *= definite_block_stack -> TopBreakPair();
    else *definitely_assigned_variables = starting_pair;

    //
    // Remove all variables that just went out of scope
    //
    for (i = 0; i < block_body -> block_symbol -> NumVariableSymbols(); i++)
    {
        VariableSymbol *variable = block_body -> block_symbol -> VariableSym(i);

        blank_finals -> RemoveElement(variable -> LocalVariableIndex());
        definitely_assigned_variables -> ReclaimElement(variable -> LocalVariableIndex());

        definite_visible_variables -> RemoveElement(variable);
    }

    BreakableStatementStack().Pop();
    definite_block_stack -> Pop();

    return;
}


void Semantic::DefiniteBreakStatement(Ast *stmt)
{
    AstBreakStatement *break_statement = (AstBreakStatement *) stmt;

    //
    // Compute the set of variables that are definitely assigned prior to
    // executing the break, including if the break occurs in a try or catch
    // block.
    //
    definite_block_stack -> BreakPair(break_statement -> nesting_level) *= *definitely_assigned_variables;
    if (definite_try_stack -> Size() > 0)
    {
        for (int i = definite_try_stack -> Size() - 1; i >= 0; i--)
        {
            AstTryStatement *try_statement = definite_try_stack -> TryStatement(i);

            if (try_statement -> block == definite_try_stack -> Block(i)) // In the main try block?
            {
                int k;
                for (k = definite_block_stack -> Size() - 1;
                     definite_block_stack -> Block(k) != definite_try_stack -> Block(i);
                     k--)
                    ;

                assert(k >= 0);

                definite_block_stack -> BreakPair(k) *= *definitely_assigned_variables;
                break;
            }
            else if (try_statement -> finally_clause_opt)
            {
                int k;
                for (k = definite_block_stack -> Size() - 1;
                     definite_block_stack -> Block(k) != definite_try_stack -> Block(i);
                     k--)
                    ;

                assert(k >= 0);

                definite_block_stack -> BreakPair(k) *= *definitely_assigned_variables;
                break;
            }
        }
    }

    //
    // After execution of a break statement, it is vacuously true
    // that every variable has definitely been assigned and no final
    // variable has been possibly assigned (as nothing is reachable
    // any way).
    //
    *definitely_assigned_variables = *universe;

    return;
}


void Semantic::DefiniteContinueStatement(Ast *stmt)
{
    AstContinueStatement *continue_statement = (AstContinueStatement *) stmt;

    //
    // Compute the set of variables that are definitely assigned prior to
    // executing the continue, including if the continue occurs in a try or
    // catch block.
    //
    definite_block_stack -> ContinuePair(continue_statement -> nesting_level) *= *definitely_assigned_variables;
    if (definite_try_stack -> Size() > 0)
    {
        for (int i = definite_try_stack -> Size() - 1; i >= 0; i--)
        {
            AstTryStatement *try_statement = definite_try_stack -> TryStatement(i);

            if (try_statement -> block == definite_try_stack -> Block(i)) // In the main try block?
            {
                int k;
                for (k = definite_block_stack -> Size() - 1;
                     definite_block_stack -> Block(k) != definite_try_stack -> Block(i);
                     k--)
                    ;

                assert(k >= 0);

                definite_block_stack -> ContinuePair(k) *= *definitely_assigned_variables;
                break;
            }
            else if (try_statement -> finally_clause_opt)
            {
                int k;
                for (k = definite_block_stack -> Size() - 1;
                     definite_block_stack -> Block(k) != definite_try_stack -> Block(i);
                     k--)
                    ;

                assert(k >= 0);

                definite_block_stack -> ContinuePair(k) *= *definitely_assigned_variables;
                break;
            }
        }
    }

    //
    // After execution of a continue statement, it is vacuously true
    // that every variable has definitely been assigned and no final
    // variable has been possibly assigned (as nothing is reachable
    // any way).
    //
    *definitely_assigned_variables = *universe;

    return;
}


void Semantic::DefiniteReturnStatement(Ast *stmt)
{
    AstReturnStatement *return_statement = (AstReturnStatement *) stmt;

    if (return_statement -> expression_opt)
        DefiniteExpression(return_statement -> expression_opt, *definitely_assigned_variables);

    //
    // Compute the set of variables that are definitely assigned prior to
    // executing this return statement. Note that this set is only relevant
    // to the method or constructor block containing this statement.
    //
    // TODO: Do we really need this?
    //
    //    definite_block_stack -> TopReturnSet() *= (*definitely_assigned_variables);
    //

    //
    // Compute the set of variables that are possibly assigned prior to
    // executing this return statement.
    // We have a few cases to consider:
    //
    //    1. The return statement is not contained in a try statement - the
    //       possibly-assigned set is only relevant to the enclosing method
    //       (or constructor) block. The definitely assigned set is updated
    //       as if the return statement was a break statement out of the
    //       method (or constructor) block.
    //
    //    2. If the return statement is contained in a try main block or a try
    //       catch block that contains a finally clause - the possibly-assigned
    //       block is relevant to that main try block or catch block.
    //
    //    3. otherwise, treat the return statement as if it immediately
    //       followed its containing try statement
    //
    if (definite_try_stack -> Size() == 0)
        definite_block_stack -> ReturnPair(0) *= *definitely_assigned_variables;
    else
    {
        for (int i = definite_try_stack -> Size() - 1; i >= 0; i--)
        {
            AstTryStatement *try_statement = definite_try_stack -> TryStatement(i);
            //
            // Is the return statement enclosed in a try main block or catch
            // block that  contains a finally clause. Note that a try
            // statement is removed from the definite_try_stack before its
            // finally clause is processed. thus, a return statement that is
            // enclosed in a finally clause will appear in an enclosing
            // try statement, if any...
            //
            if (try_statement -> block == definite_try_stack -> Block(i)) // In the main try block?
            {
                int k;
                for (k = definite_block_stack -> Size() - 1;
                     definite_block_stack -> Block(k) != definite_try_stack -> Block(i);
                     k--)
                    ;

                assert(k >= 0);

                definite_block_stack -> ReturnPair(k) *= *definitely_assigned_variables;
                break;
            }
            else if (try_statement -> finally_clause_opt)
            {
                int k;
                for (k = definite_block_stack -> Size() - 1;
                     definite_block_stack -> Block(k) != definite_try_stack -> Block(i);
                     k--)
                    ;

                assert(k >= 0);

                definite_block_stack -> ReturnPair(k) *= *definitely_assigned_variables;
                break;
            }
        }
    }

    //
    // After execution of a return statement, it is vacuously true
    // that every variable has definitely been assigned and no final
    // variable has been possibly assigned (as nothing is reachable
    // any way).
    //
    *definitely_assigned_variables = *universe;

    return;
}


void Semantic::DefiniteThrowStatement(Ast *stmt)
{
    AstThrowStatement *throw_statement = (AstThrowStatement *) stmt;

    DefiniteExpression(throw_statement -> expression, *definitely_assigned_variables);

    //
    // Compute the set of variables that are definitely assigned prior to
    // executing this throw statement. Note that this set is only relevant to
    // the method or constructor block containing this statement.
    //
    // TODO: Do we really need this?
    //
    //    definite_block_stack -> TopThrowSet() *= (*definitely_assigned_variables);
    //

    //
    // Compute the set of variables that are possibly assigned prior to
    // executing this throw statement and update the proper enclosing block
    // appropriately.
    //
    // We have a few cases to consider:
    //
    //    1. The throw statement is not contained in a try statement - the
    //       possibly-assigned set is only relevant to the enclosing method
    //       (or constructor) block. If the containing function in question
    //       is a method (i.e., not a constructor) then the definitely
    //       assigned set is updated as if the throw statement was a break
    //       statement out of the method block.
    //
    //    2. The throw statement is enclosed in a try statement main block or
    //       catch clause.
    //
    //        2a. if the nearest try-block that encloses the throw statement
    //            is a main try-block - the possibly-assigned block is
    //            relevant to that main block.
    //        2b. if the nearest try-block that encloses the throw statement
    //            is a catch-block and the try block contains a finally
    //            clause - the possibly-assigned block is relevant to the
    //            catch-block
    //        2c. otherwise, treat the throw statement as if it immediately
    //            followed its containing try statement
    //
    if (definite_try_stack -> Size() == 0)
    {
        if (ThisMethod() -> Identity() != control.init_name_symbol) // Not a constructor
            definite_block_stack -> ThrowPair(0) *= *definitely_assigned_variables;
    }
    else
    {
        for (int i = definite_try_stack -> Size() - 1; i >= 0; i--)
        {
            AstTryStatement *try_statement = definite_try_stack -> TryStatement(i);
            //
            // Is the return statement enclosed in a try main block or catch
            // block that contains a finally clause. Note that a try statement
            // is removed from the definite_try_stack before its finally
            // clause is processed. thus, a return statement that is enclosed
            // in a finally clause will appear in an enclosing try statement,
            // if any...
            //
            if (try_statement -> block == definite_try_stack -> Block(i)) // Is the throw statement enclosed in main try block?
            {
                int k;
                for (k = definite_block_stack -> Size() - 1; definite_block_stack -> Block(k) != try_statement -> block; k--)
                    ;

                assert(k >= 0);

                definite_block_stack -> ThrowPair(k) *= *definitely_assigned_variables;
                break;
            }
            else if (try_statement -> finally_clause_opt)
            {
                int k;
                for (k = definite_block_stack -> Size() - 1;
                     definite_block_stack -> Block(k) != definite_try_stack -> Block(i);
                     k--)
                    ;

                assert(k >= 0);

                definite_block_stack -> ThrowPair(k) *= *definitely_assigned_variables;
                break;
            }
        }
    }

    //
    // After execution of a throw statement, it is vacuously true
    // that every variable has definitely been assigned and no final
    // variable has been possibly assigned (as nothing is reachable
    // any way).
    //
    *definitely_assigned_variables = *universe;

    return;
}


void Semantic::DefiniteTryStatement(Ast *stmt)
{
    AstTryStatement *try_statement = (AstTryStatement *) stmt;
    definite_try_stack -> Push(try_statement);

    DefinitePair starting_pair(*definitely_assigned_variables);

    AstBlock *try_block_body = try_statement -> block;
    definite_block_stack -> Push(try_block_body);
    definite_try_stack -> SetTopBlock(try_block_body);

    for (int j = 0; j < try_block_body -> block_symbol -> NumVariableSymbols(); j++)
        definite_visible_variables -> AddElement(try_block_body -> block_symbol -> VariableSym(j));

    BitSet before_try_finals(definitely_assigned_variables -> du_set);

    DefiniteBlockStatements(try_block_body);

#ifdef DUMP
if (control.option.g && try_block_body -> NumLocallyDefinedVariables() > 0)
{
Coutput << "(6) At Line " << lex_stream -> Line(try_block_body -> RightToken())
        << " the range for the following variables end:" << endl << endl;
for (int k = 0; k < try_block_body -> NumLocallyDefinedVariables(); k++)
Coutput << "    \"" << try_block_body -> LocallyDefinedVariable(k) -> Name()
        << "\"" << endl;
}
#endif
    DefinitePair &exit_pair = definite_block_stack -> TopExitPair(*definitely_assigned_variables);
    BitSet before_catch_finals(exit_pair.du_set),
           possibly_finals_union(exit_pair.du_set);

    //
    // Once we are done with a block, its enclosed local variables are no
    // longer visible.
    //
    for (int l = 0; l < try_block_body -> block_symbol -> NumVariableSymbols(); l++)
    {
        VariableSymbol *variable = try_block_body -> block_symbol -> VariableSym(l);
        int index = variable -> LocalVariableIndex();

        blank_finals -> RemoveElement(index);
        definitely_assigned_variables -> ReclaimElement(index);
        before_catch_finals.RemoveElement(index);
        possibly_finals_union.RemoveElement(index);
        definite_visible_variables -> RemoveElement(variable);
    }

    definite_block_stack -> Pop();

    //
    // We initilize the variable after_blocks here. It is used to calculate
    // intersection of the set of variables that are definitely assigned by
    // all the blocks: the try block, all the catch blocks, if any, and the
    // finally block, if there is one.
    //
    BitSet after_blocks(definitely_assigned_variables -> da_set);

    //
    // Recall that the body of the catch blocks must not be
    // processed within the environment of the associated try whose
    // exceptions they are supposed to catch but within the immediate
    // enclosing block (which may itself be a try block).
    //
    for (int i = 0; i < try_statement -> NumCatchClauses(); i++)
    {
        definitely_assigned_variables -> da_set = starting_pair.da_set;

        //
        // We process the catch block here instead of invoking DefiniteBlock,
        // in order to make sure that the formal parameter (which is declared)
        // inside the block is identified as having been definitely assigned.
        //
        AstCatchClause *clause = try_statement -> CatchClause(i);

        AstBlock *clause_block_body = clause -> block;
        definite_block_stack -> Push(clause_block_body);
        definite_try_stack -> SetTopBlock(clause_block_body);

        for (int j = 0; j < clause_block_body -> block_symbol -> NumVariableSymbols(); j++)
            definite_visible_variables -> AddElement(clause_block_body -> block_symbol -> VariableSym(j));

        //
        // The parameter must be added as well
        //
        definitely_assigned_variables -> AddElement(clause -> parameter_symbol -> LocalVariableIndex());
        if (control.option.g)
        {
            VariableSymbol *variable = clause -> parameter_symbol;
            definite_block_stack -> TopLocallyDefinedVariables() -> AddElement(variable -> LocalVariableIndex());
#ifdef DUMP
Coutput << "(7) Variable \"" << variable -> Name() << " #" << variable -> LocalVariableIndex()
        << "\" is defined at line " << lex_stream -> Line(clause -> formal_parameter -> LeftToken())
        << endl;
#endif
        }
        definitely_assigned_variables -> du_set = before_catch_finals;

        DefiniteBlockStatements(clause_block_body);

#ifdef DUMP
if (control.option.g && clause_block_body -> NumLocallyDefinedVariables() > 0)
{
Coutput << "(8) At Line " << lex_stream -> Line(clause_block_body -> RightToken())
        << " the range for the following variables end:" << endl << endl;
for (int l = 0; l < clause_block_body -> NumLocallyDefinedVariables(); l++)
Coutput << "    \"" << clause_block_body -> LocallyDefinedVariable(l) -> Name()
        << "\"" << endl;
}
#endif
        //
        // Once we are done with a block, its enclosed local variables are no
        // longer visible.
        //
        for (int k = 0; k < clause_block_body -> block_symbol -> NumVariableSymbols(); k++)
        {
            VariableSymbol *variable = clause_block_body -> block_symbol -> VariableSym(k);

            blank_finals -> RemoveElement(variable -> LocalVariableIndex());
            definitely_assigned_variables -> ReclaimElement(variable -> LocalVariableIndex());
            definite_visible_variables -> RemoveElement(variable);
        }
        //
        // The parameter goes out of scope as well
        //
        blank_finals -> RemoveElement(clause -> parameter_symbol -> LocalVariableIndex());
        definitely_assigned_variables -> ReclaimElement(clause -> parameter_symbol -> LocalVariableIndex());
        definite_visible_variables -> RemoveElement(clause -> parameter_symbol);

        possibly_finals_union *= definite_block_stack -> TopExitPair(*definitely_assigned_variables).du_set;

        definite_block_stack -> Pop();

        //
        // Process the set of variables that were definitely assigned
        // after this catch block
        //
        after_blocks *= definitely_assigned_variables -> da_set;
    }

    definitely_assigned_variables -> du_set = possibly_finals_union;
    definite_try_stack -> Pop();

    //
    // Like the catch clauses, a finally block must not be processed
    // in the environment of its associated try block but in the
    // environment of its immediate enclosing block.
    //
    if (try_statement -> finally_clause_opt)
    {
        definitely_assigned_variables -> da_set = starting_pair.da_set;

        DefiniteBlock(try_statement -> finally_clause_opt -> block);

        definitely_assigned_variables -> da_set += after_blocks;
    }
    else definitely_assigned_variables -> da_set = after_blocks;

    return;
}


void Semantic::DefiniteEmptyStatement(Ast *stmt)
{
    return;
}


void Semantic::DefiniteClassDeclaration(Ast *decl)
{
    //
    // All the methods within the body of a local class are processed when
    // the class is compiled.
    //

    return;
}


void Semantic::DefiniteMethodBody(AstMethodDeclaration *method_declaration, Tuple<VariableSymbol *> &finals)
{
    if (! method_declaration -> method_body -> EmptyStatementCast())
    {
#ifdef DUMP
if (control.option.g)
Coutput << "(9) Processing method \"" << method_declaration -> method_symbol -> Name()
        << "\" in " << ThisType() -> ContainingPackage() -> PackageName() << "/"
        << ThisType() -> ExternalName() << endl;
#endif
        AstConstructorBlock *constructor_block = method_declaration -> method_body -> ConstructorBlockCast();
        AstBlock *block_body = (constructor_block ? constructor_block -> block : (AstBlock *) method_declaration -> method_body);

        universe = new DefinitePair(block_body -> block_symbol -> max_variable_index + finals.Length(), BitSet::UNIVERSE);
        definite_block_stack = new DefiniteBlockStack(control,
                                                      method_declaration -> method_symbol -> max_block_depth,
                                                      universe -> Size());
        definite_try_stack = new DefiniteTryStack(method_declaration -> method_symbol -> max_block_depth);
        definite_final_assignment_stack =  new DefiniteFinalAssignmentStack();
        definite_visible_variables = new SymbolSet(universe -> Size());
        definitely_assigned_variables = new DefinitePair(universe -> Size());
        blank_finals = new BitSet(universe -> Size(), BitSet::EMPTY);
        for (int i = 0; i < finals.Length(); i++) // Assume that all final instance variables have been assigned a value.
        {
            int index = block_body -> block_symbol -> max_variable_index + i;
            finals[i] -> SetLocalVariableIndex(index);
            definitely_assigned_variables -> AssignElement(index);
            definite_visible_variables -> AddElement(finals[i]);
        }

        definite_block_stack -> Push(block_body);

        AstMethodDeclarator *method_declarator = method_declaration -> method_declarator;
        for (int k = 0; k < method_declarator -> NumFormalParameters(); k++)
        {
            AstVariableDeclarator *formal_declarator = method_declarator -> FormalParameter(k) -> formal_declarator;
            definitely_assigned_variables -> AssignElement(formal_declarator -> symbol -> LocalVariableIndex());
            if (control.option.g)
            {
                VariableSymbol *variable = formal_declarator -> symbol;
                definite_block_stack -> TopLocallyDefinedVariables() -> AddElement(variable -> LocalVariableIndex());
#ifdef DUMP
Coutput << "(10) Variable \"" << variable -> Name() << " #" << variable -> LocalVariableIndex()
        << "\" is defined at line " << lex_stream -> Line(formal_declarator -> LeftToken())
        << endl;
#endif
            }

            definite_visible_variables -> AddElement(formal_declarator -> symbol);
        }

        for (int l = 0; l < block_body -> block_symbol -> NumVariableSymbols(); l++)
        {
            VariableSymbol *variable = block_body -> block_symbol -> VariableSym(l);
            definite_visible_variables -> AddElement(variable);
        }

        DefiniteBlockStatements(block_body);

#ifdef DUMP
if (control.option.g && block_body -> NumLocallyDefinedVariables() > 0)
{
Coutput << "(11) At Line " << lex_stream -> Line(block_body -> RightToken())
        << " the range for the following variables end:" << endl << endl;
for (int j = 0; j < block_body -> NumLocallyDefinedVariables(); j++)
Coutput << "    \"" << block_body -> LocallyDefinedVariable(j) -> Name()
        << "\"" << endl;
}
#endif
        definite_block_stack -> Pop();

        delete universe;
        delete definitely_assigned_variables;
        delete definite_block_stack;
        delete definite_try_stack;
        delete definite_final_assignment_stack;
        delete definite_visible_variables;
        delete blank_finals;
    }

    return;
}


void Semantic::DefiniteConstructorBody(AstConstructorDeclaration *constructor_declaration, Tuple<VariableSymbol *> &finals)
{
#ifdef DUMP
if (control.option.g)
Coutput << "(12) Processing constructor \"" << constructor_declaration -> constructor_symbol -> Name()
        << "\" in " << ThisType() -> ContainingPackage() -> PackageName() << "/"
        << ThisType() -> ExternalName() << endl;
#endif
    AstConstructorBlock *constructor_block = constructor_declaration -> constructor_body;
    AstBlock *block_body = constructor_block -> block;

    universe = new DefinitePair(block_body -> block_symbol -> max_variable_index + finals.Length(), BitSet::UNIVERSE);
    definite_block_stack = new DefiniteBlockStack(control,
                                                  constructor_declaration -> constructor_symbol -> max_block_depth,
                                                  universe -> Size());
    definite_try_stack = new DefiniteTryStack(constructor_declaration -> constructor_symbol -> max_block_depth);
    definite_final_assignment_stack =  new DefiniteFinalAssignmentStack();
    definite_visible_variables = new SymbolSet(universe -> Size());
    definitely_assigned_variables = new DefinitePair(universe -> Size());
    blank_finals = new BitSet(universe -> Size(), BitSet::EMPTY);
    for (int i = 0; i < finals.Length(); i++)
    {
        int index = block_body -> block_symbol -> max_variable_index + i;

        finals[i] -> SetLocalVariableIndex(index);
        if (finals[i] -> IsDefinitelyAssigned())
        {
            definitely_assigned_variables -> AssignElement(index);
        }
        else
        {
            //
            // only blank finals can be possibly assigned without being
            // definitely assigned
            //
            if (finals[i] -> IsPossiblyAssigned())
                definitely_assigned_variables -> AddElement(index);
            blank_finals -> AddElement(index);
        }
        definite_visible_variables -> AddElement(finals[i]);
    }

    //
    // As an explicit constructor call cannot refer to any locally declared
    // variables other than formal parameters, no local variable can be
    // assigned within it (other than a formal parameter which is considered
    // to have been assigned anyway). Therefore, the following code is not
    // necessary:
    //
    //    if (this_call)
    //         DefiniteThisCall(this_call);
    //    else if (super_call)
    //         DefiniteSuperCall(super_call);
    //

    definite_block_stack -> Push(block_body);
    if (control.option.g)
    {
        //
        // We need this initialization to prevent debug info from being
        // generated for final fields (since they are not truly local
        // variables).
        //
        BitSet &locally_defined_variables = *definite_block_stack -> TopLocallyDefinedVariables();
        for (int i = 0; i < finals.Length(); i++) // Assume that all final instance variables have been assigned a value.
        {
            int index = block_body -> block_symbol -> max_variable_index + i;
            locally_defined_variables.AddElement(index);
        }
    }

    AstMethodDeclarator *constructor_declarator = constructor_declaration -> constructor_declarator;
    for (int j = 0; j < constructor_declarator -> NumFormalParameters(); j++)
    {
        AstVariableDeclarator *formal_declarator = constructor_declarator -> FormalParameter(j) -> formal_declarator;
        definitely_assigned_variables -> AddElement(formal_declarator -> symbol -> LocalVariableIndex());
        if (control.option.g)
        {
            VariableSymbol *variable = formal_declarator -> symbol;
            definite_block_stack -> TopLocallyDefinedVariables() -> AddElement(variable -> LocalVariableIndex());
#ifdef DUMP
Coutput << "(13) Variable \"" << variable -> Name() << " #" << variable -> LocalVariableIndex()
        << "\" is defined at line " << lex_stream -> Line(formal_declarator -> LeftToken())
        << endl;
#endif
        }

        definite_visible_variables -> AddElement(formal_declarator -> symbol);
    }

    for (int l = 0; l < block_body -> block_symbol -> NumVariableSymbols(); l++)
    {
        VariableSymbol *variable = block_body -> block_symbol -> VariableSym(l);
        definite_visible_variables -> AddElement(variable);
    }

    DefiniteBlockStatements(block_body);

#ifdef DUMP
if (control.option.g && block_body -> NumLocallyDefinedVariables() > 0)
{
Coutput << "(14) At Line " << lex_stream -> Line(block_body -> RightToken())
        << " the range for the following variables end:" << endl << endl;
for (int j = 0; j < block_body -> NumLocallyDefinedVariables(); j++)
Coutput << "    \"" << block_body -> LocallyDefinedVariable(j) -> Name()
        << "\"" << endl;
}
#endif
    //
    // Compute the set of finals that has definitely been assigned in this
    // constructor
    //
    BitSet &exit_set = definite_block_stack -> TopExitPair(*definitely_assigned_variables).da_set;
    for (int k = 0; k < finals.Length(); k++)
    {
        int index = block_body -> block_symbol -> max_variable_index + k;
        if (exit_set[index])
            finals[k] -> MarkDefinitelyAssigned();
    }

    definite_block_stack -> Pop();

    delete universe;
    delete definitely_assigned_variables;
    delete definite_block_stack;
    delete definite_try_stack;
    delete definite_final_assignment_stack;
    delete definite_visible_variables;
    delete blank_finals;

    return;
}


void Semantic::DefiniteBlockInitializer(AstBlock *block_body, int stack_size, Tuple<VariableSymbol *> &finals)
{
#ifdef DUMP
if (control.option.g)
Coutput << "(15) Processing Initializer block "
        << " in " << ThisType() -> ContainingPackage() -> PackageName() << "/"
        << ThisType() -> ExternalName() << endl;
#endif
    universe = new DefinitePair(block_body -> block_symbol -> max_variable_index + finals.Length(), BitSet::UNIVERSE);
    definite_block_stack = new DefiniteBlockStack(control, stack_size + 1, universe -> Size()); // +1 for absent method block
    definite_try_stack = new DefiniteTryStack(stack_size + 1);
    definite_final_assignment_stack =  new DefiniteFinalAssignmentStack();
    definite_visible_variables = new SymbolSet(universe -> Size());
    definitely_assigned_variables = new DefinitePair(universe -> Size());
    blank_finals = new BitSet(universe -> Size(), BitSet::EMPTY);
    for (int i = 0; i < finals.Length(); i++)
    {
        int index = block_body -> block_symbol -> max_variable_index + i;

        finals[i] -> SetLocalVariableIndex(index);
        if (finals[i] -> IsDefinitelyAssigned())
        {
            definitely_assigned_variables -> AssignElement(index);
        }
        else
        {
            //
            // only blank finals can be possibly assigned without being
            // definitely assigned
            //
            if (finals[i] -> IsPossiblyAssigned())
                definitely_assigned_variables -> AddElement(index);
            blank_finals -> AddElement(index);
        }
        definite_visible_variables -> AddElement(finals[i]);
    }

    definite_block_stack -> Push(NULL); // No method available
    definite_block_stack -> Push(block_body);
    if (control.option.g)
    {
        //
        // We need this initialization to prevent debug info from being
        // generated for final fields (since they are not truly local
        // variables).
        //
        BitSet &locally_defined_variables = *definite_block_stack -> TopLocallyDefinedVariables();
        for (int i = 0; i < finals.Length(); i++) // Assume that all final instance variables have been assigned a value.
        {
            int index = block_body -> block_symbol -> max_variable_index + i;
            locally_defined_variables.AddElement(index);
        }
    }

    for (int j = 0; j < block_body -> block_symbol -> NumVariableSymbols(); j++)
    {
        VariableSymbol *variable = block_body -> block_symbol -> VariableSym(j);
        definite_visible_variables -> AddElement(variable);
    }

    DefiniteBlockStatements(block_body);

#ifdef DUMP
if (control.option.g && block_body -> NumLocallyDefinedVariables() > 0)
{
Coutput << "(16) At Line " << lex_stream -> Line(block_body -> RightToken())
        << " the range for the following variables end:" << endl << endl;
for (int j = 0; j < block_body -> NumLocallyDefinedVariables(); j++)
Coutput << "    \"" << block_body -> LocallyDefinedVariable(j) -> Name()
        << "\"" << endl;
}
#endif
    //
    // For each final that has definitely or possibly been assigned a value
    // in this block, mark it appropriately.
    //
    DefinitePair &exit_pair = definite_block_stack -> TopExitPair(*definitely_assigned_variables);
    for (int k = 0; k < finals.Length(); k++)
    {
        int index = block_body -> block_symbol -> max_variable_index + k;
        if (exit_pair.da_set[index])
            finals[k] -> MarkDefinitelyAssigned();
        if (! exit_pair.du_set[index])
            finals[k] -> MarkPossiblyAssigned();
    }

    definite_block_stack -> Pop();
    definite_block_stack -> Pop(); // remove NULL that was pushed to indicate that no method is available

    delete universe;
    delete definitely_assigned_variables;
    delete definite_block_stack;
    delete definite_try_stack;
    delete definite_final_assignment_stack;
    delete definite_visible_variables;
    delete blank_finals;

    return;
}


void Semantic::DefiniteVariableInitializer(AstVariableDeclarator *variable_declarator, Tuple<VariableSymbol *> &finals)
{
    universe = new DefinitePair(finals.Length(), BitSet::UNIVERSE);
    definite_block_stack = NULL;
    definite_try_stack = NULL;
    definite_final_assignment_stack =  new DefiniteFinalAssignmentStack();
    definite_visible_variables = new SymbolSet(universe -> Size());
    definitely_assigned_variables = new DefinitePair(universe -> Size());
    blank_finals = new BitSet(universe -> Size(), BitSet::EMPTY);
    for (int i = 0; i < finals.Length(); i++)
    {
        finals[i] -> SetLocalVariableIndex(i);
        if (finals[i] -> IsDefinitelyAssigned())
        {
            definitely_assigned_variables -> AssignElement(i);
        }
        else
        {
            //
            // only blank finals can be possibly assigned without being
            // definitely assigned
            //
            if (finals[i] -> IsPossiblyAssigned())
                definitely_assigned_variables -> AddElement(i);
            blank_finals -> AddElement(i);
        }
        definite_visible_variables -> AddElement(finals[i]);
    }

    DefiniteVariableInitializer(variable_declarator);
    VariableSymbol *symbol = variable_declarator -> symbol;
    if (symbol -> ACC_FINAL())
        symbol -> MarkDefinitelyAssigned();
    //
    // Also, update any other finals that may have been initialized as
    // a side-effect in an assignment embedded within the initializer
    // expression.
    //
    BitSet &exit_set = definitely_assigned_variables -> da_set;
    for (int k = 0; k < finals.Length(); k++)
    {
        if (exit_set[k])
            finals[k] -> MarkDefinitelyAssigned();
    }

    delete universe;
    delete definitely_assigned_variables;
    delete definite_final_assignment_stack;
    delete definite_visible_variables;
    delete blank_finals;

    return;
}

#ifdef HAVE_JIKES_NAMESPACE
} // Close namespace Jikes block
#endif

