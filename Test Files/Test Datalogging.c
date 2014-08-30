#pragma platform(NXT)

#include "JoystickDriver.c"

const int test = 0;

task main()
{
for(int i = 0; i < 10; i++){
	AddToDatalog(test, 0);
}
SaveNxtDatalog();
}
