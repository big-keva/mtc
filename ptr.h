# if !defined( __mtc_ptr_h__ )
# define __mtc_ptr_h__
# include <type_traits>
# include <cstddef>
# include <cstdint>

namespace mtc {
namespace ptr {

  template <class T>
  auto  align( T* p, size_t cb ) -> T*
  {  return cb != 0 ? (T*)((uintptr_t(p) + cb - 1) & ~(cb - 1)) : p;  }

  template <class T>
  auto  align( T* p ) -> T* {  return align( p, alignof(T) );  }

  template <class T>
  auto  dirty( T* p ) -> T* {  return (T*)(1 | uintptr_t(p));  }

  template <class T>
  auto  clean( T* v ) -> T* {  return (T*)(~1 & uintptr_t(v));  }

  template <class T>
  bool  is_dirty( T* p )  {  return (uintptr_t(p) & 1) != 0;  }

  template <class T>
  bool  is_clean( T* p )  {  return (uintptr_t(p) & 1) == 0;  }

}}

# endif // __mtc_ptr_h__
