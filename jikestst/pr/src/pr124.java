// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

package test;

public class Crash {

	void abc() {
		new UpdateThread(null);
	}

	static class UpdateThread extends Thread {
		Object new;

		UpdateThread(Object new) {
		}
	}
}

