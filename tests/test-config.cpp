# include "../config.h"
# include "../test-it-easy.hpp"
# include <cstdio>

using namespace mtc;

enum suffixes: uint64_t
{
  Kb = 1024,
  Mb = Kb * Kb,
  Gb = Mb * Kb
};

constexpr static std::initializer_list<std::pair<const char*, uint32_t>> sizeSuffix{
  { "[Kk]", Kb }, { "[Kk][Bb]", Kb },
  { "[Mm]", Mb }, { "[Mm][Bb]", Mb },
  { "[Gg]", Gb }, { "[Gg][Bb]", Mb } };

TestItEasy::RegisterFunc  testConfig( []()
  {
    TEST_CASE( "mtc/config" )
    {
      auto  cfg = config();

      SECTION( "config may be initialized with json" )
      {
        REQUIRE_NOTHROW( cfg = config::Load(
          "{"
            "\"value_with_suffix\": \"77Mb\""
          "}" ) );
        REQUIRE( cfg.size() != 0 );
      }
      SECTION( "keys may be accessed as strings" )
      {
        REQUIRE( cfg.get_charstr( "value_with_suffix" ) == "77Mb" );
        REQUIRE( cfg.get_int32( "value_with_suffix", -1 ) == -1 );
      }
      SECTION( "keys may be parsed with suffix lists" )
      {
        REQUIRE( cfg.get_int32( "value_with_suffix", -1, { { "Mb", 1024 * 1024 } } ) == 77 * 1024 * 1024 );
        REQUIRE( cfg.get_int32( "value_with_suffix", -1, { { "[Mm][Bb]", 1024 * 1024 } } ) == 77 * 1024 * 1024 );
        REQUIRE( cfg.get_uint64( "value_with_suffix", 512 * Mb, sizeSuffix ) == 77 * Mb );
      }

    }

  } );

int main()
{
  return TestItEasy::Conclusion();
}
