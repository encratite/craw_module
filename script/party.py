import packets, privileges, configuration, utility

class party_handler_class:
	def __init__(self):
		self.new_game = False
		
	def process_command(self, name, message):
		if not privileges.is_friend(name):
			return
			
		player = utility.get_player_data_by_name(name)
		if player == None:
			print 'Unable to retrieve player data of %s!' % name
			return
			
		if message == configuration.party_invitation_command:
			print 'Inviting %s' % name
			packets.invite_player(player.id)

	def process_bytes(self, bytes):
		if bytes[0] == 0x01:
			self.new_game = True
			return
			
		assignment = packets.parse_assignment(bytes)
		if assignment != None:
			player_id, character_class, player_name, x, y = assignment
			if self.new_game:
				self.new_game = False
				self.my_id = player_id
			else:
				if configuration.auto_loot_permission_to_friends and privileges.is_friend(player_name):
					print 'Granting loot permission to friend %s' % player_name
					packets.grant_loot_permission(player_id)
				
		
		message = packets.parse_message(bytes)
		if message != None:
			name, message = message
			my_name = utility.get_my_name()
			if name != my_name:
				self.process_command(name, message)
			
		player_id = packets.parse_invitiation(bytes)
		if player_id != None:
			player = utility.get_player_by_id(player_id)
			if configuration.auto_accept_invitations_by_friends and privileges.is_friend(player.name):
				print 'Accepting the invitation of friend %s' % player.name
				packets.accept_invitation(player_id)
				
		"""
		join = packets.parse_join(bytes)
		if join != None:
			my_player = utility.get_my_player()
			if my_player == None:
				return
			player_id, player_name, character_class, level = join
			if configuration.auto_loot_permission_to_friends and self.my_id != player_id and privileges.is_friend(player_name):
				print 'Granting loot permission to friend %s, who just joined the game' % player_name
				packets.grant_loot_permission(player_id)
		"""