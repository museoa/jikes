// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class Test {
    public static void main(String args[]) {
        int i = 0;
        int s = 0;
        while (i < 10) {
            s = s + i;
            i++;
        }

	System.out.println(s);
        if (s == 45) {
            System.exit(0);
        } else {
            System.exit(1);
        }
    }
}
