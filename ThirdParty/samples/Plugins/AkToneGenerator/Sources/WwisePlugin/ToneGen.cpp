//////////////////////////////////////////////////////////////////////
//
// ToneGen.cpp
//
// Defines the initialization routines for the DLL.
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <AK/Wwise/Utilities.h>
#include "ToneGen.h"
#include "ToneGenPlugin.h"
#include <AK/Tools/Common/AkAssert.h>
#include <AK/Plugin/AkToneSourceFactory.h>

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CToneGenApp
BEGIN_MESSAGE_MAP(CToneGenApp, CWinApp)
END_MESSAGE_MAP()

// CToneGenApp construction
CToneGenApp::CToneGenApp()
{
	// Place all significant initialization in InitInstance
}


// The one and only CToneGenApp object
CToneGenApp theApp;


// CToneGenApp initialization
BOOL CToneGenApp::InitInstance()
{
	__super::InitInstance();
	AK::Wwise::RegisterWwisePlugin();
	return TRUE;
}

/////////////// DLL exports ///////////////////

// Plugin creation
AK::Wwise::IPluginBase* __stdcall AkCreatePlugin( unsigned short in_usCompanyID, unsigned short in_usPluginID )
{
	return new ToneGenPlugin(in_usPluginID);
}

/// Dummy assert hook for Wwise plug-ins using AKASSERT (cassert used by default).
DEFINEDUMMYASSERTHOOK;
DEFINE_PLUGIN_REGISTER_HOOK;