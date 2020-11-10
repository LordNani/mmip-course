#include "PVector.h"
#include "opencv2/imgproc.hpp"
#include "tinyfiledialogs.h"
#include <corecrt_math_defines.h>
#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "PB.h"

using namespace cv;
using namespace std;

const int WORK_MODE = 2;

Mat img, changedImg, gsImg;
double GAUSS_SIGMA = 1;
const double KOEF = 0.25;

int sliderMinVal = 0, sliderMaxVal = 0,sliderSigmaVal = 0;
double minVal = 0, maxVal = 0;
int silderMax = 20;
int silderSigmaMax = 10;


void convolution(const Mat& in_image, const double* mask, int ksize, double koef, Mat& out_image)
{
    double temp = 0;
    int hk = ksize / 2; // floor(ksize/2)
    //cout << "KOEF = " << koef << endl;
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

    convolution(in_image, mask, ksize, sum, out_image);
}

void modifiedSobelOperator(const Mat& in_image, Mat& out_image, vector<PVector>& vecs)
{
    Mat blurredImg, verticalSobel, horizontalSobel;
    blurredImg = in_image.clone();
    horizontalSobel = in_image.clone();
    verticalSobel = in_image.clone();
    int imSize = out_image.cols * out_image.rows;

    const double xMask[] = { -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1 };
    const double yMask[] = { -1, -2, -1,
        0, 0, 0,
        1, 2, 1 };

    myGaussianBlur(in_image, blurredImg, 5, GAUSS_SIGMA);
    convolution(blurredImg, xMask, 3, KOEF, horizontalSobel);
    convolution(blurredImg, yMask, 3, KOEF, verticalSobel);

    for (int i = 0; i < imSize; ++i)
        vecs.at(i) = PVector(horizontalSobel.data[i], verticalSobel.data[i]);
	//if (WORK_MODE != 3)
	//	imshow("Horizontal Sobel", horizontalSobel);
    //imshow("Vertical Sobel", verticalSobel);
    for (int i = 0; i < imSize; ++i) {
        out_image.data[i] = sqrt(pow(verticalSobel.data[i], 2) + pow(horizontalSobel.data[i], 2));
    }
}

bool isCorrectPos(int rows, int cols, int posY, int posX)
{
    //cout << "Rows: " << rows << ", Cols:" << cols << ", PosX: " << posY + posX << ", PosY: " << rows * cols << endl;

    if (posY / cols > rows || posX > cols || posY + posX >= rows * cols || posY + posX < 0)
        return false;
    else
        return true;
}

void CannyEdge(const Mat& in_image, Mat& out_image, double min, double max)
{
	Mat temp = in_image.clone();
	int r = in_image.rows;
	int c = in_image.cols;
	int dir = 0; // direction from 0 to 7, where i = i.angle / 45
	PVector A, B, C;
	vector<PVector> pvectors;

	pvectors.resize(r * c);
	modifiedSobelOperator(in_image, temp, pvectors);
	//if (WORK_MODE != 3)
	//	imshow("After Sobel", temp);

	for (int i = 0; i < r; ++i) {
		for (int j = 0; j < c; ++j) {
			C = pvectors.at(i * c + j);
			dir = C.getDir();

			switch (dir) {
			case 0:
			case 4:
				A = isCorrectPos(r, c, i * c, j - 1) ? A = pvectors.at(i * c + j - 1) : PVector();
				B = isCorrectPos(r, c, i * c, j + 1) ? A = pvectors.at(i * c + j + 1) : PVector();
				break;
			case 1:
			case 5:
				A = isCorrectPos(r, c, (i - 1) * c, j + 1) ? A = pvectors.at((i - 1) * c + j + 1) : PVector();
				B = isCorrectPos(r, c, (i + 1) * c, j - 1) ? A = pvectors.at((i + 1) * c + j - 1) : PVector();
				break;
			case 2:
			case 6:
				A = isCorrectPos(r, c, (i - 1) * c, j) ? A = pvectors.at((i - 1) * c + j) : PVector();
				B = isCorrectPos(r, c, (i + 1) * c, j) ? A = pvectors.at((i + 1) * c + j) : PVector();
				break;
			case 3:
			case 7:
				A = isCorrectPos(r, c, (i - 1) * c, j - 1) ? A = pvectors.at((i - 1) * c + j - 1) : PVector();
				B = isCorrectPos(r, c, (i + 1) * c, j + 1) ? A = pvectors.at((i + 1) * c + j + 1) : PVector();
				break;
			}

			out_image.data[i * c + j] = A.getMag() > C.getMag() || B.getMag() > C.getMag() ? 0 : temp.data[i * c + j];
		}
	}
	//if (WORK_MODE != 3)
	//	imshow("After threshold", out_image);

	max = max * 255;
	min = min * 255;

	for (int i = 0; i < r; ++i) {
		for (int j = 0; j < c; ++j) {
			C = pvectors.at(i * c + j);
			if (C.getMag() < min) //USELESS PIXEL
				out_image.data[i * c + j] = 0;
			else if (C.getMag() > max) //STRONG PIXEL, LEAVE IT
				out_image.data[i * c + j] = 255;
			else //WEAK PIXEL
				out_image.data[i * c + j] = 127;
		}
	}

	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
			if (out_image.data[i * c + j] == 127)
				if ((isCorrectPos(r, c, i * c, j + 1) && pvectors.at(i * c + j + 1).getMag() > max) ||
					(isCorrectPos(r, c, i * c, j - 1) && pvectors.at(i * c + j - 1).getMag() > max) ||
					(isCorrectPos(r, c, (i + 1) * c, j) && pvectors.at((i + 1) * c + j).getMag() > max) ||
					(isCorrectPos(r, c, (i - 1) * c, j) && pvectors.at((i - 1) * c + j).getMag() > max) ||

					(isCorrectPos(r, c, (i - 1) * c, j - 1) && pvectors.at((i - 1) * c + j - 1).getMag() > max) ||
					(isCorrectPos(r, c, (i - 1) * c, j + 1) && pvectors.at((i - 1) * c + j + 1).getMag() > max) ||
					(isCorrectPos(r, c, (i + 1) * c, j + 1) && pvectors.at((i + 1) * c + j + 1).getMag() > max) ||
					(isCorrectPos(r, c, (i + 1) * c, j - 1) && pvectors.at((i + 1) * c + j - 1).getMag() > max))
				{
					out_image.data[i * c + j] = 255;
				}
				else
					out_image.data[i * c + j] = 0;

}

