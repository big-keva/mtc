/*

The MIT License (MIT)

Copyright (c) 2016 Андрей Коваленко aka Keva
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

*/
# if !defined( __stdlog_h__ )
# define __stdlog_h__
# include <cstdarg>
# include <stdio.h>

namespace mtc
{
  class MessageLogger
  {
    FILE*   output;

  public:
    MessageLogger(): output( stderr )
      {
      }

  public:
    int   vmessage( int err, const char* msg, va_list var )
      {
        vfprintf( output, msg, var );
        fprintf( output, "\n" );
        return err;
      }
    int   message( int err, const char* msg, ... )
      {
        if ( msg != nullptr )
        {
          va_list vaargs;

          va_start( vaargs, msg );
            vmessage( err, msg, vaargs );
          va_end( vaargs );
        }
        return err;
      }
    template <class T>
    const T&  log( const T& t, const char* msg, ... )
      {
        if ( msg != nullptr )
        {
          va_list vaargs;

          va_start( vaargs, msg );
            vfprintf( output, msg, vaargs );
          va_end( vaargs );
        }
        return t;
      }
  };

}

# define log_error( err, msg, ... )   mtc::MessageLogger().log( (err), (msg), __VA_ARGS__ )
# define log_warning( err, msg, ... ) mtc::MessageLogger().log( (err), (msg), __VA_ARGS__ )
# define log_info( msg, ... )         mtc::MessageLogger().log( 0,     (msg), __VA_ARGS__ )

# endif  // __stdlog_h__
