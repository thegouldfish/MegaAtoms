#include <genesis.h>
#include <kdebug.h>

#include "../inc/atoms.h"
#include "../inc/AtomsGameState.h"
#include "../res/gfx.h"
#include "../res/sprite.h"
#include "../res/sound.h"
#include "../inc/WinnerScreen.h"
#include "../inc/GameState.h"
#include "../inc/PadHelper.h"

#include "../inc/GamePlay.h"


#define STATE_PLAY 0
#define STATE_ANIMATE 1
#define STATE_ANIMATE_WAIT 2
#define STATE_END_CHECK 3
#define STATE_PAUSE 4

static int m_TurnCount = 0;

static int m_CurrentPlayer = 0;

static Sprite *m_Current;
static Sprite *m_Next;
static Sprite* m_MessageSpr;


static int m_GameState = STATE_PLAY;

static int m_GameFinished;
static int m_Alive[7];
static int m_TotalAtoms;

static Sprite* m_ProgressBars[14];
static Sprite* m_ProgressToppers[7];
 

static Point m_CursorPositions[7];


#define SND_MOVE 67
#define SND_GROW 68
#define SND_EXPLODE 69



int m_WinningPlayer = 0;
int m_PlayerSetup[7];


void SetupGame()
{
	int i = 0;

	for (i = 0; i < 70; i++)
	{
		m_PlayerGrid[i].Changed = 0;
		m_PlayerGrid[i].GrowSize = 0;
		m_PlayerGrid[i].Player = 0;
		m_PlayerGrid[i].Size = 0;
		m_PlayerGrid[i].Animate = -1;
		m_PlayerGrid[i].Frame = 0;
	}

	// Reset the Player
	// Player 0 is no player
	m_CurrentPlayer = 0;

	m_GameFinished = 0;

	m_TurnCount = 0;

	for (i = 0; i < 7; i++)
	{		
		if (m_PlayerSetup[i])
		{
			m_Alive[i] = 1;
		}
		else
		{
			m_Alive[i] = 0;
		}

		if (m_CurrentPlayer == 0 && m_PlayerSetup[i] != 0)
		{
			m_CurrentPlayer = i;
		}

		m_CursorPositions[i].X = -1;
		m_CursorPositions[i].Y = -1;
	}



	m_CursorX = 0;
	m_CursorY = 0;

	m_GameState = STATE_PLAY;
}


void UpdateProgress()
{
	int sizes[7] = { 16,16,16,16,16,16,16 };

	int i = 0;
	if (m_TurnCount)
	{
		int max = 0;		
		for (i = 1; i < 7; i++)
		{
			if (m_Alive[i] > max)
			{
				max = m_Alive[i];
			}
		}

		for (i = 1; i < 7; i++)
		{
			if (m_Alive[i])
			{
				sizes[i] = (int)(((float)m_Alive[i] / (float)max) * 15.0f) + 1;
			}
			else
			{
				sizes[i] = 0;
			}
		}
	}
	else
	{
		for (i = 1; i < 7; i++)
		{
			if (m_PlayerSetup[i])
			{
				sizes[i] = 16;
			}
			else
			{
				sizes[i] = 0;
			}
		}
	}

	for (i = 1; i < 7; i++)
	{
		int topperMove = 0;
		int bars[2];
		if (sizes[i] > 8)
		{
			int size = sizes[i] - 8;
			topperMove = 8 - size;
			bars[0] = size;
			bars[1] = 8;
		}
		else if (sizes[i] == 0)
		{
			topperMove = -1;
			bars[0] = 0;
			bars[1] = 0;

		}
		else
		{
			bars[0] = 0;
			bars[1] = sizes[i];
			topperMove = (8 - sizes[i]) + 8;
		}

		for (int j = 0; j < 2; j++)
		{
			int bar = ((i - 1) * 2) + j;
			if (bars[j])
			{
				SPR_setVisibility(m_ProgressBars[bar], VISIBLE);
				SPR_setAnimAndFrame(m_ProgressBars[bar], 9 - bars[j], i-1);
			}
			else
			{
				SPR_setVisibility(m_ProgressBars[bar], HIDDEN);
			}

			if (topperMove == -1)
			{
				SPR_setVisibility(m_ProgressToppers[i-1], HIDDEN);
			}
			else
			{
				SPR_setVisibility(m_ProgressToppers[i - 1], VISIBLE);
				SPR_setPosition(m_ProgressToppers[i - 1], (i - 1) * 8, 26 + topperMove);
			}
		}
	}
}

