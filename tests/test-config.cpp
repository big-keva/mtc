# include "../config.h"
# include <cstdio>

int main()
{
  auto  config = mtc::config::Open( "examples/test-config.json" );
  auto  file_1 = config.get_path( "file_1" );
    fprintf( stdout, "@1=%s\n", file_1.c_str() );
  auto  file_2 = config.get_path( "file_2" );
    fprintf( stdout, "@2=%s\n", file_2.c_str() );
  auto  file_3 = config.get_path( "file_3" );
    fprintf( stdout, "@3=%s\n", file_3.c_str() );

  return 0;
}
