// $Id$
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class C1 {
	void f() throws IOException {
		Object o = new Object() {
			{
				x.readByte();
			}
		};
	}

	DataInput x;
}
