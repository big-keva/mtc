# include "byteBuffer.h"
# include "autoptr.h"
# include "array.h"

namespace mtc
{
  class ByteBuffer: protected array<char>
  {
    implement_lifetime_control

  public:
    virtual const char* GetPtr() noexcept override
      {
        return begin();
      }
    virtual unsigned    GetLen() noexcept override {  return size();   }
    virtual int         SetBuf( const void* p, unsigned l )
      {
        if ( SetLen( l ) != 0 )
          return ENOMEM;
        memcpy( begin(), p, l );
          return 0;
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

  int   CreateByteBuffer( IByteBuffer** ppi, const void* memptr, unsigned length );
    {
      _auto_<ByteBuffer>  palloc;

      if ( ppi == nullptr )
        return EINVAL;
      if ( (palloc = allocate<ByteBuffer>()) == nullptr )
        return ENOMEM;
      if ( palloc->Append( (const char*)memptr, length ) != 0 )
        return ENOMEM;
      (*ppi = palloc.detach())->Attach();
        return 0;
    }
}
