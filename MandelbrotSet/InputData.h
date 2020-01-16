#pragma once

struct InputData
{
public:
	double minRe;
	double maxRe;
	double minIm;
	double maxIm;

	InputData(double minRe = -2.0, double maxRe = 1.5, double minIm = -1.5, double maxIm = 1.5)
		: minRe(minRe), maxRe(maxRe), minIm(minIm), maxIm(maxIm) {};
};