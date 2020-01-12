#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

// class representiong *.ppm image
class Image
{
private:
    const int width;
    const int height;
    const int maxval;
    std::string fileHeader;
    std::ofstream file;

public:
    // constructor
    Image(const std::string fileName, const int width, const int height, const int maxColor = 256);

    // drawing methods
    void putPixel(int r, int g, int b);
    void endl() { file << std::endl; }

    // getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getMaxColor() const { return maxval; }

    // destructor
    ~Image() { file.close(); }
};
