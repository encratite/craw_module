from text import *

packet_logging = False
packet_log = 'packets.log'

chicken = True
chicken_ratio = 0.8
chicken_on_hostile = True

#Automap colours configuration

npc_colour = 0x20

monster_colour = 0x0a
minion_colour = 0xab
champion_colour = 0x9b
boss_colour = 0x87

#Order of immunities: fire, cold, lightning, poison, damage, magic
immunity_colours = [red, blue, yellow, green, gold, orange]

#The symbol to draw above a monster if it is immune to a particular type of damage
immunity_symbol = 'o'

special_abilities = {}

#Extra strong
special_abilities[5] = (red, 'St')

#Extra fast
special_abilities[6] = (red, 'F')

#Cursed
special_abilities[7] = (red, 'C')

#Magic resistant
#special_abilities[8] = (blue, 'Mr')

#Fire enchanted
special_abilities[9] = (red, 'E')

#Champion
#special_abilities[16] = (blue, 'C')

#Light enchanted
special_abilities[17] = (yellow, 'E')

#Cold enchanted
special_abilities[18] = (blue, 'E')

#Steals life
#special_abilities[24] = (blue, 'L')

#Manaburn
special_abilities[25] = (blue, 'M')

#Teleportation
special_abilities[26] = (yellow, 'T')

#Spectral hit
#special_abilities[27] = (blue, 'Sh')

#Stone Skin
#special_abilities[28] = (blue, 'St')

#Multiple shots
special_abilities[29] = (red, 'Ms')

#Aura enchanted
special_abilities[32] = (orange, 'A')

#Ghostly
#special_abilities[36] = (blue, 'G')

#Fanatic
#special_abilities[37] = (red, 'Fa')

#Possessed
#special_abilities[38] = (red, 'P')

#Berserker
#special_abilities[39] = (red, 'B')