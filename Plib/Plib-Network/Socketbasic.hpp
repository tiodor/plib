/*
* Copyright (c) 2011, Push Chen
* All rights reserved.
* 
* File Name			: socket.hpp
* Propose  			: 
* 
* Current Version	: 1.4
* Change Log		: Update to AsyncSocket to Speed Up Sending and Receving in Windows
* Change Log V1.3	: Re-write all code and fix some bugs.
* Change Log V1.4	: Re-write Under the framework of Plib-1.1
* Author			: Push Chen
* Change Date		: 2010-7-6
*/

#pragma once

#ifndef _PLIB_NETWORK_SOCKET_HPP_
#define _PLIB_NETWORK_SOCKET_HPP_

#if _DEF_IOS
#include "Generic.hpp"
#include "Threading.hpp"
#include "String.hpp"
#else
#include <Plib-Generic/Generic.hpp>
#include <Plib-Threading/Threading.hpp>
#include <Plib-Text/Text.hpp>
#endif

#if _DEF_WIN32
	#include <WS2tcpip.h>
	#pragma comment( lib, "Ws2_32.lib" )
	#define PLIB_NETWORK_NOSIGNAL			0
	#define PLIB_NETWORK_IOCTL_CALL			ioctlsocket
	#define PLIB_NETWORK_CLOSESOCK			::closesocket
#else 
	#include <sys/socket.h>
	#include <unistd.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <netinet/tcp.h>
	#define PLIB_NETWORK_NOSIGNAL			MSG_NOSIGNAL
	#define PLIB_NETWORK_IOCTL_CALL			ioctl
	#define PLIB_NETWORK_CLOSESOCK			close
#endif

#if _DEF_MAC
	#undef 	PLIB_NETWORK_NOSIGNAL
	#define PLIB_NETWORK_NOSIGNAL			0
#endif

namespace Plib
{
	namespace Network
	{
		typedef long SOCKET_T;

		// In No-Windows
		#ifndef FAR
		#define FAR
		#endif

		/* Translate Domain to IP Address */
		INLINE char * Domain2Ip(const char * cp_Domain, char * pb_IPOutput, Uint32 vb_Len)
		{
			struct hostent FAR * xp_HostEnt;
			struct in_addr v_inAddr;
			char * xp_addr;

			memset(pb_IPOutput, 0, vb_Len);

			xp_HostEnt = gethostbyname(cp_Domain);
			if (xp_HostEnt == NULL) return pb_IPOutput;

			xp_addr = xp_HostEnt->h_addr_list[0];
			if (xp_addr == NULL) return pb_IPOutput;

			memmove(&v_inAddr, xp_addr, 4);
			strcpy(pb_IPOutput, inet_ntoa(v_inAddr));

			return pb_IPOutput;
		}

		/* Translate Domain to InAddr */
		inline unsigned int Domain2InAddr(const char * cp_Domain)
		{
			/* Get the IP Address of the domain by invoking usp_DomainToIP */
			char xa_IPAddress[16];

			if (cp_Domain == NULL) return INADDR_ANY;
			if (Domain2Ip(cp_Domain, xa_IPAddress, 16)[0] == '\0')
				return (unsigned int)(-1L);
			return inet_addr(xa_IPAddress);
		}

		#ifndef __SOCKET_SERVER_INIT_IN_WINDOWS__
		#define __SOCKET_SERVER_INIT_IN_WINDOWS__

		#if _DEF_WIN32

		// In Windows Only.
		// This class is used to initialize the WinSock Server.
		// A global instance of this object will be create and
		// provide nothing. only the c'str of this object
		// will invoke WSAStartup and the d'str will invoke 
		// WSACleanup.
		// In Linux or other platform, this object will not be
		// defined.
		template< int __TMP_VALUE__ = 0 >
		class __socket_init_svr_in_windows
		{
			__socket_init_svr_in_windows< __TMP_VALUE__ >()
			{
				WSADATA v_wsaData;
				WORD v_wVersionRequested;

				v_wVersionRequested = MAKEWORD(1, 1);
				WSAStartup(v_wVersionRequested, &v_wsaData);
			}

		public:
			~__socket_init_svr_in_windows< __TMP_VALUE__ >()
			{
				WSACleanup();
			}
			static __socket_init_svr_in_windows< __TMP_VALUE__ > __g_socksvrInWindows;
		};

