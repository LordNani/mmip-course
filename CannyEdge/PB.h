#pragma once
#include "PVector.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <chrono>
#include <iostream>

using namespace cv;
using namespace std;
using namespace std::chrono;
namespace pb {
class Parallel_process : public cv::ParallelLoopBody {

private:
    cv::Mat& img;
    cv::Mat& retVal;
    double max = 0, min = 0;
    int diff = 1;

    double GAUSS_SIGMA = 1;
    const double KOEF = 0.25;

    const void convolution(Mat& in_image, const double* mask, int ksize, double koef, Mat& out_image) const
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

    const void myGaussianBlur(Mat& in_image, Mat& out_image, const int ksize, double sigma) const
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

    const void modifiedSobelOperator(Mat& in_image, Mat& out_image, vector<PVector>& vecs) const
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

    const bool isCorrectPos(int rows, int cols, int posY, int posX) const
    {
        //cout << "Rows: " << rows << ", Cols:" << cols << ", PosX: " << posY + posX << ", PosY: " << rows * cols << endl;

        if (posY / cols > rows || posX > cols || posY + posX >= rows * cols || posY + posX < 0)
            return false;
        else
            return true;
    }

    const void CannyEdge(Mat& in_image, Mat& out_image, double min, double max) const
    {
        Mat temp = in_image.clone();
        int r = in_image.rows;
        int c = in_image.cols;
        int dir = 0; // direction from 0 to 7, where i = i.angle / 45
        PVector A, B, C;
        vector<PVector> pvectors;

        pvectors.resize(r * c);
        modifiedSobelOperator(in_image, temp, pvectors);
        //imshow("Before surpression", temp);

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

        max = max * 255;
        min = min * 255;
        //cout << "Min/Max: " << min << "/" << max << endl;

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


		for (int i = 0; i < r; ++i)
			for (int j = 0; j < c; ++j)
				if(i < 3)
					out_image.data[i * c + j] = out_image.data[(i + 1) * c + j];
				else if (i + 3 > r)
					out_image.data[i * c + j] = out_image.data[(i - 1) * c + j];
        //imshow("After surpression", out_image);
    }

public:
    Parallel_process(cv::Mat& inputImgage, cv::Mat& outImage, double mn, double mx, int df,double gs)
        : img(inputImgage)
        , retVal(outImage)
        , min(mn)
        , max(mx)
        , diff(df),GAUSS_SIGMA(gs)
    {
    }

    virtual void operator()(const cv::Range& range) const
    {
        for (int i = range.start; i < range.end; i++) {
			
            cv::Mat in(img, cv::Rect(0, (img.rows / diff) * i, img.cols, img.rows / diff));
            cv::Mat out(retVal, cv::Rect(0, (retVal.rows / diff) * i, retVal.cols, retVal.rows / diff));
            //std::cout << "One img : " << out.cols << 'x' << out.rows << std::endl;
            //CannyEdge(in, out, min, max);
			vector<PVector> pvectors;

			pvectors.resize(in.rows * in.cols);
			modifiedSobelOperator(in, out, pvectors);

        }
    }
};
}
