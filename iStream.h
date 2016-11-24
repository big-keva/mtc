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
# include "iBuffer.h"

namespace mtc
{
  //
  // IStream - public interface for sequental reading/writing
  //
  struct  IByteStream: public Iface
  {
    virtual word32_t  Get(       void*, word32_t ) = 0;
    virtual word32_t  Put( const void*, word32_t ) = 0;
  };

  struct  IFlatStream: public IByteStream
  {
    virtual int       GetBuf( IByteBuffer**, int64_t, word32_t ) = 0;
    virtual word32_t  PosGet(       void*,   int64_t, word32_t ) = 0;
    virtual word32_t  PosPut( const void*,   int64_t, word32_t ) = 0;
    virtual int64_t   Seek  ( int64_t                          ) = 0;
    virtual int64_t   Size  (                                  ) = 0;
    virtual int64_t   Tell  (                                  ) = 0;
  };

}

inline  mtc::IByteStream* Serialize( mtc::IByteStream* s, char c )
  {  return s != nullptr && s->Put( &c, sizeof(c) ) == sizeof(c) ? s : nullptr;  }
inline  mtc::IByteStream* Serialize( mtc::IByteStream* s, const void* p, unsigned l )
  {  return s != nullptr && s->Put( p, l ) == l ? s : nullptr;  }

inline  mtc::IFlatStream* Serialize( mtc::IFlatStream* s, char c )
  {  return s != nullptr && s->Put( &c, sizeof(c) ) == sizeof(c) ? s : nullptr;  }
inline  mtc::IFlatStream* Serialize( mtc::IFlatStream* s, const void* p, unsigned l )
  {  return s != nullptr && s->Put( p, l ) == l ? s : nullptr;  }

inline  mtc::IByteStream* FetchFrom( mtc::IByteStream* s, char& c )
  {  return s != nullptr && s->Get( &c, sizeof(c) ) == sizeof(c) ? s : nullptr;  }
inline  mtc::IByteStream* FetchFrom( mtc::IByteStream* s, void* p, unsigned l )
  {  return s != nullptr && s->Get( p, l ) == l ? s : nullptr;  }

inline  mtc::IFlatStream* FetchFrom( mtc::IFlatStream* s, char& c )
  {  return s != nullptr && s->Get( &c, sizeof(c) ) == sizeof(c) ? s : nullptr;  }
inline  mtc::IFlatStream* FetchFrom( mtc::IFlatStream* s, void* p, unsigned l )
  {  return s != nullptr && s->Get( p, l ) == l ? s : nullptr;  }

inline  int64_t           GetCurPos( mtc::IFlatStream* s )
  {  return s != nullptr ? s->Tell() : -1;  }
inline  mtc::IFlatStream*  SetCurPos( mtc::IFlatStream* s, mtc::word64_t offset )
  {  return s != nullptr && s->Seek( (int64_t)offset ) == (int64_t)offset ? s : nullptr;  }

# endif  // __mtc_istream_h__
