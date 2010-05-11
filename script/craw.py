#Nasty hack to import stuff from ../configuration
import sys, utility

sys.path.append(utility.get_configuration_directory())

import configuration, craw, automap, packets, chicken, command, town_portal, player_kill, skills, bncs, item_handler, follow, enchant, battle_orders, party, auto_cast, experience, bind, player_killer, summoner

#Set up handlers

packet_handler = packets.packet_handler_class()
craw.set_packet_handler(packet_handler.process_data)

bind_handler = bind.bind_handler_class()

command_handler = command.command_handler_class(bind_handler)
craw.set_command_handler(command_handler.process_command)

automap_handler = automap.automap_handler_class()
craw.set_automap_handler(automap_handler.process_data)

chicken_handler = chicken.chicken_handler_class()
packet_handler.add_byte_handler(chicken_handler.process_bytes)

town_portal_handler = town_portal.town_portal_class()
packet_handler.add_byte_handler(town_portal_handler.process_bytes)
command_handler.town_portal_handler = town_portal_handler

player_kill_handler = player_kill.player_kill_handler_class()
packet_handler.add_byte_handler(player_kill_handler.process_bytes)

skill_handler = skills.skill_handler_class()
packet_handler.add_byte_handler(skill_handler.process_bytes)
command_handler.skill_handler = skill_handler

bncs_packet_handler = bncs.bncs_packet_handler_class()
packet_handler.add_byte_handler(bncs_packet_handler.process_bytes)
craw.set_bncs_packet_handler(bncs_packet_handler.process_packet)
player_kill_handler.bncs_handler = bncs_packet_handler
command_handler.bncs_handler = bncs_packet_handler

item_handler = item_handler.item_handler_class()
craw.set_item_handler(item_handler.process_item)
packet_handler.add_byte_handler(item_handler.process_bytes)

follow_handler = follow.follow_handler_class(town_portal_handler)
packet_handler.add_byte_handler(follow_handler.process_bytes)

enchant_handler = enchant.enchant_handler_class()
packet_handler.add_byte_handler(enchant_handler.process_bytes)

battle_orders_handler = battle_orders.battle_orders_handler_class()
packet_handler.add_byte_handler(battle_orders_handler.process_bytes)

party_handler = party.party_handler_class()
packet_handler.add_byte_handler(party_handler.process_bytes)

auto_cast_handler = auto_cast.auto_cast_handler_class()
packet_handler.add_byte_handler(auto_cast_handler.process_bytes)

experience_handler = experience.experience_handler_class()
packet_handler.add_byte_handler(experience_handler.process_bytes)

player_killer_handler = player_killer.player_killer_class(town_portal_handler)
bind_handler.player_killer_handler = player_killer_handler
packet_handler.add_byte_handler(player_killer_handler.process_bytes)
chicken_handler.add_damage_handler(player_killer_handler.process_damage)
chicken_handler.add_hostility_handler(player_killer_handler.process_hostility)

summoner_handler = summoner.summoner_handler_class()
packet_handler.add_byte_handler(summoner_handler.process_bytes)