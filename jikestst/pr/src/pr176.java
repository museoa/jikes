// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class ambig1 implements iface {
	class inner implements iface {
		void v() {
			int i = constant;
		}
	}
}
interface iface {
	int constant = 0;
}
class Test {
 public static void main(String[] args) {
  System.out.println("0");
 }



 
}
