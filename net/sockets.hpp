# pragma once
# if !defined( __mtc_net_sockets_hpp__ )
# define __mtc_net_sockets_hpp__
# include <cstdint>
# include <string>
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

  namespace select
  {
    auto  read( socket, uint32_t wait = (uint32_t)-1 ) -> unsigned;
    auto  send( socket, uint32_t wait = (uint32_t)-1 ) -> unsigned;
    auto  both( socket, uint32_t wait = (uint32_t)-1 ) -> unsigned;   // 0x01 for read, 0x02 for write
  }

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
// ћогут бросить mtc::net::sockets::error с кодом ETIMEDOUT при таймауте, остальные коды - системные дл€ сокетов
//
auto  FetchFrom( mtc::net::sockets::timed_socket*, void*, size_t ) -> mtc::net::sockets::timed_socket*;
auto  Serialize( mtc::net::sockets::timed_socket*, const void*, size_t ) -> mtc::net::sockets::timed_socket*;

# endif   // __mtc_net_sockets_hpp__
