#include <iostream>
#include <tbb/tbb.h>
#include <random>

#include "Point.h"
#include "Image.h"

double makeReal(int x, int width, double minIm, double maxIm);
double makeImaginary(int y, int height, double minIm, double maxIm);
int findValue(double cr, double ci, int maxN);
void fractal(Image& image, int maxN, double minRe, double maxRe, double minIm, double maxIm);

int main()
{
	int width = 1024;
	int height = 768;
	int maxN = 255;
	double minRe = -2.0, maxRe = 2.0;
	double minIm = -1.5, maxIm = 1.5;

	int count = 50;

	tbb::flow::graph graph;
	tbb::flow::source_node<int> source(graph,
		[&count, &maxN](int& n) -> bool {
			if (count < maxN)
			{
				n = count;
				count += 50;
				return true;
			}
			else
				return false;
		},
		false
	);
	tbb::flow::function_node<int, Image*> calculate(graph,
		tbb::flow::unlimited,
		[=](int n) 
		{
			std::string fileName = "img" + std::to_string(n);
			Image* image = new Image(fileName, width, height);
			fractal(*image, n, minRe, maxRe, minIm, maxIm);
			return	image;
		}
	);
	tbb::flow::function_node<Image*> finalize(graph,
		tbb::flow::unlimited,
		[](Image *image) 
		{
			image->saveFile();
			delete image;
		}
	);

	tbb::flow::make_edge(source, calculate);
	tbb::flow::make_edge(calculate, finalize);
	source.activate();
	graph.wait_for_all();

	return 0;
}

double makeReal(int x, int width, double minRe, double maxRe)
{
	double range = maxRe - minRe;

	return x * (range / width) + minRe;
}

double makeImaginary(int y, int height, double minIm, double maxIm)
{
	double range = maxIm - minIm;

	return y * (range / height) + minIm;
}

int findValue(double cr, double ci, int maxN)
{
	int n = 0;
	double zr = 0.0, zi = 0.0;

	while ((n < maxN) && (zr * zr + zi * zi < 4.0))
	{
		double temp = zr * zr - zi * zi + cr;
		zi = 2.0 * zr * zi + ci;
		zr = temp;
		n++;
	}

	return n;
}

void fractal(Image& image, int maxN, double minRe, double maxRe, double minIm, double maxIm)
{
	for (int y = 0; y < image.getHeight(); y++)
	{
		for (int x = 0; x < image.getWidth(); x++)
		{

			double cr = makeReal(x, image.getWidth(), minRe, maxRe);
			double ci = makeImaginary(y, image.getHeight(), minIm, maxIm);

			int n = findValue(cr, ci, maxN);

			int r = (n % image.getMaxColor());
			int g = (n % image.getMaxColor());
			int b = (n % image.getMaxColor());

			image.setPixel(x, y, r, g, b);
		}
	}
}
