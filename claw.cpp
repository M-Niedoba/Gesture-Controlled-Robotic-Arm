#include "claw.h"

Claw::Claw(Motor *motorA, Motor *motorC){
	clawMotor = motorA;
	rotateClawMotor = motorC;
	unlocked = true;
	clawIsOpen = true;
	rotated = false;
 
}

void Claw::initialize(){
    clawMotor->reset_rotation(false);
	rotateClawMotor->reset_rotation(false);
	rotation = clawMotor->get_rotation();
}

void Claw::open(){
	if (!clawIsOpen&& !rotated){
		clawMotor->on(-20);
		clawIsOpen = true;
		while(clawMotor->get_rotation()>rotation-90){}
		clawMotor->off();
	}	
}

void Claw::close(){
	if (clawIsOpen){
		clawMotor->on(20);
		clawIsOpen = false;
		while(clawMotor->get_rotation()<rotation){
		cout<<"Claw:"<<clawMotor->get_rotation()<<"Goal"<< rotation<<endl;
		}
		clawMotor->off();
	}
}

bool Claw::isOpen(){
	return clawIsOpen;
}

void Claw::rotateClaw(int speed){
	if(!clawIsOpen&&rotated)
	rotateClawMotor->on(speed);
}

void Claw::unlockClaw(){
	unlocked = true;
}

void Claw::lockClaw(){
	unlocked = false;
	clawMotor->off();
	rotateClawMotor->off();
}


long Claw::getRotatePosition(){
	return rotateClawMotor->get_rotation();
}

bool Claw::isUnlocked(){
	return unlocked;
}


