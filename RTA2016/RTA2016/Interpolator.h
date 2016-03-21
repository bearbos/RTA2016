#pragma once

#include "Animation.h"

class Interpolator
{
public:
	Interpolator();
	~Interpolator();

	Joint& GetCurrentWorld(int Num);
	float GetTime();
	void SetAnimation(const Animation* _anim);
	void SetTime(float _time);
	void Update(float _time);

private:

	const Animation* anim;
	unsigned int currFrame;
	vector<Joint> world;

};

Interpolator::Interpolator()
{
}

Interpolator::~Interpolator()
{
}

Joint& Interpolator::GetCurrentWorld(int Num)
{
	return world[0];
}

float  Interpolator::GetTime()
{
	return 1;
}

void   Interpolator::SetAnimation(const Animation* _anim)
{
	anim = _anim;
}

void   Interpolator::SetTime(float _time)
{

}

void   Interpolator::Update(float _time)
{

}