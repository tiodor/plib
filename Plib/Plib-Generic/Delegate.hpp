/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Delegate.hpp
* Propose  			: Delegate Redefinition.
* 
* Current Version	: 1.2
* Change Log		: Redefinition, Use Plib::Generic::List.
					: Move to Generic Namespace.
* Author			: Push Chen
* Change Date		: 2011-04-26
*/

#pragma once

#ifndef _PLIB_GENERIC_DELEGATE_HPP_
#define _PLIB_GENERIC_DELEGATE_HPP_

#if _DEF_IOS
#include "StaticArray.hpp"
#else
#include <Plib-Generic/ArrayList.hpp>
#endif

namespace Plib
{
	namespace Generic
	{		
		#define _MAX_DELEGATE_ARRAY_	0x10
		/// Atom Definition.
		#define PARAM( n ) , typename _Ty##n
		#define PARAM_END typename _Ty 

		#define ARG( n ) , _Val##n
		#define ARG_END _Val 

		#define VAL( n ) , _Ty##n _Val##n
		#define VAL_END _Ty0 _Val
		#define VAL_SUBEND _Ty1 _Val

		#define TYPE( n ) , _Ty##n
		#define TYPE_END _Ty

		#define TYPEDEF( n ) ; typedef _Ty##n argument_type##n
		#define TYPEDEF_END typedef _Ty##0 argument_type

		/// Repeat Definition.
		#define DEF_PARAM( n )						REPEAT_##n( n, PARAM, PARAM_END )
		#define DEF_ARG( n )						REPEAT_##n( n, ARG, ARG_END ) 
		#define DEF_VAL( n )						REPEAT_##n( n, VAL, VAL_END )
		#define DEF_TYPE( n )						REPEAT_##n( n, TYPE, TYPE_END )
		#define DEF_ARGTYPE( n )					REPEAT_##n( n, TYPEDEF, TYPEDEF_END )


		template < typename T > struct DelegateInvokeBase { };
		template < typename T > struct DelegateInvokeProc { };
		template < typename _TyIns, typename T > struct DelegateInvokeObj { };
		template < typename _TyIns, typename T > struct DelegateInvokeFuncObj { };
		template < typename T > class Delegate { };


