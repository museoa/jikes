//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#include "config.h"
#include "unicode.h"
#include "ast.h"
#ifdef TEST
    unsigned Ast::count = 0;
#endif


void AstCompilationUnit::FreeAst()
{
     delete ast_pool;
}

//
// This procedure uses a  quick sort algorithm to sort the cases
// in a switch statement.
//
void AstSwitchStatement::SortCases()
{
     int lower,
         upper,
         lostack[32],
         histack[32];
 
     int top,
         i,
         j;
     CaseElement pivot, temp;

     top = 0;
     lostack[top] = 0;
     histack[top] = map.Length() - 1;
 
     while(top >= 0)
     {
         lower = lostack[top];
         upper = histack[top];
         top--;
 
         while(upper > lower)
         {
             //
             // The array is most-likely almost sorted. Therefore,
             // we use the middle element as the pivot element.
             //
             i = (lower + upper) / 2;
             pivot = *map[i];
             *map[i] = *map[lower];
 
             //
             // Split the array section indicated by LOWER and UPPER
             // using ARRAY(LOWER) as the pivot.
             //
             i = lower;
             for (j = lower + 1; j <= upper; j++)
                 if (map[j] -> Value() < pivot.Value() || 
                     (map[j] -> Value() == pivot.Value() && map[j] -> index < pivot.index)) // keep the sort stable
                 {
                     temp = *map[++i];
                     *map[i] = *map[j];
                     *map[j] = temp;
                 }
             *map[lower] = *map[i];
             *map[i] = pivot;

             top++;
             if ((i - lower) < (upper - i))
             {
                 lostack[top] = i + 1;
                 histack[top] = upper;
                 upper = i - 1;
             }
             else
             {
                 histack[top] = i - 1;
                 lostack[top] = lower;
                 lower = i + 1;
             }
         }
     }

     return;
}


Ast *Ast::Clone(StoragePool *ast_pool)
{
    return (Ast *) NULL;
}

Ast *AstBlock::Clone(StoragePool *ast_pool)
{
    AstBlock *clone = ast_pool -> GenBlock();

    clone -> label_token_opt = this -> label_token_opt;
    clone -> nesting_level = this -> nesting_level;
    clone -> left_brace_token = this -> left_brace_token;
    if (this -> NumStatements() == 0)
        clone -> block_statements = NULL;
    else
    {
        for (int i = 0; i < this -> NumStatements(); i++)
            clone -> AddStatement(this -> Statement(i) -> Clone(ast_pool));
    }
    clone -> right_brace_token = this -> right_brace_token;

    return clone;
}

Ast *AstPrimitiveType::Clone(StoragePool *ast_pool)
{
    AstPrimitiveType *clone = ast_pool -> GenPrimitiveType(this -> kind, this -> primitive_kind_token);

    return clone;
}

Ast *AstArrayType::Clone(StoragePool *ast_pool)
{
    AstArrayType *clone = ast_pool -> GenArrayType();

    clone -> type = this -> type -> Clone(ast_pool);
    clone -> AllocateBrackets(this -> NumBrackets());
    for (int i = 0; i < this -> NumBrackets(); i++)
        clone -> AddBrackets((AstBrackets *) this -> Brackets(i) -> Clone(ast_pool));

    return clone;
}

Ast *AstSimpleName::Clone(StoragePool *ast_pool)
{
    AstSimpleName *clone = ast_pool -> GenSimpleName(this -> identifier_token);
    clone -> resolution_opt = (AstExpression *) (this -> resolution_opt ? this -> resolution_opt -> Clone(ast_pool) : NULL);

    return clone;
}

Ast *AstPackageDeclaration::Clone(StoragePool *ast_pool)
{
    AstPackageDeclaration *clone = ast_pool -> GenPackageDeclaration();

    clone -> package_token = this -> package_token;
    clone -> name = (AstExpression *) this -> name -> Clone(ast_pool);
    clone -> semicolon_token = this -> semicolon_token;

    return clone;
}

Ast *AstImportDeclaration::Clone(StoragePool *ast_pool)
{
    AstImportDeclaration *clone = ast_pool -> GenImportDeclaration();

    clone -> import_token = this -> import_token;
    clone -> name = (AstExpression *) this -> name -> Clone(ast_pool);
    clone -> star_token_opt = this -> star_token_opt;
    clone -> semicolon_token = this -> semicolon_token;

    return clone;
}

Ast *AstCompilationUnit::Clone(StoragePool *ast_pool)
{
    AstCompilationUnit *clone = ast_pool -> GenCompilationUnit();

    clone -> package_declaration_opt = (AstPackageDeclaration *)
                                       (this -> package_declaration_opt
                                              ? this -> package_declaration_opt -> Clone(ast_pool) : NULL);
    for (int i = 0; i < this -> NumImportDeclarations(); i++)
        clone -> AddImportDeclaration((AstImportDeclaration *) this -> ImportDeclaration(i) -> Clone(ast_pool));
    for (int k = 0; k < this -> NumTypeDeclarations(); k++)
        clone -> AddTypeDeclaration(this -> TypeDeclaration(k) -> Clone(ast_pool));

    return clone;
}

Ast *AstModifier::Clone(StoragePool *ast_pool)
{
    AstModifier *clone = ast_pool -> GenModifier(this -> kind, this -> modifier_kind_token);

    return clone;
}

Ast *AstEmptyDeclaration::Clone(StoragePool *ast_pool)
{
    AstEmptyDeclaration *clone = ast_pool -> GenEmptyDeclaration(this -> semicolon_token);

    return clone;
}

Ast *AstClassBody::Clone(StoragePool *ast_pool)
{
    AstClassBody *clone = ast_pool -> GenClassBody();

    clone -> left_brace_token = this -> left_brace_token;
    for (int i = 0; i < this -> NumClassBodyDeclarations(); i++)
        clone -> AddClassBodyDeclaration(this -> ClassBodyDeclaration(i) -> Clone(ast_pool));
    clone -> right_brace_token = this -> right_brace_token;

    return clone;
}

Ast *AstClassDeclaration::Clone(StoragePool *ast_pool)
{
    AstClassDeclaration *clone = ast_pool -> GenClassDeclaration();

    for (int i = 0; i < this -> NumClassModifiers(); i++)
        clone -> AddClassModifier((AstModifier *) this -> ClassModifier(i) -> Clone(ast_pool));
    clone -> class_token = this -> class_token;
    clone -> identifier_token = this -> identifier_token;
    clone -> super_opt = (Ast *) (this -> super_opt ? this -> super_opt -> Clone(ast_pool) : NULL); 
    for (int k = 0; k < this -> NumInterfaces(); k++)
        clone -> AddInterface((AstExpression *) this -> Interface(k) -> Clone(ast_pool));
    clone -> class_body = (AstClassBody *) this -> class_body -> Clone(ast_pool);

    return clone;
}

Ast *AstArrayInitializer::Clone(StoragePool *ast_pool)
{
    AstArrayInitializer *clone = ast_pool -> GenArrayInitializer();

    clone -> left_brace_token = this -> left_brace_token;
    for (int k = 0; k < this -> NumVariableInitializers(); k++)
        clone -> AddVariableInitializer(this -> VariableInitializer(k) -> Clone(ast_pool));
    clone -> right_brace_token = this -> right_brace_token;

    return clone;
}

Ast *AstBrackets::Clone(StoragePool *ast_pool)
{
    AstBrackets *clone = ast_pool -> GenBrackets(this -> left_bracket_token, this -> right_bracket_token);

    return clone;
}

Ast *AstVariableDeclaratorId::Clone(StoragePool *ast_pool)
{
    AstVariableDeclaratorId *clone = ast_pool -> GenVariableDeclaratorId();

    clone -> identifier_token = this -> identifier_token;
    clone -> AllocateBrackets(this -> NumBrackets());
    for (int i = 0; i < this -> NumBrackets(); i++)
        clone -> AddBrackets((AstBrackets *) this -> Brackets(i) -> Clone(ast_pool));

    return clone;
}

Ast *AstVariableDeclarator::Clone(StoragePool *ast_pool)
{
    AstVariableDeclarator *clone = ast_pool -> GenVariableDeclarator();

    clone -> variable_declarator_name = (AstVariableDeclaratorId *) this -> variable_declarator_name -> Clone(ast_pool);
    clone -> variable_initializer_opt = (Ast *) (this -> variable_initializer_opt
                                                       ? this -> variable_initializer_opt -> Clone(ast_pool)
                                                       : NULL);

    return clone;
}

