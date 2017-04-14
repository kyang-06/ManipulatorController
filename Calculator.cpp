#include "StdAfx.h"
#include "Calculator.h"


double Calculator::y1 = (ArmInfo::L)[1];
double Calculator::l1 = (ArmInfo::L)[1];
double Calculator::l2 = (ArmInfo::L)[2];
double Calculator::l3 = (ArmInfo::L)[3];
double Calculator::l4 = (ArmInfo::L)[4];

Calculator::Calculator(void)
{
}



Calculator::~Calculator(void)
{
}

int Calculator::round(double x)
{
	return (int) (x+0.5);
}

int* Calculator::RadToAngle()
{
	int angle[6];
	for (int i=1; i<=thetaCount; i++)
	{
		int ang = round(theta[i] / PI * 180) + 360;
		ang %= 360;
		angle[i] = ang;
	}
	return angle;
}

void Calculator::init(Vector v)
{
	mPoint = v;
	x4 = sqrt(sqr(mPoint.x) + sqr(mPoint.z));
	y4 = mPoint.y;
	distance = sqrt(sqr(y4 - y1) + sqr(x4));
}

bool Calculator::calculate(Vector v)
{
	init(v);
	if (distance > l2 + l3 + l4) return false;
	bool step1 = slipSearch();
	if (step1)
	{
		computeTheta(1);
		if (validate())
			return true;
	}
	bool step2 = rotateSearch();
	if (step2)
	{
		computeTheta(2);
		if (validate())
			return true;
		else
			return false;
	}
	return false;
}

bool Calculator::validate()
{
	for (int i=1; i<=thetaCount; i++)
	{
		if (_isnan(theta[i]) || (theta[i] > -PI && theta[i] < -1e-1) || theta[i] > PI)
			return false;
	}
	return true;
}

bool Calculator::slipSearch() // step1
{
	double maxb, minb, midb;
	k = (y4 - y1) / x4;
	maxb = sqrt(sqr(y4/x4*l2) + sqr(l2)) + l1;
	if (distance > l2 + l4)	// 两圆相离
		minb = l1; 
	else					// 两圆相交
		minb = -sqrt(x4*((sqr(l2) + 2*l2*l4 + sqr(l4) - sqr(x4) - sqr(y1) + 2*y1*y4 - sqr(y4))*(- sqr(l2) + 2*l2*l4 - sqr(l4) + sqr(x4) + sqr(y1) - 2*y1*y4 + sqr(y4)))
		+ k*pow(x4,(int)3) + sqr(l2)*y1 - sqr(l4)*y1 - sqr(l2)*y4 + sqr(l4)*y4 - sqr(x4)*y1 - sqr(x4)*y4 + y1*sqr(y4) + sqr(y1)*y4 - pow(y1,(int)3) - pow(y4,(int)3) + k*sqr(l2)*x4 - k*sqr(l4)*x4 
		+ k*x4*sqr(y1) + k*x4*sqr(y4) - k*y1*sqrt((sqr(l2) + 2*l2*l4 + sqr(l4) - sqr(x4) - sqr(y1) + 2*y1*y4 - sqr(y4))*(- sqr(l2) + 2*l2*l4 - sqr(l4) + sqr(x4) + sqr(y1) - 2*y1*y4 + sqr(y4)))
		+ k*y4*sqrt((sqr(l2) + 2*l2*l4 + sqr(l4) - sqr(x4) - sqr(y1) + 2*y1*y4 - sqr(y4))*(- sqr(l2) + 2*l2*l4 - sqr(l4) + sqr(x4) + sqr(y1) - 2*y1*y4 + sqr(y4))) - 2*k*x4*y1*y4)/(2*sqr(x4) + 2*sqr(y1) - 4*y1*y4 + 2*sqr(y4));
	midb = (maxb + minb) / 2;
	double result = 0;
	for (int i=1; i<=iteTime; i++) // 二分6次
	{
		result = computeB(midb);
		if (result == 0)
			break;
		if (result == 1)	
			maxb = midb;
		else
			minb = midb;
		midb = (maxb + minb) / 2;
	}
	if (computeB(maxb) * result < 0 || computeB(minb) * result < 0)
	{
		b = midb;
		return true;
	}
	else
		return false;
}

bool Calculator::rotateSearch() // step2
{
	double maxk, mink, midk;
	b = 0;
	maxk = tan(asin((l4-l2)/distance)+atan((y4 - y1)/x4));
	mink = (y4 - y1) / x4;
	midk = (maxk + mink) / 2;
	double result = 0;
	for (int i=1; i<=iteTime; i++)
	{
		result = computeK(midk);
		if (result == 0)
			break;
		if (result == 1)
			maxk = midk;
		else
			mink = midk;
		midk = (maxk + mink) / 2;
	}
	if (computeK(maxk) * result < 0 || computeK(mink) * result < 0)
	{
		k = midk;
		return true;
	}
	else 
		return false;
}

int Calculator::computeB(double bx)
{
	double result;
	derta1 = -sqr(bx) + 2*bx*y1 + sqr(k)*sqr(l2) + sqr(l2) - sqr(y1);
	derta2 = -sqr(bx) - 2*bx*k*x4 + 2*bx*y4 + sqr(k)*sqr(l4) - sqr(k)*sqr(x4) + 2*k*x4*y4 + sqr(l4) - sqr(y4);
	if (derta1<0 || derta2<0) return 0;
	x2 = (sqrt(derta1) - bx*k + k*y1) / (sqr(k) + 1);
	y2 = k*x2 + bx;
	x3 = (x4 - bx*k + k*y4 - sqrt(derta2)) / (sqr(k) + 1);
	y3 = k*x3 + bx;
	result = sqrt(sqr(x3 - x2) + sqr(y3 - y2)) - l3;
	if (result > 0) return 1;
	else return -1;
}

