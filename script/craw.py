from craw import set_automap_handler, set_packet_handler
from automap import automap_handler_class
from packets import packet_handler_class
from chicken import chicken_handler_class

automap_handler = automap_handler_class()
set_automap_handler(automap_handler.process_data)

packet_handler = packet_handler_class()
set_packet_handler(packet_handler.process_data)

chicken_handler = chicken_handler_class()
packet_handler.add_byte_handler(chicken_handler.process_bytes)