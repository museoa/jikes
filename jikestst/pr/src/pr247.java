// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

public class boom {

	private void hello() {
		System.out.println("hello world!") ;
	}

	public void crash() {
		(new Runnable() {
			public void run() {
				hello() ;
			}
		}).run() ;
	}

	public static void main(String [] args) {
		new boom().crash() ;
	}
}

