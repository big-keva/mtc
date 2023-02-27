# if !defined( __mtc_ptrpatch_h__ )
# define __mtc_ptrpatch_h__
# include <cstdint>

namespace mtc {
namespace ptr {

  class pointer_to_integer
  {
    using to_int = typename std::conditional<sizeof(void*) == sizeof(uint64_t),
      uint64_t, uint32_t>::type;

    template <class V>  friend  V*      align( V* v, size_t cb );
    template <class V>  friend  V*      dirty( V* v );
    template <class V>  friend  V*      clean( V* v );
                        friend  to_int  toint( const void* p );

  };

  template <class V>
  inline  auto  align( V* v, size_t cb ) -> V*
  {  return (V*)((((pointer_to_integer::to_int)v) + cb - 1) & ~(cb - 1));  }

  template <class V>
  inline  auto  align( V* v ) -> V* {  return align( v, alignof(V) );  }

  template <class V>
  inline  auto  dirty( V* v ) -> V* {  return (V*)(1 | (pointer_to_integer::to_int)v);  }

  template <class V>
  inline  auto  clean( V* v ) -> V* {  return (V*)(~1 & (pointer_to_integer::to_int)v);  }

  inline  auto  toint( const void* p ) -> pointer_to_integer::to_int  {  return (pointer_to_integer::to_int)p;  }

}}

# endif // __mtc_ptrpatch_h__
