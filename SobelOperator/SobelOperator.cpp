#include "opencv2/imgproc.hpp"
#include "tinyfiledialogs.h"
#include <corecrt_math_defines.h>
#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

const int ksizeSliderMax = 5;
int ksizeSlider = 0;
const int sigmaSliderMax = 30;
int sigmaSlider = 0;
double sigmaValue = 0;
Mat img, changedImg;

void convolution(const Mat& in_image, const double* mask, int ksize, double koef, Mat& out_image)
{
    double temp = 0;
    int hk = ksize / 2; // floor(ksize/2)
    cout << "KOEF = " << koef << endl;
    for (int i = hk; i < in_image.rows - hk; ++i) {
        for (int j = hk; j < in_image.cols - hk; ++j) {
            for (int ik = 0; ik < ksize; ++ik) {
                for (int jk = 0; jk < ksize; ++jk) {
                    //calculating new pixel value
                    temp += mask[ik * ksize + jk] * in_image.data[(i + hk - ik) * in_image.cols + (j + hk - jk)];
                }
            }
            //cout <<"new value for pixel: " <<  (int)temp / koef << endl;
            temp = abs(temp * koef) > 255 ? 255 : abs(temp * koef);
            out_image.data[i * in_image.cols + j] = (int)temp;
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

void SobelOperator(const Mat& in_image, Mat& out_image)
{
    Mat blurredImg, verticalSobel, horizontalSobel;
    blurredImg = in_image.clone();
    verticalSobel = in_image.clone();
    horizontalSobel = in_image.clone();

    const double sigma = 0.73;
    const double xMask[] = { -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1 };
    const double yMask[] = { -1, -2, -1,
        0, 0, 0,
        1, 2, 1 };
    const double koef = 0.25;

    myGaussianBlur(in_image, blurredImg, 5, sigmaValue);
    //imshow("Gaussian", blurredImg);
    convolution(blurredImg, xMask, 3, koef, horizontalSobel);
    convolution(blurredImg, yMask, 3, koef, verticalSobel);
    //imshow("Horizontal Sobel", horizontalSobel);
    //imshow("Vertical Sobel", verticalSobel);
    for (int i = 0; i < out_image.cols * out_image.rows; ++i) {
        out_image.data[i] = sqrt(pow(verticalSobel.data[i], 2) + pow(horizontalSobel.data[i], 2));
    }
}

static void on_trackbar_change(int, void* = 0)
{
    //int ksizeVal = ((double)ksizeSlider / ksizeSliderMax) * 9;
    sigmaValue = ((double)sigmaSlider / sigmaSliderMax) * 2;
    cout << "sigma" << sigmaValue << "\n";
    imshow("Original", img);

    SobelOperator(img, changedImg);
    imshow("Sobel", changedImg);
}

//static void on_trackbar_change2(int, void* = 0)
//{
//    on_trackbar_change(ksizeSlider);
//}

int main()
{
    std::cout.sync_with_stdio(false);
    std::string imageNames[] = { "Geneva.tif", "norway.jpg", "artem_1.jpg", "portrait.jpg", "asya.jpg", "anime.jpg", "IM23.tif", "IM17.tif", "IM13.tif", "IM11.tif" };
    string selectedFile = tinyfd_openFileDialog(
        "Select Image", // NULL or ""
        "", // NULL or ""
        0, // 0
        NULL, // NULL {"*.jpg","*.png"}
        "pictures", // NULL | "image files"
        0);
    //img = imread("C:/Users/Lord/source/repos/Tif/" + imageNames[5], 0);
    img = imread(selectedFile, 0);
    changedImg = img.clone();

    if (img.data == 0) // Check for invalid input
    {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    std::cout << "Original image dimensions : " << img.cols << 'x' << img.rows << std::endl;

    namedWindow("Original", WINDOW_AUTOSIZE);
    resizeWindow("Original", img.cols, img.rows);
    //createTrackbar("KernelSize", "Gaussian blur", &ksizeSlider, ksizeSliderMax, on_trackbar_change);
    createTrackbar("Sigma", "Original", &sigmaSlider, sigmaSliderMax, on_trackbar_change);

    imshow("Original", img);
    //imshow("Gaussian blur", changedImg);

    SobelOperator(img, changedImg);
    imshow("Sobel", changedImg);

    waitKey(0);
    system("pause");
    return 0;
}
