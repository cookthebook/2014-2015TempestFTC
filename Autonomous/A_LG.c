#pragma config(Hubs,  S1, HTServo,  HTMotor,  HTMotor,  HTMotor)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     SeekerR,        sensorHiTechnicIRSeeker1200)
#pragma config(Sensor, S3,     HTSMUX,         sensorI2CCustom)
#pragma config(Sensor, S4,     SeekerL,        sensorHiTechnicIRSeeker1200)
#pragma config(Motor,  motorC,          Finger,        tmotorNXT, PIDControl, encoder)
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
Autonomous_Low_Goal Summary and Usage
-------------------------------------
Code written by 6699 Tempest head programmer, Aaron Cook

This program is the baseline and "master code" to which all other Tempest programs take their functions.
All other programs steal functions/code from this one, thus newest and most improved code will be here first, or possibly in "Teleop.c"

Summary
-------
This program is designed to do the following:
	-Initialization of all servos and gyro calibration
	-Move forward to check if module is in position 1 via ultrasonic sensor

	IF IN POSITION 1:
		-triangulate with the beacon via two IR seekers and launch autonomous ball into the center goal
		-Attempt to knock down the pole by turning right, then left, then ramming it

	IF NOT IN POSITION 1 (modPos == 0):
		-Turn so that you would be parallel if in position 2
		-Check if module is in position 2

	IF IN POSITION 2:
		-Turn to face the center goal
		-triangulate and launch via the IR seekers
		-turn and attempt to knock down the pole again

	IF NOT IN POSITION 1 or 2 (position 3 by elimination):
		-go around to position for launch
		-triangulate and launch
		-turn and knock down pole (like previous)

Function Usage
--------------
(in order of appearance)

-float RPM(tMotor input) - return the rotations per second (not minute) of "input" motor (must have encoder)

-void Right(int speed) - move both right drive motors at percentage "speed"

-void Left(int speed) - right now, moves the left motor at "speed", can be changed to have exact same functionality is "Right"

-void goStraight(bool dir, float heading) - forces the robot to actually go straight (avoid drifting), using the gyro sensor
	-if dir = true, robot is going forward, else dir = false. This is important for correction
	-heading uses the same angular system as the gyro (the "getHeading" task)
	-also features obstacle detection based off of wheel rpm
	-to be used with "straight" function

-void checkObstacle(bool dir) - Obstacle detection via ultrasonic sensor
	-like before, dir = true is forward, false backwards
	-to be used with "straight", like "goStraight"

-void trackTurning(int angle, bool dir, float origHeading) - used to make precise turns with the gyro sensor
	-"angle" is the angle you want to go to, not append
	-dir, if true is right turn, else is left turn
	-"origHeading" is to help with the tricky situations when going from 359 to 0 deg, use a variable right before use set to "currHeading"
	-to be used with "right" or "left"

-void triangulate() - positions robot such that SeekerL read 6 and SeekerR read 4, a very precise position for launching
	-NOTE: autonomous version does not feature teleop's aspect of a manual kill function, rather uses a timer to kill

-void straight(bool dir, int distance) - goes straight for "distance" encoder ticks
	-dir = true, forward, else false is backwards

- void left(int angle)/void right(int angle) - turn the robot left/right to a given "angle"

-void launch() - simple launching sequence for autonomous
*/

#define Gyro			msensor_S3_3
#define Ultra1		msensor_S3_4

int modPos;
int mSpeed = 25;
int lSpeed = 100;
float minimumRPM = 120.3;
float maximumRPM = 130.5;
float tooSlow = 0.2;
float tThreshold = 10.0;
float origHeading;


//This code for getHeading was found on the RobotC forums and is not fully from scratch//
// Current heading of the robot
float currHeading = 0;

// Task to keep track of the current heading using the HT Gyro
task getHeading() {
	float delTime = 0;
	float prevHeading = 0;
	float curRate = 0;

  PlaySound(soundBeepBeep);
  while (true) {
    time1[T1] = 0;
    curRate = HTGYROreadRot(Gyro);
    if (abs(curRate) > 3) {
      prevHeading = currHeading;
      currHeading = prevHeading + curRate * delTime;
      if (currHeading > 360) currHeading -= 360;
      else if (currHeading < 0) currHeading += 360;
    }
    wait1Msec(5);
    delTime = ((float)time1[T1]) / 1000;
    //delTime /= 1000;
  }
}



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
}



