# include "../zmap.h"
# include "../z_js.h"
# include "../utf.hpp"
# include <mtc/serialize.h>

int main()
{
  mtc::zmap zm;

  /*
  mtc::json::Parse(
    "{"
      "\"i32\": 32,"
      "\"flo\": 32,"
      "\"dbl\": 32,"
      "\"arr\": [1, 2, 3],"
      "\"map\": {"
        "\"int\": 5,"
        "\"str\": \"charstr\","
        "\"wcs\": \"погода\""
      "},"
      "\"users\": ["
        "{ \"name\": \"michael\", \"age\": 32 },"
        "{ \"name\": \"jabrael\", \"age\": 33 }"
      "]"
    "}", zm,
    {
      { "i32", mtc::zval::z_int32   },
      { "flo", mtc::zval::z_float   },
      { "dbl", "double"             },
      { "arr", "array_double"       },
      { "map",
        mtc::zmap( {
          { "int", "double" },
          { "str", "charstr" },
          { "wcs", "charstr" }
        } )
      },
      { "users",
        mtc::zmap( {
          { { "name", "charstr" },
            { "age",  "double"  } }
        } )
      }
    } );

  mtc::json::Print( stdout, zm, mtc::json::print::decorated() );
  */

  zm["char"] = (char)'c';
  zm["byte"] = 129;
  zm["int16"] = (int16_t)1290;
  zm["word16"] = (uint16_t)1290;
  zm["int32"] = (int32_t)1290;
  zm["word32"] = (uint32_t)1290;
  zm["int64"] = (int64_t)1290;
  zm["word64"] = (uint64_t)1290;
  zm["float"] = (float)1290;
  zm["double"] = (double)1290;
  zm["charstr"] = "charstr";
  zm["widestr"] = L"widestr";
  zm["charstr2"] = std::move( mtc::charstr( "charstr" ) );
  zm["widestr2"] = std::move( mtc::utf16::encode( "widestr" ) );

  zm["zmap"] = mtc::zmap{ { "a", "b" } };
  zm["zmap2"] = std::move( mtc::zmap{ { "q", "w" } } );

  mtc::json::Print( stdout, zm, mtc::json::print::decorated() );

  return 0;
}
