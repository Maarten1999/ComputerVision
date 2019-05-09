#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <vector>
#include "FRC.hpp"

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

	return 0;
}
cv::Ptr<SimpleBlobDetector> GetBlobDetector()
{
	SimpleBlobDetector::Params params;

	params.filterByArea = true;
	params.minArea = 100;
	params.maxArea = 10000000;

	params.filterByCircularity = true;
	params.minCircularity = 0.4;

	params.filterByConvexity = true;
	params.minConvexity = 0.9;

	params.filterByInertia = true;
	params.minInertiaRatio = 0.1;

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

	FRC frc;
	float lastFrameTime;
	Mat frame;
	while (cv::waitKey(1) != ESCAPE_KEY)
	{
		if (!cap.read(frame))
		{
			cout << "Failed to read frame\n";
			break;
		}
		
		//float frameTime; //= currentTimeSinceStart / 1000.0f;

		//float deltaTime = frameTime - lastFrameTime;
		//lastFrameTime = frameTime;

		//frc.update(deltaTime);


		Mat grayImage, erosion_dst, binaryx, frameKeypoints;

		//Converteer image naar zwart wit waarde.
		cv::cvtColor(frame, grayImage, CV_BGR2GRAY);

		threshold(grayImage, binaryx, 100, 255, CV_THRESH_BINARY);

		Mat element = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5), cv::Point(-1, -1));

		//ivm detecteren van donkere objecten op witte achtergrond moet voor de erosie de kleuren omgedraaid worden
		binaryx = ~binaryx;
		// 8x erosie toepassen	
		erode(binaryx, erosion_dst, element, cv::Point(-1, -1), 8);
		cv::imshow("Eroded", ~erosion_dst);
		binaryx = ~erosion_dst;

		detector->detect(binaryx, keypoints);

		cv::drawKeypoints(grayImage, keypoints, frameKeypoints, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		const string objectCount = std::to_string(keypoints.size());
		const string text = "Objects: " + objectCount;
		cv::putText(frameKeypoints, text, cv::Point(20, 30), CV_FONT_NORMAL, 0.8, cv::Scalar(0, 0, 255));

		//const string fps = 
		cv::imshow(KEYPOINT_VIDEO_WINDOW, frameKeypoints);

		for (const auto& k : keypoints)
		{
			cout << "Keypoints: x = " << k.pt.x << ", y = " << k.pt.y << endl;
		}

		keypoints.clear();
	}
}