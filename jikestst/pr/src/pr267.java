// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

import java.io.*;

class DATest
{
	DATest (java.io.File javaFile)
	{
		try
		{
			_istream = new FileInputStream(javaFile); // throws checked
		}
		finally
		{
			throw new IllegalStateException(); // finally completes
			abruptly with unchecked
		}
	}

	private FileInputStream _istream;
}


