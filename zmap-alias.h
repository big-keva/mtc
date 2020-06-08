# pragma once
# if !defined( __zmap_alias_h__ )
# define __zmap_alias_h__
# include "zmap.h"

# define  zmap_view( name )                                       \
  class name: public mtc::zmap                                    \
  {                                                               \
    using mtc::zmap::zmap;                                        \
                                                                  \
  protected:                                                      \
    template <class T>                                            \
    class array_view                                              \
    {                                                             \
      const T*  ptr;                                              \
      size_t    len;                                              \
    public:                                                       \
      array_view() noexcept: ptr( nullptr ), len( 0 ) {}          \
      array_view( const T* p, size_t l ) noexcept: ptr( p ), len( l ) {}  \
      array_view( const array_view& v ) noexcept: ptr( v.ptr ), len( v.len ) {} \
      array_view( const std::vector<T>& v ): array_view( v.data(), v.size() ) {}  \
      const T&  operator[]( size_t i ) const noexcept {  return ptr[i];  }  \
      const T&  at( size_t i ) const noexcept {  return ptr[i];  }\
      size_t    size() const noexcept {  return len;  }           \
      auto  begin() noexcept -> const T* {  return ptr;  }        \
      auto  end() noexcept -> const T* {  return ptr + len;  }    \
      auto  front() const -> const T&  {  return *ptr;  }         \
      auto  back() const -> const T&  {  return ptr[len - 1];  }  \
    };                                                            \
  public:                                                         \
    name() = default;                                             \
    name( const mtc::zmap& z ): mtc::zmap( z ) {}                 \
    auto  operator = ( const name& z ) -> name& {  return mtc::zmap::operator=( z ), *this;  } \
    auto  operator = ( const mtc::zmap& z ) -> name& {  return mtc::zmap::operator=( z ), *this;  }

  # define  __zmap_impl_alias__( name, ctyp, ztyp, key )        \
    public:                                                     \
    auto  name() const -> ctyp                                  \
    {                                                           \
      return get_##ztyp( key, ctyp() );                         \
    }                                                           \
    auto  name( ) -> ctyp&                                      \
    {                                                           \
      auto  pval = get_##ztyp( key );                           \
      return pval != nullptr ? *pval : *set_##ztyp( key, ctyp() );\
    }

  # define  zmap_alias_float( name, key ) __zmap_impl_alias__( name, float, float, key )
  # define  zmap_alias_double( name, key ) __zmap_impl_alias__( name, double, double, key )
  # define  zmap_alias_int32( name, key ) __zmap_impl_alias__( name, int32_t, int32, key )
  # define  zmap_alias_int64( name, key ) __zmap_impl_alias__( name, int64_t, int64, key )
  # define  zmap_alias_uint32( name, key ) __zmap_impl_alias__( name, uint32_t, word32, key )
  # define  zmap_alias_uint64( name, key ) __zmap_impl_alias__( name, uint64_t, word64, key )

  # define  zmap_alias_charstr( name, key ) __zmap_impl_alias__( name, mtc::charstr, charstr, key )
  # define  zmap_alias_widestr( name, key ) __zmap_impl_alias__( name, mtc::widestr, widestr, key )

  # define  zmap_alias_struct( name, type, key )                          \
    public:                                                               \
    auto  name() const -> type                                            \
    {                                                                     \
      auto  pval = get_zmap( key );                                       \
      return pval != nullptr ? *pval : type();                            \
    }                                                                     \
    auto  name() -> type&                                                 \
    {                                                                     \
      auto  pval = get_zmap( key );                                       \
      return pval != nullptr ? (type&)*pval : (type&)*set_zmap( key );    \
    }

  # define  __zmap_impl_array_alias__( name, ctyp, ztyp, key )            \
    public:                                                               \
    auto  name() const -> array_view<ctyp>                                \
    {                                                                     \
      auto  pval = get_array_##ztyp( key );                               \
      return pval != nullptr ? array_view<ctyp>( *(std::vector<ctyp>*)pval ) : array_view<ctyp>();\
    }                                                                     \
    auto  name() -> std::vector<ctyp>&                                    \
    {                                                                     \
      auto  pval = get_array_##ztyp( key );                               \
      return pval != nullptr ? *(std::vector<ctyp>*)pval : *(std::vector<ctyp>*)set_array_##ztyp( key );  \
    }

  # define  zmap_alias_array_float( name, key )  __zmap_impl_array_alias__( name, float, float, key )
  # define  zmap_alias_array_double( name, key )  __zmap_impl_array_alias__( name, double, double, key )

  # define  zmap_alias_array_int32( name, key )  __zmap_impl_array_alias__( name, int32_t, int32, key )
  # define  zmap_alias_array_int64( name, key )  __zmap_impl_array_alias__( name, int64_t, int64, key )
  # define  zmap_alias_array_uint32( name, key )  __zmap_impl_array_alias__( name, uint32_t, word32, key )
  # define  zmap_alias_array_uint64( name, key )  __zmap_impl_array_alias__( name, uint64_t, word64, key )

  # define  zmap_alias_array_struct( name, type, key )  __zmap_impl_array_alias__( name, type, zmap, key )

# define  zmap_end  };

# endif // !__zmap_alias_h__
