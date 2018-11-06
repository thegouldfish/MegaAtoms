#include "../inc/TextHelpers.h"

#include "../res/gfx.h"

void SetNumbers(int number, Sprite* sprites[], const int size)
{
	int numbs[10] = { 0,0,0,0,0,0,0,0,0,0 };
	int i = size - 1;
	while (number > 0)
	{
		if (number < 10)
		{
			numbs[i] = number;
		}
		else
		{
			numbs[i] = (number % 10);
		}

		number /= 10;
		i--;
	}



	for (int i = 0; i < 10 && i < size; i++)
	{
		SPR_setFrame(sprites[i], numbs[i]);
	}
}

void SetNumberChars(u32 number, char* buff)
{
	SetNumberCharsEx(number, buff, 11, ' ');
}

void SetNumberCharsEx(u32 number, char* buff, int size, char defaultChar)
{
	for (int i = 0; i < size - 1; i++)
	{
		buff[i] = defaultChar;
	}
	buff[size - 1] = 0;


	int i = size - 2;
	do
	{
		if (number < 10)
		{
			buff[i] = number + 48;
		}
		else
		{
			buff[i] = (number % 10) + 48;
		}

		number /= 10;
		i--;
	} while (number > 0);
}




void DrawTextToScreen(char* buffer, int size, int x, int y, u16 tileSetStart)
{
	// Time for hacky code to support the limited font!
	// lets look up our ascii maps!
	int fontCount = 36;
	int fontWidth = 2;
	int fontHight = 3;
	for (int i = 0; i < size; i++)
	{
		int place = -2;
		if (buffer[i] > 47 && buffer[i] < 58)
		{
			place = (buffer[i] - 48);
		}
		else if (buffer[i] > 64 && buffer[i] < 91)
		{
			place = (buffer[i] - 65) + 10;
		}
		else if (buffer[i] == 32)
		{
			place = -1;
		}

		if (place != -2)
		{
			for (int xx = 0; xx < fontWidth; xx++)
			{
				for (int yy = 0; yy < fontHight; yy++)
				{
					int tile = 0;

					if (place != -1)
					{
						int index = (place * fontWidth) + xx + (yy * fontCount * fontWidth);
						tile = med_font.map->tilemap[index] + tileSetStart;
					}
					
					VDP_setTileMapXY(PLAN_A, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, tile), x + xx, y + yy);
				}
			}
		}
		x += fontWidth;
	}
}