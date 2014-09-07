#pragma config(Hubs,  S1, HTMotor,  none,     none,     none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     Right,         tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     Left,          tmotorTetrix, openLoop, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"

int RDirection;
int LDirection;
int threshold = 10;
int maxVolt = 1200;

int mapValue(int value, int a1, int a2, int b1, int b2){
	if(a2 > a1 && b2 > b1){
		return (value - a1)*((b2 - b1)/(a2 - a1)) + b1;
	}else{
		return value;
	}
}

//This code maps the joystick to dynamically changing values of 0-12 volts output with a 10 degree threshold
task main()
{
while(true){
	getJoystickSettings(joystick);

	LDirection = abs(joystick.joy1_y1)/joystick.joy1_y1;
	RDirection = abs(joystick.joy1_y2)/joystick.joy1_y2;

	motor[Left] = abs(joystick.joy1_y1) > threshold ? mapValue(abs(joystick.joy1_y1), 10, 128, 0, (maxVolt / (nAvgBatteryLevel/100))) * LDirection : 0;
	motor[Right] = abs(joystick.joy1_y2) > threshold ? mapValue(abs(joystick.joy1_y2), 10, 128, 0, (maxVolt / (nAvgBatteryLevel/100))) * RDirection : 0;

	wait1Msec(50);
}
}
