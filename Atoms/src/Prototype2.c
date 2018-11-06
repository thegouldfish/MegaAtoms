#include <genesis.h>
#include <kdebug.h>

#include "../inc/atoms.h"
#include "../inc/GamePlay.h"

#include "../inc/PadHelper.h"

#include "../res/gfx.h"
#include "../res/sprite.h"
#include "../res/sound.h"

#include "../inc/GameState.h"

#include "../inc/Prototype2GameOverState.h"
#include "../inc/ChallengeModeHighScoreState.h"
#include "../inc/ChallengeModeSetNameState.h"

#define SND_MOVE 67
#define SND_GROW 68
#define SND_EXPLODE 69

// Values that will be accessed from the gameover state
u32 m_Score = 0;
int m_CurrentLevel = 0;


// All the sprites we need
static Sprite* m_AtomsSpr[6];
static Sprite* m_NumbersSpr[12];

static Sprite* m_MultiplerSpr[2];

static Sprite* m_NextAtomsSpr;
static Sprite* m_CurrentAtomsSpr;



static int m_LevelNeeded = 2;


static fix32 m_InitalLevelingAmount = intToFix32(2);
static fix32 m_LevelBoosting = FIX32(1.25);
static fix32 m_LevelingAmount;


static int m_AtomNeededCount[7];

static int m_NextAtom;
static int m_CurrentAtom;

#ifdef DEBUG
static const fix32 m_InitalMaxTime = intToFix32(5);// intToFix32(2 * 40);
#else
static const fix32 m_InitalMaxTime =  intToFix32(2 * 40);
#endif
static const fix32 m_InitalTimePerExplosion = FIX32(2.0);


static fix32 m_Time;
static fix32 m_MaxTime;
static fix32 m_MaxTimeDecrease = FIX32(0.85);
static fix32 m_TimePerExplosion;
static fix32 m_TimePenaty = FIX32(0.60);


static float m_Multiplier = 1;
static float m_ScorePerExplotion = 1;

static Sprite* m_ScoreSpr[10];

static Sprite* m_MessageSpr;

static u16 m_BarColours[92];


// States
#define PROTO2_STATE_PLAY 0
#define PROTO2_STATE_ANIMATE 1
#define PROTO2_STATE_ANIMATE_WAIT 2
#define PROTO2_STATE_END_CHECK 3
#define PROTO2_STATE_LEVEL_UP 4
#define PROTO2_STATE_GAME_OVER 5
#define PROTO2_STATE_RESET 6
#define PROTO2_STATE_PAUSED 7




static int m_GameState = PROTO2_STATE_PLAY;


static int m_TimerBacking[3];
static int m_TimerAmounts[8 * 3];

static fix32 m_PrevTime;

static int m_Exploded = 0;
static int m_EarlyOut;

static void printGrid()
{
	return;
	char t[110];
	KDebug_Alert("PrintGrid");
	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			t[(x*10)+ 0] = (char)(48 + y);
			t[(x * 10) + 1] = ',';
			t[(x * 10) + 2] = (char)(48 + x);
			t[(x * 10) + 3] = (char)(48 + m_PlayerGrid[(y * 10) + x].Player);
			t[(x * 10) + 4] = (char)(48 + m_PlayerGrid[(y * 10) + x].Size);
			t[(x * 10) + 5] = (char)(48 + m_PlayerGrid[(y * 10) + x].Animate);
			t[(x * 10) + 6] = (char)(48 + m_PlayerGrid[(y * 10) + x].Changed);
			t[(x * 10) + 7] = (char)(48 + m_PlayerGrid[(y * 10) + x].Explode);
			t[(x * 10) + 8] = ' ';
			t[(x * 10) + 9] = ' ';			
		}
		t[100] = 0;
		KDebug_Alert(t);
	}
	KDebug_Alert("-----");

}



