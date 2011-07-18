#include <Plib-Network/Network.hpp>

using namespace Plib;
using namespace Plib::Generic;
using namespace Plib::Network;
using namespace Plib::Threading;

typedef ListenerFrame< Selector< SyncSock > >	TL;

struct ListenGT
{
	static TL		gtl;
};

TL ListenGT::gtl;

void TestWorking( )
{
	while( ThreadSys::Running( ) )
	{
		TL::RefSocketT rSock = ListenGT::gtl.GetReadableSocket( 1000 );
		//if ( rSock.RefNull( ) ) continue;
		
		ListenGT::gtl.ReleaseSocket( rSock, false );
	}
}

int main( int argc, char * argv[] )
{
	ListenGT::gtl.Listen( 6543 );
	Thread< void( void ) > tW;
	tW.Jobs += TestWorking;
	tW.Start();

	WaitForExitSignal();

	return 0;
}
