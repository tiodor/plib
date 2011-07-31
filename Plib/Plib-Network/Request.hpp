/*
* Copyright (c) 2011, Push Chen
* All rights reserved.
* 
* File Name			: Request.hpp
* Propose  			: The server/client request object template
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-06-10
*/

#pragma once

#ifndef _PLIB_NETWORK_REQUEST_HPP_
#define _PLIB_NETWORK_REQUEST_HPP_

#if _DEF_IOS
#include "Response.hpp"
#else
#include <Plib-Network/Response.hpp>
#endif

namespace Plib
{
	namespace Network
	{
		// Request definition
		template< typename _TyParser, typename _TyConnect >
		class _Request
		{
		public:
			typedef Plib::Generic::Reference< _TyParser >  				RpParser;
			typedef Plib::Generic::Reference< _TyConnect >				RpConnect;
			typedef Response< _TyParser, _TyConnect > 					RResponse;
			
		protected:
			Plib::Text::RString				m_requestStream;
			RpParser						m_rpParser;
			RpConnect						m_rpConnect;
			
			RConnInfo						m_rConnectInfo;
			RResponse						m_Resp;
			bool							m_createByService;
			bool							m_beSerialized;
			
			// Error String, Record the last error message.
			Plib::Text::RString				m_LastError;
		public:
			_Request<_TyParser, _TyConnect>( )
				: m_rpParser( false ), m_rpConnect( false ), 
					m_createByService( false ), m_beSerialized(false)
			{
				CONSTRUCTURE;
				// Nothing to do.
			}
			~_Request<_TyParser, _TyConnect>( ) { DESTRUCTURE; }
			
			// This Create is for service usage.
			// Fill a request by the socket connect to the service.
			bool Create( const RpConnect & _cnnt )
			{
				if ( _cnnt.RefNull() ) {
					m_LastError = "Null Connect Object from Server Frame.";
					return false;
				}
				// Initialize the Parser
				if ( m_rpParser.RefNull() )
					m_rpParser = RpParser();
					
				// Initialize the Connect
				m_rpConnect = _cnnt;
				m_requestStream.Clear( );
				m_rpConnect->AttachReadBuffer( &m_requestStream );
				m_rpConnect->onBufferUpdate.Clear();
				m_rpConnect->onBufferUpdate += std::make_pair(
					&(*m_rpParser), &_TyParser::ParseIncoming );

				m_rConnectInfo->Host = m_rpConnect->RemoteAddress;
				m_rConnectInfo->Port = m_rpConnect->RemotePort;
				m_rConnectInfo->KeepAlive = true;
				
				if ( m_rConnectInfo->TimeOut == 0 )
					m_rConnectInfo->TimeOut = 1000;

				if ( !m_rpConnect->Read( m_rConnectInfo->TimeOut ) ) {
					m_LastError = "On Read Incoming, " + Plib::Text::LastErrorMessage;
					return false;
				}
				m_createByService = true;
				return true;
			}
			
			// The Create is used for connect to other serivce.
			void Create( const RConnInfo & _cnntInfo )
			{
				// Initialize the Parser
				if ( m_rpParser.RefNull() )
					m_rpParser = RpParser();
				
				// Initialize the connect info.
				m_rConnectInfo.DeepCopy( _cnntInfo );
				if ( m_rpConnect.RefNull() ) m_rpConnect = RpConnect( );
				// Try to connect to the peer server.
				if ( !m_rpConnect->Connect( _cnntInfo.Host(), _cnntInfo.Port(), _cnntInfo.TimeOut()/2 ) )
					m_LastError = "On Connect, " + Plib::Text::LastErrorMessage;
				
				m_createByService = false;
				m_beSerialized = false;
			}
			
			// Initialize the Request by the initializer and also initialize
			// the parser.
			template < typename _TyInitializer >
			void Create( const _TyInitializer & _initer )
			{
				if ( m_rpParser.RefNull() )
					m_rpParser = RpParser();
				m_rConnectInfo.DeepCopy( _initer.ConnectInfo() );
				m_rpParser->Initialize( _initer );
				m_createByService = false;
				m_beSerialized = false;
			}	
			