static void SetMultiplier()
{
	int number = m_Multiplier;
	if (number > 9)
	{
		int count = 0;

		while (number > 9)
		{
			number -= 10;
			count++;
		}


		SPR_setFrame(m_MultiplerSpr[0], count);
		SPR_setFrame(m_MultiplerSpr[1], number);
		SPR_setVisibility(m_MultiplerSpr[1], VISIBLE);
	}
	else
	{
		SPR_setFrame(m_MultiplerSpr[0], number);
		SPR_setVisibility(m_MultiplerSpr[1], HIDDEN);
	}
}


static void SetNumber(int number, int atomNumber)
{
	if (number > 9)
	{
		int count = 0;

		while (number > 9)
		{
			number -= 10;
			count++;
		}


		SPR_setFrame(m_NumbersSpr[(atomNumber * 2)+1], number);
		SPR_setFrame(m_NumbersSpr[(atomNumber * 2)], count);
		SPR_setVisibility(m_NumbersSpr[(atomNumber * 2) + 1], VISIBLE);
	}
	else
	{
		SPR_setFrame(m_NumbersSpr[(atomNumber * 2)], number);
		SPR_setVisibility(m_NumbersSpr[(atomNumber * 2) + 1], HIDDEN);
	}
}


static void SetScore(int number)
{
	int numbs[10] = { 0,0,0,0,0,0,0,0,0,0 };
	int i = 9;
	while (number > 0)
	{
		numbs[i] = (number % 10);
		number /= 10;
		i--;
	}

	for (int i = 0; i < 10; i++)
	{
		SPR_setFrame(m_ScoreSpr[i], numbs[i]);
	}
}





#define RandomBagCount 18
static int m_RandomBag[RandomBagCount];
static int m_CurrentRnd = 0;

static void SetupRandom(int prev)
{
	KDebug_Alert("SetupRandom:Start");
	for (int i = 0; i < RandomBagCount; i++)
	{
		m_RandomBag[i] = 0;
	}


	int r = RandomBagCount / 6;
	

	for (int k = 0; k < r; k++)
	{
		int place = k * 6;
		
		for (int i = 0; i < 6; i++)
		{
			while (1)
			{
				int rnd = random_int(1, 6);

				if (rnd != prev)
				{
					int count = 0;
					for (int j = 0; j < 6; j++)
					{
						if (m_RandomBag[place + j] == rnd)
						{
							count++;
						}
					}

					if (count == 0)
					{
						prev = rnd;
						//KDebug_AlertNumber(rnd);
						m_RandomBag[place + i] = rnd;
						break;
					}
				}
			}
		}
	}
	KDebug_Alert("SetupRandom:End");
}


