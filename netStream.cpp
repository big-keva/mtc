# include "netStream.h"
# include "netListen.h"
# include "autoptr.h"
# include "stdlog.h"
# include <sys/types.h>
# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include <errno.h>

# if defined( WIN32 )
#   include <WinSock2.h>
# else
#   include <netinet/tcp.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#   include <unistd.h>
#   include <netdb.h>
#   include <fcntl.h>
# endif   // WIN32

namespace mtc
{

# if defined( WIN32 )
# pragma comment( lib, "ws2_32.lib" )

  struct Sockets: private WSADATA
  {
    typedef ::SOCKET  SOCKET;

    enum
    {
      E_INTERRUPT   = WSAEINTR,
      E_INPROGRESS  = WSAEWOULDBLOCK,
      InvalidSocket = INVALID_SOCKET
    };

    Sockets()
      {
        WSAStartup( 0x0202, this );
      }
   ~Sockets()
      {
        WSACleanup();
      }
    static  int   GetError()
      {
        return WSAGetLastError();
      }
    static  int   SetBlock( SOCKET sockid, bool on_off )
      {
        u_long  nonblk = on_off ? 0 : 1;

        return ioctlsocket( sockid, FIONBIO, &nonblk );
      }
    static  int   NonDelay( SOCKET sockid )
      {
        int   flag = 1;

        return setsockopt( sockid, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int) ) < 0 ? GetError() : 0;
      }
    static  int   CloseSocket( SOCKET sockid )
      {
        return closesocket( sockid );
      }
    static  void* CastToVoid( SOCKET sockid )
      {
        return (void*)(word64_t)sockid;
      }
    static  SOCKET CastToSocket( void* p )
     {
        return *(SOCKET*)&p;
     }
  };

# else

  struct Sockets
  {
    typedef int   SOCKET;

    enum
    {
      E_INTERRUPT   = EINTR,
      E_INPROGRESS  = EINPROGRESS,
      InvalidSocket = -1
    };

    static  int   GetError()
      {
        return errno;
      }
    static  int   SetBlock( SOCKET sockid, bool on_off )
      {
        return fcntl( sockid, F_SETFL, on_off ? (fcntl( sockid, F_GETFL ) |  O_NONBLOCK) :
                                                (fcntl( sockid, F_GETFL ) & ~O_NONBLOCK) ) == -1 ? EINVAL : 0;
      }
    static  int   NonDelay( SOCKET sockid )
      {
        int   flag = 1;

        return setsockopt( sockid, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int) ) < 0 ? errno : 0;
      }
    static  int   CloseSocket( SOCKET sockid )
      {
        return close( sockid );
      }
    static  void* CastToVoid( SOCKET sockid )
      {
        return (void*)(word64_t)sockid;
      }
    static  SOCKET CastToSocket( void* p )
     {
        return *(SOCKET*)&p;
     }

  };

