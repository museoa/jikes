//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#ifndef javadef_INCLUDED
#define javadef_INCLUDED

enum {
      ERROR_CODE,
      BEFORE_CODE,
      INSERTION_CODE,
      INVALID_CODE,
      SUBSTITUTION_CODE,
      DELETION_CODE,
      MERGE_CODE,
      MISPLACED_CODE,
      SCOPE_CODE,
      MANUAL_CODE,
      SECONDARY_CODE,
      EOF_CODE,

      ERROR_SYMBOL      = 106,
      MAX_DISTANCE      = 30,
      MIN_DISTANCE      = 3,
      MAX_NAME_LENGTH   = 38,
      MAX_TERM_LENGTH   = 38,
      NUM_STATES        = 551,

      NT_OFFSET         = 107,
      BUFF_UBOUND       = 31,
      BUFF_SIZE         = 32,
      STACK_UBOUND      = 127,
      STACK_SIZE        = 128,
      SCOPE_UBOUND      = 53,
      SCOPE_SIZE        = 54,
      LA_STATE_OFFSET   = 5384,
      MAX_LA            = 2,
      NUM_RULES         = 351,
      NUM_TERMINALS     = 107,
      NUM_NON_TERMINALS = 159,
      NUM_SYMBOLS       = 266,
      START_STATE       = 974,
      EOFT_SYMBOL       = 95,
      EOLT_SYMBOL       = 27,
      ACCEPT_ACTION     = 5032,
      ERROR_ACTION      = 5033
     };


#endif /* javadef_INCLUDED */
