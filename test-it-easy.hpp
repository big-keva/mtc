# if !defined( __mtc_test_it_easy_hpp__ )
# define __mtc_test_it_easy_hpp__
# include "wcsstr.h"
# include "utf.hpp"
# include <functional>
# include <vector>
# include <string>
# include <cstdio>

#pragma GCC diagnostic ignored "-Wparentheses"

const int   TestItEasyShiftSpace = 0;

namespace TestItEasy {

  extern  int   testsSucceeded;
  extern  int   testsFault;

  template <class T, class P> class FCheck;
  template <class T>          class LValue;

  inline  std::string spaces( int n ) {  return std::string( n, ' ' );  }

  class Verify
  {
  public:
    const char*         file;
    int                 line;
    const std::string&  expr;

  public:
    Verify( const char* fl, int ln, const std::string& xp ):
      file( fl ),
      line( ln ),
      expr( xp )  {}

  public:
    template <class T>
    auto  operator <= ( const T& t ) -> LValue<T>;
  };

  template <class T>
  class LValue
  {
    const Verify& v;
    const T&      t;

    template <class T1, class T2>
    static  bool  equals( const T1& _1, const T2& _2 )  {  return _1 == _2;  }

    static  bool  equals( int _i, unsigned _j )  {  return _i >= 0 ? unsigned(_i) == _j : false;  }
    static  bool  equals( unsigned _i, int _j )  {  return equals( _j, _i );  }

    static  bool  equals( int _i, long unsigned _j )  {  return _i >= 0 ? unsigned(_i) == _j : false;  }
    static  bool  equals( long unsigned _i, int _j )  {  return equals( _j, _i );  }

  public:
    LValue( const Verify& vx, const T& va ): v( vx ), t( va )  {}

  public:
    bool  operator ()( int shift );

    auto  strvalue() const -> std::string;

  public:     // operators
    template <class P>  auto  operator == ( const P& p ) -> FCheck<LValue<T>, P>;
    template <class P>  auto  operator != ( const P& p ) -> FCheck<LValue<T>, P>;
    template <class P>  auto  operator <  ( const P& p ) -> FCheck<LValue<T>, P>;
    template <class P>  auto  operator <= ( const P& p ) -> FCheck<LValue<T>, P>;
    template <class P>  auto  operator >  ( const P& p ) -> FCheck<LValue<T>, P>;
    template <class P>  auto  operator >= ( const P& p ) -> FCheck<LValue<T>, P>;

  public:
    template <class P>  bool  eq( const P& p ) const  {  return equals( t, p );  }
    template <class P>  bool  ne( const P& p ) const  {  return !equals( t, p );  }
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
    template <class C>
    auto  to_str( const std::vector<C>& a ) const -> std::string
      {
        auto  out = std::string( "[" );

        for ( auto& x: a )
          if ( out.length() == 1 )  out += to_str( x );
            else out += ", " + to_str( x );

        return out + ']';
      }
    template <class A, class B>
    auto  to_str( const std::pair<A, B>& p ) const -> std::string
      {  return "{ " + to_str( p.first ) + ", " + to_str( p.second ) + " }";  }
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
    bool  operator ()( int shift );

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
  bool  LValue<T>::operator ()( int shift )
  {
    if ( !(t) )
    {
      ++testsFault;

      fprintf( stdout, "%s\x1b[34m%s:%d\x1b[0m: \x1b[31m" "FAULT" "\x1b[0m\n", spaces( shift ).c_str(),
        v.file,
        v.line );

      fprintf( stdout, "%s\texpression: %s\n", spaces( shift ).c_str(),
        v.expr.c_str() );

      return false;
    }
    return ++testsSucceeded, true;
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
  bool  FCheck<T, P>::operator ()( int shift )
  {
    if ( f == nullptr )
      throw std::logic_error( "invalid check operator" );

    if ( !(this->*f)() )
    {
      ++testsFault;

      fprintf( stdout, "%s\x1b[34m%s:%d\x1b[0m: \x1b[31m" "FAULT" "\x1b[0m\n", spaces( shift ).c_str(),
        v.file,
        v.line );

      fprintf( stdout,
        "%s\texpression: %s\n"
        "%s\tevaluation: %s %s %s\n",
          spaces( shift ).c_str(), v.expr.c_str(),
          spaces( shift ).c_str(), t.strvalue().c_str(), o, LValue<P>( v, p ).strvalue().c_str() );
      return false;
    }

    return ++testsSucceeded, true;
  }

  // Verify implementation

  template <class T>
  auto  Verify::operator <= ( const T& t ) -> LValue<T> {  return LValue<T>( *this, t );  }

  int   Conclusion();
  void  RegisterTest( std::function<void()> );

  struct RegisterFunc
  {
    RegisterFunc( std::function<void()> fn )  {  RegisterTest( fn );  }
  };

}

# define __TEST_IT_EASY_REQUIRE_IMPL( X_FILE, X_LINE, XP_STR, ... ) \
  ((TestItEasy::Verify( X_FILE, X_LINE, XP_STR ) <= __VA_ARGS__)( TestItEasyShiftSpace ) ? \
    true : succeeded = false)

# define REQUIRE( ... ) \
  __TEST_IT_EASY_REQUIRE_IMPL( __FILE__, __LINE__, #__VA_ARGS__, __VA_ARGS__ )

# define REQUIRE_EXCEPTION( expression, exception ) \
  try {                                             \
    (expression);                                   \
    ++TestItEasy::testsFault;                       \
    succeeded = false;                              \
    fprintf( stdout, "%s\x1b[34m%s:%d\x1b[0m: \x1b[31m" "FAULT" "\x1b[0m\n",          \
      TestItEasy::spaces( TestItEasyShiftSpace ).c_str(),                             \
      __FILE__,                                                                       \
      __LINE__ );                                                                     \
    fprintf( stdout,                                \
      "%s\texpression: %s,\n"                       \
      "%s\texpected expception: %s\n",              \
        TestItEasy::spaces( TestItEasyShiftSpace ).c_str(), #expression,      \
        TestItEasy::spaces( TestItEasyShiftSpace ).c_str(), #exception );     \
  }                                 \
  catch ( const exception& ) {      \
    ++TestItEasy::testsSucceeded;   \
  }                                 \
  catch ( ... ) {                   \
    ++TestItEasy::testsFault;                       \
    succeeded = false;                              \
    fprintf( stdout, "%s\x1b[34m%s:%d\x1b[0m: \x1b[31m" "FAULT" "\x1b[0m\n",          \
      TestItEasy::spaces( TestItEasyShiftSpace ).c_str(),                             \
      __FILE__,                                                                       \
      __LINE__ );                                                                     \
    fprintf( stdout,                                        \
      "%s\texpression: %s,\n"                               \
      "%s\texpected expception: %s, " "\x1b[31m" "got unexpected one" "\x1b[0m!\n",   \
        TestItEasy::spaces( TestItEasyShiftSpace ).c_str(), #expression,      \
        TestItEasy::spaces( TestItEasyShiftSpace ).c_str(), #exception );     \
  }

# define REQUIRE_NOTHROW( expression )              \
  try {                                             \
    (expression);                                   \
    ++TestItEasy::testsSucceeded;                   \
  }                                                 \
  catch ( ... ) {                                   \
    ++TestItEasy::testsFault;                       \
    succeeded = false;                              \
    fprintf( stdout, "%s\x1b[34m%s:%d\x1b[0m: \x1b[31m" "FAULT" "\x1b[0m\n",          \
      TestItEasy::spaces( TestItEasyShiftSpace ).c_str(),                             \
      __FILE__,                                                                       \
      __LINE__ );                                                                     \
    fprintf( stdout, "%s\texpression: %s\n", TestItEasy::spaces( TestItEasyShiftSpace ).c_str(), #expression );  \
  }

# define SECTION( description ) \
  fprintf( stdout, "%s%s\n", TestItEasy::spaces( TestItEasyShiftSpace ).c_str(), (description) ); \
  for( auto global = TestItEasyShiftSpace, TestItEasyShiftSpace = global + 2, nloops = 1, succeeded = 1; \
    nloops-- != 0; (void)succeeded, (void)TestItEasyShiftSpace )

# define TEST_CASE( description ) \
  fprintf( stdout, "%s[%s]\n", TestItEasy::spaces( TestItEasyShiftSpace ).c_str(), (description) ); \
  for( auto global = TestItEasyShiftSpace, TestItEasyShiftSpace = global + 2, nloops = 1, succeeded = 1; \
    nloops-- != 0; (void)succeeded )

# endif // !__mtc_test_it_easy_hpp__
