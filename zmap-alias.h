/*

The MIT License (MIT)

Copyright (c) 2020 Андрей Коваленко aka Keva
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
# if !defined( __zmap_alias_h__ )
# define __zmap_alias_h__
# include "zmap.h"

namespace mtc {

  class alias_base: public mtc::zmap
  {
    using mtc::zmap::zmap;

  protected:
    template <class T>
    class array_view
    {
      const T*  ptr;
      size_t    len;
    public:
      array_view() noexcept: ptr( nullptr ), len( 0 ) {}
      array_view( const T* p, size_t l ) noexcept: ptr( p ), len( l ) {}
      array_view( const array_view& v ) noexcept: ptr( v.ptr ), len( v.len ) {}
      array_view( const std::vector<T>& v ): array_view( v.data(), v.size() ) {}
      const T&  operator[]( size_t i ) const noexcept {  return ptr[i];  }
      const T&  at( size_t i ) const noexcept {  return ptr[i];  }
      size_t    size() const noexcept {  return len;  }
      auto  begin() noexcept -> const T* {  return ptr;  }
      auto  end() noexcept -> const T* {  return ptr + len;  }
      auto  front() const -> const T&  {  return *ptr;  }
      auto  back() const -> const T&  {  return ptr[len - 1];  }
    };

    # define derive_get( type )                                                     \
    static auto get_value( const type##_t*, const zval* v ) -> const type##_t*      \
      {  return v != nullptr ? v->get_##type() : nullptr;  }                        \
    static auto get_value( const type##_t*, zval* v ) -> type##_t*                  \
      {  return v != nullptr ? v->get_##type() : nullptr;  }                        \
    auto set_value( const type##_t*, const zmap::key& k ) -> type##_t*              \
      {  return set_##type( k );  }

    derive_get( char )
    derive_get( byte )
    derive_get( int16 )
    derive_get( int32 )
    derive_get( int64 )
    derive_get( word16 )
    derive_get( word32 )
    derive_get( word64 )
    derive_get( float )
    derive_get( double )
    derive_get( charstr )
    derive_get( widestr )
    derive_get( zmap )
    derive_get( uuid )

    derive_get( array_char )
    derive_get( array_byte )
    derive_get( array_int16 )
    derive_get( array_int32 )
    derive_get( array_int64 )
    derive_get( array_word16 )
    derive_get( array_word32 )
    derive_get( array_word64 )
    derive_get( array_float )
    derive_get( array_double )
    derive_get( array_charstr )
    derive_get( array_widestr )
    derive_get( array_zmap )
    derive_get( array_zval )
    derive_get( array_uuid )

    # undef derive_get

    template <class T>
    auto  get_value( const T*, const mtc::zmap::key& key ) const -> T
    {
      auto  val = get_value( (const T*)nullptr, get( key ) );
      return val != nullptr ? *val : T();
    }
    template <class T>
    auto  get_value( const T*, const mtc::zmap::key& key ) -> T&
    {
      auto  val = get_value( (const T*)nullptr, get( key ) );
      return val != nullptr ? *(T*)val : *(T*)set_value( (const T*)nullptr, key );
    }

    template <class T>
    auto  get_array( const T*, const mtc::zmap::key& key ) const -> array_view<T>
    {
      using element_type = typename std::conditional<std::is_base_of<mtc::zmap, T>::value, mtc::zmap, T>::type;
      using created_type = std::vector<T>;

      auto  val = (created_type*)get_value( (const std::vector<element_type>*)nullptr, get( key ) );
      return val != nullptr ? *val : array_view<T>();
    }
    template <class T>
    auto  get_array( const T*, const mtc::zmap::key& key ) -> std::vector<T>&
    {
      using element_type = typename std::conditional<std::is_base_of<mtc::zmap, T>::value, mtc::zmap, T>::type;
      using created_type = std::vector<T>;

      auto  val = (created_type*)get_value( (const std::vector<element_type>*)nullptr, get( key ) );
      return val != nullptr ? *val : *(created_type*)set_value( (const std::vector<element_type>*)nullptr, key );
    }

  public:
    alias_base() = default;
    alias_base( const mtc::zmap& z ): mtc::zmap( z ) {}
    alias_base( const mtc::zmap* z ): mtc::zmap() {  if ( z != nullptr )  *this = *z;  }
    alias_base( const alias_base& a ): mtc::zmap( a ) {}
    auto  operator = ( const alias_base& z ) -> alias_base& {  return mtc::zmap::operator=( z ), *this;  }
    auto  operator = ( const mtc::zmap& z ) -> alias_base& {  return mtc::zmap::operator=( z ), *this;  }

  };

}

# define  zmap_view( name ) \
  class name: public mtc::alias_base {  using alias_base::alias_base;

# define  zmap_value( type, name, key ) public:                                     \
  auto  name() const -> type  {  return get_value( (const type*)nullptr, key );  }  \
  auto  name() -> type& {  return get_value( (const type*)nullptr, key );  }

# define  zmap_array( type, name, key ) public:                                                 \
  auto  name() const -> array_view<type>  {  return get_array( (const type*)nullptr, key );  }  \
  auto  name() -> std::vector<type>&  {  return get_array( (const type*)nullptr, key );  }

# define  zmap_end  };

# endif // !__zmap_alias_h__
