// $Id$
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

public class TestShadow {
	public static void main(String args[]) {
		B b = new B();                      // Create an object of type B

		int foo = b.getFoo();               // Call A.getFoo() through B.
		System.out.println(foo);            // 1 - Surprise! You get A's value

		A a = (A) b;                        // Get a ref to b of type A.
		System.out.println(a.foo);          // 1 - A's value again
		System.out.println(b.foo);          // 0 - B's value, of course
	}
}

