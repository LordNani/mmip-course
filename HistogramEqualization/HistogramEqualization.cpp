#include "opencv2/imgproc.hpp"
#include <iostream>
#include <map>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#define NDEBUG 1

using namespace cv;
using namespace std;

float INTESITY_MULTIPLIER = 255;
const int intensitySliderMax = 30;
int intensitySlider = 0;
Mat img, changedImg;



void HistogramEqualization(const Mat& in_image, Mat& out_image)
{
    map<size_t, float> intensityMap;
    float totalPixel = in_image.cols * in_image.rows;
    for (size_t i = 0; i < 256; i++) {
        intensityMap.insert(make_pair(i, 0));
    }

    //counting pixels intensity
    for (int i = 0; i < (int)totalPixel; i++) {
        ++intensityMap[in_image.data[i]];
    }

    if (!NDEBUG) {
        int pixelCheckSum = 0;
        for (auto elem : intensityMap) {
            cout << elem.first << " color =  " << elem.second << "\n";
            pixelCheckSum += elem.second;
        }
        cout << "checksum: " << pixelCheckSum << endl;
    }

    // intensity => PMF => CDF
    float cumulitiveValue = 0;
    for (size_t i = 0; i < 256; i++) {
        intensityMap[i] = intensityMap[i] / totalPixel + cumulitiveValue;
        if (intensityMap[i] > 1)
            intensityMap[i] = 1;

        cumulitiveValue = intensityMap[i];
        intensityMap[i] *= INTESITY_MULTIPLIER;
        if (!NDEBUG)
            cout << "Pixel color " << i << " = CDF " << intensityMap[i] << "\n";
    }

    for (int i = 0; i < (int)totalPixel; i++) {
        out_image.data[i] = ((int)floor(intensityMap[in_image.data[i]])) % 256;
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

    double max_val = 255;
    minMaxLoc(histogram, 0, &max_val);

    cv::Mat3b hist_image = cv::Mat3b::zeros(256, 256);
    normalize(histogram, histogram, 0, hist_image.rows, NORM_MINMAX, -1, Mat());

    // visualize each bin
    for (int b = 0; b < 256; b++) {
        float const binVal = histogram.at<float>(b);
        int const height = cvRound(binVal);
        cv::line(hist_image, cv::Point(b, 256 - height), cv::Point(b, 256), cv::Scalar::all(255));
    }

    return hist_image;
}

static void on_trackbar_change(int, void* = 0)
{
	int intensity = ((double)intensitySlider / intensitySliderMax) * 255;
	cout << "intensity = " << intensity << "\n";
	INTESITY_MULTIPLIER = intensity;


	imshow("histogramBefore", computeHistogram(img));
	imshow("imageBefore", img);
	HistogramEqualization(img, changedImg);
	imshow("histogramAfter", computeHistogram(changedImg));
	imshow("imageAfter", changedImg);
}

int main()
{
    string imageNames[] = { "Geneva.tif", "norway.jpg", "bigGradient.jpg", "portrait.jpg", "test.png", "anime.jpg", "IM23.tif", "IM17.tif", "IM13.tif", "IM11.tif" };

    img = imread("C:/Users/Lord/source/repos/Tif/" + imageNames[1], 0);
    changedImg = img.clone();
    ;

    if (img.data == 0) // Check for invalid input
    {
        cout << "Could not open or find the image" << endl;
        return -1;
    }
    cout << "Original image dimensions : " << img.cols << 'x' << img.rows << endl;


    //cout << "Enter intensity" << endl;
    //cin >> INTESITY_MULTIPLIER;

    namedWindow("Eq", WINDOW_AUTOSIZE);
    resizeWindow("Eq", 500,100);
    createTrackbar("Intensity", "Eq", &intensitySlider, intensitySliderMax, on_trackbar_change);
	on_trackbar_change(intensitySlider);
	//imshow("histogramBefore", computeHistogram(img));
	//imshow("imageBefore", img);

    //HistogramEqualization(img, changedImg);

    //imshow("histogramAfter", computeHistogram(changedImg));
    //imshow("imageAfter", changedImg);

    waitKey(0);
    system("pause");
    return 0;
}
