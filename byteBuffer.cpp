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
# include "byteBuffer.h"
# include "autoptr.h"
# include "array.h"

namespace mtc
{
  class ByteBuffer final: public array<char>, public IByteBuffer
  {
    implement_lifetime_control

  public:
    virtual const char* GetPtr() const override
      {
        return begin();
      }
    virtual word32_t    GetLen() const override
      {
        return size();
      }
    virtual int         SetBuf( const void* p, word32_t l ) override
      {
        if ( SetLen( l ) != 0 )
          return ENOMEM;
        memcpy( begin(), p, l );
          return 0;
      }
    virtual int         SetLen( word32_t l ) override
      {
        return array<char>::SetLen( (int)l );
      }
  };

  int   CreateByteBuffer( IByteBuffer** ppi )
    {
      ByteBuffer* palloc;

      if ( ppi == nullptr )
        return EINVAL;
      if ( (palloc = allocate<ByteBuffer>()) == nullptr )
        return ENOMEM;
      (*ppi = palloc)->Attach();
        return 0;
    }

  int   CreateByteBuffer( IByteBuffer** ppi, const void* memptr, uint32_t length )
    {
      _auto_<ByteBuffer>  palloc;

      if ( ppi == nullptr )
        return EINVAL;
      if ( (palloc = allocate<ByteBuffer>()) == nullptr )
        return ENOMEM;
      if ( palloc->Append( length, (const char*)memptr ) != 0 )
        return ENOMEM;
      (*ppi = palloc.detach())->Attach();
        return 0;
    }

  api<IByteBuffer>  CreateByteBuffer( uint32_t cch )
    {
      _auto_<ByteBuffer>  palloc;

      if ( (palloc = allocate<ByteBuffer>()) != nullptr )
        palloc->SetLen( cch );
      return palloc.detach();
    }

  api<IByteBuffer>  CreateByteBuffer( const void* ptr, uint32_t cch )
    {
      _auto_<ByteBuffer>  palloc;

      if ( (palloc = allocate<ByteBuffer>()) != nullptr )
        palloc->Append( cch, (const char*)ptr );
      return palloc.detach();
    }

}
