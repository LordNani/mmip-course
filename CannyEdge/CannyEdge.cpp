#include "CannyEdge.h"
#include "PVector.h"
#include "opencv2/imgproc.hpp"
#include "tinyfiledialogs.h"
#include <corecrt_math_defines.h>
#include <iostream>
#include <iterator>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

Mat img, changedImg;
const double GAUSS_SIGMA = 0.5;
const double KOEF = 0.25;

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

    //imshow("Horizontal Sobel", horizontalSobel);
    //imshow("Vertical Sobel", verticalSobel);
    for (int i = 0; i < imSize; ++i) {
        out_image.data[i] = sqrt(pow(verticalSobel.data[i], 2) + pow(horizontalSobel.data[i], 2));
    }
}

bool isCorrectPos(int rows, int cols, int posY, int posX)
{
	//cout << "Rows: " << rows << ", Cols:" << cols << ", PosX: " << posY + posX << ", PosY: " << rows * cols << endl;
	
    if (posY/cols > rows || posX > cols || posY + posX >= rows * cols || posY + posX < 0)
        return false;
    else
        return true;
}

void CannyEdge(const Mat& in_image, Mat& out_image)
{
    Mat temp = in_image.clone();
    int r = in_image.rows;
    int c = in_image.cols;
    int dir = 0;  // direction from 0 to 7, where i = i.angle / 45
    double max = 0, min = 0; // double thresholds
    PVector A, B, C;
    vector<PVector> pvectors;

    pvectors.resize(r * c);
    modifiedSobelOperator(in_image, temp, pvectors);
	imshow("Before surpression", temp);

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
			C = pvectors.at(i * r + j);
            dir = C.getDir();
			
			if (C.getMag() > max) {
				max = C.getMag();
				cout << "NEW MAX = " << max << endl;
			}

            switch (dir) {
            case 0:
            case 4:
                A = isCorrectPos(r, c, i * c, j - 1) ? A = pvectors.at(i * c + j - 1) : PVector();
                B = isCorrectPos(r, c, i * c, j + 1) ? A = pvectors.at(i * c + j + 1) : PVector();
                break;
            case 1:
            case 5:
                A = isCorrectPos(r, c, i * (c - 1), j + 1) ? A = pvectors.at(i * (c - 1) + j + 1) : PVector();
                B = isCorrectPos(r, c, i * (c + 1), j - 1) ? A = pvectors.at(i * (c + 1) + j - 1) : PVector();
                break;
            case 2:
            case 6:
                A = isCorrectPos(r, c, i * (c - 1), j) ? A = pvectors.at(i * (c - 1) + j) : PVector();
                B = isCorrectPos(r, c, i * (c + 1), j) ? A = pvectors.at(i * (c + 1) + j) : PVector();
                break;
            case 3:
            case 7:
                A = isCorrectPos(r, c, i * (c - 1), j - 1) ? A = pvectors.at(i * (c - 1) + j - 1) : PVector();
                B = isCorrectPos(r, c, i * (c + 1), j + 1) ? A = pvectors.at(i * (c + 1) + j + 1) : PVector();
                break;
            }

			out_image.data[i * r + j] = A.getMag() > C.getMag() || B.getMag() > C.getMag() ? 0 : temp.data[i * r + j];
        }
    }

	min = max * 0.1;
	max = max * 0.5;

	imshow("After surpression", out_image);
}

int main()
{
    std::cout.sync_with_stdio(false);

    std::string imageNames[] = {
		"Geneva.tif",
		"norway.jpg",
		"test.png",
		"portrait.jpg",
		"smallGradient.jpg",
		"anime.jpg",
		"IM23.tif",
		"8by8.jpg"};
    //string selectedFile = tinyfd_openFileDialog(
    //	"Select Image", // NULL or ""
    //	"", // NULL or ""
    //	0, // 0
    //	NULL, // NULL {"*.jpg","*.png"}
    //	"pictures", // NULL | "image files"
    //	0);
    img = imread("C:/Users/Lord/source/repos/Tif/" + imageNames[2], 0);
    //img = imread(selectedFile, 0);
    changedImg = img.clone();

    if (img.data == 0) // Check for invalid input
    {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    std::cout << "Original image dimensions : " << img.cols << 'x' << img.rows << std::endl;

    namedWindow("Original", WINDOW_AUTOSIZE);
    resizeWindow("Original", img.cols, img.rows);
    imshow("Original", img);

    CannyEdge(img, changedImg);

    waitKey(0);
    system("pause");
    return 0;
}
