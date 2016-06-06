// Drawing.cpp
// Copyright (C) 2013 Audiokinetic Inc
/// \file 
// Implements the Drawing.h functions for ncurses


/////////////////////////
//  INCLUDES
/////////////////////////

#include "stdafx.h"
#include "Platform.h"
#include "Drawing.h"
#include <ncurses.h>
#include <string>

using std::string;

#define DEFAULT_DRAW_COLOUR  0xFFFFFFFF	 // White
#define SELECTED_DRAW_COLOUR 0xFFFFFF00  // Yellow
#define TITLE_DRAW_COLOUR    0xFFFFFFFF	 // White
#define ERROR_DRAW_COLOUR	 0xFFFF0000  // Red

extern int g_lastCh;

/////////////////////////
//  GLOBAL OBJECTS
/////////////////////////

       
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

void BeginDrawing()
{
	clear();
}

void DoneDrawing()
{
	// Present
	refresh();
	g_lastCh = getch();
}

bool InitDrawing(
	void* in_pParam,
	AkOSChar* in_szErrorBuffer,
	unsigned int in_unErrorBufferCharCount,
	AkUInt32 in_windowWidth,
	AkUInt32 in_windowHeight
)
{
	g_iWidth = in_windowWidth;
	g_iHeight = in_windowHeight;

	initscr();				/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);	/* We get F1, F2 etc..		*/
	noecho();				/* Don't echo() while we do getch */
	nodelay( stdscr, TRUE );

	return true;

cleanup:
	TermDrawing();
	return false;
}


void DrawTextOnScreen( const char* in_szText, int in_iXPos, int in_iYPos, DrawStyle in_eDrawStyle )
{
	string buffer = in_szText;

	// Replace tags in the text
	ReplaceTags( buffer );

	if ( in_eDrawStyle == DrawStyle_Selected )
		attron( A_BOLD );
	else if ( in_eDrawStyle == DrawStyle_Title )
		attron( A_UNDERLINE );

	mvprintw( in_iYPos, in_iXPos, buffer.c_str() );

	if ( in_eDrawStyle == DrawStyle_Selected )
		attroff( A_BOLD );
	else if ( in_eDrawStyle == DrawStyle_Title )
		attroff( A_UNDERLINE );
}


void TermDrawing()
{
	endwin();
}


int GetLineHeight( DrawStyle in_eDrawStyle )
{
	return 1;
}
