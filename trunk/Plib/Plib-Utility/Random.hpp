/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: random.hpp
* Propose  			: Generic Random String, Interge and so on.
* 
* Current Version	: 1.1
* Change Log		: Re-Definition.
* Author			: Push Chen
* Change Date		: 2011-01-10
*/

#pragma once

#ifndef _PLIB_UTILITY_RANDOM_HPP_
#define _PLIB_UTILITY_RANDOM_HPP_

#if _DEF_IOS
#include "String.hpp"
#else
#include <Plib-Text/String.hpp>
#endif

namespace Plib
{
	namespace Utility
	{
		/*
			class random definition.
		*/
		class Random
		{
		private:
			// Internal static object maintaince.
			INLINE static Random * _getIns()
			{
				static Random _Rd;
				return &_Rd;
			}
			// Initialize the random sead.
			INLINE static void _Randomize(void)
			{
				srand( (unsigned) time(NULL) );
			}
			// Default C'Str
			Random() { _Randomize(); }
			// Return an integer between low and high
			INLINE Uint32 _Integer( Uint32 _Low, Uint32 _High )
			{
				double dRlNum;
				dRlNum = ((double)rand())/((double)RAND_MAX + 1);
				return ( (unsigned int)(dRlNum * (double)(_High - _Low + 1)) + _Low );
			}
			// Return a double between low and high
			INLINE double _Real( double _Low, double _High )
			{
				double dRlNum;
				dRlNum = ((double)rand()) / ((double)RAND_MAX + 1);
				return (_Low + dRlNum * (_High - _Low));
			}
			// Take chance to return true.
			INLINE bool _Chance(double _C)
			{
				double dRlNum;
				dRlNum = _Real(0, 1);
				return dRlNum <= _C;
			}
			// Generate a random string.
			INLINE Plib::Text::RString _String( Uint32 _Len, bool _RandomCase = true )
			{
				Plib::Text::RString _BasicLine( _Len, 'A' );
				for ( Uint32 i = 0; i < _Len; ++i )
					_BasicLine[i] += ( _Integer(0, 25) + 
						((_RandomCase && _Chance(0.5)) ? 32 : 0) );
				return _BasicLine;
			}

		public:
			// Return an integer between low and high
			INLINE static Uint32 Integer( Uint32 _Low, Uint32 _High )
			{
				return Random::_getIns()->_Integer(_Low, _High);
			}
			// Return a double between low and high
			INLINE static double Real( double _Low, double _High )
			{
				return Random::_getIns()->_Real(_Low, _High);
			}
			// Take chance to return true.
			INLINE static bool Chance( double _C )
			{
				return Random::_getIns()->_Chance(_C);
			}
			// Generate a random string.
			INLINE static Plib::Text::RString Strings( Uint32 _Len, bool _RandomCase = true )
			{
				return Random::_getIns()->_String(_Len, _RandomCase);
			}
		};
	}
}

#endif // plib.utility.random.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */




