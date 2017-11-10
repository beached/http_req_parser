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

#pragma once

#include <cstdint>

#include <daw/daw_parse_to.h>
#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>

namespace daw {
	enum class request_method : int_fast8_t { OPTIONS = 0, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT };

	std::string to_string( request_method method ) {
		switch( method ) {
		case request_method::OPTIONS:
			return "OPTIONS";
		case request_method::GET:
			return "GET";
		case request_method::HEAD:
			return "HEAD";
		case request_method::POST:
			return "POST";
		case request_method::PUT:
			return "PUT";
		case request_method::DELETE:
			return "DELETE";
		case request_method::TRACE:
			return "TRACE";
		case request_method::CONNECT:
			return "CONNECT";
		}
		std::terminate( );
	}

	struct http_version {
		uint_fast8_t ver_minor;
		uint_fast8_t ver_major;
		constexpr double full( ) const noexcept {
			return static_cast<double>(ver_major) + (static_cast<double>(ver_minor)/10.0);
		}
	};

	struct http_request {
		request_method method;
		daw::string_view uri;
		http_version version;
	};

	constexpr request_method parse_to_value( daw::string_view str, ::daw::request_method ) {
		switch( AsciiUpper( str.front( ) ) ) {
		case 'O':
			if( str == "OPTIONS" ) {
				return daw::request_method::OPTIONS;
			}
			throw daw::parser::invalid_input_exception{};
		case 'G':
			if( str == "GET" ) {
				return daw::request_method::GET;
			}
			throw daw::parser::invalid_input_exception{};
		case 'H':
			if( str == "HEAD" ) {
				return daw::request_method::HEAD;
			}
			throw daw::parser::invalid_input_exception{};
		case 'P':
			if( str == "POST" ) {
				return daw::request_method::POST;
			}
			if( str == "PUT" ) {
				return daw::request_method::PUT;
			}
			throw daw::parser::invalid_input_exception{};
		case 'D':
			if( str == "DELETE" ) {
				return daw::request_method::DELETE;
			}
			throw daw::parser::invalid_input_exception{};
		case 'T':
			if( str == "TRACE" ) {
				return daw::request_method::TRACE;
			}
			throw daw::parser::invalid_input_exception{};
		case 'C':
			if( str == "CONNECT" ) {
				return daw::request_method::CONNECT;
			}
			throw daw::parser::invalid_input_exception{};
		default:
			throw daw::parser::invalid_input_exception{};
		}
	}

	constexpr http_version parse_to_value( daw::string_view str, ::daw::http_version ) {
		if( str.size( ) < 5 || str.substr( 0, 5 ) != "HTTP/" ) {
			throw daw::parser::invalid_input_exception{};
		}
		str.remove_prefix( 5 );
		if( str.size( ) < 3 ) {
			throw daw::parser::invalid_input_exception{};
		}
		return http_version{static_cast<uint_fast8_t>( str[0] - '0' ), static_cast<uint_fast8_t>( str[2] - '0' )};
	}

	struct unquoted_string_view {};

	constexpr daw::string_view parse_to_value( daw::string_view str, unquoted_string_view ) noexcept {
		return str;
	}
} // namespace daw

