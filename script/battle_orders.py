import craw, threading, utility, nil.thread, configuration, privileges, packets

battle_orders_skills = [
	('Battle Command', 0x9b),
	('Battle Orders', 0x95),
	('Shout', 0x8a)
]

class battle_orders_handler_class:
	def __init__(self):
		self.running = False
		self.lock = threading.Lock()
		
	def process_command(self, name, message):
		if message == configuration.battle_orders_command:
			self.cast_battle_orders()
		
	def process_bytes(self, bytes):
		self.lock.acquire()
		running = self.running
		self.lock.release()
		
		if running:
			return
			
		my_name = utility.get_my_name()
		if my_name not in configuration.battle_orders_barbarians:
			return
			
		message = packets.parse_message(bytes)
		if message != None:
			name, message = message
			if privileges.has_remote_privileges(name):
				self.process_command(name, message)
				
	def cast_battle_orders(self):
		if utility.town_check():
			packets.send_chat(configuration.battle_orders_town_error)
			return
			
		current_mana, maximum_mana = craw.get_mana()
		mana_usage = 6 + 7 + 11
		if current_mana < mana_usage:
			packets.send_chat(configuration.mana_error % (current_mana, maximum_mana))
			return
			
		for skill_name, skill in battle_orders_skills:
			level = craw.get_skill_level(skill)
			if level == 0:
				print 'This character lacks the following skill: %s' % skill_name
				return
			
		self.launch_function(self.battle_orders_thread)
			
	def launch_function(self, function):
		self.lock.acquire()
		if self.running:
			print 'Received a Battle Orders request that was blocked because the thread is still running.'
		else:
			self.running = True
			nil.thread.create_thread(lambda: self.function_wrapper(function))
		self.lock.release()
		
	def thread_termination(self):
		self.lock.acquire()
		self.running = False
		self.lock.release()
		
	def function_wrapper(self, function):
		function()
		self.thread_termination()
		
	def battle_orders_thread(self):
		packets.send_chat(configuration.battle_orders_confirmation)
		for skill_name, skill in battle_orders_skills:
			packets.perform_cast(skill, configuration.battle_orders_switch_delay, configuration.battle_orders_cast_delay)