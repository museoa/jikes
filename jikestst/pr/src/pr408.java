// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

public class myclass  {
	public static void main( String args[] ) {
		myclass newObj = new myclass();
		System.out.println("OK");
	}

	public myclass() {
		myinner depo = new myinner ();
	}

	public class myinner {
		public myinner() {
			this(true);
		}
		public myinner(boolean bool) {
		}
	}
