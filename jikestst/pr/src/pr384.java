// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class us {
	public static void main(String[] args) {
		m();
		System.out.println("hi there"); 
	}
	static void m() {
		int i=0,j=0,k=0;
		try { 
			i = j*k;
		}
		catch (Throwable e) {
			System.out.println("exception caught");
		}
	}
}
