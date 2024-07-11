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

    template <class T>
    using suffixes = std::initializer_list<std::pair<const char*, T>>;

  public:

    class error: public std::runtime_error
    {
      std::string sourcePath;

      using std::runtime_error::runtime_error;
    public:
      auto  set_source_name( const std::string& s ) ->error& {  return sourcePath = s, *this;  }
      auto  get_source_name() const -> const std::string& {  return sourcePath;  }

    };

  protected:
    config( zmap&&, charstr&& );
    config( const zmap&, const charstr& );

  public:     // construction
    config();
    config( const zmap& );
    config( const config& );
    config( const std::initializer_list<std::pair<zmap::key, zval>>& );
    config& operator = ( const zmap& );
    config& operator = ( const config& );

  public:     // iterators
    auto  begin () const -> decltype(cfgmap.begin()) {  return cfgmap.begin();  }
    auto  end   () const -> decltype(cfgmap.end())   {  return cfgmap.end();    }
    auto  size  () const -> decltype(cfgmap.size())  {  return cfgmap.size();   }
    bool  empty () const                             {  return cfgmap.empty();  }

  public:     // access
    auto  get_section ( const zmap::key& ) const -> config;
    auto  get_section ( const zmap& ) const -> config;

    auto  get_int32   ( const zmap::key&, int32_t = 0, const suffixes<uint32_t>& = {} ) const -> int32_t;
    auto  get_int64   ( const zmap::key&, int64_t = 0, const suffixes<uint32_t>& = {} ) const -> int64_t;

    auto  get_uint32  ( const zmap::key&, uint32_t = 0, const suffixes<uint32_t>& = {} ) const -> uint32_t;
    auto  get_uint64  ( const zmap::key&, uint64_t = 0, const suffixes<uint32_t>& = {} ) const -> uint64_t;
    auto  get_double  ( const zmap::key&, double = 0.0, const suffixes<double_t>& = {} ) const -> double;

    auto  get_charstr ( const zmap::key&, const charstr& = charstr() ) const -> charstr;
    auto  get_widestr ( const zmap::key&, const widestr& = widestr() ) const -> widestr;

    auto  get_uint32  ( const std::initializer_list<zmap::key>&, uint32_t = 0,
      const suffixes<uint32_t>& = {} ) const -> uint32_t;
    auto  get_uint64  ( const std::initializer_list<zmap::key>&, uint64_t = 0,
      const suffixes<uint32_t>& = {} ) const -> uint64_t;
    auto  get_double  ( const std::initializer_list<zmap::key>&, double_t = 0,
      const suffixes<double_t>& = {} ) const -> double_t;

    auto  get_charstr ( const std::initializer_list<zmap::key>&, const charstr& = charstr() ) const -> charstr;
    auto  get_widestr ( const std::initializer_list<zmap::key>&, const widestr& = widestr() ) const -> widestr;

    auto  get_path    ( const zmap::key& ) const -> charstr;

    auto  get_config  ( const zmap::key& ) const -> config;

    auto  to_zmap     (                  ) const -> const mtc::zmap&;

    auto  has_key     ( const zmap::key& ) const -> bool;

  public:     // loading
    static  auto  Open( const char*, const zmap& revive = {} ) -> config;
    static  auto  Open( const charstr&, const zmap& revive = {} ) -> config;
    static  auto  Load( const char* json, const char* path = nullptr ) -> config;
    static  auto  Load( const charstr& json, const std::string& path = std::string() ) -> config;

  protected:
    static  auto get_uint32( const zval*, uint32_t, const suffixes<uint32_t>& ) -> uint32_t;
    static  auto get_uint64( const zval*, uint64_t, const suffixes<uint32_t>& ) -> uint64_t;
    static  auto get_double( const zval*, double_t, const suffixes<double_t>& ) -> double_t;

  };
  
}

# endif  // __mtc_config_h__
