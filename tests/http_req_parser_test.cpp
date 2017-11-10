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

#include "http_req_parser.h"

constexpr daw::http_request test_func( daw::string_view str ) {
	return daw::construct_from<daw::http_request, daw::request_method, daw::unquoted_string_view, daw::http_version>(
	  std::move( str ), daw::parser::single_whitespace_splitter{} );
}

int main( int, char **argv ) {
	constexpr daw::http_request req{test_func( "GET http://www.google.ca/ HTTP/1.1" )};

	std::cout << to_string( req.method ) << " " << req.uri << " HTTP/" << static_cast<int>( req.version.ver_major ) << '.'
	          << static_cast<int>( req.version.ver_minor ) << '\n';
	return EXIT_SUCCESS;
}

