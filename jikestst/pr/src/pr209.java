// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class aa2 {
	String m = "hello " + new Object() {
		public String toString() {
			return s;
		}
	};
	String s = "world";
	public static void main(String[] args) {
		System.out.println(new aa2().m);
	}
}

