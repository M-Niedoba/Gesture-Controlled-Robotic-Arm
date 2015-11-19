// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>

#include <myo.hpp>

#include <cstdlib>
#include <conio.h>
#include "nxt.h"
#include <windows.h>
#include <stdlib.h>
#include <fstream>
#include "drive.h"
#include "armlift.h"
#include "claw.h"
//#include "laptop.h"

Connection *driveConnection = new Bluetooth();
Connection *armConnection = new Bluetooth();

Motor *motorLeftDrive = new Motor (OUT_C, driveConnection);
Motor *motorRightDrive = new Motor (OUT_A, driveConnection);

Motor *motorLiftArm = new Motor (OUT_B, driveConnection);
Motor *motorToggleClaw = new Motor (OUT_A, armConnection);
Motor *motorRotateClaw = new Motor (OUT_C, armConnection);

Drive drive(motorLeftDrive, motorRightDrive);
ArmLift arm (motorLiftArm);
Claw claw (motorToggleClaw, motorRotateClaw);

float motor_speed = 0;
const int longSpeed[15]={75, 40, 25, 15, 10, 5, 0, 0, 0, -5, -10, -15, -25, -40, -75};
const int latSpeed[16]={0, 40, 25, 15, 10, 5, 100, 0, 0, 0, -5, -10, -15, -25, -40, 0};

void selectPort(ifstream &in, int &drivePort, int &armPort){
	cout<<"\nSelect Connection Device"<<endl;
	cout<<"Port"<<setw(20)<<"Name"<<endl;
	
	string n0;
	int p0;
	
	while(in >> p0 >> n0){
		cout<<setw(4)<<p0<<setw(20)<<n0<< endl;
	}
	
	cout << "\nEnter the drive and arm com ports you would like to use " << endl;
	cout << "from the above list or any other port you would like to use." << endl;
	cin >> drivePort >> armPort;
}

class DataCollector : public myo::DeviceListener {
private:
		
