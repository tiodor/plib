
#if _DEF_WIN32
#define _PLIB_STD_DEBUG_
#endif
#define _PLIB_DEBUG_SIMPLE_

#include <Plib-Generic/Generic.hpp>
#include <Plib-Utility/Utility.hpp>
#include <Plib-Threading/Threading.hpp>
#include <Plib-Network/Network.hpp>

using namespace Plib;
using namespace Plib::Generic;
using namespace Plib::Utility;
using namespace Plib::Threading;
using namespace Plib::Network;
using namespace Plib::Text;

struct TestCreator
{
	RConnInfo	mCInfo;
	int			initData;
};

class TestParser
{
public:
	int _data;
public:
	typedef TestCreator		Creator;
public:
	TestParser( )
	{
		_data = 0;
	}

	SOCKEVENTSTATUE ParseIncoming( SyncSock * pSock, void * vData )
	{
		Plib::Text::RString * pStringBuffer = (RString *)vData;
		_data = pStringBuffer->IntValue();
		return SOEVENT_DONE;
	}


	void Create( const Creator & _ctor )
	{
		_data = _ctor.initData;
	}

	void SetData( int _d )
	{
		//(*m_stringBuffer) = _data;
		_data = (_d + 1) * 2;
	}

	void Clear( )
	{
		//m_stringBuffer->Clear();
		_data = 0;
	}
	
	void Build( Plib::Text::RString & _bufferStream )
	{
		_bufferStream.Format( "The data is %d", _data );
	}
	
	bool IsLegalRequest() {
		return true;
	}
};

typedef Service< TestParser, Selector<SyncSock> >	TServices;
typedef TServices::RpRequest						TRequest;
typedef TServices::RpResponse						TResponse;
typedef TServices::RpParser							TParser;

TResponse ServerProcess( TRequest req )
{
	return req.GetResponse( );
/*
	TParser parser = req.GetParser();
	RString _reqStream = req.GetRequestString();
	PrintAsHex( _reqStream.c_str(), _reqStream.size() );

	TResponse resp = req.GetResponse( );
	resp.GetParser()->SetData( parser->_data );
	resp.Serialize();
	PrintAsHex( resp.GetResponseString().c_str(), resp.GetResponseString().size() );
	return resp;
*/
}

void ServerError ( Uint32 eCode )
{
	printf( "server error: %d, %s\n", eCode, GetErrorMessage( eCode ).C_Str() );
}

void ServerLosePort( TServices * pServer )
{
	printf( "The server has lose the port" );
}

int main( int argc, char * argv [] )
{
	int Port = 6543;
	int Count = 1;
	if ( argc >= 2 ) Port = Convert::ToInt32( argv[1] );
	if ( argc >= 3 ) Count = Convert::ToInt32( argv[2] );
	TServices _theSvr( true, 0 );
	_theSvr.WorkProcess += ServerProcess;
	_theSvr.OnServerError += ServerError;
	
	if ( !_theSvr.StartServer( Port, Count ) )
		return 1;
	Plib::Threading::WaitForExitSignal();
	_theSvr.StopServer( );
	return 0;
}
