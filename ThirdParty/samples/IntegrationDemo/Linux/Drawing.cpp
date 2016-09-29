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
#include "FreetypeGraphicRenderer.h"
#include <string>

#include <SDL2/SDL.h>

using std::string;

/////////////////////////
//  GLOBAL OBJECTS
/////////////////////////

static FreetypeGraphicRenderer* g_pFreetypeRenderer = NULL;
static SDL_Window *win = NULL;
static SDL_Renderer *renderer = NULL;
       
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
	if (g_pFreetypeRenderer)
		g_pFreetypeRenderer->BeginDrawing();
}

void DoneDrawing()
{
	if (g_pFreetypeRenderer)
	{
		g_pFreetypeRenderer->DoneDrawing();

		// Then Mix in the SDL Video Buffer:

		int depth = 32; // 32 bit
		
		SDL_Surface * frameSurface = SDL_CreateRGBSurfaceFrom(
			(void *)g_pFreetypeRenderer->GetWindowBuffer(), //pixelBufferData
			g_pFreetypeRenderer->GetWindowWidth(),
			g_pFreetypeRenderer->GetWindowHeight(),
			depth, //depth
			g_pFreetypeRenderer->GetWindowWidth()*(depth/8),  //pitch
			0x00FF0000, // Red Mask
			0x0000FF00, // Green Mask
			0x000000FF, // Blue Mask
			0x00000000);// Alpha Mask

		if (frameSurface)
		{
			SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer, frameSurface);
			SDL_FreeSurface(frameSurface); //Unclear why it is causing random crash to call it.

			if (pTexture)
			{
				int ret = SDL_RenderCopy(renderer, pTexture, NULL, NULL);
				if (ret != 0)
					printf("SDL_RenderCopy Error! SDL Error: %s\n", SDL_GetError());
				else
					SDL_RenderPresent(renderer);
				SDL_DestroyTexture(pTexture);
			}
		}
	}
}

bool InitDrawing(
	void* in_pParam,
	AkOSChar* in_szErrorBuffer,
	unsigned int in_unErrorBufferCharCount,
	AkUInt32 in_windowWidth,
	AkUInt32 in_windowHeight
)
{
	g_pFreetypeRenderer = new FreetypeGraphicRenderer;

	bool bRet = g_pFreetypeRenderer->InitDrawing(in_pParam,
		in_szErrorBuffer,
		in_unErrorBufferCharCount,
		in_windowWidth,
		in_windowHeight);

	if( !bRet )
	{
		return bRet;
	}

	g_iWidth = in_windowWidth;
	g_iHeight = in_windowHeight;

	SDL_Init(SDL_INIT_VIDEO);

	win = SDL_CreateWindow("Integration Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, in_windowWidth, in_windowHeight, 0);
	if (win == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
		if (renderer == NULL)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		}
	}
	return true;
}

void DrawTextOnScreen( const char* in_szText, int in_iXPos, int in_iYPos, DrawStyle in_eDrawStyle )
{
	if (g_pFreetypeRenderer)
		g_pFreetypeRenderer->DrawTextOnScreen(in_szText, in_iXPos , in_iYPos , in_eDrawStyle);
}

void TermDrawing()
{
	if (g_pFreetypeRenderer != NULL)
	{
		g_pFreetypeRenderer->TermDrawing();
		delete g_pFreetypeRenderer;
		g_pFreetypeRenderer = NULL;
	}
}

int GetLineHeight( DrawStyle in_eDrawStyle )
{
	if (g_pFreetypeRenderer == NULL)
		return 0;

	return g_pFreetypeRenderer->GetLineHeight(in_eDrawStyle);
}
