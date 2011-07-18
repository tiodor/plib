/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: locker.hpp
* Propose  			: Redefinition the mutex object.
* 
* Current Version	: 1.1
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-01-10
*/


#pragma once

#ifndef _PLIB_THREAD_LOCKER_HPP_
#define _PLIB_THREAD_LOCKER_HPP_

#if _DEF_IOS
#include "Pool.hpp"
#else
#include <Plib-Generic/Pool.hpp>
#endif
#include <map>

namespace Plib
{
	namespace Threading
	{
#if _DEF_WIN32
		typedef ::CRITICAL_SECTION	MutexHandleT;
#else
		typedef pthread_mutex_t		MutexHandleT;
#endif

		// For semphore to use the internal handle.
		class Semaphore;

		class Mutex
		{
		protected:
			friend class Semaphore;
			MutexHandleT m_Mutex;
		public:
			// Create a mutex
			Mutex( ) {
	#if _DEF_WIN32
		#if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0403)
				// for better performance.
				::InitializeCriticalSectionAndSpinCount( &m_Mutex, 4000 );
		#else
				::InitializeCriticalSection( &m_Mutex );
		#endif
	#else
				pthread_mutex_init(&m_Mutex, NULL);
	#endif
			}
			~Mutex(){
	#if defined WIN32 || defined _WIN32
				::DeleteCriticalSection( &m_Mutex );
	#else
				pthread_mutex_destroy(&m_Mutex);
	#endif
			}

			// Lock the mutex.
			INLINE void Lock()
			{
	#if _DEF_WIN32
				::EnterCriticalSection( &m_Mutex );
	#else 
				pthread_mutex_lock(&m_Mutex);
	#endif
			}

			// Unlock the mutex
			INLINE void UnLock()
			{
	#if _DEF_WIN32
				::LeaveCriticalSection( &m_Mutex );
	#else
				pthread_mutex_unlock(&m_Mutex);
	#endif
			}

			INLINE bool TryLock()
			{
	#if _DEF_WIN32
		#if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0400)
				return ::TryEnterCriticalSection( &m_Mutex ) != 0;
		#else
				return false;
		#endif
	#else
				return pthread_mutex_trylock(&m_Mutex) != 0;
	#endif
			}
		};

		// Wrap for Mutex
		typedef LockerT< Mutex >	Locker;

		// Multi-Read/Single-Write Lock
		class RWLock
		{
		protected:
	#if _DEF_WIN32
			// In Windows Need 3 mutex to implement the rwlock.
			Mutex m_writeMutex;
			Mutex m_readMutex;

			// Lock for the read count.
			Mutex m_readCountMutex;
			volatile Int64 m_readCount;

			bool m_isWriting;

			// Check if is writing.
			void _chkWriteStatue()
			{
				m_writeMutex.Lock();
				m_writeMutex.UnLock();
			}

			// Increase read count
			// If read count is now zero, lock the mutex.
			void _incReadCount()
			{
				m_readCountMutex.Lock();
				if (! m_readCount ) m_readMutex.Lock();
				SELF_INCREASE(m_readCount);
				m_readCountMutex.UnLock();
			}

			// Decrease the read count
			// if the count become zero, unlock the mutex.
			void _decReadCount()
			{
				m_readCountMutex.Lock();
				if (! (SELF_DECREASE(m_readCount)) ) m_readMutex.UnLock();
				m_readCountMutex.UnLock();
			}
	#else
			pthread_rwlock_t m_rwLock;
	#endif

		public:

			RWLock()
	#if _DEF_WIN32
				: m_readCount(0), m_isWriting(false)
			{
	#else
			{
				pthread_rwlock_init(&m_rwLock, NULL);
	#endif
			}

			~RWLock()
			{
	#if _DEF_WIN32
	#else
				pthread_rwlock_destroy(&m_rwLock);
	#endif
			}

			INLINE bool ReadLock()
			{
	#if _DEF_WIN32
				_chkWriteStatue();
				_incReadCount();
	#else
				if ( 0 == pthread_rwlock_rdlock(&this->m_rwLock) ) return false;
	#endif
				return true;
			}
			INLINE void WriteLock()
			{
	#if _DEF_WIN32
				m_writeMutex.Lock();
				m_readMutex.Lock();
				m_isWriting = true;
	#else
				pthread_rwlock_wrlock(&this->m_rwLock);
	#endif
			}
			INLINE void Upgrade()
			{
				
			}
			INLINE void UnLock()
			{
	#if _DEF_WIN32
				if ( m_isWriting )
				{
					m_isWriting = false;
					m_readMutex.UnLock();
					m_writeMutex.UnLock();
				}
				else
					_decReadCount();
	#else
				pthread_rwlock_unlock(&this->m_rwLock);
	#endif
			}
		};

		// Read/Write Lock Wrap
		class ReadLocker
		{
			RWLock & m_locker;
		public:
			ReadLocker( RWLock & _l ) : m_locker( _l )
			{ m_locker.ReadLock( ); }
			~ReadLocker( ) { m_locker.UnLock( ); }
		};

		class WriteLocker
		{
			RWLock & m_locker;
		public:
			WriteLocker( RWLock & _l ) : m_locker( _l )
			{ m_locker.WriteLock( ); }
			~WriteLocker( ) { m_locker.UnLock( ); }
		};
		
		// Multi-Lock
		// To lock a many to many accessing.
		// When you have lots of threads and lots of resources, 
		// each thread will access one resource at a time.
		// Only when two or more thread try to access the same
		// resource, the resource object must be locked, otherwise,
		// the accessing is lock-free.
		template < typename _TyIdentify, typename _TyLocker = Mutex >
		class ResLock
		{
		protected:
			typedef Plib::Generic::RPool< _TyLocker > 			LockPoolT;
			typedef Plib::Generic::Pair< Uint32, _TyLocker * >	RefLockT;
			typedef std::map< _TyIdentify, _TyLocker * >		IdMapT;
			
			IdMapT									m_IdentifyMap;
			LockPoolT								m_LockerPool;
			_TyLocker								m_Locker;
			
		public:
			
			INLINE void Lock( const _TyIdentify & _Id )
			{
				//LockerT< _TyLocker > _L( m_Locker );
				m_Locker.Lock( );
				typename IdMapT::iterator idmIt = m_IdentifyMap.find( _Id );
				if ( idmIt == m_IdentifyMap.end() ) {
					_TyLocker & _theLock = m_LockerPool.Get( );
					_theLock.Lock();
					m_IdentifyMap[ _Id ] = RefLockT( 1, &_theLock );
					m_Locker.UnLock( );
					return;
				} else {
					SELF_INCREASE( idmIt->second.first );
					m_Locker.UnLock();
				}
				
				idmIt->second.second->Lock();
			}
			
			INLINE void UnLock( const _TyIdentify & _Id )
			{
				m_Locker.Lock( );
				typename IdMapT::iterator idmIt = m_IdentifyMap.find( _Id );
				if ( idmIt != m_IdentifyMap.end() )
				{
					SELF_DECREASE( idmIt->second.first );
					idmIt->second.second->UnLock();
					
					if ( idmIt->second.first == 0 ) {
						m_LockerPool.Return( *idmIt->second.second );
						m_IdentifyMap.erase( idmIt );
					}
				}
				m_Locker.UnLock( );
			}
		};
	}
}

#endif // plib.thread.lock.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */


