// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

   package x;

public class A
{
	public static void main( String[] args )
	{
		Object a = new A();
		if ( a instanceof x.A )
			System.out.println( "yes" );
	}
}
