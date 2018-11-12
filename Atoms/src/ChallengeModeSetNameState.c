#include <genesis.h>
#include <kdebug.h>

#include "../inc/ChallengeModeSetNameState.h"

#include "../inc/GameState.h"
#include "../inc/TextHelpers.h"


#include "../inc/Prototype2.h"
#include "../inc/ChallengeModeHighScoreState.h"
#include "../inc/GameSelectState.h"


#include "../res/gfx.h"
#include "../res/sprite.h"
#include "../inc/atoms.h"


static u16 m_FontStart;

static Sprite* m_Buttons[2];
static Sprite* m_Cursor;

static char m_Name[HIGHSCORE_CHARS];
static int m_Place = 0;
static int m_InitName = TRUE;
static int m_UpdatingLetter = 0;

void SetCursorPosition()
{
	SPR_setPosition(m_Cursor, 240 + (m_UpdatingLetter * 16) , 128);
}

void SetNameToScreen()
{
	DrawTextToScreen(m_Name, HIGHSCORE_CHARS - 1, 30, 13, m_FontStart);
}

void ChallengeModeSetNameStateStart()
{
	for (int i = HIGHSCORE_TOTAL - 1; i >= 0; i--)
	{
		KLog_U3("i ", i, "  m_HighScoreAmounts: ", m_HighScoreAmounts[i], "  Score ", m_Score);
		if (m_HighScoreAmounts[i] > m_Score)
		{
			m_Place = i + 1;
			break;
		}
	}
	KLog_U1("Place ",m_Place);


	if (m_InitName)
	{
		for (int i = 0; i < HIGHSCORE_CHARS - 1; i++)
		{
			m_Name[i] = 'A';
		}
		m_Name[HIGHSCORE_CHARS - 1] = 0;
		m_InitName = FALSE;
	}

	// disable interrupt when accessing VDP
	SYS_disableInts();

	SPR_reset();
	DMA_waitCompletion();
	VDP_waitVSync();


	// Set palette to black
	VDP_setPaletteColors(0, (u16*)palette_black, 64);

	int ind = TILE_USERINDEX;


	VDP_clearPlan(PLAN_A, TRUE);

	VDP_drawImageEx(PLAN_B, &NewHighScore_Background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
	ind += NewHighScore_Background.tileset->numTile;


	VDP_loadTileSet(med_font.tileset, ind, CPU);

	m_FontStart = ind;
	ind += atoms.tileset->numTile;

	VDP_fillTileMapRect(PLAN_A, TILE_ATTR_FULL(PAL1, 0, 0, 0, 0), 0, 0, 40, 28);

	m_Cursor = SPR_addSprite(&TextCursor_Sprite, 0, 0, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));

	int i = 0;
	for (i = 0; i < 2; i++)
	{
		m_Buttons[i] = SPR_addSprite(&controller, 0, 0, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
	}


	SPR_setFrame(m_Buttons[0], 4);
	SPR_setPosition(m_Buttons[0], 136, 188);

	SPR_setFrame(m_Buttons[1], 3);
	SPR_setPosition(m_Buttons[1], 242, 188);

	VDP_drawText("Change", 20, 24);
	VDP_drawText("Letter", 20, 25);
	
	VDP_drawText("Finish", 34, 24);	

	SetCursorPosition();

	SPR_update();


	SYS_enableInts();



	char place[2];
	SetNumberCharsEx(m_Place + 1, place, 2, '0');
	DrawTextToScreen(place, 1, 2, 13, m_FontStart);


	char number[11];
	SetNumberCharsEx(m_Score, number, 11, '0');
	DrawTextToScreen(number, 10, 8, 13, m_FontStart);


	SetNameToScreen();


	u16 palette[64];
	memset(palette, 0, 64);


	memcpy(&palette[0], NewHighScore_Background.palette->data, 16 * 2);
	memcpy(&palette[16], controller.palette->data, 16 * 2);
	memcpy(&palette[32], med_font.palette->data, 16 * 2);


	
	// fade in
	VDP_fadeIn(0, (4 * 16) - 1, palette, 20, FALSE);

	ResetPad(&m_Pad);
}


void ChallengeModeSetNameStateUpdate()
{
	if (m_Pad.Left == PAD_RELEASED)
	{
		m_UpdatingLetter--;
		if (m_UpdatingLetter < 0)
		{
			m_UpdatingLetter = HIGHSCORE_CHARS - 1;
		}
		SetCursorPosition();

	}
	else if (m_Pad.Right == PAD_RELEASED)
	{
		m_UpdatingLetter++;
		if (m_UpdatingLetter == HIGHSCORE_CHARS-1)
		{
			m_UpdatingLetter = 0;
		}
		SetCursorPosition();
	}
	else if (m_Pad.Up == PAD_RELEASED)
	{
		u8 letter = m_Name[m_UpdatingLetter];
		letter++;


		if (letter > 90)
		{
			// Wrap round to space
			letter = 32;
		}
		else if (letter < 48 && letter > 32)
		{
			// Jump from ! to 0
			letter = 48;
		}
		else if (letter < 65 && letter > 57)
		{
			// Jump from : A 
			letter = 65;
		}


		m_Name[m_UpdatingLetter] = letter;
		SetNameToScreen();
	}
	else if (m_Pad.Down == PAD_RELEASED)
	{
		u8 letter = m_Name[m_UpdatingLetter];
		letter--;

		if (letter < 32)
		{
			letter = 90;
		}
		else if (letter < 48 && letter > 32)
		{
			letter = 32;
		}
		else if (letter < 65 && letter > 57)
		{
			letter = 57;
		}

		m_Name[m_UpdatingLetter] = letter;
		SetNameToScreen();
	}
	else if (m_Pad.START == PAD_RELEASED)
	{
		StateMachineChange(&GameMachineState, &ChallengeModeHighScoreState);
	}
}


void ChallengeModeSetNameStateEnd()
{
	// Update the highscore table

	for (int i = 0; i < HIGHSCORE_TOTAL; i++)
	{
		KLog_U2(m_HighScoreNames[i], i, " score : ", m_HighScoreAmounts[i]);
	}

	for (int i = HIGHSCORE_TOTAL - 1 ;i > m_Place; i--)
	{
		m_HighScoreAmounts[i] = m_HighScoreAmounts[i - 1];
		strcpy(m_HighScoreNames[i], m_HighScoreNames[i - 1]);
	}
	m_HighScoreAmounts[m_Place] = m_Score;
	strcpy(m_HighScoreNames[m_Place], m_Name);


	for (int i = 0; i < HIGHSCORE_TOTAL; i++)
	{
		KLog_U2(m_HighScoreNames[i], i, " score : ", m_HighScoreAmounts[i]);
	}


	SaveScores();


	VDP_fadeOut(0, 63, 10, FALSE);

	SYS_disableInts();

	SPR_releaseSprite(m_Buttons[1]);
	SPR_releaseSprite(m_Buttons[0]);

	VDP_clearTextArea(0, 24, 40, 2);

	SYS_enableInts();
}


SimpleState ChallengeModeSetNameState =
{
	ChallengeModeSetNameStateStart,
	ChallengeModeSetNameStateUpdate,
	ChallengeModeSetNameStateEnd,
	NULL
};