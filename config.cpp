# include "config.h"
# include "wcsstr.h"
# include "z_js.h"
# include "file.h"

# if defined( _WIN32 )
#   include <direct.h>
#   if __STDC__
#     define  getcwd  _getcwd
#     define  chdir   _chdir
#   endif
# else
#   include <unistd.h>
# endif  // _WIN32

namespace mtc
{

  namespace __impl__
  {

    inline
    bool  is_slash( char ch )
      {
        return ch == '/' || ch == '\\';
      }

    char* fullpath( char* out, size_t cch, const char* psz )
    {
      char    curdir[0x400];
      char    newdir[0x400];
      size_t  srclen = psz != nullptr ? w_strlen( psz ) : 0;
      char*   pslash;

      if ( psz == nullptr || srclen >= sizeof(newdir) )
        return nullptr;

      for ( pslash = w_strcpy( newdir, psz ) + w_strlen( psz ); pslash > newdir && !is_slash( *pslash ); --pslash )
        (void)NULL;
      *pslash = 0;

      (void)(getcwd( curdir, sizeof(curdir) ) != nullptr);
      (void)(chdir ( newdir ) == 0);
      (void)(getcwd( newdir, sizeof(newdir) ) != nullptr);
      (void)(chdir ( curdir ) == 0);

      for ( pslash = (char*)psz + w_strlen( psz ); pslash > psz && !is_slash( pslash[-1] ); --pslash )
        (void)NULL;

      if ( cch < w_strlen( newdir ) + 1 + w_strlen( pslash ) + 1 )
        return nullptr;

      return w_strcat( w_strcat( w_strcpy( out, newdir ), "/" ), pslash );
    }

    char* fullpath( char* out, size_t cch, const char* psz, const char* org )
    {
      char    orgdir[0x400];
      char    curdir[0x400];
      char    getdir[0x400];
      char    newdir[0x400];
      size_t  srclen = psz != nullptr ? w_strlen( psz ) : 0;
      size_t  orglen = org != nullptr ? w_strlen( org ) : 0;
      char*   pslash;
  
      if ( psz == nullptr || orglen >= sizeof(orgdir) || srclen >= sizeof(getdir) )
        return nullptr;

    // get config dir
      if ( orglen != 0 )
      {
        for ( pslash = w_strcpy( orgdir, org ) + orglen; pslash > orgdir && !is_slash( *pslash ); --pslash )
          (void)NULL;
        if ( pslash == orgdir ) strcpy( orgdir, "." );
          else *pslash = '\0';
      } else strcpy( orgdir, "." );

    // get argument dir
      for ( pslash = w_strcpy( getdir, psz ) + srclen; pslash > getdir && !is_slash( *pslash ); --pslash )
        (void)NULL;
      *pslash = '\0';

    // save current 
      (void)(getcwd( curdir, sizeof(curdir) ) != nullptr );
      (void)(chdir ( orgdir ) == 0 );

      if ( getdir[0] != '\0' && chdir ( getdir ) != 0 )
      {
        (void)(chdir ( curdir ) == 0 );
        throw config::error( mtc::strprintf( "could not locate path '%s'", getdir ) );
      }

      (void)(getcwd( newdir, sizeof(newdir) ) != nullptr );
      (void)(chdir ( curdir ) == 0 );

      for ( pslash = (char*)psz + srclen; pslash > psz && !is_slash( *pslash ); --pslash )
        (void)NULL;
      while ( is_slash( *pslash ) )
        ++pslash;

      if ( w_strlen( newdir ) + 2 + w_strlen( pslash ) > cch )
        return nullptr;
  
      return w_strcat( w_strcat( w_strcpy( out, newdir ), "/" ), pslash );
    }

  }

  template <class C, class V>
  bool  parse_double( double& val, const std::basic_string<C>& str,
    const std::initializer_list<std::pair<const char*, V>>& suf )
  {
    C*    endp;
    auto  dval = mtc::w_strtod( str.c_str(), &endp );

    if ( endp == str.c_str() )
      return false;

    if ( *endp == (C)0 )
      return val = dval, true;

    for ( auto& s: suf )
      if ( mtc::w_strcmp( s.first, endp ) == 0 )
        return val = dval * s.second, true;

    return false;
  }

  template <class target, class value, class alter>
  auto  value_in_limits( value v, alter a ) -> target
  {
    return (double)v <= (double)std::numeric_limits<target>::max()
        && (double)v >= (double)std::numeric_limits<target>::min() ? v : a;
  }

  // config implementation

  config::config(): cfgmap() {}

  config::config( config&& c ): cfgmap( std::move( c.cfgmap ) ), origin( std::move( c.origin ) ) {}

  config::config( const config& c ): cfgmap( c.cfgmap ), origin( c.origin ) {}

  config::config( zmap&& z, charstr&& s ): cfgmap( std::move( z ) ), origin( std::move( s ) ) {}

  config::config( const zmap& z, const charstr& s ): cfgmap( z ), origin( s ) {}

  config::config( const std::initializer_list<std::pair<zmap::key, zval>>& il ): cfgmap( il ) {}

  config& config::operator = ( config&& c )
  {  return cfgmap = std::move( c.cfgmap ), *this;  }

  config& config::operator = ( const config& c )
  {  return cfgmap = c.cfgmap, origin = c.origin, *this;  }

  auto  config::get_section( const zmap::key& key ) const -> config
  {
    auto  pmap = cfgmap.get_zmap( key );

    return pmap != nullptr ? config( *pmap, origin ) : config();
  }

  auto  config::get_int32( const zmap::key& key, int32_t def,
    const init<const char*, int32_t>& suf ) const -> int32_t
  {
    auto    getval = cfgmap.get( key );
    double  dblval;

    if ( getval == nullptr )
      return def;

    switch ( getval->get_type() )
    {
      case zval::z_double:  return value_in_limits<int32_t>( *getval->get_double(), def );
      case zval::z_word64:  return value_in_limits<int32_t>( *getval->get_word64(), def );
      case zval::z_int64:   return value_in_limits<int32_t>( *getval->get_int64(), def );
      case zval::z_word32:  return value_in_limits<int32_t>( *getval->get_word32(), def );
      case zval::z_int32:   return *getval->get_int32();
      case zval::z_word16:  return *getval->get_word16();
      case zval::z_int16:   return *getval->get_int16();
      case zval::z_charstr:
        return parse_double( dblval, *getval->get_charstr(), suf ) ?
          value_in_limits<int32_t>( dblval, def ) : def;
      case zval::z_widestr:
        return parse_double( dblval, *getval->get_widestr(), suf ) ?
          value_in_limits<int32_t>( dblval, def ) : def;
      default:  return def;
    }
  }

  auto  config::get_int64( const zmap::key& key, int64_t def,
    const init<const char*, int64_t>& suf ) const -> int64_t
  {
    auto    getval = cfgmap.get( key );
    double  dblval;

    if ( getval == nullptr )
      return def;

    switch ( getval->get_type() )
    {
      case zval::z_double:  return value_in_limits<int64_t>( *getval->get_double(), def );
      case zval::z_word64:  return value_in_limits<int64_t>( *getval->get_word64(), def );
      case zval::z_int64:   return *getval->get_int64();
      case zval::z_word32:  return *getval->get_word32();
      case zval::z_int32:   return *getval->get_int32();
      case zval::z_word16:  return *getval->get_word16();
      case zval::z_int16:   return *getval->get_int16();
      case zval::z_charstr:
        return parse_double( dblval, *getval->get_charstr(), suf ) ?
          value_in_limits<int64_t>( dblval, def ) : def;
      case zval::z_widestr:
        return parse_double( dblval, *getval->get_widestr(), suf ) ?
          value_in_limits<int64_t>( dblval, def ) : def;
      default:  return def;
    }
  }

  auto  config::get_uint32( const zmap::key& key, uint32_t def,
    const init<const char*, uint32_t>& suf ) const -> uint32_t
  {
    auto    getval = cfgmap.get( key );
    double  dblval;

    if ( getval == nullptr )
      return def;

    switch ( getval->get_type() )
    {
      case zval::z_double:  return value_in_limits<uint32_t>( *getval->get_double(), def );
      case zval::z_word64:  return value_in_limits<uint32_t>( *getval->get_word64(), def );
      case zval::z_int64:   return value_in_limits<uint32_t>( *getval->get_int64(), def );
      case zval::z_int32:   return value_in_limits<uint32_t>( *getval->get_int32(), def );
      case zval::z_int16:   return value_in_limits<uint32_t>( *getval->get_int16(), def );
      case zval::z_word32:  return *getval->get_word32();
      case zval::z_word16:  return *getval->get_word16();
      case zval::z_charstr:
        return parse_double( dblval, *getval->get_charstr(), suf ) ?
          value_in_limits<uint32_t>( dblval, def ) : def;
      case zval::z_widestr:
        return parse_double( dblval, *getval->get_widestr(), suf ) ?
          value_in_limits<uint32_t>( dblval, def ) : def;
      default:  return def;
    }
  }

  auto  config::get_uint64( const zmap::key& key, uint64_t def,
    const init<const char*, uint64_t>& suf ) const -> uint64_t
  {
    auto    getval = cfgmap.get( key );
    double  dblval;

    if ( getval == nullptr )
      return def;

    switch ( getval->get_type() )
    {
      case zval::z_double:  return value_in_limits<uint64_t>( *getval->get_double(), def );
      case zval::z_word64:  return *getval->get_word64();
      case zval::z_int64:   return value_in_limits<uint64_t>( *getval->get_int64(), def );
      case zval::z_word32:  return *getval->get_word32();
      case zval::z_int32:   return value_in_limits<uint64_t>( *getval->get_int32(), def );
      case zval::z_word16:  return *getval->get_word16();
      case zval::z_int16:   return value_in_limits<uint64_t>( *getval->get_int16(), def );
      case zval::z_charstr:
        return parse_double( dblval, *getval->get_charstr(), suf ) ?
          value_in_limits<uint64_t>( dblval, def ) : def;
      case zval::z_widestr:
        return parse_double( dblval, *getval->get_widestr(), suf ) ?
          value_in_limits<uint64_t>( dblval, def ) : def;
      default:  return def;
    }
  }

  auto  config::get_double( const zmap::key&  key, double def,
    const init<const char*, double>& suf ) const -> double
  {
    auto    getval = cfgmap.get( key );
    double  dblval;

    if ( getval == nullptr )
      return def;

    switch ( getval->get_type() )
    {
      case zval::z_double:  return *getval->get_double();
      case zval::z_word64:  return *getval->get_word64();
      case zval::z_int64:   return *getval->get_int64();
      case zval::z_word32:  return *getval->get_word32();
      case zval::z_int32:   return *getval->get_int32();
      case zval::z_word16:  return *getval->get_word16();
      case zval::z_int16:   return *getval->get_int16();
      case zval::z_charstr: return parse_double( dblval, *getval->get_charstr(), suf ) ? dblval : def;
      case zval::z_widestr: return parse_double( dblval, *getval->get_widestr(), suf ) ? dblval : def;
      default:              return def;
    }
  }

  auto  config::get_charstr( const zmap::key& key, const charstr& def ) const -> charstr
  {  return cfgmap.get_charstr( key, def );  }

  auto  config::get_widestr( const zmap::key& key, const widestr& def ) const -> widestr
  {  return cfgmap.get_widestr( key, def );  }

  auto  config::get_path( const zmap::key& key ) const -> charstr
  {
    auto  stpath = cfgmap.get_charstr( key, "" );

    if ( stpath != "" )
    {
      char  expand[0x400];

      if ( __impl__::fullpath( expand, sizeof(expand), stpath.c_str(), origin.c_str() ) != nullptr )
        return charstr( expand );
    }

    return charstr();
  }

  auto  config::get_config( const zmap::key& key ) const -> config
  {
    auto  pvalue = cfgmap.get( key );

    if ( pvalue == nullptr )
      return config();

    if ( pvalue->get_type() == zval::z_zmap )
      return config( *pvalue->get_zmap(), origin );

    if ( pvalue->get_type() == zval::z_charstr )
    {
      auto  stpath = get_path( key );

      if ( stpath.length() != 0 )
        return config::Open( stpath );
    }
    return config();
  }

  auto  config::to_zmap() const -> const mtc::zmap&
  {  return cfgmap;  }

  auto  config::has_key( const zmap::key& key ) const -> bool
  {  return cfgmap.get( key ) != nullptr;  }

  auto  config::Open( const char* path ) -> config
  {
    file    lpfile = fopen( path, "rb" );
    zmap    getcfg;
    char    szpath[0x400];

    if ( lpfile == nullptr )
      throw error( strprintf( "file '%s' not found", path ) );

    json::Parse( lpfile.ptr(), getcfg );

    if ( __impl__::fullpath( szpath, sizeof(szpath), path ) == nullptr )
      throw error( strprintf( "could not create the full file path '%s'", path ) );

    return std::move( config( std::move( getcfg ), std::move( charstr( szpath ) ) ) );
  }

  auto  config::Open( const charstr& path ) -> config
  {  return std::move( Open( path.c_str() ) );  }

  auto  config::Load( const char* source, const char* path ) -> config
  {
    zmap    getcfg;
    char    szpath[0x400];

    if ( path != nullptr && *path != '\0' )
    {
      if ( __impl__::fullpath( szpath, sizeof(szpath), path ) == nullptr )
        throw error( strprintf( "could not create the full file path '%s'", path ) );
    }
      else
    szpath[0] = '\0';

    json::Parse( source, getcfg );

    return std::move( config( std::move( getcfg ), std::move( charstr( szpath ) ) ) );
  }

  auto  config::Load( const std::string& source, const std::string& path ) -> config
  {  return std::move( Load( source.c_str(), path.c_str() ) );  }
}
