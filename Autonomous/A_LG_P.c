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
/*
Autonomous_Low_Goal_Pole Summary
--------------------------------
Code written by 6699 Tempest member, Aaron Cook

For usage: please refer to A_LG.c

Summary
-------
-Determine position via ultrasonic sensor
-Move accordingly to be parallel to the long side of the module
-Ram the pole at max speed
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
servo[IR1] = 60;
servo[IR2] = 180;
bool Triangulating = true;
ClearTimer(T1);
while(Triangulating && time1(T1) < 10000){
	nxtDisplayCenteredTextLine(1, "%i", SensorValue(SeekerL));
	nxtDisplayCenteredTextLine(2, "%i", SensorValue(SeekerR));

	//Ultrasonic stop
	if(USreadDist(Ultra1) < 10){
		ClearTimer(T1);
		Right(0);
		Left(0);
		while(USreadDist(Ultra1) < 10 && time1(T1) < 1000){
		}
		if(time1(T1) >= 1000){
			break;
		}
	}

	if(SensorValue(SeekerL) == 0 || SensorValue(SeekerR) == 0){
		Right(mSpeed/2);
		Left(mSpeed/2);
	}

	if(SensorValue(SeekerL) < 6 && SensorValue(SeekerL) != 0){
	if(SensorValue(SeekerR) < 4 && SensorValue(SeekerR) != 0){
		//left
		Right(mSpeed);
		Left(-mSpeed);
	}

	if(SensorValue(SeekerR) > 4){
		//forward
		Right(mSpeed/2);
		Left(mSpeed/2);
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
		Right(-mSpeed/2);
		Left(-mSpeed/2);
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
}



void straight(bool dir, int distance){
	int factor;

	if(dir){
		factor = 1;
	}else{
		factor = -1;
	}

	origHeading = currHeading;

	nMotorEncoder(Right2) = 0;
	wait10Msec(50);

	Right(factor*mSpeed);
	Left(factor*mSpeed);

	while(abs(nMotorEncoder(Right2)) <= distance){
		//goStraight(dir, origHeading);
		//checkObstacle(dir);
		//nxtDisplayCenteredTextLine(1, "%i", nMotorEncoder(Right2));
	}

	Right(0);
	Left(0);
}



void left(int angle){
	nMotorEncoder(Right2) = 0;
	wait10Msec(50);

	origHeading = currHeading;

	Right(mSpeed);
	Left(-mSpeed);

	trackTurning(angle, false, origHeading);

	Right(0);
	Left(0);
}



void right(int angle){
	nMotorEncoder(Right2) = 0;
	wait10Msec(50);

	origHeading = currHeading;

	Right(-mSpeed);
	Left(mSpeed);

	trackTurning(angle, true, origHeading);

	Right(0);
	Left(0);
}


task main(){
	waitForStart();

	for(int i=0; i <= 20; i++){
		if(abs(USreadDist(Ultra1) - 105) < 5){
			modPos = 1;
			break;
		}

		if(abs(USreadDist(Ultra1) - 125) < 5){
			modPos = 3;
			break;
		}

		if(i == 20){
			modPos = 2;
			break;
		}
	}

	servo[Deploy] = 50;
	servo[IR1] = 30;
	servo[IR2] = 210;
	nMotorEncoder(Right2) = 0;
	wait1Msec(50);
	PlaySound(soundBeepBeep);
	HTGYROstartCal(Gyro);
	PlaySound(soundBeepBeep);
	StartTask(getHeading);


	if(modPos == 1){
		straight(true, 1440);
		right(45);
		straight(true, 1440*2);
		left(360);

		motor[Right1] = 75;
		motor[Right2] = 75;
		motor[Left1] = 75;
		wait1Msec(10000);
	}

	if(modPos == 2){
		straight(true, 1440*4);
		right(32.35);

		motor[Right1] = 75;
		motor[Right2] = 75;
		motor[Left1] = 75;
		wait1Msec(10000);
	}

	if(modPos == 3){
		straight(true, 1440);
		left(315);
		straight(true, 1440*6);
		right(90);

		motor[Right1] = 75;
		motor[Right2] = 75;
		motor[Left1] = 75;
		wait1Msec(10000);
	}
}
