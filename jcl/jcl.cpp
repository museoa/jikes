// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://ibm.com/developerworks/opensource/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#include "jcl_iclass.h"

int main(int argc, char *argv[])
{
    if (argc > 1) {
                InputClassFile  cf(argv[1]);
                cf.print();
        }

    return 0;
}
