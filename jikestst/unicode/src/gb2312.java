import java.io.*;

public class gb2312
{
    public static void main(String argv[])
    {
        try
        {
            FileOutputStream file = new FileOutputStream("gb2312.html");

            OutputStreamWriter writer = new OutputStreamWriter(file, "EUC_CN");

            writer.write("<HTML>\r\n" +
                         "<HEAD>\r\n" +
                         "    <META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=gb2312\">\r\n" +
                         "    <TITLE>gb2312 Characters</TITLE>\r\n" +
                         "</HEAD>\r\n" +
                         "<BODY>\r\n");



            writer.write("Some stuff: ");
            writer.write("亲 爱 的 china.com 电 子 邮 件 用 户 ： <BR>\r\n");
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
