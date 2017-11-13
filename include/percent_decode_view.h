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

#include <string>

#include <daw/daw_exception.h>
#include <daw/daw_parser_helper.h>

namespace daw {
	template<typename BidirectionalIterator,
	         typename CharT = typename std::iterator_traits<BidirectionalIterator>::value_type>
	struct percent_decode_iterator {
		using value_type = CharT;
		using iterator = BidirectionalIterator;
		using difference_type = typename std::iterator_traits<BidirectionalIterator>::difference_type;
		using size_type = size_t;
		using pointer = typename std::iterator_traits<BidirectionalIterator>::pointer;
		using reference = typename std::iterator_traits<BidirectionalIterator>::reference;
		using iterator_category = std::bidirectional_iterator_tag;

	private:
		iterator m_first;

		static constexpr char decode_digit( char b ) {
			if( '0' <= b && b <= '9' ) {
				return static_cast<value_type>( b - '0' );
			}
			if( 'A' <= b && b <= 'F' ) {
				return static_cast<value_type>( (b - 'A') + 10 );
			}
			if( 'a' <= b && b <= 'f' ) {
				return static_cast<value_type>( (b - 'a') + 10 );
			}
			daw::exception::daw_throw( "Expected hex digit but item is out of range for hex" );
		}

		static constexpr char decode_digits( char hb, char lb ) {
			hb = decode_digit( hb );
			lb = decode_digit( lb );
			char result = (hb << 4)|lb;
			return result;
		}

		constexpr void increment( ) noexcept( noexcept( *( ++m_first ) ) ) {
			if( *m_first != '%' ) {
				++m_first;
				return;
			}
			++m_first;
			++m_first;
			++m_first;
		}

		constexpr void decrement( ) noexcept( noexcept( *( --m_first ) ) ) {
			--m_first;
			iterator tmp = m_first;
			--tmp;
			--tmp;
			if( *tmp == '%' ) {
				m_first = tmp;
			}
		}

	public:
		constexpr percent_decode_iterator( ) = delete;
		~percent_decode_iterator( ) = default;
		constexpr percent_decode_iterator( percent_decode_iterator const & ) = default;
		constexpr percent_decode_iterator( percent_decode_iterator && ) = default;
		constexpr percent_decode_iterator &operator=( percent_decode_iterator const & ) = default;
		constexpr percent_decode_iterator &operator=( percent_decode_iterator && ) = default;

		explicit constexpr percent_decode_iterator( iterator it ) noexcept : m_first{std::move( it )} {}

		constexpr percent_decode_iterator &
		operator++( ) noexcept( noexcept( std::declval<percent_decode_iterator>( ).increment( ) ) ) {
			increment( );
			return *this;
		}

		constexpr percent_decode_iterator &
		operator--( ) noexcept( noexcept( std::declval<percent_decode_iterator>( ).decrement( ) ) ) {
			decrement( );
			return *this;
		}

		constexpr percent_decode_iterator &operator++(int)const
		  noexcept( noexcept( std::declval<percent_decode_iterator>( ).increment( ) ) ) {

			percent_decode_iterator tmp{*this};
			tmp.increment( );
			return tmp;
		}

		constexpr percent_decode_iterator &operator--(int)const
		  noexcept( noexcept( std::declval<percent_decode_iterator>( ).decrement( ) ) ) {

			percent_decode_iterator tmp{*this};
			tmp.decrement( );
			return tmp;
		}

		constexpr value_type operator*( ) const {
			if( *m_first != '%' ) {
				return *m_first;
			}
			// Do decoding
			auto result = decode_digits( *std::next( m_first, 1 ), *std::next( m_first, 2 ) );
			return result;
		}

		constexpr value_type operator->( ) const {
			return operator*( );
		}

		constexpr bool can_decode( BidirectionalIterator last ) noexcept {
			if( m_first == last ) {
				return false;
			}
			if( *m_first != '%' ) {
				return true;
			}
			return std::distance( m_first, last ) >= 3;
		}

		constexpr int compare( BidirectionalIterator rhs ) const noexcept {
			if( m_first < rhs ) {
				return -1;
			}
			if( m_first == rhs ) {
				return 0;
			}
			return 1;
		}

		constexpr int compare( percent_decode_iterator rhs ) const noexcept {
			if( m_first < rhs.m_first ) {
				return -1;
			}
			if( m_first == rhs.m_first ) {
				return 0;
			}
			return 1;
		}
	};

