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
export QXMLEDIT_COMPILE_DISABLE_TIPS=true
export QXMLEDIT_NO_QWTPLOT=Y
export QXMLEDIT_INST_USE_C11=y
export QXMLEDIT_INST_AVOID_PRECOMP_HEADERS=Y
export QXMLEDIT_UNIX_LOWERCASE_NAME=0
export QXMLEDIT_VERSIONED=Y
export QXMLEDIT_DISABLE_SCXML=Y

qmake "CONFIG+=release" \
 "QXMLEDIT_INST_DATA_DIR = /mnt/ramdisk/override/INST_DATA_DIR" \
 "QXMLEDIT_INST_DIR = /mnt/ramdisk/override/INST_DIR" \
 "QXMLEDIT_INST_DOC_DIR = /mnt/ramdisk/override/INST_DOC_DIR" \
 "QXMLEDIT_INST_LIB_DIR = /mnt/ramdisk/override/INST_LIB_DIR" \
 "QXMLEDIT_INST_INCLUDE_DIR = /mnt/ramdisk/override/INST_INCLUDE_DIR" \
 "QXMLEDIT_INST_TRANSLATIONS_DIR = /mnt/ramdisk/override/INST_TRANSLATIONS_DIR" \
 "QXMLEDIT_COMPILE_DISABLE_TIPS = override_COMPILE_DISABLE_TIPS" \
 "QXMLEDIT_NO_QWTPLOT = override_DONTUSE_QWTPLOT" \
 "QXMLEDIT_INST_AVOID_PRECOMP_HEADERS = override_INST_AVOID_PRECOMP_HEADERS" \
 "QXMLEDIT_INST_USE_C11 = override_INST_USE_C11 " \
 "QXMLEDIT_VERSIONED = override_QXMLEDIT_VERSIONED" \
 "QXMLEDIT_DISABLE_SCXML = override_QXMLEDIT_DISABLE_SCXML" \
 "QXMLEDIT_UNIX_LOWERCASE_NAME = override_QXMLEDIT_UNIX_LOWERCASE_NAME"
make $1

