# if !defined( __mtc_exceptions_h__ )
# define __mtc_exceptions_h__
# include <stdexcept>

namespace mtc {

  class file_error: public std::runtime_error  {  using std::runtime_error::runtime_error;  };

}

# endif // !__mtc_exceptions_h__
