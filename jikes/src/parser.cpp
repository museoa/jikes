// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#include "config.h"
#include <assert.h>
#include <iostream.h>
#include "parser.h"
#include "ast.h"

void Parser::ReallocateStacks()
{
    int old_stack_length = stack_length;

    stack_length += STACK_INCREMENT;

    assert(stack_length <= SHRT_MAX);

    int *old_stack = stack;
    stack = (int *) memmove(new int[stack_length], stack, old_stack_length * sizeof(int));
    delete [] old_stack;

    Location *old_location_stack = location_stack;
    location_stack = (Location *) memmove(new Location[stack_length], location_stack, old_stack_length * sizeof(Location));
    delete [] old_location_stack;

    Ast **old_parse_stack = parse_stack;
    parse_stack = (Ast **) memmove(new Ast *[stack_length], parse_stack, old_stack_length * sizeof(Ast *));
    delete [] old_parse_stack;
    parse_stack[old_stack_length] = NULL; // the first time through, we initialize parse_stack[0] to NULL !!!

    int *old_temp_stack = temp_stack;
    temp_stack = (int *) memmove(new int[stack_length], temp_stack, old_stack_length * sizeof(int));
    delete [] old_temp_stack;

    return;
}


AstListNode *Parser::AllocateListNode()
{
    AstListNode *p;

    if (free_list_nodes)
    {
        p = free_list_nodes;
        free_list_nodes = free_list_nodes -> next;
    }
    else p = list_node_pool -> NewListNode();

    return p;
}


void Parser::FreeCircularList(AstListNode *tail)
{
    AstListNode *root = tail -> next;
    tail -> next = free_list_nodes;
    free_list_nodes = root;

    return;
}


AstPackageDeclaration *Parser::PackageHeaderParse(LexStream *lex_stream_, StoragePool *ast_pool_)
{
    AstPackageDeclaration *package_declaration = NULL;

    lex_stream_ -> Reset();

    if (lex_stream_ -> Kind(lex_stream_ -> Peek()) == TK_package)
    {
        ast_pool = ast_pool_;

        parse_package_header_only = true;
        end_token = LexStream::LEX_INFINITY; // We are parsing the whole input and not just a segment.
        lex_stream = lex_stream_;
        Ast *ast = HeaderParse();
        parse_package_header_only = false;

        if (ast)
        {
            AstCompilationUnit *compilation_unit = ast -> CompilationUnitCast();
            if (compilation_unit && (! compilation_unit -> BadCompilationUnitCast()))
                package_declaration = compilation_unit -> package_declaration_opt;
        }
    }

    return package_declaration;
}


AstCompilationUnit *Parser::HeaderParse(LexStream *lex_stream_, StoragePool *ast_pool_)
{
    lex_stream_ -> Reset();

    body_pool = new StoragePool(lex_stream_ -> NumTokens());
    ast_pool = (ast_pool_ ? ast_pool_ : body_pool);
    list_node_pool = new StoragePool(lex_stream_ -> NumTokens());
    free_list_nodes = NULL;
    AstCompilationUnit *compilation_unit = NULL;

    parse_header_only = true;
    end_token = LexStream::LEX_INFINITY; // We are parsing the whole input and not just a segment.
    lex_stream = lex_stream_;
    Ast *ast = HeaderParse();
    parse_header_only = false;

    if (ast)
    {
        compilation_unit = ast -> CompilationUnitCast();
        if (compilation_unit && (! compilation_unit -> BadCompilationUnitCast()))
        {
            if (compilation_unit -> NumTypeDeclarations() == 0)
                compilation_unit -> kind = Ast::EMPTY_COMPILATION;
        }
// STG:
//      else delete ast;
//
    }

    //
    // If we succesfully parsed a compilation unit, allocate a storage pool for it.
    // Subtract the amount of space that's already been allocated for the headers
    // from the estimate for the bodies.
    //
    if (compilation_unit)
         compilation_unit -> ast_pool = body_pool;
    else delete body_pool;

    delete list_node_pool; // free the pool of list nodes

    return compilation_unit;
}


Ast *Parser::HeaderParse()
{
    TokenObject curtok = lex_stream -> Gettoken();
    int act = START_STATE,
              current_kind = lex_stream -> Kind(curtok);

/*****************************************************************/
/* Start parsing.                                                */
/*****************************************************************/
    state_stack_top = -1;

    //
    // Process a terminal
    //
    for (;;)
    {
        if (++state_stack_top >= stack_length)
            ReallocateStacks();

        stack[state_stack_top] = act;
        location_stack[state_stack_top] = Loc(curtok);

        act = t_action(act, current_kind, lex_stream);

        if (act <= NUM_RULES)
            state_stack_top--; // make reduction look like a shift-reduce
        else if (act > ERROR_ACTION)
        {
// STG:
//            token_action(curtok);
            curtok = lex_stream -> Gettoken();
            current_kind = lex_stream -> Kind(curtok);

            act -= ERROR_ACTION;
        }
        else if (act < ACCEPT_ACTION)
        {
// STG:
//            token_action(curtok);
            curtok = lex_stream -> Gettoken();
            current_kind = lex_stream -> Kind(curtok);

            continue;
        }
        else break;

        //
        // Process a non_terminal
        //
        do
        {
            state_stack_top -= (rhs[act] - 1);
            (this ->* rule_action[act])();
            act = nt_action(stack[state_stack_top], lhs[act]);
        } while(act <= NUM_RULES);
    } /* process_terminal */

    if (act == ERROR_ACTION)
    {
        //
        // If any error is found in a package declaration, do not try to repair it.
        //
        if (! parse_package_header_only)
            RepairParse(curtok);

        if (parse_stack[0] && parse_stack[0] -> CompilationUnitCast())
            parse_stack[0] -> kind = Ast::BAD_COMPILATION;
        else
        {
// STG:
//            delete parse_stack[0];
            parse_stack[0] = NULL;
        }
    }

    return parse_stack[0];
}


bool Parser::BodyParse(LexStream *lex_stream_, AstClassBody *class_body)
{
    assert(class_body -> UnparsedClassBodyCast());

    lex_stream = lex_stream_;
    ast_pool = class_body -> pool;
    body_pool = class_body -> pool;
    list_node_pool = new StoragePool(lex_stream_ -> NumTokens());
    free_list_nodes = NULL;

    bool no_errors_detected = Body(class_body);

    delete list_node_pool; // free the pool of list nodes

    class_body -> mark_parsed();

    return no_errors_detected;
}


bool Parser::Body(AstClassBody *class_body)
{
    bool no_errors_detected = true;

    for (int i = 0; i < class_body -> NumConstructors(); i++)
    {
        AstConstructorDeclaration *constructor_decl = class_body -> Constructor(i);

        if (constructor_decl -> constructor_symbol)
        {
            AstConstructorBlock *block = constructor_decl -> constructor_body;
            end_token = block -> right_brace_token; // last token in the body

            AstStatement *new_body = (AstStatement *) ParseSegment(block -> left_brace_token);

            if (! new_body)
                no_errors_detected = false;
            else
            {
                AstConstructorBlock *new_block = new_body -> ConstructorBlockCast();

                if (! new_block)
                {
                    new_block                                        = ast_pool -> GenConstructorBlock();
                    new_block -> left_brace_token                    = new_body -> LeftToken();
                    new_block -> explicit_constructor_invocation_opt = NULL;
                    new_block -> block                               = (AstBlock *) new_body;
                    new_block -> right_brace_token                   = new_body -> RightToken();
                }
// STG:
//                delete block; // destroy old empty block
//
                constructor_decl -> constructor_body = new_block;
            }
        }
    }

    for (int j = 0; j < class_body -> NumMethods(); j++)
    {
        AstMethodDeclaration *method_decl = class_body -> Method(j);

        if (method_decl -> method_symbol)
        {
            AstBlock *block = method_decl -> method_body -> BlockCast();

            if (block)
            {
                end_token = block -> right_brace_token; // last token in the body
                AstStatement *new_block = (AstStatement *) ParseSegment(block -> left_brace_token);

                if (! new_block) // a bad block ?
                    no_errors_detected = false;
                else
                {
                    //
                    // The block associated with the method may syntactically be either a
                    // block or a constructor block. If it is a block, nest it inside the
                    // initial block. If it is a constructor block, replace the initial block
                    // by the constructor block.
                    //
                    if (new_block -> BlockCast())
                        block -> AddStatement(new_block);
                    else
                    {
                        method_decl -> method_body = new_block;
// STG:
//                        delete block; // destroy old empty block
                    }
                }
            }
        }
    }

    for (int k = 0; k < class_body -> NumNestedClasses(); k++)
        no_errors_detected = no_errors_detected && Body(class_body -> NestedClass(k) -> class_body);

    for (int l = 0; l < class_body -> NumNestedInterfaces(); l++)
        no_errors_detected = no_errors_detected && Body(class_body -> NestedInterface(l));

    return no_errors_detected;
}

