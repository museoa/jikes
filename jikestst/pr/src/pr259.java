// $Id$
// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

public class PrintTest {
	public static void main(String args[]) {
		new PrintCanvas();
                System.out.println("0");
 }


}

class PrintCanvas {
	public PrintCanvas() {
		new Order();
	}
}

class Order {
	private void drawText(double pX, double pY) {
	}

	class Item {
		void paint() {
			drawText(0.0, 0.0);
		}
	}
}

