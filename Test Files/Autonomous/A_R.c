#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  HTServo)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S3,     HTSMUX,         sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     Right1,        tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     Right2,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     Left1,         tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_2,     Left2,         tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C3_1,     Launch1,       tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C3_2,     Launch2,       tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C4_1,    Deploy,               tServoStandard)
#pragma config(Servo,  srvo_S1_C4_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C4_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"
#include "drivers/hitechnic-sensormux.h"
#include "drivers/hitechnic-irseeker-v2.h"
#include "drivers/hitechnic-gyro.h"

#define SeekerL 	msensor_S3_1
#define SeekerR 	msensor_S3_2
#define Gyro			msensor_S3_3

float trans = 0;
int mSpeed = 50;
int lSpeed = 30;
float minimum = 120.3;
float maximum = 130.5;
float tooSlow = 0.2;

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



void Right(int speed){
	motor[Right1] = speed;
	motor[Right2] = speed;
}

void Left(int speed){
	motor[Left1] = speed;
	motor[Left2] = speed;
}

void goStraight(const int speed, const int angle){
	if(HTGYROreadCal(Gyro) < angle-5){
			Right(speed + 1);
			Left(speed - 1);
		}
		else if(HTGYROreadCal(Gyro) > angle+5){
			Right(speed - 1);
			Left(speed + 1);
		}
		else{
			Right(speed);
			Left(speed);
		}

		//Obstacle
		if(RPM(Right1) < tooSlow && time1(T2) > 2000){
			Right(0);
			Left(0);
			wait1Msec(2000);
			Right(speed);
			Left(speed);
			ClearTimer(T2);
		}
		if(RPM(Left1) < tooSlow && time1(T2) > 2000){
			Right(0);
			Left(0);
			wait1Msec(2000);
			Right(speed);
			Left(speed);
			ClearTimer(T2);
		}
}



void offRamp(const int distance1, const int turn1, const int distance2){
	nMotorEncoder(Right1) = 0;
	nMotorEncoder(Left1) = 0;

	Right(mSpeed);
	Left(mSpeed);

	while(abs(nMotorEncoder(Right1)) < distance1 && abs(nMotorEncoder(Left1)) < distance1){
		goStraight(mSpeed, 0);
	}

	Right(0);
	Left(0);

	nMotorEncoder(Right1) = 0;
	nMotorEncoder(Left1) = 0;

	Right(mSpeed / 2);
	Left(-mSpeed / 2);

	while(abs(nMotorEncoder(Right1)) < turn1 && abs(nMotorEncoder(Left1)) < turn1 && HTGYROreadCal(Gyro) < 90){
	}

	Right(0);
	Left(0);

	nMotorEncoder(Right1) = 0;
	nMotorEncoder(Left1) = 0;

	Right(mSpeed);
	Left(mSpeed);

	while(abs(nMotorEncoder(Right1)) < distance2 && abs(nMotorEncoder(Left1)) < distance2){
		goStraight(mSpeed, 90);
	}

	Right(0);
	Left(0);

	trans = 1;
}



void findPos(){
	if(HTIRS2readDCDir(SeekerR) == 5 && HTIRS2readDCDir(SeekerL) == 5) trans = 2.1;

	if(HTIRS2readDCDir(SeekerR) > 5 && HTIRS2readDCDir(SeekerL) > 5) trans = 2.2;

	if(HTIRS2readDCDir(SeekerR) == -1 && HTIRS2readDCDir(SeekerL) == -1) trans = 2.3;
}



//Center Module Position 1
void knockPole1(const int turn1, const int distance1, const int turn2, const int distance2, const int turn3, const int distance3, const int turn4){
	nMotorEncoder(Right1) = 0;
	nMotorEncoder(Left1) = 0;

	Right(mSpeed / 2);
	Left(-mSpeed / 2);

	while(abs(nMotorEncoder(Right1)) < turn1 && abs(nMotorEncoder(Left1)) < turn1 && HTGYROreadCal(Gyro) < 180){
	}

	Right(0);
	Left(0);

	nMotorEncoder(Right1) = 0;
	nMotorEncoder(Left1) = 0;

	Right(mSpeed);
	Left(mSpeed);

	while(abs(nMotorEncoder(Right1)) < distance1 && abs(nMotorEncoder(Left1)) < distance1){
		goStraight(mSpeed, 180);
	}

	Right(0);
	Left(0);

	nMotorEncoder(Right1) = 0;
	nMotorEncoder(Left1) = 0;

	Right(-mSpeed / 2);
	Left(mSpeed / 2);

	while(abs(nMotorEncoder(Right1)) < turn2 && abs(nMotorEncoder(Left1)) < turn2 && HTGYROreadCal(Gyro) > 90){
	}

	Right(0);
	Left(0);

	nMotorEncoder(Right1) = 0;
	nMotorEncoder(Left1) = 0;

	Right(mSpeed);
	Left(mSpeed);

	while(abs(nMotorEncoder(Right1)) < distance2 && abs(nMotorEncoder(Left1)) < distance2){
		goStraight(mSpeed, 90);
	}

	Right(0);
	Left(0);

	nMotorEncoder(Right1) = 0;
	nMotorEncoder(Left1) = 0;

	Right(-mSpeed / 2);
	Left(mSpeed / 2);

	while(abs(nMotorEncoder(Right1)) < turn3 && abs(nMotorEncoder(Left1)) < turn3 && HTGYROreadCal(Gyro) > 0){
	}

	Right(0);
	Left(0);

	nMotorEncoder(Right1) = 0;
	nMotorEncoder(Left1) = 0;

	Right(mSpeed);
	Left(mSpeed);

	while(abs(nMotorEncoder(Right1)) < distance3 && abs(nMotorEncoder(Left1)) < distance3){
		goStraight(mSpeed, 0);
	}

	Right(0);
	Left(0);

	nMotorEncoder(Right1) = 0;
	nMotorEncoder(Left1) = 0;

	Right(-mSpeed / 2);
	Left(mSpeed / 2);

	while(abs(nMotorEncoder(Right1)) < turn4 && abs(nMotorEncoder(Left1)) < turn4 && HTGYROreadCal(Gyro) > -90){
	}

	Right(0);
	Left(0);

	trans = 3;
}



task main()
{
waitForStart();
HTGYROstartCal(Gyro);

if(trans == 0) offRamp(3000, 2500, 2500);
if(trans == 1) findPos();
if(trans == 2.1) knockPole1(2500, 1000, 2500, 3000, 2500, 500, 2500);
}
