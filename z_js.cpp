# include "z_js.h"
# include <type_traits>

namespace mtc {
namespace json {
namespace parse {

  auto  Parse( reader&, byte_t&,   const zval* revive = nullptr ) -> byte_t&;
  auto  Parse( reader&, uint16_t&, const zval* revive = nullptr ) -> uint16_t&;
  auto  Parse( reader&, uint32_t&, const zval* revive = nullptr ) -> uint32_t&;
  auto  Parse( reader&, uint64_t&, const zval* revive = nullptr ) -> uint64_t&;

  auto  Parse( reader&, char_t& , const zval* revive = nullptr ) -> char_t&;
  auto  Parse( reader&, int16_t&, const zval* revive = nullptr ) -> int16_t&;
  auto  Parse( reader&, int32_t&, const zval* revive = nullptr ) -> int32_t&;
  auto  Parse( reader&, int64_t&, const zval* revive = nullptr ) -> int64_t&;

  auto  Parse( reader&, float&  , const zval* revive = nullptr ) -> float&;
  auto  Parse( reader&, double& , const zval* revive = nullptr ) -> double&;

  auto  Parse( reader&, mtc::charstr&, const zval* revive = nullptr ) -> mtc::charstr&;
  auto  Parse( reader&, mtc::widestr&, const zval* revive = nullptr ) -> mtc::widestr&;

  // reader implementation

  auto  reader::getnext() -> char
    {
      return buflen != 0 ? chbuff[--buflen] : source.get();
    }

  auto  reader::putback( char ch ) -> reader&
    {
      if ( buflen > 1 )
        throw error( "stream buffer overflow" );

      return chbuff[buflen++] = ch, *this;
    }

  bool  reader::isspace( char ch ) const
    {
      return ch != '\0' && (unsigned char)ch <= 0x20;
    }

  auto  reader::nospace() -> char
    {
      for ( ; ; )
      {
        char  getchr;
        char  chnext;

        if ( (getchr = getnext()) == '\0' )
          return getchr;

        if ( isspace( getchr ) )
          continue;

        if ( getchr != '/' )
          return getchr;

        switch ( chnext = this->getnext() )
        {
          case '*':
            {
              for ( getchr = '/'; ; getchr = chnext )
              {
                if ( (chnext = this->getnext()) == '\0')
                  return chnext;
                if ( chnext == '/' && getchr == '*' )
                  break;
              }
              break;
            }
          case '/':
            {
              while ( (getchr = this->getnext()) != '\0' && getchr != '\n' )
                (void)NULL;
              if ( getchr == '\0' ) return getchr;
                else break;
            }

          case '\0':  return getchr;
          default:    putback( chnext );
                      return getchr;
        }
      }
    }

  bool  reader::getfour( char* four )
    {
      return (four[0] = nospace()) != '\0'
          && (four[1] = getnext()) != '\0'
          && (four[2] = getnext()) != '\0'
          && (four[3] = getnext()) != '\0';
    }

  // parse helpers

  /*
    helper functions for parsing json
  */
  inline  bool  wstrtodbl( double& o, const widechar* s )
  {
    widechar* endptr;
      o = w_strtod( s, &endptr );
    return *endptr == 0;
  }

  inline  bool  wstrtoint( unsigned& o, const widestr& s )
  {
    widechar* endptr;
      o = w_strtol( s.c_str(), &endptr, 10 );
    return *endptr == 0 && endptr > s;
  }

  inline  bool  wstrtostr( charstr& o, widestr& s )
  {
    for ( auto c: s )
      if ( (c & ~0x7f) != 0 )
        return false;

    o.reserve( s.length() );

    for ( auto c: s )
      o.push_back( (char)c );
        
    return true;
  }

  inline  bool  is_num_char( char ch ) {  return ch >= '0' && ch <= '9';  }
  inline  bool  is_int_char( char ch ) {  return is_num_char( ch ) || ch == '-';  }
  inline  bool  is_flo_char( char ch ) {  return is_int_char( ch ) || ch == '.' || ch == 'e' || ch == 'E';  }

  /*
    Parsing:
      - ordinal values;
  */

  // unsigneds
  template <class U>
  auto  uParse( reader& stm, U& u ) -> U&
    {
      char  chnext;

      if ( !is_num_char( chnext = stm.nospace() ) )
        throw error( "0..9 expected" );

      for ( u = (uint8_t)chnext - '0'; is_num_char( chnext = stm.getnext() ); )
        u = u * 10 + (uint8_t)chnext - '0';

      return stm.putback( chnext ), u;
    }

