/*
* Copyright (c) 2011, Push Chen
* All rights reserved.
* 
* File Name			: Response.hpp
* Propose  			: The server/client response object template
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-06-10
*/

#pragma once
#ifndef _PLIB_NETWORK_RESPONSE_HPP_
#define _PLIB_NETWORK_RESPONSE_HPP_

#if _DEF_IOS
#include "ConnectInfo.hpp"
#include "Syncsock.hpp"
#include "String.hpp"
#else
#include <Plib-Network/ConnectInfo.hpp>
#include <Plib-Network/Syncsock.hpp>
#include <Plib-Text/String.hpp>
#endif

namespace Plib
{
	namespace Network
	{		
		// Pre-definition of the request.
		template< typename _TyParser, typename _TyConnect = SyncSock >
		class _Request;
		
		// Response Object, used with Request.
		template< typename _TyParser, typename _TyConnect = SyncSock >
		class _Response
		{
		public:
			typedef Plib::Generic::Reference< _TyParser >  RpParser;
			typedef Plib::Generic::Reference< _TyConnect >	RpConnect;
						
			friend class _Request< _TyParser, _TyConnect >;
		protected:
			
			// Internal Sharable Objects
			Plib::Text::RString				m_responseStream;
			RpParser						m_rpParser;
			RConnInfo						m_rConnectInfo;
			
			bool 							m_Serialized;
			
		protected:
			// Internal Method for Request use
			
			// Initialize the response for response use.
			// The reqeust is created by the server.
			void __Init(  RConnInfo _cnntInfo )
			{
				m_rConnectInfo.DeepCopy( _cnntInfo );				
				// Init the parser.
				if ( m_rpParser.RefNull() ) m_rpParser = RpParser( );
				m_Serialized = false;
			}
			
			// Initialize the response for request use.
			// The reqeust is create by the parser::creator.
			void __Init( RpConnect _rpConnect, RConnInfo _cnntInfo )
			{
				if ( m_rpParser.RefNull() ) m_rpParser = RpParser( );
				// Copy the connect info.
				m_rConnectInfo.DeepCopy( _cnntInfo );
				
				// bind the onParseData event.
				m_responseStream.Clear();
				_rpConnect->AttachReadBuffer( &m_responseStream );
				_rpConnect->onBufferUpdate.Clear();
				_rpConnect->onBufferUpdate += std::make_pair(
					&(*m_rpParser), &_TyParser::ParseIncoming );
				m_Serialized = false;
			}
			
		public:
			
			_Response< _TyParser, _TyConnect >( )
				: m_rpParser( false ), m_Serialized( false )
			{
				CONSTRUCTURE;
				// Nothing to do.
			}
			~_Response< _TyParser, _TyConnect >( )
			{ DESTRUCTURE; }
			// Make the parser to process the string output.
			const Plib::Text::RString & GetResponseString( )
			{
				return m_responseStream;
			}
			
			// Serialize the response package.
			void Serialize( )
			{
				if ( m_Serialized == true ) return;
				m_responseStream.Clear();
				if ( m_rpParser.RefNull() ) return;
				m_rpParser->Build( m_responseStream );
				m_Serialized = true;
			}
			
			// the operator of compare.
			bool operator == ( const _Response< _TyParser, _TyConnect > & _resp ) const
			{
				return false; 
			}
			
			bool operator != ( const _Response< _TyParser, _TyConnect > & _resp ) const
			{
				return true;
			}
			
		public:
			// Other Methods. 
			
			// Get the Parser object.
			RpParser GetParser( )
			{
				return m_rpParser;
			}
			
			Plib::Text::RString Host() const 
			{
				return m_rConnectInfo.Host();
			}
			
			Uint32 Port() const
			{
				return m_rConnectInfo.Port();
			}
			
			bool KeepAlive() const
			{
				return m_rConnectInfo.KeepAlive();
			}
		
			// Just clear the parser
			void ReuseResponse( )
			{
				if ( !m_rpParser.RefNull() ) m_rpParser->Clear();
				m_responseStream.Clear();
			}
			
			// clear the parser
			void EndResponse( )
			{
				if ( !m_rpParser.RefNull() ) m_rpParser->Clear();
				m_responseStream.Clear();
			}
			
			// Release the connect and the parser
			void ReleaseResponse( )
			{
				m_rpParser = RpParser::NullRefObj;
				m_responseStream.Clear();
			}
		};
		
		
		// Response Object, used with Request.
		template< typename _TyParser, typename _TyConnect = SyncSock >
		class Response : public Plib::Generic::Reference< _Response< _TyParser, _TyConnect > >
		{
			typedef Plib::Generic::Reference< _Response< _TyParser, _TyConnect > >		TFather;
			typedef Plib::Generic::Reference< _TyParser >								RpParser;
		protected:
			Response<_TyParser, _TyConnect>( bool _beNull ) : TFather( false ) { CONSTRUCTURE; }
		public:
			// Default C'str
			Response<_TyParser, _TyConnect>( ) { CONSTRUCTURE; }
			// Copy C'str
			Response<_TyParser, _TyConnect>( const Response<_TyParser, _TyConnect> & rhs )
				: TFather( rhs )  { CONSTRUCTURE; }
			~Response< _TyParser, _TyConnect >( ) { DESTRUCTURE; }
			// Make the parser to process the string output.
			const Plib::Text::RString & GetResponseString( ) {
				return TFather::_Handle->_PHandle->GetResponseString();
			}
			
			// Build the response package.
			void Serialize( ) {
				TFather::_Handle->_PHandle->Serialize( );
			}
			
			// Get the Parser object.
			RpParser GetParser( ) {
				return TFather::_Handle->_PHandle->GetParser();
			}
			
			// Just clear the parser
			void ReuseResponse( ) {
				TFather::_Handle->_PHandle->ReuseResponse( );
			}
			
			// Release the connection and clear the parser
			void EndResponse( ) {
				TFather::_Handle->_PHandle->EndResponse( );
			}
			
			// Release the connect and the parser
			void ReleaseResponse( ) {
				TFather::_Handle->_PHandle->ReleaseResponse( );
			}
			
			// Get the host
			const Plib::Text::RString & Host() const {
				return TFather::_Handle->_PHandle->Host();
			}
			
			Uint32 Port() const {
				return TFather::_Handle->_PHandle->Port();
			}
			
			Uint32 KeepAlive( ) const {
				return TFather::_Handle->_PHandle->KeepAlive();
			}
			
			const static Response<_TyParser, _TyConnect> Null;
		};
		
		// Const Null object
		template< typename _TyParser, typename _TyConnect >
		const Response< _TyParser, _TyConnect > Response< _TyParser, _TyConnect >::Null(false);
	}
}

#endif // plib.network.resposne.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */


