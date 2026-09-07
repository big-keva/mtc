# if !defined( __mtc_tape_hpp__ )
# define __mtc_tape_hpp__
# include "serialize.h"
# include <functional>
# include <cstdlib>

namespace mtc
{
 /*
  * tape is a class for linear serialization to fragmented memory dump
  * with possibility of appending and prepending
  */
  class tape
  {
    using FnGetPage = std::function<void*( size_t )>;
    using FnDelPage = std::function<void(void*)>;

    struct page
    {
      page*     next;
      char*     pend;
      unsigned  size;

      page( size_t cb ): next( nullptr ),
        pend( (char*)(this + 1) ), size( cb ) {}
      auto  begin() const -> char*
        {  return (char*)(1 + this);  }
      auto  limit() const -> char*
        {  return size + begin();  }
      auto  space() const -> size_t
        {  return limit() - pend;  }
      auto  usage() const -> size_t
        {  return pend - begin();  }
    };

    FnGetPage get_page;
    FnDelPage del_page;

    page*     list = nullptr;
    page**    last = &list;

    class sink;
    
    tape( const tape& ) = delete;
    tape& operator=( const tape& ) = delete;
  public:
    class head;
    class tail;

    tape();
    tape( tape&& ) noexcept;
    tape( FnGetPage, FnDelPage );
    tape& operator=( tape&& ) noexcept;
   ~tape();

    tail  append( size_t = 0 );
    head  prepend( size_t );
    tape& append( tape&& );
    tape& prepend( tape&& );

    size_t  GetBufLen() const;
    template <class O>
    O*      Serialize( O* ) const;
  };

  class tape::sink
  {
    friend class tape;

    sink() = delete;
    sink( const sink& ) = delete;
  protected:
    sink( tape* t, size_t l ): toTape( t ), ccPage( l )  {}
    sink( sink&& t ): toTape( t.toTape ), ccPage( t.ccPage ) {  t.toTape = nullptr;  }

    tape*   toTape;
    size_t  ccPage;

  };

  class tape::head: public sink
  {
    using sink::sink;
    friend class tape;
    friend head*  ::Serialize( head*, const void*, size_t );

  public:
    auto ptr() const -> head* {  return (head*)this;  }

  protected:
    size_t  store( const void*, size_t );

  };

  class tape::tail: public sink
  {
    using sink::sink;
    friend class tape;
    friend tail* ::Serialize( tail*, const void*, size_t );

  public:
    auto ptr() const -> tail* {  return (tail*)this;  }

  protected:
    size_t  store( const void*, size_t );

  };

  // tape inline implementation

  template <class O>
  O*      tape::Serialize( O* o ) const
  {
    for ( auto p = list; o != nullptr && p != nullptr; p = p->next )
      o = ::Serialize( o, p->begin(), p->usage() );
    return o;
  }

}

template <> inline
mtc::tape::head*  Serialize( mtc::tape::head* o, const void* p, size_t l )
{
  return o != nullptr && o->store( p, l ) == l ? o : nullptr;
}

template <> inline
mtc::tape::tail*  Serialize( mtc::tape::tail* o, const void* p, size_t l )
{
  return o != nullptr && o->store( p, l ) == l ? o : nullptr;
}

# endif   // !__mtc_tape_hpp__
