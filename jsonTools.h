/*

The MIT License (MIT)

Copyright (c) 2016 Àíäðåé Êîâàëåíêî aka Keva
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

Äàííàÿ ëèöåíçèÿ ðàçðåøàåò ëèöàì, ïîëó÷èâøèì êîïèþ äàííîãî ïðîãðàììíîãî îáåñïå÷åíèÿ
è ñîïóòñòâóþùåé äîêóìåíòàöèè (â äàëüíåéøåì èìåíóåìûìè «Ïðîãðàììíîå Îáåñïå÷åíèå»),
áåçâîçìåçäíî èñïîëüçîâàòü Ïðîãðàììíîå Îáåñïå÷åíèå áåç îãðàíè÷åíèé, âêëþ÷àÿ íåîãðàíè÷åííîå
ïðàâî íà èñïîëüçîâàíèå, êîïèðîâàíèå, èçìåíåíèå, ñëèÿíèå, ïóáëèêàöèþ, ðàñïðîñòðàíåíèå,
ñóáëèöåíçèðîâàíèå è/èëè ïðîäàæó êîïèé Ïðîãðàììíîãî Îáåñïå÷åíèÿ, à òàêæå ëèöàì, êîòîðûì
ïðåäîñòàâëÿåòñÿ äàííîå Ïðîãðàììíîå Îáåñïå÷åíèå, ïðè ñîáëþäåíèè ñëåäóþùèõ óñëîâèé:

Óêàçàííîå âûøå óâåäîìëåíèå îá àâòîðñêîì ïðàâå è äàííûå óñëîâèÿ äîëæíû áûòü âêëþ÷åíû âî
âñå êîïèè èëè çíà÷èìûå ÷àñòè äàííîãî Ïðîãðàììíîãî Îáåñïå÷åíèÿ.

ÄÀÍÍÎÅ ÏÐÎÃÐÀÌÌÍÎÅ ÎÁÅÑÏÅ×ÅÍÈÅ ÏÐÅÄÎÑÒÀÂËßÅÒÑß «ÊÀÊ ÅÑÒÜ», ÁÅÇ ÊÀÊÈÕ-ËÈÁÎ ÃÀÐÀÍÒÈÉ,
ßÂÍÎ ÂÛÐÀÆÅÍÍÛÕ ÈËÈ ÏÎÄÐÀÇÓÌÅÂÀÅÌÛÕ, ÂÊËÞ×Àß ÃÀÐÀÍÒÈÈ ÒÎÂÀÐÍÎÉ ÏÐÈÃÎÄÍÎÑÒÈ,
ÑÎÎÒÂÅÒÑÒÂÈß ÏÎ ÅÃÎ ÊÎÍÊÐÅÒÍÎÌÓ ÍÀÇÍÀ×ÅÍÈÞ È ÎÒÑÓÒÑÒÂÈß ÍÀÐÓØÅÍÈÉ, ÍÎ ÍÅ ÎÃÐÀÍÈ×ÈÂÀßÑÜ
ÈÌÈ.

ÍÈ Â ÊÀÊÎÌ ÑËÓ×ÀÅ ÀÂÒÎÐÛ ÈËÈ ÏÐÀÂÎÎÁËÀÄÀÒÅËÈ ÍÅ ÍÅÑÓÒ ÎÒÂÅÒÑÒÂÅÍÍÎÑÒÈ ÏÎ ÊÀÊÈÌ-ËÈÁÎ ÈÑÊÀÌ,
ÇÀ ÓÙÅÐÁ ÈËÈ ÏÎ ÈÍÛÌ ÒÐÅÁÎÂÀÍÈßÌ, Â ÒÎÌ ×ÈÑËÅ, ÏÐÈ ÄÅÉÑÒÂÈÈ ÊÎÍÒÐÀÊÒÀ, ÄÅËÈÊÒÅ ÈËÈ ÈÍÎÉ
ÑÈÒÓÀÖÈÈ, ÂÎÇÍÈÊØÈÌ ÈÇ-ÇÀ ÈÑÏÎËÜÇÎÂÀÍÈß ÏÐÎÃÐÀÌÌÍÎÃÎ ÎÁÅÑÏÅ×ÅÍÈß ÈËÈ ÈÍÛÕ ÄÅÉÑÒÂÈÉ
Ñ ÏÐÎÃÐÀÌÌÍÛÌ ÎÁÅÑÏÅ×ÅÍÈÅÌ.

*/
# if !defined( __jsonTools_h__ )
# define __jsonTools_h__
# include "zarray.h"

namespace mtc
{
  char*   PrintJson( char* o, char c )
    {  if ( o != nullptr ) *o++ = c;  return o;  }
  char*   PrintJson( char* o, const char* s, size_t l )
    {  if ( o != nullptr ) while ( l-- > 0 ) *o++ = *s++;  return o;  }
  FILE*   PrintJson( FILE* o, char c )
    {  if ( o != nullptr ) o = fwrite( &c, 1, 1, o ) == 1 ? o : nullptr;  return o;  }
  FILE*   PrintJson( FILE* o, const char* s, size_t l )
    {  if ( o != nullptr ) o = fwrite( s, 1, l, o ) == l ? o : nullptr;  return o;  }

// JSON serialization
  # define  derive_printjson_dec( _type_, _tmpl_ )                      \
  template <class O> inline  O*  PrintJson( O* o, _type_ t )            \
    {                                                                   \
      char  decval[0x10];                                               \
      return PrintJson( o, decval, sprintf( decval, _tmpl_, t ) );      \
    }
    derive_printjson_dec( char,   "%d" )
    derive_printjson_dec( byte_t, "%u" )
    derive_printjson_dec( int16_t,  "%d" )
    derive_printjson_dec( word16_t, "%u" )
    derive_printjson_dec( int32_t,  "%d" )
    derive_printjson_dec( word32_t, "%u" )
  # if defined( _MSC_VER )
    derive_printjson_dec( int64_t,  "%I64d" )
    derive_printjson_dec( word64_t, "%I64u" )
  # else
    derive_printjson_dec( int64_t,  "%lld" )
    derive_printjson_dec( word64_t, "%llu" )
  # endif  // _MSC_VER
  # undef derive_printjson_dec

  # define  derive_printjson_flo( _type_ )                              \
    template <class O> inline  O*  PrintJson( O* o, _type_ t )          \
    {                                                                   \
      char  floval[0x10];                                               \
      return PrintJson( o, floval, sprintf( floval, "%f", t ) );        \
    }
    derive_printjson_flo( float )
    derive_printjson_flo( double )
  # undef derive_printjson_flo

  template <class O, class C>
  inline  O*  PrintText( O* o, const C* s, size_t l )
  {
    static char         repsrc[] = "\b\t\n\f\r\"/\\";
    static const char*  repval[] = { "\\b", "\\t", "\\n", "\\f", "\\r", "\\\"", "\\/", "\\\\" };
    char*               reppos;
    char                chnext[0x10];

    if ( s == nullptr )
      return PrintJson( o, "null", 4 );

    for ( o = PrintJson( o, '\"' ); l-- > 0; ++s )
    {
      if ( (*s & ~0xff) == 0 && (reppos = strchr( repsrc, *s )) != nullptr )
        o = PrintJson( o, repval[reppos - repsrc], strlen( repval[reppos - repsrc] ) );   else
      if ( (unsigned)*s >= 0x80 || (unsigned)*s < 0x20 )
        o = PrintJson( o, chnext, sprintf( chnext, "\\u%04x", (unsigned)*s ) );           else
      o = PrintJson( o, (char)*s );
    }

    return PrintJson( o, '\"' );
  }

  template <class O>
  inline  O*  PrintJson( O* o, const char* s )
    {  return PrintText( o, (const unsigned char*)s, w_strlen( s ) );  }
  template <class O>
  inline  O*  PrintJson( O* o, const widechar* s )
    {  return PrintText( o, s, w_strlen( s ) );  }
  template <class O>
  inline  O*  PrintJson( O* o, const _auto_<char>& s )
    {  return PrintText( o, (const unsigned char*)(const char*)s, w_strlen( s ) );  }
  template <class O>
  inline  O*  PrintJson( O* o, const _auto_<widechar>& s )
    {  return PrintText( o, (const widechar*)s, w_strlen( s ) );  }

