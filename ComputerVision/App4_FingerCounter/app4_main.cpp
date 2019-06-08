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
#include "FingerDetector.hpp"
#include "INIReader.h"

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

std::string getExePath();

bool configureDetector(FingerDetector& fingerDetector)
{
	string path = getExePath() + "\\settings.ini";
	INIReader reader(path);

	if (reader.ParseError() != 0)
		return false;

	HSV minHSV, maxHSV;
	minHSV.h = reader.GetInteger("MinHSV", "H", 0);
	minHSV.s = reader.GetInteger("MinHSV", "S", 40);
	minHSV.v = reader.GetInteger("MinHSV", "V", 16);

	maxHSV.h = reader.GetInteger("MaxHSV", "H", 73);
	maxHSV.s = reader.GetInteger("MaxHSV", "S", 200);
	maxHSV.v = reader.GetInteger("MaxHSV", "V", 255);

	fingerDetector.minHSV = minHSV;
	fingerDetector.maxHSV = maxHSV;

	return true;
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

	// Creeër venster.
	cv::namedWindow(WINDOW_NAME);

	FingerDetector detector;
	if (!configureDetector(detector))
	{
		cout << "Failed to read ini file\n";
		return 1;
	}

	HSV* minHSV = &detector.minHSV;
	HSV* maxHSV = &detector.maxHSV;
	cv::createTrackbar("MinH", WINDOW_NAME, &minHSV->h, 180);
	cv::createTrackbar("MaxH", WINDOW_NAME, &maxHSV->h, 180);
	cv::createTrackbar("MinS", WINDOW_NAME, &minHSV->s, 255);
	cv::createTrackbar("MaxS", WINDOW_NAME, &maxHSV->s, 255);
	cv::createTrackbar("MinV", WINDOW_NAME, &minHSV->v, 255);
	cv::createTrackbar("MaxV", WINDOW_NAME, &maxHSV->v, 255);

	Mat frame;
	Fingers prevFingers;
	while (cv::waitKey(1) != ESCAPE_KEY)
	{
		if (!cap.read(frame))
		{
			cout << "Failed to read frame\n";
			break;
		}

		Fingers f = detector.detect(frame);

		std::string text = "Fingers: " + std::to_string(f.size());
		cv::putText(frame, text, cv::Point(20, 30), CV_FONT_NORMAL, 0.8, cv::Scalar(255, 255, 255));
		
		cv::imshow(WINDOW_NAME, frame);
	
		// Code om te vergelijken met vorige aantal vingers om het accurater te maken
		/*const int currentSize = static_cast<int>(f.size());
		const int prevSize = static_cast<int>(prevFingers.size());
		const int sizeDiff = std::abs(currentSize - prevSize);

		int nrToDisplay = 0;
		if (f.size() > 0 && sizeDiff == 1)
		{
			nrToDisplay = prevFingers.size();
		}
		else if (f.size() > 0)
		{
			nrToDisplay = f.size();
		}

		prevFingers = f;*/
	}
	return 0;
}

std::string getExePath()
{
	HMODULE handle = GetModuleHandle(NULL);

	if (handle == NULL)
		return "";
	char path[MAX_PATH];
	GetModuleFileName(handle, path, (sizeof(path)));
	std::string pathStr = std::string(path);
	const size_t index = pathStr.find_last_of("/\\");

	return pathStr.substr(0, index);
}