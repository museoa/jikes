// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

package test;

import test.subpackage.C;

class B extends C {
	void m2(C c) {
		c.x = 3;
		System.out.println("c.x = " + c.x);
	}
}

public class Protect {
	public static void main(String[] args) {
		B b = new B();
		C c = new C();
		b.m2(c);
	}
}


