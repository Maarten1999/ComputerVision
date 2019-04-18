// Threshold van een binaire afbeelding
// Informatie over thresholding: http://docs.opencv.org/doc/tutorials/imgproc/threshold/threshold.html
// Jan Oostindie, dd 22-2-2015

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
	// zie: project properties - configuration properties - debugging - command arguments
	if (argc != 2)
	{
		cout << "NB! Geef als command argument volledige padnaam van de imagefile mee" << endl;
		return -1;
	}
	else cout << "De imagefile = " << argv[1] << endl;

	// Lees de afbeelding in
	Mat image;
	image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	if (!image.data)
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	// De afbeelding converteren naar een grijswaarde afbeelding
	Mat gray_image;
	cvtColor(image, gray_image, CV_BGR2GRAY);

	// Converteren naar grijswaarde afbeelding
	cout << "Imagefile: " << argv[1] << " met succes geconverteerd naar grijswaarde beeld." << endl;

	// Grijswaarde afbeelding thresholden
	Mat binaryx;
	threshold(gray_image, binaryx, 100, 1, CV_THRESH_BINARY);

	namedWindow("Grijswaarde Beeld", WINDOW_AUTOSIZE);
	imshow("Grijswaarde Beeld", gray_image);
	waitKey(0);

	namedWindow("Threshold Binary", WINDOW_AUTOSIZE);
	imshow("Threshold Binary", binaryx*255);
	waitKey(0);

	// Saven op disk om het resultaat in VisionLab nader te kunnen bekijken
	imwrite("c:\\dump\\binairbeeld.bmp", binaryx);

	//waitKey(0);

}
