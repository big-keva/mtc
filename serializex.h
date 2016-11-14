/*

The MIT License (MIT)

Copyright (c) 2000-2016 Андрей Коваленко aka Keva
  keva@meta.ua
  keva@rambler.ru
  skype: big_keva
  phone: +7(495)648-4058, +7(916)015-5592

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

=============================================================================

Данная лицензия разрешает лицам, получившим копию данного программного обеспечения
и сопутствующей документации (в дальнейшем именуемыми «Программное Обеспечение»),
безвозмездно использовать Программное Обеспечение без ограничений, включая неограниченное
право на использование, копирование, изменение, слияние, публикацию, распространение,
сублицензирование и/или продажу копий Программного Обеспечения, а также лицам, которым
предоставляется данное Программное Обеспечение, при соблюдении следующих условий:

Указанное выше уведомление об авторском праве и данные условия должны быть включены во
все копии или значимые части данного Программного Обеспечения.

ДАННОЕ ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ПРЕДОСТАВЛЯЕТСЯ «КАК ЕСТЬ», БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ,
ЯВНО ВЫРАЖЕННЫХ ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ ГАРАНТИИ ТОВАРНОЙ ПРИГОДНОСТИ,
СООТВЕТСТВИЯ ПО ЕГО КОНКРЕТНОМУ НАЗНАЧЕНИЮ И ОТСУТСТВИЯ НАРУШЕНИЙ, НО НЕ ОГРАНИЧИВАЯСЬ
ИМИ.

НИ В КАКОМ СЛУЧАЕ АВТОРЫ ИЛИ ПРАВООБЛАДАТЕЛИ НЕ НЕСУТ ОТВЕТСТВЕННОСТИ ПО КАКИМ-ЛИБО ИСКАМ,
ЗА УЩЕРБ ИЛИ ПО ИНЫМ ТРЕБОВАНИЯМ, В ТОМ ЧИСЛЕ, ПРИ ДЕЙСТВИИ КОНТРАКТА, ДЕЛИКТЕ ИЛИ ИНОЙ
СИТУАЦИИ, ВОЗНИКШИМ ИЗ-ЗА ИСПОЛЬЗОВАНИЯ ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ ИЛИ ИНЫХ ДЕЙСТВИЙ
С ПРОГРАММНЫМ ОБЕСПЕЧЕНИЕМ.

*/
# if !defined( __serializex_h__)
# define __serializex_h__
# include "platform.h"
# include "serialize.h"
# include "autoptr.h"
# include "wcsstr.h"
# include "array.h"

/* _auto_<char> */

  template <class M>
  inline  unsigned  GetBufLen( const mtc::_auto_<char, M>& s )
    {
      unsigned  l = (unsigned)w_strlen( s );

      return GetBufLen( l ) + l;
    }
  template <class O, class M>
  O*        Serialize( O* o, const mtc::_auto_<char, M>& s )
    {
      unsigned  l = (unsigned)w_strlen( s );

      return Serialize( Serialize( o, l ), (const char*)s, l );
    }
  template <class S, class M = mtc::def_alloc<>>
  S*        FetchFrom( S* s, mtc::_auto_<char, M>& o )
    {
      unsigned  l;

      if ( (s = FetchFrom( s, l )) == nullptr || (o = (char*)M().alloc( l + 1 )) == nullptr )
        return nullptr;
      ((char*)o)[l] = 0;
        return FetchFrom( s, (char*)o, l );
    }

/* _auto_<widechar> */

  template <class M>
  inline  unsigned  GetBufLen( const mtc::_auto_<widechar, M>& s )
    {
      unsigned l = (unsigned)w_strlen( s );

      return GetBufLen( l ) + sizeof(widechar) * l;
    }
  template <class O, class M>
  O*        Serialize( O* o, const mtc::_auto_<widechar, M>& s )
    {
      unsigned  l = (unsigned)w_strlen( s );

      return Serialize( Serialize( o, l ), (const widechar*)s, sizeof(widechar) * l );
    }
  template <class S, class M = mtc::def_alloc<>>
  S*        FetchFrom( S* s, mtc::_auto_<widechar, M>& o )
    {
      unsigned  l;

      if ( (s = FetchFrom( s, l )) == nullptr || (o = (widechar*)M().alloc( sizeof(widechar) * (l + 1) )) == nullptr )
        return nullptr;
      o[l] = 0;
        return FetchFrom( s, (widechar*)o, sizeof(widechar) * l );
    }

/* array<T> */

  template <class T, class M>
  unsigned  GetBufLen( const mtc::array<T, M>& a )
    {
      unsigned  l = GetBufLen( a.size() );

      for ( auto p = a.begin(); p < a.end(); )
        l += GetBufLen( *p++ );

      return l;
    }
  template <class O, class T, class M>
  O*        Serialize( O* o, const mtc::array<T, M>& a )
    {
      if ( (o = Serialize( o, a.size() )) != nullptr )
        a.for_each( [&o]( const T& t ){  o = Serialize( o, t );  return (int)(o == nullptr);  } );
      return o;
    }
  template <class S, class T, class M>
  S*        FetchFrom( S* s, mtc::array<T, M>& a )
    {
      int   l;

      if ( (s = FetchFrom( s, l )) == nullptr || a.SetLen( l ) != 0 )
        return nullptr;
      for ( auto p = a.begin(); s != nullptr && p < a.end(); )
        s = FetchFrom( s, *p++ );

      return s;
    }

# define derive_ordinal_serialize( _type_ )                                         \
  template <class M>  inline unsigned GetBufLen( const mtc::array<_type_, M>& a )   \
    {                                                                               \
      return GetBufLen( a.size() ) + sizeof(_type_) * a.size();                     \
    }                                                                               \
  template <class O, class M> inline O* Serialize( O* o, const mtc::array<_type_, M>& a ) \
    {                                                                               \
      return Serialize( Serialize( o, a.size() ),                                   \
        (const _type_*)a, sizeof(_type_) * a.size() );                              \
    }                                                                               \
  template <class S, class M> inline S* FetchFrom( S* s, mtc::array<_type_, M>& a ) \
    {                                                                               \
      int l;                                                                        \
      return (s = FetchFrom( s, l )) != nullptr && a.SetLen( l ) == 0 ?             \
        FetchFrom( s, a.begin(), sizeof(_type_) * l ) : nullptr;                    \
    }
    
  derive_ordinal_serialize( mtc::char_t )
  derive_ordinal_serialize( mtc::byte_t )
  derive_ordinal_serialize( float )
  derive_ordinal_serialize( double )

# undef derive_ordinal_serialize

# endif  // __serializex_h__
