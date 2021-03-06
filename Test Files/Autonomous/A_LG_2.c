#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTMotor,  none)
#pragma config(Hubs,  S2, HTServo,  none,     none,     none)
#pragma config(Sensor, S3,     HTSMUX,         sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     Left1,         tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     Left2,         tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_1,     Right1,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     Right2,        tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C3_1,     Launch1,       tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C3_2,     Launch2,       tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S2_C1_1,    IR1,                  tServoStandard)
#pragma config(Servo,  srvo_S2_C1_2,    IR2,                  tServoStandard)
#pragma config(Servo,  srvo_S2_C1_3,    Deploy,               tServoStandard)
#pragma config(Servo,  srvo_S2_C1_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S2_C1_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S2_C1_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"
#include "drivers/hitechnic-sensormux.h"
#include "drivers/hitechnic-irseeker-v2.h"
#include "drivers/hitechnic-gyro.h"
#include "drivers/lego-ultrasound.h"

#define SeekerL 	msensor_S3_1
#define SeekerR 	msensor_S3_2
#define Gyro			msensor_S3_3
#define Ultra1		msensor_S3_4

int modPos;
int mSpeed = 30;
int lSpeed = 30;
float minimumRPM = 120.3;
float maximumRPM = 130.5;
float tooSlow = 0.2;
float tThreshold = 10.0;

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



void goStraight(bool dir){
	int factor;

	if(dir){
		factor = 1;
	}else{
		factor = -1;
	}

	if(HTGYROreadRot(Gyro) < -tThreshold){
			Right(mSpeed*factor - 5);
			Left(mSpeed*factor + 5);
		}
	else if(HTGYROreadRot(Gyro) > tThreshold){
		Right(mSpeed*factor + 5);
		Left(mSpeed*factor - 5);
	}
	else{
		Right(mSpeed*factor);
		Left(mSpeed*factor);
	}

		//Obstacle based off RPM
	/*if(RPM(Right1) < tooSlow && time1(T2) > 2000){
		Right((mSpeed/4)*(-factor));
		Left((mSpeed/4)*(-factor));
		wait1Msec(1000);
		Right(0);
		Left(0);
		wait1Msec(1000);
		Right(mSpeed*factor);
		Left(mSpeed*factor);
		ClearTimer(T2);
	}*/
}



void checkObstacle(bool dir){
	int factor;

	if(dir){
		factor = 1;
	}else{
		factor = -1;
	}

	if(USreadDist(Ultra1) <= 30 && USreadDist(Ultra1) >= 10){
		Right(mSpeed/2 * factor);
		Left(mSpeed/2 * factor);
	}
	else if(USreadDist(Ultra1) < 10){
		Right(0);
		Left(0);
	}
	else{
		Right(mSpeed * factor);
		Left(mSpeed * factor);
	}
}



void trackTurning(int angle, int distance){
	bool turning = true;
	float totalAngle = 0;
	ClearTimer(T2);

	while(turning){
		if(time1(T2) >= 250){
			totalAngle += HTGYROreadRot(Gyro) * .250;
			ClearTimer(T2);
		}

		if(totalAngle >= angle && abs(nMotorEncoder(Right2)) > distance) break;
	}
}



void triangulate(){
servo[IR1] = 30;
servo[IR2] = 210;
bool Triangulating = true;
while(Triangulating){
	nxtDisplayCenteredTextLine(1, "%i", HTIRS2readDCDir(SeekerL));
	nxtDisplayCenteredTextLine(2, "%i", HTIRS2readDCDir(SeekerR));

	if(HTIRS2readDCDir(SeekerL) == 0 || HTIRS2readDCDir(SeekerR) == 0){
		Right(mSpeed/2);
		Left(mSpeed/2);
	}

	if(HTIRS2readDCDir(SeekerL) < 6 && HTIRS2readDCDir(SeekerL) != 0){
	if(HTIRS2readDCDir(SeekerR) < 4 && HTIRS2readDCDir(SeekerR) != 0){
		//left
		Right(mSpeed);
		Left(-mSpeed);
	}

	if(HTIRS2readDCDir(SeekerR) > 4){
		//forward
		Right(mSpeed/2);
		Left(mSpeed/2);
	}

	if(HTIRS2readDCDir(SeekerR) == 4){
		//left
		Right(mSpeed);
		Left(-mSpeed);
	}
	}



	if(HTIRS2readDCDir(SeekerL) > 6){
	if(HTIRS2readDCDir(SeekerR) < 4 && HTIRS2readDCDir(SeekerR) != 0){
		//back
		Right(-mSpeed/2);
		Left(-mSpeed/2);
	}

	if(HTIRS2readDCDir(SeekerR) > 4){
		//right
		Right(-mSpeed);
		Left(mSpeed);
	}

	if(HTIRS2readDCDir(SeekerR) == 4){
		//right
		Right(-mSpeed);
		Left(mSpeed);
	}
	}



	if(HTIRS2readDCDir(SeekerL) == 6){
	if(HTIRS2readDCDir(SeekerR) < 4 && HTIRS2readDCDir(SeekerR) != 0){
		//left
		Right(-mSpeed);
		Left(mSpeed);
	}

	if(HTIRS2readDCDir(SeekerR) > 4){
		//right
		Right(-mSpeed);
		Left(mSpeed);
	}

	if(HTIRS2readDCDir(SeekerR) == 4){
		//stop, break
		Right(0);
		Left(0);
		Triangulating = false;
	}
	}
}
}



