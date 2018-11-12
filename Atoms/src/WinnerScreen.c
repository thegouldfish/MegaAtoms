#include <genesis.h>

#include "../inc/WinnerScreen.h"
#include "../res/gfx.h"
#include "../inc/GameState.h"
#include "../inc/AtomsGameState.h"
#include "../inc/PlayerSelectState.h"
#include "../inc/PadHelper.h"
#include "../inc/atoms.h"


static u8 m_Delay = 0;
static u8 m_DelayTime = 20;

void WinnerScreenStart()
{
	// disable interrupt when accessing VDP
	SYS_disableInts();

	// Set palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	int ind = TILE_USERINDEX;


	//VDP_setPalette(PAL0, ingame_back.palette->data);
	VDP_drawImageEx(PLAN_B, &winner_background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, DMA);
	ind += winner_background.tileset->numTile;

	
	const Image* winner = &winner_player6;
	switch (m_WinningPlayer)
	{
		case 1:
			if (m_PlayerSetup[m_WinningPlayer] == 1)
				winner = &winner_player1;
			else if(m_PlayerSetup[m_WinningPlayer] == 2)
				winner = &winner_ai1;
			break;


		case 2:
			if (m_PlayerSetup[m_WinningPlayer] == 1)
				winner = &winner_player2;
			else if (m_PlayerSetup[m_WinningPlayer] == 2)
				winner = &winner_ai2;
			break;


		case 3:
			if (m_PlayerSetup[m_WinningPlayer] == 1)
				winner = &winner_player3;
			else if (m_PlayerSetup[m_WinningPlayer] == 2)
				winner = &winner_ai3;
			break;


		case 4:
			if (m_PlayerSetup[m_WinningPlayer] == 1)
				winner = &winner_player4;
			else if (m_PlayerSetup[m_WinningPlayer] == 2)
				winner = &winner_ai4;
			break;


		case 5:
			if (m_PlayerSetup[m_WinningPlayer] == 1)
				winner = &winner_player5;
			else if (m_PlayerSetup[m_WinningPlayer] == 2)
				winner = &winner_ai5;
			break;


		case 6:
			if (m_PlayerSetup[m_WinningPlayer] == 1)
				winner = &winner_player6;
			else if (m_PlayerSetup[m_WinningPlayer] == 2)
				winner = &winner_ai6;
			break;


		default:
			break;
	}
	
	
	VDP_drawImageEx(PLAN_A, winner, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, DMA);
	ind += winner->tileset->numTile;

	SYS_enableInts();
	u16 palette[64];

	memcpy(&palette[0], winner_background.palette->data, 16 * 2);
	memcpy(&palette[16], winner->palette->data, 16 * 2);

	

	// fade in
	VDP_fadeIn(0, (2 * 16) - 1, palette, 20, FALSE);

	m_Delay = 0;
	ResetPad(&m_Pad);
}

void WinnerScreenUpdate()
{
	m_Delay++;

	if (m_Delay > m_DelayTime)
	{
		if (m_Pad.A == PAD_RELEASED || m_Pad.START == PAD_RELEASED)
		{
			StateMachineChange(&GameMachineState, &PlayerSelectState);
		}
	}
}

void WinnerScreenEnd()
{
	VDP_fadeOut(0, 63, 10, FALSE);
}


SimpleState WinnerScreenState=
{
	WinnerScreenStart,
	WinnerScreenUpdate,
	WinnerScreenEnd,
	NULL
};