import java.io.*;

public class utf8
{
    public static void main(String argv[])
    {
        try
        {
            FileOutputStream file = new FileOutputStream("utf8.html");

            OutputStreamWriter writer = new OutputStreamWriter(file, "UTF-8");

            writer.write("<HTML>\r\n" +
                         "<HEAD>\r\n" +
                         "    <META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf-8\">\r\n" +
                         "    <TITLE>UTF(</TITLE>\r\n" +
                         "</HEAD>\r\n" +
                         "<BODY>\r\n");



            writer.write("¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾<BR>\r\n");
            writer.write("ĀāĂăĄąĆćĈĉĊċČčĎďĐđĒē<BR>\r\n");
            writer.write("ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡ<BR>\r\n");
            writer.write("абвгдежзийклмнопрстуфхцчшщъыьэюя<BR>\r\n");
            writer.write("אבגדהוזחטיךכלםמןנסעףפץצקרש<BR>\r\n");
            writer.write("ءآأؤإئابةتثجحخدذرزسشصضطظع<BR>\r\n");
            writer.write("€<BR>\r\n");
            writer.write("</BODY>\r\n" +
                         "</HTML>\r\n");

            writer.close();
            file.close();
        }
        catch (Exception exc)
        {
            System.err.println(exc.getMessage());
        }
    }
}
