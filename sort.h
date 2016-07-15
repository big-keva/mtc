# if !defined( __mtc_sort_h__ )
# define __mtc_sort_h__

namespace mtc
{
  template <class T>
  void  __inplace_swap__( T& t1, T& t2 )
    {
      char* c1 = (char*)&t1;
      char* c2 = (char*)&t2;
      int   cc = sizeof(t1);

      while ( cc-- > 0 )  {  char c = *c1;  *c1++ = *c2;  *c2++ = c;  }
    }

  template<class _iterator, class T, class compare>
  void  __inplace_sort__( _iterator first, _iterator last, const T& pv, compare cmp )
  {
      T*        v = (first + ((last - first)/2));
      _iterator f = first;
      _iterator l = last;

      do
      {
        while ( f < last  && cmp( *f, *v ) < 0 ) ++f;
        while ( l > first && cmp( *v, *l ) < 0 ) --l;

        if ( f <= l )
          __inplace_swap__( *f++, *l-- );
      } while ( f <= l );

      if ( l > first )
        __inplace_sort__( first, l, *v, cmp );
      if ( last > f )
        __inplace_sort__( f, last, *v, cmp );
  }

  template<typename _iterator, typename compare>
  void  inplace_sort(_iterator first, _iterator last, compare cmp )
  {
    if ( first < --last )
      __inplace_sort__( first, last, *first, cmp );    
  }

}

# endif  // __mtc_sort_h__
