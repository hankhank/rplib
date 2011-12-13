import os
import sys

# Repost Build Environment
if sys.platform == 'win32':
    platform = 'win'
elif sys.platform == 'darwin':
    platform = 'darwin'
else:
    platform = 'linux'

env = Environment(
    ROOT = Dir('.').abspath,
    PLATFORM = platform,
)

# optional flags
if int(ARGUMENTS.get('debug', 0)):
    env.Append(CCFLAGS='-DDEBUG_ON -DRP_STRIP_LOG=0')
    if (platform =='win'):
      env.Append(CCFLAGS=['/Zi'],
          LINKFLAGS=['/INCREMENTAL:NO','/DEBUG','/OPT:REF','/OPT:ICF'])
    else:
      env.Append(CCFLAGS='-g')
else:
    env.Append(CCFLAGS='-DRP_STRIP_LOG=1')

if int(ARGUMENTS.get('localpath',0)):
    env.AppendENVPath('PATH',os.environ['PATH'])

# OS flags
if (platform =='darwin'):
    env.Append(CCFLAGS="-DOSX=1 -arch i386  -O2", LINKFLAGS = "-arch i386 ")
    env.AppendENVPath('PATH',os.environ['PATH'])
elif platform =='linux':
    env.Append(CCFLAGS="-DLINUX")
elif platform =='win':
    env.Append(CCFLAGS=['/DWIN32=1','/D_X86_','/EHsc']) 

Export('env')

# Library Dependencies
deplibs = '''
pthread
'''

objs = env.SharedObject([Glob('*.cpp')])
lib = env.StaticLibrary('rpcore', objs)

# Unit tests
test = env.Program("rpcore_test",Glob("test/*.cpp"),CPPPATH=["."], LIBS=[lib,Split(deplibs)])

Alias('test',test)
Return('lib')
