#include <genesis.h>
#include <kdebug.h>
#include "../inc/Prototype2GameOverState.h"


#include "../inc/GameState.h"
#include "../inc/TextHelpers.h"


#include "../inc/Prototype2.h"
#include "../inc/ChallengeModeHighScoreState.h"
#include "../inc/GameSelectState.h"


#include "../res/gfx.h"
#include "../res/sprite.h"
#include "../inc/atoms.h"


static int m_Waiter;
static u16 m_FontStart;

static Sprite* m_Buttons[2];

void Prototype2GameOverStateStart()
{
	// disable interrupt when accessing VDP
	SYS_disableInts();
	
	SPR_reset();
	DMA_waitCompletion();
	VDP_waitVSync();


	// Set palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	int ind = TILE_USERINDEX;


	VDP_clearPlan(PLAN_A, TRUE);
	
	VDP_drawImageEx(PLAN_B, &GameOver_Background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
	ind += GameOver_Background.tileset->numTile;


	VDP_loadTileSet(med_font.tileset, ind, CPU);

	m_FontStart = ind;
	ind += atoms.tileset->numTile;

	VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, 0), 0, 0, 40, 28);



	int i = 0;
	for (i = 0; i < 2; i++)
	{
		m_Buttons[i] = SPR_addSprite(&controller, 0, 0, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
	}


	SPR_setFrame(m_Buttons[0], 1);
	SPR_setPosition(m_Buttons[0], 136, 188);

	SPR_setFrame(m_Buttons[1], 3);
	SPR_setPosition(m_Buttons[1], 242, 188);

	VDP_drawText("to", 20, 24);
	VDP_drawText("menu", 20, 25);

	VDP_drawText("try", 34, 24);
	VDP_drawText("again", 34, 25);

	SPR_update();

	
	SYS_enableInts();
	



	char number[11];
	SetNumberCharsEx(m_Score, number,11, '0');
	DrawTextToScreen(number, 10, 10, 11, m_FontStart);

	char level[3];
	SetNumberCharsEx(m_CurrentLevel, level, 3, '0');
	DrawTextToScreen(level, 3, 18, 20, m_FontStart);


	u16 palette[64];
	memset(palette, 0, 64);


	memcpy(&palette[0], GameOver_Background.palette->data, 16 * 2);
	memcpy(&palette[16], controller.palette->data, 16 * 2);
	memcpy(&palette[32], med_font.palette->data, 16 * 2);


	// fade in
	VDP_fadeIn(0, (4 * 16) - 1, palette, 20, FALSE);

	ResetPad(&m_Pad);
	m_Waiter = 40;
}


void Prototype2GameOverStateUpdate()
{
	if (m_Waiter > 0)
	{
		m_Waiter--;
	}
	else
	{
		if (m_Pad.A == PAD_RELEASED)
		{
			StateMachineChange(&GameMachineState, &ChallengeModeHighScoreState);
			return;
		}
		else if (m_Pad.START == PAD_RELEASED)
		{
			StateMachineChange(&GameMachineState, &Protype2ScreenState);
			return;
		}
		else if (m_Pad.B == PAD_RELEASED)
		{
			StateMachineChange(&GameMachineState, &GameSelectState);
		}
	}

}


void Prototype2GameOverStateEnd()
{
	VDP_fadeOut(0, 63, 10, FALSE);

	SYS_disableInts();

	SPR_releaseSprite(m_Buttons[1]);
	SPR_releaseSprite(m_Buttons[0]);


	VDP_clearTextArea(0, 24, 40, 2);

	SYS_enableInts();

}


SimpleState Prototype2GameOverState =
{
	Prototype2GameOverStateStart,
	Prototype2GameOverStateUpdate,
	Prototype2GameOverStateEnd,
	NULL
};