  template <class O, class M>  O*  PrintJson( O*, const xvalue<M>& );
  template <class O, class M>  O*  PrintJson( O*, const zarray<M>& );

// arrays
  template <class O, class T, class M>
  inline  O*  PrintJson( O* o, const array<T, M>&  a )
  {
    auto  ptop = a.begin();
    auto  pend = a.end();

    for ( o = PrintJson( o, '[' ); o != nullptr && ptop < pend; ++ptop )
      o = PrintJson( PrintJson( o, *ptop ), ", ", ptop < pend - 1 ? 2 : 0 );
    return PrintJson( o, ']' );
  }

  template <class O, class M>
  inline  O*  PrintJson( O* o, const xvalue<M>& v )
  {
    switch ( v.gettype() )
    {
      case z_char:    return PrintJson( o, *v.get_char() );
      case z_byte:    return PrintJson( o, *v.get_byte() );
      case z_int16:   return PrintJson( o, *v.get_int16() );
      case z_word16:  return PrintJson( o, *v.get_word16() );
      case z_int32:   return PrintJson( o, *v.get_int32() );
      case z_word32:  return PrintJson( o, *v.get_word32() );
      case z_int64:   return PrintJson( o, *v.get_int64() );
      case z_word64:  return PrintJson( o, *v.get_word64() );
      case z_float:   return PrintJson( o, *v.get_float() );
      case z_double:  return PrintJson( o, *v.get_double() );

      case z_charstr: return PrintJson( o, v.get_charstr() );
      case z_widestr: return PrintJson( o, v.get_widestr() );
      case z_zarray:  return PrintJson( o, *v.get_zarray() );

      case z_array_char:    return PrintJson( o, *v.get_array_char() );
      case z_array_byte:    return PrintJson( o, *v.get_array_byte() );
      case z_array_int16:   return PrintJson( o, *v.get_array_int16() );
      case z_array_word16:  return PrintJson( o, *v.get_array_word16() );
      case z_array_int32:   return PrintJson( o, *v.get_array_int32() );
      case z_array_word32:  return PrintJson( o, *v.get_array_word32() );
      case z_array_int64:   return PrintJson( o, *v.get_array_int64() );
      case z_array_word64:  return PrintJson( o, *v.get_array_word64() );
      case z_array_float:   return PrintJson( o, *v.get_array_float() );
      case z_array_double:  return PrintJson( o, *v.get_array_double() );

      case z_array_charstr: return PrintJson( o, *v.get_array_charstr() );
      case z_array_widestr: return PrintJson( o, *v.get_array_widestr() );
      case z_array_zarray:  return PrintJson( o, *v.get_array_zarray() );
      case z_array_xvalue:  return PrintJson( o, *v.get_array_xvalue() );

      default:  assert( false );  abort();  return o;
    }
  }

  template <class O, class M>
  inline  O*  PrintJson( O* o, const zarray<M>& v )
  {
    bool  bcomma = false;

    o = PrintJson( o, '{' );

    v.for_each( [&o, &bcomma]( const typename zarray<M>::zkey& k, const xvalue<M>& v )
      {
      // possible comma
        if ( bcomma )
          o = PrintJson( o, ", ", 2 );

      // key
        if ( (const char*)k != nullptr )      o = PrintJson( o, (const char*)k );  else
        if ( (const widechar*)k != nullptr )  o = PrintJson( o, (const widechar*)k );  else
                                              o = PrintJson( PrintJson( PrintJson( o, '"' ), (unsigned)k ), '"' );

      // value
        o = PrintJson( PrintJson( o, ": ", 2 ), v );  bcomma = true;
        return 0;
      } );

    return PrintJson( o, '}' );
  }

}

# endif  // __jsonTools_h__
