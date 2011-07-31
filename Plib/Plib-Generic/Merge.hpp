/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Merge.hpp
* Propose  			: Merge multiple data into one.
* 
* Current Version	: 1.1
* Change Log		: Redefine the old merger.
* Author			: Push Chen
* Change Date		: 2011-07-02
*/

#pragma once

#ifndef _PLIB_GENERIC_MERGE_HPP_
#define _PLIB_GENERIC_MERGE_HPP_

#if _DEF_IOS
#include "ArrayList.hpp"
#include "Order.hpp"
#include "Pair.hpp"
#else
#include <Plib-Generic/ArrayList.hpp>
#include <Plib-Generic/Order.hpp>
#include <Plib-Generic/Pair.hpp>
#endif

namespace Plib
{
	namespace Generic
	{
		template < typename _TyIter, typename _Compare > 
		class Merge {
		public:
			// Typedef
			typedef Pair< _TyIter, _TyIter > 			ATOMNODE_T;
			typedef Array< _TyIter >					RESULT_T;
			typedef Order< ATOMNODE_T, _Compare >		SORTLIST_T;
		protected:
			RESULT_T									m_ResultArray;
			SORTLIST_T									m_SortArray;
		public:
			// Add new list to this merge object to be merged.
			// The input list must be sorted.
			INLINE void Add( _TyIter _begin, _TyIter _end ) {
				assert( _begin != _end );
				m_SortArray.SortInsert( ATOMNODE_T(_begin, _end) );
			}
			
			// Do the merge job. the result is limited to the _limit.
			// if the _limit is 0, merge all the node added to this object.
			void Do( bool _distinguish = false, Uint32 _limit = 0 ) {
				if ( _limit == 0 ) _limit = (Uint32)-1;	// Make the _limit to be the max unsigned integer.
				while ( m_ResultArray.Size() < _limit && m_SortArray.Size() > 0 )
				{
					// Get the first node from the sorted list.
					ATOMNODE_T _node = m_SortArray[0];
					m_SortArray.Remove(0);
					
					if ( _distinguish == false || m_ResultArray.Empty() || 
						!(*_node.First == *m_ResultArray.Last()) )  
					{
						// Append the node to the result list.
						m_ResultArray.PushBack( _node.First );
					}
					
					// Move to the next node.
					SELF_INCREASE( _node.First );
					if ( _node.First == _node.Second ) continue;
					
					// if not arrive the end of this list, insert to
					// the sort list again.
					m_SortArray.SortInsert( _node );
				}
			}
			
			// Clear the merge result.
			INLINE void Clear( ) { m_ResultArray.Clear(); m_SortArray.Clear(); }
			
			// Get the size of the merged result.
			INLINE Uint32 Size( ) { return m_ResultArray.Size(); }
			
			// Get the specified position result.
			INLINE _TyIter operator [] ( Uint32 _idx ) { return m_ResultArray[_idx]; }
		};
	}
}

#endif // plib.generic.merge.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

