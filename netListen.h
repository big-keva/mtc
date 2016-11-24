# if !defined( __mtc_netListen_h__ )
# define __mtc_netListen_h__
# include "netStream.h"
# include <atomic>

namespace mtc
{

  class NetListen
  {
    enum {  InvalidSocket = -1  };

    class ListenSocket
    {
      friend class NetListen;

    public:     // construction
      ListenSocket( int socket = InvalidSocket ): socketid( (void*)socket ), refcount( 0 ) {}
      ListenSocket( const ListenSocket& ) = delete;
      ListenSocket& operator = ( const ListenSocket& ) = delete;
     ~ListenSocket()
        {
          if ( socketid != (void*)InvalidSocket )
            Detach( socketid );
        }

    protected:  // helpers
      static  int   Accept( void**, void*, unsigned );
      static  int   Attach( void**, const char*, unsigned, unsigned );
      static  void  Detach( void* );

    protected:  // variables
      void*             socketid;
      std::atomic_long  refcount;

    };

    const char*   szListenHost;
    unsigned      dwListenPort;
    word32_t      msWaitListen;
    word32_t      msWaitAccept;
    ListenSocket* listenSocket;

  public:     // construction
    NetListen( const char* szhost = nullptr, unsigned dwport = 0 ):
        szListenHost( szhost ),
        dwListenPort( dwport ),
        msWaitListen( -1 ),
        msWaitAccept( -1 ),
        listenSocket( nullptr )
      {
      }
    NetListen( const NetListen& nl ):
        szListenHost( nl.szListenHost ),
        dwListenPort( nl.dwListenPort ),
        msWaitListen( nl.msWaitListen ),
        msWaitAccept( nl.msWaitAccept ),
        listenSocket( nl.listenSocket )
      {
        if ( listenSocket != nullptr )
          ++listenSocket->refcount;
      }
   ~NetListen()
      {
        if ( listenSocket != nullptr && --listenSocket->refcount == 0 )
          delete listenSocket;
      }
    NetListen& operator = ( const NetListen& nl )
      {
        if ( listenSocket != nullptr && --listenSocket->refcount == 0 )
          delete listenSocket;
        szListenHost = nl.szListenHost;
        dwListenPort = nl.dwListenPort;
        msWaitListen = nl.msWaitListen;
        msWaitAccept = nl.msWaitAccept;
        if ( (listenSocket = nl.listenSocket) != nullptr )
          ++listenSocket->refcount;
        return *this;
      }

  public:     // customiz
    NetListen& SetListenHost( const char*  szhost ) {  szListenHost = szhost;  return *this;  }
    NetListen& SetListenPort( unsigned     dwport ) {  dwListenPort = dwport;  return *this;  }
    NetListen& SetAttachTimeout( word32_t tout )    {  msWaitListen = tout;    return *this;  }
    NetListen& SetAcceptTimeout( word32_t tout )    {  msWaitAccept = tout;    return *this;  }

  public:
    int   Attach( const char* host = nullptr, unsigned port = 0 )
      {
        const char* connectHost = host != nullptr ? host : szListenHost;
        unsigned    connectPort = port != 0       ? port : dwListenPort;
        int         socketError;

      // close and allocate new socket handler
        Detach();

        if ( (listenSocket = allocate<ListenSocket>()) != nullptr ) ++listenSocket->refcount;
          else return ENOMEM;

      // try open socket
        if ( (socketError = ListenSocket::Attach( &listenSocket->socketid, connectHost, connectPort, msWaitListen )) != 0 )
          Detach();
          
        return socketError;
      }
    int   Accept( void** ppvout )
      {
        return listenSocket != nullptr ? ListenSocket::Accept( ppvout, listenSocket->socketid, msWaitAccept ) : EINVAL;
      }
    void  Detach()
      {
      // close existing socket
        if ( listenSocket != nullptr && --listenSocket->refcount == 0 )
          delete listenSocket;
        listenSocket = nullptr;
      }

  };

}

# endif  // __mtc_netListen_h__
