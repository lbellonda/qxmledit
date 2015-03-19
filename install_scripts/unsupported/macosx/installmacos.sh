#!/bin/sh
# Copyright 2007 David Johnson
# The author grants unlimited permission to
# copy, distribute and modify this script

# APPNAME=MyApp

# this script should be launched in the main project directory

APPNAME=QXmlEdit
APPVERSION=0.5.1

### get system configuration ########################################

# as long as we can find qmake, we don't need QTDIR
FWPATH=`qmake -query QT_INSTALL_LIBS`
if [ ! -d $FWPATH/QtGui.framework ] ; then
    echo "ERROR: cannot find the Qt frameworks. Make sure Qt is installed"
    echo "and qmake is in your environment path."
    exit
fi

### get required user input #########################################

if [ -z $APPNAME ] ; then
    echo
    echo "This script prepares a Qt application bundle for deployment. It will"
    echo "copy over the required Qt frameworks and sets the installation"
    echo "identifications. Please see the \"Deploying an Application on Qt/Mac\""
    echo "page in the Qt documentation for more information."
    echo
    echo "This script assumes you have already built the application bundle."
    echo
    echo -n "What is the name of the application? "
    read userinput
    APPNAME=$userinput
fi


imagedir="build/macos_bundle"
SOURCEDIR=build/$APPNAME.app
SHAREDRESOURCES="$imagedir/$APPNAME.app/Contents/SharedSupport"

rm -rd $imagedir
mkdir -p $imagedir
cp -R $SOURCEDIR $imagedir

BUNDLE=$imagedir/$APPNAME.app
APPBIN="$BUNDLE/Contents/MacOS/$APPNAME"

if [ ! -d $BUNDLE ] ; then
    echo "ERROR: cannot find application bundle \"$BUNDLE\" in current directory"
    exit
fi

if [ ! -x $APPBIN ] ; then
    echo "ERROR: cannot find application in bundle. Did you forget to run make?"
    exit
fi

echo "application: $APPNAME - $APPVERSION"
echo "bundle:      $BUNDLE"
echo "imagedir:    $imagedir"

### query binary for frameworks #####################################

### make install (already done ) ####################################################

#echo "Running make install"
#if [ -e Makefile.Release ] ; then
#    make -f Makefile.Release install
#elif [ -e Makefile ] ; then
#    make install
#else
#    echo "ERROR: Makefile not found. This script requires the macx-g++ makespec"
#fi
strip $APPBIN

### create disk image ###############################################

echo "Creating disk image"
#imagedir="build/macos_bundle"
#SHAREDRESOURCES="$imagedir/$APPNAME.app/Contents/SharedSupport"

#mkdir -p $imagedir
#cp -R $BUNDLE $imagedir

### copy specific resources ########################
mkdir $SHAREDRESOURCES
cp -p src/data/sample.style $SHAREDRESOURCES
cp -pr src/data/snippets $SHAREDRESOURCES
cp -pr src/languages/*.qm $SHAREDRESOURCES/translations
cp -p README $SHAREDRESOURCES
cp -p COPYING $SHAREDRESOURCES
cp -p GPLV3.txt $SHAREDRESOURCES
cp -p LGPLV3.txt $SHAREDRESOURCES
cp -p doc/QXmlEdit_manual.pdf $SHAREDRESOURCES


### misc cleanup ###############################################

find $imagedir/$APPNAME.app/Contents | egrep ".svn" | xargs rm -rf

####
hdiutil create -ov -srcfolder $imagedir -format UDBZ -volname "$APPNAME-$APPVERSION" "build/$APPNAME-$APPVERSION.dmg"
hdiutil internet-enable -yes "build/$APPNAME-$APPVERSION.dmg"
###rm -rf $imagedir

echo "Done"

