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
# include <mutex>

namespace mtc
{

  class recursive_shared_mutex
  {
    std::recursive_mutex        thrdLock;
    std::condition_variable_any w_Reader;
    std::condition_variable_any w_Writer;
    int                         nReaders = 0;
    int                         nWriters = 0;
    std::thread::id             writerId;

  public:     // constructors
    recursive_shared_mutex()  {}
    recursive_shared_mutex( const recursive_shared_mutex& ) = delete;

  public:     // exclusive locking
    void    lock();
//    bool    try_lock();
    void    unlock();

  public:     // shared locking
    void    lock_shared();
//    bool    try_lock_shared();
    void    unlock_shared();

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
  void  recursive_shared_mutex::lock()
    {
      auto  u_lock = make_unique_lock( thrdLock );
      auto  thrdId = std::this_thread::get_id();

      if ( thrdId != writerId )
        w_Writer.wait( u_lock, [&](){  return nWriters == 0;  } );

      writerId = thrdId;
        ++nWriters;

      w_Reader.wait( u_lock, [&](){  return nReaders == 0;  } );
    }

  inline
  void  recursive_shared_mutex::unlock()
    {
      auto  aulock = make_unique_lock( thrdLock );

      if ( --nWriters == 0 )
        w_Writer.notify_all();
    }

  inline
  void  recursive_shared_mutex::lock_shared()
    {
      auto  u_lock = make_unique_lock( thrdLock );
      auto  thrdId = std::this_thread::get_id();

      if ( thrdId != writerId )
        w_Writer.wait( u_lock, [&](){  return nReaders < INT_MAX && (nWriters == 0 || nReaders > 0);  } );
      ++nReaders;
    }

  inline
  void  recursive_shared_mutex::unlock_shared()
    {
      auto  aulock = make_unique_lock( thrdLock );

      if ( --nReaders, nWriters != 0 )
      {
      // writer is waiting, and this thread is a last reader
        if ( nReaders == 0 )
          w_Reader.notify_one();
      }
        else
      {
      // Nobody is waiting for shared locks to clear, if we were at the max
      // capacity, release one thread waiting to obtain a shared lock in lock_shared().
        if ( nReaders == INT_MAX - 1 )
          w_Writer.notify_one();
      }
    }

}

# endif   // __priority_mutex_h__
