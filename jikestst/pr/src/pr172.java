// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

final class Test {
	static public String foo() { return "\u0000\uffff"; }
	static public String bar() { return

			"\u0f81\u0f82\u0f81\u0f82\u0f81\u0f82\u0f81\u0f82" +
			"\u0f81\u0f82\u0f81\u0f82\u0f81\u0f82\u0f81\u0f82" +
			"\u0f81\u0f82\u0f81\u0f82\u0f81\u0f82\u0f81\u0f82" +
			"\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000" +
			"\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000" +
			"\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000" +
			"\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000" +
			"\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000" +
			"\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000" +
			"\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000" +
			"\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82" +
			"\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82" +
			"\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82" +
			"\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82" +
			"\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82" +
			"\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82\u0f82";

	}
	public static void main(String[] args) {
		System.err.println(Integer.toHexString((int)(foo().charAt(0))));
		System.err.println(Integer.toHexString((int)(foo().charAt(1))));
		int l = bar().length();
		System.err.println("" + l);
		for (int i = 0; i < l; i++) {
			System.err.println(Integer.toHexString(i) + " " + Integer.toHexString((int)bar().charAt(i)));
		}
	}
}


