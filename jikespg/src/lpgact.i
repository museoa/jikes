#line 57 "jikespg.g"
#define SYM1 terminal[stack_top + 1]
#define SYM2 terminal[stack_top + 2]
#define SYM3 terminal[stack_top + 3]

static void null_action(void)
{
    return;
}

static void add_macro_definition(char *name, struct terminal_type *term)
{
    if (num_defs >= (int)defelmt_size)
    {
        defelmt_size += DEFELMT_INCREMENT;
        defelmt = (struct defelmt_type *)
            (defelmt == (struct defelmt_type *) NULL
             ? malloc(defelmt_size * sizeof(struct defelmt_type))
             : realloc(defelmt, defelmt_size * sizeof(struct defelmt_type)));
        if (defelmt == (struct defelmt_type *) NULL)
            nospace(__FILE__, __LINE__);
    }

    defelmt[num_defs].length       = (*term).length;
    defelmt[num_defs].start_line   = (*term).start_line;
    defelmt[num_defs].start_column = (*term).start_column;
    defelmt[num_defs].end_line     = (*term).end_line;
    defelmt[num_defs].end_column   = (*term).end_column;
    strcpy(defelmt[num_defs].name, name);
    num_defs++;

    return;
}

static void add_block_definition(struct terminal_type *term)
{
    if (num_acts >= (int) actelmt_size)
    {
        actelmt_size += ACTELMT_INCREMENT;
        actelmt = (struct actelmt_type *)
            (actelmt == (struct actelmt_type *) NULL
             ? malloc(actelmt_size * sizeof(struct actelmt_type))
             : realloc(actelmt, actelmt_size * sizeof(struct actelmt_type)));
        if (actelmt == (struct actelmt_type *) NULL)
            nospace(__FILE__, __LINE__);
    }

    actelmt[num_acts].rule_number  = num_rules;
    actelmt[num_acts].start_line   = (*term).start_line;
    actelmt[num_acts].start_column = (*term).start_column;
    actelmt[num_acts].end_line     = (*term).end_line;
    actelmt[num_acts].end_column   = (*term).end_column;
    actelmt[num_acts].header_block = ((*term).kind == HBLOCK_TK);
    num_acts++;

    return;
}

