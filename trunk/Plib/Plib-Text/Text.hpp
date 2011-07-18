/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Text.hpp
* Propose  			: Include All Text Header files in this file
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-07-08
*/


#pragma once

#ifndef _PLIB_TEXT_TEXT_HPP_
#define _PLIB_TEXT_TEXT_HPP_

#if _DEF_IOS
#include "String.hpp"
#include "Convert.hpp"
#include "Common.hpp"
#include "File.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Json.hpp"
#include "Regexp.hpp"
#include "Xml.hpp"
#else
#include <Plib-Text/String.hpp>
#include <Plib-Text/Convert.hpp>
#include <Plib-Text/Common.hpp>
#include <Plib-Text/File.hpp>
#include <Plib-Text/Config.hpp>
#include <Plib-Text/Logger.hpp>
#include <Plib-Text/Json.hpp>
#include <Plib-Text/Regexp.hpp>
#include <Plib-Text/Xml.hpp>
#endif

#endif // plib.text.text.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
