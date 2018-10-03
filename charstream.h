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
# if !defined( __mtc_charstream_h__ )
# define __mtc_charstream_h__
# include "serialize.decl.h"

namespace mtc
{

  /*
    charstream - класс для вычитывания данных из потока с промежуточной буферизацией,
    позволяющий делать getnext() и putback(). Глубина стека тождественно равна 2 :)
  */
  template <class S>
  class charstream
  {
    S*    stream;
    char  chbuff[3];
    int   buflen;

  public:     // construction
    charstream( S*  s = nullptr ): stream( s ), buflen( 0 )  {}
    charstream& operator = ( S* s )   {  stream = s;  return *this;  }
    operator S* ()  {  return stream;  }

  public:     // reading
    char  getnext()
      {
        char  getchr;
        
        assert( buflen == 0 || buflen == 1 || buflen == 2 );

        if ( buflen != 0 )
          return chbuff[--buflen];
        return (stream = (S*)::FetchFrom( stream, (char&)getchr )) != nullptr ? getchr : '\0';
      }
    /*
      get first non-space character
    */
    char  nospace()
      {
        for ( ; ; )
        {
          char  getchr;
          char  chnext;

          if ( (getchr = getnext()) == '\0' )
            return getchr;

          if ( (unsigned char)getchr <= 0x20 )
            continue;

          if ( getchr != '/' )
            return getchr;

          switch ( chnext = getnext() )
          {
            case '*':
              {
                for ( getchr = '/'; ; getchr = chnext )
                {
                  if ( (chnext = getnext()) == '\0')
                    return chnext;
                  if ( chnext == '/' && getchr == '*' )
                    break;
                }
                break;
              }
            case '/':
              {
                while ( (getchr = getnext()) != '\0' && getchr != '\n' )
                  (void)NULL;
                if ( getchr == '\0' ) return getchr;
                  else break;
              }

            case '\0':  return getchr;
            default:    this->putchar( chnext );
                        return getchr;
          }
        }
      }
    charstream&  putback( char chr )
      {
        assert( buflen == 0 || buflen == 1 );
        chbuff[buflen++] = chr;
        return *this;
      }

  };

}

# endif  // __mtc_charstream_h__
