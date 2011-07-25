/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: string.hpp
* Propose  			: Reference String Definition.
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-01-09
*/

#pragma once

#ifndef _PLIB_COMMON_STRING_HPP_
#define _PLIB_COMMON_STRING_HPP_

#if _DEF_IOS
#include "Pool.hpp"
#include "ArrayList.hpp"
#else
#include <Plib-Generic/Pool.hpp>
#include <Plib-Generic/ArrayList.hpp>
#endif

namespace Plib
{
	namespace Text
	{
		// String Basic Object.
		// Can also act as string buffer.
		// The template parameter _Basic_C is used to 
		// specified different char type in case you need to use
		// Unicode.
		template< typename _Basic_C >
		class _StringBasic
		{
			// _Basic_C should support:
			//	Typename CharType, the char type.
			//	CharType EOL, the end of line, '\0' or L'\0';
			//	Function StringLength.
			typedef typename _Basic_C::CharType	CharType;
			typedef Uint32				Size_T;
			typedef Plib::Generic::Pair< typename _Basic_C::CharType *, Uint32 >	DataPairT;

			#define _FORMAT_LENGTH( _Basic, _Captial ) 	\
				( ( ( _Basic / _Captial ) + (Uint32)( _Basic % _Captial > 0) ) * _Captial )
				
		public:
			enum { NoPos = (Size_T)-1, Captial = 0x40U };
		
		protected:
			Size_T					_BufferSize;
			Size_T					_Length;
			CharType *				_Buffer;
			DataPairT *				_BufferPair;
			
			static Plib::Generic::RPool< DataPairT >		gBufferPool;
			PLIB_THREAD_SAFE_DEFINE;
		protected:
			// Check if has enough buffer to append words.
			// if not, realloc the buffer.
			void _CheckAndRealloc( Size_T _AppendSize )
			{
				if ( (_BufferSize - _Length) > _AppendSize ) return;
				_BufferSize += _FORMAT_LENGTH( _AppendSize, Captial );
				PCREALLOC( CharType, _Buffer, _Tmp, sizeof(CharType) * _BufferSize );
				assert( _Tmp != NULL );
				_Buffer = _Tmp;
			}
		public:
			// Default C'Str
			_StringBasic< _Basic_C > ( )
				: _BufferSize( 0 ), _Length( 0 )
			{
				CONSTRUCTURE;
				_BufferPair = &gBufferPool.Get( );
				_Buffer = _BufferPair->First;
				_BufferSize = _BufferPair->Second;
				if ( _Buffer == NULL ) {
					_BufferSize = 0x40;
					PMALLOC(CharType, _Buffer, sizeof(CharType) * _BufferSize);
					assert( _Buffer != NULL );
				}
				_Buffer[0] = _Basic_C::EOL;
			}
			
			// Initialize a string with onc char.
			_StringBasic< _Basic_C > ( CharType _C )
				: _BufferSize( 0 ), _Length( 1 )
			{
				CONSTRUCTURE;
				_BufferPair = &gBufferPool.Get( );
				_Buffer = _BufferPair->First;
				_BufferSize = _BufferPair->Second;
				if ( _Buffer == NULL ) {
					_BufferSize = 0x40;
					PMALLOC(CharType, _Buffer, sizeof(CharType) * _BufferSize);
					assert( _Buffer != NULL );
				} else {
					_CheckAndRealloc( 1 );
				}
				_Buffer[0] = _C;
				_Buffer[1] = _Basic_C::EOL;
			}

			// Initialize a string with multiple char.
			_StringBasic< _Basic_C > ( Uint32 _Len, CharType _C )
				:_Length( _Len )
			{
				CONSTRUCTURE;
				_BufferPair = &gBufferPool.Get( );
				_Buffer = _BufferPair->First;
				_BufferSize = _BufferPair->Second;
				if ( _Buffer == NULL ) {
					_BufferSize = _FORMAT_LENGTH( _Len, 0x40 );
					PMALLOC(CharType, _Buffer, sizeof(CharType) * _BufferSize);
					assert( _Buffer != NULL );					
				} else {
					_CheckAndRealloc( _Len );
				}
				if ( _Len != 0 )
					::memset( _Buffer, _C, _Len );
				_Buffer[_Length] = _Basic_C::EOL;
			}

			// Initialize the string with a string.
			_StringBasic< _Basic_C > ( const CharType * _Data )
				:_Length(0)
			{
				CONSTRUCTURE;
				_BufferPair = &gBufferPool.Get( );
				_Buffer = _BufferPair->First;
				_BufferSize = _BufferPair->Second;

				Uint32 _Len = (_Data == NULL) ? 0x40 : _Basic_C::StringLength(_Data);
				
				if ( _Buffer == NULL ) {
					_BufferSize = _FORMAT_LENGTH( _Len, 0x40 );
					PMALLOC(CharType, _Buffer, sizeof(CharType) * _BufferSize);
					assert( _Buffer != NULL );
				} else {
					_CheckAndRealloc( _Len );
				}
				
				if ( _Data != NULL ) {
					::memcpy(_Buffer, _Data, _Len);
					_Length = _Len;
				}
				_Buffer[_Length] = _Basic_C::EOL;
			}
			
