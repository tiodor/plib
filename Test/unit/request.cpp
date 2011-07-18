#include <Plib-Network/Network.hpp>

using namespace Plib;
using namespace Plib::Generic;
using namespace Plib::Network;
using namespace Plib::Text;
using namespace Plib::Threading;

struct TCreator
{
	RConnInfo		mCInfo;
	RConnInfo &		ConnectInfo( ) { return mCInfo; }
};

class TParser
{
public:
	typedef TCreator Creator;

	SOCKEVENTSTATUE ParseIncoming( SyncSock * pSock, void * vData )
	{ return SOEVENT_DONE; }

	void Create( const Creator & _ctor ){ }
	void Clear( ) { }
	void Build( RString & _bufferStream ){ _bufferStream = "0"; }
	bool IsLeagalRequest( ) { return true; }
};

int main( int argc, char * argv[] )
{
	Request< TParser > req;
	req.GetResponse( );
	return 0;
}
