# include "platform.h"
# include <stdexcept>

namespace mtc {

  class SharedLibrary
  {
    struct impl;

  public:
    class error: public std::runtime_error {  using runtime_error::runtime_error;  };

    using ModuleHandle = void*;

  public:
    SharedLibrary() = default;
    SharedLibrary( SharedLibrary&& );
    SharedLibrary( const SharedLibrary& );
    SharedLibrary& operator = ( SharedLibrary&& );
    SharedLibrary& operator = ( const SharedLibrary& );
   ~SharedLibrary();

    bool  operator == ( const SharedLibrary& ) const;
    bool  operator == ( nullptr_t ) const;
  template <class T>
    bool  operator != ( T t ) const {  return !(*this == t);  }

    auto  Find( const char*, const enable_exceptions_t& = {} ) const -> void*;
    auto  Find( const char*, const disable_exceptions_t& ) const -> void*;

    static
    auto  Load( const char*, const enable_exceptions_t& = {} ) -> SharedLibrary;
    static
    auto  Load( const char*, const disable_exceptions_t& ) -> SharedLibrary;

    auto  native_handle() const -> ModuleHandle;

  protected:
    impl*   module = nullptr;

  };

}
