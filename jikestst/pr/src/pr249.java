// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

public interface EzlDict {
	public Object get(String key);
	public void put(String key, Object value);
	public Object call(String name, Object[] argv, Object argd);
	public Object call_new(Object[] argv, Object argd);
	public String  name;
	public EzlEnumerate  enumerate();
}
interface EzlEnumerate {
	boolean  next_elements(Object[] key_value);
}

