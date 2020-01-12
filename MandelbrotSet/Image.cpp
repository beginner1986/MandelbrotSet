#include "Image.h"

/*
Each PPM image consists of the following:

    1. A "magic number" for identifying the file type. A ppm image's magic number is
        the two characters "P3".
    2. Whitespace (blanks, TABs, CRs, LFs).
    3. A width, formatted as ASCII characters in decimal.
    4. Whitespace.
    5. A height, again in ASCII decimal.
    6. Whitespace.
    7. The maximum color value (Maxval), again in ASCII decimal. Must be less than 65536
        and more than zero.
    8. A single whitespace character (usually a newline).
    9. A raster of Height rows, in order from top to bottom. Each row consists of Width pixels,
        in order from left to right. Each pixel is a triplet of red, green, and blue samples,
        in that order. Each sample is represented in pure binary by either 1 or 2 bytes.
        If the Maxval is less than 256, it is 1 byte. Otherwise, it is 2 bytes. The most
        significant byte is first.
*/

Image::Image(std::string fileName, int width, int height, int maxColor)
    : width(width), height(height), maxval(maxColor)
{
    // check file extension in lower case
    std::string extension;
    if (fileName.size() > 4)
    {
        extension = fileName.substr(fileName.size() - 5, 4);
        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](char c) { return std::tolower(c); }
        );
    }
    else
        extension = fileName;

    // if not included in file name fix it
    if (extension != ".ppm")
        fileName += ".ppm";

    // open the file
    file.open(fileName, std::ofstream::out);

    if (!file)
    {
        std::cout << "Cannot open file: " << fileName << std::endl;
        exit(0);
    }

    // create file header string
    this->fileHeader = "P3 "        // 1, 2
        + std::to_string(width)     // 3
        + " "                       // 4
        + std::to_string(height)    // 5
        + " "                       // 6
        + std::to_string(maxval)    // 7
        + '\n';                     // 8

    // save the header to the file
    file << fileHeader;
}

void Image::putPixel(int r, int g, int b)
{
    file << r << " " << g << " " << b << " ";
}
