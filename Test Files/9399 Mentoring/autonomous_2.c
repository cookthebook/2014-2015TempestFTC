#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  HTMotor)
#pragma config(Sensor, S2,     ir,             sensorHiTechnicIRSeeker1200)
#pragma config(Motor,  mtr_S1_C1_1,     Shooter,       tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     Left,          tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C3_1,     Right,         tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C3_2,     Grabber,       tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_1,     motorH,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     motorI,        tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C2_1,    Sweeper,              tServoContinuousRotation)
#pragma config(Servo,  srvo_S1_C2_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"

int speed = 50;

void forward(int distance){
	nMotorEncoder(Left)= 0;
	nMotorEncoder(Right) = 0;

	motor[Left] = speed;
	motor[Right] = speed;

	while(abs(nMotorEncoder(Left)) < distance){
	}

	motor[Left] = 0;
	motor[Right] = 0;
}



void backward(int distance){
	nMotorEncoder(Left)= 0;
	nMotorEncoder(Right) = 0;

	motor[Left] = -speed;
	motor[Right] = -speed;

	while (abs(nMotorEncoder(Left)) < distance){
	}

	motor[Left] = 0;
	motor[Right] = 0;
}



void left(int distance){
	nMotorEncoder(Left)= 0;
	nMotorEncoder(Right) = 0;

	motor[Left] = -speed;
	motor[Right] = speed;

	while (abs(nMotorEncoder(Left)) < distance){
	}

	motor[Left] = 0;
	motor[Right] = 0;
}



void right(int distance){
	nMotorEncoder(Left)= 0;
	nMotorEncoder(Right) = 0;

	motor[Left] = speed;
	motor[Right] = -speed;

	while (abs(nMotorEncoder(Left)) < distance){
	}

	motor[Left] = 0;
	motor[Right] = 0;
}

task main()
{
while(true)
{
	//forward(1440*3);
	if (SensorValue(ir) == 5) {
		forward(1400);

		right(1400);

		forward(1400);

		left(1400);

		forward(1400);
	}



	else if (SensorValue(ir) != 5) {
		forward (1400);

		left (1400);

		forward (1400);

		right (1400);
		//you will be facing the same direction you were before, how did you plan on this working?
		if (SensorValue(ir) == 5) {
			right (1400);

			forward (1400);

			left (1400);

			forward (1400);
		}

		else if (SensorValue(ir) !=5){
			right (1400);

			forward (1400);
		}
	}
}
}
