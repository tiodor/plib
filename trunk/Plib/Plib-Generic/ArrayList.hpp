/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Array.hpp
* Propose  			: A new defined Array-List by Push Chen.
* 
* Current Version	: 1.02
* Change Log		: 1.02: Rename ArrayList to Array_, Rename RArray to Array
* Change Log		: Rewrite the class after my MacBook Pro accident... 
* Author			: Push Chen
* Change Date		: 2011-06-27
*/

#pragma once

#ifndef _PLIB_GENERIC_ARRAY_HPP_
#define _PLIB_GENERIC_ARRAY_HPP_

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
		template < 
			typename _TyObject,
			typename _TyAlloc = Plib::Basic::Allocator< _TyObject >
		> class Array;
			
		/*
		 * This class is about to create an array list container.
		 * Using the previous Array_Block_ as internal orginizer.
		 * The ArrayList is going to create an self-maintained Array_Block_ array.
		 * In this array, store all outside objects.
		 */
		template< 
			typename _TyObject, 
			typename _TyAlloc = Plib::Basic::Allocator< _TyObject >
		>
		class Array_ : public ArrayOrganizer< _TyObject, _TyAlloc >
		{
		public:
			typedef ArrayOrganizer< _TyObject, _TyAlloc >	TFather;
			// Default C'Str.
			// Create an empty array list.
			Array_< _TyObject, _TyAlloc >( )
				: TFather( ) { CONSTRUCTURE; }
			
			// Copy C'str
			Array_< _TyObject, _TyAlloc >( 
				const Array_< _TyObject, _TyAlloc > & rhs )
				: TFather( ) { CONSTRUCTURE; this->Append( rhs ); }
			
			// C'Str with array
			template < typename _TyIterator >
			Array_< _TyObject, _TyAlloc >( 
			 	_TyIterator _begin, _TyIterator _end )
				: TFather( ) { CONSTRUCTURE; this->Append( _begin, _end ); }

			~Array_< _TyObject, _TyAlloc >( ) { DESTRUCTURE; }
			
			// Push a new object to the end of the list.
			INLINE void PushBack( const _TyObject & _vobj ) { 
				TFather::__AppendLast( _vobj ); }
			
			// Push a new object to the head of the list.
			INLINE void PushFront( const _TyObject & _vobj ) { 
				TFather::__AppendHead( _vobj ); }
			
			// Pop the tail object.
			INLINE void PopBack( ) { 
				TFather::__RemoveLast( ); }
			
			// Pop the first object.
			INLINE void PopFront( ) { 
				TFather::__RemoveHead( ); }
			
			// Append an single object to the end of the array list.
			INLINE void Append( const _TyObject & _vobj ) { 
				TFather::__AppendLast( _vobj ); }
			
			// Append other arraylist the end of current one.
			INLINE void Append( const Array_< _TyObject, _TyAlloc > & _array ) {
				for ( Uint32 i = 0; i < _array.Size(); ++i ) 
					TFather::__AppendLast( _array[i] );
			}
			
			// Append a range of objects to the end of current array list.
			template< typename _TyIterator >
			INLINE void Append( _TyIterator _begin, _TyIterator _end ) {
				for ( ; _begin != _end; ++_begin ) 
					TFather::__AppendLast( *_begin );
			}
			
			// Remove specified item.
			INLINE void Remove ( Uint32 _idx ) { 
				TFather::__Remove( _idx ); }
			
			// A loop to remove _count elements start from _start.
			INLINE void Remove ( Uint32 _start, Uint32 _count ) {
				for ( Uint32 i = 0; i < _count; ++i ) 
					TFather::__Remove( _start );
			}
			
			// Insert an object to the specified position.
			INLINE void Insert( const _TyObject & _vobj, Uint32 _idx ) { 
				TFather::__Insert( _vobj, _idx ); }
			
			// Get the specified item
			INLINE _TyObject & operator [] ( Uint32 _idx ) { 
				return TFather::__Get( _idx ); }
			
			// Const version of operator [], get the specified item.
			INLINE const _TyObject & operator [] ( Uint32 _idx ) const { 
				return TFather::__Get( _idx ); }
			
			// Get the last item
			INLINE _TyObject & Last( ) { 
				return TFather::__Get(TFather::__Size() - 1); }
			
			// Get the item count of the array list.
			INLINE Uint32 Size() const { 
				return TFather::__Size(); }
			
			// Check if the array list is empty.
			INLINE bool Empty( ) const { 
				return TFather::__Empty(); }
			
			// Clear the storages' data.
			// The only way is to start a two-layer-loop to release all object.
			INLINE void Clear( ) { 
				TFather::__Clear(); }
		};
		
		// Reference Version of Array List.
		// The Array List is always used to be returned for multiple value.
		template< typename _TyObject, typename _TyAlloc >
		class Array : public Reference< Array_< _TyObject, _TyAlloc > >
		{
		public:
			typedef Reference< Array_< _TyObject, _TyAlloc > >	TFather;
			
		protected:
			// For Null Array
			Array< _TyObject, _TyAlloc >( bool _beNull ) : TFather( false ) 
			{ CONSTRUCTURE; }
			
		public:
			// Default C'Str.
			// Create an empty array list.
			Array< _TyObject, _TyAlloc >( )
				: TFather( true ) { CONSTRUCTURE; }
			
			// Copy C'str
			Array< _TyObject, _TyAlloc >( 
				const Array_< _TyObject, _TyAlloc > & rhs )
				: TFather( true ) { 
					CONSTRUCTURE; 
					TFather::_Handle->_PHandle->Append( rhs ); 
				}
			
			// Default Copy
			Array< _TyObject, _TyAlloc > ( const Array< _TyObject, _TyAlloc > & rhs )
				: TFather( rhs ) { CONSTRUCTURE; }
			// C'Str with array
			template < typename _TyIterator >
			Array< _TyObject, _TyAlloc >( 
			 	_TyIterator _begin, _TyIterator _end )
				: TFather( true ) { 
					CONSTRUCTURE; 
					TFather::_Handle->_PHandle->Append( _begin, _end ); 
				}
			// D'str
			virtual ~Array< _TyObject, _TyAlloc >( ) { DESTRUCTURE; }
			
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
			
			// Append an single object to the end of the array list.
			INLINE void Append( const _TyObject & _vobj ) { 
				TFather::_Handle->_PHandle->Append( _vobj ); }
			
			// Append other arraylist the end of current one.
			INLINE void Append( const Array_< _TyObject, _TyAlloc > & _array ) {
				TFather::_Handle->_PHandle->Append( _array ); }
			
			INLINE void Append( const Array< _TyObject, _TyAlloc > & _rArray ) {
				TFather::_Handle->_PHandle->Append( *(_rArray._Handle->_PHandle) );	}
			
			// Append a range of objects to the end of current array list.
			template< typename _TyIterator >
			INLINE void Append( _TyIterator _begin, _TyIterator _end ) {
				TFather::_Handle->_PHandle->Append( _begin, _end ); }
			
			// Remove specified item.
			INLINE void Remove ( Uint32 _idx ) { 
				TFather::_Handle->_PHandle->Remove( _idx ); }
			
			// A loop to remove _count elements start from _start.
			INLINE void Remove ( Uint32 _start, Uint32 _count ) {
				TFather::_Handle->_PHandle->Remove( _start, _count ); }
			
			// Insert an object to the specified position.
			INLINE void Insert( const _TyObject & _vobj, Uint32 _idx ) { 
				TFather::_Handle->_PHandle->Insert( _vobj, _idx ); }
			
			// Get the specified item
			INLINE _TyObject & operator [] ( Uint32 _idx ) {
				return TFather::_Handle->_PHandle->operator [] ( _idx ); }
			
			// Const version of operator [], get the specified item.
			INLINE const _TyObject & operator [] ( Uint32 _idx ) const { 
				return TFather::_Handle->_PHandle->operator []( _idx ); }
			
			// Get the last Item
			INLINE _TyObject & Last( ) {
				return TFather::_Handle->_PHandle->Last( ); }
			
			// Get the item count of the array list.
			INLINE Uint32 Size() const { 
				return TFather::_Handle->_PHandle->Size(); }
			
			// Check if the array list is empty.
			INLINE bool Empty( ) const { 
				return TFather::_Handle->_PHandle->Empty(); }
			
			// Clear the storages' data.
			// The only way is to start a two-layer-loop to release all object.
			INLINE void Clear( ) { 
				TFather::_Handle->_PHandle->Clear(); }
			
			const static Array< _TyObject, _TyAlloc > Null;
			
			static Array< _TyObject, _TyAlloc > CreateNullArray( ){
				return Array< _TyObject, _TyAlloc >( false );
			}			
		};
		
		template< typename _TyObject, typename _TyAlloc >
			const Array< _TyObject, _TyAlloc > 
				Array< _TyObject, _TyAlloc >::Null( false );
	}
}

#endif // plib.generic.array.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */


