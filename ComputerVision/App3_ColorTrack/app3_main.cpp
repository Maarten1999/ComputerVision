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
	const auto color_lowest = cv::Scalar(2, 128, 150);//8, 40, 150
	const auto color_highest = cv::Scalar(20, 255, 255); // 20,255,255

	cv::Mat frame, scaled_frame, flipped_frame, blurred, hsv, color_mask;

	VideoCapture cap(1);

	if(!cap.isOpened())
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
			cv::flip(frame, flipped_frame, +1);
			cv::GaussianBlur(flipped_frame, blurred, cv::Size(5, 5), 0);

			cv::cvtColor(blurred, hsv, CV_BGR2HSV); //Convert the blurred image to HSV.
		
			cv::inRange(hsv, color_lowest, color_highest, color_mask);

			//Erode & Dilate to remove smaller objects
			cv::erode(color_mask, color_mask, element, cv::Point(-1, -1), 5);
			cv::dilate(color_mask, color_mask, element, cv::Point(-1, -1), 5);
		
			//-------OBJECT-DETECTION------//
			cv::findContours(color_mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

			if (contours.size() > 0) cout << "\n\nContours: " << contours.size() << endl;
			else cout << "\n\nNo contours found" << endl;

			std::vector<cv::Moments> mu(contours.size());

			for (auto i = 0; i < contours.size(); i++)
			{
				mu[i] = cv::moments(contours[i], false); //Add the contours to the moments vector
			}

			// get the centroid of the objects.
			std::vector<cv::Point2f> mc(contours.size());
			for (auto i = 0; i < contours.size(); i++)
			{
				mc[i] = cv::Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
			}

			for (auto i = 0; i < contours.size(); i++)
			{
				cv::Scalar color = cv::Scalar(0, 0, 255); // B G R values
				drawContours(flipped_frame, contours, i, color, 2, cv::LINE_8, hierarchy, 0);
				circle(flipped_frame, mc[i], 4, color, CV_FILLED, 8, 0); //draw circle at center of contour
				
				cout << "Center: x = " << mc[i].x << ", y = " << mc[i].x << endl;//print center of contour in console
			}
			cv::imshow("Contours", flipped_frame); //Show the image with the contours

		}
	}
	return 0;
}

/*
 * // Mat is een class voor objecten waarin een afbeelding kan worden opgeslagen.
	Mat image;

	// Lees de afbeelding in en sla deze op in image. 
	// De filenaam is het eerste argument dat meegegeven is bij aanroep van het programma.
	image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

	// Controleer of alles goed is gegaan
	if (!image.data)
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	// Laat de afbeelding zien in een apart window
	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
	imshow("Display window", image);

	// Create a new matrix to hold the HSV image
	Mat HSV;

	// convert RGB image to HSV
	cvtColor(image, HSV, CV_BGR2HSV);



	//// red color range
	cv::Scalar hsv_l(170, 150, 150);
	cv::Scalar hsv_h(180, 255, 255);
	Mat bw;
	inRange(HSV, hsv_l, hsv_h, bw);
	imshow("Specific Colour", bw);
	////

	// hue value

	//define ranges
 */