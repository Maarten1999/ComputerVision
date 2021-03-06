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
void detectionWithContours(Mat frame);

#define VIDEO_WINDOW "Video" 
#define KEYPOINT_VIDEO_WINDOW "Keypoints" 
#define ESCAPE_KEY 27 

Mat invertImage(const Mat& img)
{
	return 255 - img;
}

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

	return 0;
}
cv::Ptr<SimpleBlobDetector> GetBlobDetector()
{
	SimpleBlobDetector::Params params;

	//params.minThreshold = 80; // 120 
	//params.maxThreshold = 250; // 150 
	params.minThreshold = 10;
	params.maxThreshold = 200;
	//params.minDistBetweenBlobs = 0; 
	params.filterByArea = true;
	params.minArea = 1000;

	params.filterByCircularity = false;
	params.minCircularity = 0.7;

	/*params.filterByInertia = false;
	params.minInertiaRatio = 0.1;

	params.filterByConvexity = false;
	params.minConvexity = 0.5;*/

	cv::Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

	return detector;
}
void Run()
{
	VideoCapture cap(1);

	// Controle of de camera wordt herkend. 
	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam" << endl;
		return;
	}
	// 640 x 480 
	//cap.set(CV_CAP_PROP_FRAME_WIDTH, 320); 
	//cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240); 
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

		Mat grayImage;
		//Converteer image naar zwart wit waarde. 
		cv::cvtColor(frame, grayImage, CV_BGR2GRAY);

		grayImage = ~grayImage; // bitwise not om kleuren om te draaien 
		detector->detect(grayImage, keypoints);

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

void detectionWithContours(Mat frame)
{
	Mat grayImage;
	cv::cvtColor(frame, grayImage, CV_BGR2GRAY);

	Mat binary;
	cv::threshold(grayImage, binary, 100, 255, CV_THRESH_BINARY);

	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;

	cv::findContours(binary, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	if (contours.size() <= 0)
	{
		cout << "No contours found\n";
		return;
	}

	vector<vector<cv::Point>> filteredBlobs;
	Mat centers = Mat::zeros(0, 2, CV_64FC1);

	for (int i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		double arcLength = cv::arcLength(contours[i], true);

		double circularity = 4 * CV_PI * area / (arcLength * arcLength);

		if (circularity > 0.8)
		{
			filteredBlobs.push_back(contours[i]);

			cv::Moments mu = cv::moments(contours[i], false);
			Mat centerpoint = Mat(1, 2, CV_64FC1);
			centerpoint.at<double>(i, 0) = mu.m10 / mu.m00; // x 
			centerpoint.at<double>(i, 1) = mu.m01 / mu.m00;

			centers.push_back(centerpoint);
		}
	}

	if (filteredBlobs.size() <= 0)
	{
		cout << "No circular blobs found\n";
		return;
	}

	cv::drawContours(grayImage, filteredBlobs, -1, cv::Scalar(0, 0, 255), CV_FILLED, 8);
	cv::imshow(KEYPOINT_VIDEO_WINDOW, grayImage);
	cv::imwrite("test.png", grayImage);
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