#!/bin/sh
# this file is part of QXmlEdit
# merge_branch_squash.sh <branch name>: merges the given branch to master with squash of the commits
export BRANCH=$1
if [ -z $BRANCH  ]; then
    echo "branch not set"
    exit -1
fi

echo "merging and squashing the branch $1"
git checkout master
if [ $? -ne 0 ]; then
    echo "cannot switch to master"
    exit -1
fi
git merge --squash $BRANCH


