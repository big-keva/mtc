# if !defined( __mtc_zipStream_h__ )
# define __mtc_zipStream_h__
# include "iStream.h"
# include <string>

namespace mtc {
namespace zip {

  auto  OpenStream( const char* sz, unsigned mode, const enable_exceptions_t& = enable_exceptions,
    unsigned buflen = 0x400 * 0x400 ) -> api<IFlatStream>;
  auto  OpenStream( const char* sz, unsigned mode, const disable_exceptions_t&,
    unsigned buflen = 0x400 * 0x400 ) -> api<IFlatStream>;
  auto  OpenStream( const std::string& sz, unsigned mode, const enable_exceptions_t& = enable_exceptions,
    unsigned buflen = 0x400 * 0x400 ) -> api<IFlatStream>;
  auto  OpenStream( const std::string& sz, unsigned mode, const disable_exceptions_t&,
    unsigned buflen = 0x400 * 0x400 ) -> api<IFlatStream>;

  auto  LoadBuffer( const char* sz, const enable_exceptions_t& = enable_exceptions ) -> api<IByteBuffer>;
  auto  LoadBuffer( const char* sz, const disable_exceptions_t& ) -> api<IByteBuffer>;
  auto  LoadBuffer( const std::string& sz, const enable_exceptions_t& = enable_exceptions ) -> api<IByteBuffer>;
  auto  LoadBuffer( const std::string& sz, const disable_exceptions_t& ) -> api<IByteBuffer>;

}}

# endif  // __mtc_zipStream_h__
