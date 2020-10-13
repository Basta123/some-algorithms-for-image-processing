#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdint.h>

int Original_Pixels[256]; 
int Specified_Pixels[256];  

using namespace std;
using namespace cv;

// Algorithm to histogram matching

void show_histogram(string const& name, int *arrOfPixels) 
{

	int *MaxNumOfPixels = max_element(arrOfPixels, arrOfPixels + 255); 
	
	Mat3b hist_image = Mat3b::zeros(256, 256);  

	for (int b = 0; b < 256; b++)
	{
		int binval = arrOfPixels[b];
		int height = cvRound(binval * 256 / *MaxNumOfPixels);
		line(hist_image, Point(b, 256 - height), Point(b, 256), Scalar::all(256)); 
	}

	imshow(name, hist_image); 

}

void show_histogram_cumulative(string const& name, int *arrOfIntensity, int TotalPixels)
{
	int *MaxNumOfPixels = max_element(Original_Pixels, Original_Pixels + 255);

	Mat3b hist_image = Mat3b::zeros(256, 256);

	for (int b = 0; b < 256; b++)
	{
		int binval = arrOfIntensity[b];
		int height = cvRound(binval * 256 / TotalPixels);
		line(hist_image, Point(b, 256 - height), Point(b, 256), Scalar::all(256));
	}

	imshow(name, hist_image);

}

void CalcHist(Mat &image, int Pixels[])
{
	for (int x = 0; x < image.rows; x++)
	{
		for (int y = 0; y < image.cols; y++)
		{

			int PixelIntensity = image.at<uchar>(x, y);
			Pixels[PixelIntensity] = Pixels[PixelIntensity] + 1;

		}
	}
}

void CalcHistCumulative(int CumulativeIntensity[], int Pixels[])
{
	int Cumulative_Pixels = 0;
	for (int i = 0; i < 256; i++)
	{
		Cumulative_Pixels += Pixels[i];

		CumulativeIntensity[i] = Cumulative_Pixels;
	}
}

void EqualizeHist(int totalcount, int CPMultiplied_rounded[],int Pixels[])
{
	float probability[256];
	for (int i = 0; i < 256; i++)
	{
		probability[i] = (float)Pixels[i] / totalcount;

	}

	float CumulativeProbability = 0;
	float CPMultiplied[256];

	for (int i = 0; i < 256; i++)
	{
		CumulativeProbability += probability[i];
		CPMultiplied[i] = 255 * CumulativeProbability;
		CPMultiplied_rounded[i] = round(CPMultiplied[i]);

	}
}

void Hist_matching(int Original_CPMultiplied_rounded[], int Specified_CPMultiplied_rounded[]) 
{
	for (int i = 0; i < 256; i++) 
	{
		int original_CPMultiplied_roundedPlus = Original_CPMultiplied_rounded[i];
		int original_CPMultiplied_roundedMinus = Original_CPMultiplied_rounded[i];
		bool PlusOrMinus = true;

		for (int j = 0; j < 256; j++)
		{

			
			bool equal = Original_CPMultiplied_rounded[i] == Specified_CPMultiplied_rounded[j];
			if (equal)
			{
				Original_CPMultiplied_rounded[i] = j;
				break;

			}
			else
			{
				if (255 == j)
				{
					if (PlusOrMinus)
					{

						original_CPMultiplied_roundedPlus += 1;
						Original_CPMultiplied_rounded[i] = original_CPMultiplied_roundedPlus;

						PlusOrMinus = false;

					}
					else
					{
						original_CPMultiplied_roundedMinus -= 1;
						Original_CPMultiplied_rounded[i] = original_CPMultiplied_roundedMinus;
						PlusOrMinus = true;
					}

					j = -1;


				}
			}
		}


	}
}


int main()
{

	Mat original = imread("sample.png", 0);
	namedWindow("sample", WINDOW_FREERATIO);

	Mat specified = imread("avignon.jpg", 0);
	namedWindow("black_photo", WINDOW_FREERATIO);

	CalcHist(original, Original_Pixels);

	CalcHist(specified, Specified_Pixels);

	int original_totalcount = 0; 
	original_totalcount = original.rows * original.cols;
	int Original_CPMultiplied_rounded[256];
	EqualizeHist(original_totalcount, Original_CPMultiplied_rounded, Original_Pixels);

	int specified_totalcount = 0;
	specified_totalcount = specified.rows * specified.cols;
	int Specified_CPMultiplied_rounded[256];
	EqualizeHist(specified_totalcount, Specified_CPMultiplied_rounded, Specified_Pixels);

	Hist_matching(Original_CPMultiplied_rounded, Specified_CPMultiplied_rounded);
	
	for (int x = 0; x < original.rows; x++)
	{
		for (int y = 0; y < original.cols; y++)
		{

			int IntensityPixel = original.at<uchar>(x, y); 
			original.at<uchar>(x, y) = Original_CPMultiplied_rounded[IntensityPixel]; 
			

		}
		
	}

	for (int x = 0; x < 256; x++)
	{
		
			Original_Pixels[x] = 0; 
	}

	CalcHist(original, Original_Pixels);

	int CumulativeIntensity_Original[256];
	int CumulativeIntensity_Modified[256];
	CalcHistCumulative(CumulativeIntensity_Original, Original_Pixels);
	CalcHistCumulative(CumulativeIntensity_Modified, Specified_Pixels);
	
	imshow("sample", original);
	imshow("black_photo", specified);
	
	namedWindow("sample_hist", WINDOW_FREERATIO);
	namedWindow("sample_hist2", WINDOW_FREERATIO);
	namedWindow("sample_hist_cumulative", WINDOW_FREERATIO);
	namedWindow("sample_hist_cumulative2", WINDOW_FREERATIO);
	
		
	show_histogram("sample_hist", Original_Pixels);
	show_histogram("sample_hist2", Specified_Pixels);

	show_histogram_cumulative("sample_hist_cumulative", CumulativeIntensity_Original, original_totalcount);
	show_histogram_cumulative("sample_hist_cumulative2", CumulativeIntensity_Modified, specified_totalcount);

	

	waitKey();

	return 0;
}
