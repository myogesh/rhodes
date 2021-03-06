#ifndef _RHOSTRINGCONV_H_
#define _RHOSTRINGCONV_H_

namespace rho{
namespace common{
/*
#ifdef  _UNICODE
#define convertFromString convertFromStringW
#define convertToString convertToStringW
#else
#define convertFromString convertFromStringA
#define convertToString convertToStringA
#endif

template<typename T>  inline void convertFromStringW( const wchar_t* szValue, T& value ){ value = szValue; }
template<typename T>  inline StringW convertToStringW( const T& value ){ return value; }
*/

template<typename T>  inline void convertFromStringA( const char* szValue, T& value ){ value = szValue; }
template<typename T>  inline String convertToStringA( const T& value ){ return value; }

inline String convertToStringA( const wchar_t* szValue ){ 
    String res;
    int nSize = wcstombs( NULL, szValue, 0 );
    if ( nSize >= 0 ){
        char* buf = new char[nSize+1];
        int nRes = wcstombs( buf, szValue, nSize );
        if ( nRes >= 0 ){
            buf[nRes] = 0;
            res = buf; 
        }
    }

    return res;
}

inline String convertToStringA( wchar_t* szValue ){ 
    return convertToStringA( (const wchar_t*)szValue );
}

/*inline String convertToStringA( const char* szValue ){ 
	return String(szValue);
}*/
	
template<>  inline String convertToStringA<StringW>( const StringW& strValue ){
    return convertToStringA(strValue.c_str());
}

inline void convertToStringW( const char* szValue, StringW& res ){ 
    int nSize = mbstowcs( NULL, szValue, 0 );
    if ( nSize >= 0 ){
        res.resize(nSize);
        int nRes = mbstowcs( &res[0], szValue, nSize );
        if ( nRes >= 0 ){
            res[nRes] = 0;
        }
    }
}

/*template<>  inline void convertFromStringA<StringW>( const char* szValue, StringW& value ){ value = StringW(szValue); }
template<>  inline String convertToStringA<StringW>( const StringW& value ){ return String(value); }

#define CONVERT_TYPE_W( Type, Fmt ) \
template<> inline void convertFromStringW<Type>( const wchar_t* szValue, Type& value )\
{ swscanf( szValue, Fmt, &value ); }\
template<> inline StringW convertToStringW<Type>( const Type& value )\
{ \
    wchar_t buf[100];\
    swprintf( buf, Fmt, value );\
    return StringW(buf);\
}
*/

#define CONVERT_TYPE_A( Type, Fmt ) \
template<> inline void convertFromStringA<Type>( const char* szValue, Type& value )\
{ sscanf( szValue, Fmt, &value ); }\
template<> inline String convertToStringA<Type>( const Type& value )\
{ \
    char buf[100];\
    sprintf( buf, Fmt, value );\
    return String(buf);\
}
/*
CONVERT_TYPE_W( unsigned int, L"%u" );
CONVERT_TYPE_W( int, L"%d" );
CONVERT_TYPE_W( unsigned long, L"%u" );
CONVERT_TYPE_W( long, L"%d" );
CONVERT_TYPE_W( wchar_t, L"%c" );
CONVERT_TYPE_W( char, L"%c" );
CONVERT_TYPE_W( unsigned char, L"%c" );
CONVERT_TYPE_W( short, L"%d" );
//CONVERT_TYPE_W( unsigned short, _T("%u") );
CONVERT_TYPE_W( bool, L"%d" );
CONVERT_TYPE_W( float, L"%f" );
CONVERT_TYPE_W( double, L"%f" );
*/

CONVERT_TYPE_A( unsigned int, "%u" );
CONVERT_TYPE_A( int, "%d" );
CONVERT_TYPE_A( unsigned long, "%u" );
CONVERT_TYPE_A( long, "%d" );
CONVERT_TYPE_A( wchar_t, "%c" );
CONVERT_TYPE_A( char, "%c" );
CONVERT_TYPE_A( unsigned char, "%c" );
CONVERT_TYPE_A( short, "%d" );
//CONVERT_TYPE_A( unsigned short, "%u" );
CONVERT_TYPE_A( bool, "%d" );
CONVERT_TYPE_A( float, "%f" );
CONVERT_TYPE_A( double, "%f" );

CONVERT_TYPE_A( uint64, "%llu" );

}
}

#endif //_RHOSTRINGCONV_H_
