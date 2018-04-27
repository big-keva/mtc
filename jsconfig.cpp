# include "jsconfig.h"
# include "wcsstr.h"

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

  namespace configuration
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
        while ( !is_slash( *pslash ) )
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

      template <class K>
      std::string path( const config& cfg, K key )
        {
          const char* szpath = cfg.get_charstr( key );
          auto        zvconf = cfg.get_zarray( configGUID );

          if ( szpath != nullptr && zvconf != nullptr )
          {
            char  expand[0x400];

            if ( __impl__::fullpath( expand, sizeof(expand), szpath, zvconf->get_charstr( 0U ) ) != nullptr )
              return std::string( expand );
          }

          return std::string();
        }

    }

    config  Open( const config& cfg, unsigned key )  {  return __impl__::open( cfg, key );  }
    config  Open( const config& cfg, const char* key )  {  return __impl__::open( cfg, key );  }
    config  Open( const config& cfg, const widechar* key )  {  return __impl__::open( cfg, key );  }

    config  Section( const config& cfg, unsigned key )  {  return __impl__::section( cfg, key );  }
    config  Section( const config& cfg, const char* key )  {  return __impl__::section( cfg, key );  }
    config  Section( const config& cfg, const widechar* key )  {  return __impl__::section( cfg, key );  }

    std::string  Path( const config& cfg, unsigned key )  {  return __impl__::path( cfg, key );  }
    std::string  Path( const config& cfg, const char* key )  {  return __impl__::path( cfg, key );  }
    std::string  Path( const config& cfg, const widechar* key )  {  return __impl__::path( cfg, key );  }

  }

}