			// Initialize the string with data
			_StringBasic< _Basic_C > ( const CharType * _Data, Size_T _DLength )
				: _Length( _DLength )
			{
				CONSTRUCTURE;
				_BufferPair = &gBufferPool.Get( );
				_Buffer = _BufferPair->First;
				_BufferSize = _BufferPair->Second;

				Uint32 _Len = (_DLength == 0) ? 
					0x40 : 
					((_Data == NULL) ? 0x40 : _Basic_C::StringLength(_Data));
					
				if ( _Buffer == NULL ) {
					_BufferSize = _FORMAT_LENGTH( _Len, 0x40 );
					PMALLOC(CharType, _Buffer, sizeof(CharType) * _BufferSize);
					assert( _Buffer != NULL );					
				} else {
					_CheckAndRealloc( _Len );
				}
				
				if ( _Data != NULL && _DLength != 0 ) {
					::memcpy(_Buffer, _Data, _Length);
				}
				_Buffer[_Length] = _Basic_C::EOL;
			}

			_StringBasic< _Basic_C > ( const _StringBasic< _Basic_C > & _String )
			{
				CONSTRUCTURE;
				_BufferPair = &gBufferPool.Get( );
				_Buffer = _BufferPair->First;
				_BufferSize = _BufferPair->Second;

				PLIB_OBJ_THREAD_SAFE( _String );
				_Length = _String._Length;
				
				if ( _Buffer == NULL ) {
					_BufferSize = _String._BufferSize;
					PMALLOC(CharType, _Buffer, sizeof(CharType) * _BufferSize);
					assert( _Buffer != NULL );
				} else {
					if ( _Length >= _BufferSize )
						_CheckAndRealloc( _Length - _BufferSize );
				}
				
				if ( _Length != 0 ) {
					::memcpy( _Buffer, _String._Buffer, _Length );
				}
				_Buffer[_Length] = _Basic_C::EOL;
			}
			
			~_StringBasic< _Basic_C >( )
			{
				DESTRUCTURE;
				assert( _BufferPair != NULL );
				_BufferPair->First = _Buffer;
				_BufferPair->Second = _BufferSize;
				gBufferPool.Return( *_BufferPair );
				_BufferSize = 0;
				_Length = 0;
				_Buffer = NULL;
			}
						
			// Basic Operator Function.
			// Append new words to the end of the string.
			bool Append( const CharType _c )
			{
				PLIB_THREAD_SAFE;
				this->_CheckAndRealloc( 1 );
				_Buffer[_Length] = _c;
				SELF_INCREASE( _Length );
				_Buffer[_Length] = _Basic_C::EOL;
				return true;
			}
			
			bool Append( const CharType * _Data, Size_T _DLength )
			{
				if ( _Data == NULL || _DLength == 0 ) return false;
				PLIB_THREAD_SAFE;
				this->_CheckAndRealloc( _DLength );
				::memcpy( _Buffer + _Length, _Data, _DLength );
				_Length += _DLength;
				_Buffer[_Length] = _Basic_C::EOL;				
				return true;
			}
			
			bool Append( const CharType * _Data )
			{
				if ( _Data == NULL ) return false;
				Size_T _AppendLength = _Basic_C::StringLength( _Data );
				return Append( _Data, _AppendLength );
			}

			bool Append( const _StringBasic< _Basic_C > & SB )
			{
				PLIB_OBJ_THREAD_SAFE( SB );
				return Append( SB._Buffer, SB._Length );
			}

			// Insert some words to specified position of the string.
			bool Insert( const CharType * _Data, Size_T _DLength, Size_T _Pos = NoPos)
			{
				if ( _Data == NULL || _DLength == 0 ) return false;
				if ( _Pos == NoPos || _Length == 0 ) return Append( _Data, _DLength );
				
				PLIB_THREAD_SAFE;
				if ( _Pos >= _Length ) return false;
				this->_CheckAndRealloc( _DLength );
				::memmove( _Buffer + _Pos + _DLength, _Buffer + _Pos, 
					sizeof(CharType) * (_Length - _Pos + 1) );
				::memcpy( _Buffer + _Pos, _Data, sizeof(CharType) * _DLength );
				_Length += _DLength;
				_Buffer[_Length] = _Basic_C::EOL;
				return true;
			}
			
			bool Insert( const CharType * _Data, Size_T _Pos = NoPos )
			{
				if ( _Data == NULL ) return false;
				Size_T _Length = _Basic_C::StringLength( _Data );
				return Insert( _Data, _Length, _Pos );
			}
			
			bool Remove( Size_T _First, Size_T _Size )
			{
				if ( _First >= _Length ) return false;
				if ( _Size == 0 || _Size > (_Length - _First) ) return false;
				//Size_T _RemoveSize = _Last - _First;
				PLIB_THREAD_SAFE;
				::memmove( _Buffer + _First, _Buffer + _First + _Size, 
					sizeof(CharType) * ( _Length - _First - _Size ) );
				_Length -= _Size;
				_Buffer[_Length] = _Basic_C::EOL;
				return true;
			}
			
			bool Remove( Size_T _First )
			{
				return Remove( _First, 1 );
			}

			// SubString
			_StringBasic< _Basic_C > SubString( Size_T _OffSet, Size_T _DLength = NoPos ) const
			{
				assert( _OffSet < _Length );
				Size_T _CopyLength = ( _DLength == NoPos ? (_Length - _OffSet) : _DLength );
				assert( _CopyLength + _OffSet <= _Length );
				return _StringBasic< _Basic_C >( _Buffer + _OffSet, _CopyLength );
			}
						
			// Operators
			CharType & operator [] ( Uint32 _Idx )
			{
				assert( _Idx < _Length );
				return _Buffer[_Idx];
			}

			const CharType & operator [] ( Uint32 _Idx ) const
			{
				assert( _Idx < _Length );
				return _Buffer[_Idx];
			}
			
			_StringBasic< _Basic_C > & operator += ( const CharType _c )
			{
				Append( _c );
				return *this;
			}
			
			_StringBasic< _Basic_C > & operator += ( const CharType * _Data )
			{
				Append( _Data );
				return *this;
			}

			_StringBasic< _Basic_C > & operator += ( const _StringBasic< _Basic_C > & SB )
			{
				Append( SB );
				return *this;
			}

			_StringBasic< _Basic_C > operator + ( const CharType * _Data )
			{
				_StringBasic< _Basic_C > _String(*this);
				_String.Append( _Data );
				return _String;
			}

			_StringBasic< _Basic_C > operator + ( const _StringBasic< _Basic_C > & SB )
			{
				_StringBasic< _Basic_C > _String(*this);
				_String.Append( SB );
				return _String;
			}

			// Copy Operators.
			_StringBasic< _Basic_C > & operator = ( const _StringBasic< _Basic_C > & SB )
			{
				if ( this == &SB ) return *this;
				PLIB_THREAD_SAFE;
				PLIB_OBJ_THREAD_SAFE( SB );
				if ( _BufferSize < SB._BufferSize )
					this->_CheckAndRealloc( SB._BufferSize );
				_Length = SB._Length;
				::memcpy( _Buffer, SB._Buffer, _Length );
				_Buffer[_Length] = _Basic_C::EOL;
				return *this;
			}

			_StringBasic< _Basic_C > & operator = ( const CharType * _Data )
			{
				this->Clear();
				if ( _Data == NULL ) return *this;
				this->Append( _Data );
				return *this;
			}
			
			// Compare Operators.
			bool operator == ( const _StringBasic< _Basic_C > & SB ) const
			{
				if ( this == &SB ) return true;
				if ( _Length != SB._Length ) return false;
				if ( _Length == 0 ) return true;
				return (memcmp( _Buffer, SB._Buffer, sizeof(CharType) * _Length ) == (int)0);
			}

			bool operator == ( const CharType * _Data ) const
			{
				if ( _Data == NULL && _Length == 0 ) return true;
				if ( _Data == NULL ) return false;
				if ( _Length == 0 ) return false;
				Size_T _DLength = _Basic_C::StringLength( _Data );
				if ( _DLength != _Length ) return false;
				return (memcmp( _Buffer, _Data, sizeof(CharType) * _Length ) == (int)0);
			}
			
			bool operator != ( const _StringBasic< _Basic_C > & SB ) const
			{
				return !( *this == SB );
			}

			bool operator != ( const CharType * _Data ) const
			{
				return !( *this == _Data );
			}
			
			bool operator < ( const _StringBasic< _Basic_C > & SB ) const
			{
				if ( this == &SB ) return false;
				if ( _Length == 0 && SB._Length > 0 ) return true;
				if ( _Length == 0 && SB._Length == 0 ) return false;
				if ( _Length < SB._Length ) {
					return (memcmp(_Buffer, SB._Buffer, sizeof(CharType) * _Length) <= (int)0);
				}
				return (memcmp(_Buffer, SB._Buffer, sizeof(CharType) * SB._Length) < (int)0);
			}

			bool operator < ( const CharType * _Data ) const
			{
				if ( _Data == NULL ) return false;
				if ( _Length == 0 ) return true;
				Size_T _DLength = _Basic_C::StringLength( _Data );
				if ( _Length < _DLength ) {
					return (memcmp(_Buffer, _Data, sizeof(CharType) * _Length) <=(int)0);
				}
				return (memcmp(_Buffer, _Data, sizeof(CharType) * _DLength) < (int)0);
			}

			// Cast
			operator const CharType * ( ) const
			{
				return _Buffer;
			}

			const CharType * C_Str( ) const
			{
				return _Buffer;
			}

			// Size and clear.
			Size_T Size( ) const
			{
				return _Length;
			}

			void Clear( )
			{
				PLIB_THREAD_SAFE;
				_Length = 0;
				_Buffer[_Length] = _Basic_C::EOL;
			}
			
			bool Empty( ) const 
			{
				PLIB_THREAD_SAFE;
				return _Length == 0;
			}

			// Format
			Size_T Format( const CharType * _Format, Size_T _FLength, va_list _Args )
			{
				this->Clear();
				PLIB_THREAD_SAFE;
				this->_CheckAndRealloc( _FLength );
				_Length = _Basic_C::StringPrintf( _Buffer, _BufferSize, _Format, _Args );
				return _Length;
			}
			Size_T Format( const CharType * _Format, ... )
			{
				va_list pArgList;
				va_start(pArgList, _Format);
				Size_T _FLength = _Basic_C::CalcVStringLen( _Format, pArgList );
				va_end( pArgList );
				va_start(pArgList, _Format);
				Size_T _L = Format( _Format, _FLength, pArgList );
				va_end( pArgList );
				return _L;
			}

			static _StringBasic< _Basic_C > Parse( const CharType * _Format, ... )
			{
				va_list pArgList;
				va_start( pArgList, _Format );
				_StringBasic< _Basic_C > _String;
				Size_T _FLength = _Basic_C::CalcVStringLen( _Format, pArgList );
				va_end( pArgList );
				va_start( pArgList, _Format );
				_String.Format( _Format, _FLength, pArgList );
				va_end(pArgList);
				return _String;
			}