Ast *AstFieldDeclaration::Clone(StoragePool *ast_pool)
{
    AstFieldDeclaration *clone = ast_pool -> GenFieldDeclaration();

    for (int i = 0; i < this -> NumVariableModifiers(); i++)
        clone -> AddVariableModifier((AstModifier *) this -> VariableModifier(i) -> Clone(ast_pool));
    clone -> type = this -> type -> Clone(ast_pool);
    for (int k = 0; k < this -> NumVariableDeclarators(); k++)
        clone -> AddVariableDeclarator((AstVariableDeclarator *) this -> VariableDeclarator(k) -> Clone(ast_pool));
    clone -> semicolon_token = this -> semicolon_token;

    return clone;
}

Ast *AstFormalParameter::Clone(StoragePool *ast_pool)
{
    AstFormalParameter *clone = ast_pool -> GenFormalParameter();

    if (this -> NumParameterModifiers() == 0)
        clone -> parameter_modifiers = NULL;
    else
    {
        for (int i = 0; i < this -> NumParameterModifiers(); i++)
            clone -> AddParameterModifier((AstModifier *) this -> ParameterModifier(i) -> Clone(ast_pool));
    }
    clone -> type = this -> type -> Clone(ast_pool);
    clone -> variable_declarator_name = (AstVariableDeclaratorId *) this -> variable_declarator_name -> Clone(ast_pool);

    return clone;
}

Ast *AstMethodDeclarator::Clone(StoragePool *ast_pool)
{
    AstMethodDeclarator *clone = ast_pool -> GenMethodDeclarator();

    clone -> identifier_token = this -> identifier_token;
    clone -> left_parenthesis_token = this -> left_parenthesis_token;
    clone -> AllocateFormalParameters(this -> NumFormalParameters());
    for (int i = 0; i < this -> NumFormalParameters(); i++)
        clone -> AddFormalParameter((AstFormalParameter *) this -> FormalParameter(i) -> Clone(ast_pool));
    clone -> right_parenthesis_token = this -> right_parenthesis_token;
    clone -> AllocateBrackets(this -> NumBrackets());
    for (int k = 0; k < this -> NumBrackets(); k++)
        clone -> AddBrackets((AstBrackets *) this -> Brackets(k) -> Clone(ast_pool));

    return clone;
}

Ast *AstMethodDeclaration::Clone(StoragePool *ast_pool)
{
    AstMethodDeclaration *clone = ast_pool -> GenMethodDeclaration();

    for (int i = 0; i < this -> NumMethodModifiers(); i++)
        clone -> AddMethodModifier((AstModifier *) this -> MethodModifier(i) -> Clone(ast_pool));
    clone -> type = this -> type -> Clone(ast_pool);
    clone -> method_declarator = (AstMethodDeclarator *) this -> method_declarator -> Clone(ast_pool);
    for (int k = 0; k < this -> NumThrows(); k++)
        clone -> AddThrow((AstExpression *) this -> Throw(k) -> Clone(ast_pool));
    clone -> method_body = (AstStatement *) this -> method_body -> Clone(ast_pool);

    return clone;
}

Ast *AstStaticInitializer::Clone(StoragePool *ast_pool)
{
    AstStaticInitializer *clone = ast_pool -> GenStaticInitializer();

    clone -> static_token = this -> static_token;
    clone -> block = (AstBlock *) this -> block -> Clone(ast_pool);

    return clone;
}

Ast *AstThisCall::Clone(StoragePool *ast_pool)
{
    AstThisCall *clone = ast_pool -> GenThisCall();

    clone -> this_token = this -> this_token;
    clone -> left_parenthesis_token = this -> left_parenthesis_token;
    clone -> AllocateArguments(this -> NumArguments());
    for (int i = 0; i < this -> NumArguments(); i++)
        clone -> AddArgument((AstExpression *) this -> Argument(i) -> Clone(ast_pool));
    clone -> right_parenthesis_token = this -> right_parenthesis_token;
    clone -> semicolon_token = this -> semicolon_token;

    return clone;
}

Ast *AstSuperCall::Clone(StoragePool *ast_pool)
{
    AstSuperCall *clone = ast_pool -> GenSuperCall();

    clone -> base_opt = (AstExpression *) (this -> base_opt ? this -> base_opt -> Clone(ast_pool) : NULL);
    clone -> dot_token_opt = this -> dot_token_opt;
    clone -> super_token = this -> super_token;
    clone -> left_parenthesis_token = this -> left_parenthesis_token;
    clone -> AllocateArguments(this -> NumArguments());
    for (int i = 0; i < this -> NumArguments(); i++)
        clone -> AddArgument((AstExpression *) this -> Argument(i) -> Clone(ast_pool));
    clone -> right_parenthesis_token = this -> right_parenthesis_token;
    clone -> semicolon_token = this -> semicolon_token;
    clone -> AllocateLocalArguments(this -> NumLocalArguments());
    for (int k = 0; k < this -> NumLocalArguments(); k++)
        clone -> AddLocalArgument((AstExpression *) this -> LocalArgument(k) -> Clone(ast_pool));

    return clone;
}

Ast *AstConstructorBlock::Clone(StoragePool *ast_pool)
{
    AstConstructorBlock *clone = ast_pool -> GenConstructorBlock();

    clone -> left_brace_token = this -> left_brace_token;
    clone -> explicit_constructor_invocation_opt = (Ast *) 
                                                   (this -> explicit_constructor_invocation_opt
                                                          ? this -> explicit_constructor_invocation_opt -> Clone(ast_pool)
                                                          : NULL);
    clone -> block = (AstBlock *) this -> block -> Clone(ast_pool);
    clone -> right_brace_token = this -> right_brace_token;

    return clone;
}

Ast *AstConstructorDeclaration::Clone(StoragePool *ast_pool)
{
    AstConstructorDeclaration *clone = ast_pool -> GenConstructorDeclaration();

    for (int i = 0; i < this -> NumConstructorModifiers(); i++)
        clone -> AddConstructorModifier((AstModifier *) this -> ConstructorModifier(i) -> Clone(ast_pool));
    clone -> constructor_declarator = (AstMethodDeclarator *) this -> constructor_declarator -> Clone(ast_pool);
    for (int k = 0; k < this -> NumThrows(); k++)
        clone -> AddThrow((AstExpression *) this -> Throw(k) -> Clone(ast_pool));
    clone -> constructor_body = (AstConstructorBlock *) this -> constructor_body -> Clone(ast_pool);

    return clone;
}

Ast *AstInterfaceDeclaration::Clone(StoragePool *ast_pool)
{
    AstInterfaceDeclaration *clone = ast_pool -> GenInterfaceDeclaration();

    for (int i = 0; i < this -> NumInterfaceModifiers(); i++)
        clone -> AddInterfaceModifier((AstModifier *) this -> InterfaceModifier(i) -> Clone(ast_pool));
    clone -> interface_token = this -> interface_token;
    clone -> identifier_token = this -> identifier_token;
    for (int k = 0; k < this -> NumExtendsInterfaces(); k++)
        clone -> AddExtendsInterface((AstExpression *) this -> ExtendsInterface(k) -> Clone(ast_pool));
    clone -> left_brace_token = this -> left_brace_token;
    for (int l = 0; l < this -> NumExtendsInterfaces(); l++)
        clone -> AddInterfaceMemberDeclaration((AstExpression *) this -> InterfaceMemberDeclaration(l) -> Clone(ast_pool));
    clone -> right_brace_token = this -> right_brace_token;

    return clone;
}

Ast *AstLocalVariableDeclarationStatement::Clone(StoragePool *ast_pool)
{
    AstLocalVariableDeclarationStatement *clone = ast_pool -> GenLocalVariableDeclarationStatement();

    for (int i = 0; i < this -> NumLocalModifiers(); i++)
        clone -> AddLocalModifier((AstModifier *) this -> LocalModifier(i) -> Clone(ast_pool));
    clone -> type = this -> type -> Clone(ast_pool);
    for (int k = 0; k < this -> NumVariableDeclarators(); k++)
        clone -> AddVariableDeclarator((AstVariableDeclarator *) this -> VariableDeclarator(k) -> Clone(ast_pool));
    clone -> semicolon_token_opt = this -> semicolon_token_opt;

    return clone;
}

Ast *AstIfStatement::Clone(StoragePool *ast_pool)
{
    AstIfStatement *clone = ast_pool -> GenIfStatement();

    clone -> if_token = this -> if_token;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> true_statement = (AstStatement *) this -> true_statement -> Clone(ast_pool);
    clone -> false_statement_opt = (AstStatement *) 
                                   (this -> false_statement_opt
                                          ? this -> false_statement_opt -> Clone(ast_pool)
                                          : NULL);

    return clone;
}

Ast *AstEmptyStatement::Clone(StoragePool *ast_pool)
{
    AstEmptyStatement *clone = ast_pool -> GenEmptyStatement(this -> semicolon_token);

    return clone;
}

Ast *AstExpressionStatement::Clone(StoragePool *ast_pool)
{
    AstExpressionStatement *clone = ast_pool -> GenExpressionStatement();

    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> semicolon_token_opt = this -> semicolon_token_opt;

    return clone;
}

Ast *AstCaseLabel::Clone(StoragePool *ast_pool)
{
    AstCaseLabel *clone = ast_pool -> GenCaseLabel();

    clone -> case_token = this -> case_token;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> colon_token = this -> colon_token;
    clone -> map_index = this -> map_index;

    return clone;
}

Ast *AstDefaultLabel::Clone(StoragePool *ast_pool)
{
    AstDefaultLabel *clone = ast_pool -> GenDefaultLabel();

    clone -> default_token = this -> default_token;
    clone -> colon_token = this -> colon_token;

    return clone;
}

Ast *AstSwitchBlockStatement::Clone(StoragePool *ast_pool)
{
    AstSwitchBlockStatement *clone = ast_pool -> GenSwitchBlockStatement();

    clone -> AllocateSwitchLabels(this -> NumSwitchLabels());
    for (int i = 0; i < this -> NumSwitchLabels(); i++)
        clone -> AddSwitchLabel(this -> SwitchLabel(i) -> Clone(ast_pool));

    clone -> AllocateBlockStatements(this -> NumStatements());
    for (int k = 0; k < this -> NumStatements(); k++)
        clone -> AddStatement((AstStatement *) this -> Statement(k) -> Clone(ast_pool));

    return clone;
}

Ast *AstSwitchStatement::Clone(StoragePool *ast_pool)
{
    AstSwitchStatement *clone = ast_pool -> GenSwitchStatement();

    clone -> switch_token = this -> switch_token;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> switch_block = (AstBlock *) this -> switch_block -> Clone(ast_pool);

    return clone;
}

Ast *AstWhileStatement::Clone(StoragePool *ast_pool)
{
    AstWhileStatement *clone = ast_pool -> GenWhileStatement();

    clone -> while_token = this -> while_token;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> statement = (AstStatement *) this -> statement -> Clone(ast_pool);

    return clone;
}

Ast *AstDoStatement::Clone(StoragePool *ast_pool)
{
    AstDoStatement *clone = ast_pool -> GenDoStatement();

    clone -> do_token = this -> do_token;
    clone -> statement = (AstStatement *) this -> statement -> Clone(ast_pool);
    clone -> while_token = this -> while_token;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> semicolon_token = this -> semicolon_token;

    return clone;
}

Ast *AstForStatement::Clone(StoragePool *ast_pool)
{
    AstForStatement *clone = ast_pool -> GenForStatement();

    clone -> for_token = this -> for_token;
    for (int i = 0; i < this -> NumForInitStatements(); i++)
        clone -> AddForInitStatement((AstStatement *) this -> ForInitStatement(i) -> Clone(ast_pool));
    clone -> end_expression_opt = (AstExpression *)
                                  (this -> end_expression_opt
                                         ? this -> end_expression_opt -> Clone(ast_pool)
                                         : NULL);
    for (int k = 0; k < this -> NumForUpdateStatements(); k++)
        clone -> AddForUpdateStatement((AstExpressionStatement *) this -> ForUpdateStatement(k) -> Clone(ast_pool));
    clone -> statement = (AstStatement *) this -> statement -> Clone(ast_pool);

    return clone;
}

Ast *AstBreakStatement::Clone(StoragePool *ast_pool)
{
    AstBreakStatement *clone = ast_pool -> GenBreakStatement();

    clone -> break_token = this -> break_token;
    clone -> identifier_token_opt = this -> identifier_token_opt;
    clone -> semicolon_token = this -> semicolon_token;
    clone -> nesting_level = this -> nesting_level;

    return clone;
}

Ast *AstContinueStatement::Clone(StoragePool *ast_pool)
{
    AstContinueStatement *clone = ast_pool -> GenContinueStatement();

    clone -> continue_token = this -> continue_token;
    clone -> identifier_token_opt = this -> identifier_token_opt;
    clone -> semicolon_token = this -> semicolon_token;
    clone -> nesting_level = this -> nesting_level;

    return clone;
}

Ast *AstReturnStatement::Clone(StoragePool *ast_pool)
{
    AstReturnStatement *clone = ast_pool -> GenReturnStatement();

    clone -> return_token = this -> return_token;
    clone -> expression_opt = (AstExpression *) (this -> expression_opt ? this -> expression_opt -> Clone(ast_pool) : NULL);
    clone -> semicolon_token = this -> semicolon_token;

    return clone;
}

Ast *AstThrowStatement::Clone(StoragePool *ast_pool)
{
    AstThrowStatement *clone = ast_pool -> GenThrowStatement();

    clone -> throw_token = this -> throw_token;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> semicolon_token = this -> semicolon_token;

    return clone;
}

Ast *AstSynchronizedStatement::Clone(StoragePool *ast_pool)
{
    AstSynchronizedStatement *clone = ast_pool -> GenSynchronizedStatement();

    clone -> synchronized_token = this -> synchronized_token;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> block = (AstBlock *) this -> block -> Clone(ast_pool);

    return clone;
}

Ast *AstCatchClause::Clone(StoragePool *ast_pool)
{
    AstCatchClause *clone = ast_pool -> GenCatchClause();

    clone -> catch_token = this -> catch_token;
    clone -> formal_parameter = (AstFormalParameter *) this -> formal_parameter -> Clone(ast_pool);
    clone -> block = (AstBlock *) this -> block -> Clone(ast_pool);

    return clone;
}

Ast *AstFinallyClause::Clone(StoragePool *ast_pool)
{
    AstFinallyClause *clone = ast_pool -> GenFinallyClause();

    clone -> finally_token = this -> finally_token;
    clone -> block = (AstBlock *) this -> block -> Clone(ast_pool);

    return clone;
}

Ast *AstTryStatement::Clone(StoragePool *ast_pool)
{
    AstTryStatement *clone = ast_pool -> GenTryStatement();

    clone -> try_token = this -> try_token;
    clone -> block = (AstBlock *) this -> block -> Clone(ast_pool);
    for (int k = 0; k < this -> NumCatchClauses(); k++)
        clone -> AddCatchClause((AstCatchClause *) this -> CatchClause(k) -> Clone(ast_pool));
    clone -> finally_clause_opt = (AstFinallyClause *)
                                  (this -> finally_clause_opt
                                         ? this -> finally_clause_opt -> Clone(ast_pool)
                                         : NULL);

    return clone;
}

Ast *AstIntegerLiteral::Clone(StoragePool *ast_pool)
{
    AstIntegerLiteral *clone = ast_pool -> GenIntegerLiteral(this -> integer_literal_token);

    return clone;
}

Ast *AstLongLiteral::Clone(StoragePool *ast_pool)
{
    AstLongLiteral *clone = ast_pool -> GenLongLiteral(this -> long_literal_token);

    return clone;
}

Ast *AstFloatingPointLiteral::Clone(StoragePool *ast_pool)
{
    AstFloatingPointLiteral *clone = ast_pool -> GenFloatingPointLiteral(this -> floating_point_literal_token);

    return clone;
}

Ast *AstDoubleLiteral::Clone(StoragePool *ast_pool)
{
    AstDoubleLiteral *clone = ast_pool -> GenDoubleLiteral(this -> double_literal_token);

    return clone;
}

Ast *AstTrueLiteral::Clone(StoragePool *ast_pool)
{
    AstTrueLiteral *clone = ast_pool -> GenTrueLiteral(this -> true_literal_token);

    return clone;
}

Ast *AstFalseLiteral::Clone(StoragePool *ast_pool)
{
    AstFalseLiteral *clone = ast_pool -> GenFalseLiteral(this -> false_literal_token);

    return clone;
}

Ast *AstStringLiteral::Clone(StoragePool *ast_pool)
{
    AstStringLiteral *clone = ast_pool -> GenStringLiteral(this -> string_literal_token);

    return clone;
}

Ast *AstCharacterLiteral::Clone(StoragePool *ast_pool)
{
    AstCharacterLiteral *clone = ast_pool -> GenCharacterLiteral(this -> character_literal_token);

    return clone;
}

Ast *AstNullLiteral::Clone(StoragePool *ast_pool)
{
    AstNullLiteral *clone = ast_pool -> GenNullLiteral(this -> null_token);

    return clone;
}

