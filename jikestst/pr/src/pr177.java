// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

// anonymous inner classes should be final for instanceof and casts
class instanceof1 {
	void v() {
		Object o = new Object() {
			boolean b = this instanceof myinterface;
		};
	}
}
interface myinterface {
}

