/*
* Copyright (c) 2011, Push Chen
* All rights reserved.
* 
* File Name			: listener.hpp
* Propose  			: A Listener Frame.
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-01-11
*/

#pragma once

#ifndef _PLIB_NETWORK_LISTENER_HPP_
#define _PLIB_NETWORK_LISTENER_HPP_

#if _DEF_IOS
#include "Socketbasic.hpp"
#include "Utility.hpp"
#include "Threading.hpp"
#include "Generic.hpp"
#else
#include <Plib-Network/Socketbasic.hpp>
#include <Plib-Utility/Utility.hpp>
#include <Plib-Threading/Threading.hpp>
#include <Plib-Generic/Generic.hpp>
#endif

namespace Plib
{
	namespace Network
	{
		typedef enum {
			LF_SUCCESS = 0,
			LF_INVALIDP,	// Invalid Listen Port.
			LF_POLLTERR,	// Failed to start poll thread.
			LF_ELPOLL,		// Error In Pool's Loop
			LF_ALRSTART,	// The frame object has already been used to listen on some port.
			LF_ESOCKET,		// Failed to create socket.
			LF_ESETOPT,		// Failed when set socket opt.
			LF_EBIND,		// Failed to bind the socket.
			LF_ELISTEN,		// Failed to listen on the port.
			LF_ESELECT,		// Error Occured in Select.
			LF_FULLQUEUE	// Select Queue is full.
		} LF_RETCODE;

		template < typename _TyPoller >
		class ListenerFrame
		{
		public:
			// Internal Typedef.
			typedef Plib::Generic::Reference< typename _TyPoller::ClientSocketT >	RefSocketT;
			typedef Plib::Generic::RDequeue< RefSocketT >							SocketListT;
			typedef Uint32															PortT;
			
			enum { VALID_MIN_PORT = 1, VALID_MAX_PORT = 65535 };
			enum { DEFAULT_MAX_SUPPORT = 0xFFFF };
		protected:
			_TyPoller						_FDPoller;
			SocketListT						_SL_Free;
			SocketListT						_SL_Readable;

			Uint32							_MaxSupport;
			PortT							_ListenPort;

			bool							_Statue;
			Plib::Threading::RWLock			_StatueLock;

			Plib::Threading::Mutex			_FreeListLock;
			Plib::Threading::Mutex			_ReadListLock;
			Plib::Threading::Semaphore		_ReadableSem;

			Plib::Threading::Thread< void () >		_PollingThread;

		public:
			Plib::Generic::Delegate< bool ( Uint32 ) >	OnPollLoopError;
			Plib::Generic::Delegate< void( Uint32 ) > 	OnPortLose;
		protected:

			// Get an Free Socket Reference Item.
			// The Item can act as a buffer item or a client
			// item to connect to other server.
			INLINE RefSocketT GetFreeSockItem( )
			{
				Plib::Threading::Locker _FLLock( _FreeListLock );
				if ( _SL_Free.Empty() ) return RefSocketT( true );
				RefSocketT _RefSock = _SL_Free.Head();
				_SL_Free.PopFront( );
				return _RefSock;
			}

			// Add Readable Socket to the list.
			// if the list is full( semaphore up to the max support )
			// return false.
			INLINE bool AddReadableSocket( RefSocketT _RefSock )
			{
				Plib::Threading::Locker _RLLocker( _ReadListLock );
				if ( !_ReadableSem.Release( ) ) return false;
				_SL_Readable.PushBack( _RefSock );
				return true;
			}

			// Statue Change
			INLINE void SetStatue( bool _Stat )
			{
				Plib::Threading::WriteLocker _StatWLock( _StatueLock );
				_Statue = _Stat;
			}

		protected:
			
			INLINE void PollSocketLoopThread( )
			{
				// Internal Delegate Object.
				// Used in Poller
				Plib::Generic::Delegate< bool ( RefSocketT ) >
					_AddReadSockDelg( this, &ListenerFrame::AddReadableSocket );
				Plib::Generic::Delegate< void ( RefSocketT, bool ) > 
					_ReleaseSockDelg( this, &ListenerFrame::ReleaseSocket );
				Plib::Generic::Delegate< RefSocketT ( ) >
					_GetFreeSockDelg( this, &ListenerFrame::GetFreeSockItem );

				while ( Plib::Threading::ThreadSys::Running() )
				{
					// Invoke _TyPoller's Poll Method
					// Syntax: 
					//		LF_RETCODE _Poller(
					//			Delegate &	_AddReadSockDelg, 
					//			Delegate &	_ReleaseSockDelg,
					//			Delegate &	_GetFreeScokDelg
					//		);
					if ( _FDPoller.LoopPoll(	_AddReadSockDelg, 
												_ReleaseSockDelg,
												_GetFreeSockDelg )
							== LF_SUCCESS ) continue;
					if ( OnPollLoopError && OnPollLoopError( PLIB_LASTERROR ) )
						continue;
					SetStatue( false );
					break;
				}
			}
			
