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
# if !defined( __mtc_strptime_h__ )
# define __mtc_strptime_h__
# include "wcsstr.h"
# include <time.h>

namespace mtc
{

  namespace __strptime
  {
    inline  bool  is_space( char c )  {  return c > 0 && c <= 0x20;  }
    inline  bool  is_space( widechar c )  {  return c <= 0x20;  }

# define derive_check( out, str, key, val )                   \
    if ( w_strncmp( key, str, sizeof(key) - 1 ) == 0 )  {  out = val;  return str + sizeof(key) - 1;  } else

    struct ParseWeekDayAbbrEn
    {
      template <class chartype>
      static  const chartype* parse( int& d, const chartype* s )
        {
          derive_check( d, s, "Fri", 5 )
          derive_check( d, s, "Mon", 1 )
          derive_check( d, s, "Sat", 6 )
          derive_check( d, s, "Sun", 0 )
          derive_check( d, s, "Thu", 4 )
          derive_check( d, s, "Tue", 2 )
          derive_check( d, s, "Wed", 3 )
          return nullptr;
        }
    };

    struct ParseWeekDayFullEn
    {
      template <class chartype>
      static  const chartype* parse( int& d, const chartype* s )
        {
          derive_check( d, s, "Friday",   5 )
          derive_check( d, s, "Monday",   1 )
          derive_check( d, s, "Saturday", 6 )
          derive_check( d, s, "Sunday",   0 )
          derive_check( d, s, "Thursday", 4 )
          derive_check( d, s, "Tuesday",  2 )
          derive_check( d, s, "Wednesday", 3 )
          return nullptr;
        }
    };

    struct ParseMonAbbrEn
    {
      template <class chartype>
      static  const chartype* parse( int& d, const chartype* s )
        {
          derive_check( d, s, "Apr", 3 )
          derive_check( d, s, "Aug", 7 )
          derive_check( d, s, "Dec", 11 )
          derive_check( d, s, "Feb", 1 )
          derive_check( d, s, "Jan", 0 )
          derive_check( d, s, "Jul", 6 )
          derive_check( d, s, "Jun", 5 )
          derive_check( d, s, "Mar", 2 )
          derive_check( d, s, "May", 4 )
          derive_check( d, s, "Nov", 10 )
          derive_check( d, s, "Oct", 9 )
          derive_check( d, s, "Sep", 8 )
          return nullptr;
        }
    };

    struct ParseMonFullEn
    {
      template <class chartype>
      static  const chartype* parse( int& d, const chartype* s )
        {
          derive_check( d, s, "April", 3 )
          derive_check( d, s, "August", 7 )
          derive_check( d, s, "December", 11 )
          derive_check( d, s, "February", 1 )
          derive_check( d, s, "January", 0 )
          derive_check( d, s, "July", 6 )
          derive_check( d, s, "June", 5 )
          derive_check( d, s, "March", 2 )
          derive_check( d, s, "May", 4 )
          derive_check( d, s, "November", 10 )
          derive_check( d, s, "October", 9 )
          derive_check( d, s, "September", 8 )
          return nullptr;
        }
    };

    struct ParseAmPm
    {
      template <class chartype>
      static  const chartype* parse( int& d, const chartype* s )
        {
          derive_check( d, s, "am", 0 )
          derive_check( d, s, "pm", 1 )
          return nullptr;
        }
    };
# undef derive_check

    template <class chartype, class intvalue>
    const chartype* parseint( intvalue& v, const chartype* s )
      {
        chartype* p;
        int       r;

        r = w_strtol( s, &p, 10 );

        if ( p == s )
          return nullptr;
        v = r;
          return p;
      }

    /*
     * tm_year is relative this year 
     */
    const int tm_year_base = 1900;

    /*
     * Return TRUE iff `year' was a leap year.
     * Needed for strptime.
     */
    inline  int   is_leap( int year )
    {
        return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
    }

    /* Needed for strptime. */
    inline  int   first_day( int year )
    {
        int ret = 4;

        for (; year > 1970; --year)
          ret = (ret + 365 + is_leap( year ) ? 1 : 0) % 7;
        return ret;
    }

    /*
     * Set `t' given `wnum' (week number [0, 53])
     * Needed for strptime
     */

    inline  void  set_week_number_sun (struct tm *t, int wnum)
    {
        int fday = first_day (t->tm_year + tm_year_base);

        t->tm_yday = wnum * 7 + t->tm_wday - fday;
        if (t->tm_yday < 0)
        {
          t->tm_wday = fday;
          t->tm_yday = 0;
        }
    }

    /*
     * Set `t' given `wnum' (week number [0, 53])
     * Needed for strptime
     */

    inline  void  set_week_number_mon (struct tm *t, int wnum)
    {
        int fday = (first_day (t->tm_year + tm_year_base) + 6) % 7;

        t->tm_yday = wnum * 7 + (t->tm_wday + 6) % 7 - fday;
        if (t->tm_yday < 0)
        {
          t->tm_wday = (fday + 1) % 7;
          t->tm_yday = 0;
        }
    }

    /*
     * Set `t' given `wnum' (week number [0, 53])
     * Needed for strptime
     */
    inline  void  set_week_number_mon4 (struct tm *t, int wnum)
    {
        int fday = (first_day (t->tm_year + tm_year_base) + 6) % 7;
        int offset = 0;

        if (fday < 4)
          offset += 7;

        t->tm_yday = offset + (wnum - 1) * 7 + t->tm_wday - fday;
        if (t->tm_yday < 0)
        {
          t->tm_wday = fday;
          t->tm_yday = 0;
        }
    }

  }

  template <class c1, class c2>
  const c1* __impl_strptime( const c1* s, const c2* f, struct tm* t );

  inline  const char* strptime( const char* s, const char* f, struct tm* t )
    {  return __impl_strptime( s, f, t );  }
  inline  const char* strptime( const char* s, const widechar* f, struct tm* t )
    {  return __impl_strptime( s, f, t );  }
  inline  const widechar* strptime( const widechar* s, const char* f, struct tm* t )
    {  return __impl_strptime( s, f, t );  }
  inline  const widechar* strptime( const widechar* s, const widechar* f, struct tm* t )
    {  return __impl_strptime( s, f, t );  }

