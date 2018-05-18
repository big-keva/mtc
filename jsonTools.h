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
# if !defined( __jsonTools_h__ )
# define __jsonTools_h__
# include "json.h"

namespace mtc
{
  using ParseJsonError = json::ParseError;

  using json_use_exceptions_t = enable_exceptions_t;
  using json_no_exceptions_t = disable_exceptions_t;

  constexpr json_use_exceptions_t json_use_exceptions{};
  constexpr json_no_exceptions_t  json_no_exceptions{};

  using jsonRevive = json::Revive;

# define  derive_revive( _type_ )                   \
    template <class M = def_alloc, class K>         \
    auto  add_##_type_( K k, jsonRevive* p = nullptr )  {  return json::add_##_type_( k, p );  }

  derive_revive( char )
  derive_revive( byte )
  derive_revive( int16 )
  derive_revive( word16 )
  derive_revive( int32 )
  derive_revive( word32 )
  derive_revive( int64 )
  derive_revive( word64 )
  derive_revive( float )
  derive_revive( double )
  derive_revive( charstr )
  derive_revive( widestr )
  derive_revive( buffer )
  derive_revive( array_char )
  derive_revive( array_byte )
  derive_revive( array_int16 )
  derive_revive( array_word16 )
  derive_revive( array_int32 )
  derive_revive( array_word32 )
  derive_revive( array_int64 )
  derive_revive( array_word64 )
  derive_revive( array_float )
  derive_revive( array_double )
  derive_revive( array_charstr )
  derive_revive( array_widestr )
  derive_revive( array_buffer )
# undef derive_revive

# define  derive_revive( _type_ )                                         \
  template <class M = def_alloc, class K>                                 \
  auto  add_##_type_( K k, jsonRevive* subn, jsonRevive* next = nullptr ) \
    {  return json::add_##_type_( k, subn, next );  }

  derive_revive( zarray )
  derive_revive( array_zarray )
  
# undef derive_revive

// JSON decoration modes
  using json_compact = json::print_compact;
  using json_decorated = json::print_decorated;

// JSON serialization

  template <class O, class T, class D = json_decorated>
  O*  PrintJson( O* o, T t, const D& d = D() )  {  return json::Print( o, t, d );  }

  template <class O, class S>
  O*  PrintText( O* o, S s, size_t l )          {  return json::PrintText( o, s, l );  }

  template <class S, class M, class X = json_use_exceptions_t>
  S*  ParseJson( S* s, xvalue<M>& x, const jsonRevive* p = nullptr, const X& e = X() )  {  return json::Parse( s, x, p, e );  }

  template <class S, class M, class X = json_use_exceptions_t>
  S*  ParseJson( S* s, zarray<M>& z, const jsonRevive* p = nullptr, const X& x = X() )  {  return json::Parse( s, z, p, x );  }

}

# endif  // __jsonTools_h__
