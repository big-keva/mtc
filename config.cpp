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

      getcwd( curdir, sizeof(curdir) );
      chdir ( newdir );
      getcwd( newdir, sizeof(newdir) );
      chdir ( curdir );

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
      getcwd( curdir, sizeof(curdir) );
      chdir ( orgdir );
      chdir ( getdir );
      getcwd( newdir, sizeof(newdir) );
      chdir ( curdir );

      for ( pslash = (char*)psz + srclen; pslash > psz && !is_slash( *pslash ); --pslash )
        (void)NULL;
      while ( is_slash( *pslash ) )
        ++pslash;

      if ( w_strlen( newdir ) + 2 + w_strlen( pslash ) > cch )
        return nullptr;
  
      return w_strcat( w_strcat( w_strcpy( out, newdir ), "/" ), pslash );
    }

    template <class K>
    config  open( const config& cfg, K key )
      {
        const char* substr = cfg.get_charstr( key );
        auto        zvconf = cfg.get_zarray( configGUID );

        if ( substr != nullptr && zvconf != nullptr )
        {
          auto  szroot = zvconf->get_charstr( 0U );
          auto  except = zvconf->get_int32( 1U, 0 ) != 0;
          char  szpath[0x400];

          if ( __impl__::fullpath( szpath, sizeof(szpath), substr, szroot ) != nullptr )
          {
            if ( except )
              return Open( szpath, enable_exceptions );
            else
              return Open( szpath, disable_exceptions );
          }
        }

        return config();
      }

    template <class K>
    config  section( const config& cfg, K key )
      {
        const zarray<>* pzsect = cfg.get_zarray( key );
        auto            zvconf = cfg.get_zarray( configGUID );

        if ( pzsect != nullptr && zvconf != nullptr )
        {
          config  newcfg( *pzsect );

          return (newcfg.set_zarray( configGUID, *zvconf ), newcfg);
        }

        return config();
      }

  }

  // config implementation

  config::config(): cfgmap() {}

  config::config( config&& c ): cfgmap( std::move( c.cfgmap ) ), origin( std::move( c.origin ) ) {}

  config::config( zmap&& z, charstr&& s ): cfgmap( std::move( z ) ), origin( std::move( s ) ) {}

  config::config( const zmap& z, const charstr& s ): cfgmap( z ), origin( s ) {}

  auto  config::get_section( const zmap::key& key ) const -> config
    {
      auto  pmap = cfgmap.get_zmap( key );

      return pmap != nullptr ? config( *pmap, origin ) : config();
    }

  auto  config::get_int32( const zmap::key& key, int32_t def ) const -> int32_t
    {  return cfgmap.get_int32( key, def );  }

  auto  config::get_int64( const zmap::key& key, int64_t def ) const -> int64_t
    {  return cfgmap.get_int64( key, def );  }

  auto  config::get_uint32( const zmap::key& key, uint32_t def ) const -> uint32_t
    {  return cfgmap.get_word32( key, def );  }

  auto  config::get_uint64( const zmap::key& key, uint64_t def ) const -> uint64_t
    {  return cfgmap.get_word64( key, def );  }

  auto  config::get_double( const zmap::key& key, double def ) const -> double
    {  return cfgmap.get_double( key, def );  }

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
      auto  stpath = get_path( key );

      return stpath.length() != 0 ? config::Open( stpath ) : config();
    }

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

}