		// Basic Delegate Definition.
		#define _DELEGATE_BASIC_DEFINITION_COMMON_( n )									\
		template < typename _TyRet, DEF_PARAM( n ) >									\
		struct DelegateInvokeBase< _TyRet ( DEF_TYPE( n ) ) > {							\
			virtual _TyRet Invoke( DEF_VAL( n ) ) const = 0;							\
			virtual DelegateInvokeBase< _TyRet ( DEF_TYPE( n ) ) > * Copy( ) = 0;		\
			virtual ~DelegateInvokeBase< _TyRet ( DEF_TYPE( n ) ) > ( ) { }				\
		};																				\
		template < typename _TyRet, DEF_PARAM( n ) >									\
		struct DelegateInvokeProc< _TyRet ( DEF_TYPE( n ) ) >							\
			: public DelegateInvokeBase< _TyRet ( DEF_TYPE( n ) ) > {					\
			_TyRet (*mfp)( DEF_TYPE( n ) );												\
			DelegateInvokeProc( ) : mfp( NULL ) { }										\
			DelegateInvokeProc( _TyRet (*fp)( DEF_TYPE( n ) ) ) : mfp(fp) { }           \
			DelegateInvokeProc( const DelegateInvokeProc< _TyRet( DEF_TYPE( n ) ) > &	\
					rhs ) : mfp( rhs.mfp ) { }											\
			inline virtual _TyRet Invoke( DEF_VAL( n ) ) const {						\
				return (*mfp)( DEF_ARG( n ) ); }										\
			inline virtual DelegateInvokeBase< _TyRet ( DEF_TYPE( n ) ) > * Copy() {	\
				typedef DelegateInvokeProc< _TyRet( DEF_TYPE(n) ) > TDelg;				\
				PCNEWPARAM( TDelg, p, *this );											\
				return p;																\
			}																			\
			virtual ~DelegateInvokeProc< _TyRet ( DEF_TYPE( n ) ) >() {}				\
		};																				\
		template < typename _TyIns, typename _TyRet, DEF_PARAM( n ) >					\
		struct DelegateInvokeObj< _TyIns, _TyRet( DEF_TYPE( n ) ) >						\
			: public DelegateInvokeBase< _TyRet ( DEF_TYPE( n ) ) > {					\
			_TyRet (_TyIns::*mfp)( DEF_TYPE( n ) );										\
			_TyIns * pIns;																\
			DelegateInvokeObj( ) : mfp( NULL ), pIns( NULL ) { }						\
			DelegateInvokeObj( _TyIns * pObj, _TyRet (_TyIns::*fp)( DEF_TYPE( n ) ) )	\
				: mfp(fp), pIns(pObj) { }												\
			DelegateInvokeObj( const DelegateInvokeObj< _TyIns, 						\
					_TyRet( DEF_TYPE( n ) ) > & rhs )									\
				: mfp( rhs.mfp ), pIns( rhs.pIns ) { }									\
			inline virtual _TyRet Invoke( DEF_VAL( n ) ) const {						\
				return (pIns->*mfp)( DEF_ARG( n ) ); }									\
			inline virtual DelegateInvokeBase< _TyRet ( DEF_TYPE( n ) ) > * Copy() {	\
				typedef DelegateInvokeObj< _TyIns, _TyRet( DEF_TYPE( n ) ) > TDelg;		\
				PCNEWPARAM( TDelg, p, *this );											\
				return p;																\
			}																			\
			virtual ~DelegateInvokeObj< _TyIns, _TyRet( DEF_TYPE( n ) ) >() {}			\
		};																				\
		template < typename _TyIns, typename _TyRet, DEF_PARAM( n ) >					\
		struct DelegateInvokeFuncObj< _TyIns, _TyRet( DEF_TYPE( n ) ) >					\
			: public DelegateInvokeBase< _TyRet( DEF_TYPE( n ) ) > {					\
			_TyIns pIns;																\
			DelegateInvokeFuncObj( ) { }												\
			DelegateInvokeFuncObj( _TyIns & rObj ) : pIns(rObj) { }						\
			DelegateInvokeFuncObj( const DelegateInvokeFuncObj< 						\
					_TyIns, _TyRet( DEF_TYPE( n ) ) > & rhs ) : pIns( rhs.pIns ) { }	\
			inline virtual _TyRet Invoke( DEF_VAL( n ) ) const {						\
				return (_TyRet)(pIns)( DEF_ARG( n ) ); }								\
			inline virtual DelegateInvokeBase< _TyRet ( DEF_TYPE( n ) ) > * Copy() {	\
				typedef DelegateInvokeFuncObj< _TyIns, _TyRet( DEF_TYPE( n ) ) > TDelg;	\
				PCNEWPARAM( TDelg, p, *this );											\
				return p;																\
			}																			\
			virtual ~DelegateInvokeFuncObj< _TyIns, _TyRet( DEF_TYPE( n ) ) >() {}		\
		};																				\
		template < typename _TyRet, DEF_PARAM( n ) >									\
		class Delegate< _TyRet ( DEF_TYPE( n ) ) > {									\
		protected:																		\
			RArray< DelegateInvokeBase< _TyRet( DEF_TYPE( n ) ) > * > m_FooList;		\
		public:																			\
			DEF_ARGTYPE( n );															\
			typedef argument_type0 argument_type;										\
			typedef argument_type0 first_argument_type;									\
			typedef CHAR_CONNECT(argument_type, n) second_argument_type;				\
			typedef _TyRet result_type;													\
			typedef _TyRet (*function_type)(DEF_TYPE( n ));								\
			Delegate< _TyRet( DEF_TYPE( n ) ) >(										\
					const Delegate<_TyRet( DEF_TYPE( n ) )> & rhs) 						\
			{																			\
				CONSTRUCTURE;															\
				(*this) = rhs;															\
			}																			\
			Delegate<_TyRet( DEF_TYPE( n ) )> & operator = (							\
				const Delegate<_TyRet( DEF_TYPE( n ) )> & rhs ) {						\
				if ( this == &rhs ) return *this;										\
				for ( unsigned int i = 0; i < m_FooList.Size(); ++i )					\
				{																		\
					PDELETE( m_FooList[i] );											\
				}																		\
				this->m_FooList.Clear();												\
				for ( unsigned int i = 0; i < rhs.m_FooList.Size(); ++i )				\
					m_FooList.PushBack(rhs.m_FooList[i]->Copy());						\
				return (*this);															\
			}																			\
		public:																			\
			void Add ( _TyRet (*fp)( DEF_TYPE( n ) ) ) {								\
				if ( fp == NULL ) return;												\
				typedef DelegateInvokeProc< _TyRet( DEF_TYPE( n ) ) > TDelg;			\
				PCNEW( TDelg, pDelg );													\
				pDelg->mfp = fp;														\
				m_FooList.PushBack( pDelg );											\
			}																			\
			template <class _TyIns> void Add ( _TyIns * pObj,							\
				_TyRet (_TyIns::*fp)( DEF_TYPE( n ) ) ) {								\
				if ( pObj == NULL || fp == NULL ) return;								\
				typedef DelegateInvokeObj< _TyIns, _TyRet( DEF_TYPE(n) ) > TDelg;		\
				PCNEW( TDelg, pDelg );													\
				pDelg->mfp = fp;														\
				pDelg->pIns = pObj;														\
				m_FooList.PushBack( pDelg );											\
			}																			\
			template <class _TyIns> void Add ( _TyIns & rObj ) {						\
				typedef DelegateInvokeFuncObj< _TyIns, _TyRet( DEF_TYPE(n) ) > TDelg;	\
				PCNEW(TDelg, pDelg);													\
				pDelg->pIns = rObj;														\
				m_FooList.PushBack( pDelg );											\
			}																			\
			operator bool ( ) const {													\
				return this->m_FooList.Size() != 0;										\
			}																			\
			_TyRet operator () ( DEF_VAL( n ) ) {										\
				for ( Uint32 i = 0; i < m_FooList.Size( ) - 1;							\
					m_FooList[i++]->Invoke( DEF_ARG( n ) ) );							\
				return (_TyRet)(m_FooList[m_FooList.Size() - 1]->Invoke(DEF_ARG( n )));	\
			}																			\
			_TyRet operator () ( DEF_VAL( n ) ) const {									\
				for ( unsigned int i = 0; i < m_FooList.Size() - 1;						\
					m_FooList[i++]->Invoke( DEF_ARG( n ) ) );							\
				return (_TyRet)m_FooList[m_FooList.Size() - 1]->Invoke( DEF_ARG( n ) );	\
			}																			\
			Delegate< _TyRet( DEF_TYPE( n ) ) > & operator += (							\
				_TyRet (*fp)( DEF_TYPE( n ) ) ) {										\
				this->Add(fp);															\
				return (*this);															\
			}																			\
			template < class _TyIns > Delegate< _TyRet ( DEF_TYPE( n ) ) > &			\
			operator += ( std::pair< _TyIns *,											\
				_TyRet (_TyIns::*)( DEF_TYPE( n ) ) > pairFp) {							\
				this->Add(pairFp.first, pairFp.second);									\
				return (*this);															\
			}																			\
			template < class _TyIns > Delegate< _TyRet ( DEF_TYPE( n ) ) > &			\
				operator += ( _TyIns & rObj ) {											\
				this->Add( rObj );														\
				return (*this);															\
			}																			\
			Delegate<_TyRet( DEF_TYPE( n ) )> () {CONSTRUCTURE;}						\
			Delegate<_TyRet( DEF_TYPE( n ) )> ( _TyRet (*fp)( DEF_TYPE( n ) ) ) { 		\
				CONSTRUCTURE; this->Add( fp ); }										\
			template < class _TyIns > 													\
			Delegate<_TyRet( DEF_TYPE( n ) )>(_TyIns * pObj,							\
				_TyRet (_TyIns::*fp)( DEF_TYPE( n ) ) ) {								\
				CONSTRUCTURE;															\
				this->Add(pObj, fp); 													\
			}																			\
			template < class _TyIns > 													\
			Delegate< _TyRet( DEF_TYPE( n ) ) >(_TyIns & rObj) { 						\
				CONSTRUCTURE; this->Add(rObj); }										\
			~Delegate< _TyRet( DEF_TYPE( n ) ) >  () {									\
				DESTRUCTURE;															\
				for ( unsigned int i = 0; i < m_FooList.Size(); ++i ) {					\
					PDELETE( m_FooList[i] );											\
				}																		\
				m_FooList.Clear();														\
			}																			\
			unsigned int Count() const { return m_FooList.Size(); }						\
			void Clear() { 																\
				for ( unsigned int i = 0; i < m_FooList.Size(); ++i ) {					\
					PDELETE( m_FooList[i] );											\
				}																		\
				m_FooList.Clear(); 														\
			}																			\
		};

