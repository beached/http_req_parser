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

#include <daw/daw_exceptions.h>
#include <daw/daw_parser_helper.h>

namespace daw {
	template<typename RandomIterator, typename CharT = typename std::iterator_traits<RandomIterator>::value_type>
	struct percent_decode_view {
		using value_type = CharT;
		using difference_type = typename std::iterator_traits<RandomIterator>::difference_type; 
		using size_type = size_t;
		using iterator = RandomIterator;

	private:
		RandomIterator m_first;
		RandomIterator m_last;

		static constexpr value_type decode_digit( value_type b ) {
			if( b < 10 ) {
				return static_cast<value_type>( b - '0' );
			}
			if( b >= 'A' && b <= 'F' ) {
				return static_cast<value_type>( b - 'A' );
			}
			return static_cast<value_type>( b - 'a' );
		}

		static constexpr value_type decode_digits( value_type hb, value_type lb ) {
			daw::exception::daw_throw_on_false( daw::parser::is_hex( hb ) && daw::parser::is_hex( lb ),
			                                    "Expected	hex digits but item is out of range for hex" );
			return ( decode_digit( hb ) << 8 ) | decode_digit( lb );
		}

	public:
		constexpr percent_decode_view( iterator first, iterator last )
		  : m_first{std::move( first )}, m_last{std::move( last )} {
			daw::exception::daw_throw_on_false( first <= last, "Invalid range.  first must preceed last" );
		}

		constexpr value_type front( ) const {
			if( *m_first != '%' ) {
				return *m_first;
			}
			daw::exception::daw_throw_on_false( std::distance( first, last ) >= 3,
			                                    "Percent encoding requested but insufficient data to complete" );

			// Do decoding
			return decode_digits( *std::next( first, 1 ), *std::next( first, 2 ) );
		}

		constexpr void remove_prefix( ) {
			if( m_first == m_last ) {
				return; // fail softly, may not be correct decision
			}
			if( *m_first != '%' ) {
				++m_first;
				return;
			}
			daw::exception::daw_throw_on_false( std::distance( first, last ) >= 3,
			                                    "Percent encoding requested but insufficient data to complete" );
			std::advance( m_first, 3 );
		}

		constexpr void remove_prefix( size_type const count ) {
			for( size_t n=0; n<count; ++n ) {
				remove_prefix( );
			}
		}

		constexpr iterator begin( ) const {
			return m_first;
		}

		constexpr iterator end( ) const {
			return m_last;
		}

		constexpr value_type operator[]( size_type const pos ) const {
			percent_decode_view tmp{*this};
			tmp.remove_prefix( pos );
			return tmp;
		}
	};
} // namespace daw