			// if the request is created by the service,
			// use this function to check if has new data incoming.
			int Check( )
			{
				if ( m_createByService == false ) return -1;
				if ( m_rpConnect.RefNull() ) return -1;
				if ( m_rpConnect->IsConnect() == false ) return -1;
				if ( m_rpConnect->IsReadable() == false ) return 0;
				
				if ( !m_rpConnect->Read( m_rConnectInfo->TimeOut ) ) return -1;
				return 1;
			}
			
		protected:
			
			// Create a empty response object.
			RResponse _CreateResponseForService( )
			{
				// End the prevoius response
				m_Resp.EndResponse();
				
				// Share current connect with the response.
				(*m_Resp).__Init( m_rConnectInfo );
				return m_Resp;
			}
			
			// Get the other service returns response data.
			RResponse _GetResponseForClient( )
			{
				// Check if the parser object has already been initialized.
				if ( m_rpParser.RefNull() ) {
					m_LastError = "Null Parser Object.";
					return RResponse::Null;
				}
				// End the prevoius response
				m_Resp.EndResponse();
								
				// Check the connect statue.
				// If the request object is a reusable object, the socket
				// should be already connected.
				// If the request object is first used, connect.
				if ( m_rpConnect.RefNull() ) m_rpConnect = RpConnect( );
				// Initialize the response
				m_Resp->__Init( m_rpConnect, m_rConnectInfo );
				
				// Check the connection statue.
				if ( m_rpConnect->IsConnect() == false )
				{
					if ( ! m_rpConnect->Connect(m_rConnectInfo->Host.c_str(), 
						m_rConnectInfo->Port, m_rConnectInfo->TimeOut) )
					{
						// On Error
						m_LastError = "On Connect, " + Plib::Text::LastErrorMessage;
						return RResponse::Null;
					}
				}

				// Build the request package.
				if ( m_beSerialized == false ) {
					m_requestStream.Clear();
					m_rpParser->Build( m_requestStream );
					m_beSerialized = true;
				}

				if ( ! m_rpConnect->Write( m_requestStream.c_str(), m_requestStream.size() ) )
				{
					// On Error
					m_beSerialized = false;
					m_LastError = "On Write Data, " + Plib::Text::LastErrorMessage;
					return RResponse::Null;
				}
				
				m_beSerialized = false;
				if ( ! m_rpConnect->Read( m_rConnectInfo->TimeOut ) )
				{
					// On Error
					m_LastError = "On Read Response, " + Plib::Text::LastErrorMessage;
					return RResponse::Null;
				}
				
				if ( m_rConnectInfo->KeepAlive == false )
				{
					// Close the connect according to the setting.
					m_rpConnect->Close();
				}
				
				return m_Resp;
			}
			
		public:
			// Get the connection
			INLINE RpConnect & GetConnect() {
				return m_rpConnect;
			}
			// Check the connection statue
			INLINE bool IsConnect( ) const {
				return m_rpConnect.RefNull( ) ? false : m_rpConnect->IsConnect( );
			}
			// For debug usage.
			// Get the request stream.
			INLINE const Plib::Text::RString & GetRequestString( )
			{
				if ( m_createByService == true ) return m_requestStream;
				// Build the request package.
				if ( m_beSerialized == false ) {
					m_requestStream.Clear();
					m_rpParser->Build( m_requestStream );				
				}
				return m_requestStream;
			}
			
			// Get the response.
			INLINE RResponse GetResponse( )
			{
				if ( m_createByService == true ) return _CreateResponseForService( );
				return _GetResponseForClient( );
			}
			
			// When Error Happend, return the last error message.
			INLINE const Plib::Text::RString & GetErrorMessage( ) const
			{
				return m_LastError;
			}
			
