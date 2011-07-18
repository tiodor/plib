/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: ArrayBlock.hpp
* Propose  			: An Internal Block Object for array list used.
* 
* Current Version	: 1.0.2
* Change Log		: Re-orgenized file position.
* Author			: Push Chen
* Change Date		: 2011-07-02
*/

#pragma once

#ifndef _PLIB_GENERIC_ARRAYBLOCK_HPP_
#define _PLIB_GENERIC_ARRAYBLOCK_HPP_

#if _DEF_IOS
#include "Allocator.hpp"
#else
#include <Plib-Basic/Allocator.hpp>
#endif

namespace Plib
{
	namespace Generic
	{
		#pragma pack(1)
	
		// This is the basic item node in all type array list.
		// lpVal_ is the pointer point to the real object allocated
		// by the Allocator.
		// The other two integers mean the node's brother nodes position
		// in the block.
		template < typename _TyObject > struct ARRAY_ITEM_NODE_ {
			_TyObject * 			lpVal;
			Uint16					prev;
			Uint16					next;
		};
	
		#pragma pack()
	
		// Each Array_Block_'s size.
		enum { AL_BLOCK_SIZE = 256 };
		
		// Pre-define.
		template< 
			typename _TyObject,
			Uint32 _BSize,
			typename _TyAlloc
		> class Array_Block_;
			
		template< 
			typename _TyObject, 
			typename _TyAlloc = Plib::Basic::Allocator< _TyObject >
		>
		class ArrayOrganizer;
	
		// This is the internal storage of any array list objects.
		// Do not use this class in your logic code.
		template < typename _TyObject, Uint32 _BSize, typename _TyAlloc > 
		class Array_Block_ {
		protected:
			
			typedef ARRAY_ITEM_NODE_< _TyObject >		TItemNode;
			enum { ERROR_POINT = (Uint16)-1 };
		private:
			mutable TItemNode				m_Block[_BSize];
			bool							m_Bitmap[_BSize];
			Uint32							m_Count;
		
			mutable Uint16					m_First;
			mutable Uint16		 			m_Last;
			mutable Uint16					m_FirstFree;
			mutable Uint16					m_LastChanged;
		
		private:
			// Do a loop from current node, find the first 'false' in the bitmap.
			// the new first free room in the block will be m_FirstFree += _delta %= _BSize
			INLINE void __FindNextFree( ) {
				if ( m_Count == _BSize ) { m_FirstFree = ERROR_POINT; return; }
				for ( Uint32 _delta = 0; _delta < _BSize; ++_delta ) {
					if ( m_Bitmap[ (m_FirstFree + _delta) % _BSize ] == true ) continue;
					(m_FirstFree += _delta) %= _BSize;
					break;
				}
			}
		
