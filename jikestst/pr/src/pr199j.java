// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

interface Noisy {
	int method();
}

interface Quiet {
	void method();
}

public class InterfaceCast {
	public static void main(String [] args) {
		Noisy one = null;
		Quiet two = (Quiet) one;
	}
}
