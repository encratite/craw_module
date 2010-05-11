import nil.directory, sys, os

module = 'craw'
boost = ARGUMENTS.get('boost')
boost_lib = ARGUMENTS.get('boost_lib')
ail = ARGUMENTS.get('ail')
ail_lib = ARGUMENTS.get('ail_lib')
python = ARGUMENTS.get('python')

dependencies = [boost, ail, boost_lib, ail_lib, python]

if len(filter(lambda x: x == None, dependencies)) > 0:
	print 'This module requires boost (http://www.boost.org/), ail (http://repo.or.cz/w/ail.git) and Python 2.6 (32-bit version, http://www.python.org/) so you will have to specify the paths in the scons arguments:'
	print 'scons boost=<boost header directory> boost_lib=<boost binary directory> ail=<ail header directory> ail_lib=<path to ail.lib> python=<path to your Python installation>'
	sys.exit(1)
	
python_library = os.path.join(python, 'libs', 'python26.lib')
python_include = os.path.join(python, 'include')
	
include_directories = [boost, ail, python_include]

defines = {
	'_CRT_SECURE_NO_WARNINGS': 1,
	'BOOST_LIB_DIAGNOSTIC': 1
}

"""
flags = [
	'/O2',
	'/Oi',
	'/GL',
	#'/FD',
	'/MD',
	'/Gy',
	'/W3',
	'/nologo',
	'/c',
	#'/Zi',
	'/EHsc'
]
"""

flags = [
	'/EHsc'
]

relative_source_directory = os.path.join('..', module)

source_files = map(lambda path: os.path.basename(path), nil.directory.get_files_by_extension(relative_source_directory, 'cpp'))

include_directories += [module]

cpus = int(os.environ.get('NUMBER_OF_PROCESSORS', 2))

thread_string = 'thread'
if cpus > 1:
	thread_string += 's'
print 'Compiling project with %d %s' % (cpus, thread_string)

environment = Environment(CPPPATH = include_directories, CCFLAGS = flags, LIBPATH = boost_lib, LIBS = [ail_lib, python_library, 'user32.lib', 'kernel32.lib'], CPPDEFINES = defines)
environment.SetOption('num_jobs', cpus)
environment.SharedLibrary(module, source_files)