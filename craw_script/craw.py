from automap import *
from packets import *

automap_handler = automap_handler_class()
craw.set_automap_handler(automap_handler.process_data)

packet_handler = packet_handler_class()
craw.set_packet_handler(packet_handler.process_data)