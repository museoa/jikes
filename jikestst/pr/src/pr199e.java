// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class Test {
 char  c1 = '\u000D';
 // this comment about \u000D is not legal; it is really two lines
 
}##pr199f.java##
class Test {
	void[] m(int i) {}
}##pr199g.java##
class Test {
	abstract private void am() {}
}##pr199h.java##
public class Example1 {
	public static void main(String [] args) {
		int k =0;
		for (final int a;;) {
			k++;
			a=k;
			System.out.println("a="+a);
			if (k>3)
				return;
		}
	}
}

public class Example2 {
	final int k;

	Example2() {
		k = 1;
	}
	Example2(Object whatever) {
		this();
		k = 2;
	}
	static public void main(String[] args) {
		Example2 t = new Example2(null);
		System.out.println("k is " + t.k);
	}
}
