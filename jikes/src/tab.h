// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#ifndef tab_INCLUDED
#define tab_INCLUDED

#include "config.h"
#include <wchar.h>
#include <string.h>

class Tab
{
public:
    enum { TABSIZE = 8 };

    //
    // Compute the length of a wide character string segment
    // after expanding tabs.
    //
    static int Wcslen(wchar_t *line, int start, int end)
    {
        for (int i = start--; i <= end; i++)
        {
            if (line[i] == U_HORIZONTAL_TAB)
            {
                int offset = (i - start) - 1;
                start -= ((TABSIZE - 1) - offset % TABSIZE);
            }
        }

        return (end - start);
    }
};

#endif

