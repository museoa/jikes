// $Id$
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

package com.qwest.common.account;

import com.qwest.common.customer.Customer;

public interface Account
{
	AccountId getIdentity();
	String getAttribute1();
	Customer getCustomer();
}
