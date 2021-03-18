#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>
#include <opencv2/ml.hpp>

// Non-local means filter

using namespace cv;
using namespace std;

void nonLocalMeans(Mat& src, Mat& dst, int x, int y, int diameter, double sigmaI, int height, int width);
double gaussian(float x, double sigma);
double normOfVector(Mat& vec1, Mat& vec2);
Mat neighboursValues(int area, Mat& src, int x, int y);
Mat myNLMeansFilter(Mat& src, Mat& dst, int diameter, double sigmaI);

double gaussian(float x, double sigma)
{
	return exp(-(pow(x, 2)) / (2 * pow(sigma, 2))) / (2 * CV_PI * pow(sigma, 2));
}

double normOfVector(Mat& vec1, Mat& vec2)
{
	double norm = 0;
	for (int i = 0; i < vec1.rows; i++)
	{
		norm = norm + pow((vec1.at<uchar>(i, 0) - vec2.at<uchar>(i, 0)), 2);
	}
	norm = sqrt(norm);
	return norm;
}

Mat neighboursValues(int area, Mat& src, int x, int y)
{
	Mat values = Mat::zeros(area*area, 1, CV_8U);
	for (int i = 0; i < area; i++)
	{
		for (int j = 0; j < area; j++)
		{
			int LocalX = (x - (area - 1) / 2) + j;

			int LocalY = (y - (area - 1) / 2) + i;

			if (LocalX < 0)
			{
				LocalX = 0;
			}
			if (LocalY < 0)
			{
				LocalY = 0;
			}
			{
				values.at<uchar>(j + i, 0) = src.at<uchar>(LocalX, LocalY);
			}
		}
	}
	return values;
}

void nonLocalMeans(Mat& src, Mat& dst, int x, int y, int diameter, double sigmaI, int height, int width)
{
	double iFiltered = 0;
	double wP = 0;
	int xNeighbor = 0;
	int yNeighbor = 0;
	int half = diameter / 2;

	for (int i = 0; i < diameter; i++)
	{
		for (int j = 0; j < diameter; j++)
		{
			xNeighbor = x - (i - half);
			yNeighbor = y - (j - half);
			if (xNeighbor < 0) xNeighbor = 0;
			if (yNeighbor < 0) yNeighbor = 0;
			while (xNeighbor >= height - half) xNeighbor--;
			while (yNeighbor >= width - half) yNeighbor--;
			if (x < half) x = half;
			if (y < half) y = half;
			if (x >= height - half)
				x = height - half;
			if (y >= width - half)
				y = width - half;
			Mat vector1 = neighboursValues(half, src, x, y);
			Mat vector2 = neighboursValues(half, src, xNeighbor, yNeighbor);
			double vecNorm = normOfVector(vector1, vector2);

			double gr = gaussian(vecNorm, sigmaI);
			double w = gr;
			iFiltered = iFiltered + src.at<uchar>(xNeighbor, yNeighbor) * w;
			wP = wP + w;
		}
	}
	iFiltered = iFiltered / wP;
	dst.at<uchar>(x, y) = (uchar)iFiltered;
}

Mat myNLMeansFilter(Mat& src, Mat& dst, int diameter, double sigmaI)
{
	for (int i = 0; i < dst.cols; i++)
	{
		for (int j = 0; j < dst.rows; j++)
		{
			nonLocalMeans(src, dst, i, j, diameter, sigmaI, dst.cols, dst.rows);
		}
	}
	return dst;
}

void noising(Mat &src)
{
	normal_distribution<double> dist(0.0, 10.0);
	default_random_engine generator;

	for (int x = 0; x < src.cols; x++)
	{
		for (int y = 0; y < src.rows; y++)
		{
			src.at<uchar>(x, y) += dist(generator);
		}
	}
}

int main()
{
	Mat srcNoNoize;
	srcNoNoize = imread("Lenna.png", IMREAD_GRAYSCALE);

	Mat srcWithNoize;
	srcWithNoize = imread("Lenna.png", IMREAD_GRAYSCALE);

	noising(srcWithNoize);
	imshow("origNoNoize", srcNoNoize);
	imshow("origWithNoize", srcWithNoize);

	Mat NLMeansTest;
	srcWithNoize.copyTo(NLMeansTest);

	Mat NLMeansTestOpenCV;
	srcWithNoize.copyTo(NLMeansTestOpenCV);

	Mat NLMeansFilteredImage = myNLMeansFilter(NLMeansTest, NLMeansTestOpenCV, 12, 20);
	imshow("MyNLM", NLMeansFilteredImage);

	int sum = 0;
	for (int x = 0; x < srcNoNoize.rows; x++)
	{
		for (int y = 0; y < srcNoNoize.cols; y++)
		{
			int difference = srcNoNoize.at<uchar>(x, y) - NLMeansFilteredImage.at<uchar>(x, y);
			if (difference < 0)
			{
				difference = difference * -1;
				sum += difference;
			}
			else
			{
				sum += difference;
			}
		}
	}

	int average = 0;
	average = sum / (srcNoNoize.rows*srcNoNoize.cols);

	cout << average << endl;

	waitKey(0);
}

