// Threshold van een binaire afbeelding
// Informatie over thresholding: http://docs.opencv.org/doc/tutorials/imgproc/threshold/threshold.html
// Informatie over toevoegen slider: https://docs.opencv.org/2.4/doc/tutorials/highgui/trackbar/trackbar.html
// Jan Oostindie, dd 22-2-2015

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

/// Globale Variabelen voor de slider 
const int threshold_slider_max = 255;
int threshold_slider = 100;
int thresholdvalue = 100;

Mat gray_image;
Mat binaryx;


// Callback functie voor afhandeling trackbar events
void on_trackbar(int, void*)
{
	// waarde ophalen van de slider
	thresholdvalue = threshold_slider;
	
	// met verkregen waarde een operatie uitvoeren
	threshold(gray_image, binaryx, thresholdvalue, 1, CV_THRESH_BINARY);

	// nieuwe resultaat tonen
	imshow("binair beeld", binaryx*255);
}



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
	cvtColor(image, gray_image, CV_BGR2GRAY);

	// Converteren naar grijswaarde afbeelding
	cout << "Imagefile: " << argv[1] << " met succes geconverteerd naar grijswaarde beeld." << endl;

	namedWindow("Grijswaarde Beeld", WINDOW_AUTOSIZE);
	imshow("Grijswaarde Beeld", gray_image);
	waitKey(0);

	/// Initialisatie slider waarde
	namedWindow("binair beeld", WINDOW_AUTOSIZE);
	createTrackbar("Threshold", "binair beeld", &threshold_slider, threshold_slider_max, on_trackbar);

	// Rechtstreeks aanroepen van de callback functie om het eerste beeld te krijgen
	on_trackbar(threshold_slider, 0);
	
	waitKey(0);


}
