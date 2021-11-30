#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

#include <genesis.h>
#include "PadHelper.h"

typedef struct
{
	int X;
	int Y;
}Point;

struct GridSquare
{
	int Player;
	int PreviousPlayer;

	int Size;
	int GrowSize;
	int Changed;
	int MaxSize;
	int Explode;

	int Animate;
	int Frame;
	int FrameCounter;
} m_PlayerGrid[70];

typedef struct
{
	int Start;
	int Count;
	int FrameRate;
	int Loop;
}anim;


extern anim m_Animations[];
extern u16 m_AtomTileStart;


extern int m_CursorX;
extern int m_CursorY;


extern Sprite* m_Cursor;


void HideCursor();
void UpdateCursor();

void GridSetup();



void IncrementSquare(int x, int y, int player);


int TryIncrementSquare(int x, int y, int player);

int PlayerAtSquare(int x, int y);

int SizeAtSquare(int x, int y);





void DrawAtom(int player, int x, int y, int size, int gridSquare);
void DrawFullGrid();
void DrawGameGrid();


// quick helper to half the value of the passed in colour
#define FADE_COLOUR(x)  (((VDPPALETTE_REDMASK & x) / 2) | ((VDPPALETTE_BLUEMASK & x) / 2) | ((VDPPALETTE_GREENMASK & x) / 2 ))
extern u16 m_RegularPalette[];
extern u16 m_FadedPalette[];

void SetupFadedPalette();


#endif