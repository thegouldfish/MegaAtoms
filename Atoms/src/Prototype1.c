#include <genesis.h>
#include <kdebug.h>

#include "../inc/atoms.h"
#include "../inc/Prototype1.h"
#include "../inc/GamePlay.h"

#include "../inc/PadHelper.h"

#include "../res/gfx.h"
#include "../res/sprite.h"
#include "../res/sound.h"


#define SND_MOVE 67
#define SND_GROW 68
#define SND_EXPLODE 69

Sprite* m_AtomsSpr[6];
Sprite* m_NumbersSpr[12];


#define STUFF(X,Y,player, size)  (( X << 12 ) | ( Y << 8 ) | (player << 4) | size)

#define GET_X(x) ((x & 0xf000) >> 12)
#define GET_Y(x) ((x & 0x0f00) >> 8)
#define GET_PLAYER(x) ((x & 0x00f0) >> 4)
#define GET_SIZE(x) ((x & 0x000f))

typedef struct
{
	const char * name;	
	const u8 Colours[6];
	const u8 size;
	const u16 Data[];
} Puzzle;



const static Puzzle Level1 =
{
	"Level 1",
	{ 1,0,0,0,0,0 },
	3,	
	{STUFF(4,1,1,3),STUFF(5,1,2,3),STUFF(6,1,2,3) },
};


const static Puzzle * levels[] = { &Level1 };

const static Puzzle * m_CurrentPuzzle;

static int m_Colours[6];
static int m_CurrentColour = 0;

static void SetNumber(int number, int atom)
{
	if (number > 9)
	{
		int count = 0;

		while (number > 9)
		{
			number -= 10;
			count++;
		}


		SPR_setFrame(m_NumbersSpr[(atom * 2)], number);
		SPR_setFrame(m_NumbersSpr[(atom * 2) + 1], count);
		SPR_setVisibility(m_NumbersSpr[(atom * 2) + 1], VISIBLE);
	}
	else
	{
		SPR_setFrame(m_NumbersSpr[(atom * 2)], number);
		SPR_setVisibility(m_NumbersSpr[(atom * 2) + 1], HIDDEN);
	}
}



static void DrawAtom1(int player, int x, int y, int size)
{
	if (player < 0)
	{
		player = 0;
		size = 0;
	}

	int startingFrame = 0;

	/*
	int animate = m_PlayerGrid[(y * 10) + x].Animate;
	
	if (animate >= 0)
	{
		anim* details = &m_Animations[animate];
		m_PlayerGrid[(y * 10) + x].FrameCounter++;

		if (m_PlayerGrid[(y * 10) + x].FrameCounter > details->FrameRate)
		{
			m_PlayerGrid[(y * 10) + x].Frame++;
			m_PlayerGrid[(y * 10) + x].FrameCounter = 0;
		}

		if (m_PlayerGrid[(y * 10) + x].Frame >= details->Count)
		{
			if (details->Loop)
			{
				m_PlayerGrid[(y * 10) + x].Frame = 0;
			}
			else
			{
				m_PlayerGrid[(y * 10) + x].Animate = -1;
				m_PlayerGrid[(y * 10) + x].Frame = details->Count - 1;
			}
		}
		startingFrame = (details->Start + m_PlayerGrid[(y * 10) + x].Frame);
	}
	else
	*/
	{
		if (size == 4)
		{
			startingFrame = 41;
		}
		else
		{
			startingFrame = size * 4;
		}
	}

	u16 t = (startingFrame * 3 * 6 * 3) + (player * 3);
	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 3; column++)
		{
			u16 tile = m_AtomTileStart;
			if (size != 0)
			{
				tile += atoms.map->tilemap[t];
			}
			VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, tile), column + (x * 3), row + (y * 3));
			t++;
		}
		t += 15;
	}
}


