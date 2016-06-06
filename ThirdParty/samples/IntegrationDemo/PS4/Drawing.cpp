// Drawing.cpp
// Copyright (C) 2010 Audiokinetic Inc 
/// \file 
/// Implements the Drawing.h functions for the Vita platform.

/////////////////////////
//  INCLUDES
/////////////////////////

#include "stdafx.h"
#include "Drawing.h"
#include "Platform.h"
#include <libdbg.h>

#include "SampleApplication.h"

using std::string;

// "Default" draw colour: White
#define DEFAULT_DRAW_COLOUR vecmath::Vector4(1.0, 1.0, 1.0, 1.0)

// "Selected" draw colour: Yellow
#define SELECTED_DRAW_COLOUR vecmath::Vector4(1.0, 1.0, 0.0, 1.0)
//0xff00ffff

// "Title" draw colour: White
#define TITLE_DRAW_COLOUR vecmath::Vector4(1.0, 1.0, 1.0, 1.0)

// "Error" draw colour: Red
#define ERROR_DRAW_COLOUR vecmath::Vector4(1.0, 0.0, 0.0, 1.0)
//0xff0000ff

/////////////////////////
//  UTILS
/////////////////////////
extern PS4SampleApplication g_application;

/*namespace vecmath = sce::Vectormath::Simd::Aos;
namespace ssg = sce::SampleUtil::Graphics;

//ssg::GraphicsContextOption m_graphicsOption;
sce::SampleUtil::Graphics::GraphicsContext m_graphicsContext;
ssg::SpriteRenderer m_sprite;
static const int DISPLAY_WIDTH  = 1920;
static const int DISPLAY_HEIGHT = 1080;
Camera m_camera;*/

/////////////////////////
//  FUNCTIONS
/////////////////////////

static AkUInt32 g_iWidth;
static AkUInt32 g_iHeight;

AkUInt32 GetWindowWidth()
{
	return g_iWidth;
}

AkUInt32 GetWindowHeight()
{
	return g_iHeight;
}


bool InitDrawing(
	void* in_pParam,
	AkOSChar* in_szErrorBuffer,
	unsigned int in_unErrorBufferCharCount,
	AkUInt32 in_windowWidth,			///< - window widht in pixels
	AkUInt32 in_windowHeight			///< - window height in pixels
)
{
	g_iWidth = in_windowWidth;
	g_iHeight = in_windowHeight;


	/*int returnCode = AK::DrawingUtility::Init();
	if ( returnCode != SCE_OK )
	{
		__AK_OSCHAR_SNPRINTF( in_szErrorBuffer, in_unErrorBufferCharCount, AKTEXT("initDrawing() returned %d"), returnCode );
		return false;
	}

	// Blue background by default
	AK::DrawingUtility::SetBackgroundColor( 0x002864FF );*/


	/*int ret;
	(void)ret;
		
	ret = initializeUtil(FUNCTION_FLAG_ALL, in_windowWidth, in_windowHeight);
	assert(ret == SCE_OK);

	m_sprite.initialize(&m_graphicsContext);
	m_sprite.setRenderTargetSize(vecmath::Vector2(in_windowWidth, in_windowHeight));

	m_camera.reset();*/
	

	return true;
}


void BeginDrawing()
{
	g_application.BeginScene();
}


void DoneDrawing()
{
	g_application.EndScene();
}


void DrawTextOnScreen( const char* in_szText, int in_iXPos, int in_iYPos, DrawStyle in_eDrawStyle )
{
	// Transform from window coordinates to screen coordinates
	float x = (float)in_iXPos / g_iWidth;
	float y = (float)in_iYPos / g_iHeight;

	// Replace the tags in the text
	string translated = in_szText;
	ReplaceTags( translated );
		
	// Set colour based on the desired DrawStyle
	vecmath::Vector4 colour;
	switch ( in_eDrawStyle )
	{
	case DrawStyle_Title:
		colour = TITLE_DRAW_COLOUR;
		break;
	case DrawStyle_Selected:
		colour = SELECTED_DRAW_COLOUR;
		break;
	case DrawStyle_Error:
		colour = ERROR_DRAW_COLOUR;
		break;
	case DrawStyle_Text:
		colour = DEFAULT_DRAW_COLOUR;
		break;
	case DrawStyle_Control:
	default:
		colour = DEFAULT_DRAW_COLOUR;
		break;
	}

	g_application.DrawString(vecmath::Vector2(x, y), colour, translated.c_str());
}

void TermDrawing()
{
	//AK::DrawingUtility::Shutdown();
}


int GetLineHeight( DrawStyle in_eDrawStyle )
{
	return 30;
}


