// $Id$
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class P {
	P () {
		I bar = new I ();
		bar.foo ();
	}

	private void p () {
		System.out.println ("In p()");
	}

	public static void main (String[] args) {
		new P ();
	}

	class I {
		void foo () {
			System.out.println ("Calling p()");
			p ();
		}
	}
} 
