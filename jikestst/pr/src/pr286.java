// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class BlankFinals {
	final int i;

	BlankFinals() {
		System.out.println(i); // use i when it is definitely unassigned

		i = 5;
	}

	public static void main (String[] args) {
		new BlankFinals(); // prints '0'
	}
}