		template< > __socket_init_svr_in_windows< 0 > 
		__socket_init_svr_in_windows< 0 >::__g_socksvrInWindows;

		#endif

		#endif

		// Socket Process Statue
		typedef enum {
			SOST_EMPTY = 0,			// Unconnected Socket.
			SOST_IDLE,				// Connected Socket With NO processing.
			SOST_CONNECTING,		// When connect method is been invoked.
			SOST_BINDING,			// When bind method is been invoked.
			SOST_CLOSING,			// When close method is been invoked.
			SOST_WRITING,			// When write method is been invoked.
			SOST_READING,			// When read method is been invoked.
			SOST_TIMEOUT,			// When get a timeout signal in read method.
			SOST_ERROR				// Anytime when an error is happened.
		} SOSTATUE;

		// Socket Process Return Value
		typedef enum {
			SOPROC_OK = 0,
			SOPROC_ERROR,
			SOPROC_TIMEOUT
		} SOPROCRET;
		
		// Socket read statue
		typedef enum {
			SOEVENT_OK = 0,
			SOEVENT_ILLEAGE = 1,
			SOEVENT_UNFINISHED = 2,
			SOEVENT_DONE = 3
		} SOCKEVENTSTATUE;

		typedef struct tagDATAPAIR {
			const char *	data;
			unsigned int	length;
		} SODATAPAIR, *LPDATAPAIR;

		/*
			IO Socket
			A class implements only the framework of the socket processes.
			The template object inside will do the detail work of write and read.
			_TySo should contain some callbacks of 
				Connecting
				Binding
			and should contain some methods of
				Writing
				Reading
		*/
		template < class _TySo, int SOCK_BUF_LENGTH = 1024 >
		class SocketBasic
		{
		public:
			// Public Definition.
			enum { SO_ADDR_LEN = 64 };

			typedef SocketBasic< _TySo, SOCK_BUF_LENGTH > SockType;
			typedef Plib::Generic::Delegate< SOCKEVENTSTATUE ( SockType *, void * ) > so_event;

		protected:
			// Inside Object.
			SOCKET_T		m_hSo;
			bool			m_bBound;	// if the socket is came from outside.

			// Socket Information.
			char			m_remoteAddr[SO_ADDR_LEN];
			char			m_localAddr[SO_ADDR_LEN];
			unsigned int	m_remotePort;
			unsigned int	m_localPort;

			// Statue
			SOSTATUE		m_lastStatue;
			SOSTATUE		m_statue;

			// Error Message
			char			m_errorMessage[SOCK_BUF_LENGTH];
			int				m_errorCode;

			// Stopwatch
			Plib::Threading::StopWatch		m_timeCalc;
			Plib::Text::RString *			m_bufferString;

		public:
			// Read Only Properities
			const SOCKET_T &		hSo;
			const bool &			IsBind;
			const char * const		RemoteAddress;
			const char * const		LocalAddress;
			const unsigned int &	RemotePort;
			const unsigned int &	LocalPort;

			const SOSTATUE &		LastStatue;
			const SOSTATUE &		Statue;

			const char * const		ErrorMessage;
			const int &				ErrorCode;
			const char * const		BufferData;

		public:
			// Event
			so_event	onStatueChange;
			so_event	onBinding;
			so_event	onConnecting;
			so_event	onConnected;
			so_event	onClosed;

			so_event	onParseData;
			so_event	onBufferUpdate;

			so_event	onTimeOut;
			so_event	onError;
			so_event	onBufferWarn;

		protected:
			// Inside Object
			_TySo		_T_so;

		protected:


			// Intenal Event Methods
			// On Statue Change.
			INLINE void _so_changeStatue( SOSTATUE _statue ) 
			{
				// Cannot change the statue from empty to empty or idle.
				if ( m_statue == SOST_EMPTY && 
					( _statue == SOST_IDLE || _statue == SOST_EMPTY ) ) return;
				m_lastStatue = m_statue;
				m_statue = _statue;
				if ( onStatueChange ) onStatueChange( this, NULL );
				m_timeCalc.SetStart( );
			}

