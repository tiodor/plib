/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Pool.hpp
* Propose  			: Object Pool.
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-04-26
*/


#pragma once

#ifndef _PLIB_GENERIC_POOL_HPP_
#define _PLIB_GENERIC_POOL_HPP_

#if _DEF_IOS
#include "Stack.hpp"
#include "Pair.hpp"
#include "Reference.hpp"
#else
#include <Plib-Generic/Stack.hpp>
#include <Plib-Generic/Pair.hpp>
#include <Plib-Generic/Reference.hpp>
#endif

namespace Plib
{
	namespace Generic
	{
		// Object Pool, make a collection of all objece in a stack.
		// If the stack is empty, then create new object.
		template < 
			typename _TyObject, 
			typename _TyAlloc = Plib::Basic::Allocator< _TyObject > >
		class Pool_
		{
		public:
			typedef _TyObject *			PointT;
		protected:
			Stack< _TyObject * > 		_ObjStackPool;
			Uint32 						_AllCount;
			
			static _TyAlloc				gObjectAlloc;
			PLIB_THREAD_SAFE_DEFINE;
			
		protected:
			// No copy
			Pool_< _TyObject, _TyAlloc >( 
				const Pool_< _TyObject, _TyAlloc > & rhs );
			Pool_< _TyObject, _TyAlloc > & operator = ( 
				const Pool_< _TyObject, _TyAlloc > & rhs );
		public:
			
			Pool_< _TyObject, _TyAlloc >( )
				: _AllCount( 0 ) { CONSTRUCTURE; }
			~Pool_< _TyObject, _TyAlloc >( )
			{
				DESTRUCTURE;
				while ( !_ObjStackPool.Empty( ) ) {
					_TyObject * _pObj = _ObjStackPool.Top( );
					_ObjStackPool.Pop( );
					gObjectAlloc.Destroy( _pObj );
				}
			}
			
			// Get one object from the pool
			// If the pool is empty, create a new object.
			INLINE PointT Get( ) {
				PLIB_THREAD_SAFE;
				if ( _ObjStackPool.Empty() ) {
					_TyObject * _pObj = gObjectAlloc.Create( );
					SELF_INCREASE( _AllCount );
					return _pObj;
				}
				else {
					_TyObject * _pObj = _ObjStackPool.Top( );
					_ObjStackPool.Pop( );
					return _pObj;
				}
			}
			
			// Return the object into the pool
			// The internal stack object is thread-safe, 
			// so we do not need to define PLIB_THREAD_SAFE again.
			INLINE void Return( PointT _Item ) {
				PLIB_THREAD_SAFE;
				if ( (_AllCount / 2) < _ObjStackPool.Size( ) ) {
					gObjectAlloc.Destroy( _Item );
					SELF_DECREASE( _AllCount );
				} else {
					_ObjStackPool.Push( _Item );
				}
			}
			
			// Return the number of object-in-the-pool right now.
			INLINE Uint32 LeftCount( ) const {
				return _ObjStackPool.Size();
			}
			
			// Return the count of objects are in using.
			INLINE Uint32 UsingCount( ) const {
				return _AllCount - _ObjStackPool.Size();
			}
			
			// Return all count of objects have been created.
			INLINE Uint32 AllCount( ) const {
				return _AllCount;
			}
			
			// The same to AllCount()
			INLINE Uint32 Size( ) const {
				return _AllCount;
			}
		};
		
		// Static Allocator.
		template < typename _TyObject, typename _TyAlloc >
		_TyAlloc	Pool< _TyObject, _TyAlloc >::gObjectAlloc;
		
		
		// Sepcifial Version for String.
		typedef Pair< char *, Uint32 >		BasicCharBufferT;
		typedef Pair< wchar_t *, Uint32 >	WideCharBufferT;
		
		// For Basic Char Type
		template < typename _TyAlloc >
		class Pool_< BasicCharBufferT, _TyAlloc >
		{
			typedef BasicCharBufferT *			PointT;
		protected:
			Stack< BasicCharBufferT * > 		_ObjStackPool;
			Uint32 								_AllCount;
			
			static _TyAlloc						gObjectAlloc;
			PLIB_THREAD_SAFE_DEFINE;
			
