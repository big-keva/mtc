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
# if !defined( __mtc_file_h__ )
# define __mtc_file_h__
# include <stdio.h>

namespace mtc
{
  class file
  {
    FILE* f;

  public:     // construction
    file( FILE* in = nullptr ):
      f( in ) {}
    file( file&& s ):
      f( s.f ) {  s.f = nullptr;  }
    file( const file& s ):
      f( s.f ) {  ((file&)s).f = nullptr;  } 
   ~file()
      {  if ( f ) fclose( f );  }

  public:     // operators
    file& operator = ( FILE* in )
      {  if ( f ) fclose( f );  f = in;  return *this;  }
    file& operator = ( file&& in )
      {  if ( f ) fclose( f );  f = in.f;  in.f = nullptr;  return *this;  }
    operator FILE* () const
      {  return f;  }
    FILE* detach()
      {  FILE* p = f;  f = nullptr;  return p;  }
  };
}

# endif  // __mtc_file_h__