			// On Error Happen. if defined ErrorMsg, the error code will be set to -1
			// which mean customized.
			INLINE void _so_errorHappen( const char * _errorMsg = NULL )
			{
				_so_changeStatue( SOST_ERROR );
				if ( _errorMsg ) {
					m_errorCode = -1;
					int _tLen = strlen( _errorMsg );
					::memcpy( m_errorMessage, _errorMsg, _tLen );
					m_errorMessage[_tLen] = 0;
				} else {
					Plib::Text::RString _ErrMessage = Plib::Text::LastErrorMessage;
					int _tLen = SOCK_BUF_LENGTH < _ErrMessage.Size( ) ? 
						SOCK_BUF_LENGTH - 1 : _ErrMessage.Size( );
					::memcpy( m_errorMessage, _ErrMessage.C_Str( ), _tLen );
					m_errorMessage[_tLen] = 0;
					//getLastErrorMsg( m_errorMessage, SOCK_BUF_LENGTH );
				}
				if ( onError ) onError( this, NULL );
				this->Close();
			}

			// Get socket Information.
			INLINE void _so_sockInfo( )
			{
				if ( m_hSo == -1 ) return;

				struct sockaddr_in _addr;
				socklen_t _addrLen = sizeof(_addr);
				::memset( &_addr, 0, sizeof(_addr) );
				if ( 0 == getsockname( m_hSo, (struct sockaddr *)&_addr, &_addrLen) )
				{
					m_localPort = ntohs(_addr.sin_port);
					sprintf( m_localAddr, "%u.%u.%u.%u",
						(unsigned int)(_addr.sin_addr.s_addr >> (0 * 8)) & 0x00FF,
						(unsigned int)(_addr.sin_addr.s_addr >> (1 * 8)) & 0x00FF,
						(unsigned int)(_addr.sin_addr.s_addr >> (2 * 8)) & 0x00FF,
						(unsigned int)(_addr.sin_addr.s_addr >> (3 * 8)) & 0x00FF );
				}
				memset( &_addr, 0, sizeof(_addr) );
				if ( 0 == getpeername( m_hSo, (struct sockaddr *)&_addr, &_addrLen ) )
				{
					m_remotePort = ntohs(_addr.sin_port);
					sprintf( m_remoteAddr, "%u.%u.%u.%u",
						(unsigned int)(_addr.sin_addr.s_addr >> (0 * 8)) & 0x00FF,
						(unsigned int)(_addr.sin_addr.s_addr >> (1 * 8)) & 0x00FF,
						(unsigned int)(_addr.sin_addr.s_addr >> (2 * 8)) & 0x00FF,
						(unsigned int)(_addr.sin_addr.s_addr >> (3 * 8)) & 0x00FF );
				}
			}

			INLINE int _so_select( bool _read = true )
			{
				if ( m_hSo == -1 ) return -1;
				fd_set _fs;
				FD_ZERO(&_fs);
				FD_SET( m_hSo, &_fs );

				int _retCode = 0;
				struct timeval _tv = {0, 0};
				do {
					_retCode = ::select( m_hSo + 1, (_read ? &_fs : NULL),
													(_read ? NULL : &_fs),
													NULL, &_tv );
					if ( _retCode == -1 ) {
						// Check the error number
						int _err = PLIB_LASTERROR;
						if ( _err != EINTR ) return -1;
						continue;
					}
					if ( _retCode == 0 ) {
						return 0;
					}
					return 1;
				} while ( 1 );

				return -1;
			}

		public:
			// Public Methods

			// C'Str
			SocketBasic< _TySo, SOCK_BUF_LENGTH >( ) :
				// Protected Init
				m_hSo( -1 ), m_bBound( false ), 
				m_lastStatue( SOST_EMPTY ), m_statue( SOST_EMPTY ),
				m_errorCode( 0 ), m_bufferString( NULL ),
				// Reference Inti
				hSo( m_hSo ), IsBind( m_bBound ),
				RemoteAddress( m_remoteAddr ), LocalAddress( m_localAddr ),
				RemotePort( m_remotePort ), LocalPort( m_localPort ),
				LastStatue( m_lastStatue ), Statue( m_statue ),
				ErrorMessage( m_errorMessage ), ErrorCode( m_errorCode ), 
				BufferData( m_errorMessage )
			{
				CONSTRUCTURE;
				m_remoteAddr[0] = 0;
				m_localAddr[0] = 0;
				m_errorMessage[0] = 0;

				_T_so.initialize( this );
			}