  /* strptime: roken */
  /* extern "C" */
  /* strptime (const char *buf, const char *format, struct tm *t) */
  template <class c1, class c2>
  const c1* __impl_strptime( const c1* s, const c2* f, struct tm* t )
  {
    c2  c;

    for ( ; (c = *f) != 0; ++f )
    {
      int       r;

      if ( __strptime::is_space( c ) )
      {
        while ( __strptime::is_space( *s ) )
          ++s;
      }
        else
      if ( c == '%' && f[1] != 0 )
      {
        if ( (c = *++f) == 'E' || c == 'O' )
          c = *++f;

        switch (c)
        {
          case 'A':
            if ( (s = __strptime::ParseWeekDayFullEn::parse( r, s )) == nullptr )  return nullptr;
              else t->tm_wday = r;
            break;
            
          case 'a':
            if ( (s = __strptime::ParseWeekDayAbbrEn::parse( r, s )) == nullptr )  return nullptr;
              else t->tm_wday = r;
            break;

          case 'B':
            if ( (s = __strptime::ParseMonFullEn::parse( r, s )) == nullptr )  return nullptr;
              else t->tm_mon = r;
            break;

          case 'b':
          case 'h':
            if ( (s = __strptime::ParseMonAbbrEn::parse( r, s )) == nullptr )  return nullptr;
              else t->tm_mon = r;
            break;

          case 'C':
            if ( (s = __strptime::parseint( t->tm_mday, s )) != nullptr ) t->tm_year = (t->tm_year * 100) - __strptime::tm_year_base;
              else return nullptr;
            break;

          case 'c':
            assert( "Unsupported format character" == 0 );
            abort();

          case 'D':    /* %m/%d/%y */
            if ( (s = __impl_strptime( s, "%m/%d/%y", t )) == nullptr )
              return nullptr;
            break;

          case 'd':
          case 'e':
            if ( (s = __strptime::parseint( t->tm_mday, s )) == nullptr )
              return nullptr;
            break;

          case 'H':
          case 'k':
            if ( (s = __strptime::parseint( t->tm_hour, s )) == nullptr )
              return nullptr;
            break;

          case 'I':
          case 'l':
            if ( (s = __strptime::parseint( t->tm_hour, s )) == nullptr )
              return nullptr;
            if ( t->tm_hour == 12 )
              t->tm_hour = 0;
            break;

          case 'j':
            if ( (s = __strptime::parseint( t->tm_yday, s )) == nullptr )
              return nullptr;
            --t->tm_yday;
            break;

          case 'm':
            if ( (s = __strptime::parseint( t->tm_mon, s )) == nullptr )
              return nullptr;
            --t->tm_mon;
            break;

          case 'M':
            if ( (s = __strptime::parseint( t->tm_min, s )) == nullptr )
              return nullptr;
            break;

          case 'n':
            if ( *s == '\n' ) do ++s;  while ( *s == '\r' || *s == '\n' );
              else return nullptr;
            break;

          case 'p':
            if ( (s = __strptime::ParseAmPm::parse( r, s )) == nullptr )
              return nullptr;
            if ( t->tm_hour == 0 && r == 1 ) t->tm_hour = 12;
              else  t->tm_hour += 12;
            break;

          case 'r':   /* %I:%M:%S %p */
            if ( (s = mtc::strptime( s, "%I:%M:%S %p", t )) == nullptr )
              return nullptr;
            break;

          case 'R':   /* %H:%M */
            if ( (s = mtc::strptime( s, "%H:%M", t )) == nullptr )
              return nullptr;
            break;

           case 'S':
            if ( (s = __strptime::parseint( t->tm_sec, s )) == nullptr )
              return nullptr;
            break;

          case 't':
            if ( *s == '\t' ) do ++s;  while ( *s == '\t' );
              else return nullptr;
            break;

          case 'T':    /* %H:%M:%S */
          case 'X':
            if ( (s = mtc::strptime( s, "%H:%M:%S", t )) == nullptr )
              return nullptr;
            break;

          case 'u':
            if ( (s = __strptime::parseint( t->tm_wday, s )) != nullptr ) --t->tm_wday;
              else return nullptr;
            break;

          case 'w':
            if ( (s = __strptime::parseint( t->tm_wday, s )) == nullptr )
              return nullptr;
            break;

          case 'U':
            if ( (s = __strptime::parseint( r, s )) != nullptr )  __strptime::set_week_number_sun( t, r );
              else return nullptr;
            break;

          case 'V':
            if ( (s = __strptime::parseint( r, s )) != nullptr )  __strptime::set_week_number_mon4( t, r );
              else return nullptr;
            break;

          case 'W':
            if ( (s = __strptime::parseint( r, s )) != nullptr )  __strptime::set_week_number_mon( t, r );
              else return nullptr;
            break;

          case 'x':
            if ( (s = mtc::strptime( s, "%Y:%m:%d", t )) == nullptr )
              return nullptr;
            break;

          case 'y':
            if ( (s = __strptime::parseint( t->tm_year, s )) != nullptr ) t->tm_year = t->tm_year < 70 ? t->tm_year + 100 : t->tm_year;
              else return nullptr;
            break;

          case 'Y':
            if ( (s = __strptime::parseint( t->tm_year, s )) != nullptr ) t->tm_year -= __strptime::tm_year_base;
              else return nullptr;
            break;

          case 'Z':
            abort ();

          case '\0' :
            --f;

            /* FALLTHROUGH */
          case '%':
            if ( *s == '%' )  ++s;
              else return nullptr;
            break;

          default:
            if ( *s == '%' || *++s == c ) ++s;
              else return nullptr;
            break;
        }
      }
        else
      {
        if ( *s == c )  ++s;
          else return nullptr;
      }
    }
    return s;
  }

}

# endif  // __mtc_strptime_h__
