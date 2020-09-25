#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

//change it to decrease
const bool UPSCALE = false;

void decrease(const Mat in_image, Mat& out_image, int n = 2)
{
    //Bilinear implementation of downscaling, done by Roman Gaivoronskyi
    float x_diff, y_diff;

    int upPixel, rightPixel, downPixel, leftPixel, x, y, currentPixel;
    int offset = 0;
    for (int i = 0; i < out_image.rows; i++) {
        for (int j = 0; j < out_image.cols; j++) {

            x = n * j;
            y = n * i;
            x_diff = n * j - x;
            y_diff = n * i - y;
            currentPixel = y * in_image.cols + x;

            upPixel = in_image.data[currentPixel];
            rightPixel = in_image.data[currentPixel + 1];
            downPixel = in_image.data[currentPixel + in_image.cols];
            leftPixel = in_image.data[currentPixel + in_image.cols + 1];

            out_image.data[offset] = (int)(upPixel * (1 - x_diff) * (1 - y_diff) + rightPixel * (x_diff) * (1 - y_diff) + downPixel * (y_diff) * (1 - x_diff) + leftPixel * (x_diff * y_diff));
            ++offset;
            //std::cout << "row = " << i << ", col = " << j << std::endl;
        }
    }
}

void increase(const Mat in_image, Mat& out_image, int n = 2)
{
    //Bilinear implementation of upscaling, done by Roman Gaivoronskyi
    float x_diff, y_diff;

    int upPixel, rightPixel, downPixel, leftPixel, x, y, currentPixel;
    int offset = 0;
	float resizeRatio = (float) 1 / n;
	//std::cout << resizeRatio << std::endl;
    for (int i = 0; i < out_image.rows; i++) {
        for (int j = 0; j < out_image.cols; j++) {

            x = resizeRatio * j;
            y = resizeRatio * i;
            x_diff = resizeRatio * j - x;
            y_diff = resizeRatio * i - y;
            currentPixel = y * in_image.cols + x;

            upPixel = in_image.data[currentPixel];
            rightPixel = in_image.data[currentPixel + 1];
            downPixel = in_image.data[currentPixel + in_image.cols];
            leftPixel = in_image.data[currentPixel + in_image.cols + 1];

            //std::cout << "gray = " << (int)(upPixel * (1 - x_diff) * (1 - y_diff) + rightPixel * (x_diff) * (1 - y_diff) + downPixel * (y_diff) * (1 - x_diff) + leftPixel * (x_diff * y_diff)) << '\n';
            out_image.data[offset] = (int)(upPixel * (1 - x_diff) * (1 - y_diff) + rightPixel * (x_diff) * (1 - y_diff) + downPixel * (y_diff) * (1 - x_diff) + leftPixel * (x_diff * y_diff));
            ++offset;
            //std::cout << "row = " << i << ", col = " << j << std::endl;
        }
    }

}


int main()
{
    std::string firstWindowName = "Before";
    std::string secondWindowName = "After";
	std::string imageNames[] = { "IM0.jpg", "IM1.tif", "IM_CAT.png","smallGradient.jpg", "IM10.tif", "IM8.tif", "IM23.tif", "IM17.tif", "IM13.tif", "IM11.tif" };

    Mat img = imread("C:/Users/Lord/source/repos/Tif/" + imageNames[3], 0);
    Mat newImg;
    if (img.data == 0) // Check for invalid input
    {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    std::cout << "Original image dimensions : " << img.cols << 'x' << img.rows << std::endl;

    if (!UPSCALE) {
        newImg = Mat(img.rows * 2, img.cols * 2, CV_8UC1, Scalar(0));
        std::cout << "Edited image dimensions : " << newImg.cols << 'x' << newImg.rows << std::endl;
        increase(img, newImg);
    } else {
        newImg = Mat(img.rows / 2, img.cols / 2, CV_8UC1, Scalar(0));
        std::cout << "Edited image dimensions : " << newImg.cols << 'x' << newImg.rows << std::endl;
        decrease(img, newImg);
    }

    imshow(firstWindowName, img);
    imshow(secondWindowName, newImg);
    waitKey(0);
    system("pause");
    return 0;
}
