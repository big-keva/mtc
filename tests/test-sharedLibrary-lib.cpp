# include <string>

extern "C"
auto  ExportedStringFunc( const std::string& src ) -> std::string
{
  return "string '" + src + "' from library";
}
