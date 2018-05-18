# if !defined( __mtc_jsconfig_h__ )
# define __mtc_jsconfig_h__
# include "fileStream.h"
# include "json.h"
# include "zarray.h"
# include <type_traits>
# include <stdexcept>

namespace mtc
{

  namespace configuration
  {
    using config = mtc::zarray<>;

    class error: public std::runtime_error
    {
      using std::runtime_error::runtime_error;
    };

    constexpr const char configGUID[] = "09c8bf44-e796-4abd-8d6a-be23ec31f58d";

    namespace __impl__
    {

      char*   fullpath( char* out, size_t cch, const char* str );
      char*   fullpath( char* out, size_t cch, const char* str, const char* base );

      inline  config  fault( const char* err, const enable_exceptions_t& )
        {
          throw configuration::error( err );
        }

      inline  config  fault( const char* err, const disable_exceptions_t& )
        {
          config  out;

          return (out.set_charstr( "error", err ), out);
        }

      constexpr inline  int32_t derive_exception_flag( const enable_exceptions_t& )
        {  return 1;  }

      constexpr inline  int32_t derive_exception_flag( const disable_exceptions_t& )
        {  return 0;  }

      char*   fullpath( char* out, size_t cch, const char* str );
      char*   fullpath( char* out, size_t cch, const char* str, const char* base );

      template <class S, class X>
      config  parsesrc( S* s, X& x )
        {
          config  cfg;

          return json::Parse( s, cfg, nullptr, x ) != nullptr ? cfg : fault( "cound not parse source", x );
        }

      template <class X>
      config  openfile( const char* s, X& x )
        {
          auto    lpfile = OpenFileStream( s, O_RDONLY, x );
          config  getcfg;
          config* p_intl;
          char    szpath[0x400];

          if ( lpfile == nullptr )
            return fault( strprintf( "could not open file '%s'", s ).c_str(), x );
          
          if ( (p_intl = (getcfg = parsesrc( lpfile.ptr(), x )).set_zarray( configGUID )) == nullptr )
            return config();

          if ( fullpath( szpath, sizeof(szpath), s ) == nullptr )
            return fault( "could not create the full file path", x );

          p_intl->set_charstr( 0U, szpath );
          p_intl->set_int32( 1U, derive_exception_flag( x ) );

          return getcfg;
        }

    }

    template <class S>
    config  Load( S* s, const mtc::enable_exceptions_t& x )
      {  return __impl__::parsesrc( s, x );  }

    template <class S>
    config  Load( S* s, const mtc::disable_exceptions_t& x )
      {  return __impl__::parsesrc( s, x );  }

    inline
    config  Open( const char* s, const mtc::enable_exceptions_t& x )
      {  return __impl__::openfile( s, x );  }

    inline
    config  Open( const char* s, const mtc::disable_exceptions_t& x )
      {  return __impl__::openfile( s, x );  }

    config  Open( const config&, unsigned   key );
    config  Open( const config&, const char*  key );
    config  Open( const config&, const widechar*  key );

    config  Section( const config&, unsigned  key );
    config  Section( const config&, const char* key );
    config  Section( const config&, const widechar* key );

    std::string Path( const config&, unsigned  key );
    std::string Path( const config&, const char* key );
    std::string Path( const config&, const widechar* key );

  }

}

# endif  // __mtc_jsconfig_h__
