import craw, utility

def hostile_player(id):
	packet = '\x5d\x04\x01' + utility.pack_bytes(id, 4)
	craw.send_packet(packet)
	
def hostile_players():
	print 'Declaring hostility to all players'
	player_ids = craw.get_player_ids()
	for id in player_ids:
		hostile_player(player_ids)
		