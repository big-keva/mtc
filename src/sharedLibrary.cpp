# include "../sharedLibrary.hpp"
# if defined( _WIN32 ) || defined( _WIN64 )
#   include <libloaderapi.h>
# else
#   include <dlfcn.h>
# endif

namespace mtc {

# if defined( _WIN32 ) || defined( _WIN64 )
#   define PlatformLoadModule( module )        (void*)LoadLibrary( (LPCSTR)(module) )
#   define PlatformFreeModule( module )               FreeLibrary( (HMODULE)(module) )
#   define PlatformFindMethod( module, fnname ) (void*)GetProcAddress( (HMODULE)(module), (LPCSTR)(fnname) )
#   define PlatformFnNotFound "method not found"
#   define PlatformFnLoadFail "could not load library"
# else
#   define PlatformLoadModule( module ) dlopen( (module), RTLD_NOW )
#   define PlatformFreeModule( module ) dlclose( (module) )
#   define PlatformFindMethod( module, fnname ) (void*)dlsym( (module), (fnname) )
#   define PlatformFnNotFound dlerror()
#   define PlatformFnLoadFail dlerror()
# endif

  struct SharedLibrary::impl
  {
    long          rcount;
    ModuleHandle  module;

    impl( ModuleHandle mh ): rcount( 1 ), module( mh )
    {
    }
   ~impl()
    {
      if ( module != nullptr )
        PlatformFreeModule( module );
    }
  };

  SharedLibrary::SharedLibrary( SharedLibrary&& lib ): module( lib.module )
  {
    lib.module = nullptr;
  }

  SharedLibrary::SharedLibrary( const SharedLibrary& lib ): module( lib.module )
  {
    if ( module != nullptr )
      ++module->rcount;
  }

  SharedLibrary& SharedLibrary::operator=( SharedLibrary&& lib )
  {
    if ( module != nullptr && --module->rcount == 0 )
      delete module;
    module = lib.module;
      lib.module = nullptr;
    return *this;
  }

  SharedLibrary& SharedLibrary::operator=( const SharedLibrary& lib )
  {
    if ( module != nullptr && --module->rcount == 0 )
      delete module;
    if ( (module = lib.module) != nullptr )
      ++module->rcount;
    return *this;
  }

  SharedLibrary::~SharedLibrary()
  {
    if ( module != nullptr && --module->rcount == 0 )
      delete module;
  }

  bool  SharedLibrary::operator == ( const SharedLibrary& lib ) const
  {
    return module == lib.module || (module != nullptr && lib.module != nullptr
      && module->module == lib.module->module);
  }

  bool  SharedLibrary::operator == ( nullptr_t ) const
  {
    return module == nullptr || module->module == nullptr;
  }

  auto  SharedLibrary::Find( const char* name, const enable_exceptions_t& ) const -> void*
  {
    void*  func_p;

    if ( module == nullptr )
      throw error( "library was not loaded" );

    if ( (func_p = PlatformFindMethod( module->module, name )) == nullptr )
      throw error( PlatformFnNotFound );

    return func_p;
  }

  auto  SharedLibrary::Find( const char* name, const disable_exceptions_t& ) const -> void*
  {
    return module != nullptr && module->module != nullptr ? PlatformFindMethod( module, name ) : nullptr;
  }

  auto  SharedLibrary::Load( const char* path, const enable_exceptions_t& ) -> SharedLibrary
  {
    SharedLibrary output;
    auto          handle = PlatformLoadModule( path );

    if ( handle == nullptr )
      throw SharedLibrary::error( PlatformFnLoadFail );

    return output.module = new impl( handle ), output;
  }

  auto  SharedLibrary::Load( const char* path, const disable_exceptions_t& ) -> SharedLibrary
  {
    SharedLibrary output;
    auto          handle = PlatformLoadModule( path );

    if ( handle != nullptr )
      output.module = new impl( handle );

    return output;
  }

  auto  SharedLibrary::native_handle() const -> ModuleHandle
  {
    return module != nullptr ? module->module : nullptr;
  }

}
