#include "drive.h"

Drive::Drive(Motor *motorA, Motor *motorB){
	leftDrive = motorA;
	rightDrive = motorB;
	unlocked = true;
}

void Drive::forward(int lSpeed, int rSpeed){
	//if (unlocked){
		//cout << "hello";
		leftDrive->on(lSpeed);
		rightDrive->on(rSpeed);
<<<<<<< HEAD
		//cout<<"encoders?"<<leftDrive->get_rotation();
=======
>>>>>>> master
	//}
}

void Drive::reverse(int speed){
	if (unlocked){
		leftDrive->on(-speed);
		rightDrive->on(-speed);
	}
}

void Drive::turnLeft(int speed){
	if (unlocked){
		leftDrive->off();
		rightDrive->on(speed);
	}
}

void Drive::rotateCCW(int speed){
		leftDrive->on(-speed);
		rightDrive->on(speed);
}

void Drive::turnRight(int speed){
	if (unlocked){
		leftDrive->on(speed);
		rightDrive->off();
	}
}

void Drive::rotateCW(int speed){
	if (unlocked){
		leftDrive->on(speed);
		rightDrive->on(-speed);
	}
}

void Drive::unlockDrive(){
	unlocked = true;
}

void Drive::lockDrive(){
	unlocked = false;
	leftDrive->off();
	rightDrive->off();
}
