// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

// file A.java
public class A {

	A() {

		C cat = new C();
	}

	class B {

		private void foo() {
		}
	}

	class C extends B {

		C () {
			foo();
		}
	}
}