static void AnimateScreen()
{
	u8 animating = 1;
	int x = 0;
	int y = 0;
	u8 allSame = 1;
	char lastPlayer = -1;
	char exploded = 0;
	char done = 0;
	char grow = 0;
	m_EarlyOut = 0;

	//while (animating)
	{
		lastPlayer = -1;
		allSame = 1;
		exploded = 0;
		grow = 0;
		done = 1;
		for (int j = 0; j < 70; j++)
		{
			if (m_PlayerGrid[j].GrowSize && m_PlayerGrid[j].Size != 5)
			{
				m_PlayerGrid[j].Changed = 1;
				if (m_PlayerGrid[j].GrowSize)
				{
					done = 0;
				}


				m_PlayerGrid[j].Size += m_PlayerGrid[j].GrowSize;

				if (m_PlayerGrid[j].Size > m_PlayerGrid[j].MaxSize)
				{
					m_PlayerGrid[j].GrowSize = m_PlayerGrid[j].Size - m_PlayerGrid[j].MaxSize;
					m_PlayerGrid[j].Size = m_PlayerGrid[j].MaxSize;
				}
				else
				{
					m_PlayerGrid[j].GrowSize = 0;
				}
				

				
				m_PlayerGrid[j].Animate = m_PlayerGrid[j].Size - 1;
			}
		}

		for (y = 0; y < 7; y++)
		{
			for (x = 0; x < 10; x++)
			{
				int i = (y * 10) + x;

				u16 size = m_PlayerGrid[i].Size;
				u16 player = m_PlayerGrid[i].Player;

				if (lastPlayer == -1 && player != 0)
				{
					lastPlayer = player;
				}
				else if (lastPlayer != player && player != 0)
				{
					allSame = 0;
				}

				if (m_PlayerGrid[i].Changed)
				{
					animating = 2;

					if (size == m_PlayerGrid[i].MaxSize)
					{
						//DrawSquare(x, y, 4, player, 1);
						m_PlayerGrid[i].Changed = 1;
						m_PlayerGrid[i].Size = 5;
						exploded = 1;
						m_PlayerGrid[i].Animate = 8;

						//char blah[30];
						//sprintf(blah, "anim %d - %d - %d", i, m_PlayerGrid[i].Animate, m_PlayerGrid[i].Size);
						//KDebug_Alert(blah);


						m_AtomNeededCount[m_PlayerGrid[i].Player]--;

						if (m_AtomNeededCount[m_PlayerGrid[i].Player] < 0)
						{
							m_AtomNeededCount[m_PlayerGrid[i].Player] = 0;
						}


						m_Time = fix32Add(m_Time, m_TimePerExplosion);

						m_Score += (int)(m_Multiplier * m_ScorePerExplotion);
						m_Exploded = 1;
					}
					else if (size == 5)
					{
						m_PlayerGrid[i].Size = 0;
						m_PlayerGrid[i].Player = 0;
						m_PlayerGrid[i].Changed = 0;
						// Clear it and just set the Changed flag

						// Do explosion logic!

						if (y == 0 && x == 0)
						{
							IncrementSquare(x + 1, y, player);
							IncrementSquare(x, y + 1, player);
						}
						else if (y == 0 && x == 9)
						{
							IncrementSquare(x - 1, y, player);
							IncrementSquare(x, y + 1, player);
						}
						else if (y == 0)
						{
							IncrementSquare(x + 1, y, player);
							IncrementSquare(x - 1, y, player);
							IncrementSquare(x, y + 1, player);
						}
						else if (y == 6 && x == 0)
						{
							IncrementSquare(x + 1, y, player);
							IncrementSquare(x, y - 1, player);
						}
						else if (x == 0)
						{
							IncrementSquare(x + 1, y, player);
							IncrementSquare(x, y - 1, player);
							IncrementSquare(x, y + 1, player);
						}
						else if (y == 6 && x == 9)
						{
							IncrementSquare(x - 1, y, player);
							IncrementSquare(x, y - 1, player);
						}
						else if (y == 6)
						{
							IncrementSquare(x + 1, y, player);
							IncrementSquare(x - 1, y, player);
							IncrementSquare(x, y - 1, player);
						}
						else if (x == 9)
						{
							IncrementSquare(x - 1, y, player);
							IncrementSquare(x, y - 1, player);
							IncrementSquare(x, y + 1, player);
						}
						else
						{
							IncrementSquare(x + 1, y, player);
							IncrementSquare(x, y - 1, player);
							IncrementSquare(x, y + 1, player);
							IncrementSquare(x - 1, y, player);
						}
					}
					else
					{
						if (size)
						{
							//DrawSquare(x, y, size, player, 0);
							m_PlayerGrid[i].Player = player;
							m_PlayerGrid[i].Changed = 0;
							grow = 1;
						}
					}
				}
			}
		}
		if (done)
		{
			animating--;
		}

		if (exploded)
		{
			XGM_startPlayPCM(SND_EXPLODE, 1, SOUND_PCM_CH4);
		}
		else if (grow)
		{
			XGM_startPlayPCM(SND_GROW, 1, SOUND_PCM_CH3);
		}

		// Allow for early out!
		if (allSame)
		{
			KDebug_Alert("Early out");
			animating = 0;
			m_EarlyOut = 1;
			m_GameState = PROTO2_STATE_END_CHECK;
		}

		if (!animating)
		{			
			m_GameState = PROTO2_STATE_END_CHECK;
		}
		else
		{
			m_GameState = PROTO2_STATE_ANIMATE_WAIT;
		}

	}
}