Ast *AstThisExpression::Clone(StoragePool *ast_pool)
{
    AstThisExpression *clone = ast_pool -> GenThisExpression(this -> this_token);

    return clone;
}

Ast *AstSuperExpression::Clone(StoragePool *ast_pool)
{
    AstSuperExpression *clone = ast_pool -> GenSuperExpression(this -> super_token);

    return clone;
}

Ast *AstParenthesizedExpression::Clone(StoragePool *ast_pool)
{
    AstParenthesizedExpression *clone = ast_pool -> GenParenthesizedExpression();

    clone -> left_parenthesis_token = this -> left_parenthesis_token;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> right_parenthesis_token = this -> right_parenthesis_token;

    return clone;
}

Ast *AstTypeExpression::Clone(StoragePool *ast_pool)
{
    AstTypeExpression *clone = ast_pool -> GenTypeExpression(this -> type -> Clone(ast_pool));

    return clone;
}

Ast *AstClassInstanceCreationExpression::Clone(StoragePool *ast_pool)
{
    AstClassInstanceCreationExpression *clone = ast_pool -> GenClassInstanceCreationExpression();

    clone -> base_opt = (AstExpression *) (this -> base_opt ? this -> base_opt -> Clone(ast_pool) : NULL);
    clone -> dot_token_opt = this -> dot_token_opt;
    clone -> new_token = this -> new_token;
    clone -> class_type = (AstTypeExpression *) this -> class_type -> Clone(ast_pool);
    clone -> left_parenthesis_token = this -> left_parenthesis_token;
    clone -> AllocateArguments(this -> NumArguments());
    for (int i = 0; i < this -> NumArguments(); i++)
        clone -> AddArgument((AstExpression *) this -> Argument(i) -> Clone(ast_pool));
    clone -> right_parenthesis_token = this -> right_parenthesis_token;
    clone -> class_body_opt = (AstClassBody *) (this -> class_body_opt ? this -> class_body_opt -> Clone(ast_pool) : NULL);
    clone -> AllocateLocalArguments(this -> NumLocalArguments());
    for (int k = 0; k < this -> NumLocalArguments(); k++)
        clone -> AddLocalArgument((AstExpression *) this -> LocalArgument(k) -> Clone(ast_pool));

    return clone;
}

Ast *AstDimExpr::Clone(StoragePool *ast_pool)
{
    AstDimExpr *clone = ast_pool -> GenDimExpr();

    clone -> left_bracket_token = this -> left_bracket_token;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> right_bracket_token = this -> right_bracket_token;

    return clone;
}

Ast *AstArrayCreationExpression::Clone(StoragePool *ast_pool)
{
    AstArrayCreationExpression *clone = ast_pool -> GenArrayCreationExpression();

    clone -> new_token = this -> new_token;
    clone -> array_type = this -> array_type -> Clone(ast_pool);
    clone -> AllocateDimExprs(this -> NumDimExprs());
    for (int i = 0; i < this -> NumDimExprs(); i++)
        clone -> AddDimExpr((AstDimExpr *) this -> DimExpr(i) -> Clone(ast_pool));
    clone -> AllocateBrackets(this -> NumBrackets());
    for (int k = 0; k < this -> NumBrackets(); k++)
        clone -> AddBrackets((AstBrackets *) this -> Brackets(k) -> Clone(ast_pool));
    clone -> array_initializer_opt = (AstArrayInitializer *)
                                     (this -> array_initializer_opt ? this -> array_initializer_opt -> Clone(ast_pool) : NULL);

    return clone;
}

Ast *AstFieldAccess::Clone(StoragePool *ast_pool)
{
    AstFieldAccess *clone = ast_pool -> GenFieldAccess();

    clone -> base = (AstExpression *) this -> base -> Clone(ast_pool);
    clone -> dot_token = this -> dot_token;
    clone -> identifier_token = this -> identifier_token;
    clone -> resolution_opt = (AstExpression *) (this -> resolution_opt ? this -> resolution_opt -> Clone(ast_pool) : NULL);

    return clone;
}

Ast *AstMethodInvocation::Clone(StoragePool *ast_pool)
{
    AstMethodInvocation *clone = ast_pool -> GenMethodInvocation();

    clone -> method = (AstExpression *) this -> method -> Clone(ast_pool);
    clone -> left_parenthesis_token = this -> left_parenthesis_token;
    clone -> AllocateArguments(this -> NumArguments());
    for (int i = 0; i < this -> NumArguments(); i++)
        clone -> AddArgument((AstExpression *) this -> Argument(i) -> Clone(ast_pool));
    clone -> right_parenthesis_token = this -> right_parenthesis_token;

    return clone;
}

Ast *AstArrayAccess::Clone(StoragePool *ast_pool)
{
    AstArrayAccess *clone = ast_pool -> GenArrayAccess();

    clone -> base = (AstExpression *) this -> base -> Clone(ast_pool);
    clone -> left_bracket_token = this -> left_bracket_token;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> right_bracket_token = this -> right_bracket_token;

    return clone;
}

Ast *AstPostUnaryExpression::Clone(StoragePool *ast_pool)
{
    AstPostUnaryExpression *clone = ast_pool -> GenPostUnaryExpression(this -> post_unary_tag);

    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);
    clone -> post_operator_token = this -> post_operator_token;

    return clone;
}

Ast *AstPreUnaryExpression::Clone(StoragePool *ast_pool)
{
    AstPreUnaryExpression *clone = ast_pool -> GenPreUnaryExpression(this -> pre_unary_tag);

    clone -> pre_operator_token = this -> pre_operator_token;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);

    return clone;
}

Ast *AstCastExpression::Clone(StoragePool *ast_pool)
{
    AstCastExpression *clone = ast_pool -> GenCastExpression();

    clone -> left_parenthesis_token_opt = this -> left_parenthesis_token_opt;
    clone -> type_opt = (Ast *) (this -> type_opt ? this -> type_opt -> Clone(ast_pool) : NULL);
    clone -> AllocateBrackets(this -> NumBrackets());
    for (int i = 0; i < this -> NumBrackets(); i++)
        clone -> AddBrackets((AstBrackets *) this -> Brackets(i) -> Clone(ast_pool));
    clone -> right_parenthesis_token_opt = this -> right_parenthesis_token_opt;
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);

    return clone;
}

Ast *AstBinaryExpression::Clone(StoragePool *ast_pool)
{
    AstBinaryExpression *clone = ast_pool -> GenBinaryExpression(this -> binary_tag);

    clone -> left_expression = (AstExpression *) this -> left_expression -> Clone(ast_pool);
    clone -> binary_operator_token = this -> binary_operator_token;
    clone -> right_expression = (AstExpression *) this -> right_expression -> Clone(ast_pool);

    return clone;
}

Ast *AstConditionalExpression::Clone(StoragePool *ast_pool)
{
    AstConditionalExpression *clone = ast_pool -> GenConditionalExpression();

    clone -> test_expression = (AstExpression *) this -> test_expression -> Clone(ast_pool);
    clone -> question_token = this -> question_token;
    clone -> true_expression = (AstExpression *) this -> true_expression -> Clone(ast_pool);
    clone -> colon_token = this -> colon_token;
    clone -> false_expression = (AstExpression *) this -> false_expression -> Clone(ast_pool);

    return clone;
}

Ast *AstAssignmentExpression::Clone(StoragePool *ast_pool)
{
    AstAssignmentExpression *clone = ast_pool -> GenAssignmentExpression(this -> assignment_tag, this -> assignment_operator_token);

    clone -> left_hand_side = (AstExpression *) this -> left_hand_side -> Clone(ast_pool);
    clone -> expression = (AstExpression *) this -> expression -> Clone(ast_pool);

    return clone;
}

