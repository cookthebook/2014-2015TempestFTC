#pragma platform(NXT)
task main()
{
	eraseDisplay();
while(true){
	nxtDisplayCenteredTextLine(0, "Test");
	wait1Msec(2000);
}


}
