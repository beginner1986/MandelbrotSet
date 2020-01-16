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

Image* mergeImages(const std::string resultFolder, const Image& img1, const Image& img2, const int index);

int main()
{
	int width = 640;
	int height = 480;
	int maxN = 255;
	std::string resultFolder = "products\\";

	std::vector<InputData> input;
	input.push_back(InputData());
	input.push_back(InputData(-1.0, 0.0, 0.0, 1.0));
	input.push_back(InputData(-0.75, -0.25, 0.25, 0.75));
	int count = 0;

	tbb::flow::graph graph;
	tbb::flow::source_node<InputData> source(
		graph,
		[&](InputData& in) -> bool
		{
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

	tbb::flow::function_node<InputData, Image*> fractalRed(
		graph,
		tbb::flow::unlimited,
		[&](InputData in) -> Image*
		{
			Pixel palette(20, 1, 1);
			std::string fileName = resultFolder + "img" + std::to_string(count) + "red";
			Image* result = new Image(fileName, width, height);
			fractal(*result, maxN, in.minRe, in.maxRe, in.minIm, in.maxIm, palette);

			return result;
		}
	);

	tbb::flow::function_node<Image*> saveFile(
		graph,
		tbb::flow::unlimited,
		[=](Image* image) -> void
		{
			image->saveFile();

			delete image;
		}
	);

	tbb::flow::make_edge(fractalRed, saveFile);
	tbb::flow::make_edge(source, fractalRed);
	source.activate();
	graph.wait_for_all();

	/*
	Image* img1, * img2, * result;

	for (int i = 0; i < input.size(); i++)
	{
		InputData in = input[i];
		Pixel paletteRed(20, 1, 1);
		Pixel paletteGreen(1, 20, 1);

		std::string fileNameRed = resultFolder + "img" + std::to_string(i) + "red";
		Image* img1 = new Image(fileNameRed, width, height);
		fractal(*img1, maxN, in.minRe, in.maxRe, in.minIm, in.maxIm, paletteRed);

		std::string fileNameGreen = resultFolder + "img" + std::to_string(i) + "green";
		Image* img2 = new Image(fileNameGreen, width, height);
		fractal(*img2, maxN, in.minRe, in.maxRe, in.minIm, in.maxIm, paletteGreen);

		result = mergeImages(resultFolder, *img1, *img2, i);

		img1->saveFile();
		img2->saveFile();
		result->saveFile();

		delete img1;
		delete img2;
		delete result;
	}
	*/

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

			image.setPixel(x, y, Pixel(r, g, b));
		}
	}
}

Image* mergeImages(const std::string resultFolder, const Image& img1, const Image& img2, const int index)
{
	std::string fileNameRed = resultFolder + "img" + std::to_string(index) + "result";

	Image* result = new Image(fileNameRed, img1.getWidth(), img1.getHeight(), img1.getMaxColor());

	for (int y = 0; y < img1.getHeight(); y++)
	{
		for (int x = 0; x < img1.getWidth(); x++)
		{
			int b = img1.getMaxColor() - (img1.getPixel(x, y).getR() * img2.getPixel(x, y).getG()) % img1.getMaxColor();
			Pixel p(1, 1, b);
			result->setPixel(x, y, p);
		}
	}

	return result;
}