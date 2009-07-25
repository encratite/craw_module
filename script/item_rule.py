import item_handler, types

class item_rule:
	def __init__(self, item_type, group, quality, ethereal, sockets):
		quality = item_rule.listify(quality)
		sockets = item_rule.listify(sockets)
		self.item_type = item_type
		self.group = group
		self.quality = quality
		self.ethereal = ethereal
		self.sockets = sockets
		
	@staticmethod
	def listify(input):
		if type(input) == types.IntType:
			return [input]
		else:
			return input
		
	def applies_to(self, item):
		if self.group != None:
			try:
				item_types = item_groups[self.group]
			except KeyError:
				print 'Unable to find item group "%s"' % self.group
				return False
			if item.type not in item_types:
				print 'Group rejection for %s' % item_type
				return False
		
		elif self.item_type != None and self.item_type != item.type:
			print 'Type rejection: %s != %s' % (self.item_type, item.type)
			return False
			
		if self.quality != None and item.quality not in self.quality:
			print 'Quality rejection: %s not in %s' % (item.quality, self.quality)
			return False
			
		elif self.ethereal != None and self.ethereal != item.ethereal:
			print 'Ethereal rejection: %s != %s' % (self.ethereal, item.ethereal)
			return False
			
		elif self.sockets != None and item.sockets not in self.sockets:
			print 'Socket rejection: %d not in %s' % item.sockets, self.sockets
			return False
			
		return True 

def item(item_type = None, group = None, quality = None, ethereal = None, sockets = None):
	new_item_rule = item_rule(item_type, group, quality, ethereal, sockets)
	item_handler.item_rules.append(new_item_rule)