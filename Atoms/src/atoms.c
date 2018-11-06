#include <genesis.h>
#include <kdebug.h>
#include <sram.h>

#include "../inc/atoms.h"

Pad m_Pad;

u32 m_HighScoreAmounts[HIGHSCORE_TOTAL];
char m_HighScoreNames[HIGHSCORE_TOTAL][HIGHSCORE_CHARS];

static u8 m_Version = 1;

void DefaultScores()
{
	m_HighScoreAmounts[0] = 20000l;
	m_HighScoreAmounts[1] = 10000l;
	m_HighScoreAmounts[2] = 2000l;
	m_HighScoreAmounts[3] = 1000l;
	m_HighScoreAmounts[4] = 200l;

	strcpy(m_HighScoreNames[0], "GFG");
	strcpy(m_HighScoreNames[1], "LGR");
	strcpy(m_HighScoreNames[2], "NSN");
	strcpy(m_HighScoreNames[3], "PUR");
	strcpy(m_HighScoreNames[4], "SGR");
}


// random range
int random_int(int min, int max)
{
	return min + random() % (max + 1 - min);
}



void SaveScores()
{
	KDebug_Alert("SRAM SAVE - Start");
	// Only load if versions match
	
	SRAM_writeByte(0, m_Version);

	u32 currentByte = 1;
	for (int i = 0; i < HIGHSCORE_TOTAL; i++)
	{
		SRAM_writeLong(currentByte, m_HighScoreAmounts[i]);
		currentByte += 4;

		for (int j = 0; j < HIGHSCORE_CHARS; j++)
		{
			SRAM_writeByte(currentByte, m_HighScoreNames[i][j]);
			currentByte++;
		}
	}

	KDebug_AlertNumber(currentByte);
	KDebug_Alert("SRAM SAVE - End");

}


u8 LoadScores()
{	
	KDebug_Alert("SRAM Load - Start");
	
	// Only load if versions match
	// this could be due to a newer version of the save data
	// or there isn't any save data
	u8 version = SRAM_readByte(0);

	KDebug_AlertNumber(version);
	
	u8 result = FALSE;
	u32 currentByte = 1;
	if (m_Version == version)
	{
		for (int i = 0; i < HIGHSCORE_TOTAL; i++)
		{
			m_HighScoreAmounts[i] = SRAM_readLong(currentByte);
			currentByte += 4;

			for (int j = 0; j < HIGHSCORE_CHARS; j++)
			{
				m_HighScoreNames[i][j] = SRAM_readByte(currentByte);
				currentByte++;
			}
		}
		result = TRUE;
	}
	else
	{
		return FALSE;
	}

	
	KDebug_AlertNumber(currentByte);
	KDebug_Alert("SRAM Load - End");

	return result;
}