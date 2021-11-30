#include <genesis.h>

#include "../inc/atoms.h"
#include "../inc/PlayerSelectState.h"
#include "../res/gfx.h"
#include "../res/sound.h"
#include "../res/sprite.h"

#include "../inc/simpleState.h"
#include "../inc/PlayerSelectState.h"

#include "../inc/Prototype2.h"
#include "../inc/Prototype2GameOverState.h"
#include "../inc/ChallengeModeHighScoreState.h"

#include "../inc/GameState.h"
#include "../inc/PadHelper.h"

#include "../inc/PadHelper.h"

#define SND_START 66
void GameSelectStateStart()
{
//	SYS_disableInts();

	int ind = TILE_USERINDEX;
	
	VDP_drawImageEx(PLAN_A, &GameSelect, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, DMA_QUEUE);
	ind += GameSelect.tileset->numTile;	

//	SYS_enableInts();
	

	u16 palette[64];
	memset(palette, 0, 128);

	memcpy(&palette[0], GameSelect.palette->data, 16 * 2);
	
	XGM_setPCM(SND_START, sound_4, sizeof(sound_4));

	// fade in
	VDP_fadeIn(0, 15, palette, 20, FALSE);		

	ResetPad(&m_Pad);
}



void GameSelectStateUpdate()
{
	if (m_Pad.A == PAD_RELEASED)
	{
		XGM_startPlayPCM(SND_START, 0, 2);
		StateMachineChange(&GameMachineState, &PlayerSelectState);
	}
	else if (m_Pad.C == PAD_RELEASED)
	{
		XGM_startPlayPCM(SND_START, 0, 2);
		StateMachineChange(&GameMachineState, &ChallengeModeHighScoreState);
	}
}

void GameSelectStateEnd()
{
	VDP_fadeOut(0, 31, 10, FALSE);
}


SimpleState GameSelectState =
{
	GameSelectStateStart,
	GameSelectStateUpdate,
	GameSelectStateEnd,
	NULL
};