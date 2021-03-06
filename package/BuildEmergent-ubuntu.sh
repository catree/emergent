#!/bin/bash
# This script builds debian packages for emergent.
# It has been tested on Ubuntu 10.04, 10.10, 11.04, 11.10, 12.04, and 12.10.
set -e

# Make sure we're on the right kind of Linux.
# (could use /usr/bin/lsb_release to get this info)
NEED_BACKPORT="n"
ISSUE="Ubuntu 14.10"
USE_LIBCOIN="libcoin80-dev"
if ! grep -q "$ISSUE" /etc/issue; then
  ISSUE="Ubuntu 14.04"
  if ! grep -q "$ISSUE" /etc/issue; then
    ISSUE="Ubuntu 13.10"
    if ! grep -q "$ISSUE" /etc/issue; then
      ISSUE="Ubuntu 13.04"
      if ! grep -q "$ISSUE" /etc/issue; then
        ISSUE="Ubuntu 12.10"
        USE_LIBCOIN="libcoin60-dev"
        if ! grep -q "$ISSUE" /etc/issue; then
          ISSUE="Ubuntu 12.04"
          USE_LIBCOIN="libcoin60-dev"
          if ! grep -q "$ISSUE" /etc/issue; then
            echo "ERROR: This script should be run on ${ISSUE} or higher"
            exit
          fi
        fi
      fi
    fi
  fi	
fi
REPONAME=`lsb_release -cs`

echo "Note: you must provide your password for sudo a few times for this script."

# Get emergent svn revision to build
REV="$1"
if [ -z $REV ]; then
  read -p "Enter the emergent svn revision number to retrieve: [HEAD] " REV
  if [ -z $REV ]; then REV="HEAD"; fi
fi

TAG="$2"
if [ -z $TAG ]; then
  read -p "Enter the emergent tag to use: [trunk] " TAG
  if [ -z $TAG ]; then TAG="trunk"; fi
fi
if [ $TAG != "trunk" ]; then TAG="tags/$TAG"; fi

QTVER="qt4"
read -p "Use QT5? [Y/n]:" 
if [ -z $REPLY ] || [ $REPLY == "Y" ] || [ $REPLY == "y" ]; then
  QTVER="qt5"
  if [ -z $QTDIR ]; then
    read -p "Where is Qt5 installed (leave blank if installed from package)?" QTDIR; fi
fi

# Check if the Quarter library should be rebuilt.  Defaults to Yes, unless
# it is already installed.  In that case, defer to a command line option
# (if present) or prompt the user.
BUILD_QUARTER="y"
if dpkg-query -s libquarter0 2>&1 | grep -q "Status.*installed"; then
  # If it's already installed, then check if a command line option
  # was given for whether to rebuild it.
  BUILD_QUARTER="$3"
  if [ -z $BUILD_QUARTER ]; then
    # No command line option given, so prompt to rebuild (defaults to No).
    BUILD_QUARTER="n"
    read -p "Rebuild the Quarter library from source? [y/N]: " BUILD_QUARTER
  fi
fi
if [ "$BUILD_QUARTER" == "Y" ]; then BUILD_QUARTER="y"; fi
if [ "$BUILD_QUARTER" == "y" ]; then
  echo "The Quarter library will be rebuilt."
fi

# Make sure the backports repo is enabled so we can get cmake 2.8.3 on
# maverick (cmake 2.8.2 has a bug that prevents creating .deb packages
# that work) and cmake 2.8.1 on lucid (cmake 2.8.0 has some other bug).
if [ "$NEED_BACKPORT" == "y" ]; then
  REPOS=/etc/apt/sources.list
  FIND_BACKPORTS="^\s*deb\s\s*http://us.archive.ubuntu.com/ubuntu/\s\s*${REPONAME}-backports"
  if ! grep -q $FIND_BACKPORTS $REPOS; then
    echo "Need to add the ${REPONAME}-backports repository to get good cmake version"
    REPOS_BACKUP="${REPOS}-backup"
    echo "Backing up $REPOS to $REPOS_BACKUP..."
    sudo cp $REPOS $REPOS_BACKUP
    echo "Trying to uncomment ${REPONAME}-backports repo line..."
    sudo sed -i "/^\s*##*\s*deb\(-src\)\{0,1\}\s\s*http:\/\/us.archive.ubuntu.com\/ubuntu\/\s\s*${REPONAME}-backports/s/^\s*##*\s*//" $REPOS
    if ! grep -q $FIND_BACKPORTS $REPOS; then
      echo "Didn't find a commented out ${REPONAME}-backports line; creating new"
      sudo sh -c "echo 'deb http://us.archive.ubuntu.com/ubuntu/ ${REPONAME}-backports main restricted universe multiverse' >> $REPOS"
    fi
  fi
  
  # Double check that backports is enabled.
  if grep -q $FIND_BACKPORTS $REPOS; then
    echo "Found ${REPONAME}-backport repository"
  else
    echo "Could not add ${REPONAME}-backport repository needed for cmake upgrade.  Quitting."
    exit
  fi
fi

# Install prereq packages
# Need to update to make sure we see the backport repo and get the latest cmake.
echo "Updating repositories..."
sudo apt-get -qq update | sed 's/^/  /'

# Packages needed for debuild
DEBUILD_PKGS="build-essential gnupg lintian fakeroot debhelper dh-make subversion-tools devscripts mercurial"

# Packages needed to build emergent
# This list should match the CPACK_DEBIAN_PACKAGE_DEPENDS line of emergent/CMakeModules/EmergentCPack.cmake
# Except:
#  * only need checkinstall here to make the Quarter package.
#    * (that was the old way of packaging)
#  * don't need libquarter here since we will be building it ourselves.
# Changed libCoin60 to libCoin80, and removed libqt4-dev, for packaging. (Legacy versions can build against the older one 
# but we don't care here in the packaging script.) tmankad 20140522
EMERGENT_PKGS="checkinstall subversion cmake g++ $USE_LIBCOIN libreadline6-dev libgsl0-dev zlib1g-dev libode-sp-dev libpng-dev libjpeg-dev libncurses-dev libsvn-dev"

# Packages needed to build Quarter.  It also requires some of the
# ones already listed in EMERGENT_PKGS, but this one is specifically
# needed for building Quarter (otherwise compile error).
QUARTER_PKGS="pkg-config"

# Use a separate xterm window to track progress
XTERM=`which xterm`
if [ -z "$DISPLAY" ]; then XTERM=""; fi

echo -e "\nInstalling packages needed to build (log will open in separate xterm)..."
echo "  (ctrl-c in *this* window will kill the package install process)"
OUTPUT="apt-get-install-output.txt"
if [ -x "$XTERM" ]; then
  $XTERM -si -geometry 160x50 -T "apt-get install progress (safe to close this window)" -e tail -F $OUTPUT &
  sudo apt-get -q -y install $DEBUILD_PKGS $EMERGENT_PKGS $QUARTER_PKGS 2>&1 > $OUTPUT
else
  sudo apt-get -q -y install $DEBUILD_PKGS $EMERGENT_PKGS $QUARTER_PKGS 2>&1 | tee $OUTPUT
fi

# This may fail (expectedly) if the packages aren't already installed,
# so allow it with the echo alternative (otherwise set -e would bail).
if [ "$BUILD_QUARTER" == "y" ]; then
  echo "Removing any existing libquarter and emergent installations..."
  sudo apt-get -q -y remove emergent libquarter libquarter0 | sed 's/^/  /' || echo "(OK)"
else
  echo "Removing any existing emergent installations..."
  sudo apt-get -q -y remove emergent libquarter | sed 's/^/  /' || echo "(OK)"
fi

# Make sure we don't keep an old copy of libquarter0 in the cache,
# since later we need to install the one we built.
sudo apt-get -qq autoremove | sed 's/^/  /'

# If we're not already in the build scripts directory,
# then get it and change into it
if [ ! -x ./ubuntu-motu-emergent ]; then
  svn checkout http://grey.colorado.edu/svn/emergent/emergent/trunk/package
  cd package
fi

if [ "$BUILD_QUARTER" == "y" ]; then
  echo -e "\nBuilding and packaging Quarter (log will open in separate xterm)..."
  echo "  (ctrl-c in *this* window will kill the build/package process)"
  OUTPUT="libQuarter-build-output.txt"
  if [ -x "$XTERM" ]; then
    $XTERM -si -geometry 160x50 -T "libQuarter build progress (safe to close this window)" -e tail -F $OUTPUT &
    if [ $QTVER == qt4 ]; then
      ./ubuntu-motu-quarter-qt4 2>&1 > $OUTPUT
    else
      ./ubuntu-motu-quarter 2>&1 > $OUTPUT
    fi
  else
    if [ $QTVER != qt5 ]; then
      echo "Expecting QT to be version 4 or 5 - exiting"
      exit $?
    fi

    if [ $QTVER == qt4 ]; then
      ./ubuntu-motu-quarter-qt4 2>&1 | tee $OUTPUT
    else
      ./ubuntu-motu-quarter 2>&1 | tee $OUTPUT
    fi
  fi
  echo -e "\nInstalling the Quarter libraries before building emergent..."
  sudo dpkg -i /tmp/libquarter0_*.deb
fi

echo -e "\nBuilding and packaging emergent (log will open in separate xterm)..."
echo "  (ctrl-c in *this* window will kill the build/package process)"
OUTPUT="emergent-build-output.txt"

if [ -x "$XTERM" ]; then
  $XTERM -si -geometry 160x50 -T "emergent build progress (safe to close this window)" -e tail -F $OUTPUT &
  ./ubuntu-motu-emergent $REV $TAG $QTVER 2>&1 > $OUTPUT
else
  ./ubuntu-motu-emergent $REV $TAG $QTVER 2>&1 | tee $OUTPUT
fi

# .. in a symlinked directory doesn't work as expected, so .deb files end up in ~/ rather than in /tmp/.
# Tilde (~) doesn't expand inside quotes, so use ${HOME}
# Put them back where the script is expecting them.
TEMP_FILES="${HOME}/emergent*.deb ${HOME}/libquarter*.deb ${HOME}/emergent_*.build ${HOME}/emergent_*.changes ${HOME}/libquarter_*.build ${HOME}/libquarter_*.changes"
mv ${TEMP_FILES} /tmp 2> /dev/null || echo "Files moved to /tmp: ${TEMP_FILES}"

DEBS="/tmp/emergent*.deb"
if [ "$BUILD_QUARTER" == "y" ]; then
  DEBS="/tmp/libquarter*.deb ${DEBS}"
fi

cat <<INSTRUCTIONS

Done!


** If all went well, copy the libquarter*.deb and emergent*.deb files
** to your home directory on grey and run update-ubuntu-repo.sh
**
** scp ${DEBS} username@grey.colorado.edu:/home/username/$REPONAME/
** ssh username@grey.colorado.edu
** sudo update-ubuntu-repo.sh

INSTRUCTIONS

GREY_USER=""
read -p "Please enter your user name on grey.colorado.edu: " GREY_USER

scp ${DEBS} $GREY_USER@grey.colorado.edu:/home/$GREY_USER/$REPONAME
