// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.


class Test {
    public static void main ( String args[] ) {
        int i;
	char c;

        i = 0xFFFF8000;
	c = (char)i;
        System.out.println((byte)(c >> 15));
        System.exit((byte)(c >> 15));
    }
}