void goStraight(bool dir, float heading){
	int factor;

	if(dir){
		factor = 1;
	}else{
		factor = -1;
	}

	if((heading - currHeading)%360 > 5){
			Right(mSpeed*factor - 5);
			Left(mSpeed*factor + 5);
		}
	else if((currHeading - heading)%360 > 5){
		Right(mSpeed*factor + 5);
		Left(mSpeed*factor - 5);
	}
	else{
		Right(mSpeed*factor);
		Left(mSpeed*factor);
	}

	//Obstacle based off RPM
	if(RPM(Right1) >= tooSlow){
		ClearTimer(T2);
	}

	if(RPM(Right1) < tooSlow && time1(T2) > 2000){
		Right((mSpeed/4)*(-factor));
		Left((mSpeed/4)*(-factor));
		wait1Msec(1000);
		Right(0);
		Left(0);
		wait1Msec(1000);
		Right(mSpeed*factor);
		Left(mSpeed*factor);
		ClearTimer(T2);
	}
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



void trackTurning(int angle, bool dir, float origHeading){
	while(true){
		if(dir && origHeading > angle){
			while(360-currHeading <= 365-origHeading){
			}
		}
		if(!dir && origHeading < angle){
			while(currHeading <= origHeading + 5){
			}
		}

		if(dir){
			if(currHeading >= angle) break;
		}
		if(!dir){
			if(currHeading <= angle) break;
		}
	}
}



void triangulate(){
servo[IR1] = 30;
servo[IR2] = 210;
bool Triangulating = true;
ClearTimer(T1);
while(Triangulating && time1(T1) < 10000){
	nxtDisplayCenteredTextLine(1, "%i", SensorValue(SeekerL));
	nxtDisplayCenteredTextLine(2, "%i", SensorValue(SeekerR));

	//Ultrasonic stop
	/*if(USreadDist(Ultra1) < 10){
		ClearTimer(T1);
		Right(0);
		Left(0);
		while(USreadDist(Ultra1) < 10 && time1(T1) < 1000){
		}
		if(time1(T1) >= 1000){
			break;
		}
	}*/

	if(SensorValue(SeekerL) == 0 || SensorValue(SeekerR) == 0){
		Right(mSpeed/3);
		Left(mSpeed/3);
	}

	if(SensorValue(SeekerL) < 6 && SensorValue(SeekerL) != 0){
	if(SensorValue(SeekerR) < 4 && SensorValue(SeekerR) != 0){
		//left
		Right(mSpeed/2);
		Left(-mSpeed/2);
	}

	if(SensorValue(SeekerR) > 4){
		//forward
		Right(mSpeed/3);
		Left(mSpeed/3);
	}

	if(SensorValue(SeekerR) == 4){
		//left
		Right(mSpeed/2);
		Left(-mSpeed/2);
	}
	}



	if(SensorValue(SeekerL) > 6){
	if(SensorValue(SeekerR) < 4 && SensorValue(SeekerR) != 0){
		//back
		Right(-mSpeed/3);
		Left(-mSpeed/3);
	}

	if(SensorValue(SeekerR) > 4){
		//right
		Right(-mSpeed/2);
		Left(mSpeed/2);
	}

	if(SensorValue(SeekerR) == 4){
		//right
		Right(-mSpeed/2);
		Left(mSpeed/2);
	}
	}



	if(SensorValue(SeekerL) == 6){
	if(SensorValue(SeekerR) < 4 && SensorValue(SeekerR) != 0){
		//left
		Right(-mSpeed/2);
		Left(mSpeed/2);
	}

	if(SensorValue(SeekerR) > 4){
		//right
		Right(-mSpeed/2);
		Left(mSpeed/2);
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
}



void straight(bool dir, int distance){
	int factor;

	if(dir){
		factor = 1;
	}else{
		factor = -1;
	}

	//origHeading = currHeading;
	Right(0);
	Left(0);

	nMotorEncoder(Right2) = 0;
	wait10Msec(100);

	Right(factor*mSpeed);
	Left(factor*mSpeed);

	while(abs(nMotorEncoder(Right2)) < distance || abs(nMotorEncoder(Right2)) > distance + 200){
		//goStraight(dir, origHeading);
		//checkObstacle(dir);
		if(abs(nMotorEncoder(Right2)) > distance + 200){
			PlaySound(soundBeepBeep);
		}
	}

	Right(0);
	Left(0);
}



void left(int angle){
	nMotorEncoder(Right2) = 0;
	wait10Msec(100);

	origHeading = currHeading;

	Right(mSpeed);
	Left(-mSpeed);

	trackTurning(angle, false, origHeading);

	Right(0);
	Left(0);
}



void right(int angle){
	nMotorEncoder(Right2) = 0;
	wait10Msec(100);

	origHeading = currHeading;

	Right(-mSpeed);
	Left(mSpeed);

	trackTurning(angle, true, origHeading);

	Right(0);
	Left(0);
}



void launch(){
	triangulate();

	if(modPos == 1) left(360);
	if(modPos == 2)	left(45);
	if(modPos == 3) left(90);

	motor[Launch1] = 0;
	motor[Launch2] = 0;

	ClearTimer(T4);
	wait1Msec(100);

	if(USreadDist(Ultra1) <= 47.5){
	while(USreadDist(Ultra1) <= 47.5 || USreadDist(Ultra1) > 250){
		Right(-mSpeed/2);
		Left(-mSpeed/2);
		PlaySound(soundBeepBeep);
	}
	}
	else{
	while(USreadDist(Ultra1) > 47.5 || USreadDist(Ultra1) > 250){
		Right(mSpeed/3);
		Left(mSpeed/3);
		PlaySound(soundUpwardTones);
	}
	}
	Right(0);
	Left(0);

	PlaySound(soundBlip);
	wait1Msec(2000);

	motor[Launch1] = lSpeed/4;
	motor[Launch2] = lSpeed/4;
	wait1Msec(250);
	motor[Launch1] = lSpeed/2;
	motor[Launch2] = lSpeed/2;
	wait1Msec(250);
	motor[Launch1] = lSpeed*3/4;
	motor[Launch2] = lSpeed*3/4;
	wait1Msec(250);
	motor[Launch1] = lSpeed;
	motor[Launch2] = lSpeed;
	wait1Msec(3000);
	servo[Deploy] = 0;
	wait1Msec(2000);
	servo[Deploy]= 50;
}



task main(){
	waitForStart();

	int modCount = 0;
	for(int i=0; i <= 20; i++){
		if(abs(USreadDist(Ultra1) - 105) < 5){
			modPos = 1;
			break;
		}

		if(abs(USreadDist(Ultra1) - 125) < 5){
			modCount++;
		}
	}
	if(modPos != 1){
		if(modCount >= 15){
			modPos = 3;
		}
		else{
			modPos = 2;
		}
	}

	servo[Deploy] = 50;
	servo[IR1] = 30;
	servo[IR2] = 210;
	HTGYROstartCal(Gyro);
	StartTask(getHeading);

	if(modPos == 1){
		straight(true, 1440);
		launch();
	}

	if(modPos == 2){
		straight(true, 1440*2);
		left(315);
		straight(true, 1440*2.5);
		right(45);
		launch();
	}

	if(modPos == 3){
		straight(true, 1440*2);
		left(315);
		straight(true, 1440*3);
		right(90);
		launch();
	}
	/*straight(true, 1440*3);

	if((SensorValue(SeekerL) == 5 && SensorValue(SeekerR) == 5) || (SensorValue(SeekerL) == 6 || SensorValue(SeekerR) == 4) || (SensorValue(SeekerL) == 7 || SensorValue(SeekerR) == 3)){
		modPos = 1;
	}else{
		modPos = 0;
	}

	if(modPos == 1){
		launch();
		right(90);
		straight(true, 1440);
		left(0);
		Right(mSpeed*2);
		Left(mSpeed*2);
		wait1Msec(5000);
	}

	if(modPos == 0){
		Left(360-32.35);
		straight(true, 1440*1.5);
		if((SensorValue(SeekerL) == 8 || SensorValue(SeekerR) == 8) || (SensorValue(SeekerL) == 9 && SensorValue(SeekerR) == 9)){
			modPos = 2;
		}else{
			modPos = 3;
		}
	}

	if(modPos == 2){
		right(90-32.35);
		launch();
		right(90+32.35);
		straight(true, 1440);
		left(32.35);
		Right(mSpeed*2);
		Left(mSpeed*2);
		wait1Msec(5000);
	}

	if(modPos == 3){
		straight(true, 1440*1.5);
		right(0);
		straight(true, 1440);
		right(90);
		launch();
		right(180);
		straight(true, 1440);
		left(90);
		Right(mSpeed*2);
		Left(mSpeed*2);
		wait1Msec(5000);
	}*/
}
