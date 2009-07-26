#Chicken configuration

#Enable/disable chicken on low life
chicken = True

#This tells the script to leave the game when life <= 80%
chicken_ratio = 0.8

#Leave the game when somebody declares hostility to you?
chicken_on_hostile = True

#Breakpoint settings

increased_attack_speed = 240
faster_cast_rate = 240
faster_block_rate = 240
faster_hit_recovery = 240
faster_run_walk = 60

#Names of characters that are allowed to issue follow bot commands
follow_leaders = []

#Names of characters that are meant to follow the leader
followers = []

#Names of enchanters that will listen to enchant commands issued
enchanters = []

#Names of BO barbarians for the BO bot
battle_orders_barbarians = []

"""
The following variable can either be a list of players to whom the enchant and
BO commands are restricted or you can simply set it to 'None' which indicates
that all players have access to the commands.
"""

remote_command_privilege_users = None

#Packet logging for experiments/debugging

packet_logging = False
packet_log = 'packets.log'