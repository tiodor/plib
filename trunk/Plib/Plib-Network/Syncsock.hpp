/*
* Copyright (c) 2011, Push Chen
* All rights reserved.
* 
* File Name			: syncsock.hpp
* Propose  			: 
* 
* Current Version	: 1.1
* Change Log		: Common Socket Inside Object of IOSocket
* Change Log v1.1	: Update under Plib-1.1
* Author			: Push Chen
* Change Date		: 2010-11-17
* Change Date		: 2011-01-10
*/

#pragma once

#ifndef _PLIB_NETWORK_SYNCSOCK_HPP_
#define _PLIB_NETWORK_SYNCSOCK_HPP_

#if _DEF_IOS
#include "Socketbasic.hpp"
#include "Threading.hpp"
#else
#include <Plib-Network/Socketbasic.hpp>
#include <Plib-Threading/Threading.hpp>
#endif

namespace Plib
{
	namespace Network
	{
		class internal_common_sock 
		{
			friend class SocketBasic< internal_common_sock, 256 >;

			typedef SocketBasic< internal_common_sock, 256 > _TySo;

		protected:
			Plib::Threading::StopWatch			calcTime;

			unsigned int					m_writeTimeOut;
			unsigned int					m_readTimeOut;

		protected:
			// All methods are protected. 
			// So that no one can invoke these methods outside the 
			// class beside iosocket.
			internal_common_sock( ) { CONSTRUCTURE; }
			~internal_common_sock( ) { DESTRUCTURE; }

			INLINE void initialize( _TySo * pSo )
			{
				// Set default timeout value to 10 seconds.
				m_readTimeOut = 10000;
				m_writeTimeOut = 1000;
			}

			INLINE bool SetWriteTimeOut( _TySo * pSo, unsigned int _mileSec )
			{
				m_writeTimeOut = _mileSec;
				if ( pSo->hSo == -1 ) return false;
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
				return (0 == setsockopt( pSo->hSo, SOL_SOCKET, SO_SNDTIMEO,
					(const char *)&_mileSec, sizeof( unsigned int ) ));
#else
				struct timeval w_Tv = { _mileSec / 1000, (_mileSec % 1000) * 1000 };
				return (0 == setsockopt(pSo->hSo, SOL_SOCKET, SO_SNDTIMEO, 
					(const char *)&w_Tv, sizeof(struct timeval)));
#endif
			}
			INLINE bool SetReadTimeOut( _TySo * pSo, unsigned int _mileSec )
			{
				m_readTimeOut = _mileSec;
				return true;
			}

			INLINE SOPROCRET writeData( _TySo * pSo, const char * _data, unsigned int & _length )
			{
				int _allSent = 0;
				int _preSent = 0;
				calcTime.SetStart();
				while ( (unsigned int)_allSent < _length )
				{
					_preSent = ::send( pSo->hSo, _data + _allSent, 
						(_length - (unsigned int)_allSent), 0 | PLIB_NETWORK_NOSIGNAL );
					if( _preSent < 0 ) {
						return SOPROC_ERROR;
					}
					_allSent += _preSent;
					calcTime.Tick();
					if ( calcTime.GetMileSecUsed() >= m_writeTimeOut && 
						((unsigned int)_allSent < _length) ) {
						_length = _allSent;
						return SOPROC_TIMEOUT;
					}
				}
				return SOPROC_OK;
			}
			
