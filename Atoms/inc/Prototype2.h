#ifndef PROTO2_SCREEN_STATE
#define PROTO2_SCREEN_STATE

#include "SimpleState.h"



extern u32 m_Score;
extern int m_CurrentLevel;


void Protype2ScreenStart();
void Protype2ScreenUpdate();
void Protype2ScreenEnd();


extern SimpleState Protype2ScreenState;


#endif