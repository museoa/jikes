// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#include "config.h"
#include <sys/stat.h>
#include <iostream.h>
#include "bool.h"
#include "control.h"
#include "scanner.h"
#include "parser.h"
#include "semantic.h"
#include "unicode.h"
#include "set.h"

//
// Check whether or not there are files to be rempiled.
//
bool Control::IncrementalRecompilation()
{
    cerr << "\nTo Be Supplied Later...\n";
    cerr.flush();

    return false;
}
