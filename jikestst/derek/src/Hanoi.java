// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class Globals {
        static public int NumDisks;
        static public int MaxDisks = 64; // this will do!
}

public class Hanoi {

    static Peg peg1 = new Peg(1),
               peg2 = new Peg(2),
               peg3 = new Peg(3);

    public static void main(String args[]) {

    //   Globals.NumDisks = 25;
         Globals.NumDisks = 19;
       System.out.println("moving " + Globals.NumDisks + " disks...");

       if (Globals.NumDisks > Globals.MaxDisks)
           Globals.NumDisks = Globals.MaxDisks;

       for (int i = Globals.NumDisks; i > 0; i--)
           peg1.addDisk(i);

        peg1.print("Peg1"); peg2.print("Peg2"); peg3.print("Peg3");

        long start = System.currentTimeMillis();

        moveDisks(Globals.NumDisks, peg1, peg3, peg2);

        long stop = System.currentTimeMillis();

        peg1.print("Peg1"); peg2.print("Peg2"); peg3.print("Peg3");

        long t = (stop - start) / 100;

        System.out.println("finished in " + (t / 10) + "." + (t % 10) + " seconds");

       try { Thread.currentThread().sleep(1000000); } catch (InterruptedException e) {}

    }

    public static void moveDisks(int numDisks, Peg fromPeg, Peg toPeg, Peg usingPeg) {
        if (numDisks == 1) {
            int disk;
            toPeg.addDisk(disk = fromPeg.removeDisk());
        } else {
            moveDisks(numDisks - 1, fromPeg, usingPeg, toPeg);
            moveDisks(1, fromPeg, toPeg, usingPeg);
            moveDisks(numDisks - 1, usingPeg, toPeg, fromPeg);
        }
    }
}

class Peg {

    int pegNum;
    int disks[] = new int[64];
    int nDisks;

    public Peg(int n) {
        pegNum = n;
        for (int i = 0; i < Globals.NumDisks; i++)
            disks[i] = 0;
        nDisks = 0;
    }

    public int pegNum() {
        return pegNum;
    }

    public void addDisk(int diskNum) {
        disks[nDisks++] = diskNum;
    }

    public int removeDisk() {
        return disks[--nDisks];
    }

    public void print(String name) {
        System.out.print(name + ":");
        for (int i = 0; i < nDisks; ++i)
           System.out.print(" " + disks[i]);
        System.out.println();
    }
}