			// Switch two given item in the block.
			// The two item must not be the same one.
			// Also, this method will re-organize the order of these two items
			INLINE void __SwitchTwoItems( Uint16 _first, Uint16 _second ) const {
				if ( _first == _second || _first == ERROR_POINT || _second == ERROR_POINT ) return;
			
				// Information Collection.
				bool _is_first_the_father_of_second = (m_Block[_first].next == _second);
				bool _is_second_the_father_of_first = (m_Block[_second].next == _first);
				TItemNode * _first_prev = (m_Block[_first].prev == ERROR_POINT) ? 
					NULL : m_Block + m_Block[_first].prev;
				TItemNode * _second_prev = (m_Block[_second].prev == ERROR_POINT) ?
					NULL : m_Block + m_Block[_second].prev;
				TItemNode * _first_next = (m_Block[_first].next == ERROR_POINT) ?
					NULL : m_Block + m_Block[_first].next;
				TItemNode * _second_next = (m_Block[_second].next == ERROR_POINT) ?
					NULL : m_Block + m_Block[_second].next;
			
				// Switch Core Code
				_TyObject * _ptemp = m_Block[_first].lpVal;
				m_Block[_first].lpVal = m_Block[_second].lpVal;
				m_Block[_second].lpVal = _ptemp;
				m_Block[_first].next ^= m_Block[_second].next;
				m_Block[_second].next ^= m_Block[_first].next;
				m_Block[_first].next ^= m_Block[_second].next;				
				m_Block[_first].prev ^= m_Block[_second].prev;
				m_Block[_second].prev ^= m_Block[_first].prev;
				m_Block[_first].prev ^= m_Block[_second].prev;				
			
				// New First = _second
				// New Second = _first
				// Re-organize the order
				if ( _is_first_the_father_of_second ) {
					m_Block[_first].prev = _second;
					m_Block[_second].next = _first;
					if ( _first_prev != NULL ) _first_prev->next = _second;
					if ( _second_next != NULL ) _second_next->prev = _first;
				}
				else if ( _is_second_the_father_of_first ) {
					m_Block[_second].prev = _first;
					m_Block[_first].next = _second;
					if ( _first_next != NULL ) _first_next->prev = _second;
					if ( _second_prev != NULL ) _second_prev->next = _first;
				}
				else {
					if ( _first_prev != NULL ) _first_prev->next = _second;
					if ( _first_next != NULL ) _first_next->prev = _second;
					if ( _second_prev != NULL ) _second_prev->next = _first;
					if ( _second_next != NULL ) _second_next->prev = _first;
				}
			
				if ( _first == m_Last ) { m_Last = _second; return; }
				if ( _second == m_Last ) { m_Last = _first; return; }
			}
		
			// Sort the items in the range.
			// _from must be less than _to.
			INLINE void __SortItem( Uint16 _from, Uint16 _to ) const {
				assert( _from <= _to );
				TItemNode * _pStart = (_from == 0) ? NULL : &m_Block[_from - 1];
				for ( Uint16 i = _from; i <= _to; ++i ) {
					if ( _pStart == NULL ) {
						__SwitchTwoItems( 0, m_First );
						m_First = 0;
						_pStart = m_Block;
						continue;
					}
					if (_pStart->next != i ) {
						__SwitchTwoItems( _pStart->next, i );
					}
					if ( m_Block[i].next == ERROR_POINT ) break;
					_pStart = &m_Block[i];
				}
				m_LastChanged = _to + 1;
			}
		
			// Add the item to the first free room in the block.
			INLINE void __SetItemToNextFree( _TyObject * _pobj ) {
				m_Block[m_FirstFree].lpVal = _pobj;
				m_Block[m_FirstFree].next = m_Block[m_FirstFree].prev = ERROR_POINT;
				m_Bitmap[m_FirstFree] = true;
				SELF_INCREASE( m_Count );
			}
			
			// Reset a node to be unused.
			INLINE void __ResetNode( Uint32 _idx ) {
				m_Bitmap[_idx] = false;						
				m_Block[ _idx ].lpVal = NULL;
				m_Block[ _idx ].next = m_Block[ _idx ].prev = ERROR_POINT;					
			}
		
		public:
			// The default c'str.
			Array_Block_< _TyObject, _BSize, _TyAlloc >( ) 
				: m_Count( 0 ), m_First( ERROR_POINT ), m_Last( ERROR_POINT ), 
				  m_FirstFree( 0), m_LastChanged( ERROR_POINT )
			{
				CONSTRUCTURE;
				::memset( m_Bitmap, 0, _BSize );
			}
			~Array_Block_<_TyObject, _BSize, _TyAlloc >( )
			{
				DESTRUCTURE;
			}
	
			// Check if the block stroage is full of elements.
			INLINE bool IsFull( ) const { return ( m_Count == _BSize ); }
		
			// Get the count of the block storage.
			INLINE Uint16 Count( ) const { return m_Count; }
		
			// Clear all data.
			INLINE void Clear( ) { 
				m_Count = m_FirstFree = 0;
				m_First = m_Last = m_LastChanged = ERROR_POINT;
				::memset( m_Bitmap, 0, _BSize );
			}
		
