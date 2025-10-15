# if defined( _MSC_VER )
#   define EXPORT __declspec(dllexport)
# else
#   define EXPORT
# endif

extern "C"  EXPORT
auto  ExportedStringFunc( const char* src ) -> const char*
{
  return src;
}
