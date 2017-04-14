#include "StdAfx.h"
#include "ArmInfo.h"

double ArmInfo::L[5] = {0, 9.5, 10, 10, 15};

ArmInfo::ArmInfo(void)
{
	init();
}


ArmInfo::~ArmInfo(void)
{
}

void ArmInfo::init()
{
	isPositionUpdated = false;
	isGrabberUpdated = false;
	grabberRoll = 0;
	grabStrength = 0;
	mAngle[1] = 90;
	mAngle[2] = 120;
	mAngle[3] = 0;
	mAngle[4] = 60;
	mAngle[5] = 90;
	mAngle[6] = 0;
}

int ArmInfo::checkUpdate()
{
	int status = 0;
	if (isPositionUpdated) status += Object::position;
	if (isGrabberUpdated) status += Object::grabber;
	isPositionUpdated = false;
	isGrabberUpdated = false;
	return status;
}
	
Vector ArmInfo::getEndPoint()
{
	return endPoint;
}

bool ArmInfo::updateEndPoint(Vector v)
{
	//printf("Displacement: %.4f\n", endPoint.distanceTo(v));
	if (endPoint.distanceTo(v) > 5e-1)
	{
		endPoint = v;
		isPositionUpdated = true;
		return true;
	}
	return false;
}

void ArmInfo::setAngles(double *theta, int size)
{
	for (int i=1; i<=size; i++)
	{
		int ang = Calculator::round(theta[i] / PI * 180) + 360;
		mAngle[i] = ang % 360;

		/*****test*****/
		if (mAngle[i] == -128)
		{
			int test;
			test = 1;
		}
		/**************/
	}
	
	/*设置关节5旋转角度*/
	mAngle[5] = 180 - Calculator::round(grabberRoll / PI * 180);
	/*设置关节6抓取角度*/
	mAngle[6] = Calculator::round(grabStrength * grabMaxAngle);
}
int* ArmInfo::getAngles()
{
	/*mAngle[1] = 90;
	mAngle[2] = 120;
	mAngle[3] = 0;
	mAngle[4] = 60;
	mAngle[5] = 180 - Calculator::round(grabberRoll / PI * 180);
	mAngle[6] = Calculator::round(grabStrength * grabMaxAngle);*/
	return mAngle;
}

bool ArmInfo::updateGrabber(double roll, double strength)
{
	double dd = abs(roll - grabberRoll), ds = abs(strength - grabStrength);
	if (dd > 3e-2 || ds > 1e-2)
	{
		isGrabberUpdated = true;
		if (roll > 0 && roll < PI) grabberRoll = roll;
		grabStrength = strength;
		return true;
	}
	return false;
}

void ArmInfo::updateSpeed(double speed)
{
	mSpeed = speed;
	//printf("Actual speed: %.2f\n", mSpeed);
}

double ArmInfo::getSpeed()
{
	return mSpeed;
}