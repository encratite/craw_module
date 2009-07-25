import types, string
from item_constants import *
from item_configuration import item_rules

class item_handler_class:
	def __init__(self):
		self.is_drop = False
		
	def process_bytes(self, bytes):
		if len(bytes) < 2:
			return
			
		self.is_drop = (bytes[0 : 2] == [0x9c, 0x00])
				
	def process_item(self, item):
		if self.is_drop:
			self.process_drop(item)
			
	def dump_item(item):
		print 'Name: %s' % item.type
		print 'Code: %s' % item.code
		print 'ID: %08x' % item.id
		print 'Quality: %d' % item.quality
		print 'Level: %d' % item.level
		print 'Ethereal: %s' % item.ethereal
		print 'Sockets: %d' % item.sockets
		
	@staticmethod
	def get_quality_string(quality):
		qualities = {
			inferior: 'Inferior',
			normal: 'Normal',
			superior: 'Superior',
			magical: 'Magical',
			set: 'Set',
			rare: 'Rare',
			unique: 'Unique',
			crafted: 'Crafted'
		}
		
		try:
			return qualities[quality]
		except KeyError:
			return 'Unknown'
		
	def print_item(self, item):
		quality_string = item_handler_class.get_quality_string(item.quality)
		description = '%s %s' % (quality_string, item.type)
		if item.ethereal:
			description = 'Ethereal %s' % description
			
		additional_information = []
		if item.level > 1:
			additional_information.append('Level %d' % item.level)
			
		if item.sockets == 1:
			additional_information.append('1 socket')
			
		elif item.sockets > 1:
			additional_information.append('%d sockets' % item.sockets)
			
		if len(additional_information) > 0:
			description = '%s (%s)' % (description, string.join(additional_information, ', '))
			
		print description
	
	def process_drop(self, item):
		for rule in item_rules:
			if rule.applies_to(item):
				self.print_item(item)
				break