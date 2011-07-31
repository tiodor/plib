/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Order.hpp
* Propose  			: An sorted list based on Array Organized.
* 
* Current Version	: 1.0.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-07-02
*/

#pragma once

#ifndef _PLIB_GENERIC_SORTEDLIST_HPP_
#define _PLIB_GENERIC_SORTEDLIST_HPP_

#if _DEF_IOS
#include "Plib.hpp"
#include "ArrayOrganizer.hpp"
#include "Operator.hpp"
#else
#include <Plib-Basic/Plib.hpp>
#include <Plib-Generic/ArrayOrganizer.hpp>
#include <Plib-Generic/Operator.hpp>
#endif

namespace Plib
{
	namespace Generic
	{
		/*
		 * This class is about to create an sorted list container.
		 * Using the previous Array_Block_ as internal orginizer.
		 * The ArrayList is going to create an self-maintained Array_Block_ array.
		 * In this array, store all outside objects.
		 */
		template< 
			typename _TyObject, 
			typename _TyComp = Less<_TyObject >,
			typename _TyAlloc = Plib::Basic::Allocator< _TyObject >
		>
		class Order_ : public ArrayOrganizer< _TyObject, _TyAlloc >
		{
		protected:
			typedef ArrayOrganizer< _TyObject, _TyAlloc >	TFather;
			// the compare operator
			_TyComp				m_Comp;
			
			PLIB_THREAD_SAFE_DEFINE;
		protected:
				
			// If the array is sorted, you can invoke this method
			// to check if the given object is in the specified storage.
			INLINE Uint32 __IsObjectIn( const _TyObject & _vobj, Uint32 _storageId ) {
				typename TFather::PSTORAGE_T _ps = TFather::__GetStorage( _storageId );
				bool _checkFirst = m_Comp( *(_ps->operator[] (0)), _vobj );
				bool _checkLast = m_Comp( *(_ps->operator[] (_ps->Count() - 1)), _vobj );
				
				return ( _checkFirst && _checkLast ) ? 1 
					: (_checkFirst || _checkLast) ? 0 : -1;
			}

			INLINE Uint32 __Binary( 
					const _TyObject & _vobj, 
					const typename TFather::STORAGE_T & storage, 
					Uint32 _begin, Uint32 _end )
			{
				if ( _begin >= _end ) return _begin;
				Uint32 _half = ( _begin + _end ) / 2;
				if ( _half == _begin ) return ( 
					m_Comp( *storage[_begin], _vobj ) ? _begin + 1 : _begin 
				);
				
				if ( m_Comp(*storage[_half], _vobj) ) {
					return __Binary( _vobj, storage, _half, _end );
				} else {
					return __Binary( _vobj, storage, _begin, _half );
				}
			}

			INLINE Uint32 __BinaryFind( const _TyObject & _vobj, Uint32 _storageId )
			{
				Uint32 _beforeCount = TFather::__ItemCountBefore( _storageId );
				typename TFather::PSTORAGE_T pStorage = TFather::__GetStorage( _storageId );
				return _beforeCount + __Binary( _vobj, *pStorage, 0, pStorage->Count() );
			}
			
			INLINE Uint32 __BinarySearch( 
				const _TyObject & _vobj, 
				Uint32 _begin, 
				Uint32 _end 
			) {
				Uint32 _halfStorage = (_begin + _end) / 2;
				if ( _halfStorage == _begin ) return __BinaryFind( _vobj, _begin );
				//if ( _halfStorage == _end ) return __BinaryFind( _vobj, _end );
				
				// Search.
				int _halfRet = __IsObjectIn( _vobj, _halfStorage );
				if ( _halfRet == 0 ) return __BinaryFind( _vobj, _halfStorage );
				if ( _halfRet > 0 ) return __BinarySearch( _vobj, _halfStorage + 1, _end );
				if ( _halfRet < 0 ) return __BinarySearch( _vobj, _begin, _halfStorage - 1 );
				
				return 0;
			}
		public:
			// Default C'Str.
			// Create an empty array list.
			Order_< _TyObject, _TyComp, _TyAlloc >( )
				: TFather( ) { }
			
			// Copy C'str
			Order_< _TyObject, _TyComp, _TyAlloc >( 
				const Order_< _TyObject, _TyComp, _TyAlloc > & rhs )
				: TFather( ) { this->SortInsert( rhs ); }
			
			// C'Str with array
			template < typename _TyIterator >
			Order_< _TyObject,  _TyComp, _TyAlloc >( 
				_TyIterator _begin, _TyIterator _end )
				: TFather( ) { this->SortInsert( _begin, _end ); }
			
			// Remove specified item.
			INLINE void Remove ( Uint32 _idx ) { TFather::__Remove( _idx ); }
			
			// A loop to remove _count elements start from _start.
			INLINE void Remove ( Uint32 _start, Uint32 _count ) {
				for ( Uint32 i = 0; i < _count; ++i ) TFather::__Remove( _start );
			}
			
			// Find if the object is in the list.
			// This method is not very effective.
			INLINE Uint32 Find( const _TyObject & _vobj ) {
				PLIB_THREAD_SAFE;
				if ( TFather::m_AllSize == 0 ) return (Uint32)-1;
				Uint32 _start = TFather::__IsHeadEqualHeadFree() ? 0 : 1;
				Uint32 _end = TFather::__IsTailEqualTailFree() ? 
					TFather::m_CacheUsed: 
					TFather::m_CacheUsed - 1;
				Uint32 _pos = __BinarySearch( _vobj, _start, _end );
				if ( TFather::__Get( _pos ) == _vobj ) return _pos;
				return (Uint32)-1;
			}
			
