# include "../test-it-easy.hpp"
# include "../prefixtree.h"

using namespace mtc;

TestItEasy::RegisterFunc  testPrefixTree( []()
  {
    TEST_CASE( "mtc/prefixTree" )
    {
      prefix_tree<int>  pt;

      SECTION( "access to values" )
      {
        SECTION( "pt.at( non-existing-key ) throws exception" )
        {  REQUIRE_EXCEPTION( pt.at( "non-existing-key" ), std::range_error );   }
        SECTION( "pt[non-existing-key] expands tree" )
        {  REQUIRE_NOTHROW( pt["non-existing-key"] );   }
      }
    }
  } );
