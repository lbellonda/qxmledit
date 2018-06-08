#!/bin/sh
# rem this file is part of QXmlEdit
if test -f src/QXmlEdit.pro; then lrelease src/QXmlEdit.pro;fi
if test -f src/QXmlEditWidget.pro; then lrelease src/QXmlEditWidget.pro;fi
if test -f src/sessions/QXmlEditSessions.pro; then lrelease src/sessions/QXmlEditSessions.pro;fi
qmake "CONFIG+=release" "INST_DATA_DIR = override_INST_DATA_DIR" "INST_DIR = override_INST_DIR" \
 "INST_DOC_DIR = override_INST_DOC_DIR" "INST_LIB_DIR = override_INST_LIB_DIR" "INST_INCLUDE_DIR = override_INST_INCLUDE_DIR" \
 "INST_TRANSLATIONS_DIR = override_INST_TRANSLATIONS_DIR" "COMPILE_DISABLE_TIPS = override_COMPILE_DISABLE_TIPS" \
 "DONTUSE_QWTPLOT = override_DONTUSE_QWTPLOT" "INST_AVOID_PRECOMP_HEADERS = override_INST_AVOID_PRECOMP_HEADERS" \
 "INST_USE_C11 = override_INST_USE_C11 "
make $1

