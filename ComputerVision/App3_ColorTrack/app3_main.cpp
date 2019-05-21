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

int main(int argc, char** argv)
{
	// Controle of er een argument aan het programma is meegegeven.
	if (argc != 2)
	{
		cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}

	// Mat is een class voor objecten waarin een afbeelding kan worden opgeslagen.
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

	cv::waitKey(0);
	return 0;
}
