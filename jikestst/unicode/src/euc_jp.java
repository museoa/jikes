import java.io.*;

public class euc_jp
{
    public static void main(String argv[])
    {
        try
        {
            FileOutputStream file = new FileOutputStream("euc_jp.html");

            OutputStreamWriter writer = new OutputStreamWriter(file, "EUC_JP");

            writer.write("<HTML>\r\n" +
                         "<HEAD>\r\n" +
                         "    <META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=euc-jp\">\r\n" +
                         "    <TITLE>Japanese</TITLE>\r\n" +
                         "</HEAD>\r\n" +
                         "<BODY>\r\n");



            writer.write("　、。，．・：；？！゛゜´｀¨＾￣＿ヽヾゝゞ〃仝々〆〇ー―‐／＼〜‖｜<BR>\r\n");
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
