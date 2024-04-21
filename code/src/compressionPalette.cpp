// compressionPalette.cpp : Compression palette

#include "compressionPalette.h"

void compressionPalette(ImageBase src, int height, int width, char nom[150]){

    std::vector<Palette> colorsPalette;
    std::vector<Palette> palette;
    
    for (int i=0; i < height; i++){
        for (int j=0; j < width; j++){
            
            int r = src[i*3][j*3];
            int g = src[i*3][j*3+1];
            int b = src[i*3][j*3+2];

            // palette compression
            Palette color;
            color.r = r;
            color.g = g;
            color.b = b;
            colorsPalette.push_back(color);
        }
    }

    // palette initialization
    for(Palette cp : colorsPalette){
        
        bool uniquecolor = true;
        
        for(Palette p : palette){
            if(cp.r == p.r and cp.g == p.g and cp.b == p.b){
                uniquecolor = false;
                break;
            }
        }

        if(uniquecolor == true){
            palette.push_back(cp);
        }
    }

    // palette image
    ImageBase paletteImg(int(sqrt(palette.size()) + 0.5), int(sqrt(palette.size()) + 0.5), src.getColor());

    for(int i = 0; i < paletteImg.getHeight(); i++){
        for(int j = 0; j < paletteImg.getWidth(); j++){
            paletteImg[i*3][j*3]     = palette[i * paletteImg.getWidth() + j].r;
            paletteImg[i*3][j*3 + 1] = palette[i * paletteImg.getWidth() + j].g;
            paletteImg[i*3][j*3 + 2] = palette[i * paletteImg.getWidth() + j].b;
        }
    }

    paletteImg.save(nom);
}
