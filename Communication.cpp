#include "StdAfx.h"
#include "Communication.h"

#define PORT
#define WRITE2FILE2
#define SENDSPEED2

HWND Communication::hWnd = (HWND) 10068;

Communication::Communication(void)
{
	portState = false;
}


Communication::~Communication(void)
{
}

Communication::Communication(CodeType type)
{
	codeType = type;
}

void Communication::startComm()
{
#ifdef PORT
	BOOL state = mSerial.InitPort(hWnd, portNr, baud);
	if (state > 0)
	{
		portState = true;
		mSerial.StartMonitoring();
	}
	else
	{
		portState = false;
	}
#else
	portState = false;
#endif
	printf("Initiate Port %s\n", portState?"Started":"Failed");
}

void Communication::stopComm()
{
	if (portState)
	{
		mSerial.StopMonitoring();
		mSerial.ClosePort();
		portState = false;
	}
}

void Communication::send(ArmInfo *arm, int status)
{
	char msg[15];
	int *angles = arm->getAngles();

	int startJoint = 1;
	if (status == Object::grabber)
		startJoint = ArmInfo::GRABBER_INIT_JOINT;

#ifdef WRITE2FILE
	FILE *out = fopen("F:/matlab project/angle1.txt", "w");
#endif

#ifdef SENDSPEED
	sprintf(msg,"#%d*", (int)(arm->getSpeed()));
	printf("%s\n", msg);
	mSerial.WriteToPort(msg);
#endif

	for (int i=startJoint; i<=ArmInfo::ANGLE_COUNT; i++)
	{
		//if (i != 5) continue;
		if (codeType == IIICode)
			sprintf(msg, "#%d#%03d*", i-1, angles[i]);
		else
			if (angles[i] > 0)	
				sprintf(msg,"#%d#%04d*", i-1, angles[i]);
			else				
				sprintf(msg,"#%d#1%03d*", i-1, -angles[i]);
		printf("%s\n", msg);
#ifdef PORT
		mSerial.WriteToPort(msg);
#endif

#ifdef WRITE2FILE
		fprintf(out, "%d ", angles[i]);
#endif
	}
		
#ifdef WRITE2FILE
	fclose(out);
#endif
}