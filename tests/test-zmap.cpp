# include "../zmap.h"
# include "../z_js.h"
# include <mtc/serialize.h>

int main()
{
  mtc::zmap zm;

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

  return 0;
}
