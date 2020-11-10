#include "opencv2/imgproc.hpp"
#include <corecrt_math_defines.h>
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
	//cout << "Rows: " << rows << ", Cols:" << cols << ", PosX: " << posY + posX << ", PosY: " << rows * cols << endl;

	if (posY / cols > rows || posX > cols || posY + posX >= rows * cols || posY + posX < 0)
		return false;
	else
		return true;
}

void ToBinary(const Mat& in, Mat& out, int threshold = 127) {
	for (int i = 0; i < in.rows; i++)
		for (int j = 0; j < in.cols; j++)
			out.data[i * out.cols + j] = in.data[i * out.cols + j] > threshold ? 255 : 0;
}

void myAdaptiveThreshold(const Mat& in, Mat& out) {
	int hist[256] = { 0 };
	int avgColor = 0;
	//building histogram, and finding T0
	for (int i = 0; i < in.cols * in.rows; ++i) {
		avgColor += in.data[i];
		++hist[in.data[i]];
	}
	avgColor /= in.cols * in.rows;


	int current, prev, n0, n1, avg1,numen,denom;
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
		for (int i = prev+1; i < 256; ++i) {
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
	cout << "Adaptive threshold for this img: " << current <<endl;
}

int main()
{
	namedWindow("Result", WINDOW_AUTOSIZE);

	switch (WORK_MODE) {
	case 1: {
		string imageNames[] = {
			"Geneva.tif", //0
			"norway.jpg",//1
			"art.jpg",//2
			"portrait.jpg",//3
			"smallGradient.jpg",//4
			"anime.jpg",//5
			"IM23.tif",//6
			"8by8.jpg",//7
			"IM_CAT.png"//8
		};
		img = imread("C:/Users/Lord/source/repos/Tif/" + imageNames[2], 0);
		changedImg = img.clone();

		std::cout << "Changed image dimensions : " << changedImg.cols << 'x' << changedImg.rows << std::endl;
		myAdaptiveThreshold(img, changedImg);
		imshow("Result", changedImg);
		waitKey(0);
		system("pause");
		break;
	}
	
	}


	return 0;
}