static void EnableLoopingAnims()
{
	int y = 0;
	int x = 0;
	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 10; x++)
		{
			int size = m_PlayerGrid[(y * 10) + x].Size;
			if (size + 1 == m_PlayerGrid[(y * 10) + x].MaxSize)
			{
				m_PlayerGrid[(y * 10) + x].Animate = 4 + size;
			}
			else
			{
				m_PlayerGrid[(y * 10) + x].Animate = 2 + size;
			}

			m_PlayerGrid[(y * 10) + x].Frame = 0;
			m_PlayerGrid[(y * 10) + x].FrameCounter = 0;
		}
	}
}

static void DisableLoopingAnims()
{
	int y = 0;
	int x = 0;
	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 10; x++)
		{
			m_PlayerGrid[(y * 10) + x].Animate = -1;
			m_PlayerGrid[(y * 10) + x].Frame = 0;
			m_PlayerGrid[(y * 10) + x].FrameCounter = 0;
		}
	}
	printGrid();
}




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
	else if (m_Pad.START == PAD_RELEASED)
	{
		SPR_setAnim(m_MessageSpr, 2);
		SPR_update();
		SPR_setVisibility(m_MessageSpr, VISIBLE);

		VDP_setPaletteColors(0, m_FadedPalette, 63);
		m_GameState = PROTO2_STATE_PAUSED;
		HideCursor();
		return;
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
		if (TryIncrementSquare(m_CursorX, m_CursorY, m_CurrentAtom))
		{
			m_GameState = PROTO2_STATE_ANIMATE;
			DisableLoopingAnims();
		}
	}

	UpdateCursor();
}





static void RandomiseGrid()
{
	for (int i = 0; i < 70; i++)
	{
		m_PlayerGrid[i].Changed = 0;
		m_PlayerGrid[i].GrowSize = 0;
		m_PlayerGrid[i].Player = 0;
		m_PlayerGrid[i].Size = 0;
		m_PlayerGrid[i].Animate = -1;
		m_PlayerGrid[i].Frame = 0;
		m_PlayerGrid[i].Explode = 0;		
	}

	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			u16 player = random_int(0, 6);
			u16 size = 0;
			if (player != 0)
			{
				size = random_int(1, m_PlayerGrid[(y * 10) + x].MaxSize - 1);
			}

			m_PlayerGrid[(y * 10) + x].Size = size;
			m_PlayerGrid[(y * 10) + x].Player = player;
		}
	}
}


static void SetupLevel()
{
	if (m_CurrentLevel > 0)
	{
		m_MaxTime = fix32Mul(m_MaxTime, m_MaxTimeDecrease);
		m_TimePerExplosion = fix32Mul(m_TimePerExplosion, m_MaxTimeDecrease);
	}

	m_CurrentLevel++;

	m_AtomNeededCount[0] = 0;
	for (int i = 1; i < 7; i++)
	{
		m_AtomNeededCount[i] = m_LevelNeeded;
	}


	RandomiseGrid();
	EnableLoopingAnims();

	m_LevelingAmount = fix32Add(m_LevelingAmount, m_LevelBoosting);
	m_LevelNeeded += fix32ToInt(m_LevelingAmount);

	// Cap at 99 for now as that is the max number we can show with just 2 number slots
	if (m_LevelNeeded > 99)
	{
		m_LevelNeeded = 99;
	}
}


static void NextAtom()
{
	m_CurrentAtom = m_NextAtom;
	m_NextAtom = m_RandomBag[m_CurrentRnd];
	m_CurrentRnd++;


	if (m_CurrentRnd >= RandomBagCount)
	{
		SetupRandom(m_CurrentAtom);
		m_CurrentRnd = 0;
	}



	SPR_setAnim(m_NextAtomsSpr, m_NextAtom - 1);
	SPR_setAnim(m_CurrentAtomsSpr, m_CurrentAtom - 1);
	
	SPR_setAnim(m_Cursor, m_CurrentAtom - 1);	

}

void UpdateNumbers()
{
	for (int i = 0; i < 6; i++)
	{
		SetNumber(m_AtomNeededCount[i + 1], i);
	}
}


