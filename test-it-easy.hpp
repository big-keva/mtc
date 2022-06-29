# if !defined( __mtc_test_it_easy_hpp__ )
# define __mtc_test_it_easy_hpp__
# include "wcsstr.h"
# include "utf.hpp"
# include <string>
# include <cstdio>

namespace TestItEasy {

  namespace {
    int   testsSucceeded = 0;
    int   testsFault = 0;
  }

  template <class T, class P> class FCheck;
  template <class T>          class LValue;

  class Verify
  {
  public:
    const char*         f;
    int                 l;
    const std::string&  s;
    const std::string&  x;

  public:
    Verify( const char* fl, int ln, const std::string& st, const std::string& xp ):
      f( fl ),
      l( ln ),
      s( st ),
      x( xp )  {}

  public:
    template <class T>
    auto  operator <= ( const T& t ) -> LValue<T>;
  };

  template <class T>
  class LValue
  {
    const Verify& v;
    const T&      t;

  public:
    LValue( const Verify& vx, const T& va ): v( vx ), t( va )  {}

  public:
    void  operator ()();

    auto  strvalue() const -> std::string;

  public:     // operators
    template <class P>  auto  operator == ( const P& p ) -> FCheck<LValue<T>, P>;
    template <class P>  auto  operator != ( const P& p ) -> FCheck<LValue<T>, P>;
    template <class P>  auto  operator <  ( const P& p ) -> FCheck<LValue<T>, P>;
    template <class P>  auto  operator <= ( const P& p ) -> FCheck<LValue<T>, P>;
    template <class P>  auto  operator >  ( const P& p ) -> FCheck<LValue<T>, P>;
    template <class P>  auto  operator >= ( const P& p ) -> FCheck<LValue<T>, P>;

  public:
    template <class P>  bool  eq( const P& p ) const  {  return t == p;  }
    template <class P>  bool  ne( const P& p ) const  {  return t != p;  }
    template <class P>  bool  le( const P& p ) const  {  return t <= p;  }
    template <class P>  bool  lt( const P& p ) const  {  return t < p;  }
    template <class P>  bool  gt( const P& p ) const  {  return t > p;  }
    template <class P>  bool  ge( const P& p ) const  {  return t >= p;  }

  protected:
    struct  Dummy
    {
      template <class A>
      static  auto  to_string( const A& ) -> std::string  {  return "{?}";  }
    };
    struct  bySTD
    {
      template <class A>
      static  auto  to_string( const A& a ) -> std::string  {  return std::to_string( a );  }
    };
  protected:
    template <class A>
    auto  to_str( const A& a ) const -> std::string
    {
      using stringize = typename std::conditional<
        std::is_fundamental<A>::value, bySTD, Dummy>::type;

      return stringize::to_string( a );
    }
    auto  to_str( const mtc::widestr& s ) const -> std::string
      {  return mtc::utf8::encode( s );  }
    auto  to_str( const std::string& s ) const -> std::string
      {  return '\"' + s + '\"';  }
    auto  to_str( const char* s ) const -> std::string
      {  return s != nullptr ? '\"' + std::string( s ) + '\"' : "(const char*)nullptr";  }
    auto  to_str( std::nullptr_t ) const -> std::string
      {  return "nullptr";  }
  };

  template <class T, class P>
  class FCheck
  {
    typedef bool (FCheck::*Fn)() const;

  public:
    FCheck( const Verify& vx, Fn fn, const char* op, T&& v1, const P& v2 ):
      v( vx ),
      f( fn ),
      o( op ),
      t( std::move( v1 ) ),
      p( v2 ) {}

  public:
    void  operator ()();

  public:
    bool  eq() const  {  return t.eq( p );  }
    bool  ne() const  {  return t.ne( p );  }
    bool  le() const  {  return t.le( p );  }
    bool  lt() const  {  return t.lt( p );  }
    bool  gt() const  {  return t.gt( p );  }
    bool  ge() const  {  return t.ge( p );  }

  protected:
    const Verify& v;
    Fn            f;
    const char*   o;
    T             t;
    const P&      p;

  };

  // LValue implementation

  template <class T>
  void  LValue<T>::operator ()()
  {
    fprintf( stdout, "\x1b[34m%s:%d\x1b[0m: ", v.f, v.l );

    if ( t )
    {
      ++testsSucceeded;

      fprintf( stdout, "\x1b[32m" "OK" "\x1b[0m%s%s\n", v.s.empty() ? "" : ", ", v.s.c_str() );
    }
      else
    {
      ++testsFault;

      fprintf( stdout, "\x1b[31m" "FAULT" "\x1b[0m\n" );
      fprintf( stdout, "\texpression: %s\n", v.x.c_str() );
    }
  }

  template <class T>
  auto  LValue<T>::strvalue() const -> std::string
  {
    return to_str( t );
  }

  template <class T> template <class P>
  auto  LValue<T>::operator == ( const P& p ) -> FCheck<LValue<T>, P>
  {  return FCheck<LValue<T>, P>( v, &FCheck<LValue<T>, P>::eq, "==", std::move( *this ), p );  }