			// Sort and insert the new object to the list.
			INLINE void SortInsert( const _TyObject & _vobj ) {
				PLIB_THREAD_SAFE;
				Uint32 _start = TFather::__IsHeadEqualHeadFree() ? 0 : 1;
				Uint32 _end = TFather::__IsTailEqualTailFree() ? 
					TFather::m_CacheUsed: 
					TFather::m_CacheUsed - 1;
				Uint32 _pos = (TFather::m_AllSize == 0) ? 0 : 
					__BinarySearch( _vobj, _start, _end );
				TFather::__Insert( _vobj, _pos );
			}
			
			INLINE void SortInsert( const Order_< _TyObject, _TyComp, _TyAlloc > & rhs ) {
				for ( Uint32 i = 0; i < rhs.Size(); ++i ) {
					this->SortInsert( rhs[i] );
				}
			}
			
			template < typename _TyIterator >
			INLINE void SortInsert( _TyIterator _begin, _TyIterator _end ) {
				for ( ; _begin != _end; ++_begin ) this->Sortisert( *_begin );
			}
			
			// Non-const version. return the copy of the object.
			INLINE _TyObject operator [] ( Uint32 _idx ) {
				return TFather::__Get( _idx );
			}
			// Const version of operator [], get the specified item.
			INLINE const _TyObject & operator [] ( Uint32 _idx ) const { 
				return TFather::__Get( _idx ); }
			
			// Get the item count of the array list.
			INLINE Uint32 Size() const { return TFather::__Size(); }
			
			// Check if the array list is empty.
			INLINE bool Empty( ) const { return TFather::__Empty(); }
			
			// Clear the storages' data.
			// The only way is to start a two-layer-loop to release all object.
			INLINE void Clear( ) { TFather::__Clear(); }
		};
		
		// Reference Version of Order_
		// The Array List is always used to be returned for multiple value.
		template< typename _TyObject, typename _TyAlloc >
		class Order : public Reference< Order_< _TyObject, _TyAlloc > >
		{
		public:
			typedef Reference< Order_< _TyObject, _TyAlloc > >	TFather;
			
		protected:
			// For Null Order
			Order< _TyObject, _TyAlloc >( bool _beNull ) : TFather( false ) 
			{ CONSTRUCTURE; }
			
		public:
			// Default C'Str.
			// Create an empty array list.
			Order< _TyObject, _TyAlloc >( )
				: TFather( true ) { CONSTRUCTURE; }
			
			// Copy C'str
			Order< _TyObject, _TyAlloc >( 
				const Order_< _TyObject, _TyAlloc > & rhs )
				: TFather( true ) { 
					CONSTRUCTURE; 
					TFather::_Handle->_PHandle->SortInsert( rhs ); 
				}
			
			// Default Copy
			Order< _TyObject, _TyAlloc > ( const Order< _TyObject, _TyAlloc > & rhs )
				: TFather( rhs ) { CONSTRUCTURE; }
			// C'Str with array
			template < typename _TyIterator >
			Order< _TyObject, _TyAlloc >( 
			 	_TyIterator _begin, _TyIterator _end )
				: TFather( true ) { 
					CONSTRUCTURE; 
					TFather::_Handle->_PHandle->SortInsert( _begin, _end ); 
				}
			// D'str
			virtual ~Order< _TyObject, _TyAlloc >( ) { DESTRUCTURE; }
			
			// Sort and insert the new object to the list.
			INLINE void SortInsert( const _TyObject & _vobj ) { 
				TFather::_Handle->_PHandle->SortInsert( _vobj ); }

			INLINE void SortInsert( const Order< _TyObject, _TyAlloc > & _rOrder ) {
				TFather::_Handle->_PHandle->SortInsert( *(_rOrder._Handle->_PHandle) );	}
			
			template< typename _TyIterator >
			INLINE void SortInsert( _TyIterator _begin, _TyIterator _end ) {
				TFather::_Handle->_PHandle->Append( _begin, _end ); }
				
			// Find if the object is in the list.
			// This method is not very effective.
			INLINE Uint32 Find( const _TyObject & _vobj ) {
				return TFather::_Handle->_PHandle->Find( _vobj ); }
				
			// Remove specified item.
			INLINE void Remove ( Uint32 _idx ) { 
				TFather::_Handle->_PHandle->Remove( _idx ); }
			
			// A loop to remove _count elements start from _start.
			INLINE void Remove ( Uint32 _start, Uint32 _count ) {
				TFather::_Handle->_PHandle->Remove( _start, _count ); }
			
			// Non const version of operator [], return the copy object.
			INLINE _TyObject operator [] ( Uint32 _idx ) {
				return TFather::_Handle->_PHandle->operator [] ( _idx ); }
			// Const version of operator [], get the specified item.
			INLINE const _TyObject & operator [] ( Uint32 _idx ) const { 
				return TFather::_Handle->_PHandle->operator []( _idx ); }
						
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
			
			const static Order< _TyObject, _TyAlloc > Null;
			
			static Order< _TyObject, _TyAlloc > CreateNullOrder( ){
				return Order< _TyObject, _TyAlloc >( false );
			}			
		};
		
		template< typename _TyObject, typename _TyAlloc >
			const Order< _TyObject, _TyAlloc > 
				Order< _TyObject, _TyAlloc >::Null( false );
	}
}

#endif // plib.generic.arraylist.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */


