#!/bin/sh
# Copyright 2007 David Johnson
# The author grants unlimited permission to
# copy, distribute and modify this script

#export APPVERSION_RELEASE=0.8.9
#export QMAKE_PATH=~/opt/Qt5.2.0/5.2.0/clang_64/bin/
# this script should be launched in the main project directory

APPNAME=QXmlEdit
APPVERSION=$APPVERSION_RELEASE

### get required user input #########################################

if [ -z $APPVERSION_RELEASE ] ; then
    echo
    echo "please set the variable APPVERSION_RELEASE"
    exit -1
fi

if [ -z $QMAKE_PATH ] ; then
    echo
    echo "please set the variable QMAKE_PATH"
    exit -1
fi

### get system configuration ########################################

# as long as we can find qmake, we don't need QTDIR
FWPATH=`$QMAKE_PATH/qmake -query QT_INSTALL_LIBS`
if [ ! -d $FWPATH/QtGui.framework ] ; then
    echo "ERROR: cannot find the Qt frameworks. Make sure Qt is installed"
    echo "and qmake is in your environment path."
    exit
fi



imagedir="build/macos_bundle-framework"
SOURCEDIR=build/$APPNAME.app
SHAREDRESOURCES="$imagedir/$APPNAME.app/Contents/SharedSupport"

rm -rd $imagedir
mkdir -p $imagedir
cp -R $SOURCEDIR $imagedir

### dyn libs
SOURCEDIR=build/$APPNAME.app
SHAREDRESOURCES="$imagedir/$APPNAME.app/Contents/SharedSupport"
###
FRAMEWORKS="$imagedir/$APPNAME.app/Contents/Frameworks"
echo mkdiring  "$FRAMEWORKS"
mkdir -p "$FRAMEWORKS"

cp -pav build/*.dylib "$FRAMEWORKS"

$QMAKE_PATH/macdeployqt $imagedir/$APPNAME.app -verbose 2

BUNDLE=$imagedir/$APPNAME.app
APPBIN="$BUNDLE/Contents/MacOS/$APPNAME"

if [ ! -d $BUNDLE ] ; then
    echo "ERROR: cannot find application bundle \"$BUNDLE\" "
    exit
fi

if [ ! -x $APPBIN ] ; then
    echo "ERROR: cannot find application in bundle. Did you forget to run make?"
    exit
fi

echo "application: $APPNAME - $APPVERSION"
echo "bundle:      $BUNDLE"
echo "imagedir:    $imagedir"

strip $APPBIN

# remove unwanted parts
find $BUNDLE/Contents/Frameworks | egrep "debug|Headers" | xargs rm -rf

### create disk image ###############################################

echo "Creating disk image"

### copy specific resources ########################
mkdir $SHAREDRESOURCES
cp -p src/data/sample.style $SHAREDRESOURCES
cp -pr src/data/snippets $SHAREDRESOURCES/snippets
cp -pr src/languages_app/*.qm $SHAREDRESOURCES/translations
cp -pr src/languages_lib/*.qm $SHAREDRESOURCES/translations
cp -pr src/sessions/languages/*.qm $SHAREDRESOURCES/translations
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

echo "Done"
