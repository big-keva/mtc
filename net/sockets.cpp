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
# include "sockets.hpp"
# include <algorithm>
# include <stdexcept>
# include <cassert>
# include <cstring>
# include <chrono>
# include <memory>

# if defined( _WIN32 )
#   include <WinSock2.h>
#   include <ws2ipdef.h>
#   include <WS2tcpip.h>
# else
#   include <netinet/tcp.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#   include <unistd.h>
#   include <netdb.h>
#   include <fcntl.h>
# endif   // _WIN32

inline  auto  make_timeval( uint32_t milliseconds ) -> struct timeval
{
  return timeval{ (long)(milliseconds / 1000), (int)(milliseconds % 1000) * 1000 };
}

namespace mtc {
namespace net {
namespace sockets {

# if defined( _WIN32 )
  class api final: private WSADATA
  {
  public:
    api();
   ~api();

  public:
    using socket = ::SOCKET;

    enum: socket  {  InvalidSocket = INVALID_SOCKET  };
# else
  class api final
  {
  public:
    using socket = int;

    enum: socket  {  InvalidSocket = -1  };
# endif

  public:
    static  auto  GetLastError() -> int;
    static  void  DetachSocket( socket );
    static  void  SetBlockMode( socket, bool );
    static  void  DeclineDelay( socket );
    static  auto  CastToSocket( socket ) -> sockets::socket;
    static  auto  CastToHandle( sockets::socket ) -> socket;
  };

  api system;

  // api implementation

# if defined( _WIN32 )
# pragma comment( lib, "ws2_32.lib" )

  api::api()  {  (void)WSAStartup( 0x0202, this );  }
  api::~api() {  WSACleanup();  }

  auto  api::GetLastError() -> int  {  return WSAGetLastError();  }
  void  api::DetachSocket( socket fd ) {  closesocket( fd );  }
  void  api::SetBlockMode( socket fd, bool enable )
  {
    u_long  nonblk = enable ? 0 : 1;

    ioctlsocket( fd, FIONBIO, &nonblk );
  }
# else
  auto  api::GetLastError() -> int  {  return errno;  }
  void  api::DetachSocket( socket fd ) {  close( fd );  }
  void  api::SetBlockMode( socket fd, bool on_off )
  {
    fcntl( fd, F_SETFL, on_off ? (fcntl( fd, F_GETFL ) |  O_NONBLOCK) :
                                 (fcntl( fd, F_GETFL ) & ~O_NONBLOCK) );
  }
# endif

  void  api::DeclineDelay( socket fd )
  {
    int   flag = 1;

    if ( setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int) ) < 0 )
      throw std::runtime_error( "could not switch off Negail non-delay scheme" );
  }

  auto  api::CastToSocket( socket s ) -> sockets::socket  {  return reinterpret_cast<sockets::socket>( s );  }
  auto  api::CastToHandle( sockets::socket s ) -> socket  {  return (socket)(uint64_t)s;  }

  // select::input_sockets implementation

  class select::input_sockets::inner_t
  {
  public:
    fd_set  fds;
    socket  max = invalid_socket;

  public:
    inner_t( )  {  FD_ZERO( &fds );  }

    void  set( sockets::socket s )
      {
        if ( s != invalid_socket )
        {
          FD_SET( api::CastToHandle( s ), &fds );

          if ( (int)api::CastToHandle( s ) > (int)api::CastToHandle( max ) )
            max = s;
        }
      }
  };

  auto  select::input_sockets::begin() const -> const_iterator  {  return const_iterator( fdset );  }
  auto  select::input_sockets::end() const -> const_iterator  {  return const_iterator();  }

  auto  select::input_sockets::set( socket s ) -> input_sockets&
  {
    if ( fdset == nullptr )
      fdset = std::make_shared<inner_t>();
    return fdset->set( s ), *this;
  }

  // select::const_iterator implementation

  select::const_iterator::const_iterator( std::shared_ptr<input_sockets::inner_t> p ):
    fdset( p )
  {
    auto  snext = api::CastToHandle( invalid_socket );

    if ( fdset != nullptr )
    {
      while ( fnext == invalid_socket && snext <= api::CastToHandle( fdset->max ) )
        if ( FD_ISSET( snext, &fdset->fds ) ) fnext = api::CastToSocket( snext );
          else ++snext;
    }

    if ( fnext == invalid_socket )
      fdset.reset();
  }

  select::const_iterator::const_iterator( const const_iterator& it ):
    fdset( it.fdset ), fnext( it.fnext )  {}

  auto  select::const_iterator::operator = ( const const_iterator& it ) -> const_iterator&
  {
    fdset = it.fdset;
    fnext = it.fnext;
    return *this;
  }