bool Parser::BodyParse(LexStream *lex_stream_, AstInterfaceDeclaration *interface_declaration)
{
    assert(interface_declaration -> UnparsedInterfaceBodyCast());

    lex_stream = lex_stream_;
    ast_pool = interface_declaration -> pool;
    body_pool = interface_declaration -> pool;
    list_node_pool = new StoragePool(lex_stream_ -> NumTokens());
    free_list_nodes = NULL;

    bool no_errors_detected = Body(interface_declaration);

    delete list_node_pool; // free the pool of list nodes

    interface_declaration -> mark_parsed();

    return no_errors_detected;
}


bool Parser::Body(AstInterfaceDeclaration *interface_declaration)
{
    bool no_errors_detected = true;

    for (int k = 0; k < interface_declaration -> NumNestedClasses(); k++)
        no_errors_detected = no_errors_detected && Body(interface_declaration -> NestedClass(k) -> class_body);

    for (int l = 0; l < interface_declaration -> NumNestedInterfaces(); l++)
        no_errors_detected = no_errors_detected && Body(interface_declaration -> NestedInterface(l));

    return no_errors_detected;
}


bool Parser::InitializerParse(LexStream *lex_stream_, AstClassBody *class_body)
{
    lex_stream = lex_stream_;
    ast_pool = class_body -> pool;
    body_pool = class_body -> pool;
    list_node_pool = new StoragePool(lex_stream_ -> NumTokens());
    free_list_nodes = NULL;

    bool no_errors_detected = Initializer(class_body);

    delete list_node_pool; // free the pool of list nodes

    return no_errors_detected;
}


bool Parser::InitializerParse(LexStream *lex_stream_, AstInterfaceDeclaration *interface_declaration)
{
    lex_stream = lex_stream_;
    ast_pool = interface_declaration -> pool;
    body_pool = interface_declaration -> pool;
    list_node_pool = new StoragePool(lex_stream_ -> NumTokens());
    free_list_nodes = NULL;

    bool no_errors_detected = Initializer(interface_declaration);

    delete list_node_pool; // free the pool of list nodes

    return no_errors_detected;
}


bool Parser::Initializer(AstClassBody *class_body)
{
    bool no_errors_detected = true;

    for (int i = 0; i < class_body -> NumStaticInitializers(); i++)
    {
         AstBlock *block = class_body -> StaticInitializer(i) -> block;
         end_token = block -> right_brace_token; // last token in the body
         class_body -> StaticInitializer(i) -> block = (AstBlock *) ParseSegment(block -> left_brace_token);

        if (! class_body -> StaticInitializer(i) -> block)
        {
            no_errors_detected = false;
            class_body -> StaticInitializer(i) -> block = block; // restore old empty block
        }
// STG:
//      else
//      {
//          delete block; // destroy old empty block
//      }
    }

    for (int j = 0; j < class_body -> NumBlocks(); j++)
    {
         AstBlock *block = class_body -> Block(j);
         end_token = block -> right_brace_token; // last token in the body
         class_body -> Block(j) = (AstBlock *) ParseSegment(block -> left_brace_token);

        if (! class_body -> Block(j))
        {
            no_errors_detected = false;
            class_body -> Block(j) = block; // restore old empty block
        }
//
// STG: if we ever go back to a new/free model of allocating Ast nodes
// This loop needs to be rewritten as otherwise, the new block allocated
// here would cause a memory leak since the class_body_declarations list
// is unaware of it. The solution is to iterate over the class_body_declarations
// list in order to locate the old block, free it and update the class_body_declarations
// list to point to the new block.
//
    }

    for (int k = 0; k < class_body -> NumNestedClasses(); k++)
        no_errors_detected = no_errors_detected && Initializer(class_body -> NestedClass(k) -> class_body);

    for (int l = 0; l < class_body -> NumNestedInterfaces(); l++)
        no_errors_detected = no_errors_detected && Initializer(class_body -> NestedInterface(l));

    return no_errors_detected;
}


bool Parser::Initializer(AstInterfaceDeclaration *interface_declaration)
{
    bool no_errors_detected = true;

    for (int k = 0; k < interface_declaration -> NumNestedClasses(); k++)
        no_errors_detected = no_errors_detected && Initializer(interface_declaration -> NestedClass(k) -> class_body);

    for (int l = 0; l < interface_declaration -> NumNestedInterfaces(); l++)
        no_errors_detected = no_errors_detected && Initializer(interface_declaration -> NestedInterface(l));

    return no_errors_detected;
}


