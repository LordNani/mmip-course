#include "opencv2/imgproc.hpp"
#include <corecrt_math_defines.h>
#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

const int ksizeSliderMax = 5;
int ksizeSlider = 0;
const int sigmaSliderMax = 20;
int sigmaSlider = 0;
Mat img, changedImg, changedImg2;

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
    double sum = 0;
    for (int i = 0; i < ksize; ++i) {
        for (int j = 0; j < ksize; ++j) {
            dist = pow((i - hk), 2) + pow((j - hk), 2);
            cout << dist << endl;
            mask[i * ksize + j] = exp(-1.0 * (dist / denumEuler)) / denumMain;
            sum += mask[i * ksize + j];
            //cout << mask[i * ksize + j] << "  ";
        }
        //cout << endl;
    }

    //for (int i = 0; i < ksize * ksize; ++i) {
    //	mask[i] /= sum;
    //	cout << mask[i] << "  " << endl;
    //}

    convolution(in_image, mask, ksize, sum, out_image);
}

static void on_trackbar_change(int, void* = 0)
{
    int ksizeVal = ((double)ksizeSlider / ksizeSliderMax) * 9;
    double sigmaVal = ((double)sigmaSlider / sigmaSliderMax) * 5;
    cout << "ksize = " << ksizeVal << ", sigma = " << sigmaVal << "\n";
    if (ksizeVal % 2 == 0)
        ksizeVal++;
    myGaussianBlur(img, changedImg, ksizeVal, sigmaVal);

    ostringstream kernelSizeText;
    kernelSizeText << "ksize = " << ksizeVal << ", sigma = " << sigmaVal;
    putText(changedImg, kernelSizeText.str(), Point(20, 30),
        FONT_HERSHEY_SIMPLEX, 0.9, 220, 2);

    imshow("Gaussian blur", changedImg);
}

static void on_trackbar_change2(int, void* = 0)
{
    on_trackbar_change(ksizeSlider);
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

    //namedWindow("Gaussian blur", WINDOW_AUTOSIZE);
    //resizeWindow("Gaussian blur", changedImg.cols, changedImg.rows);
    //createTrackbar("KernelSize", "Gaussian blur", &ksizeSlider, ksizeSliderMax, on_trackbar_change);
    //createTrackbar("Sigma", "Gaussian blur", &sigmaSlider, sigmaSliderMax, on_trackbar_change2);

    imshow("Original", img);
    //imshow("Gaussian blur", changedImg);

    double shrp_mask[] = { 1, 2, 1,
        2, 4, 2,
        1, 2, 1 };

	//	double shrp_mask[] = { 0, -1, 0,
	//- 1, 5, -1,
	//0, -1, 0 };

    double koeff = 0;
    for (int i = 0; i < 9; ++i) {
        koeff += shrp_mask[i];
    }

    convolution(img, shrp_mask, 3,1 / koeff, changedImg);
    imshow("convolution", changedImg);

    waitKey(0);
    system("pause");
    return 0;
}