#ifdef TEST
    void Ast::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (Ast):  ";
        cout << "Node number " << (int) kind << " does not contain a print routine\n";
    }

    void AstBlock::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (";
        Unicode::Cout(label_token_opt ? lex_stream.Name(label_token_opt) : L"");
        cout << (label_token_opt ? ": " : "")
             << "Block at level " << nesting_level  
             << ", max_variable_index " << block_symbol -> max_variable_index 
             << ", try_variable_index " << block_symbol -> try_variable_index 
             << ")";

        if (NumStatements() > 0)
        {
            cout << "    {";
            for (int i = 0; i < this -> NumStatements(); i++)
            {
                if (i % 10 == 0)
                    cout << "\n        ";
                cout << " #" << this -> Statement(i) -> id;
            }
            cout << "    }";
            for (int k = 0; k < this -> NumStatements(); k++)
                this -> Statement(k) -> Print(lex_stream);
        }
        cout <<"\n";
    }

    void AstPrimitiveType::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (PrimitiveType):  ";
        Unicode::Cout(lex_stream.Name(primitive_kind_token));
        cout << "\n";
    }

    void AstArrayType::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ArrayType):  "
             << "#" << type -> id;
        for (int i = 0; i < this -> NumBrackets(); i++)
             cout << " []";
        cout << "\n";
        type -> Print(lex_stream);
    }

    void AstSimpleName::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (SimpleName):  ";
        Unicode::Cout(lex_stream.Name(identifier_token));
        cout << "\n";
    }

    void AstPackageDeclaration::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (PackageDeclaration):  ";
        Unicode::Cout(lex_stream.Name(package_token));
        cout << " #" << name -> id << "\n";
        name -> Print(lex_stream);
    }

    void AstImportDeclaration::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ImportDeclaration):  ";
        Unicode::Cout(lex_stream.Name(import_token));
        cout << " #" << name -> id << (star_token_opt ? "." : "");
        Unicode::Cout(star_token_opt ? lex_stream.Name(star_token_opt) : L"");
        cout << "\n";
        name -> Print(lex_stream);
    }

    void AstCompilationUnit::Print(LexStream& lex_stream)
    {
        cout << "\nAST structure for ";
        Unicode::Cout(lex_stream.FileName());
        cout << ":\n\n";
        cout << "#" << this -> id << " (CompilationUnit):  "
             << "#" << (package_declaration_opt ? package_declaration_opt -> id : 0);
        cout << " (";
        for (int i = 0; i < this -> NumImportDeclarations(); i++)
            cout << " #" << this -> ImportDeclaration(i) -> id;
        cout << " ) (";
        for (int k = 0; k < this -> NumTypeDeclarations(); k++)
            cout << " #" << this -> TypeDeclaration(k) -> id;
        cout << ")\n";

        if (package_declaration_opt)
            package_declaration_opt -> Print(lex_stream);
        for (int m = 0; m < this -> NumImportDeclarations(); m++)
            this -> ImportDeclaration(m) -> Print(lex_stream);
        for (int n = 0; n < this -> NumTypeDeclarations(); n++)
            this -> TypeDeclaration(n) -> Print(lex_stream);
    }

    void AstModifier::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (Modifier):  ";
        Unicode::Cout(lex_stream.Name(modifier_kind_token));
        cout << "\n";
    }

    void AstEmptyDeclaration::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (EmptyDeclaration):  ";
        Unicode::Cout(lex_stream.Name(semicolon_token));
        cout << "\n";
    }

    void AstClassBody::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ClassBody):  ";
        cout << "\n    {";
        for (int i = 0; i < this -> NumClassBodyDeclarations(); i++)
        {
            if (i % 10 == 0)
                 cout << "\n       ";
            cout << " #" << this -> ClassBodyDeclaration(i) -> id;
        }
        cout << "\n    }\n";

        for (int k = 0; k < this -> NumClassBodyDeclarations(); k++)
            this -> ClassBodyDeclaration(k) -> Print(lex_stream);
    }

    void AstClassDeclaration::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ClassDeclaration):  ";
        for (int i = 0; i < this -> NumClassModifiers(); i++)
        {
            Unicode::Cout(lex_stream.Name(this -> ClassModifier(i) -> modifier_kind_token));
            cout << " ";
        }
        Unicode::Cout(lex_stream.Name(class_token));
        cout << " ";
        Unicode::Cout(lex_stream.Name(identifier_token));
        cout << " #" << (super_opt ? super_opt -> id : 0);
        cout << "(";
        for (int j = 0; j < NumInterfaces(); j++)
            cout << " #" << this -> Interface(j) -> id;
        cout << ") #" << class_body -> id << "\n";

        if (super_opt)
            super_opt -> Print(lex_stream);
        for (int k = 0; k < NumInterfaces(); k++)
            this -> Interface(k) -> Print(lex_stream);
        class_body -> Print(lex_stream);
    }

    void AstArrayInitializer::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ArrayInitializer):  ";
        cout << "\n    {";
        for (int i = 0; i < NumVariableInitializers(); i++)
        {
            if (i % 10 == 0)
                 cout << "\n       ";
            cout << " #" << this -> VariableInitializer(i) -> id;
        }
        cout << "\n    }\n";

        for (int k = 0; k < NumVariableInitializers(); k++)
            this -> VariableInitializer(k) -> Print(lex_stream);
    }

    void AstBrackets::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (Brackets):  []" << "\n";
    }

    void AstVariableDeclaratorId::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (VariableDeclaratorId):  ";
        Unicode::Cout(lex_stream.Name(identifier_token));
        for (int i = 0; i < NumBrackets(); i++)
             cout << " []";
        cout << "\n";
    }

    void AstVariableDeclarator::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (VariableDeclarator):  " << "#" << variable_declarator_name -> id << " #" <<
                (variable_initializer_opt ? variable_initializer_opt -> id : 0) << "\n";
        variable_declarator_name -> Print(lex_stream);
        if (variable_initializer_opt)
            variable_initializer_opt -> Print(lex_stream);
        
    }

    void AstFieldDeclaration::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (FieldDeclaration):  ";
        for (int i = 0; i < this -> NumVariableModifiers(); i++)
        {
            Unicode::Cout(lex_stream.Name(this -> VariableModifier(i) -> modifier_kind_token));
            cout << " ";
        }
        cout << " #" << type -> id;
        cout << "(";
        for (int j = 0; j < this -> NumVariableDeclarators(); j++)
            cout << " #" << this -> VariableDeclarator(j) -> id;
        cout << ") \n";

        type -> Print(lex_stream);
        for (int k = 0; k < this -> NumVariableDeclarators(); k++)
            this -> VariableDeclarator(k) -> Print(lex_stream);
    }

    void AstFormalParameter::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (FormalParameter):  ";
        for (int i = 0; i < this -> NumParameterModifiers(); i++)
        {
            Unicode::Cout(lex_stream.Name(this -> ParameterModifier(i) -> modifier_kind_token));
            cout << " ";
        }
        cout << "#" << type -> id
             << " #" << variable_declarator_name -> id << "\n";
        type -> Print(lex_stream);
        variable_declarator_name -> Print(lex_stream);
    }

    void AstMethodDeclarator::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (MethodDeclarator):  ";
        Unicode::Cout(lex_stream.Name(identifier_token));
        cout << " (";
        for (int k = 0; k < this -> NumFormalParameters(); k++)
            cout << " #" << this -> FormalParameter(k) -> id; 
        cout << " )";
        for (int i = 0; i < NumBrackets(); i++)
             cout << " []";
        cout <<  "\n";

        for (int j = 0; j < this -> NumFormalParameters(); j++)
            this -> FormalParameter(j) -> Print(lex_stream);
    }

    void AstMethodDeclaration::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (MethodDeclaration):  ";
        for (int i = 0; i < this -> NumMethodModifiers(); i++)
        {
            Unicode::Cout(lex_stream.Name(this -> MethodModifier(i) -> modifier_kind_token));
            cout << " ";
        }
        cout << " #" << type -> id
             << " #" << method_declarator -> id;
        cout << " throws: (";
        for (int j = 0; j < this -> NumThrows(); j++)
            cout << " #" << this -> Throw(j) -> id;
        cout << ") #" << method_body -> id << "\n";

        type -> Print(lex_stream);
        method_declarator -> Print(lex_stream);
        for (int k = 0; k < this -> NumThrows(); k++)
            this -> Throw(k) -> Print(lex_stream);
        method_body -> Print(lex_stream);
    }

    void AstStaticInitializer::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (StaticInitializer):  ";
        Unicode::Cout(lex_stream.Name(static_token));
        cout << " #" << block -> id << "\n";
        block -> Print(lex_stream);
    }

    void AstThisCall::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ThisCall):  ";
        Unicode::Cout(lex_stream.Name(this_token));
        cout << " (";
        for (int i = 0; i < this -> NumArguments(); i++)
            cout << " #" << this -> Argument(i) -> id;
        cout << " ) \n";
        
        for (int j = 0; j < NumArguments(); j++)
            this -> Argument(j) -> Print(lex_stream);
    }

    void AstSuperCall::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (SuperCall):  ";
        if (base_opt)
        {
            cout << "#" << base_opt -> id;
            Unicode::Cout(lex_stream.Name(dot_token_opt));
        }
        Unicode::Cout(lex_stream.Name(super_token));
        cout << " (";
        for (int i = 0; i < this -> NumArguments(); i++)
            cout << " #" << this -> Argument(i) -> id;
        cout << " ) \n";
        base_opt -> Print(lex_stream);

        for (int j = 0; j < NumArguments(); j++)
            this -> Argument(j) -> Print(lex_stream);
    }

    void AstConstructorBlock::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ConstructorBlock):  ";
        if (explicit_constructor_invocation_opt)
             cout << " #" << explicit_constructor_invocation_opt -> id;
        else cout << " #0";
        cout << " #" << block -> id
             << "\n";

        if (explicit_constructor_invocation_opt)
            explicit_constructor_invocation_opt -> Print(lex_stream);
        block -> Print(lex_stream);
    }

    void AstConstructorDeclaration::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ConstructorDeclaration):  ";
        for (int i = 0; i < this -> NumConstructorModifiers(); i++)
        {
            Unicode::Cout(lex_stream.Name(this -> ConstructorModifier(i) -> modifier_kind_token));
            cout << " ";
        }
        cout << " #" << constructor_declarator -> id;
        cout << " throws: (";
        for (int j = 0; j < this -> NumThrows(); j++)
            cout << " #" << this -> Throw(j) -> id;
        cout << ") #" << constructor_body -> id
             << "\n";

        constructor_declarator -> Print(lex_stream);
        for (int k = 0; k < this -> NumThrows(); k++)
            this -> Throw(k) -> Print(lex_stream);
        constructor_body -> Print(lex_stream);
    }

    void AstInterfaceDeclaration::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (InterfaceDeclaration):  ";
        for (int i = 0; i < this -> NumInterfaceModifiers(); i++)
        {
            Unicode::Cout(lex_stream.Name(this -> InterfaceModifier(i) -> modifier_kind_token));
            cout << " ";
        }
        Unicode::Cout(lex_stream.Name(interface_token));
        cout << " ";
        Unicode::Cout(lex_stream.Name(identifier_token));
        cout << "(";
        for (int j = 0; j < NumExtendsInterfaces(); j++)
            cout << " #" << this -> ExtendsInterface(j) -> id;
        cout << ") {";
        for (int m = 0; m < NumInterfaceMemberDeclarations(); m++)
            cout << " #" << this -> InterfaceMemberDeclaration(m) -> id;
        cout << "}\n";

        for (int k = 0; k < NumExtendsInterfaces(); k++)
            this -> ExtendsInterface(k) -> Print(lex_stream);
        for (int n = 0; n < NumInterfaceMemberDeclarations(); n++)
            this -> InterfaceMemberDeclaration(n) -> Print(lex_stream);
    }

    void AstLocalVariableDeclarationStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (LocalVariableDeclarationStatement):  ";
        for (int i = 0; i < this -> NumLocalModifiers(); i++)
        {
            Unicode::Cout(lex_stream.Name(this -> LocalModifier(i) -> modifier_kind_token));
            cout << " ";
        }
        cout << "#" << type -> id;
        cout << "(";
        for (int j = 0; j < this -> NumVariableDeclarators(); j++)
            cout << " #" << this -> VariableDeclarator(j) -> id;
        cout << ") \n";

        type -> Print(lex_stream);
        for (int k = 0; k < this -> NumVariableDeclarators(); k++)
            this -> VariableDeclarator(k) -> Print(lex_stream);
    }

    void AstIfStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (IfStatement):  ";
        Unicode::Cout(lex_stream.Name(if_token));
        cout << " ( #" << expression -> id << " ) #" << true_statement -> id;
        if (false_statement_opt)
            cout << " else #" << false_statement_opt -> id;
        else cout << " #0";
        cout << "\n";

        expression -> Print(lex_stream);
        true_statement -> Print(lex_stream);
        if (false_statement_opt)
            false_statement_opt -> Print(lex_stream);
    }

    void AstEmptyStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (EmptyStatement):  ";
        Unicode::Cout(lex_stream.Name(semicolon_token));
        cout << "\n";
    }

    void AstExpressionStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ExpressionStatement):  " << "#" << expression -> id << "\n";
        expression -> Print(lex_stream);
    }

    void AstCaseLabel::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (CaseLabel):  ";
        Unicode::Cout(lex_stream.Name(case_token));
        cout << " #" << expression -> id << ":\n";
        expression -> Print(lex_stream);
        cout << "    map_index: " << map_index << "\n";
    }

    void AstDefaultLabel::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (DefaultLabel):  ";
        Unicode::Cout(lex_stream.Name(default_token));
        cout << ":\n";
    }

    void AstSwitchBlockStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (SwitchBlockStatement): ";
        for (int i = 0; i < NumSwitchLabels(); i++)
        {
            if (i % 10 == 0)
                 cout << "\n        ";
            cout << " #" << this -> SwitchLabel(i) -> id << ':';
        }
        cout << "\n";
        for (int k = 0; k < NumStatements(); k++)
        {
            if (k % 10 == 0)
                 cout << "\n            ";
            cout << " #" << this -> Statement(k) -> id;
        }
        cout << "\n";

        for (int j = 0; j < NumSwitchLabels(); j++)
            this -> SwitchLabel(j) -> Print(lex_stream);
        for (int l = 0; l < NumStatements(); l++)
            this -> Statement(l) -> Print(lex_stream);
    }

    void AstSwitchStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (SwitchStatement):  ";
        Unicode::Cout(lex_stream.Name(switch_token));
        cout << " ( #" << expression -> id << " ) #" << switch_block -> id << "\n";

        cout << "default case: index " << default_case.index << "\n";
        for (int i = 0; i < map.Length(); i++)
        {
            cout << "map: " << i << "  index: " << map[i] -> index << "  value: " << map[i] -> Value() << "\n";   
        }

        expression -> Print(lex_stream);
        switch_block -> Print(lex_stream);
    }

    void AstWhileStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (WhileStatement):  ";
        Unicode::Cout(lex_stream.Name(while_token));
        cout << " ( #" << expression -> id << " ) #" << statement -> id << "\n";
        expression -> Print(lex_stream);
        statement -> Print(lex_stream);
    }

    void AstDoStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (DoStatement):  ";
        Unicode::Cout(lex_stream.Name(do_token));
        cout << " { #" << statement -> id << " } ";
        Unicode::Cout(lex_stream.Name(while_token));
        cout << " ( #" << expression -> id << " ) #\n";

        statement -> Print(lex_stream);
        expression -> Print(lex_stream);
    }

    void AstForStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ForStatement):  (";
        Unicode::Cout(lex_stream.Name(for_token));
        for (int i = 0; i < this -> NumForInitStatements(); i++)
            cout << " #" << this -> ForInitStatement(i) -> id;
        cout << "; #" << (end_expression_opt ? end_expression_opt -> id : 0) << ";";
        for (int k = 0; k < this -> NumForInitStatements(); k++)
            cout << " #" << this -> ForUpdateStatement(k) -> id;
        cout << ") #" << statement -> id << "\n";

        for (int m = 0; m < this -> NumForInitStatements(); m++)
            this -> ForInitStatement(m) -> Print(lex_stream);
        if (end_expression_opt)
            end_expression_opt -> Print(lex_stream);
        for (int n = 0; n < this -> NumForUpdateStatements(); n++)
            this -> ForUpdateStatement(n) -> Print(lex_stream);
        statement -> Print(lex_stream);
    }

    void AstBreakStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (BreakStatement):  ";
        Unicode::Cout(lex_stream.Name(break_token));
        cout << " ";
        Unicode::Cout(identifier_token_opt ? lex_stream.Name(identifier_token_opt) : L"");
        cout << " at nesting_level " << nesting_level << "\n";
    }

    void AstContinueStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ContinueStatement):  ";
        Unicode::Cout(lex_stream.Name(continue_token));
        cout << " ";
        Unicode::Cout(identifier_token_opt ? lex_stream.Name(identifier_token_opt) : L"");
        cout << " at nesting_level " << nesting_level << "\n";
    }

    void AstReturnStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ReturnStatement):  ";
        Unicode::Cout(lex_stream.Name(return_token));
        cout << " "
             << " #" << (expression_opt ? expression_opt -> id : 0) << "\n";
        if (expression_opt)
            expression_opt -> Print(lex_stream);
    }

    void AstThrowStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ThrowStatement):  ";
        Unicode::Cout(lex_stream.Name(throw_token));
        cout << " "
             << " #" << expression -> id << "\n";
        expression -> Print(lex_stream);
    }

    void AstSynchronizedStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (SynchronizedStatement):  ";
        Unicode::Cout(lex_stream.Name(synchronized_token));
        cout << " ( #" << expression -> id
             << " ) #" << block -> id << "\n";
        expression -> Print(lex_stream);
        block -> Print(lex_stream);
    }

    void AstCatchClause::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (CatchClause):  ";
        Unicode::Cout(lex_stream.Name(catch_token));
        cout << " #" << formal_parameter -> id
             << " #" << block -> id << "\n";
        formal_parameter -> Print(lex_stream);
        block -> Print(lex_stream);
    }

    void AstFinallyClause::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (FinallyClause):  ";
        Unicode::Cout(lex_stream.Name(finally_token));
        cout << " #" << block -> id << "\n";
        block -> Print(lex_stream);
    }

    void AstTryStatement::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (TryStatement):  ";
        Unicode::Cout(lex_stream.Name(try_token));
        cout << " #" << block -> id
             << " catch (";
        for (int i = 0; i < this -> NumCatchClauses(); i++)
            cout << " #" << this -> CatchClause(i) -> id;
        cout << ") finally " << "#" << (finally_clause_opt ? finally_clause_opt -> id : 0) << "\n";

        block -> Print(lex_stream);
        for (int k = 0; k < this -> NumCatchClauses(); k++)
            this -> CatchClause(k) -> Print(lex_stream);
        if (finally_clause_opt)
            finally_clause_opt -> Print(lex_stream);
    }

    void AstIntegerLiteral::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (IntegerLiteral):  ";
        Unicode::Cout(lex_stream.Name(integer_literal_token));
        cout << "\n";
    }

    void AstLongLiteral::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (LongLiteral):  ";
        Unicode::Cout(lex_stream.Name(long_literal_token));
        cout << "\n";
    }

    void AstFloatingPointLiteral::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (FloatingPointLiteral):  ";
        Unicode::Cout(lex_stream.Name(floating_point_literal_token));
        cout << "\n";
    }

    void AstDoubleLiteral::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (DoubleLiteral):  ";
        Unicode::Cout(lex_stream.Name(double_literal_token));
        cout << "\n";
    }

    void AstTrueLiteral::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (TrueLiteral):  ";
        Unicode::Cout(lex_stream.Name(true_literal_token));
        cout << "\n";
    }

    void AstFalseLiteral::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (FalseLiteral):  ";
        Unicode::Cout(lex_stream.Name(false_literal_token));
        cout << "\n";
    }

    void AstStringLiteral::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (StringLiteral):  ";
        Unicode::Cout(lex_stream.Name(string_literal_token));
        cout << "\n";
    }

    void AstCharacterLiteral::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (CharacterLiteral):  ";
        Unicode::Cout(lex_stream.Name(character_literal_token));
        cout << "\n";
    }

    void AstNullLiteral::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (NullLiteral):  ";
        Unicode::Cout(lex_stream.Name(null_token));
        cout << "\n";
    }

    void AstThisExpression::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ThisExpression):  ";
        Unicode::Cout(lex_stream.Name(this_token));
        cout << "\n";
    }

    void AstSuperExpression::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (SuperExpression):  ";
        Unicode::Cout(lex_stream.Name(super_token));
        cout << "\n";
    }

    void AstParenthesizedExpression::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ParenthesizedExpression):  ";
        Unicode::Cout(lex_stream.Name(left_parenthesis_token));
        cout << "#" << expression -> id;
        Unicode::Cout(lex_stream.Name(right_parenthesis_token));
        cout << "\n";
    }

    void AstTypeExpression::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (TypeExpression):  "
             << " #" << type -> id << "\n";
        type -> Print(lex_stream);
    }

    void AstClassInstanceCreationExpression::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ClassInstanceCreationExpression):  ";
        if (dot_token_opt)
        {
            cout << "#" << base_opt -> id;
            Unicode::Cout(lex_stream.Name(dot_token_opt));
        }
        Unicode::Cout(lex_stream.Name(new_token));
        cout << " #" << class_type -> id;
        cout << " (";
        for (int i = 0; i < this -> NumArguments(); i++)
            cout << " #" << this -> Argument(i) -> id;
        cout << " ) \n";
        cout << "#" << (class_body_opt ? class_body_opt -> id : 0) << "\n";

        if (dot_token_opt /* base_opt - see ast.h for explanation */)
            base_opt -> Print(lex_stream);
        class_type -> Print(lex_stream);
        for (int j = 0; j < NumArguments(); j++)
            this -> Argument(j) -> Print(lex_stream);
        if (class_body_opt)
            class_body_opt -> Print(lex_stream);
    }

    void AstDimExpr::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (DimExpr):  [ #" << expression -> id << " ]\n";
        expression -> Print(lex_stream);
    }

    void AstArrayCreationExpression::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ArrayCreationExpression):  ";
        Unicode::Cout(lex_stream.Name(new_token));
        cout << " #" << array_type -> id;
        for (int i = 0; i < NumDimExprs(); i++)
            cout << " [#" << DimExpr(i) -> id << "]";
        for (int k = 0; k < NumBrackets(); k++)
             cout << " []";
        cout << " ";
        cout << "#" << (array_initializer_opt ? array_initializer_opt -> id : 0) << "\n";

        array_type -> Print(lex_stream);
        for (int j = 0; j < this -> NumDimExprs(); j++)
            DimExpr(j) -> Print(lex_stream);
    }

    void AstFieldAccess::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (FieldAccess):  "
             << " #" << base -> id << " ";
        Unicode::Cout(lex_stream.Name(identifier_token));
        cout << "\n";

        base -> Print(lex_stream);
    }

    void AstMethodInvocation::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (MethodInvocation):  "
             << "#" << method -> id;
        cout << " (";
        for (int i = 0; i < this -> NumArguments(); i++)
            cout << " #" << this -> Argument(i) -> id;
        cout << " ) \n";
        
        method -> Print(lex_stream);
        for (int j = 0; j < NumArguments(); j++)
            this -> Argument(j) -> Print(lex_stream);
    }

    void AstArrayAccess::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ArrayAccess):  "
             << "#" << base -> id
             << " [ #" << expression -> id << " ]\n";

        base -> Print(lex_stream);
        expression -> Print(lex_stream);
    }

    void AstPostUnaryExpression::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (PostUnaryExpression):  "
             << "#" << expression -> id;
        Unicode::Cout(lex_stream.Name(post_operator_token));
        cout << "\n";

        expression -> Print(lex_stream);
    }

    void AstPreUnaryExpression::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (PreUnaryExpression):  ";
        Unicode::Cout(lex_stream.Name(pre_operator_token));
        cout << " #" << expression -> id << "\n";

        expression -> Print(lex_stream);
    }

    void AstCastExpression::Print(LexStream& lex_stream)
    {
        if (left_parenthesis_token_opt)
        {
          cout << "#" << this -> id << (kind == CAST ? " (CastExpression: just cast):  " : " (CastExpression: check and cast):  ")
                 << "( #" << (type_opt ? type_opt -> id : 0);
            for (int i = 0; i < NumBrackets(); i++)
                 cout << " []";
            cout << " ) #" << expression -> id << "\n";
            if (type_opt)
                type_opt -> Print(lex_stream);
        }
        else
        {
            cout << "#" << this -> id << " (Java Semantic Cast to " << Type() -> Name()
                 << "):  #" << expression -> id << "\n";
        }

        expression -> Print(lex_stream);
    }

    void AstBinaryExpression::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (BinaryExpression):  "
             << "#" << left_expression -> id << " ";
        Unicode::Cout(lex_stream.Name(binary_operator_token));
        cout << " #" << right_expression -> id << "\n";

        left_expression -> Print(lex_stream);
        right_expression -> Print(lex_stream);
    }

    void AstConditionalExpression::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (ConditionalExpression):  "
             << "#" << test_expression -> id
             << " ? #" << true_expression -> id
             << " : #" << false_expression -> id << "\n";

        test_expression -> Print(lex_stream);
        true_expression -> Print(lex_stream);
        false_expression -> Print(lex_stream);
    }

    void AstAssignmentExpression::Print(LexStream& lex_stream)
    {
        cout << "#" << this -> id << " (AssignmentExpression):  "
             << "#" << left_hand_side -> id << " ";
        Unicode::Cout(lex_stream.Name(assignment_operator_token));
        cout << " #" << expression -> id << "\n";

        left_hand_side -> Print(lex_stream);
        expression -> Print(lex_stream);
    }

