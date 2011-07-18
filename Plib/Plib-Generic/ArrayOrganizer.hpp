/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: ArrayOrganizer.hpp
* Propose  			: An organizer of ArrayBlocks.
* 
* Current Version	: 1.0.0
* Change Log		: Re-organize the ArrayList code. 
* Author			: Push Chen
* Change Date		: 2011-07-02
*/

#ifndef _PLIB_GENERIC_ARRAYORGANIZER_HPP_
#define _PLIB_GENERIC_ARRAYORGANIZER_HPP_

#if _DEF_IOS
#include "ArrayBlock.hpp"
#else
#include <Plib-Generic/ArrayBlock.hpp>
#endif

namespace Plib
{
	namespace Generic
	{
		/*
		 * This class is about to create an array list container.
		 * Using the previous Array_Block_ as internal orginizer.
		 * The ArrayList is going to create an self-maintained Array_Block_ array.
		 * In this array, store all outside objects.
		 */
		template< 
			typename _TyObject, 
			typename _TyAlloc
		>
		class ArrayOrganizer
		{
		public:
			// Common Type definition.
			typedef Array_Block_< _TyObject, AL_BLOCK_SIZE, _TyAlloc >		STORAGE_T;
			typedef Array_Block_< _TyObject, AL_BLOCK_SIZE, _TyAlloc > *	PSTORAGE_T;
		
			typedef _TyAlloc												ITEMALLOC_T;
			typedef typename _TyAlloc::template Rebind< STORAGE_T >::Other	STORAGEALLOC_T;
		protected:
			// Global Allocator.
			static ITEMALLOC_T												g_ItemAlloc;
			static STORAGEALLOC_T											g_StorageAlloc;
		protected:
			// Internal Parameters.
			PSTORAGE_T * 			m_StorageCache;			// the Array_Block_ List. 
															// When need to append, we re-alloc the array.
			PSTORAGE_T				m_TailStorage;			// the Tail Storage Pointer.
			PSTORAGE_T				m_HeadStorage;			// the Head Storage Pointer.
			PSTORAGE_T				m_TailFree;				// The Last Not Full Storage.
			PSTORAGE_T				m_HeadFree;				// The not full storage before the frist element.
			Uint32					m_CacheSize;			// the size of m_StorageCache.
			Uint32					m_CacheUsed;			// the storage used.
			Uint32					m_FirstUnFull;			// The first Not full storage in the list.
			Uint32					m_AllSize;				// Element Count.
		
			PLIB_THREAD_SAFE_DEFINE;
		private:
			INLINE void __CheckStorageCacheSize( ) {
				if ( m_CacheSize > m_CacheUsed ) return;
				// If the storage cache size is too small, realloc it.
				PCREALLOC( PSTORAGE_T, m_StorageCache, _appendCache, 
						sizeof(PSTORAGE_T) * m_CacheSize * 2 );
				assert( _appendCache != NULL );
				m_StorageCache = _appendCache;
				m_CacheSize *= 2;
			}
		
			/*
			 * When the head free storage contains no elements
			 * and the head storage is not full, invoke this method
			 * to release the head free storage.
			 * this method will return the storage space to the 
			 * memory pool.
			 */
			INLINE void __ReleaseHeadFreeStorage( ) {
				assert( m_HeadFree->Count() == 0 );
				g_StorageAlloc.Destroy( m_HeadFree );
				memmove( m_StorageCache, m_StorageCache + 1,
					sizeof( PSTORAGE_T ) * (m_CacheUsed - 1) );
				SELF_DECREASE(m_CacheUsed);
				m_StorageCache[m_CacheUsed] = NULL;
				m_HeadStorage = m_HeadFree = m_StorageCache[0];	
				SELF_DECREASE( m_FirstUnFull );			
			}
		
			/*
			 * When the head storage is full, invoke this method
			 * to alloc a new free storage and add before the head
			 * storage as the head free storage.
			 * The first storage is always head-storage, the 
			 * headfree storage is for quick push-front.
			 */
			INLINE void __AddHeadFreeStorage( ) {
				assert( m_HeadFree->IsFull() );
				memmove( m_StorageCache + 1, m_StorageCache, 
					sizeof( PSTORAGE_T ) * m_CacheUsed );
				// Move the first unfull to the next one.
				SELF_INCREASE(m_FirstUnFull);
				m_HeadFree = g_StorageAlloc.Create( );
				SELF_INCREASE(m_CacheUsed);
				m_StorageCache[0] = m_HeadFree;
			}
		
			/*
			 * Check the tail storage's statue.
			 * This method is only been invoked when the tail storage
			 * contains no elements.
			 */
			INLINE void __CheckTailStatueAndRelease( ) {
				assert( m_TailStorage->Count() == 0 );
				assert( m_TailFree == m_TailStorage );
				if ( m_TailStorage == m_HeadStorage ) return;
				//SELF_DECREASE( m_TailStorage );
				m_TailStorage = m_StorageCache[m_CacheUsed - 2];
				if ( !m_TailStorage->IsFull() ) {
					assert( m_TailFree->Count() == 0 );
					g_StorageAlloc.Destroy( m_TailFree );
					m_TailFree = m_TailStorage;
				}
			}
		
			/*
			 * Check the tail storage's statue.
			 * This method is only been invoked when the tail storage
			 * is full and the tailfree is the same as tail storage.
			 */
			INLINE void __CheckTailStatueAndAppend( ) {
				assert( m_TailStorage->IsFull() );
				assert( m_TailStorage == m_TailFree );
				__CheckStorageCacheSize( );
				if ( m_FirstUnFull == m_CacheUsed - 1 ) SELF_INCREASE( m_FirstUnFull );
				m_StorageCache[m_CacheUsed] = g_StorageAlloc.Create( );
				m_TailFree = m_StorageCache[m_CacheUsed];
				SELF_INCREASE( m_CacheUsed );
			}
		
			/*
			 * Check the head storage's statue.
			 * This method is only been invoked when the head free storage
			 * is the same as head storage, and after one remove from the 
			 * head storage, the storage becomes empty.
			 */
			INLINE void __CheckHeadStatueAndRelease( ) {
				assert( m_HeadStorage->Count() == 0 );
				assert( m_HeadStorage == m_HeadFree );
				if ( m_HeadStorage == m_TailStorage ) return;
				m_HeadStorage = m_StorageCache[1];
				if ( m_HeadStorage->IsFull() ) return;
				__ReleaseHeadFreeStorage( );
			}
		
			/*
			 * Check the head storage's statue.
			 * This method is only been invoked when the head storage
			 * is full and need to add new storege for quick insert.
			 */
			INLINE void __CheckHeadStatueAndAppend( ) {
				assert( m_HeadStorage == m_HeadFree );
				assert( m_HeadStorage->IsFull() );
				__CheckStorageCacheSize();
				__AddHeadFreeStorage( );
			}
		
			// After Append/Insert/Delete an object in the last storage,
			// Invoke this method to update the tail storage's statue.
			INLINE void __UpdateTailFreeStorage( ) {
				if ( m_TailStorage->Count() == 0 )	// Release this storage.
				{
					if ( m_CacheUsed == 1 ) return;
					__CheckTailStatueAndRelease();
					return;
				}
				if ( m_TailStorage->IsFull() )		// Append new storage.
				{
					if ( m_TailStorage == m_TailFree )
					{
						__CheckTailStatueAndAppend();
						return;
					}
					if ( m_TailFree->Count() > 0 )
					{
					 	m_TailStorage = m_TailFree;
						return;
					}
					return;
				}
				if ( m_TailStorage != m_TailFree )	// Release the tail free
				{
					assert( m_TailFree->Count() == 0 );
					g_StorageAlloc.Destroy(m_TailFree);
					m_TailFree = m_TailStorage;
					return;
				}
			}
		