void SwitchPlayer()
{
	u8 setPlayer = m_CurrentPlayer - 1;

	if (setPlayer < 0)
	{
		setPlayer = 0;
	}

	SPR_setAnim(m_Cursor, setPlayer);
}






static void EnableLoopingAnims()
{
	int y = 0;
	int x = 0;
	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 10; x++)
		{						
			if (m_PlayerGrid[(y * 10) + x].Player == m_CurrentPlayer)
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
}


static void AnimateScreen()
{
	u8 animating = 1;
	int x = 0;
	int y = 0;
	u8 j;
	u8 allSame = 1;
	char lastPlayer = -1;
	char exploded = 0;
	char done = 0;
	char grow = 0;

	//while (animating)
	{
		lastPlayer = -1;
		allSame = 1;
		exploded = 0;
		grow = 0;
		done = 1;
		for (j = 0; j < 70; j++)
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

		m_TotalAtoms = 0;
		if (m_TurnCount)
		{			
			for (j = 0; j < 7; j++)
			{
				m_Alive[j] = 0;
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

				
				if (m_TurnCount)
				{
					m_Alive[player]++;
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
		if (allSame && m_TurnCount)
		{
			m_GameFinished = 1;
			m_WinningPlayer = m_CurrentPlayer;
			animating = 0;
		}

		if (!animating)
		{
			m_GameState = STATE_END_CHECK;
		}
		else
		{
			m_GameState = STATE_ANIMATE_WAIT;
		}

	}
}


void UpdateUpNext()
{

	int next = m_CurrentPlayer;

	u16 startingPlayer = m_CurrentPlayer;
	while (1)
	{
		next++;

		if (next > 6)
		{
			next = 1;
		}

		if (next == startingPlayer)
		{
			next = -1;
			break;
		}

		if (m_PlayerSetup[next] != 0 && m_Alive[next])
		{
			break;
		}
	}
	
	if (m_PlayerSetup[m_CurrentPlayer] == 1)
	{
		SPR_setAnim(m_Current, m_CurrentPlayer - 1);
		SPR_setPalette(m_Current, PAL2);
	}
	else if(m_PlayerSetup[m_CurrentPlayer] == 2)
	{
		SPR_setAnim(m_Current, m_CurrentPlayer -1 + 6);
		SPR_setPalette(m_Current, PAL3);
	}		





	if (next != -1)
	{
		if (m_PlayerSetup[next] == 1)
		{
			SPR_setAnim(m_Next, next - 1);
			SPR_setPalette(m_Next, PAL2);
		}
		else if (m_PlayerSetup[next] == 2)
		{
			SPR_setAnim(m_Next, next -1 + 6);
			SPR_setPalette(m_Next, PAL3);
		}			
	}
}


 


void AtomGameStart()
{	
	// disable interrupt when accessing VDP
	SYS_disableInts();

	// Clear anything left over from the previous state
	VDP_resetScreen();
	SPR_reset();


	// Set palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	int ind = TILE_USERINDEX;



	SYS_enableInts();

	VDP_drawImageEx(PLAN_B, &ingame_back, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, DMA_QUEUE);
	ind += ingame_back.tileset->numTile;


	VDP_loadTileSet(atoms.tileset, ind, DMA_QUEUE);

	m_AtomTileStart = ind;
	ind += atoms.tileset->numTile;

	VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, m_AtomTileStart), 0, 0, 40, 28);



	m_Cursor = SPR_addSprite(&Cursor, 0, 0, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
	SPR_setAnim(m_Cursor, 0);

	m_MessageSpr = SPR_addSprite(&messages, 84, 92, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
	SPR_setAnim(m_MessageSpr, 2);
	SPR_setVisibility(m_MessageSpr, HIDDEN);


	VDP_setVerticalScroll(PLAN_A, -28);
	VDP_setHorizontalScroll(PLAN_A,60);


	int y = 26;
	for (int i = 0; i < 6; i++)
	{
		m_ProgressToppers[i] = SPR_addSprite(&Scalers, i * 8, y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
		SPR_setAnimAndFrame(m_ProgressToppers[i], 0, i);
		

		for (int j = 0; j < 2; j++)
		{
			m_ProgressBars[(i*2)+j] = SPR_addSprite(&Scalers, i * 8, y + 8 + (j * 8), TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
			SPR_setAnimAndFrame(m_ProgressBars[(i * 2) + j],1, i);
		}	

	}

	UpdateProgress();

	// prepare palettes
	memcpy(&m_RegularPalette[0], ingame_back.palette->data, 16 * 2);
	memcpy(&m_RegularPalette[16], atoms.palette->data, 16 * 2);
	memcpy(&m_RegularPalette[32], profs.palette->data, 16 * 2);
	memcpy(&m_RegularPalette[48], robo_pal.palette->data, 16 * 2);

	SetupFadedPalette();

	GridSetup();

	HideCursor();

	SetupGame();

	m_WinningPlayer = 0;

	XGM_setPCM(SND_MOVE, sound_3, sizeof(sound_3));
	XGM_setPCM(SND_GROW, sound_grow, sizeof(sound_grow));
	XGM_setPCM(SND_EXPLODE, sound_explode, sizeof(sound_explode));




	m_Current = SPR_addSprite(&profs, 0, 0, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
	SPR_setPosition(m_Current, 2, 52);

	m_Next = SPR_addSprite(&profs, 0, 0, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
	SPR_setPosition(m_Next, 2, 124);

	UpdateUpNext();


	// fade in
	VDP_fadeIn(0, (4 * 16) - 1, m_RegularPalette, 20, FALSE);

	ResetPad(&m_Pad);
}




void AIInput()
{
	int attempt = 5;
	int done = 0;
	int rndx, rndy, player;

	while (attempt > 0 && done == 0)
	{
		do
		{
			rndx = random();
			rndx = (rndx & 0xF);
		} while (rndx > 9);

		do
		{
			rndy = random();
			rndy = rndy & 0x7;
		} while (rndy > 6);

		player = PlayerAtSquare(rndx, rndy);

		if (player == 0 || player == m_CurrentPlayer)
		{
			IncrementSquare(rndx, rndy, m_CurrentPlayer);
			done = 1;
		}

		attempt--;
	}

	while (!done)
	{
		rndx++;

		if (rndx > 9)
		{
			rndx = 0;
			rndy++;
		}

		if (rndy > 6)
		{
			rndy = 0;
			rndx = 0;
		}

		player = PlayerAtSquare(rndx, rndy);

		if (player == 0 || player == m_CurrentPlayer)
		{
			IncrementSquare(rndx, rndy, m_CurrentPlayer);
			done = 1;
		}
	}

	m_CursorX = rndx;
	m_CursorY = rndy;

	XGM_startPlayPCM(SND_MOVE, 1, SOUND_PCM_CH2);
	m_GameState = STATE_ANIMATE;

	UpdateCursor();
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
		m_GameState = STATE_PAUSE;
		HideCursor();
		
		SPR_setVisibility(m_MessageSpr, VISIBLE);
		VDP_setPaletteColors(0, m_FadedPalette, 63);
	}

	if (m_CursorX > 9)
	{
		m_CursorX = 9;
	}
	else if(m_CursorX < 0)
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

		if (TryIncrementSquare(m_CursorX, m_CursorY, m_CurrentPlayer))
		{
			HideCursor();
			// change logic
			m_GameState = STATE_ANIMATE;
			
			
			return;
		}
	}


	UpdateCursor();
}





void CheckForFinished()
{
	u16 startingPlayer = m_CurrentPlayer;
	while (1)
	{
		m_CurrentPlayer++;

		if (m_CurrentPlayer > 6)
		{
			m_CurrentPlayer = 1;
			m_TurnCount++;
		}

		if (m_TurnCount)
		{
			if (m_CurrentPlayer == startingPlayer)
			{
				m_GameFinished = 1;
				m_WinningPlayer = startingPlayer;
				break;
			}
			else if (m_Alive[m_CurrentPlayer] && m_PlayerSetup[m_CurrentPlayer] != 0)
			{
				break;
			}
		}
		else
		{
			if (m_PlayerSetup[m_CurrentPlayer] != 0)
			{
				break;
			}
		}
	}
}






void AtomGameUpdate()
{
	switch (m_GameState)
	{
		case STATE_PLAY:
		{
			if (m_PlayerSetup[m_CurrentPlayer] == 1)
			{
				PlayerInput();
			}
			else if (m_PlayerSetup[m_CurrentPlayer] == 2)
			{
				AIInput();
			}
			
			break;
		}

		case STATE_ANIMATE:
		{
			DisableLoopingAnims();
			AnimateScreen();
			UpdateProgress();
			break;
		}

		case STATE_ANIMATE_WAIT:
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
				m_GameState = STATE_ANIMATE;
			}

			break;
		}

		case STATE_END_CHECK:
		{
			m_CursorPositions[m_CurrentPlayer].X = m_CursorX;
			m_CursorPositions[m_CurrentPlayer].Y = m_CursorY;


			CheckForFinished();

			if (m_GameFinished)
			{
				m_WinningPlayer = m_CurrentPlayer;
				StateMachineChange(&GameMachineState, &WinnerScreenState);
				return;
			}
			else
			{
				SwitchPlayer();
				UpdateUpNext();
				EnableLoopingAnims();


				if (m_CursorPositions[m_CurrentPlayer].X != -1 && m_CursorPositions[m_CurrentPlayer].Y != -1)
				{
					m_CursorX = m_CursorPositions[m_CurrentPlayer].X;
					m_CursorY = m_CursorPositions[m_CurrentPlayer].Y;
				}

				m_GameState = STATE_PLAY;
			}
			
			break;
		}

		case STATE_PAUSE:
		{
			HideCursor();
			if (m_Pad.START == PAD_RELEASED)
			{
				m_GameState = STATE_PLAY;
				UpdateCursor();

				SPR_setVisibility(m_MessageSpr, HIDDEN);
				VDP_setPaletteColors(0, m_RegularPalette, 63);
			}

			break;
		}
	}

	
	DrawGameGrid();
}



void AtomGameEnd()
{
	VDP_fadeOut(0, 63, 20, FALSE);

	SPR_setVisibility(m_Cursor, HIDDEN);
	SPR_releaseSprite(m_Cursor);
	
	SPR_setVisibility(m_Current,HIDDEN);	
	SPR_releaseSprite(m_Current);
	
	SPR_setVisibility(m_Next, HIDDEN);
	SPR_releaseSprite(m_Next);
	
	VDP_setVerticalScroll(PLAN_A, 0);
	VDP_setHorizontalScroll(PLAN_A, 0);
	
	SPR_reset();
	SPR_update();
}



SimpleState AtomsGameState =
{
	AtomGameStart,
	AtomGameUpdate,
	AtomGameEnd,
	NULL
};