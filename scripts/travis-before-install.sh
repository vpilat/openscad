#!/bin/bash

DIST="$1"

#
# This script must be run as root to install packages.
#
# Simple way for testing, e.g. replace command calls
# with echo.
#
# WGET=echo ./scripts/travis-before-install.sh trusty
#
WGET=${WGET:-wget}

QUIET="-qq"

echo "DIST = $DIST / id = $(id) / Running on:"
echo
cat /etc/os-release
echo

"$WGET" -qO - http://files.openscad.org/OBS-Repository-Key.pub | apt-key add -

PACKAGES='
	build-essential
	libqt4-dev
	libqt4-opengl-dev
	libxmu-dev
	cmake
	bison
	flex
	git-core
	libboost-all-dev
	libXi-dev
	libmpfr-dev
	libboost-dev
	libglew-dev
	libeigen3-dev
	libcgal-dev
	libgmp3-dev
	libgmp-dev
	curl
	imagemagick
	libfontconfig-dev
	libopencsg-dev
'

# Purge fglrx driver if that is installed by default
# as this contains a libGL that breaks the GLX extension
# in Xvfb causing all graphical tests to fail.
apt-get update "$QUIET"
apt-get purge "$QUIET" fglrx

if [[ "$DIST" == "trusty" ]]
then

	echo "Adding external repositories for $DIST"
	echo 'yes' | add-apt-repository 'deb http://download.opensuse.org/repositories/home:/t-paul:/lib3mf/xUbuntu_14.04/ ./'

	apt-get update "$QUIET"
	apt-get install "$QUIET" $PACKAGES libharfbuzz-dev lib3mf-dev

elif [[ "$DIST" == "precise" ]]
then

	echo 'yes' | add-apt-repository ppa:chrysn/openscad
	echo 'yes' | add-apt-repository 'deb http://download.opensuse.org/repositories/home:/t-paul:/lib3mf/xUbuntu_12.04/ ./'
	apt-get update "$QUIET"
	apt-get install "$QUIET" $PACKAGES lib3mf-dev

	echo 'yes' | add-apt-repository ppa:mapnik/nightly-trunk
	apt-get update "$QUIET"
	apt-get install "$QUIET" libharfbuzz-dev

	echo 'yes' | add-apt-repository ppa:oibaf/graphics-drivers
	apt-get update "$QUIET"
	apt-get install "$QUIET" --install-recommends libgl1-mesa-dev-lts-quantal

else

	echo "WARNING: No install recipe for distribution '$DIST'"

fi
