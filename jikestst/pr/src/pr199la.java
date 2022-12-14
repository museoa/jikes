// $Id$
// This software is subject to the terms of the IBM Jikes Compiler
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
}
