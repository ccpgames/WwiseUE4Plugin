//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AkSoundEngineDLL.h"
#include <AK/Tools/Common/AkAssert.h>
#include <malloc.h>

#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include "AkDefaultIOHookBlocking.h"
#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>
#endif

#include <AK/Plugin/AkVorbisDecoderFactory.h>
#include <AK/Plugin/AkMeterFXFactory.h>


// Defines.

// Default memory manager settings.
#define COMM_POOL_SIZE			(256 * 1024)
#define COMM_POOL_BLOCK_SIZE	(48)

namespace AK
{
	void * AllocHook( size_t in_size )
	{
		return malloc( in_size );
	}
	void FreeHook( void * in_ptr )
	{
		free( in_ptr );
	}
	void * VirtualAllocHook(
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwAllocationType,
		DWORD in_dwProtect
		)
	{
		return VirtualAlloc( in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect );
	}
	void VirtualFreeHook( 
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwFreeType
		)
	{
		VirtualFree( in_pMemAddress, in_size, in_dwFreeType );
	}

#ifdef AK_XBOX360
	void * PhysicalAllocHook( 
		size_t in_size,					///< Number of bytes to allocate
		ULONG_PTR in_ulPhysicalAddress, ///< Parameter for XPhysicalAlloc
		ULONG_PTR in_ulAlignment,		///< Parameter for XPhysicalAlloc
		DWORD in_dwProtect				///< Parameter for XPhysicalAlloc
		)
	{
		return XPhysicalAlloc( in_size, in_ulPhysicalAddress, in_ulAlignment, in_dwProtect );
	}
	void PhysicalFreeHook( 
		void * in_pMemAddress	///< Pointer to start of memory allocated with PhysicalAllocHook
		)
	{
		XPhysicalFree( in_pMemAddress );
	}
#endif

#ifdef AK_XBOXONE
	void * APUAllocHook( 
		size_t in_size,				///< Number of bytes to allocate.
		unsigned int in_alignment	///< Alignment in bytes (must be power of two, greater than or equal to four).
		)
	{
		void * pReturn = nullptr;
//		ApuAlloc( &pReturn, NULL, (UINT32) in_size, in_alignment );
		return pReturn;
	}

	void APUFreeHook( 
		void * in_pMemAddress	///< Virtual address as returned by APUAllocHook.
		)
	{
		//ApuFree( in_pMemAddress );
	}
#endif

	namespace SOUNDENGINE_DLL
    {
        CAkDefaultIOHookBlocking m_lowLevelIO;

        //-----------------------------------------------------------------------------------------
        // Sound Engine initialization.
        //-----------------------------------------------------------------------------------------
        AKRESULT Init( 
            AkMemSettings *     in_pMemSettings,
            AkStreamMgrSettings * in_pStmSettings,
            AkDeviceSettings *  in_pDefaultDeviceSettings,
            AkInitSettings *    in_pSettings,
            AkPlatformInitSettings * in_pPlatformSettings,
			AkMusicSettings *	in_pMusicSettings
            )
        {
            // Check required arguments.
            if ( !in_pMemSettings ||
                 !in_pStmSettings ||
                 !in_pDefaultDeviceSettings )
            {
                AKASSERT( !"Invalid arguments" );
                return AK_InvalidParameter;
            }

            // Create and initialise an instance of our memory manager.
            if ( MemoryMgr::Init( in_pMemSettings ) != AK_Success )
            {
                AKASSERT( !"Could not create the memory manager." );
                return AK_Fail;
            }

            // Create and initialise an instance of the default stream manager.
			if ( !StreamMgr::Create( *in_pStmSettings ) )
            {
                AKASSERT( !"Could not create the Stream Manager" );
                return AK_Fail;
            }

            // Create an IO device.
			if ( m_lowLevelIO.Init( *in_pDefaultDeviceSettings ) != AK_Success )
            {
                AKASSERT( !"Cannot create streaming I/O device" );
                return AK_Fail;
            }
            
			// Initialize sound engine.
			if ( SoundEngine::Init( in_pSettings, in_pPlatformSettings ) != AK_Success )
            {
                AKASSERT( !"Cannot initialize sound engine" );
                return AK_Fail;
            }

			// Initialize music engine.
			if ( MusicEngine::Init( in_pMusicSettings ) != AK_Success )
            {
                AKASSERT( !"Cannot initialize music engine" );
                return AK_Fail;
            }

#ifndef AK_OPTIMIZED
			// Initialize communication.
			AkCommSettings settingsComm;
			AK::Comm::GetDefaultInitSettings( settingsComm );
			if ( AK::Comm::Init( settingsComm ) != AK_Success )
			{
				AKASSERT( !"Cannot initialize communication" );
				return AK_Fail;
			}
#endif // AK_OPTIMIZED

#ifdef AK_APPLE
			AK::SoundEngine::RegisterCodec(
				AKCOMPANYID_AUDIOKINETIC,
				AKCODECID_AAC,
				CreateAACFilePlugin,
				CreateAACBankPlugin);
#endif

#ifdef AK_VITA
			AK::SoundEngine::RegisterCodec(
				AKCOMPANYID_AUDIOKINETIC,
				AKCODECID_ATRAC9,
				CreateATRAC9FilePlugin,
				CreateATRAC9BankPlugin);
#endif			
		
			return AK_Success;
        }

        //-----------------------------------------------------------------------------------------
        // Sound Engine termination.
        //-----------------------------------------------------------------------------------------
        void Term( )
        {
#ifndef AK_OPTIMIZED
			Comm::Term();
#endif // AK_OPTIMIZED

			MusicEngine::Term();

			SoundEngine::Term();

			m_lowLevelIO.Term();
            if ( IAkStreamMgr::Get() )
            	IAkStreamMgr::Get()->Destroy();
            	
	        MemoryMgr::Term();
        }
      
        //-----------------------------------------------------------------------------------------
        // Sound Engine periodic update.
        //-----------------------------------------------------------------------------------------
		void Tick( )
		{
			SoundEngine::RenderAudio( );
		}

        //-----------------------------------------------------------------------------------------
        // Access to LowLevelIO's file localization.
        //-----------------------------------------------------------------------------------------
        // File system interface.
		AKRESULT SetBasePath(
			const AkOSChar*   in_pszBasePath
			)
		{
			return m_lowLevelIO.SetBasePath( in_pszBasePath );
		}
		AKRESULT SetBankPath(
			const AkOSChar*   in_pszBankPath
			)
		{
			return m_lowLevelIO.SetBankPath( in_pszBankPath );
		}
		AKRESULT SetAudioSrcPath(
			const AkOSChar*   in_pszAudioSrcPath
			)
		{
			return m_lowLevelIO.SetAudioSrcPath( in_pszAudioSrcPath );
		}		
	}	
}

#if defined AK_XBOX360 || defined AK_XBOXONE
BOOL APIENTRY DllMain( HANDLE hModule, 
	DWORD  ul_reason_for_call, 
	LPVOID lpReserved )
{
	switch ( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;

	}
	return TRUE;
}
#endif