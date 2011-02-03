# This script attempts to download and install all the tools needed
# to build Emergent, checkout the source from Subversion, build it,
# and create an installer.
# NOTE: if you commit a new version of this file to svn, make sure to
# update the file on the ftp site as well

import os, re, subprocess, sys, urllib, webbrowser, _winreg
from zipfile import ZipFile

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
		return ""
	_winreg.CloseKey(key)
	return value

def getUserEnvVar(name):
	return getEnvVar(name, _winreg.HKEY_CURRENT_USER, 'Environment')

def getSysEnvVar(name):
	env = 'SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment'
	return getEnvVar(name, _winreg.HKEY_LOCAL_MACHINE, env)

def setUserEnvVar(name, value):
	return setEnvVar(name, value, _winreg.HKEY_CURRENT_USER, 'Environment')

def setSysEnvVar(name, value):
	env = 'SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment'
	return setEnvVar(name, value, _winreg.HKEY_LOCAL_MACHINE, env)


try:
	progfiles = set([os.environ['ProgramFiles(x86)'], os.environ['ProgramFiles'], os.environ['ProgramW6432']])
	isWin64 = True
except KeyError:
	progfiles = set([os.environ['ProgramFiles']])
	isWin64 = False

def makeDir(dir):
	try:
		os.makedirs(dir)
	except os.error:
		pass

devtool_dir = 'C:/src/devtools'
makeDir(devtool_dir)

def fixPath(path):
	return re.sub(r'/', r'\\', path)

def fileExists(file):
	if os.path.isfile(file):
		print "  Detected " + fixPath(file)
		return True
	return False

def dirExists(dir):
	if os.path.isdir(dir):
		print "  Detected " + fixPath(dir)
		return True
	return False

def findInProgFiles(path):
	for prefix in progfiles:
		file = os.path.join(prefix, path)
		if fileExists(file):
			return file # True
	return False

def getUrl(url):
	try:
		file = os.path.basename(url)
		file = os.path.join(devtool_dir, file)
		if not fileExists(file):
			print "Downloading ", url
			urllib.urlretrieve(url, file)
	except Exception:
		os.remove(file)
		print "\nERROR: could not download file from: " + url
		print "\nAborting."
		sys.exit(1)
	return file

def getOptions(default):
	if default == "y" or default == "Y":   return " [Y/n/q/a]: "
	elif default == "n" or default == "N": return " [y/N/q/a]: "
	else:                                  return " [y/n/Q/a]: "

yes_to_all = False
def askUser(question, default="Y"):
	global yes_to_all
	if yes_to_all: return True
	response = raw_input(question + getOptions(default))
	if response == "" : response = default
	if response[0] == "a" or response[0] == "A":
		yes_to_all = True
		return True
	if response[0] == "q" or response[0] == "Q":
		print "\nQuitting at user request."
		sys.exit(0)
	return response[0] == "y" or response[0] == "Y"

def inst_msvs():
	while not findInProgFiles('Microsoft Visual Studio 9.0/VC/bin/cl.exe'):
		print "\nYou need to install Visual C++ 2008 Express Edition.  Make sure to get"
		print "the 2008 edition, not 2010!  Download the installer from the website"
		print "that will be opened, then run it.  Accept default installation options."
		print "After installation is complete, return to this script."
		if askUser("\nReady to download and install Visual C++?"):
			webbrowser.open_new_tab('http://www.microsoft.com/express/Downloads/#Visual_Studio_2008_Express_Downloads')
			print "\nOnce Visual C++ has been installed, press enter to continue"
			response = raw_input("")

def inst_nsis():
	while not findInProgFiles('NSIS/NSIS.exe'):
		print "\nYou need to install NSIS.  A website will be opened that will prompt"
		print "you to save a file.  Save it and run it to install NSIS.  Accept default"
		print "installation options.  After installation is complete, return to this script."
		if askUser("\nReady to download and install NSIS?"):
			webbrowser.open_new_tab('http://prdownloads.sourceforge.net/nsis/nsis-2.46-setup.exe?download')
			print "\nOnce NSIS has been installed, press enter to continue"
			response = raw_input("")

qt_dir = 'C:/Qt/4.6.2'
def inst_qt():
	while not fileExists(qt_dir + '/bin/moc.exe'):
		print "\nYou need to install Qt.  This can be downloaded for you to install."
		if askUser("\nReady to download and install Qt?"):
			print "\nDownloading Qt.  When the download completes, installation will begin."
			print "Accept default installation options.  Return to this script when complete."
			print "\nTHIS FILE IS HUGE, be patient while it downloads....."
			file = getUrl("ftp://grey.colorado.edu/pub/emergent/qt-win-opensource-4.6.2-vs2008.exe")
			os.system(file)
			print "\nOnce Qt has been installed, press enter to continue"
			response = raw_input("")

