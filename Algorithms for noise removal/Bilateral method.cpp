#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>

using namespace cv;
using namespace std;

// BilateralFilter

float distance(int x, int y, int i, int j);
double NormalDistributionGauss(float x, double sigma);
void FunctionBilateralFilter(Mat src, Mat &dst, int x, int y, int diameter, double sigmaI, double sigmaS, int height, int width);

Mat FunctionBilateralFilter(Mat src, Mat &dst, int diameter, double sigmaI, double sigmaS)
{
	for (int i = 0; i < src.cols; i++)
	{
		for (int j = 0; j < src.rows; j++)
		{
			FunctionBilateralFilter(src, dst, i, j, diameter, sigmaI, sigmaS, src.cols, src.rows);
		}
	}
	return dst;
}

void FunctionBilateralFilter(Mat src, Mat &dst, int x, int y, int diameter, double sigmaI, double sigmaS, int height, int width)
{
	double sumKernelCoefficients = 0;
	double wP = 0;
	int neighbor_x = 0;
	int neighbor_y = 0;
	int half = diameter / 2;

	for (int i = 0; i < diameter; i++)
	{
		for (int j = 0; j < diameter; j++)
		{
			neighbor_x = x - (i - half);
			neighbor_y = y - (j - half);
			if (neighbor_x < 0)
			{
				neighbor_x = 0;
			}
			if (neighbor_y < 0)
			{
				neighbor_y = 0;
			}
			while (neighbor_x >= height) neighbor_x--;
			while (neighbor_y >= width) neighbor_y--;
			double gi = NormalDistributionGauss(src.at<uchar>(neighbor_x, neighbor_y) - src.at<uchar>(x, y), sigmaI);
			double gs = NormalDistributionGauss(distance(x, y, neighbor_x, neighbor_y), sigmaS);
			double w = gi * gs;
			sumKernelCoefficients = sumKernelCoefficients + src.at<uchar>(neighbor_x, neighbor_y) * w;
			wP = wP + w;
		}
	}
	sumKernelCoefficients = sumKernelCoefficients / wP;
	dst.at<uchar>(x, y) = sumKernelCoefficients;
}

float distance(int x, int y, int i, int j)
{
	return float(sqrt(pow(x - i, 2) + pow(y - j, 2)));
}

double NormalDistributionGauss(float x, double sigma)
{
	return exp(-(pow(x, 2)) / (2 * pow(sigma, 2))) / (2 * CV_PI * pow(sigma, 2));
}

void noising(Mat &src)
{
	normal_distribution<double> dist(0.0, 10.0);
	std::default_random_engine generator;

	for (int x = 0; x < src.cols; x++)
	{
		for (int y = 0; y < src.rows; y++)
		{
			uchar value = dist(generator);
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
	imshow("origNoize", srcWithNoize);


	Mat bilateralTest;
	srcWithNoize.copyTo(bilateralTest);


	Mat bilateralTestDST;
	bilateralTestDST = Mat(srcWithNoize.cols, srcWithNoize.rows, CV_8U);


	Mat bilateralFilteredImage = FunctionBilateralFilter(bilateralTest, bilateralTestDST, 8, 10.0, 10.0);
	imshow("blurred", bilateralFilteredImage);


	int sum = 0;
	for (int x = 0; x < srcNoNoize.rows; x++)
	{
		for (int y = 0; y < srcNoNoize.cols; y++)
		{

			int difference = srcNoNoize.at<uchar>(x, y) - bilateralFilteredImage.at<uchar>(x, y);
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

