/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Threading.hpp
* Propose  			: Include All Threading Head Files together.
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-01-10
*/

#pragma once

#ifndef _PLIB_THREADING_THREADING_HPP_
#define _PLIB_THREADING_THREADING_HPP_

#if _DEF_IOS
#include "Locker.hpp"
#include "Semaphore.hpp"
#include "Thread.hpp"
#include "Stopwatch.hpp"
#include "Timer.hpp"
#else
#include <Plib-Threading/Locker.hpp>
#include <Plib-Threading/Semaphore.hpp>
#include <Plib-Threading/Thread.hpp>
#include <Plib-Threading/Stopwatch.hpp>
#include <Plib-Threading/Timer.hpp>
#endif

// For Server Running in Background:
#include <signal.h>
namespace Plib
{
	namespace Threading
	{
#if _DEF_WIN32
		INLINE void WaitForExitSignal( )
		{
			char _c = getc( );
		}
#else
		// Global Signal
		template < Uint32 _Dummy = 0 >
		struct __GlobalSignal {
			static Semaphore	__WaitFor;
		};
		
		template < Uint32 _Dummy >
		Semaphore	__GlobalSignal< _Dummy >::__WaitFor(0, 1);
		
		INLINE void __HandleSignal( int _Sig ) {
			if (SIGTERM == _Sig || SIGINT == _Sig || SIGQUIT == _Sig) 
				__GlobalSignal< 0 >::__WaitFor.Release();			
		}
		
		INLINE void WaitForExitSignal( )
		{
			// Hook the signal
			sigset_t sgset, osgset;
			sigfillset(&sgset);
			sigdelset(&sgset, SIGTERM);
			sigdelset(&sgset, SIGINT);
			sigdelset(&sgset, SIGQUIT);
			sigdelset(&sgset, 11);
			sigprocmask(SIG_SETMASK, &sgset, &osgset);
			signal(SIGTERM, __HandleSignal);
			signal(SIGINT, __HandleSignal);
			signal(SIGQUIT, __HandleSignal);
			
			// Wait for exit signal
			__GlobalSignal< 0 >::__WaitFor.Get( );		
		}
#endif
	}
}

#endif // plib.threading.threading.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