void straight(bool dir, int distance){
	int factor;

	if(dir){
		factor = 1;
	}else{
		factor = -1;
	}

	nMotorEncoder(Right2) = 0;

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



void left(int angle, int distance){
	nMotorEncoder(Right2) = 0;

	Right(mSpeed);
	Left(-mSpeed);

	trackTurning(angle, distance);

	Right(0);
	Left(0);
}



void right(int angle, int distance){
	nMotorEncoder(Right2) = 0;

	Right(-mSpeed);
	Left(mSpeed);

	trackTurning(angle, distance);

	Right(0);
	Left(0);
}



void launch(){
	bool Launched = false;

	while(!Launched)
		if(HTIRS2readDCDir(SeekerR) == 4 && HTIRS2readDCDir(SeekerL) == 6){
			motor[Launch1] = lSpeed;
			motor[Launch2] = lSpeed;
			wait1Msec(2000);
			ClearTimer(T2);
			do{
				if((RPM(Launch1) + RPM(Launch2))/2 < minimumRPM){
					lSpeed += 2;//The ball will add resistance, so incriment by 2
					motor[Launch1] = lSpeed;
					motor[Launch2] = lSpeed;
				}
				else if((RPM(Launch1) + RPM(Launch2))/2 > maximumRPM){
					lSpeed--;
					motor[Launch1] = lSpeed;
					motor[Launch2] = lSpeed;
				}
				else{
					//Deploy
					servo[Deploy] = 90;
					if(time1(T2) > 5000){
						Launched = true;
						servo[Deploy] = 180;
					}
				}
			}while(!Launched);
		}
		else{
			triangulate();
			straight(false, 1440);
		}
}



void findPos(){
	if(HTIRS2readDCDir(SeekerR) == 5 && HTIRS2readDCDir(SeekerL) == 5) modPos = 1;

	if(HTIRS2readDCDir(SeekerR) == 0 || HTIRS2readDCDir(SeekerL) == 0) modPos = 2;

	if(HTIRS2readDCDir(SeekerR) == 3 && HTIRS2readDCDir(SeekerL) == 3) modPos = 3;
}



task main(){
	HTGYROstartCal(Gyro);
	ClearTimer(T1);
	straight(true, 1440*4.5);
	servo[IR1] = 30;
	servo[IR2] = 215;
	wait1Msec(500);
	findPos();

	if(modPos == 1){
		PlaySound(soundBeepBeep);
		left(90, 1440);
		/*straight(true, 1440*5);
		right(0, 1440);
		straight(true, 1440/2);
		right(-90, 1440);
		launch();*/
	}
	else if(modPos == 2){
		PlaySound(soundBeepBeep);
		wait1Msec(500);
		PlaySound(soundBeepBeep);
		straight(false, 1440/2);
		left(-135, (1440*3.5) / 2);
		straight(true, 1440*5);
		right(45, 1440);
		straight(true, 1440/2);
		right(45, 1440);
		launch();
	}
	else if(modPos == 3){
		PlaySound(soundBeepBeep);
		wait1Msec(500);
		PlaySound(soundBeepBeep);
		wait1Msec(500);
		PlaySound(soundBeepBeep);
		wait1Msec(500);
		straight(true, 1440);
		/*straight(false, 1440*3);
		left(90, 1440);
		straight(true, 1440/2);
		right(0, 1440);
		launch();*/
	}
}
