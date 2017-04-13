# if !defined( __mtc_cstr_h__ )
# define __mtc_cstr_h__
# include "wcsstr.h"

namespace mtc
{

  template <class C>
  class _base_cstr_
  {
    C* pszstr;

  public:     // construction
    _base_cstr_( const C* s = nullptr );
    _base_cstr_( const _base_cstr_& );
   ~_base_cstr_();
    _base_cstr_& operator = ( const _base_cstr_& );
    _base_cstr_& operator = ( const C* );

  public:     // operators
    operator const C* () const;
    operator C* ();

  public:     // initialization
    int     setstr( const C* s, size_t l = (size_t)-1 );
    size_t  length() const {  return pszstr != nullptr ? w_strlen( pszstr ) : 0;  }

  protected:    // helpers
    const std::atomic_int*  getptr() const {  return pszstr != nullptr ? ((std::atomic_int*)pszstr) - 1 : nullptr;  }
          std::atomic_int*  getptr() {  return pszstr != nullptr ? ((std::atomic_int*)pszstr) - 1 : nullptr;  }

  };

  typedef _base_cstr_<char>     cstr;
  typedef _base_cstr_<widechar> wstr;

  template <class C>
  _base_cstr_<C>::_base_cstr_( const C* s ): pszstr( nullptr )
    {
      setstr( s );
    }

  template <class C>
  _base_cstr_<C>::_base_cstr_( const _base_cstr_& s )
    {
      if ( (pszstr = s.pszstr) != nullptr )
        ++*getptr();
    }

  template <class C>
  _base_cstr_<C>::~_base_cstr_()
    {
      if ( pszstr != nullptr && --*getptr() == 0 )
        delete getptr();
    }

  template <class C>
  _base_cstr_<C>& _base_cstr_<C>::operator = ( const _base_cstr_<C>& s )
    {
      if ( pszstr != nullptr && --*getptr() == 0 )
        delete getptr();
      if ( (pszstr = s.pszstr) != nullptr )
        ++*getptr();
      return *this;
    }

  template <class C>
  _base_cstr_<C>& _base_cstr_<C>::operator = ( const C* s )
    {
      setstr( s );
      return *this;
    }

  template <class C>
  _base_cstr_<C>::operator const C* () const
    {
      return pszstr;
    }

  template <class C>
  _base_cstr_<C>::operator C* ()
    {
      return pszstr;
    }

  template <class C>
  int   _base_cstr_<C>::setstr( const C* s, size_t l = (size_t)-1 )
    {
      if ( pszstr != nullptr && --*getptr() == 0 )
        delete getptr();

      if ( l == (size_t)-1 )
      {
        if ( s != (pszstr = nullptr) ) for ( l = 0; s[l] != 0; ++l ) (void)0;
          else return 0;
      }

      if ( (pszstr = (char*)malloc( sizeof(C) * (l + 1) + sizeof(std::atomic_int) )) == nullptr )
        return ENOMEM;

      new( (std::atomic_int*)pszstr ) std::atomic_int( 1 );

      if ( s != nullptr ) pszstr = (char*)memcpy( pszstr + sizeof(std::atomic_int), s, l );
        else pszstr = pszstr + sizeof(std::atomic_int);

      pszstr[l] = 0;
        return 0;
    }

  template <class C>
  _base_cstr_<C>& w_strcpy( _base_cstr_<C>& d, const C* s )
    {
      d.setstr( s ); return d;
    }

  template <class C>
  _base_cstr_<C>& w_strcpy( _base_cstr_<C>& d, const _base_cstr_<C>& s )
    {
      d.setstr( (const C*)s );  return d;
    }

  template <class C>
  _base_cstr_<C>& w_strcat( _base_cstr_<C>& d, const C* s )
    {
      _base_cstr_<C>  oldstr = d;
      size_t          newlen = oldstr.length() + w_strlen( s );

      if ( d.setstr( nullptr, newlen ) == 0 )
        w_strcat( w_strcpy( (C*)d, oldstr ), s );

      return d;
    }

  template <class C>
  _base_cstr_<C>& w_strcat( _base_cstr_<C>& d, const _base_cstr_<C>& s )
    {
      return w_strcat( d, (const C*)s );
    }

}

# endif  // __mtc_cstr_h__
