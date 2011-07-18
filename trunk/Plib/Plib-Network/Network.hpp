/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Network.hpp
* Propose  			: Include All Network Header files in this file
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-06-14
*/

#pragma once

#ifndef _PLIB_NETWORK_NETWORK_HPP_
#define _PLIB_NETWORK_NETWORK_HPP_

#if _DEF_IOS
#include "Listener.hpp"
#include "Network.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Selector.hpp"
#include "Service.hpp"
#include "Socketbasic.hpp"
#include "Syncsock.hpp"
#else
#include <Plib-Network/Listener.hpp>
#include <Plib-Network/Network.hpp>
#include <Plib-Network/Request.hpp>
#include <Plib-Network/Response.hpp>
#include <Plib-Network/Selector.hpp>
#include <Plib-Network/Service.hpp>
#include <Plib-Network/Socketbasic.hpp>
#include <Plib-Network/Syncsock.hpp>
#endif

#endif // plib.network.network.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

