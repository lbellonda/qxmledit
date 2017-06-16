#!/bin/sh
# this file is part of QXmlEdit
# push_branch_to_remote.sh: pushes a branch to remote
export BRANCH=$1
if [ -z $BRANCH  ]; then
    echo "branch not set"
    exit -1
fi
git push -u origin $BRANCH

