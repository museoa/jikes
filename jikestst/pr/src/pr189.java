// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

import java.awt.*;
import java.awt.event.*;

public class B
{
	private static  int numberOfWindows = 0;
	private static WindowListener windowCloser =
												new WindowAdapter()
	{
													public void windowClosing(WindowEvent e)
													{
														B.numberOfWindows--;
													}
	};
 public static void main(String[] args) {
  System.out.println("0");
 }



 
}

