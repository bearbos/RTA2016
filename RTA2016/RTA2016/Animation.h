#pragma once

#include "keyFrame.h"

class Animation
{
public:
	Animation();
	Animation(int numKeyFrames, float totalTime);
	~Animation();

	const XMFLOAT4X4* GetFrames(int _index);
	int GetNumJoints();
	void Init(int _num, float _time);
	const float GetTotalTime();
	int GetNumKeyFrames();

	std::vector<KeyFrame> keyFrames;
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
}

Animation::~Animation()
{

}

const XMFLOAT4X4* Animation::GetFrames(int _index)
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
}

const float Animation::GetTotalTime()
{
	return totalTime;
}

int Animation::GetNumKeyFrames()
{
	return numKeyFrames;
}