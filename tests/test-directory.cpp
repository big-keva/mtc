# include "../dir.hpp"
# include <cstdio>

int   main()
{
  auto  dir = mtc::fs::directory::open( "../", mtc::fs::directory::attr_any );

  for ( auto ent = dir.get(); ent; ent = dir.get() )
    fprintf( stdout, "%s%s\n", ent.path().charstr(), ent.name().charstr() );
  {
  }
  return 0;
}
