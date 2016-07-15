# if !defined( __xresult_h__ )
# define __xresult_h__

namespace mtc
{
  template <class resultType>
  class xresult
  {
    resultType  result;
    int         nerror;

  public:     // construction
    xresult( const resultType& r = 0 ): result( r ), nerror( 0 )
      {}
    xresult( int e ): nerror( e ) 
      {}

  public:     // work
    bool  OK() const              {  return nerror == 0;  }
    bool  FAULT() const           {  return nerror != 0;  }

    resultType& getvalue()        {  return result; }
    int         geterror() const  {  return nerror; }
  };

}

# endif  // __xresult_h__