Ast *Parser::ParseSegment(TokenObject start_token)
{
    //
    // The next call to Gettoken will return the start_token.
    // However, we initialize curtok to start_token in order
    // to obtain a valid location for the BodyMarker.
    //
    lex_stream -> Reset(start_token);
    TokenObject curtok = start_token; // get the location of the start_token
    int act = START_STATE,
              current_kind = TK_BodyMarker;

/*****************************************************************/
/* Start parsing.                                                */
/*****************************************************************/
    state_stack_top = -1;

    //
    // Process a terminal
    //
    for (;;)
    {
        if (++state_stack_top >= stack_length)
            ReallocateStacks();

        stack[state_stack_top] = act;
        location_stack[state_stack_top] = Loc(curtok);

        act = t_action(act, current_kind, lex_stream);

        if (act <= NUM_RULES)
            state_stack_top--; // make reduction look like a shift-reduce
        else if (act > ERROR_ACTION)
        {
// STG:
//            token_action(curtok);
            curtok = lex_stream -> Gettoken(end_token);
            current_kind = lex_stream -> Kind(curtok);

            act -= ERROR_ACTION;
        }
        else if (act < ACCEPT_ACTION)
        {
// STG:
//            token_action(curtok);
            curtok = lex_stream -> Gettoken(end_token);
            current_kind = lex_stream -> Kind(curtok);

            continue;
        }
        else break;

        //
        // Process a nonterminal
        //
        do
        {
            state_stack_top -= (rhs[act] - 1);
            (this ->* rule_action[act])();
            act = nt_action(stack[state_stack_top], lhs[act]);
        } while(act <= NUM_RULES);
    } /* process_terminal */

    if (act == ERROR_ACTION)
    {
        RepairParse(curtok);

// STG:
//        delete parse_stack[0];
        parse_stack[0] = NULL;
    }

    return parse_stack[0];
}


void Parser::RepairParse(TokenObject curtok)
{
    //
    // Repair an error
    //
    for (;;)
    {
        //
        // Pop state stack up to first state that had an
        // action on the error token.  The net effect is to
        // remove all default reductions on an empty rule
        // caused by the error token.
        //
        int k;
        for (k = state_stack_top - 1; k >= 0 && location_stack[k] == Loc(curtok); k--)
             ;
        k++;
// STG:
//        for (int i = k; i < state_stack_top; i++)
//        {
//            delete parse_stack[i];
//        }

        state_stack_top = k;

        ErrorRepair(curtok);

        curtok = lex_stream -> Gettoken(end_token);
        int act = stack[state_stack_top--];
        int current_kind = lex_stream -> Kind(curtok);

        //
        // Process a terminal
        //
        for (;;)
        {
            if (++state_stack_top >= stack_length)
                 ReallocateStacks();

            stack[state_stack_top] = act;
            location_stack[state_stack_top] = Loc(curtok);

            act = t_action(act, current_kind, lex_stream);

            if (act <= NUM_RULES)
                state_stack_top--; // make reduction look like a shift-reduce
            else if (act > ERROR_ACTION)
            {
// STG:
//                token_action(curtok);
                curtok = lex_stream -> Gettoken(end_token);
                current_kind = lex_stream -> Kind(curtok);

                act -= ERROR_ACTION;
            }
            else if (act < ACCEPT_ACTION)
            {
// STG:
//                token_action(curtok);
                curtok = lex_stream -> Gettoken(end_token);
                current_kind = lex_stream -> Kind(curtok);

                continue;
            }
            //
            // Since the null string is a valid Java program, this function
            // will always succeed even if it has to delete the whole input.
            //
            else if (act == ACCEPT_ACTION)
                 return;
            else break; // loop around and keep trying until something is accepted.

            //
            // Process a nonterminal
            //
            do
            {
                state_stack_top -= (rhs[act] - 1);
                (this ->* rule_action[act])();
                act = nt_action(stack[state_stack_top], lhs[act]);
            } while(act <= NUM_RULES);
        } /* process_terminal */
    }

    return;
}


