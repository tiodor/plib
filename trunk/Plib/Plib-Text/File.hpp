/*
* Copyright (c) 2010, Push Chen
* All rights reserved.
* 
* File Name			: File.hpp
* Propose  			: File Operators, as old ioblock.
* 
* Current Version	: 1.0
* Change Log		: First Definition.
* Author			: Push Chen
* Change Date		: 2011-07-08
*/

#pragma once

#ifndef _PLIB_TEXT_FILE_HPP_
#define _PLIB_TEXT_FILE_HPP_

#if _DEF_IOS
#include "Convert.hpp"
#else
#include <Plib-Text/Convert.hpp>
#endif

#include <fcntl.h>
#if _DEF_WIN32
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif

namespace Plib
{
	namespace Text
	{
		
	#if _DEF_WIN32
		#define __OPEN__			_open
		#define __CLOSE__			_close
		#define __READ__			_read
		#define __WRITE__			_write
		#define __SEEK__			_lseek
		#define __EOF__				_eof
		#define __PERMISSION__		_S_IREAD | _S_IWRITE
		#define __DIR__				_getcwd
	#else
		#define __OPEN__			::open
		#define __CLOSE__			::close
		#define __READ__			::read
		#define __WRITE__			::write
		#define __SEEK__			::lseek
		#define __EOF__				::eof
		#define __PERMISSION__		0644
		#define __DIR__				::getcwd
	#endif
	
		/*
		 * File Stream, Act as a file object.
		 */
		class FileStream_
		{
		public:
			typedef Int32			FHandle;
			// File Stream Open Statue.
			enum FOSTATUE {
				FS_READ 	= O_RDONLY,
				FS_WRITE	= O_WRONLY | O_CREAT | O_TRUNC,
				FS_APPEND	= O_APPEND | O_CREAT
			};
			
			// Max buffer size.
			enum { FS_MAX_BUFFER_SIZE = 0x4000 };
			
		protected:
			// Inner Buffer, when the buffer contains the data to a limit
			// range, flush to the file.
			RString 				__Buffer;
			// The file path buffer.
			RString					__FilePath;
			// File Handle
			FHandle					__Handle;
			// File Open Statue.
			FOSTATUE				__OStatue;
		private:
			// No Copy
			FileStream_( const FileStream_ & _fs );
			FileStream_ & operator = ( const FileStream_ & _fs );
			
		protected:
			// Flush the data to the file.
			// if the data is empty or the file is not opened or just for read, return false.
			INLINE bool __FlushData( const RString & __data ) {
				// Parameters checking.
				if ( __Handle == -1 || __OStatue == FS_READ || __data.Empty() ) return false;
				
				// Invoke the c-api to write the data to the file.
				if ( -1 == __WRITE__( __Handle, __data.c_str(), __data.size() ) )
				{
					// On error close the file and reset the handle.
					__CLOSE__(__Handle);
					__Handle = -1;
					return false;
				}
				return true;
			}
			
			// Get the file's length.
			INLINE Uint32 __FileSize( ) {
			#if _DEF_WIN32
				struct _stat st;
				if ( 0 != _stat(__FilePath.c_str(), &st) ) 
				{
			#else
				struct stat st;
				if ( 0 != stat(__FilePath.c_str(), &st) ) 
				{
			#endif
					return -1;
				}
				return st.st_size;				
			}
			
		public:
			// Default c'str
			FileStream_( ) : __Handle( -1 ) { }
			// Set the file path
			INLINE void FilePath( const RString & _filePath ) {
				if ( !_filePath.RefNull() )
				//if ( _filePath != RString::Null )
					__FilePath.DeepCopy( _filePath );
			}
			
			// C'Str, init the filestream with the filepath,
			// the filepath must not be Null.
			FileStream_( const RString & _filePath ) : __Handle( -1 ) {
				assert( _filePath != RString::Null );
				__FilePath.DeepCopy( _filePath );
			}
			
			// D'Str, close on destroy.
			~FileStream_( ) { Close( ); }
			
			// Open the file for reading only.
			bool OpenRead( ) {
				if ( __Handle != -1 ) return __OStatue == FS_READ;
				if ( (__Handle = __OPEN__( __FilePath.c_str(), FS_READ )) == -1 )
					return false;
				__OStatue = FS_READ;
				return true;
			}
			// Open the file for writing only.
			bool OpenWrite( ) {
				if ( __Handle != -1 ) return __OStatue == FS_WRITE;
				if ( (__Handle = __OPEN__( __FilePath.c_str(), FS_WRITE, __PERMISSION__ )) == -1 )
					return false;
				__OStatue = FS_WRITE;
				return true;
			}
			// Open the file for data appending only.
			bool OpenAppend( ) {
				if ( __Handle != -1 ) return __OStatue == FS_APPEND;
				if ( (__Handle = __OPEN__( __FilePath.c_str(), FS_APPEND, __PERMISSION__ )) == -1 )
					return false;
				__OStatue = FS_APPEND;
				return true;
			}
			// Check if the file is opened correctly.
			operator bool ( ) const {
				return (__Handle != -1);
			}

			// Save the file, flush the buffer to disk.
			INLINE bool Save( ) {
				if ( !__FlushData( __Buffer ) ) return false;
				__Buffer.Clear();
				return true;
			}
			
			// Close the file handle
			INLINE void Close( ) {
				if ( __Handle == -1 ) return;
				if ( !this->Save( ) ) return;
				__CLOSE__(__Handle);
				__Handle = -1;
			}
			
			// Write data to the buffer, if the buffer is full, flush 
			// it to the file.
			bool Write( const RString & _data ) {
				if ( __Handle == -1 || __OStatue != FS_WRITE ) return false;
				
				// Check buffer size
				if ( _data.Size() >= FS_MAX_BUFFER_SIZE ) {
					// Save buffer failed.
					if ( !Save( ) ) return false;
					// Flush current data.
					return __FlushData( _data );
				}
				// if data is not bigger enough, but the final data is too big
				if ( (_data.Size() + __Buffer.Size()) >= FS_MAX_BUFFER_SIZE )
				{
					if ( !Save( ) ) return false;
				}
				// RString is thread safe.
				__Buffer += _data;
				return true;
			}
			
			// write the data to the file, and append a new line.
			bool WriteLine( const RString & _line ) {
				if ( !Write( _line ) ) return false;
				return Write( "\r\n" );
			}
			
			// Append the data to the file.
			bool Append( const RString & _data ) {
				if ( __Handle == -1 || __OStatue != FS_APPEND ) return false;
				
				// Check buffer size
				if ( _data.Size() >= FS_MAX_BUFFER_SIZE ) {
					// Save buffer failed.
					if ( !Save( ) ) return false;
					// Flush current data.
					return __FlushData( _data );
				}
				// if data is not bigger enough, but the final data is too big
				if ( (_data.Size() + __Buffer.Size()) >= FS_MAX_BUFFER_SIZE )
				{
					if ( !Save( ) ) return false;
				}
				// RString is thread safe.
				__Buffer += _data;
				return true;
			}
			
			// Append the data with new line.
			bool AppendLine( const RString & _line ) {
				if ( !Append( _line ) ) return false;
				return Append( "\r\n" );
			}
			
			// Read one word from the file
			RString Read( ) {
				if ( __Handle == -1 || __OStatue != FS_READ ) return RString::Null;
				char _c;
				RString _word;
				Int32 _ret = __READ__(__Handle, &_c, sizeof(char));
				while ( _ret > 0 ) {
					// Meet a space character
					if ( isspace(_c) > 0 ) {
						_word.Trim( );
						// if the word not still empty, then return the word
						if ( !_word.Empty( ) ) break;
						// otherwise, continue reading the file for another word.
					} else {
						_word += _c;
					}
					_ret = __READ__(__Handle, &_c, sizeof(char));
				}
				// if error occurred..
				if ( _ret < 0 ) return RString::Null;
				return _word;
			}
			
			// Read specified length data.
			RString Read( Uint32 _length ) {
				if ( __Handle == -1 || __OStatue != FS_READ ) return RString::Null;
				char _c;
				Int32 _ret;
				RString _word;
				// Just a loop, ignoral the space character.
				for ( Uint32 i = 0; i < _length; ++i ) {
					_ret = __READ__(__Handle, &_c, sizeof(char));
					if ( _ret <= 0 ) break;
					_word += _c;
				}
				if ( _ret < 0 ) return RString::Null;
				return _word;
			}
			
			// Read a line from the file. May return an empty line.
			RString ReadLine( ) {
				if ( __Handle == -1 || __OStatue != FS_READ ) return RString::Null;
				char _c;
				Int32 _ret;
				RString _line;
				for ( ; ; ) {
					_ret = __READ__(__Handle, &_c, sizeof(char));
					if ( _ret <= 0 ) break;
					if ( _c == '\n' ) break;
					_line += _c;
				}
				if ( _ret <= 0 ) return RString::Null;
				return _line.Trim( );
			}
			
			// Read to the end of the file.
			RString ReadToEnd( ) {
				if ( __Handle == -1 || __OStatue != FS_READ ) return RString::Null;
				Uint32 _fSize = __FileSize( );
				RString _buffer( 0, _fSize + 1 );
				Int32 _ret = __READ__(__Handle, const_cast< char * >(_buffer.c_str( )), 
					sizeof(char) * _fSize);
				if ( _ret < 0 ) return RString::Null;
				return _buffer;
			}
		};
		
