#include <genesis.h>

#include "../inc/simpleState.h"
#include "../inc/Title.h"
#include "../inc/AtomsGameState.h"
#include "../inc/PlayerSelectState.h"

#include "../inc/GameState.h"

#include "../res/gfx.h"
#include "../res/sound.h"


int timer = 160;

void TitleStart()
{
	int ind = TILE_USERINDEX;

	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	VDP_setPalette(PAL1, gouldfish.palette->data);
	VDP_drawImageEx(PLAN_B, &gouldfish, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);


	XGM_setPCM(65, gouldfish_chime, sizeof(gouldfish_chime));
	XGM_startPlayPCM(65, 0, SOUND_PCM_CH2);
}



int scroll = 0;
int slow = 2;
void TitleUpdate()
{
	if (timer > 0)
	{
		timer--;
	}
	else if (timer == 0)
	{						
		VDP_fadeOut(16, 31, 20, FALSE);

		SND_startPlay_XGM(xgm_1);

		// disable interrupt when accessing VDP
		SYS_disableInts();

		VDP_setHilightShadow(FALSE);
		int ind = TILE_USERINDEX;
		
		VDP_drawImageEx(PLAN_B, &title_back, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
		ind += title_back.tileset->numTile;

		VDP_drawImageEx(PLAN_A, &title_front, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);

		VDP_setTextPalette(PAL0);
		

		SYS_enableInts();


		
		u16 palette[64];
		memset(palette, 0, 64);
		memcpy(&palette[0], title_back.palette->data, 16 * 2);
		memcpy(&palette[16], title_front.palette->data, 16 * 2);

		// fade in
		VDP_fadeIn(0, 63, palette, 20, FALSE);	

		// VDP_drawText("Press Start to Continue", 8, 24);


		timer = -1;
	}
	else
	{
		
		slow--;
		if (timer < -60)
		{
			slow = 2;
			timer = -1;
			scroll = 0;
		}
		else if (timer < -30)
		{
			if (slow == 0)
			{
				timer--;
				scroll++;
				slow = 2;
			}
		}
		else
		{
			if (slow == 0)
			{
				timer--;
				scroll--;
				slow = 2;
			}
		}
		
		
		

		VDP_setHorizontalScroll(PLAN_B, scroll);
		//VDP_setVerticalScroll(PLAN_B, scroll);

		u16 value = JOY_readJoypad(JOY_1);

		if (value & BUTTON_A || value & BUTTON_START)
		{
			//StateMachineChange(&GameMachineState, &AtomsGameState);
			StateMachineChange(&GameMachineState, &PlayerSelectState);
		}
	}
}



void TitleEnd()
{
	//VDP_clearText(8, 24, 24);


	VDP_fadeOut(16, 31, 10, FALSE);


	//VDP_fadeOut(16, 31, 20, FALSE);

	VDP_setHilightShadow(FALSE);
	

}



SimpleState TitleState =
{
	TitleStart,
	TitleUpdate,
	TitleEnd
};