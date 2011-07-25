/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Operator.hpp
* Propose  			: Operator Set.
* 
* Current Version	: 1.0.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-07-02
*/

#pragma once

#ifndef _PLIB_GENERIC_OPERATOR_HPP_
#define _PLIB_GENERIC_OPERATOR_HPP_

#if _DEF_IOS
#include "Plib.hpp"
#else
#include <Plib-Basic/Plib.hpp>
#endif

namespace Plib
{
	namespace Generic
	{
		// Compare Operators
		
		// Operator >
		template < typename _TyObject > struct Great {
			INLINE bool operator () ( const _TyObject & _l, const _TyObject & _r ) {
				return ( _l > _r );
			}
		};
		
		// Operator >=
		template < typename _TyObject > struct GreatEqual {
			INLINE bool operator () ( const _TyObject & _l, const _TyObject & _r ) {
				return ( _l >= _r );
			}
		};
		
		// Operator <
		template < typename _TyObject > struct Less {
			INLINE bool operator () ( const _TyObject & _l, const _TyObject & _r ) {
				return ( _l < _r );
			}
		};
		
		// Operator <=
		template < typename _TyObject > struct LessEqual {
			INLINE bool operator () ( const _TyObject & _l, const _TyObject & _r ) {
				return ( _l <= _r );
			}
		};
		
		// Operator ==
		template < typename _TyObject > struct Equal {
			INLINE bool operator () ( const _TyObject & _l, const _TyObject & _r ) {
				return ( _l == _r );
			}
		};
		
		template < typename _TyObject1, typename _TyObject2 > struct TypeEqual {
			INLINE bool operator () ( const _TyObject1 & _l, const _TyObject2 & _r ) {
				return ( _l == _r );
			}
		};
	}
}

#endif // plib.generic.operator

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

