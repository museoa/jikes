// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

public class Locals {
	public static void main(final String[] args) {
		new Locals().tryIt();
	}
	private void tryIt() {
		int i = 1;
		new Thread() {
			public void run() {
				int i = 2;
				System.out.println(i);
			}
		}.start();
	}
}

