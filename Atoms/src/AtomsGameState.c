#include <genesis.h>
#include <kdebug.h>

#include "../inc/AtomsGameState.h"
#include "../res/gfx.h"
#include "../res/sprite.h"
#include "../res/sound.h"
#include "../inc/WinnerScreen.h"
#include "../inc/GameState.h"
#include "../inc/PadHelper.h"


int m_AtomTileStart;

int m_LastCursorX = 0;
int m_LastCursorY = 0;

int m_CursorX = 0;
int m_CursorY = 0;

int m_TurnCount = 0;

int m_CurrentPlayer = 0;

int m_WinningPlayer;

Sprite* m_Cursor;

Sprite *m_Current;
Sprite *m_Next;

Pad m_Pad;

#define STATE_PLAY 0
#define STATE_ANIMATE 1
#define STATE_ANIMATE_WAIT 2
#define STATE_END_CHECK 3

int m_GameState = STATE_PLAY;

int m_GameFinished;
int m_Alive[7];
int m_PlayerSetup[7];




#define SND_MOVE 67
#define SND_GROW 68
#define SND_EXPLODE 69


struct GridSquare
{
	int Player;
	int Size;
	int GrowSize;
	int Changed;
	int MaxSize;
	int Explode;

	int Animate;
} m_PlayerGrid[70];



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
}


