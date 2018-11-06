#include <genesis.h>
#include <kdebug.h>

#include "../inc/atoms.h"
#include "../inc/ChallengeModeHighScoreState.h"
#include "../inc/GameSelectState.h"
#include "../inc/ChallengeModeTutorialState.h"

#include "../inc/Prototype2.h"
#include "../inc/GameState.h"
#include "../inc/PadHelper.h"

#include "../inc/PadHelper.h"
#include "../inc/TextHelpers.h"

#include "../res/gfx.h"
#include "../res/sprite.h"


static Sprite* m_Buttons[2];

static u16 m_FontStart;





void ChallengeModeHighScoreStateStart()
{
	SYS_disableInts();

	SPR_reset();
	DMA_waitCompletion();
	VDP_waitVSync();

	
	// 6 x 7
	

	

	// Set palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);
	int ind = TILE_USERINDEX;

	VDP_clearPlan(PLAN_A, TRUE);
	VDP_drawImageEx(PLAN_B, &HighScoreTable_Background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
	ind += HighScoreTable_Background.tileset->numTile;
	
	VDP_loadTileSet(med_font.tileset, ind, CPU);

	m_FontStart = ind;
	ind += atoms.tileset->numTile;

	VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, 0), 0, 0, 40, 28);


	
	KDebug_Alert("buttons");
	int i = 0;
	for (i = 0; i < 2; i++)
	{
		m_Buttons[i] = SPR_addSprite(&controller, 0, 0, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
		KDebug_AlertNumber(i);
		
		if (m_Buttons[i] != NULL)
		{
			KDebug_AlertNumber(m_Buttons[i]->VDPSpriteIndex);
		}
		else
		{
			KDebug_Alert("NULL");
		}
	}


	SPR_setFrame(m_Buttons[0], 2);
	SPR_setPosition(m_Buttons[0], 136, 188);

	SPR_setFrame(m_Buttons[1], 3);	
	SPR_setPosition(m_Buttons[1], 242, 188);

	VDP_drawText("for", 20, 24);
	VDP_drawText("tutorial", 20, 25);

	VDP_drawText("to", 34, 24);
	VDP_drawText("start", 34, 25);
	
	SYS_enableInts();

	SPR_update();


	u16 palette[64];
	memset(palette, 0, 64);

	
	memcpy(&palette[0], HighScoreTable_Background.palette->data, 16 * 2);
	memcpy(&palette[16], playerselect_stands_background.palette->data, 16 * 2);
	memcpy(&palette[32], med_font.palette->data, 16 * 2);




	
	char number[11];

	char place[2] = "1";
	for (int i = 0; i < HIGHSCORE_TOTAL; i++)
	{
		place[0] = 49 + i;
		

		SetNumberChars(m_HighScoreAmounts[i], number);

		DrawTextToScreen(place, 2, 3, 7 + (i * 3), m_FontStart);
		DrawTextToScreen(number, 10, 6, 7 + (i * 3), m_FontStart);
		DrawTextToScreen(m_HighScoreNames[i], HIGHSCORE_CHARS, 30, 7 + (i * 3), m_FontStart);
	}
	
	// fade in
	VDP_fadeIn(0, 47, palette, 20, FALSE);

	ResetPad(&m_Pad);
}


void ChallengeModeHighScoreStateUpdate()
{
	if (m_Pad.START == PAD_RELEASED)
	{
		//XGM_startPlayPCM(SND_START, 0, 2);

		StateMachineChange(&GameMachineState, &Protype2ScreenState);
	}
	else if (m_Pad.B == PAD_RELEASED)
	{
		StateMachineChange(&GameMachineState, &GameSelectState);
	}
	else if (m_Pad.C == PAD_RELEASED)
	{
		StateMachineChange(&GameMachineState, &ChallengeModeTutorialState);
	}
}


void ChallengeModeHighScoreStateEnd()
{
	VDP_fadeOut(0, 63, 20, FALSE);

	SYS_disableInts();

	SPR_releaseSprite(m_Buttons[1]);
	SPR_releaseSprite(m_Buttons[0]);

	
	VDP_clearTextArea(0, 24, 40, 2);

	SYS_enableInts();

	u16 count = SPR_getNumActiveSprite();
	KDebug_Alert("ChallengeModeHighScoreStateEnd - active sprite count");
	KDebug_AlertNumber(count);
}


SimpleState ChallengeModeHighScoreState =
{
	ChallengeModeHighScoreStateStart,
	ChallengeModeHighScoreStateUpdate,
	ChallengeModeHighScoreStateEnd
};