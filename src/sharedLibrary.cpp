# include "../sharedLibrary.hpp"
# include <dlfcn.h>

namespace mtc {

# if defined( _WIN32 )
# else
  void  FreeHandle( SharedLibrary::ModuleHandle p ) {  dlclose( p );  }
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
        FreeHandle( module );
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

    if ( (func_p = dlsym( module->module, name )) == nullptr )
      throw error( dlerror() );

    return func_p;
  }

  auto  SharedLibrary::Find( const char* name, const disable_exceptions_t& ) const -> void*
  {
    return module != nullptr && module->module != nullptr ? dlsym( module, name ) : nullptr;
  }

  auto  SharedLibrary::Load( const char* path, const enable_exceptions_t& ) -> SharedLibrary
  {
    SharedLibrary output;
    auto          handle = dlopen( path, RTLD_NOW );

    if ( handle == nullptr )
      throw SharedLibrary::error( dlerror() );

    return output.module = new impl( handle ), output;
  }

  auto  SharedLibrary::Load( const char* path, const disable_exceptions_t& ) -> SharedLibrary
  {
    SharedLibrary output;
    auto          handle = dlopen( path, RTLD_NOW );

    if ( handle != nullptr )
      output.module = new impl( handle );

    return output;
  }

  auto  SharedLibrary::native_handle() const -> ModuleHandle
  {
    return module != nullptr ? module->module : nullptr;
  }

}