static void CleanUp()
{
	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			m_PlayerGrid[(y * 10) + x].GrowSize = 0;
			m_PlayerGrid[(y * 10) + x].Animate = -1;
			m_PlayerGrid[(y * 10) + x].Frame = 0;
			m_PlayerGrid[(y * 10) + x].FrameCounter = 0;


			if (m_PlayerGrid[(y * 10) + x].Player == 0)
			{
				m_PlayerGrid[(y * 10) + x].Size = 0;
			}
		}
	}
}


// Update the timebar at the bottom of the screen
void UpdateTime()
{	
	// work out how much time is left in percentage
	// as well work it out into 8pixel chunks and what is left over
	fix32 div = fix32Div(m_Time, m_MaxTime);
	fix32 amount = fix32Mul(div, intToFix32(184));
	int rounded = fix32ToInt(amount) / 8;

	int ranged = 184 - fix32ToInt(amount);
	

	// the bar colour is based on how far though the range we are
	if (ranged < 138 && ranged > 46)
	{
		VDP_setPaletteColor(4, m_BarColours[ranged - 46]);
	}
	else if (ranged > 136)
	{
		VDP_setPaletteColor(4, m_BarColours[91]);		
	}
	else if (ranged < 46)
	{
		VDP_setPaletteColor(4, m_BarColours[0]);
	}	

	// draw the full chunks into the background
	int i = 0;
	for (i = 0; i < rounded; i++)
	{
		VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, m_TimerAmounts[0]), 11 + i, 25);
		VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, m_TimerAmounts[1]), 11 + i, 26);
		VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, m_TimerAmounts[2]), 11 + i, 27);
	}
	int value = fix32ToInt(amount) - (rounded * 8);


	// set the last chunk to be whats left over when rounded.
	if (value > 0)
	{
		int use = ((8 - value) * 3);

		VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, m_TimerAmounts[use]), 11 + i, 25);
		VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, m_TimerAmounts[use+ 1]), 11 + i, 26);
		VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, m_TimerAmounts[use+ 2]), 11 + i, 27);
		i++;
	}
	
	// draw the empty chunks
	for (; i < 23; i++)
	{
		VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, m_TimerBacking[0]), 11 + i, 25);
		VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, m_TimerBacking[1]), 11 + i, 26);
		VDP_setTileMapXY(PLAN_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, m_TimerBacking[2]), 11 + i, 27);
	}


	// the music tempo is changed if the user is running out of time
	if (ranged > 136)
	{
		XGM_setMusicTempo(65);
	}
	else if (ranged > 92)
	{
		XGM_setMusicTempo(62);
	}
	else
	{
		XGM_setMusicTempo(60);
	}
}




