#include <ros/ros.h>
#include "std_msgs/String.h"
#include <std_msgs/Float32.h>  //for subsriber
#include <vector>
#include <iostream>
#include <tf/tf.h>
#include "Python.h"
#include <sound_play/sound_play.h>

bool pathBlocked = false;
bool laserObstacle = false;

void laserObstacle_cb(std_msgs::Float32 blockedReading){
	if(blockedReading.data == 1){
			laserObstacle = true;
	}else{
			laserObstacle = false;
	}
}

void robotStuck_cb(std_msgs::Float32 robotStuck){
		if(robotStuck.data == 1){
				pathBlocked = true;
		}else{
				pathBlocked = false;
		}
	
}

void playSound(){
	printf("%s\n","Ok should be speaking now");
	
}

void sleepok(int t, ros::NodeHandle &n){
		if(n.ok()){
				sleep(t);
		}
}

int main(int argc, char **argv){
		ros::init(argc, argv, "speaker");
		ros::NodeHandle n;
		sound_play::SoundClient sc;
		sleepok(1,n);
		ros::Subscriber sub = n.subscribe("laser_pub", 100, laserObstacle_cb);
		ros::Subscriber subTwo = n.subscribe("path_pub",100,robotStuck_cb);
		ros::Rate r(60);//70
		while (ros::ok())
		{
			if(pathBlocked || laserObstacle){
					playSound();
					sc.say("get out of my way please");
					sleepok(2,n);
			}
			ros::spinOnce();
			r.sleep();
		}
		return 0;
}