			INLINE SOPROCRET readData( _TySo * pSo, 
				Plib::Text::RString * _string, 
				char * _buffer, unsigned int _bufSize, unsigned int _timeOut )
			{
				if ( pSo->hSo == -1 ) return SOPROC_ERROR;

				struct timeval _tv;
				fd_set recvFs;
				FD_ZERO( &recvFs );
				FD_SET( pSo->hSo, &recvFs );

				calcTime.SetStart();
				Uint64 _leftTime = _timeOut;
				do {
					_tv.tv_sec = (long)_leftTime / 1000;
					_tv.tv_usec = ((long)_leftTime % 1000) * 1000;

					if ( pSo->hSo == -1 ) return SOPROC_ERROR;
					FD_ZERO( &recvFs );
					FD_SET( pSo->hSo, &recvFs );

					int _retCode = ::select( pSo->hSo + 1, &recvFs, NULL, NULL, &_tv );
					if ( _retCode < 0 )		// Error
						return SOPROC_ERROR;
					if ( _retCode == 0 )	// TimeOut
						return SOPROC_TIMEOUT;
					
					_retCode = ::recv( pSo->hSo, _buffer, _bufSize, 0 );
					if ( _retCode < 0 ) return SOPROC_ERROR;

					_string->Append( _buffer, _retCode );

					// Parse the recived data.
					if ( pSo->onBufferUpdate ) {
						SOCKEVENTSTATUE _ret = pSo->onBufferUpdate( pSo, _string );
						if ( _ret == SOEVENT_ILLEAGE ) return SOPROC_ERROR;
						if ( _ret == SOEVENT_DONE ) break;
						if ( _ret != SOEVENT_UNFINISHED ) return SOPROC_ERROR;
					}
					else break;

					calcTime.Tick();
					if ( calcTime.GetMileSecUsed() >= _timeOut ) return SOPROC_TIMEOUT;
					_leftTime = _timeOut - calcTime.GetMileSecUsed();
				} while ( true );

				return SOPROC_OK;				
			}

			INLINE SOPROCRET readData( _TySo * pSo, char * _outBuf, 
				unsigned int & _bufSize, unsigned int _timeOut )
			{
				if ( pSo->hSo == -1 ) return SOPROC_ERROR;

				struct timeval _tv;
				fd_set recvFs;
				FD_ZERO( &recvFs );
				FD_SET( pSo->hSo, &recvFs );

				unsigned int _RecvSize = 0;
				unsigned int _AllBufSize = _bufSize;

				SODATAPAIR dp;
				dp.data = _outBuf;
				dp.length = 0;

				calcTime.SetStart();
				Uint64 _leftTime = _timeOut;
				do {
					_tv.tv_sec = (long)_leftTime / 1000;
					_tv.tv_usec = ((long)_leftTime % 1000) * 1000;

					if ( pSo->hSo == -1 ) return SOPROC_ERROR;
					FD_ZERO( &recvFs );
					FD_SET( pSo->hSo, &recvFs );

					int _retCode = ::select( pSo->hSo + 1, &recvFs, NULL, NULL, &_tv );
					if ( _retCode < 0 )		// Error
						return SOPROC_ERROR;
					if ( _retCode == 0 )	// TimeOut
						return SOPROC_TIMEOUT;
					
					_retCode = ::recv( pSo->hSo, _outBuf, _AllBufSize, 0 );
					if ( _retCode < 0 ) return SOPROC_ERROR;
					dp.length = _retCode;	// This is current loop read size
					_RecvSize += _retCode;	// This is all read size.

					// Parse the recived data.
					if ( pSo->onParseData ) {
						SOCKEVENTSTATUE _ret = pSo->onParseData( pSo, &dp );
						if ( _ret == SOEVENT_ILLEAGE ) return SOPROC_ERROR;
						if ( _ret == SOEVENT_DONE ) break;
						if ( _ret != SOEVENT_UNFINISHED ) return SOPROC_ERROR;
					}
					else break;

					calcTime.Tick();
					if ( calcTime.GetMileSecUsed() >= _timeOut ) return SOPROC_TIMEOUT;
					_leftTime = _timeOut - calcTime.GetMileSecUsed();
				} while ( true );

				_bufSize = _RecvSize;
				return SOPROC_OK;
			}
		};

		// Type definition of syncSocket.
		typedef SocketBasic< internal_common_sock, 256 > SyncSock;
		typedef SocketBasic< internal_common_sock, 256 > CommonSock;
		typedef Plib::Generic::Reference< SyncSock > RPSyncSock;
	}
}

#endif // plib.network.syncsock.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */


