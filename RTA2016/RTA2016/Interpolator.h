#pragma once

#include "Animation.h"

class Intterpolator
{
public:
	Intterpolator();
	~Intterpolator();

	Joint& GetCurrentWorld(int Num);
	float GetTime();
	void SetAnimation(const Animation*);
	void SetTime(float _time);
	void Update(float _time);

private:

	Animation* anim;
	unsigned int currFrame;
	vector<Joint> world;

};

Intterpolator::Intterpolator()
{
}

Intterpolator::~Intterpolator()
{
}