static void DrawGameGrid1()
{
	int x = 0;
	int y = 0;

	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 10; x++)
		{
			int size = m_PlayerGrid[(y * 10) + x].Size;
			if (size == 5)
			{
				size = 4;
			}

			DrawAtom(m_PlayerGrid[(y * 10) + x].Player - 1, x, y, size);
		}
	}
}




void SetupLevel(int levelNumber)
{
	m_CurrentPuzzle = levels[levelNumber];
	m_CurrentColour = -1;

	for (int i = 0; i < 6; i++)
	{
		SetNumber(m_CurrentPuzzle->Colours[i], i);
		m_Colours[i] = m_CurrentPuzzle->Colours[i];
		if (m_CurrentColour == -1 && m_Colours[i] > 0)
		{
			m_CurrentColour = i;
		}
	}

	for (int i = 0; i < 70; i++)
	{
		m_PlayerGrid[i].Changed = 0;
		m_PlayerGrid[i].GrowSize = 0;
		m_PlayerGrid[i].Player = 0;
		m_PlayerGrid[i].Size = 0;
		m_PlayerGrid[i].Animate = -1;
		m_PlayerGrid[i].Frame = 0;
	}

	KDebug_Alert("size");
	KDebug_AlertNumber(m_CurrentPuzzle->size);


	for (int i = 0; i < m_CurrentPuzzle->size; i++)
	{
		int place = GET_X(m_CurrentPuzzle->Data[i]) + (GET_Y(m_CurrentPuzzle->Data[i]) * 10);
		m_PlayerGrid[place].Size = GET_SIZE(m_CurrentPuzzle->Data[i]);
		m_PlayerGrid[place].Player = GET_PLAYER(m_CurrentPuzzle->Data[i]);


		KDebug_Alert("place");
		KDebug_AlertNumber(i);
		KDebug_AlertNumber(place);
		KDebug_AlertNumber(m_PlayerGrid[place].Size);
		KDebug_AlertNumber(m_PlayerGrid[place].Player);

	}
}





void PlayerInputUpdate()
{


}





