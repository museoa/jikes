// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

package pack1;
public class P1 {
	public P2 p2 = new P2();
}
// Note the absence of any access modifier, mkaing
// 'P2' accessible only from within package 'pack1'.

class P2 {
	public int i = 0;
}
