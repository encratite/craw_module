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

#Chat trigger strings for the follow bot
follow_command = 'follow'
stop_command = 'stop'
enter_tp_command = 'enter my tp'

#Replies to chat triggers for the follow bot
follow_confirmation = 'Following %s'
stop_confirmation = 'No longer following %s'
enter_tp_confirmation = 'Entering the town portal of %s'

stop_error = 'Currently not following anybody'
tp_error = 'Unable to detect the town portal of %s'

#Names of enchanters that will listen to enchant commands issued
enchanters = []

#Chat trigger for the enchant bot
enchant_command = 'enchant'
enchant_mercenary_command = 'enchant mercenary'
enchant_both_command = 'enchant us'

#Replies to chat triggers for the enchant bot
enchant_confirmation = 'I have enchanted you. (%d/%d mana remaining)'
enchant_mercenary_confirmation = 'I have enchanted your mercenary. (%d/%d mana remaining)'
enchant_both_confirmation = 'I have enchanted you and your mercenary. (%d/%d mana remaining)'

enchant_range_error = 'You do not appear to be within range.'
enchant_mercenary_error = 'You do not appear to have a mercenary.'
enchant_mana_error = 'I currently do not have enough mana. (%d/%d)'

maximal_enchant_distance = 25
enchant_delay = 0.3

#Names of BO barbarians for the BO bot
battle_orders_barbarians = []

#Chat triggers for the BO bot
battle_orders_command = 'bo'

party_error = 'We are not in the same party.'

"""
The following variable can either be a list of players to whom the enchant and
BO commands are restricted or you can simply set it to 'None' which indicates
that all players have access to the commands.
"""

remote_command_privilege_users = None

#Packet logging for experiments/debugging

packet_logging = False
packet_log = 'packets.log'