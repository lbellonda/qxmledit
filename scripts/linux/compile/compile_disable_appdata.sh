#!/bin/sh
# rem this file is part of QXmlEdit
if test -f src/QXmlEdit.pro; then lrelease src/QXmlEdit.pro;fi
if test -f src/QXmlEditWidget.pro; then lrelease src/QXmlEditWidget.pro;fi
if test -f src/sessions/QXmlEditSessions.pro; then lrelease src/sessions/QXmlEditSessions.pro;fi

unset QXMLEDIT_NO_METAINFO

qmake "CONFIG+=release" \
 "QXMLEDIT_NO_METAINFO = Y" \
 "QXMLEDIT_USE_FAKE_SOURCES = y"
make $1
make install

