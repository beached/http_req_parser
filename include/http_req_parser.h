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

#include "daw_parsing.h"

namespace daw {
	namespace http {
		namespace char_sets {
			using namespace daw::parsing;
			// one_of str or char
			using dot = chr<'.'>;

			struct alpha {
				using check_type = any_of<chr_rng<'a', 'z'>, chr_rng<'A', 'Z'>>;
				static CONSTEXPR bool check( char const c ) noexcept {
					return check_type::check( c );
				}
				static CONSTEXPR test_result check( daw::string_view const str ) noexcept {
					return check_str<check_type>( str );
				}
			};

			using digit = chr_rng<'0', '9'>;
			using safe = chr_set<'$', '-', '_', '@', '.', '&', '+', '-'>;
			using extra = chr_set<'!', '*', '"', '/', '(', ')', ','>;
			using hex = any_of<chr_rng<'a', 'f'>, chr_rng<'A', 'F'>, chr_rng<'0', '9'>>;
			using reserved = chr_set<'=', ';', '/', '#', '?', ':', ' '>;
			using alphanum2 = any_of<alpha, digit, chr_set<'-', '_', '.', '+'>>;

			using escape = sequence<chr<'%'>, hex, hex>;

			struct xalpha {
				using check_type = any_of<alpha, digit, safe, extra, escape>;
				static CONSTEXPR bool check( char const c ) noexcept {
					return check_type::check( c );
				}
				static CONSTEXPR test_result check( daw::string_view const str ) noexcept {
					return check_str<check_type>( str );
				}
			};
			using xpalpha = any_of<xalpha, chr<'+'>>;

			// str only
			using ialpha = sequence<alpha, zero_or_more<xalpha>>;
			using xpalphas = one_or_more<xpalpha>;
			using xalphas = one_or_more<xalpha>;

			using hostname = sequence<ialpha, zero_or_more<sequence<dot, ialpha>>>;
			using hostnumber = sequence<digit, repeat_n<3, sequence<dot, digit>>>;
			using portnumber = at_least_up_to<1, 5, digit>;
			using segment = xpalphas;
			using username = one_or_more<alphanum2>;
			using password = one_or_more<alphanum2>;

			// high level
			using scheme = parse_parts<ialpha, chr_seq<':', '/', '/'>>;
			using userinfo = parse_parts<username, chr<':'>, password, chr<'@'>>;
			using host = parse_parts<one_of<hostname, hostnumber>>;
			using port = parse_parts<chr<':'>, portnumber>;
			using path = parse_parts<zero_or_more<sequence<chr<'/'>, segment>>>;
			using query = parse_parts<chr<'?'>, sequence<xalphas, zero_or_more<sequence<chr<'+'>, xalphas>>>>;
			using fragment = parse_parts<chr<'#'>, xpalphas>;
		} // namespace char_sets

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
			CONSTEXPR double full( ) const noexcept {
				return static_cast<double>( ver_major ) + ( static_cast<double>( ver_minor ) / 10.0 );
			}
		};

		struct http_url_auth_info {
			daw::string_view username;
			daw::string_view password;
		};

		struct http_uri {
			daw::string_view scheme;
			http_url_auth_info auth;
			daw::string_view host;
			uint16_t port;
			daw::string_view path;
			daw::string_view query;
			daw::string_view fragment;

			CONSTEXPR http_uri( ) noexcept : scheme{}, auth{}, host{}, port{}, path{}, query{}, fragment{} {}

			CONSTEXPR http_uri( http_uri const &other ) noexcept
			  : scheme{other.scheme}
			  , auth{other.auth}
			  , host{other.host}
			  , port{other.port}
			  , path{other.path}
			  , query{other.query}
			  , fragment{other.fragment} {}

			CONSTEXPR http_uri( http_uri &&other ) noexcept
			  : scheme{std::move( other.scheme )}
			  , auth{std::move( other.auth )}
			  , host{std::move( other.host )}
			  , port{std::move( other.port )}
			  , path{std::move( other.path )}
			  , query{std::move( other.query )}
			  , fragment{std::move( other.fragment )} {}

