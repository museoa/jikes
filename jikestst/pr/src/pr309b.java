// $Id$
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

// Package Y, file BB.java:

   import X.AA;

public class BB
{
	private AA.AAListener myInterf = new my_aa_implem ();

	class my_aa_implem implements AA.AAListener
	{
		public void handleEvent (AA.AAEvent evt) {
				// ...
		}
	}
}
