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
