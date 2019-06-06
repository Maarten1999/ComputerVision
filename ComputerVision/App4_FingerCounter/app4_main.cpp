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

typedef cv::Ptr<cv::BackgroundSubtractorMOG2> MOG2Ptr;

Mat subMask(MOG2Ptr sub, const Mat& frame);

int main(int argc, char** argv)
{
	const auto color_lowest = cv::Scalar(2, 80, 150);//8, 40, 150
	const auto color_highest = cv::Scalar(20, 255, 255); // 20,255,255

	cv::Mat frame, scaled_frame, flipped_frame, blurred, hsv, color_mask;

	VideoCapture cap(0);

	if (!cap.isOpened())
	{
		std::cout << "Cannot open the video" << std::endl;
		return 1;
	}
	// Get generated widht and height from camera 
	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	const auto element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));

	MOG2Ptr subtractor;
	subtractor = cv::createBackgroundSubtractorMOG2(10, 30, false);
	while (cv::waitKey(1) != ESCAPE_KEY)
	{
		std::vector<cv::Mat> contours;
		vector<cv::Vec4i> hierarchy;

		if (!cap.read(frame))
		{
			cout << "Failed to read frame\n"; 
			break;
		}
		Mat res = subMask(subtractor, frame);
		
		cv::imshow("WINDOW", res);

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


