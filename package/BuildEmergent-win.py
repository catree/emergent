# This script attempts to download and install all the tools needed
# to build emergent, checkout the source from Subversion, build it,
# and create an installer.
# NOTE: if you commit a new version of this file to svn, make sure to
# update the file on the ftp site as well

import os, re, subprocess, sys, urllib, webbrowser, _winreg
from zipfile import ZipFile

##############################################################################
# Misc functions

def print_horizontal():
  print '\n-------------------------------------------------------------------------------'

def quit(exit_val):
  os.system('color 07')
  response = raw_input('\n\nPress enter to close the window...')
  sys.exit(exit_val)

##############################################################################
# Environment variable functions

def setEnvVar(name, value, reg, env):
  try:
    key = _winreg.OpenKey(reg, env, 0, _winreg.KEY_ALL_ACCESS)
    _winreg.SetValueEx(key, name, 0, _winreg.REG_EXPAND_SZ, value)
  except WindowsError:
    pass
  _winreg.CloseKey(key)

def getEnvVar(name, reg, env):
  try:
    key = _winreg.OpenKey(reg, env, 0, _winreg.KEY_READ)
    value, type = _winreg.QueryValueEx(key, name)
  except WindowsError:
    _winreg.CloseKey(key)
    return ''
  _winreg.CloseKey(key)
  return value

def getUserEnvVar(name):
  return getEnvVar(name, _winreg.HKEY_CURRENT_USER, 'Environment')

def getSysEnvVar(name):
  env = 'SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment'
  return getEnvVar(name, _winreg.HKEY_LOCAL_MACHINE, env)

def setUserEnvVar(name, value):
  # Using 'setx' is better since it is a "live" update,
  # rather than just updating the registry and waiting
  # for the next reboot for the changes to take effect.
  #setEnvVar(name, value, _winreg.HKEY_CURRENT_USER, 'Environment')
  os.system('setx ' + name + ' "' + value + '"')

def setSysEnvVar(name, value):
  #env = 'SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment'
  #setEnvVar(name, value, _winreg.HKEY_LOCAL_MACHINE, env)
  os.system('setx ' + name + ' "' + value + '" /M')

##############################################################################
# Filesystem functions

def makeDir(dir):
  try:
    os.makedirs(dir)
  except os.error:
    pass

def fixPath(path):
  return re.sub(r'/', r'\\', path)

def quote(string):
  return '"' + string + '"'

def quoteFixPath(path):
  return quote(fixPath(path))

def fileExists(file):
  if os.path.isfile(file):
    print '  Detected ' + fixPath(file)
    return True
  return False

def dirExists(dir):
  if os.path.isdir(dir):
    print '  Detected ' + fixPath(dir)
    return True
  return False

##############################################################################
# Function to find a file in one of the 'Program Files' folders

# Determine if this is 64-bit Windows or 32-bit Windows.
try:
  progfiles = set([os.environ['ProgramFiles(x86)'], os.environ['ProgramFiles'], os.environ['ProgramW6432']])
  isWin64 = True
except KeyError:
  progfiles = set([os.environ['ProgramFiles']])
  isWin64 = False

def findInProgFiles(path):
  for prefix in progfiles:
    file = os.path.join(prefix, path)
    if fileExists(file) or dirExists(file):
      return file # True
  return False

##############################################################################
# Function to download and cache a file into the 'devtools' directory

devtool_dir = 'C:/src/devtools'
makeDir(devtool_dir)

def getUrl(url):
  try:
    file = os.path.basename(url)
    file = os.path.join(devtool_dir, file)
    if not fileExists(file):
      print 'Downloading ', url
      urllib.urlretrieve(url, file)
  except Exception:
    os.remove(file)
    print '\nERROR: could not download file from: ' + url
    print '\nAborting.'
    quit(1)
  return file

##############################################################################
# User interaction functions

def getOptions(default):
  if default == '':                      return ' [y/n/q/a]: '
  elif default == 'y' or default == 'Y': return ' [Y/n/q/a]: '
  elif default == 'n' or default == 'N': return ' [y/N/q/a]: '
  else:                                  return ' [y/n/Q/a]: '

yes_to_all = False
def process_response(response):
  global yes_to_all
  if response[0] == 'a' or response[0] == 'A':
    yes_to_all = True
    return True
  if response[0] == 'q' or response[0] == 'Q':
    print '\nQuitting at user request.'
    quit(0)
  return response[0] == 'y' or response[0] == 'Y'

def askUser(question, default='Y'):
  if yes_to_all and default == 'Y': return True
  while True:
    response = raw_input(question + getOptions(default))
    if response == '': response = default
    if response != '': break
  return process_response(response)

def get_svn_rev(question):
  while True:
    response = raw_input(question + getOptions(''))
    if response != '': break
  if process_response(response):
    return 'HEAD'
  if response[0] == 'n' or response[0] == 'N':
    return -1
  return response

##############################################################################
# Functions to install Microsoft Visual Studio Express and associated packages

def inst_msvs_2008():
  while not findInProgFiles('Microsoft Visual Studio 9.0/VC/bin/cl.exe'):
    print_horizontal()
    print 'You need to install Visual C++ 2008 Express Edition.  Make sure to get'
    print 'the 2008 edition, not 2010!  Download the installer from the website'
    print 'that will be opened, then run it.  Accept default installation options.'
    print "Don't forget to register it within 30 days!"
    print 'After installation is complete, return to this script.'
    if askUser('\nReady to download and install Visual C++?'):
      webbrowser.open_new_tab('http://go.microsoft.com/?linkid=7729279')
      response = raw_input('\nOnce Visual C++ has been installed, press enter to continue...')

