// Communication : 用于上位机和机械臂控制器之间的数据通信（串口）
//

#pragma once
#include <Windows.h>
#include "SerialPort.h"
#include "ArmInfo.h"
#include <fstream>

enum CodeType
{
	IIICode, // 三位角度码 eg:#1#010*
	IVCode   // 四位角度码 eg:#1#0010* （首位符号位）
};

class Communication
{
public:
	Communication(CodeType);
	Communication(void);
	~Communication(void);
	void send(ArmInfo*, int);
	bool checkPortState();
	void startComm();
	void stopComm();

private:
	CSerialPort mSerial;
	static HWND hWnd;
	const static UINT portNr = 9;
	const static UINT baud = 9600;
	bool portState;
	CodeType codeType;
};

