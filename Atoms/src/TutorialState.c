#include <genesis.h>
#include <kdebug.h>

#include "../inc/atoms.h"
#include "../inc/TutorialState.h"
#include "../res/gfx.h"
#include "../inc/GameState.h"
#include "../inc/PlayerSelectState.h"
#include "../inc/PadHelper.h"
#include "../inc/GamePlay.h"

int m_PreviousState = 0;


StateMachine m_TutorialMachine;



void TutorialDrawAtom(int player, int x, int y, int size)
{
	int row = 0;
	int column = 0;

	if (player < 0)
	{
		player = 0;
		size = 0;
	}


	int startingFrame = 0;

	if (size == 4)
	{
		startingFrame = 37;
	}
	else
	{
		startingFrame = size * 4;
	}
	if (size == 5)
	{
		startingFrame = 41;
	}


	u16 t = (startingFrame * 3 * 6 * 3) + (player * 3);
	for (row = 0; row < 3; row++)
	{
		for (column = 0; column < 3; column++)
		{

			u16 tile = m_AtomTileStart;
			if (size != 0)
			{
				tile += atoms.map->tilemap[t];
			}




			VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, tile), column + (x * 3) + 7, row + (y * 3) + 3);
			t++;
		}
		t += 15;
	}
}




SimpleState Part1;
SimpleState Part2;
SimpleState Part3;
SimpleState Part4;
SimpleState Part5;
SimpleState Part6;
SimpleState Part7;
SimpleState Part8;
SimpleState Part9;
SimpleState Part10;
SimpleState Part11;

void Part1Start()
{
	// Set palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	SYS_disableInts();

	int ind = TILE_USERINDEX;

	VDP_clearTextArea(0, 0, 40, 28);

	//VDP_setPalette(PAL0, ingame_back.palette->data);
	VDP_drawImageEx(PLAN_B, &tutorial_backing, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, DMA_QUEUE);
	ind += tutorial_backing.tileset->numTile;

	VDP_setPalette(PAL0, tutorial_backing.palette->data);
	

	SYS_enableInts();

	VDP_setVerticalScroll(PLAN_A, -4);
	VDP_setHorizontalScroll(PLAN_A, 4);


	VDP_drawText("Welcome to MegaAtoms!", 14, 5);
	VDP_drawText("I'm professor Loggy", 14, 7);
	VDP_drawText("I will teach you", 14, 8);
	VDP_drawText("how to play!", 14, 9);

	VDP_drawText("Press A to move on", 14, 13);
	VDP_drawText("Press B to move back", 14, 14);
	VDP_drawText("Press C to quit", 14, 15);

}	

void Part1Update()
{
	if (m_Pad.A == PAD_PRESSED)
	{
		m_PreviousState = 1;
		StateMachineChange(&m_TutorialMachine, &Part2);
	}
}	

void Part1End()
{
}

SimpleState Part1 =
{
	Part1Start,
	Part1Update,
	Part1End
};	






void Part2Start()
{
	

	if (m_PreviousState == 1)
	{
		// Set palette to black
		VDP_setPaletteColors(0, (u16*)palette_black, 64);

		SYS_disableInts();

		int ind = TILE_USERINDEX;

		VDP_clearTextArea(0, 0, 40, 28);

		//VDP_setPalette(PAL0, ingame_back.palette->data);
		VDP_drawImageEx(PLAN_B, &tutorial_backing_2, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, DMA_QUEUE);
		ind += tutorial_backing_2.tileset->numTile;

		VDP_setPalette(PAL0, tutorial_backing_2.palette->data);


		VDP_setPalette(PAL1, atoms.palette->data);
		VDP_loadTileSet(atoms.tileset, ind, DMA_QUEUE);

		m_AtomTileStart = ind;
		ind += atoms.tileset->numTile;

		VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, m_AtomTileStart), 0, 0, 40, 28);


		SYS_enableInts();
	}

	VDP_clearTextArea(0, 0, 40, 28);
	VDP_drawText("This is the game grid",4,25);
	VDP_drawText("where all the action takes place", 4, 26);
}	

void Part2Update()
{
	if (m_Pad.A == PAD_PRESSED)
	{
		m_PreviousState = 2;
		StateMachineChange(&m_TutorialMachine, &Part3);
	}
	else if (m_Pad.B == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part1);
	}
}	

void Part2End()
{
}	

SimpleState Part2 =
{
	Part2Start,
	Part2Update,
	Part2End
};	




void Part3Start()
{
	VDP_clearTextArea(0, 0, 40, 28);
	VDP_drawText("each player takes it in turn", 4, 25);
	VDP_drawText("to place atoms on the grid", 4, 26);

}	

void Part3Update()
{
	if (m_Pad.A == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part4);
	}
	else if (m_Pad.B == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part2);
	}
}	
void Part3End()
{
}	

SimpleState Part3 =
{
	Part3Start,
	Part3Update,
	Part3End
};


int m_Part4Counter = 0;
void Part4Start()
{
	VDP_clearTextArea(0, 0, 40, 28);
	VDP_drawText("you place an atom on an empty", 4, 25);
	VDP_drawText("square or can grow your own", 4, 26);

	TutorialDrawAtom(1, 3, 3, 1);	
	TutorialDrawAtom(3, 4, 4, 2);
	
	TutorialDrawAtom(0, 4, 3, 0);
	m_Part4Counter = 0;
}	
void Part4Update()
{
	if (m_Pad.A == PAD_PRESSED)
	{
		m_Part4Counter++;
		if (m_Part4Counter == 4)
		{
			StateMachineChange(&m_TutorialMachine, &Part5);
		}
		else
		{
			TutorialDrawAtom(0, 4, 3, m_Part4Counter);
		}		
	}
	else if (m_Pad.B == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part3);
	}
}	
void Part4End()
{
}	