#endif

Ast::~Ast()
{
    assert(0);
}

AstStatement::~AstStatement()
{
    assert(0);
}

AstExpression::~AstExpression()
{
    assert(0);
}

AstBlock::~AstBlock()
{
    assert(0);
    //    delete block_statements;
}

AstPrimitiveType::~AstPrimitiveType()
{
    assert(0);
}

AstArrayType::~AstArrayType()
{
    assert(0);
    //    delete type;
    //    delete brackets;
}

AstSimpleName::~AstSimpleName()
{
    assert(0);
    //    delete resolution_opt;
}
 
AstPackageDeclaration::~AstPackageDeclaration()
{
    assert(0);
    //    delete name;
}

AstImportDeclaration::~AstImportDeclaration()
{
    assert(0);
    //    delete name;
}

AstCompilationUnit::~AstCompilationUnit()
{
    assert(0);
    //    delete package_declaration_opt;
    //    delete import_declarations;
    //    delete type_declarations;
}

AstModifier::~AstModifier()
{
    assert(0);
}

AstEmptyDeclaration::~AstEmptyDeclaration()
{
    assert(0);
}

AstClassBody::~AstClassBody()
{
    assert(0);
    //    delete default_constructor;
    //    delete instance_variables;
    //    delete class_variables;
    //    delete methods;
    //    delete constructors;
    //    delete static_initializers;
    //    delete inner_classes;
    //    delete inner_interfaces;
    //    delete blocks;
    //    delete class_body_declarations;
    //    delete this_block;
}

