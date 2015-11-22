#ifndef __CLAW__
#define __CLAW__
#include "motor.h"
//#include "connection.h"
/*enum motorPort{
	A = OUT_A;
	B = OUT_B;
};*/
class Claw{
	public :
		Claw(Motor*, Motor*);
		
		void initialize();
		
		void open();
		void close();
		bool isOpen();
		
		void rotateClaw(int);
		
		void unlockClaw();
		void lockClaw();
		
		long getRotatePosition();
		bool isUnlocked();
		
		
		
		Motor *clawMotor;
		Motor *rotateClawMotor;
		bool unlocked, clawIsOpen, rotated;
		long rotation;
};
#endif
