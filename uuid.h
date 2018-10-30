# pragma once
# if !defined( __mtc_uuid_h__ )
# define __mtc_uuid_h__
# include "serialize.decl.h"
# include <cstdint>

namespace mtc {

  class uuid
  {
    friend std::string  to_string( const uuid& );

    uint8_t ubytes[16];

  public:
    class parse_error: public std::runtime_error  {  using std::runtime_error::runtime_error;  };

  public:
    uuid()
      {  std::fill( std::begin( ubytes ), std::end( ubytes ), 0 );  }
    uuid( const uint8_t (&val)[16] )
      {  std::copy( std::begin( val ), std::end( val ), std::begin( ubytes ) );  }
    uuid( const uuid& rt )
      {  std::copy( std::begin( rt.ubytes ), std::end( rt.ubytes ), std::begin( ubytes ) );  }
    uuid& operator = ( const uuid& rt )
      {  return std::copy( std::begin( rt.ubytes ), std::end( rt.ubytes ), std::begin( ubytes ) ), *this;  }

  public:
    auto  data() const -> const uint8_t*  {  return ubytes;  }
    auto  size() const -> size_t          {  return 16;  }

  public:
    static  uuid  get_uuid( const char* s, const char* e = nullptr )  {  return get_uuid<char>( s, e );  }
    static  uuid  get_uuid( const widechar* s, const widechar* e = nullptr )  {  return get_uuid<widechar>( s, e );  }
    static  uuid  get_uuid( const charstr& s )   {  return get_uuid( s.c_str() );  }
    static  uuid  get_uuid( const widestr& s )   {  return get_uuid( s.c_str() );  }

    static  auto  parse_uuid( uuid& o, const char* s, const char* e = nullptr )  -> const char*  {  return parse_uuid<char>( o, s, e );  }
    static  auto  parse_uuid( uuid& o, const widechar* s, const widechar* e = nullptr )  -> const widechar*  {  return parse_uuid<widechar>( o, s, e );  }

  protected:
    template <class C>
    static  uuid      get_uuid( const C*, const C* );
    template <class C>
    static  const C*  parse_uuid( uuid&, const C*, const C* );

  public:
    bool  operator == ( const uuid& rt ) const  {  return compare( rt ) == 0;  }
    bool  operator != ( const uuid& rt ) const  {  return !(*this == rt);  }
    bool  operator <  ( const uuid& rt ) const  {  return compare( rt ) <  0;  }
    bool  operator <= ( const uuid& rt ) const  {  return compare( rt ) <= 0;  }
    bool  operator >  ( const uuid& rt ) const  {  return compare( rt ) >  0;  }
    bool  operator >= ( const uuid& rt ) const  {  return compare( rt ) >= 0;  }
    auto  compare( const uuid& rt ) const -> int
      {  return memcmp( ubytes, rt.ubytes, sizeof(ubytes) );  }

  };

  using uuid_t = uuid;

  inline  auto  to_string( const uuid_t& uuid ) -> std::string
    {
      return strprintf( "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid.ubytes[0x0], uuid.ubytes[0x1], uuid.ubytes[0x2], uuid.ubytes[0x3],
        uuid.ubytes[0x4], uuid.ubytes[0x5], uuid.ubytes[0x6], uuid.ubytes[0x7],
        uuid.ubytes[0x8], uuid.ubytes[0x9], uuid.ubytes[0xa], uuid.ubytes[0xb],
        uuid.ubytes[0xc], uuid.ubytes[0xd], uuid.ubytes[0xe], uuid.ubytes[0xf] );
    }

  template <class C>
  uuid  uuid::get_uuid( const C* s, const C* e )
    {
      uuid  uvalue;

      return parse_uuid( uvalue, s, e ), uvalue;
    }

  template <class C>
  const C*  uuid::parse_uuid( uuid& o, const C* s, const C* e )
    {
      auto  getchr = []( uint8_t& o, const C* s ) -> const C*
        {
          auto  chnext = *s++;

          if ( chnext >= '0' && chnext <= '9' ) o = (chnext - '0' + 0x0) << 4;  else
          if ( chnext >= 'a' && chnext <= 'f' ) o = (chnext - 'a' + 0xa) << 4;  else
          if ( chnext >= 'A' && chnext <= 'F' ) o = (chnext - 'A' + 0xa) << 4;  else
          return nullptr;

          chnext = *s++;

          if ( chnext >= '0' && chnext <= '9' ) o |= (chnext - '0' + 0x0);  else
          if ( chnext >= 'a' && chnext <= 'f' ) o |= (chnext - 'a' + 0xa);  else
          if ( chnext >= 'A' && chnext <= 'F' ) o |= (chnext - 'A' + 0xa);  else
          return nullptr;

          return *s == '-' ? s + 1 : s;
        };
      auto* output = std::begin( o.ubytes );

      if ( e == nullptr )
        for ( auto e = s; *e != '\0'; ++e ) (void)NULL;

      for ( auto i = 0; i != 16 && s < e - 1; ++i )
        if ( (s = getchr( *output++, s )) == nullptr )  throw parse_error( "hex character expected" );

      if ( output != std::end( o.ubytes ) )
        throw parse_error( "not enough data to initialize uuid" );

      return s;
    }

}

inline  size_t  GetBufLen( const mtc::uuid_t& uuid )
  {  return 16;  }
template <class O>
inline  O*      Serialize( O* o, const mtc::uuid_t& uuid )
  {
    return ::Serialize( o, uuid.data(), uuid.size() );
  }
template <class S>
inline  S*      FetchFrom( S* s, mtc::uuid_t& uuid )
  {
    return ::FetchFrom( s, (uint8_t*)uuid.data(), uuid.size() );
  }

# endif   // __mtc_uuid_h__
