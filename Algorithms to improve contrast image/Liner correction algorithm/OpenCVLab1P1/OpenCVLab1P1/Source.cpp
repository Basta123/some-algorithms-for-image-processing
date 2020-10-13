#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdint.h>

int Original_Pixels[256]; 
int Modified_Pixels[256]; 

using namespace std;
using namespace cv;

// Liner correction algorithm

void show_histogram(string const& name, int *arrOfIntensity, Mat1b const& image) 
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



int main()
{
	Mat original = imread("back.jpg", 0); 
	namedWindow("girl.jpg", WINDOW_FREERATIO); 
	Mat modified = imread("back.jpg", 0); 
	namedWindow("girl2.jpg", WINDOW_FREERATIO); 




	CalcHist(original, Original_Pixels);

	// определяем оступы	
	float FirstRange = 0;
	float EndRange = 0;
	int CountPixels = 0;

	int TotalCount = original.rows* original.cols;  
	int FivePercent = TotalCount *  0.05; 
	FivePercent = FivePercent / 2; 
	cout << FivePercent << endl;

	for (int i = 0; i < 256; i++)
	{

		

		

		if (CountPixels + Original_Pixels[i] < 1) 
		{
			CountPixels += Original_Pixels[i];
		}
		
		else
		{
			FirstRange = i;
			CountPixels = 0;
			break;
		}
	}


	for (int i = 256; i > 0; i--)
	{
		
		

		if (CountPixels + Original_Pixels[i] <1) 
		{
			CountPixels += Original_Pixels[i];
		}

		else
		{
			CountPixels = 0;
			EndRange = i; 
			break;
		}
	}



	// лиинейное растяжение
	for (int x = 0; x < modified.rows; x++)  
	{
		for (int y = 0; y < modified.cols; y++) 
		{

			int PixelIntensity = modified.at<uchar>(x, y); 
			PixelIntensity = (PixelIntensity - FirstRange) * (((float)255 - 0) / (EndRange - FirstRange));
			modified.at<uchar>(x, y) = PixelIntensity; 

		}
		 
	}





	CalcHist(modified, Modified_Pixels);



	imshow("girl.jpg", original); 
	imshow("girl2.jpg", modified);

	namedWindow("sample_hist", WINDOW_FREERATIO);
	namedWindow("sample_hist2", WINDOW_FREERATIO);
	show_histogram("sample_hist", Original_Pixels, original); 
	show_histogram("sample_hist2", Modified_Pixels, modified);

	
	
	waitKey();

	return 0;
}


