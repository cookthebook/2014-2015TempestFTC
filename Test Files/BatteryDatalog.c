#pragma config(Hubs,  S1, HTMotor,  none,     none,     none)
#pragma config(Motor,  mtr_S1_C1_1,     Left,          tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     Right,         tmotorTetrix, openLoop, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#pragma platform(NXT)

#include "JoystickDriver.c"

int startVolts;
int speed = 75;
int encSpeed;
int newSpeed;
int time;

int startVoltsDat = 0;
int newVoltsDat = 1;
int encSpeedDat = 2;
int newSpeedDat = 3;
int timeDat = 4;
int speedDat = 5;

//This code measures how quickly a battery will drain (please start as fully charged)//
task main()
{
	waitForStart();
	nMotorEncoder(Right) = 0;
	nMotorEncoder(Left) = 0;
	wait1Msec(50);

	ClearTimer(T2);

	motor[Right] = speed;
	motor[Left] = -speed;
	AddToDatalog(speedDat, speed);
	wait1Msec(8000);
	//Get up to speed//
	ClearTimer(T1);
	startVolts = nAvgBatteryLevel;
	while(time1(T1) < 10000){
	}
	//Starting encoder ticks per second//
	encSpeed = ((nMotorEncoder(Right) / 10) + (abs(nMotorEncoder(Left)) / 10)) / 2;
	AddToDatalog(encSpeedDat, encSpeed);
	AddToDatalog(startVoltsDat, startVolts);

	newSpeed = encSpeed;

	//Run while at 80% of original speed or greater//
	while(newSpeed >= (encSpeed * 80 / 100)){
		ClearTimer(T1);

		nMotorEncoder(Right) = 0;
		nMotorEncoder(Left) = 0;
		wait1Msec(50);

		while(time1(T1) < 10000){
		}
		//Current encoder ticks per second//
		newSpeed = ((nMotorEncoder(Right) / 10) + (abs(nMotorEncoder(Left)) / 10)) / 2;
		AddToDatalog(newSpeedDat, newSpeed);
		AddToDatalog(newVoltsDat, nAvgBatteryLevel);
	}

  time = time100(T2) / 10;
	AddToDatalog(timeDat, time);
	SaveNxtDatalog();
}
