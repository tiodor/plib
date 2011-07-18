/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Allocator.hpp
* Propose  			: Generic Alloctor Definition
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2010-12-01
*/

#pragma once

#ifndef _PLIB_BASIC_ALLOCATOR_HPP_
#define _PLIB_BASIC_ALLOCATOR_HPP_

#if _DEF_IOS
#include "StaticArray.hpp"
#include "StaticDequeue.hpp"
#else
#include <Plib-Basic/StaticArray.hpp>
#include <Plib-Basic/StaticDequeue.hpp>
#endif

// Error Check,
// If the project decided to use the Allocator, the project must
// enable the PLIB_BASIC_THREAD_SAFE macro.
#ifdef PLIB_USE_MEMPOOL
	#ifndef PLIB_BASIC_THREAD_SAFE
		#error "When use the Plib::General::Allocator, must enable PLIB_BASIC_THREAD_SAFE macro."
	#endif
#endif

namespace Plib
{
	namespace Basic
	{
		// Pre-define.
		template< typename _TyObject >
		class Allocator;
		
		// Block Manager
		template < typename _TyObject, unsigned int _BLOCKSIZE >
		class MemoryMgr
		{
			StaticDequeue< _TyObject, char * > _Buf_Addr_Queue;
			StaticDequeue< _TyObject, char *, _BLOCKSIZE * 2 > _Free_Object_Queue;
			
			enum { _OBJECTSIZE = sizeof(_TyObject) };
		private:
			void _Alloc_New_Buf( )
			{
				//char * _New_Block = (char *)::malloc( _OBJECTSIZE * _BLOCKSIZE );
				PCMALLOC(char, _New_Block, _OBJECTSIZE * _BLOCKSIZE );
				//char * _New_Block = (char *)Memory::Malloc( _OBJECTSIZE * _BLOCKSIZE );
				_Buf_Addr_Queue.PushBack( _New_Block );
				for ( unsigned int i = 0; i < _BLOCKSIZE; ++i )
					_Free_Object_Queue.PushBack( _New_Block + (i * _OBJECTSIZE) );
			}
		public:
			MemoryMgr< _TyObject, _BLOCKSIZE >( )
			{
				CONSTRUCTURE;
				_Alloc_New_Buf( );
			}
			~MemoryMgr< _TyObject, _BLOCKSIZE >( )
			{
				DESTRUCTURE;
				char * _Block_Addr = NULL;
				while( _Buf_Addr_Queue.PopFront( _Block_Addr ) ) {
					//Memory::Free( _Block_Addr );
					PFREE( _Block_Addr );
					//::free( _Block_Addr );
				}
			}

			// Public Interface
			INLINE void * Get( )
			{
				char * _Block_Addr = NULL;
				if ( !_Free_Object_Queue.PopFront( _Block_Addr ) )
				{
					_Alloc_New_Buf( );
					_Free_Object_Queue.PopFront( _Block_Addr );
				}
				return (void *)_Block_Addr;
			}

			INLINE void Release( void * _BLOCK )
			{
				_Free_Object_Queue.PushBack( (char *)_BLOCK );
			}

			// Reset all block as free
			// Does not free any memory.
			INLINE void Reset( )
			{
				_Free_Object_Queue.clear( );

				char * _FirstBlock;
				_Buf_Addr_Queue.PopFront( _FirstBlock );
				// The first block become the last block.
				_Buf_Addr_Queue.PushBack( _FirstBlock );

				char * _TmpBlock;
				while ( _TmpBlock != _FirstBlock )
				{
					_Buf_Addr_Queue.PopFront( _TmpBlock );
					for ( int i = 0; i < _BLOCKSIZE; ++i )
						_Free_Object_Queue.PushBack( _TmpBlock + (i * _OBJECTSIZE) );
					_Buf_Addr_Queue.PushBack( _TmpBlock );
				}
			}
		};

		// Allocate Object Use blockMgr
		template< typename _TyObject >
		class Allocator
		{
		protected:
		#ifdef PLIB_USE_MEMPOOL
			MemoryMgr< _TyObject, 256 > _VOID_BLOCK;
		#endif
			// Protected Copy Constructure.
			// Does not provide this feature.
			Allocator< _TyObject >( const Allocator< _TyObject > & rhs );
		public:
			Allocator< _TyObject > ( ) 
			{ 
				CONSTRUCTURE;
			}
			~Allocator< _TyObject > ( )
			{
				DESTRUCTURE;
			}

			_TyObject * Create( )
			{
			#ifdef PLIB_USE_MEMPOOL
				void * _Free_Mem = _VOID_BLOCK.Get( );
				_TyObject * _P = new ((void *)_Free_Mem) _TyObject;
			#else
				PCNEW( _TyObject, _P );
				//_TyObject * _P = Memory::New< _TyObject >( ); // new _TyObject;
			#endif
				return _P;
			}

			_TyObject * Create( const _TyObject & _T )
			{
			#ifdef PLIB_USE_MEMPOOL
				void * _Free_Mem = _VOID_BLOCK.Get( );
				_TyObject * _P = new ((void *)_Free_Mem) _TyObject( _T );
			#else
				PCNEWPARAM( _TyObject, _P, _T );
				//_TyObject * _P = Memory::New< _TyObject >( _T ); // new _TyObject( _T );
			#endif
				return _P;
			}

			void Destroy( _TyObject * _P )
			{
			#ifdef PLIB_USE_MEMPOOL
				_P->~_TyObject();
				_VOID_BLOCK.Release( (void *)_P );
			#else
				PDELETE( _P );
				//Memory::Delete< _TyObject >( _P ); // delete _P;
			#endif
			}
			
			// Rebind
			template < typename _TyRebindObject >
			struct Rebind {
				typedef Allocator< _TyRebindObject > Other;
			};
		};
	}
}

#endif // plib.baisc.allocator.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