  // signed integers

  template <class I>
  auto  iParse( reader& stm, I& i ) -> I&
    {
      typename std::make_unsigned<I>::type  uvalue;
      char                                  chnext;
      bool                                  is_neg;

      if ( (is_neg = ((chnext = stm.nospace()) == '-')) == false )
        stm.putback( chnext );

      uParse( stm, uvalue );

      if ( is_neg )
        return (i = 0 - uvalue);
      else
        return (i = uvalue);
    }

  // floats

  template <class F>
  auto  fParse( reader& stm, F& flo ) -> F&
    {
      char  flobuf[0x40];
      char* floptr;
      char* endptr;

      for ( *(floptr = flobuf) = stm.nospace();
        floptr < flobuf + 0x40 && is_flo_char( *floptr ); *++floptr = stm.getnext() )  (void)0;

      flo = (F)strtod( flobuf, &endptr );

      if ( floptr >= std::end(flobuf) )
        throw error( "sequence too long to be float" );

      if ( endptr != floptr )
        throw error( "invalid floating point value" );

      return stm.putback( *floptr ), flo;
    }

  template <class C>
  auto  sParse( reader& src, std::basic_string<C>&  str ) -> std::basic_string<C>&
    {
      char  chnext;
      char  chprev;

      if ( (chnext = src.nospace()) != '\"' )
        throw error( "'\"' expected" );

      for ( chprev = '\0'; (chnext = src.getnext()) != '\0'; chprev = chnext )
      {
      // check for end of identifier
        if ( chnext == '\"' && chprev != '\\' )
          return str;

      // check for '\\'
        if ( chnext == '\\' && chprev != '\\' )
          continue;

      // check regular char
        if ( chprev != '\\' )
          {
            str.push_back( (C)(std::make_unsigned<char>::type)chnext );
            continue;
          }

      // check long code: 4 symbols
        if ( chnext == 'u' )
        {
          char      hexchr[5] = "    ";
          widechar  uvalue;
          char*     endptr;

          if ( src.getfour( hexchr ) )
            uvalue = (widechar)strtoul( hexchr, &endptr, 0x10 );
          else
            throw error( "4-digit hexadecimal character code expected" );

          if ( endptr - hexchr != 4 )
            throw error( "4-digit hexadecimal character code expected" );
              
          if ( sizeof(C) == sizeof(char) && uvalue > 127 )
          {
            if ( (uvalue & ~0x07ff) == 0 )
            {
              str.push_back( (C)(0xC0 | (byte_t)(uvalue >> 0x6)) );
              str.push_back( (C)(0x80 | (byte_t)(uvalue & 0x3f)) );
            }
              else
            {
              str.push_back( (C)(0xE0 | (byte_t)((uvalue >> 0xc))) );
              str.push_back( (C)(0x80 | (byte_t)((uvalue >> 0x6) & 0x3F)) );
              str.push_back( (C)(0x80 | (byte_t)((uvalue & 0x3f))) );
            }
          }
            else
          str.push_back( (C)uvalue );
        }
          else
        switch ( chnext )
        {
          case 'b':   str.push_back( (C)'\b' ); break;
          case 't':   str.push_back( (C)'\t' ); break;
          case 'n':   str.push_back( (C)'\n' ); break;
          case 'f':   str.push_back( (C)'\f' ); break;
          case 'r':   str.push_back( (C)'\r' ); break;
          case '\"':  str.push_back( (C)'\"' ); break;
          case '/':   str.push_back( (C)'/'  ); break;
          case '\\':  str.push_back( (C)'\\' ); 
                      chnext = '\0';            break;
          default:    throw error( "invalid escape sequence" );
        }
        chprev = '\0';
      }
      throw error( "unexpected end of stream" );
    }
    
  auto  Parse( reader& s, byte_t&   u, const zval* ) -> byte_t&   {  return uParse( s, u );  }
  auto  Parse( reader& s, uint16_t& u, const zval* ) -> uint16_t& {  return uParse( s, u );  }
  auto  Parse( reader& s, uint32_t& u, const zval* ) -> uint32_t& {  return uParse( s, u );  }
  auto  Parse( reader& s, uint64_t& u, const zval* ) -> uint64_t& {  return uParse( s, u );  }

  auto  Parse( reader& s, char_t&   i, const zval* ) -> char_t&   {  return iParse( s, i );  }
  auto  Parse( reader& s, int8_t&   i, const zval* ) -> int8_t&   {  return iParse( s, i );  }
  auto  Parse( reader& s, int16_t&  i, const zval* ) -> int16_t&  {  return iParse( s, i );  }
  auto  Parse( reader& s, int32_t&  i, const zval* ) -> int32_t&  {  return iParse( s, i );  }
  auto  Parse( reader& s, int64_t&  i, const zval* ) -> int64_t&  {  return iParse( s, i );  }

  auto  Parse( reader& s, float&    f, const zval* ) -> float&    {  return fParse( s, f );  }
  auto  Parse( reader& s, double&   f, const zval* ) -> double&   {  return fParse( s, f );  }

  auto  Parse( reader& s, charstr& r, const zval* ) -> mtc::charstr&  {  return sParse( s, r );  }
  auto  Parse( reader& s, widestr& r, const zval* ) -> mtc::widestr&  {  return sParse( s, r );  }

  template <class V>
  auto  Parse( reader& stm, std::vector<V>& out, const zval* revive = nullptr ) -> std::vector<V>&
    {
      char  chnext;

      if ( (chnext = stm.nospace()) != '[' )
        throw error( "'[' expected" );

      while ( (chnext = stm.nospace()) != '\0' && chnext != ']' )
        {
          V avalue;

          out.push_back( std::move( Parse( stm.putback( chnext ), avalue, revive ) ) );

          if ( (chnext = stm.nospace()) == ',' )
            continue;
          if ( chnext == ']' )  stm.putback( chnext );
            else throw error( "',' or ']' expected" );
        }

      if ( chnext != ']' )
        throw error( "']' expected" );

      return out;
    }

  static zmap type_set(
  {
    { "char",     (uint32_t)zval::z_char },
    { "int8",     (uint32_t)zval::z_char },
    { "byte",     (uint32_t)zval::z_byte },
    { "uint8",    (uint32_t)zval::z_byte },
    { "int16",    (uint32_t)zval::z_int16 },
    { "int32",    (uint32_t)zval::z_int32 },
    { "int64",    (uint32_t)zval::z_int64 },
    { "word16",   (uint32_t)zval::z_word16 },
    { "word32",   (uint32_t)zval::z_word32 },
    { "word64",   (uint32_t)zval::z_word64 },
    { "float",    (uint32_t)zval::z_float },
    { "double",   (uint32_t)zval::z_double },

    { "charstr",  (uint32_t)zval::z_charstr },
    { "widestr",  (uint32_t)zval::z_widestr },

    { "array_char"  ,   (uint32_t)zval::z_array_char    },
    { "array_byte"  ,   (uint32_t)zval::z_array_byte    },
    { "array_int16" ,   (uint32_t)zval::z_array_int16   },
    { "array_word16",   (uint32_t)zval::z_array_word16  },
    { "array_int32" ,   (uint32_t)zval::z_array_int32   },
    { "array_word32",   (uint32_t)zval::z_array_word32  },
    { "array_int64" ,   (uint32_t)zval::z_array_int64   },
    { "array_word64",   (uint32_t)zval::z_array_word64  },
    { "array_float" ,   (uint32_t)zval::z_array_float   },
    { "array_double",   (uint32_t)zval::z_array_double  },

    { "array_charstr",  (uint32_t)zval::z_array_charstr },
    { "array_widestr",  (uint32_t)zval::z_array_widestr }
  } );

  auto  map_type( const std::string& s_type ) -> unsigned
    {
      auto  pval = type_set.get_word32( s_type );

      return pval != nullptr ? *pval : zval::z_untyped;
    }

