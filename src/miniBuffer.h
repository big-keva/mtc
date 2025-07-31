# if !defined( __mtc_src_miniBuffer_h__ )
# define __mtc_src_miniBuffer_h__

namespace mtc {

  template <class error>
  class MiniBuffer final: public IByteBuffer
  {
    std::atomic_long  refCount = 0;

    MiniBuffer( size_t l ): length( l ) {}

    void  operator delete( void* p ) {  delete [] (char*)p;  }

  public:   // creation
    static  auto  Create( size_t length ) -> api<MiniBuffer>
    {
      try
        {  return new ( new char[sizeof(MiniBuffer) + length - 1] ) MiniBuffer( length );  }
      catch ( const std::bad_alloc& xp )
        {  return error()( nullptr, xp );  }
      catch ( ... )
        {  return error()( nullptr, std::current_exception() );  }
    }

  // Iface
    long  Attach() override
    {
      return ++refCount;
    }
    long  Detach() override
    {
      auto  refcount = --refCount;

      if ( refcount == 0 )
        delete this;

      return refcount;
    }

  // overridables
    const char* GetPtr() const noexcept override
      {  return buffer;  }
    size_t      GetLen() const noexcept override
      {  return length;  }
    int         SetBuf( const void*, size_t ) noexcept override
      {  return error()( EINVAL, std::logic_error( "not implemented" ) );  }
    int         SetLen( size_t newlen ) noexcept override
      {  return newlen > length ? error()( EINVAL, std::logic_error( "not implemented" ) ) : (length = newlen), 0;  }

  protected:  // variables
    size_t  length;
    char    buffer[1];

  };

}

# endif   // !__mtc_src_miniBuffer_h__