			// Connect to the peer server
			INLINE bool Connect( ) {
				// Server Request cannot invoke this method.
				if ( m_createByService == true ) return false;
				if ( m_rpConnect->IsConnect( ) ) return true;
				bool _ret = m_rpConnect->Connect( m_rConnectInfo.Host(), 
					m_rConnectInfo.Port(), m_rConnectInfo.TimeOut() / 2 );
				if ( _ret == false ) {
					// Update the error message.
					m_LastError = "On Connect, " + Plib::Text::LastErrorMessage;
				}
				return _ret;
			}
			// Clear the inner buffer.
			INLINE void ClearBuffer( )
			{
				m_requestStream.Clear();
			}
			
		public:
			// Other methods
						
			// Get the request host info.
			const Plib::Text::RString & Host() const {
				return m_rConnectInfo->Host;
			}
			
			void SetHost( const Plib::Text::RString & _host ) {
				m_rConnectInfo->Host.DeepCopy( _host );
			}
			
			// Get the request port info.
			Uint32 Port() const {
				return m_rConnectInfo->Port;
			}
			
			void SetPort( Uint32 _port ) {
				m_rConnectInfo->Port = _port;
			}
			
			// Get the request timeout setting info.
			Uint32 TimeOut() const {
				return m_rConnectInfo->TimeOut;
			}
			
			void SetTimeOut( Uint32 _timeout ) {
				m_rConnectInfo->TimeOut = _timeout;
			}
			
			// Get the request connection's keepalive setting info.
			bool KeepAlive() const {
				return m_rConnectInfo->KeepAlive;
			}
			
			void SetKeepAlive( bool _keepAlive ) {
				m_rConnectInfo->KeepAlive = _keepAlive;
			}
			
			// Get the connection's identify.
			const Plib::Text::RString & Identify() const {
				return m_rConnectInfo->Identify;
			}
			
			// Get the Parser of the request.
			RpParser & GetParser() {
				return m_rpParser;
			}
			
			RpParser & operator() ( )
			{
				return m_rpParser;
			}

			// Just clear the parser and the bufferstream.
			void ReuseRequest( ) {
				m_Resp.ReuseResponse();
				m_beSerialized = false;
				if ( !m_rpParser.RefNull() ) m_rpParser->Clear();
			}

			// close current connection.
			void EndRequest( ) {
				m_Resp.EndResponse();
				m_beSerialized = false;
				if ( !m_rpParser.RefNull() ) m_rpParser->Clear();
				m_requestStream.Clear();
				if ( m_rpConnect.RefNull() ) return;
				m_rpConnect->Close( );
			}
			
			// Clear the request object and release the data.
			void ReleaseRequest( ) {
				m_Resp.ReleaseResponse();
				m_beSerialized = false;
				m_rpParser = RpParser::NullRefObj;
				m_rpConnect = RpConnect::NullRefObj;
				m_requestStream.Clear();
			}
			
			// the operator of compare.
			bool operator == ( const _Request< _TyParser, _TyConnect > & _resp ) const
			{
				return false; 
			}
			
			bool operator != ( const _Request< _TyParser, _TyConnect > & _resp ) const
			{
				return true;
			}
		};

		// Refernce Request.
		template< typename _TyParser, typename _TyConnect = SyncSock >
		class Request : public Plib::Generic::Reference< _Request< _TyParser, _TyConnect > >
		{
		public:
			typedef Plib::Generic::Reference< _TyParser > 							RpParser;
			typedef Plib::Generic::Reference< _TyConnect >							RpConnect;
			typedef Plib::Generic::Reference< _Request< _TyParser, _TyConnect > >	TFather;
			typedef Response< _TyParser, _TyConnect >								RResponse;
			typedef Response< _TyParser, _TyConnect >								TResponse;
		protected:
			Request< _TyParser, _TyConnect >( bool beNull ):TFather( false ) { CONSTRUCTURE; }
		public:
			// Default C'str
			Request< _TyParser, _TyConnect >( ) { CONSTRUCTURE; }
			// Copy
			Request< _TyParser, _TyConnect >( const Request< _TyParser, _TyConnect > & rhs )
				: TFather( rhs ) { CONSTRUCTURE; }
			~Request< _TyParser, _TyConnect >( ) { DESTRUCTURE; }
			
