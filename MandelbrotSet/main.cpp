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
	int width = 800;
	int height = 600;
	int maxN = 255;
	std::string resultFolder = "products\\";

	std::vector<InputData> input;
	input.push_back(InputData());
	input.push_back(InputData(-1.0, 0.0, 0.0, 1.0));
	input.push_back(InputData(-0.7463, -0.7513, 0.1102, 0.1152));
	int countSource = 0;
	int countResults = 1;

	tbb::flow::graph graph;

	tbb::flow::source_node<InputData> source(
		graph,
		[&](InputData& in) -> bool
		{
			if (countSource < input.size())
			{
				in = input[countSource++];
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
			std::string fileName = resultFolder + "img" + std::to_string(countSource) + "red";
			std::cout << fileName + '\n';

			Image* result = new Image(fileName, width, height);
			fractal(*result, maxN, in.minRe, in.maxRe, in.minIm, in.maxIm, palette);

			return result;
		}
	);

	tbb::flow::function_node<InputData, Image*> fractalGreen(
		graph,
		tbb::flow::unlimited,
		[&](InputData in) -> Image*
		{
			Pixel palette(1, 20, 1);
			std::string fileName = resultFolder + "img" + std::to_string(countSource) + "green";
			std::cout << fileName + '\n';
			Image* result = new Image(fileName, width, height);
			fractal(*result, maxN, in.minRe, in.maxRe, in.minIm, in.maxIm, palette);

			return result;
		}
	);

	tbb::flow::join_node<tbb::flow::tuple<Image*, Image*>, tbb::flow::queueing> join(graph);

	tbb::flow::function_node
			<tbb::flow::tuple<Image*, Image*>, std::tuple<Image*, Image*, Image*> > merge(
		graph,
				tbb::flow::unlimited,
		[&](tbb::flow::tuple<Image*, Image*> input) -> std::tuple<Image*, Image*, Image*>
		{
			Image* img1 = tbb::flow::get<0>(input);
			Image* img2 = tbb::flow::get<1>(input);
			Image* result = mergeImages(resultFolder, *img1, *img2, countResults++);
				
			return std::make_tuple(img1, img2, result);
		}
	);

	tbb::flow::function_node<std::tuple<Image*, Image*, Image*> > save(
		graph,
		tbb::flow::unlimited,
		[&](std::tuple<Image*, Image*, Image*> images) -> void
		{
			Image* img1 = std::get<0>(images);
			Image* img2 = std::get<1>(images);
			Image* result = std::get<2>(images);

			img1->saveFile();
			img2->saveFile();
			result->saveFile();

			delete img1, img2, result;
		}
	);

	tbb::flow::make_edge(merge, save);
	tbb::flow::make_edge(join, merge);
	tbb::flow::make_edge(fractalRed, tbb::flow::get<0>(join.input_ports()));
	tbb::flow::make_edge(fractalGreen, tbb::flow::get<1>(join.input_ports()));
	tbb::flow::make_edge(source, fractalRed);
	tbb::flow::make_edge(source, fractalGreen);
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

			image.setPixel(x, y, Pixel(r, g, b));
		}
	}
}

Image* mergeImages(const std::string resultFolder, const Image& img1, const Image& img2, const int index)
{
	std::string fileName = resultFolder + "img" + std::to_string(index) + "result";
	std::cout << fileName + '\n';

	Image* result = new Image(fileName, img1.getWidth(), img1.getHeight(), img1.getMaxColor());

	for (int y = 0; y < img1.getHeight(); y++)
	{
		for (int x = 0; x < img1.getWidth(); x++)
		{
			int b = (img1.getPixel(x, y).getG() + img2.getPixel(x, y).getR()) % result->getMaxColor();
			Pixel p(1, 1, b);
			result->setPixel(x, y, p);
		}
	}

	return result;
}