static void on_trackbar_change(int, void* = 0)
{
	minVal = (double)sliderMinVal / silderMax / 4.0;
	maxVal = (double)sliderMaxVal / silderMax / 4.0;
	GAUSS_SIGMA = (double)sliderSigmaVal / silderSigmaMax;
	cout << "MinVal/MaxVal: " << minVal << " " << maxVal << endl;
	cout << "Gauss " << GAUSS_SIGMA  << endl;

	if (WORK_MODE != 3) {
		CannyEdge(img, changedImg, minVal, maxVal);
		imshow("Result", changedImg);
		createTrackbar("Sigma", "Result", &sliderSigmaVal, silderSigmaMax, on_trackbar_change);
		resizeWindow("Result", Size(changedImg.cols * 1.5, changedImg.rows));
	}
}

int main()
{
    namedWindow("Result", WINDOW_AUTOSIZE);

	createTrackbar("Min Threshold", "Result", &sliderMinVal, silderMax, on_trackbar_change);
	createTrackbar("Max Threshold", "Result", &sliderMaxVal, silderMax, on_trackbar_change);
    switch (WORK_MODE) {
    case 1: {
        string imageNames[] = {
            "Geneva.tif",
            "norway.jpg",
            "test.png",
            "portrait.jpg",
            "smallGradient.jpg",
            "anime.jpg",
            "IM23.tif",
            "8by8.jpg",
            "IM_CAT.png"
        };
        img = imread("C:/Users/Lord/source/repos/Tif/" + imageNames[5], 0);
        changedImg = img.clone();

		std::cout << "Changed image dimensions : " << changedImg.cols << 'x' << changedImg.rows << std::endl;
        CannyEdge(img, changedImg, 0.1, 0.2);
		imshow("Result", changedImg);
        waitKey(0);
        system("pause");
        break;
    }
    case 2: {
        string selectedFile = tinyfd_openFileDialog(
            "Select Image", // NULL or ""
            "", // NULL or ""
            0, // 0
            NULL, // NULL {"*.jpg","*.png"}
            "pictures", // NULL | "image files"
            0);

        img = imread(selectedFile, 0);
		changedImg = img.clone();
		std::cout << "Changed image dimensions : " << changedImg.cols << 'x' << changedImg.rows << std::endl;
		CannyEdge(img, changedImg, 0.1, 0.2);
		imshow("Result", changedImg);
		waitKey(0);
		system("pause");
        break;
    }
    case 3: {
		VideoCapture camera = VideoCapture(0, CAP_ANY);
		if (!camera.isOpened()) {
			std::cout << "ERROR: Could not open camera" << std::endl;
			return 1;
		}

		for (;;) {

			camera.read(img);
			gsImg = img.clone();
			cvtColor(img, gsImg, cv::COLOR_BGR2GRAY);
			changedImg = gsImg.clone();
			//std::cout << "Changed image dimensions : " << changedImg.cols << 'x' << changedImg.rows << std::endl;
			parallel_for_(cv::Range(0, 8), pb::Parallel_process(gsImg, changedImg, minVal, maxVal, 8,0.75));

			imshow("Result", changedImg);
			// wait (10ms) for a key to be pressed
			if (cv::waitKey(10) == 27)
				break;
		}
        break;
    }
    }


    return 0;
}
