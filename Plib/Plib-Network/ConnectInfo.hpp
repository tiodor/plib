/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: ConnectInfo.hpp
* Propose  			: Network connection info
* 
* Current Version	: 1.1.0
* Change Log		: Re-organize.
* Author			: Push Chen
* Change Date		: 2011-07-04
*/

#pragma once

#ifndef _PLIB_NETWORK_CONNECTINFO_HPP_
#define _PLIB_NETWORK_CONNECTINFO_HPP_

#if _DEF_IOS
#include "Generic.hpp"
#else
#include <Plib-Generic/Generic.hpp>
#endif

namespace Plib
{
	namespace Network
	{		
		/*
			Connection Info 
			Host: the peer address, can be a domain or an IP.
			Port: the peer port.
			TimeOut: for connect, read, write
			KeepAlive: if the server or the request should maintains the connection.
		*/
		typedef struct tagCONNECTINFO
		{
			Plib::Text::RString			Host;
			Uint32						Port;
			Uint32						TimeOut;
			bool						KeepAlive;
			
			tagCONNECTINFO & operator = ( const tagCONNECTINFO & rtci )
			{
				Host.DeepCopy( rtci.Host );
				Port = rtci.Port;
				TimeOut = rtci.TimeOut;
				KeepAlive = rtci.KeepAlive;
				return *this;
			}
		} CONNECTINFO;
				
		// RConnInfo, re-orginized.
		class RConnInfo : public Plib::Generic::Reference< CONNECTINFO >
		{
			typedef Plib::Generic::Reference< CONNECTINFO >	TFather;
		protected:
			RConnInfo( bool _beNull ) : TFather( false ) { CONSTRUCTURE; }
		public:
			// Default C'str
			RConnInfo( ) { CONSTRUCTURE; }
			RConnInfo( const RConnInfo & rhs ) : TFather( rhs ) { CONSTRUCTURE; }
			~RConnInfo( ) { DESTRUCTURE; }
			// Host
			Plib::Text::RString Host() const {
				return TFather::_Handle->_PHandle->Host;
			}
			void SetHost( Plib::Text::RString _host ) {
				TFather::_Handle->_PHandle->Host.DeepCopy(_host);
			}
			
			// Port
			Uint32 Port() const {
				return TFather::_Handle->_PHandle->Port;
			}
			void SetPort( Uint32 _port ) {
				TFather::_Handle->_PHandle->Port = _port;
			}
			
			// TimeOut
			Uint32 TimeOut() const {
				return TFather::_Handle->_PHandle->TimeOut;
			}
			void SetTimeOut( Uint32 _timeout ) {
				TFather::_Handle->_PHandle->TimeOut = _timeout;
			}
			
			// KeepAlive
			bool KeepAlive() const {
				return TFather::_Handle->_PHandle->KeepAlive;
			}
			void SetKeepAlive( bool _keepalive ) {
				TFather::_Handle->_PHandle->KeepAlive = _keepalive;
			}
			
			const static RConnInfo	Null;
		};
		
		// Null object.
		const RConnInfo RConnInfo::Null( false );
	}
}

#endif // plib.network.connectinfo.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

