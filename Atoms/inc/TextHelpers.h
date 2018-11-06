#ifndef _TEXT_HELPERS_H_
#define _TEXT_HELPERS_H_

#include <genesis.h>

void SetNumbers(int number, Sprite* sprites[], const int size);
void SetNumberChars(u32 number, char* charArray);
void SetNumberCharsEx(u32 number, char* charArray, int size, char defaultChar);

void DrawTextToScreen(char* buffer, int size, int x, int y, u16 tileSetStart);

#endif