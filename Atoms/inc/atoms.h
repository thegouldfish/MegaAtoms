#ifndef _ATOMS_H_
#define _ATOMS_H_



#include <genesis.h>
#include "PadHelper.h"


extern Pad m_Pad;




#define HIGHSCORE_TOTAL 5
#define HIGHSCORE_CHARS 4
extern u32 m_HighScoreAmounts[HIGHSCORE_TOTAL];
extern char m_HighScoreNames[HIGHSCORE_TOTAL][HIGHSCORE_CHARS];
void DefaultScores();

void SaveScores();
u8 LoadScores();

int random_int(int min, int max);

#endif // !_ATOMS_H_