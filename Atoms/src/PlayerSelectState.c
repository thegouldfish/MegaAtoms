#include <genesis.h>

#include "../inc/PlayerSelectState.h"
#include "../res/gfx.h"
#include "../res/sprite.h"
#include "../inc/PadHelper.h"
#include "../inc/AtomsGameState.h"
#include "../inc/GameState.h"

#include "../res/sound.h"

int m_PlayerType[6];

Sprite* m_Players[6];
Sprite* m_Cursor;

Pad m_PlayerSelectPad;
int m_PlayerSelectCurrentlySelected;

void MoveCursor()
{
	switch (m_PlayerSelectCurrentlySelected)
	{
		case 0:
			SPR_setPosition(m_Cursor, 41, 4);
			break;
		
		case 1:
			SPR_setPosition(m_Cursor, 130, 4);
			break;

		case 2:
			SPR_setPosition(m_Cursor, 219, 4);
			break;


		case 3:
			SPR_setPosition(m_Cursor, 41, 93);
			break;

		case 4:
			SPR_setPosition(m_Cursor, 130, 93);
			break;

		case 5:
			SPR_setPosition(m_Cursor, 219, 93);
			break;
	}

}

void ChangePlayer(int id, int type)
{
	if (m_Players[id] != 0)
	{
		SPR_releaseSprite(m_Players[id]);
	}

	switch (type)
	{
		case 0:
		{
			m_Players[id] = 0;
			return;
		}

		case 1:
		{
			m_Players[id] = SPR_addSprite(&profs_player, 0, 0, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
			break;
		}


		case 2:
		{
			m_Players[id] = SPR_addSprite(&profs_robot, 0, 0, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
			break;
		}
	}

	SPR_setVisibility(m_Players[id], HIDDEN);

	SPR_setAnim(m_Players[id], id);

	switch (id)
	{
		case 0:
		{
			SPR_setPosition(m_Players[id], 41, 9);
			break;
		}

		case 1:
		{
			SPR_setPosition(m_Players[id], 130, 9);
			break;
		}


		case 2:
		{
			SPR_setPosition(m_Players[id], 219, 9);
			break;
		}

		case 3:
		{
			SPR_setPosition(m_Players[id], 41, 98);
			break;
		}

		case 4:
		{
			SPR_setPosition(m_Players[id], 130, 98);
			break;
		}

		case 5:
		{
			SPR_setPosition(m_Players[id], 219, 98);
			break;
		}
	}


	SPR_setVisibility(m_Players[id], VISIBLE);
}


#define SND_MOVE 64
#define SND_CHANGE 65
#define SND_START 66

void PlayerSelectStart()
{
	// disable interrupt when accessing VDP
	//

	SYS_disableInts();
	// Clear anything left over from the previous state
	VDP_resetScreen();
	SPR_reset();

	// Set palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);
	int ind = TILE_USERINDEX;

	
	VDP_drawImageEx(PLAN_B, &playerselect_background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
	ind += playerselect_background.tileset->numTile;
	VDP_drawImageEx(PLAN_A, &playerselect_stands_background, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
	SYS_enableInts();


	m_PlayerType[0] = 1;
	m_PlayerType[1] = 2;
	m_PlayerType[2] = 0;
	m_PlayerType[3] = 0;
	m_PlayerType[4] = 0;
	m_PlayerType[5] = 0;


	for (int i = 0; i < 6; i++)
	{		
		m_Players[i] = 0;
		ChangePlayer(i, m_PlayerType[i]);
	}

	m_Cursor = SPR_addSprite(&playerselect_cursor, 0, 0, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
	SPR_setPosition(m_Cursor, 41, 4);


	VDP_setTextPalette(PAL0);

	



	u16 palette[64];
	memcpy(&palette[0], playerselect_background.palette->data, 16 * 2);
	memcpy(&palette[16], playerselect_stands_background.palette->data, 16 * 2);
	memcpy(&palette[32], profs_player.palette->data, 16 * 2);
	memcpy(&palette[48], profs_robot.palette->data, 16 * 2);

	m_PlayerSelectCurrentlySelected = 0;
	SetupPad(&m_PlayerSelectPad, JOY_1);

	MoveCursor();


	SPR_update();

	// fade in
	VDP_fadeIn(0, 63, palette, 10, FALSE);



	XGM_setPCM(SND_MOVE, sound_3, sizeof(sound_3));
	XGM_setPCM(SND_CHANGE, sound_2, sizeof(sound_2));
	XGM_setPCM(SND_START, sound_4, sizeof(sound_4));	
}




void PlayerSelectUpdate()
{
	UpdatePad(&m_PlayerSelectPad);


	if (m_PlayerSelectPad.Left == PAD_PRESSED)
	{
		m_PlayerSelectCurrentlySelected--;

		if (m_PlayerSelectCurrentlySelected < 0)
		{
			m_PlayerSelectCurrentlySelected = 5;
		}

		MoveCursor();
		XGM_startPlayPCM(SND_MOVE, 0, 2);
	}


	if (m_PlayerSelectPad.Right == PAD_PRESSED)
	{
		m_PlayerSelectCurrentlySelected++;

		if (m_PlayerSelectCurrentlySelected > 5)
		{
			m_PlayerSelectCurrentlySelected = 0;
		}

		MoveCursor();
		XGM_startPlayPCM(SND_MOVE, 0, 2);
	}

	if (m_PlayerSelectPad.Down == PAD_PRESSED)
	{
		m_PlayerSelectCurrentlySelected += 3;

		if (m_PlayerSelectCurrentlySelected > 5)
		{
			m_PlayerSelectCurrentlySelected -= 6;
		}

		MoveCursor();
		XGM_startPlayPCM(SND_MOVE, 0, 2);
	}


	if (m_PlayerSelectPad.Up == PAD_PRESSED)
	{
		m_PlayerSelectCurrentlySelected -= 3;

		if (m_PlayerSelectCurrentlySelected < 0)
		{
			m_PlayerSelectCurrentlySelected += 6;
		}

		MoveCursor();
		XGM_startPlayPCM(SND_MOVE, 0, 2);
	}


	if (m_PlayerSelectPad.A == PAD_PRESSED)
	{
		m_PlayerType[m_PlayerSelectCurrentlySelected] ++;

		if (m_PlayerType[m_PlayerSelectCurrentlySelected] > 2)
		{
			m_PlayerType[m_PlayerSelectCurrentlySelected] = 0;
		}

		ChangePlayer(m_PlayerSelectCurrentlySelected, m_PlayerType[m_PlayerSelectCurrentlySelected]);

		XGM_startPlayPCM(SND_CHANGE, 0, 2);
	}


	if (m_PlayerSelectPad.START == PAD_PRESSED)
	{

		u8 willPlay = 0;

		for (int i = 0; i < 6; i++)
		{
			if (m_PlayerType[i])
			{
				willPlay++;
			}
		}

		if (willPlay)
		{
			m_PlayerSetup[0] = 0;
			for (int i = 1; i < 7; i++)
			{
				m_PlayerSetup[i] = m_PlayerType[i - 1];
			}

			XGM_startPlayPCM(SND_START, 0, 2);

			StateMachineChange(&GameMachineState, &AtomsGameState);
		}
		return;
	}

}


void PlayerSelectEnd()
{
	VDP_fadeOut(0, 63, 20, TRUE);

	for (int i = 0; i < 20; i++)
	{
		SPR_update();
		VDP_waitVSync();
	}


	for (int i = 0; i < 6; i++)
	{
		if (m_Players[i] != 0)
		{
			SPR_setVisibility(m_Players[i], HIDDEN);
			SPR_releaseSprite(m_Players[i]);
		}
	}

	SPR_releaseSprite(m_Cursor);	

	SPR_update();
	VDP_waitVSync();
}

SimpleState PlayerSelectState =
{
	PlayerSelectStart,
	PlayerSelectUpdate,
	PlayerSelectEnd
};