// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class JitBug {

	static void except() {
		int one = 1, zero = 0;
		try {
			if (one == 2)
				one = one / zero;
		} catch (RuntimeException e) {
			throw e;
		} finally {
		}
	}

	static public void main(String[] args) {
		for (int i = 0; i < 50; ++i) {
			except();
		}
	}

}
