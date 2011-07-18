/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: stopwatch.hpp
* Propose  			: Stop Watch Object Definition.
* 
* Current Version	: 1.1
* Change Log		: Re-Definition.
* Author			: Push Chen
* Change Date		: 2011-01-09
*/

#pragma once

#ifndef _PLIB_THREADING_STOPWATCH_HPP_
#define _PLIB_THREADING_STOPWATCH_HPP_

#if _DEF_IOS
#include "Plib.hpp"
#else
#include <Plib-Basic/Plib.hpp>
#endif

namespace Plib
{
	namespace Threading
	{
		// Calculate the Time Passed in mileseconds
		class StopWatch
		{
	#if _DEF_WIN32
			LARGE_INTEGER _Start, _End, _Frequency;
	#else
			struct timeval _Start, _End;
	#endif
			double _TimePassed;
		public:
			StopWatch( bool _StartNow = true )
				: _TimePassed( 0.0 )
			{
	#if _DEF_WIN32
				_Start.QuadPart = 0;
				_End.QuadPart = 0;
				_Frequency.QuadPart = 0;
				::QueryPerformanceFrequency(&_Frequency);
	#endif
				if ( _StartNow ) SetStart( );
			}

			INLINE void SetStart( )
			{
	#if _DEF_WIN32
				::QueryPerformanceCounter( &_Start );
	#else
				::gettimeofday( &_Start, NULL );
	#endif
			}

			INLINE void Tick( )
			{
	#if _DEF_WIN32
				::QueryPerformanceCounter( &_End );
				_TimePassed = (double)(_End.QuadPart - _Start.QuadPart) / 
					(double)_Frequency.QuadPart;
	#else
				::gettimeofday( &_End, NULL );
				_TimePassed = 1000000 * ( _End.tv_sec - _Start.tv_sec ) + 
					( _End.tv_usec - _Start.tv_usec );
				_TimePassed /= 1000000;
	#endif
			}

			INLINE double GetTimePassed()
			{
				return _TimePassed;
			}

			INLINE Uint64 GetMileSecUsed()
			{
				return ( Uint64 )(_TimePassed * 1000);
			}
		};
	}
}

#endif // plib.threading.stopwatch.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

