// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1983, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class Parser extends exprprs implements exprsym
{
    final static int STACK_INCREMENT = 128;

    LexStream lex_stream;

    exprhdr actions = new exprhdr(this);

    int stack_length = 0,
        state_stack_top,
        stack[],
        location_stack[];
    Ast parse_stack[];

    //
    // Given a rule of the form     A ::= x1 x2 ... xn     n > 0
    //
    // the function TOKEN(i) yields the symbol xi, if xi is a terminal
    // or ti, if xi is a nonterminal that produced a string of the form
    // xi => ti w.
    //
    final int TOKEN(int i)
    {
        return location_stack[state_stack_top + (i - 1)];
    }

    //
    // Given a rule of the form     A ::= x1 x2 ... xn     n > 0
    //
    // The function SYM(i) yields the AST subtree associated with symbol
    // xi. NOTE that if xi is a terminal, SYM(i) is undefined ! (However,
    // see token_action below.)
    //
    // setSYM1(Ast ast) is a function that allows us to assign an AST
    // tree to SYM(1).
    //
    final Ast SYM(int i) { return parse_stack[state_stack_top + (i - 1)]; }
    final void setSYM1(Ast ast) { parse_stack[state_stack_top] = ast; }

    //
    // When a token is shifted, we may wish to perform an action on 
    // it. One possibility is to invoke "setSYM(null)" to associate
    // a null subtree with this terminal symbol.
    //
    void token_action(int tok) {}

    Parser(LexStream lex_stream)
    {
        this.lex_stream = lex_stream;
    }

    void reallocate_stacks()
    {
        int old_stack_length = (stack == null ? 0 : stack.length);
        int old_stack[] = stack; 
        stack = new int[old_stack_length + STACK_INCREMENT];
        for (int i = 0; i < old_stack_length; i++)
             stack[i] = old_stack[i];

        old_stack = location_stack; 
        location_stack = new int[old_stack_length + STACK_INCREMENT];
        for (int i = 0; i < old_stack_length; i++)
             location_stack[i] = old_stack[i];

        Ast old_parse_stack[] = parse_stack; 
        parse_stack = new Ast[old_stack_length + STACK_INCREMENT];
        for (int i = 0; i < old_stack_length; i++)
             parse_stack[i] = old_parse_stack[i];

        return;
    }
 
    Ast parse()
    {
        lex_stream.Reset();
        int curtok = lex_stream.Gettoken(),
            act = START_STATE,
            current_kind = lex_stream.Kind(curtok);
 
    /*****************************************************************/
    /* Start parsing.                                                */
    /*****************************************************************/
        state_stack_top = -1;

        ProcessTerminals: for (;;)
        {
            if (++state_stack_top >= stack_length)
                reallocate_stacks();
 
            stack[state_stack_top] = act;

            location_stack[state_stack_top] = curtok;

            act = t_action(act, current_kind, lex_stream);
 
            if (act <= NUM_RULES)
                state_stack_top--; // make reduction look like a shift-reduce
            else if (act > ERROR_ACTION)
            {
                token_action(curtok);
                curtok = lex_stream.Gettoken();
                current_kind = lex_stream.Kind(curtok);

                act -= ERROR_ACTION;
            }
            else if (act < ACCEPT_ACTION)
            {
                token_action(curtok);
                curtok = lex_stream.Gettoken();
                current_kind = lex_stream.Kind(curtok);

                continue ProcessTerminals;
            }
            else break ProcessTerminals;

            ProcessNonTerminals: do
            {
                state_stack_top -= (rhs[act] - 1);
                actions.rule_action[act].action();
                act = nt_action(stack[state_stack_top], lhs[act]);
            } while(act <= NUM_RULES);
        }

        if (act == ERROR_ACTION)
        {
            //
            // Recover or Scream or Whatever !!!
            //
            System.out.println("Error detected on token " + curtok);
            return null;
        }

        return parse_stack[0];
    }
}
