/*
* Copyright (c) 2011, Push Chen
* All rights reserved.
* 
* File Name			: selector.hpp
* Propose  			: A Select Socket Listener.
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-01-11
*/

#pragma once

#ifndef _PLIB_NETWORK_SELECTOR_HPP_
#define _PLIB_NETWORK_SELECTOR_HPP_

#if _DEF_IOS
#include "Listener.hpp"
#else
#include <Plib-Network/Listener.hpp>
#endif

namespace Plib
{
	namespace Network
	{
		template< typename _TySocketInside >
		class Selector
		{
			friend class ListenerFrame< Selector< _TySocketInside > >;
		public:
			typedef _TySocketInside												ClientSocketT;
			typedef Plib::Generic::Reference< ClientSocketT >					SelectRefSockT;
			
			typedef Plib::Generic::Delegate< bool ( SelectRefSockT ) >			AddReadDelegate;
			typedef Plib::Generic::Delegate< void ( SelectRefSockT, bool ) >	ReleaseDelegate;
			typedef Plib::Generic::Delegate< SelectRefSockT ( ) >				GetFreeDelegate;

		protected:
			typedef Plib::Generic::RArray< SelectRefSockT >						SocketList;
			//typedef std::map< SOCKET_T, SelectRefSockT >						SocketList;

			SOCKET_T								_ListenFD;
			Uint64									_SocketIdleTime;
			SocketList								_AliveSockList;
			Plib::Threading::RWLock					_ListLock;
			struct timeval							_SelectTime;
			fd_set									_SockSet;
			struct sockaddr_in						_SvrAddr;
			struct sockaddr_in						_CltAddr;

			Selector( ) : _ListenFD( -1 ), _SocketIdleTime( 120000 ) {CONSTRUCTURE;}
			~Selector( ) { DESTRUCTURE; ShutdownListen( ); }

			// Idle Time Setting.
			void SetMaxIdleTime( Uint64 _IdleTime ) { _SocketIdleTime = _IdleTime; }

			INLINE LF_RETCODE ListenOnPort( Uint32 _Port, Uint32 _MaxSupport )
			{
				if ( _ListenFD != -1 ) return LF_ALRSTART;
				::memset( &_SvrAddr, 0, sizeof(_SvrAddr) );
				::memset( &_CltAddr, 0, sizeof(_CltAddr) );

				if ( ( _ListenFD = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == -1 )
					return LF_ESOCKET;

				_SvrAddr.sin_family = AF_INET;
				_SvrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
				_SvrAddr.sin_port = htons(_Port);

				int _Val = 1;
				if ( setsockopt( _ListenFD, SOL_SOCKET, 
					SO_REUSEADDR, (const char *)&_Val, sizeof(_Val) ) != 0 )
				{
					PLIB_NETWORK_CLOSESOCK( _ListenFD );
					_ListenFD = -1;
					return LF_ESETOPT;
				}

				if ( ::bind( _ListenFD, (struct sockaddr *)&_SvrAddr, sizeof(_SvrAddr) ) == -1 )
				{
					PLIB_NETWORK_CLOSESOCK( _ListenFD );
					_ListenFD = -1;
					return LF_EBIND;
				}

				if ( ::listen( _ListenFD, ( _MaxSupport > 1024 ? 1024 : _MaxSupport ) ) == -1 )
				{
					PLIB_NETWORK_CLOSESOCK( _ListenFD );
					_ListenFD = -1;
					return LF_ELISTEN;
				}
				return LF_SUCCESS;
			}

			INLINE void KeepSockAlive( SelectRefSockT _RefSock )
			{
				Plib::Threading::WriteLocker _ListLocker( _ListLock );
				_AliveSockList[_RefSock->hSo] = _RefSock;
			}

			INLINE LF_RETCODE ShutdownListen( )
			{
				if ( _ListenFD == -1 ) return LF_SUCCESS;
				//Plib::Threading::WriteLocker _ListLocker( _ListLock );
				_AliveSockList.Clear();
				PLIB_NETWORK_CLOSESOCK( _ListenFD );
				_ListenFD = -1;
				return LF_SUCCESS;
			}

			INLINE LF_RETCODE LoopPoll( AddReadDelegate & _AddD, 
				ReleaseDelegate & _RelD, GetFreeDelegate & _GetD )
			{
				if ( _ListenFD == -1 ) return LF_ESELECT;
				FD_ZERO( &_SockSet );
				FD_SET( _ListenFD, &_SockSet );

				_SelectTime.tv_sec = 0;
				_SelectTime.tv_usec = 1;
				Int32 _Ret = ::select( _ListenFD + 1, &_SockSet, NULL, NULL, &_SelectTime );
				if ( _Ret < 0 ) return LF_ESELECT;
				if ( _Ret > 0 )
				{
					// New client
					socklen_t _AddrLen = sizeof( _CltAddr );
					SOCKET_T _ClientFD = ::accept( _ListenFD, 
						(struct sockaddr *)&_CltAddr, &_AddrLen );

					if ( _ClientFD != -1 )
					{

						SelectRefSockT _FreeSock = _GetD();
						_FreeSock->Bind( _ClientFD, true );
						_FreeSock->SetNoDelay( );
						_FreeSock->SetLingerTime( 0 );
						_FreeSock->SetReUsable( true );
						
						_AliveSockList.PushBack( _FreeSock );
					}
				}

				// Loop Check all socket statue.
				if ( _AliveSockList.Size() == 0 ) return LF_SUCCESS;
				for ( Uint32 i = 0; i < _AliveSockList.Size(); ++i )
				{
					SelectRefSockT _CheckSock = _AliveSockList[i];
					if ( _CheckSock->IsReadable() ) {	// Socket has data incoming.
						if ( !_AddD( _CheckSock ) ) {
							_RelD( _CheckSock, false );
						}
						_AliveSockList.Remove( i );
						SELF_DECREASE( i );
						continue;
					} else {
						if ( !_CheckSock->IsConnect( ) ) { // The Socket has been closed.
							_RelD( _CheckSock, false );
							_AliveSockList.Remove( i );
							SELF_DECREASE( i );
							continue;
						}
						if ( _CheckSock->GetIdleTime( ) >= _SocketIdleTime ) {
							_RelD( _CheckSock, false );
							_AliveSockList.Remove( i );
							SELF_DECREASE( i );
							continue;
						}
					}
				}
				return LF_SUCCESS;
			}
		public:	
			INLINE static bool IsErrorFatal( Uint32 _errCode )
			{
			#if _DEF_WIN32
				if ( _errCode == WSAEFAULT || _errCode == WSAENETDOWN || _errCode == WSAEINVAL )
					return false;
			#elif _DEF_MAC
				if ( _errCode == EAGAIN || _errCode == EINVAL )
					return false;
			#else
				if ( _errCode == EINVAL || _errCode == ENOMEM )
					return false;
			#endif
				return true;
			}
		};
	}
}

#endif // plib.network.selector.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