  template <class T> template <class P>
  auto  LValue<T>::operator != ( const P& p ) -> FCheck<LValue<T>, P>
  {  return FCheck<LValue<T>, P>( v, &FCheck<LValue<T>, P>::ne, "!=", std::move( *this ), p );  }

  template <class T> template <class P>
  auto  LValue<T>::operator < ( const P& p ) -> FCheck<LValue<T>, P>
  {  return FCheck<LValue<T>, P>( v, &FCheck<LValue<T>, P>::lt, "<", std::move( *this ), p );  }

  template <class T> template <class P>
  auto  LValue<T>::operator <= ( const P& p ) -> FCheck<LValue<T>, P>
  {  return FCheck<LValue<T>, P>( v, &FCheck<LValue<T>, P>::le, "<=", std::move( *this ), p );  }

  template <class T> template <class P>
  auto  LValue<T>::operator >= ( const P& p ) -> FCheck<LValue<T>, P>
  {  return FCheck<LValue<T>, P>( v, &FCheck<LValue<T>, P>::ge, ">=", std::move( *this ), p );  }

  template <class T> template <class P>
  auto  LValue<T>::operator > ( const P& p ) -> FCheck<LValue<T>, P>
  {  return FCheck<LValue<T>, P>( v, &FCheck<LValue<T>, P>::gt, ">", std::move( *this ), p );  }

  // FCheck implementation

  template <class T, class P>
  void  FCheck<T, P>::operator ()()
  {
    fprintf( stdout, "\x1b[34m%s:%d\x1b[0m: ", v.f, v.l );

    if ( f == nullptr )
      throw std::logic_error( "invalid check operator" );

    if ( (this->*f)() )
    {
      ++testsSucceeded;

      fprintf( stdout, "\x1b[32m" "OK" "\x1b[0m%s%s\n", v.s.empty() ? "" : ", ", v.s.c_str() );
    }
      else
    {
      ++testsFault;

      fprintf( stdout, "\x1b[31m" "FAULT" "\x1b[0m\n" );
      fprintf( stdout, "\texpression: %s\n", v.x.c_str() );
      fprintf( stdout, "\tevaluation: %s %s %s\n", t.strvalue().c_str(), o, LValue<P>( v, p ).strvalue().c_str() );
    }
  }

  // Verify implementation

  template <class T>
  auto  Verify::operator <= ( const T& t ) -> LValue<T> {  return LValue<T>( *this, t );  }

  int   Conclusion()
  {
    if ( testsFault == 0 )  fputs( "\x1b[32mOK\x1b[0m:\n", stdout );
      else fputs( "\x1b[31mFAULT:\n", stdout );

    fprintf( stdout,
      "\t%u tests passed,\n"
      "\t%u tests failed\n", testsSucceeded, testsFault );

    return testsFault == 0 ? 0 : EFAULT;
  }

}

# define __TEST_IT_EASY_VERIFY_IMPL( X_FILE, X_LINE, X_STAT, XP_STR, ... ) \
  (TestItEasy::Verify( X_FILE, X_LINE, X_STAT, XP_STR ) <= __VA_ARGS__)()

# define VERIFY( ... ) \
  __TEST_IT_EASY_VERIFY_IMPL( __FILE__, __LINE__, "", #__VA_ARGS__, __VA_ARGS__)

# define VERIFY_STATEMENT( statement, ... ) \
  __TEST_IT_EASY_VERIFY_IMPL( __FILE__, __LINE__, statement, #__VA_ARGS__, __VA_ARGS__)

# define EXPECT_EXCEPTION( statement, expression, exception ) \
  try {                                                       \
    (expression);                                             \
    ++TestItEasy::testsFault;                                 \
    fprintf( stdout, "\x1b[34m%s:%d\x1b[0m: \x1b[31mFAULT\x1b[0m\n" \
      "\texpected %s\n", __FILE__, __LINE__, #exception );    \
  }                                                           \
  catch ( const exception& ) {                                \
    ++TestItEasy::testsSucceeded;                             \
    fprintf( stdout, "\x1b[34m%s:%d\x1b[0m: \x1b[32m" "OK" "\x1b[0m, %s\n", __FILE__, __LINE__, (statement) );  \
  }

# define EXPECT_NOTHROW( statement, expression )  \
  try {                                           \
    (expression);                                 \
    ++TestItEasy::testsSucceeded;                 \
    fprintf( stdout, "\x1b[34m%s:%d\x1b[0m: \x1b[32m" "OK" "\x1b[0m, %s\n", __FILE__, __LINE__, (statement) );  \
  }                                               \
  catch ( ... ) {                                 \
    ++TestItEasy::testsFault;                     \
    fprintf( stdout, "\x1b[34m%s:%d\x1b[0m: \x1b[31mFAULT\x1b[0m\n", __FILE__, __LINE__ ); \
  }

# define SECTION( description )

# endif // !__mtc_test_it_easy_hpp__