/* bad_symbol ::= EQUIVALENCE */
#line 128 "jikespg.g"
static void bad_first_symbol(void)
{
    sprintf(msg_line,
            "First symbol: \"%s\" found in file is illegal. "
            "Line %d, column %d",
            SYM1.name, SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* bad_symbol ::= BLOCK */
#line 154 "jikespg.g"
static void act10(void)
{
    sprintf(msg_line,
            "Action block cannot be first object in file. "
            "Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* macro_list ::= macro_name_symbol macro_block */
#line 174 "jikespg.g"
static void act13(void)
{
    if (action_bit)
        add_macro_definition(SYM1.name, &(SYM2));

    return;
}

/* macro_list ::= macro_list macro_name_symbol macro_block */
#line 185 "jikespg.g"
static void act14(void)
{
    if (action_bit)
        add_macro_definition(SYM2.name, &(SYM3));

    return;
}

/* macro_name_symbol ::= SYMBOL */
#line 199 "jikespg.g"
static void act16(void)
{
    sprintf(msg_line,
            "Macro name \"%s\" does not start with the "
            "escape character. Line %d, column %d",
            SYM1.name, SYM1.start_line, SYM1.start_column);
    PRNTWNG(msg_line);

     return;
}

/* macro_name_symbol ::= OR */
#line 213 "jikespg.g"
static void bad_macro_name(void)
{
    sprintf(msg_line,
            "Reserved symbol cannot be used as macro name. "
            "Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* macro_name_symbol ::= BLOCK */
#line 233 "jikespg.g"
static void act21(void)
{
    sprintf(msg_line,
            "Macro name not supplied for macro definition. "
            "Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* macro_name_symbol ::= DEFINE_KEY */
#line 247 "jikespg.g"
static void act22(void)
{
    sprintf(msg_line,
            "Macro keyword misplaced. Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* macro_block ::= OR */
#line 263 "jikespg.g"
static void definition_expected(void)
{
    sprintf(msg_line,
            "Definition block expected where symbol found. "
            "Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* terminal_symbol ::= SYMBOL */
#line 294 "jikespg.g"
static void process_terminal(void)
{
    assign_symbol_no(SYM1.name, OMEGA);

    return;
}

/* terminal_symbol ::= DEFINE_KEY */
#line 308 "jikespg.g"
static void bad_terminal(void)
{
    sprintf(msg_line,
            "Keyword  has been misplaced in Terminal section."
            "  Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* terminal_symbol ::= BLOCK */
#line 324 "jikespg.g"
static void act37(void)
{
    sprintf(msg_line,
            "Misplaced block found in TERMINALS section."
            "  Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* alias_definition ::= alias_lhs produces alias_rhs */
#line 342 "jikespg.g"
static void act39(void)
{
    register int image;
    char tok_string[SYMBOL_SIZE + 1];

    switch(SYM3.kind)
    {
        case EMPTY_SYMBOL_TK:
            image = empty;
            break;

        case SYMBOL_TK:
            assign_symbol_no(SYM3.name, OMEGA);
            image = symbol_image(SYM3.name);
            break;

        case ERROR_SYMBOL_TK:
            if (error_image > num_terminals)
            {
                restore_symbol(tok_string, kerror);
                sprintf(msg_line,
                        "Illegal aliasing to %s prior to its "
                        "definition.  Line %d, column %d",
                        tok_string,
                        SYM3.start_line, SYM3.start_column);
                PRNTERR(msg_line);
                exit(12);
            }
            image = error_image;
            break;

        case EOF_SYMBOL_TK:
            if (eoft_image > num_terminals)
            {
                restore_symbol(tok_string, keoft);
                sprintf(msg_line,
                        "Illegal aliasing to %s prior to its "
                        "definition. Line %d, column %d",
                        tok_string,
                        SYM3.start_line, SYM3.start_column);
                PRNTERR(msg_line);
                exit(12);
            }
            image = eoft_image;
            break;

        case EOL_SYMBOL_TK:
            if (eolt_image == OMEGA)
            {
                sprintf(msg_line,
                        "Illegal aliasing to EOL prior to its "
                        "definition. Line %d, column %d",
                        SYM3.start_line, SYM3.start_column);
                PRNTERR(msg_line);
                exit(12);
            }
            image = eolt_image;
            break;

        default: /* if SYM3.kind == symbol */
            image = symbol_image(SYM3.name);
            break;
    }

    switch(SYM1.kind)
    {
        case SYMBOL_TK:
            if (symbol_image(SYM1.name) != OMEGA)
            {
                restore_symbol(tok_string, SYM1.name);
                sprintf(msg_line,
                        "Symbol %s was previously defined. "
                        "Line %d, column %d", tok_string,
                        SYM1.start_line, SYM1.start_column);
                PRNTERR(msg_line);
                exit(12);
            }
            assign_symbol_no(SYM1.name, image);
            break;

        case ERROR_SYMBOL_TK:
            if (error_image > num_terminals || ! error_maps_bit)
            {
                if (image == empty      || image == eolt_image ||
                    image == eoft_image || image > num_terminals)
                {
                    restore_symbol(tok_string, kerror);
                    sprintf(msg_line,
                            "Illegal alias for symbol %s. "
                            "Line %d, column %d.",
                            tok_string,
                            SYM1.start_line, SYM1.start_column);
                    PRNTERR(msg_line);
                    exit(12);
                }
                alias_map(kerror, image);
                error_image = image;
            }
            else
            {
                restore_symbol(tok_string, kerror);
                sprintf(msg_line,
                        "Symbol %s was previously defined. "
                        "Line %d, column %d",
                        tok_string,
                        SYM1.start_line, SYM1.start_column);
                PRNTERR(msg_line);
                exit(12);
            }
            break;

        case EOF_SYMBOL_TK:
            if (eoft_image > num_terminals)
            {
                if (image == empty       || image == eolt_image  ||
                    image == error_image || image > num_terminals)
                {
                    restore_symbol(tok_string, keoft);
                    sprintf(msg_line,
                            "Illegal alias for symbol %s. "
                            "Line %d, column %d.",
                            tok_string,
                            SYM1.start_line, SYM1.start_column);
                    PRNTERR(msg_line);
                    exit(12);
                }
                alias_map(keoft, image);
                eoft_image = image;
            }
            else
            {
                restore_symbol(tok_string, keoft);
                sprintf(msg_line,
                        "Symbol %s was previously defined. "
                        "Line %d, column %d",
                        tok_string,
                        SYM1.start_line, SYM1.start_column);
                PRNTERR(msg_line);
                exit(12);
            }
            break;

        default: /* if SYM1.kind == EOL_SYMBOL */
            if (eolt_image == OMEGA)
            {
                if (image == empty ||
                    image == eoft_image ||
                    image == error_image ||
                    image > num_terminals)
                {
                    sprintf(msg_line,
                            "Illegal alias for symbol EOL. "
                            "Line %d, column %d.",
                            SYM1.start_line, SYM1.start_column);
                    PRNTERR(msg_line);
                    exit(12);
                }
                eolt_image = image;
            }
            else
            {
                sprintf(msg_line,
                        "Symbol EOL was previously defined. "
                        "Line %d, column %d",
                        SYM1.start_line, SYM1.start_column);
                PRNTERR(msg_line);
                exit(12);
            }
            break;
    }

    return;
}

/* bad_alias_rhs ::= DEFINE_KEY */
#line 550 "jikespg.g"
static void bad_alias_rhs(void)
{
    sprintf(msg_line,
            "Misplaced keyword found in Alias section. "
            "Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* bad_alias_rhs ::= BLOCK */
#line 568 "jikespg.g"
static void act57(void)
{
    sprintf(msg_line,
            "Misplaced block found in Alias section. "
            "Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* bad_alias_lhs ::= EMPTY_SYMBOL */
#line 586 "jikespg.g"
static void act59(void)
{
    sprintf(msg_line,
            "Empty symbol cannot be aliased. "
            "Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* bad_alias_lhs ::= produces */
#line 600 "jikespg.g"
static void missing_quote(void)
{
    sprintf(msg_line,
            "Symbol must be quoted when used as a "
            "grammar symbol. Line %d, column %d",
            ormark,
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* start_symbol ::= SYMBOL */
#line 622 "jikespg.g"
/*********************************************************************/
/*********************************************************************/
static void act63(void)
{
    register struct node *q;

    assign_symbol_no(SYM1.name, OMEGA);
    q = Allocate_node();
    q -> value = symbol_image(SYM1.name);
    if (start_symbol_root == NULL)
        q -> next = q;
    else
    {
        q -> next = start_symbol_root -> next;
        start_symbol_root -> next = q;
    }
    start_symbol_root = q;
    num_rules++;
    num_items++;
    SHORT_CHECK(num_items);

    return;
}

/* start_symbol ::= OR */
#line 649 "jikespg.g"
static void bad_start_symbol(void)
{
    sprintf(msg_line,
            "Symbol cannot be used as Start symbol. "
            "Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* start_symbol ::= BLOCK */
#line 669 "jikespg.g"
static void act68(void)
{
    sprintf(msg_line,
            "Misplaced block found in Start section. "
            "Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* start_symbol ::= DEFINE_KEY */
#line 683 "jikespg.g"
static void misplaced_keyword_found_in_START_section(void)
{
    sprintf(msg_line,
            "Misplaced keyword found in START section. "
            "Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* rules_block ::= RULES_KEY */
#line 704 "jikespg.g"
static void act73(void)
{
    register struct node *q;

    if (start_symbol_root == NULL)
    {
        q = Allocate_node();
        q -> value = empty;
        q -> next = q;
        start_symbol_root = q;
        num_rules = 0;                 /* One rule */
        num_items = 0;                 /* 0 items */
    }
    build_symno();

    return;
}

/* rules_block ::= RULES_KEY rule_list */
#line 725 "jikespg.g"
static void act74(void)
{
    build_symno();

    return;
}

/* rule_list ::= {action_block} SYMBOL produces */
#line 741 "jikespg.g"
static void act77(void)
{
    register struct node *q;

    assign_symbol_no(SYM2.name, OMEGA);
    if (start_symbol_root == NULL)
    {
        q = Allocate_node();
        q -> value = symbol_image(SYM2.name);
        q -> next = q;

        start_symbol_root = q;

        num_rules = 1;
        num_items = 1;
    }

/*********************************************************************/
/* Since we don't know for sure how many start symbols we have, a    */
/* "while" loop is used to increment the size of rulehdr. However,   */
/* it is highly unlikely that this loop would ever execute more than */
/* once if the size of RULE_INCREMENT is reasonable.                 */
/*********************************************************************/
    while (num_rules >= (int)rulehdr_size)
    {
        rulehdr_size += RULEHDR_INCREMENT;
        rulehdr = (struct rulehdr_type *)
            (rulehdr == (struct rulehdr_type *) NULL
             ? malloc(rulehdr_size * sizeof(struct rulehdr_type))
             : realloc(rulehdr, rulehdr_size * sizeof(struct rulehdr_type)));
        if (rulehdr == (struct rulehdr_type *) NULL)
            nospace(__FILE__, __LINE__);
    }

    rulehdr[num_rules].sp = ((SYM3.kind == ARROW_TK) ? TRUE : FALSE);
    rulehdr[num_rules].lhs = symbol_image(SYM2.name);
    rulehdr[num_rules].rhs_root = NULL;

    return;
}

/* rule_list ::= rule_list OR */
#line 786 "jikespg.g"
static void act78(void)
{
    num_rules++;
    if (num_rules >= (int)rulehdr_size)
    {
        rulehdr_size += RULEHDR_INCREMENT;
        rulehdr = (struct rulehdr_type *)
            (rulehdr == (struct rulehdr_type *) NULL
             ? malloc(rulehdr_size * sizeof(struct rulehdr_type))
             : realloc(rulehdr, rulehdr_size * sizeof(struct rulehdr_type)));
        if (rulehdr == (struct rulehdr_type *) NULL)
            nospace(__FILE__, __LINE__);
    }
    rulehdr[num_rules].sp = rulehdr[num_rules - 1].sp;
    rulehdr[num_rules].lhs = OMEGA;
    rulehdr[num_rules].rhs_root = NULL;

    return;
}

/* rule_list ::= rule_list SYMBOL produces */
#line 809 "jikespg.g"
static void act79(void)
{
    num_rules++;
    if (num_rules >= (int)rulehdr_size)
    {
        rulehdr_size += RULEHDR_INCREMENT;
        rulehdr = (struct rulehdr_type *)
            (rulehdr == (struct rulehdr_type *) NULL
             ? malloc(rulehdr_size * sizeof(struct rulehdr_type))
             : realloc(rulehdr, rulehdr_size * sizeof(struct rulehdr_type)));
        if (rulehdr == (struct rulehdr_type *) NULL)
            nospace(__FILE__, __LINE__);
    }
    rulehdr[num_rules].sp = ((SYM3.kind == ARROW_TK) ? TRUE : FALSE);
    assign_symbol_no(SYM2.name, OMEGA);
    rulehdr[num_rules].lhs = symbol_image(SYM2.name);
    rulehdr[num_rules].rhs_root = NULL;

    return;
}

/* rule_list ::= rule_list ERROR_SYMBOL */
#line 838 "jikespg.g"
static void act82(void)
{
    register struct node *q;
    char tok_string[SYMBOL_SIZE + 1];

    if (error_image == DEFAULT_SYMBOL)
    {
        restore_symbol(tok_string, kerror);
        sprintf(msg_line,
                "%s not declared or aliased to terminal "
                "symbol. Line %d, column %d",
                tok_string,
                SYM2.start_line, SYM2.start_column);
        PRNTERR(msg_line);
        exit(12);
    }
    q = Allocate_node();
    q -> value = error_image;
    num_items++;
    SHORT_CHECK(num_items);
    if (rulehdr[num_rules].rhs_root == NULL)
        q -> next = q;
    else
    {
        q -> next = rulehdr[num_rules].rhs_root -> next;
         rulehdr[num_rules].rhs_root -> next = q;
    }
    rulehdr[num_rules].rhs_root = q;

    return;
}

/* rule_list ::= rule_list SYMBOL */
#line 873 "jikespg.g"
static void act83(void)
{
    register int sym;
    register struct node *q;

    assign_symbol_no(SYM2.name, OMEGA);
    sym = symbol_image(SYM2.name);
    if (sym != empty)
    {
        if (sym == eoft_image)
        {
            sprintf(msg_line,
                    "End-of-file symbol cannot be used "
                    "in rule. Line %d, column %d",
                    SYM2.start_line, SYM2.start_column);
            PRNTERR(msg_line);
            exit(12);
        }
        q = Allocate_node();
        q -> value = sym;
        num_items++;
        SHORT_CHECK(num_items);
        if (rulehdr[num_rules].rhs_root == NULL)
            q -> next = q;
        else
        {
            q -> next = rulehdr[num_rules].rhs_root -> next;
            rulehdr[num_rules].rhs_root -> next = q;
        }
        rulehdr[num_rules].rhs_root = q;
    }

    return;
}

/* rule_list ::= OR */
#line 911 "jikespg.g"
static void bad_first_symbol_in_RULES_section(void)
{
    sprintf(msg_line,
            "First symbol in Rules section is not "
            "a valid left-hand side.\n Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* rule_list ::= rule_list OR produces */
#line 931 "jikespg.g"
static void rule_without_left_hand_side(void)
{
    sprintf(msg_line,
            "Rule without left-hand-side.  Line %d, column %d",
            SYM3.start_line, SYM3.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* rule_list ::= rule_list keyword produces */
#line 948 "jikespg.g"
static void act91(void)
{
    sprintf(msg_line,
            "Misplaced keyword found in Rules section "
            "Line %d, column %d",
            SYM2.start_line, SYM2.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* action_block ::= BLOCK */
#line 963 "jikespg.g"
static void act92(void)
{
    if (action_bit)
        add_block_definition(&(SYM1));

    return;
}

/* action_block ::= HBLOCK */
#line 974 "jikespg.g"
static void act93(void)
{
    if (action_bit)
        add_block_definition(&(SYM1));

    return;
}

/* keyword ::= DEFINE_KEY */
#line 986 "jikespg.g"
static void misplaced_keyword_found_in_RULES_section(void)
{
    sprintf(msg_line,
            "Misplaced keyword found in RULES section. "
            "Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* names_definition ::= name produces name */
#line 1012 "jikespg.g"
static void act100(void)
{
    if (error_maps_bit)
    {
        int symbol;

        switch(SYM1.kind)
        {
            case EMPTY_SYMBOL_TK:
                symbol = empty;
                break;

            case ERROR_SYMBOL_TK:
                symbol = error_image;
                break;

            case EOL_SYMBOL_TK:
                symbol = eolt_image;
                break;

            case EOF_SYMBOL_TK:
                symbol = eoft_image;
                break;

            default:
                symbol = symbol_image(SYM1.name);
                break;
        }

        if (symbol == OMEGA)
        {
            sprintf(msg_line,
                    "Symbol %s is undefined. Line %d, column %d",
            SYM1.name, SYM1.start_line, SYM1.start_column);
            PRNTERR(msg_line);

            exit(12);
        }

        if (symno[symbol].name_index != OMEGA)
        {
            sprintf(msg_line,
                    "Symbol %s has been named more than once. "
                    "Line %d, column %d.",
            SYM1.name, SYM1.start_line, SYM1.start_column);
            PRNTERR(msg_line);

            exit(12);
        }
         symno[symbol].name_index = name_map(SYM3.name);
     }

     return;
}

/* bad_name ::= DEFINE_KEY */
#line 1090 "jikespg.g"
static void misplaced_keyword_found_in_NAMES_section(void)
{
    sprintf(msg_line,
            "Keyword  has been misplaced in NAMES section."
            "  Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* bad_name ::= BLOCK */
#line 1114 "jikespg.g"
static void act116(void)
{
    sprintf(msg_line,
            "Misplaced action block found in NAMES "
            "section. Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* bad_name ::= MACRO_NAME */
#line 1128 "jikespg.g"
static void act117(void)
{
    sprintf(msg_line,
            "Misplaced macro name found in NAMES "
            "section. Line %d, column %d",
            SYM1.start_line, SYM1.start_column);
    PRNTERR(msg_line);

    exit(12);
}

/* [terminals_block] ::= */
#line 1150 "jikespg.g"
static void process_TERMINALS_section(void)
{
    num_terminals = num_symbols;
    assign_symbol_no(keoft, OMEGA);
    eoft_image = symbol_image(keoft);

    if (error_maps_bit)
    {
        assign_symbol_no(kerror, OMEGA);
        error_image = symbol_image(kerror);
    }
    else error_image = DEFAULT_SYMBOL;   /* should be 0 */

    assign_symbol_no(kaccept, OMEGA);
    accept_image = symbol_image(kaccept);

    return;
}

/* [alias_block] ::= */
#line 1175 "jikespg.g"
static void process_ALIAS_section(void)
{
    register int i,
                 k;
    register struct hash_type *p;

    k = 0;
    if (eoft_image <= num_terminals)
        k++;
    else
        num_terminals++;

    if (error_maps_bit)
    {
        if (error_image <= num_terminals)
            k++;
        else
        {
            num_terminals++;
            if (k == 1)
                error_image--;
        }
    }

    if (k > 0)
    {
        for (i = 0; i < HT_SIZE; i++)
        {
            p = hash_table[i];
            while(p != NULL)
            {
                if (p -> number > num_terminals)
                    p -> number -= k;
                else if (p -> number < -num_terminals)
                    p -> number += k;
                p = p -> link;
            }
        }
        num_symbols -= k;
        accept_image -= k;
    }
    if (eolt_image == OMEGA)
        eolt_image = eoft_image;
    if (error_image == DEFAULT_SYMBOL)
        alias_map(kerror, DEFAULT_SYMBOL);

    return;
}

/* {terminal_symbol} ::= */
#line 1252 "jikespg.g"
static void act132(void)
{
    assign_symbol_no(kempty, OMEGA);
    empty = symbol_image(kempty);

    return;
}