			// Get socket outside, bind it.
			SocketBasic< _TySo, SOCK_BUF_LENGTH >( SOCKET_T _hSo, bool _CompleteControl = false ) : 
				// Protected Init
				m_hSo( -1 ), m_bBound( false ), 
				m_lastStatue( SOST_EMPTY ), m_statue( SOST_EMPTY ),
				m_errorCode( 0 ), m_bufferString( NULL ),
				// Reference Inti
				hSo( m_hSo ), IsBind( m_bBound ),
				RemoteAddress( m_remoteAddr ), LocalAddress( m_localAddr ),
				RemotePort( m_remotePort ), LocalPort( m_localPort ),
				LastStatue( m_lastStatue ), Statue( m_statue ),
				ErrorMessage( m_errorMessage ), ErrorCode( m_errorCode ), 
				BufferData( m_errorMessage )
			{
				CONSTRUCTURE;
				m_remoteAddr[0] = 0;
				m_localAddr[0] = 0;
				m_errorMessage[0] = 0;
				
				_T_so.initialize( this );

				Bind( _hSo, _CompleteControl );
			}

			// D'str
			// Close the socket.
			~SocketBasic< _TySo, SOCK_BUF_LENGTH >( )
			{
				DESTRUCTURE;
				Close();
			}

			// Socket Option Change
			INLINE bool SetSoWriteTimeOut( unsigned int _mileSec )
			{
				return _T_so.SetWriteTimeOut( this, _mileSec );
			}
			INLINE bool SetSoReadTimeOut( unsigned int _mileSec )
			{
				return _T_so.SetReadTimeOut( this, _mileSec );
			}

			INLINE bool SetReUsable( bool beReused )
			{
				if ( m_hSo == -1 ) return false;
				int _reused = beReused ? 1 : 0;
				return setsockopt( m_hSo, SOL_SOCKET, SO_REUSEADDR,
					(const char *)&_reused, sizeof(int) ) != -1;
			}

			INLINE bool SetNoDelay( )
			{
				if ( m_hSo == -1 ) return false;
				int flag = 1;
				return setsockopt( m_hSo, IPPROTO_TCP, 
					TCP_NODELAY, (const char *)&flag, sizeof(int) ) != -1;
 			}

			INLINE bool SetSoWriteBufferSize( unsigned int _size )
			{
				if ( m_hSo == -1 ) return false;
				return setsockopt( hSo, SOL_SOCKET, SO_SNDBUF,
					(const char *)&_size, sizeof(unsigned int) ) != -1;
			}
			INLINE bool SetSoReadBufferSize( unsigned int _size )
			{
				if ( m_hSo == -1 ) return false;
				return setsockopt( m_hSo, SOL_SOCKET, SO_RCVBUF,
					(const char *)&_size, sizeof(unsigned int) ) != -1;
			}
			// Set the Linger time when close socket.
			// Set _time to 0 to turn off this feature.
			INLINE bool SetLingerTime( unsigned int _time )
			{
				if ( m_hSo == -1 ) return false;
				linger _lnger = { (_time > 0 ? 1 : 0), _time };
				return setsockopt( m_hSo, SOL_SOCKET, SO_LINGER,
					(const char*)&_lnger, sizeof(linger) ) != -1;
			}
			
			// Bind the string buffer object.
			INLINE void AttachReadBuffer( Plib::Text::RString * _stringBuffer ) {
				if ( _stringBuffer == NULL ) return;
				m_bufferString = _stringBuffer;
			}
			
			INLINE void UnAttachReadBuffer( ) {
				m_bufferString = NULL;
			}