//
// This routine is invoked when an error is encountered in a "repair"
// parser. It will place the parser back into a working configuration
// by adjusting the state stack, the current token and the buffer.
//
void Parser::ErrorRepair(TokenObject error_token)
{
    SecondaryRepairInfo repair;

    repair.code = 0;
    do
    {
        repair.distance = 0;
        repair.num_deletions = state_stack_top + BUFF_UBOUND;

        buffer[1] = error_token;
        buffer[0] = lex_stream -> Previous(buffer[1]);

        for (int k = 2; k < BUFF_SIZE; k++)
            buffer[k] = lex_stream -> Next(buffer[k - 1]);

        int last_index;
        for (last_index = MAX_DISTANCE - 1;
             last_index >= 1 && lex_stream -> Kind(buffer[last_index]) == EOFT_SYMBOL;
             last_index--);
        last_index++;

        repair = ErrorSurgery(stack, state_stack_top, last_index, repair);
        error_token = buffer[MAX_DISTANCE - MIN_DISTANCE + 2];
    } while (repair.code == 0);

// STG:
//    for (int i = repair.stack_position; i < state_stack_top; i++)
//    {
//        delete parse_stack[i];
//    }

    state_stack_top = repair.stack_position;
    lex_stream -> Reset(buffer[repair.buffer_position]);

    return;
}


//
// Keep cutting "stuff" out around the error until a stable configuration
// is found.
//
SecondaryRepairInfo Parser::ErrorSurgery
         (int stck[], int stack_top,
          int last_index, SecondaryRepairInfo repair)
{
    int stack_deletions = 0;
    Location  previous_loc = Loc(buffer[2]);

    for (int top = stack_top; top >= 0 && repair.num_deletions >= stack_deletions; top--)
    {
        if (location_stack[top] < previous_loc)
            stack_deletions++;
        previous_loc = location_stack[top];

        for (int i = 1; i <= last_index && repair.num_deletions >= (stack_deletions + i - 1); i++)
        {
            int j = ParseCheck(stck, top, lex_stream -> Kind(buffer[i]), i + 1);

            if ((j - i + 1) > MIN_DISTANCE)
            {
                int k = stack_deletions + i - 1;
                if ((k < repair.num_deletions) ||
                    (j - k) > (repair.distance - repair.num_deletions))
                {
                    repair.code = DELETION_CODE;
                    repair.distance = j;
                    repair.stack_position = top;
                    repair.buffer_position = i;
                    repair.num_deletions = k;
                }
            }
        }
    }

    return repair;
}


/*****************************************************************/
/* Try to parse until first_token and all tokens in BUFFER have  */
/* been consumed, or an error is encountered. Return the number  */
/* of tokens that were expended before the parse blocked.        */
/*****************************************************************/
int Parser::ParseCheck(int stck[], int stack_top, int first_token, int buffer_position)
{
    int max_pos,
        indx,
        ct,
        act,
        lhs_symbol;

/*****************************************************************/
/* Initialize pointer for temp_stack and initialize maximum      */
/* position of state stack that is still useful.                 */
/*****************************************************************/
    act = stck[stack_top];
    if (first_token > NT_OFFSET)
    {
        temp_stack_top = stack_top;
        max_pos = stack_top;
        indx = buffer_position;
        ct = lex_stream -> Kind(buffer[indx]);
        lex_stream -> Reset(lex_stream -> Next(buffer[indx]));
        lhs_symbol = first_token - NT_OFFSET;
        act = nt_action(act, lhs_symbol);
        if (act <= NUM_RULES)
            goto process_non_terminal;
    }
    else
    {
        temp_stack_top = stack_top - 1;
        max_pos = temp_stack_top;
        indx = buffer_position - 1;
        ct = first_token;
        lex_stream -> Reset(buffer[buffer_position]);
    }

    process_terminal: for (;;)
    {
        if (++temp_stack_top >= stack_length)  /* Stack overflow!!! */
            return indx;
        temp_stack[temp_stack_top] = act;

        act = t_action(act, ct, lex_stream);

        if (act <= NUM_RULES)                   /* reduce action */
            temp_stack_top--;
        else if (act < ACCEPT_ACTION ||          /* shift action */
                 act > ERROR_ACTION)        /*shift-reduce action*/
        {
            if (indx == MAX_DISTANCE)
                return indx;
            indx++;
            ct = lex_stream -> Kind(buffer[indx]);
            lex_stream -> Reset(lex_stream -> Next(buffer[indx]));
            if (act > ERROR_ACTION)
                 act -= ERROR_ACTION;
            else goto process_terminal;
        }
        else if (act == ACCEPT_ACTION)           /* accept action */
             return MAX_DISTANCE;
        else return indx;                         /* error action */

        process_non_terminal:
        do
        {
            temp_stack_top -= (rhs[act]-1);
            lhs_symbol = lhs[act];
            act = (temp_stack_top > max_pos
                                  ? temp_stack[temp_stack_top]
                                  : stck[temp_stack_top]);
            act = nt_action(act, lhs_symbol);
        } while(act <= NUM_RULES);

        max_pos = Min(max_pos, temp_stack_top);
    } // process_terminal;

    return 0;
}