# endif   //
  Sockets socketsAPI;

  class CNetStream: public INetStream
  {
    implement_lifetime_control

  public:     // construction
	  CNetStream( Sockets::SOCKET sockid = Sockets::InvalidSocket );
	  CNetStream( const CNetStream& ) = delete;
    CNetStream& operator = ( const CNetStream& ) = delete;
   ~CNetStream();

  public:     // overridables from IByteStream
    virtual word32_t  Get(       void*, word32_t ) override;
    virtual word32_t  Put( const void*, word32_t ) override;

  public:     // from NetStream
    virtual int   SetGetTimeout( word32_t ) override;
    virtual int   SetPutTimeout( word32_t ) override;

  public:     // client
	  int   ConnectSocket( const char* szhost, unsigned short dwport, unsigned msconn = 0 );

  protected:  // helpers
    bool  HasDataToRead() const noexcept;
    int   SetGetTimeout() const noexcept;
    int   SetPutTimeout() const noexcept;

  protected:  // variables
    Sockets::SOCKET sockid;
    word32_t        tmoGet;
    word32_t        tmoPut;

  };

  // CNetStream implementation

	CNetStream::CNetStream( Sockets::SOCKET idsock ): sockid( idsock ), tmoGet( -1 ), tmoPut( -1 )
    {
    }

  CNetStream::~CNetStream()
    {
      if ( sockid != Sockets::InvalidSocket )
        Sockets::CloseSocket( sockid );
    }

  word32_t  CNetStream::Get( void*  o, word32_t l )
  {
    char* outptr = (char*)o;
    char* outend = outptr + l;

    // check data available
    if ( HasDataToRead() )
      while ( outptr < outend )
      {
        int   cbread;
        int   nerror;

      // try read data
        if ( (cbread = recv( sockid, outptr, outend - outptr, 0 )) < 0 )
        { 
          if ( (nerror = Sockets::GetError()) == Sockets::E_INPROGRESS )
            continue;
          log_error( nerror, "recv( ... %u, 0 ) error (%d)!", outend - outptr, nerror );
            return (word32_t)-1;
        }

      // check for zero byte count
        if ( cbread != 0 )  outptr += cbread;
          else break;
      }

    return outptr - (char*)o;
  }

  int   CNetStream::SetGetTimeout( word32_t msTimeout )
  {
    tmoGet = msTimeout;
    return sockid != Sockets::InvalidSocket ? SetGetTimeout() : 0;
  }

  int   CNetStream::SetPutTimeout( word32_t msTimeout )
  {
    tmoPut = msTimeout;
    return sockid != Sockets::InvalidSocket ? SetPutTimeout() : 0;
  }

  int   CNetStream::ConnectSocket( const char* szhost, unsigned short dwport, unsigned msconn )
  {
    sockaddr_in   scaddr;
    int           nerror;
    auto          eclose = [this]( int nerror )
      {
        if ( sockid != Sockets::InvalidSocket )
          Sockets::CloseSocket( sockid );
        sockid = Sockets::InvalidSocket;
          return nerror;
      };

  // check if socket is not created; create the socket
    if ( sockid != Sockets::InvalidSocket )
      Sockets::CloseSocket( sockid );

    if ( (sockid = socket( AF_INET, SOCK_STREAM, 0 )) == Sockets::InvalidSocket )
      return Sockets::GetError();

  // fill connection address
    memset( &scaddr, 0, sizeof(scaddr) );
      scaddr.sin_family      = AF_INET;  
      scaddr.sin_addr.s_addr = INADDR_ANY;  
      scaddr.sin_port        = htons( dwport );

    if ( (scaddr.sin_addr.s_addr = inet_addr( szhost )) == INADDR_NONE )
    {
      struct hostent* lphost;

      if ( (lphost = gethostbyname( szhost )) == nullptr )
        return eclose( win32_decl( WSAGetLastError() ) posix_decl( errno ) );

      scaddr.sin_addr.s_addr = *(word32_t*)lphost->h_addr_list[0];
    }

  // set socket to be non-blocking
    Sockets::SetBlock( sockid, msconn != 0 && msconn != (unsigned)-1 );

  // try connect socket
    if ( (nerror = connect( sockid, (const sockaddr*)&scaddr, sizeof(scaddr) )) == 0 )
      return Sockets::NonDelay( sockid );

  /*  на платформе WIN32 типичным возвратом функции является WSAEWOULDBLOCK,  */
  /*  что означает, что коннект пошёл устанавливаться; следует подождать,     */
  /*  пока для select() с указанным таймаутом не будет выставлен writefds     */
  /*  как признак успешного завершения, или exceptfds как ошибка              */
    if ( nerror < 0 && Sockets::GetError() == Sockets::E_INPROGRESS )
    {
  # if defined( _MSC_VER )
      fd_set          wr_fds;
      fd_set          ex_fds;
      struct timeval  tbreak;

      FD_ZERO( &wr_fds );   FD_SET( sockid, &wr_fds );
      FD_ZERO( &ex_fds );   FD_SET( sockid, &ex_fds );

      tbreak.tv_sec   =  msconn / 1000;
      tbreak.tv_usec  = (msconn % 1000) * 1000;

    /*  проверить статус сокета                                             */
      do  nerror = select( sockid + 1, nullptr, &wr_fds, &ex_fds, &tbreak );
        while ( nerror < 0 && Sockets::GetError() == Sockets::E_INTERRUPT );
  # else
      fd_set          rd_fds;
      fd_set          wr_fds;
      struct timeval  tbreak;

      FD_ZERO( &rd_fds );   FD_SET( sockid, &rd_fds );
      FD_ZERO( &wr_fds );   FD_SET( sockid, &wr_fds );

      tbreak.tv_sec   =  msconn / 1000;
      tbreak.tv_usec  = (msconn % 1000) * 1000;

    /*  проверить статус сокета                                             */
      do  nerror = select( sockid + 1, &rd_fds, &wr_fds, NULL, &tbreak );
        while ( nerror < 0 && Sockets::GetError() == Sockets::E_INTERRUPT );
  # endif  // *NIX

    /*  если возникла ошибка, отработать её                                 */
      if ( nerror <= 0 )
        return eclose( nerror == 0 ? EAGAIN : ENOENT );

    /*  проверить результат select() - успешно открыт или на фиг            */
      return FD_ISSET( sockid, &wr_fds ) ? Sockets::NonDelay( sockid ) : eclose( EFAULT );
    }
      else
    return eclose( ENOENT );
  }

  word32_t  CNetStream::Put( const void* p, word32_t l )
  {
    word32_t  ncbput;
    int       nerror;
    int       cbpart;

    if ( (nerror = SetGetTimeout()) != 0 )
      return nerror;

    for ( ncbput = 0; ncbput < l; ncbput += cbpart )
      if ( (cbpart = send( sockid, ncbput + (const char*)p, l - ncbput, win32_decl( 0 ) posix_decl( MSG_NOSIGNAL ) )) < 0 )
        break;
	  return ncbput;
  }

  bool  CNetStream::HasDataToRead() const noexcept
  {
    fd_set          fd;
    struct timeval  tv;

  // check if there is data in stream
    FD_ZERO( &fd );
    FD_SET( sockid, &fd );

    tv.tv_usec = (tmoGet - (tv.tv_sec = tmoGet / 1000) * 1000) * 1000;

  // check the data in buffer
    return select( sockid + 1, &fd, nullptr, nullptr, tmoGet != (word32_t)-1 ? &tv : nullptr ) > 0;
  }

  int   CNetStream::SetGetTimeout() const noexcept
  {
    if ( tmoGet != (word32_t)-1 )
    {
	    struct timeval  tv;

      tv.tv_usec = (tmoGet - (tv.tv_sec = tmoGet / 1000) * 1000) * 1000;

      return setsockopt( sockid, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv) ) != 0 ?
        Sockets::GetError() : 0;
    }
      else
    return Sockets::SetBlock( sockid, true );
  }

  int   CNetStream::SetPutTimeout() const noexcept
  {
    if ( tmoPut != (word32_t)-1 )
    {
	    struct timeval  tv;

      tv.tv_usec = (tmoPut - (tv.tv_sec = tmoPut / 1000) * 1000) * 1000;

      return setsockopt( sockid, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv) ) != 0 ?
        Sockets::GetError() : 0;
    }
      else
    return Sockets::SetBlock( sockid, true );
  }

  /*
    real connect wrapper
  */
  int   NetStream::Attach( void** ppvout, const char* szhost, unsigned dwport, unsigned msconn )
  {
    _auto_<CNetStream>  palloc;
    int                 nerror;

    if ( (palloc = allocate<CNetStream>()) == nullptr )
      return ENOMEM;
    if ( (nerror = palloc->ConnectSocket( szhost, dwport, msconn )) != 0 )
      return nerror;
    ((Iface*)(*ppvout = (INetStream*)palloc.detach()))->Attach();
      return 0;
  }

  /*
    listen conn functions
  */

  int   NetListen::ListenSocket::Accept( void** ppvout, void* listen, unsigned mswait )
  {
    fd_set          solist;
    struct timeval  tlimit;
    Sockets::SOCKET getone;
    int             nerror;

    if ( listen == nullptr )
      return EINVAL;

  // begin listen socket - check if there are incoming connection
    FD_ZERO( &solist );
    FD_SET( Sockets::CastToSocket( listen ), &solist );

    tlimit.tv_usec = (mswait - (tlimit.tv_sec = mswait / 1000) * 1000) * 1000;

  // check if the query present in queue
    if ( select( 1 + Sockets::CastToSocket( listen ), &solist, nullptr, nullptr, mswait != (unsigned)-1 ? &tlimit : nullptr ) <= 0 )
      return EAGAIN;

    // get incoming socket
    if ( (getone = accept( Sockets::CastToSocket( listen ), nullptr, nullptr )) != Sockets::InvalidSocket )
    {
      _auto_<CNetStream>  palloc;

      if ( (palloc = allocate<CNetStream>( getone )) == nullptr )
      {
        Sockets::CloseSocket( getone );
        return ENOMEM;
      }
      ((INetStream*)(*ppvout = (void*)palloc.detach()))->Attach();
        return 0;
    }
    return (nerror = Sockets::GetError()) == Sockets::E_INPROGRESS ? EAGAIN : nerror;
  }

  int   NetListen::ListenSocket::Attach( void** ppvout, const char* szhost, unsigned dwport, unsigned mswait )
  {
    sockaddr_in     soaddr;
    struct hostent* hoaddr;
    int             socopt = 1;
    Sockets::SOCKET sockid;
    auto            eclose = [sockid]( int nerror ) {  Sockets::CloseSocket( sockid ); return nerror; };

  // resolve host address
    if ( (hoaddr = gethostbyname( szhost )) == NULL )
      return Sockets::GetError();

  // initialize the address
    soaddr.sin_family       = AF_INET;
    soaddr.sin_addr.s_addr  = htonl( INADDR_ANY );
    soaddr.sin_port         = htons( dwport );

  // create the socket
    if ( (sockid = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP )) == Sockets::InvalidSocket )
      return Sockets::GetError();

  // set nonblocking mode
    Sockets::SetBlock( sockid, false );

    setsockopt( sockid, SOL_SOCKET, SO_REUSEADDR, (const char*)&socopt, sizeof(socopt) );

  // bind to port
    if ( bind( sockid, (sockaddr*)&soaddr, sizeof(sockaddr_in) ) != 0 )
      return eclose( Sockets::GetError() );

  // set nonblocking mode
    Sockets::SetBlock( sockid, false );

  // set che socket to the listen state
    if ( listen( sockid, mswait ) != 0 )
      return eclose( Sockets::GetError() );

  // set nonblocking mode
    Sockets::SetBlock( sockid, false );

    *ppvout = Sockets::CastToVoid( sockid );
      return 0;
  }

  void  NetListen::ListenSocket::Detach( void* listen )
  {
    if ( Sockets::CastToSocket( listen ) != Sockets::InvalidSocket )
      Sockets::CloseSocket( Sockets::CastToSocket( listen ) );
  }

}
