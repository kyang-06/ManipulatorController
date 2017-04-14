#include "StdAfx.h"
#include "LeapDevice.h"

#pragma region LeapDevice

double MyListener::AMEND_SPEED = 1;

LeapDevice::LeapDevice(void)
{
}

LeapDevice::LeapDevice(ArmInfo *arm)
{
	mListener.setArm(arm);
}

LeapDevice::~LeapDevice(void)
{
}

void LeapDevice::start()
{
	mController.addListener(mListener);
}


void LeapDevice::stop()
{
	mController.removeListener(mListener);
}

#pragma endregion

#pragma region MyListener

MyListener::MyListener()
{
	id = 0;
}

void MyListener::setArm(ArmInfo* arm)
{
	mArm = arm;
}

void MyListener::onConnect(const Controller& controller)
{
	cout<<"Leap motion connected"<<endl;
}

Vector MyListener::jointPosition(const Frame& frame)
{
	HandList hands = frame.hands();
	for (HandList::const_iterator h = hands.begin(); h != hands.end(); h++)
	{
		const Hand hand = *h;
		FingerList fingers = hand.fingers();
		for (FingerList::const_iterator f = fingers.begin(); f != fingers.end(); f++)
		{
			const Finger finger = *f;
			if (finger.type() == Finger::Type::TYPE_MIDDLE)
			{
				Vector pos = (*f).bone(Bone::Type::TYPE_METACARPAL).nextJoint() / 10; // 中指掌指关节
				pos.z -= 14;
				return pos;
				break;
			}
		}
	}
	return Vector::zero();
}


void MyListener::onFrame(const Controller& controller)
{
	const Frame frame = controller.frame();
	if (frame.hands().count() == 0) return;

	Vector normal, pos;
	double roll;
	double grabStrength;
	double speed;
	const Hand hand = *(frame.hands().begin());

	//pos = jointPosition(frame); // 抓取器采用关节位置
	pos = hand.palmPosition() / 10; // 抓取器采用掌心位置
	if (pos == Vector::zero()) return;

	normal = hand.palmNormal();
	roll = normal.roll() + PI / 2;
	grabStrength = hand.grabStrength();
	speed = (hand.palmVelocity()/10).magnitude() * AMEND_SPEED;
	if (speed > 45) return;

#ifdef OUTPUT
	printf("Start\n");
	printf("ID: %04lld\n", id++);
	printf("Hand palm position: %.2f, %.2f, %.2f\n", pos.x, pos.y, pos.z);
	printf("Roll: %.4lf\n", roll);
	printf("Palm direction: %03d\n", (int) (roll / PI * 180));
	printf("Grab strength: %.2f\n", grabStrength);
	printf("Speed: %.2f\n", speed);
#endif

#ifdef RECORD
	recordData(pos);
#endif

	/********test*********/
	//mArm->updateEndPoint(Vector(-13.767, 14.508, -6.99));
	/*********************/

	//bool isDisplaced = mArm->updateEndPoint(Vector(x, y, z)) + mArm->updateGrabber(roll, grabStrength);
	mArm->updateSpeed(speed * AMEND_SPEED);
	bool isDisplaced = mArm->updateEndPoint(pos) + mArm->updateGrabber(roll, grabStrength);
	
#ifdef OUTPUT
	if (!isDisplaced) 
		printf("No displacement\n");
#endif


	Sleep(10);
}

#pragma endregion