		protected:
			// No copy
			Pool_< BasicCharBufferT, _TyAlloc >( 
				const Pool_< BasicCharBufferT, _TyAlloc > & rhs );
			Pool_< BasicCharBufferT, _TyAlloc > & operator = ( 
				const Pool_< BasicCharBufferT, _TyAlloc > & rhs );
		public:
			
			Pool_< BasicCharBufferT, _TyAlloc >( )
				: _AllCount( 0 ) { CONSTRUCTURE; }
			~Pool_< BasicCharBufferT, _TyAlloc >( )
			{
				DESTRUCTURE;
				while ( !_ObjStackPool.Empty( ) ) {
					BasicCharBufferT * _pObj = _ObjStackPool.Top( );
					_ObjStackPool.Pop( );
					if ( _pObj->First != NULL ) {
						PFREE( _pObj->First );
					}
					gObjectAlloc.Destroy( _pObj );
				}
			}
			// Get one object from the pool
			// If the pool is empty, create a new object.
			INLINE PointT Get( ) {
				PLIB_THREAD_SAFE;
				if ( _ObjStackPool.Empty() ) {
					BasicCharBufferT * _pObj = gObjectAlloc.Create( );
					SELF_INCREASE( _AllCount );
					_pObj->First = NULL;
					_pObj->Second = 0;
					return _pObj;
				}
				else {
					BasicCharBufferT * _pObj = _ObjStackPool.Top( );
					_ObjStackPool.Pop( );
					return _pObj;
				}
			}
			
			// Return the object into the pool
			// The internal stack object is thread-safe, 
			// so we do not need to define PLIB_THREAD_SAFE again.
			INLINE void Return( PointT _Item ) {
				PLIB_THREAD_SAFE;
				if ( (_AllCount / 2) < _ObjStackPool.Size( ) ) {
					if ( _Item->First != NULL ) {
						PFREE(_Item->First);
					}
					_Item.Second = 0;
					gObjectAlloc.Destroy( _Item );
					SELF_DECREASE( _AllCount );
				} else {
					_ObjStackPool.Push( _Item );
				}
			}
			
			// Return the number of object-in-the-pool right now.
			INLINE Uint32 LeftCount( ) const {
				return _ObjStackPool.Size();
			}
			
			// Return the count of objects are in using.
			INLINE Uint32 UsingCount( ) const {
				return _AllCount - _ObjStackPool.Size();
			}
			
			// Return all count of objects have been created.
			INLINE Uint32 AllCount( ) const {
				return _AllCount;
			}
			
			// The same to AllCount()
			INLINE Uint32 Size( ) const {
				return _AllCount;
			}			
		};
		// Static Allocator.
		template < typename _TyAlloc >
		_TyAlloc	Pool_< BasicCharBufferT, _TyAlloc >::gObjectAlloc;
		
		// For Wide Char Type
		template < typename _TyAlloc >
		class Pool_< WideCharBufferT, _TyAlloc >
		{
		public:
			typedef WideCharBufferT *			PointT;
		protected:
			Stack< WideCharBufferT * > 			_ObjStackPool;
			Uint32 								_AllCount;
			
			static _TyAlloc						gObjectAlloc;
			PLIB_THREAD_SAFE_DEFINE;
			
		protected:
			// No copy
			Pool_< WideCharBufferT, _TyAlloc >( 
				const Pool_< WideCharBufferT, _TyAlloc > & rhs );
			Pool_< WideCharBufferT, _TyAlloc > & operator = ( 
				const Pool_< WideCharBufferT, _TyAlloc > & rhs );
		public:
			
			Pool_< WideCharBufferT, _TyAlloc >( )
				: _AllCount( 0 ) { CONSTRUCTURE; }
			~Pool_< WideCharBufferT, _TyAlloc >( )
			{
				DESTRUCTURE;
				while ( !_ObjStackPool.Empty( ) ) {
					WideCharBufferT * _pObj = _ObjStackPool.Top( );
					_ObjStackPool.Pop( );
					if ( _pObj->First != NULL ){
						PFREE( _pObj->First );
					}
					gObjectAlloc.Destroy( _pObj );
				}
			}
			// Get one object from the pool
			// If the pool is empty, create a new object.
			INLINE PointT Get( ) {
				PLIB_THREAD_SAFE;
				if ( _ObjStackPool.Empty() ) {
					WideCharBufferT * _pObj = gObjectAlloc.Create( );
					SELF_INCREASE( _AllCount );
					_pObj->First = NULL;
					_pObj->Second = 0;
					return _pObj;
				}
				else {
					WideCharBufferT * _pObj = _ObjStackPool.Top( );
					_ObjStackPool.Pop( );
					return _pObj;
				}
			}
			
