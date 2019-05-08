#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using cv::Mat;
using cv::VideoCapture;
using cv::SimpleBlobDetector;
using std::vector;
using std::string;

void Run();
void findContours(Mat img);

#define VIDEO_WINDOW "Video"
#define KEYPOINT_VIDEO_WINDOW "Keypoints"
#define ESCAPE_KEY 27



int Init()
{
	//cv::namedWindow(VIDEO_WINDOW, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(KEYPOINT_VIDEO_WINDOW, CV_WINDOW_AUTOSIZE);
	return 0;
}
int main(int argc, char** argv)
{
	Init();

	Run();

	std::cin.get();
	return 0;
}
cv::Ptr<SimpleBlobDetector> GetBlobDetector()
{
	SimpleBlobDetector::Params params;

	params.minThreshold = 120;
	params.maxThreshold = 150;
	params.filterByArea = true;
	params.minArea = 50;
	params.maxArea = 1000000;
	//params.minDistBetweenBlobs = 5;
	//params.blobColor = 255;

	//params.filterByCircularity = true;
	//params.minCircularity = 0.8;

	params.filterByConvexity = true;
	params.minConvexity = 0.1;

	params.filterByInertia = false;
	params.minInertiaRatio = 0.01;


	cv::Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

	return detector;
}
void Run()
{
	VideoCapture cap(0);

	// Controle of de camera wordt herkend.
	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam" << endl;
		return;
	}
	// Breedte en hooogte van de frames die de camera genereert ophalen. 
	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	cv::Ptr<SimpleBlobDetector> detector = GetBlobDetector();
	vector<cv::KeyPoint> keypoints;

	Mat frame;
	while (cv::waitKey(1) != ESCAPE_KEY)
	{
		if (!cap.read(frame))
		{
			cout << "Failed to read frame\n";
			break;
		}

		//cv::imshow(VIDEO_WINDOW, frame);

		Mat grayImage, dilation_dst, Gaussianblur, canny;
		//Converteer image naar zwart wit waarde.
		cv::cvtColor(frame, grayImage, CV_BGR2GRAY);

		Mat binaryx;
		threshold(grayImage, binaryx, 150, 1, CV_THRESH_BINARY);

		Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));

		// 10x dilation toepassen
		for (int i = 1; i < 10; i++) {
			dilate(binaryx, dilation_dst, element);

			cv::imshow("Dilation", dilation_dst * 255);
			binaryx = dilation_dst * 255;
		}

		cv::GaussianBlur(grayImage, Gaussianblur, cv::Size(3, 3), 3, 3, cv::BORDER_DEFAULT);
		cv::Canny(Gaussianblur, canny, 60, 100, 3, false);
		cv::imshow("Edge Detected Image", canny);

		detector->detect(binaryx, keypoints);

		Mat frameKeypoints;
		cv::drawKeypoints(grayImage, keypoints, frameKeypoints, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		const string objectCount = std::to_string(keypoints.size());

		const string text = "Objects: " + objectCount;
		cv::putText(frameKeypoints, text, cv::Point(20, 30), CV_FONT_NORMAL, 0.8, cv::Scalar(0, 0, 255));

		cv::imshow(KEYPOINT_VIDEO_WINDOW, frameKeypoints);

		for (const auto& k : keypoints)
		{
			cout << "Keypoints: x = " << k.pt.x << ", y = " << k.pt.y << endl;
		}

		keypoints.clear();
	}
}

// https://stackoverflow.com/questions/44633740/opencv-simple-blob-detection-getting-some-undetected-blobs
void findContours(Mat img)
{
	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	cv::findContours(img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	// draw contours:
	Mat imgWithContours = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
	cv::RNG rng(12345);
	for (int i = 0; i < contours.size(); i++)
	{
		if (cv::contourArea(contours[i]) > 500) {
			cv::Scalar color = cv::Scalar(rng.uniform(50, 255), rng.uniform(50, 255), rng.uniform(50, 255));
			cv::drawContours(imgWithContours, contours, i, color, 1, 8, hierarchy, 0);
		}
	}
	cv::imshow(KEYPOINT_VIDEO_WINDOW, imgWithContours);
}