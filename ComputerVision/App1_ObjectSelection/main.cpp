
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
#include <chrono>
#include <ctime>
#include <string>
#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif
#include "image.h"
#include "blobdetectionavans.h"

using std::string;
using cv::Mat;
using std::cout;
using std::endl;
using namespace std::chrono;

#define THRESHOLD_VALUE			200.0
#define MAIN_WINDOW				"OBJECT_SELECTION"
#define ORIGINAL_IMG_WINDOW		"ORIGINAL_IMG"
#define TRACKBAR_MIN			"AREA_SELECT_MIN"
#define TRACKBAR_MIN_MAX_SLIDER 20000
#define TRACKBAR_MAX			"AREA_SELECT_MAX"
#define TRACKBAR_MAX_MAX_SLIDER 40000

#define NO_KEY_PRESSED			-1


Mat LoadImage(const string& path);
int CountObjects(Mat binaryImage);
void DrawObjects();
void OnSliderMove(int, void*);
string OpenFileDialog();


int minAreaValue = 14000;
int maxAreaValue = 20000;
Mat binary16S;
time_point<system_clock> slideTime;

bool sliderMoved(false);
int main(int argc, char** argv)
{
	const string imagePath = OpenFileDialog();
	if (imagePath == "")
	{
		cout << "Kon afbeelding niet selecteren\n";
		return 1;
	}
	
	Mat origImg = LoadImage(imagePath);

	if (!origImg.data)
	{
		cout << "Kon afbeelding niet laden\n";
		return 1;
	}

	cv::namedWindow(ORIGINAL_IMG_WINDOW, CV_WINDOW_AUTOSIZE);
	cv::imshow(ORIGINAL_IMG_WINDOW, origImg);

	cv::namedWindow(MAIN_WINDOW, CV_WINDOW_AUTOSIZE);
	cv::createTrackbar(TRACKBAR_MIN, MAIN_WINDOW, &minAreaValue, TRACKBAR_MIN_MAX_SLIDER, OnSliderMove);
	cv::createTrackbar(TRACKBAR_MAX, MAIN_WINDOW, &maxAreaValue, TRACKBAR_MAX_MAX_SLIDER, OnSliderMove);

	Mat grayImage;
	cv::cvtColor(origImg, grayImage, CV_BGR2GRAY);

	Mat binaryImage;
	cv::threshold(grayImage, binaryImage, THRESHOLD_VALUE, 1, CV_THRESH_BINARY_INV);

	binaryImage.convertTo(binary16S, CV_16S);
	
	DrawObjects();
	
	while (cv::waitKey(10) == NO_KEY_PRESSED)
	{
		if (!sliderMoved) continue;

		auto currentTime = system_clock::now();
		duration<double> elapsedTime = currentTime - slideTime;
		
		if (elapsedTime.count() > 1.5)
		{
			DrawObjects();
			sliderMoved = false;
		}
	}	

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

void OnSliderMove(int pos, void* userData)
{
	sliderMoved = true;
	slideTime = system_clock::now();
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

string OpenFileDialog()
{
	char filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;  
	ofn.lpstrFilter = "Images\0*.bmp;*.png;*.jpg\0Any File\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select an image.";
	ofn.Flags = OFN_PATHMUSTEXIST| OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))
	{
		return filename;
	}
	return "";
}

