// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#ifndef code_INCLUDED
#define code_INCLUDED

#include "config.h"
#include <ctype.h>
#include <assert.h>
#include "bool.h"

class Code
{
    //
    // To facilitate the scanning, the character set is partitioned into
    // 8 classes using the array CODE. The classes are described below
    // together with some self-explanatory functions defined on CODE.
    // 
    enum {
             LOG_BASE_SIZE       = 9,
             LOG_COMPLEMENT_SIZE = 7,
             BASE_SIZE           = 512,
             SLOT_SIZE           = 128,
             SLOT_MASK           = 127,

             NEWLINE_CODE        = 1, // \n, \r
             SPACE_CODE          = 2, // \t, \v, \f, ' '
             BAD_CODE            = 3, // Everything not covered by other codes ...
             DIGIT_CODE          = 4, // '0'..'9'
             OTHER_DIGIT_CODE    = 5, // all unicode digits other than '0'..'9'
             LOWER_CODE          = 6, // 'a'..'z'
             UPPER_CODE          = 7, // 'A'..'Z'
             OTHER_LETTER_CODE   = 8  // '$', '_', all other unicode Letters
         };

    static char code[18048];
    static char *base[512];

#ifdef EBCDIC
    static char to_ascii[256];
    static char to_ebcdic[256];
#endif

public:

    static inline void SetBadCode(wchar_t c)
    {
        base[c >> LOG_COMPLEMENT_SIZE][c] = BAD_CODE;
    }

    static inline void CodeCheck(wchar_t c)
    {
         assert(c >> LOG_COMPLEMENT_SIZE < BASE_SIZE);
         assert(base[c >> LOG_COMPLEMENT_SIZE] + c >= (&code[0]));
         assert(base[c >> LOG_COMPLEMENT_SIZE] + c < (&code[18048]));
    }

    static inline bool IsNewline(wchar_t c) // \r characters are replaced by \x0a in read_input.
    {
        return c == '\x0a';
    }

    static inline bool IsSpaceButNotNewline(wchar_t c)
    {
        return base[c >> LOG_COMPLEMENT_SIZE][c] == SPACE_CODE;
    }

    static inline bool IsSpace(wchar_t c)
    {
        return base[c >> LOG_COMPLEMENT_SIZE][c] <= SPACE_CODE;
    }

    static inline bool IsDigit(wchar_t c)
    {
        return base[c >> LOG_COMPLEMENT_SIZE][c] == DIGIT_CODE;
    }

    static inline bool IsUpper(wchar_t c)
    {
        return base[c >> LOG_COMPLEMENT_SIZE][c] == UPPER_CODE;
    }

    static inline bool IsLower(wchar_t c)
    {
        return base[c >> LOG_COMPLEMENT_SIZE][c] == LOWER_CODE;
    }

    static inline bool IsAlpha(wchar_t c)
    {
        return base[c >> LOG_COMPLEMENT_SIZE][c] >= LOWER_CODE;
    }

    static inline bool IsAlnum(wchar_t c)
    {
        return base[c >> LOG_COMPLEMENT_SIZE][c] >= DIGIT_CODE;
    }

#ifdef EBCDIC
    static inline char ToASCII(char c)         { return to_ascii[c]; }
    static inline char ToEBCDIC(char c)        { return to_ebcdic[c]; }
#endif

};

#endif
