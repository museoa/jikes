// $Id$
public class InitTest2 {
	public int j = method();
	public int i = 0;

	public int method() {
		i = 100;
		return 200;
	}

	public static void main(String[] args) {
		InitTest2 test = new InitTest2();
		System.out.println(test.i);
	}
}

	##pr199ma.java##
import pack1.P1;
public class Cmain {
	public static void main(String [] args) {
		P1 p1 = new P1();
		// the access to field 'i' below are
		// ill3gal, as the type of 'p2', the
		// class type 'pack1.P2', is not accessible.
		p1.p2.i = 3;
		System.out.println(p1.p2.i);
	}
}
