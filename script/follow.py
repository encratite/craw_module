import configuration, utility, packets, craw

def follow_handler_class:
	def __init__(self):
		self.following = False
		
	def process_command(self, name, message):
		my_name = utility.get_my_name()
		player = utility.get_player_by_name(name)
		if player == None:
			print 'Unable to retrieve player data of player %s' % player
			return
			
		if (message == configuration.all_follow_command) or (message == configuration.follow_command % my_name):
			self.following = True
			self.leader = player.name
			self.leader_id = player.id
			print 'Following %s' % self.leader
			packets.send_chat(configuration.follow_confirmation % self.leader)
			
		elif (message == configuration.all_stop_command) or (message == configuration.stop_command % my_name):
			if self.following:
				self.following = False
				print 'No longer following %s' % self.leader
				packets.send_chat(configuration.following_confirmation % self.leader)
			else:
				print '%s asked us to stop following the leader but we are currently not following anybody' % name
				packets.send_chat(configuration.stop_error)
		
	def process_bytes(self, bytes):
		message = packets.parse_message(bytes)
		if message != None:
			name, message = message
			print 'Processing message: <%s> %s' % (name, message)
			my_name = utility.get_my_name()
			if my_name not in configuration.followers or name not in configuration.follow_leaders:
				return
			
			self.process_command(name, message)
		
		move = packets.parse_move(bytes)
		if move != None:
			player_id, x, y = move
			print 'Processing move by %d' % player_id
			if self.following and player_id == self.leader_id:
				print 'Following %s to (%d, %d)' % (self.leader, x, y)
				craw.move_click(x, y)