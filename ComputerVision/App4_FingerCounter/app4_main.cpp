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
#define WINDOW_NAME "Vinger teller"
typedef cv::Ptr<cv::BackgroundSubtractorMOG2> MOG2Ptr;

Mat subMask(MOG2Ptr sub, const Mat& frame);

int findBiggestContour(vector<vector<cv::Point> > contours);
int IndexOfBiggestContour;

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

	int hueMin = 0, hueMax = 180;
	int saturationMin = 0, saturationMax = 255;
	int valueMin = 0, valueMax = 255;

	cv::namedWindow(WINDOW_NAME);
	cv::createTrackbar("Min Hue", WINDOW_NAME, &hueMin, 180);
	cv::createTrackbar("Max Hue", WINDOW_NAME, &hueMax, 180);
	cv::createTrackbar("Min Saturation", WINDOW_NAME, &saturationMin, 255);
	cv::createTrackbar("Max Saturation", WINDOW_NAME, &saturationMax, 255);
	cv::createTrackbar("Min Value", WINDOW_NAME, &valueMin, 255);
	cv::createTrackbar("Max Value", WINDOW_NAME, &valueMax, 255);



	Mat frame(cv::Size(640, 420), CV_8UC3);
	Mat frame2(cv::Size(640, 420), CV_8UC3);

	while (cv::waitKey(1) != ESCAPE_KEY)
	{
		if (!cap.read(frame))
		{
			cout << "Failed to read frame\n";
			break;
		}

		cv::Size kSize;
		kSize.height = 3;
		kSize.width = 3;

		double sigma = 0.3 *(3 / 2 - 1) + 0.8;
		cv::GaussianBlur(frame, frame, kSize, sigma, 0.0, 4);
		Mat hsv(cv::Size(640, 420), CV_8UC3);
		cv::cvtColor(frame, hsv, CV_BGR2HSV);

		Mat bw(cv::Size(640, 420), CV_8UC1);
		cv::inRange(hsv, cv::Scalar(hueMin, saturationMin, saturationMin), cv::Scalar(hueMax, saturationMax, valueMax), bw);


		vector<cv::Vec4i> hierarchy;
		vector<vector<cv::Point> > contours_hull;


		Mat Erode(cv::Size(640, 420), CV_8UC1);
		cv::erode(bw, Erode, cv::Mat(), cv::Point(-1, -1));

		Mat Dialate(cv::Size(640, 420), CV_8UC1);
		cv::dilate(Erode, Dialate, cv::Mat(), cv::Point(-1, -1), 2);


		cv::findContours(Dialate.clone(), contours_hull, hierarchy, CV_RETR_TREE, CV_CLOCKWISE, cv::Point(0, 0)); // CV_CHAIN_APPROX_SIMPLE

		if (contours_hull.size() > 0)
		{
			/// Find the convex hull object for each contour
			vector<vector<cv::Point> >hull(contours_hull.size());
			//find the defects points for each contour
			vector<vector<cv::Vec4i>> defects(contours_hull.size());

			vector<vector<int> > hullsI(contours_hull.size());

			//find the biggest contour
			IndexOfBiggestContour = findBiggestContour(contours_hull);

			cv::Point2f rect_points[4];
			vector<cv::RotatedRect> minRect(contours_hull.size());

			vector<vector<cv::Point> > contours_poly(contours_hull.size());
			vector<cv::Rect> boundRect(contours_hull.size());



			for (int i = 0; i < contours_hull.size(); i++)
			{
				cv::convexHull(Mat(contours_hull[i]), hull[i], false);
				cv::convexHull(Mat(contours_hull[i]), hullsI[i], false);
				cv::convexityDefects(Mat(contours_hull[i]), hullsI[i], defects[i]);

				if (IndexOfBiggestContour == i)
				{
					minRect[i] = minAreaRect(Mat(contours_hull[i]));

					cv::drawContours(frame2, contours_hull, IndexOfBiggestContour, CV_RGB(255, 255, 255), 2, 8, hierarchy, 0, cv::Point());
					cv::drawContours(frame2, hull, IndexOfBiggestContour, CV_RGB(255, 0, 0), 2, 8, hierarchy, 0, cv::Point());

					cv::approxPolyDP(Mat(contours_hull[i]), contours_poly[i], 3, true);
					boundRect[i] = boundingRect(Mat(contours_poly[i]));

					cv::rectangle(frame2, boundRect[i].tl(), boundRect[i].br(), CV_RGB(0, 0, 0), 2, 8, 0);

					cv::Point2f rect_points[4];
					minRect[i].points(rect_points);
					
					for (int j = 0; j < 4; j++)
					{
						cv::line(frame2, rect_points[j], rect_points[(j + 1) % 4], CV_RGB(255, 255, 0), 2, 8);
					}

				}
			}
			for (int i = 0; i < contours_hull.size(); i++)
			{
				size_t count = contours_hull[i].size();
				std::cout << "Count : " << count << std::endl;
				if (count < 300)
					continue;

				vector<cv::Vec4i>::iterator d = defects[i].begin();

				while (d != defects[i].end()) {
					cv::Vec4i& v = (*d);
					if (IndexOfBiggestContour == i) {

						int startidx = v[0];
						cv::Point ptStart(contours_hull[i][startidx]); // point of the contour where the defect begins
						int endidx = v[1];
						cv::Point ptEnd(contours_hull[i][endidx]); // point of the contour where the defect ends
						int faridx = v[2];
						cv::Point ptFar(contours_hull[i][faridx]); // the farthest from the convex hull point within the defect
						float depth = v[3] / 256; // distance between the farthest point and the convex hull


						if (depth > 20 && depth < 80)
						{
							
							cv::line(frame2, ptStart, ptFar, CV_RGB(0, 255, 0), 2);
							cv::line(frame2, ptEnd, ptFar, CV_RGB(0, 255, 0), 2);
							cv::circle(frame2, ptStart, 4, cv::Scalar(100, 0, 255), 2);
						}

					}
					d++;
				}
			}

			cv::imshow("Erode", Erode);
			cv::imshow("Dialate", Dialate);
			cv::imshow("Original Video", frame);
			cv::imshow("HSV Converted Video", hsv);
			cv::imshow("BW Converted Video", bw);
			cv::imshow("Frame 2", frame2);
		}
	}
	return 0;
}

Mat subMask(MOG2Ptr sub, const Mat& frame)
{
	Mat mask;
	sub->apply(frame, mask, 0);

	cv::MatExpr kernel = Mat::ones(cv::Size(4, 4), CV_8U);
	cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel, cv::Point(-1, -1), 2);
	cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel, cv::Point(-1, -1), 2);

	Mat result;
	cv::bitwise_and(frame, frame, result, mask);

	return result;
}

int findBiggestContour(vector<vector<cv::Point> > contours)
{
	int indexOfBiggestContour = -1;
	int sizeOfBiggestContour = 0;

	for (int i = 0; i < contours.size(); i++) {
		if (contours[i].size() > sizeOfBiggestContour) {
			sizeOfBiggestContour = contours[i].size();
			indexOfBiggestContour = i;
		}
	}
	return indexOfBiggestContour;
}


