// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class final2 {
	int a = (final2.k = 1);
	final static int k = 5;
	int b = (final2.k = 2);
	int c = (k = 3);
}

