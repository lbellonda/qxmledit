#!/bin/sh
# rem this file is part of QXmlEdit
if test -f src/QXmlEdit.pro; then lrelease src/QXmlEdit.pro;fi
if test -f src/QXmlEditWidget.pro; then lrelease src/QXmlEditWidget.pro;fi
if test -f src/sessions/QXmlEditSessions.pro; then lrelease src/sessions/QXmlEditSessions.pro;fi

export QXMLEDIT_INST_DATA_DIR=/mnt/ramdisk/qxmledit/data
export QXMLEDIT_INST_DIR=/mnt/ramdisk/qxmledit
export QXMLEDIT_INST_DOC_DIR=/mnt/ramdisk/qxmledit/doc
export QXMLEDIT_INST_LIB_DIR=/mnt/ramdisk/qxmledit/lib
export QXMLEDIT_INST_TRANSLATIONS_DIR=/mnt/ramdisk/qxmledit/po
export QXMLEDIT_COMPILE_DISABLE_TIPS=true
export QXMLEDIT_NO_QWTPLOT=Y
export QXMLEDIT_INST_USE_C11=y
export QXMLEDIT_INST_AVOID_PRECOMP_HEADERS=Y
export QXMLEDIT_UNIX_LOWERCASE_NAME=y
export LIB_VERSIONED=Y

qmake "CONFIG+=release"
make $1

