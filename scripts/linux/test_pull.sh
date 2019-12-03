#!/bin/sh
# this file is part of QXmlEdit
# pull a PR on a local branch
export PR=$1
if [ -z $PR  ]; then
    echo "PR not set"
    exit -1
fi
git fetch origin pull/${PR}/head:test_pull_${PR}
git checkout test_pull_${PR}
