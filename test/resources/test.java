import java.io.StringReader;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;


public class test extends DefaultHandler {

	public static String 
	 a = ""
		 + "<?xml version='1.0' encoding='UTF-8'?>\n"
		 + "<root>\n"
		 + "    <child attr=\"value\" attribute=\"val2\"/>\n"
		 + "    <a value=\"&#x153;&#x20ac;&quot;\"/>\n"
		 + "    <b enc=\"&#x161;&amp;\"/>\n"
		 + "    <!-- commento --><![CDATA[\n"
		 + "valore=&&&\n"
		 + "]]></root>";
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		test t = new test();
		SAXParserFactory factory = SAXParserFactory.newInstance();
		SAXParser saxParser;
		try {
			saxParser = factory.newSAXParser();
			saxParser.parse(new InputSource( new StringReader(a)), t);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	public void startElement(String namespaceURI, 
			String simpleName,
			String qualifiedName,
			Attributes attributes ) throws SAXException
	{
		System.out.println("<"+qualifiedName+"/"+simpleName+">\n");
	}
}
