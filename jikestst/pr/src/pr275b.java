// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

interface C {
	public void m(Object o);
	public void m(String o);
}

public class Main2 implements C {
	public static void main(String argv[]) {
		C c = new Main2();
		c.m("A String");
	}
	public void m(Object o) { System.out.println("Object"); }
	public void m(String o) { System.out.println("String"); }
}