	template<typename Iterator, typename CharT = typename std::iterator_traits<Iterator>::value_type>
	auto make_percent_decode_iterator( Iterator it ) {
		return percent_decode_iterator<Iterator, CharT>{it};
	}

	template<typename Iterator, typename CharT, typename ForwardIterator>
	constexpr bool operator==( percent_decode_iterator<Iterator, CharT> const &lhs,
	                           ForwardIterator const &rhs ) noexcept {
		return lhs.compare( rhs ) == 0;
	}

	template<typename Iterator, typename CharT, typename ForwardIterator>
	constexpr bool operator!=( percent_decode_iterator<Iterator, CharT> const &lhs,
	                           ForwardIterator const &rhs ) noexcept {
		return lhs.compare( rhs ) != 0;
	}

	template<typename Iterator, typename CharT, typename ForwardIterator>
	constexpr bool operator<( percent_decode_iterator<Iterator, CharT> const &lhs, ForwardIterator const &rhs ) noexcept {
		return lhs.compare( rhs ) < 0;
	}

	template<typename Iterator, typename CharT, typename ForwardIterator>
	constexpr bool operator<=( percent_decode_iterator<Iterator, CharT> const &lhs,
	                           ForwardIterator const &rhs ) noexcept {
		return lhs.compare( rhs ) <= 0;
	}

	template<typename Iterator, typename CharT, typename ForwardIterator>
	constexpr bool operator>( percent_decode_iterator<Iterator, CharT> const &lhs, ForwardIterator const &rhs ) noexcept {
		return lhs.compare( rhs ) > 0;
	}

	template<typename Iterator, typename CharT, typename ForwardIterator>
	constexpr bool operator>=( percent_decode_iterator<Iterator, CharT> const &lhs,
	                           ForwardIterator const &rhs ) noexcept {
		return lhs.compare( rhs ) >= 0;
	}

	/// A rage view of the underlying range (first, last] with percent decoding.  It supports random access
	/// operations but not efficiently.  use the while( !view.empty( ) ) { ...view.front( ); ... } idiom as the
	/// underlying iterators are BidirectionalIterators
	template<typename ForwardIterator, typename CharT = typename std::iterator_traits<ForwardIterator>::value_type>
	struct percent_decode_view {
		using value_type = CharT;
		using difference_type = typename std::iterator_traits<ForwardIterator>::difference_type;
		using size_type = size_t;
		using iterator = percent_decode_iterator<ForwardIterator, CharT>;

	private:
		iterator m_first;
		ForwardIterator m_last;

	public:
		constexpr percent_decode_view( ForwardIterator first, ForwardIterator last )
		  : m_first{std::move( first )}, m_last{std::move( last )} {
			daw::exception::daw_throw_on_false( first <= last, "Invalid range.  first must preceed last" );
		}

		constexpr value_type front( ) const {
			daw::exception::daw_throw_on_false( m_first.can_decode( m_last ),
			                                    "Percent encoding requested but insufficient data to complete" );
			return *m_first;
		}

		constexpr void remove_prefix( ) {
			if( !m_first.can_decode( m_last ) ) {
				return; // fail softly, may not be correct decision
			}
			++m_first;
		}

		constexpr void remove_prefix( size_type const count ) {
			for( size_t n = 0; n < count; ++n ) {
				remove_prefix( );
			}
		}

		constexpr iterator begin( ) const {
			return m_first;
		}

		constexpr iterator end( ) const {
			return iterator{m_last};
		}

		constexpr value_type operator[]( size_type const pos ) const {
			percent_decode_view tmp{*this};
			tmp.remove_prefix( pos );
			return tmp;
		}

		constexpr bool empty( ) const noexcept {
			return m_first == m_last;
		}

		constexpr size_t size( ) const noexcept {
			auto tmp = m_first;
			size_t sz = 0;
			while( tmp != m_last ) {
				++tmp;
				++sz;
			}
			return sz;
		}

		operator std::basic_string<value_type>( ) const {
			std::basic_string<value_type> result{};
			std::copy( begin( ), end( ), std::back_inserter( result ) );
			return result;
		}
	};

	template<typename Iterator, typename CharT = typename std::iterator_traits<Iterator>::value_type>
	auto make_percent_decode_view( Iterator first, Iterator last ) {
		return percent_decode_view<Iterator, CharT>{first, last};
	}

} // namespace daw
