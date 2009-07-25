import item_handler, types
from item_groups import item_groups

class item_rule:
	def __init__(self, item_type = None, group = None, quality = None, ethereal = None, sockets = None):
		quality = item_rule.listify(quality)
		sockets = item_rule.listify(sockets)
		self.item_type = item_type
		self.group = group
		self.quality = quality
		self.ethereal = ethereal
		self.sockets = sockets
		
		self.debug_mode = False
		
	@staticmethod
	def listify(input):
		if type(input) == types.IntType:
			return [input]
		else:
			return input
			
	def debug(self, text):
		if self.debug_mode:
			print text
		
	def applies_to(self, item):
		if self.group != None:
			try:
				item_types = item_groups[self.group]
			except KeyError:
				print 'Unable to find item group "%s"' % self.group
				return False
			if item.type not in item_types:
				self.debug('Group rejection for %s: %s' % (item.type, self.group))
				return False
		
		elif self.item_type != None and self.item_type != item.type:
			self.debug('Type rejection: %s != %s' % (self.item_type, item.type))
			return False
			
		if self.quality != None and item.quality not in self.quality:
			self.debug('Quality rejection: %s not in %s' % (item.quality, self.quality))
			return False
			
		elif self.ethereal != None and self.ethereal != item.ethereal:
			self.debug('Ethereal rejection: %s != %s' % (self.ethereal, item.ethereal))
			return False
			
		elif self.sockets != None and item.sockets not in self.sockets:
			self.debug('Socket rejection: %d not in %s' % item.sockets, self.sockets)
			return False
			
		self.debug('Item rule success')
			
		return True