#include "FRC.hpp"



FRC::FRC()
{
}


FRC::~FRC()
{
}

float FRC::getQueueAvg(std::deque<float> q)
{
	float avg = 0;

	for (int i = 0; i < q.size(); i++)
		avg += q[i];
	avg /= q.size();
	
	return avg;
}

float FRC::getAverageFramesPerSecond() const
{
	return averageFramesPerSecond;
}
void FRC::update(float deltaTime)
{
	currentFramesPerSecond = 1.0f / deltaTime;

	sampleBuffer.push_back(currentFramesPerSecond);

	if (sampleBuffer.size() > MAXIMUM_SAMPLES)
	{
		sampleBuffer.pop_front();
		averageFramesPerSecond = getQueueAvg(sampleBuffer);
	}
	else
		averageFramesPerSecond = currentFramesPerSecond;

	totalFrames++;
	totalSeconds += deltaTime;
}
