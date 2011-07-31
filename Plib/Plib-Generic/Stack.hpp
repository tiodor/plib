/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Stack.hpp
* Propose  			: A stack based on Array Organizer
* 
* Current Version	: 1.1.0
* Change Log		: Update the stack to use array organizer.
* Author			: Push Chen
* Change Date		: 2011-07-04
*/

#pragma once

#ifndef _PLIB_GENERIC_STACK_HPP_
#define _PLIB_GENERIC_STACK_HPP_

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
		> class Stack;
			
		/*
		 * This is a stack container based on Array Organizer like array list.
		 * The stack is a tiny version of dequeue without push back and pop back.
		 * The stack is also thread safe if enable the macro PLIB_THREAD_BASIC_SAFE
		 */
		template< 
			typename _TyObject, 
			typename _TyAlloc = Plib::Basic::Allocator< _TyObject >
		>
		class Stack_ : public ArrayOrganizer< _TyObject, _TyAlloc >
		{
		public:
			// The father alias.
			typedef ArrayOrganizer< _TyObject, _TyAlloc >	TFather;

			// Default C'Str.
			// Create an empty dequeue.
			Stack_< _TyObject, _TyAlloc >( ) : TFather( ) { CONSTRUCTURE; }
			
			// Copy C'str
			Stack_< _TyObject, _TyAlloc >( 
				const Stack_< _TyObject, _TyAlloc > & rhs )
				: TFather( ) 
			{
				CONSTRUCTURE;
				// For each item in rhs, copy it.
				for ( Uint32 i = 0; i < rhs.Size(); ++i )
					TFather::__AppendLast( rhs.__Get(i) ); 
			}
			~Stack_< _TyObject, _TyAlloc >( ) { DESTRUCTURE; }
				
			// Push a new object to the end of the list.
			INLINE void Push( const _TyObject & _vobj ) { TFather::__AppendHead( _vobj ); }
			
			// Pop the first object.
			INLINE void Pop( ) { TFather::__RemoveHead( ); }

			// Get the first object.
			INLINE _TyObject & Top( ) { return TFather::__Get( 0 ); }

			// Get the item count of the stack.
			INLINE Uint32 Size() const { return TFather::__Size(); }
			
			// Check if the stack is empty.
			INLINE bool Empty( ) const { return TFather::__Empty(); }
			
			// Clear the storages' data.
			// The only way is to start a two-layer-loop to release all object.
			INLINE void Clear( ) { TFather::__Clear(); }
		};
		
		// Reference Version of Queue
		// The Reference Queue is always used to be returned for multiple value.
		template< 
			typename _TyObject, 
			typename _TyAlloc
		>
		class Stack : public Reference< Stack_< _TyObject, _TyAlloc > >
		{
		public:
			typedef Reference< Stack_< _TyObject, _TyAlloc > >	TFather;
			
		protected:
			// For Null Array
			Stack< _TyObject, _TyAlloc >( bool _beNull ) : TFather( false ) 
			{ CONSTRUCTURE; }
			
		public:
			// Default C'Str.
			// Create an empty queue
			Stack< _TyObject, _TyAlloc >( )
				: TFather( true ) { CONSTRUCTURE; }
			
			Stack< _TyObject, _TyAlloc >( const Stack< _TyObject, _TyAlloc > & rhs )
				: TFather( rhs ) { CONSTRUCTURE; }
			// Copy C'str
			Stack< _TyObject, _TyAlloc >( 
				const Stack_< _TyObject, _TyAlloc > & rhs )
				: TFather( rhs ) { CONSTRUCTURE; }

			~Stack< _TyObject, _TyAlloc >( ) { DESTRUCTURE; }

			// Push a new object to the end of the list.
			INLINE void Push( const _TyObject & _vobj ) { 
				TFather::_Handle->_PHandle->Push( _vobj ); }

			// Pop the first object.
			INLINE void Pop( ) { TFather::_Handle->_PHandle->Pop( ); }
			
			// Get the first object.
			INLINE _TyObject & Top( ) { return TFather::_Handle->_PHandle->Top(); }

			// Get the item count of the stack.
			INLINE Uint32 Size() const { return TFather::_Handle->_PHandle->Size(); }
			
			// Check if the stack is empty.
			INLINE bool Empty( ) const { return TFather::_Handle->_PHandle->Empty(); }
			
			// Clear the storages' data.
			// The only way is to start a two-layer-loop to release all object.
			INLINE void Clear( ) { TFather::_Handle->_PHandle->Clear(); }
			
			const static Stack< _TyObject, _TyAlloc > Null;
			
			static Stack< _TyObject, _TyAlloc > CreateNullStack( )
			{
				return Stack< _TyObject, _TyAlloc >( false );
			}			
		};
		
		template< typename _TyObject, typename _TyAlloc >
			const Stack< _TyObject, _TyAlloc > 
				Stack< _TyObject, _TyAlloc >::Null( false );
	}
}

#endif // plib.generic.stack.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