		public:
			// This Create is for service usage.
			// Fill a request by the socket connect to the service.
			INLINE bool Create( const RpConnect & _cnnt ) {
				return TFather::_Handle->_PHandle->Create( _cnnt );
			}
			// The Create is used for connect to other serivce.
			INLINE void Create( const RConnInfo & _cnntInfo ) {
				TFather::_Handle->_PHandle->Create( _cnntInfo );
			}
			// Initialize the Request by the creator and also initialize
			// the parser.
			template < typename _TyInitializer >
			INLINE void Create( const _TyInitializer & _initer ) {
				TFather::_Handle->_PHandle->Create( _initer );
			}
			// if the request is created by the service,
			// use this function to check if has new data incoming.
			INLINE int Check( ) {
				return TFather::_Handle->_PHandle->Check( );
			}
			
			INLINE Plib::Generic::Reference< _TyConnect > & GetConnect( ) {
				return TFather::_Handle->_PHandle->GetConnect();
			}
			INLINE bool IsConnect( ) const {
				return TFather::_Handle->_PHandle->IsConnect( );
			}
			// For debug usage.
			// Get the request stream.
			INLINE const Plib::Text::RString & GetRequestString( ) {
				return TFather::_Handle->_PHandle->GetRequestString( );
			}
			// Get the response.
			INLINE RResponse GetResponse( ) {
				return TFather::_Handle->_PHandle->GetResponse();
			}
			
			// Get the last error message
			INLINE const Plib::Text::RString & GetErrorMessage( ) const {
				return TFather::_Handle->_PHandle->GetErrorMessage();
			}
			// Connect to the Peer Server
			INLINE bool Connect( ) {
				return TFather::_Handle->_PHandle->Connect();
			}
			INLINE void ClearBuffer( ) {
				TFather::_Handle->_PHandle->ClearBuffer();
			}
			// Host
			INLINE const Plib::Text::RString & Host() const { 
				return TFather::_Handle->_PHandle->Host();
			}
			INLINE void SetHost( const Plib::Text::RString & _host ) { 
				TFather::_Handle->_PHandle->SetHost( _host );
			}
			// Get the request port info.
			INLINE Uint32 Port() const {
				return TFather::_Handle->_PHandle->Port();
			}
			
			INLINE void SetPort( Uint32 _port ) {
				TFather::_Handle->_PHandle->SetPort( _port );
			}
			
			// Get the request timeout setting info.
			INLINE Uint32 TimeOut() const {
				return TFather::_Handle->_PHandle->TimeOut();
			}
			
			INLINE void SetTimeOut( Uint32 _timeout ) {
				TFather::_Handle->_PHandle->SetTimeOut( _timeout );
			}
			
			// Get the request connection's keepalive setting info.
			INLINE bool KeepAlive() const {
				return TFather::_Handle->_PHandle->KeepAlive();
			}
			
			INLINE void SetKeepAlive( bool _keepAlive ) {
				TFather::_Handle->_PHandle->SetKeepAlive( _keepAlive );
			}
			
			// Get the connect's identify
			INLINE const Plib::Text::RString & Identify( ) const {
				return TFather::_Handle->_PHandle->Identify( );
			}
			
			// Get the Parser of the request.
			INLINE RpParser & GetParser() {
				return TFather::_Handle->_PHandle->GetParser();
			}

			// Just clear the parser and the bufferstream.
			INLINE void ReuseRequest( ) {
				TFather::_Handle->_PHandle->ReuseRequest();
			}

			// close current connection.
			INLINE void EndRequest( ) {
				TFather::_Handle->_PHandle->EndRequest();
			}
			
			// Clear the request object and release the data.
			INLINE void ReleaseRequest( ) {
				TFather::_Handle->_PHandle->ReleaseRequest();
			}
			
			const static Request< _TyParser, _TyConnect > Null;
			
			static Request< _TyParser, _TyConnect > CreateNullRequest( )
			{
				return Request< _TyParser, _TyConnect >( false );
			}
		};
		
		template< typename _TyParser, typename _TyConnect >
		const Request< _TyParser, _TyConnect > Request< _TyParser, _TyConnect >::Null( false );
	}
}

#endif // plib.network.request.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

