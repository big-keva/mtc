# include "../serialize.h"
# include <initializer_list>
# include <cassert>

template <class A>
std::string to_string( const A& );
std::string to_string( const char* const& a ) {  return '"' + std::string( a ) + '"';  }
std::string to_string( const std::string& );

std::string type_name( const char* )  {  return "char";  }
std::string type_name( const unsigned char* )  {  return "unsigned char";  }
std::string type_name( const int16_t* )  {  return "int16_t";  }
std::string type_name( const int32_t* )  {  return "int32_t";  }
std::string type_name( const int64_t* )  {  return "int64_t";  }
std::string type_name( const uint16_t* )  {  return "uint16_t";  }
std::string type_name( const uint32_t* )  {  return "uint32_t";  }
std::string type_name( const uint64_t* )  {  return "uint64_t";  }
std::string type_name( const float* )  {  return "float";  }
std::string type_name( const double* )  {  return "double";  }
std::string type_name( const char* const* )  {  return "const char*";  }

template <class C>
std::string type_name( const std::basic_string<C>* )  {  return "std::basic_string<" + type_name( (C*)nullptr ) + ">";  }

template <class T1, class T2>
std::string type_name( const std::pair<T1, T2>* )  {  return "std::pair<"
  + type_name( (T1*)nullptr ) + ", "
  + type_name( (T2*)nullptr ) + ">";  }

template <size_t I>
struct  tuple_type
{
  template <class ... Types>
  static  auto  pref( const std::tuple<Types...>* ) -> std::string
  {
    return I == sizeof...(Types) ? "" : ", ";
  }
  template <class ... Types>
  static  auto  name( const std::tuple<Types...>* ) -> std::string
  {
    return pref( (const std::tuple<Types...>*)nullptr )
      + type_name( (typename std::tuple_element<sizeof...(Types) - I, std::tuple<Types...>>::type*)nullptr )
      + tuple_type<I - 1>::name( (const std::tuple<Types...>*)nullptr );
  }
  template <class ... Types>
  static  auto  text( const std::tuple<Types...>& t ) -> std::string
  {
    return pref( &t )
      + to_string( std::get<sizeof...(Types) - I>( t ) )
      + tuple_type<I - 1>::text( t );
  }
};

template <>
struct  tuple_type<0>
{
  template <class ... Types>
  static  auto  name( const std::tuple<Types...>* ) -> std::string  {  return "";  }
  template <class ... Types>
  static  auto  text( const std::tuple<Types...>& ) -> std::string  {  return "";  }
};

template <class ... Types>
std::string type_name( const std::tuple<Types...>* )  {  return "std::tuple<"
  + tuple_type<sizeof...(Types)>::name( (const std::tuple<Types...>*)nullptr ) + ">";  }

template <class T>
std::string type_name( const std::vector<T>* )  {  return "std::vector<"
  + type_name( (T*)nullptr ) + ">";  }

template <class T>
std::string type_name( const std::list<T>* )  {  return "std::list<"
  + type_name( (T*)nullptr ) + ">";  }

template <class K, class V>
std::string type_name( const std::map<K, V>* )  {  return "std::map<"
  + type_name( (typename std::map<K, V>::value_type*)nullptr ) + ">";  }

template <class A>
std::string to_string( const A& a ) {  return std::to_string( a );  }
std::string to_string( const std::string& s )  {  return '"' + s + '"';  }

template <class K, class V>
std::string to_string( const std::pair<K, V>& v )
{
  return "{" + to_string( v.first ) + ", " + to_string( v.second ) + "}";
}

template <class ... Types>
std::string to_string( const std::tuple<Types...>& v )
{
  return "{" + tuple_type<sizeof...(Types)>::text( v ) + "}";
}

template <class T>
std::string to_string( const std::vector<T>& v )
{
  auto  s = std::string();

  for ( auto& t: v )
    (s += (s.empty() ? "{" : ", ")) += to_string( t );

  return s + "}";
}

template <class T>
std::string to_string( const std::list<T>& v )
{
  auto  s = std::string();

  for ( auto& t: v )
    (s += (s.empty() ? "{" : ", ")) += to_string( t );

  return s + "}";
}

template <class K, class V>
std::string to_string( const std::map<K, V>& v )
{
  auto  s = std::string();

  for ( auto& t: v )
    (s += (s.empty() ? "{" : ", ")) += to_string( t );

  return s + "}";
}

template <class T>
std::vector<T>  make_vector( const std::initializer_list<T>& t )  {  return std::vector<T>( t.begin(), t.end() );  }

template <class T>
std::list<T>  make_list( const std::initializer_list<T>& t )  {  return std::list<T>( t.begin(), t.end() );  }

template <class K, class V>
std::map<K, V>  make_map( const std::initializer_list<std::pair<K, V>>& l )
{
  std::map<K, V>  map;

  for ( auto& kv: l )
    map.emplace( kv.first, kv.second );

  return map;
}

template <class ... Pairs>
bool  TestGetBufLen( FILE* output, const Pairs&... pairs );
template <class ... Types>
bool  TestSerializeDeserialize( FILE*, const Types&... values );

template <class T1, class T2, class ... Pairs>
bool  TestGetBufLen( FILE* output, const std::pair<T1, T2>& p, const Pairs&... pairs )
{
  auto  buflen = ::GetBufLen( p.first );
  bool  result = true;

  if ( buflen != (size_t)p.second )
  {
    fprintf( output, "'%s' value of type '%s' size mismatch: %s vs %s\n",
      to_string( p.first ).c_str(), type_name( &p.first ).c_str(),
      to_string( buflen ).c_str(),
      to_string( p.second ).c_str() );
    result = false;
  }

  return TestGetBufLen( output, pairs... ) && result;
}

template <>
bool  TestGetBufLen( FILE* ) { return true; }
template <>
bool  TestSerializeDeserialize( FILE* ) { return true; }

template <class T>
bool  are_objects_equal( const T& t1, const T& t2 ) {  return t1 == t2;  }
bool  are_objects_equal( const char* t1, const char* t2 ) {  return t1 == t2 || strcmp( t1, t2 ) == 0;  }

template <class T, class ... Types>
bool  TestSerializeDeserialize( FILE* output, const T& value, const Types&... values )
{
  auto              vallen = ::GetBufLen( value );
  std::vector<char> serial( vallen * 2 + 0x1000 );
  auto              endptr = ::Serialize( serial.data(), value );
  auto              result = false;

  if ( endptr != nullptr )
  {
    if ( (size_t)(endptr - serial.data()) == vallen )
    {
      T newval;

      if ( (endptr = (char*)::FetchFrom( (const char*)serial.data(), newval )) != nullptr )
      {
        if ( (size_t)(endptr - serial.data()) == vallen )
        {
          if ( !are_objects_equal( newval, value ) )
          {
            fprintf( output, "the deserialized object value '%s' of type '%s' is not equal to original value '%s'!\n",
              to_string( newval ).c_str(), type_name( &value ).c_str(), to_string( value ).c_str() );
          }
            else
          result = true;
        }
          else
        {
          fprintf( output, "FetchFrom() deserialized the previously serialized object '%s' of type '%s' with fault size!\n",
            to_string( value ).c_str(), type_name( &value ).c_str() );
        }
      }
        else
      {
        fprintf( output, "could not FetchFrom() the previously serialized object '%s' of type '%s', function returted nullptr!\n",
          to_string( value ).c_str(), type_name( &value ).c_str() );
      }
    }
      else
    {
      fprintf( output, "'%s' value of type '%s' serialized to %u bytes, but %u was expected!\n",
        to_string( value ).c_str(), type_name( &value ).c_str(),
          (unsigned)(endptr - serial.data()), (unsigned)vallen );
    }
  }
    else
  {
    fprintf( output, "could not Serialize() '%s' value of type '%s', function returted nullptr!\n",
      to_string( value ).c_str(), type_name( &value ).c_str() );
  }
  return TestSerializeDeserialize( output, values... ) && result;
}

template <class I1, class I2>
bool  same_iterable( const I1& i1, const I2& i2 )
{
  auto  _1 = i1.begin();
  auto  _2 = i2.begin();

  while ( _1 != i1.end() && _2 != i2.end() && *_1 == *_2 )
    { ++_1, ++_2; }

  return _1 == i1.end() && _2 == i2.end();
}

template <class T1, class T2>
bool  TestCrossTypeIterableSerialization( FILE* output, const T1& obj1, const T2* )
{
  auto  buff = std::vector<char>( ::GetBufLen( obj1 ) );
  auto  obj2 = T2();

  ::Serialize(              buff.data(), obj1 );
  ::FetchFrom( (const char*)buff.data(), obj2 );

  if ( !same_iterable( obj1, obj2 ) )
  {
    fprintf( output, "object of type '%s' deserialized from object of type '%s' differs in value: '%s' instead of '%s'\n",
      type_name( &obj1 ).c_str(), type_name( &obj1 ).c_str(),
      to_string( obj2 ).c_str(), to_string( obj2 ).c_str() );
    return false;
  }
  return true;
}

int   main()
{
  auto  result = true;

  result &= TestGetBufLen( stdout,
    std::make_pair( (char)0, 1 ),
    std::make_pair( (char)256, 1 ),

    std::make_pair( (unsigned char)0, 1 ),
    std::make_pair( (unsigned char)256, 1 ),

    std::make_pair( (int16_t)0, 1 ),
    std::make_pair( (int16_t)127, 1 ),
    std::make_pair( (int16_t)128, 2 ),
    std::make_pair( (int16_t)0x3fff, 2 ),
    std::make_pair( (int16_t)0x4000, 3 ),
    std::make_pair( (int16_t)0xffff, 3 ),

    std::make_pair( (int32_t)0, 1 ),
    std::make_pair( (int32_t)127, 1 ),
    std::make_pair( (int32_t)128, 2 ),
    std::make_pair( (int32_t)0x3fff, 2 ),
    std::make_pair( (int32_t)0x4000, 3 ),
    std::make_pair( (int32_t)0xffff, 3 ),
    std::make_pair( (int32_t)0x01fffff, 3 ),
    std::make_pair( (int32_t)0x03fffff, 4 ),
    std::make_pair( (int32_t)0x0fffffff, 4 ),
    std::make_pair( (int32_t)0x1fffffff, 5 ),
    std::make_pair( (int32_t)0xffffffff, 5 ),

    std::make_pair( (uint32_t)0, 1 ),
    std::make_pair( (uint32_t)127, 1 ),
    std::make_pair( (uint32_t)128, 2 ),
    std::make_pair( (uint32_t)0x3fff, 2 ),
    std::make_pair( (uint32_t)0x4000, 3 ),
    std::make_pair( (uint32_t)0xffff, 3 ),
    std::make_pair( (uint32_t)0x01fffff, 3 ),
    std::make_pair( (uint32_t)0x03fffff, 4 ),
    std::make_pair( (uint32_t)0x0fffffff, 4 ),
    std::make_pair( (uint32_t)0x1fffffff, 5 ),
    std::make_pair( (uint32_t)0xffffffff, 5 ),

    std::make_pair( (int64_t)0, 1 ),
    std::make_pair( (int64_t)127, 1 ),
    std::make_pair( (int64_t)128, 2 ),
    std::make_pair( (int64_t)0x3fff, 2 ),
    std::make_pair( (int64_t)0x4000, 3 ),
    std::make_pair( (int64_t)0xffff, 3 ),
    std::make_pair( (int64_t)0x01fffff, 3 ),
    std::make_pair( (int64_t)0x03fffff, 4 ),
    std::make_pair( (int64_t)0x0fffffff, 4 ),
    std::make_pair( (int64_t)0x1fffffff, 5 ),
    std::make_pair( (int64_t)0xffffffff, 5 ),
    std::make_pair( (int64_t)0x07ffffffff, 5 ),
    std::make_pair( (int64_t)0x0fffffffff, 6 ),

    std::make_pair( (uint64_t)0, 1 ),
    std::make_pair( (uint64_t)127, 1 ),
    std::make_pair( (uint64_t)128, 2 ),
    std::make_pair( (uint64_t)0x3fff, 2 ),
    std::make_pair( (uint64_t)0x4000, 3 ),
    std::make_pair( (uint64_t)0xffff, 3 ),
    std::make_pair( (uint64_t)0x01fffff, 3 ),
    std::make_pair( (uint64_t)0x03fffff, 4 ),
    std::make_pair( (uint64_t)0x0fffffff, 4 ),
    std::make_pair( (uint64_t)0x1fffffff, 5 ),
    std::make_pair( (uint64_t)0xffffffff, 5 ),
    std::make_pair( (int64_t)0x07ffffffff, 5 ),
    std::make_pair( (int64_t)0x0fffffffff, 6 ),

    std::make_pair( (float)1, 4 ),
    std::make_pair( (double)1, 8 ),

    std::make_pair( "char string", 12 ),

    std::make_pair( std::string( "char string" ), 12 ),

    std::make_pair( std::make_pair( (double).7, "float value" ), 20 ),

    std::make_pair( std::make_tuple( "tuple of keys", (float).9, 127U ), 19 ),

    std::make_pair( make_vector( { 1U, 2U, 3U } ), 4 ),
    std::make_pair( make_vector( { (float).1, (float).3 } ), 9 ),

    std::make_pair( make_list( { 1U, 2U, 3U } ), 4 ),

    std::make_pair( make_map<char const*, int>( { { "_1", 1 }, { "_2", 2 } } ), 9 ) );

  result &= TestSerializeDeserialize( stdout,
    (char)'A',
    (unsigned char)256,
    (int16_t)72456,
    (int32_t)724569,
    (int64_t)7245693019,
    (uint16_t)72456,
    (uint32_t)724569,
    (uint64_t)7245693019,
    (float)7245693019,
    (double)72456,
    (const char*)"char string",    // !!!memory lost!!!
    std::string( "c++ string" ),
    std::make_pair( 1, std::string( "c++ string" ) ),
    std::make_tuple( 1, std::string( "c++ string" ), .1 ),
    make_vector( { std::make_pair( std::string( "_1" ), 1U ), std::make_pair( std::string( "_2" ), 2U ) } ),
    make_map<std::string, unsigned>( { { std::string( "_1" ), 1U }, { std::string( "_2" ), 2U } } ) );

  result &= TestCrossTypeIterableSerialization( stdout,
    make_vector( { 1U, 2U, 3U, 4U, 5U } ), (std::list<unsigned>*)nullptr );

  return result ? 0 : EINVAL;
}