#include <genesis.h>

#include "../inc/WinnerScreen.h"
#include "../res/gfx.h"
#include "../inc/GameState.h"
#include "../inc/AtomsGameState.h"
#include "../inc/PlayerSelectState.h"

void WinnerScreenStart()
{
	// disable interrupt when accessing VDP
	SYS_disableInts();

	// Clear anything left over from the previous state
	//VDP_resetScreen();
	//SPR_reset();


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
	
	
	VDP_drawImageEx(PLAN_A, winner, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, CPU);
	ind += winner->tileset->numTile;

	SYS_enableInts();
	u16 palette[64];

	memcpy(&palette[0], winner_background.palette->data, 16 * 2);
	memcpy(&palette[16], winner->palette->data, 16 * 2);

	

	// fade in
	VDP_fadeIn(0, (2 * 16) - 1, palette, 20, FALSE);
}

void WinnerScreenUpdate()
{
	u16 value = JOY_readJoypad(JOY_1);

	if (value & BUTTON_A || value & BUTTON_START)
	{
		//StateMachineChange(&GameMachineState, &AtomsGameState);
		StateMachineChange(&GameMachineState, &PlayerSelectState);
	}
}

void WinnerScreenEnd()
{


}


SimpleState WinnerScreenState=
{
	WinnerScreenStart,
	WinnerScreenUpdate,
	WinnerScreenEnd	
};