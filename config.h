# if !defined( __mtc_config_h__ )
# define __mtc_config_h__
# include "fileStream.h"
# include "zmap.h"
# include <type_traits>
# include <stdexcept>

namespace mtc {

  class config
  {
    zmap    cfgmap;
    charstr origin;

    template <class key, class val>
    using init = std::initializer_list<std::pair<key, val>>;

  public:
    class error: public std::runtime_error
    {  using std::runtime_error::runtime_error;  };

  protected:
    config( zmap&&, charstr&& );
    config( const zmap&, const charstr& );

  public:     // construction
    config();
    config( config&& );
    config( const config& );
    config( const init<zmap::key, zval>& );
    config& operator = ( config&& );
    config& operator = ( const config& );

  public:     // iterators
    auto  begin () const -> decltype(cfgmap.begin()) {  return cfgmap.begin();  }
    auto  end   () const -> decltype(cfgmap.end())   {  return cfgmap.end();    }
    auto  size  () const -> decltype(cfgmap.size())  {  return cfgmap.size();   }

  public:     // access
    auto  get_section ( const zmap::key& ) const -> config;

    auto  get_int32   ( const zmap::key&, int32_t = 0 ) const -> int32_t;
    auto  get_int64   ( const zmap::key&, int64_t = 0 ) const -> int64_t;

    auto  get_uint32  ( const zmap::key&, uint32_t = 0 ) const -> uint32_t;
    auto  get_uint64  ( const zmap::key&, uint64_t = 0 ) const -> uint64_t;

    auto  get_double  ( const zmap::key&, double = 0.0,
      const init<const char*, double>& = {} ) const -> double;

    auto  get_charstr ( const zmap::key&, const charstr& = charstr() ) const -> charstr;
    auto  get_widestr ( const zmap::key&, const widestr& = widestr() ) const -> widestr;

    auto  get_path    ( const zmap::key& ) const -> charstr;

    auto  get_config  ( const zmap::key& ) const -> config;

    auto  to_zmap     (                  ) const -> const mtc::zmap&;

    auto  has_key     ( const zmap::key& ) const -> bool;

  public:     // loading
    static  auto  Open( const char* ) -> config;
    static  auto  Open( const charstr& ) -> config;
    static  auto  Load( const char* json, const char* path = nullptr ) -> config;
    static  auto  Load( const charstr& json, const std::string& path = std::string() ) -> config;

  };
  
}

# endif  // __mtc_config_h__