			INLINE void PollSocketLoopJoin( )
			{
				if ( OnPortLose ) OnPortLose( _ListenPort );
			}

		public:
			ListenerFrame< _TyPoller >( Uint32 _MaxSpt = DEFAULT_MAX_SUPPORT ) 
				: _MaxSupport( _MaxSpt ), _ListenPort( 0 ), _Statue( false )
			{
				CONSTRUCTURE;
				_PollingThread.Jobs += std::make_pair( 
					this, &ListenerFrame< _TyPoller >::PollSocketLoopThread );
				_PollingThread.Join += std::make_pair(
					this, &ListenerFrame< _TyPoller >::PollSocketLoopJoin );
			}
			ListenerFrame< _TyPoller >( PortT _LPort, Uint32 _MaxSpt = DEFAULT_MAX_SUPPORT ) 
				: _MaxSupport( _MaxSpt ), _ListenPort( _LPort ), _Statue( false )
			{ 
				CONSTRUCTURE;
				_PollingThread.Jobs += std::make_pair( 
					this, &ListenerFrame< _TyPoller >::PollSocketLoopThread );
				_PollingThread.Join += std::make_pair(
					this, &ListenerFrame< _TyPoller >::PollSocketLoopJoin );
				if ( _ListenPort < VALID_MIN_PORT || _ListenPort > VALID_MAX_PORT )
					_ListenPort = 0;
			}
			~ListenerFrame< _TyPoller >( )
			{
				DESTRUCTURE;
				Shutdown();
				_SL_Free.Clear();
				_SL_Readable.Clear();
			}
		public:
			INLINE RefSocketT GetReadableSocket( Uint32 _TimeOut = 1000 )
			{
				if ( !_ReadableSem.Get( _TimeOut ) ) 
					return ListenerFrame< _TyPoller >::RefSocketT::NullRefObj;
				Plib::Threading::Locker _RLLock( _ReadListLock );
				RefSocketT _RefSock = _SL_Readable.Head();
				_SL_Readable.PopFront( );
				return _RefSock;
			}
		
			INLINE void ReleaseSocket( RefSocketT _RefSock, 
				bool _KeepAlive = false )
			{
				if ( _RefSock.RefNull( ) ) return;
				if ( !_KeepAlive || _RefSock->Statue == SOST_EMPTY || !Statue( ) ) {
					_RefSock->Close( );
					Plib::Threading::Locker _FLLock( _FreeListLock );
					_SL_Free.PushBack( _RefSock );
					return;
				}
				_FDPoller.KeepSockAlive( _RefSock );
			}

			INLINE LF_RETCODE Listen( PortT _OnPort = 0 )
			{
				if ( Statue( ) ) return LF_ALRSTART;
				if ( _OnPort == 0 && _ListenPort == 0) return LF_INVALIDP;
				if ( _OnPort != 0 ) _ListenPort = _OnPort;
				LF_RETCODE _RTC = _FDPoller.ListenOnPort( _ListenPort, _MaxSupport );
				if ( _RTC != LF_SUCCESS ) return _RTC;

				_ReadableSem.Init( 0, _MaxSupport );
				if ( !_PollingThread.Start( ) ) {
					_ReadableSem.Destroy( );
					return LF_POLLTERR;
				}
				SetStatue( true );
				return _RTC;
			}

			INLINE LF_RETCODE Shutdown( ) {
				_PollingThread.Stop();
				_ReadableSem.Destroy();
				LF_RETCODE _RTC = _FDPoller.ShutdownListen( );
				SetStatue( false );
				return _RTC;
			}

			INLINE bool Statue( ) {
				Plib::Threading::ReadLocker _StatRLock( _StatueLock );
				return _Statue;
			}
			
			INLINE void SetIdleTime( Uint32 _IdleTime ) {
				_FDPoller.SetMaxIdleTime( _IdleTime );
			}
		};
	}
}

#endif // plib.network.listener.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
