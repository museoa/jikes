// $Id$
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

class JikesBug {
	JikesBug( ActionListener e ) {
		new JikesBug( new ActionListener() {
			public void actionPerformed( ActionEvent e ) { }
		} );
	}
}

