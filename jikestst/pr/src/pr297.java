// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

import java.io.*;
public class Foo {
	public static class FooBar extends DataInputStream {
		public FooBar() {
			super(new InputStream() {
				public int read() {
					return -1;
				}
			});
		}
	}
}
