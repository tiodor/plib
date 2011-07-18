#if _DEF_WIN32
#define _PLIB_STD_DEBUG_
#endif
#define _PLIB_DEBUG_SIMPLE_

#include <Plib-Generic/Generic.hpp>
#include <Plib-Utility/Utility.hpp>
#include <Plib-Threading/Threading.hpp>
#include <Plib-Network/Network.hpp>
#include <Plib-Text/String.hpp>

using namespace Plib;
using namespace Plib::Generic;
using namespace Plib::Utility;
using namespace Plib::Threading;
using namespace Plib::Network;
using namespace Plib::Text;

int main( int argc, char * argv[] )
{
	Int32 Port = 6543;
	Int32 _repeatCount = 100;
	if ( argc >= 2 ) Port = Convert::ToInt32( argv[1] );
	if ( argc >= 3 ) _repeatCount = Convert::ToInt32( argv[2] );
	if ( _repeatCount == 0 ) _repeatCount = 100;

	RPSyncSock rpSock;
	RString _buffer;
	rpSock->AttachReadBuffer( &_buffer );

	for ( int i = 0; i < _repeatCount; ++i )
	{
		if ( !rpSock->Connect( "127.0.0.1", Port, 1000 ) )
		{
			PTRACE( "Failed to connect to test server, check the server statue." );
			return 1;
		}
		rpSock->SetNoDelay();
		rpSock->SetReUsable( true );
		rpSock->SetLingerTime( 0 );
		RString _data = Convert::ToString( i );
		rpSock->Write( _data.C_Str(), _data.Size() );
		rpSock->Read( 1000 );
		PrintAsHex( _buffer.c_str(), _buffer.size() );
		_buffer.Clear();
		rpSock->Close();
	}

	return 0;
}

