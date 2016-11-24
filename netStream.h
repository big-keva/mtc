# if !defined( __mtc_netStream_h__ )
# define __mtc_netStream_h__
# include "iStream.h"

namespace mtc
{

  struct INetStream: public IByteStream
  {
    virtual int   SetGetTimeout( word32_t ) = 0;
    virtual int   SetPutTimeout( word32_t ) = 0;
  };

  class NetStream
  {
    const char* tryConnectHost;
    unsigned    tryConnectPort;
    word32_t    connectTimeout;
    word32_t    receiveTimeout;
    word32_t    sendingTimeout;

  public:     // construction
    NetStream( const char* szhost = nullptr, unsigned dwport = 0 ): tryConnectHost( szhost ),
                                                                    tryConnectPort( dwport ),
                                                                    connectTimeout( 100000 ),
                                                                    receiveTimeout( -1 ),
                                                                    sendingTimeout( -1 ) {}
    NetStream( const NetStream& nc ): tryConnectHost( nc.tryConnectHost ),
                                      tryConnectPort( nc.tryConnectPort ),
                                      connectTimeout( nc.connectTimeout ),
                                      receiveTimeout( nc.receiveTimeout ),
                                      sendingTimeout( nc.sendingTimeout ) {}
    NetStream& operator = ( const NetStream& nc )
      {
        tryConnectHost = tryConnectHost;
        tryConnectPort = tryConnectPort;
        connectTimeout = connectTimeout;
        receiveTimeout = receiveTimeout;
        sendingTimeout = sendingTimeout;
        return *this;
      }

  public:     // settings parameters
    NetStream& SetHost( const char*  szhost )     {  tryConnectHost = szhost;  return *this;  }
    NetStream& SetPort( unsigned     dwport )     {  tryConnectPort = dwport;  return *this;  }
    NetStream& SetGetTimeout( word32_t tout )     {  receiveTimeout = tout;    return *this;  }
    NetStream& SetPutTimeout( word32_t tout )     {  sendingTimeout = tout;    return *this;  }
    NetStream& SetConnectTimeout( word32_t tout ) {  connectTimeout = tout;    return *this;  }

  public:     // simple connect
    static  int   Attach( void** ppvout, const char* szhost, unsigned dwport, unsigned msconn = 0 );

  public:     // connect
    int   Attach( void** ppvout )
      {
        API<INetStream> netstm;
        int             nerror;

        if ( (nerror = Attach( netstm, tryConnectHost, tryConnectPort, connectTimeout )) != 0 )
          return nerror;
        if ( receiveTimeout != (word32_t)-1 )
          netstm->SetGetTimeout( receiveTimeout );
        if ( sendingTimeout != (word32_t)-1 )
          netstm->SetPutTimeout( sendingTimeout );
        ((INetStream*)(*ppvout = (INetStream*)netstm))->Attach();
          return 0;
      }
  };

}

# endif  // __mtc_netStream_h__
