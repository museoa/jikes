// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#ifndef diagnose_INCLUDED
#define diagnose_INCLUDED

#include "platform.h"
#include "parser.h"
#include "jikesapi.h"

#ifdef	HAVE_NAMESPACE
namespace Jikes {	// Open namespace Jikes block
#endif

struct RepairCandidate
{
    int symbol;
    Location location;
};

struct StateInfo
{
    int state,
        next;
};

class ParseErrorInfo: public JikesError
{
    friend class ParseError;
    
 public:
    
    virtual const wchar_t *getErrorMessage();
    virtual const wchar_t *getErrorReport();
    
    virtual JikesErrorSeverity getSeverity();
    virtual const char *getFileName();
    
    virtual int getLeftLineNo      ();
    virtual int getLeftColumnNo    ();
    virtual int getRightLineNo     ();
    virtual int getRightColumnNo   ();

 protected:        

 private:
    
    int left_line_no    ;
    int left_column_no  ;
    int right_line_no   ;
    int right_column_no ;
    
    static bool emacs_style_report;
    LexStream *lex_stream;
    
    void Initialize(LexStream *);

    wchar_t *regularErrorString ();
    wchar_t *emacsErrorString   ();
    
    LexStream::TokenIndex left_token  ;
    LexStream::TokenIndex right_token ;
    
    int                   name_index;   
    int                   right_string_length;
    int                   num;
    unsigned char         msg_level;
    ParseErrorCode        msg_code;
    unsigned              scope_name_index;
};


class ParseError : public javaprs_table
{
 public:

    void Report(int msg_level, ParseErrorCode, int name_index,
                LexStream::TokenIndex left_token, LexStream::TokenIndex right_token,
                int scope_name_index = 0);

    void SortMessages();

    ParseError(Control &control_, LexStream *lex_stream_);
    void PrintMessages();

private:

    Control &control;
    LexStream *lex_stream;

    Tuple<ParseErrorInfo> errors;

    void PrintPrimaryMessage   (int k);
    void PrintSecondaryMessage (int k);
    
};


class DiagnoseParser : public Parser
{
public:

    DiagnoseParser(Control &control_, LexStream *lex_stream_) : next_stack(NULL),
                                                                prev_stack(NULL),
                                                                scope_index(NULL),
                                                                scope_position(NULL),
                                                                state_pool(256),
                                                                error(control_, lex_stream_)
    {
        lex_stream = lex_stream_;
        memset(list, 0, NUM_SYMBOLS * sizeof(int));
        DiagnoseParse();

        return;
    }

    ~DiagnoseParser()
    {
        delete [] next_stack;
        delete [] prev_stack;
        delete [] scope_index;
        delete [] scope_position;
    }

private:

    int next_stack_top,
        *next_stack,

        prev_stack_top,
        *prev_stack,

        scope_stack_top,
        *scope_index,
        *scope_position;

    int list[NUM_SYMBOLS + 1];

    enum { NIL = -1 };
    Tuple<StateInfo> state_pool;
    int *state_seen; // this variable is managed entirely by the function "scope_trial"

    ParseError error;

    void DiagnoseParse();

    void ReallocateStacks();

    RepairCandidate ErrorRecovery(TokenObject error_token);
    RepairCandidate PrimaryPhase(TokenObject error_token);
    int MergeCandidate(int state, int buffer_position);
    PrimaryRepairInfo CheckPrimaryDistance(int stack[],
                                           int stack_top,
                                           PrimaryRepairInfo repair);
    RepairCandidate PrimaryDiagnosis(PrimaryRepairInfo repair);
    int GetTermIndex(int stack[], int stack_top,
                     int tok, int buffer_position);
    int GetNtermIndex(int start, int sym, int buffer_position);
    int Misspell(int sym, TokenObject tok);
    RepairCandidate SecondaryPhase(TokenObject error_token);
    SecondaryRepairInfo MisplacementRecovery
             (int stack[],
              int stack_top,
              int last_index,
              SecondaryRepairInfo misplaced, bool stack_flag);
    SecondaryRepairInfo SecondaryRecovery
             (int stack[],
              int stack_top,
              int last_index,
              SecondaryRepairInfo repair, bool stack_flag);
    void SecondaryDiagnosis(SecondaryRepairInfo repair);

    void RepairParse(TokenObject);

    PrimaryRepairInfo ScopeTrial(int stack[], int stack_top,
                                 PrimaryRepairInfo repair);
    void ScopeTrialCheck(int stack[], int stack_top,
                         PrimaryRepairInfo& repair, int indx);
    bool SecondaryCheck(int stack[], int stack_top,
                        int buffer_position, int distance);
};

#ifdef	HAVE_NAMESPACE
}			// Close namespace Jikes block
#endif

#endif