void Protype2ScreenStart()
{
	u16 count = SPR_getNumActiveSprite();
	KDebug_Alert("Protype2ScreenStart - active sprite count");
	KDebug_AlertNumber(count);

	// disable interrupt when accessing VDP
	SYS_disableInts();

	SPR_reset();
	DMA_waitCompletion();
	VDP_waitVSync();



	// Set palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	int ind = TILE_USERINDEX;


	//VDP_setPalette(PAL0, ingame_back.palette->data);
	VDP_drawImageEx(PLAN_B, &Prototype2_Background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
	ind += Prototype2_Background.tileset->numTile;

	//VDP_setPalette(PAL1, atoms.palette->data);
	VDP_loadTileSet(atoms.tileset, ind, CPU);

	m_AtomTileStart = ind;
	ind += atoms.tileset->numTile;	

	

	VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, m_AtomTileStart), 0, 0, 40, 28);

	

	m_Cursor = SPR_addSprite(&Cursor, 0, 0, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
	SPR_setAnim(m_Cursor, 0);



	VDP_setVerticalScroll(PLAN_A, -28);
	VDP_setHorizontalScroll(PLAN_A, 60);


	m_NextAtomsSpr = SPR_addSprite(&AtomsSample, 0, 164, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
	SPR_setAnim(m_NextAtomsSpr, 0);

	m_CurrentAtomsSpr = SPR_addSprite(&AtomsSample, 24, 164, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
	SPR_setAnim(m_CurrentAtomsSpr, 0);

	for (int i = 0; i < 6; i++)
	{
		m_AtomsSpr[i] = SPR_addSprite(&AtomsSample, 0, 8 + (24 * i), TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		SPR_setAnim(m_AtomsSpr[i], i);
	}

	int j = 0;
	int p = 0;
	for (int i = 0; i < 12; i += 2)
	{
		m_NumbersSpr[i] = SPR_addSprite(&Numbers, 20, 8 + (24 * j), TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
		m_NumbersSpr[i + 1] = SPR_addSprite(&Numbers, 36, 8 + (24 * j), TILE_ATTR(PAL1, FALSE, FALSE, FALSE));

		SPR_setFrame(m_NumbersSpr[i], p++);
		SPR_setFrame(m_NumbersSpr[i + 1], p++);

		if (p >= 10)
		{
			p = 0;
		}

		j++;
	}



	for (int i = 0; i < 10; i++)
	{
		m_ScoreSpr[i] = SPR_addSprite(&Numbers, 140 + (i * 16), 0, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
	}

	m_MultiplerSpr[0] = SPR_addSprite(&Numbers, 34 , 196, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
	m_MultiplerSpr[1] = SPR_addSprite(&Numbers, 50, 196, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));


	
	m_MessageSpr = SPR_addSprite(&messages, 84, 92, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
	SPR_setVisibility(m_MessageSpr, HIDDEN);


	SYS_enableInts();
	// 88 * 200
	// 11 * 25
	// 
	m_TimerBacking[0] = Prototype2_Background.map->tilemap[(25 * 40) + 11] + TILE_USERINDEX;
	m_TimerBacking[1] = Prototype2_Background.map->tilemap[(26 * 40) + 11] + TILE_USERINDEX;
	m_TimerBacking[2] = Prototype2_Background.map->tilemap[(27 * 40) + 11] + TILE_USERINDEX;

	for (int i = 0; i < 8; i++)
	{
		m_TimerAmounts[(i * 3)]  = Prototype2_Background.map->tilemap[(25 * 40) + 12 + i] + TILE_USERINDEX;
		m_TimerAmounts[(i * 3) + 1] = Prototype2_Background.map->tilemap[(26 * 40) + 12 + i] + TILE_USERINDEX;
		m_TimerAmounts[(i * 3) + 2] = Prototype2_Background.map->tilemap[(27 * 40) + 12 + i] + TILE_USERINDEX;
	}


	// prepare palettes
	memcpy(&m_RegularPalette[0], Prototype2_Background.palette->data, 16 * 2);
	memcpy(&m_RegularPalette[16], atoms.palette->data, 16 * 2);
	memcpy(&m_RegularPalette[32], profs.palette->data, 16 * 2);
	memcpy(&m_RegularPalette[48], robo_pal.palette->data, 16 * 2);

	SetupFadedPalette();

	GridSetup();

	XGM_setPCM(SND_MOVE, sound_3, sizeof(sound_3));
	XGM_setPCM(SND_GROW, sound_grow, sizeof(sound_grow));
	XGM_setPCM(SND_EXPLODE, sound_explode, sizeof(sound_explode));


	// GET_HCOUNTER * GET_VCOUNTER
	setRandomSeed(GET_HCOUNTER * GET_VCOUNTER);
	
	SetupRandom(0);

	m_LevelingAmount = m_InitalLevelingAmount;
	m_LevelNeeded = 2;
	SetupLevel();

	m_CursorX = 0;
	m_CursorY = 0;
	UpdateCursor();

	
	
	m_NextAtom = random_int(1, 6);
	m_NextAtom = m_RandomBag[0];
	m_CurrentRnd = 1;
	m_Multiplier = 1;

	m_TimePerExplosion = m_InitalTimePerExplosion;
	m_MaxTime = m_InitalMaxTime;
	m_Time = m_MaxTime;
	
	m_GameState = PROTO2_STATE_PLAY;

	NextAtom();

	UpdateTime();
	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	DrawGameGrid();
	UpdateNumbers();
	SetMultiplier();

	SPR_update();

	m_Score = 0;

	m_CurrentLevel = 0;



	// fade in
	VDP_fadeIn(0, (4 * 16) - 1, m_RegularPalette, 20, FALSE);	

	
	// Generate colour fades for the timing bar
	m_BarColours[0] = 0x00E0;
	u16 start = 0x00E0;
	for (int i = 0; i < 45; i += 3)
	{
		m_BarColours[i] = start;
		m_BarColours[i + 1] = start;
		m_BarColours[i + 2] = start;

		start += 1;
	}
	m_BarColours[45] = 0x00EE;

	start = 0x00EE;
	for (int i = 46; i < 91; i += 3)
	{
		m_BarColours[i] = start;
		m_BarColours[i + 1] = start;
		m_BarColours[i + 2] = start;

		start -= 0x0010;
	}
	m_BarColours[91] = 0x000E;
	
	m_PrevTime = getTimeAsFix32(getTick());
	printGrid();
}


static int CheckForSpace()
{
	for (int i = 0; i < 70; i++)
	{
		if (m_PlayerGrid[i].Player == 0)
		{
			return TRUE;
		}
		else if (m_PlayerGrid[i].Player == m_CurrentAtom)
		{
			return TRUE;
		}
	}

	return FALSE;
}

#define WAITFOR 100
static int m_levelWait = WAITFOR;

void Protype2ScreenUpdate()
{
	fix32 current = getTimeAsFix32(getTick());
	int dif = fix32Sub(current, m_PrevTime);
	
	m_PrevTime = current;

	if (m_Pad.START == PAD_RELEASED)
	{
		printGrid();
	}

	switch (m_GameState)
	{
		case PROTO2_STATE_PLAY:
		{			
			PlayerInput();
			m_Exploded = 0;
			m_Time = fix32Sub( m_Time, dif);

			if (m_Time < 0)
			{
				m_Time = 0;
				DisableLoopingAnims();
				m_GameState = PROTO2_STATE_GAME_OVER;
			}
			break;
		}

		case PROTO2_STATE_ANIMATE:
		{
			HideCursor();
			DisableLoopingAnims();
			AnimateScreen();
			break;
		}

		case PROTO2_STATE_ANIMATE_WAIT:
		{
			u8 animating = 0;
			for (int i = 0; i < 70; i++)
			{
				if (m_PlayerGrid[i].Animate >= 0)
				{
					animating++;
				}
			}

			if (!animating)
			{
				m_GameState = PROTO2_STATE_ANIMATE;
			}

			break;
		}

		case PROTO2_STATE_END_CHECK:
		{
			
			int count = 0;
			for (int i = 1; i < 7; i++)
			{
				if (m_AtomNeededCount[i] > 0)
				{
					count++;
				}
			}

			if (count == 0)
			{
				m_GameState = PROTO2_STATE_LEVEL_UP;
			}
			else
			{
				if (m_EarlyOut)
				{
					m_levelWait = WAITFOR;
					m_GameState = PROTO2_STATE_RESET;
				}
				else
				{
					m_GameState = PROTO2_STATE_PLAY;
					CleanUp();

					NextAtom();
					EnableLoopingAnims();

					if (!CheckForSpace())
					{
						m_levelWait = WAITFOR;
						m_GameState = PROTO2_STATE_RESET;
					}
				}
			}

			if (m_Exploded)
			{
				m_Multiplier++;

				if (m_Multiplier > m_CurrentLevel)
				{
					m_Multiplier = m_CurrentLevel + 1;
				}
			}
			else
			{
				m_Multiplier = 1;
			}			
			
			break;
		}

		case PROTO2_STATE_LEVEL_UP:
		{
			if (m_levelWait == WAITFOR)
			{
				VDP_setPaletteColors(0, m_FadedPalette, 63);

				SPR_setAnim(m_MessageSpr, 0);
				SPR_update();
				SPR_setVisibility(m_MessageSpr, VISIBLE);

				SetupLevel();
				SetupRandom(0);
				HideCursor();				
			}
			if (m_levelWait < 0)
			{
				VDP_setPaletteColors(0, m_RegularPalette, 63);
				SPR_setVisibility(m_MessageSpr, HIDDEN);
				m_GameState = PROTO2_STATE_PLAY;
				m_levelWait = WAITFOR + 1;
				UpdateCursor();
			}

			m_levelWait--;
			break;
		}


		case PROTO2_STATE_RESET:
		{
			if (m_levelWait == WAITFOR)
			{
				VDP_setPaletteColors(0, m_FadedPalette, 63);
				
				SPR_setAnim(m_MessageSpr, 1);
				SPR_update();
				SPR_setVisibility(m_MessageSpr, VISIBLE);

				RandomiseGrid();
				SetupRandom(m_CurrentAtom);
				HideCursor();
				m_Multiplier = 1;
				m_Time = fix32Mul(m_Time, m_TimePenaty);
			}
			if (m_levelWait < 0)
			{
				VDP_setPaletteColors(0, m_RegularPalette, 63);
				SPR_setVisibility(m_MessageSpr, HIDDEN);
				AnimateScreen();
				m_GameState = PROTO2_STATE_PLAY;
				m_levelWait = WAITFOR + 1;
				UpdateCursor();
			}

			m_levelWait--;
			break;
		}

		case PROTO2_STATE_GAME_OVER:
		{
			//StateMachineChange(&GameMachineState, &ChallengeModeHighScoreState);

			if (m_Score > m_HighScoreAmounts[HIGHSCORE_TOTAL - 1])
			{
				StateMachineChange(&GameMachineState, &ChallengeModeSetNameState);
			}
			else
			{
				StateMachineChange(&GameMachineState, &Prototype2GameOverState);
			}

			return;
		}

		case PROTO2_STATE_PAUSED:
		{
			if (m_Pad.START == PAD_RELEASED)
			{
				SPR_setVisibility(m_MessageSpr, HIDDEN);
				m_GameState = PROTO2_STATE_PLAY;
				VDP_setPaletteColors(0, m_RegularPalette, 63);
				UpdateCursor();
			}

			return;
		}

		default:
			break;
	}


	if (m_GameState != PROTO2_STATE_GAME_OVER)
	{
		for (int i = 1; i < 7; i++)
		{
			if (m_AtomNeededCount[i] < 0)
			{
				m_AtomNeededCount[i] = 0;
			}
		}


		if (m_Time > m_MaxTime)
		{
			m_Time = m_MaxTime;
		}

		DrawGameGrid();
		UpdateNumbers();
		UpdateTime();
		SetScore(m_Score);
		SetMultiplier();
	}
}



void Protype2ScreenEnd()
{
	VDP_fadeOut(0, 63, 10, FALSE);	
	XGM_setMusicTempo(60);

	SYS_disableInts();
	
	SPR_releaseSprite(m_MessageSpr);
	
	SPR_releaseSprite(m_MultiplerSpr[1]);
	SPR_releaseSprite(m_MultiplerSpr[0]);


	for (int i = 9; i >= 0; i--)
	{
		SPR_releaseSprite(m_ScoreSpr[i]);
	}

	for (int i = 11; i >= 0; i--)
	{
		SPR_releaseSprite(m_NumbersSpr[i]);
	}

	for (int i = 5; i >= 0; i--)
	{
		SPR_releaseSprite(m_AtomsSpr[i]);
	}

	SPR_releaseSprite(m_CurrentAtomsSpr);
	SPR_releaseSprite(m_NextAtomsSpr);
	SPR_releaseSprite(m_Cursor);

	u16 count = SPR_getNumActiveSprite();
	KDebug_Alert("Protype2ScreenStart - active sprite count");
	KDebug_AlertNumber(count);


	VDP_clearPlan(PLAN_A, TRUE);
	VDP_clearPlan(PLAN_B, TRUE);

	VDP_setVerticalScroll(PLAN_A, 0);
	VDP_setHorizontalScroll(PLAN_A, 0);	

	SYS_enableInts();

	//SPR_update();
	//VDP_resetScreen();
}


SimpleState Protype2ScreenState =
{
	Protype2ScreenStart,
	Protype2ScreenUpdate,
	Protype2ScreenEnd
};