			INLINE bool Connect( const char * _addr, unsigned _port, 
				unsigned _timeOut = 0, unsigned _localPort = 0 )
			{
				//
				_so_changeStatue( SOST_CONNECTING );
				if ( _addr == NULL || _port == 0 )
				{
					_so_errorHappen( "Invalidate Server Address or Port" );
					return false;
				}
				
				// Close Old Socket Object.
				if ( m_hSo != -1 ) this->Close();

				unsigned int _inAddr = Domain2InAddr( _addr );
				if ( _inAddr == (unsigned int)(-1) ) {
					_so_errorHappen();
					return false;
				}

				// Create Socket Handle
				m_hSo = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if ( hSo == -1 ) {
					_so_errorHappen();
					return false;
				}
				
				// Set With TCP_NODELAY
				int flag = 1;
				if( setsockopt( m_hSo, IPPROTO_TCP, 
					TCP_NODELAY, (const char *)&flag, sizeof(int) ) == -1 )
				{
					_so_errorHappen();
					return false;
				}

				struct sockaddr_in _sockAddr;
				memset( &_sockAddr, 0, sizeof(_sockAddr) );

				// Bind Local Port
				if ( _localPort != 0 )
				{
					_sockAddr.sin_addr.s_addr = INADDR_ANY;
					_sockAddr.sin_family = AF_INET;
					_sockAddr.sin_port = htons(_localPort);
					if ( ::bind(m_hSo, (struct sockaddr *)&_sockAddr, 
						sizeof(_sockAddr)) == -1) /* Error */
					{
						_so_errorHappen();
						return false;
					}
				}

				_sockAddr.sin_addr.s_addr = _inAddr;
				_sockAddr.sin_family = AF_INET;
				_sockAddr.sin_port = htons(_port);

				if ( _timeOut > 0 )
				{
					unsigned long _u = 1;
					PLIB_NETWORK_IOCTL_CALL(m_hSo, FIONBIO, &_u);
				}

				// Before Connect
				if ( onConnecting )  onConnecting( this, NULL );

				if ( ::connect( m_hSo, (struct sockaddr *)&_sockAddr, 
						sizeof(_sockAddr) ) == -1 )
				{
					if ( _timeOut == 0 ) {
						_so_errorHappen();
						return false;
					}
					struct timeval _tm = { _timeOut / 1000, (_timeOut % 1000) * 1000 };
					fd_set _fs;
					int _error = 0, len = 0;
					FD_ZERO( &_fs );
					FD_SET( m_hSo, &_fs );
					bool rtn = false;
					if ( ::select(m_hSo + 1, NULL, &_fs, NULL, &_tm) > 0 )
					{
						getsockopt( m_hSo, SOL_SOCKET, SO_ERROR, 
							(char *)&_error, (socklen_t *)&len);
						if ( _error == 0 ) rtn = true;
					}
					if ( !rtn )
					{
						_so_errorHappen();
						return false;
					}
				}
				// Reset Socket Statue
				if ( _timeOut > 0 )
				{
					unsigned long _u = 0;
					PLIB_NETWORK_IOCTL_CALL(m_hSo, FIONBIO, &_u);
				}

				// Get Socket Remote Address and Local Port
				_so_sockInfo();

				if ( onConnected ) onConnected( this, NULL );
				_so_changeStatue( SOST_IDLE );
				return true;
			}

			INLINE bool Connect( const std::string & _addr, unsigned _port,
				unsigned _timeOut = 0, unsigned _localPort = 0 )
			{
				return this->Connect( _addr.c_str(), _port, _timeOut, _localPort );
			}
			
			INLINE bool Connect( const Plib::Text::RString & _addr, unsigned _port,
			 	unsigned _timeOut = 0, unsigned _localPort = 0 )
			{
				return this->Connect( _addr.C_Str(), _port, _timeOut, _localPort );
			}
			
			// Connect Statue Check
			INLINE bool IsConnect( )
			{
				int _ret = _so_select();
				if ( _ret == -1 ) {
					//PTRACE( "_so_select return error." );
					this->Close();
					return false;
				}
				if ( _ret == 0 ) return true;
				if ( ::recv(m_hSo, m_errorMessage, 1, MSG_PEEK) <= 0 ) {
					//PTRACE( "recv return error." );
					this->Close();
					return false;
				}
				return true;
			}

			// ReadStatue Check
			INLINE bool IsReadable( )
			{
				int _ret = _so_select();
				if ( _ret == -1 ) {
					//PTRACE( "_so_select return error." );
					this->Close();
					return false;
				}
				if ( _ret == 0 ) {
					//PTRACE( "_so_select return timeout." );
					return false;
				}
				if ( ::recv( m_hSo, m_errorMessage, 1, MSG_PEEK ) <= 0 ) {
					//PTRACE( "recv return error." );
					this->Close();
					return false;
				}
				return true;
			}

			// WriteStatue Check
			INLINE bool IsWritable( )
			{
				int _ret = _so_select( false );
				if ( _ret == -1 ) {
					//PTRACE( "_so_select return error." );
					this->Close();
					return false;
				}
				return true;
			}

