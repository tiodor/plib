/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Queue.hpp
* Propose  			: A queue based on Array Organizer
* 
* Current Version	: 1.1.0
* Change Log		: Update the queue to use array organizer.
* Author			: Push Chen
* Change Date		: 2011-07-04
*/

#pragma once

#ifndef _PLIB_GENERIC_QUEUE_HPP_
#define _PLIB_GENERIC_QUEUE_HPP_

#if _DEF_IOS
#include "ArrayOrganizer.hpp"
#include "Reference.hpp"
#else
#include <Plib-Generic/ArrayOrganizer.hpp>
#include <Plib-Generic/Reference.hpp>
#endif

namespace Plib
{
	namespace Generic
	{
		
		// Pre-define
		template< 
			typename _TyObject, 
			typename _TyAlloc = Plib::Basic::Allocator< _TyObject >
		> class Queue;
		
		/*
		 * This is a queue container based on Array Organizer like array list.
		 * The queue is a tiny version of dequeue without push front and pop back.
		 * The queue is also thread safe if enable the macro PLIB_THREAD_BASIC_SAFE
		 */
		template< 
			typename _TyObject, 
			typename _TyAlloc = Plib::Basic::Allocator< _TyObject >
		>
		class Queue_ : public ArrayOrganizer< _TyObject, _TyAlloc >
		{
		public:
			// The father alias.
			typedef ArrayOrganizer< _TyObject, _TyAlloc >	TFather;
		
			// Default C'Str.
			// Create an empty dequeue.
			Queue_< _TyObject, _TyAlloc >( ) : TFather( ) { CONSTRUCTURE; }
			
			// Copy C'str
			Queue_< _TyObject, _TyAlloc >( 
				const Queue_< _TyObject, _TyAlloc > & rhs )
				: TFather( ) 
			{
				CONSTRUCTURE;
				// For each item in rhs, copy it.
				for ( Uint32 i = 0; i < rhs.Size(); ++i )
					TFather::__AppendLast( rhs.__Get(i) ); 
			}
			~Queue_< _TyObject, _TyAlloc >( ) { DESTRUCTURE; }
				
			// Push a new object to the end of the list.
			INLINE void Push( const _TyObject & _vobj ) { TFather::__AppendLast( _vobj ); }
			
			// Pop the first object.
			INLINE void Pop( ) { TFather::__RemoveHead( ); }

			// Get the first object.
			INLINE _TyObject & Head( ) { return TFather::__Get( 0 ); }
			
			// Get the last object.
			INLINE _TyObject & Tail( ) { return TFather::__Get( TFather::__Size() - 1 ); }

			// Get the item count of the queue.
			INLINE Uint32 Size() const { return TFather::__Size(); }
			
			// Check if the queue is empty.
			INLINE bool Empty( ) const { return TFather::__Empty(); }
			
			// Clear the storages' data.
			// The only way is to start a two-layer-loop to release all object.
			INLINE void Clear( ) { TFather::__Clear(); }
		};
		
		// Reference Version of Queue_
		// The Reference Queue_ is always used to be returned for multiple value.
		template< 
			typename _TyObject, 
			typename _TyAlloc
		>
		class Queue : public Reference< Queue_< _TyObject, _TyAlloc > >
		{
		public:
			typedef Reference< Queue_< _TyObject, _TyAlloc > >	TFather;
			
		protected:
			// For Null Array
			Queue< _TyObject, _TyAlloc >( bool _beNull ) : TFather( false ) 
			{ CONSTRUCTURE; }
			
		public:
			// Default C'Str.
			// Create an empty queue
			Queue< _TyObject, _TyAlloc >( )
				: TFather( true ) { CONSTRUCTURE; }
			Queue< _TyObject, _TyAlloc >( const Queue< _TyObject, _TyAlloc > & rhs )
				: TFather( rhs ) { CONSTRUCTURE; }
			
			// Copy C'str
			Queue< _TyObject, _TyAlloc >( 
				const Queue_< _TyObject, _TyAlloc > & rhs )
				: TFather( rhs ) { CONSTRUCTURE;  }
			~Queue< _TyObject, _TyAlloc >( ) { DESTRUCTURE; }

			// Push a new object to the end of the list.
			INLINE void Push( const _TyObject & _vobj ) { 
				TFather::_Handle->_PHandle->Push( _vobj ); }

			// Pop the first object.
			INLINE void Pop( ) { TFather::_Handle->_PHandle->Pop( ); }
						
			// Get the first object.
			INLINE _TyObject & Head( ) { return TFather::_Handle->_PHandle->Head(); }
			
			// Get the last object.
			INLINE _TyObject & Tail( ) { return TFather::_Handle->_PHandle->Tail(); }

			// Get the item count of the queue.
			INLINE Uint32 Size() const { return TFather::_Handle->_PHandle->Size(); }
			
			// Check if the queue is empty.
			INLINE bool Empty( ) const { return TFather::_Handle->_PHandle->Empty(); }
			
			// Clear the storages' data.
			// The only way is to start a two-layer-loop to release all object.
			INLINE void Clear( ) { TFather::_Handle->_PHandle->Clear(); }
			
			const static Queue< _TyObject, _TyAlloc > Null;
			
			static Queue< _TyObject, _TyAlloc > CreateNullQueue( )
			{
				return Queue< _TyObject, _TyAlloc >( false );
			}			
		};
		
		template< typename _TyObject, typename _TyAlloc >
			const Queue< _TyObject, _TyAlloc > 
				Queue< _TyObject, _TyAlloc >::Null( false );
	}
}

#endif // plib.generic.queue.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

