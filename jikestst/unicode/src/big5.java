import java.io.*;

public class big5
{
    public static void main(String argv[])
    {
        try
        {
            FileOutputStream file = new FileOutputStream("big5.html");

            OutputStreamWriter writer = new OutputStreamWriter(file, "Big5");

            writer.write("<HTML>\r\n" +
                         "<HEAD>\r\n" +
                         "    <META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=BIG5\">\r\n" +
                         "    <TITLE>big5 Characters</TITLE>\r\n" +
                         "</HEAD>\r\n" +
                         "<BODY>\r\n");



            writer.write("Some stuff: ");
            writer.write("華康中黑體\">下使用的檔案管理共享軟體。<BR>\r\n");
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
