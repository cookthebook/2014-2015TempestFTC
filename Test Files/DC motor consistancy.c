#pragma config(Hubs,  S1, HTMotor,  none,     none,     none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     Left,          tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     motorE,        tmotorTetrix, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"

int motorSpeed = 35;

float target = 124.2;
float minimum = 120.3;
float maximum = 130.5;
float CurrentRPM;

bool tooSlow;
bool tooFast;
bool justRight;

int fastCount = 0;
int slowCount = 0;
int rightCount = 0;

long slowTotal = 0;
long fastTotal = 0;
long rightTotal = 0;

float slowAvg;
float fastAvg;
float rightAvg;

float RPM(tMotor input){
	int position1;
	int position2;
	float rpm;

	wait1Msec(100);
	position1 = nMotorEncoder(input);
	wait1Msec(100);
	position2 = nMotorEncoder(input);

	rpm = abs(((float)position2 - (float)position1) * 600 / 1440);//converting from ticks/decisecond to rotations/second

	return rpm;
}

task main()
{
nMotorEncoder(Left) = 0;
eraseDisplay();


	motor[Left] = 0;
	wait1Msec(100);
	motor[Left] = motorSpeed;
	wait1Msec(500);

	while(true){
		if(RPM(Left) < minimum){
			ClearTimer(T1);
			while(RPM(Left) < minimum){
				CurrentRPM = RPM(Left);
				tooSlow = true;
				motorSpeed++;
				motor[Left] = motorSpeed;
				wait1Msec(50);//Get to speed;
			}
			tooSlow = false;
			slowTotal += time1(T1);
			slowCount++;
		}

		if(RPM(Left) > maximum){
			ClearTimer(T1);
			while(RPM(Left) > maximum){
				CurrentRPM = RPM(Left);
				tooFast = true;
				motorSpeed--;
				motor[Left] = motorSpeed;
				wait1Msec(50);
			}
			tooFast = false;
			fastTotal += time1(T1);
			fastCount++;
		}

		if(RPM(Left) >= minimum || RPM(Left) <= maximum){
			ClearTimer(T1);
			while(RPM(Left) >= minimum && RPM(Left) <= maximum){
				CurrentRPM = RPM(Left);
				justRight = true;
				nxtDisplayCenteredTextLine(3, "RPM:%f", RPM(Left));
			}
			justRight = false;
			rightTotal += time1(T1);
			rightCount++;
		}

		slowAvg = slowTotal/slowCount;
		fastAvg = fastTotal/fastCount;
		rightAvg = rightTotal/rightCount;
	}
}
