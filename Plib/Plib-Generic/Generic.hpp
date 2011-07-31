/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Generic.hpp
* Propose  			: Include All Generic Head Files together.
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-01-10
*/

#pragma once

#ifndef _PLIB_GENERIC_GENERIC_HPP_
#define _PLIB_GENERIC_GENERIC_HPP_

#if _DEF_IOS
#include "Dequeue.hpp"
#include "Queue.hpp"
#include "Stack.hpp"
#include "Pair.hpp"
#include "Dictionary.hpp"
#include "Hashmap.hpp"
#include "Reference.hpp"
#include "Delegate.hpp"
#include "Pool.hpp"
#include "ArrayList.hpp"
#include "Order.hpp"
#include "Merge.hpp"
#include "Operator.hpp"
#else
#include <Plib-Generic/Dequeue.hpp>
#include <Plib-Generic/Queue.hpp>
#include <Plib-Generic/Stack.hpp>
#include <Plib-Generic/Pair.hpp>
#include <Plib-Generic/Dictionary.hpp>
#include <Plib-Generic/Hashmap.hpp>
#include <Plib-Generic/Reference.hpp>
#include <Plib-Generic/Delegate.hpp>
#include <Plib-Generic/Pool.hpp>
#include <Plib-Generic/ArrayList.hpp>
#include <Plib-Generic/Order.hpp>
#include <Plib-Generic/Merge.hpp>
#include <Plib-Generic/Operator.hpp>
#endif

#endif // plib.generic.generic.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

