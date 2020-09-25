#include "opencv2/imgproc.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <map>
#define NDEBUG 0

using namespace cv;
using namespace std;

void HistogramEqualization(const Mat& in_image, Mat& out_image, Mat& in_histogram, Mat& out_histogram)
{
	map<size_t, float> intensityMap;
	float totalPixel = in_image.cols * in_image.rows;
	for (size_t i = 0; i < 256; i++)
	{
		intensityMap.insert(make_pair(i, 0));
	}

	//counting pixels intensity
	for (int i = 0; i <(int) totalPixel; i++)
	{
		++intensityMap[in_image.data[i]];
	}

	if (!NDEBUG) {
		int pixelCheckSum = 0;
		for (auto elem : intensityMap)
		{
			cout << elem.first << " color =  " << elem.second << "\n";
			pixelCheckSum += elem.second;
		}
		cout << "checksum: " << pixelCheckSum << endl;
	}

	// intensity => PMF => CDF
	float cumulitiveValue = 0;
	for (size_t i = 0; i < 256; i++)
	{
		
		intensityMap[i] = intensityMap[i] / totalPixel + cumulitiveValue;
		if (intensityMap[i] > 1)
			intensityMap[i] = 1;

		cumulitiveValue = intensityMap[i];
		if (!NDEBUG) 
			cout << "Pixel color " << i << "= CDF " << intensityMap[i] << "\n";
	}


}

Mat computeHistogram(Mat& input_image)
{

    Mat histogram;
    int channels[] = { 0 };
    int histSize[] = { 256 };
    float range[] = { 0, 256 };
    const float* ranges[] = { range };

    calcHist(&input_image, 1, channels, Mat(), histogram, 1, histSize, ranges);

    double max_val = 0;
    minMaxLoc(histogram, 0, &max_val);

    cv::Mat3b hist_image = cv::Mat3b::zeros(256, 256);

    // visualize each bin
    for (int b = 0; b < 256; b++) {
        float const binVal = histogram.at<float>(b);
        int const height = cvRound(binVal);
        cv::line(hist_image, cv::Point(b, 256 - height), cv::Point(b, 256), cv::Scalar::all(255));
    }

    return hist_image;
}


int main()
{
    string imageNames[] = { "IM0.jpg", "IM1.tif", "IM_CAT.png","smallGradient.jpg", "IM10.tif", "IM8.tif", "IM23.tif", "IM17.tif", "IM13.tif", "IM11.tif" };

    Mat img = imread("C:/Users/Lord/source/repos/Tif/" + imageNames[3], 0);
    Mat newImg(img.rows, img.cols, CV_8UC1, Scalar(0));

    if (img.data == 0) // Check for invalid input
    {
        cout << "Could not open or find the image" << endl;
        return -1;
    }
    cout << "Original image dimensions : " << img.cols << 'x' << img.rows << endl;

    /*displayHistogram(img);*/

    imshow("histogramBefore", computeHistogram(img));
    imshow("imageBefore", img);
	HistogramEqualization(img, newImg, newImg, newImg);
    //imshow(secondWindowName, newImg);
    waitKey(0);
    system("pause");
    return 0;
}
