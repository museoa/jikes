// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

interface Junk { public void junk(); }
public class External {
	public External() {
	}

	public Junk foo() { return new inner(); }
	private void bar() {
		System.out.println("bar");
	}

	class inner implements Junk {
		public void junk() {
			bar();
		}

	}


	public static void main(String[] args)
	{
		new External().foo().junk();

	}
}


