//////////////////////////////////////////////////////////////////////
//
// AkUnrealIOHookDeferred.h
//
// Implementation of the IO Hook redirecting 
// the Read requests into Unreal I/O system.
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AkInclude.h"

//-----------------------------------------------------------------------------
// Name: class CAkUnrealIOHookDeferred.
// Desc: Implements IAkIOHookDeferred low-level I/O hook, and 
//		 IAkFileLocationResolver. Can be used as a standalone Low-Level I/O
//		 system, or as part of a system with multiple devices.
//		 File location is resolved using simple path concatenation logic
//		 (implemented in CAkFileLocationBase).
//-----------------------------------------------------------------------------
class CAkUnrealIOHookDeferred :	public AK::StreamMgr::IAkFileLocationResolver
								,public AK::StreamMgr::IAkIOHookDeferred
{
public:

	/**
	 * Constructor.
	 */
	CAkUnrealIOHookDeferred();

	/**
	 * Destructor.
	 */
	virtual ~CAkUnrealIOHookDeferred();

	/** 
	 * Initialization. Init() registers this object as the one and 
	 * only File Location Resolver if none were registered before. Then 
	 * it creates a streaming device with scheduler type AK_SCHEDULER_DEFERRED_LINED_UP.
	 *
	 * @param in_deviceSettings		Device settings.
	 * @param in_bAsyncOpen			If true, files are opened asynchronously when possible.
	 * @return	AK_Success if initialization was successful, error code otherwise
	 */
	AKRESULT Init(
		const AkDeviceSettings &	in_deviceSettings
		);

	/**
	 * Terminate.
	 */
	void Term();

	void Update();

	//
	// IAkFileLocationAware interface.
	//-----------------------------------------------------------------------------

    /**
	 * Returns a file descriptor for a given file name (string).
	 *
	 * @param in_pszFileName	File name.
	 * @param in_eOpenMode		Open mode.
	 * @param in_pFlags			Special flags. Can pass NULL.
	 * @param io_bSyncOpen		If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
	 * @param out_fileDesc		Returned file descriptor.
	 * @return	AK_Success if operation was successful, error code otherwise
	 */
    virtual AKRESULT Open( 
        const AkOSChar*			in_pszFileName,		
		AkOpenMode				in_eOpenMode,		
        AkFileSystemFlags *		in_pFlags,			
		bool &					io_bSyncOpen,		
        AkFileDesc &			out_fileDesc        
        );

    /**
	 * Returns a file descriptor for a given file ID.
	 *
	 * @param in_fileID			File ID.
	 * @param in_eOpenMode		Open mode.
	 * @param in_pFlags			Special flags. Can pass NULL.
	 * @param io_bSyncOpen		If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
	 * @param out_fileDesc		Returned file descriptor.
	 * @return	AK_Success if operation was successful, error code otherwise
	 */
    virtual AKRESULT Open( 
        AkFileID				in_fileID,          // File ID.
        AkOpenMode				in_eOpenMode,       // Open mode.
        AkFileSystemFlags *		in_pFlags,			// Special flags. Can pass NULL.
		bool &					io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
        AkFileDesc &			out_fileDesc        // Returned file descriptor.
        );


	//
	// IAkIOHookDeferred interface.
	//-----------------------------------------------------------------------------

    /**
	 * Reads data from a file (asynchronous).
	 *
	 * @param in_fileDesc		File descriptor.
	 * @param in_heuristics		Heuristics for this data transfer.
	 * @param io_transferInfo	Asynchronous data transfer info.
	 * @return	AK_Success if operation was successful, error code otherwise
	 */
    virtual AKRESULT Read(
		AkFileDesc &			in_fileDesc,
		const AkIoHeuristics &	in_heuristics,
		AkAsyncIOTransferInfo & io_transferInfo
		);

    // Writes data to a file (asynchronous).
    virtual AKRESULT Write(
		AkFileDesc &			in_fileDesc,        // File descriptor.
		const AkIoHeuristics &	in_heuristics,		// Heuristics for this data transfer.
		AkAsyncIOTransferInfo & io_transferInfo		// Platform-specific asynchronous IO operation info.
		);

    /**
	 * Notifies that a transfer request is cancelled. It will be flushed by the streaming device when completed.
	 *
	 * @param in_fileDesc							File descriptor.
	 * @param io_transferInfo						Transfer info to cancel.
	 * @param io_bCancelAllTransfersForThisFile		Flag indicating whether all transfers should be cancelled for this file (see notes in function description).
	 */
    virtual void Cancel(
		AkFileDesc &			in_fileDesc,
		AkAsyncIOTransferInfo & io_transferInfo,
		bool & io_bCancelAllTransfersForThisFile
		);

	/**
	 * Cleans up a file.
	 *
	 * @param in_fileDesc		File descriptor.
	 * @return	AK_Success if operation was successful, error code otherwise
	 */
	virtual AKRESULT Close(
        AkFileDesc &			in_fileDesc
        );

    // Returns the block size for the file or its storage device. 
    virtual AkUInt32 GetBlockSize(
        AkFileDesc &  			in_fileDesc			// File descriptor.
        );

	// Returns a description for the streaming device above this low-level hook.
    virtual void GetDeviceDesc(
        AkDeviceDesc &  		out_deviceDesc      // Description of associated low-level I/O device.
        );
	
	// Returns custom profiling data: 1 if file opens are asynchronous, 0 otherwise.
	virtual AkUInt32 GetDeviceData();

	// Base path is prepended to all file names.
	// Audio source path is appended to base path whenever uCompanyID is AK and uCodecID specifies an audio source.
	// Bank path is appended to base path whenever uCompanyID is AK and uCodecID specifies a sound bank.
	// Language specific dir name is appended to path whenever "bIsLanguageSpecific" is true.
	AKRESULT SetBasePath(
		const FString&   in_pszBasePath
		);

	// String overload.
	// Returns AK_Success if input flags are supported and the resulting path is not too long.
	// Returns AK_Fail otherwise.
	AKRESULT GetFullFilePath(
		const FString&		in_szFileName,		// File name.
		AkFileSystemFlags * in_pFlags,			// Special flags. Can be NULL.
		AkOpenMode			in_eOpenMode,		// File open mode (read, write, ...).
		FString*			out_szFullFilePath	// Full file path.
		);

	// ID overload.
	// Returns AK_Success if input flags are supported and the resulting path is not too long.
	// Returns AK_Fail otherwise.
	AKRESULT GetFullFilePath(
		AkFileID			in_fileID,			// File ID.
		AkFileSystemFlags * in_pFlags,			// Special flags. Can be NULL.
		AkOpenMode			in_eOpenMode,		// File open mode (read, write, ...).
		FString*			out_szFullFilePath	// Full file path.
		);


protected:

	enum GlobalCallbackLocation_Constant { GlobalCallbackLocation = AkGlobalCallbackLocation_BeginRender, };

	/**
	 * Global callback to Sound Engine.
	 */
	static void GlobalCallback(AK::IAkGlobalPluginContext * in_pContext, AkGlobalCallbackLocation in_eLocation, void * in_pCookie);

	/** True if global callback was registered to the sound engine. */
	bool m_bCallbackRegistered;
	
	/** Lists possible states used by Thread-safe counter. */
	enum EAkIOState
	{
		// There are no pending requests/ all requests have been fulfilled.
		IOState_ReadyFor_Requests = -1,

		// Initial request has completed and finalization needs to be kicked off.
		IOState_ReadyFor_Callback = 0,

		// We're currently loading in data.
		IOState_InProgress_Loading = 1,
	};

	struct AkDeferredReadInfo
	{
		AkDeferredReadInfo()
			: State(IOState_ReadyFor_Requests)
		{
			RequestIndex = 0;
		}

#if AK_SUPPORTS_EVENT_DRIVEN_LOADING
		bool SetupRequest(IAsyncReadFileHandle* IORequestHandle, AkAsyncIOTransferInfo& transferInfo)
		{
			AsyncReadRequest = IORequestHandle->ReadRequest(transferInfo.uFilePosition, transferInfo.uRequestedSize, AIOP_High, &AsyncFileCallBack, (uint8*)transferInfo.pBuffer);
			if (!AsyncReadRequest)
			{
				SetState(IOState_ReadyFor_Requests);
				return false;
			}

			return true;
		}
#endif

		AkAsyncIOTransferInfo AkTransferInfo;
		FThreadSafeCounter State;

		void SetState(const EAkIOState& state) { State.Set(state); }
		bool IsInState(const EAkIOState& state) const { return State.GetValue() == state; }

		union
		{
			uint64 RequestIndex;

#if AK_SUPPORTS_EVENT_DRIVEN_LOADING
			IAsyncReadRequest* AsyncReadRequest;
#endif
		};

		void CallAkTransferInfoCallback()
		{
			if (AkTransferInfo.pCallback)
				AkTransferInfo.pCallback(&AkTransferInfo, AK_Success);

			SetState(IOState_ReadyFor_Requests);
		}

	private:
#if AK_SUPPORTS_EVENT_DRIVEN_LOADING
		FAsyncFileCallBack AsyncFileCallBack = [this](bool bWasCancelled, IAsyncReadRequest* Req) { CallAkTransferInfoCallback(); };
#endif
	};

	/** Get a free index in the pending transfer arrays
	 *
	 * @return An index pointer to a free entry. -1 if no free entry. Not thread-safe.
	 */
	AkDeferredReadInfo* GetFreeTransfer();

	/**
	 * Find a transfer, using the custom param pointer as key, in the pending transfer arrays
	 * @param pCustomParam	The custom param pointer for the file we are searching
	 * @return The index of the transfer. -1 if not found. Not thread-safe.
	 */
	AkDeferredReadInfo* FindTransfer(void* pBuffer);

	/** Array for pending transfers. */
	AkDeferredReadInfo aPendingTransfers[AK_UNREAL_MAX_CONCURRENT_IO];

	/** Critical section used to synchronize access to pending transfers array */
	FCriticalSection CriticalSection;

	// Base path, where the SoundBanks are located.
	FString			m_szBasePath;

private:
	/** 
	 * Reset the file descriptor
	 *
	 * @param out_fileDesc The file descriptor
	 */
	void CleanFileDescriptor( AkFileDesc& out_fileDesc );

	/**
	 * Perform the open, whether we have a file ID or a file Name
	 *
	 * @param in_fileDescriptor		file ID or file Name
	 * @param in_eOpenMode			Open mode.
	 * @param in_pFlags				Special flags. Can pass NULL.
	 * @param io_bSyncOpen			If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
	 * @param out_fileDesc			Returned file descriptor.
	 * @return AK_Success if operation was successful, error code otherwise
	 */
	template<typename T>
	AKRESULT PerformOpen( 
		T				in_fileDescriptor,  // Can be file ID or file Name
		AkOpenMode      in_eOpenMode,       // Open mode.
		AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
		bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
		AkFileDesc &    out_fileDesc        // Returned file descriptor.
		);

	AkDeviceID			m_deviceID;
	bool				m_bAsyncOpen;	// If true, opens files asynchronously when it can.

};
