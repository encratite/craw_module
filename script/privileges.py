import configuration, utility

def has_remote_privileges(name):
	my_name = utility.get_my_name()
	if name == my_name:
		return False
		
	return configuration.remote_command_privilege_users == None or name in configuration.remote_command_privilege_users
	
def is_friend(name):
	return name in configuration.friends