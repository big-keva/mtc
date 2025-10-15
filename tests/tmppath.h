# if !defined( __DelphiX_tests_tmppath_h__ )
# define __DelphiX_tests_tmppath_h__
# include <string>

auto  GetTmpPath() -> std::string;
auto  GetTmpName( const char* tmpl = "mtcXXXXXX" ) -> std::string;

# endif   // !__DelphiX_tests_tmppath_h__
