#! /bin.sh
# this file is part of QXmlEdit
# archive_head.sh: creates the archive of the head in the current folder
git archive --format=tgz --output=qxmledit-git-head-backup.tgz HEAD