			INLINE bool Bind( SOCKET_T _hSo, bool _CompleteControl = false )
			{
				assert( _hSo <= 65535 );
				if ( _hSo == -1 ) return false;
				m_hSo = _hSo;
				if ( !_CompleteControl ) m_bBound = true;
				else m_bBound = false;

				// Get Socket info.
				_so_sockInfo();
				_so_changeStatue( SOST_BINDING );
				if ( onBinding ) onBinding( this, NULL );
				
				_so_changeStatue( SOST_IDLE );

				return true;
			}

			INLINE void Close( )
			{
				if ( m_hSo == -1 ) return;
				if ( m_bBound ) return;
				// Before Close;
				_so_changeStatue( SOST_CLOSING );

				PLIB_NETWORK_CLOSESOCK( m_hSo );

				m_hSo = -1;
				this->m_localPort = 0;
				this->m_remotePort = 0;
				this->m_localAddr[0] = '\0';
				this->m_remoteAddr[0] = '\0';

				if ( onClosed ) onClosed( this, NULL );
				_so_changeStatue( SOST_EMPTY );
			}

			INLINE bool Write( const char * _data, unsigned _length )
			{
				if ( m_hSo == -1 ) return false;
				if ( _data == NULL || _length == 0 ) return false;

				_so_changeStatue( SOST_WRITING );
				SOPROCRET _ret = _T_so.writeData( this, _data, _length );
				if ( _ret == SOPROC_OK ) _so_changeStatue( SOST_IDLE );
				else if ( _ret == SOPROC_ERROR ) _so_errorHappen();
				else {
					_so_changeStatue( SOST_TIMEOUT );
					if ( onTimeOut ) onTimeOut( this, (void *)&_length );
					_so_changeStatue( SOST_IDLE );
				}
				return _ret == SOPROC_OK;
			}

			// _in_out_ bufSize;
			INLINE bool Read( char * _outBuf, unsigned & _bufSize, unsigned int _timeOut = 1000 )
			{
				_so_changeStatue( SOST_READING );
				SOPROCRET _ret = _T_so.readData( this, _outBuf, _bufSize, _timeOut );
				if ( _ret == SOPROC_OK ) _so_changeStatue( SOST_IDLE );
				else if ( _ret == SOPROC_ERROR ) _so_errorHappen();
				else {
					_so_changeStatue( SOST_TIMEOUT );
					if ( onTimeOut ) onTimeOut( this, NULL );
					_so_changeStatue( SOST_IDLE );
				}
				return _ret == SOPROC_OK;
			}
			
			INLINE bool Read( unsigned int _timeOut = 1000 )
			{
				if ( m_bufferString == NULL ) {
					_so_changeStatue( SOST_IDLE );
					if ( onBufferWarn ) onBufferWarn( this, NULL );
					return false;
				}
				_so_changeStatue( SOST_READING );
				SOPROCRET _ret = _T_so.readData( this, m_bufferString, 
					m_errorMessage, SOCK_BUF_LENGTH, _timeOut );
				if ( _ret == SOPROC_OK ) _so_changeStatue( SOST_IDLE );
				else if ( _ret == SOPROC_ERROR ) _so_errorHappen();
				else {
					_so_changeStatue( SOST_TIMEOUT );
					if ( onTimeOut ) onTimeOut( this, NULL );
					_so_changeStatue( SOST_IDLE );
				}
				return _ret == SOPROC_OK;
			}
			
			// Echo what current socket recived to the sender
			INLINE void Echo( char * _echoBuf, unsigned int _bufSize )
			{
				unsigned int _readSize = _bufSize;
				while ( this->Read( _echoBuf, _readSize ) )
				{
					if ( !this->Write( _echoBuf, _readSize ) )
						break;
					_readSize = _bufSize;
				}
			}
			
			INLINE void Echo( )
			{
					//char * _Buffer = new char[SOCK_BUF_LENGTH];
				Echo( m_errorMessage, SOCK_BUF_LENGTH );
			}

			INLINE Uint64 GetIdleTime( )
			{
				m_timeCalc.Tick( );
				return m_timeCalc.GetMileSecUsed( );
			}
		};
	}
}

#endif // plib.network.socket.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */


