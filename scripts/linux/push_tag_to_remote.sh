#!/bin/sh
# Copyright 2010 Luca Bellonda
# The author grants unlimited permission to
# copy, distribute and modify this script

# this script creates a tag
# push_tag_to_remote (env): pushes the tag tagname to remote

#REQUIRES ENVIRONMENT VARIABLE  ENV_APPVERSION
# use it in a empty folder

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
echo "the tag '$APPVERSION' will be pushed to remote"

while true; do
    read -p "Do you want to continue [y/n]?" yesno
    case $yesno in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer y or n.";;
    esac
done

git push origin $APPVERSION
