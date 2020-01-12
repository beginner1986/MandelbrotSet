#pragma once

#include <string>

class Point
{
private:
	int r;
	int g;
	int b;

public:
	Point() : r(0), g(0), b(0) {};
	Point(int r, int g, int b) : r(r), g(g), b(b) {};

	void setR(int r) { this->r = r; }
	void setG(int g) { this->g = g; }
	void setB(int b) { this->b = b; }

	std::string toString() 
	{ 
		return std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b); 
	}
};

