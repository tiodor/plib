/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: semaphore.hpp
* Propose  			: Redefinition the semaphore object.
* 
* Current Version	: 1.1
* Change Log		: Re-Definition.
* Author			: Push Chen
* Change Date		: 2011-01-10
*/

#pragma once

#ifndef _PLIB_THREAD_SEMAPHORE_HPP_
#define _PLIB_THREAD_SEMAPHORE_HPP_

#if _DEF_IOS
#include "Locker.hpp"
#else
#include <Plib-Threading/Locker.hpp>
#endif

namespace Plib
{
	namespace Threading
	{
	#if _DEF_WIN32
		typedef void *			SemHandleT;
	#else
		typedef pthread_cond_t  SemHandleT;
	#endif

		// Semaphore 
		class Semaphore {
		public:
			enum { MAXCOUNT = 0x0FFF, MAXTIMEOUT = 0xFFFFFFFF };

		protected:

			// SemHandle
			SemHandleT			m_Sem;
			Int32				m_Max;
			volatile Int32		m_Current;
			volatile bool		m_Available;

			// Mutex to lock the m_Current.
			Mutex				m_Mutex;
	#if !(_DEF_WIN32)
			// Cond Mutex.
			pthread_condattr_t m_CondAttr;
	#endif

		public:
			// Constructor
			Semaphore() : m_Max(0), m_Current(0), m_Available(false) { }
			Semaphore( unsigned int nInit, unsigned int nMax = MAXCOUNT )
				: m_Max(0), m_Current(0), m_Available(false) {
					this->Init(nInit, nMax);
			}
			~Semaphore()
			{
				this->Destroy();
			}
			// Return current count of the semaphore
			INLINE Uint32 Count() {
				Locker _Lock( m_Mutex );
				return m_Current;
			}
			// Get the semaphore with specified timeout
			INLINE bool Get( Uint32 nTimeOut = MAXTIMEOUT ){
				if (!Statue()) return false;
	#if defined WIN32 || defined _WIN32
				if (::WaitForSingleObject(m_Sem, nTimeOut) != 0 ) return false;
	#else
				Locker _Locker( m_Mutex );
				if ( m_Current > 0 ) 
				{
					--m_Current;
					return true;
				}
				int err;
				if ( nTimeOut == MAXTIMEOUT ) {
					while( m_Current == 0 ) 
					{
						if (pthread_cond_wait(&m_Sem, &m_Mutex.m_Mutex) == EINVAL) {
							return false;
						}
					}
					m_Current -= 1;
					return true;
				}
				else {
					struct timespec ts;
					struct timeval  tv;

					gettimeofday( &tv, NULL );
					ts.tv_nsec = tv.tv_usec * 1000 + ( nTimeOut % 1000 ) * 1000000;
					int _OP = (ts.tv_nsec / 1000000000);
					if ( _OP ) ts.tv_nsec %= 1000000000;
					ts.tv_sec = tv.tv_sec + nTimeOut / 1000 + _OP; 
					while( m_Current == 0 )
					{
						err = pthread_cond_timedwait(&m_Sem, &m_Mutex.m_Mutex, &ts);
						// On Time Out or Invalidate Object.
						if ( err == ETIMEDOUT || err == EINVAL ) {
							return false;
						}
					}
					m_Current -= 1;
					return true;
				}
	#endif	
	#if _DEF_WIN32
				Locker _Lock( m_Mutex );
				::InterlockedDecrement((LONG *)&m_Current);
				return (m_Current >= 0);
	#endif	
			}

			// Release a semaphore
			INLINE bool Release(){
				if ( !Statue() ) return false;
				Locker _Lock( m_Mutex );
				if ( m_Current == this->m_Max ) {
					return false;
				}
	#if _DEF_WIN32
				::InterlockedIncrement((LONG *)&m_Current);
				::ReleaseSemaphore(m_Sem, 1, NULL);
	#else
				++m_Current;
				pthread_cond_signal(&m_Sem);
	#endif
				return true;
			}

			// Init the semaphore.
			INLINE void Init( unsigned int nInit, unsigned int nMax = MAXCOUNT )
			{
				Destroy();
	#if _DEF_WIN32
				m_Sem = ::CreateSemaphore(NULL, nInit, nMax, NULL);
	#else
				pthread_condattr_init(&m_CondAttr);
				pthread_cond_init(&m_Sem, &m_CondAttr);
	#endif
				this->m_Current = nInit;
				this->m_Max = nMax;
				TrySetStatue(true);
			}

			INLINE void Destroy( )
			{
				if ( !this->Statue() ) return;
	#if _DEF_WIN32
				::CloseHandle(m_Sem);
	#else
				//sem_destroy(&m_Sem);
				pthread_condattr_destroy(&m_CondAttr);
				pthread_cond_destroy(&m_Sem);
	#endif
				TrySetStatue(false);
				this->m_Current = 0;
			}

			INLINE bool Statue()
			{
				Locker _Lock( m_Mutex );
				return m_Available;
			}

			INLINE void TrySetStatue(bool _statue)
			{
				Locker _Lock( m_Mutex );
				if ( m_Available == _statue ) return;
				m_Available = _statue;
			}
		};
	}
}

#endif // plib.thread.semaphore.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

