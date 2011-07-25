/*
* Copyright (c) 2011, Push Chen
* All rights reserved.
* 
* File Name			: Service.hpp
* Propose  			: The server framework
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-06-14
*/

#pragma once

#ifndef _PLIB_NETWORK_SERVICE_HPP_
#define _PLIB_NETWORK_SERVICE_HPP_

#if _DEF_IOS
#include "Request.hpp"
#include "Listener.hpp"
#else
#include <Plib-Network/Request.hpp>
#include <Plib-Network/Listener.hpp>
#endif

namespace Plib
{
	namespace Network
	{
		// Common Network Services Framework
		// The parser is used to create the request/response object.
		// The poller is used to listen on the certain port.
		template< typename _TyParser, typename _TyPoller = Selector<SyncSock> >
		class Service
		{
		public:
			// Type definition of important objects.
			typedef ListenerFrame< _TyPoller >							TService;
			typedef typename _TyPoller::ClientSocketT					TConnect;
			typedef Request< _TyParser, TConnect >						RpRequest;
			typedef Response< _TyParser, TConnect >						RpResponse;
			typedef Plib::Generic::Reference< _TyParser >				RpParser;
			typedef Plib::Generic::Reference< TConnect >				RpConnect;
		
			typedef Plib::Threading::Thread<void()>		WorkThreadT;
			
		protected:
			
			class InnerQueue 
			{
			protected:
				Plib::Generic::RQueue< RpRequest >						_innerQueue;
				Plib::Generic::RQueue< RpRequest >						_innerReuseQueue[2];
				Plib::Generic::RQueue< RpRequest > *					_workingReuseQueue;
				Plib::Threading::Mutex									_reuseQueueLock;
				Plib::Threading::Semaphore								_innerSem;
				Plib::Threading::StopWatch								_timer;
				Service< _TyParser, _TyPoller > *						_theService;
				Plib::Threading::Thread< void() >						_CheckThread;
				
				PLIB_THREAD_SAFE_DEFINE;
				
			protected:
				void __SocketChecker( )  {
					while ( Plib::Threading::ThreadSys::Running() ) {
						bool __hasReadableSocket = false;
						// Get the old reuse queue.
						Plib::Generic::RQueue< RpRequest > * _checkQueue = _workingReuseQueue;
						_reuseQueueLock.Lock();
						// Swap the reuse queue.
						_workingReuseQueue = (_workingReuseQueue == _innerReuseQueue ) ? 
							_innerReuseQueue + 1 : _innerReuseQueue;
						_reuseQueueLock.UnLock();
						
						// Check the socket one by one.
						while ( !_checkQueue->Empty() ) {
							RpRequest _req = _checkQueue->Head();
							_checkQueue->Pop();
							// get the connection.
							RpConnect _cnnt = _req.GetConnect();
							if ( _cnnt.RefNull() ) {
								_req.EndRequest();
								// put into recycle
								_theService->RequestIdlePool.Return( _req );
								continue;
							}
							// check socket statue.
							int _ret = _req.Check();
							if ( _ret == 0 ) {
								_workingReuseQueue->Push( _req );
								continue;
							}
							// on error happen
							if ( _ret < 0 ) {
								_req.EndRequest();
								// put into recycle
								_theService->ServicePort.ReleaseSocket( _cnnt, false );
								_theService->RequestIdlePool.Return( _req );
								continue;
							}
							// The Socket has some new incoming data.
							_innerQueue.Push( _req );
							_innerSem.Release();
							__hasReadableSocket = true;
						}
						
						if ( __hasReadableSocket == false ) Plib::Threading::ThreadSys::Sleep( 1 );
					}
				}
			public:
				InnerQueue( ) 
					: _workingReuseQueue(_innerReuseQueue), _innerSem(0, 0xFFFF), _theService( NULL ) 
				{
					CONSTRUCTURE;
					_CheckThread.Jobs += std::make_pair( this, &InnerQueue::__SocketChecker );
				}
				~InnerQueue( )
				{
					DESTRUCTURE;
				}
				
				void SetService( Service< _TyParser, _TyPoller > * _service )
				{
					_theService = _service;
				}
				
				void StartChecking( )
				{
					_CheckThread.Start();
				}
				
				RpRequest Get( Uint32 _timeOut ) {
					if ( !_innerSem.Get( _timeOut ) ) {
						PLIB_THREAD_SAFE;
						_theService->__CheckReuseThreadAndMinus( _timer );
						return RpRequest::Null;
					}
					PLIB_THREAD_SAFE;
					RpRequest _req = _innerQueue.Head( );
					_innerQueue.Pop();
					return _req;
				}
				
				void Return( RpRequest _req ) {
					if ( _req.RefNull() ) return;
					_reuseQueueLock.Lock();
					_workingReuseQueue->Push( _req );
					_reuseQueueLock.UnLock();
					PLIB_THREAD_SAFE;
					_theService->__CheckReuseThreadAndAdd( _innerQueue.Size() );
					// Recalculate the time
					_timer.SetStart();
				}
			};
			
			friend class InnerQueue;
			
			class InnerPool
			{
			protected:
				Plib::Generic::RStack< RpRequest >						_innerStack;
				PLIB_THREAD_SAFE_DEFINE;
			public:
				InnerPool( ) { CONSTRUCTURE; }
				~InnerPool( ) { DESTRUCTURE; }
				RpRequest Get( ) {
					PLIB_THREAD_SAFE;
					if ( _innerStack.Empty() ) return RpRequest();
					RpRequest _req = _innerStack.Top();
					_innerStack.Pop( );
					return _req;
				}
				
				void Return( RpRequest _req ) {
					_innerStack.Push( _req );
				}
			};
			
		protected:
			// The Listener object.
			TService									ServicePort;
			// Request Pool
			InnerPool									RequestIdlePool;
			InnerQueue									RequestUsingQueue;
			
			// Working thread array of the service.
			Plib::Generic::RArray< WorkThreadT * >		NormalConnectionList;
			Plib::Generic::RArray< WorkThreadT * >		ReuseConnectionList;
			
			// Service statu config
			bool				_restartOnError;
			Uint32				_restartInterval;	// 0 means immediatly.
			
			Uint32				_MaxIdleTime;
			Uint32				_MaxSockCountPreThread;
			
			Uint32				_workThreadCount;
						
		public:
			
			Service<_TyParser, _TyPoller>( bool rsOnError = true, Uint32 rsInt = 0 )
				:_restartOnError(rsOnError), _restartInterval(rsInt), 
				_MaxIdleTime( 180000 ), _MaxSockCountPreThread( 300 ),
				_workThreadCount( 0 )
			{
				CONSTRUCTURE;
				RequestUsingQueue.SetService( this );
				RequestUsingQueue.StartChecking();
				ServicePort.OnPollLoopError += std::make_pair(
						this, &Service<_TyParser, _TyPoller>::__PollerError);
			}
			
			~Service< _TyParser, _TyPoller >( ) {DESTRUCTURE; StopServer(); }
			
			void SetRestartOnError( bool _rsOnError )
			{
				_restartOnError = _rsOnError;
			}
			
			void SetRestartInterval( Uint32 _rsInt )
			{
				_restartInterval = _rsInt;
			}
			
			void SetMaxIdleTIme( Uint32 _maxIdleTime )
			{
				_MaxIdleTime = _maxIdleTime;
			}
			
			void SetMaxSockCountPreThread( Uint32 _count )
			{
				_MaxSockCountPreThread = _count;
			}
			
