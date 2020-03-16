# include "../utf.hpp"

using namespace mtc;

void  test_encode_decode_char()
{
  char      buf[0x100];
  widechar  wcs[0x100];
  uint32_t  ucs[0x100];

// test 'encode-decode'
  assert( utf8::encode( buf, 0x10, 0x414 ) == 2 );
    assert( buf[0] == (char)208 );
    assert( buf[1] == (char)148 );

    assert( utf::encode( utf16::out( wcs, 0x100 ), utf8::in( buf, 2 ) ) == 1 );
      assert( wcs[0] == 0x414 );
    assert( utf::encode( utf32::out( ucs, 0x100 ), utf8::in( buf, 2 ) ) == 1 );
      assert( ucs[0] == 0x414 );

  assert( utf8::encode( buf, 0x10, 0x11414 ) == 4 );
     assert( buf[0] == (char)0xf0 );
     assert( buf[1] == (char)0x91 );
     assert( buf[2] == (char)0x90 );
     assert( buf[3] == (char)0x94 );

// test 'encode-decode' reversibility
  for ( auto i = 0; i != 0x4ffff; ++i )
  {
    auto  enc = utf8::encode( buf, 0x10, i );
      assert( enc != 0 );

    auto  d16 = utf::encode( utf16::out( wcs, 0x100 ), utf8::in( buf, enc ) );
      assert( d16 == (i <= 0xffff ? 1 : 2) );
      assert( d16 != 1 || wcs[0] == i );

    auto  d32 = utf::encode( utf32::out( ucs, 0x100 ), utf8::in( buf, enc ) );
      assert( d32 == 1 );
      assert( ucs[0] == i );
  }
}

void  test_encode_decode_strs()
{
  auto  s16 = utf::encode( utf16::out(), utf8::in( "погода" ) );
    assert( s16 == L"������" );

  auto  utf = utf::encode( utf8::out(), utf16::in( s16 ) );

  auto  l08 = utf::strlen( utf8(), utf8::in( "погода" ) );
  auto  l16 = utf::strlen( utf16(), utf16::in( s16 ) );
  auto  l32 = utf::strlen( utf32(), utf16::in( s16 ) );
  /*
  template <class encoding> static  auto  decode( const widechar* pch, size_t = (size_t)-1 ) -> typename encoding::string_t;
  template <class encoding> static  auto  decode( const uint32_t* pch, size_t = (size_t)-1 ) -> typename encoding::string_t;

  template <class encoding> static  auto  decode( const std::basic_string<char>&     ) -> typename encoding::string_t;
  template <class encoding> static  auto  decode( const std::basic_string<widechar>& ) -> typename encoding::string_t;
  template <class encoding> static  auto  decode( const std::basic_string<uint32_t>& ) -> typename encoding::string_t;

  template <> static  auto  decode<utf16>( const char*     pch, size_t cch ) -> typename utf16::string_t;
  template <> static  auto  decode<utf16>( const widechar* pch, size_t cch ) -> typename utf16::string_t;
  template <> static  auto  decode<utf16>( const uint32_t* pch, size_t cch ) -> typename utf16::string_t;
  template <> static  auto  decode<utf32>( const char*     pch, size_t cch ) -> typename utf32::string_t;
  template <> static  auto  decode<utf32>( const widechar* pch, size_t cch ) -> typename utf32::string_t;
  template <> static  auto  decode<utf32>( const uint32_t* pch, size_t cch ) -> typename utf32::string_t;

  template <> static  auto  decode<utf16>( const std::basic_string<char>&     src ) -> typename utf16::string_t;
  template <> static  auto  decode<utf16>( const std::basic_string<widechar>& src ) -> typename utf16::string_t;
  template <> static  auto  decode<utf16>( const std::basic_string<uint32_t>& src ) -> typename utf16::string_t;
  template <> static  auto  decode<utf32>( const std::basic_string<char>&     src ) -> typename utf32::string_t;
  template <> static  auto  decode<utf32>( const std::basic_string<widechar>& src ) -> typename utf32::string_t;
  template <> static  auto  decode<utf32>( const std::basic_string<uint32_t>& src ) -> typename utf32::string_t;
  */
}

int   main()
{
  test_encode_decode_char();
  test_encode_decode_strs();

  return 0;
}
