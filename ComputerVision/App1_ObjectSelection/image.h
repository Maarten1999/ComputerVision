#pragma once

#include <string>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
using std::string;
using cv::Mat;

class Image
{
public:
	Image();
	Image(Mat img);
	~Image();
	bool LoadImage(const string& path);
	Image ConvertColorSpace(int code);
	Image ThresholdImage(double thresValue, double maxValue);

private:
	Mat img;
};

