/*

The MIT License (MIT)

Copyright (c) 2000-2016 Андрей Коваленко aka Keva
  keva@meta.ua
  keva@rambler.ru
  skype: big_keva
  phone: +7(495)648-4058, +7(916)015-5592

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

=============================================================================

Данная лицензия разрешает лицам, получившим копию данного программного обеспечения
и сопутствующей документации (в дальнейшем именуемыми «Программное Обеспечение»),
безвозмездно использовать Программное Обеспечение без ограничений, включая неограниченное
право на использование, копирование, изменение, слияние, публикацию, распространение,
сублицензирование и/или продажу копий Программного Обеспечения, а также лицам, которым
предоставляется данное Программное Обеспечение, при соблюдении следующих условий:

Указанное выше уведомление об авторском праве и данные условия должны быть включены во
все копии или значимые части данного Программного Обеспечения.

ДАННОЕ ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ПРЕДОСТАВЛЯЕТСЯ «КАК ЕСТЬ», БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ,
ЯВНО ВЫРАЖЕННЫХ ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ ГАРАНТИИ ТОВАРНОЙ ПРИГОДНОСТИ,
СООТВЕТСТВИЯ ПО ЕГО КОНКРЕТНОМУ НАЗНАЧЕНИЮ И ОТСУТСТВИЯ НАРУШЕНИЙ, НО НЕ ОГРАНИЧИВАЯСЬ
ИМИ.

НИ В КАКОМ СЛУЧАЕ АВТОРЫ ИЛИ ПРАВООБЛАДАТЕЛИ НЕ НЕСУТ ОТВЕТСТВЕННОСТИ ПО КАКИМ-ЛИБО ИСКАМ,
ЗА УЩЕРБ ИЛИ ПО ИНЫМ ТРЕБОВАНИЯМ, В ТОМ ЧИСЛЕ, ПРИ ДЕЙСТВИИ КОНТРАКТА, ДЕЛИКТЕ ИЛИ ИНОЙ
СИТУАЦИИ, ВОЗНИКШИМ ИЗ-ЗА ИСПОЛЬЗОВАНИЯ ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ ИЛИ ИНЫХ ДЕЙСТВИЙ
С ПРОГРАММНЫМ ОБЕСПЕЧЕНИЕМ.

*/
# pragma once
# if !defined( __mtc_mutex__ )
# define __mtc_mutex__
# include <condition_variable>
# include <climits>
# include <thread>
# include <atomic>
# include <mutex>

namespace mtc
{

  class recursive_shared_mutex
  {
    class wait_event: protected std::condition_variable
    {
      std::mutex  _m;

    public:
      void  wait()
      {
        std::unique_lock<std::mutex>  wxlock( _m );

        return condition_variable::wait( wxlock );
      }
      void  notify_all()  {  return condition_variable::notify_all();      }
      void  notify_one()  {  return condition_variable::notify_one();      }
    };

    recursive_shared_mutex( const recursive_shared_mutex& ) = delete;
    recursive_shared_mutex( recursive_shared_mutex&& ) = delete;
    recursive_shared_mutex& operator = ( const recursive_shared_mutex& ) = delete;
    recursive_shared_mutex& operator = ( recursive_shared_mutex&& ) = delete;

  public:
    recursive_shared_mutex(): mtxState( 0 ) {}

  public:
    void  lock();
    void  unlock();
    void  lock_shared();
    void  unlock_shared();
//    void  shared_to_write();

  private:
    // bit  0 - 20: readers
    // bit 21 - 41: waiting readers
    // bit 42 - 62: waiting writers
    // bit      61: writer-flag
    std::atomic<uint64_t>   mtxState;
    std::thread::id         writerId;
    std::uint32_t           nRecurse;

    wait_event              cvReader;
    wait_event              cvWriter;

    static const unsigned WAITING_READERS_BASE  = 21;
    static const unsigned WAITING_WRITERS_BASE  = 42;
    static const unsigned WRITER_FLAG_BASE      = 63;

    static const uint64_t MASK21                = 0x1FFFFFu;
    static const uint64_t READERS_MASK          = MASK21;

