#include "opencv2/imgproc.hpp"
#include "tinyfiledialogs.h"
#include <corecrt_math_defines.h>
#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;


Mat img, binImg, changedImg, gsImg, changedImgTwo, stackedImg;

int ksizeSlider = 1;
int ksizeSliderMax = 7;

int thSlider = 0;
int thSliderMax = 20;

bool isCorrectPos(int rows, int cols, int posY, int posX)
{
	//cout << "Rows: " << rows << ", Cols:" << cols << ", PosX: " << posY + posX << ", PosY: " << rows * cols << endl;

	if (posY / cols > rows || posX > cols || posY + posX >= rows * cols || posY + posX < 0)
		return false;
	else
		return true;
}

void mySubtract(const Mat& in1, const Mat& in2, Mat& out) {
	for (int i = 0; i < in1.cols * in1.rows;i++) 
		out.data[i] = abs(in1.data[i] - in2.data[i]);
	
}

void ToBinary(const Mat& in, Mat& out, int threshold = 127) {
	for (int i = 0; i < in.rows; i++)
		for (int j = 0; j < in.cols; j++)
			out.data[i * out.cols + j] = in.data[i * out.cols + j] > threshold ? 255 : 0;
}

void Dilute(const Mat& in, Mat& out, int ksize = 3) {
	int hk = ksize / 2;
	bool toDilute = false;
	for (int i = 0; i < in.rows; ++i)
		for (int j = 0; j < in.cols; ++j) {
			toDilute = false;
			for (int ik = 0; ik < ksize; ++ik) {
				for (int jk = 0; jk < ksize; ++jk) 
					if (isCorrectPos(in.rows, in.cols, (i + hk - ik) * in.cols, (j + hk - jk))) 
						if (in.data[(i + hk - ik) * in.cols + (j + hk - jk)] == 255) {
							toDilute = true;
							break;
						}
				if (toDilute)
					break;
			}
			out.data[i * in.cols + j] = toDilute ? 255 : 0;
		}
}

void Erode(const Mat& in, Mat& out, int ksize = 3) {
	int hk = ksize / 2;
	bool toErode = false;
	for (int i = 0; i < in.rows; ++i)
		for (int j = 0; j < in.cols; ++j) {
			toErode = false;
			for (int ik = 0; ik < ksize; ++ik) {
				for (int jk = 0; jk < ksize; ++jk)
					if (isCorrectPos(in.rows, in.cols, (i + hk - ik) * in.cols, (j + hk - jk)))
						if (in.data[(i + hk - ik) * in.cols + (j + hk - jk)] == 0) {
							toErode = true;
							break;
						}
				if (toErode)
					break;
			}
			out.data[i * in.cols + j] = toErode ? 0 : 255;
		}
}

static void on_trackbar_change(int, void* = 0)
{
	int ksizeVal = ((double)ksizeSlider / ksizeSliderMax) * 13;
	int thValue = ((double)thSlider / thSliderMax) * 255;
	if (ksizeVal % 2 == 0)
		ksizeVal++;
	cout << "ksize = " << ksizeVal << "thresholdBinary = " << thValue <<  "\n";

	ToBinary(img, binImg, thValue);
	Dilute(binImg, changedImg, ksizeVal);
	Erode(binImg, changedImgTwo, ksizeVal);
	hconcat(binImg, changedImg, stackedImg);
	hconcat(stackedImg,changedImgTwo, stackedImg);

	putText(stackedImg, "Binary", Point(stackedImg.cols * 1 / 6, 30),
		FONT_HERSHEY_SIMPLEX, 0.9, 220, 2);

	putText(stackedImg,"Diluted", Point(stackedImg.cols* 3 / 6, 30),
		FONT_HERSHEY_SIMPLEX, 0.9, 220, 2);

	putText(stackedImg, "Eroded", Point(stackedImg.cols * 5 / 6, 30),
		FONT_HERSHEY_SIMPLEX, 0.9, 220, 2);
	imshow("Result", stackedImg);


	mySubtract(changedImg, binImg, gsImg);
	imshow("edge", gsImg);
}

const int WORK_MODE = 2;

int main()
{
	namedWindow("Result", WINDOW_AUTOSIZE);

	createTrackbar("Kernel", "Result", &ksizeSlider, ksizeSliderMax, on_trackbar_change);
	createTrackbar("Threshold", "Result", &thSlider, thSliderMax, on_trackbar_change);
	switch (WORK_MODE) {
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
		changedImgTwo = img.clone();
		binImg = img.clone();
		gsImg = binImg.clone();
		on_trackbar_change(0);
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
			binImg = gsImg.clone();
			changedImgTwo = gsImg.clone();
			img = binImg.clone();
			on_trackbar_change(0);
			// wait (10ms) for a key to be pressed
			if (cv::waitKey(10) == 27)
				break;
		}
		break;
	}
	}


	return 0;
}
