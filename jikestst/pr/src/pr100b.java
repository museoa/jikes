// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

//-----------------------------------------------------------
// File Derived.java - puts jikes into an endless loop of:
//
// CODE "0" is an invalid tag!!!

public class Derived extends Whoops
{
	private static Statement stmt;

	private static boolean init()
	{
		stmt = con.createStatement();
	}
}
