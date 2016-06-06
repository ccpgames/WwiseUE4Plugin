//////////////////////////////////////////////////////////////////////
//
// AkDefaultIOHookDeferred.cpp
//
// Default Deferred low level IO hook (AK::StreamMgr::IAkIOHookDeferred) 
// and file system (AK::StreamMgr::IAkFileLocationResolver) implementation 
// on Vita. It can be used as a standalone implementation of the 
// Low-Level I/O system.
// 
// AK::StreamMgr::IAkFileLocationResolver: 
// Resolves file location using simple path concatenation logic 
// (implemented in ../Common/CAkFileLocationBase). It can be used as a 
// standalone Low-Level IO system, or as part of a multi device system. 
// In the latter case, you should manage multiple devices by implementing 
// AK::StreamMgr::IAkFileLocationResolver elsewhere (you may take a look 
// at class CAkDefaultLowLevelIODispatcher).
//
// AK::StreamMgr::IAkIOHookDeferred: 
// Uses the standard, Deferred API for I/O (cellFsRead, cellFsWrite).
// The AK::StreamMgr::IAkIOHookDeferred interface is meant to be used with
// AK_SCHEDULER_Deferred streaming devices. 
//
// Init() creates a streaming device (by calling AK::StreamMgr::CreateDevice()).
// AkDeviceSettings::uSchedulerTypeFlags is set inside to AK_SCHEDULER_Deferred.
// If there was no AK::StreamMgr::IAkFileLocationResolver previously registered 
// to the Stream Manager, this object registers itself as the File Location Resolver.
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AkDefaultIOHookDeferred.h"
#include "AkFileHelpers.h"

// TO DO: Use fios2!

#define _DEFERRED_DEVICE_NAME		(L"Vita Deferred")	// Device Deferred name.

CAkDefaultIOHookDeferred::CAkDefaultIOHookDeferred()
: m_deviceID( AK_INVALID_DEVICE_ID )
, m_bAsyncOpen( false )
{
}

CAkDefaultIOHookDeferred::~CAkDefaultIOHookDeferred()
{
}

// Initialization/termination. Init() registers this object as the one and 
// only File Location Resolver if none were registered before. Then 
// it creates a streaming device with scheduler type AK_SCHEDULER_Deferred.
AKRESULT CAkDefaultIOHookDeferred::Init(
	const AkDeviceSettings &	in_deviceSettings,		// Device settings.
	bool						in_bAsyncOpen/*=true*/,	// If true, files are opened asynchronously when possible.
	AkOSChar * 					in_pszMountPoint/*=NULL*/	// Mount point (if NULL, SYS_DEV_HDD0 is used).
	)
{
	if ( in_deviceSettings.uSchedulerTypeFlags != AK_SCHEDULER_DEFERRED_LINED_UP )
	{
		AKASSERT( !"CAkDefaultIOHookDeferred I/O hook only works with AK_SCHEDULER_DEFERRED_LINED_UP devices" );
		return AK_Fail;
	}
	
	m_bAsyncOpen = in_bAsyncOpen;
	
	// If the Stream Manager's File Location Resolver was not set yet, set this object as the 
	// File Location Resolver (this I/O hook is also able to resolve file location).
	if ( !AK::StreamMgr::GetFileLocationResolver() )
		AK::StreamMgr::SetFileLocationResolver( this );

	// Create a device in the Stream Manager, specifying this as the hook.
	m_deviceID = AK::StreamMgr::CreateDevice( in_deviceSettings, this );
	if ( m_deviceID != AK_INVALID_DEVICE_ID )
		return AK_Success;

	return AK_Fail;
}

void CAkDefaultIOHookDeferred::Term()
{
	if ( AK::StreamMgr::GetFileLocationResolver() == this )
		AK::StreamMgr::SetFileLocationResolver( NULL );
	
	AK::StreamMgr::DestroyDevice( m_deviceID );
}

//
// IAkFileLocationAware implementation.
//-----------------------------------------------------------------------------

// Returns a file descriptor for a given file name (string).
AKRESULT CAkDefaultIOHookDeferred::Open( 
    const AkOSChar* in_pszFileName,     // File name.
    AkOpenMode      in_eOpenMode,       // Open mode.
    AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
	bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
    AkFileDesc &    out_fileDesc        // Returned file descriptor.
    )
{
	// Perform synchronous opening if required, or if m_bAsyncOpen is false.
	if ( io_bSyncOpen || !m_bAsyncOpen )
	{
		io_bSyncOpen = true;

	    // Get the full file path, using path concatenation logic.
	    AkOSChar szFullFilePath[AK_MAX_PATH];
	    if ( GetFullFilePath( in_pszFileName, in_pFlags, in_eOpenMode, szFullFilePath ) == AK_Success )
		{
			// Open the file.
			AKRESULT eResult = CAkFileHelpers::OpenFile( 
				szFullFilePath,
				in_eOpenMode,
				out_fileDesc.hFile );
			if ( eResult == AK_Success )
			{
				// Fill AkFileDescriptor structure.
				SceFiosStat stats;
				int res = sceFiosFHStatSync( NULL, out_fileDesc.hFile, &stats );
				if ( res == SCE_FIOS_OK )
				{
					out_fileDesc.iFileSize 			= stats.fileSize;
					out_fileDesc.uSector			= 0;
					out_fileDesc.deviceID			= m_deviceID;
					out_fileDesc.pCustomParam		= NULL;
					out_fileDesc.uCustomParamSize	= 0;
				}
				else
				{
					AKASSERT( ! "CAkDefaultIOHookBlocking::Open() - sceFiosFHStatSync() failed!" );
					out_fileDesc.iFileSize 			= 0;
					out_fileDesc.uSector			= 0;
					out_fileDesc.deviceID			= AK_INVALID_DEVICE_ID;
					out_fileDesc.pCustomParam		= NULL;
					out_fileDesc.uCustomParamSize	= 0;
					eResult = AK_Fail;
				}
			}
			return eResult;
		}
		return AK_Fail;    
	}
	else
	{
		// The client allows us to perform asynchronous opening.
		// We only need to specify the deviceID, and leave the boolean to false.
		out_fileDesc.iFileSize			= 0;
		out_fileDesc.uSector			= 0;
		out_fileDesc.deviceID			= m_deviceID;
		out_fileDesc.pCustomParam		= NULL;
		out_fileDesc.uCustomParamSize	= 0;
		return AK_Success;		
	}
}

// Returns a file descriptor for a given file ID.
AKRESULT CAkDefaultIOHookDeferred::Open( 
    AkFileID        in_fileID,          // File ID.
    AkOpenMode      in_eOpenMode,       // Open mode.
    AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
	bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
    AkFileDesc &    out_fileDesc        // Returned file descriptor.
    )
{
	// Perform synchronous opening if required, or if m_bAsyncOpen is false.
    if ( io_bSyncOpen || !m_bAsyncOpen )
	{
		io_bSyncOpen = true;

	    // Get the full file path, using path concatenation logic.
	    AkOSChar szFullFilePath[AK_MAX_PATH];
	    if ( GetFullFilePath( in_fileID, in_pFlags, in_eOpenMode, szFullFilePath ) == AK_Success )
		{
			// Open the file.
			AKRESULT eResult = CAkFileHelpers::OpenFile( 
				szFullFilePath,
				in_eOpenMode,
				out_fileDesc.hFile );
			if ( eResult == AK_Success )
			{
				// Fill AkFileDescriptor structure.
				SceFiosStat stats;
				int res = sceFiosFHStatSync( NULL, out_fileDesc.hFile, &stats );
				if ( res == SCE_FIOS_OK )
				{
					out_fileDesc.iFileSize 			= stats.fileSize;
					out_fileDesc.uSector			= 0;
					out_fileDesc.deviceID			= m_deviceID;
					out_fileDesc.pCustomParam		= NULL;
					out_fileDesc.uCustomParamSize	= 0;
				}
				else
				{
					AKASSERT( ! "CAkDefaultIOHookBlocking::Open() - sceFiosFHStatSync() failed!" );
					out_fileDesc.iFileSize 			= 0;
					out_fileDesc.uSector			= 0;
					out_fileDesc.deviceID			= AK_INVALID_DEVICE_ID;
					out_fileDesc.pCustomParam		= NULL;
					out_fileDesc.uCustomParamSize	= 0;
					eResult = AK_Fail;
				}
			}
			return eResult;
		}
		return AK_Fail;
	}	
	else
	{
		// The client allows us to perform asynchronous opening.
		// We only need to specify the deviceID, and leave the boolean to false.
		out_fileDesc.iFileSize			= 0;
		out_fileDesc.uSector			= 0;
		out_fileDesc.deviceID			= m_deviceID;
		out_fileDesc.pCustomParam		= NULL;
		out_fileDesc.uCustomParamSize	= 0;
		return AK_Success;		
	}
}

//
// IAkIOHookDeferred implementation.
//-----------------------------------------------------------------------------

// Reads data from a file (synchronous). 
AKRESULT CAkDefaultIOHookDeferred::Read(
    AkFileDesc &			in_fileDesc,        // File descriptor.
	const AkIoHeuristics & /*in_heuristics*/,	// Heuristics for this data transfer (not used in this implementation).
	AkAsyncIOTransferInfo & io_transferInfo		// Asynchronous data transfer info.
    )
{
    AKASSERT( io_transferInfo.pBuffer );
	
	// Read.
	AkUInt32 uSizeTransferred;
	AKRESULT eResult = CAkFileHelpers::ReadBlocking(
		in_fileDesc.hFile,
		io_transferInfo.pBuffer,
		io_transferInfo.uFilePosition,
		io_transferInfo.uRequestedSize,
		uSizeTransferred );

	if ( AK_Success == eResult )
		io_transferInfo.pCallback( &io_transferInfo, AK_Success );

	return eResult;
}

// Writes data to a file (synchronous). 
AKRESULT CAkDefaultIOHookDeferred::Write(
	AkFileDesc &			in_fileDesc,        // File descriptor.
	const AkIoHeuristics & /*in_heuristics*/,	// Heuristics for this data transfer (not used in this implementation).
    AkAsyncIOTransferInfo & io_transferInfo		// Platform-specific asynchronous IO operation info.
    )
{
    AKASSERT( io_transferInfo.pBuffer );

	// Seek to desired position
	SceFiosOffset newOffset = sceFiosFHSeek( in_fileDesc.hFile, io_transferInfo.uFilePosition, SCE_FIOS_SEEK_SET );
	if ( newOffset != io_transferInfo.uFilePosition )
	{
		return AK_Fail;
	}

	// Write to file
	SceFiosSize uSizeWritten = sceFiosFHWriteSync( NULL, in_fileDesc.hFile, io_transferInfo.pBuffer, io_transferInfo.uRequestedSize );

	if ( uSizeWritten > 0 )
	{
		io_transferInfo.pCallback( &io_transferInfo, AK_Success );
		return AK_Success;
	}

	return AK_Fail;
}

void CAkDefaultIOHookDeferred::Cancel(
		AkFileDesc &			in_fileDesc,		// File descriptor.
		AkAsyncIOTransferInfo & io_transferInfo,	// Transfer info to cancel.
		bool & io_bCancelAllTransfersForThisFile	// Flag indicating whether all transfers should be cancelled for this file (see notes in function description).
	)
{
	// TO DO
}

// Cleans up a file.
AKRESULT CAkDefaultIOHookDeferred::Close(
    AkFileDesc & in_fileDesc	// File descriptor.
    )
{
    return CAkFileHelpers::CloseFile( in_fileDesc.hFile );
}

// Returns the block size for the file or its storage device. 
AkUInt32 CAkDefaultIOHookDeferred::GetBlockSize(
    AkFileDesc &  /*in_fileDesc*/     // File descriptor.
    )
{
	// No constraint on block size (file seeking).
    return 1;
}


// Returns a description for the streaming device above this low-level hook.
void CAkDefaultIOHookDeferred::GetDeviceDesc(
    AkDeviceDesc & 
#ifndef AK_OPTIMIZED
	out_deviceDesc      // Description of associated low-level I/O device.
#endif
    )
{
#ifndef AK_OPTIMIZED
	out_deviceDesc.deviceID       = m_deviceID;
	out_deviceDesc.bCanRead       = true;
	out_deviceDesc.bCanWrite      = true;
	AKPLATFORM::SafeStrCpy( out_deviceDesc.szDeviceName, _DEFERRED_DEVICE_NAME, AK_MONITOR_DEVICENAME_MAXLENGTH );
	out_deviceDesc.uStringSize   = (AkUInt32)wcslen( out_deviceDesc.szDeviceName ) + 1;
#endif
}


// Returns custom profiling data: 1 if file opens are asynchronous, 0 otherwise.
AkUInt32 CAkDefaultIOHookDeferred::GetDeviceData()
{
	return ( m_bAsyncOpen ) ? 1 : 0;
}
