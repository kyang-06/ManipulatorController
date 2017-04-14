// LeapArm.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ArmInfo.h"
#include "Calculator.h"
#include "Communication.h"
#include "LeapDevice.h"
#include <process.h>

#define RECORD2							// 采集手部坐标数据
#define DEBUG2							// 采集转换失败数据
#define RECORD_FILE_NAME "data.txt"
#define DEBUG_FILE_NAME "failData.txt"

using namespace std;
FILE *rfp, *dfp;
struct LeapUnion{
	Communication* comm;
	ArmInfo* arm;
	Calculator* calc;
	LeapUnion(Communication* co, ArmInfo* ar, Calculator* ca)
		:comm(co), arm(ar), calc(ca)
	{}
};

bool btnClosePressed;

void recordData(Vector v)
{
	static int dataSize = 0;
	dataSize++;
	if (dataSize%1000 == 0)
	{
		printf("Data Size: %d\n", dataSize);
		Sleep(100);
	}
	fprintf(rfp, "%.2f %.2f %.2f\n", v.x, v.y, v.z);
}

void __cdecl run(LPVOID lpParamter)
{
	LeapUnion *set = (LeapUnion*) lpParamter;
	ArmInfo* arm = set->arm;
	Communication* comm = set->comm;
	Calculator* calc = set->calc;
	int runtime = 0;
	while (!btnClosePressed)
	{
		
		/****test*****/
		//arm->updateEndPoint(Vector(-11.37, 2.9, -13.55)); // 常规坐标系数据按照(y,z,x)带入
		/*************/

		int status = arm->checkUpdate();
		if (status == 0) continue;
		//printf("Run time: %d\n", ++runtime);

		/*arm->getEndPoint();
		comm->send(arm->getAngles(), ArmInfo::ANGLE_COUNT, IIICode);*/
		Vector point = arm->getEndPoint();
		Vector ls_point = Vector::zero();
		if ((status&Object::position) && calc->calculate(point))
		{
#ifdef RECORD
			recordData(point);
#else
			arm->setAngles(calc->getTheta(), Calculator::thetaCount);
			comm->send(arm, status);
			//Sleep(200);
#endif
		}
		else if ((status & Object::grabber) > 0) // 抓取器有更新动作
		{
			arm->setAngles(NULL, 0);
			comm->send(arm, Object::grabber);
		}
		else
		{
#ifdef DEBUG
			if (ls_point.distanceTo(point) > 1)
			{
				fprintf(dfp, "%.2f %.2f %.2f\n", point.x, point.y, point.z);
			}
			ls_point = point;
#endif
			cout<<"Failed"<<endl;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	
	ArmInfo mechArm;
	LeapDevice leapMotion(&mechArm);
	Communication comm(CodeType::IIICode);
	Calculator calc;

	leapMotion.start();
	comm.startComm();
	//atexit(stopThread);
#ifdef RECORD
	rfp = fopen(FILENAME, "w");
#endif
#ifdef DEBUG
	dfp = fopen(DEBUG_FILE_NAME, "a");
#endif

	cout<<"Press ENTER to quit..."<<endl;
	btnClosePressed = false;
	_beginthread(run, 0, &LeapUnion(&comm, &mechArm, &calc));

	cin.get();
	btnClosePressed = true;
	leapMotion.stop();
#ifdef RECORD
	fclose(rfp);
#endif
#ifdef DEBUG
	fclose(dfp);
#endif

	return 0;
}

