#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp"
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

	while (cv::waitKey(1) != ESCAPE_KEY)
	{
		std::vector<cv::Mat> contours;
		vector<cv::Vec4i> hierarchy;

		if (cap.read(frame))
		{
			

		}
	}
	return 0;
}