AstClassDeclaration::~AstClassDeclaration()
{
    assert(0);
    //    delete class_modifiers;
    //    delete super_opt;
    //    delete interfaces;
    //    delete class_body;
}

AstArrayInitializer::~AstArrayInitializer()
{
    assert(0);
    //    delete variable_initializers;
}

AstBrackets::~AstBrackets()
{
    assert(0);
}

AstVariableDeclaratorId::~AstVariableDeclaratorId()
{
    assert(0);
    //    delete brackets;
}

AstVariableDeclarator::~AstVariableDeclarator()
{
    assert(0);
    //    delete variable_declarator_name;
    //    delete variable_initializer_opt;
}

AstFieldDeclaration::~AstFieldDeclaration()
{
    assert(0);
    //    delete variable_modifiers;
    //    delete type;
    //    delete variable_declarators;
}

AstFormalParameter::~AstFormalParameter()
{
    assert(0);
    //    delete parameter_modifiers;
    //    delete type;
    //    delete variable_declarator_name;
}

AstMethodDeclarator::~AstMethodDeclarator()
{
    assert(0);
    //    delete formal_parameters;
    //    delete brackets;
}

AstMethodDeclaration::~AstMethodDeclaration()
{
    assert(0);
    //    delete method_modifiers;
    //    delete type;
    //    delete method_declarator;
    //    delete throws;
    //    delete method_body;
}

