#include <iostream>
#include "Image.h"

int main()
{
	// Image class test
	Image image("img", 800, 600);

	for (int i = 0; i < 800; i++)
	{
		for (int j = 0; j < 600; j++)
		{
			int r = i % image.getMaxColor();
			int g = j % image.getMaxColor();
			int b = (i + j) % image.getMaxColor();
			image.putPixel(r, g, b);
		}
		image.endl();
	}

	return 0;
}