void Protype1ScreenStart()
{
	// disable interrupt when accessing VDP
	SYS_disableInts();

	// Clear anything left over from the previous state
	VDP_resetScreen();
	SPR_reset();


	// Set palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	int ind = TILE_USERINDEX;


	//VDP_setPalette(PAL0, ingame_back.palette->data);
	VDP_drawImageEx(PLAN_B, &prototype1_backing, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
	ind += prototype1_backing.tileset->numTile;

	SYS_enableInts();
	u16 palette[64];


	VDP_setPalette(PAL1, atoms.palette->data);
	VDP_loadTileSet(atoms.tileset, ind, CPU);

	m_AtomTileStart = ind;
	ind += atoms.tileset->numTile;

	VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, m_AtomTileStart), 0, 0, 40, 28);



	m_Cursor = SPR_addSprite(&Cursor, 0, 0, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
	SPR_setAnim(m_Cursor, 0);



	VDP_setVerticalScroll(PLAN_A, -28);
	VDP_setHorizontalScroll(PLAN_A, 60);


	for (int i = 0; i < 6; i++)
	{
		m_AtomsSpr[i] = SPR_addSprite(&AtomsSample, 0, 20 + (33 * i), TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setAnim(m_AtomsSpr[i], i);
	}

	int j = 0;
	int p = 0;
	for (int i = 0; i < 12; i+=2)
	{
		m_NumbersSpr[i] = SPR_addSprite(&Numbers, 20, 20 + (33 * j), TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		m_NumbersSpr[i+1] = SPR_addSprite(&Numbers, 36, 20 + (33 * j), TILE_ATTR(PAL1, FALSE, FALSE, FALSE));

		SPR_setFrame(m_NumbersSpr[i], p++);
		SPR_setFrame(m_NumbersSpr[i+1], p++);

		if (p >= 10)
		{
			p = 0;
		}

		j++;
	}


	// prepare palettes
	memcpy(&palette[0], ingame_back.palette->data, 16 * 2);
	memcpy(&palette[16], atoms.palette->data, 16 * 2);
	memcpy(&palette[32], profs.palette->data, 16 * 2);
	memcpy(&palette[48], robo_pal.palette->data, 16 * 2);

	GridSetup();

//	HideCursor();

//	SetupGame();


	XGM_setPCM(SND_MOVE, sound_3, sizeof(sound_3));
	XGM_setPCM(SND_GROW, sound_grow, sizeof(sound_grow));
	XGM_setPCM(SND_EXPLODE, sound_explode, sizeof(sound_explode));


	// fade in
	VDP_fadeIn(0, (4 * 16) - 1, palette, 20, FALSE);

	m_CursorX = 0;
	m_CursorY = 0;
	UpdateCursor();
	SetupLevel(0);
}


void ProtoIncrementSquare(int x, int y, int player)
{
	int square = (y * 10) + x;

	m_PlayerGrid[square].Size++;
	m_PlayerGrid[square].Changed = 1;
	m_PlayerGrid[square].Player = player;
}


int ProtoTryIncrementSquare(int x, int y, int player)
{
	int square = (y * 10) + x;

	if (m_PlayerGrid[square].Player == player || m_PlayerGrid[square].Player == 0)
	{
		ProtoIncrementSquare(x, y, player);
		return 1;
	}

	return 0;
}


#define PROTO1_SETUP 0
#define PROTO1_INPUT 1
#define PROTO1_READY 2
#define PROTO1_SOLVE 3
static int m_GameState = PROTO1_INPUT;


static void PlayerInput()
{
	if (m_Pad.Up == PAD_PRESSED)
	{
		m_CursorY--;
		XGM_startPlayPCM(SND_MOVE, 1, SOUND_PCM_CH2);
	}
	else if (m_Pad.Down == PAD_PRESSED)
	{
		m_CursorY++;
		XGM_startPlayPCM(SND_MOVE, 1, SOUND_PCM_CH2);
	}

	if (m_Pad.Left == PAD_PRESSED)
	{
		m_CursorX--;
		XGM_startPlayPCM(SND_MOVE, 1, SOUND_PCM_CH2);
	}
	else if (m_Pad.Right == PAD_PRESSED)
	{
		m_CursorX++;
		XGM_startPlayPCM(SND_MOVE, 1, SOUND_PCM_CH2);
	}


	if (m_CursorX > 9)
	{
		m_CursorX = 9;
	}
	else if (m_CursorX < 0)
	{
		m_CursorX = 0;
	}

	if (m_CursorY > 6)
	{
		m_CursorY = 6;
	}
	else if (m_CursorY < 0)
	{
		m_CursorY = 0;
	}

	if (m_Pad.A == PAD_RELEASED)
	{
		if (ProtoTryIncrementSquare(m_CursorX, m_CursorY, m_CurrentColour + 1))
		{
			m_Colours[m_CurrentColour]--;

			if (m_Colours[m_CurrentColour] == 0)
			{				
				int start = m_CurrentColour;
				for (int i = start + 1; i < 6; i++)
				{
					if (m_Colours[i] > 0)
					{
						m_CurrentColour = i;
					}
				}

				if (start == m_CurrentColour)
				{
					m_GameState = PROTO1_SOLVE;
				}
			}
		}
	}


	UpdateCursor();
}





void Protype1ScreenUpdate()
{
	switch (m_GameState)
	{
		case PROTO1_INPUT:
		{
			PlayerInput();
			break;
		}

		case PROTO1_SOLVE:
		{

			break;
		}

	default:
		break;
	}

	DrawGameGrid();
}


void Protype1ScreenEnd()
{


}


SimpleState Protype1ScreenState =
{
	Protype1ScreenStart,
	Protype1ScreenUpdate,
	Protype1ScreenEnd
};
