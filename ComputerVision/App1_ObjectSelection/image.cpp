#include "image.h"



Image::Image()
{
}

Image::Image(Mat img)
{
	this->img = img;
}


Image::~Image()
{
}

bool Image::LoadImage(const string& path)
{
	img = cv::imread(path, CV_LOAD_IMAGE_COLOR);
	return !img.data ? false : true;
}

Image Image::ConvertColorSpace(int code)
{
	Mat newImage;
	cv::cvtColor(img, newImage, code);
	return Image(newImage);
}

Image Image::ThresholdImage(double thresValue, double maxValue)
{
	Mat binaryImg;
	cv::threshold(this->img, binaryImg, thresValue, maxValue, CV_THRESH_BINARY_INV);
	return Image(binaryImg);
}