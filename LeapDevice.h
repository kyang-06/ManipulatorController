// LeapDevice : 用于捕获leap motion的各种数据
//

#pragma once
#include "Leap.h"
#include "ArmInfo.h"
#include <Windows.h>

#define OUTPUT
using namespace Leap;
using namespace std;
class MyListener : public Listener {
public:
	MyListener();
	virtual void onConnect(const Controller&);
	virtual void onFrame(const Controller&);
	void setArm(ArmInfo*);
private:
	ArmInfo *mArm;
	long long id; // 运行id
	Vector jointPosition(const Frame&); // 手部某关节位置
	static double AMEND_SPEED;
};

class LeapDevice
{
public:
	LeapDevice(void);
	LeapDevice(ArmInfo*);
	~LeapDevice(void);
	void start();
	void stop();
private:
	MyListener mListener;
	Controller mController;
};

