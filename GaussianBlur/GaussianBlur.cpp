#include "opencv2/imgproc.hpp"
#include <corecrt_math_defines.h>
#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

const int sliderValueMax = 5;
int sliderValue = 0;
Mat img, changedImg,changedImg2;

void convolution(const Mat& in_image, const double* mask, int ksize, double koef, Mat& out_image)
{
	double temp = 0;
	int hk = ksize / 2; // floor(ksize/2)
	cout << "Hk = " << hk << endl;
	for (int i = hk; i < in_image.rows - hk; ++i) {
		for (int j = hk; j < in_image.cols - hk; ++j) {
			for (int ik = 0; ik < ksize; ++ik) {
				for (int jk = 0; jk < ksize; ++jk) {
					//calculating new pixel value
					temp += mask[ik * ksize + jk] * in_image.data[(i + hk - ik) * in_image.cols + (j + hk - jk)];
					//cout << "Pixel: " << (i + hk - ik) * in_image.cols + (j + hk - jk) << endl;
				}
			}
			//cout <<"new value for pixel: " <<  (int)temp / koef << endl;
			out_image.data[i * in_image.cols + j] = (int)round(temp * koef);
			temp = 0;
		}
	}
}

void myGaussianBlur(const Mat& in_image, Mat& out_image, const int ksize, double sigma)
{
	double* mask = new double[ksize * ksize];
	int hk = ksize / 2; // floor(ksize/2)
	double denumMain = 2.0 * M_PI * pow(sigma, 2);
	double denumEuler = 2.0 * pow(sigma, 2);
	double dist = 0;
	for (int i = 0; i < ksize; ++i) {
		for (int j = 0; j < ksize; ++j) {
			dist = pow((i - hk), 2) + pow((j - hk), 2);
			cout << dist << endl;
			mask[i * ksize + j] = exp(-1.0 * ( dist / denumEuler) ) / denumMain ;
			cout << mask[i * ksize + j] << "  ";
		}
		cout << endl;
	}

	convolution(in_image, mask, ksize, 1, out_image);
}

static void on_trackbar_change(int, void* = 0)
{
	int sizeVal = ((double)sliderValue / sliderValueMax) * 9;
	cout << sizeVal << "\n";
	if (sizeVal % 2 == 0)
		sizeVal++;

	myGaussianBlur(img, changedImg, sizeVal, 0.75);

	ostringstream kernelSizeText;
	kernelSizeText << "Sigma = " << sizeVal;
	putText(changedImg, kernelSizeText.str(), Point(30, 30),
		FONT_HERSHEY_SIMPLEX, 0.9, 220, 2);

	imshow("Gaussian blur", changedImg);
}

int main()
{
	std::cout.sync_with_stdio(false);
	std::string imageNames[] = { "Geneva.tif", "norway.jpg", "bigGradient.jpg", "portrait.jpg", "test.png", "anime.jpg", "IM23.tif", "IM17.tif", "IM13.tif", "IM11.tif" };
	img = imread("C:/Users/Lord/source/repos/Tif/" + imageNames[0], 0);
	changedImg = img.clone();
	changedImg2 = img.clone();

	if (img.data == 0) // Check for invalid input
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	std::cout << "Original image dimensions : " << img.cols << 'x' << img.rows << std::endl;

	namedWindow("Gaussian blur", WINDOW_AUTOSIZE);
	resizeWindow("Gaussian blur", changedImg.cols, changedImg.rows);
	//createTrackbar("KernelSize", "Gaussian blur", &sliderValue, sliderValueMax, on_trackbar_change);

	myGaussianBlur(img, changedImg, 5, 0.73);
	GaussianBlur(img,changedImg2,Size(3,3), 2);imshow("Gaussian blur", changedImg);
	imshow("Gaussian blur REAL", changedImg2);
	//on_trackbar_change(sliderValue);

	waitKey(0);
	system("pause");
	return 0;
}