			// After Insert/Delete an object in the first storage,
			// Invoke this method to update head storage's statue.
			INLINE void __UpdateHeadFreeStorage( ) {
				if ( m_HeadStorage->Count() == 0 )
				{
					if ( m_CacheUsed == 1 ) return;
					__CheckHeadStatueAndRelease();
					return;
				}
				if ( m_HeadStorage->IsFull() )
				{
					if ( m_HeadStorage == m_HeadFree ) 
					{
						__CheckHeadStatueAndAppend();
						return;
					}
					if ( m_HeadFree->Count() > 0 ) 
					{
						m_HeadStorage = m_HeadFree;
						m_FirstUnFull = 0;
					}
					return;
				}
				if ( m_HeadStorage != m_HeadFree )
				{
					__ReleaseHeadFreeStorage( );
					return;
				}
			}
		
			// Common Storage checking.
			INLINE void __CheckAndUpdateStorage( ) {
				__UpdateHeadFreeStorage();
				__UpdateTailFreeStorage();
			}
		
			// Split the storage and move half elements
			// to the splited storage.
			void __SplitTheStorageOf( Uint32 _idx ) {
				// Create the new storage and insert to the specified position.
				PSTORAGE_T _newInsertStorage = g_StorageAlloc.Create( );
				assert( _newInsertStorage != NULL );
				__CheckStorageCacheSize( );
				memmove( m_StorageCache + _idx + 1, m_StorageCache + _idx, 
					sizeof( PSTORAGE_T ) * (m_CacheUsed - _idx) );
				m_StorageCache[_idx] = _newInsertStorage;
				SELF_INCREASE( m_CacheUsed );
			
				// Copy Data from the old storage to the new storage.
				Uint16 _halfSize = m_StorageCache[_idx + 1]->Count() / 2;
				for ( Uint16 i = 0; i < _halfSize; ++i ) {
					m_StorageCache[_idx]->Append( m_StorageCache[_idx + 1]->operator [] (0) );
					m_StorageCache[_idx + 1]->Remove( 0 );
				}
			
				// Update the first Unfull.
				if ( _idx < m_FirstUnFull ) m_FirstUnFull = _idx;
			
				// Check the head/tail statue.
				m_HeadFree = m_HeadStorage = m_StorageCache[0];
				if ( m_HeadFree->Count() == 0 ) m_HeadStorage = m_StorageCache[1];
				m_TailFree = m_TailStorage = m_StorageCache[m_CacheUsed - 1];
				if ( m_TailFree->Count() == 0 ) m_TailStorage = m_StorageCache[m_CacheUsed - 2];
			}
		
			// Search all storages to locate the _idx th element.
			// Return the storage index
			// the position in the storage will be returned as
			// the new value of _posInStorage.
			Uint32 __SearchItemOfIndex( Uint32 _idx, Uint32 * _posInStorage ) const {
				assert( _idx < m_AllSize );
				Uint32 _unfullCount = (m_FirstUnFull - (Uint32)(!__IsHeadEqualHeadFree())) * AL_BLOCK_SIZE 
					+ m_StorageCache[m_FirstUnFull]->Count();
				if ( _idx < _unfullCount ) {
					if ( _posInStorage != NULL ) *_posInStorage = _idx % AL_BLOCK_SIZE;
					return _idx / AL_BLOCK_SIZE + ((m_HeadFree != m_HeadStorage) ? 1 : 0);
				}
				Uint32 _searchId = m_FirstUnFull + 1;
				for ( ; _searchId < m_CacheUsed; ++_searchId ) {
					if ( m_StorageCache[_searchId]->Count() + _unfullCount > _idx ) break;
					_unfullCount += m_StorageCache[_searchId]->Count();
				}
				if ( _posInStorage != NULL ) *_posInStorage = (_idx - _unfullCount);
				return _searchId;
			}
		protected:
			
			// Common C'str and D'str
			ArrayOrganizer< _TyObject, _TyAlloc > ( )
				{CONSTRUCTURE; this->__Initialize(); }
			~ArrayOrganizer< _TyObject, _TyAlloc > ( ) { 
				DESTRUCTURE;
				this->__Clear(); 
				g_StorageAlloc.Destroy( m_StorageCache[0] );
				PFREE( m_StorageCache );
			}
			
		protected:
			// Initialize the storage cache.
			INLINE void __Initialize( ) {
				PLIB_THREAD_SAFE;
				m_CacheSize = 8;
				m_CacheUsed = 1;
				m_FirstUnFull = m_AllSize = 0;
				PMALLOC( PSTORAGE_T, m_StorageCache, sizeof(PSTORAGE_T) * m_CacheSize );
				m_StorageCache[0] = g_StorageAlloc.Create( );
				m_HeadStorage = m_HeadFree = m_TailStorage = m_TailFree = m_StorageCache[0];					
			}
			
			// Clear the storage.
			INLINE void __Clear( ) {
				PLIB_THREAD_SAFE;
				// For each storage in this array list
				// erase the items one-by-one in the storage.
				for ( Uint32 i = 0; i < m_CacheUsed; ++i ) {
					for ( Uint32 j = 0; j < m_StorageCache[i]->Count(); ++j ) {
						g_ItemAlloc.Destroy((*m_StorageCache[i])[j]);
					}
					m_StorageCache[i]->Clear();
				}
				// Release the unused storage.
				for ( Uint32 i = 1; i < m_CacheUsed; ++i ) {
					g_StorageAlloc.Destroy( m_StorageCache[i] ); 
				}
				// Reset the flag in the arraylist.
				m_CacheUsed = 1;
				m_HeadFree = m_HeadStorage = m_TailFree = m_TailStorage = m_StorageCache[0];
				m_AllSize = 0;
				m_FirstUnFull = 0;					
			}
			
			// Get the all item size.
			INLINE Uint32 __Size( ) const { return m_AllSize; }
			
			// Check if the array list is empty.
			INLINE Uint32 __Empty( ) const { return m_AllSize == 0; }
			
		protected:
			// Append new object to the end of the list.
			INLINE void __AppendLast( const _TyObject & _vobj ) {
				PLIB_THREAD_SAFE;
				m_TailFree->Append( g_ItemAlloc.Create( _vobj ) );
				__CheckAndUpdateStorage();
				SELF_INCREASE( m_AllSize );					
			}
			
			INLINE void __AppendHead( const _TyObject & _vobj )
			{
				PLIB_THREAD_SAFE;
				m_HeadFree->Insert( g_ItemAlloc.Create( _vobj ), 0 );
				__CheckAndUpdateStorage();
				SELF_INCREASE( m_AllSize );					
			}
			
			INLINE void __RemoveLast( ) {
				PLIB_THREAD_SAFE;
				_TyObject * _pObj = m_TailStorage->operator[] ( m_TailStorage->Count() - 1 );
				m_TailStorage->Remove( m_TailStorage->Count() - 1 );
				g_ItemAlloc.Destroy( _pObj );
				__CheckAndUpdateStorage();
				SELF_DECREASE( m_AllSize );					
			}
			
			INLINE void __RemoveHead( ) {
				PLIB_THREAD_SAFE;
				_TyObject * _pObj = m_HeadStorage->operator[] ( 0 );
				m_HeadStorage->Remove( 0 );
				g_ItemAlloc.Destroy( _pObj );
				__CheckAndUpdateStorage();
				SELF_DECREASE( m_AllSize );					
			}
			
