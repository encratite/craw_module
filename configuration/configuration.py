#Chicken configuration

#Enable/disable chicken on low life
chicken = True

#This tells the script to leave the game when life <= 80%
chicken_ratio = 0.8

#Leave the game when somebody declares hostility to you?
chicken_on_hostile = False

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

#Entering TPs in town
enter_town_tp_command = 'enter town tp'

#Entering TPs outside of town
enter_tp_command = 'enter tp'

#Attempt to make a town portal and enter it
go_to_town_command = 'go to town'

#Order bots to leave the game
leave_command = 'leave'

#Order bots to attack monsters with left skill (BOWS!)
attack_command = 'attack'

#Order bots to stop firing at monsters
stop_attacking_command = 'stop attacking'

#Delay between attacks in seconds
follower_attack_delay = 0.7

#Radius for turning off randomised attacks in follower attack mode
follower_defence_radius = 20
follower_attack_radius = 80

follower_attack_maximal_randomisation_position = 3

#Timing for the follow bot
follow_portal_move_delay = 0.4
follow_portal_interact_delay = 0.05

#Replies to chat triggers for the follow bot
follow_confirmation = 'Following %s'
stop_confirmation = 'No longer following %s'
enter_tp_confirmation = 'Entering the town portal of %s'
attack_confirmation = 'Commencing attack'
stop_attacking_command = 'Stopping attack'

stop_error = 'Currently not following anybody'
tp_error = 'Unable to detect the town portal of %s'
attack_error = 'Already attacking'
stop_attacking_error = 'Currently not attacking any targets'

mana_remaining = ' (%d/%d mana remaining)'

#Names of enchanters that will listen to enchant commands issued
enchanters = []

#Chat trigger for the enchant bot
enchant_command = 'enchant'
enchant_minions_command = 'enchant minions'
enchant_all_command = 'enchant all'
enchant_players_command = 'enchant players'

#Replies to chat triggers for the enchant bot

enchant_confirmation = 'I have enchanted you.' + mana_remaining
enchant_minions_confirmation = 'I have enchanted %d of your minions.' + mana_remaining
enchant_all_confirmation = 'I have enchanted you and %d of your minions.' + mana_remaining

enchant_range_error = 'You do not appear to be within range.'
enchant_mercenary_error = 'You do not appear to have any minions.'
enchant_mana_lack_warning = 'Warning: I currently do not have enough mana to fully complete this operation.' + mana_remaining

maximal_enchant_distance = 25
enchant_delay = 0.3

#Names of BO barbarians for the BO bot
battle_orders_barbarians = []

#Chat triggers for the BO bot
battle_orders_command = 'bo'

#Cast delay for the BO barbarian
battle_orders_switch_delay = 0.2
battle_orders_cast_delay = 0.5

battle_orders_confirmation = 'Casting Battle Command, Battle Orders and Shout, please wait...'

battle_orders_town_error = 'Cannot cast Battle Orders in town.'

party_error = 'We are not in the same party.'
mana_error = 'I currently do not have enough mana.' + mana_remaining

"""
The following variable can either be a list of players to whom the enchant and
BO commands are restricted or you can simply set it to 'None' which indicates
that all players have access to the commands.
"""

remote_command_privilege_users = None

#Characters for which to enable automatic casting
auto_casters = followers
auto_cast_switch_delay = 0.15
auto_cast_cast_delay = 0.35

#Necromancers which are supposed to automatically revive dead monsters
summoners = []

summoner_check_delay = 0.1
summoner_switch_delay = 0.2
summoner_cast_delay = 0.4

#Friends/party settings

friends = enchanters + battle_orders_barbarians + follow_leaders + followers + summoners + []

party_invitation_command = 'invite'
auto_accept_invitations_by_friends = True
auto_loot_permission_to_friends = False

#Packet logging for experiments/debugging

packet_logging = False
packet_log = 'packets.log'

"""
This variable is used by the experience information script.
It allows you to filter out monsters that give you little or no experience.
If you need to kill more monsters than [experience_peanut_limit] to reach the next level then it will be ignored.
"""

experience_peanut_limit = 2000

#BNCS configuration for auto whois on player kills

whois_delay = 1.0

#PK script configuration

maximal_attack_distance = 60

#delays in seconds

town_portal_delay = 0.28
bone_prison_delay = 0.28

bone_spirit_teleport_offset = (0, 6)

fist_of_heavens_distance = 40
fist_of_heavens_delay = 0.3