		boolean armOn, driveOn, controllingDrive;
			
public:
    DataCollector()
    : onArm(false), isUnlocked(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose()
    {
    }
    
    void initializeBools(){
    	armOn = true;
    	driveOn = true;
    	controllingDrive = true;
	}

    void onUnpair(myo::Myo* myo, uint64_t timestamp)
    {
        roll_w = 0;
        pitch_w = 0;
        yaw_w = 0;
        onArm = false;
        isUnlocked = false;
    }
	
	void longitudinalSpeed (float pitch){
		int pitch_w = static_cast<int>((pitch + (float)M_PI/2.0f)/M_PI * 18);
		if (controllingDrive && driveOn){
			drive.forward(longSpeed[pitch_w]);
		//cout << longSpeed[pitch_w] << endl;
		} else if (armOn){
			arm.raise(longSpeed[pitch_w]);
		}
	}
	
	void lateralSpeed (float yaw){
		int yaw_w = static_cast<int>((yaw + (float)M_PI)/(M_PI * 2.0f) * 18);
		if (controllingDrive && driveOn){
			if (yaw_w > 2 && yaw_w < 7){
				drive.turnLeft(latSpeed[yaw_w]);
			} else if (yaw_w > 9 && yaw_w < 14){
				drive.turnRight(latSpeed[yaw_w]);
			}
		}
		//cout << latSpeed[yaw_w] << endl;
	}
	
/*	void toggleDriveControl (){
		if (controllingDrive){
			controllingDrive = false;
		} else {
			controllingDrive = true;
		}
	}*/
	
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
    {
        using std::atan2;
        using std::asin;
        using std::sqrt;
        using std::max;
        using std::min;

        float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
        float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
        float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                        1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
		
		//longitudinalSpeed(pitch);
		//lateralSpeed(yaw);
        roll_w = static_cast<int>((roll + (float)M_PI)/(M_PI * 2.0f) * 18);
       	pitch_w = static_cast<int>((pitch + (float)M_PI/2.0f)/M_PI * 18);
        yaw_w = static_cast<int>((yaw + (float)M_PI)/(M_PI * 2.0f) * 18);   
    }

    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        currentPose = pose;

        if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
            myo->unlock(myo::Myo::unlockHold);
            
            //If the user makes a fist
            if (pose == myo::Pose::waveIn){
            	
            	//If not controlling a drive, switch this on
	    		if (!controllingDrive){
					controllingDrive = true;
				}
				
				//If drive is locked, unlock it
	        	if(!driveOn){
	        		driveOn = true;
	        		std::cout << "Drive Unlocked!" << endl;
        		//Else lock it , stop the drive
				} else {
					driveOn = false;
					drive.forward(0);
					std::cout << "Drive Locked!" << endl;
				}
				
			//If the user spreads their fingers	
			} else if (pose == myo::Pose::waveOut){
				//If the user is controlling the drive, switch to arm control
				if (controllingDrive){
					controllingDrive = false;
				}
				
				//If the arm is locked, unlock it
				if (!armOn){
					armOn = true;
					std::cout << "Arm Unlocked!" << endl;
				
				//If the arm is unlocked, lock it	
				} else {
					armOn = false;
					std::cout << "Arm Locked!" << endl;
				}
			}
			
			//If user swipes to the right
			else if (pose == myo::Pose::fingersSpread){
				cout<< "WAVEIN DETECTED" << endl;
				if(!controllingDrive)
			       cout<< "cond1 met"<<endl;
	   			if(!armOn)
				   cout<< "cond2 Met"<<endl;   
				if((!controllingDrive) && (!armOn) ){
					cout<<"Opening Claw"<<endl;
					claw.open();
				}
			}
			else if(pose == myo::Pose::fist){
				 cout<< "WAVEOUT DETECTED" << controllingDrive << armOn << endl;
				if(!controllingDrive && !armOn){
					cout<<"Closing Claw";
					claw.close();
				}
			}
			
			
			if (pose == myo::Pose::doubleTap){
				myo->lock();
			}

            myo->notifyUserAction();
        } else {
            myo->unlock(myo::Myo::unlockHold);
        }
    }

    void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation,
                   myo::WarmupState warmupState)
    {
        onArm = true;
        whichArm = arm;
    }

    void onArmUnsync(myo::Myo* myo, uint64_t timestamp)
    {
        onArm = false;
    }

    void onUnlock(myo::Myo* myo, uint64_t timestamp)
    {
        isUnlocked = true;
    }

    void onLock(myo::Myo* myo, uint64_t timestamp)
    {
        isUnlocked = false;
    }

    void print()
    {
        string controllingNow;
		std::cout << '\r';
        if (controllingDrive){
			controllingNow = "drive";
		} else {
			controllingNow = "arm"; 
		}
        std::cout   //<< '[' << std::string(roll_w, 'R') << std::string(18 - roll_w, ' ') << ']'
//                  	<< '[' << std::string(pitch_w, 'P') << std::string(18 - pitch_w, ' ') << ']'
//                  	<< '[' << std::string(yaw_w, 'Y') << std::string(18 - yaw_w, ' ') << ']'
                  	<< '[' << controllingNow << yaw_w << ": " << latSpeed[yaw_w] << std::string(18 - yaw_w, ' ') << ']'
                  	<< '[' << controllingNow << pitch_w << ": " << longSpeed [pitch_w] << std::string(18 - pitch_w, ' ') << ']';
//                  << '[' << motorSpeed[pitch_w] << std::string(18 - pitch_w, ' ') << ']'
//                  << '[' << motor_speed << std::string(18 - pitch_w, ' ') << ']';

        if (onArm) {
            std::string poseString = currentPose.toString();

            std::cout << '[' << (isUnlocked ? "unlocked" : "locked  ") << ']'
//                      << '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
                      << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
        } else {
//            std::cout << '[' << std::string(8, ' ') << ']' << "[?]" << '[' << std::string(14, ' ') << ']';
        }

        std::cout << std::flush;
    }

    bool onArm;
    myo::Arm whichArm;

    bool isUnlocked;

    int roll_w, pitch_w, yaw_w;
    myo::Pose currentPose;
};



int main(int argc, char** argv)
{
	ifstream comIn ("comports.txt");
	cout<<"flag";
	int driveCom = 0, armCom = 0;
	selectPort(comIn, driveCom, armCom);
	
	try {
	
	cout << "Trying to connect" << endl;
    driveConnection->connect(driveCom);
    armConnection->connect(armCom);
    cout << "Connected" << endl;
    claw.initialize();
	
    myo::Hub hub("com.example.hello-myo");

    std::cout << "Attempting to find a Myo..." << std::endl;

    myo::Myo* myo = hub.waitForMyo(10000);

    if (!myo) {
        throw std::runtime_error("Unable to find a Myo!");
    }

    std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

    DataCollector collector;
	collector.initializeBools();
    hub.addListener(&collector);

    while (1) {
        hub.run(1000/20);
        collector.print();
    }
	//driveConnection->disconnect();
	//armConnection->disconnect();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        driveConnection->disconnect();
        armConnection->disconnect();
        return 1;
    }
}