			// Return the object into the pool
			// The internal stack object is thread-safe, 
			// so we do not need to define PLIB_THREAD_SAFE again.
			INLINE void Return( PointT _Item ) {
				PLIB_THREAD_SAFE;
				if ( (_AllCount / 2) < _ObjStackPool.Size( ) ) {
					if ( _Item->First != NULL ) {
						PFREE( _Item->First );
					}
					_Item.Second = 0;
					gObjectAlloc.Destroy( _Item );
					SELF_DECREASE( _AllCount );
				} else {
					_ObjStackPool.Push( _Item );
				}
			}
			
			// Return the number of object-in-the-pool right now.
			INLINE Uint32 LeftCount( ) const {
				return _ObjStackPool.Size();
			}
			
			// Return the count of objects are in using.
			INLINE Uint32 UsingCount( ) const {
				return _AllCount - _ObjStackPool.Size();
			}
			
			// Return all count of objects have been created.
			INLINE Uint32 AllCount( ) const {
				return _AllCount;
			}
			
			// The same to AllCount()
			INLINE Uint32 Size( ) const {
				return _AllCount;
			}			
		};
		// Static Allocator.
		template < typename _TyAlloc >
		_TyAlloc	Pool_< WideCharBufferT, _TyAlloc >::gObjectAlloc;
		
		
		// Reference Version of Pool
		template < 
			typename _TyObject, 
			typename _TyAlloc = Plib::Basic::Allocator< _TyObject > 
		>
		class Pool 
			: public Plib::Generic::Reference< Pool_< _TyObject, _TyAlloc > >
		{
			typedef Plib::Generic::Reference< Pool_< _TyObject, _TyAlloc > >	TFather;
			Pool<_TyObject, _TyAlloc>( bool _beNull ) : TFather( false ) 
			{ CONSTRUCTURE; }
		public:
			typedef _TyObject *		PointT;
		public:
			Pool<_TyObject, _TyAlloc>( ) : TFather( true ) { CONSTRUCTURE; }
			Pool<_TyObject, _TyAlloc>( const Pool<_TyObject, _TyAlloc> & rhs)
				: TFather( rhs ) { CONSTRUCTURE; }
			~Pool< _TyObject, _TyAlloc > ( ) { DESTRUCTURE; }
			
			// Get one object from the pool
			// If the pool is empty, create a new object.
			INLINE PointT Get( ) { 
				return TFather::_Handle->_PHandle->Get( ); }
			
			// Return the object into the pool
			// The internal stack object is thread-safe, 
			// so we do not need to define PLIB_THREAD_SAFE again.
			INLINE void Return( PointT _Item ) { 
				TFather::_Handle->_PHandle->Return( _Item ); }
			
			// Return the number of object-in-the-pool right now.
			INLINE Uint32 LeftCount( ) const { 
				return TFather::_Handle->_PHandle->LeftCount(); }
			
			// Return the count of objects are in using.
			INLINE Uint32 UsingCount( ) const { 
				return TFather::_Handle->_PHandle->UsingCount( ); }
			
			// Return all count of objects have been created.
			INLINE Uint32 AllCount( ) const { 
				return TFather::_Handle->_PHandle->AllCount( ); }
			
			// The same to AllCount()
			INLINE Uint32 Size( ) const { 
				return TFather::_Handle->_PHandle->Size( ); }
			
			// Null Object.
			const static Pool<_TyObject, _TyAlloc> Null;
			Pool<_TyObject, _TyAlloc> CreateNullPool( ) {
				return Pool<_TyObject, _TyAlloc>( false );
			}
		};
		
		// Null Pool
		template < typename _TyObject, typename _TyAlloc >
			const Pool<_TyObject, _TyAlloc> 
				Pool<_TyObject, _TyAlloc>::Null( false );
	}
}

#endif // plib.generic.pool.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

