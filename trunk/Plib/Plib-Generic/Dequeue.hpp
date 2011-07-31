/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Dequeue.hpp
* Propose  			: A Dequeue based on Array Organizer
* 
* Current Version	: 1.1.0
* Change Log		: Update the dequeue to use array organizer.
* Author			: Push Chen
* Change Date		: 2011-07-04
*/

#pragma once

#ifndef _PLIB_GENERIC_DEQUEUE_HPP_
#define _PLIB_GENERIC_DEQUEUE_HPP_

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
		
		// Pre-define.
		template< 
			typename _TyObject, 
			typename _TyAlloc = Plib::Basic::Allocator< _TyObject >
		> class Dequeue;
		
		/*
		 * This is a dequeue container based on Array Organizer like array list.
		 * The dequeue is a tiny version of array list without random access.
		 * that means no operator [], no inserting or removing objects in the middle
		 * of the dequeue.
		 * The Dequeue is also thread safe if enable the macro PLIB_THREAD_BASIC_SAFE
		 */
		template< 
			typename _TyObject, 
			typename _TyAlloc = Plib::Basic::Allocator< _TyObject >
		>
		class Dequeue_ : public ArrayOrganizer< _TyObject, _TyAlloc >
		{
		public:
			// The father alias.
			typedef ArrayOrganizer< _TyObject, _TyAlloc >	TFather;

			// Default C'Str.
			// Create an empty dequeue.
			Dequeue_< _TyObject, _TyAlloc >( ) : TFather( ) { CONSTRUCTURE; }
			
			// Copy C'str
			Dequeue_< _TyObject, _TyAlloc >( 
				const Dequeue_< _TyObject, _TyAlloc > & rhs )
				: TFather( ) 
			{
				CONSTRUCTURE;
				// For each item in rhs, copy it.
				for ( Uint32 i = 0; i < rhs.Size(); ++i )
					TFather::__AppendLast( rhs.__Get(i) ); 
			}
			~Dequeue_< _TyObject, _TyAlloc > ( ) { DESTRUCTURE; }
				
			// Push a new object to the end of the list.
			INLINE void PushBack( const _TyObject & _vobj ) { TFather::__AppendLast( _vobj ); }
			
			// Push a new object to the head of the list.
			INLINE void PushFront( const _TyObject & _vobj ) { TFather::__AppendHead( _vobj ); }
			
			// Pop the tail object.
			INLINE void PopBack( ) { TFather::__RemoveLast( ); }
			
			// Pop the first object.
			INLINE void PopFront( ) { TFather::__RemoveHead( ); }

			// Get the first object.
			INLINE _TyObject & Head( ) { return TFather::__Get( 0 ); }
			
			// Get the last object.
			INLINE _TyObject & Tail( ) { return TFather::__Get( TFather::__Size() - 1 ); }

			// Get the item count of the dequeue.
			INLINE Uint32 Size() const { return TFather::__Size(); }
			
			// Check if the dequeue is empty.
			INLINE bool Empty( ) const { return TFather::__Empty(); }
			
			// Clear the storages' data.
			// The only way is to start a two-layer-loop to release all object.
			INLINE void Clear( ) { TFather::__Clear(); }
		};
		
		// Reference Version of Dequeue
		// The Reference Dequeue is always used to be returned for multiple value.
		template< 
			typename _TyObject, 
			typename _TyAlloc
		>
		class Dequeue : public Reference< Dequeue_< _TyObject, _TyAlloc > >
		{
		public:
			typedef Reference< Dequeue_< _TyObject, _TyAlloc > >	TFather;
			
		protected:
			// For Null Array
			Dequeue< _TyObject, _TyAlloc >( bool _beNull ) : TFather( false ) 
			{ CONSTRUCTURE; }
			
		public:
			// Default C'Str.
			// Create an empty array list.
			Dequeue< _TyObject, _TyAlloc >( ) : TFather( true ) { CONSTRUCTURE; }
			Dequeue< _TyObject, _TyAlloc >( const Dequeue< _TyObject, _TyAlloc > & rhs )
				: TFather( rhs ) { CONSTRUCTURE; }
			~Dequeue< _TyObject, _TyAlloc >( ) { DESTRUCTURE; }
			
			// Copy C'str
			Dequeue< _TyObject, _TyAlloc >( 
				const Dequeue_< _TyObject, _TyAlloc > & rhs )
				: TFather( rhs ) { }

			// Push a new object to the end of the list.
			INLINE void PushBack( const _TyObject & _vobj ) { 
				TFather::_Handle->_PHandle->PushBack( _vobj ); }
			
			// Push a new object to the head of the list.
			INLINE void PushFront( const _TyObject & _vobj ) { 
				TFather::_Handle->_PHandle->PushFront( _vobj ); }
			
			// Pop the tail object.
			INLINE void PopBack( ) { TFather::_Handle->_PHandle->PopBack( ); }
			
			// Pop the first object.
			INLINE void PopFront( ) { TFather::_Handle->_PHandle->PopFront( ); }
						
			// Get the first object.
			INLINE _TyObject & Head( ) { return TFather::_Handle->_PHandle->Head(); }
			
			// Get the last object.
			INLINE _TyObject & Tail( ) { return TFather::_Handle->_PHandle->Tail(); }

			// Get the item count of the dequeue.
			INLINE Uint32 Size() const { return TFather::_Handle->_PHandle->Size(); }
			
			// Check if the dequeue is empty.
			INLINE bool Empty( ) const { return TFather::_Handle->_PHandle->Empty(); }
			
			// Clear the storages' data.
			// The only way is to start a two-layer-loop to release all object.
			INLINE void Clear( ) { TFather::_Handle->_PHandle->Clear(); }
			
			const static Dequeue< _TyObject, _TyAlloc > Null;
			
			static Dequeue< _TyObject, _TyAlloc > CreateNullDequeue( )
			{
				return Dequeue< _TyObject, _TyAlloc >( false );
			}			
		};
		
		template< typename _TyObject, typename _TyAlloc >
			const Dequeue< _TyObject, _TyAlloc > 
				Dequeue< _TyObject, _TyAlloc >::Null( false );
	}
}

#endif // plib.generic.dequeue.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

