// compressionPalette.h : Compression palette

#include <stdio.h>
#include <vector>
#include <cmath>

#include "ImageBase.h"

struct Palette {
	int r, g, b;
};

void compressionPalette(ImageBase src, int height, int width, char nom[150]);