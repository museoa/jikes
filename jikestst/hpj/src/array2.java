// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

// Test of ArrayIndexOutOfBounds exception

class array2 {
  public static void main (String args[]) {
    int[] a;
    int i,j;
    a = new int[5];
   
    i = 4;
    a[i] = 3;
    j = a[i];

    i = 5;
    a[i] = 9;  // i is too large
    System.out.println(i);
    System.exit(i);
  }
}
