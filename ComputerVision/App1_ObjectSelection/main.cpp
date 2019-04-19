
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

#define THRESHOLD_VALUE			200.0
#define MAIN_WINDOW				"OBJECT_SELECTION"
#define TRACKBAR_MIN			"AREA_SELECT_MIN"
#define TRACKBAR_MIN_MAX_SLIDER 20000
#define TRACKBAR_MAX			"AREA_SELECT_MAX"
#define TRACKBAR_MAX_MAX_SLIDER 40000


Mat LoadImage(const string& path);
int CountObjects(Mat binaryImage);
void DrawObjects();
void OnTrackbarMin(int, void*);
void OnTrackbarMax(int, void *);


int minAreaValue = 14000;
int maxAreaValue = 20000;
Mat binary16S;
int main(int argc, char** argv)
{
	const string imagePath = "D:/Libraries/Pictures/Projecten/opencv/Images/testsetblokjes/testset3_2.bmp";
	Mat origImg = LoadImage(imagePath);

	if (!origImg.data)
	{
		cout << "Kon afbeelding niet laden\n";
		return 1;
	}

	cv::namedWindow(MAIN_WINDOW, CV_WINDOW_AUTOSIZE);
	cv::createTrackbar(TRACKBAR_MIN, MAIN_WINDOW, &minAreaValue, TRACKBAR_MIN_MAX_SLIDER, OnTrackbarMin);
	cv::imshow(MAIN_WINDOW, origImg);
	cv::waitKey(0);

	Mat grayImage;
	cv::cvtColor(origImg, grayImage, CV_BGR2GRAY);

	Mat binaryImage;
	cv::threshold(grayImage, binaryImage, THRESHOLD_VALUE, 1, CV_THRESH_BINARY_INV);

	binaryImage.convertTo(binary16S, CV_16S);
	
	DrawObjects();
	
	
	/*Mat labeledImage;
	vector<Point2d *> firstpixelVec;
	vector<Point2d *> posVec;
	vector<int> areaVec;
	int minArea = 13000;
	int maxArea = 16000;
	labelBLOBsInfo(binary16S, labeledImage,
		firstpixelVec, posVec, areaVec, minArea, maxArea);

	cv::namedWindow("Labeled_Image", CV_WINDOW_AUTOSIZE);
	show16SImageStretch(labeledImage, "Labeled_Image");
	
	cout << "Aantal gevonden objecten = " << firstpixelVec.size() << endl;
	for (int i = 0; i < firstpixelVec.size(); i++) 
	{
		cout << "Object " << i + 1 << endl;
		cout << "firstpixel = (" << firstpixelVec[i]->x << "," << firstpixelVec[i]->y << ")" << endl;
		cout << "centre = (" << posVec[i]->x << "," << posVec[i]->y << ")" << endl;
		cout << "area = " << areaVec[i] << endl;
	}*/
	cv::waitKey(0);
	//Mat labeledImage;
	//const int objects = labelBLOBs(binary16S, labeledImage);

	//cout << "Aantal objecten: " << objects << endl;

	//cv::namedWindow("Labeled_Image", CV_WINDOW_AUTOSIZE);
	//show16SImageStretch(labeledImage, "Labeled_Image");

	//cv::waitKey(0);

	return 0;
}

Mat LoadImage(const string& path)
{
	Mat img = cv::imread(path, CV_LOAD_IMAGE_COLOR);
	return img;
}

int CountObjects(Mat binaryImage)
{
	Mat temp;
	return labelBLOBs(binaryImage, temp);
}

void OnTrackbarMin(int, void*)
{
	DrawObjects();
}
void OnTrackbarMax(int, void*)
{

}

void DrawObjects()
{
	Mat labeledImage;
	vector<Point2d *> firstpixelVec;
	vector<Point2d *> posVec;
	vector<int> areaVec;
	labelBLOBsInfo(binary16S, labeledImage,
		firstpixelVec, posVec, areaVec, minAreaValue, maxAreaValue);

	for (int i = 0; i < firstpixelVec.size(); i++)
	{
		cout << "Object " << i + 1 << endl;
		cout << "firstpixel = (" << firstpixelVec[i]->x << "," << firstpixelVec[i]->y << ")" << endl;
		cout << "centre = (" << posVec[i]->x << "," << posVec[i]->y << ")" << endl;
		cout << "area = " << areaVec[i] << endl;

		//cv::Rect r = cv::Rect(posVec[i]->x, posVec[i]->y, areaVec[i] / 2, areaVec[i] / 2);
		//cv::Point p = cv::Point(posVec[i]->x, posVec[i]->y);
		//cv::circle(labeledImage, p, areaVec[i] , 3);
		//cv::rectangle(, r, (0, 2, 255));
	}

	show16SImageStretch(labeledImage, MAIN_WINDOW);
	
}

//
//int GetAllObjectsInRange(const Mat binary16S, int minArea, int maxArea, Mat& labeledImage, vector<Point2d*>firstPixelVec&, 
//	)
//{
//	Mat labeledImage;
//	vector<Point2d *> firstpixelVec;
//	vector<Point2d *> posVec;
//	vector<int> areaVec;
//	labelBLOBsInfo(binary16S, labeledImage,
//		firstpixelVec, posVec, areaVec, minArea, maxArea);
//}
//
//