# Install the base Visual C++ 2010 stuff.
def inst_msvs_2010():
  while not findInProgFiles('Microsoft Visual Studio 10.0/VC/bin/cl.exe'):
    print_horizontal()
    print 'You need to install Visual C++ 2010 Express Edition.'
    print 'Download the installer from the website that will be opened,'
    print 'then run it.  Accept default installation options.'
    print "Don't forget to register it within 30 days!"
    print 'After installation is complete, return to this script.'
    print '\nNOTE: Do NOT run Windows Update until after the Windows SDK is installed!'
    if askUser('\nReady to download and install Visual C++?'):
      webbrowser.open_new_tab('http://go.microsoft.com/?linkid=9709949')
      response = raw_input('\nOnce Visual C++ has been installed, press enter to continue...')

# Install the Win SDK, which contains the 64-bit compiler.
setenv_cmd = ''
def inst_win_sdk():
  global setenv_cmd
  while True:
    setenv_cmd = findInProgFiles('Microsoft SDKs/Windows/v7.1/bin/setenv.cmd')
    if setenv_cmd: break
    print_horizontal()
    print 'In addition to MSVC++ 2010, you also need to install the Windows SDK'
    print 'in order to build 64-bit versions of emergent.  Click the "Install Now"'
    print 'link on the website that will be opened, then run it.  Accept defaults.'
    print 'After installation is complete, return to this script.'
    print '\nNOTE: Make sure all options are checked in the tree view!'
    if askUser('\nReady to download and install Windows SDK?'):
      webbrowser.open_new_tab('http://msdn.microsoft.com/en-us/windowsserver/bb980924.aspx')
      print '\nNOTE: If there was a "Fatal error during installation" related to'
      print '"Microsoft Windows SDK for Windows 7 Compilers", rerun the install,'
      print 'but this time, clear the "Visual C++ Compilers" checkbox.'
      response = raw_input('\nOnce Windows SDK has been installed, press enter to continue...')

# Install SP1 for Visual Studio 2010, which fixes a bug related to
# 64-bit compiles with optimization (among other things surely).
def inst_msvs_2010_sp1():
  msvs2010sp1_installed_file = os.path.join(devtool_dir, 'msvs2010sp1_has_been_installed.txt')
  if not fileExists(msvs2010sp1_installed_file):
    print_horizontal()
    print 'Now you need to update VS2010 to Service Pack 1.'
    print 'Download the installer from the website that will be opened, then run it.'
    print 'It will download about 600MB of extra stuff and take a while to install.'
    print 'After installation is complete, return to this script.'
    if askUser('\nReady to download and install VS2010 SP1?'):
      webbrowser.open_new_tab('http://www.microsoft.com/download/en/details.aspx?id=23691')
      print '\nWait for VS2010 SP1 to finish downloading and installing.'
      if askUser('\nDid VS2010 SP1 install successfully?', default=''):
        with open(msvs2010sp1_installed_file, 'w') as f:
          f.write('Microsoft Visual Studio 2010 Service Pack 1 has been installed,')
          f.write('according to the user.  If that is not so, delete this file.')
        if askUser('Were you prompted to reboot?', default=''):
          print '\nPlease reboot now and then re-run this script'
          quit(0)
        else:
          print '\nContinuing to build...'
        return
    # User chose not to install, or installation failed.
    print '\nWithout VS2010 SP1, 64-bit builds may not build correctly.'
    print 'Continuing to build, but you will be prompted again next time.\n'

# Installing SP1 basically uninstalls the 64-bit compiler, so install
# the Win SDK compiler update to re-install it.
def inst_win_sdk_compiler_update():
  win_sdk_compiler_update_installed_file = os.path.join(devtool_dir, 'win_sdk_compiler_update_has_been_installed.txt')
  if not fileExists(win_sdk_compiler_update_installed_file):
    print_horizontal()
    print 'Now you need the Visual C++ 2010 SP1 Compiler Update for the Windows SDK 7.1.'
    print 'Download the installer from the website that will be opened, then run it.'
    print 'After installation is complete, return to this script.'
    if askUser('\nReady to download and install Win SDK compiler update?'):
      webbrowser.open_new_tab('http://go.microsoft.com/fwlink/?LinkId=212355')
      print '\nWait for the Win SDK compiler update to finish downloading and installing.'
      if askUser('\nDid the Win SDK compiler update install successfully?', default=''):
        with open(win_sdk_compiler_update_installed_file, 'w') as f:
          f.write('Visual C++ 2010 SP1 Compiler Update for the Windows SDK 7.1')
          f.write('has been installed, according to the user.  If not, delete this file.')
        if askUser('Were you prompted to reboot?', default=''):
          print '\nPlease reboot now and then re-run this script'
          quit(0)
        else:
          print '\nContinuing to build...'
        return
    # User chose not to install, or installation failed.
    print '\nWithout the Win SDK compiler update, building may fail.'

# After installing VC++ 2010, Win SDK, SP1, and the compiler update,
# you *still* don't have everything you need to compile a 64-bit app.
# Need to create vcvars64.bat.  It just defers to the setenv command.
amd64_dir = ''
def create_vcvars64_bat():
  global amd64_dir
  amd64_dir = findInProgFiles('Microsoft Visual Studio 10.0/VC/bin/amd64')
  if not amd64_dir:
    print_horizontal()
    print 'Could not find amd64 directory.  Two likely scenarios:'
    print '  1) The Windows SDK was not installed correctly.'
    print '  2) VS 2010 SP1 was installed, but the Windows SDK compiler update was not.'
    print '\nAborting.  Re-running this script may remedy the situation.'
    quit(1)
  vcvars64_bat = fixPath(os.path.join(amd64_dir, 'vcvars64.bat'))
  while not fileExists(vcvars64_bat):
    print_horizontal()
    print 'Need to create vcvars64.bat file, since it is missing (as expected).'
    print 'A Windows security dialog will appear -- click Yes on it.'
    if askUser('\nOK to create ' + quote(vcvars64_bat) + '?'):
      # Have to make a temp batch file to be run as admin to create the
      # batch file we actually care about.  "runas" is kinda like sudo.
      make_vcvars64_bat = os.path.join(devtool_dir, 'make_vcvars64.bat')
      with open(make_vcvars64_bat, 'w') as f:
        f.write('echo call ' + quoteFixPath(setenv_cmd) + ' /x64 > ' + quote(vcvars64_bat))
      os.startfile(make_vcvars64_bat, 'runas')
      # Without this prompt, python won't notice the file creation.
      response = raw_input('After clicking Yes on the Windows security dialog, hit enter in this window...')

def check_windows_update():
  wu_done_file = os.path.join(devtool_dir, 'windows_update_was_run_after_vs2010_install.txt')
  if not fileExists(wu_done_file):
    print_horizontal()
    if askUser('\nOK to check for updates for all the software that was just installed?'):
      os.system('wuapp')
      response = raw_input('\nCheck for and install updates in Windows Update then press enter to continue...')
      with open(wu_done_file, 'w') as f:
        f.write('Checked Windows Update after installing VC++ 2010, WinSDK, SP1, WinSDK compiler update')

# Default to VC++2010 now that it actually works.
msvs = 2010
def inst_msvs():
  global msvs
  while True:
    response = raw_input('\nBuild using Visual C++ 2008 or 2010? [%d]: ' % msvs)
    if response == 'q' or response == 'Q': quit(0)
    if response == '': break
    if response == '2008' or response == '2010':
      msvs = int(response)
      break

  if msvs == 2008:
    inst_msvs_2008()
  elif msvs == 2010:
    # http://blogs.msdn.com/b/vcblog/archive/2011/03/31/10148110.aspx
    # Our recommended install order is:
    #   1. Visual Studio 2010
    #   2. Windows SDK 7.1
    #   3. Visual Studio 2010 SP1
    #   4. Visual C++ 2010 SP1 Compiler Update for the Windows SDK 7.1
    # Note the msiexec calls on this page: http://msdn.microsoft.com/en-us/visualc/gg697159
    inst_msvs_2010()
    inst_win_sdk()
    inst_msvs_2010_sp1()
    inst_win_sdk_compiler_update()
    check_windows_update()
    create_vcvars64_bat()
  else:
    raise 'Bad MSVS version'

##############################################################################
# Function to install Nullsoft Install System

def inst_nsis():
  while not findInProgFiles('NSIS/NSIS.exe'):
    print_horizontal()
    print 'You need to install NSIS.  A website will be opened that will prompt'
    print 'you to save a file.  Save it and run it to install NSIS.  Accept default'
    print 'installation options.  After installation is complete, return to this script.'
    if askUser('\nReady to download and install NSIS?'):
      webbrowser.open_new_tab('http://prdownloads.sourceforge.net/nsis/nsis-2.46-setup.exe?download')
      response = raw_input('\nOnce NSIS has been installed, press enter to continue...')

##############################################################################
# Function to install Subversion

svnclient = ''
def inst_svn():
  global svnclient
  while True:
    svnclient = findInProgFiles('SlikSvn/bin/svn.exe')
    if svnclient: break
    print_horizontal()
    print 'You need to install a Subversion client (svn).  This can be downloaded'
    print 'for you to install.'
    if askUser('\nReady to download and install Subversion?'):
      print '\nDownloading Slik Subversion client.  When the download completes,'
      print 'installation will begin.  Accept default installation options.'
      print 'Return to this script when complete.'
      url = 'http://www.sliksvn.com/pub/Slik-Subversion-1.7.5-win32.msi'
      if isWin64:
        url = 'http://www.sliksvn.com/pub/Slik-Subversion-1.7.5-x64.msi'
      file = getUrl(url)
      os.system(file)
      response = raw_input('\nOnce Subversion has been installed, press enter to continue...')

##############################################################################
# Function to install CMake

cmake_exe = ''
def inst_cmake():
  global cmake_exe
  while True:
    cmake_exe = findInProgFiles('CMake 2.8/bin/cmake.exe')
    if cmake_exe: break
    print_horizontal()
    print 'You need to install CMake.  This can be downloaded for you to install.'
    if askUser('\nReady to download and install CMake?'):
      print '\nDownloading CMake.  When the download completes, installation will begin.'
      print 'Accept default installation options.  Return to this script when complete.'

      # Need (at least) CMake 2.8.4 to fix bug calling moc when using VC++2010.
      file = getUrl('http://www.cmake.org/files/v2.8/cmake-2.8.8-win32-x86.exe')
      os.system(file)
      response = raw_input('\nOnce CMake has been installed, press enter to continue...')

##############################################################################
# Function to install JOM (an nmake clone that supports parallel compiling)

use_jom = False
jom_exe = ''
jom_dir = os.path.join(devtool_dir, 'jom')
def inst_jom():
  global use_jom, jom_exe
  jom_exe = os.path.join(jom_dir, 'jom.exe')
  if fileExists(jom_exe):
    use_jom = True
  else:
    print_horizontal()
    print 'You should install Jom.  This can be downloaded and installed for you.'
    if askUser('\nReady to download and install Jom?'):
      print '\nDownloading Jom...'
      file = getUrl('ftp://ftp.qt.nokia.com/jom/jom.zip')
      zip = ZipFile(file)
      zip.extractall(jom_dir)
    if fileExists(jom_exe):
      use_jom = True

##############################################################################
# Functions to determine which emergent directory to build in

# Check if current directory is an emergent source directory.
# If so, try to build there.  Otherwise, use this default:
emer_src = fixPath('C:/src/emergent')
def is_emer_src_path(path):
  global emer_src
  # Don't bulid in a temp directory.
  if path.find('\\Temp\\') > 0:
    return False
  # If the directory name contains 'emergent', grab everything up to the first
  # slash following that 'emergent' (e.g., C:/src/emergent-foo/)
  loc = path.find('emergent')
  if loc == -1:
    return False
  slash = path.find('\\', loc)
  if slash == -1: slash = path.find('/', loc)
  if slash > 0: path = path[:slash]
  # If non-standard directory name, double-check with user.
  if path != emer_src:
    if askUser('Build in this emergent source directory: ' + path + ' ?'):
      emer_src = path
  return True

def get_emer_src_path():
  print_horizontal()
  if is_emer_src_path(os.getcwd()) or is_emer_src_path(sys.argv[0]):
    pass
  print '\nWill build in emergent source directory: ' + emer_src

##############################################################################
# Function to prompt user to build Debug or Release

build_debug = False
def ask_debug_or_release():
  global build_debug
  print_horizontal()
  while True:
    if build_debug:
      os.system('color 0b')
      if askUser('\nMake DEBUG build? (say no for release)'): break
      else: build_debug = False
    else:
      os.system('color 07')
      if askUser('\nMake RELEASE build? (say no for debug)'): break
      else: build_debug = True
  # Debug and Release versions can share the same source files, so no need for this:
  #if build_debug:
  # emer_src += '-dbg'

##############################################################################
# Function to prompt user to build 32-bit or 64-bit

build_64bit = True
def ask_32_or_64():
  global build_64bit
  print_horizontal()
  if msvs == 2008:
    build_64bit = False
    print '\nMSVS 2008 only supports 32-bit builds.'
  else:
    while True:
      if build_64bit:
        if askUser('\nMake 64-bit build? (say no for 32)'): break
        else: build_64bit = False
      else:
        if askUser('\nMake 32-bit build? (say no for 64)'): break
        else: build_64bit = True

##############################################################################
# Function to prompt user for verbose compile output

verbose_build = False
def ask_verbose():
  global verbose_build
  print_horizontal()
  if askUser('\nVerbose compile?', default='N'):
    verbose_build = True

##############################################################################
# Functions to get directory extension string, such as '-msvs2008-32-dbg'

# Get the extension to add to directories to differentiate
# VC++2008 vs VC++2010, 32-bit vs 64-bit, and debug vs release.
def get_compiler_extension_args(is_64, is_dbg):
  ext = '-msvs' + str(msvs)
  ext += '-64' if is_64 else '-32'
  if is_dbg: ext += '-dbg'
  return ext

def get_compiler_extension():
  return get_compiler_extension_args(build_64bit, build_debug)

##############################################################################
# Functions to download (or update) emergent source from Subversion

def get_tag_or_branch():
  # Get the tag or branch to build on (defaults to 'trunk')
  if askUser('\nGet source from trunk? (say no for tag/branch build)'):
    tag_or_branch = 'trunk'
  else:
    tob = raw_input('\nGet source from tag or branch? [branch]: ')
    if tob == '' or tob[0] == 'b':
      tob = 'branch'
      tobs = 'branches'
    else:
      tob = 'tag'
      tobs = 'tags'
    tag_or_branch = raw_input('\nEnter the emergent ' + tob + ' to use: ')
    tag_or_branch = tobs + '/' + tag_or_branch
  return tag_or_branch

def inst_emer_src():
  root_emer_url = 'http://grey.colorado.edu/svn/emergent/emergent/'
  print_horizontal()
  if not dirExists(os.path.join(emer_src, '.svn')):
    print 'You need to checkout the emergent source code from subversion.'
    print 'This can be done for you.'
    tag_or_branch = get_tag_or_branch()
    svn_rev = get_svn_rev('\nGet the latest emergent source from Subversion? (or enter svn rev)')
    if (svn_rev <= 0):
      print "Can't continue because no source available!  Quitting."
      quit(1)
    username = raw_input('\nEnter your username (blank for anonymous) [anonymous]: ')

    cmd = quote(svnclient) + ' checkout -r ' + svn_rev
    if username == '' or username == 'anonymous':
      cmd += ' --username anonymous --password emergent'
    else:
      cmd += ' --username ' + username
    cmd += ' ' + root_emer_url + tag_or_branch + ' ' + emer_src

    print '\nChecking out emergent source code...'
    print cmd
    makeDir(emer_src)
    os.system(cmd)

  else:
    # Found .svn folder, check which branch/tag/trunk and make sure it's up to date.
    svn_info = subprocess.check_output([svnclient, 'info', emer_src])
    lines = [line for line in svn_info.splitlines()
                    if line[:3] == "URL" or line[:3] == "Rev"]
    if len(lines) > 0:
      print '\nFound emergent source directory: '
      for line in lines:
        print '  ' + line

    print '\nYou can change to a different branch and/or update to another revision.'
    if not askUser('Build on current branch?'):
      tag_or_branch = get_tag_or_branch()
      cmd = quote(svnclient) + ' switch ' + root_emer_url + tag_or_branch + ' ' + emer_src
      print '\nSwitching branches...'
      print cmd
      os.system(cmd)

    svn_rev = get_svn_rev('\nGet the latest emergent source from Subversion? (or enter svn rev)')
    if (svn_rev > 0):
      cmd = quote(svnclient) + ' update -r ' + svn_rev + ' ' + emer_src
      print '\nUpdating emergent source code...'
      print cmd
      os.system(cmd)

##############################################################################
# Function to prompt for a build suffix.

build_suffix = ""
def ask_suffix():
  global build_suffix
  print_horizontal()
  build_suffix = raw_input('\nEnter a build suffix (optional): ')
  if build_suffix and build_suffix[0] != '_':
    build_suffix = '_' + build_suffix

##############################################################################
# Function to download third-party libraries (GSL, ODE)

tools_dir = ''
third_party_dir = ''
def inst_3rd_party():
  global tools_dir, third_party_dir
  # Make a tools directory specific to the VC++ compiler version and
  # architecture (32-bit or 64-bit).  Doesn't matter debug vs. release.
  tools_dir = os.path.join(devtool_dir, 'tools' + get_compiler_extension_args(build_64bit, False))
  # Extract the bundle of third-party stuff into this tools directory.
  third_party_dir = os.path.join(tools_dir, '3rdparty')
  while not fileExists(os.path.join(third_party_dir, 'lib/gsl.lib')):
    print_horizontal()
    print 'You need to get the "third party" package, which includes'
    print 'the GSL and ODE libraries.  This can be done for you.'
    if askUser('\nReady to download and unzip 3rd party tools?'):
      print '\nDownloading 3rd party tools...'
      if msvs == 2008:
        file = getUrl('ftp://grey.colorado.edu/pub/emergent/3rdparty-5.0.1-win32.zip')
      elif build_64bit:
        file = getUrl('ftp://grey.colorado.edu/pub/emergent/3rdparty-5.3.10-msvs2010-64.zip')
      else:
        file = getUrl('ftp://grey.colorado.edu/pub/emergent/3rdparty-5.3.10-msvs2010-32.zip')
      zip = ZipFile(file)
      zip.extractall(third_party_dir)

##############################################################################
# Function to download the Subversion development libraries

subversion_libs_dir = ''
def inst_subversion_libs():
  global subversion_libs_dir
  # Extract Subversion (and related) libraries into its own directory.
  subversion_libs_dir = os.path.join(tools_dir, 'subversion')
  while not fileExists(os.path.join(subversion_libs_dir, 'include/svn_client.h')):
    print_horizontal()
    print 'You need to get the Subversion development libraries.'
    print 'This can be done for you.'
    if askUser('\nReady to download and unzip Subversion development libraries?'):
      print '\nDownloading Subversion development libraries...'
      if msvs == 2008:
        print "\nSorry, the Subversion development libraries haven't been built for VS 2008"
        return
      elif build_64bit:
        file = getUrl('ftp://grey.colorado.edu/pub/emergent/Subversion-1.7.7-bin-msvs2010-64.zip')
      else:
        file = getUrl('ftp://grey.colorado.edu/pub/emergent/Subversion-1.7.7-bin-msvs2010-32.zip')
      zip = ZipFile(file)
      zip.extractall(subversion_libs_dir)

##############################################################################
# Function to download Qt libraries

qt_dir = ''
def inst_qt():
  global qt_dir
  if msvs == 2008:
    # VC++2008 only supports 32-bit compiles, so just use the pre-built Qt
    # downloaded from Nokia's website.
    qt_ver = '4.6.2'
    qt_dir = 'C:/Qt/' + qt_ver
  else:
    # VC++2010 supports 32- and 64-bit compiles.  Use a Qt package that was
    # custom-built for emergent (includes zlib and libjpeg) for the given arch.
    qt_ver = '4.8.2'
    qt_dir = 'C:/Qt/' + qt_ver + get_compiler_extension_args(build_64bit, False)

  while not fileExists(os.path.join(qt_dir, 'bin/moc.exe')):
    print_horizontal()
    print 'You need to install Qt.  This can be downloaded for you to install.'
    if askUser('\nReady to download and install Qt?'):
      print '\nDownloading Qt.  THIS FILE IS HUGE, be patient while it downloads.....'
      if msvs == 2008:
        print 'When the download completes, installation will begin.'
        print 'Accept default installation options.  Return to this script when complete.'
        file = getUrl('ftp://grey.colorado.edu/pub/emergent/qt-win-opensource-' + qt_ver + '-vs2008.exe')
        os.system(file)
        response = raw_input('\nOnce Qt has been installed, press enter to continue...')
      else:
        print 'When the download completes, Qt will be installed automatically.'
        if build_64bit:
          file = getUrl('ftp://grey.colorado.edu/pub/emergent/qt-win-opensource-' + qt_ver + '-vs2010-64.exe')
        else:
          file = getUrl('ftp://grey.colorado.edu/pub/emergent/qt-win-opensource-' + qt_ver + '-vs2010-32.exe')

        print '\n\nDownload complete.  Installing Qt...'
        cmd = file + ' -y -oC:\\Qt\\'
        os.system(cmd)
        response = raw_input('\nQt has been installed.  Press enter to continue...')

##############################################################################
# Function to download Coin3d library

coin_dir = ''
def inst_coin():
  global coin_dir
  coin_dir = os.path.join(tools_dir, 'Coin/3.1.3')

  if msvs == 2008:
    coin_ftp = 'ftp://grey.colorado.edu/pub/emergent/Coin-3.1.3-bin-msvc9.zip'
  elif build_64bit:
    coin_ftp = 'ftp://grey.colorado.edu/pub/emergent/Coin-3.1.3-bin-msvs2010-64.zip'
  else:
    coin_ftp = 'ftp://grey.colorado.edu/pub/emergent/Coin-3.1.3-bin-msvs2010-32.zip'

  while not fileExists(os.path.join(coin_dir, 'bin/coin3.dll')):
    print_horizontal()
    print 'You need to get the Coin package.  This can be done for you.'
    if askUser('\nReady to download and unzip Coin?'):
      makeDir(coin_dir)
      print '\nDownloading Coin...'
      file = getUrl(coin_ftp)
      zip = ZipFile(file)
      zip.extractall(coin_dir)

##############################################################################
# Functions to ensure specific environment variables are set correctly for
# building

def setEnvironmentVariable(name, value):
  setUserEnvVar(name, value)
  os.environ[name] = value
  print 'Set ' + name + '=' + value

def checkEnvironmentVariable(name, value):
  oldval = ''
  try: oldval = os.environ[name]
  except KeyError: pass
  regval = getUserEnvVar(name)
  if regval == value:
    if oldval != value:
      print 'Updating ' + name + ' in local environment to match registry settings'
      os.environ[name] = value
    print 'Environment variable OK: ' + name + '=' + value
  elif regval == '' and oldval == '':
    print 'Need to set environment variable: ' + name + '=' + value
    if askUser('OK to set variable?'):
      setEnvironmentVariable(name, value)
  else:
    print '\nNeed to update environment variable: ' + name
    print '  Current registry value:    ' + regval
    print '  Current environment value: ' + oldval
    print '  New value to be set:       ' + value
    if askUser('OK to change variable?'):
      setEnvironmentVariable(name, value)

def list_difference(orig, remove):
  for item in remove:
    orig[:] = [x for x in orig if x != item]

# Remove items from the list that match the regexp,
# unless they are identical to the 'keep' value.
def remove_like_except(list, regexp, keep):
  re_comp = re.compile(regexp, re.IGNORECASE)
  list[:] = [x for x in list if not re_comp.search(x) or x == keep]

def fix_environment():
  print_horizontal()
  checkEnvironmentVariable('EMER_3RDPARTY_DIR', fixPath(third_party_dir))
  checkEnvironmentVariable('EMER_SVN_LIBS_DIR', fixPath(subversion_libs_dir))
  checkEnvironmentVariable('COINDIR', fixPath(coin_dir))
  if msvs == 2008:
    checkEnvironmentVariable('QMAKESPEC', 'win32-msvc2008')
  else:
    # This seems to be ok for 64-bit VC++2010 compiles too.
    checkEnvironmentVariable('QMAKESPEC', 'win32-msvc2010')
  checkEnvironmentVariable('QTDIR', fixPath(qt_dir))
  checkEnvironmentVariable('EMERGENTDIR', fixPath('C:/Emergent'))

  qt_bin_dir = fixPath(os.path.join(qt_dir, 'bin'))
  coin_bin_dir = fixPath(os.path.join(coin_dir, 'bin'))
  third_party_bin_dir = fixPath(os.path.join(third_party_dir, 'bin'))
  subversion_libs_bin_dir = fixPath(os.path.join(subversion_libs_dir, 'bin'))
  cmake_bin_dir = fixPath(os.path.dirname(cmake_exe))
  needed_paths = [
    # TBD: need this?
    #fixPath(amd64_dir),
    qt_bin_dir,
    coin_bin_dir,
    third_party_bin_dir,
    subversion_libs_bin_dir,
    fixPath(jom_dir),
    cmake_bin_dir,
    ]

  syspath = getSysEnvVar('PATH').split(';')
  orig_userpath = getUserEnvVar('PATH')
  userpath = orig_userpath.split(';') if orig_userpath != '' else []

  # Check if the paths contains any instances of the wrong Coin dir,
  # wrong Qt dir, etc., and get rid of them if so.
  # TODO: need to update this to reflect tools_dir locations.
  for list in [userpath, syspath]:
    # Even with r'' raw strings, need to escape backslashes so
    # they aren't treated as escapes by the regexp compiler.
    remove_like_except(list, r'C:\\Qt\\[-0-9.msv]{5,}\\bin', qt_bin_dir)
    remove_like_except(list, r'C:\\Coin\\[-0-9.]{5,}\\bin', coin_bin_dir)
    # The 3rdparty directory is now in C:\src\devtools, not in C:\src\emergent-foo\,
    # but we still need to check there in case someone has a REALLY old environment
    # variable set from long ago.
    remove_like_except(list, r'emergent.*\\3rdparty\\bin', third_party_bin_dir)
    remove_like_except(list, r'CMake [-0-9.]{3,}\\bin', cmake_bin_dir)
    remove_like_except(list, r'C:\\src\\devtools\\.*\\Coin\\3.1.3\\bin', coin_bin_dir)
    remove_like_except(list, r'C:\\src\\devtools\\.*\\3rdparty\\bin', third_party_bin_dir)
    remove_like_except(list, r'C:\\src\\devtools\\.*\\subversion\\bin', subversion_libs_bin_dir)

  # Only need to add things that aren't already in the path (if any).
  list_difference(needed_paths, syspath + userpath)
  if needed_paths:
    new_userpath_list = userpath + needed_paths
    new_userpath_str = ';'.join(new_userpath_list)
    print_horizontal()
    print 'Need to update PATH environment variable.'
    print 'Old user path: ' + orig_userpath
    print 'New user path: ' + new_userpath_str
    print 'System path will not be changed.'
    if askUser('\nReady to update path?'):
      setUserEnvVar('PATH', new_userpath_str)
      userpath = new_userpath_list

  wholepath = ';'.join(syspath) + ';' + ';'.join(userpath)
  cmd = os.environ['ComSpec']
  expanded_path = subprocess.check_output([cmd, '/c', '@echo ' + wholepath])
  os.environ['PATH'] = expanded_path
  print '\nPATH = ' + expanded_path

##############################################################################
# Functions to download and build the Quarter library from source

def inst_quarter():
  if msvs == 2008:
    quarter_sln = os.path.join(tools_dir, 'Quarter/build/msvc9/quarter1.sln')
  elif msvs == 2010:
    quarter_sln = os.path.join(tools_dir, 'Quarter/build/msvc10/quarter1.sln')
  while not fileExists(quarter_sln):
    print_horizontal()
    print 'You need to get the Quarter package.  This can be done for you.'
    if askUser('\nReady to download and unzip Quarter?'):
      print '\nDownloading Quarter...'
      if msvs == 2008:
        file = getUrl('ftp://grey.colorado.edu/pub/emergent/Quarter-latest.zip')
      else:
        file = getUrl('ftp://grey.colorado.edu/pub/emergent/Quarter-r460-with-msvc10.zip')
      zip = ZipFile(file)
      zip.extractall(tools_dir)
  return quarter_sln

# Quarter build outputs get copied to COINDIR as part of the build process,
# so test that output to see if this step has completed.
def compile_quarter():
  # Check for the quarter *debug* lib to be installed, since that is the
  # last one to be built and installed by this script.
  while not fileExists(os.path.join(coin_dir, 'lib/quarter1d.lib')):
    quarter_sln = inst_quarter()

    print_horizontal()
    print 'You need to compile the Quarter package.  This can be done for you.'
    if askUser('\nReady to compile Quarter?'):
      if msvs == 2008:
        msbuild = 'C:/WINDOWS/Microsoft.NET/Framework/v3.5/MSBuild.exe'
        if not fileExists(msbuild):
          print '\nCould not find file: ' + msbuild
          print 'Quitting'
          quit(1)

        # os.system() won't work here -- because of the space after DLL??
        # Build Release and Debug libraries
        print '\nCompiling Quarter (RELEASE)...\n'
        subprocess.call([msbuild, '/p:Configuration=DLL (Release)', '/p:Platform=Win32', quarter_sln])
        print '\nCompiling Quarter (DEBUG)...\n'
        subprocess.call([msbuild, '/p:Configuration=DLL (Debug)', '/p:Platform=Win32', quarter_sln])
        print '\nWarnings are expected when building Quarter.  It should say "Build succeeded".\n'
        return

      # Otherwise 2010.
      if build_64bit:
        platform = 'x64'
        arch = '/x64'
      else:
        platform = 'Win32'
        arch = '/x86'

      configs = ['Release', 'Debug']
      for config in configs:
        print '\nCompiling Quarter (' + config + ')...\n'
        quarter_build_bat = os.path.join(tools_dir, 'Quarter/build/make-quarter-' + config + '-' + platform + '.bat')
        with open(quarter_build_bat, 'w') as f:
          # setenv sets the path so the right msbuild is in the path.
          f.write('call ' + quoteFixPath(setenv_cmd) + ' ' + arch + ' /' + config + '\n')
          f.write('msbuild "/p:Configuration=DLL (' + config + ')" /p:Platform=' + platform + ' ' + quarter_sln + '\n')
        while 0 != os.system(quarter_build_bat):
          global yes_to_all
          yes_to_all = False
          print_horizontal()
          if not askUser('Compilation failed.  OK to rebuild?', default=''): quit(0)
        print_horizontal()
        print 'Compilation succeeded.\n'

##############################################################################
# Function to build emergent

emer_build = ''
def build_emergent():
  global emer_build
  print_horizontal()
  print 'Your build environment is configured.'
  if not askUser('\nReady to build?'): quit(0)

  print '\nBuilding emergent...'
  emer_build = os.path.join(emer_src, 'build' + build_suffix + get_compiler_extension())
  makeDir(emer_build)

  # TODO: probably should update this to use setenv instead, at least for VC++2010.
  if msvs == 2008:
    vcvarsall_bat = findInProgFiles('Microsoft Visual Studio 9.0/VC/vcvarsall.bat')
  elif msvs == 2010:
    vcvarsall_bat = findInProgFiles('Microsoft Visual Studio 10.0/VC/vcvarsall.bat')

  if not vcvarsall_bat:
    print "Can't continue because vcvarsall.bat file not found!  Quitting."
    quit(1)

  # Build the CMake command line.
  cmake_cmd = quoteFixPath(cmake_exe)
  if verbose_build:    cmake_cmd += ' -DCMAKE_VERBOSE_MAKEFILE=ON'
  else:                cmake_cmd += ' -DCMAKE_VERBOSE_MAKEFILE=OFF'

  if build_suffix:     cmake_cmd += ' -DEXTRA_SUFFIX=' + build_suffix

  if build_debug:
    if msvs == 2008:   cmake_cmd += ' .. -G "Visual Studio 9 2008" -DCMAKE_BUILD_TYPE=Debug'
    elif msvs == 2010:
      if build_64bit:  cmake_cmd += ' .. -G "Visual Studio 10 Win64" -DCMAKE_BUILD_TYPE=Debug'
      else:            cmake_cmd += ' .. -G "Visual Studio 10" -DCMAKE_BUILD_TYPE=Debug'
  elif use_jom:        cmake_cmd += ' .. -G "NMake Makefiles JOM" -DCMAKE_BUILD_TYPE=Release'
  else:                cmake_cmd += ' .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release'

  if build_64bit: arch_flag = 'x64'
  else:           arch_flag = 'x86'

  if build_debug: start_cmd = 'start Emergent.sln'
  elif use_jom:   start_cmd = quoteFixPath(jom_exe) + ' package'
  else:           start_cmd = 'nmake package'

  # Create a batch file to prepare the environment and kick off the build.
  cmake_bat = os.path.join(emer_src, 'do_cmake' + get_compiler_extension() + '.bat')
  with open(cmake_bat, 'w') as f:
    f.write('echo on\n')
    f.write('call ' + quoteFixPath(vcvarsall_bat) + ' ' + arch_flag + '\n')
    f.write('cd /d ' + fixPath(emer_build) + '\n')
    f.write(cmake_cmd + '\n')
    f.write('if ERRORLEVEL 1 exit\n')   # CMake error
    f.write('if ERRORLEVEL 255 exit\n') # Ctrl-C
    f.write(start_cmd + '\n')

  while 0 != os.system(cmake_bat):
    global yes_to_all
    yes_to_all = False
    print_horizontal()
    if not askUser('Compilation failed.  OK to rebuild?'): quit(0)
  print_horizontal()
  print 'Compilation succeeded.\n'

##############################################################################
# Function to rename the emergent package based on build type and revision,
# in preparation for upload

def rename_package():
  version = ''
  revision = ''
  for line in open(os.path.join(emer_src, 'temt_version.h')):
    if 'VERSION' in line:
      version = line.split('"')[-2]
  for line in open(os.path.join(emer_build, 'src/temt/lib/svnrev.h')):
    if 'SVN_REV' in line:
      revision = line.split(' ')[-1].rstrip()
      revision
  print_horizontal()
  print 'Detected version: ' + version
  print '         svn rev: ' + revision
  if len(version) > 0 and len(revision) > 0:
    # Old name must match format produced by EmergentCPack.cmake.
    win_arch_suffix = '-win64.exe' if build_64bit else '-win32.exe'
    old_name = 'emergent' + build_suffix + '-' + version + win_arch_suffix
    old_path = os.path.join(emer_build, old_name)

    new_name = 'emergent' + build_suffix + '-' + version + '-' + revision + win_arch_suffix
    new_path = os.path.join(emer_build, new_name)

    if not os.path.isfile(old_path):
      print '\nCould not find installer: ' + old_path
      quit(1)
    ok_to_overwrite = False
    if os.path.isfile(new_path):
      print '\nInstaller already exists with name: ' + new_path
      if askUser('\nOK to overwrite?'):
        ok_to_overwrite = True
        os.remove(new_path)
      else:
        print '\nWill not overwrite existing installer, quitting.'
        quit(0)
    print '\nWill rename installer:'
    print '  from: ' + old_path
    print '  to:   ' + new_path
    if ok_to_overwrite or askUser('\nOK to rename?'):
      os.rename(old_path, new_path)
      print 'Installer renamed.'
      scp_exe = 'c:\\cygwin\\bin\\scp.exe'
      if fileExists(scp_exe) and askUser('\nOK to scp to grey?'):
        cygpath_exe = 'c:\\cygwin\\bin\\cygpath.exe'
        cygname = subprocess.check_output([cygpath_exe, new_path]).strip()
        subprocess.call([scp_exe, cygname, 'tmankad@grey.colorado.edu:/home/tmankad/'])

##############################################################################
# Main

def main():
  try:
    os.system('color 07')
    inst_msvs() # asks whether to use VC++2008 or 2010
    inst_nsis()
    inst_svn()
    inst_cmake()
    inst_jom()
    get_emer_src_path()
    ask_debug_or_release()
    ask_32_or_64()
    ask_verbose()
    ask_suffix()
    inst_emer_src()
    inst_3rd_party()
    inst_subversion_libs()
    inst_qt()
    inst_coin()
    fix_environment()
    compile_quarter()
    build_emergent()
    if not build_debug:
      rename_package()
    quit(0)
  except KeyboardInterrupt:
    print '\n\nQuitting at user request (Ctrl-C).'

if __name__ == '__main__':
  main()