			CONSTEXPR http_uri &operator=( http_uri const &rhs ) noexcept {
				if( this != &rhs ) {
					scheme = rhs.scheme;
					auth = rhs.auth;
					host = rhs.host;
					port = rhs.port;
					path = rhs.path;
					query = rhs.query;
					fragment = rhs.fragment;
				}
				return *this;
			}

			CONSTEXPR http_uri &operator=( http_uri &&rhs ) noexcept {
				if( this != &rhs ) {
					scheme = std::move( rhs.scheme );
					auth = std::move( rhs.auth );
					host = std::move( rhs.host );
					port = std::move( rhs.port );
					path = std::move( rhs.path );
					query = std::move( rhs.query );
					fragment = std::move( rhs.fragment );
				}
				return *this;
			}

			~http_uri( ) noexcept = default;

			CONSTEXPR http_uri( daw::string_view s, http_url_auth_info a, daw::string_view h, uint16_t p, daw::string_view pa,
			                    daw::string_view q, daw::string_view f ) noexcept
			  : scheme{std::move( s )}
			  , auth{std::move( a )}
			  , host{std::move( h )}
			  , port{std::move( p )}
			  , path{std::move( pa )}
			  , query{std::move( q )}
			  , fragment{std::move( f )} {}
		};

		struct http_request {
			request_method method;
			http_uri uri;
			http_version version;

			CONSTEXPR http_request( ) noexcept : method{}, uri{}, version{} {}

			CONSTEXPR http_request( http_request const &other ) noexcept
			  : method{other.method}, uri{other.uri}, version{other.version} {}

			CONSTEXPR http_request( http_request &&other ) noexcept
			  : method{std::move( other.method )}, uri{std::move( other.uri )}, version{std::move( other.version )} {}

			CONSTEXPR http_request &operator=( http_request const &rhs ) noexcept {
				if( this != &rhs ) {
					method = rhs.method;
					uri = rhs.uri;
					version = rhs.version;
				}
				return *this;
			}

			CONSTEXPR http_request &operator=( http_request &&rhs ) noexcept {
				if( this != &rhs ) {
					method = std::move( rhs.method );
					uri = std::move( rhs.uri );
					version = std::move( rhs.version );
				}
				return *this;
			}

			~http_request( ) noexcept = default;
			CONSTEXPR http_request( request_method m, http_uri u, http_version v ) noexcept
			  : method{std::move( m )}, uri{std::move( u )}, version{std::move( v )} {}
		};

		CONSTEXPR request_method parse_to_value( daw::string_view str, request_method ) {
			switch( AsciiUpper( str.front( ) ) ) {
			case 'O':
				if( str == "OPTIONS" ) {
					return request_method::OPTIONS;
				}
				throw daw::parser::invalid_input_exception{};
			case 'G':
				if( str == "GET" ) {
					return request_method::GET;
				}
				throw daw::parser::invalid_input_exception{};
			case 'H':
				if( str == "HEAD" ) {
					return request_method::HEAD;
				}
				throw daw::parser::invalid_input_exception{};
			case 'P':
				if( str == "POST" ) {
					return request_method::POST;
				}
				if( str == "PUT" ) {
					return request_method::PUT;
				}
				throw daw::parser::invalid_input_exception{};
			case 'D':
				if( str == "DELETE" ) {
					return request_method::DELETE;
				}
				throw daw::parser::invalid_input_exception{};
			case 'T':
				if( str == "TRACE" ) {
					return request_method::TRACE;
				}
				throw daw::parser::invalid_input_exception{};
			case 'C':
				if( str == "CONNECT" ) {
					return request_method::CONNECT;
				}
				throw daw::parser::invalid_input_exception{};
			default:
				throw daw::parser::invalid_input_exception{};
			}
		}

