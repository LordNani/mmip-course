#include "opencv2/imgproc.hpp"
#include <iostream>
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
	kernelSizeText << "KernelSize = "  << sizeVal;
	putText(changedImg, kernelSizeText.str(), Point(30, 30),
		FONT_HERSHEY_SIMPLEX, 0.9, 220,2);

    imshow("Gaussian blur", changedImg);
}

int main()
{
    std::string imageNames[] = { "IM0.jpg", "norway.jpg", "IM_CAT.png","portrait.jpg" "smallGradient.jpg", "IM8.tif", "IM23.tif", "IM17.tif", "IM13.tif", "IM11.tif" };
    img = imread("C:/Users/Lord/source/repos/Tif/" + imageNames[1], 0);
    //changedImg = Mat(img.rows, img.cols, CV_8UC1, Scalar(0));
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

    imshow("Gaussian blur", changedImg);
	on_trackbar_change(sliderValue);

    waitKey(0);
    system("pause");
    return 0;
}
