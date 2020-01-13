#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

#include "Pixel.h"

// class representiong *.ppm image
class Image
{
private:
    const int width;
    const int height;
    const int maxval;
    std::string fileHeader;
    std::ofstream file;
    Pixel* content;

public:
    // constructor
    Image(const std::string fileName, const int width, const int height, const int maxColor = 256);

    // drawing methods
    void saveFile();

    // getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getMaxColor() const { return maxval; }
    Pixel getPixel(int x, int y) const { return content[x + y * width]; }

    // setters
    void setPixel(int x, int y, int r, int g, int b);
    void setPixel(int x, int y, Pixel p);

    // destructor
    ~Image();

private:
    std::ofstream openFile(std::string fileName);
};
