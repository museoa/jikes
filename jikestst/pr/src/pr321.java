// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

package sides.monitor;

public class Token {

	public int kind;
	public int beginLine, beginColumn, endLine, endColumn;
	public String image;
	public Token next;
	public Token specialToken;

	public final String toString()
	{
		return image;
	}

	public static final Token newToken(int ofKind)
	{
		switch(ofKind)
		{
			default : return new Token();
		}
	}
}
