// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

abstract class Parent {
	Parent() {
		setI(100);
	}

	abstract public void setI(int value);
}

public class InitTest extends Parent {
	public int i = 0;

	public void setI(int value) {
		i = value;
	}

	public static void main(String [] args) {
		InitTest test = new InitTest();
		System.out.println(test.i);
	}
}##pr199lb.java##
public class InitTest2 {
	public int j = method();
	public int i = 0;

	public int method() {
		i = 100;
		return 200;
	}

	public static void main(String[] args) {
		InitTest2 test = new InitTest2();
		System.out.println(test.i);
	}
}

	##pr199ma.java##
import pack1.P1;
public class Cmain {
	public static void main(String [] args) {
		P1 p1 = new P1();
		// the access to field 'i' below are
		// ill3gal, as the type of 'p2', the
		// class type 'pack1.P2', is not accessible.
		p1.p2.i = 3;
		System.out.println(p1.p2.i);
	}
}
