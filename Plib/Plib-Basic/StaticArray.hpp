/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: StaticArray.hpp
* Propose  			: Static Array Generic Container Definition.
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2010-11-28
*/

#pragma once 

#ifndef _PLIB_GENERIC_INTERNAL_STATICARRAY_HPP_
#define _PLIB_GENERIC_INTERNAL_STATICARRAY_HPP_

#if _DEF_IOS
#include "Plib.hpp"
#else
#include <Plib-Basic/Plib.hpp>
#endif

namespace Plib
{
	namespace Basic
	{
		// Pre-defined for staticdequeue and alloc.
		template < 
			typename _TyObject,  
			typename _TyValueObj, 
			Uint32 _BlockSize = 64 > 
		class StaticDequeue;
		
		// A Simple Cycle Cache Container
		// Use a static array to story valuable object.
		// Support for both ports' get/put.
		template < typename _TyObject, typename _TyValueObj, Uint32 _ArraySize >
		class StaticArray
		{
		protected:
			Uint32	_Used;
			Uint32	_Get;
			Uint32	_Put;

			// Buffer Block Array
			_TyValueObj		_BufBlock[ _ArraySize ];

			PLIB_THREAD_SAFE_DEFINE;
		public:
			// Const
			static const Uint32 MaxCount = _ArraySize;
			//enum { MaxCount = _BlockCount };

			// C'str
			StaticArray < _TyObject, _TyValueObj, _ArraySize > ( ) 
				: _Used(0), _Get(0), _Put(0) { CONSTRUCTURE; }
			StaticArray < _TyObject, _TyValueObj, _ArraySize > (
				const StaticArray< _TyObject, _TyValueObj, _ArraySize > & rhs )
				: _Used(rhs._Used), _Get(0), _Put(0) 
			{	
				CONSTRUCTURE;
				PLIB_OBJ_THREAD_SAFE( rhs );
				if ( rhs._Put > rhs._Get ) {
					::memcpy( _BufBlock, rhs._BufBlock + rhs._Get, 
						_Used * sizeof(_TyValueObj) );
					_Put = rhs._Put - rhs._Get;
				}
				else {
					::memcpy( _BufBlock, rhs._BufBlock + rhs._Get, 
						(_ArraySize - rhs._Get) * sizeof(_TyValueObj) );
					::memcpy( _BufBlock + (_ArraySize - rhs._Get), rhs._BufBlock,
						rhs._Put * sizeof(_TyValueObj) );
					_Put = _Used;
				}
			}
			~StaticArray< _TyObject, _TyValueObj, _ArraySize >()
			{
				DESTRUCTURE;
			}

			// Put Back
			INLINE bool PushBack( const _TyValueObj & _VObj ) {
				PLIB_THREAD_SAFE;
				// Full
				if ( _Used == _ArraySize ) return false;
				_BufBlock[_Put] = _VObj;
				SELF_INCREASE(_Put), SELF_INCREASE(_Used);
				if ( _Put == _ArraySize ) _Put = 0;
				return true;	
			}
			// Put Head
			INLINE bool PushFront( const _TyValueObj & _VObj ) {
				PLIB_THREAD_SAFE;
				// Full
				if ( _Used == _ArraySize ) return false;
				_BufBlock[(_Get == 0 ? _Get = (_ArraySize - 1) : 
					SELF_DECREASE(_Get))] = _VObj;
				SELF_INCREASE(_Used);
				return true;
			}
			// Get Head
			INLINE bool PopFront( _TyValueObj & _VObj ) {
				PLIB_THREAD_SAFE;
				if ( _Used == 0 ) return false;
				_VObj = _BufBlock[VALUE_INCREASED(_Get)];
				SELF_DECREASE(_Used);
				if ( _Get == _ArraySize ) _Get = 0;
				return true;
			}
			INLINE bool PopFront( ) {
				PLIB_THREAD_SAFE;
				if ( _Used == 0 ) return false;
				SELF_INCREASE(_Get);
				if ( _Get == _ArraySize ) _Get = 0;
				SELF_DECREASE(_Used);
				return true;
			}
			// Get Back
			INLINE bool PopBack( _TyValueObj & _VObj ) {
				PLIB_THREAD_SAFE;
				if ( _Used == 0 ) return false;
				_VObj = _BufBlock[(_Put == 0 ? (_Put = _ArraySize - 1) 
					: SELF_DECREASE(_Put))];
				SELF_DECREASE(_Used);
				return true;
			}
			INLINE bool PopBack( ) {
				PLIB_THREAD_SAFE;
				if ( _Used == 0 ) return false;
				_Put == 0 ? (_Put = _ArraySize - 1) : SELF_DECREASE(_Put);
				SELF_DECREASE(_Used);
				return true;
			}

			// This is a changable operator
			INLINE _TyValueObj & operator [] ( Uint32 _Idx ) {
				PLIB_THREAD_SAFE;
				return _BufBlock[ (_Get + _Idx) % _ArraySize ];
			}
			INLINE const _TyValueObj & operator [] ( Uint32 _Idx ) const {
				PLIB_THREAD_SAFE;
				return _BufBlock[ (_Get + _Idx) % _ArraySize ];
			}
			
			// Get the count.
			INLINE Uint32 Size() const { return _Used; }
			
			// Check if the array contains no object.
			INLINE bool Empty() const { return _Used == 0; }
			// Check if the array is full.
			INLINE bool Full() const { return _Used == _ArraySize; }
			
			// Reset the array's flag.
			// the value will not be cleared.
			INLINE void Clear() { 
				PLIB_THREAD_SAFE;
				_Used = _Get = _Put = 0; 
			}

			// Common Methods
			INLINE std::ostream & Print( std::ostream & os, Uint32 _level = 0 ) {
				BUILD_TAB( _level );
				os << "@{//Plib::Generic::Internal::StaticArray\n";
				BUILD_TAB( _level + 1 );
				os << "this: " << this << "\n";
				BUILD_TAB( _level + 1 );
				os << "objects: \n";
				BUILD_TAB( _level + 1 );
				os << "{\n";
				for ( Uint32 i = 0; i < _Used; ++i ) {
					BUILD_TAB( _level + 2 );
					os << "(" << i << ")=>" << (*this)[i] << ((i == _Used - 1) ? "\n" : ",\n");
				}
				BUILD_TAB( _level + 1 );
				os << "}\n";
				BUILD_TAB( _level );
				os << "}";
				return os;
			}
		};
	}
}

#endif	// plib.basic.staticarray.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

