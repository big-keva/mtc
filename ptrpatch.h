# if !defined( __mtc_ptrpatch_h__ )
# define __mtc_ptrpatch_h__

namespace mtc {
namespace ptr {

  using to_int = typename std::conditional<sizeof(void*) == sizeof(uint64_t),
    uint64_t, uint32_t>::type;

  template <class V>
  auto  dirty( V* v ) -> V* {  return (V*)(1 | (to_int)v);  }
  template <class V>
  auto  clean( V* v ) -> V* {  return (V*)(~1 & (to_int)v);  }

}}

# endif // __mtc_ptrpatch_h__