// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class Test {
    public static void main(String args[]) {
        int i, j;
        int s[] = new int[7];
        try {
            for (i = 0; s[i] < 10; i++) {
                j = i + i;
            }
        } catch (Exception e) {
            System.out.println(0);
            System.exit(0);
        }
	System.out.println(1);
        System.exit(1);
    }
}
