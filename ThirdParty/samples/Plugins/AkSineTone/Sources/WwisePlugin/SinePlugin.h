//////////////////////////////////////////////////////////////////////
//
// SinePlugin.h
//
// Sine tone Wwise plugin implementation.
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <AK/Wwise/AudioPlugin.h>

class SinePlugin
	: public AK::Wwise::DefaultAudioPluginImplementation
{
public:
	SinePlugin();
	~SinePlugin();

	// AK::Wwise::IPluginBase
	virtual void Destroy();

	// AK::Wwise::IAudioPlugin
	virtual void SetPluginPropertySet( AK::Wwise::IPluginPropertySet * in_pPSet );

	virtual HINSTANCE GetResourceHandle() const;
	virtual bool GetDialog( eDialog in_eDialog, UINT & out_uiDialogID, AK::Wwise::PopulateTableItem *& out_pTable ) const;

    virtual bool GetBankParameters( const GUID & in_guidPlatform, AK::Wwise::IWriteData* in_pDataWriter ) const;

	virtual bool Help( HWND in_hWnd, eDialog in_eDialog, LPCWSTR in_szLanguageCode ) const;

	virtual bool GetSourceDuration( double& out_dblMinDuration, double& out_dblMaxDuration ) const;
private:
	AK::Wwise::IPluginPropertySet * m_pPSet;
};
