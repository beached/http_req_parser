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

#include <daw/daw_string_view.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>

#define CONSTEXPR

namespace daw {
	namespace parsing {
		struct test_result {
			size_t first;
			size_t last;
			bool found;

			explicit CONSTEXPR operator bool( ) const {
				return found;
			}
		};

		template<typename Range>
		constexpr test_result check_str( daw::string_view str ) noexcept( noexcept( Range::check( char{} ) ) ) {
			test_result result{0, 0, ( !str.empty( ) || Range::check( str[0] ) )};
			if( !result ) {
				return result;
			}
			for( size_t n = 1; n < str.size( ); ++n ) {
				if( !Range::check( str[n] ) ) {
					result.last = n - 1;
					return result;
				}
			}
			return test_result{0, str.size( ), true};
		}

		// Range of characters - Max true if c is withing range (first, last)
		template<char first, char last>
		struct chr_rng {
			static_assert( first <= last, "first in range must be <= last" );

			static CONSTEXPR bool check( char const c ) noexcept {
				return first <= c && c <= last;
			}
			static CONSTEXPR test_result check( daw::string_view const str ) noexcept {
				return check_str<chr_rng>( str );
			}
		};

		// Specific character - true if c is equal to item
		template<char item>
		struct chr {
			static CONSTEXPR bool check( char const c ) noexcept {
				return c == item;
			}
			static CONSTEXPR test_result check( daw::string_view const str ) noexcept {
				return check_str<chr>( str );
			}
		};
		namespace impl {
			template<char item>
			CONSTEXPR bool are_equal( char const c ) noexcept {
				return item == c;
			}
			template<char item, char... items, std::enable_if_t<( sizeof...( items ) != 0 ), std::nullptr_t> = nullptr>
			CONSTEXPR bool are_equal( char const c ) noexcept {
				return c == item || are_equal<items...>( c );
			}
		} // namespace impl

		template<char... items>
		struct chr_set {
			static CONSTEXPR bool check( char const c ) noexcept {
				return impl::are_equal<items...>( c );
			}
			static CONSTEXPR test_result check( daw::string_view const str ) noexcept {
				return check_str<chr_set>( str );
			}
		};
		namespace impl {
			template<size_t N, char item>
			CONSTEXPR bool check_str( daw::string_view const str ) noexcept {
				return str[N] == item;
			}

			template<size_t N, char item, char... items,
			         std::enable_if_t<( sizeof...( items ) > 0 ), std::nullptr_t> = nullptr>
			CONSTEXPR bool check_str( daw::string_view const str ) noexcept {
				return str[N] == item && check_str<N + 1, items...>( str );
			}
		} // namespace impl

		// Continue until Range is true
		template<typename Range>
		struct until {
			static CONSTEXPR bool check( char const c ) noexcept {
				return !Range::check( c );
			}
			static CONSTEXPR test_result check( daw::string_view const str ) noexcept {
				return check_str<until<Range>>( str );
			}
		};

		// Match Any character
		struct any {
			static CONSTEXPR bool check( char const ) noexcept {
				return true;
			}
		};

		namespace impl {
			template<typename Head, typename... Tails,
			         std::enable_if_t<( sizeof...( Tails ) == 0 ), std::nullptr_t> = nullptr>
			CONSTEXPR bool check_in_range( char const c ) noexcept {
				return Head::check( c );
			}

			template<typename Head, typename... Tails,
			         std::enable_if_t<( sizeof...( Tails ) != 0 ), std::nullptr_t> = nullptr>
			CONSTEXPR bool check_in_range( char const c ) noexcept {
				return Head::check( c ) || check_in_range<Tails...>( c );
			}
		} // namespace impl

		// true while any of the ranges is true.  Like Logical Or
		template<typename... Ranges>
		struct any_of {
			static CONSTEXPR bool check( char const c ) noexcept {
				return impl::check_in_range<Ranges...>( c );
			}
			static CONSTEXPR test_result check( daw::string_view const str ) noexcept {
				return check_str<any_of<Ranges...>>( str );
			}
		};

		template<typename Range>
		CONSTEXPR size_t find_end_of_range( daw::string_view str ) noexcept {
			static_assert( daw::is_detected_v<decltype( Range::check( char{} ) )>,
			               "Supplied range does not support character check" );
			size_t n = 0;
			for( ; n < str.size( ) && Range::check( str[n] ); ++n ) {
			}
			return n;
		}

		// Whatever the result of Range, negate it
		template<typename Range>
		struct neg {
			static CONSTEXPR bool check( char const c ) noexcept {
				return !Range::check( c );
			}
			static CONSTEXPR test_result check( daw::string_view const str ) noexcept {
				return !Range::check( str );
			}
		};