def inst_svn():
	while not findInProgFiles('SlikSvn/bin/svn.exe'):
		print "\nYou need to install a Subversion client (svn).  This can be downloaded"
		print "for you to install."
		if askUser("\nReady to download and install Subversion?"):
			print "\nDownloading Slik Subversion client.  When the download completes,"
			print "installation will begin.  Accept default installation options."
			print "Return to this script when complete."
			url = "http://www.sliksvn.com/pub/Slik-Subversion-1.6.15-win32.msi"
			if isWin64:
				url = "http://www.sliksvn.com/pub/Slik-Subversion-1.6.15-x64.msi"
			file = getUrl(url)
			os.system(file)
			print "\nOnce Subversion has been installed, press enter to continue"
			response = raw_input("")

cmake_exe = ""
def inst_cmake():
	global cmake_exe
	while True:
		cmake_exe = findInProgFiles('CMake 2.8/bin/cmake.exe')
		if cmake_exe: break
		print "\nYou need to install CMake.  This can be downloaded for you to install."
		if askUser("\nReady to download and install CMake?"):
			print "\nDownloading CMake.  When the download completes, installation will begin."
			print "Accept default installation options.  Return to this script when complete."
			file = getUrl("ftp://grey.colorado.edu/pub/emergent/cmake-2.8.1-win32-x86.exe")
			os.system(file)
			print "\nOnce CMake has been installed, press enter to continue"
			response = raw_input("")

use_jom = False
jom_exe = ""
jom_dir = os.path.join(devtool_dir, "jom")
def inst_jom():
	global use_jom, jom_exe
	jom_exe = os.path.join(jom_dir, "jom.exe")
	if not fileExists(jom_exe):
		print "\nYou should install Jom.  This can be downloaded and installed for you."
		if askUser("\nReady to download and install Jom?"):
			print "\nDownloading Jom..."
			file = getUrl("ftp://ftp.qt.nokia.com/jom/jom.zip")
			zip = ZipFile(file)
			zip.extractall(jom_dir)
	if fileExists(jom_exe):
		use_jom = True

emer_src = 'C:/src/emergent'
def inst_emer_src():
	if not dirExists(emer_src + '/.svn'):
		print "\nYou need to checkout the Emergent source code from subversion."
		print "This can be done for you."
		if askUser("\nReady to checkout the Emergent souce?"):
			makeDir(emer_src)
			svnclient = findInProgFiles('SlikSvn/bin/svn.exe')
			if not svnclient:
				print "Can't continue because SVN client needs to be installed!  Quitting."
				sys.exit(1)

			response = raw_input("\nEnter your username (blank for anonymous) [anonymous]: ")
			cmd = '"' + svnclient + '" checkout'
			if response == "" or response == "anonymous":
				cmd += " --username anonymous --password emergent"
			else:
				cmd += " --username " + response
			cmd += " http://grey.colorado.edu/svn/emergent/emergent/trunk " + emer_src
			print "\nChecking out Emergent source code..."
			print cmd
			os.system(cmd)
	else:
		# Found .svn folder, make sure it's up to date
		print "\nYou probably want to make sure the Emergent source is up to date."
		if askUser("\nReady to update the Emergent source?"):
			svnclient = findInProgFiles('SlikSvn/bin/svn.exe')
			if not svnclient:
				print "Can't continue because SVN client needs to be installed!  Quitting."
				sys.exit(1)
			cmd = '"' + svnclient + '" update ' + emer_src
			print "\nUpdating Emergent source code..."
			print cmd
			os.system(cmd)

coin_dir = 'C:/Coin/3.1.3'
def inst_coin():
	while not fileExists(coin_dir + '/bin/coin3.dll'):
		print "\nYou need to get the Coin package.  This can be done for you."
		if askUser("\nReady to download and unzip Coin?"):
			makeDir(coin_dir)
			print "\nDownloading Coin..."
			file = getUrl("ftp://grey.colorado.edu/pub/emergent/Coin-3.1.3-bin-msvc9.zip")
			zip = ZipFile(file)
			zip.extractall(coin_dir)

third_party_dir = 'C:/src/emergent/3rdparty'
def inst_3rd_party():
	while not fileExists(third_party_dir + '/lib/gsl.lib'):
		print "\nYou need to get the 'third party' package.  This can be done for you."
		if askUser("\nReady to download and unzip 3rd party tools?"):
			print "\nDownloading 3rd party tools..."
			file = getUrl("ftp://grey.colorado.edu/pub/emergent/3rdparty-5.0.1-win32.zip")
			zip = ZipFile(file)
			zip.extractall(third_party_dir)

