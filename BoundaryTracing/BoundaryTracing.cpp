
#include "PVector.h"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

const int WORK_MODE = 1;

Mat img, changedImg, gsImg;

bool isCorrectPos(int rows, int cols, int posY, int posX)
{
    //cout << "Rows: " << rows << ", Cols:" << in.cols << ", PosX: " << posY + posX << ", PosY: " << rows * in.cols << endl;

    if (posY / cols > rows || posX > cols || posY + posX >= rows * cols || posY + posX < 0)
        return false;
    else
        return true;
}

void ToBinary(const Mat& in, Mat& out, int threshold = 127)
{
    for (int i = 0; i < in.rows; i++)
        for (int j = 0; j < in.cols; j++)
            out.data[i * out.cols + j] = in.data[i * out.cols + j] > threshold ? 255 : 0;
}

void traceBoundaries(Mat& in, Mat& cont)
{

    int startX, startY, prevX, prevY;

    for (int i = 0; i < in.rows; i++)
        for (int j = 0; j < in.cols; j++)
            if (in.data[i * in.cols + j] == 255) {
                prevX = i;
                startX = i;
                prevY = j;
                startY = j;
                cont.data[i * in.cols + j] = 255;
                break;
            }

    int dir = 0;
    int temp_dir = 3;

    while (true) {
        if (temp_dir == 0) {
            if (in.data[prevX * in.cols + prevY + 1] == 255) {
                prevY = prevY + 1;
                dir = 0;
            } else if (in.data[(prevX - 1) * in.cols + prevY] == 255) {
                prevX = prevX - 1;
                dir = 1;
            } else if (in.data[prevX * in.cols + prevY - 1] == 255) {
                prevY = prevY - 1;
                dir = 2;
            } else if (in.data[(prevX + 1) * in.cols + prevY] == 255) {
                prevX = prevX + 1;
                dir = 3;
            }
        } else if (temp_dir == 1) {
            if (in.data[(prevX - 1) * in.cols + prevY] == 255) {
                prevX = prevX - 1;
                dir = 1;
            } else if (in.data[prevX * in.cols + prevY - 1] == 255) {
                prevY = prevY - 1;
                dir = 2;
            } else if (in.data[(prevX + 1) * in.cols + prevY] == 255) {
                prevX = prevX + 1;
                dir = 3;
            } else if (in.data[prevX * in.cols + prevY + 1] == 255) {
                prevY = prevY + 1;
                dir = 0;
            }
        } else if (temp_dir == 2) {
            if (in.data[prevX * in.cols + prevY - 1] == 255) {
                prevY = prevY - 1;
                dir = 2;
            } else if (in.data[(prevX + 1) * in.cols + prevY] == 255) {
                prevX = prevX + 1;
                dir = 3;
            } else if (in.data[prevX * in.cols + prevY + 1] == 255) {
                prevY = prevY + 1;
                dir = 0;
            } else if (in.data[(prevX - 1) * in.cols + prevY] == 255) {
                prevX = prevX - 1;
                dir = 1;
            }
        } else if (temp_dir == 3) {
            if (in.data[(prevX + 1) * in.cols + prevY] == 255) {
                prevX = prevX + 1;
                dir = 3;
            } else if (in.data[prevX * in.cols + prevY + 1] == 255) {
                prevY = prevY + 1;
                dir = 0;
            } else if (in.data[(prevX - 1) * in.cols + prevY] == 255) {
                prevX = prevX - 1;
                dir = 1;
            } else if (in.data[prevX * in.cols + prevY - 1] == 255) {
                prevY = prevY - 1;
                dir = 2;
            }
        }


        temp_dir = (dir + 3) % 4;
        
        cont.data[prevX * in.cols + prevY] = 255;
        if (prevX == startX && prevY == startY)
            break;
    }
}

void myAdaptiveThreshold(const Mat& in, Mat& out)
{
    int hist[256] = { 0 };
    int avgColor = 0;
    //building histogram, and finding T0
    for (int i = 0; i < in.cols * in.rows; ++i) {
        avgColor += in.data[i];
        ++hist[in.data[i]];
    }
    avgColor /= in.cols * in.rows;

    int current, prev, n0, n1, avg1, numen, denom;
    prev = avgColor;
    while (true) {
        denom = 0;
        numen = 0;
        for (int i = 0; i < prev; ++i) {
            numen += i * hist[i];
            denom += hist[i];
        }
        n0 = numen / denom;

        denom = 0;
        numen = 0;
        for (int i = prev + 1; i < 256; ++i) {
            numen += i * hist[i];
            denom += hist[i];
        }
        n1 = numen / denom;

        current = (n0 + n1) / 2;

        if (current == prev)
            break;
        else
            prev = current;
    }

    ToBinary(in, out, current);
    cout << "Adaptive threshold for this img: " << current << endl;
}

int main()
{
    namedWindow("Result", WINDOW_AUTOSIZE);
    for (int i = 1; i >= -1; --i) {
        for (int j = 1; j >= -1; --j) {
            cout << i << "  " << j << endl;
        }
    }
    switch (WORK_MODE) {
    case 1: {
        string imageNames[] = {
            "Geneva.tif", //0
            "norway.jpg", //1
            "art.jpg", //2
            "conc.png ", //3
            "image(3).png", //4
            "anime.jpg", //5
            "IM14.tif", //6
            "8by8.jpg", //7
            "IM_CAT.png" //8
        };
        img = imread("C:/Users/Lord/source/repos/Tif/" + imageNames[4], 0);
        gsImg = img.clone();
        myAdaptiveThreshold(img, gsImg);
        changedImg = gsImg.clone();
        changedImg.setTo(Scalar::all(0));
        //std::cout << "Changed image dimensions : " << changedImg.in.cols << 'x' << changedImg.rows << std::endl;
        //traceBoundaries(img, changedImg);

        traceBoundaries(gsImg, changedImg);
        imshow("Result", changedImg);
        //imshow("Result", gsImg);
        waitKey(0);
        system("pause");
        break;
    }
    }

    return 0;
}
