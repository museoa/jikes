// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class lse
{
	public static void main(String argv[])
	{
		lse p = new lse();
	}

	byte b = 1;
	long l = 1;
	lse() {
		b <<= l; // Offending Line
	}
}

