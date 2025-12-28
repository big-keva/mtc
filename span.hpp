# if !defined( __mtc_span_hpp__ )
# define __mtc_span_hpp__

# if ( __cplusplus >= 202000L )
# include <span>

namespace mtc
{
  template <class T>
  using span = std::span<T>;
}

# else
# include <type_traits>
# include <stdexcept>
# include <algorithm>
# include <vector>

namespace mtc
{

  template <class T>
  class span
  {
  public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer	= const T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = pointer;
    using const_iterator = const_pointer;

  public:
    constexpr span() noexcept = default;
    constexpr span( pointer p, size_type n ) noexcept: m_data( p ), m_size( n ) {}
    constexpr span( pointer p, pointer e ) noexcept: m_data( p ), m_size( e - p ) {}
  template <std::size_t N>
    constexpr span( value_type (&a)[N] ) noexcept: m_data( a ), m_size( N ) {}
  template <class Allocator>
    constexpr span( const std::vector<value_type, Allocator>& v ) noexcept: m_data( v.data() ), m_size( v.size() ) {}
    constexpr span( const std::initializer_list<value_type>& i ) noexcept: m_data( i.data() ), m_size( i.size() ) {}
    constexpr span( const span& s ) noexcept: m_data( s.m_data ), m_size( s.m_size ) {}
    constexpr span( span&& s ) noexcept: m_data( s.m_data ), m_size( s.m_size ) {  s.m_data = nullptr;  s.m_size = 0;  }

    span& operator=( const span& s ) noexcept {  return m_data = s.m_data, m_size = s.m_size, *this;  }

    constexpr iterator begin() noexcept {  return m_data;  }
    constexpr iterator end() noexcept {  return m_data + m_size;  }
    constexpr const_iterator begin() const noexcept {  return m_data;  }
    constexpr const_iterator end() const noexcept {  return m_data + m_size;  }
    constexpr const_iterator cbegin() const noexcept {  return m_data;  }
    constexpr const_iterator cend() const noexcept {  return m_data + m_size;  }

    constexpr reference front() const noexcept {  return *m_data;  }
    constexpr reference back() const noexcept {  return m_data[m_size - 1];  }
    constexpr reference at( size_type pos ) const {  return pos < m_size ? m_data[pos] : throw std::out_of_range( "index out of range" );  }

    constexpr reference operator[]( size_type pos ) const noexcept {  return at( pos );  }

    constexpr pointer data() const noexcept { return m_data;  }
    constexpr size_type size() const noexcept { return m_size;  }
    constexpr size_type size_bytes() const noexcept {  return m_size * sizeof(value_type);  }
    constexpr bool empty() const noexcept {  return m_size == 0;  }

    constexpr span first( size_type count ) const noexcept
      {  return { m_data, std::min( m_size, count ) };  }
    constexpr span last( size_type count ) const noexcept
      {  return m_size >= count ? span{ m_data + m_size - count, m_size - count } : span();  }
    constexpr span subspan( size_type off, size_t count ) const noexcept
      {  return off < m_size ? span{ m_data + off, std::min( off + count, m_size - off ) } : span();  }

  protected:
    pointer   m_data = nullptr;
    size_type m_size = 0;

  };

}

# endif

# endif   // !__mtc_span_hpp__
