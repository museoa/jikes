// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

public class foo {

	public static void foos_function() {
		class foos_bar {
			foos_bar () {
				System.out.println("Hey");
			}
		}
		foos_bar bar = new foos_bar ();
	}

	public static void main(String[] args) {
	}

}
