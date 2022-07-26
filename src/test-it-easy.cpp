# include "../test-it-easy.hpp"
# include <memory>

namespace TestItEasy {

  int   testsSucceeded = 0;
  int   testsFault = 0;

  struct  TestFunc
  {
    std::function<void()>     call;
    std::unique_ptr<TestFunc> next;
  };

  std::unique_ptr<TestFunc> list;

  int   Conclusion()
  {
    for ( auto p = list.get(); p != nullptr; p = p->next.get() )
      if ( p->call != nullptr ) p->call();

    if ( testsFault == 0 )  fputs( "\x1b[32mOK\x1b[0m:\n", stdout );
      else fputs( "\x1b[31mFAULT:\n", stdout );

    fprintf( stdout,
      "\t%u tests passed,\n"
      "\t%u tests failed\n", testsSucceeded, testsFault );

    return testsFault == 0 ? 0 : EFAULT;
  }

  void   RegisterTest( std::function<void()> fn )
  {
    auto  test = &list;

    while ( *test != nullptr )
      test = &(*test)->next;

    (*test = std::move( std::unique_ptr<TestFunc>( new TestFunc() ) ) )->call = fn;
  }

}
