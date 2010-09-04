# About

http-parser-net is a managed .NET wrapper around Ryan Dahl's node.js http-parser, 
which is implemented in C.

# Usage

http-parser is a streaming parser that relies on callbacks implemented
as C function pointers. In .NET callbacks are implemented using delegates,
and are commonly exposed as an event-based API. http-parser-net exposes 
a series of events that are called as data is parsed.

Sample header:

	string http = 
		"GET /test HTTP/1.1\r\n" +
		"User-Agent: curl/7.18.0 (i486-pc-linux-gnu)\r\n" +
		"Host: 0.0.0.0=5000\r\n" +
		"Accept: */*\r\n" +
		"\r\n";

Setting up the parser:

	Parser parser = new Parser();
	parser.Path += new HttpDataDelegate( HttpDataCallback );

Sample callback function:

	static void HttpDataCallback( string in_str ) {
		Console.WriteLine( in_str );
	}

Parsing data:

	parser.Parse( System.Text.Encoding.ASCII.GetBytes( http ) );

Cleaning up:

	parser.Dispose();

# Status

This .NET implementation is very basic and does not support using the parser
to parse server response headers. It also is not optimized for performance
and may have some memory leaks. This code was written for future use in 
Node.net, and I don't expect that it will be robust enough for other uses.

# License

The http-parser-net wrapper is copyright 2010 Dan Newcome and is based on 
work by Ryan Dahl, copyright 2009-10. See LICENSE for the full license.
