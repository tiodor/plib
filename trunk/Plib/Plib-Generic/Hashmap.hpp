/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Hashmap.hpp
* Propose  			: A hashmap
* 
* Current Version	: 1.00
* Change Log		: First Defintions.
* Author			: Push Chen
* Change Date		: 2011-07-02
*/

#pragma once

#ifndef _PLIB_GENERIC_HASHMAP_HPP_
#define _PLIB_GENERIC_HASHMPA_HPP_

#if _DEF_IOS
#include "Allocator.hpp"
#include "Pair.hpp"
#else
#include <Plib-Basic/Allocator.hpp>
#include <Plib-Generic/Pair.hpp>
#endif

namespace Plib
{
	namespace Generic
	{
		/*
		template < 
			typename _TyKey, 
			typename _TyValue, 
			typename _HashFunc = IntegerHash, 
			typename _TyAlloc = Plib::Basic::Allocator< Pair< _TyKey, _TyValue > >,
			Uint32 _HashSize = 1024
		>
		class Hashmap
		{
			typedef Pair< _TyKey, _TyValue >		ENTRY;
			ENTRY[_HashSize]						_HashTable;
			
		};
		*/
	}
}

#endif // plib.generic.hashmap.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */


