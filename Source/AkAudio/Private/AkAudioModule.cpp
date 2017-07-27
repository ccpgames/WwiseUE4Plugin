// Copyright (c) 2006-2012 Audiokinetic Inc. / All Rights Reserved

#include "AkAudioDevice.h"
#include "AkAudioModule.h"

#if UE_4_14_OR_LATER
#include "Misc/CoreDelegates.h"
#endif
IMPLEMENT_MODULE( FAkAudioModule, AkAudio )

void FAkAudioModule::StartupModule()
{
	AkAudioDevice = new FAkAudioDevice;

	if (!AkAudioDevice)
		return;

	if (!AkAudioDevice->Init())
	{
		delete AkAudioDevice;
		AkAudioDevice = NULL;
		return;
	}

	OnTick = FTickerDelegate::CreateRaw( AkAudioDevice, &FAkAudioDevice::Update);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker( OnTick );

	FCoreDelegates::OnPreExit.AddLambda([]{FAkAudioDevice::SetEngineExiting(true);});

}

void FAkAudioModule::ShutdownModule()
{
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	if (AkAudioDevice) 
	{
		AkAudioDevice->Teardown();
		delete AkAudioDevice;
		AkAudioDevice = NULL;
	}
}

FAkAudioDevice * FAkAudioModule::GetAkAudioDevice()
{ 
	return AkAudioDevice;
}
