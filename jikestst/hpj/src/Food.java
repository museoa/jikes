// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class Food implements cost {
  int costofitem;

  Food(int i) {
    costofitem = i + 2;
   }

  public int price() {
    return costofitem;
  }

}