		// The Range repeats N times
		// It may be true afterwards, but isn't checked
		template<size_t N, typename Range>
		struct repeat_n {
			static CONSTEXPR test_result check( daw::string_view str ) noexcept {
				test_result result{0, 0, false};
				for( size_t n = 0; n < N; ++n ) {
					auto result2 = Range::check( str );
					if( !result2 ) {
						return {0, 0, false};
					}
					result.found = true;
					result.last += result2.last;
					str.remove_prefix( result2.last );
					if( n < N && str.empty( ) ) {
						return {0, 0, false};
					}
				}
				return result;
			}
		};

		// At least Min must be true up to Max
		// There may be more Ranges true after Max
		template<size_t Min, size_t Max, typename Range>
		struct at_least_up_to {
			static_assert( Min <= Max, "Min cannot be greater than Max" );
			static CONSTEXPR test_result check( daw::string_view str ) noexcept {
				test_result result = repeat_n<Min, Range>::check( str );
				if( !result ) {
					return result;
				}
				for( size_t n = 0; n < ( Max - Min ); ++n ) {
					auto result2 = Range::check( str );
					if( !result2 ) {
						return result;
					}
					result.found = true;
					result.last += result2.last;
					str.remove_prefix( result2.last );
					if( str.empty( ) ) {
						return result;
					}
				}
				return result;
			}
		};

		// Match at either none or up to 1
		template<typename Range>
		using one_or_none = at_least_up_to<0, 1, Range>;

		// Sequence - All must be true
		template<typename...>
		struct sequence;

		template<typename Range, typename... Ranges>
		struct sequence<Range, Ranges...> {
			static CONSTEXPR test_result check( daw::string_view const str ) noexcept {
				auto result = Range::check( str );
				if( result ) {
					auto result2 = sequence<Ranges...>::check( str.substr( result.last ) );
					result.last += result2.last;
				}
				return result;
			}
		};

		template<typename Range>
		struct sequence<Range> {
			static CONSTEXPR test_result check( daw::string_view const str ) noexcept {
				return Range::check( str );
			}
		};

		template<char... items>
		struct chr_seq {
			static CONSTEXPR test_result check( daw::string_view const str ) noexcept {
				using chr_seq = sequence<chr<items>...>;
				return chr_seq::check( str );
			}
		};

		// Will consume zero or more ranges, this is like kleene star
		template<typename Range>
		struct zero_or_more {
			static CONSTEXPR test_result check( daw::string_view str ) noexcept {
				auto result = Range::check( str );
				if( !result || result.last == 0 ) {
					return result;
				}
				str.remove_prefix( result.last );
				while( !str.empty( ) ) {
					auto result2 = Range::check( str );
					if( !result2 || result2.last = 0 ) {
						return result;
					}
					result.last += result2.last;
					str.remove_prefix( result2.last );
				}
				return result;
			}
		};

		template<size_t N, typename Range>
		using at_least_n = sequence<repeat_n<N, Range>, zero_or_more<Range>>;

		template<typename Range>
		using one_or_more = at_least_n<1, Range>;

		template<typename Lhs, typename Rhs>
		struct one_of {
			static CONSTEXPR test_result check( daw::string_view str ) noexcept {
				auto result = Lhs::check( str );
				if( !result ) {
					result = Rhs::check( str );
				}
				return result;
			}
		};

		namespace impl {
			template<size_t N, typename... Parts, typename TPrev,
			         std::enable_if_t<( ( N + 1 ) == sizeof...( Parts ) ), std::nullptr_t> = nullptr>
			CONSTEXPR auto parse( daw::string_view str, TPrev &&prev, size_t start_pos ) {
				using part_t = daw::type_n_t<sizeof...( Parts ) - 1, Parts...>;

				auto result = part_t::check( std::move( str ) );
				return std::tuple_cat( std::forward<TPrev>( prev ),
				                       std::make_tuple( test_result{start_pos, start_pos + result.last, result.found} ) );
			}

			template<size_t N, typename... Parts, typename TPrev,
			         std::enable_if_t<( ( N + 1 ) < sizeof...( Parts ) ), std::nullptr_t> = nullptr>
			CONSTEXPR auto parse( daw::string_view str, TPrev &&prev, size_t start_pos ) {
				using part_t = daw::type_n_t<N, Parts...>;

				auto result = part_t::check( str );

				auto tresult =
				  std::tuple_cat( std::forward<TPrev>( prev ),
				                  std::make_tuple( test_result{start_pos, start_pos + result.last, result.found} ) );
				if( !result ) {
					str.clear( );
				}
				str.remove_prefix( result.last );
				return parse<N + 1, Parts...>( std::move( str ), std::move( tresult ), result.last + start_pos );
			}

			template<typename... Parts>
			CONSTEXPR auto parse_parts( daw::string_view str ) {
				return parse<0, Parts...>( str, std::tuple<>{}, 0 );
			}
		} // namespace impl

		template<typename... Parts>
		struct parse_parts {
			static CONSTEXPR auto check( daw::string_view str ) {
				return impl::parse_parts<Parts...>( str );
			}
		};
	} // namespace parsing
} // namespace daw
