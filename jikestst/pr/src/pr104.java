// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class C0 {}
class C1 extends C0 {
	Object o = new Object() {
		void f() { o = null; };
	}
}
class Test {
	public static void main(String[] args) {
		System.out.println("0");
	}
}


