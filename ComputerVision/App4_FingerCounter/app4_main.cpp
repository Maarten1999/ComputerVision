#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <Windows.h>

using std::cout;
using std::endl;
using cv::Mat;
using cv::VideoCapture;
using cv::SimpleBlobDetector;
using std::vector;
using std::string;

#define ESCAPE_KEY 27
#define WINDOW_NAME "WINDOW"

typedef cv::Ptr<cv::BackgroundSubtractorMOG2> MOG2Ptr;

Mat subMask(MOG2Ptr sub, const Mat& frame);

int main(int argc, char** argv)
{
	VideoCapture cap(1);

	if (!cap.isOpened())
	{
		std::cout << "Cannot open the video" << std::endl;
		return 1;
	}
	// Get generated widht and height from camera 
	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	MOG2Ptr subtractor;
	//subtractor = cv::createBackgroundSubtractorMOG2(10, 30, false);

	cv::namedWindow(WINDOW_NAME);
	int minH = 0, maxH = 73, minS = 0, maxS = 201, minV = 16, maxV = 255;
	cv::createTrackbar("MinH", WINDOW_NAME, &minH, 180);
	cv::createTrackbar("MaxH", WINDOW_NAME, &maxH, 180);
	cv::createTrackbar("MinS", WINDOW_NAME, &minS, 255);
	cv::createTrackbar("MaxS", WINDOW_NAME, &maxS, 255);
	cv::createTrackbar("MinV", WINDOW_NAME, &minV, 255);
	cv::createTrackbar("MaxV", WINDOW_NAME, &maxV, 255);

	Mat frame;
	while (cv::waitKey(1) != ESCAPE_KEY)
	{
		if (!cap.read(frame))
		{
			cout << "Failed to read frame\n"; 
			break;
		}

		//Mat res = subMask(subtractor, frame);
		Mat hsv;
		cv::cvtColor(frame, hsv, CV_BGR2HSV);
		cv::inRange(hsv, cv::Scalar(minH, minS, minV), cv::Scalar(maxH, maxS, maxV), hsv);
		int blurSize = 5;
		int elementSize = 5;
		cv::medianBlur(hsv, hsv, blurSize);
		cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), cv::Point(elementSize, elementSize));
		cv::dilate(hsv, hsv, element);
		cv::imshow("DILATE", hsv);
		// Contour 
		std::vector<std::vector<cv::Point> > contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(hsv, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
		size_t largestContour = 0;
		for (size_t i = 1; i < contours.size(); i++)
		{
			if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour]))
				largestContour = i;
		}
		cv::drawContours(frame, contours, largestContour, cv::Scalar(255, 0, 0), 1);
		
		// Convex hull
		if (!contours.empty())
		{
			std::vector<std::vector<cv::Point> > hull(1);
			cv::convexHull(cv::Mat(contours[largestContour]), hull[0], false);
			cv::drawContours(frame, hull, 0, cv::Scalar(66, 116, 244), 3);
		}

		cv::imshow(WINDOW_NAME, frame);
	}
	return 0;
}

Mat subMask(MOG2Ptr sub, const Mat& frame)
{
	Mat mask;
	sub->apply(frame, mask, 0);
	
	cv::MatExpr kernel = Mat::ones(cv::Size(4, 4), CV_8U);
	cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel, cv::Point(-1,-1), 2);
	cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel, cv::Point(-1,-1), 2);

	Mat result;
	cv::bitwise_and(frame, frame, result, mask);
	
	return result;
}


