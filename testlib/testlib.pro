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

#-------------------------------------------------
#
# Project created by QtCreator 2012-01-04T14:29:49
#
#-------------------------------------------------

include("../src/version.pri")

#default value for the lib version name
LIB_VERSIONED_DEFAULT=""
linux: {
   LIB_VERSIONED_DEFAULT="1"
}


equals(LIB_VERSIONED, "") {
    LIB_VERSIONED = $$LIB_VERSIONED_DEFAULT
}

QXMLEDIT_LIB_SUFFIX = ""
!equals(LIB_VERSIONED, "") {
    QXMLEDIT_LIB_SUFFIX = -$$QXMLEDIT_VERSION
}

equals(QXMLEDIT_LIB_SUFFIX, "") {
    VERSION=""
}


QT  += core gui xml xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += printsupport widgets
}

greaterThan(QT_MAJOR_VERSION, 4) {
win32 {
    DEFINES += "_NO_W32_PSEUDO_MODIFIERS"
    DEFINES += "NOGDI"
}
}

TARGET = testlibApp
TEMPLATE = app


DESTDIR = ../build
OBJECTS_DIR = ../build/testlib/obj
MOC_DIR = ../build/testlib/moc
UI_DIR = ../build/testlib/ui
UI_HEADERS_DIR = ../build/testlib/ui/include
UI_SOURCES_DIR = ../build/testlib/ui/src
RCC_DIR = ../build/testlib/rcc

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

INCLUDEPATH += $$PWD/../src/globals/includes
INCLUDEPATH += $$PWD/../src/framework/include
DEPENDPATH += $$PWD/../src/globals/includes
DEPENDPATH += $$PWD/../src/framework/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../build/lib/release/ -L$$OUT_PWD/../build -lQXmlEditWidget$${QXMLEDIT_LIB_SUFFIX}
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../build/lib/debug/ -L$$OUT_PWD/../build -lQXmlEditWidget$${QXMLEDIT_LIB_SUFFIX}
else:symbian: LIBS += -lQXmlEditWidget$${QXMLEDIT_LIB_SUFFIX}
else:unix: LIBS += -L$$OUT_PWD/../build/ -lQXmlEditWidget$${QXMLEDIT_LIB_SUFFIX}

