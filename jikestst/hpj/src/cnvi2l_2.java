// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class Test {
    public static void main ( String args[] ) {
        int i;
	long l;

        i = 0x80000000;
	l = (long)i;
        System.out.println(2 + (byte)(l >> 31));
        System.exit(2 + (byte)(l >> 31));
    }
}
