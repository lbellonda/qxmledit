#/**************************************************************************
# *  This file is part of QXmlEdit                                         *
# *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
# *    as indicated in the AUTHORS file                                    *
# *  lbellonda _at_ gmail.com                                              *
# *                                                                        *
# * This library is free software; you can redistribute it and/or          *
# * modify it under the terms of the GNU Library General Public            *
# * License as published by the Free Software Foundation; either           *
# * version 2 of the License, or (at your option) any later version.       *
# *                                                                        *
# * This library is distributed in the hope that it will be useful,        *
# * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
# * Library General Public License for more details.                       *
# *                                                                        *
# * You should have received a copy of the GNU Library General Public      *
# * License along with this library; if not, write to the                  *
# * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,       *
# * Boston, MA  02110-1301  USA                                            *
# **************************************************************************/
#
# Project created by QtCreator 2011-08-15T09:23:52
#
#-------------------------------------------------

############################ BEGIN INSTALLATION FOLDERS DECLARATION ###########################################

#message(QXMLEDIT_INST_DATA_DIR = $$(QXMLEDIT_INST_DATA_DIR))
#message(QXMLEDIT_INST_DIR = $$(QXMLEDIT_INST_DIR))
#message(QXMLEDIT_INST_DOC_DIR = $$(QXMLEDIT_INST_DOC_DIR))
#message(QXMLEDIT_INST_LIB_DIR = $$(QXMLEDIT_INST_LIB_DIR))
#message(QXMLEDIT_INST_TRANSLATIONS_DIR = $$(QXMLEDIT_INST_TRANSLATIONS_DIR))
#message(QXMLEDIT_UNIX_LOWERCASE_NAME = $$(QXMLEDIT_UNIX_LOWERCASE_NAME))

INST_DATA_DIR=$$(QXMLEDIT_INST_DATA_DIR)
isEmpty(INST_DATA_DIR) {
    INST_DATA_DIR = /opt/qxmledit
}

INST_DIR=$$(QXMLEDIT_INST_DIR)
isEmpty(INST_DIR) {
    INST_DIR = /opt/qxmledit
}

INST_DOC_DIR=$$(QXMLEDIT_INST_DOC_DIR)
isEmpty(INST_DOC_DIR) {
    INST_DOC_DIR = /opt/qxmledit
}

INST_LIB_DIR=$$(QXMLEDIT_INST_LIB_DIR)
isEmpty(INST_LIB_DIR) {
    INST_LIB_DIR = /opt/qxmledit
}

INST_TRANSLATIONS_DIR=$$(QXMLEDIT_INST_TRANSLATIONS_DIR)
isEmpty(INST_TRANSLATIONS_DIR) {
    INST_TRANSLATIONS_DIR=$$INST_DATA_DIR/translations
}

USE_QWTPLOT=$$(QXMLEDIT_USE_QWTPLOT)
isEmpty(USE_QWTPLOT) {
    USE_QWTPLOT="N"
}

TARGET_NAME_UNIXSTYLE=$$(QXMLEDIT_UNIX_LOWERCASE_NAME)

############################ END INSTALLATION FOLDERS DECLARATION #############################################

include("../src/version.pri")

#default value for the unix/Linux target name
TARGET_NAME_UNIXSTYLE_DEFAULT=""

unix:!macx: {
   TARGET_NAME_UNIXSTYLE_DEFAULT="1"
}

equals(TARGET_NAME_UNIXSTYLE, "") {
    TARGET_NAME_UNIXSTYLE = $$TARGET_NAME_UNIXSTYLE_DEFAULT
}

# This is necessary to build the executable as an app
DEFINES += LIBQXMLEDIT_LIBRARY_STATIC
DEFINES += QXMLEDITSESSIONS_LIBRARY_STATIC

QT       += gui xml xmlpatterns svg testlib network sql

macx: {
    QT       += macextras
}

greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += printsupport widgets core
}

greaterThan(QT_MAJOR_VERSION, 4) {
win32 {
    DEFINES += "_NO_W32_PSEUDO_MODIFIERS"
    DEFINES += "NOGDI"
}
}

lessThan(QT_MAJOR_VERSION, 5) {
    QMAKE_CXXFLAGS +=-Wno-unused-local-typedefs
}

QMAKE_CXXFLAGS +=-Werror
QMAKE_CXXFLAGS +=-Wall

TARGET = QXmlEdit

TEMPLATE = app

DESTDIR = ../build

CONFIG += precompile_header
PRECOMPILED_HEADER  = ../src/precompiled_app.h
HEADERS += ../src/precompiled_app.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"

# object files
OBJECTS_DIR = ../build/singleapp/obj
MOC_DIR = ../build/singleapp/moc
UI_DIR = ../build/singleapp/ui
UI_HEADERS_DIR = ../build/singleapp/ui/include
UI_SOURCES_DIR = ../build/singleapp/ui/src
RCC_DIR = ../build/singleapp/rcc

INCLUDEPATH += ../src
INCLUDEPATH += ../sessions/include
INCLUDEPATH += ../framework/include
INCLUDEPATH += ../globals/includes
INCLUDEPATH += ../src/sessions

RESOURCES += ../src/sessions/sessions.qrc

ICON = ../src/images/qxmledit.icns
win32:RC_FILE = ../src/win.rc
os2:RC_FILE = ../src/os2.rc

DEFINES += TRANLASTION_DIR= translations

macx:QMAKE_INFO_PLIST    = ../src/QXmlEdit.plist

TRANSLATIONS = \
   ../src/languages_app/QXmlEdit_cs.ts \
    ../src/languages_lib/QXmlEditlib_cs.ts \
    ../src/sessions/languages/QXmlEditSessions_cs.ts
TRANSL = \
   ../src/languages_app/QXmlEdit_cs.qm \
    ../src/languages_lib/QXmlEditlib_cs.qm \
    ../src/sessions/languages/QXmlEditSessions_cs.qm


target.path = $$INST_DIR
translationsfiles.path = $$INST_TRANSLATIONS_DIR
translationsfiles.files = $$TRANSL


INSTALLS += target translationsfiles

#-----------------------------------------------------------
styles.path = $$INST_DATA_DIR
styles.files = data/sample.style
snippets.path = $$INST_DATA_DIR/snippets
snippets.files = data/snippets/{6d11b8d1-e285-46b2-8375-79e17cab9862}.xml \
    data/snippets/{b3303cb3-8537-46d0-943b-cf898cc93b3f}.xml \
    data/snippets/{bd4984f5-107e-48ca-9780-c263ff1aa58d}.xml \
    data/snippets/{d78f4e09-4003-4178-adec-134a7b996d6e}.xml \
    data/snippets/{e40792fb-16de-4418-9320-4feaa92c28a3}.xml \
    data/snippets/{edc60671-0ae0-401b-989d-5d0b82976bce}.xml \
    data/snippets/{f6d7ebdd-c505-4a6a-8a16-f7c99581a995}.xml
manual.path = $$INST_DOC_DIR
manual.files = ../doc/QXmlEdit_manual.pdf

desktopInfo.path = $$INST_DATA_DIR
desktopInfo.files = ../install_scripts/environment/desktop/QXmlEdit.desktop \
    ../install_scripts/environment/desktop/QXmlEdit.appdata.xml

#-----------------------------------------------------------

# resources are defined in paths.h
unix:!macx:DEFINES += UNIX_RESOURCES

DEFINES += UNIX_RESOURCE_PATH=$$INST_DATA_DIR
DEFINES += UNIX_DOC_PATH=$$INST_DOC_DIR

unix:INSTALLS += snippets
unix:INSTALLS += styles
unix:INSTALLS += manual
unix:INSTALLS += desktopInfo

contains(USE_QWTPLOT, Y) {
    QT += opengl
    LIBS += -lqwtplot3d-qt4
    DEFINES += QWT_PLOT3D
    #LIBS += -L$$OUT_PWD/../../../qwtplot/qwtplot3d/lib -lqwtplot3d
}

include(singleapp.pri)
