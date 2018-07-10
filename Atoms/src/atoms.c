#include <genesis.h>
#include <kdebug.h>

#include "../inc/atoms.h"

Pad m_Pad;


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
