// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

public class SubC extends SuperC {
// Fall 1:
	public String name2 = null;
// Fall 2:
//public String name2 = name;

	public SubC() {
		super();
		System.out.println("SubC() name2="+name2);
	}

	public void init() {
		super.init();
		name2 = "Frank";
	}
	public static void main( String args[] ) {
		System.out.println( "main() name ="+new SubC().name );
	}
}