			INLINE void __Insert( const _TyObject & _vobj, Uint32 _idx ) {
				Uint32 _posInStorage = 0;
				if ( _idx == 0 ) {
					__AppendHead( _vobj );
					return;
				}
				if ( _idx >= m_AllSize ) {
					__AppendLast( _vobj );
					return;
				}
				PLIB_THREAD_SAFE;
				Uint32 _storageId = __SearchItemOfIndex( _idx, &_posInStorage );
				// If the found storage is already full, split it and re-search.
				if ( m_StorageCache[_storageId]->IsFull() ) {
					// Insert a new storage block
					__SplitTheStorageOf( _storageId );
					_storageId = __SearchItemOfIndex( _idx, &_posInStorage );
				}
				// Set the value.
				m_StorageCache[_storageId]->Insert( g_ItemAlloc.Create( _vobj ), _posInStorage );

				__CheckAndUpdateStorage();
				SELF_INCREASE(m_AllSize);

				// Check if the first unfull storage has been full.
				if ( _storageId != m_FirstUnFull ) return;
				if ( !m_StorageCache[m_FirstUnFull]->IsFull() ) return;
				for ( Uint32 i = m_FirstUnFull + 1; i < m_CacheUsed; ++i ) {
					if ( m_StorageCache[i]->IsFull() ) continue;
					m_FirstUnFull = i;
					break;
				}					
			}
			
			// Remove the specified object.
			INLINE void __Remove( Uint32 _idx ) {
				Uint32 _posInStorage = 0;
				PLIB_THREAD_SAFE;
				Uint32 _storageId = __SearchItemOfIndex( _idx, &_posInStorage );
				// Release the item.
				_TyObject * _pObj = m_StorageCache[_storageId]->operator [] ( _posInStorage );
				m_StorageCache[_storageId]->Remove( _posInStorage );
				g_ItemAlloc.Destroy( _pObj );

				// Check the first unfull.
				if ( _storageId < m_FirstUnFull ) m_FirstUnFull = _storageId;
				__CheckAndUpdateStorage();
				SELF_DECREASE(m_AllSize);					
			}
			
			// Non-const version of _Get.
			INLINE _TyObject & __Get( Uint32 _idx ) {
				return const_cast< _TyObject & >( 
					static_cast<const ArrayOrganizer< _TyObject, _TyAlloc > &>(*this)
						.__Get( _idx )
				);
			}
			
			// Get the specified position object.
			INLINE const _TyObject & __Get( Uint32 _idx ) const {
				Uint32 _posInStorage = 0;
				PLIB_THREAD_SAFE;
				Uint32 _sId = __SearchItemOfIndex( _idx, &_posInStorage );
				return *(*m_StorageCache[_sId])[_posInStorage];					
			}
			
			// Get the specified storage.
			INLINE PSTORAGE_T __GetStorage( Uint32 _storageId ) { return m_StorageCache[_storageId]; }
			
			// these methods are about to tell if the free storage is the same
			// as the head or tail storage.
			INLINE bool __IsHeadEqualHeadFree( ) const { return m_HeadFree == m_HeadStorage; }
			INLINE bool __IsTailEqualTailFree( ) const { return m_TailFree == m_TailStorage; }
			
			INLINE Uint32 __ItemCountBefore( Uint32 _storageId ) {
				_storageId -= (_storageId == 0) ? 0 : (Uint32)(!__IsHeadEqualHeadFree());
				if ( _storageId == 0 ) return 0;
				if ( _storageId <= m_FirstUnFull ) {
					return (_storageId - 1) * AL_BLOCK_SIZE;
				}
				Uint32 _unfullCount = (m_FirstUnFull - (Uint32)(!__IsHeadEqualHeadFree())) * AL_BLOCK_SIZE 
					+ m_StorageCache[m_FirstUnFull]->Count();
				Uint32 _searchId = m_FirstUnFull + 1;
				for ( ; _searchId < _storageId; ++_searchId ) {
					_unfullCount += m_StorageCache[_searchId]->Count();
				}
				return _unfullCount;
			}
		};
		// Global Static Allocator of any ArrayList.
		template< typename _TyObject, typename _TyAlloc >
		_TyAlloc
			ArrayOrganizer< _TyObject, _TyAlloc >::g_ItemAlloc;
			
		template< typename _TyObject, typename _TyAlloc >
		typename _TyAlloc::template Rebind< Array_Block_< _TyObject, AL_BLOCK_SIZE, _TyAlloc > >::Other
			ArrayOrganizer< _TyObject, _TyAlloc >::g_StorageAlloc;
	}
}

#endif // plib.generic.arrayorganizer.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