    static const uint64_t WAITING_READERS_MASK  = MASK21 << WAITING_READERS_BASE;
    static const uint64_t WAITING_WRITERS_MASK  = MASK21 << WAITING_WRITERS_BASE;

    static const uint64_t WRITER_FLAG_MASK      = (uint64_t)1 << WRITER_FLAG_BASE;

    static const uint64_t READER_VALUE          = (uint64_t)1;
    static const uint64_t WAITING_READERS_VALUE = (uint64_t)1 << WAITING_READERS_BASE;
    static const uint64_t WAITING_WRITERS_VALUE = (uint64_t)1 << WAITING_WRITERS_BASE;
  };

  template <class Mtx>
  class shared_lock
  {
  public:
    shared_lock():
      pmtx( nullptr ), owns( false ) {}
    shared_lock( Mtx& mtx ):
      pmtx( &mtx ), owns( true )  {  mtx.lock_shared();  }
	  shared_lock( Mtx& mtx, std::defer_lock_t ):
	    pmtx( &mtx ), owns( false ) {}
	  shared_lock( Mtx& mtx, std::adopt_lock_t ):
	    pmtx( &mtx ), owns( true )  {}
    shared_lock( shared_lock&& s ) noexcept:
      pmtx( s.pmtx ), owns( s.owns )  {  s.pmtx = nullptr;  s.owns = false;  }
   ~shared_lock()
      {
        if ( pmtx != nullptr && owns )
          pmtx->unlock_shared();
      }
    shared_lock& operator = ( shared_lock&& s ) noexcept
      {
        if ( pmtx != nullptr && owns )
          pmtx->unlock_shared();
        pmtx = s.pmtx;  s.pmtx = nullptr;
        owns = s.owns;  s.owns = false;
          return *this;
      }
    void  lock()
      {
        if ( pmtx == nullptr )
          throw std::runtime_error( "shared_lock::lock is not properly initialized" );
        if ( owns )
          throw std::runtime_error( "shared_lock::lock is already locked" );
        pmtx->lock_shared();
          owns = true;
      }
    void  unlock()
      {
        if ( pmtx == nullptr )
          throw std::runtime_error( "shared_lock::lock is not properly initialized" );
        if ( !owns )
          throw std::runtime_error( "shared_lock::lock is not locked" );
        pmtx->unlock_shared();
          owns = false;
      }
    bool  owns_lock() const
      {
        return owns;
      }
  protected:
    Mtx*  pmtx;
    bool  owns;

  };

  template <class Mtx>
  auto  make_unique_lock( Mtx& mtx ) -> std::unique_lock<Mtx>
    {  return std::move( std::unique_lock<Mtx>( mtx ) );  }

  template <class Mtx>
  auto  make_unique_lock( Mtx& mtx, const std::defer_lock_t& defer ) -> std::unique_lock<Mtx>
    {  return std::move( std::unique_lock<Mtx>( mtx, defer ) );  }

  template <class Mtx>
  auto  make_shared_lock( Mtx& mtx ) -> mtc::shared_lock<Mtx>
    {  return std::move( mtc::shared_lock<Mtx>( mtx ) );  }

  template <class Mtx>
  auto  make_shared_lock( Mtx& mtx, const std::defer_lock_t& ) -> mtc::shared_lock<Mtx>
    {  return std::move( mtc::shared_lock<Mtx>( mtx, std::defer_lock ) );  }

  template <class lock, class action, class ...argset>
  auto  interlocked( lock, action fn, argset... ag ) -> decltype(fn(ag...))
    {  return fn( ag... );  }

// recursive_shared_mutex implementation

  inline
  void  recursive_shared_mutex::lock_shared()
  {
    for ( auto uState = mtxState.load( std::memory_order_relaxed ); ; )
    {
      if ( (uState & WRITER_FLAG_MASK) == 0 )
      {
        if ( mtxState.compare_exchange_weak( uState, uState + READER_VALUE, std::memory_order_acquire, std::memory_order_relaxed ) )
          return;
      }
        else
      if ( mtxState.compare_exchange_weak( uState, uState + WAITING_READERS_VALUE, std::memory_order_relaxed, std::memory_order_relaxed ) )
        return (void)cvReader.wait();
    }
  }

