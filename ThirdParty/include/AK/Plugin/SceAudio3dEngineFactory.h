//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

// SceAudio3dMixerFactory.h

/// \file
/// Plug-in unique ID and creation functions (hooks) necessary to register the mixer plug-in in the sound engine.
/// <br><b>Wwise effect name:</b>  SCE Audio3d
/// <br><b>Library file:</b> SCEAudio3dMixer.lib

#ifndef _SCE_AUDIO_3D_MIXER_FACTORY_H_
#define _SCE_AUDIO_3D_MIXER_FACTORY_H_

#include <AK/SoundEngine/Common/IAkPlugin.h>

///
/// - This is the plug-in's unique ID (combined with the AKCOMPANYID_AUDIOKINETIC company ID)
/// - This ID must be the same as the plug-in ID in the plug-in's XML definition file, and is persisted in project files. 
/// \akwarning
/// Changing this ID will cause existing projects not to recognize the plug-in anymore.
/// \endakwarning
const unsigned long AKEFFECTID_SCE_AUDIO3D = 149;
const unsigned long AKEFFECTID_SCE_AUDIO3D_ATTACHMENT = 150;
const unsigned long AKEFFECTID_SCE_AUDIO3D_SINK = 151;
const unsigned long AKEFFECTID_SCE_AUDIO3D_SINK_EFFECT = 165;

/// Static creation function that returns an instance of the sound engine plug-in parameter node.
AK_FUNC( AK::IAkPluginParam *, CreateSceAudio3dMixerParams )(
	AK::IAkPluginMemAlloc * in_pAllocator			///< Memory allocator interface
	);

/// Static creation function that returns an instance of the sound engine plug-in.
AK_FUNC( AK::IAkPlugin*, CreateSceAudio3dMixer )(
	AK::IAkPluginMemAlloc * in_pAllocator			///< Memory allocator interface
	);

/// Static creation function that returns an instance of the sound engine plug-in attachment properties.
AK_FUNC( AK::IAkPluginParam*, CreateSceAudio3dAttachment )(
	AK::IAkPluginMemAlloc * in_pAllocator			///< Memory allocator interface
	);

/// Static creation function that returns an instance of the sound engine sink effect parameter node.
AK_FUNC( AK::IAkPluginParam *, CreateSceAudio3dSinkEffectParams )(
	AK::IAkPluginMemAlloc * in_pAllocator			///< Memory allocator interface
	);

/// Static creation function that returns an instance of the sound engine sink effect.
AK_FUNC( AK::IAkPlugin*, CreateSceAudio3dSinkEffect )(
	AK::IAkPluginMemAlloc * in_pAllocator			///< Memory allocator interface
	);

/// Static creation function that returns an instance of the sound engine sink plug-in.
AK_FUNC( AK::IAkPlugin*, CreateSceAudio3dSink )(
	AK::IAkPluginMemAlloc * in_pAllocator			///< Memory allocator interface
	);

/*
Use the following code to register your plug-in

// Register mixer.
AK::SoundEngine::RegisterPlugin( AkPluginTypeMixer, 
								 AKCOMPANYID_AUDIOKINETIC, 
								 AKEFFECTID_SCE_AUDIO3D,
								 CreateSceAudio3dMixer,
								 CreateSceAudio3dMixerParams );

// Register attachment properties.
AK::SoundEngine::RegisterPlugin( AkPluginTypeEffect, 
								 AKCOMPANYID_AUDIOKINETIC, 
								 AKEFFECTID_SCE_AUDIO3D_ATTACHMENT, 
								 NULL, 
								 CreateSceAudio3dAttachment );

// Register sink attachment properties.
AK::SoundEngine::RegisterPlugin( AkPluginTypeEffect, 
								 AKCOMPANYID_AUDIOKINETIC, 
								 AKEFFECTID_SCE_AUDIO3D_SINK_EFFECT, 
								 CreateSceAudio3dSinkEffect, 
								 CreateSceAudio3dSinkEffectParams );

// And pass CreateSceAudio3dSink to AkInitSettings::settingsMainOutput::pfSinkPluginFactory when initializing the sound engine.
AkInitSettings settings;
AK::SoundEngine::GetDefaultInitSettings( settings );
settings.settingsMainOutput.pfSinkPluginFactory = CreateSceAudio3dSink;
AK::SoundEngine::Init( &settings, &platformSettings );
*/

#endif // _SCE_AUDIO_3D_MIXER_FACTORY_H_

