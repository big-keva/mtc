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
# pragma once
# if !defined( __mtc_net_sockets_hpp__ )
# define __mtc_net_sockets_hpp__
# include <functional>
# include <cstdint>
# include <string>
# include <memory>
# include <chrono>
# if defined( _WIN32 )
#   include <winerror.h>
# else
#   include <errno.h>
# endif

namespace mtc {
namespace net {
namespace sockets {

  enum error: int {
# if defined( _WIN32 )
    E_INTERRUPT  = WSAEINTR,
    E_WOULDBLOCK = WSAEWOULDBLOCK,
    E_INPROGRESS = WSAEWOULDBLOCK,
    E_NOTSOCK    = WSAENOTSOCK
# else
    E_INTERRUPT  = EINTR,
    E_WOULDBLOCK = EWOULDBLOCK,
    E_INPROGRESS = EINPROGRESS,
    E_NOTSOCK    = ENOTSOCK
# endif
  };

  using socket = void*;

  constexpr socket invalid_socket = nullptr;

  auto  attach( const std::string& host, uint16_t port, uint32_t wait = (uint32_t)-1 ) -> socket;
  auto  listen( const std::string& host, uint16_t port ) -> socket;

  auto  accept( socket, uint32_t wait = (uint32_t)-1 ) -> socket;
  auto  detach( socket ) -> void;

  class select
  {
  public:
    class const_iterator;
    class input_sockets
    {
      friend class select;

      class inner_t;

    public:
      input_sockets()  {}
      input_sockets( const input_sockets& fs ): fdset( fs.fdset ) {}
      input_sockets( socket s )  {  set( s );  }
      input_sockets( const std::initializer_list<socket>& s ) {  set( s );  }
      template <class _It>
      input_sockets( _It beg, _It end )  {  set( beg, end );  }
      template <class _It>
      input_sockets( socket s, _It beg, _It end )  {  set( s ).set( beg, end );  }
      template <class _It>
      input_sockets( const std::initializer_list<socket>& s, _It beg, _It end )  {  set( s ).set( beg, end );  }

    public:
      const_iterator  begin() const;
      const_iterator  end() const;

    public:
      auto  set( socket ) -> input_sockets&;
      auto  set( const std::initializer_list<socket>& s ) -> input_sockets& {  return set( s.begin(), s.end() );  }
      template <class _It>
      auto  set( _It beg, _It end ) -> input_sockets& {  while ( beg != end )  set( *beg++ );  return *this;  }

    protected:
      std::shared_ptr<inner_t>  fdset;

    };

  public:
    select() = default;
    select( const input_sockets&, const input_sockets&, const input_sockets&, uint32_t = (uint32_t)-1 );

  public:
    auto  read() const -> const input_sockets&;
    auto  send() const -> const input_sockets&;
    auto  expt() const -> const input_sockets&;

  public:
    static  auto  read( socket, uint32_t wait = (uint32_t)-1 ) -> unsigned;
    static  auto  send( socket, uint32_t wait = (uint32_t)-1 ) -> unsigned;
    static  auto  both( socket, uint32_t wait = (uint32_t)-1 ) -> unsigned;   // 0x01 for read, 0x02 for write

  protected:
    input_sockets rd;
    input_sockets wr;
    input_sockets ex;

  };

  class select::const_iterator
  {
    friend class select;

    std::shared_ptr<input_sockets::inner_t> fdset;
    socket                                  fnext = invalid_socket;

    const_iterator( std::shared_ptr<input_sockets::inner_t> );

  public:
    const_iterator() = default;
    const_iterator( const const_iterator& );
    auto  operator = ( const const_iterator& ) -> const_iterator&;

    auto  operator ++ () -> const_iterator&;
    auto  operator ++ ( int ) -> const_iterator;

    bool  operator == ( const const_iterator& it ) const;
    bool  operator != ( const const_iterator& it ) const  {  return !(*this == it);  }

    auto  operator * () const -> socket;
  };

  namespace io
  {
    auto  read( socket,       void*, size_t, uint32_t wait = (uint32_t)-1 ) -> int;
    auto  send( socket, const void*, size_t, uint32_t wait = (uint32_t)-1 ) -> int;
  }

  auto  get_peer_host( socket ) -> std::string;
  auto  get_last_error() -> int;

  struct timed_socket
  {
    socket    sockid;
    uint32_t  waitms;

  public:
    timed_socket( socket s, uint32_t t = (uint32_t)-1 ): sockid( s ), waitms( t )  {}

  public:
    auto  ptr() const -> timed_socket*  {  return (timed_socket*)this;  }
  };

}}}

// FetchFrom()
// Serialize()
//
// Могут бросить mtc::net::sockets::error с кодом ETIMEDOUT при таймауте, остальные коды - системные для сокетов
//
auto  FetchFrom( mtc::net::sockets::timed_socket*, void*, size_t ) -> mtc::net::sockets::timed_socket*;
auto  Serialize( mtc::net::sockets::timed_socket*, const void*, size_t ) -> mtc::net::sockets::timed_socket*;

# endif   // __mtc_net_sockets_hpp__
