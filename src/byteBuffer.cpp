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
# include "../byteBuffer.h"
# include <cstring>
# include <vector>

namespace mtc
{
  template <class error>
  class ByteBuffer final: public std::vector<char>, public IByteBuffer
  {
    implement_lifetime_control

    void  operator delete( void* p )  {  nothrow_allocator::free( p );  }

  public:
    const char* GetPtr() const override {  return data();  }
    word32_t    GetLen() const override {  return size();  }
    int         SetBuf( const void* p, word32_t l ) override
      {
        try
        {  return resize( l ), memcpy( data(), p, l ), 0;  }
        catch ( const std::bad_alloc& x )
        {  return error::result( x, ENOMEM );  }
      }
    int         SetLen( word32_t l ) override
      {
        try
        {  return resize( l ), 0;  }
        catch ( const std::bad_alloc& x )
        {  return error::result( x, ENOMEM );  }
      }
  };

  struct throw_error
  {
    template<class X, class R>
    static R  result( const X& except, R ) {  throw except;  }
  };

  struct nothrow_error
  {
    template<class X, class R>
    static R  result( const X&, R result ) {  return result;  }
  };

  int   CreateByteBuffer( IByteBuffer** ppi )
  {
    ByteBuffer<nothrow_error>*  palloc;

    if ( ppi == nullptr )
      return EINVAL;

    if ( (palloc = allocate_with<nothrow_allocator, ByteBuffer<nothrow_error>>()) == nullptr )
      return ENOMEM;

    return (*ppi = palloc)->Attach(), 0;
  }

  int   CreateByteBuffer( IByteBuffer** ppi, const void* memptr, uint32_t length )
  {
    ByteBuffer<nothrow_error>*  palloc;

    if ( ppi == nullptr )
      return EINVAL;

    if ( (palloc = allocate_with<nothrow_allocator, ByteBuffer<nothrow_error>>()) == nullptr )
      return ENOMEM;

    if ( palloc->SetBuf( memptr, length ) != 0 )
      return delete palloc, ENOMEM;

    return (*ppi = palloc)->Attach(), 0;
  }

  api<IByteBuffer>  CreateByteBuffer( uint32_t cch, const enable_exceptions_t& )
  {
    auto  palloc = api<IByteBuffer>( new ByteBuffer<throw_error>() );

    return palloc->SetLen( cch ), palloc.ptr();
  }

  api<IByteBuffer>  CreateByteBuffer( uint32_t cch, const disable_exceptions_t& )
  {
    api<IByteBuffer>  palloc;

    return CreateByteBuffer( (IByteBuffer**)palloc ) == 0
      && palloc->SetLen( cch ) == 0 ? palloc : nullptr;
  }

  api<IByteBuffer>  CreateByteBuffer( const void* ptr, uint32_t cch, const enable_exceptions_t& )
  {
    auto  palloc = api<IByteBuffer>( new ByteBuffer<throw_error>() );

    return palloc->SetBuf( ptr, cch ), palloc.ptr();
  }

  api<IByteBuffer>  CreateByteBuffer( const void* ptr, uint32_t cch, const disable_exceptions_t& )
  {
    api<IByteBuffer>  palloc;

    return CreateByteBuffer( (IByteBuffer**)palloc, ptr, cch ) == 0 ? palloc : nullptr;
  }

}