			// Change Case
			// Return a new object.
			_StringBasic< _Basic_C > & ToUpper( )
			{
				PLIB_THREAD_SAFE;

				std::locale _Loc;
				for ( Uint32 i = 0; i < _Length; ++i )
					_Buffer[i] = std::toupper( _Buffer[i], _Loc );
				return *this;
			}
			_StringBasic< _Basic_C > & ToLower( )
			{
				PLIB_THREAD_SAFE;
				
				std::locale _Loc;
				for ( Uint32 i = 0; i < _Length; ++i )
					_Buffer[i] = std::tolower( _Buffer[i], _Loc );
				return *this;
			}

			_StringBasic< _Basic_C > & Trim( )
			{
				std::locale _Loc;
				// Trim Head.
				Size_T _EmptyCount = 0;
				while ( _EmptyCount < _Length && 
					std::isspace(_Buffer[_EmptyCount], _Loc ) ) ++_EmptyCount;
				this->Remove(0, _EmptyCount);
				// Trim Tail.
				_EmptyCount = 0;
				while ( _EmptyCount < _Length &&
					std::isspace(_Buffer[_Length - 1 - _EmptyCount], _Loc) )
					++_EmptyCount;
				if ( _EmptyCount == 0 ) return *this;
				_Length -= _EmptyCount;
				_Buffer[_Length] = _Basic_C::EOL;
				return *this;
			}

			bool StartWith( const CharType * _Data ) const
			{
				if ( _Data == NULL ) return false;
				if ( _Length == 0 ) return false;
				Size_T _DLength = _Basic_C::StringLength( _Data );
				if ( _Length < _DLength ) return false;
				PLIB_THREAD_SAFE;
				return _Basic_C::StrnCmp( _Buffer, _Data, _DLength );
			}

			bool StartWith( const _StringBasic< _Basic_C > & SB ) const
			{
				if ( SB._Length == 0 ) return false;
				if ( _Length < SB._Length ) return false;
				PLIB_THREAD_SAFE;
				PLIB_OBJ_THREAD_SAFE( SB );
				return _Basic_C::StrnCmp( _Buffer, SB._Buffer, SB._Length );
			}

			bool EndWith( const CharType * _Data ) const
			{
				if ( _Data == NULL ) return false;
				if ( _Length == 0 ) return false;
				Size_T _DLength = _Basic_C::StringLength( _Data );
				if ( _Length < _DLength ) return false;
				return _Basic_C::StrnCmp( _Buffer + _Length - _DLength, _Data, _DLength );
			}

			bool EndWith( const _StringBasic< _Basic_C > & SB ) const
			{
				if ( SB._Length == 0 ) return false;
				if ( _Length < SB._Length ) return false;
				PLIB_THREAD_SAFE;
				PLIB_OBJ_THREAD_SAFE( SB );
				return _Basic_C::StrnCmp( _Buffer + _Length - SB._Length, SB._Buffer, SB._Length );
			}

			// Alogrithm.
			Size_T Find( CharType _C, Size_T _OffSet = 0 ) const
			{
				if ( _Length == 0 ) return NoPos;
				if ( _OffSet >= _Length ) return NoPos;
				PLIB_THREAD_SAFE;
				for ( Size_T _idx = _OffSet; _idx < _Length; ++_idx )
					if ( _Buffer[_idx] == _C ) return _idx;
				return NoPos;
			}

			Size_T Find( const CharType * _Data, Size_T _DLength, Uint32 _OffSet ) const
			{
				if ( _OffSet >= _Length || _DLength > (_Length - _OffSet) ) return NoPos;
				Size_T _Pos = _Basic_C::Find( _Buffer + _OffSet, _Length - _OffSet, _Data, _DLength );
				if ( _Pos == NoPos ) return NoPos;
				return _OffSet + _Pos;
			}
			
			Size_T Find( const CharType * _Data, Uint32 _OffSet = 0 ) const 
			{
				if ( _Data == NULL ) return NoPos;
				Size_T _length = _Basic_C::StringLength( _Data );
				if ( _OffSet >= _Length || _length > (_Length - _OffSet) ) return NoPos;
				Size_T _Pos = _Basic_C::Find( _Buffer + _OffSet, _Length - _OffSet, _Data, _length );
				if ( _Pos == NoPos ) return NoPos;
				return _OffSet + _Pos;
			}
			
			Size_T Find( const _StringBasic< _Basic_C > & SB, Uint32 _OffSet = 0) const 
			{
				return Find( SB._Buffer, SB._Length, _OffSet );
			}
			
			Size_T FindLast( CharType _C, Size_T _OffSet = NoPos ) const
			{
				if ( _Length == 0 ) return NoPos;
				if ( _OffSet == NoPos || _OffSet >= _Length ) _OffSet = _Length - 1;
				PLIB_THREAD_SAFE;
				for ( Size_T _idx = _OffSet; _idx >= 0; --_idx )
					if ( _Buffer[_idx] == _C ) return _idx;
				return NoPos;
			}
			
			// Return the int value.
			INLINE Int32 IntValue() const {
				return _Basic_C::AtoI( _Buffer );
			}
			INLINE Uint32 UintValue() const {
				return (Uint32)_Basic_C::AtoI( _Buffer );
			}
			INLINE Int64 Int64Value() const {
				return _Basic_C::AtoL( _Buffer );
			}
			INLINE Uint64 Uint64Value() const {
				return (Uint64)_Basic_C::AtoL( _Buffer );
			}
			// Return Float Value
			INLINE double DoubleValue() const {
				return (double)_Basic_C::AtoF( _Buffer );
			}
			INLINE float FloatValue() const {
				return (float)_Basic_C::AtoF( _Buffer );
			}
			// Bool Value
			INLINE bool BoolValue() const {
				return _Basic_C::BoolValue( _Buffer, _Length );
			}
		};
		
		template< typename _Basic_C >
		_StringBasic< _Basic_C > operator + ( const typename _Basic_C::CharType * _Data, 
				const _StringBasic< _Basic_C > & SB )
		{
			_StringBasic< _Basic_C > _tmp( _Data );
			_tmp += SB;
			return _tmp;	
		}
		
		// Global Pool
		template < typename _Basic_C >
		Plib::Generic::RPool< Plib::Generic::Pair< typename _Basic_C::CharType *, Uint32 > >
			_StringBasic< _Basic_C >::gBufferPool;
		
			
		// ASCII String Function Struct.
		struct _Basic_Char
		{
			typedef char CharType;
			const static char EOL = '\0';
			static INLINE Uint32 StringLength( const CharType * _Data ) {
				return (Uint32)::strlen( _Data );
			}
			static INLINE Uint32 CalcVStringLen( const CharType * _Format, va_list _Args )
			{
				Uint32 _Len = 0;
#if _DEF_WIN32
				_Len = (Uint32)_vscprintf( _Format, _Args );
#else
				_Len = (Uint32)vsnprintf(NULL, 0, _Format, _Args);
#endif
				return _Len;
			}

			static INLINE Uint32 StringPrintf( CharType * _Buffer, Uint32 _Size,
				const CharType * _Format, va_list _Args )
			{
				Uint32 _Len = 0;
#if _DEF_WIN32
				_Len = vsprintf_s( _Buffer, _Size, _Format, _Args );
#else
				_Len = vsnprintf( _Buffer, _Size, _Format, _Args );
				//_Len = vsprintf( _Buffer, _Format, _Args );
#endif
				return _Len;
			}

			static bool StrnCmp( const CharType * _Src1, const CharType * _Src2, Uint32 _Size )
			{
				return strncmp(_Src1, _Src2, _Size) == (int)0;
			}
			
			// Return the int value.
			static Int32 AtoI( const CharType * _Buffer ) {
				return atoi(_Buffer);
			}
			static Int64 AtoL( const CharType * _Buffer ) {
	#if _DEF_WIN32
				return _atoi64(_Buffer);
	#else
				return atoll(_Buffer);
	#endif
			}
			// Return Float Value
			static double AtoF( const CharType * _Buffer ) {
				double _d = 0.0;
				sscanf( _Buffer, "%lf", &_d );
				return _d;
			}
			// Bool Value
			static bool BoolValue( const CharType * _Buffer, Uint32 _Length ) {
				if ( _Length == 5 ) {
					CharType _tempFalse[6] = { 0 };
					std::locale loc;
					for ( Uint32 i = 0; i < 5; ++i ) {
						_tempFalse[i] = std::tolower(_Buffer[i], loc);
					}
					return strncmp( _tempFalse, "false", 5 ) != (int)0;
				}
				return true;
			}
			
			static Uint32 Find( const CharType * _Buffer, Uint32 _Length, 
								const CharType * _Data, Uint32 _DLength )
			{
				if ( _Length < _DLength ) return (Uint32)-1;
				if ( _Length == _DLength ) 
					return ( strncmp( _Buffer, _Data, _Length ) == (int)0 ) ? 0 : (Uint32)-1;
					
				// Build BitMap;
				Uint16 _InData[256] = { (Uint16)-1 };
				for ( Uint16 i = 0; i < (Uint16)_DLength; ++i ) {
					_InData[(Uint8)_Data[i]] = i;
				}
				
				for ( Uint32 i = 0; ; /*Nothing to do, all in for*/) {
					if ( _Buffer[i] == _Data[0] ) {
						for ( Uint32 k = 0; k < _DLength; ++k ) {
							if ( _Buffer[i + k] != _Data[k] ) break;
						}
						return (Uint32)i;
					}
					i += _DLength;
					while ( (Uint16)-1 == _InData[ (Uint8)_Buffer[i] ] && i < _Length ) ++i;
					if ( i == _Length ) return (Uint32)-1;
					i -= _InData[ (Uint8)_Buffer[i] ];
				}
				return (Uint32)-1;
			}		
		};
		
		// Unicode String Function Struct.
		struct _Basic_WChar
		{
			typedef wchar_t CharType;
			const static wchar_t EOL = L'\0';
			static INLINE Uint32 StringLength( const CharType * _Data ) {
				return (Uint32)::wcslen( _Data );
			}
			static INLINE Uint32 CalcVStringLen( const CharType * _Format, va_list _Args )
			{
				Uint32 _Len = 0;
#if _DEF_WIN32
				_Len = (Uint32)_vscwprintf( _Format, _Args );
#else
				_Len = (Uint32)vswprintf(NULL, 0, _Format, _Args);
#endif
				return _Len;
			}

			static INLINE Uint32 StringPrintf( CharType * _Buffer, Uint32 _Size,
				const CharType * _Format, va_list _Args )
			{
				Uint32 _Len = 0;
#if _DEF_WIN32
				_Len = vswprintf_s( _Buffer, _Size, _Format, _Args );
#else
				_Len = vswprintf( _Buffer, _Size, _Format, _Args );
#endif
				return _Len;
			}
			static bool StrnCmp( const CharType * _Src1, const CharType * _Src2, Uint32 _Size )
			{
				return wcsncmp(_Src1, _Src2, _Size) == (int)0;
			}
			// Return the int value.
			static Int32 AtoI( const CharType * _Buffer ) {
#if _DEF_WIN32
				return _wtoi( _Buffer );
#else
				Int32 _v = 0;
				swscanf(_Buffer, L"%d", &_v);
				return _v;
#endif
			}
			static Int64 AtoL( const CharType * _Buffer ) {
				Int64 _v = 0;
				swscanf(_Buffer, L"%lld", &_v);
				return _v;
			}
			// Return Float Value
			static double AtoF( const CharType * _Buffer ) {
				double _f = 0;
				swscanf(_Buffer, L"%lf", &_f);
				return _f;
			}
			// Bool Value
			static bool BoolValue( const CharType * _Buffer, Uint32 _Length ) {
				if ( _Length == 5 ) {
					CharType _tempFalse[6] = { 0 };
					std::locale loc;
					for ( Uint32 i = 0; i < 5; ++i ) {
						_tempFalse[i] = std::tolower(_Buffer[i], loc);
					}
					return wcsncmp( _tempFalse, L"false", 5 ) != (int)0;
				}
				return true;
			}
			static Uint32 Find( const CharType * _WBuffer, Uint32 _WLength, 
								const CharType * _WData, Uint32 _WDLength )
			{
				if ( _WLength < _WDLength ) return (Uint32)-1;
				if ( _WLength == _WDLength ) 
					return ( wcsncmp( _WBuffer, _WData, _WLength ) == (int)0 ) ? 0 : (Uint32)-1;
				
				const char * _Buffer = (const char *)_WBuffer;
				Uint32 _Length = sizeof(CharType) * _WLength;
				const char * _Data = (const char *)_WData;
				Uint32 _DLength = sizeof(CharType) * _WDLength;
				
				// Build BitMap;
				Uint16 _InData[256] = { (Uint16)-1 };
				for ( Uint16 i = 0; i < (Uint16)_DLength; ++i ) {
					_InData[(Uint8)_Data[i]] = i;
				}
				
				for ( Uint32 i = 0; ; /*Nothing to do, all in for*/) {
					if ( _Buffer[i] == _Data[0] ) {
						for ( Uint32 k = 0; k < _DLength; ++k ) {
							if ( _Buffer[i + k] != _Data[k] ) break;
						}
						return (Uint32)i;
					}
					i += _DLength;
					while ( (Uint16)-1 != _InData[ (Uint8)_Buffer[i] ] && i < _Length ) ++i;
					if ( i == _Length ) return (Uint32)-1;
					i -= _InData[ (Uint8)_Buffer[i] ];
				}
				return (Uint32)-1;
			}		
		};
		
		// This is the reference version of string.		
		template< typename _Basic_C >
		class _RString : public Plib::Generic::Reference< _StringBasic< _Basic_C > >
		{
		public:
			typedef typename _Basic_C::CharType								CharType;
			typedef Uint32													Size_T;
			typedef Plib::Generic::Reference< _StringBasic< _Basic_C > >	TFather;
			enum { NoPos = _StringBasic<_Basic_C>::NoPos };
		protected:
			// For Null String C'Str
			_RString< _Basic_C >( bool _beNull ) : TFather( false ) { CONSTRUCTURE; }
		public:
			_RString< _Basic_C >( ) : TFather( true ) { CONSTRUCTURE; }
			_RString< _Basic_C >( CharType _c ) : TFather( true ) {
				CONSTRUCTURE;
				TFather::_Handle->_PHandle->operator [] ( 0 ) = _c;
				TFather::_Handle->_PHandle->operator [] ( 1 ) = _Basic_C::EOL;
			}
			_RString< _Basic_C >( Size_T _l, CharType _c ) 
				: TFather( _StringBasic< _Basic_C >(_l, _c) ) {CONSTRUCTURE;}
			_RString< _Basic_C >( const CharType * _data ) : TFather( true ) {
				CONSTRUCTURE;
				TFather::_Handle->_PHandle->Append( _data );
			}
			_RString< _Basic_C >( const CharType * _data, Size_T _len ) 
				: TFather( true ) {
				CONSTRUCTURE;
				TFather::_Handle->_PHandle->Append( _data, _len );
			}
			_RString< _Basic_C >( const _StringBasic< _Basic_C > & _string )
				: TFather( _string ) { CONSTRUCTURE; }
			_RString< _Basic_C >( const _RString< _Basic_C > & rhs ) : TFather( rhs )
				{ CONSTRUCTURE; }
			~_RString< _Basic_C >( ){ DESTRUCTURE; }
			
			// Methods
			INLINE bool Append( const CharType _c ) {
				return TFather::_Handle->_PHandle->Append( _c );
			}
			INLINE bool Append( const CharType * _data, Size_T _len ) {
				return TFather::_Handle->_PHandle->Append( _data, _len );
			}
			INLINE bool Append( const CharType * _data ) {
				return TFather::_Handle->_PHandle->Append( _data );
			}
			INLINE bool Append( const _RString< _Basic_C > & _rstring ) {
				return TFather::_Handle->_PHandle->Append( *(_rstring._Handle->_PHandle) );
			}
			INLINE bool Append( const _StringBasic< _Basic_C > & _string ) {
				return TFather::_Handle->_PHandle->Append( _string );
			}
			
			INLINE bool Insert( const CharType * _data, Size_T _pos = _StringBasic< _Basic_C >::NoPos ) {
				return TFather::_Handle->_PHandle->Insert( _data, _pos );
			}
			INLINE bool Insert( const CharType * _data, Size_T _length, Size_T _pos ) { 
				return TFather::_Handle->_PHandle->Insert( _data, _length, _pos );
			}
			
			INLINE bool Remove( Size_T _first, Size_T _size ) {
				return TFather::_Handle->_PHandle->Remove( _first, _size );
			}
			INLINE bool Remove( Size_T _first ) {
				return TFather::_Handle->_PHandle->Remove( _first );
			}
			
			INLINE _RString< _Basic_C > 
				SubString( Size_T _offset, Size_T _len = _StringBasic< _Basic_C >::NoPos ) const {
				return _RString< _Basic_C >( TFather::_Handle->_PHandle->SubString( _offset, _len ) );
			}
			
			INLINE CharType & operator [] ( Int32 _idx ) {
				return TFather::_Handle->_PHandle->operator [] ( (Uint32)_idx );
			}
			INLINE const CharType & operator [] ( Int32 _idx ) const {
				return TFather::_Handle->_PHandle->operator [] ( (Uint32)_idx );
			}
			INLINE _RString< _Basic_C > & operator += ( const CharType _c ) {
				TFather::_Handle->_PHandle->operator += ( _c );
				return *this;
			}
			INLINE _RString< _Basic_C > & operator += ( const CharType * _data ) {
				TFather::_Handle->_PHandle->operator += ( _data );
				return *this;
			}
			INLINE _RString< _Basic_C > & operator += ( const _RString< _Basic_C > & _rstring ) {
				TFather::_Handle->_PHandle->operator += ( *_rstring._Handle->_PHandle );
				return *this;
			}
			INLINE _RString< _Basic_C > & operator += ( const _StringBasic< _Basic_C > & _string ) {
				TFather::_Handle->_PHandle->operator += ( _string) ;
				return *this;
			}
			INLINE _RString< _Basic_C > & operator + ( const CharType * _data ) {
				TFather::_Handle->_PHandle->operator + ( _data );
				return *this;
			}
			INLINE _RString< _Basic_C > & operator + ( const _RString< _Basic_C > & _rstring ) {
				TFather::_Handle->_PHandle->operator + ( *_rstring._Handle->_PHandle );
				return *this;
			}
			INLINE _RString< _Basic_C > & operator + ( const _StringBasic< _Basic_C > & _string ) {
				TFather::_Handle->_PHandle->operator + ( _string );
				return *this;
			}
			INLINE _RString< _Basic_C > & operator = ( const CharType * _data ) {
				TFather::_Handle->_PHandle->operator = ( _data );
				return *this;
			}
			INLINE _RString< _Basic_C > & operator = ( const _StringBasic< _Basic_C > & _string ) {
				TFather::_Handle->_PHandle->operator = ( _string );
				return *this;
			}
			
			INLINE bool operator == ( const CharType * _data ) const {
				return TFather::_Handle->_PHandle->operator == ( _data );
			}
			INLINE bool operator != ( const CharType * _data ) const {
				return !(TFather::_Handle->_PHandle->operator == ( _data ));
			}
			INLINE bool operator < ( const _RString< _Basic_C > & _rstring ) const {
				return TFather::_Handle->_PHandle->operator < ( *_rstring._Handle->_PHandle );
			}
			INLINE bool operator < ( const _StringBasic< _Basic_C > & _string ) const { 
				return TFather::_Handle->_PHandle->operator < ( _string );
			}
			INLINE bool operator < ( const CharType * _data) const {
				return TFather::_Handle->_PHandle->operator < ( _data );
			}
			INLINE operator const CharType * ( ) const { 
				return (const CharType *)(*(*this));
			}
			INLINE const CharType * C_Str( ) const {
				return TFather::_Handle->_PHandle->C_Str();
			}
			INLINE const CharType * c_str( ) const {
				return TFather::_Handle->_PHandle->C_Str();
			}
			INLINE Size_T Size( ) const {
				return TFather::_Handle->_PHandle->Size();
			}
			INLINE Size_T size( ) const {
				return TFather::_Handle->_PHandle->Size();
			}
			INLINE bool Empty( ) const {
				return TFather::_Handle->_PHandle->Empty( );
			}
			INLINE bool empty( ) const {
				return TFather::_Handle->_PHandle->Empty( );
			}
			INLINE void Clear( ) {
				TFather::_Handle->_PHandle->Clear();
			} 
			INLINE void clear( ) { 
				TFather::_Handle->_PHandle->Clear();
			}
			
			INLINE Size_T Format( Uint32 _length, const CharType * _format, va_list _Args ) {
				return TFather::_Handle->_PHandle->Format( _format, _length, _Args );
			}
			
			INLINE Size_T Format( const CharType * _format, ... ) { 
				va_list pArgList;
				va_start(pArgList, _format);
				Size_T _length = _Basic_C::CalcVStringLen( _format, pArgList );
				va_end( pArgList );
				va_start(pArgList, _format);
				Size_T _L = TFather::_Handle->_PHandle->Format( _format, _length, pArgList );
				va_end( pArgList );
				return _L;
			}
			static _RString< _Basic_C > Parse( const CharType * _format, ... ) {
				_RString< _Basic_C > _String;
				va_list pArgList;
				va_start( pArgList, _format );
				Size_T _FLength = _Basic_C::CalcVStringLen( _format, pArgList );
				va_end( pArgList );
				va_start( pArgList, _format );
				_String->Format( _format, _FLength, pArgList );
				va_end(pArgList);
				return _String;
			}
			
