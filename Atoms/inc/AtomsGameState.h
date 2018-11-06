#ifndef ATOMSGAMESTATE
#define ATOMSGAMESTATE

#include "SimpleState.h"




void AtomGameStart();
void AtomGameUpdate();
void AtomGameEnd();


typedef struct
{
	int PlayerCount;
	int PlayerType[6];

} GameSetup;


extern SimpleState AtomsGameState;
extern int m_PlayerSetup[7];
extern int m_WinningPlayer;

#endif