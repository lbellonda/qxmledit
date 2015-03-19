#!/bin/sh
# Copyright 2010 Luca Bellonda
# The author grants unlimited permission to
# copy, distribute and modify this script

# this script should be launched in the main project directory
# it builds a linux binary package

#deve fare due cose:
# 1 - fare un tar dei sorgenti del tag
# 2 - compilare ed eseguire un tar dei binari
# utilizza la directory di estrazione, quindi deve operare sul checkout di un tag
# directory name is app+version

#REQUIRES ENVIRONMENT VARIABLE  ENV_APPVERSION
# use it in a empty folder

APPNAME=QXmlEdit
APPVERSION=$ENV_APPVERSION
BASEDIRNAME=qxmledit
DIRNAME=$BASEDIRNAME-$APPVERSION
BUILDDIR=build
#PKGNAME="$APPNAME-$APPVERSION-bin-ubuntu-qt4.7-32bit"

echo "application:       $APPNAME-$APPVERSION"
echo "directory:         $DIRNAME"
#echo "build directory:   $BUILDDIR"
#echo "package:           $PKGNAME"
echo "a directory '$BASEDIRNAME-$APPVERSION' will be created "

while true; do
    read -p "Do you want to continue [y/n]?" yesno
    case $yesno in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer y or n.";;
    esac
done
#deve controllare l'esecuzione

mkdir $BASEDIRNAME-$APPVERSION

cd $BASEDIRNAME-$APPVERSION

svn export https://qxmledit.googlecode.com/svn/tags/qxmledit-$APPVERSION $BASEDIRNAME-$APPVERSION

tar -czf qxmledit-$APPVERSION-src.tgz --exclude='doc/design' --exclude='doc/images' --exclude='doc/tutorial' --exclude='doc/headers' --exclude='doc/*.svg' --exclude='doc/*.svgz' $BASEDIRNAME-$APPVERSION

exit 0;



#cd ..
#tar -czf $APPNAME-$APPVERSION-sources.tgz $DIRNAME
#cd $DIRNAME

#cd src
#qmake -config RELEASE
#make
#cd ..
## costruisce il package binario

#strip $BUILDDIR/$APPNAME

#### create image ###############################################

#echo "Creating binary package"
#imagedir="$BUILDDIR/linux_bundle/$PKGNAME"
#SHAREDRESOURCES="$imagedir"

#rm -rf $imagedir
#mkdir -p $imagedir
#cp -R $BUILDDIR/$APPNAME $imagedir

#### copy specific resources ########################
#mkdir -p $SHAREDRESOURCES
#cp -p src/data/sample.style $SHAREDRESOURCES
#cp -pr src/data/snippets $SHAREDRESOURCES
#cp -pr src/languages/*.qm $SHAREDRESOURCES/translations
#cp -p README $SHAREDRESOURCES
#cp -p COPYING $SHAREDRESOURCES
#cp -p GPLV3.txt $SHAREDRESOURCES
#cp -p LGPLV3.txt $SHAREDRESOURCES
#cp -p doc/QXmlEdit_manual.pdf $SHAREDRESOURCES

#### misc cleanup ###############################################
#find $imagedir | egrep ".svn" | xargs rm -rf

#### final build #############################################
#CURRDIR=$(pwd)

##imagedir
#cd $imagedir/..

##cd ..
##tar -czf ../../$PKGNAME.tgz $imagedir
##pushd
#tar -czf $PKGNAME.tgz $PKGNAME
#cp -p $PKGNAME.tgz $CURRDIR/..
#cd $CURRDIR
##popd
#echo "Done"

