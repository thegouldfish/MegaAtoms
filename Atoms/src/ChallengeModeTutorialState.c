#include <genesis.h>

#include "../inc/ChallengeModeTutorialState.h"
#include "../inc/PadHelper.h"
#include "../inc/GamePlay.h"
#include "../inc/atoms.h"
#include "../inc/ChallengeModeHighScoreState.h"
#include "../inc/AtomsGameState.h"
#include "../inc/GameState.h"

#include "../res/gfx.h"

typedef struct
{	
	char* HelpText[16];
	int TextCount;
}HelpScreen;


HelpScreen m_Screens[] =
{
	{
		{
			"Welcome to MegaAtoms!",
			"",
			"I'm professor Loggy",
			"This is Challenge mode",
			"Your Goal is simple",
			"Get A high score",
			"",
			"the basic gameplay is",
			"the same as classic mode",
			"Check out that tutorial",
			"to learn the basics",
			"",
			"Press A to move on",
			"Press B to move back",
			"Press C to quit",
		},
		15
	},
	{
		{
			"You get points for",
			"each atom you explode",
			"this is increased via",
			"the multiplier.",
			"",
			"The Multiplier is grows",
			"with each chain reaction",
			"that you start.",
			""
			"And resets when you",
			"Fail to start a reaction",
			"",
			"Multiplier is limited",
			"by the current level."
		},
		14
	},
	{
		{
			"To complete a level",
			"you need to explode",
			"a target number of",
			"specific atoms.",
			"",
			"this is shown on",
			"the left of the",
			"left of the screen.",
			"",
			"You need to do this",
			"within the time limit",
			"which is indicated by",
			"the bar at the bottom",
			"of the screen"
		},
		14
	},
	{
		{
			"Time is always ticking",
			"down and speeds up when",
			"you go up a level.",
			"",
			"to gain more time you",
			"can explode atoms or",
			"you get a time bonus",
			"when a level completes",
			"",
			"you can loose time when",
			"you can't place an atom",
			"or the whole screen is",
			"a single colour",
		},
		13
	},
	{
		{
			"Chain reactions are a",
			"blessing and a curse!",
			"",
			"they can give you lots",
			"of points and time",
			"but risk making it",
			"harder to complete a",
			"level or by clearing",
			"the screen which can",
			"give a time penalty.",
			"",
			"So be careful!"
		},
	11
	},
	{
		{
			"This concludes this",
			"basic tutorial on",
			"Challenge mode",
			"",
			"Have fun and try",
			"to get to the top of",
			"high score table",
			"",
			"Press C to quit"
		},
		9
	}
};


static int m_ScreenCount = 6;
static int m_CurrentScreen = 0;

void UpdateText(HelpScreen* screen)
{
	VDP_clearTextArea(0, 0, 40, 28);

	KLog_U1("TextCount : ", screen->TextCount);

	for (int i = 0; i < screen->TextCount; i++)
	{
		KLog_U1(screen->HelpText[i], i);

		VDP_drawText(screen->HelpText[i], 13, 4 + i);
	}
}

void ChallengeModeTutorialStateStart()
{
	m_CurrentScreen = 0;

	// Set palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	SYS_disableInts();

	int ind = TILE_USERINDEX;

	VDP_clearTextArea(0, 0, 40, 28);

	//VDP_setPalette(PAL0, ingame_back.palette->data);
	VDP_drawImageEx(PLAN_B, &tutorial_backing, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, DMA);
	ind += tutorial_backing.tileset->numTile;

	VDP_setPalette(PAL0, tutorial_backing.palette->data);


	SYS_enableInts();

	VDP_setVerticalScroll(PLAN_A, -4);
	VDP_setHorizontalScroll(PLAN_A, 4);


	UpdateText(&m_Screens[m_CurrentScreen]);

	ResetPad(&m_Pad);
}


void ChallengeModeTutorialStateUpdate()
{
	if (m_Pad.A == PAD_RELEASED)
	{
		m_CurrentScreen++;
		if (m_CurrentScreen >= m_ScreenCount)
		{
			m_CurrentScreen = m_ScreenCount - 1;
		}

		UpdateText(&m_Screens[m_CurrentScreen]);
	}
	else if (m_Pad.B == PAD_RELEASED)
	{
		m_CurrentScreen--;
		if (m_CurrentScreen < 0)
		{
			m_CurrentScreen = 0;
		}

		UpdateText(&m_Screens[m_CurrentScreen]);
	}
	else if (m_Pad.C == PAD_RELEASED)
	{
		StateMachineChange(&GameMachineState, &ChallengeModeHighScoreState);
	}
}


void ChallengeModeTutorialStateEnd()
{
	VDP_fadeOut(0, 63, 20, FALSE);
}


SimpleState ChallengeModeTutorialState=
{
	ChallengeModeTutorialStateStart,
	ChallengeModeTutorialStateUpdate,
	ChallengeModeTutorialStateEnd
};