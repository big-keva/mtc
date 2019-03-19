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
# if !defined( __mtc_dir_hpp__ )
# define __mtc_dir_hpp__
# include <cstddef>
# include <cstdint>

# if !defined( __widechar_defined__ )
# define  __widechar_defined__
#   if defined(WCHAR_MAX) && (WCHAR_MAX >> 16) == 0
    typedef wchar_t         widechar;
#   else
    typedef unsigned short  widechar;
#   endif  // size
# endif  // __widechar_defined__

namespace mtc {
namespace fs {

  class directory
  {
    class inner_t;
    class string;

  public:     // constants
    enum
    {
      attr_dir  = 0x00000001,
      attr_file = 0x00000002,
      attr_any  = 0x00000003
    };

    class entry;

    inner_t*  data;

  public:     // construction
    directory();
    directory( directory&& );
    directory( const directory& );
   ~directory();
    auto  operator = ( directory&& ) -> directory&;
    auto  operator = ( const directory& ) -> directory&;

  public:     // open
    static  auto  open( const char*     dir, unsigned attrib = attr_any ) -> directory;
    static  auto  open( const widechar* dir, unsigned attrib = attr_any ) -> directory;

  public:     // get
    auto  get() -> entry;

  public:     // initialized
    operator bool() const  {  return defined();  }
    bool  defined() const  {  return data != nullptr;  }

  };

  class directory::string
  {
    friend class directory;
    class inner_t;

    static  widechar  zero;
    inner_t*          data;

  protected:  // construction
    string( const widechar*, size_t = (size_t)-1 );
    string( const char*, size_t = (size_t)-1 );
    string();

  public:     // construction
    string( const string& );
   ~string();
    auto  operator = ( const string& ) -> string&;
    auto  operator == ( const nullptr_t ) const -> bool {  return data == nullptr;  }
    auto  operator != ( const nullptr_t ) const -> bool {  return data != nullptr;  }

  public:     // access
    operator const char*     () const {  return charstr();  }
    operator const widechar* () const {  return widestr();  }

    auto  charstr() const -> const char*;
    auto  widestr() const -> const widechar*;

  };

  class directory::entry
  {
    friend class directory;

  protected:
    entry( const string& _path, const string& _name, unsigned _attr );

  public:     // initialization
    entry();
    entry( const entry& );

    operator bool () const  {  return defined();  }
    bool   defined() const  {  return w_name != nullptr;  }

  public:
    auto  path() const -> const string& {  return w_path;  }
    auto  name() const -> const string& {  return w_name;  }
    auto  attr() const -> unsigned      {  return u_attr;  }

  protected:
    string    w_path;
    string    w_name;
    unsigned  u_attr;

  };

}}

# endif   // !__mtc_dir_hpp__
