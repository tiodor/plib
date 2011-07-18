/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: pair.hpp
* Propose  			: Pair Definition. Make Two object into one pair.
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2010-12-30
*/

#pragma once

#ifndef _PLIB_GENERIC_PAIR_HPP_
#define _PLIB_GENERIC_PAIR_HPP_

#if _DEF_IOS
#include "Plib.hpp"
#else
#include <Plib-Basic/Plib.hpp>
#endif

namespace Plib
{
    namespace Generic
    {
        using namespace Generic;
        
        // Template Structure Pair
        // Make two items in one object.
        template < typename _TyFirst, typename _TySecond >
        struct Pair
        {
            _TyFirst        First;
            _TySecond       Second;
        
            // C'str support all three styles
            Pair< _TyFirst, _TySecond >( ) { }
            Pair< _TyFirst, _TySecond >( const _TyFirst & _1st, const _TySecond & _2nd )
                : First( _1st ), Second( _2nd ) { }
            Pair< _TyFirst, _TySecond >( const Pair< _TyFirst, _TySecond > & _rp )
                : First( _rp.First ), Second( _rp.Second ) { }
            
            // Baisc Operator.
            INLINE Pair< _TyFirst, _TySecond > & operator = ( 
                const Pair< _TyFirst, _TySecond > & _rp )
            {
                if ( this == &_rp ) return (*this);
                this->First = _rp.First;
                this->Second = _rp.Second;
                return (*this);
            }
            
            INLINE bool operator == ( const Pair< _TyFirst, _TySecond > & _rp )
            {
                return ( this == &_rp ) ? true : 
                    ( this->First == _rp.First ? 
                        ( this->Second == _rp.Second ) : false );
            }
            
            INLINE bool operator != ( const Pair< _TyFirst, _TySecond > & _rp )
            {
                return !( *this == _rp );
            }
        };
    }
}

#endif // plib.generic.pair.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */


