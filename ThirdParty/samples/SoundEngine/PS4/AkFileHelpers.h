//////////////////////////////////////////////////////////////////////
//
// AkFileHelpers.h
//
// Platform-specific helpers for files.
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_FILE_HELPERS_H_
#define _AK_FILE_HELPERS_H_

#include <AK/Tools/Common/AkPlatformFuncs.h>

#include <fios2.h>

#include <AK/Tools/Common/AkAssert.h>
#include <string.h>

#define __AKFILEHELPERS_ASSERT( Condition ) AKASSERT( Condition );	

class CAkFileHelpers
{
public:

	// Wrapper for PS3 cellFsOpen().
	static AKRESULT OpenFile( 
        const AkOSChar* in_pszFilename,     // File name.
        AkOpenMode      in_eOpenMode,       // Open mode.
        AkFileHandle &  out_hFile           // Returned file identifier/handle.
        )
	{
		// Check parameters.
		__AKFILEHELPERS_ASSERT(in_pszFilename != NULL);
		__AKFILEHELPERS_ASSERT(strlen(in_pszFilename) < AK_MAX_PATH);

		out_hFile = (AkFileHandle)NULL;

		// Open mode
		SceFiosOpenParams params = SCE_FIOS_OPENPARAMS_INITIALIZER;
	    switch ( in_eOpenMode)
	    {
			case AK_OpenModeRead:
				params.openFlags = SCE_FIOS_O_RDONLY;
	            break;
	        case AK_OpenModeWrite:
				params.openFlags = SCE_FIOS_O_CREAT | SCE_FIOS_O_WRONLY;
	            break;
	        case AK_OpenModeWriteOvrwr:
				params.openFlags = SCE_FIOS_O_CREAT | SCE_FIOS_O_WRONLY | SCE_FIOS_O_TRUNC;
	            break;
	        case AK_OpenModeReadWrite:
				params.openFlags = SCE_FIOS_O_RDWR;
	            break;
			default:
	            __AKFILEHELPERS_ASSERT( !"Invalid open mode");
	            out_hFile = -1;
	            return AK_InvalidParameter;
	    }

		const int result = sceFiosFHOpenSync( NULL, &out_hFile, in_pszFilename, &params );
		if ( result != SCE_FIOS_OK )
	    {
			if (out_hFile != (AkFileHandle)NULL)
				sceFiosFHCloseSync( NULL, out_hFile );

			out_hFile = (AkFileHandle)NULL;

			if ( SCE_FIOS_ERROR_BAD_PATH == result )
				return AK_FileNotFound;

			return AK_Fail;
		}
	
		return AK_Success;
	}

	// Wrapper for system file handle closing.
	static AKRESULT CloseFile( AkFileHandle in_hFile )
	{
		if ( sceFiosFHCloseSync( NULL, in_hFile ) != SCE_FIOS_OK )
		{
			__AKFILEHELPERS_ASSERT( !"Failed to close file handle" );
			return AK_Fail;
		}
		return AK_Success;
	}

	//
	// Simple platform-independent API to open and read files using AkFileHandles, 
	// with blocking calls and minimal constraints.
	// ---------------------------------------------------------------------------

	// Open file to use with ReadBlocking().
	static AKRESULT OpenBlocking(
        const AkOSChar* in_pszFilename,     // File name.
        AkFileHandle &  out_hFile           // Returned file handle.
		)
	{
		return OpenFile( 
			in_pszFilename,
			AK_OpenModeRead,
			out_hFile );
	}

	// Required block size for reads (used by ReadBlocking() below).
	static const AkUInt32 s_uRequiredBlockSize = 1;

	// Simple blocking read method.
	static AKRESULT ReadBlocking(
        AkFileHandle &	in_hFile,			// Returned file identifier/handle.
		void *			in_pBuffer,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.
		SceFiosOffset	in_uPosition,		// Position from which to start reading.
		AkUInt32		in_uSizeToRead,		// Size to read. Must be a multiple of CAkFileHelpers::s_uRequiredBlockSize.
		AkUInt32 &		out_uSizeRead		// Returned size read.        
		)
	{
		__AKFILEHELPERS_ASSERT( in_uSizeToRead % s_uRequiredBlockSize == 0 
			&& in_uPosition % s_uRequiredBlockSize == 0 );

		SceFiosOffset newOffset = sceFiosFHSeek( in_hFile, in_uPosition, SCE_FIOS_SEEK_SET );
		if ( newOffset != in_uPosition )
		{
			return AK_Fail;
		}

		SceFiosSize uSizeRead = sceFiosFHReadSync( NULL, in_hFile, in_pBuffer, in_uSizeToRead );
		if ( uSizeRead >= 0 )
		{
			out_uSizeRead = (AkUInt32)uSizeRead;
			return AK_Success;
		}
		return AK_Fail;
	}

	/// Returns AK_Success if the directory is valid, AK_Fail if not.
	/// For validation purposes only.
	/// Some platforms may return AK_NotImplemented, in this case you cannot rely on it.
	static AKRESULT CheckDirectoryExists( const AkOSChar* in_pszBasePath )
	{
		// Not Implemented on this platform. For validation purposes only so return true in this situation.
		return AK_NotImplemented;
	}

	static AKRESULT WriteBlocking(
		AkFileHandle &	in_hFile,			// Returned file identifier/handle.
		void *			in_pData,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.		
		AkUInt64		in_uPosition,		// Position from which to start writing.
		AkUInt32		in_uSizeToWrite)
	{
		AKASSERT( in_pData );

		// Seek to desired position
		SceFiosOffset newOffset = sceFiosFHSeek( in_hFile, in_uPosition, SCE_FIOS_SEEK_SET );
		if ( newOffset != in_uPosition )
		{
			return AK_Fail;
		}

		// Write to file
		SceFiosSize uSizeWritten = sceFiosFHWriteSync( NULL, in_hFile, in_pData, in_uSizeToWrite );
		if ( uSizeWritten > 0 )
			return AK_Success;

		return AK_Fail;
	}
};

#endif //_AK_FILE_HELPERS_H_
