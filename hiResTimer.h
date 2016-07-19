# if !defined( __hiResTimer_h__ )
# define  __hiResTimer_h__
# include <time.h>
# include <sys/timeb.h>

# if defined( _MSC_VER )
#   include <windows.h>

#	define timeb _timeb
#	define ftime _ftime
# endif

namespace mtc
{

	inline double	GetMilliTime()
	{
	  struct timeb timebuffer;
	  ftime( &timebuffer );

	  return (double)(timebuffer.time + timebuffer.millitm / 1000.0);
	}

}

# endif // __hiResTimer_h__
