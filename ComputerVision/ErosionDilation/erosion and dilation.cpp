// Erosion en Dilation van een grijswaarde afbeelding
// Informatie over deze operaties: http://docs.opencv.org/doc/tutorials/imgproc/erosion_dilatation/erosion_dilatation.html
// Jan Oostindie, dd 16-9-2015

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

int erosion_elem = 0;
int erosion_size = 0;
int dilation_elem = 0;
int dilation_size = 0;
int const max_elem = 2;
int const max_kernel_size = 21;

int main(int argc, char** argv)
{
	// src = bronafbeelding
	// erosion_dst = resultaat na erosion
	// dilation_dst = resultaat na dilation
	Mat src, erosion_dst, dilation_dst;

	// Inladen van een afbeelding
	src = imread(argv[1]);

	// Controle of de afbeelding in orde is.
	if (!src.data)
	{
		return -1;
	}

	// De afbeelding converteren naar een grijswaarde afbeelding
	Mat gray_image;
	cvtColor(src, gray_image, CV_BGR2GRAY);

	// Grijswaarde afbeelding thresholden
	Mat binaryx;
	threshold(gray_image, binaryx, 190, 1, CV_THRESH_BINARY_INV);

	namedWindow("Gray Image", WINDOW_AUTOSIZE);
	imshow("Gray Image", gray_image);

	//namedWindow("Binary Image", WINDOW_AUTOSIZE);
	imshow("Binary Image", binaryx*255);
	waitKey(0);

	// StructuurElement maken
	// Info getStructuringElement: http://docs.opencv.org/modules/imgproc/doc/filtering.html

	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));

	// 10x erosion toepassen
	for (int i = 1; i < 10; i++) {
		erode(binaryx, erosion_dst, element);

		//namedWindow("Erosion", WINDOW_AUTOSIZE);
		imshow("Erosion", erosion_dst * 255);
		waitKey(0);
		binaryx = erosion_dst;
	}

	// 10x Dilation toepassen
	for (int i = 1; i < 10; i++) {
		dilate(binaryx, dilation_dst, element);

		imshow("Dilation", dilation_dst * 255);
		waitKey(0);
		binaryx = dilation_dst;
	}

	return 0;
}

