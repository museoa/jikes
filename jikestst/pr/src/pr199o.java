// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class Test {
	void method() {
		int i=9;
		int j=21;
        sameName:
		while (i>j) {
			sameName:
			while (j<i) {
				break sameName;
			}
		}
	}
}##pr199p.java##
class Test {
	void method() {
		label: int j=1;
			   j++;
			   return;
	}
}##pr199qa.java##
package java.lang.String;
class Illegal {
}