AstStaticInitializer::~AstStaticInitializer()
{
    assert(0);
    //    delete block;
}

AstThisCall::~AstThisCall()
{
    assert(0);
    //    delete arguments;
    //    delete base_opt;
    //    delete local_arguments_opt;
}

AstSuperCall::~AstSuperCall()
{
    assert(0);
    //    delete base_opt;
    //    delete arguments;
    //    delete local_arguments_opt;
}

AstConstructorBlock::~AstConstructorBlock()
{
    assert(0);
    //    delete explicit_constructor_invocation_opt;
    //    delete block;
    //    delete local_init_block;
    //    delete original_constructor_invocation;
}

AstConstructorDeclaration::~AstConstructorDeclaration()
{
    assert(0);
    //    delete constructor_modifiers;
    //    delete constructor_declarator;
    //    delete throws;
    //    delete constructor_body;
}

AstInterfaceDeclaration::~AstInterfaceDeclaration()
{
    assert(0);
    //    delete class_variables;
    //    delete abstract_methods;
    //    delete inner_classes;
    //    delete inner_interfaces;
    //    delete interface_modifiers;
    //    delete extends_interfaces;
    //    delete interface_member_declarations;
}

AstLocalVariableDeclarationStatement::~AstLocalVariableDeclarationStatement()
{
    assert(0);
    //    delete local_modifiers;
    //    delete type;
    //    delete variable_declarators;
}

AstIfStatement::~AstIfStatement()
{
    assert(0);
    //    delete expression;
    //    delete true_statement;
    //    delete false_statement_opt;
}

AstEmptyStatement::~AstEmptyStatement()
{
    assert(0);
}

AstExpressionStatement::~AstExpressionStatement()
{
    assert(0);
    //    delete expression;
}

AstCaseLabel::~AstCaseLabel()
{
    assert(0);
    //    delete expression;
}

AstDefaultLabel::~AstDefaultLabel()
{
    assert(0);
}

AstSwitchBlockStatement::~AstSwitchBlockStatement()
{
    assert(0);
    //    delete switch_labels;
    //    delete block_statements;
}

AstSwitchStatement::~AstSwitchStatement()
{
    assert(0);
    //    delete expression;
    //    delete switch_block;
}

AstWhileStatement::~AstWhileStatement()
{
    assert(0);
    //    delete expression;
    //    delete statement;
}

AstDoStatement::~AstDoStatement()
{
    assert(0);
    //    delete statement;
    //    delete expression;
}

AstForStatement::~AstForStatement()
{
    assert(0);
    //    delete for_init_statements;
    //    delete end_expression_opt;
    //    delete for_update_statements;
    //    delete statement;
}

AstBreakStatement::~AstBreakStatement()
{
    assert(0);
}

AstContinueStatement::~AstContinueStatement()
{
    assert(0);
}

AstReturnStatement::~AstReturnStatement()
{
    assert(0);
    //    delete expression_opt;
}

AstThrowStatement::~AstThrowStatement()
{
    assert(0);
    //    delete expression;
}

AstSynchronizedStatement::~AstSynchronizedStatement()
{
    assert(0);
    //    delete expression;
    //    delete block;
}

AstCatchClause::~AstCatchClause()
{
    assert(0);
    //    delete formal_parameter;
    //    delete block;
}

AstFinallyClause::~AstFinallyClause()
{
    assert(0);
    //    delete block;
}

AstTryStatement::~AstTryStatement()
{
    assert(0);
    //    delete block;
    //    delete catch_clauses;
    //    delete finally_clause_opt;
}

AstIntegerLiteral::~AstIntegerLiteral()
{
    assert(0);
}

AstLongLiteral::~AstLongLiteral()
{
    assert(0);
}

AstFloatingPointLiteral::~AstFloatingPointLiteral()
{
    assert(0);
}

AstDoubleLiteral::~AstDoubleLiteral()
{
    assert(0);
}

AstTrueLiteral::~AstTrueLiteral()
{
    assert(0);
}

AstFalseLiteral::~AstFalseLiteral()
{
    assert(0);
}

AstStringLiteral::~AstStringLiteral()
{
    assert(0);
}

AstCharacterLiteral::~AstCharacterLiteral()
{
    assert(0);
}

AstNullLiteral::~AstNullLiteral()
{
    assert(0);
}

AstThisExpression::~AstThisExpression()
{
    assert(0);
}

AstSuperExpression::~AstSuperExpression()
{
    assert(0);
}

AstParenthesizedExpression::~AstParenthesizedExpression()
{
    assert(0);
    //    delete expression;
}

AstTypeExpression::~AstTypeExpression()
{
    assert(0);
    //    delete type;
}

AstClassInstanceCreationExpression::~AstClassInstanceCreationExpression()
{
    assert(0);
    //    delete base_opt;
    //    delete class_type;
    //    delete arguments;
    //    delete class_body_opt;
    //    delete local_arguments_opt;
}

AstDimExpr::~AstDimExpr()
{
    assert(0);
    //    delete expression;
}

AstArrayCreationExpression::~AstArrayCreationExpression()
{
    assert(0);
    //    delete array_type;
    //    delete dim_exprs;
    //    delete brackets;
    //    delete array_initializer_opt;
}

AstFieldAccess::~AstFieldAccess()
{
    assert(0);
    //    delete base;
    //    delete resolution_opt;
}

AstMethodInvocation::~AstMethodInvocation()
{
    assert(0);
    //    delete method;
    //    delete arguments;
}

AstArrayAccess::~AstArrayAccess()
{
    assert(0);
    //    delete base;
    //    delete expression;
}

AstPostUnaryExpression::~AstPostUnaryExpression()
{
    assert(0);
    //    delete expression;
}

AstPreUnaryExpression::~AstPreUnaryExpression()
{
    assert(0);
    //    delete expression;
}

AstCastExpression::~AstCastExpression()
{
    assert(0);
    //    delete type_opt;
    //    delete brackets;
    //    delete expression;
}

AstBinaryExpression::~AstBinaryExpression()
{
   assert(0);
   //    delete left_expression;
   //    delete right_expression;
}

AstConditionalExpression::~AstConditionalExpression()
{
    assert(0);
    //    delete test_expression;
    //    delete true_expression;
    //    delete false_expression;
}

AstAssignmentExpression::~AstAssignmentExpression()
{
    assert(0);
    //    delete left_hand_side;
    //    delete expression;
}
