#include <genesis.h>

#include "../inc/simpleState.h"
#include "../inc/Title.h"
#include "../inc/AtomsGameState.h"
#include "../inc/PlayerSelectState.h"
#include "../inc/GameSelectState.h"

#include "../inc/GameState.h"
#include "../inc/atoms.h"
#include "../inc/PadHelper.h"


#include "../res/gfx.h"
#include "../res/sound.h"
#include "../res/sprite.h"

#include <kdebug.h>

static int timer = 160;

static int animTimer = 0;
static int frame = 0;
static u16 animPal[16];

void TitleStart()
{
	frame = 0;
	animTimer = 0;
	timer = 160;


	SYS_disableInts();

	int ind = TILE_USERINDEX;

	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	
	VDP_drawImageEx(PLAN_B, &logo_bg_1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
	ind += logo_bg_1.tileset->numTile;

	VDP_drawImageEx(PLAN_A, &logo_bg_2, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);



	VDP_setPalette(PAL0, logo_bg_1.palette->data);
	VDP_setPalette(PAL1, logo_bg_2.palette->data);

	SYS_enableInts();


	XGM_setPCM(65, gouldfish_chime, sizeof(gouldfish_chime));
	XGM_startPlayPCM(65, 0, SOUND_PCM_CH2);	

	ResetPad(&m_Pad);
}








void AnimatePalette(int shadow, int bright)
{
	// reset everything to the hidden colours
	for (int i = 1; i < 11; i += 2)
	{
		animPal[i] = title_anim.palette->data[12];
		animPal[i + 1] = title_anim.palette->data[13];
	}

	// shdaow is the after image part
	animPal[shadow] = title_anim.palette->data[14];
	animPal[shadow + 1] = title_anim.palette->data[15];
	
	// this is the main bright bit 
	animPal[bright] = title_anim.palette->data[1];
	animPal[bright + 1] = title_anim.palette->data[2];
}

void TitleUpdate()
{
	if (timer > 0)
	{
		timer--;
	}
	else if (timer == 0)
	{						
		VDP_fadeOut(0, 31, 20, FALSE);

		SND_startPlay_XGM(xgm_1);

		// disable interrupt when accessing VDP
		SYS_disableInts();

		VDP_setHilightShadow(FALSE);
		int ind = TILE_USERINDEX;
		
		VDP_drawImageEx(PLAN_B, &title_anim, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
		ind += title_anim.tileset->numTile;

		VDP_drawImageEx(PLAN_A, &title_front, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);

		VDP_setTextPalette(PAL0);
		

		SYS_enableInts();


		
		u16 palette[64];
		memset(palette, 0, 64);

		memcpy(&animPal, title_anim.palette->data, 16 * 2);
		AnimatePalette(9, 1);

		memcpy(&palette[0], animPal, 16 * 2);	
		memcpy(&palette[16], title_front.palette->data, 16 * 2);

		// fade in
		VDP_fadeIn(0, 31, palette, 20, FALSE);	

		// VDP_drawText("Press Start to Continue", 8, 24);


		timer = -1;
		animTimer = 10;
	}
	else
	{
		
		if (animTimer >= 10)
		{
			frame++;
			if (frame == 5)
			{
				frame = 0;
			}
			if (frame == 0)
			{
				AnimatePalette(9, 1);
			}
			else if (frame == 1)
			{
				AnimatePalette(1, 3);
			}
			else if (frame == 2)
			{
				AnimatePalette(3, 5);
			}
			else if (frame ==3)
			{
				AnimatePalette(5, 7);
			}
			else if (frame == 4)
			{
				AnimatePalette(7, 9);
			}
			animTimer = 0;
		}

		animTimer++;

		if (m_Pad.A == PAD_RELEASED || m_Pad.START == PAD_RELEASED)
		{
			StateMachineChange(&GameMachineState, &GameSelectState);
			return;
		}		
	}
}



void TitleEnd()
{	
	VDP_fadeOut(0, 31, 10, FALSE);
}


void TitleVInterupt()
{
	if (timer < 0)
	{
		VDP_setPalette(PAL0, animPal);
	}
}


SimpleState TitleState =
{
	TitleStart,
	TitleUpdate,
	TitleEnd,
	TitleVInterupt
};