// ArmInfo : 用于机械臂各项参数描述
//

#pragma once
#include "Leap.h"
#include "Calculator.h"
using namespace Leap;
enum Object{
	null,
	position,
	grabber,
};
class ArmInfo
{
public:
	ArmInfo(void);
	~ArmInfo(void);
	
	static double L[5]; // 机械臂各个连杆长度

	bool updateEndPoint(Vector);
	void updateSpeed(double);
	double getSpeed();
	int checkUpdate(); // 0:未更新 1:坐标更新 2:抓取器更新

	Vector getEndPoint(); // 将isUpdated重置
	
	void setAngles(double *theta, int size); // theta为弧度制
	int* getAngles();
	
	const static int ANGLE_COUNT = 6;
	const static int GRABBER_INIT_JOINT = 5;

	bool updateGrabber(double roll, double strength);
	const static int grabMaxAngle = 60;

private:
	double mSpeed; // 机械臂运动速度
	Vector endPoint; // 机械臂末端位置
	bool isPositionUpdated; // 坐标更新
	bool isGrabberUpdated; // 抓取器状态更新
	int mAngle[6]; // 各舵机转动角度
	double grabStrength; // 抓取力度
	double grabberRoll; // 抓取器滚动角
	void init(); // 初始化各种参数
};
