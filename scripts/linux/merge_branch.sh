#!/bin/sh
# this file is part of QXmlEdit
# merge_branch.sh <branch name>: merges the given branch to master
export BRANCH=$1
if [ -z $BRANCH  ]; then
    echo "branch not set"
    exit -1
fi

echo "merging the branch $1"
git checkout master
if [ $? -ne 0 ]; then
    echo "cannot switch to master"
    exit -1
fi
git merge  $BRANCH


