/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: Config.hpp
* Propose  			: Rewrite the old ioconfig, use new RString and ArrayList.
* 
* Current Version	: 1.1
* Change Log		: Re-Write..
* Author			: Push Chen
* Change Date		: 2011-07-02
*/

#pragma once

#ifndef _PLIB_TEXT_CONFIG_HPP_
#define _PLIB_TEXT_CONFIG_HPP_

#if _DEF_IOS
#include "String.hpp"
#include "File.hpp"
#else
#include <Plib-Text/String.hpp>
#include <Plib-Text/File.hpp>
#endif

#include <map>

namespace Plib
{
	namespace Text
	{
		typedef enum {
			CFG_COMMENT,		// Comment start chars
			CFG_KVSPILT,		// Key-value split chars
			CFG_MVWRAP,			// Multiple value wrap chars.
			CFG_MVSPILT			// Multiple value split chars.
		} CFG_PARAM;
		
		/*
		 * Configure file parser class.
		 * One can define the comment type, key-value split character,
		 * multiple value wrap character, multiple value split character.
		 */
		class Config_
		{			
		protected:
			RString													m_Params[4];
			std::map< RString, RString > 							m_KeyValuePair;
			std::map< RString, Plib::Generic::RArray< RString > > 	m_KeyArrayValue;

			RString 												_LastKey;
			Int32 													_bArrayContinue;
			bool 													_bUnCompleteLine;
			
		protected:
			bool __ParseIncomingString( RString _lineString ) {
				// For each line
				// is comment
				_lineString.Trim( );		// Trim to be processed.
				if ( _lineString.size() == 0 ) return true;	// empty line.
				
				if ( m_Params[CFG_COMMENT].Find( _lineString[0] ) != RString::NoPos )
					return true;	// this line is a comment line.
				if ( _bArrayContinue > 0 ) { // Check the array data.
					if ( _lineString.Find(m_Params[CFG_MVWRAP][0]) != RString::NoPos )
						SELF_INCREASE( _bArrayContinue );
					if ( _lineString[_lineString.size() - 1] == m_Params[CFG_MVWRAP][1] ) {
						SELF_DECREASE( _bArrayContinue );
						if ( _bArrayContinue == 0 ) {
							_lineString.Remove( _lineString.Size() - 1 );
						}
					}
					m_KeyValuePair[ _LastKey ] += _lineString;
					return true;
				}
				// Get the Key.
				Uint32 _pos = RString::NoPos;
				for ( Uint32 i = 0; i < m_Params[CFG_KVSPILT].size(); ++i ) {
					Uint32 _kvPos = _lineString.Find( m_Params[CFG_KVSPILT][i] );
					_pos = ( _kvPos < _pos ) ? _kvPos : _pos;
				}
				RString _Key = _lineString.SubString( 0, _pos );
				if ( _Key.Trim().Size() == 0 ) {
					// No Key
					if ( _bUnCompleteLine == false ) // Error Here
					{
						return false;
					}
				} else {
					if ( _bUnCompleteLine == true ) // Error Here, Last line is uncomplete
					{
						return false;
					}
					_LastKey.DeepCopy( _Key.Trim() );	// backup the last key.
				}
				if ( _pos == RString::NoPos ) {
					// No Value in this line.
					_bUnCompleteLine = true;
					return true;
				}
				RString _Value = _lineString.SubString( _pos + 1 );
				_Value.Trim();
				if ( _Value[0] == m_Params[CFG_MVWRAP][0] ) {
					_Value.Remove( 0 );
					SELF_INCREASE(_bArrayContinue);
					if ( _Value.Size() == 0 ) return true;
					if ( _Value[_Value.size() - 1] == m_Params[CFG_MVWRAP][1] ) {
						_Value.Remove( _Value.size() - 1 );
						SELF_DECREASE(_bArrayContinue);
					}
				}
				m_KeyValuePair[ _LastKey ] = _Value;
				return true;
			}
			
		public:
			// Default C'str
			Config_( 
				RString _comment = "#", 
				RString _kvsplit = "=", 
				RString _mvwrap = "{}", 
				RString _mvsplit = ";," )
				: _bArrayContinue( 0 ), _bUnCompleteLine( false )
			{
				CONSTRUCTURE;
				m_Params[CFG_COMMENT].DeepCopy(_comment);
				m_Params[CFG_KVSPILT].DeepCopy(_kvsplit);
				m_Params[CFG_MVWRAP].DeepCopy(_mvwrap);
				m_Params[CFG_MVSPILT].DeepCopy(_mvsplit);
			}
			~Config_( ) { DESTRUCTURE; }
			
			// Set specified characters.
			void Set( CFG_PARAM _param, RString _data )
			{
				m_Params[_param].DeepCopy( _data );
			}
			
			// Clear last parse result.
			void Clear( )
			{
				m_KeyValuePair.clear();
				m_KeyArrayValue.clear();
				_bArrayContinue = 0;
				_bUnCompleteLine = false;
			}
			
			// Parse the input array to be a config object.
			bool Parse( const Plib::Generic::RArray< RString > & _LineArray )
			{
				Clear(); 
				for ( Uint32 i = 0; i < _LineArray.Size(); ++i )
				{
					if ( !__ParseIncomingString( _LineArray[i] ) )
						return false;
				}
				return true;
			}
			
			// Parse the incoming string.
			bool Parse( const RString & _stringConfig )
			{
				Plib::Generic::RArray< RString > _LineArray = _stringConfig.Split("\r\n" + m_Params[CFG_MVSPILT]);
				return Parse( _LineArray );
			}
			
			// Open the file and parse it.
			bool ParseConfigFile( const RString & _filePath )
			{
				Clear();

				// Load the config file
				ReadStream rsConfig( _filePath );
				for ( ; ; ) {
					RString _line = rsConfig.ReadLine( );
					if ( __ParseIncomingString( _line ) == false )
						return false;
				}
				
				// Check if the configure file is well formated.
				if ( _bArrayContinue == true ) return false;
				return true;
			}
			
			// Random access the data.			
			RString operator [] ( const char * _key ) {
				return m_KeyValuePair[ RString( _key ) ];
			}
			// RString version.
			RString operator [] ( const RString & _key ) {
				return m_KeyValuePair[_key];
			}
			// Get specified value
			RString Get( const char * _key ) {
				return m_KeyValuePair[ RString( _key ) ];
			}
			// RString Version.
			RString Get( const RString & _key ) {
				return m_KeyValuePair[_key];
			}
			// Get an array
			Plib::Generic::RArray<RString> GetArray( const char * _key ) {
				return GetArray( RString( _key ) );
			}
			// RString Version.
			Plib::Generic::RArray<RString> GetArray( const RString & _key )
			{
				if ( m_KeyArrayValue.find( _key ) != m_KeyArrayValue.end() )
					return m_KeyArrayValue[_key];
				RString _srcValue = this->Get( _key );
				Plib::Generic::RArray< RString > _rArray = _srcValue.Split( m_Params[CFG_MVSPILT] );
				m_KeyArrayValue[_key] = _rArray;
				return _rArray;
			}
		};
		
		// Referernce Version of Config_
		class Config : public Plib::Generic::Reference< Config_ >
		{
			typedef Plib::Generic::Reference< Config_ >		TFather;
		protected:
			Config( bool _beNull ) : TFather( false ) { CONSTRUCTURE; }
		public:
			// C'Str
			Config( ) : TFather( true ) { CONSTRUCTURE; }
			Config( const Config & rhs ) : TFather( rhs ) { CONSTRUCTURE; }
			~Config( ) { DESTRUCTURE; }
			// Open a configure file.
			Config( const RString & _filePath ) : TFather( true ) {
				CONSTRUCTURE;
				TFather::_Handle->_PHandle->ParseConfigFile( _filePath );
			}
			// Clear last parse result.
			INLINE void Clear( ) { TFather::_Handle->_PHandle->Clear( ); }
			// Set specified characters.
			INLINE void Set( CFG_PARAM _param, RString _data ) {
				TFather::_Handle->_PHandle->Set( _param, _data );
			}
			// Parse the input array to be a config object.
			INLINE bool Parse( const Plib::Generic::RArray< RString > & _LineArray ) {
				return TFather::_Handle->_PHandle->Parse( _LineArray );
			}
			// Parse the incoming string.
			INLINE bool Parse( const RString & _stringConfig ) {
				return TFather::_Handle->_PHandle->Parse( _stringConfig );
			}
			// Open the file and parse it.
			INLINE bool ParseConfigFile( const RString & _filePath ) {
				return TFather::_Handle->_PHandle->ParseConfigFile( _filePath );
			}
			// Random access the data.			
			RString operator [] ( const char * _key ) {
				return TFather::_Handle->_PHandle->operator [] ( _key );
			}
			RString operator [] ( const RString & _key ) {
				return TFather::_Handle->_PHandle->operator [] ( _key );
			}
			// Get array
			Plib::Generic::RArray<RString> GetArray( const char * _key ) {
				return TFather::_Handle->_PHandle->GetArray( _key );
			}
			// RString Version.
			Plib::Generic::RArray<RString> GetArray( const RString & _key ) {
				return TFather::_Handle->_PHandle->GetArray( _key );
			}
			
			// Get another config
			Config GetConfig( const char * _key ) {
				Config _2ndConfig;
				_2ndConfig.Parse( TFather::_Handle->_PHandle->GetArray(_key) );
				return _2ndConfig;
			}
			Config GetConfig( const RString & _key ) {
				Config _2ndConfig;
				_2ndConfig.Parse( TFather::_Handle->_PHandle->GetArray(_key) );
				return _2ndConfig;
			}
			
			// Null Object
			static Config			Null;
			static Config CreateNullConfig( ) {
				return Config( false );
			}
		};
		
		Config	Config::Null( false );
	}
}

#endif // plib.text.config.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */

