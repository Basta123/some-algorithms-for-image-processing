#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdint.h>

int Original_Pixels[256]; 
int Modified_Pixels[256]; 



using namespace std;
using namespace cv;

// histogram equalization algorithm

void show_histogram(string const& name, int *arrOfIntensity) 
{
	int *MaxNumOfPixels = max_element(Original_Pixels, Original_Pixels + 255);
	cout << *MaxNumOfPixels << endl;

	Mat3b hist_image = Mat3b::zeros(256, 256);    

	for (int b = 0; b < 256; b++)
	{
		int binval = arrOfIntensity[b];
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

void EqualizeHist(int totalcount, int CPMultiplied_rounded[])
{
	float probability[256];
	for (int i = 0; i < 256; i++)
	{
		probability[i] = (float)Original_Pixels[i] / totalcount;

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

int main()
{

	Mat original = imread("back.jpg", 0);
	namedWindow("sample", WINDOW_FREERATIO); 

	Mat modified = imread("back.jpg", 0);
	namedWindow("sample2", WINDOW_FREERATIO); 


	

	CalcHist(original, Original_Pixels);

	int totalcount = 0; 
	totalcount = original.rows * original.cols;
	int CPMultiplied_rounded[256];
	EqualizeHist(totalcount, CPMultiplied_rounded);



	for (int x = 0; x < modified.rows; x++)
	{
		for (int y = 0; y < modified.cols; y++)
		{

			int IntensityPixel = modified.at<uchar>(x, y); 
			modified.at<uchar>(x, y) = CPMultiplied_rounded[IntensityPixel]; 


		}

	}


	

	CalcHist(modified, Modified_Pixels);


	int CumulativeIntensity_Original[256];
	int CumulativeIntensity_Modified[256];
	CalcHistCumulative(CumulativeIntensity_Original, Original_Pixels);
	CalcHistCumulative(CumulativeIntensity_Modified, Modified_Pixels);


	imshow("sample", original); 
	imshow("sample2", modified);


	namedWindow("sample_hist", WINDOW_FREERATIO);
	namedWindow("sample_hist2", WINDOW_FREERATIO);

	show_histogram("sample_hist", Original_Pixels); 
	show_histogram("sample_hist2", Modified_Pixels);


	namedWindow("sample_hist_cumulative", WINDOW_FREERATIO);
	namedWindow("sample_hist_cumulative2", WINDOW_FREERATIO);

	show_histogram_cumulative("sample_hist_cumulative", CumulativeIntensity_Original, totalcount);
	show_histogram_cumulative("sample_hist_cumulative2", CumulativeIntensity_Modified, totalcount);

	waitKey();

	return 0;
}