			INLINE _RString< _Basic_C > & ToUpper( ) {
				TFather::_Handle->_PHandle->ToUpper( );
				return *this;
			}
			INLINE _RString< _Basic_C > & ToLower( ) { 
				TFather::_Handle->_PHandle->ToLower( );
				return *this;
			}
			INLINE _RString< _Basic_C > & Trim( ) { 
				TFather::_Handle->_PHandle->Trim( );
				return *this;
			}
			
			INLINE Plib::Generic::RArray< _RString< _Basic_C > >
			 	Split ( _RString< _Basic_C > _Carry ) const {
				Plib::Generic::RArray< _RString< _Basic_C > > _resultArray;
				if ( this->Size() == 0 ) return _resultArray;
				Uint32 _pos = 0;
				do {
					Uint32 _lastPos = _StringBasic< _Basic_C >::NoPos;
					for ( Uint32 i = 0; i < _Carry.size(); ++i ) {
						Uint32 _nextCarry = this->Find( _Carry[i], _pos );
						_lastPos = ( _nextCarry < _lastPos ) ? _nextCarry : _lastPos;
					}
					if ( _lastPos == _StringBasic< _Basic_C >::NoPos ) _lastPos = this->size();
					_RString< _Basic_C > _value = this->SubString( _pos, _lastPos - _pos );
					if ( _value.Trim().Size() > 0 )
						_resultArray.PushBack( _value );
					_pos = _lastPos + 1;
				} while ( _pos < this->size() );
				return _resultArray;
			}
			
			INLINE bool StartWith( const CharType * _data ) const { 
				return TFather::_Handle->_PHandle->StartWith( _data );
			}
			INLINE bool StartWith( const _StringBasic< _Basic_C > & _string ) const {
				return TFather::_Handle->_PHandle->StartWith( _string );
			}
			INLINE bool StartWith( const _RString< _Basic_C > & _rstring ) const  {
				return TFather::_Handle->_PHandle->StartWith( *_rstring );
			}
			
			INLINE bool EndWith( const CharType * _data ) const {
				return TFather::_Handle->_PHandle->EndWith( _data );
			}
			INLINE bool EndWith( const _StringBasic< _Basic_C > & _string ) const {
				return TFather::_Handle->_PHandle->EndWith( _string );
			}
			INLINE bool EndWith( const _RString< _Basic_C > & _rstring ) const {
				return TFather::_Handle->_PHandle->EndWith( *_rstring );
			}
			
			INLINE Size_T Find( CharType _c, Size_T _offset = 0 ) const {
				return TFather::_Handle->_PHandle->Find( _c, _offset );
			}
			INLINE Size_T Find( const CharType * _data, Size_T _length, Size_T _offSet ) const {
				return TFather::_Handle->_PHandle->Find( _data, _length, _offSet );
			}
			INLINE Size_T Find( const CharType * _data, Size_T _offSet = 0 ) const {
				return TFather::_Handle->_PHandle->Find( _data, _offSet );
			}
			INLINE Size_T Find( const _StringBasic< _Basic_C > & _string, Size_T _offSet = 0 ) const {
				return TFather::_Handle->_PHandle->Find( _string, _offSet );
			}
			INLINE Size_T Find( const _RString< _Basic_C > & _rstring, Size_T _offSet = 0 ) const {
				return TFather::_Handle->_PHandle->Find( *_rstring._Handle->_PHandle, _offSet );
			}
			INLINE Size_T FindLast( CharType _c, Size_T _offset = _StringBasic< _Basic_C >::NoPos ) const {
				return TFather::_Handle->_PHandle->FindLast( _c, _offset );
			}		
			
			// Return the int value.
			INLINE Int32 IntValue() const {
				return TFather::_Handle->_PHandle->IntValue();
			}
			INLINE Uint32 UintValue() const {
				return TFather::_Handle->_PHandle->UintValue();
			}
			INLINE Int64 Int64Value() const {
				return TFather::_Handle->_PHandle->Int64Value();
			}
			INLINE Uint64 Uint64Value() const {
				return TFather::_Handle->_PHandle->Uint64Value();
			}
			// Return Float Value
			INLINE double DoubleValue() const {
				return TFather::_Handle->_PHandle->DoubleValue();
			}
			INLINE float FloatValue() const {
				return TFather::_Handle->_PHandle->FloatValue();
			}
			// Bool Value
			INLINE bool BoolValue() const {
				return TFather::_Handle->_PHandle->BoolValue();
			}
			
			const static _RString< _Basic_C > Null;
			
			static _RString< _Basic_C > CreateNullString( )
			{
				return _RString< _Basic_C >( false );
			}
			
			INLINE std::ostream Print( std::ostream & os, Uint32 _level = 0 ) {
				BUILD_TAB( _level );
				os << "@{//RString\n";
				BUILD_TAB( _level + 1 );
				os << "this: " << this << "\n";
				BUILD_TAB( _level + 1 );
				os << "data: " << *this << "\n";
				BUILD_TAB( _level + 1 );
				os << "father:\n";
				TFather::Print(os, _level + 1);
				BUILD_TAB( _level );
				os << "\n}";
				return os;
			}
		};
		
		template< typename _Basic_C >
		_RString< _Basic_C > operator + ( const typename _Basic_C::CharType * _Data, 
				const _RString< _Basic_C > & SB )
		{
			_RString< _Basic_C > _tmp( _Data );
			_tmp += SB;
			return _tmp;	
		}
		
		template< typename _Basic_C >
		const _RString< _Basic_C > _RString< _Basic_C >::Null( false );
		
		
		typedef _RString< _Basic_Char >		RString;
		typedef _RString< _Basic_WChar >	RWString;
	}
}

#endif // plib.basic.string.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

