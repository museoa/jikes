// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

// z.java
// Unamed package, or something other than jvs.util
class z
{
	// interface NoExtends
	// Each interface type named in extends must be accessable
	interface ix extends jvs.util.NoExtends
			{}
}

// NoExtends
package jvs.util;

/* This interface is not public
 * outside of the jvs.util package
 */

interface NoExtends{
	int m1();
	int m2();
}

