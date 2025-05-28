# pragma once
# if !defined( __mtc_arena_hpp__ )
# define __mtc_arena_hpp__
# include "./ptrpatch.h"
# include <atomic>

namespace mtc {

  class Arena
  {
    class block;

    class arena
    {
      friend class Arena;

      const size_t          lblock;
      std::atomic<block*>   blocks;   // list of elements
      std::atomic<
      std::atomic<block*>*> pchain;   // last in list
      std::atomic_uint32_t  nblock;   // count of blocks
      std::atomic_uint32_t  mcount;   // count of blocks
      std::atomic_uint64_t  musage;   // memory allocated

      std::atomic_long      rcount;   // arena lifetime

    public:
      arena( size_t section );
     ~arena();

    public:
      void*   allocate( size_t len, size_t align );
      auto    capacity() const -> std::uint64_t {  return mcount * lblock;  }
      auto    memcount() const -> std::uint32_t {  return mcount;  }
      auto    memusage() const -> std::uint64_t {  return musage;  }
    };

    arena*  memory = nullptr;

  public:
    template <class T>
    class allocator;

  public:
    Arena( size_t section = allocation_unit_size );
    Arena( const Arena& );
    Arena( Arena&& );
   ~Arena();
    Arena& operator = ( Arena&& );
    Arena& operator = ( const Arena& );

  public:
    template <class T>
    auto  get_allocator() -> allocator<T>;
    auto  get_default_allocator() -> allocator<void>;

  public:
    void*   allocate( size_t bytes, size_t align );
    auto    capacity() const -> std::uint64_t     {  return memory != nullptr ? memory->capacity() : 0;  }
    auto    memcount() const -> std::uint32_t     {  return memory != nullptr ? memory->memcount() : 0;  }
    auto    memusage() const -> std::uint64_t     {  return memory != nullptr ? memory->memusage() : 0;  }

  public:
    template <class Object> [[nodiscard]]
    auto  Create() -> Object*
    {
      auto  palloc = allocate( sizeof(Object), alignof(Object) );

      return new( palloc ) Object( get_allocator<char>() );
    }
    template <class Object, class ... Args> [[nodiscard]]
    auto  Create( Args&&... args ) -> Object*
    {
      auto  palloc = allocate( sizeof(Object), alignof(Object) );

      return new( palloc ) Object( std::forward<Args>( args )..., *this );
    }
    template <class Object, class ... Args> [[nodiscard]]
    auto  Create( const Args&... args ) -> Object*
    {
      auto  palloc = allocate( sizeof(Object), alignof(Object) );

      return new( palloc ) Object( args..., *this );
    }

  protected:
    enum: std::size_t{  allocation_unit_size = 4 * 0x400 * 0x144  };

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
    allocator( Arena& mp ) noexcept:  memory( mp.memory ) {}

    auto  operator = ( const allocator& other ) -> allocator&
      {  return memory = other.memory, *this;  }

  public:
    allocator( allocator&& a ) noexcept:  memory( a.memory )  {  a.memory = nullptr;  }
    allocator( const allocator& other ) noexcept: memory( other.memory )  {}
    template< class U >
    allocator( const allocator<U>& other ) noexcept:  memory( other.memory )  {}
   ~allocator() = default;

    bool  operator == ( const allocator& other ) const noexcept
      {  return memory == other.memory;  }

  public:
    T*    allocate( std::size_t n )
      {  return (T*)memory->allocate( n * (sizeof(T) + alignof(T)), alignof(T) );  }
    void  deallocate( T* p, std::size_t n )
      {  (void)p, (void)n;  }

    size_type max_size() const noexcept
      {  return memory->lblock / (sizeof(T) + alignof(T));  }

    template <class U, class... Args>
    void  construct( U* p, Args&&... args )
      {  new( p ) U( std::forward<Args>( args )... );  }
    template <class U>
    void  destroy( U* p )
      {  if ( p != nullptr )  p->~U();  }

  protected:
    arena*  memory;

  };

  template <class T, class Another>
  bool  operator != ( const Arena::allocator<T>& me, const Another& to ) noexcept {  return !(me == to);  }
  template <class T, class Another>
  bool  operator != ( const Another& me, const Arena::allocator<T>& to ) noexcept {  return !(me == to);  }
  template <class T1, class T2>
  bool  operator != ( const Arena::allocator<T1>& me, const Arena::allocator<T2>& to ) noexcept {  return !(me == to);  }

  template <class T, class Another>
  bool  operator == ( const Arena::allocator<T>&, const Another& ) noexcept {  return false;  }
  template <class T, class Another>
  bool  operator == ( const Another&, const Arena::allocator<T>& ) noexcept {  return false;  }
  template <class T1, class T2>
  bool  operator == ( const Arena::allocator<T1>&, const Arena::allocator<T2>& ) noexcept {  return false;  }

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

  // Arena::arena implementation

  inline
  Arena::arena::arena( size_t section ):
    lblock( section ),
    blocks( nullptr ),
    pchain( &blocks ),
    nblock( 0 ),
    mcount( 0 ),
    musage( 0 ),
    rcount( 1 ) {}

  inline
  Arena::arena::~arena()
  {
    for ( auto pblock = ptr::clean( blocks.load() ); pblock != nullptr; )
    {
      while ( pblock != nullptr && !blocks.compare_exchange_strong( pblock, nullptr ) )
        pblock = ptr::clean( pblock );
      if ( pblock != nullptr )
        pblock->Delete();
    }
  }

  inline
  void* Arena::arena::allocate( size_t size, size_t align )
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
        while ( ptr::clean( (*pplast).load() ) != nullptr && !pchain.compare_exchange_strong( pplast, &(*pplast).load()->next ) )
          (void)NULL;
      }
        else
      // else pblock is broken nullptr; try initialize it
      {
        assert( pblock == nullptr && (*pplast).load() == ptr::dirty( pblock ) );

        ++nblock, *pplast = block::Create( size + align, lblock );
      }
    }
  }

  // Arena implemntation

  inline
  Arena::Arena( size_t section ):
    memory( new arena( section ) )  {}

  inline
  Arena::Arena( const Arena& a ):
    memory( a.memory )
  {
    if ( memory != nullptr )
      ++memory->rcount;
  }

  inline
  Arena::Arena( Arena&& a ):
    memory( a.memory )  {  a.memory = nullptr;  }

  inline
  Arena::~Arena()
  {
    if ( memory != nullptr && --memory->rcount == 0 )
      delete memory;
  }

  inline
  Arena& Arena::operator = ( Arena&& a )
  {
    if ( memory != nullptr && --memory->rcount == 0 )
      delete memory;
    if ( (memory = a.memory) != nullptr )
      a.memory = nullptr;
    return *this;
  }

  inline
  Arena& Arena::operator = ( const Arena& a )
  {
    if ( memory != nullptr && --memory->rcount == 0 )
      delete memory;
    if ( (memory = a.memory) != nullptr )
      ++memory->rcount;
    return *this;
  }

  inline
  void* Arena::allocate( size_t size, size_t align )
  {
    return memory != nullptr ? memory->allocate( size, align ) : nullptr;
  }

  // Arena implementation

  template <class T>
  auto  Arena::get_allocator() -> allocator<T>
  {
    if ( memory == nullptr )
      throw std::logic_error( "call to allocator on uninitialized arena" );
    return allocator<T>( *this );
  }

  inline
  auto  Arena::get_default_allocator() -> allocator<void>
  {
    return get_allocator<void>();
  }

}

# endif   // __mtc_arena_hpp__
