// The MIT License (MIT)
//
// Copyright (c) 2016-2017 Darrell Wright
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

#include <iostream>

#define BOOST_TEST_MODULE percent_decoding
#include <daw/boost_test.h>
#include <daw/daw_string_view.h>

#include "percent_decode_view.h"

BOOST_AUTO_TEST_CASE( daw_percent_decoding_test_001 ) {
	std::string const enc_str = "http%3A%2F%2Fwww.google.com";
	std::string const expected_str = "http://www.google.com";

	std::string result{};
	std::copy( daw::make_percent_decode_iterator( enc_str.data( ) ),
	           daw::make_percent_decode_iterator( enc_str.data( ) + enc_str.size( ) ), std::back_inserter( result ) );

	BOOST_REQUIRE_EQUAL( expected_str, result );
}

BOOST_AUTO_TEST_CASE( daw_percent_decoding_test_002 ) {
	std::string enc_str = "http%3A%2F%2Fwww.google.com";
	std::string const expected_str = "http://www.google.com";
	auto view = daw::make_percent_decode_view( enc_str.data( ), enc_str.data( ) + enc_str.size( ) );

	BOOST_REQUIRE_EQUAL( view.size( ), expected_str.size( ) );
	auto str_result = static_cast<std::string>(view);
	BOOST_REQUIRE_EQUAL( str_result.size( ), expected_str.size( ) );
	BOOST_REQUIRE_EQUAL( str_result, expected_str );
}

