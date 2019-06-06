// https://www.codeproject.com/Articles/782602/Beginners-guide-to-understand-Fingertips-counting
// https://picoledelimao.github.io/blog/2015/11/15/fingertip-detection-on-opencv/

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
#define DILATE_WINDOW "Dilatie"


float innerAngle(float px1, float py1, float px2, float py2, float cx1, float cy1)
{

	float dist1 = std::sqrt((px1 - cx1)*(px1 - cx1) + (py1 - cy1)*(py1 - cy1));
	float dist2 = std::sqrt((px2 - cx1)*(px2 - cx1) + (py2 - cy1)*(py2 - cy1));

	float Ax, Ay;
	float Bx, By;
	float Cx, Cy;

	//find closest point to C  
	//printf("dist = %lf %lf\n", dist1, dist2);  

	Cx = cx1;
	Cy = cy1;
	if (dist1 < dist2)
	{
		Bx = px1;
		By = py1;
		Ax = px2;
		Ay = py2;


	}
	else {
		Bx = px2;
		By = py2;
		Ax = px1;
		Ay = py1;
	}


	float Q1 = Cx - Ax;
	float Q2 = Cy - Ay;
	float P1 = Bx - Ax;
	float P2 = By - Ay;


	float A = std::acos((P1*Q1 + P2 * Q2) / (std::sqrt(P1*P1 + P2 * P2) * std::sqrt(Q1*Q1 + Q2 * Q2)));

	A = A * 180 / CV_PI;

	return A;
}


int main(int argc, char** argv)
{
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

	// Trackbar waardes.
	int inAngleMin = 120, inAngleMax = 300, angleMin = 120, angleMax = 359, lengthMin = 10, lengthMax = 80;

	// Creeër venster.
	cv::namedWindow(WINDOW_NAME);
	int minH = 0, maxH = 73, minS = 40, maxS = 200, minV = 16, maxV = 255;

	// Maak trackbars aan.
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

		Mat hsv;
		// Converteer naar HSV waarde om bewerkingen op te doen.
		cv::cvtColor(frame, hsv, CV_BGR2HSV);
		// Zoek in kleuren range (huidskleur)
		cv::inRange(hsv, cv::Scalar(minH, minS, minV), cv::Scalar(maxH, maxS, maxV), hsv);

		int blurSize = 5;
		int elementSize = 5;
		// Median blur om aparte pixels te verwijderen.
		cv::medianBlur(hsv, hsv, blurSize);
		// Element voor dilatie.
		cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), cv::Point(elementSize, elementSize));
		// Vul de verwijderde pixels op met dilatie.
		cv::dilate(hsv, hsv, element);
		cv::imshow(DILATE_WINDOW, hsv);

		std::vector<std::vector<cv::Point> > contours;
		std::vector<cv::Vec4i> hierarchy;
		// Vind hand contour
		cv::findContours(hsv, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
		size_t largestContour = 0;
		bool handFound = false;
		// Berekend de contour grootte, als deze te klein is is dit geen hand en wordt deze niet meegenomen.
		for (size_t i = 1; i < contours.size(); i++)
		{
			const double contourArea = cv::contourArea(contours[i]);

			if (contourArea < 2000)
				continue;
			handFound = true;

			const double largestContourArea = cv::contourArea(contours[largestContour]);

			if (contourArea > largestContourArea)
				largestContour = i;
		}

		if (handFound)
		{
			cv::drawContours(frame, contours, largestContour, cv::Scalar(255, 0, 0), 1);

			// Convex hull
			if (!contours.empty())
			{
				std::vector<std::vector<cv::Point> > hull(1);
				Mat largestContourMat = cv::Mat(contours[largestContour]);
				cv::convexHull(largestContourMat, hull[0], false);
				cv::drawContours(frame, hull, 0, cv::Scalar(66, 116, 244), 3);

				if (hull[0].size() > 2)
				{
					vector<cv::Vec4i> convexityDefects;
					vector<int> hullIndexes;
					cv::convexHull(largestContourMat, hullIndexes, true);
					cv::convexityDefects(largestContourMat, hullIndexes, convexityDefects);

					cv::Rect boundingBox = cv::boundingRect(hull[0]);
					cv::rectangle(frame, boundingBox, cv::Scalar(255, 0, 0));
					cv::Point center = cv::Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);

					vector<cv::Point> fingers;
					for (int i = 0; i < convexityDefects.size(); i++)
					{
						cv::Point p1 = contours[largestContour][convexityDefects[i][0]];
						cv::Point p2 = contours[largestContour][convexityDefects[i][1]];
						cv::Point p3 = contours[largestContour][convexityDefects[i][2]];
						cv::line(frame, p1, p3, cv::Scalar(0, 0, 255), 2);
						cv::line(frame, p3, p2, cv::Scalar(0, 0, 255), 2);

						double angle = std::atan2(center.y - p1.y, center.x - p1.x) * 180 / CV_PI;
						double inAngle = innerAngle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
						double length = std::sqrt(std::pow(p1.x - p3.x, 2) + std::pow(p1.y - p3.y, 2));
						if (angle > angleMin - 180 && angle < angleMax - 180 && inAngle > inAngleMin - 180 && inAngle < inAngleMax - 180 && length > lengthMin / 100.0 * boundingBox.height && length < lengthMax / 100.0 * boundingBox.height)
						{
							fingers.push_back(p1);
						}

					}

					for (size_t i = 0; i < fingers.size(); i++)
					{
						cv::circle(frame, fingers[i], 9, cv::Scalar(0, 255, 0), 2);
					}

					std::string text = "Fingers: " + std::to_string(fingers.size());
					cv::putText(frame, text, cv::Point(20, 30), CV_FONT_NORMAL, 0.8, cv::Scalar(255, 255, 255));

				}
			}
		}

		cv::imshow(WINDOW_NAME, frame);
	}
	return 0;
}