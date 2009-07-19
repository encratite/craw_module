#Nasty hack to import stuff from ../configuration
import sys, utility

sys.path.append(utility.get_configuration_directory())

import configuration, craw, automap, packets, chicken, command, town_portal, keyboard

#Set up handlers

command_handler = command.command_handler_class()
craw.set_command_handler(command_handler.process_command)

automap_handler = automap.automap_handler_class()
craw.set_automap_handler(automap_handler.process_data)

packet_handler = packets.packet_handler_class()
craw.set_packet_handler(packet_handler.process_data)

chicken_handler = chicken.chicken_handler_class()
packet_handler.add_byte_handler(chicken_handler.process_bytes)

town_portal_handler = town_portal.town_portal_class()
packet_handler.add_byte_handler(town_portal_handler.process_bytes)
command_handler.town_portal_handler = town_portal_handler