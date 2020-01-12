#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

#include "Point.h"

// class representiong *.ppm image
class Image
{
private:
    const int width;
    const int height;
    const int maxval;
    std::string fileHeader;
    std::ofstream file;
    Point* content;

public:
    // constructor
    Image(const std::string fileName, const int width, const int height, const int maxColor = 256);

    // drawing methods
    void saveFile();

    // getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getMaxColor() const { return maxval; }

    // setters
    void setPixel(int x, int y, int r, int g, int b);

    // destructor
    ~Image();

private:
    std::ofstream openFile(std::string fileName);
};
