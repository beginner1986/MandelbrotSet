#include <iostream>
#include <tbb/tbb.h>
#include <random>

#include "Pixel.h"
#include "Image.h"

double makeReal(int x, int width, double minIm, double maxIm);
double makeImaginary(int y, int height, double minIm, double maxIm);
int findValue(double cr, double ci, int maxN);
void fractal(Image& image, int maxN, double minRe, double maxRe, double minIm, double maxIm, Pixel palette);

int main()
{
	int width = 1024;
	int height = 768;
	int maxN = 255;
	double minRe = -2.0, maxRe = 2.0;
	double minIm = -1.5, maxIm = 1.5;

	int count = 0;

	tbb::flow::graph graph;
	tbb::flow::source_node<double> source(graph,
		[&count, minRe](double& border) -> bool {
			if (count < 2)
			{
				border = minRe + 2.0 * (double)count;
				count++;
				return true;
			}
			else
				return false;
		},
		false
	);
	tbb::flow::function_node<double, Image*> calculate1(graph,
		tbb::flow::unlimited,
		[=](double border) 
		{
			std::random_device rd;
			std::uniform_int_distribution<int> rng(1, 256);
			Pixel palette(rng(rd), rng(rd), rng(rd));
			std::string fileName = "img" + std::to_string(border);
			Image* image = new Image(fileName, width, height);
			fractal(*image, maxN, border, border + 2.0, minIm, maxIm, palette);
			return	image;
		}
	);
	tbb::flow::function_node<double, Image*> calculate2(graph,
		tbb::flow::unlimited,
		[=](double border)
		{
			std::random_device rd;
			std::uniform_int_distribution<int> rng(1, 256);
			Pixel palette(rng(rd), rng(rd), rng(rd));
			std::string fileName = "img" + std::to_string(border);
			Image* image = new Image(fileName, width, height);
			fractal(*image, maxN, border, border + 2.0, minIm, maxIm, palette);
			return	image;
		}
	);
	tbb::flow::join_node<tbb::flow::tuple<Image*, Image*> > merge(graph);
	tbb::flow::function_node<tbb::flow::tuple<Image*, Image*> > finalize(graph,
		tbb::flow::unlimited,
		[](tbb::flow::tuple<Image*, Image*> images)
		{
			tbb::flow::get<0>(images)->saveFile();
			tbb::flow::get<1>(images)->saveFile();
		}
	);

	tbb::flow::make_edge(source, calculate1);
	tbb::flow::make_edge(source, calculate2);
	tbb::flow::make_edge(calculate1, tbb::flow::input_port<0>(merge));
	tbb::flow::make_edge(calculate2, tbb::flow::input_port<1>(merge));
	tbb::flow::make_edge(merge, finalize);
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

void fractal(Image& image, int maxN, double minRe, double maxRe, double minIm, double maxIm, Pixel palette)
{
	for (int y = 0; y < image.getHeight(); y++)
	{
		for (int x = 0; x < image.getWidth(); x++)
		{

			double cr = makeReal(x, image.getWidth(), minRe, maxRe);
			double ci = makeImaginary(y, image.getHeight(), minIm, maxIm);

			int n = findValue(cr, ci, maxN);

			int r = ((n * palette.getR()) % image.getMaxColor());
			int g = ((n * palette.getG()) % image.getMaxColor());
			int b = ((n * palette.getB()) % image.getMaxColor());

			image.setPixel(x, y, r, g, b);
		}
	}
}
