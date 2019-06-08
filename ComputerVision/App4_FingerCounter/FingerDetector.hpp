#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/highgui/highgui.hpp>

#define SHOW_DILATION
struct HSV
{
	int h;
	int s;
	int v;

	HSV() {}
	HSV(int h, int s, int v) : h(h), s(s), v(v) {}

};

typedef std::vector<std::vector<cv::Point>> Contours;
typedef std::vector<cv::Point> Fingers;
class FingerDetector
{
public:
	FingerDetector() {}
	FingerDetector(const HSV& minHSV, const HSV& maxHSV);
	~FingerDetector() {};

	// Detecteer vingers
	Fingers detect(cv::Mat& frame);

	// HSV waardes om huidskleur te vinden
	HSV minHSV;
	HSV maxHSV;
private:
	// Hoek waardes voor vingers
	const int inAngleMin = 120; 
	const int inAngleMax = 300;
	const int angleMin = 120;
	const int angleMax = 359;
	const int lengthMin = 10;
	const int lengthMax = 80;

	// Minimale grootte contour
	const int minAreaSize = 2000;

	// Bereid frame voor om op te zoeken.
	cv::Mat prepareFrame(cv::Mat& frame);
	// Vind hand in frame.
	bool findHand(cv::Mat& hsv, Contours* contours, int* largestContour);
	// Vind vingers op de hand.
	Fingers findFingers(cv::Mat& frame, Contours& contours, int largestContour);
	// Vind hoek tussen de vingers.
	double findInnerAngle(double px1, double py1, double px2, double py2, double cx1, double cy1);
};