  inline
  void  recursive_shared_mutex::unlock_shared()
  {
    for ( auto uState = mtxState.load( std::memory_order_relaxed ); ; )
    {
      if ( (uState & READERS_MASK) != READER_VALUE || (uState & WAITING_WRITERS_MASK) == 0 )
      {
        if ( mtxState.compare_exchange_weak( uState, uState - READER_VALUE, std::memory_order_relaxed, std::memory_order_relaxed ) )
          return;
      }
        else
      {
        auto  uValue = (uState - READER_VALUE - WAITING_WRITERS_VALUE) | WRITER_FLAG_MASK;

        if ( mtxState.compare_exchange_weak( uState, uValue, std::memory_order_relaxed, std::memory_order_relaxed ) )
          return (void)cvWriter.notify_one();
      }
    }
  }

  inline
  void  recursive_shared_mutex::lock()
  {
    auto  uState = mtxState.load( std::memory_order_acquire );

    if ( (uState & WRITER_FLAG_MASK) && writerId == std::this_thread::get_id() )
      return (void)++nRecurse;

    for ( ; ; )
    {
      if ( (uState & (WRITER_FLAG_MASK | READERS_MASK)) == 0 )
      {
        if ( mtxState.compare_exchange_weak( uState, uState | WRITER_FLAG_MASK, std::memory_order_acquire, std::memory_order_relaxed ) )
          return (void)(writerId = std::this_thread::get_id(), nRecurse = 0);
      }
        else
      if ( mtxState.compare_exchange_weak( uState, uState + WAITING_WRITERS_VALUE, std::memory_order_relaxed, std::memory_order_relaxed ) )
      {
        cvWriter.wait();
        writerId = std::this_thread::get_id();
        nRecurse = 0;
        return;
      }
    }
  }

  inline
  void  recursive_shared_mutex::unlock()
  {
    auto  uState = mtxState.load( std::memory_order_relaxed );

    if ( (uState & WRITER_FLAG_MASK) != 0 && nRecurse != 0 && writerId == std::this_thread::get_id() )
      return (void)--nRecurse;

    writerId = std::thread::id();

    for ( ; ; )
    {
      if ( (uState & WAITING_WRITERS_MASK) != 0 )
      {
        auto  uValue = uState - WAITING_WRITERS_VALUE;

        if ( mtxState.compare_exchange_weak( uState, uValue, std::memory_order_release, std::memory_order_relaxed ) )
          return (void)cvWriter.notify_one();
        else continue;
      }
      if ( (uState & WAITING_READERS_MASK) != 0 )
      {
        auto  wakeups = (uState & WAITING_READERS_MASK) >> WAITING_READERS_BASE;
        auto  uChange = (uState & ~WRITER_FLAG_MASK) - (uState & WAITING_READERS_MASK) + wakeups;

        if ( mtxState.compare_exchange_weak( uState, uChange, std::memory_order_release, std::memory_order_relaxed ) )
          return (void)cvReader.notify_all();
        else continue;
      }
      if ( mtxState.compare_exchange_weak( uState, 0, std::memory_order_release, std::memory_order_relaxed ) )
        return;
    }
  }
/*
  inline
  void  recursive_shared_mutex::shared_to_write()
  {
    for ( auto uState = mtxState.load( std::memory_order_relaxed ); ; )
    {
      if ( (uState & READERS_MASK) == READER_VALUE )
      {
        if ( mtxState.compare_exchange_weak( uState, (uState - READER_VALUE) | WRITER_FLAG_MASK, std::memory_order_acquire, std::memory_order_relaxed ) )
          return (void)(writerId = std::this_thread::get_id(), nRecurse = 0);
      }
        else
      if ( mtxState.compare_exchange_weak( uState, uState - READER_VALUE + WAITING_WRITERS_VALUE, std::memory_order_relaxed, std::memory_order_relaxed ) )
        return (void)(cvWriter.wait(), writerId = std::this_thread::get_id(), nRecurse = 0);
    }
  }
*/

}

# endif   // __priority_mutex_h__
