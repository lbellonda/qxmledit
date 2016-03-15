#!/bin/sh
# Copyright 2010 Luca Bellonda
# The author grants unlimited permission to
# copy, distribute and modify this script

# this script should be launched in the main project directory
# it builds a linux binary package
# pkglinux.sh <tagname>: creates the archive of the given tag

#REQUIRES ENVIRONMENT VARIABLE  ENV_APPVERSION
# use it in a empty folder

APPNAME=QXmlEdit
APPVERSION=$ENV_APPVERSION
BASEDIRNAME=qxmledit
DIRNAME=tagsrc_${BASEDIRNAME}-${APPVERSION}
BUILDDIR=build
TAG_NAME=$ENV_APPVERSION

if [ -z "$APPVERSION" ]; then
    echo "ENV_APPVERSION not set"
    exit 1
fi

echo "application:       $APPNAME-$APPVERSION"
echo "directory:         $DIRNAME"
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

mkdir $DIRNAME

#local release
git archive --format tgz --output ${DIRNAME}/qxmledit-${APPVERSION}-src.tgz --prefix=qxmledit-${APPVERSION}/ ${TAG_NAME}

if [ $? -ne 0 ]; then
    echo "unable to build package"
    exit -1
fi

openssl sha256 ${DIRNAME}/qxmledit-${APPVERSION}-src.tgz >${DIRNAME}/qxmledit-${APPVERSION}-src.tgz.sha256

echo 'done'
exit 0;

