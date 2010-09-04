using System;

// Test program for parser wrapper
class ParseTest
{
	static string raw = 
		"GET /test HTTP/1.1\r\n" +
		"User-Agent: curl/7.18.0 (i486-pc-linux-gnu)\r\n" +
		"Host: 0.0.0.0=5000\r\n" +
		"Accept: */*\r\n" +
		"\r\n";

	static void Main( string[] args ) {
		Parser parser = new Parser();
		parser.MessageBegin += new HttpDelegate( HttpCallback );
		parser.Path += new HttpDataDelegate( HttpDataCallback );
		parser.HeaderField += new HttpDataDelegate( HttpDataCallback );
		parser.HeaderValue += new HttpDataDelegate( HttpDataCallback );
		parser.Parse( System.Text.Encoding.ASCII.GetBytes( raw ) );
		parser.Dispose();
		Console.ReadLine();
	}

	static void HttpCallback() {
		Console.WriteLine( "Called non-data cb" );
	}
	static void HttpDataCallback( string in_str ) {
		Console.WriteLine( in_str );
	}
}