void DrawAtom(int player, int x, int y, int size)
{
	int row = 0;
	int column = 0;

	if (player < 0)
	{
		player = 0;
		size = 0;
	}

	u16 t = (3 * size) + (player * 3 * 18);
	for (row = 0; row < 3; row++)
	{
		for (column = 0; column < 3; column++)
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



void SetupGame()
{
	int i = 0;

	for (i = 0; i < 70; i++)
	{
		m_PlayerGrid[i].Changed = 0;
		m_PlayerGrid[i].GrowSize = 0;
		m_PlayerGrid[i].Player = 0;
		m_PlayerGrid[i].Size = 0;
	}

	// Reset the Player
	// Player 0 is no player
	m_CurrentPlayer = 0;

	m_GameFinished = 0;

	m_TurnCount = 0;

	for (i = 0; i<7; i++)
	{
		
		// Set each player to be human for now
		//m_PlayerSetup[i] = 1;
		m_Alive[i] = 0;

		if (m_CurrentPlayer == 0 && m_PlayerSetup[i] != 0)
		{
			m_CurrentPlayer = i;
		}
	}



	m_CursorX = 0;
	m_CursorY = 0;

	m_GameState = STATE_PLAY;
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


void HideCursor()
{
	SPR_setPosition(m_Cursor, -32, -32);
}

void UpdateCursor()
{
	SPR_setPosition(m_Cursor, (m_CursorX * 24) + 56, (m_CursorY * 24) + 24);
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

			int rnd = (((y + x)* y) + 5 / (x+1)) % (6);
			DrawAtom(rnd, x, y, maxSize);
		}
	}
}


void DrawGameGrid()
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


void AnimateScreen()
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
			}
		}

		for (j = 0; j<7; j++)
		{
			m_Alive[j] = 0;
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

				m_Alive[player]++;

				if (m_PlayerGrid[i].Changed)
				{
					animating = 2;

					if (size == m_PlayerGrid[i].MaxSize)
					{
						//DrawSquare(x, y, 4, player, 1);
						m_PlayerGrid[i].Changed = 1;
						m_PlayerGrid[i].Size = 5;
						exploded = 1;
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


u8 m_CurrentType = 0;
u8 m_CurrentId = 0;

u8 m_NextType = 0;
u8 m_NextId = 0;

void UpdateUpNext()
{
	Sprite* prevCurrent = m_Current;
	Sprite* prevNext = m_Next;

	u8 updateCurrent = FALSE;
	u8 updateNext = FALSE;

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

		if (m_PlayerSetup[next] != 0)
		{
			break;
		}
	}


	if (m_CurrentPlayer != m_CurrentId)
	{
		updateCurrent = TRUE;
	}
	if (m_PlayerSetup[m_CurrentPlayer] == m_CurrentType)
	{
		updateCurrent = TRUE;
	}

	if (next != m_NextId)
	{
		updateNext = TRUE;
	}
	if (m_PlayerSetup[next] == m_NextType)
	{
		updateNext = TRUE;
	}

	
	if (updateCurrent)
	{
		SPR_setVisibility(prevCurrent, HIDDEN);
		if (m_PlayerSetup[m_CurrentPlayer] == 1)
		{
			m_Current = SPR_addSprite(&profs_player, 0, 0, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
		}
		else
		{
			m_Current = SPR_addSprite(&profs_robot, 0, 0, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
		}
		SPR_setPosition(m_Current, 2, 52);
		SPR_setAnim(m_Current, m_CurrentPlayer - 1);
	}




	if (next != -1)
	{
		if (updateNext)
		{
			SPR_setVisibility(prevNext, HIDDEN);
			if (m_PlayerSetup[next] == 1)
			{
				m_Next = SPR_addSprite(&profs_player, 0, 0, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
			}
			else
			{
				m_Next = SPR_addSprite(&profs_robot, 0, 0, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
			}
			SPR_setPosition(m_Next, 2, 124);
			SPR_setAnim(m_Next, next - 1);
		}
	}
	
	if (updateCurrent && updateNext)
	{
		SPR_update();
		
		//VDP_waitVSync();
	}
	

	if (prevCurrent != NULL)
	{
		if (updateCurrent)
		{
			SPR_releaseSprite(prevCurrent);
			m_CurrentId = m_CurrentPlayer;
			m_CurrentType = m_PlayerSetup[m_CurrentPlayer];
		}
	}

	if (prevNext != NULL)
	{
		if (updateNext)
		{
			SPR_releaseSprite(prevNext);
			m_NextId = next;
			m_NextType = m_PlayerSetup[next];
		}
	}


	//SPR_update();
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


	//VDP_setPalette(PAL0, ingame_back.palette->data);
	VDP_drawImageEx(PLAN_B, &ingame_back, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
	ind += ingame_back.tileset->numTile;

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
	VDP_setHorizontalScroll(PLAN_A,60);






	// prepare palettes
	memcpy(&palette[0], ingame_back.palette->data, 16 * 2);
	memcpy(&palette[16], atoms.palette->data, 16 * 2);
	memcpy(&palette[32], profs_player.palette->data, 16 * 2);
	memcpy(&palette[48], profs_robot.palette->data, 16 * 2);

	GridSetup();

	HideCursor();

	//DrawFullGrid();

	SetupGame();
	SetupPad(&m_Pad, JOY_1);

	m_WinningPlayer = 0;

	XGM_setPCM(SND_MOVE, sound_3, sizeof(sound_3));
	XGM_setPCM(SND_GROW, sound_grow, sizeof(sound_grow));
	XGM_setPCM(SND_EXPLODE, sound_explode, sizeof(sound_explode));




	m_Current = NULL;
	m_Next = NULL;

	m_CurrentType = -1;
	m_CurrentId = -1;

	m_NextType = -1;
	m_NextId = -1;

	UpdateUpNext();


	// fade in
	VDP_fadeIn(0, (4 * 16) - 1, palette, 20, FALSE);
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


void PlayerInput()
{
	m_LastCursorX = m_CursorX;
	m_LastCursorY = m_CursorY;


	UpdatePad(&m_Pad);

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




int wait = 0;

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
			wait = 3;
			AnimateScreen();
			break;
		}

		case STATE_ANIMATE_WAIT:
		{
			wait--;

			if (wait < 0)
			{
				m_GameState = STATE_ANIMATE;
			}

			break;
		}

		case STATE_END_CHECK:
		{
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
				m_GameState = STATE_PLAY;
			}
		}
	}

	
	DrawGameGrid();
}



void AtomGameEnd()
{
	VDP_fadeOut(0, 64, 20, FALSE);

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
	AtomGameEnd
};