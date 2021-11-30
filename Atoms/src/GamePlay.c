#include <genesis.h>
#include "../inc/GamePlay.h"
#include "../res/gfx.h"
#include <kdebug.h>

u16 m_AtomTileStart = 0;

int m_CursorX = 0;
int m_CursorY = 0;


Sprite* m_Cursor = 0;


#define TileCount 1344
u16 m_DrawGrid[TileCount];

anim m_Animations[] =
{
	{ 1,4,2,0 }, // Grow to 1
	{ 4,4,2,0 }, // Grow to 2
	{ 8,4,2,0 }, // Grow to 3

	{ 13,5,2,1 }, // Idle 1
	{ 19,5,2,1 }, // Idle 2

	{ 25,4,3,1 }, // Crit 1
	{ 29,4,3,1 }, // Crit 2	
	{ 33,4,3,1 }, // Crit 3

	{ 37,4,2,0 } // Explode
};


u16 m_RegularPalette[64];
u16 m_FadedPalette[64];

void SetupFadedPalette()
{
	for (int i = 0; i < 64; i++)
	{
		if (i < 12 || i > 16)
		{
			m_FadedPalette[i] = FADE_COLOUR(m_RegularPalette[i]);
		}
		else
		{
			m_FadedPalette[i] = m_RegularPalette[i];
		}
	}
}


void GridSetup()
{
	int i = 0;
	int y = 0;
	int x = 0;
	for (y = 0; y<7; y++)
	{
		for (x = 0; x<10; x++)
		{
			m_PlayerGrid[i].Player = 0;

			if (y == 0 && x == 0)
			{
				m_PlayerGrid[i].MaxSize = 2;
			}
			else if (y == 0 && x == 9)
			{
				m_PlayerGrid[i].MaxSize = 2;
			}
			else if (y == 0)
			{
				m_PlayerGrid[i].MaxSize = 3;
			}
			else if (y == 6 && x == 0)
			{
				m_PlayerGrid[i].MaxSize = 2;
			}
			else if (x == 0)
			{
				m_PlayerGrid[i].MaxSize = 3;
			}
			else if (y == 6 && x == 9)
			{
				m_PlayerGrid[i].MaxSize = 2;
			}
			else if (y == 6)
			{
				m_PlayerGrid[i].MaxSize = 3;
			}
			else if (x == 9)
			{
				m_PlayerGrid[i].MaxSize = 3;
			}
			else
			{
				m_PlayerGrid[i].MaxSize = 4;
			}

			i++;
		}
	}

	memset(m_DrawGrid, 0, TileCount << 1);
}


void UpdateCursor()
{
	SPR_setPosition(m_Cursor, (m_CursorX * 24) + 56, (m_CursorY * 24) + 24);
}





void IncrementSquare(int x, int y, int player)
{
	int square = (y * 10) + x;

	m_PlayerGrid[square].GrowSize++;
	m_PlayerGrid[square].Changed = 1;
	m_PlayerGrid[square].Player = player;
}


int TryIncrementSquare(int x, int y, int player)
{	
	int square = (y * 10) + x;

	if (m_PlayerGrid[square].Player == player || m_PlayerGrid[square].Player == 0)
	{
		IncrementSquare(x, y, player);
		return 1;
	}

	return 0;
}

int PlayerAtSquare(int x, int y)
{
	return m_PlayerGrid[(y * 10) + x].Player;
}

int SizeAtSquare(int x, int y)
{
	return m_PlayerGrid[(y * 10) + x].Size;
}

void HideCursor()
{
	SPR_setPosition(m_Cursor, -32, -32);
}



void DrawAtom(int player, int x, int y, int size, int gridSquare)
{
	if (player < 0)
	{
		player = 0;
		size = 0;
	}

	int startingFrame = 0;
	int animate = m_PlayerGrid[gridSquare].Animate;
	if (animate >= 0)
	{
		anim* details = &m_Animations[animate];
		m_PlayerGrid[gridSquare].FrameCounter++;

		if (m_PlayerGrid[gridSquare].FrameCounter > details->FrameRate)
		{
			m_PlayerGrid[gridSquare].Frame++;
			m_PlayerGrid[gridSquare].FrameCounter = 0;
		}

		if (m_PlayerGrid[gridSquare].Frame >= details->Count)
		{
			if (details->Loop)
			{
				m_PlayerGrid[gridSquare].Frame = 0;
			}
			else
			{
				m_PlayerGrid[gridSquare].Animate = -1;
				m_PlayerGrid[gridSquare].Frame = details->Count - 1;
			}
		}
		startingFrame = (details->Start + m_PlayerGrid[gridSquare].Frame);
	}
	else
	{
		if (size == 4)
		{
			startingFrame = 39;
		}
		else
		{
			startingFrame = size * 4;
		}
	}

	u16 tileMapIndex = (startingFrame * 3 * 6 * 3) + (player * 3);
	u16 gridLocation = ((y * 3) << 6) + (x * 3);
	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 3; column++)
		{
			u16 tile = m_AtomTileStart;
			if (size != 0)
			{
				tile += atoms.map->tilemap[tileMapIndex];
			}
			m_DrawGrid[gridLocation] = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, tile);

			gridLocation++;
			tileMapIndex++;
		}
		gridLocation += 61;
		tileMapIndex += 15;
	}
}


void DrawFullGrid()
{
	int x = 0;
	int y = 0;

	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 10; x++)
		{
			int maxSize = m_PlayerGrid[(y * 10) + x].MaxSize;

			if (maxSize == 4)
				maxSize = 5;

			int rnd = (((y + x)* y) + 5 / (x + 1)) % (6);
			DrawAtom(rnd, x, y, maxSize, (y * 10) + x);
		}
	}
}


void DrawGameGrid()
{
	int x = 0;
	int y = 0;

	int playerGridLocation = 0;
	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 10; x++)
		{		
			int size = m_PlayerGrid[playerGridLocation].Size;
			if (size == 5)
			{
				size = 4;
			}
			DrawAtom(m_PlayerGrid[playerGridLocation].Player - 1, x, y, size, playerGridLocation);
			
			playerGridLocation++;
		}
	}

	VDP_setTileMapData(VDP_PLAN_A, m_DrawGrid, 0, TileCount, DMA);	
}