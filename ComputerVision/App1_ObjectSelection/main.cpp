
/*
 Selecteer objecten in een image die een area hebben tussen een bepaald
 minimum en een bepaald maximum.

 -	Zorg ervoor dat je over tenminste drie afbeeldingen beschikt waarop objecten te zien zijn van verschillende grootte. 
	De objecten liggen los van elkaar.
 
 -	Ontwikkel een visionapplicatie die een afbeelding inleest. De afbeelding wordt getoond op het scherm. Met twee sliders kan een minimum
	en een maximum waarde van een area ingesteld worden. De applicatie laat alleen de objeten zien die een arera hebben die daartussen ligt.
 -	Test de visionapplicatie voor de drie afbeeldingen.
*/

#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <string>
#include "image.h"
#include "blobdetectionavans.h"
// Laad image
// Converteer naar zwart wit
// Threshold image
using std::string;
using cv::Mat;
using std::cout;
using std::endl;

#define THRESHOLD_VALUE 200.0
Mat LoadImage(const string& path);
int main(int argc, char** argv)
{
	const string imagePath = "D:/Libraries/Pictures/Projecten/opencv/Images/testsetblokjes/testset3_2.bmp";
	Mat origImg = LoadImage(imagePath);

	if (!origImg.data)
	{
		cout << "Kon afbeelding niet laden\n";
		return 1;
	}

	cv::namedWindow("Original", CV_WINDOW_AUTOSIZE);
	cv::imshow("Original", origImg);
	cv::waitKey(0);
	Mat grayImage;
	cv::cvtColor(origImg, grayImage, CV_BGR2GRAY);

	Mat binaryImage;
	cv::threshold(grayImage, binaryImage, THRESHOLD_VALUE, 1, CV_THRESH_BINARY_INV);

	Mat binary16S;
	binaryImage.convertTo(binary16S, CV_16S);
	
	Mat labeledImage;
	const int objects = labelBLOBs(binary16S, labeledImage);

	cout << "Aantal objecten: " << objects << endl;

	cv::namedWindow("Labeled_Image", CV_WINDOW_AUTOSIZE);
	show16SImageStretch(labeledImage, "Labeled_Image");

	cv::waitKey(0);

	return 0;
}

Mat LoadImage(const string& path)
{
	Mat img = cv::imread(path, CV_LOAD_IMAGE_COLOR);
	return img;
}




