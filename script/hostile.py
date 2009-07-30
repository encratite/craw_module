import craw, utility

def hostile_player(id):
	packet = '\x5d\x04\x01' + utility.pack_number(id, 4)
	return packet
	
def hostile_players():
	print 'Declaring hostility to all players'
	players = craw.get_players()
	my_id = craw.get_player_id()
	players = filter(lambda player: player.id != my_id and player.level >= 9, players)
	packet = ''
	for player in players:
		packet += hostile_player(player.id)
		
	craw.send_packet(packet)