		// Reference Version of File Stream.
		// Read stream.
		template < Uint32 _dummy >
		class ReadStream_ : public Plib::Generic::Reference< FileStream_ >
		{
			typedef Plib::Generic::Reference< FileStream_ >	TFather;
		protected:
			ReadStream_( bool _bNull ) : TFather( false ) { CONSTRUCTURE; }
		public:
			// C'Str, set the file path and open for reading.
			ReadStream_( const RString & _filePath ) : TFather( true ) {
				CONSTRUCTURE;
				TFather::_Handle->_PHandle->FilePath( _filePath );
				TFather::_Handle->_PHandle->OpenRead( );
			}
			ReadStream_( const ReadStream_ & rhs ) : TFather( rhs )
				{ CONSTRUCTURE; }
			~ReadStream_( ) { DESTRUCTURE; }
						
			// Close the file stream.
			INLINE void Close( ) { TFather::_Handle->_PHandle->Close( ); }
			// Read a word
			INLINE RString Read( ) { return TFather::_Handle->_PHandle->Read(  ); }
			// Read specified length data.
			INLINE RString Read( Uint32 _Length ) { return TFather::_Handle->_PHandle->Read( _Length ); }
			// Read a line.
			INLINE RString ReadLine( ) { return TFather::_Handle->_PHandle->ReadLine( ); }
			// Read all data.
			INLINE RString ReadToEnd( ) { return TFather::_Handle->_PHandle->ReadToEnd( ); }
			// if the readstream is validate.
			INLINE operator bool ( ) const { return TFather::_Handle->_PHandle->operator bool(); }
			
			// Null object.
			static ReadStream_			Null;
			
			// Create a null object.
			static ReadStream_ CreateNullReadStream( ) {
				return ReadStream_( false );
			}
		};
		// Static null readstream.
		template < Uint32 _dummy >
		ReadStream_<_dummy>	ReadStream_<_dummy>::Null;
		typedef ReadStream_<0> ReadStream;
		
		// Write Stream..
		template< Uint32 _dummy >
		class WriteStream_ : public Plib::Generic::Reference< FileStream_ >
		{
			typedef Plib::Generic::Reference< FileStream_ >	TFather;
		protected:
			WriteStream_( bool _bNull ) : TFather( false ) { CONSTRUCTURE; }
		public:
			// C'Str, set the file path and open for reading.
			WriteStream_( const RString & _filePath ) : TFather( true ) {
				CONSTRUCTURE;
				TFather::_Handle->_PHandle->FilePath( _filePath );
				TFather::_Handle->_PHandle->OpenWrite( );
			}
			WriteStream_( const WriteStream_ & rhs ) : TFather( rhs )
				{ CONSTRUCTURE; }
			~WriteStream_( ) { DESTRUCTURE; }
						
			// Close the file stream.
			INLINE void Close( ) { TFather::_Handle->_PHandle->Close( ); }
			// Save data
			INLINE bool Save( ) { return TFather::_Handle->_PHandle->Save( ); }
			// Write data.
			INLINE bool Write( const RString & _data ) { 
				return TFather::_Handle->_PHandle->Write( _data );
			}
			// Write Line.
			INLINE bool WriteLine( const RString & _data ) {
				return TFather::_Handle->_PHandle->WriteLine( _data );
			}
			INLINE operator bool ( ) const { return TFather::_Handle->_PHandle->operator bool(); }
			
			// Null object.
			static WriteStream_			Null;
			
			// Create a null object.
			static WriteStream_ CreateNullWriteStream( ) {
				return WriteStream_( false );
			}
		};
		// Static null readstream.
		template < Uint32 _dummy >
		WriteStream_<_dummy>	WriteStream_<_dummy>::Null;
		typedef WriteStream_<0>	WriteStream;
		
		// Append Stream..
		template < Uint32 _dummy >
		class AppendStream_ : public Plib::Generic::Reference< FileStream_ >
		{
			typedef Plib::Generic::Reference< FileStream_ >	TFather;
		protected:
			AppendStream_( bool _bNull ) : TFather( false ) { CONSTRUCTURE; }
		public:
			// C'Str, set the file path and open for reading.
			AppendStream_( const RString & _filePath ) : TFather( true ) {
				CONSTRUCTURE;
				TFather::_Handle->_PHandle->FilePath( _filePath );
				TFather::_Handle->_PHandle->OpenAppend( );
			}
			AppendStream_( const AppendStream_ & rhs ) : TFather( rhs )
				{ CONSTRUCTURE; }
			~AppendStream_( ) { DESTRUCTURE; }
						
			// Close the file stream.
			INLINE void Close( ) { TFather::_Handle->_PHandle->Close( ); }
			// Save data
			INLINE bool Save( ) { return TFather::_Handle->_PHandle->Save( ); }
			// Write data.
			INLINE bool Append( const RString & _data ) { 
				return TFather::_Handle->_PHandle->Append( _data );
			}
			// Write Line.
			INLINE bool AppendLine( const RString & _data ) {
				return TFather::_Handle->_PHandle->AppendLine( _data );
			}
			INLINE operator bool ( ) const { return TFather::_Handle->_PHandle->operator bool(); }
			
			// Null object.
			static AppendStream_			Null;
			
			// Create a null object.
			static AppendStream_ CreateNullAppendStream( ) {
				return AppendStream_( true );
			}
		};
		// Static null readstream.
		template < Uint32 _dummy >
		AppendStream_<_dummy>	AppendStream_<_dummy>::Null;
		typedef AppendStream_<0>	AppendStream;
		
		/*
		 * Directory
		 */
		class Directory_
		{
			
		};
		
		// Reference Version of Directory.
		class Directory : public Plib::Generic::Reference< Directory_ >
		{
			
		};
		
		/*
		 * File Operator Structure
		 * Like Convert, All static methods
		 */
		struct File
		{
			// Check if a file is already existed.
			static bool IsExisted( const RString & _filePath ) {
			#if __DEF_WIN32
				return _access(_filePath.c_str(), 0) == 0;
			#else
				return access(_filePath.c_str(), 0) == 0;
			#endif
			}
			
			// Create a file according to the _filePath, 
			// If the file is already existed, do nothing.
			static bool Create( const RString & _filePath ) {
				if ( IsExisted( _filePath ) ) return true;
				Int32 _fHandle = __OPEN__( _filePath.c_str(), FileStream_::FS_WRITE, __PERMISSION__ );
				if ( _fHandle < 0 ) return false;
				__CLOSE__(_fHandle);
				return true;
			}
			
			// Delete the file of _filePath.
			// On error return false. if the file is not existed, do nothing.
			static bool Delete( const RString & _filePath ) {
				return remove( _filePath.c_str() ) == 0;
			}
			
			// Move the file to other place.
			// if the file is not existed, return false.
			static bool Move( const RString & _source, const RString & _dest ) {
				return rename( _source.c_str(), _dest.c_str() ) == 0;
			}
			
			// Copy a file to another one.
			static bool Copy( const RString & _source, const RString & _dest, bool _overwrite ) {
				if ( ! IsExisted( _source ) ) return false;
				if ( IsExisted( _dest ) && !_overwrite ) return false;
				// rb, wb, two file
				FILE * pRead = fopen( _source.c_str(), "r" );
				if ( pRead == NULL ) return false;
				
				FILE * pWrite = fopen( _source.c_str(), "w" );
				if ( pWrite == NULL ) { fclose( pRead ); return false; }
				
				// Write data
				bool _allright = true;
				char _c = fgetc(pRead);
				while ( _c != EOF ) {
					if ( EOF == fputc( _c, pWrite ) ) {
						_allright = false;
						break;
					}
					_c = fgetc( pRead );
				}
				fclose( pRead );
				fflush( pWrite );
				fclose( pWrite );
				
				return _allright;
			}
			
			// Append data to the file.
			// if the file is not existed, print the data use cerr, switch
			// with the flag.
			static bool Append( const RString & _filePath, const RString & _data, bool _onErrorPrint = true ) {
				AppendStream _fStream( _filePath );
				if ( !_fStream || !_fStream.Append( _data ) ) {
					if ( _onErrorPrint == true ) {
						std::cerr << _data;
						return true;
					}
					return false;
				}
				return _fStream.Save();
			}
			
			// Clear a file's content.
			// Make the file size to be zero.
			static bool Clear( const RString & _filePath ) {
				if ( !IsExisted( _filePath ) ) return false;
				FILE * pFile = fopen( _filePath.c_str(), "w" );
				if ( pFile == NULL ) return false;
				fclose( pFile );
				return true;
			}
			
			// Resize a file.
			static bool Truncate( const RString & _filePath, Uint32 _size, bool _beHole = true ) {
				
				return false;
			}
			
			// Get the file's info.
			// Create time
			static Uint32 CreateTime( const RString & _filePath ) {
			#if _DEF_WIN32
				struct _stat st;
				if ( 0 != _stat(_filePath.c_str(), &st) ) 
				{
			#else
				struct stat st;
				if ( 0 != stat(_filePath.c_str(), &st) ) 
				{
			#endif
					return -1;
				}
				
			#if _DEF_MAC
				return st.st_ctimespec.tv_sec;
			#else
				return st.st_ctime;
			#endif
			}
			
			// Modify time
			static Uint32 ModifyTime( const RString & _filePath ) {
			#if _DEF_WIN32
				struct _stat st;
				if ( 0 != _stat(_filePath.c_str(), &st) ) 
				{
			#else
				struct stat st;
				if ( 0 != stat(_filePath.c_str(), &st) ) 
				{
			#endif
					return -1;
				}
				
			#if _DEF_MAC
				return st.st_mtimespec.tv_sec;
			#else
				return st.st_mtime;
			#endif
			}
			
			// Last Access Time
			static Uint32 AccessTime( const RString & _filePath ) {
			#if _DEF_WIN32
				struct _stat st;
				if ( 0 != _stat(_filePath.c_str(), &st) ) 
				{
			#else
				struct stat st;
				if ( 0 != stat(_filePath.c_str(), &st) ) 
				{
			#endif
					return -1;
				}
				
			#if _DEF_MAC
				return st.st_atimespec.tv_sec;
			#else
				return st.st_atime;
			#endif
			}
			
			// File Size
			static Uint32 FileSize( const RString & _filePath ) {
			#if _DEF_WIN32
				struct _stat st;
				if ( 0 != _stat(_filePath.c_str(), &st) ) 
				{
			#else
				struct stat st;
				if ( 0 != stat(_filePath.c_str(), &st) ) 
				{
			#endif
					return -1;
				}
				return st.st_size;
			}
		};
	}
}

#endif // plib.text.file.hpp

/*
 Push Chen.
 littlepush@gmail.com
 http://pushchen.com
 http://twitter.com/littlepush
 */
