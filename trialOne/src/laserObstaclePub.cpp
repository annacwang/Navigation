#include <ros/ros.h>
#include <vector>
#include <iostream>
#include <tf/tf.h>
#include "sensor_msgs/LaserScan.h"
#include <std_msgs/Float32.h>  //for subsriber
#include "geometry_msgs/Twist.h"

sensor_msgs::LaserScan scanData;
ros::Publisher laser_pub ;
float minDist = 100;	//is this in metres?
std_msgs::Float32 result ;
bool moving = false;
bool inRecovery = false;
void scannerCallBack(const sensor_msgs::LaserScan::ConstPtr& msg){
	minDist = 100;
	scanData = *msg;
	int startIndex = scanData.ranges.size()/5;
	int stopIndex = (scanData.ranges.size()*4)/5;
	for(int i = startIndex; i < stopIndex ;i++){
			if(scanData.ranges[i] < minDist){
					minDist = scanData.ranges[i];
					//std::cout<<"min dist is now "<<minDist<<std::endl;
			}
	}

	result.data = ((minDist <= 0.65 && moving) || inRecovery) ? 1 : 0;
	float angle_min = scanData.angle_min;
	float angle_max = scanData.angle_max;
	//printf("%s ,%f , %s ,%f","min :",angle_min, " max:",angle_max); 
	laser_pub.publish(result);
	ros::spinOnce();
} 

void cmd_velCallBackFunction(const geometry_msgs::Twist::ConstPtr& msg){

	geometry_msgs::Twist twist = *msg;
	//std::cout<<"in call back for cmd "<<std::endl;
	//ROS_INFO("%s", "Im in callback for cmd");
	
	if(twist.angular.z == 1) {
		std::cout<<"only rotating"<<std::endl;
		inRecovery = true;
	}
	else if(twist.linear.x == 0.0 && twist.linear.y == 0.0 ){ //&& twist.angular.z == 0
		moving = false;
		inRecovery = false;
		//printf("%s","Ok not moving");
	}
	else{
		inRecovery = false;
		moving = true;
	}
}

int main(int argc, char** argv) {
		ros::init(argc, argv, "distanceObstacle");
		ros:: NodeHandle nh;
		laser_pub = nh.advertise<std_msgs::Float32>("laser_pub",1);
		ros::Subscriber subScan = nh.subscribe("/scan",1,scannerCallBack);
		ros::Subscriber subVel = nh.subscribe("/cmd_vel",1000,cmd_velCallBackFunction);
		ros::spin();
		return 0;
		
	
}
