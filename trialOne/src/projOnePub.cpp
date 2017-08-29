#include <ros/ros.h>
#include "std_msgs/String.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/PoseArray.h"
#include "geometry_msgs/PoseStamped.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Vector3.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <std_msgs/Float32.h>  //for subsriber
#include <vector>
#include <iostream>
#include "nav_msgs/Path.h"
#include <tf/tf.h>
#include "move_base_msgs/MoveBaseAction.h"
#include "move_base_msgs/MoveBaseActionResult.h"
#include "actionlib_msgs/GoalStatus.h"
#include <math.h>
#include "geometry_msgs/Point.h"
#include "Python.h"
#include <sstream>
#include <climits>

geometry_msgs::Pose currPose;
geometry_msgs::Pose targetPose;
nav_msgs::Path current_path;
bool planReceived = false;
bool currPoseReceived = false;
bool goalReached = false;
bool targetReceived = false;
bool notMoving = false;
geometry_msgs::Pose intermediateGoal;
std::vector<geometry_msgs::PoseStamped> currentPathPoses;



void pathCallBackFunction(const nav_msgs::Path::ConstPtr& msg){

		current_path = *msg;
		planReceived = true;
		//currentPathPoses = current_path.poses;
		
				
}

void poseCallBackFunction(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& msg){
		geometry_msgs::PoseWithCovarianceStamped poseStamped = *msg;
		currPoseReceived = true;
		currPose = poseStamped.pose.pose;
		ROS_INFO("poseCallBackFunction"); 
}

void goalReachedCallBackFunction(const move_base_msgs::MoveBaseActionResult::ConstPtr& msg){
	//std::cout<<"in reachGoal callBack "<<std::endl;
	//ROS_INFO("%s","in goalReached callback");
	move_base_msgs::MoveBaseActionResult moveBaseActResult = *msg;
	actionlib_msgs::GoalStatus status = moveBaseActResult.status;
	unsigned int goalStatus = status.status;
	if(goalStatus == 3) {
			goalReached = true;

	}
	if(goalStatus == 4) {
			//Plan was aborted
			std::cout<<"target recieved is now false as plan was aborted "<<std::endl;
			targetReceived = false;
	}
}

void targetPoseCallBackFunction(const geometry_msgs::PoseStamped::ConstPtr& msg){
		geometry_msgs::PoseStamped poseMsg = *msg;
		targetPose = poseMsg.pose;
		targetReceived = true;
		std::cout<<"target recieved is true "<<std::endl;
		
}

void cmd_velCallBackFunction(const geometry_msgs::Twist::ConstPtr& msg){

	geometry_msgs::Twist twist = *msg;
	//std::cout<<"in call back for cmd "<<std::endl;
	//ROS_INFO("%s", "Im in callback for cmd");
	if(twist.linear.x == 0.0 && twist.linear.y == 0.0 && targetReceived){
		notMoving = true;
		//printf("%s","Ok not moving");
	}else{
		if(twist.angular.z == 1.0){
			//printf("%s","ok now im not moving - only roatating");
		}
		notMoving = false;
	}
}


double calculateDistance(geometry_msgs::Pose target, geometry_msgs::Pose other){
		return sqrt( pow((target.position.x - other.position.x),2) + pow((target.position.y - other.position.y),2));
}

int main(int argc, char **argv)
{
	
  
  ros::init(argc, argv, "move_base_client");
  ros::NodeHandle n;
 std::cout<<"in initial part of main "<<std::endl;
 // typedef::geometry_msgs::PoseStamped[]_<std::allocator<void> >PoseStamped;
  ros::Subscriber subPath = n.subscribe("/move_base/EBandPlannerROS/global_plan", 1000, pathCallBackFunction);

  ros::Subscriber subPose = n.subscribe("/amcl_pose",1000, poseCallBackFunction);
  ros::Subscriber subTargetPose = n.subscribe("/move_base/current_goal", 1000, targetPoseCallBackFunction);
  ros::Subscriber subGoalReached = n.subscribe("/move_base/result",1000,goalReachedCallBackFunction);
  ros::Subscriber subVelocity = n.subscribe("/cmd_vel",1000,cmd_velCallBackFunction);
  ros::Publisher path_pub = n.advertise<std_msgs::Float32>("path_pub",1);
  ros::Rate loop_rate(10);//30
  ros::Rate inner_loop_rate(100);//100
  std_msgs::Float32 res;
  
  while((!planReceived || !currPoseReceived) && ros::ok() ) {
  	//std::cout<<"inside while"<<std::endl;
  	//std::cout<<plaaaaaaaaanReceived<<std::endl; 
	ros::spinOnce();
    loop_rate.sleep();
	 
	}
  std::cout<<"outside while "<<std::endl;

  float prevDist = 0;
  float prevTargetX = 0.0;
  float prevTargetY = 0.0;
  float oldSize = INT_MAX;
  float currPlanSize = 0;
  std::cout<<"next step will start ros::ok"<<std::endl;
  while(ros::ok()) {
	res.data = 0;
	path_pub.publish(res); 
	//std::cout<<"now in rosok loop "<<std::endl;
	while(targetReceived ) {
		std::cout<<"in target recieved loop"<<std::endl;
		currPlanSize = current_path.poses.size();
		//ROS_INFO("%lu ",current_path.poses.size());	//Long unsigned int
		geometry_msgs::Pose intermediateGoal = (current_path.poses).front().pose;
		float currX = (current_path.poses).front().pose.position.x;
		float currY = (current_path.poses).front().pose.position.y;
		//ROS_INFO("%f, %f", (current_path.poses).front().pose.position.x, (current_path.poses).front().pose.position.y);
		float currDist = calculateDistance(intermediateGoal,currPose);
		ROS_INFO("%s , %f", " Distance from target", currDist);
		//loop_rate.sleep
		if((currDist - prevDist > 0.5 && currX == prevTargetX && currY == prevTargetY) || (oldSize > currPlanSize + 5)){ //||notMoving || (oldSize < currPlanSize + 5) 
				ROS_INFO("%s, %f, %f, %f %s, %f , %f , %f", "Robot stuck. current dist :" , currDist, currX, currY," prev was ", prevDist, prevTargetX, prevTargetY);
				std::cout<<"Stuck detetct"<<std::endl;
				//runPythonFile(argv);
				res.data = 1;
		}else{
				res.data = 0;
		}
		if(goalReached) {
			targetReceived = false;
			std::cout<<"target recieved is false now as goal reached "<<std::endl;
			goalReached = false;
		}
		path_pub.publish(res);
		prevDist = currDist;
		prevTargetX = currX;
		prevTargetY = currY;
		oldSize  = currPlanSize;
		ros::spinOnce();			//if do only spin - doesnt update enough
		inner_loop_rate.sleep();
	}
	res.data = 0;
	path_pub.publish(res);
	ros::spinOnce(); //changed from spinOnce
	loop_rate.sleep();
  }
  ROS_INFO("%s","out of both loops");
  return 0;
}
	
