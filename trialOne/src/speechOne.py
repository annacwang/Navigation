#!/usr/bin/env python
import roslib; roslib.load_manifest('sound_play')
import rospy
from sound_play.libsoundplay import SoundClient


rospy.init_node('play_sound')
sound_client = SoundClient()

rospy.sleep(1)
sound = sound_client.say('Could you get out of my way please.')
#sound.play()