			// Start the server on certain port with _threadCount working thread.
			bool StartServer( Uint32 _port, Uint32 _threadCount = 1 )
			{
				if ( ServicePort.Statue() )
				{
					// Service has already start. 
					return false;
				}
				if ( !WorkProcess )
				{
					// No work process.
					return false;
				}
				
				NormalConnectionList.Clear();
				ReuseConnectionList.Clear();
				if ( LF_SUCCESS != ServicePort.Listen(_port) )
				{
					// On Error
					if ( OnServerError ) OnServerError( PLIB_LASTERROR );
					return false;
				}
				
				if ( _threadCount < 1 ) _threadCount = 1;
				_workThreadCount = _threadCount;
				// Start normal thread
				for ( Uint32 count = 0; count < _threadCount; ++count )
				{
					PCNEW( WorkThreadT, pThread );
					//WorkThreadT * pThread = Plib::Basic::Memory::New< WorkThreadT >( );
					//WorkThreadT * pThread = new WorkThreadT;
					pThread->Jobs += std::make_pair( this, 
						&Service< _TyParser, _TyPoller >::__threadForGetIncomingSocket );
					NormalConnectionList.PushBack(pThread);
					pThread->Start();
				}
				
				// Start one reuse thread
				PCNEW( WorkThreadT, pThread );
				//WorkThreadT * pThread = Plib::Basic::Memory::New< WorkThreadT >( );
				//WorkThreadT * pThread = new WorkThreadT;
				pThread->Jobs += std::make_pair( this, 
					&Service< _TyParser, _TyPoller >::__threadForReuseSocket );
				ReuseConnectionList.PushBack(pThread);
				pThread->Start();
				
				return true;
			}
			
			// Stop the server and clear the working thread list.
			void StopServer( )
			{
				ServicePort.Shutdown();
				for ( Uint32 i = 0; i < NormalConnectionList.Size(); ++i )
				{
					NormalConnectionList[i]->GiveSignal();
				}
				for ( Uint32 i = 0; i < NormalConnectionList.Size(); ++i )
				{
					PDELETE( NormalConnectionList[i] );
					//Plib::Basic::Memory::Delete( NormalConnectionList[i] );
					//delete NormalConnectionList[i];
				}
				for ( Uint32 i = 0; i < ReuseConnectionList.Size(); ++i )
				{
					ReuseConnectionList[i]->GiveSignal();
				}
				for ( Uint32 i = 0; i < ReuseConnectionList.Size(); ++i )
				{
					PDELETE( ReuseConnectionList[i] );
					//Plib::Basic::Memory::Delete( ReuseConnectionList[i] );
					//delete ReuseConnectionList[i];
				}
				ReuseConnectionList.Clear();
				NormalConnectionList.Clear();
			}
			
		public:
			// Callback delegate
			Plib::Generic::Delegate< RpResponse ( RpRequest ) > 		WorkProcess;
			typename TConnect::so_event									OnConnectionError;
			Plib::Generic::Delegate< void ( Uint32 ) >					OnServerError;
			Plib::Generic::Delegate< bool ( Uint32 ) >					IsErrorFatalDelegate;
			Plib::Generic::Delegate< void ( RpRequest, Plib::Threading::StopWatch ) >	
																		AfterOneRequest;
			Plib::Generic::Delegate< void ( Service< _TyParser, _TyPoller > * ) >
																		OnLoseServerPort;
		protected:
			
			void __CheckReuseThreadAndMinus( Plib::Threading::StopWatch & calc )
			{
				if ( ReuseConnectionList.Size() == 1 ) return;
				calc.Tick();
				if ( calc.GetMileSecUsed() <= _MaxIdleTime ) return;
				
				// Minus a thread
				WorkThreadT * pThread = ReuseConnectionList.Last( );
				pThread->GiveSignal();
				ReuseConnectionList.Remove( ReuseConnectionList.Size() - 1 );
				PDELETE( pThread );
				calc.SetStart();
			}
			
			void __CheckReuseThreadAndAdd( Uint32 _Size )
			{
				if ( _Size / ReuseConnectionList.Size() < _MaxSockCountPreThread )
					return;
				PCNEW( WorkThreadT, pThread );
				//WorkThreadT * pThread = Plib::Basic::Memory::New< WorkThreadT >( );
				//WorkThreadT * pThread = new WorkThreadT;
				pThread->Jobs += std::make_pair( this, 
					&Service< _TyParser, _TyPoller >::__threadForReuseSocket );
				ReuseConnectionList.PushBack(pThread);
				pThread->Start();
			}
			
