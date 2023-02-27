# pragma once
# if !defined( __mtc_arena_hpp__ )
# define __mtc_arena_hpp__
# include <mtc/ptrpatch.h>
# include <atomic>

namespace mtc {

  class Arena
  {
    class block;

    const size_t          lblock;
    std::atomic<block*>   blocks;   // list of elements
    std::atomic<
    std::atomic<block*>*> pchain;   // last in list
    std::atomic_uint32_t  nblock;   // count of blocks
    std::atomic_uint32_t  mcount;   // count of blocks
    std::atomic_uint64_t  musage;   // memory allocated

  public:
    template <class T>
    class allocator;

  public:
    Arena( size_t section = 0x4 * 0x400 * 0x400 ):
      lblock( section ),
      blocks( nullptr ),
      pchain( &blocks ),
      nblock( 0 ),
      mcount( 0 ),
      musage( 0 ) {}
   ~Arena();

  public:
    template <class T>
    auto  get_allocator() -> allocator<T>;
    auto  get_default_allocator() -> allocator<void>;

  public:
    void*   allocate( size_t len, size_t align );
    auto    capacity() const -> std::uint64_t {  return mcount * lblock;  }
    auto    memcount() const -> std::uint32_t {  return mcount;  }
    auto    memusage() const -> std::uint64_t {  return musage;  }

  public:
    template <class Object> [[nodiscard]]
    auto  Create() -> Object*
    {
      auto  palloc = allocate( sizeof(Object), alignof(Object) );

      return new( palloc ) Object( *this );
    }
    template <class Object, class ... Args> [[nodiscard]]
    auto  Create( Args&&... args ) -> Object*
    {
      auto  palloc = allocate( sizeof(Object), alignof(Object) );

      return new( palloc ) Object( std::move( args... ), *this );
    }

  protected:
    std::size_t allocation_unit_size = 0x10000;

  };

  class Arena::block
  {
    friend class Arena;

    std::atomic<block*> next;
    std::atomic<char*>  ptop;
    const char* const   pend;     // end marker

  protected:
    block( size_t ubytes ):
      next( nullptr ),
      ptop( (char*)(1 + this) ),
      pend( ubytes + (char*)this )  {}
   ~block()
      {
        if ( next != nullptr )
          next.load()->Delete();
      }
    void  operator delete( void* ) = delete;

  public:
    static  auto  Create( size_t at_least, size_t section ) -> block*;
            void  Delete();

  public:
    void* allocate( size_t size, size_t align );

  };

  template <class T>
  class Arena::allocator
  {
    template <class U>
    friend class allocator;

  public:
    using value_type = T;

    using pointer = T*;
    using const_pointer = const T*;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;

    allocator() noexcept = delete;

  public:
    allocator( Arena& mp ) noexcept:
      memoryPool( mp ) {}
    allocator( const allocator& other ) noexcept:
      memoryPool( other.memoryPool )  {}
    template< class U >
    allocator( const allocator<U>& other ) noexcept:
      memoryPool( other.memoryPool )  {}
   ~allocator() = default;

  public:
    T*    allocate( std::size_t n )
      {  return (T*)memoryPool.allocate( n * (sizeof(T) + alignof(T)), alignof(T) );  }
    void  deallocate( T* p, std::size_t n )
      {  (void)p, (void)n;  }

    size_type max_size() const noexcept
      {  return memoryPool.allocation_unit_size / (sizeof(T) + alignof(T));  }

    template <class U, class... Args>
    void  construct( U* p, Args&&... args )
      {  new( p ) U( args... );  }
    template <class U>
    void  destroy( U* p )
      {  if ( p != nullptr )  p->~U();  }

  protected:
    Arena& memoryPool;

  };

  template <class T1, class Another>
  bool  operator != ( const Arena::allocator<T1>&, const Another& ) noexcept
    {  return true;  }

  template <class T1, class Another>
  bool  operator == ( const Arena::allocator<T1>&, const Another& ) noexcept
    {  return false;  }

  template <class T1, class T2>
  bool  operator != ( const Arena::allocator<T1>& lhs, const Arena::allocator<T2>& rhs ) noexcept
    {  return !(lhs == rhs);  }

  template <class T1, class T2>
  bool  operator == ( const Arena::allocator<T1>& lhs, const Arena::allocator<T2>& rhs ) noexcept;

  // Arena implemntation

  inline
  Arena::~Arena()
  {
    if ( blocks != nullptr )
      blocks.load()->Delete();
  }

  inline
  void* Arena::allocate( size_t size, size_t align )
  {
    auto  pplast = ptr::clean( pchain.load() );   // &atomic<block*>, never nullptr

    for ( ; ; )
    {
      auto  pblock = ptr::clean( (*pplast).load() );  // pointer to last block, may be nullptr
      void* newptr;

      // set pointer to be eigher !nullptr, or dirty nullptr
      while ( pblock == nullptr && !(*pplast).compare_exchange_strong( pblock, ptr::dirty( pblock ) ) )
        pblock = ptr::clean( pblock );

      // if !nullptr, try allocate subblock
      if ( pblock != nullptr )
      {
        // if allocated, finish work
        if ( (newptr = pblock->allocate( size, align )) != nullptr )
          return musage += ((size + align - 1) & ~(align - 1)), ++mcount, newptr;

        // else block can not provide subblock, perhaps is filled; switch chain to
        // it's next subblock
        while ( (*pplast).load() != nullptr && !pchain.compare_exchange_strong( pplast, &(*pplast).load()->next ) )
          (void)NULL;
        continue;
      }

      // else pblock is broken nullptr; try initialize it
      assert( pblock == nullptr && (*pplast).load() == ptr::dirty( pblock ) );

      ++nblock, *pplast = block::Create( size + align, lblock );
    }
  }

  // Arena::block implementation

  inline
  auto  Arena::block::Create( size_t at_least, size_t section ) -> block*
  {
    size_t  ualloc = (std::max( at_least, section ) + 0x0fff) & ~0x0fff;
    auto    palloc = new char[ualloc];

    return new( palloc ) block( ualloc );
  }

  inline
  void  Arena::block::Delete()
  {
    this->~block();
    delete [] (char*)this;
  }

  inline
  void* Arena::block::allocate( size_t size, size_t align )
  {
    assert( align != 0 );

    for ( ; ; )
    {
      auto  pstart = ptop.load();
      auto  palign = ptr::align( pstart, align );
      auto  pfinal = palign + size;

      if ( pfinal >= pend )
        return nullptr;
      if ( ptop.compare_exchange_strong( pstart, pfinal ) )
        return palign;
    }
  }

  // Arena implementation

  template <class T>
  auto  Arena::get_allocator() -> allocator<T>  {  return allocator<T>( *this );  }

  inline
  auto  Arena::get_default_allocator() -> allocator<void>  {  return get_allocator<void>();  }

}

# endif   // __mtc_arena_hpp__
