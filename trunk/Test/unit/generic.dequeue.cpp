#include <Plib-Generic/Dequeue.hpp>

int main ( int argc, char * argv[] )
{
	Plib::Generic::Dequeue< int, 32 > _intQueue;
	std::string _cmd;
	while ( true )
	{
		std::cout << "dequeue_ut: $> ";
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
				_intQueue.PushBack( atoi( _Val ) );
			} while ( true );
			_intQueue.Print( std::cout );
			std::cout << std::endl;
		}
		else if ( strcmp( _Val, "pushfront" ) == 0 ) {
			do {
				_Val = strtok( NULL, " \t;," );
				if ( _Val == NULL ) break;
				_intQueue.PushFront( atoi( _Val ) );
			} while ( true );
			_intQueue.Print( std::cout );
			std::cout << std::endl;
		}
		else if ( strcmp( _Val, "popback" ) == 0 ) {
			_intQueue.PopBack( );
			_intQueue.Print( std::cout );
			std::cout << std::endl;
		}
		else if ( strcmp( _Val, "popfront" ) == 0 ) {
			_intQueue.PopFront( );
			_intQueue.Print( std::cout );
			std::cout << std::endl;
		}
		else if ( strcmp( _Val, "head" ) == 0 ) {
			std::cout << "Head: " << _intQueue.Head() << std::endl;
		}
		else if ( strcmp( _Val, "tail" ) == 0 ) {
			std::cout << "Tail: " << _intQueue.Tail() << std::endl;
		}
		else if ( strcmp( _Val, "print" ) == 0 ) {
			_intQueue.Print( std::cout );
			std::cout << std::endl;
		}
		else if ( strcmp( _Val, "size" ) == 0 ) {
			std::cout << "Size of Array is: " << _intQueue.Size() << std::endl;
		}
		else if ( strcmp( _Val, "empty" ) == 0 ) {
			std::cout << "Array statue is: " << (_intQueue.Empty() ? "EMPTY" : "NOT EMPTY") << std::endl;
		}
		else if ( strcmp( _Val, "clear" ) == 0 ) {
			_intQueue.Clear();
			_intQueue.Print( std::cout );
			std::cout << std::endl;
		}
		else {
			std::cout << "Command Not Support." << std::endl;
		}

		delete [] _tmp;
	}
	
	return 0;
}