  auto  select::const_iterator::operator ++ () -> const_iterator&
  {
    if ( fdset == nullptr )
      throw std::invalid_argument( "invalid ++ call to uninitialized iterator" );

    for ( auto  snext = api::CastToHandle( fnext ) + 1; snext <= api::CastToHandle( fdset->max ); ++snext )
      if ( FD_ISSET( snext, &fdset->fds ) )
        return fnext = api::CastToSocket( snext ), *this;

    return fnext = invalid_socket, fdset.reset(), *this;
  }

  auto  select::const_iterator::operator ++ ( int ) -> const_iterator
  {
    auto  ret( *this );
      operator ++();
    return ret;
  }

  bool  select::const_iterator::operator == ( const const_iterator& it ) const
  {
    return fnext == it.fnext && fdset == it.fdset;
  }

  auto  select::const_iterator::operator * () const -> socket
  {
    if ( fnext == invalid_socket )
      throw std::invalid_argument( "invalid * call to iterator" );
    return fnext;
  }

  // select implementation

  select::select( const input_sockets& r, const input_sockets& w, const input_sockets& x, uint32_t waitms ):
    rd( r ), wr( w ), ex( x )
  {
    auto  w_time = make_timeval( waitms );
    int   so_max = -1;

    if ( rd.fdset != nullptr && (int)api::CastToHandle( rd.fdset->max ) > so_max )
      so_max = api::CastToHandle( rd.fdset->max );
    if ( wr.fdset != nullptr && (int)api::CastToHandle( wr.fdset->max ) > so_max )
      so_max = api::CastToHandle( wr.fdset->max );
    if ( ex.fdset != nullptr && (int)api::CastToHandle( ex.fdset->max ) > so_max )
      so_max = api::CastToHandle( ex.fdset->max );

    if ( so_max != -1 )
    {
      auto  rds = rd.fdset != nullptr ? &rd.fdset->fds : nullptr;
      auto  wrs = wr.fdset != nullptr ? &wr.fdset->fds : nullptr;
      auto  exs = ex.fdset != nullptr ? &ex.fdset->fds : nullptr;
      auto  res = ::select( 1 + so_max, rds, wrs, exs, &w_time );

      if ( res <= 0 )
      {
        rd.fdset.reset();
        wr.fdset.reset();
        ex.fdset.reset();
      }
    }
  }

  auto  select::read() const -> const input_sockets&  {  return rd;  }
  auto  select::send() const -> const input_sockets&  {  return wr;  }
  auto  select::expt() const -> const input_sockets&  {  return ex;  }

  auto  select::read( socket sockid, uint32_t waitms ) -> unsigned
  {
    fd_set  rd_set;
    auto    w_time = make_timeval( waitms );

    if ( sockid == invalid_socket )
      return 0;

    FD_ZERO( &rd_set );   FD_SET( api::CastToHandle( sockid ), &rd_set );

    return ::select( 1 + (int)api::CastToHandle( sockid ), &rd_set, nullptr, nullptr, &w_time ) > 0 ? 0x01 : 0x00;
  }

  auto  select::send( socket sockid, uint32_t waitms ) -> unsigned
  {
    fd_set  wr_set;
    auto    w_time = make_timeval( waitms );

    if ( sockid == invalid_socket )
      return 0;

    FD_ZERO( &wr_set );   FD_SET( api::CastToHandle( sockid ), &wr_set );

    return ::select( 1 + (int)api::CastToHandle( sockid ), nullptr, &wr_set, nullptr, &w_time ) > 0 ? 0x02 : 0x00;
  }

  auto  select::both( socket sockid, uint32_t waitms ) -> unsigned
  {
    fd_set  rd_set;   FD_ZERO( &rd_set );   FD_SET( api::CastToHandle( sockid ), &rd_set );   
    fd_set  wr_set = rd_set;
    auto    w_time = make_timeval( waitms );
    auto    result = 0U;

    if ( sockid == invalid_socket )
      return 0;

    if ( ::select( 1 + (int)api::CastToHandle( sockid ), &rd_set, &wr_set, nullptr, &w_time ) > 0 )
    {
      if ( FD_ISSET( api::CastToHandle( sockid ), &rd_set ) ) result |= 0x01;
      if ( FD_ISSET( api::CastToHandle( sockid ), &wr_set ) ) result |= 0x02;
    }
    return result;
  }

  // socket impementation

