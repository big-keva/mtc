# if !defined( __mtc_tests_tmppath_h__ )
# define __mtc_tests_tmppath_h__
# include <string>

auto  GetTmpPath() -> std::string;
auto  GetTmpName( const char* tmpl = "mtcXXXXXX" ) -> std::string;

# endif   // !__mtc_tests_tmppath_h__
