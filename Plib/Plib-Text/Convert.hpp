/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: convert.hpp
* Propose  			: Object Convert Class Definition.
* 
* Current Version	: 1.1
* Change Log		: Redefinition Use RPString.
* Author			: Push Chen
* Change Date		: 2011-01-09
*/

#ifndef _PLIB_UTILITY_CONVERT_HPP_
#define _PLIB_UTILITY_CONVERT_HPP_

#if _DEF_IOS
#include "String.hpp"
#else
#include <Plib-Text/String.hpp>
#endif

namespace Plib
{
	namespace Text
	{
		// Object Convert Structure.
		struct Convert
		{
			static INLINE Plib::Text::RString ToString( const char * _pData ) {
				//return String( _pData );
				return _pData;
			}
			
			template < typename _TyPoint >
			static INLINE Plib::Text::RString ToString( const _TyPoint * _pPoint ) {
				return (_pPoint == NULL ) ? "<NULL>" : Plib::Text::RString::Parse( "%p", _pPoint );
			}

			static INLINE Plib::Text::RString ToString( Uint8 _int ) {
				return Plib::Text::RString::Parse( "%u", (Uint8)_int );
			}
			static INLINE Plib::Text::RString ToString( Uint16 _int ) {
				return Plib::Text::RString::Parse( "%u", (Uint16)_int );
			}
			static INLINE Plib::Text::RString ToString( Uint32 _int ) {
				return Plib::Text::RString::Parse( "%u", (Uint32)_int );
			}
			static INLINE Plib::Text::RString ToString( Uint64 _int ) {
				return Plib::Text::RString::Parse( "%llu", (Uint64)_int );
			}

			static INLINE Plib::Text::RString ToString( Int8 _int ) {
				return Plib::Text::RString::Parse( "%d", (Int8)_int );
			}
			static INLINE Plib::Text::RString ToString( Int16 _int ) {
				return Plib::Text::RString::Parse( "%d", (Int16)_int );
			}
			static INLINE Plib::Text::RString ToString( Int32 _int ) {
				return Plib::Text::RString::Parse( "%d", (Int32)_int );
			}
			static INLINE Plib::Text::RString ToString( Int64 _int ) {
				return Plib::Text::RString::Parse( "%lld", (Int64)_int );
			}
			// For Thread Id.
		#if _DEF_WIN32
			static INLINE Plib::Text::RString ToString( long _int ) {
				return Plib::Text::RString::Parse( "%ld", _int );
			}
		#else
			static INLINE Plib::Text::RString ToString( pthread_t _int ) {
				return Plib::Text::RString::Parse( "%ld", (long)_int );
			}
		#endif
			static INLINE Plib::Text::RString ToString( double _int ) {
				return Plib::Text::RString::Parse( "%lf", _int );
			}

			static INLINE Plib::Text::RString ToString( time_t _Time ) {
				struct tm *  _timeStruct;
				_timeStruct = localtime( &(_Time) );
				return Plib::Text::RString::Parse( "%04d-%02d-%02d %02d:%02d:%02d", 
					(Int32)_timeStruct->tm_year + 1900, (Int32)_timeStruct->tm_mon + 1, 
					(Int32)_timeStruct->tm_mday, (Int32)_timeStruct->tm_hour, 
					(Int32)_timeStruct->tm_min, (Int32)_timeStruct->tm_sec
					);
			}

			static INLINE Plib::Text::RString ToString( const Plib::Text::RString & _String ) {
				return _String;
			}

			static INLINE Plib::Text::RString ToString( const std::string & _String ) {
				return Plib::Text::RString( _String.c_str(), _String.size() );
			}
			
			static INLINE Plib::Text::RWString ToString( const std::wstring & _WString ) {
				return Plib::Text::RWString( _WString.c_str(), _WString.size() );
			}

			static INLINE Plib::Text::RString ToString( char _C ) {
				return Plib::Text::RString( _C );
			}

			static INLINE Plib::Text::RString ToString( bool _B ) {
				return _B ? "True" : "False";
			}

			// Convert the string to int/uint.
			static INLINE Uint32 ToUint32(const Plib::Text::RString & _intVal){
				return (Uint32)atoi(_intVal.C_Str());
			}
			static INLINE Uint32 ToUint32(const char * _intVal){
				return (Uint32)atoi(_intVal);
			}
			static INLINE Int32 ToInt32(const Plib::Text::RString & _intVal){
				return atoi(_intVal.C_Str());
			}
			static INLINE Int32 ToInt32(const char * _intVal){
				return atoi(_intVal);
			}
			static INLINE Int64 ToInt64(const char * _intVal) {
	#if _DEF_WIN32
				return _atoi64(_intVal);
	#else
				return atoll(_intVal);
	#endif
			}
			static INLINE Int64 ToInt64(const Plib::Text::RString & _intVal){
				return ToInt64(_intVal.C_Str());
			}
			static INLINE Uint64 ToUint64(const char * _intVal){
				return (Uint64)ToInt64(_intVal);
			}
			static INLINE Uint64 ToUint64(const Plib::Text::RString & _intVal){
				return (Uint64)ToInt64(_intVal.C_Str());
			}

			// To Double
			static INLINE double ToDouble(const char * _dbVal) {
				return atof(_dbVal);
			}
			static INLINE double ToDouble(const Plib::Text::RString & _dbVal){
				return atof(_dbVal.C_Str());
			}

			// Convert the string to Boolean.
			// Either TRUE or !0 can be true.
			// "_Bool" is some how a keyword in Mac OS X..FML.
			// So I changed it to _SBool, which means "Source Bool Value"
			static INLINE bool ToBool( const Plib::Text::RString & _SBool ) {
				Plib::Text::RString _TBool;
				_TBool.DeepCopy(_SBool);
				_TBool.Trim().ToLower();
				if ( _TBool == "false" ) return false;
				return true;
			}

			// Convert unsigned long to IP Address
			static INLINE Plib::Text::RString ToIP( const unsigned long _addr ){
				return Plib::Text::RString::Parse("%u.%u.%u.%u", 
					(unsigned int)(_addr >> (0 * 8)) & 0x00FF,
					(unsigned int)(_addr >> (1 * 8)) & 0x00FF,
					(unsigned int)(_addr >> (2 * 8)) & 0x00FF,
					(unsigned int)(_addr >> (3 * 8)) & 0x00FF );
			}

		};
	}
}

#endif // plib.utility.convert.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
