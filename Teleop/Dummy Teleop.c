#pragma config(Hubs,  S1, HTMotor,  HTMotor,  none,     none)
#pragma config(Sensor, S2,     Front,          sensorSONAR)
#pragma config(Sensor, S3,     Back,           sensorSONAR)
#pragma config(Motor,  mtr_S1_C1_1,     Right1,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     Left1,         tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_1,     Right2,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     Left2,         tmotorTetrix, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"

int mSpeed = 50;
int threshold = 10;
int frontUltra;
int backUltra;

void Left(int speed){
	motor[Left1] = speed;
	motor[Left2] = speed;
}

void Right(int speed){
	motor[Right1] = speed;
	motor[Right2] = speed;
}


task main()
{
eraseDisplay();
while(true){
	getJoystickSettings(joystick);

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

	frontUltra = SensorValue(Front);
	backUltra = SensorValue(Back);
	nxtDisplayCenteredTextLine(1, "%i", frontUltra);
	nxtDisplayCenteredTextLine(2, "%i", backUltra);

	wait1Msec(50);
}
}
