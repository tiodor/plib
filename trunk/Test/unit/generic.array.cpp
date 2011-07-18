#include <Plib-Generic/Array.hpp>

int main( int argc, char * argv [] )
{
	Plib::Generic::Array< int, 10 > _intArray;

	std::string _cmd;

	while ( true )
	{
		std::cout << "array_ut: $> ";
		getline( std::cin, _cmd );
		if ( _cmd.empty() ) continue;
		char * _tmp = new char[ _cmd.size() + 1 ];
		memcpy( _tmp, _cmd.c_str(), _cmd.size() );
		_tmp[ _cmd.size() ] = '\0';
		char * _Val = strtok( _tmp, " \t;," );
		if ( _Val == NULL ) {
			delete [] _tmp;
			continue;
		}
		if ( strcmp( _Val, "exit" ) == 0 ) {
			delete [] _tmp;
			break;
		}
		else if ( strcmp( _Val, "pushback" ) == 0 ) {
			do {
				_Val = strtok( NULL, " \t;," );
				if ( _Val == NULL ) break;
				_intArray.PushBack( atoi( _Val ) );
			} while ( true );
			_intArray.Print( std::cout );
			std::cout << std::endl;
		}
		else if ( strcmp( _Val, "pushfront" ) == 0 ) {
			do {
				_Val = strtok( NULL, " \t;," );
				if ( _Val == NULL ) break;
				_intArray.PushFront( atoi( _Val ) );
			} while ( true );
			_intArray.Print( std::cout );
			std::cout << std::endl;
		}
		else if ( strcmp( _Val, "popback" ) == 0 ) {
			_intArray.PopBack( );
			_intArray.Print( std::cout );
			std::cout << std::endl;
		}
		else if ( strcmp( _Val, "popfront" ) == 0 ) {
			_intArray.PopFront( );
			_intArray.Print( std::cout );
			std::cout << std::endl;
		}
		else if ( strcmp( _Val, "get" ) == 0 ) {
			do {
				_Val = strtok( NULL, " \t;," );
				if ( _Val == NULL ) break;
				std::cout << _Val << ") " << _intArray[atoi(_Val)] << std::endl;
			} while (true);
		}
		else if ( strcmp( _Val, "print" ) == 0 ) {
			_intArray.Print( std::cout );
			std::cout << std::endl;
		}
		else if ( strcmp( _Val, "set" ) == 0 ) {
			do {
				_Val = strtok( NULL, "=" );
				if ( _Val == NULL ) break;
				char * _v = strtok( NULL, " \t;," );
				if ( _v == NULL ) break;
				_intArray[atoi(_Val)] = atoi(_v);
			} while( true );
			_intArray.Print( std::cout );
			std::cout << std::endl;
		}
		else if ( strcmp( _Val, "size" ) == 0 ) {
			std::cout << "Size of Array is: " << _intArray.Size() << std::endl;
		}
		else if ( strcmp( _Val, "full" ) == 0 ) {
			std::cout << "Array statue is: " << (_intArray.Full() ? "FULL" : "NOT FULL") << std::endl;
		}
		else if ( strcmp( _Val, "empty" ) == 0 ) {
			std::cout << "Array statue is: " << (_intArray.Empty() ? "EMPTY" : "NOT EMPTY") << std::endl;
		}
		else if ( strcmp( _Val, "clear" ) == 0 ) {
			_intArray.Clear();
			_intArray.Print( std::cout );
			std::cout << std::endl;
		}
		else {
			std::cout << "Command Not Support." << std::endl;
		}

		delete [] _tmp;
	}

	return 0;
}
