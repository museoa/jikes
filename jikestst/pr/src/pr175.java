// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class float1 {
	void v() {
		switch (1) {
			case (int) (1.0/0.0):
				break;
			case (int) (2.0/0.0):
				break;
		}
	}
}