int Calculator::computeK(double kx)
{
	double result;
	b = l2*sqrt(sqr(kx)+1)+y1;
	x2 = -l2*kx/sqrt(sqr(kx) + 1);
	y2 = l2/sqrt(sqr(kx) + 1) + y1;
	//double x31 =  (x4 - b*kx + kx*y4 + sqrt(- sqr(b) - 2*b*kx*x4 + 2*b*y4 + sqr(kx)*sqr(l4) - sqr(kx)*sqr(x4) + 2*kx*x4*y4 + sqr(l4) - sqr(y4)))/(sqr(k) + 1),
	/*if (kx > 0) x3 = std::min(x31, x32);
	else x3 = std::max(x31, x32);*/
	x3 = (x4 - b*kx + kx*y4 - sqrt(- sqr(b) - 2*b*kx*x4 + 2*b*y4 + sqr(kx)*sqr(l4) - sqr(kx)*sqr(x4) + 2*kx*x4*y4 + sqr(l4) - sqr(y4)))/(sqr(kx) + 1);
	/*x3 = -(y1 - (y1 - (sqr(kx) + 1)*sqrt(-(sqr(kx)*(sqr(kx)*sqr(x4) - 2*y1*y4 + sqr(l2) - sqr(l4) + sqr(y1) + sqr(y4) + sqr(kx)*sqr(l2) - sqr(kx)*sqr(l4) + 2*kx*x4*y1 - 2*kx*x4*y4 + 2*l2*y1*sqrt(sqr(kx) + 1) - 2*l2*y4*sqrt(sqr(kx) + 1) + 2*kx*l2*x4*sqrt(sqr(kx) + 1)))
		/sqr(sqr(kx) + 1)) + kx*x4 + l2*sqrt(sqr(kx) + 1) + sqr(kx)*y4)/(sqr(kx) + 1) + l2*sqrt(sqr(kx) + 1))/kx;
	x3 = -(y1 - (y1 - (sqr(kx) + 1)*sqrt(-(sqr(kx)*(sqr(kx)*sqr(x4) - 2*y1*y4 + sqr(l2) - sqr(l4) + sqr(y1) + sqr(y4) + sqr(kx)*sqr(l2) - sqr(kx)*sqr(l4) + 2*kx*x4*y1 - 2*kx*x4*y4 + 2*l2*y1*sqrt(sqr(kx) + 1) - 2*l2*y4*sqrt(sqr(kx) + 1) + 2*kx*l2*x4*sqrt(sqr(kx) + 1)))/sqr(sqr(kx) + 1)) + kx*x4 + l2*sqrt(sqr(kx) + 1) + sqr(kx)*y4)/(sqr(kx) + 1) + l2*sqrt(sqr(kx) + 1))/kx;*/

	y3 = y1 + kx*x3 + l2*sqrt(sqr(kx) + 1);
	result = sqrt(sqr(x3 - x2) + sqr(y3 - y2)) - l3;
	if (result > 0) return 1;
	else return -1;
}

void Calculator::computeTheta(int step)
{
	if (step == 1)
	{
		double derta1, derta2;
		derta1 = -sqr(b) + 2*b*y1 + sqr(k)*sqr(l2) + sqr(l2) - sqr(y1);
		derta2 = -sqr(b) - 2*b*k*x4 + 2*b*y4 + sqr(k)*sqr(l4) - sqr(k)*sqr(x4) + 2*k*x4*y4 + sqr(l4) - sqr(y4);
		x2=(sqrt(derta1) - b*k + k*y1) / (sqr(k) + 1); 
		y2=k*x2+b;
		x3=(x4 - b*k + k*y4 - sqrt(derta2)) / (sqr(k) + 1); 
		y3=k*x3+b;
	}
	else
	{
		x2 = -l2*k/sqrt(sqr(k) + 1);
		y2 = l2/sqrt(sqr(k) + 1) + y1;
		/*x3 = -(y1 - (y1 - (sqr(k) + 1)
		*sqrt(-(sqr(k)*(sqr(k)*sqr(x4) - 2*y1*y4 + sqr(l2) - sqr(l4) + sqr(y1) + sqr(y4) + sqr(k)*sqr(l2) - sqr(k)*sqr(l4) + 2*k*x4*y1 - 2*k*x4*y4 + 2*l2*y1*sqrt(sqr(k) + 1) - 2*l2*y4*sqrt(sqr(k) + 1) + 2*k*l2*x4*sqrt(sqr(k) + 1)))
		/sqr(sqr(k) + 1)) + k*x4 + l2*sqrt(sqr(k) + 1) + sqr(k)*y4)/(sqr(k) + 1) + l2*sqrt(sqr(k) + 1))/k;*/
		x3 = (x4 - b*k + k*y4 - sqrt(- sqr(b) - 2*b*k*x4 + 2*b*y4 + sqr(k)*sqr(l4) - sqr(k)*sqr(x4) + 2*k*x4*y4 + sqr(l4) - sqr(y4)))/(sqr(k) + 1);
		y3 = y1 + k*x3 + l2*sqrt(sqr(k) + 1);
	}

	theta[1] = atan2( -mPoint.z, mPoint.x); // z must be negative
	theta[2] = atan2(y2-y1, x2);
	theta[3] = atan2(y3-y2, x3-x2) + PI/2 - theta[2];
	theta[4] = atan2(y4-y3, x4-x3) + PI/2 - atan2(y3-y2, x3-x2);
	//printf("%.2f\t%.2f\t%.2f\t%.2f\n",theta[1]*180/PI,theta[2]*180/PI,theta[3]*180/PI,theta[4]*180/PI);
}

double* Calculator::getTheta()
{
	return theta;
}
