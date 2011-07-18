/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: dequeue.hpp
* Propose  			: Scalable StaticArray to act as a queue.
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2010-12-01
*/

#pragma once

#ifndef _PLIB_GENERIC_INTERNAL_STATICDEQUEUE_HPP_
#define _PLIB_GENERIC_INTERNAL_STATICDEQUEUE_HPP_

#if _DEF_IOS
#include "StaticArray.hpp"
#else
#include <Plib-Basic/StaticArray.hpp>
#endif

namespace Plib
{
	namespace Basic
	{
		// Pre-define for Allocator
		template < typename _TyObject, unsigned int _BLOCKSIZE = 256 > class MemoryMgr;
		
		// Scalable Cycle Block
		// When a cycle block is full then create a new block
		// to store more data.
		template< typename _TyObject, typename _TyValueObj, Uint32 _BlockSize >
		class StaticDequeue
		{
		protected:
			template < typename _TObj, typename _TVObj, Uint32 _BlockPiece >
			struct CBLOCK_NODE_ {
				StaticArray< _TObj, _TVObj, _BlockPiece > _BLOCK;
				CBLOCK_NODE_< _TObj, _TVObj, _BlockPiece > * _NEXT;
				CBLOCK_NODE_< _TObj, _TVObj, _BlockPiece > * _PREV;
			};

			typedef CBLOCK_NODE_< _TyObject, _TyValueObj, _BlockSize > TBLOCK;

		private:
			TBLOCK *	_BPut;
			TBLOCK *	_BGet;
			TBLOCK *	_BFree;

			Uint32		_AllSize;

			PLIB_THREAD_SAFE_DEFINE;
		private:
			// Free Block Op.
			inline TBLOCK * _NewFreeBlock( ) {
				TBLOCK * _BNew = _BFree;
				_BFree = _BFree ? _BFree->_NEXT : NULL;
				if ( _BNew == NULL ) {
					PNEW( TBLOCK, _BNew );
				}
				//_BNew = _BNew ? _BNew : Memory::New< TBLOCK >( ); //new TBLOCK;
				_BNew->_NEXT = NULL;
				return _BNew;
			}
			inline void _RetFreeBlock( TBLOCK * _BOld ) {
				_BOld->_NEXT = _BFree;
				_BFree = _BOld;
			}

			// Protected. No Copy.
			StaticDequeue< _TyObject, _TyValueObj, _BlockSize > ( const StaticDequeue <
				_TyObject, _TyValueObj, _BlockSize > & rhs ) { }
		public:
			StaticDequeue< _TyObject, _TyValueObj, _BlockSize > ( )
				: _BPut( NULL ), _BGet( NULL ), _BFree( NULL ), _AllSize( 0 )
			{
				CONSTRUCTURE;
				PNEW( TBLOCK, _BPut );
				_BGet = _BPut;
				_BPut->_NEXT = _BPut->_PREV = NULL;
			}
			~StaticDequeue< _TyObject, _TyValueObj, _BlockSize > ( ) {
				DESTRUCTURE;
				TBLOCK * _BTmp = NULL;
				while ( _BGet != NULL ) {
					_BTmp = _BGet->_NEXT;
					PDELETE( _BGet );
					_BGet = _BTmp;
				}
				while ( _BFree != NULL ) {
					_BTmp = _BFree->_NEXT;
					PDELETE( _BFree );
					_BFree = _BTmp;
				}
			}

			// Object Process
			// Push:
			//	Push the valued object to the Block, when push back and the 
			//	current put-block is full, then get a new block from the 
			//	free list.
			INLINE void PushBack( const _TyValueObj & vObj ) {
				PLIB_THREAD_SAFE;
				SELF_INCREASE(_AllSize);
				if ( _BPut->_BLOCK.PushBack( vObj ) ) return;
				_BPut->_NEXT = _NewFreeBlock();
				_BPut->_NEXT->_PREV = _BPut;
				_BPut = _BPut->_NEXT;
				// Push Again.
				_BPut->_BLOCK.PushBack( vObj );
			}
			INLINE void PushFront( const _TyValueObj & vObj ) {
				PLIB_THREAD_SAFE;
				SELF_INCREASE(_AllSize);
				if ( _BGet->_BLOCK.PushFront( vObj ) ) return;
				TBLOCK * _BTMP = _NewFreeBlock();
				_BTMP->_NEXT = _BGet;
				_BGet->_PREV = _BTMP;
				_BGet = _BTMP;
				_BGet->_PREV = NULL;
				// Push Again.
				_BGet->_BLOCK.PushFront( vObj );
			}
			
			// Pop:
			//	Pop the valued object.
			INLINE bool PopBack( _TyValueObj & _vObj ) {
				PLIB_THREAD_SAFE;
				if ( _AllSize == 0 ) return false;
				SELF_DECREASE(_AllSize);
				if ( _BPut->_BLOCK.PopBack( _vObj ) ) return true;
				_BPut = _BPut->_PREV;
				_RetFreeBlock( _BPut->_NEXT );
				_BPut->_NEXT = NULL;
				return _BPut->_BLOCK.PopBack( _vObj );
			}
			
			// Pop and throw the last object.
			// if the count is 0, return false.
			INLINE bool PopBack( ) {
				PLIB_THREAD_SAFE;
				if ( _AllSize == 0 ) return false;
				SELF_DECREASE(_AllSize);
				if ( _BPut->_BLOCK.PopBack( ) ) return true;
				_BPut = _BPut->_PREV;
				_RetFreeBlock( _BPut->_NEXT );
				_BPut->_NEXT = NULL;
				return _BPut->_BLOCK.PopBack( );
			}
			
			// Pop and get the first object.
			// if the count is 0, return false.
			INLINE bool PopFront( _TyValueObj & _vObj ) {
				PLIB_THREAD_SAFE;
				if ( _AllSize == 0 ) return false;
				SELF_DECREASE(_AllSize);
				if ( _BGet->_BLOCK.PopFront( _vObj ) ) return true;
				_BGet = _BGet->_NEXT;
				_RetFreeBlock( _BGet->_PREV );
				_BGet->_PREV = NULL;
				return _BGet->_BLOCK.PopFront( _vObj );
			}
			
			// Pop and throw the first object.
			// if the count is 0, return false.
			INLINE bool PopFront( ) {
				PLIB_THREAD_SAFE;
				if ( _AllSize == 0 ) return false;
				SELF_DECREASE(_AllSize);
				if ( _BGet->_BLOCK.PopFront( ) ) return true;
				_BGet = _BGet->_NEXT;
				_RetFreeBlock( _BGet->_PREV );
				_BGet->_PREV = NULL;
				return _BGet->_BLOCK.PopFront( );				
			}

			// Return the first object.
			INLINE _TyValueObj & Head( ) {
				assert( _AllSize > 0 );
				PLIB_THREAD_SAFE;
				if ( _AllSize == 0 || _BGet->_BLOCK.Size() > 0 ) return _BGet->_BLOCK[0];
				_BGet = _BGet->_NEXT;
				_RetFreeBlock( _BGet->_PREV );
				_BGet->_PREV = NULL;
				return _BGet->_BLOCK[0];
			}

			// Get the tail object.
			INLINE _TyValueObj & Tail( ) {
				assert( _AllSize > 0 );
				PLIB_THREAD_SAFE;
				return _BPut->_BLOCK[ ((_BPut->_BLOCK.Size() > 0) ? _BPut->_BLOCK.Size() - 1 : 0) ];
			}

			// Get the item count.
			INLINE Uint32 Size( ) const { return _AllSize; }
			
			// Check if the dequeue is empty, if Size equals 0, then be empty.
			INLINE bool Empty( ) { return _AllSize == 0; }
		
			// Clear all inner objects.
			INLINE void Clear( ) {
				PLIB_THREAD_SAFE;
				while ( _BGet != _BPut ) {
					_BGet = _BGet->_NEXT;
					_BGet->_PREV->_BLOCK.Clear();
					_RetFreeBlock( _BGet->_PREV );
				}
				_BGet->_PREV = NULL;
				_BGet->_BLOCK.Clear();
				_AllSize = 0;
			}
		
			// Print the object as json style.
			INLINE std::ostream & Print( std::ostream & os, Uint32 _level ) {
				BUILD_TAB( _level );
				os << "@{//Plib::Generic::Internal::StaticDequeue\n";
				BUILD_TAB( _level + 1 );
				os << "this: " << this << "\n";
				BUILD_TAB( _level + 1 );
				os << "inner-array: \n";
				BUILD_TAB( _level + 1 );
				os << "{";
				TBLOCK * _BTmp = _BGet;
				while ( _BTmp != NULL ) {
					_BTmp->Print( os, _level + 1 );
					_BTmp = _BGet->_NEXT;
				}
				BUILD_TAB( _level + 1 );
				os << "}";
				BUILD_TAB( _level );
				os << "}";
				return os;
			}
		};
	}
}

#endif // plib.generic.internal.staticqueue.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