  auto  Parse( reader& s, zval& z, const zval* revive ) -> mtc::zval&
    {
      char        chnext;
      unsigned    v_type = zval::z_untyped;
      const zmap* z_data = nullptr;

      if ( revive != nullptr )
      {
        if ( revive->get_type() == zval::z_word16 )     v_type = *revive->get_word16(); else
        if ( revive->get_type() == zval::z_word32 )     v_type = *revive->get_word32(); else
        if ( revive->get_type() == zval::z_charstr )    v_type = map_type( *revive->get_charstr() );  else
        if ( revive->get_type() == zval::z_zmap )       z_data = revive->get_zmap();    else
        if ( revive->get_type() == zval::z_array_zmap ) z_data = revive->get_zmap();
      }

      switch ( v_type )
      {
        case zval::z_char:    return Parse( s, *z.set_char() ), z;
        case zval::z_int16:   return Parse( s, *z.set_int16()  ), z;
        case zval::z_int32:   return Parse( s, *z.set_int32()  ), z;
        case zval::z_int64:   return Parse( s, *z.set_int64()  ), z;
        case zval::z_byte:    return Parse( s, *z.set_byte()   ), z;
        case zval::z_word16:  return Parse( s, *z.set_word16() ), z;
        case zval::z_word32:  return Parse( s, *z.set_word32() ), z;
        case zval::z_word64:  return Parse( s, *z.set_word64() ), z;
        case zval::z_float:   return Parse( s, *z.set_float()  ), z;
        case zval::z_double:  return Parse( s, *z.set_double() ), z;
        case zval::z_charstr: return Parse( s, *z.set_charstr() ), z;
        case zval::z_widestr: return Parse( s, *z.set_widestr() ), z;
        case zval::z_zmap:    return Parse( s, *z.set_zmap(), z_data ), z;

        case zval::z_array_char:    return Parse( s, *z.set_array_char() ), z;
        case zval::z_array_byte:    return Parse( s, *z.set_array_byte() ), z;
        case zval::z_array_int16:   return Parse( s, *z.set_array_int16() ), z;
        case zval::z_array_int32:   return Parse( s, *z.set_array_int32() ), z;
        case zval::z_array_int64:   return Parse( s, *z.set_array_int64() ), z;
        case zval::z_array_word16:  return Parse( s, *z.set_array_word16() ), z;
        case zval::z_array_word32:  return Parse( s, *z.set_array_word32() ), z;
        case zval::z_array_word64:  return Parse( s, *z.set_array_word64() ), z;
        case zval::z_array_float:   return Parse( s, *z.set_array_float() ), z;
        case zval::z_array_double:  return Parse( s, *z.set_array_double() ), z;
        case zval::z_array_charstr: return Parse( s, *z.set_array_charstr() ), z;
        case zval::z_array_widestr: return Parse( s, *z.set_array_widestr() ), z;
        default:  break;
      }

    // untyped xvalue load: object zarray {...}, untyped array [...], string "...", integer -?[0-9]+ or float -?[0-9]+\.?[0-9]*([Ee]-?[0-9]+)?
      switch ( chnext = s.nospace() )
      {
        case '{':
          return Parse( s.putback( chnext ), *z.set_zmap(), z_data ), z;

        case '[':
          {
            auto        arrval = z.set_array_zval();
            zval        newval;
            zval        _zvrev;
            const zval* pzvrev = nullptr;

            if ( z_data != nullptr )
              {
                _zvrev.set_zmap( *z_data );  pzvrev = &_zvrev;
              }

            Parse( s.putback( chnext ), *arrval, pzvrev );

          // check if std::vector<zval> is convertible to simple types; if is, try convert to simple types
            if ( arrval->empty() )
              return z;

            for ( auto prev = arrval->begin(), next = prev + 1; next != arrval->end(); prev = next++ )
              if ( next->get_type() != prev->get_type() )
                return z;

            switch ( arrval->front().get_type() )
            {
            # define  derive_transform( _type_ )                                \
              case zval::z_##_type_:                                            \
                {                                                               \
                  mtc::array_##_type_&  newarr = *newval.set_array_##_type_();  \
                                                                                \
                  for ( auto it: *arrval )                                      \
                    newarr.push_back( std::move( *it.get_##_type_() ) );        \
                  break;                                                        \
                }
              derive_transform( char )
              derive_transform( byte )
              derive_transform( int16 )
              derive_transform( int32 )
              derive_transform( int64 )
              derive_transform( float )
              derive_transform( word16 )
              derive_transform( word32 )
              derive_transform( word64 )
              derive_transform( double )
              derive_transform( charstr )
              derive_transform( widestr )
              derive_transform( zmap )
            # undef  derive_transform
              default:  return z;
            }
            z = std::move( newval );
          }
          return z;

        case '\"':
          {
            mtc::charstr  chrstr;
            mtc::widestr  wcsstr;

          // check if charstr or widestr
            if ( wstrtostr( chrstr, Parse( s.putback( chnext ), wcsstr ) ) )
              z.set_charstr( std::move( chrstr ) );
            else
              z.set_widestr( std::move( wcsstr ) );

            return z;
          }
        default:  break;
      }
      if ( is_int_char( chnext ) )
      {
        charstr cvalue( 1, chnext );
        bool    bpoint;
        bool    bexpon;
        char    chprev;

        for ( chprev = '\0', bexpon = bpoint = false; ; )
        {
          if ( is_num_char( chnext = s.getnext() ) )
          {
            cvalue.push_back( chprev = chnext );
          }
            else
          if ( chnext == '.' )
          {
            if ( bpoint || bexpon )
              throw error( "unexpected '.' in numeric format" );
            cvalue.push_back( chprev = chnext );
              bpoint = true;
          }
            else
          if ( chnext == 'e' || chnext == 'E' )
          {
            if ( bexpon )
              throw error( "unexpected 'e' in numeric format" );
            cvalue.push_back( chprev = chnext );
              bexpon = true;
          }
            else
          if ( chnext == '-' )
          {
            if ( chprev != 'e' && chprev != 'E' )
              throw error( "unexpected '-' in numeric format" );
            cvalue.push_back( chprev = chnext );
          }
            else
          {
            s.putback( chnext );
            break;
          }
        }
        if ( bpoint || bexpon )
        {
          char*   endptr;
          double  dvalue = strtod( cvalue.c_str(), &endptr );

          if ( *endptr != '\0' )
            throw error( "invalid numeric format" );
          z.set_double( dvalue );
        }
          else
        {
          char*   endptr;
          int32_t nvalue = strtol( cvalue.c_str(), &endptr, 10 );

          if ( *endptr != '\0' )
            throw error( "invalid numeric format" );
          z.set_int32( nvalue );
        }
        return z;
      }

    // check for boolean represented as byte
      if ( chnext == 't' )
      {
        if ( (chnext = s.getnext()) != 'r'
          || (chnext = s.getnext()) != 'u'
          || (chnext = s.getnext()) != 'e' )
            throw( strprintf( "unexpected character '%c'", chnext ) );
        return z.set_byte( 1 ), z;
      }
        else
      if ( chnext == 'f' )
      {
        if ( (chnext = s.getnext()) != 'a'
          || (chnext = s.getnext()) != 'l'
          || (chnext = s.getnext()) != 's'
          || (chnext = s.getnext()) != 'e' )
            throw error( strprintf( "unexpected character '%c'", chnext ) );
        return z.set_byte( 0 ), z;
      }
        else
      if ( chnext == 'n' )
      {
        if ( (chnext = s.getnext()) != 'u'
          || (chnext = s.getnext()) != 'l'
          || (chnext = s.getnext()) != 'l' )
            throw error( strprintf( "unexpected character '%c'", chnext ) );
        return z.set_charstr(), z;
      }
        else
      throw error( strprintf( "unexpected character '%c'", chnext ) );
    }

  auto  Parse( reader& s, zmap& z, const zmap* revive ) -> zmap&
  {
    char  chnext;

    z.clear();

  // open object
    if ( (chnext = s.nospace()) != '{' )
      throw error( "'{' expected" );

  // char by char until end or '}'
    while ( (chnext = s.nospace()) != '\0' && chnext != '}' )
    {
      unsigned      intkey = 0;
      charstr       strkey;
      widestr       wcskey;
      zval*         newval = nullptr;
      const zval*   revval = nullptr;

    // get variable name as widestring
      try
        {  Parse( s.putback( chnext ), wcskey );  }
      catch ( const error& jx )
        {  throw error( strprintf( "%s while parsing variable name", jx.what() ) );  }

      if ( wstrtoint( intkey, wcskey ) )
        {
          newval = z.put( intkey );
          revval = revive != nullptr ? revive->get( intkey ) : nullptr;
        }
      else
      if ( wstrtostr( strkey, wcskey ) )
        {
          newval = z.put( strkey );
          revval = revive != nullptr ? revive->get( strkey ) : nullptr;
        }
      else
        {
          newval = z.put( wcskey );
          revval = revive != nullptr ? revive->get( wcskey ) : nullptr;
        }

    // check for colon
      if ( (chnext = s.nospace()) != ':' )
        throw error( "':' expected" );

    // get the value
      try
        {  Parse( s, *newval, revval );  }
      catch ( const error& jx )
        {  throw error( strprintf( "%s while parsing variable value", jx.what() ) );  }

    // check for comma
      if ( (chnext = s.nospace()) == ',' )
        continue;
      if ( chnext == '}' )  s.putback( chnext );
        else throw error( "'}' or ',' expected" );
    }

  // check valid script
    if ( chnext != '}' )
      throw error( "'}' expected" );

    return z;
  }

}}}
