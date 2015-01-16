#pragma config(Hubs,  S1, HTServo,  HTMotor,  HTMotor,  HTMotor)
#pragma config(Sensor, S2,     SeekerR,        sensorHiTechnicIRSeeker1200)
#pragma config(Sensor, S3,     HTSMUX,         sensorI2CCustom)
#pragma config(Sensor, S4,     SeekerL,        sensorHiTechnicIRSeeker1200)
#pragma config(Motor,  motorA,          Finger,        tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     Left1,         tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_2,     Elevator,      tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C3_1,     Right1,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     Right2,        tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C4_1,     Launch1,       tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C4_2,     Launch2,       tmotorTetrix, openLoop, reversed)
#pragma config(Servo,  srvo_S1_C1_1,    IR1,                  tServoStandard)
#pragma config(Servo,  srvo_S1_C1_2,    IR2,                  tServoStandard)
#pragma config(Servo,  srvo_S1_C1_3,    Deploy,               tServoStandard)
#pragma config(Servo,  srvo_S1_C1_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"
#include "drivers/hitechnic-sensormux.h"
#include "drivers/hitechnic-gyro.h"
#include "drivers/lego-ultrasound.h"

/*
Teleop
------
Code written by 6699 Tempest member, Aaron Cook

For usage: please refer to A_LG.c

Summary
-------
Button Mapping, joy_1:
-Left/Right y-axes -> left/right drive wheels
-B7+B8 -> begin triangulation and launch sequence
	-B7 after B7+B8 -> begin launching balls

Button Mapping, joy_2:
-B6 -> collect balls
-B5 -> drop collected balls
-B4 -> raise finger
-B2 -> lower finger
*/

#define Gyro			msensor_S3_3
#define Ultra1		msensor_S3_4

#include "JoystickDriver.c"

int mSpeed = 15;
int lSpeed = 100;
int threshold = 10;

void Left(int speed){
	motor[Left1] = speed;
}

void Right(int speed){
	motor[Right1] = speed;
	motor[Right2] = speed;
}



void triangulate(int btn1, int btn2){
servo[IR1] = 30;
servo[IR2] = 210;
bool Triangulating = true;

ClearTimer(T1);

while(Triangulating && joy1Btn(btn1) && joy1Btn(btn2)){
	nxtDisplayCenteredTextLine(1, "%i", SensorValue(SeekerL));
	nxtDisplayCenteredTextLine(2, "%i", SensorValue(SeekerR));

	if(time1(T1) > 10000) break;

	if(SensorValue(SeekerL) == 0 || SensorValue(SeekerR) == 0){
		Right(mSpeed);
		Left(mSpeed);
	}

	if(SensorValue(SeekerL) < 6 && SensorValue(SeekerL) != 0){
	if(SensorValue(SeekerR) < 4 && SensorValue(SeekerR) != 0){
		//left
		Right(mSpeed);
		Left(mSpeed);
	}

	if(SensorValue(SeekerR) > 4){
		//forward
		Right(mSpeed);
		Left(mSpeed);
	}

	if(SensorValue(SeekerR) == 4){
		//left
		Right(mSpeed);
		Left(-mSpeed);
	}
	}



	if(SensorValue(SeekerL) > 6){
	if(SensorValue(SeekerR) < 4 && SensorValue(SeekerR) != 0){
		//back
		Right(-mSpeed);
		Left(-mSpeed);
	}

	if(SensorValue(SeekerR) > 4){
		//right
		Right(-mSpeed);
		Left(mSpeed);
	}

	if(SensorValue(SeekerR) == 4){
		//right
		Right(-mSpeed);
		Left(mSpeed);
	}
	}



	if(SensorValue(SeekerL) == 6){
	if(SensorValue(SeekerR) < 4 && SensorValue(SeekerR) != 0){
		//left
		Right(-mSpeed);
		Left(mSpeed);
	}

	if(SensorValue(SeekerR) > 4){
		//right
		Right(-mSpeed);
		Left(mSpeed);
	}

	if(SensorValue(SeekerR) == 4){
		//stop, break
		Right(0);
		Left(0);
		Triangulating = false;
		break;
	}
	}
}
Right(0);
Left(0);
}



void straight(bool dir, int distance){
	int factor;

	if(dir){
		factor = 1;
	}else{
		factor = -1;
	}

	nMotorEncoder(Right2) = 0;
	wait10Msec(50);

	Right(factor*mSpeed);
	Left(factor*mSpeed);

	while(abs(nMotorEncoder(Right2)) < distance){
		//goStraight(dir);
		//checkObstacle(dir);
		nxtDisplayCenteredTextLine(1, "%i", nMotorEncoder(Right2));
	}

	Right(0);
	Left(0);
}



void CheckDrive(){
	//Drive
	if(abs(joystick.joy1_y1) > threshold){
		Left(mSpeed*(abs(joystick.joy1_y1)/joystick.joy1_y1));
	}else{
		Left(0);
	}

	if(abs(joystick.joy1_y2) > threshold){
		Right(mSpeed*(abs(joystick.joy1_y2)/joystick.joy1_y2));
	}else{
		Right(0);
	}

	//Elevator
	if(joy2Btn(6) && !joy2Btn(5)){
		motor[Elevator] = 50;
	}
	else if(joy2Btn(5) && !joy2Btn(6)){
		motor[Elevator] = -50;
	}
	else{
		motor[Elevator] = 0;
	}

	//Finger
	if(joy2Btn(4) && !joy2Btn(2)){
		motor[Finger] = -50;
	}
	else if(joy2Btn(2) && !joy2Btn(4)){
		motor[Finger] = 50;
	}
	else{
		motor[Finger] = 0;
	}
}


void LaunchSequence(int btn1, int btn2){
	//Launcher
	motor[Launch1] = 0;
	motor[Launch2] = 0;

	triangulate(btn1, btn2);
	/*while(USreadDist(Ultra1) >= 10){
		Right(mSpeed/2);
		Left(mSpeed/2);
	}
	straight(false, 1440*0.7);*/
	//triangulate(btn1, btn2);

	while(joy1Btn(btn1) && joy1Btn(btn2)){
		motor[Launch1] = lSpeed;
		motor[Launch2] = lSpeed;
	}

	if(joy1Btn(btn1) && !joy1Btn(btn2)){
		ClearTimer(T4);
		while(joy1Btn(btn1)){
			CheckDrive();

			//if(SensorValue(SeekerL) != 6 || SensorValue(SeekerR) != 4) triangulate(btn1, btn1);

			if(time1(T4) < 150){
				servo[Deploy] = 0;
			}
			else if(time1(T4) >= 150 && time1(T4) < 1000){
				servo[Deploy] = 50;
			}
			else{
				ClearTimer(T4);
			}
		}
		motor[Launch1] = 0;
		motor[Launch2] = 0;
	}
	motor[Launch1] = 0;
	motor[Launch2] = 0;
}



task main()
{
waitForStart();
while(true){
	getJoystickSettings(joystick);

	servo[IR1] = 30;
	servo[IR2] = 210;
	servo[Deploy] = 50;

	if(joy1Btn(7) && joy1Btn(8)){
		LaunchSequence(7, 8);
	}
	else{
		CheckDrive();
	}

	wait1Msec(50);
}
}
