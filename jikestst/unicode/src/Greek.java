import java.io.*;

public class Greek
{
    public static void main(String argv[])
    {
        try
        {
            FileOutputStream file = new FileOutputStream("greek.html");

            OutputStreamWriter writer = new OutputStreamWriter(file, "ISO8859_7");

            writer.write("<HTML>\r\n" +
                         "<HEAD>\r\n" +
                         "    <META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=iso-8859-7\">\r\n" +
                         "    <TITLE>Greek</TITLE>\r\n" +
                         "</HEAD>\r\n" +
                         "<BODY>\r\n");



            writer.write("Χρυσή Αυγή ανεπίσημη σελίδα του Λαικού συνδέσμου<BR>\r\n");
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
