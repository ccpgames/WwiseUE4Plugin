// DemoPositioningMenu.cpp
// Copyright (C) 2016 Audiokinetic Inc
/// \file 
/// Defines the methods declared in DemoPositioningMenu.h.

#include "stdafx.h"

#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

#include "Menu.h"
#include "DemoPositioningMenu.h"
#include "DemoPositioning.h"


/////////////////////////////////////////////////////////////////////
// DemoDialogueMenu Public Methods
/////////////////////////////////////////////////////////////////////

DemoPositioningMenu::DemoPositioningMenu(Menu& in_ParentMenu) :Page(in_ParentMenu, "Positioning Demos Menu")
{

}


/////////////////////////////////////////////////////////////////////
// DemoPositioningMenu Private Methods
/////////////////////////////////////////////////////////////////////

void DemoPositioningMenu::InitControls()
{
	ButtonControl* newBtn;

	// Add button to open the localization demo page
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Position Demo" );
	newBtn->SetDelegate((PageMFP)&DemoPositioningMenu::SinglePosition_Pressed);
	m_Controls.push_back( newBtn );

	// Add button to open the dynamic dialogue demo page
	newBtn = new ButtonControl( *this );
	newBtn->SetLabel( "Multi-Position Demo" );
	newBtn->SetDelegate((PageMFP)&DemoPositioningMenu::Multiposition_Pressed);
	m_Controls.push_back( newBtn );
}

void DemoPositioningMenu::SinglePosition_Pressed(void*, ControlEvent*)
{
	DemoPositioning* pg = new DemoPositioning(*m_pParentMenu, false);
	m_pParentMenu->StackPage( pg );
}

void DemoPositioningMenu::Multiposition_Pressed(void*, ControlEvent*)
{
	DemoPositioning* pg = new DemoPositioning(*m_pParentMenu, true);
	m_pParentMenu->StackPage( pg );
}