quarter_dir = 'C:/src'
quarter_sln = quarter_dir + '/Quarter/build/msvc9/quarter1.sln'
def inst_quarter():
	while not fileExists(quarter_sln):
		print "\nYou need to get the Quarter package.  This can be done for you."
		if askUser("\nReady to download and unzip Quarter?"):
			print "\nDownloading Quarter..."
			file = getUrl("ftp://grey.colorado.edu/pub/emergent/Quarter-latest.zip")
			zip = ZipFile(file)
			zip.extractall(quarter_dir)

def setEnvironmentVariable(name, value):
	setUserEnvVar(name, value)
	os.environ[name] = value
	print "Set " + name + "=" + value

def checkEnvironmentVariable(name, value):
	try:
		oldval = os.environ[name]
		if oldval == value:
			print "Environment variable OK: " + name + "=" + oldval
		else:
			print "Environment variable " + name + ":"
			print "  Old value: " + oldval
			print "  New value: " + value
			response = raw_input("OK to change variable? [Y/n]: ")
			if isYes(response):
				setEnvironmentVariable(name, value)
	except KeyError:
		print "Environment variable " + name + " is not currently set."
		print "  New value: " + value
		response = raw_input("OK to set variable? [Y/n]: ")
		if isYes(response):
			setEnvironmentVariable(name, value)

# Quarter build outputs get copied to COINDIR as part of the build process,
# so test that output to see if this step has completed.
def compile_quarter():
	while not fileExists(coin_dir + '/lib/quarter1d.lib'):
		print "\nYou need to compile the Quarter package.  This can be done for you."
		if askUser("\nReady to compile Quarter?"):
			msbuild = "C:/WINDOWS/Microsoft.NET/Framework/v3.5/MSBuild.exe"
			print "\nCompiling Quarter..."
			# os.system() won't work here -- because of the space after DLL??
			# Build Release and Debug libraries
			subprocess.call([msbuild, '/p:Configuration=DLL (Release)', quarter_sln])
			subprocess.call([msbuild, '/p:Configuration=DLL (Debug)', quarter_sln])

def fix_environment():
	checkEnvironmentVariable('COINDIR', fixPath(coin_dir))
	checkEnvironmentVariable('QMAKESPEC', 'win32-msvc2008')
	checkEnvironmentVariable('QTDIR', fixPath(qt_dir))
	checkEnvironmentVariable('EMERGENTDIR', fixPath('C:/Emergent'))

	syspath = getSysEnvVar('PATH')
	userpath = getUserEnvVar('PATH')
	wholepath = syspath + ';' + userpath
	pathitems = set(wholepath.split(';'))
	newitems = set([
		fixPath(qt_dir + "/bin"),
		fixPath(coin_dir + "/bin"),
		fixPath(third_party_dir + "/bin"),
		fixPath(jom_dir),
		fixPath(os.path.dirname(cmake_exe))
		]).difference(pathitems)
	if newitems:
		newuserpath = ';'.join(newitems)
		if userpath != "":
			newuserpath = ';'.join([userpath, newuserpath])
		print "\nNeed to update PATH environment variable."
		print "Old user path: " + userpath
		print "New user path: " + newuserpath
		print "System path will not be changed."
		if askUser("\nReady to update path?"):
			setUserEnvVar('PATH', newuserpath)
			os.environ['PATH'] = syspath + ';' + newuserpath
	else:
		print "\nPath seems OK."
	if os.environ['PATH'] != wholepath:
		cmd = os.environ['ComSpec']
		expanded_path = subprocess.check_output([cmd, '/c', '@echo ' + wholepath])
		os.environ['PATH'] = expanded_path

def build_emergent():
	print "\nYour build environment is configured."
	if not askUser("\nReady to build?"): return

	print "\nBuilding Emergent..."
	emer_build = emer_src + '/build'
	makeDir(emer_build)

	cmake_bat = emer_src + '/do_cmake.bat'
	f = open(cmake_bat, 'w')
	vcvarsall_bat = findInProgFiles('Microsoft Visual Studio 9.0/VC/vcvarsall.bat')
	f.write('echo on\n')
	f.write('call "' + vcvarsall_bat + '"\n')
	f.write("cd /d " + fixPath(emer_build) + "\n")
	if use_jom:
		f.write('"' + cmake_exe + '" .. -G "NMake Makefiles JOM" -DCMAKE_BUILD_TYPE=Release\n')
		f.write('"' + jom_exe + '" package')
	else:
		f.write('"' + cmake_exe + '" .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release\n')
		f.write('nmake package')	
	f.close()
	#subprocess.Popen([cmake_exe, '..', '-G', 'NMake Makefiles', '-DCMAKE_BUILD_TYPE=Release'], cwd=emer_build).wait()
	os.system(cmake_bat)

inst_msvs()
inst_nsis()
inst_qt()
inst_svn()
inst_cmake()
inst_jom()
inst_emer_src()
inst_coin()
inst_3rd_party()
inst_quarter()
fix_environment()
compile_quarter()
build_emergent()

response = raw_input("\n\n\nPress enter to close the window.\n")
