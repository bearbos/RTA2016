#pragma once

#include "keyFrame.h"

class Animation
{
public:
	Animation();
	Animation(int numKeyFrames, float totalTime);
	~Animation();

	const Joint* GetFrames(int _index);
	int GetNumJoints();
	void Init(int _num, float _time);
	const float GetTotalTime();
	int GetNumKeyFrames();

	KeyFrame* keyFrames;
	float AnimStartTime;
	bool completed = true;

private:

	int numKeyFrames;
	float totalTime;

};

Animation::Animation()
{
}
Animation::Animation(int _numKeyFrames, float _totalTime)
{
	numKeyFrames = _numKeyFrames;
	totalTime = _totalTime;
	keyFrames = new KeyFrame[numKeyFrames];
}

Animation::~Animation()
{
	delete[] keyFrames;
}

const Joint* Animation::GetFrames(int _index)
{
	return keyFrames[_index].world;
}

int Animation::GetNumJoints()
{
	return keyFrames[0].numBones;
}

void Animation::Init(int _num, float _time)
{
	numKeyFrames = _num;
	totalTime = _time;
	keyFrames = new KeyFrame[numKeyFrames];
}

const float Animation::GetTotalTime()
{
	return totalTime;
}

int Animation::GetNumKeyFrames()
{
	return numKeyFrames;
}