			// Error Processer
			bool __PollerError( Uint32 _errCode )
			{
				// Now we know the server is getting an error.
				// we will notify the user
				// then we will check if the error is fatal
				if ( OnServerError ) OnServerError( _errCode );
				if ( IsErrorFatalDelegate ) return IsErrorFatalDelegate( _errCode );
				return _TyPoller::IsErrorFatal( _errCode );
			}
			
			// On Port Lose.
			void __PollerLosePort( Uint32 _Port )
			{
				if ( OnLoseServerPort ) OnLoseServerPort( this );
				if ( !_restartOnError ) return;
				if ( _restartInterval != 0 ) 
					Plib::Threading::ThreadSys::Sleep( _restartInterval );
				this->StopServer();
				this->StartServer( _Port, _workThreadCount );
			}
			
			// Working Thread.
			void __threadForReuseSocket( )
			{
				Plib::Threading::StopWatch calc;
				while ( Plib::Threading::ThreadSys::Running() )
				{
					RpRequest req = RequestUsingQueue.Get(1);
					if ( req.RefNull() ) continue;
					
					calc.SetStart();
					RpConnect _cnnt = req.GetConnect();
					// Process the request, get the response
					RpResponse resp = WorkProcess( req );
					if ( resp.RefNull() ) {
						// The package is not validate
						ServicePort.ReleaseSocket( _cnnt, false );
						req.EndRequest();
						RequestIdlePool.Return( req );
						continue;
					}
					resp.Serialize( );
					Plib::Text::RString _respString = resp.GetResponseString();
					if ( _respString.Size() > 0 ) {
						_cnnt->Write(_respString.C_Str(), _respString.Size() );
					}
					else {
						// Build Response Error.
						ServicePort.ReleaseSocket( _cnnt, false );
						req.EndRequest();
						RequestIdlePool.Return( req );						
						continue;
					}

					calc.Tick();
					if ( AfterOneRequest ) AfterOneRequest( req, calc );					
					
					if ( req.KeepAlive() ) {
						req.ReuseRequest();
						RequestUsingQueue.Return( req );
					} else {
						ServicePort.ReleaseSocket( _cnnt, false );
						req.EndRequest();
						RequestIdlePool.Return( req );
					}
				}
			}
			void __threadForGetIncomingSocket( )
			{
				Plib::Threading::StopWatch calc;
				while ( Plib::Threading::ThreadSys::Running() )
				{
					RpConnect _cnnt = ServicePort.GetReadableSocket( 1 );
					if ( _cnnt.RefNull() ) {	// No new connect
						continue;
					}
					
					calc.SetStart();
					// Re-bind the onError event.
					if ( _cnnt->onError.Count() == 0 ) _cnnt->onError = OnConnectionError;
					
					// Fetch a reusable request object from the pool.
					RpRequest req = RequestIdlePool.Get();
					
					if ( !req.Create( _cnnt ) )
					{
						// On Error
						req.EndRequest();
						ServicePort.ReleaseSocket( _cnnt, false );
						RequestIdlePool.Return( req );
						continue;
					}
					// Process the request, get the response
					RpResponse resp = WorkProcess( req );
					// Failed to build the response
					if ( resp.RefNull() ) {
						req.EndRequest();
						ServicePort.ReleaseSocket( _cnnt, false );
						RequestIdlePool.Return( req );
						continue;
					}
					
					resp.Serialize( );					
					Plib::Text::RString _respString = resp.GetResponseString();
					if ( _respString.Size() == 0 || !_cnnt->Write(
					 						_respString.c_str(), _respString.size() ) )
					{
						req.EndRequest();
						ServicePort.ReleaseSocket( _cnnt, false );
						RequestIdlePool.Return( req );
						continue;
					}

					calc.Tick();
					if ( AfterOneRequest ) AfterOneRequest( req, calc );
					
					// Release the connection object according to
					// the KeepAlive property of the request.
					if ( req.KeepAlive() ) {
						req.ReuseRequest();
						RequestUsingQueue.Return( req );
					}
					else {
						req.EndRequest();
						ServicePort.ReleaseSocket( _cnnt, false );
						RequestIdlePool.Return( req );
					}
				}
			}
		};
	}
}

#endif // plib.network.servcie.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */


