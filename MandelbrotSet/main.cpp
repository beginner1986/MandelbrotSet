#include <iostream>
#include <tbb/tbb.h>
#include <random>

#include "Pixel.h"
#include "Image.h"

double makeReal(const int x, const int width, const double minRe, const double maxRe);
double makeImaginary(const int y, const int height, const double minIm, const double maxIm);
int findValue(const double cr, const double ci, const int maxN);
void fractal(Image& image, int maxN, double minRe, double maxRe, double minIm, double maxIm, Pixel palette);
Image* mergeImages(const Image* img1, const Image* img2);

int main()
{
	int width = 640;
	int height = 480;
	int maxN = 128;
	double minRe = -2.0, maxRe = 1.5;
	double minIm = -1.5, maxIm = 1.5;
	int maxColor = 255;

	int count = 0;
	double step = 1.75;
	Image* img1, * img2, * result;

	tbb::flow::graph graph;
	tbb::flow::source_node<double> source(graph,
		[&count, step, minRe](double& bound) -> bool {
			if (count < 2)
			{
				bound = minRe + step * static_cast<double>(count);
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
		[=](double bound)
		{
			std::random_device seed;
			std::uniform_int_distribution<int> rng(1, maxColor);
			Pixel palette(rng(seed), rng(seed), rng(seed));

			std::string fileName = "img1";
			Image* image = new Image(fileName, width, height, maxColor);
			fractal(*image, maxN, bound, bound + step, minIm, maxIm, palette);

			return	image;
		}
	);
	tbb::flow::function_node<double, Image*> calculate2(graph,
		tbb::flow::unlimited,
		[=](double bound)
		{
			std::random_device seed;
			std::uniform_int_distribution<int> rng(1, maxColor);
			Pixel palette(rng(seed), rng(seed), rng(seed));
			
			std::string fileName = "img2";
			Image* image = new Image(fileName, width, height, maxColor);
			fractal(*image, maxN, bound, bound + step, minIm, maxIm, palette);

			return	image;
		}
	);
	tbb::flow::join_node<tbb::flow::tuple<Image*, Image*> > merge(graph);
	tbb::flow::function_node<tbb::flow::tuple<Image*, Image*> > finalize(graph,
		tbb::flow::unlimited,
		[&](tbb::flow::tuple<Image*, Image*> images)
		{
			img1 = tbb::flow::get<0>(images);
			img2 = tbb::flow::get<1>(images);

			result = mergeImages(img1, img2);
		}
	);

	//			 (calculate1)
	//			/			 \
	// (source)				  (merge) - (finalize)
	//			\			 /
	//			 (calculate2)

	tbb::flow::make_edge(calculate1, tbb::flow::input_port<0>(merge));
	tbb::flow::make_edge(calculate2, tbb::flow::input_port<1>(merge));
	tbb::flow::make_edge(merge, finalize);
	tbb::flow::make_edge(source, calculate1);
	tbb::flow::make_edge(source, calculate2);
	source.activate();
	graph.wait_for_all();

	img1->saveFile();
	img2->saveFile();
	result->saveFile();

	delete img1;
	delete img2;
	delete result;

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

void fractal(Image& image, int maxN, double minRe, double maxRe, double minIm, double maxIm, Pixel palette)
{
	for (int y = 0; y < image.getHeight(); y++)
	{
		for (int x = 0; x < image.getWidth(); x++)
		{

			double cr = makeReal(x, image.getWidth(), minRe, maxRe);
			double ci = makeImaginary(y, image.getHeight(), minIm, maxIm);

			int n = findValue(cr, ci, maxN);

			int r = (n * palette.getR() % image.getMaxColor());
			int g = (n * palette.getG() % image.getMaxColor());
			int b = (n * palette.getB() % image.getMaxColor());

			image.setPixel(x, y, Pixel(r, g, b));
		}
	}
}

Image* mergeImages(const Image* img1, const Image* img2)
{
	int width = img1->getWidth() + img2->getWidth();
	int height = img1->getHeight();
	int maxColor = img1->getMaxColor();

	Image* result = new Image("result", width, height, maxColor);

	for (int y = 0; y < img1->getHeight(); y++)
	{
		for (int x = 0; x < img1->getHeight(); x++)
		{
			if(x < img1->getWidth())
				result->setPixel(x, y, img1->getPixel(x, y));
			else
				result->setPixel(x + img1->getWidth(), y, img2->getPixel(x, y));
		}
	}

	return result;
}
