import java.io.*;

public class Hebrew
{
    public static void main(String argv[])
    {
        try
        {
            FileOutputStream file = new FileOutputStream("Hebrew.html");

            OutputStreamWriter writer = new OutputStreamWriter(file, "ISO8859_8");

            writer.write("<HTML>\r\n" +
                         "<HEAD>\r\n" +
                         "    <META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=iso-8859-8\">\r\n" +
                         "    <TITLE>Hebrew</TITLE>\r\n" +
                         "</HEAD>\r\n" +
                         "<BODY>\r\n");



            writer.write("תא ליבות הנוכנ הגהנה .לארשי תנידמ יחרזאבו לארשי תנידמב ןימאמ ינא<BR>\r\n");
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
