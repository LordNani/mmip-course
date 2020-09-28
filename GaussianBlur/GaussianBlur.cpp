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
Mat img, changedImg;

static void on_trackbar_change(int, void* = 0)
{
    int sizeVal = ((double)sliderValue / sliderValueMax) * 9;
    //cout << sizeVal << "\n";
    if (sizeVal % 2 == 0)
        sizeVal++;

    GaussianBlur(img, changedImg, Size(sizeVal, sizeVal), 0);

    ostringstream kernelSizeText;
    kernelSizeText << "KernelSize = " << sizeVal;
    putText(changedImg, kernelSizeText.str(), Point(30, 30),
        FONT_HERSHEY_SIMPLEX, 0.9, 220, 2);

    imshow("Gaussian blur", changedImg);
}

void convolution(const Mat& in_image, const double* mask, int ksize, double koef, Mat& out_image)
{
	double temp = 0;
	int hk = ksize / 2; // floor(ksize/2)

	for (int i = 1; i < in_image.rows - 1; ++i) {
		for (int j = 1; j < in_image.cols - 1; ++j) {
			for (int ik = 0; ik < ksize; ++ik) {
				for (int jk = 0; jk < ksize; ++jk) {
					//calculating new pixel value
					temp += mask[ik * ksize + jk] * in_image.data[(i + hk - ik) * in_image.rows + (j + hk - jk)];
				}
			}
			out_image.data[i * in_image.rows + j] = (int)temp / koef;
		}
	}
}

void GaussianBlur(const Mat& in_image, Mat& out_image, const int ksize, double sigma)
{
    double* mask = new double[ksize * ksize];
    int hk = ksize / 2; // floor(ksize/2)

    for (int i = 0; i < ksize; ++i) {
        for (int j = 0; j < ksize; ++j) {
            mask[i * ksize + j] = exp(-1 * (pow((i - hk), 2) + pow((j - hk), 2)) / (2 * pow(sigma, 2))) / (2 * M_PI * pow(sigma, 2));
            cout << mask[i * ksize + j] << "  ";
        }
        cout << endl;
    }

	convolution(in_image, mask, ksize, 1, out_image);
}


int main()
{
    std::cout.sync_with_stdio(false);
    std::string imageNames[] = { "IM0.jpg", "norway.jpg", "8by8.jpg", "portrait.jpg", "smallGradient.jpg", "IM8.tif", "IM23.tif", "IM17.tif", "IM13.tif", "IM11.tif" };
    img = imread("C:/Users/Lord/source/repos/Tif/" + imageNames[0], 0);
    changedImg = img.clone();

    if (img.data == 0) // Check for invalid input
    {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    std::cout << "Original image dimensions : " << img.cols << 'x' << img.rows << std::endl;

    namedWindow("Gaussian blur", WINDOW_AUTOSIZE);
    resizeWindow("Gaussian blur", changedImg.cols, changedImg.rows);
    //createTrackbar("Blur sigma", "Gaussian blur", &sliderValue, sliderValueMax, on_trackbar_change);

    GaussianBlur(img, changedImg, 3, 0.75);
    //std::cout << "Fit image dimensions : " << newImg.cols << 'x' << newImg.rows << std::endl;
    imshow("Gaussian blur", changedImg);
    //on_trackbar_change(sliderValue);

    waitKey(0);
    system("pause");
    return 0;
}
