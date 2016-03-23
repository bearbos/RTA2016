#pragma once

#include "Animation.h"
#include "Defines.h"
class Interpolator
{
public:
	Interpolator();
	~Interpolator();

	XMFLOAT4X4* GetCurrentWorld();
	float GetTime();
	float GetElapsedTime();
	void SetAnimation(Animation* _anim);
	void SetTime(float _time);
	//void SetAnimStartTime();
	void Update(float _time);
	XMFLOAT4X4 ratioMult(XMFLOAT4X4 _mat, float _float);
	XMFLOAT4X4 matrixAddition(XMFLOAT4X4 _mat1, XMFLOAT4X4 _mat2);
	XMFLOAT4X4 world[50];

private:

	Animation* anim;
	unsigned int currFrame;

};

Interpolator::Interpolator()
{
}

Interpolator::~Interpolator()
{
}

XMFLOAT4X4* Interpolator::GetCurrentWorld()
{
	return world;
}

float Interpolator::GetTime()
{
	//return Renderer::clock.GetElapsedTime();
	return 0.0f;
}

//float  Interpolator::GetElapsedTime()
//{
//	float tempTime = 0.0f;
//
//	tempTime = GetTime() - anim->AnimStartTime;
//
//	return tempTime;
//}

void   Interpolator::SetAnimation(Animation* _anim)
{
	anim = _anim;
}

void   Interpolator::SetTime(float _time)  // need to figure out what this should be
{
	
}

//void   Interpolator::SetAnimStartTime()  
//{
//	if (anim->completed)
//	{
//		anim->AnimStartTime = GetTime();
//	}
//}

void   Interpolator::Update(float _time)
{

	float ratio = 0.0f;
	//bool lastFrame = false;

	if (anim->completed)
	{
		return;
	}
	if (_time > anim->GetTotalTime())
	{
		anim->completed = true;
		return;
	}
	for (int i = 0; i < anim->GetNumKeyFrames(); i++)
	{
		if (i == anim->GetNumKeyFrames() - 1)
		{
			currFrame = i;
			ratio = _time - anim->keyFrames[i].time / anim->GetTotalTime() - anim->keyFrames[i].time;
		}
		else if (_time >= anim->keyFrames[i].time && _time < anim->keyFrames[i+1].time)
		{
			currFrame = i;
			ratio = _time - anim->keyFrames[i].time / anim->keyFrames[i + 1].time - anim->keyFrames[i].time;
			break;
		}
	}
	for (int i = 0; i < anim->GetNumJoints(); i++)
	{
		world[i] =  matrixAddition(ratioMult(anim->keyFrames[currFrame].world[i], 1 - ratio), ratioMult(anim->keyFrames[currFrame + 1].world[i], ratio));
	}
}

XMFLOAT4X4 Interpolator::ratioMult(XMFLOAT4X4 _mat, float _float)
{
	XMFLOAT4X4 tempMat;

	tempMat._11 = _mat._11 * _float;
	tempMat._12 = _mat._12 * _float;
	tempMat._13 = _mat._13 * _float;
	tempMat._14 = _mat._14 * _float;
	tempMat._21 = _mat._21 * _float;
	tempMat._22 = _mat._22 * _float;
	tempMat._23 = _mat._23 * _float;
	tempMat._24 = _mat._24 * _float;
	tempMat._31 = _mat._31 * _float;
	tempMat._32 = _mat._32 * _float;
	tempMat._33 = _mat._33 * _float;
	tempMat._34 = _mat._34 * _float;
	tempMat._41 = _mat._41 * _float;
	tempMat._42 = _mat._42 * _float;
	tempMat._43 = _mat._43 * _float;
	tempMat._44 = _mat._44 * _float;
	

	return tempMat;
}

XMFLOAT4X4 Interpolator::matrixAddition(XMFLOAT4X4 _mat1, XMFLOAT4X4 _mat2)
{
	XMFLOAT4X4 tempMat;

	tempMat._11 = _mat1._11 + _mat2._11;
	tempMat._12 = _mat1._12 + _mat2._12;
	tempMat._13 = _mat1._13 + _mat2._13;
	tempMat._14 = _mat1._14 + _mat2._14;
	tempMat._21 = _mat1._21 + _mat2._21;
	tempMat._22 = _mat1._22 + _mat2._22;
	tempMat._23 = _mat1._23 + _mat2._23;
	tempMat._24 = _mat1._24 + _mat2._24;
	tempMat._31 = _mat1._31 + _mat2._31;
	tempMat._32 = _mat1._32 + _mat2._32;
	tempMat._33 = _mat1._33 + _mat2._33;
	tempMat._34 = _mat1._34 + _mat2._34;
	tempMat._41 = _mat1._41 + _mat2._41;
	tempMat._42 = _mat1._42 + _mat2._42;
	tempMat._43 = _mat1._43 + _mat2._43;
	tempMat._44 = _mat1._44 + _mat2._44;

	return tempMat;
}