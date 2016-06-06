// InputMgr.h
// Copyright (C) 2010 Audiokinetic Inc 
/// \file 
/// Contains the declaration of the Vita Input Manager class.
/// This class handles the relaying of input to the Menu System.

#pragma once

#include "UniversalInput.h"
#include <stdint.h>
//#include "sample_utilities/input.h"
#include <user_service\user_service_defs.h>
#include <pad.h>

/// Vita-Specific Input Manager class that handles all input devices and relays input
/// data to the Menu system.
class InputMgr
{
public:

	/// InputMgr constructor
	InputMgr();

	/// InputMgr destructor
	~InputMgr();

	/// Initializes the Input Manager object.
	/// \return True if the Input Manager has been properly initialized and false otherwise.
	bool Init( 
		void* in_pParam,						///< - Extra parameter, in this case for the application's main window handle
		AkOSChar* in_szErrorBuffer,				///< - Buffer where error details will be written (if the function returns false)
		unsigned int in_unErrorBufferCharCount	///< - Number of characters available in in_szErrorBuffer, including terminating NULL character
		);

	/// Accessor method for the UniversalInput object.
	/// \return A pointer to the UniversalInput object being used by the Input Manager.
	UniversalInput* UniversalInputAdapter() const;

	/// Updates the input from all devices and relays the new input states to the Menu system.
	/// \remark This method should be called every frame before the Menu system's Update call.
	void Update();

	/// Ends the Input Manager class and releases all used resources.
	void Release();

	int AddDevice( int index, SceUserServiceUserId in_guidInstance );
	bool RemoveDevice( int index );

	UniversalInput* GetUInput(){ return m_pUInput; }

	AkUInt32 GetUserId(AkUInt8 in_iPlayerIndex) const; 
private:

	void UpdateConnectedDevices();
	void UpdatePadData();

	/// Controller state data
	// CtrlUtilData m_ctrlData;

	/// Retrieves input state from the input device then translates it to a Button State 
	/// usable by the Universal Input class and sets it.
	void TranslateInput(ScePadData* in_PadData, int in_deviceIndex);

	/// Pointer to the Universal Input object to interface with the Menu System
	/// \sa UniversalInput
	UniversalInput* m_pUInput;
//	int m_DefaultUserPad;
	ScePadData m_UserPadData[SCE_USER_SERVICE_MAX_LOGIN_USERS];

	SceUserServiceUserId m_LoggedUserId[SCE_USER_SERVICE_MAX_LOGIN_USERS]; 
};
