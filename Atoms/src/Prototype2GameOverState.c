#include <genesis.h>
#include <kdebug.h>

#include "../inc/Prototype2GameOverState.h"
#include "../inc/Prototype2.h"
#include "../inc/GameState.h"
#include "../inc/GameSelectState.h"


#include "../res/gfx.h"
#include "../res/sprite.h"
#include "../inc/atoms.h"


static Sprite* m_ScoreSpr[10];
static Sprite* m_LevelSpr[2];

static int m_Waiter;




void Prototype2GameOverStateStart()
{
	// disable interrupt when accessing VDP
	SYS_disableInts();


	// Set palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	int ind = TILE_USERINDEX;


	//VDP_setPalette(PAL0, ingame_back.palette->data);
	VDP_drawImageEx(PLAN_A, &GameOver_background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
	ind += GameOver_background.tileset->numTile;


	SYS_enableInts();
	
	for (int i = 0; i < 10; i++)
	{
		m_ScoreSpr[i] = SPR_addSprite(&NumbersBig, (i * 26) + 40, 102, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
	}
	
	for (int i = 0; i < 2; i++)
	{
		m_LevelSpr[i] = SPR_addSprite(&NumbersBig, (i * 26) + 146, 180, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
	}


	u16 palette[64];
	memset(palette, 0, 64);


	memcpy(&palette[0], GameOver_background.palette->data, 16 * 2);
	memcpy(&palette[16], NumbersBig.palette->data, 16 * 2);
		
	SetNumbers(m_Score, m_ScoreSpr, 10);
	SetNumbers(m_CurrentLevel+1, m_LevelSpr, 2);
	SPR_update();

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
		if (m_Pad.A == PAD_RELEASED || m_Pad.START == PAD_RELEASED)
		{
			StateMachineChange(&GameMachineState, &GameSelectState);
		}
	}

}


void Prototype2GameOverStateEnd()
{
	VDP_fadeOut(0, 31, 10, FALSE);

	for (int i = 0; i < 10; i++)
	{
		if (i < 2)
		{
			SPR_releaseSprite(m_LevelSpr[i]);
		}
		SPR_releaseSprite(m_ScoreSpr[i]);
	}
	
	SPR_reset();
	SPR_update();

}


SimpleState Prototype2GameOverState =
{
	Prototype2GameOverStateStart,
	Prototype2GameOverStateUpdate,
	Prototype2GameOverStateEnd
};
