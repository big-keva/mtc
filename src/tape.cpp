# include "../tape.hpp"
# include <stdexcept>

namespace mtc
{
  // tape implementation

  tape::tape():
    get_page( malloc ),
    del_page( free )  {}

  tape::tape( tape&& t ) noexcept:
    get_page( std::move( t.get_page ) ),
    del_page( std::move( t.del_page ) ),
    list( t.list ),
    last( t.last == &t.list ? &list : t.last )
  {
    t.last = &(t.list = nullptr);
  }

  tape::tape( FnGetPage get, FnDelPage del ):
    get_page( get ),
    del_page( del )  {}

  tape& tape::operator=( tape&& t ) noexcept
  {
    if ( this != &t )
    {
    // delete self data
      for ( auto next = list; next != nullptr; )
      {
        auto  free = next;
          next = next->next;
        free->~page();
          del_page( free );
      }

    // assign new tape
      get_page = std::move( t.get_page );
      del_page = std::move( t.del_page );

      list = t.list;
      last = t.last == &t.list ? &list : t.last;
      t.last = &(t.list = nullptr);
      t.get_page = malloc;
      t.del_page = free;
    }
    return *this;
  }

  tape::~tape()
  {
    for ( auto next = list; next != nullptr; )
    {
      auto  free = next;
        next = next->next;
      free->~page();
        del_page( free );
    }
  }

  tape::tail  tape::append( size_t pageSize )
  {
    return tail( this, pageSize != 0 ? pageSize : 0x8000 );
  }

  tape::head  tape::prepend( size_t pageSize )
  {
    auto  pnew = new( get_page( pageSize + sizeof(page) ) ) page( pageSize );

    if ( last == &list )
      last = &(pnew->next = list);
    pnew->next = list;
      list = pnew;
    return head( this, 0 );
  }

  tape& tape::append( tape&& t )
  {
    if ( this != &t )
    {
      if ( *last == nullptr )
      {
        if ( t.last == &t.list )  last = &(list = t.list);
          else last = t.last;
      }
        else
      {
      // copy contents to current element
        while ( t.list != nullptr && t.list->usage() <= (*last)->space() )
        {
          auto  delptr = t.list;

          memcpy( (*last)->pend, delptr->begin(), delptr->usage() );
            (*last)->pend += delptr->usage();
          t.list = delptr->next;
            delptr->next = nullptr;
          t.del_page( delptr );
        }

        if ( t.list != nullptr )
        {
          for ( last = &((*last)->next = t.list); *last != nullptr && (*last)->next != nullptr; )
            last = &(*last)->next;
        }
      }
    // clear added tape
      t.last = &(t.list = nullptr);
    }
    return *this;
  }

  tape& tape::prepend( tape&& t )
  {
    if ( this != &t )
    {
      while ( *t.last != nullptr )
        t.last = &((*t.last)->next);

      *t.last = list;
       t.last = *last == list ? &t.list : last;

      last = &(list = nullptr);

      std::swap( last, t.last );
      std::swap( list, t.list );
    }
    return *this;
  }

  size_t  tape::GetBufLen() const
  {
    size_t  cc = 0;

    for ( auto p = list; p != nullptr; p = p->next )
      cc += p->usage();

    return cc;
  }

  // tape::head implementation

  size_t  tape::head::store( const void* p, size_t l )
  {
    if ( toTape == nullptr || toTape->list == nullptr )
      throw std::logic_error( "attempt to write to uninitialized tape stub" );

    if ( toTape->list->pend + l > toTape->list->limit() )
      throw std::logic_error( "tape head stub overflow" );

    memcpy( toTape->list->pend, p, l );
      toTape->list->pend += l;

    return l;
  }

  // tape::tail implementation

  size_t  tape::tail::store( const void* p, size_t l )
  {
    auto  srcbeg = (char*)p;
    auto  srcend = (char*)p + l;

    if ( toTape == nullptr )
      throw std::logic_error( "attempt to write to uninitialized tape stub" );

    do
    {
      while ( *toTape->last != nullptr && (*toTape->last)->space() == 0 )
        toTape->last = &(*toTape->last)->next;

      if ( *toTape->last == nullptr )
        *toTape->last = new( toTape->get_page( ccPage + sizeof(page) ) ) page( ccPage );

      {
        auto  cbcopy = std::min( size_t(srcend - srcbeg), (*toTape->last)->space() );
        auto  outptr = (*toTape->last)->pend;
        auto  endptr = srcbeg + cbcopy;

        while ( srcbeg < endptr )
          *outptr++ = *srcbeg++;
        (*toTape->last)->pend = outptr;
      }
    } while ( srcbeg != srcend );

    return l;
  }

}