SimpleState Part4 =
{
	Part4Start,
	Part4Update,
	Part4End
};	



void Part5Start()
{
	VDP_clearTextArea(0, 0, 40, 28);
	VDP_drawText("When a square has too many", 4, 25);
	VDP_drawText("atoms it will explode", 4, 26);

	TutorialDrawAtom(1, 3, 3, 1);
	TutorialDrawAtom(3, 4, 4, 2);
	TutorialDrawAtom(0, 4, 3, 4);
}	
void Part5Update()
{
	if (m_Pad.A == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part6);
	}
	else if (m_Pad.B == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part4);
	}
}	
void Part5End()
{
}	
SimpleState Part5 =
{
	Part5Start,
	Part5Update,
	Part5End
};	




void Part6Start()
{
	VDP_clearTextArea(0, 0, 40, 28);
	VDP_drawText("This will capture the squares", 4, 25);
	VDP_drawText("around it and grow them", 4, 26);

	TutorialDrawAtom(0, 3, 3, 2);
	TutorialDrawAtom(0, 4, 4, 3);
	TutorialDrawAtom(0, 4, 3, 0);
}

void Part6Update()
{
	if (m_Pad.A == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part7);
	}
	else if (m_Pad.B == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part5);
	}
}

void Part6End()
{
}

SimpleState Part6 =
{
	Part6Start,
	Part6Update,
	Part6End
};




void Part7Start()
{
	VDP_clearTextArea(0, 0, 40, 28);
	VDP_drawText("the number of atoms needed", 4, 25);
	VDP_drawText("to explode can be seen above", 4, 26);

	int x, y = 0;
	for (y = 0; y<7; y++)
	{
		for (x = 0; x<10; x++)
		{
			if (y == 0 && x == 0)
			{
				TutorialDrawAtom(0, x, y, 2);
			}
			else if (y == 0 && x == 9)
			{
				TutorialDrawAtom(0, x, y, 2);
			}
			else if (y == 0)
			{
				TutorialDrawAtom(0, x, y, 3);
			}
			else if (y == 6 && x == 0)
			{
				TutorialDrawAtom(0, x, y, 2);
			}
			else if (x == 0)
			{
				TutorialDrawAtom(0, x, y, 3);
			}
			else if (y == 6 && x == 9)
			{
				TutorialDrawAtom(0, x, y, 2);
			}
			else if (y == 6)
			{
				TutorialDrawAtom(0, x, y, 3);
			}
			else if (x == 9)
			{
				TutorialDrawAtom(0, x, y, 3);
			}
			else
			{
				TutorialDrawAtom(0, x, y, 5);
			}
		}
	}

}	

void Part7Update()
{
	if (m_Pad.A == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part8);
	}
	else if (m_Pad.B == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part6);
	}
}	
void Part7End()
{
}	
SimpleState Part7 =
{
	Part7Start,
	Part7Update,
	Part7End
};


void Part8Start()
{
	VDP_clearTextArea(0, 0, 40, 28);
	VDP_drawText("exploding atoms is the only", 4, 25);
	VDP_drawText("way to capture opponents atoms", 4, 26);

}	
void Part8Update()
{
	if (m_Pad.A == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part9);
	}
	else if (m_Pad.B == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part7);
	}
}	
void Part8End()
{
}	

SimpleState Part8 =
{
	Part8Start,
	Part8Update,
	Part8End
};	



void Part9Start()
{
	VDP_clearTextArea(0, 0, 40, 28);
	VDP_drawText("you need to do this as you win", 4, 25);
	VDP_drawText("when you are the last player", 4, 26);
}	
void Part9Update()
{
	if (m_Pad.A == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part10);
	}
	else if (m_Pad.B == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part8);
	}
}	
void Part9End()
{
}	
SimpleState Part9 =
{
	Part9Start,
	Part9Update,
	Part9End
};	



void Part10Start()
{
	VDP_clearTextArea(0, 0, 40, 28);
	VDP_drawText("Use chain reactions to capture", 4, 25);
	VDP_drawText("large areas in a single turn", 4, 26);
}
void Part10Update()
{
	if (m_Pad.A == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part11);
	}
	else if (m_Pad.B == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part9);
	}
}
void Part10End()
{
}
SimpleState Part10 =
{
	Part10Start,
	Part10Update,
	Part10End
};



void Part11Start()
{
	VDP_clearTextArea(0, 0, 40, 28);
	VDP_drawText("Press A to start again", 4, 25);
	VDP_drawText("Press C to leave", 4, 26);
}
void Part11Update()
{
	if (m_Pad.A == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part1);
	}
	else if (m_Pad.B == PAD_PRESSED)
	{
		StateMachineChange(&m_TutorialMachine, &Part10);
	}
}
void Part11End()
{
}
SimpleState Part11 =
{
	Part11Start,
	Part11Update,
	Part11End
};












void TutorialStateStart()
{
	StateMachineStart(&m_TutorialMachine, &Part1);
	ResetPad(&m_Pad);
}


void TutorialStateUpdate()
{
	if (m_Pad.C == PAD_PRESSED)
	{
		StateMachineChange(&GameMachineState, &PlayerSelectState);
		return;
	}

	StateMachineUpdate(&m_TutorialMachine);
}


void TutorialStateEnd()
{
	VDP_fadeOut(0, 63, 20, FALSE);

	VDP_setVerticalScroll(PLAN_A, 0);
	VDP_setHorizontalScroll(PLAN_A, 0);
}


SimpleState TutorialState =
{
	TutorialStateStart,
	TutorialStateUpdate,
	TutorialStateEnd,
	NULL
};