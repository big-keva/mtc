# if !defined( __mtc_exceptions_h__ )
# define __mtc_exceptions_h__
# include "wcsstr.h"
# include <stdexcept>
# include <cstdarg>

namespace mtc {

  class file_error: public std::runtime_error {  using std::runtime_error::runtime_error;  };

  template <class F>
  F   FormatError( const char* fmt, ... )
  {
    va_list vaargs;

    va_start( vaargs, fmt );
      auto  strarg = vstrduprintf( fmt, vaargs );
    va_end( vaargs );

    return F( strarg );
  }

}

# endif // !__mtc_exceptions_h__
