#include <Plib-Generic/Generic.hpp>

using namespace Plib::Generic;
using namespace Plib;

int main( int argc, char * argv[] )
{
	RArray< int > raInt;
	for ( Uint32 i = 0; i < 300; ++i )
	{
		raInt.PushBack( i );
	}
	for ( Uint32 i = 0; i < raInt.Size(); ++i )
	{
		std::cout << raInt[i] << " ";
	}
	std::cout << std::endl;
	for ( Uint32 i = 0; i < 299; ++i )
	{
		raInt.PopFront( );
	}
	for ( Uint32 i = 0; i < raInt.Size(); ++i )
	{
		std::cout << raInt[i] << " ";
	}
	std::cout << std::endl;
	for ( Uint32 i = 0; i < 298; ++i )
	{
		raInt.PushFront( i + 1 );
	}
	for ( Uint32 i = 0; i < raInt.Size(); ++i )
	{
		std::cout << raInt[i] << " ";
	}
	std::cout << std::endl;
	raInt.Clear();
	return 0;
}
