// DemoPositioningMenu.h
// Copyright (C) 2016 Audiokinetic Inc 
/// \file
/// Contains the declaration for the DemoDialogueMenu class.

#pragma once

#include "Page.h"

/// This class represents the Positioning Demos Menu page. This is simply a submenu to
/// link to various demonstrations involving dialogue.
class DemoPositioningMenu : public Page
{
public:

	/// DemoPositioningMenu class constructor
	DemoPositioningMenu(Menu& in_ParentMenu);

private:

	/// Initializes the controls on the page.
	virtual void InitControls();

	/// Delegate function for the "Localization Demo" button.
	void SinglePosition_Pressed( void* in_pSender, ControlEvent* in_pEvent );

	/// Delegate function for the "Dynamic Dialogue Demo" button.
	void Multiposition_Pressed( void* in_pSender, ControlEvent* in_pEvent );
};
