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
# if !defined( __mtc_istream_h__ )
# define __mtc_istream_h__
# include "platform.h"
# include "serialize.h"
# include "iBuffer.h"

namespace mtc
{
  //
  // IStream - public interface for sequental reading/writing
  //
  struct  IByteStream: public Iface
  {
    virtual uint32_t  Get(       void*, uint32_t ) = 0;
    virtual uint32_t  Put( const void*, uint32_t ) = 0;
  };

  struct  IFlatStream: public IByteStream
  {
    virtual auto  PGet(                int64_t, uint32_t ) -> mtc::api<IByteBuffer> = 0;
    virtual int   PGet( IByteBuffer**, int64_t, uint32_t ) = 0;
    virtual auto  PGet(       void*,   int64_t, uint32_t ) -> uint32_t = 0;
    virtual auto  PPut( const void*,   int64_t, uint32_t ) -> uint32_t = 0;
    virtual auto  Seek( int64_t                          ) -> int64_t  = 0;
    virtual auto  Size(                                  ) -> int64_t  = 0;
    virtual auto  Tell(                                  ) -> int64_t  = 0;

  };

}

template <> inline
auto  Serialize( mtc::IByteStream* s, const void* p, size_t l ) -> mtc::IByteStream*
  {  return s != nullptr && s->Put( p, static_cast<uint32_t>( l ) ) == l ? s : nullptr;  }

template <> inline
auto  FetchFrom( mtc::IByteStream* s, void* p, size_t l ) -> mtc::IByteStream*
  {  return s != nullptr && s->Get( p, static_cast<uint32_t>( l ) ) == l ? s : nullptr;  }

template <> inline
auto  Serialize( mtc::IFlatStream* s, const void* p, size_t l ) -> mtc::IFlatStream*
  {  return s != nullptr && s->Put( p, static_cast<uint32_t>( l ) ) == l ? s : nullptr;  }

template <> inline
auto  FetchFrom( mtc::IFlatStream* s, void* p, size_t l ) -> mtc::IFlatStream*
  {  return s != nullptr && s->Get( p, static_cast<uint32_t>( l ) ) == l ? s : nullptr;  }

# endif  // __mtc_istream_h__
