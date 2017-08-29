Members :

Bonny Mahajan
Anna Wang

Install instructions :

1)Our program uses sound_play node which uses audio_common package . But I've included a copy of the audio_common package in the include folder for our project – so it need not be installed separately.


Note incase – audio_common not found use this link to install it :

http://wiki.ros.org/audio_common

2)audio_common does require – gstreamer to work– which can be installed using command from the terminal

apt-get install libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev

(May need to do use sudo)

To run our program : 

1. Run bwi_launch segbot_v2.launch file on the robot first – so that rviz can load up .
2. Localize the robot in rviz
3. Run the projOnePub.cpp file from the terminal using 

	rosrun trialOne projOnePub


4.Run the laserObstaclePub.cpp from the temrinal using

        rosrun trialOne laserObstaclePub

5.Run the sound_play node in another terminal window [ required for speaking]
The sound_play node needs roscore to be running so make sure step 1 was executed

	rosrun sound_play soundplay_node.py

6.Run our speaker.cpp file ine the terminal using

	rosrun trialOne speaker

7.Give the robot a goal in rviz and get in it's way to hear it speak.



Link to our code 

https://github.com/bmahajan/friSpring2017


Thank you to Jivko and Nathan for suggesting approaches on how to detect when the robot was stuck.Thank you to other TAs and mentors for helping when we were stuck.

Links to our result videos :

https://www.youtube.com/watch?v=mTQ2EqeW9aU

https://www.youtube.com/watch?v=61HV83vgC3w

