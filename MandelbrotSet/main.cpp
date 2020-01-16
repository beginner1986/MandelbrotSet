#include <iostream>
#include <tbb/tbb.h>
#include <vector>

#include "Pixel.h"
#include "Image.h"
#include "InputData.h"

double makeReal(const int x, const int width, const double minRe, const double maxRe);
double makeImaginary(const int y, const int height, const double minIm, const double maxIm);
int findValue(const double cr, const double ci, const int maxN);
void fractal(Image& image, const int maxN, const double minRe, const double maxRe, 
	const double minIm, const double maxIm, const Pixel palette);

int main()
{
	int width = 320;
	int height = 240;
	int maxN = 255;
	double minRe = -2.0, maxRe = 2.0;
	double minIm = -1.5, maxIm = 1.5;
	std::string resultFolder = "products\\";

	std::vector<InputData> input;
	input.push_back(InputData());
	input.push_back(InputData(-1.0, 0.0, 0.0, 1.0));

	int count = 0;

	tbb::flow::graph graph;
	tbb::flow::source_node<InputData> source(graph,
		[&](InputData& in) -> bool {
			if (count < input.size())
			{
				in = input[count++];
				return true;
			}
			else
				return false;
		},
		false
	);
	tbb::flow::function_node<InputData, Image*> calculate1(graph,
		tbb::flow::unlimited,
		[=](InputData in) -> Image*
		{
			Pixel palette(20, 1, 1);
			std::string fileName = resultFolder + "img" + std::to_string(count) + "red";
			Image* image = new Image(fileName, width, height);
			fractal(*image, maxN, minRe, maxRe, minIm, maxIm, palette);
			return	image;
		}
	);
	tbb::flow::function_node<InputData, Image*> calculate2(graph,
		tbb::flow::unlimited,
		[=](InputData in) -> Image*
		{
			Pixel palette(1, 20, 1);
			std::string fileName = resultFolder + "img" + std::to_string(count) + "green";
			Image* image = new Image(fileName, width, height);
			fractal(*image, maxN, minRe, maxRe, minIm, maxIm, palette);
			return	image;
		}
	);
	tbb::flow::join_node<tbb::flow::tuple<Image*, Image*> > join(graph);
	tbb::flow::function_node<tbb::flow::tuple<Image*, Image*> > finalize(graph,
		tbb::flow::unlimited,
		[&](tbb::flow::tuple<Image*, Image*> images)
		{
			tbb::flow::get<0>(images)->saveFile();
			tbb::flow::get<1>(images)->saveFile();
		}
	);

	tbb::flow::make_edge(join, finalize);
	tbb::flow::make_edge(calculate1, tbb::flow::input_port<0>(join));
	tbb::flow::make_edge(calculate2, tbb::flow::input_port<1>(join));
	tbb::flow::make_edge(source, calculate1);
	tbb::flow::make_edge(source, calculate2);
	source.activate();
	graph.wait_for_all();

	return 0;
}

double makeReal(const int x, const int width, const double minRe, const double maxRe)
{
	double range = maxRe - minRe;

	return x * (range / width) + minRe;
}

double makeImaginary(const int y, const int height, const double minIm, const double maxIm)
{
	double range = maxIm - minIm;

	return y * (range / height) + minIm;
}

int findValue(const double cr, const double ci, const int maxN)
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


void fractal(Image& image, const int maxN, const double minRe, const double maxRe,
	const double minIm, const double maxIm, const Pixel palette)
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
