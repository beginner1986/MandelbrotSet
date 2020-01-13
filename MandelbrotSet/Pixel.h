#pragma once

#include <string>

class Pixel
{
private:
	int r;
	int g;
	int b;

public:
	Pixel() : r(0), g(0), b(0) {};
	Pixel(int r, int g, int b) : r(r), g(g), b(b) {};

	void setR(int r) { this->r = r; }
	void setG(int g) { this->g = g; }
	void setB(int b) { this->b = b; }

	int getR() const { return r; }
	int getG() const { return g; }
	int getB() const { return b; }

	std::string toString() 
	{ 
		return std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b); 
	}
};

