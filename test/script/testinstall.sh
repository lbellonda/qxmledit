#!/bin/bash
#
# This script install .
#

#put a category in each folder

#1-normal install

read -p "Clean installation folders. Press [Enter] key to continue."

unset QXMLEDIT_INST_DATA_DIR
unset QXMLEDIT_INST_DIR
unset QXMLEDIT_INST_DOC_DIR
unset QXMLEDIT_INST_LIB_DIR
unset QXMLEDIT_INST_TRANSLATIONS_DIR
unset QXMLEDIT_INST_INCLUDE_DIR

touch src/QXmlEdit.pro
touch src/QXmlEditWidget.pro
touch src/sessions/QXmlEditSessions.pro

qmake
make
make install

read -p "Check installation folder. Press [Enter] key to continue."

read -p "Clean installation folders. Press [Enter] key to continue."

export QXMLEDIT_INST_DATA_DIR=/opt/qxmledit/q/data
export QXMLEDIT_INST_DIR=/opt/qxmledit/q/exe
export QXMLEDIT_INST_DOC_DIR=/opt/qxmledit/q/doc
export QXMLEDIT_INST_LIB_DIR=/opt/qxmledit/q/lib
export QXMLEDIT_INST_TRANSLATIONS_DIR=/opt/qxmledit/q/transl
export QXMLEDIT_INST_INCLUDE_DIR=/opt/qxmledit/q/includes

touch src/QXmlEdit.pro
touch src/QXmlEditWidget.pro
touch src/sessions/QXmlEditSessions.pro

qmake
make
make install

read -p "Check installation folder. Press [Enter] key to continue."
unset QXMLEDIT_INST_DATA_DIR
unset QXMLEDIT_INST_DIR
unset QXMLEDIT_INST_DOC_DIR
unset QXMLEDIT_INST_LIB_DIR
unset QXMLEDIT_INST_TRANSLATIONS_DIR
unset QXMLEDIT_INST_INCLUDE_DIR
