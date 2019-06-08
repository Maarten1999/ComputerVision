#include "FingerDetector.hpp"


using cv::Mat;
using cv::Scalar;
using cv::Point;
using std::cout;
using std::endl;
using std::vector;
using std::string;

FingerDetector::FingerDetector(const HSV& minHSV, const HSV& maxHSV)
{
	this->minHSV = minHSV;
	this->maxHSV = maxHSV;
}

Fingers FingerDetector::detect(Mat& frame)
{
	Mat hsv = prepareFrame(frame);
	Contours contours;
	int largestContour;
	const bool found = findHand(hsv, &contours, &largestContour);

	Fingers fingers;
	if (!found)
		return fingers;

	fingers = findFingers(frame, contours, largestContour);
	
	return fingers;
}

Mat FingerDetector::prepareFrame(Mat& frame)
{
	Mat hsv;
	// Converteer naar HSV waarde om bewerkingen op te doen.
	cv::cvtColor(frame, hsv, CV_BGR2HSV);
	// Zoek in kleuren range (huidskleur)
	cv::inRange(hsv, cv::Scalar(minHSV.h, minHSV.s, minHSV.v), cv::Scalar(maxHSV.h, maxHSV.s, maxHSV.v), hsv);

	int blurSize = 5;
	int elementSize = 5;
	// Median blur om aparte pixels te verwijderen.
	cv::medianBlur(hsv, hsv, blurSize);
	// Element voor dilatie.
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), cv::Point(elementSize, elementSize));
	// Vul de verwijderde pixels op met dilatie.
	cv::dilate(hsv, hsv, element);

#ifdef SHOW_DILATION
	cv::imshow("Dilatie", hsv);
#endif
	return hsv;
}

bool FingerDetector::findHand(Mat& hsv, Contours* contours, int* largestContour)
{
	//Contours contours;
	std::vector<cv::Vec4i> hierarchy;
	// Vind hand contour
	cv::findContours(hsv, *contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	*largestContour = 0;
	bool handFound = false;
	// Berekend de contour grootte, als deze te klein is is dit geen hand en wordt deze niet meegenomen.
	for (int i = 1; i < (*contours).size(); i++)
	{
		const double contourArea = cv::contourArea((*contours)[i]);

		if (contourArea < minAreaSize)
			continue;
		handFound = true;
		
		const double largestContourArea = cv::contourArea((*contours)[*largestContour]);

		if (contourArea > largestContourArea)
			*largestContour = i;
	}

	return handFound;
}

Fingers FingerDetector::findFingers(Mat& frame, Contours& contours, int largestContour)
{
	// Teken de contours
	cv::drawContours(frame, contours, largestContour, Scalar(255, 0, 0), 1);

	Fingers fingers;

	if (!contours.empty())
	{
		vector<vector<Point> > hull(1);
		Mat largestContourMat = Mat(contours[largestContour]);
		cv::convexHull(largestContourMat, hull[0], false);
		cv::drawContours(frame, hull, 0, Scalar(66, 116, 244), 3);

		// Vingertoppen binnen convex hull regio.
		if (hull[0].size() > 2)
		{
			// Definieër convexDefects array.
			vector<cv::Vec4i> convexityDefects;
			vector<int> hullIndexes;
			cv::convexHull(largestContourMat, hullIndexes, true);
			// Vind doormiddel van convexityDefects waar de vingertoppen zich bevinden
			cv::convexityDefects(largestContourMat, hullIndexes, convexityDefects);

			// Maak een bounding box om de hand heen om de vingers er binnen te vinden.
			cv::Rect boundingBox = cv::boundingRect(hull[0]);
			cv::rectangle(frame, boundingBox, Scalar(255, 0, 0));
			// Middelpunt van bounding box.
			cv::Point center = Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);

			// Loop de convexity defect array.
			for (int i = 0; i < convexityDefects.size(); i++)
			{
				cv::Point p1 = contours[largestContour][convexityDefects[i][0]];
				cv::Point p2 = contours[largestContour][convexityDefects[i][1]];
				cv::Point p3 = contours[largestContour][convexityDefects[i][2]];
				cv::line(frame, p1, p3, cv::Scalar(0, 0, 255), 2);
				cv::line(frame, p3, p2, cv::Scalar(0, 0, 255), 2);

				double angle = std::atan2(center.y - p1.y, center.x - p1.x) * 180 / CV_PI;
				double inAngle = findInnerAngle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
				double length = std::sqrt(std::pow(p1.x - p3.x, 2) + std::pow(p1.y - p3.y, 2));
				// Liggen de vingers tussen de aangegeven hoeken, en liggen ze in de bounding box, dan zijn ze gevonden.
				if (angle > angleMin - 180 && angle < angleMax - 180 && inAngle > inAngleMin - 180 && inAngle < inAngleMax - 180 && length > lengthMin / 100.0 * boundingBox.height && length < lengthMax / 100.0 * boundingBox.height)
				{
					fingers.push_back(p1);
				}

			}

			for (size_t i = 0; i < fingers.size(); i++)
			{
				cv::circle(frame, fingers[i], 9, cv::Scalar(0, 255, 0), 2);
			}
		}
	}
	
	return fingers;
}

// Vind hoek tussen vingers.
double FingerDetector::findInnerAngle(double px1, double py1, double px2, double py2, double cx1, double cy1)
{
	double dist1 = std::sqrt((px1 - cx1)*(px1 - cx1) + (py1 - cy1)*(py1 - cy1));
	double dist2 = std::sqrt((px2 - cx1)*(px2 - cx1) + (py2 - cy1)*(py2 - cy1));

	double Ax, Ay;
	double Bx, By;
	double Cx, Cy;

	Cx = cx1;
	Cy = cy1;
	if (dist1 < dist2)
	{
		Bx = px1;
		By = py1;
		Ax = px2;
		Ay = py2;
	}
	else
	{
		Bx = px2;
		By = py2;
		Ax = px1;
		Ay = py1;
	}


	double Q1 = Cx - Ax;
	double Q2 = Cy - Ay;
	double P1 = Bx - Ax;
	double P2 = By - Ay;


	double A = std::acos((P1*Q1 + P2 * Q2) / (std::sqrt(P1*P1 + P2 * P2) * std::sqrt(Q1*Q1 + Q2 * Q2)));
	
	A = A * 180 / CV_PI;

	return A;
}