			// Insert one new object to the block storage before _idx.
			// if _idx is equal to 0, means insert at the first of the list.
			// _idx must be less than m_Count.
			// if _idx is equal to the Count, than invoke the append method.
			INLINE TItemNode * Insert( _TyObject * _pobj, Uint16 _idx ) {
				if ( _idx >= m_Count ) return Append( _pobj ); 
				
				if ( _idx != 0 && m_LastChanged <= _idx ) __SortItem(m_LastChanged, _idx);
				this->__SetItemToNextFree( _pobj );
				TItemNode * pRetNode = m_Block + m_FirstFree;
				
				if ( _idx == 0 ) {
					// Insert the first object.
					if ( m_Last == ERROR_POINT ) m_Last = m_FirstFree;
					if ( m_First != ERROR_POINT ) m_Block[m_First].prev = m_FirstFree;
					pRetNode->prev = ERROR_POINT;
					pRetNode->next = m_First;
					m_First = m_FirstFree;
				}
				else {
					TItemNode * pNext = &m_Block[_idx];
					TItemNode * pPrev = &m_Block[m_Block[_idx].prev];
					pRetNode->prev = m_Block[_idx].prev;
					pRetNode->next = _idx;
					pNext->prev = m_FirstFree;
					pPrev->next = m_FirstFree;
				}
			
				m_LastChanged = _idx;
				__FindNextFree( );
				return pRetNode;
			}
		
			// Append the new object at the end of the list.
			INLINE TItemNode * Append( _TyObject * _pobj ) {
				this->__SetItemToNextFree( _pobj );
				if ( m_First == ERROR_POINT ) m_First = m_FirstFree;
				if ( m_Last != ERROR_POINT ) m_Block[m_Last].next = (Uint16)m_FirstFree;
				m_Block[m_FirstFree].prev = m_Last;
				m_Last = m_FirstFree;
				__FindNextFree( );
				return m_Block + m_Last;
			}
		
			// Remove the item at the position of _idx.
			INLINE void Remove( Uint16 _idx ) {
				assert( _idx < m_Count );
				SELF_DECREASE( m_Count );
				if ( m_Count == 0 ) { Clear(); return; }
				if ( _idx == 0 ) {	// Remove the head item.
					Uint16 _tmpFirst = m_First;
					m_First = m_Block[_tmpFirst].next;
					m_Block[m_First].prev = ERROR_POINT;
					m_LastChanged = 0;
					__ResetNode( _tmpFirst );
				} else if ( _idx >= (m_Count - 1) ) { // Remove the last item.
					Uint16 _tmpLast = m_Last;
					m_Last = m_Block[_tmpLast].prev;
					m_Block[m_Last].next = ERROR_POINT;
					__ResetNode( _tmpLast );
				} else {
					if ( m_LastChanged <= _idx ) __SortItem( m_LastChanged, _idx );
					m_Block[ _idx - 1].next = _idx + 1;
					m_Block[ _idx + 1].prev = _idx - 1;
					m_LastChanged = _idx;
					__ResetNode( _idx );
				}
				if ( m_FirstFree == ERROR_POINT ) m_FirstFree = _idx;
			}
		
			// Get the internal value point.
			INLINE _TyObject * operator [] ( Uint16 _idx ) {
				return const_cast< _TyObject * >(
					static_cast< const Array_Block_< _TyObject, _BSize, _TyAlloc > & >(*this)
					 	[_idx]
						);
			}
		
			INLINE const _TyObject * operator [] ( Uint16 _idx ) const {
				if ( _idx >= m_Count ) return NULL;
				if ( m_LastChanged > _idx ) return m_Block[_idx].lpVal;
				if ( _idx == m_Count - 1 ) return m_Block[m_Last].lpVal;
				if ( _idx == 0 ) return m_Block[m_First].lpVal;
				__SortItem( m_LastChanged, _idx );
				return m_Block[_idx].lpVal;
			}
		};
	}
}

#endif // plib.generic.arrayblock.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

