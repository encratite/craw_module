import subprocess, os, sys, urllib2, platform, shutil, time, winsound, _winreg

#All the external sources are defined here:

git_url = 'http://msysgit.googlecode.com/files/Git-1.6.3.2-preview20090608.exe'
scons_url = 'http://surfnet.dl.sourceforge.net/sourceforge/scons/scons-1.2.0.d20090223.win32.exe'
python_url = 'http://www.python.org/ftp/python/2.6.2/python-2.6.2.msi'

boost_git = 'git://gitorious.org/boost/svn.git'

nil_git = 'git://repo.or.cz/nil.git'
ail_git = 'git://repo.or.cz/ail.git'
syringe_git = 'git://repo.or.cz/syringe.git'
craw_git = 'git://repo.or.cz/craw.git'

#Functions used by the setup:

def is_64_bit():
	return platform.architecture()[0] == '64bit'

def download(url):
	try:
		print 'Downloading %s' % url
		request = urllib2.Request(url)
		response = urllib2.urlopen(request)
		data = response.read()
		return data
	except:
		return None
		
def download_file(url, download_path):
	data = download(url)
	if data == None:
		print 'Failed to download %s' % url
		sys.exit(1)
	file = open(download_path, 'w+b')
	file.write(data)
	file.close()

def has_command(command):
	return len(os.popen(command).read()) > 0

def has_git():
	return has_command('%s --help' % git_path)
	
def has_scons():
	return has_command('%s --help' % scons_path)
		
def has_bjam():
	return has_command('boost\\bjam.exe --help')
	
def string_filter(filter_string, lines):
	return filter(lambda line: line.find(filter_string) != -1, lines)
	
def make_directory(directory):
	try:
		os.mkdir(directory)
	except:
		pass
		
def copy_binary(source):
	try:
		target_directory = 'binaries'
		make_directory(target_directory)
		destination = os.path.join(target_directory, os.path.basename(source))
		shutil.copy(source, destination)
	except:
		pass
	
def get_site_packages():
	target = 'site-packages'
	for path in sys.path:
		if len(path) > len(target) and path[- len(target) : ] == target:
			return path
	return None
	
def get_wow_node():
	wow = ''
	if is_64_bit():
		wow = 'Wow6432Node\\'
	return wow
	
def get_git_path():
	try:
		key = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, 'SOFTWARE\\%sMicrosoft\\Windows\\CurrentVersion\\Uninstall\\Git_is1' % get_wow_node())
		value, type = _winreg.QueryValueEx(key, 'InstallLocation')
		return '"%s"' % os.path.join(value, 'bin', 'git.exe')
	except:
		return None
		
def get_scons_path():
	target = 'lib'
	for path in sys.path:
		if os.path.basename(path) == target:
			return os.path.join(path[0 : - len(target)], 'Scripts', 'scons.bat')
	return None
	
def get_python_path():
	try:
		key = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, 'SOFTWARE\\%sPython\\PythonCore\\2.6\\InstallPath' % get_wow_node())
		value, type = _winreg.QueryValueEx(key, '')
		return value
	except:
		return None
	
def execute(command):
	print 'Executing [%s] in directory %s' % (command, os.getcwd())
	return os.system(command)
	
def clone(url, diretory):
	command = '%s clone %s %s' % (git_path, url, diretory)
	execute(command)
	
def pull(directory):
	os.chdir(directory)
	command = '%s pull' % git_path
	execute(command)
	os.chdir('..')
	
def git(url, directory):
	clone(url, directory)
	pull(directory)
	
def vcvars():
	try:
		key = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, 'SOFTWARE\\%sMicrosoft\\VisualStudio\\9.0\\Setup' % get_wow_node())
		value, type = _winreg.QueryValueEx(key, 'Dbghelp_path')
		path = value + '..\\..\\VC\\bin\\vcvars32.bat'
		execute('"%s"' % path)
	except:
		print 'Unable to locate the Visual C++ 9.0 installation to execute vcvars32.bat!'
		sys.exit(1)
	
def setup_git():
	global git_path
	git_path = 'git'
	if has_git():
		return
		
	git_path = get_git_path()
	if git_path != None:
		return
	
	print 'Setting up msysgit'
	target_directory = 'git'
	url = git_url
	path = os.path.join(target_directory, os.path.basename(url))
	make_directory(target_directory)
	download_file(url, path)
	print 'Successfully downloaded the msysgit setup to %s, running it' % path
	execute(path)
	print 'Setup has finished'
	
	git_path = get_git_path()
	if git_path == None:
		print 'Fatal error: I was unable to detect msysgit in the registry'
		sys.exit(1)
		
def brutal_removal(target):
	execute('rmdir /q /s %s' % target)
		
def setup_nil():
	print 'Setting up the nil general purpose library for Python'
	target = 'nil'
	git(nil_git, target)
	path = os.path.join(get_site_packages(), target)
	print 'Creating a symbolic link to the library at %s' % path
	source = '%s\\%s' % (target, target)
	if execute('mklink /D  %s %s' % (path, source)) != 0:
		print 'Failed to create symbolic link, this is probably not Vista/Windows 7'
		print 'Deleting the old library and moving the new one there'
		try:
			print 'Removing old entry %s' % path
			brutal_removal(target)
			brutal_removal(path)
			git(nil_git, target)
		except:
			pass
		print 'Moving %s to %s' % (source, path)
		try:
			shutil.move(source, path)
		except:
			print 'Failed to install nil library!'
			sys.exit(1)
	
def setup_scons():
	global scons_path
	scons_path = get_scons_path()
	if scons_path == False:
		print 'Unable to get the Python library directory to determine the SCons path'
		sys.exit(1)
		
	if has_scons():
		return
		
	print 'Setting up the SCons building system (written in Python)'
	target_directory = 'scons'
	url = scons_url
	path = os.path.join(target_directory, os.path.basename(url))
	make_directory(target_directory)
	download_file(url, path)
	print 'Successfully downloaded the SCons setup to %s, running it' % path
	execute(path)
	print 'Setup has finished'
		
def setup_boost():
	print 'Setting up the mighty boost C++ library'
	target_directory = 'boost'
	
	git(boost_git, target_directory)
	
	os.chdir(target_directory)
	
	bjam_binary = 'bjam.exe'
	
	vcvars()
	
	if not os.path.exists(bjam_binary):
		print 'Compiling bjam'
		execute('bootstrap.bat')
		if not os.path.exists(bjam_binary):
			sys.exit('Failed to build bjam.')
			#download_file(prebuilt_bjam_url, bjam_binary)
	
	print 'Compiling boost'
	cpus = int(os.environ.get('NUMBER_OF_PROCESSORS', 2))
	print 'Number of CPUs detected: %d' % cpus
	execute('%s toolset=%s variant=release threading=multi runtime-link=static link=static stage -j %d --without-graph --without-graph_parallel --without-iostreams --without-math --without-mpi --without-python --without-program_options --without-python --without-regex --without-serialization --without-signals --without-test --without-wave' % (bjam_binary, toolset, cpus))
	os.chdir('..')
	
def setup_ail():
	print 'Setting up the ail C++ general purpose library'
	target_directory = 'ail'
	git(ail_git, target_directory)
	os.chdir(target_directory)
	execute('%s boost=..\\..\\boost' % scons_path)
	os.chdir('..')
	
def setup_syringe():
	print 'Setting up the syringe injection tool'
	
	target = 'syringe'
	git(syringe_git, target)
	os.chdir(target)
	execute('%s boost=..\\..\\boost ail=..\\..\\ail ail_lib=..\\..\\ail\\build\\ail.lib' % scons_path)
	os.chdir('..')
	copy_binary('%s\\build\\%s.exe' % (target, target))
	
def setup_python():
	print 'Setting up the 32-bit version of Python (the 64-bit library/DLL is of no use to us because Diablo II is 32-bit)'
	
	target_directory = 'python'
	url = python
	base = os.path.basename(url)
	path = os.path.join(target_directory, base)
	make_directory(target_directory)
	download_file(url, path)
	print 'Successfully downloaded the Python 32-bit setup to %s, attempting to perform an unattended MSI installation' % path
	execute('msiexec /package %s\\%s /passive' % (target_directory, base))
	print 'Setup has finished'
	
def setup_craw():
	print 'Setting up the Charles Romley Alder Wright (short C.R.A.W.) Diablo II module'
	
	target = 'craw'
	git(craw_git, target)
	
	python_path = get_python_path()
	if python_path == False:
		setup_python()
		python_path = get_python_path()
		if python_path == False:
			print 'Unable to detect the 32-bit installation of Python 2.6'
			sys.exit(1)
	
	os.chdir(target)
	execute('%s boost=..\\..\\boost boost_lib=..\\..\\boost\\stage\\lib ail=..\\..\\ail ail_lib=..\\ail\\build\\ail.lib python=%s' % (scons_path, python_path))
	os.chdir('..')
	copy_binary('%s\\build\\%s.dll' % (target, target))
	
toolset = 'msvc-9.0'

start = time.time()

setup_git()
setup_nil()
setup_scons()
setup_boost()
setup_ail()
setup_syringe()
setup_craw()

seconds_per_minute = 60
seconds_passed = int(time.time() - start)
print 'Setup finished after %d:%02d minute(s)' % (seconds_passed / seconds_per_minute, seconds_passed % seconds_per_minute)
winsound.MessageBeep()