		CONSTEXPR http_version parse_to_value( daw::string_view str, http_version ) {
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

		CONSTEXPR daw::string_view parse_to_value( daw::string_view str, unquoted_string_view ) noexcept {
			return str;
		}

		CONSTEXPR http_url_auth_info parse_to_value( daw::string_view str, http_url_auth_info ) {
			{
				auto const end_pos = str.find( '@' );
				if( end_pos >= str.size( ) ) {
					return http_url_auth_info{};
				}
				str = str.substr( 0, end_pos );
			}
			auto const mid_pos = str.find( ':' );
			if( mid_pos >= str.size( ) ) {
				return http_url_auth_info{str, {}};
			}
			return http_url_auth_info{str.substr( 0, mid_pos ), str.substr( mid_pos + 1 )};
		}

		struct http_url_port {};
		CONSTEXPR uint16_t parse_to_value( daw::string_view str, http_url_port ) {
			return daw::parser::converters::helpers::parse_unsigned_int<uint16_t>( str );
		}

		namespace impl {
			CONSTEXPR daw::string_view parse_scheme( daw::string_view &str, bool req ) {
				auto parse_result = char_sets::scheme::check( str );
				if( !( std::get<0>( parse_result ) && std::get<0>( parse_result ) ) ) {
					if( req ) {
						throw daw::parser::invalid_input_exception{};
					}
					return daw::string_view{};
				}
				auto result = str.substr( 0, std::get<0>( parse_result ).last );
				str.remove_prefix( std::get<1>( parse_result ).last );
				return result;
			}

			CONSTEXPR http_url_auth_info parse_auth_info( daw::string_view &str ) {
				auto const auth_end = str.find( '@' );
				if( auth_end != str.npos ) {
					auto auth_info = parse_to_value( str.substr( 0, auth_end ), http_url_auth_info{} );
					str.remove_prefix( auth_end + 1 );
					return auth_info;
				}
				return http_url_auth_info{};
			}

			struct hostinfo_t {
				daw::string_view hostname;
				uint16_t port;
			};

			CONSTEXPR daw::string_view parse_hostname( daw::string_view &str, bool req ) {
				if( !req && !str.empty( ) ) {
					return daw::string_view{};
				}
				auto parse_item = char_sets::host::check( str );

				if( !std::get<0>( parse_item ) ) {
					throw daw::parser::invalid_input_exception{};
				}
				auto result = str.substr( 0, std::get<0>( parse_item ).last );
				str.remove_prefix( std::get<0>( parse_item ).last );
				return result;
			}

			CONSTEXPR uint16_t parse_port( daw::string_view &str ) {
				if( str.empty( ) || str.front( ) == '/' ) {
					return 80;
				}
				auto const port_end = str.find( '/' );
				auto port = parse_to_value( str.substr( 0, port_end ), http_url_port{} );
				str.remove_prefix( port_end );
				return port;
			}

			CONSTEXPR hostinfo_t parse_hostinfo( daw::string_view &str, bool req ) {
				hostinfo_t result{parse_hostname( str, req ), 80};
				result.port = parse_port( str );
				return result;
			}

			CONSTEXPR daw::string_view parse_path( daw::string_view &str ) {
				auto const parse_result = char_sets::path::check( str );
				if( !std::get<0>(parse_result) ) {
					throw daw::parser::invalid_input_exception{};
				}
				auto path_result = str.substr( 0, std::get<0>(parse_result).last );
				str.remove_prefix( std::get<0>(parse_result).last );
				return path_result;
			}

			CONSTEXPR daw::string_view parse_query( daw::string_view &str ) {
				auto const query_end = str.find( '#' );
				auto query = str.substr( 0, query_end );
				str.remove_prefix( query_end );
				if( query_end != str.npos ) {
					str.remove_prefix( );
				}
				return query;
			}
		} // namespace impl

		CONSTEXPR http_uri parse_to_value( daw::string_view str, http_uri ) {
			auto scheme = impl::parse_scheme( str, false );
			auto auth_info = impl::parse_auth_info( str );
			auto host_info = impl::parse_hostinfo( str, !scheme.empty( ) );
			auto path = impl::parse_path( str );
			auto query = impl::parse_query( str );
			return http_uri{std::move( scheme ), std::move( auth_info ), std::move( host_info.hostname ),
			                host_info.port,      std::move( path ),      std::move( query ),
			                std::move( str )};
		}
	} // namespace http
} // namespace daw
