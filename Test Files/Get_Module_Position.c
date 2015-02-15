#pragma config(Hubs,  S1, HTServo,  HTMotor,  HTMotor,  HTMotor)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     SeekerL,        sensorHiTechnicIRSeeker1200)
#pragma config(Sensor, S3,     HTSMUX,         sensorI2CCustom)
#pragma config(Sensor, S4,     SeekerR,        sensorHiTechnicIRSeeker1200)
#pragma config(Motor,  motorA,          Finger,        tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     Left1,         tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_2,     Elevator,      tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C3_1,     Right1,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     Right2,        tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C4_1,     Launch1,       tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     Launch2,       tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C1_1,    Deploy,               tServoStandard)
#pragma config(Servo,  srvo_S1_C1_2,    IR1,                  tServoStandard)
#pragma config(Servo,  srvo_S1_C1_3,    IR2,                  tServoStandard)
#pragma config(Servo,  srvo_S1_C1_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"
#include "drivers/hitechnic-sensormux.h"
#include "drivers/hitechnic-gyro.h"
#include "drivers/lego-ultrasound.h"

#define Gyro			msensor_S3_3
#define Ultra1		msensor_S3_4

int modPos;

task main(){
eraseDisplay();
int modCount = 0;
modPos = 0;
while(true){
	modCount = 0;
	for(int i=0; i <= 20; i++){
		if(abs(USreadDist(Ultra1) - 105) < 10){
			modPos = 1;
			break;
		}

		if(abs(USreadDist(Ultra1) - 125) < 10){
			modCount++;
		}
	}
	if(modPos != 1){
		if(modCount >= 15){
			modPos = 3;
		}
		else if(modCount < 5){
			modPos = 2;
		}
	}

	nxtDisplayCenteredTextLine(1, "%i", modPos);
}
}
