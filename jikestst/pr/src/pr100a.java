// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

//-----------------------------------------------------------------------
// File Whoops.java - compiles OK
import java.net.URL;
import java.sql.*;

public class Whoops
{
	private static boolean forceInit = init();
	private static Connection con;   // Derived tries to access this

	private static boolean init()
	{
		try
		{
			String url   = "jdbc:odbc:shares";
			Class.forName ("sun.jdbc.odbc.JdbcOdbcDriver");
			con = DriverManager.getConnection (
											   url, "", "");
		}
		catch (java.lang.Exception ex)
		{
		}
		return true;
	}
}
