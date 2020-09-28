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

//Mat fitToKernel(const Mat& in_image, int ksize)
//{
//    int halfKernel = ksize / 2;
//    cout << "halfKernel = " << halfKernel << endl;
//    Mat fixedImg = Mat(img.rows + 2 * halfKernel, img.cols + 2 * halfKernel, CV_8UC1, Scalar(255));
//
//    cout << "in_image.cols * in_image.rows = " << in_image.cols * in_image.rows << endl;
//
//    int offset = halfKernel * in_image.cols + halfKernel;
//    cout << "offset = " << offset << endl;
//    int accum = 0;
//    for (int i = 0; i < in_image.cols * in_image.rows; i++) {
//        //fixedImg.data[i + halfKernel * in_image.cols + halfKernel] = in_image.data[i];
//
//        if (i % in_image.cols + 1 >= in_image.cols) {
//            accum = 2;
//            cout << " edge pixel, i = " << i << endl;
//        } else
//            accum = 0;
//        int test = i + offset + accum;
//        fixedImg.data[test] = in_image.data[i];
//        //cout << "PixelNum = " << i + halfKernel * in_image.cols + halfKernel << '\n';
//        //cout.flush();
//    }
//
//    return fixedImg;
//}

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
}

void convolution(const Mat& in_image, const double* mask, int ksize, double koef, Mat& out_image)
{
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
    createTrackbar("Blur sigma", "Gaussian blur", &sliderValue, sliderValueMax, on_trackbar_change);

	GaussianBlur(img, changedImg, 3, 0.75);
    //std::cout << "Fit image dimensions : " << newImg.cols << 'x' << newImg.rows << std::endl;
    imshow("Gaussian blur", changedImg);
    on_trackbar_change(sliderValue);

    waitKey(0);
    system("pause");
    return 0;
}
