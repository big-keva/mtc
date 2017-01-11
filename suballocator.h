# if !defined( __mtc_suballocator_h__ )
# define __mtc_suballocator_h__
# include "autoptr.h"
# include "array.h"

namespace mtc
{

  template <class M = def_alloc>
  class PagedAllocator
  {
    const unsigned  page_size_limit;

    class MemSection
    {
      char*   allocTop;
      char*   memLimit;

    public:     // creation
      static MemSection* Create( const unsigned minlen, const unsigned alimit )
        {
          MemSection* palloc;
          unsigned    nalloc = minlen < alimit - sizeof(*palloc) ? alimit - sizeof(*palloc) : minlen;

          if ( (palloc = (MemSection*)M().alloc( nalloc + sizeof(*palloc) )) != nullptr )
            new( palloc ) MemSection( nalloc );
          return palloc;
        }

    public:     // API
      void*   alloc( size_t nalloc )
        {
          char* palloc;

          if ( (palloc = allocTop) + nalloc > memLimit )
            return nullptr;
          allocTop += nalloc;
            return palloc;
        }

    private:    // construction
      MemSection( unsigned l ): allocTop( (char*)(this + 1) ), memLimit( allocTop + l )  {}
      MemSection( const MemSection& );
      MemSection& operator = ( const MemSection& );

    };

  public:     // construction
    PagedAllocator( unsigned memlimit = 16 * 1024 * 1024 ):
      page_size_limit( (memlimit + 0x100000 - 1) & (0x100000 - 1) ) {  }

  public:     // malloc
    void*   GetMem( size_t nalloc )
      {
        void* palloc;

        if ( apages.size() == 0 || (palloc = apages[apages.size() - 1]->alloc( nalloc )) == nullptr )
        {
          _auto_<MemSection, M> memsec;

          if ( (memsec = MemSection::Create( nalloc, page_size_limit )) == nullptr )
            return nullptr;
          palloc = apages.Append( memsec ) == 0 ? apages[apages.size() - 1]->alloc( nalloc ) : nullptr;
        }
        return palloc;
      }
    void    DelAll()
      {
        apages.SetLen( 0 );
      }

  protected:  // allocations
    array<_auto_<MemSection, M>, M> apages;

  };

}

# endif  // __mtc_suballocator_h__
