from craw import send_packet, get_player_ids
from utility import pack_bytes

def hostile_player(id):
	packet = '\x5d\x04\x01' + pack_bytes(id, 4)
	send_packet(packet)
	
def hostile_players():
	print 'Declaring hostility to all players'
	player_ids = get_player_ids()
	for id in player_ids:
		hostile_player(player_ids)
		