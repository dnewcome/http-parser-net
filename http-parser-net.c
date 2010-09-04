/**
* http-parser-net is free software provided under the MIT license.
*	See LICENSE file for full text of the license.
*	Copyright 2010 Dan Newcome.
*/

#include "http_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#using <System.dll>

using namespace System::Runtime::InteropServices;
using namespace System;

// delegates for .NET events 
public delegate void HttpDataDelegate( System::String^ s );
public delegate void HttpDelegate();

public ref class Parser {
public: 

	event HttpDelegate^ MessageBegin;
	event HttpDataDelegate^ Path;
	event HttpDataDelegate^ QueryString;
	event HttpDataDelegate^ Url;
	event HttpDataDelegate^ Fragment;
	event HttpDataDelegate^ HeaderField;
	event HttpDataDelegate^ HeaderValue;
	event HttpDelegate^ HeadersComplete;
	event HttpDataDelegate^ Body;
	event HttpDelegate^ MessageComplete;

	Parser() {
		m_parser = ( http_parser* )malloc( sizeof( http_parser ) );
		// todo: this only enables parsing of requests
		// do we need to handle responses?
		http_parser_init( m_parser, HTTP_REQUEST );
	}
	!Parser() {
		message_begin_gch.Free();
		path_gch.Free();
		query_string_gch.Free();
		url_gch.Free();
		fragment_gch.Free();
		header_field_gch.Free();
		header_value_gch.Free();
		headers_complete_gch.Free();
		body_gch.Free();
		message_complete_gch.Free();
	}
	~Parser() { this->!Parser(); }

	int Parse( array<unsigned char>^ in_buf ) {

		// MessageBegin 
		http_cb_delegate^ message_begin_del = gcnew http_cb_delegate( this, &Parser::message_begin_cb );
		message_begin_gch = GCHandle::Alloc( message_begin_del );
		System::IntPtr message_begin_ip = Marshal::GetFunctionPointerForDelegate( message_begin_del );

		// Path 
		http_data_cb_delegate^ path_del = gcnew http_data_cb_delegate( this, &Parser::path_cb );
		path_gch = GCHandle::Alloc( path_del );
		System::IntPtr path_ip = Marshal::GetFunctionPointerForDelegate( path_del );
		
		// QueryString 
		http_data_cb_delegate^ query_string_del = gcnew http_data_cb_delegate( this, &Parser::query_string_cb );
		query_string_gch = GCHandle::Alloc( query_string_del );
		System::IntPtr query_string_ip = Marshal::GetFunctionPointerForDelegate( query_string_del );
		
		// Url 
		http_data_cb_delegate^ url_del = gcnew http_data_cb_delegate( this, &Parser::url_cb );
		url_gch = GCHandle::Alloc( url_del );
		System::IntPtr url_ip = Marshal::GetFunctionPointerForDelegate( url_del );

		// Fragment 
		http_data_cb_delegate^ fragment_del = gcnew http_data_cb_delegate( this, &Parser::fragment_cb );
		fragment_gch = GCHandle::Alloc( fragment_del );
		System::IntPtr fragment_ip = Marshal::GetFunctionPointerForDelegate( fragment_del );

		// HeaderField
		http_data_cb_delegate^ header_field_del = gcnew http_data_cb_delegate( this, &Parser::header_field_cb );
		header_field_gch = GCHandle::Alloc( header_field_del );
		System::IntPtr header_field_ip = Marshal::GetFunctionPointerForDelegate( header_field_del );

		// HeaderValue
		http_data_cb_delegate^ header_value_del = gcnew http_data_cb_delegate( this, &Parser::header_value_cb );
		header_value_gch = GCHandle::Alloc( header_value_del );
		System::IntPtr header_value_ip = Marshal::GetFunctionPointerForDelegate( header_value_del );

		// HeadersComplete 
		http_cb_delegate^ headers_complete_del = gcnew http_cb_delegate( this, &Parser::headers_complete_cb );
		headers_complete_gch = GCHandle::Alloc( headers_complete_del );
		System::IntPtr headers_complete_ip = Marshal::GetFunctionPointerForDelegate( headers_complete_del ); 

		// Body 
		http_data_cb_delegate^ body_del = gcnew http_data_cb_delegate( this, &Parser::body_cb );
		body_gch = GCHandle::Alloc( body_del );
		System::IntPtr body_ip = Marshal::GetFunctionPointerForDelegate( body_del );

		// MessageComplete 
		http_cb_delegate^ message_complete_del = gcnew http_cb_delegate( this, &Parser::message_complete_cb );
		message_complete_gch = GCHandle::Alloc( message_complete_del );
		System::IntPtr message_complete_ip = Marshal::GetFunctionPointerForDelegate( message_complete_del );

		size_t nparsed;
		http_parser_settings parser_settings = {   
			static_cast<http_cb>( message_begin_ip.ToPointer() ),
			static_cast<http_data_cb>( path_ip.ToPointer() ),
			static_cast<http_data_cb>( query_string_ip.ToPointer() ),
			static_cast<http_data_cb>( url_ip.ToPointer() ),
			static_cast<http_data_cb>( fragment_ip.ToPointer() ),
			static_cast<http_data_cb>( header_field_ip.ToPointer() ),
			static_cast<http_data_cb>( header_value_ip.ToPointer() ),
			static_cast<http_cb>( headers_complete_ip.ToPointer() ),
			static_cast<http_data_cb>( body_ip.ToPointer() ),
			static_cast<http_cb>( message_complete_ip.ToPointer() )
		};

		pin_ptr<unsigned char> ibuf = &in_buf[0];	
		char* buf = (char*)ibuf;
		nparsed = http_parser_execute( m_parser, &parser_settings, buf, strlen( buf ) );
		free( m_parser );
		return nparsed;
	}
	
private:	
	http_parser* m_parser;
	http_parser_settings* parser_settings; 

	GCHandle message_begin_gch;
	GCHandle path_gch;
	GCHandle query_string_gch;
	GCHandle url_gch;
	GCHandle fragment_gch;
	GCHandle header_field_gch;
	GCHandle header_value_gch;
	GCHandle headers_complete_gch;
	GCHandle body_gch;
	GCHandle message_complete_gch;

	// delegates to wrap the http parser function pointers
	delegate int http_data_cb_delegate( http_parser *p, const char *buf, size_t len );
	delegate int http_cb_delegate( http_parser *p );

	// callback functions raise events when called by parser
	int message_begin_cb( http_parser *p ) {
		MessageBegin();
		return 0;
	}

	int path_cb( http_parser *p, const char *buf, size_t len ) {
		System::String^ netstring = gcnew System::String( buf, 0, len );
		Path( netstring );
		return 0;
	}

	int query_string_cb( http_parser *p, const char *buf, size_t len ) {
		System::String^ netstring = gcnew System::String( buf, 0, len );
		QueryString( netstring );
		return 0;
	}

	int url_cb( http_parser *p, const char *buf, size_t len ) {
		System::String^ netstring = gcnew System::String( buf, 0, len );
		Url( netstring );
		return 0;
	}
	int fragment_cb( http_parser *p, const char *buf, size_t len ) {
		System::String^ netstring = gcnew System::String( buf, 0, len );
		Fragment( netstring );
		return 0;
	}
	int header_field_cb( http_parser *p, const char *buf, size_t len ) {
		System::String^ netstring = gcnew System::String( buf, 0, len );
		HeaderField( netstring );
		return 0;
	}

	int header_value_cb( http_parser *p, const char *buf, size_t len ) {
		System::String^ netstring = gcnew System::String( buf, 0, len );
		HeaderValue( netstring );
		return 0;
	}
	int headers_complete_cb( http_parser *p ) {
		HeadersComplete();
		return 0;
	}
	int body_cb( http_parser *p, const char *buf, size_t len ) {
		System::String^ netstring = gcnew System::String( buf, 0, len );
		Body( netstring );
		return 0;
	}
	int message_complete_cb( http_parser *p ) {
		MessageComplete();
		return 0;
	}
};

