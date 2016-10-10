#ifndef _OCULUSSPATIALIZER_H_
#define _OCULUSSPATIALIZER_H_

#include <AK/SoundEngine/Common/IAkPlugin.h>

const unsigned long AKEFFECTID_OCULUS							= 266;
const unsigned long AKEFFECTID_OCULUS_SPATIALIZER				= 146;
const unsigned long AKEFFECTID_OCULUS_SPATIALIZER_ATTACHMENT	= 501;
/*
//
// Initializing the Oculus Spatializer Plugin dll (preferred method) -  
//  The following code should be added after AK SoundEngine initialization 
//  and loading of built-in plugins, but before loading any sound banks.
// 

//---------------------------------------------------------------------
// OCULUS_START
//---------------------------------------------------------------------

// Load Oculus Spatializer dll plug-in
HMODULE OculusSpatializerLibrary = LoadLibrary(L"OculusSpatializerWwise.dll");

// Successful?
if(OculusSpatializerLibrary)
{
    typedef bool(__stdcall *AkGetSoundEngineCallbacksType)
        (unsigned short in_usCompanyID,
        unsigned short in_usPluginID,
        AkCreatePluginCallback& out_funcEffect,
        AkCreateParamCallback&  out_funcParam);

    AkGetSoundEngineCallbacksType AkGetSoundEngineCallbacks =
        (AkGetSoundEngineCallbacksType)(void*)GetProcAddress(OculusSpatializerLibrary,"AkGetSoundEngineCallbacks");

    if(AkGetSoundEngineCallbacks)
    {
        AkCreatePluginCallback CreateOculusFX;
        AkCreateParamCallback  CreateOculusFXParams;

        // Register plugin effect
        if(AkGetSoundEngineCallbacks(AKEFFECTID_OCULUS,AKEFFECTID_OCULUS_SPATIALIZER,CreateOculusFX,CreateOculusFXParams))
        {
            if(AK::SoundEngine::RegisterPlugin(AkPluginTypeMixer,AKEFFECTID_OCULUS,AKEFFECTID_OCULUS_SPATIALIZER,CreateOculusFX,CreateOculusFXParams) != AK_Success)
            {
                printf("Failed to register OculusSpatializer plugin.");
            }
        }
        else
        {
            printf("Failed call to AkGetSoundEngineCallbacks in OculusSpatializer.dll");
        }

        // Register plugin attachment (for data attachment on individual sounds, like frequency hints etc.)
        if(AkGetSoundEngineCallbacks(AKEFFECTID_OCULUS,AKEFFECTID_OCULUS_SPATIALIZER_ATTACHMENT,CreateOculusFX,CreateOculusFXParams))
        {
            if(AK::SoundEngine::RegisterPlugin(AkPluginTypeEffect,AKEFFECTID_OCULUS,AKEFFECTID_OCULUS_SPATIALIZER_ATTACHMENT,NULL,CreateOculusFXParams) != AK_Success)
            {
                printf("Failed to register OculusSpatializer attachment.");
            }
        }
        else
        {
            printf("Failed call to AkGetSoundEngineCallbacks in OculusSpatializer.dll");
        }
    }
    else
    {
        printf("Failed to load functions AkGetSoundEngineCallbacks in OculusSpatializer.dll");
    }
}
else
{
    printf("Failed to load OculusSpatializer.dll");
}
//---------------------------------------------------------------------
// OCULUS_END
//---------------------------------------------------------------------

*/

#endif // _OCULUS_SPATIALIZER_H_