  auto  attach( const std::string& host, uint16_t port, uint32_t wait ) -> socket
  {
    sockaddr_in scaddr;
    api::socket sockid;
    int         nerror;
    int         serror;

  // fill socket address
    memset( &scaddr, 0, sizeof(scaddr) );
      scaddr.sin_family      = AF_INET;  
      scaddr.sin_addr.s_addr = INADDR_ANY;  
      scaddr.sin_port        = htons( port );

    if ( (scaddr.sin_addr.s_addr = inet_addr( host.c_str() )) == INADDR_NONE )
    {
      struct hostent* lphost;

      if ( (lphost = gethostbyname( host.c_str() )) == nullptr )
        return invalid_socket;

      scaddr.sin_addr.s_addr = *(uint32_t*)lphost->h_addr_list[0];
    }

    if ( (sockid = ::socket( AF_INET, SOCK_STREAM, 0 )) == api::InvalidSocket )
      return invalid_socket;

    // set socket to be non-blocking
    api::SetBlockMode( sockid, wait == (uint32_t)-1 );

    // try connect socket
    if ( (nerror = ::connect( sockid, (const sockaddr*)&scaddr, sizeof(scaddr) )) == 0 )
      return api::DeclineDelay( sockid ), api::CastToSocket( sockid );

    assert( nerror < 0 );

    /*  на платформе _WIN32 типичным возвратом функции является WSAEWOULDBLOCK, */
    /*  что означает, что коннект пошёл устанавливаться; следует подождать,     */
    /*  пока для select() с указанным таймаутом не будет выставлен writefds     */
    /*  как признак успешного завершения, или exceptfds как ошибка              */
    if ( (serror = api::GetLastError()) == E_WOULDBLOCK || serror == E_INPROGRESS || serror == EAGAIN )
    {
      fd_set  ms_fds;
      auto    tbreak = make_timeval( wait );

      FD_ZERO( &ms_fds );   FD_SET( sockid, &ms_fds );

      do {
        auto  rd_fds = ms_fds;
        auto  wr_fds = ms_fds;

        nerror = ::select( 1 + (int)sockid, &rd_fds, &wr_fds, nullptr, &tbreak );
      } while ( nerror < 0 && (serror = api::GetLastError()) == E_INTERRUPT );

      if ( nerror > 0 )
        return api::DeclineDelay( sockid ), api::CastToSocket( sockid );
    }
    return api::DetachSocket( sockid ), invalid_socket;
  }

  auto  listen( const std::string& host, uint16_t port ) -> socket
  {
    sockaddr_in scaddr;
    api::socket sockid;
    int         socopt = 1;

  // fill socket address
    memset( &scaddr, 0, sizeof(scaddr) );
      scaddr.sin_family      = AF_INET;  
      scaddr.sin_addr.s_addr = INADDR_ANY;  
      scaddr.sin_port        = htons( port );

    if ( (scaddr.sin_addr.s_addr = inet_addr( host.c_str() )) == INADDR_NONE )
    {
      struct hostent* lphost;

      if ( (lphost = gethostbyname( host.c_str() )) == nullptr )
        return invalid_socket;

      scaddr.sin_addr.s_addr = *(uint32_t*)lphost->h_addr_list[0];
    }

    if ( (sockid = ::socket( AF_INET, SOCK_STREAM, 0 )) == api::InvalidSocket )
      return invalid_socket;

    setsockopt( sockid, SOL_SOCKET, SO_REUSEADDR, (const char*)&socopt, sizeof(socopt) );

  // bind to port
    if ( bind( sockid, (sockaddr*)&scaddr, sizeof(scaddr) ) != 0 )
      return api::DetachSocket( sockid ), invalid_socket;

  // set che socket to the listen state
    if ( ::listen( sockid, 100 ) != 0 )
      return api::DetachSocket( sockid ), invalid_socket;

    return api::CastToSocket( sockid );
  }

  auto  accept( socket sockid, uint32_t waitms ) -> socket
  {
    api::socket getone;

    if ( waitms != (uint32_t)-1 )
    {
      auto    bkr = make_timeval( waitms );
      fd_set  rds;
      fd_set  wds;
      int     err;

      FD_ZERO( &rds );  FD_SET( api::CastToHandle( sockid ), &rds );  wds = rds;

      if ( (err = ::select( 1 + (int)api::CastToHandle( sockid ), &rds, &wds, nullptr, &bkr )) <= 0 )
        return invalid_socket;
    }

    if ( (getone = ::accept( api::CastToHandle( sockid ), nullptr, nullptr )) == api::InvalidSocket )
      return invalid_socket;

    return api::CastToSocket( getone );
  }

  auto  detach( socket sockid ) -> void
  {
    api::DetachSocket( api::CastToHandle( sockid ) );
  }

  namespace io
  {
    auto  read( socket sockid, void* p, size_t l, uint32_t t ) -> int
    {
      if ( sockid == invalid_socket )
        return -1;

      api::SetBlockMode( api::CastToHandle( sockid ), true );

      if ( t != (uint32_t)-1 )
      {
        fd_set  rd_set;
        auto    tbreak = make_timeval( t );
        int     nerror;

        FD_ZERO( &rd_set );
        FD_SET ( api::CastToHandle( sockid ), &rd_set );

        if ( (nerror = ::select( 1 + (int)api::CastToHandle( sockid ), &rd_set, 0, 0, &tbreak )) < 0 )
          return nerror;

        if ( nerror == 0 )      // timeout
          return -1;

        api::SetBlockMode( api::CastToHandle( sockid ), false );
      }
      return ::recv( api::CastToHandle( sockid ), (char*)p, (int)l, 0 );
    }

    auto  send( socket sockid, const void* p, size_t l, uint32_t t ) -> int
    {
      if ( sockid == invalid_socket )
        return -1;

      api::SetBlockMode( api::CastToHandle( sockid ), true );

      if ( t != (uint32_t)-1 )
      {
        fd_set  wr_set;
        auto    tbreak = make_timeval( t );
        int     nerror;

        FD_ZERO( &wr_set );
        FD_SET ( api::CastToHandle( sockid ), &wr_set );

        if ( (nerror = ::select( 1 + (int)api::CastToHandle( sockid ), 0, &wr_set, 0, &tbreak )) < 0 )
          return nerror;

        if ( nerror == 0 )      // timeout
          return -1;

        api::SetBlockMode( api::CastToHandle( sockid ), false );
      }
      return ::send( api::CastToHandle( sockid ), (const char*)p, (int)l, 0 );
    }

  }

  auto  get_peer_host( socket s ) -> std::string
  {
    struct sockaddr_storage scaddr;
    socklen_t               scsize = sizeof(scaddr);
    char                    ip_str[INET6_ADDRSTRLEN];

    if ( getpeername( api::CastToHandle( s ), (struct sockaddr*)&scaddr, &scsize ) != 0 )
      return "";

  // deal with both IPv4 and IPv6:
    if ( scaddr.ss_family == AF_INET )
    {
      struct sockaddr_in *s = (struct sockaddr_in *)&scaddr;
      inet_ntop( AF_INET, &s->sin_addr, ip_str, sizeof(ip_str) );
    }
      else
    { // AF_INET6
      struct sockaddr_in6 *s = (struct sockaddr_in6 *)&scaddr;
      inet_ntop(AF_INET6, &s->sin6_addr, ip_str, sizeof(ip_str) );
    }
    return ip_str;
  }

  auto  get_last_error() -> int
  {
    return api::GetLastError();
  }

}}}

//
// reading from blocking/non-blocking socket
//
auto  FetchFrom( mtc::net::sockets::timed_socket* s, void* p, size_t l ) -> mtc::net::sockets::timed_socket*
{
  auto  outptr = (char*)p;
  auto  outend = l + outptr;
  auto  tstart = std::chrono::steady_clock::now();

  if ( s == nullptr || outptr == outend )
    return s;

  for ( auto tmcurr = tstart; s != nullptr && outptr != outend; tmcurr = std::chrono::steady_clock::now() )
  {
    auto  tms = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>( tmcurr - tstart ).count(); // ms since start
    auto  cbr = mtc::net::sockets::io::read( s->sockid, outptr, outend - outptr, tms <= s->waitms ? s->waitms - tms : 0 );

    if ( cbr > 0 )  // received data
    {
      outptr += cbr;
      continue;
    }

    if ( cbr < 0 )
    {
      auto  err = mtc::net::sockets::get_last_error();

      if ( err == EAGAIN || err == EWOULDBLOCK || err == mtc::net::sockets::E_WOULDBLOCK )
        if ( std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - tstart ).count() < s->waitms )
          continue;
    }
    return nullptr;
  }
  return s;
}

auto  Serialize( mtc::net::sockets::timed_socket* o, const void* p, size_t l ) -> mtc::net::sockets::timed_socket*
{
  auto    srcptr = (const char*)p;
  auto    srcend = l + srcptr;
  auto    tstart = std::chrono::steady_clock::now();

  if ( o == nullptr || srcptr == srcend )
    return o;

  for ( auto tmcurr = tstart; o != nullptr && srcptr != srcend; tmcurr = std::chrono::steady_clock::now() )
  {
    auto  tms = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>( tmcurr - tstart ).count(); // ms since start
    auto  cbw = mtc::net::sockets::io::send( o->sockid, srcptr, srcend - srcptr, tms <= o->waitms ? o->waitms - tms : 0 );

    if ( cbw > 0 )                                              // received data
    {
      srcptr += cbw;
      continue;
    }

    if ( cbw < 0 )                                  // error reading data
    {
      int   err = mtc::net::sockets::get_last_error();     // EAGAIN or EWOULDBLOCK for non-blocking soclets - continue until time passed

      if ( err == EAGAIN || err == EWOULDBLOCK || err == mtc::net::sockets::E_WOULDBLOCK )
        if ( std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - tstart ).count() < o->waitms )
          continue;
    }
    return nullptr;
  }
  return o;
}
