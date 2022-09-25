# include "../test-it-easy.hpp"
# include "../utf.hpp"

using namespace mtc;

TestItEasy::RegisterFunc  testUtf( []
  {
    TEST_CASE( "mtc/utf" )
    {
      SECTION( "utf8/16/32 encode/decode character" )
      {
        char      buf[0x100];
        widechar  wcs[0x100];
        uint32_t  ucs[0x100];

      // test 'encode-decode'
        REQUIRE( utf8::encode( buf, 0x10, 0x414 ) == 2 );
          REQUIRE( buf[0] == (char)208 );
          REQUIRE( buf[1] == (char)148 );

        REQUIRE( utf::encode( utf16::out( wcs, 0x100 ), utf8::in( buf, 2 ) ) == 1 );
          REQUIRE( wcs[0] == 0x414 );

        REQUIRE( utf::encode( utf32::out( ucs, 0x100 ), utf8::in( buf, 2 ) ) == 1 );
          REQUIRE( ucs[0] == 0x414 );

        REQUIRE( utf8::encode( buf, 0x10, 0x11414 ) == 4 );
          REQUIRE( buf[0] == (char)0xf0 );
          REQUIRE( buf[1] == (char)0x91 );
          REQUIRE( buf[2] == (char)0x90 );
          REQUIRE( buf[3] == (char)0x94 );

      // test 'encode-decode' reversibility
        for ( auto i = 0; i != 0x4ffff; ++i )
        {
          auto  enc = utf8::encode( buf, 0x10, i );
            REQUIRE( enc != 0 );

          auto  d16 = utf::encode( utf16::out( wcs, 0x100 ), utf8::in( buf, enc ) );
            REQUIRE( d16 == (i <= 0xffff ? 1 : 2) );
            REQUIRE( (d16 != 1 || wcs[0] == i) );

          auto  d32 = utf::encode( utf32::out( ucs, 0x100 ), utf8::in( buf, enc ) );
            REQUIRE( d32 == 1 );
            REQUIRE( ucs[0] == i );
        }
      }
      SECTION( "endoce/decode/strlen" )
      {
        auto  s16 = utf::encode( utf16::out(), utf8::in( "погода" ) );
          REQUIRE( s16.length() == 6 );

        auto  utf = utf::encode( utf8::out(), utf16::in( s16 ) );
          REQUIRE( utf::buflen( utf8(), utf16::in( s16 ) ) == 12 );
          REQUIRE( utf::buflen( utf8(), utf8::in( "погода" ) ) == 12 );
          REQUIRE( utf::buflen( utf16(), utf16::in( s16 ) ) == 6 );
          REQUIRE( utf::buflen( utf32(), utf16::in( s16 ) ) == 6 );

          REQUIRE( utf::strlen( "погода" ) == 6 );
      }
    }
  } );