		// Specifial Definition of Non-Parameter Version
		template < typename _TyRet >
		struct DelegateInvokeBase< _TyRet() > {
			virtual _TyRet Invoke () const = 0;
			virtual DelegateInvokeBase< _TyRet() > * Copy() = 0;
			virtual ~DelegateInvokeBase< _TyRet() >(){}
		};
		template < typename _TyRet >
		struct DelegateInvokeProc< _TyRet () > 
			: public DelegateInvokeBase< _TyRet() > {
			_TyRet (*mfp)();
			DelegateInvokeProc( ) : mfp( NULL ) { }
			DelegateInvokeProc( _TyRet (*fp)() ) : mfp(fp) { }
			DelegateInvokeProc( const DelegateInvokeProc< _TyRet( ) > & rhs )
				: mfp( rhs.mfp ) { }
			inline virtual _TyRet Invoke() const { return (*mfp)(); }
			inline virtual DelegateInvokeBase< _TyRet() > * Copy() { 
				PCNEWPARAM( DelegateInvokeProc< _TyRet( ) >, p, *this );
				return p;
			}
			virtual ~DelegateInvokeProc< _TyRet() >() { }
		};
		template < typename _TyIns, typename _TyRet >
		struct DelegateInvokeObj<_TyIns, _TyRet()> 
			: public DelegateInvokeBase< _TyRet() > {
			_TyRet (_TyIns::*mfp)();
			_TyIns * pIns;
			DelegateInvokeObj( ) : mfp( NULL ), pIns( NULL ) { }
			DelegateInvokeObj(_TyIns * pObj, _TyRet (_TyIns::*fp)()) : mfp(fp), pIns(pObj){ }
			DelegateInvokeObj( const DelegateInvokeObj< _TyIns, _TyRet( ) > & rhs )
				: mfp( rhs.mfp ), pIns( rhs.pIns ) { }
			inline virtual _TyRet Invoke() const { return (pIns->*mfp)(); }
			inline virtual DelegateInvokeBase< _TyRet() > * Copy() {
				typedef DelegateInvokeObj< _TyIns, _TyRet( ) > TDelg;
				PCNEWPARAM( TDelg, p, *this );
				return p;
			}
			virtual ~DelegateInvokeObj<_TyIns, _TyRet()>(){ }
		};
		template < typename _TyIns, typename _TyRet >
		struct DelegateInvokeFuncObj< _TyIns, _TyRet() > 
			: public DelegateInvokeBase< _TyRet() > {
			_TyIns pIns;
			DelegateInvokeFuncObj( ) { }
			DelegateInvokeFuncObj( _TyIns & rObj ) : pIns(rObj) { } 
			DelegateInvokeFuncObj( const DelegateInvokeFuncObj< _TyIns, _TyRet( ) > & rhs )
				: pIns( rhs.pIns ) { }
			inline virtual _TyRet Invoke( ) const { return (_TyRet)(pIns)( ); }
			inline virtual DelegateInvokeBase< _TyRet() > * Copy() {
				typedef DelegateInvokeFuncObj< _TyIns, _TyRet( ) > TDelg;
				PCNEWPARAM( TDelg, p, *this );
				return p;
			}
			virtual ~DelegateInvokeFuncObj< _TyIns, _TyRet() >() {}
		};
		template < typename _TyRet > class Delegate<_TyRet()> {
		protected:
			RArray< DelegateInvokeBase< _TyRet( ) > * > m_FooList;
		public:
			typedef void argument_type;
			typedef void first_argument_type;
			typedef void second_argument_type;
			typedef void result_type;
			typedef _TyRet (*function_type)();

			// Copy'str
			Delegate< _TyRet( ) >(const Delegate<_TyRet()> & rhs) 
			{
				CONSTRUCTURE;
				(*this) = rhs;
			}

			Delegate< _TyRet() > & operator = (const Delegate<_TyRet()> & rhs) 
			{
				if ( this == &rhs ) return *this;
				for ( unsigned int i = 0; i < m_FooList.Size(); ++i ) {
					PDELETE( m_FooList[i] );
				}
				this->m_FooList.Clear();
				for ( unsigned int i = 0; i < rhs.m_FooList.Size(); ++i )
					m_FooList.PushBack(rhs.m_FooList[i]->Copy());
				return (*this);
			}
		public:
			void Add(_TyRet (*fp)()) {
				if ( fp == NULL ) return;
				PCNEW( DelegateInvokeProc< _TyRet( ) >, pDelg );
				pDelg->mfp = fp;
				this->m_FooList.PushBack( pDelg );
			}
			template < class _TyIns >
			void Add(_TyIns * pObj, _TyRet (_TyIns::*fp)()) {
				if ( pObj == NULL || fp == NULL ) return;
				typedef DelegateInvokeObj< _TyIns, _TyRet( ) > TDelg;
				PCNEW( TDelg, pDelg );
				pDelg->pIns = pObj;
				pDelg->mfp = fp;
				this->m_FooList.PushBack( pDelg );
			}
			template < class _TyIns >
			void Add( _TyIns & rObj ) { 
				typedef DelegateInvokeObj< _TyIns, _TyRet( ) > TDelg;
				PCNEW( TDelg, pDelg );
				pDelg->pIns = rObj;
				this->m_FooList.PushBack( pDelg );
			}
			Delegate<_TyRet( )>() { CONSTRUCTURE; }
			Delegate< _TyRet () >(_TyRet (*fp)()) { CONSTRUCTURE; this->Add(fp); }
			template < class _TyIns > Delegate< _TyRet( ) >(_TyIns * pObj, _TyRet (_TyIns::*fp)()) {
				CONSTRUCTURE;
				this->Add(pObj, fp);
			}
			template < class _TyIns > Delegate< _TyRet( ) >( _TyIns & rObj ) {
				CONSTRUCTURE;
				this->Add( rObj );
			}
			operator bool ( ) const {
				return this->m_FooList.Size() != 0;
			}
			~Delegate< _TyRet( ) >() { 
				DESTRUCTURE;
				for (unsigned int i = 0; i < m_FooList.Size(); ++i ) {
					PDELETE( m_FooList[i] );
				}
				m_FooList.Clear();
			}
			_TyRet operator ()() {
				//_TyRet rtn;
				for ( unsigned int i = 0; i < m_FooList.Size() - 1; 
					m_FooList[i++]->Invoke() );
				return m_FooList[m_FooList.Size() - 1]->Invoke();
			}
			_TyRet operator ()() const {
				for ( Uint32 i = 0; i < m_FooList.Size() - 1;
					m_FooList[i++]->Invoke() );
				return m_FooList[m_FooList.Size() - 1]->Invoke();
			}
			Delegate< _TyRet() > & operator += (_TyRet (*fp)()) {
				this->Add(fp);
				return (*this);
			}
			template < class _TyIns > Delegate<_TyRet()> & operator += (
				std::pair< _TyIns *, _TyRet (_TyIns::*)() > pairFp) {
					this->Add(pairFp.first, pairFp.second);
					return (*this);
			}
			template < class _TyIns > Delegate<_TyRet()> & operator += ( _TyIns & rObj ){
				this->Add(rObj);
				return (*this);
			}
			unsigned int Count() const { return m_FooList.Size(); }
			void Clear() { 
				for ( unsigned int i = 0; i < m_FooList.Size(); ++i ) {
					PDELETE( m_FooList[i] );
				}
				m_FooList.Clear(); 
			}
		};

		_DELEGATE_BASIC_DEFINITION_COMMON_(0)
		_DELEGATE_BASIC_DEFINITION_COMMON_(1)
		_DELEGATE_BASIC_DEFINITION_COMMON_(2)
		_DELEGATE_BASIC_DEFINITION_COMMON_(3)
		_DELEGATE_BASIC_DEFINITION_COMMON_(4)
		_DELEGATE_BASIC_DEFINITION_COMMON_(5)
		_DELEGATE_BASIC_DEFINITION_COMMON_(6)
		_DELEGATE_BASIC_DEFINITION_COMMON_(7)
		_DELEGATE_BASIC_DEFINITION_COMMON_(8)
		_DELEGATE_BASIC_DEFINITION_COMMON_(9)
		
	}
}

#endif // plib.generic.Delegate.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

