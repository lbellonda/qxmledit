#!/bin/sh
# Copyright 2010 Luca Bellonda
# The author grants unlimited permission to
# copy, distribute and modify this script

# this script creates a tag

#REQUIRES ENVIRONMENT VARIABLE  ENV_APPVERSION
# use it in a empty folder
# maketag_local.sh (env): create a tag named tagname without pushing it to remote


APPNAME=QXmlEdit
APPVERSION=$ENV_APPVERSION
BASEDIRNAME=qxmledit
DIRNAME=$BASEDIRNAME-$APPVERSION
BUILDDIR=build

if [ -z "$APPVERSION" ]; then
    echo "ENV_APPVERSION not set"
    exit 1
fi

echo "application:       $APPNAME-$APPVERSION"
echo "the tag '$APPVERSION' will be created "

while true; do
    read -p "Do you want to continue [y/n]?" yesno
    case $yesno in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer y or n.";;
    esac
done

git tag -a $APPVERSION -m "tag $APPVERSION"

