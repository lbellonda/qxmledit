#!/bin/sh
# rem this file is part of QXmlEdit
if test -f src/QXmlEdit.pro; then lrelease src/QXmlEdit.pro;fi
if test -f src/QXmlEditWidget.pro; then lrelease src/QXmlEditWidget.pro;fi
if test -f src/sessions/QXmlEditSessions.pro; then lrelease src/sessions/QXmlEditSessions.pro;fi

export QXMLEDIT_INST_DATA_DIR=/mnt/ramdisk/qxmledit/data
export QXMLEDIT_INST_DIR=/mnt/ramdisk/qxmledit
export QXMLEDIT_INST_DOC_DIR=/mnt/ramdisk/qxmledit/doc
export QXMLEDIT_INST_LIB_DIR=/mnt/ramdisk/qxmledit/lib
export QXMLEDIT_INST_INCLUDE_DIR=/mnt/ramdisk/qxmledit/include
export QXMLEDIT_INST_TRANSLATIONS_DIR=/mnt/ramdisk/qxmledit/po
export QXMLEDIT_INST_ICON_DIR=/mnt/ramdisk/qxmledit/icon
export QXMLEDIT_INST_METAINFO_DIR=/mnt/ramdisk/qxmledit/metainfo
export QXMLEDIT_COMPILE_DISABLE_TIPS=true
export QXMLEDIT_NO_QWTPLOT=Y
export QXMLEDIT_INST_USE_C11=y
export QXMLEDIT_INST_AVOID_PRECOMP_HEADERS=Y
export QXMLEDIT_UNIX_LOWERCASE_NAME=0
export QXMLEDIT_DISABLE_SCXML=Y
export QXMLEDIT_INST_DISABLE_COMPILE_WARNINGS=Y
export QXMLEDIT_LIB_SUFFIX=Y
export QXMLEDIT_NOVERSIONED=Y
export QXMLEDIT_USE_FAKE_SOURCES=y

qmake "CONFIG+=release"
make $1

