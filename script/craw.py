#Nasty hack to import stuff from ../configuration
import os, sys

sys.path.append(os.path.join(sys.path[-1], '..', 'configuration'))

import configuration

#Set up handlers
from craw import set_automap_handler, set_packet_handler, set_command_handler, set_keyboard_handler
from automap import automap_handler_class
from packets import packet_handler_class
from chicken import chicken_handler_class
from command import command_handler_class
from town_portal import town_portal_class
from keyboard import keyboard_handler_class

command_handler = command_handler_class()
set_command_handler(command_handler.process_command)

automap_handler = automap_handler_class()
set_automap_handler(automap_handler.process_data)

packet_handler = packet_handler_class()
set_packet_handler(packet_handler.process_data)

chicken_handler = chicken_handler_class()
packet_handler.add_byte_handler(chicken_handler.process_bytes)

town_portal_handler = town_portal_class()
packet_handler.add_byte_handler(town_portal_handler.process_bytes)
command_handler.town_portal_handler = town_portal_handler

keyboard_handler = keyboard_handler_class()
set_keyboard_handler(keyboard_handler.process_key)

configuration.town_portal_handler = town_portal_handler