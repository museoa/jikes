// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

import com.sun.java.swing.*;

public class Bug {
	private JTree tree;

	public Bug() {
		tree = new JTree();

		tree.setCellEditor (new DefaultCellEditor (new JTextField()) {
			public Object getCellEditorValue () {
				return (String)delegate.getCellEditorValue();
			}
		});
	}
}
