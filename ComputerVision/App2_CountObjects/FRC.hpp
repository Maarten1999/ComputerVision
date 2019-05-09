#ifndef FRC_HPP
#define FRC_HPP
#include <deque>

class FRC
{
public:
	FRC();
	void update(float deltaTime);
	~FRC();
private: 
#define MAXIMUM_SAMPLES 100
	long totalFrames;
	float totalSeconds;
	float averageFramesPerSecond;
	float currentFramesPerSecond;
	std::deque<float> sampleBuffer;
	static float getQueueAvg(std::deque<float> q);
};
#endif
