// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

// Test of null pointer exception 

class Test {
  public static void main (String args[]) {
    int[] a;
    int i;
   
    a = null;
    a[2] = 3;
    i = a[2];
    System.out.println(i);
    System.exit(i);
  }
}
