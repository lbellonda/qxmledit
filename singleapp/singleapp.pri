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

SOURCES += $$files(../src/*.cpp)
SOURCES += $$files(../src/widgets/*.cpp)
SOURCES += $$files(../src/xsdeditor/*.cpp)
SOURCES += $$files(../src/xsdeditor/widgets/*.cpp)
SOURCES += $$files(../src/xsdeditor/validator/*.cpp)
SOURCES += $$files(../src/xsdeditor/items/*.cpp)
SOURCES += $$files(../src/xsdeditor/io/*.cpp)
SOURCES += $$files(../src/xsdeditor/names/*.cpp)
SOURCES += $$files(../src/config/*.cpp)
SOURCES += $$files(../src/extraction/*.cpp)
SOURCES += $$files(../src/balsamiqsrc/*.cpp)
SOURCES += $$files(../src/sessions/*.cpp)
SOURCES += $$files(../src/sessions/widgets/*.cpp)
SOURCES += $$files(../src/services/*.cpp)
SOURCES += $$files(../src/filter/*.cpp)
SOURCES += $$files(../src/visualization/*.cpp)
SOURCES += $$files(../src/undo/*.cpp)
SOURCES += $$files(../src/sessions/data_access/*.cpp)
SOURCES += $$files(../src/sessions/data_access/model/*.cpp)
SOURCES += $$files(../src/modules/anonymize/*.cpp)
SOURCES += $$files(../src/modules/binaryviewer/*.cpp)
SOURCES += $$files(../src/modules/compare/*.cpp)
SOURCES += $$files(../src/modules/graph/*.cpp)
SOURCES += $$files(../src/modules/search/*.cpp)
SOURCES += $$files(../src/modules/copyattr/*.cpp)
SOURCES += $$files(../src/modules/services/*.cpp)
SOURCES += $$files(../src/modules/uiutil/*.cpp)
SOURCES += $$files(../src/modules/widgets/*.cpp)
SOURCES += $$files(../src/modules/xslt/*.cpp)
SOURCES += $$files(../src/modules/metadata/*.cpp)
SOURCES += $$files(../src/modules/xml/*.cpp)
SOURCES += $$files(../src/modules/delegates/*.cpp)
SOURCES += $$files(../src/modules/encoding/*.cpp)
SOURCES += $$files(../src/modules/xsd/*.cpp)
SOURCES += $$files(../src/modules/namespace/*.cpp)


HEADERS += $$files(../src/*.h)
HEADERS += $$files(../src/widgets/*.h)
HEADERS += $$files(../src/xsdeditor/*.h)
HEADERS += $$files(../src/xsdeditor/widgets/*.h)
HEADERS += $$files(../src/xsdeditor/validator/*.h)
HEADERS += $$files(../src/xsdeditor/items/*.h)
HEADERS += $$files(../src/xsdeditor/io/*.h)
HEADERS += $$files(../src/xsdeditor/names/*.h)
HEADERS += $$files(../src/config/*.h)
HEADERS += $$files(../src/extraction/*.h)
HEADERS += $$files(../src/balsamiqsrc/*.h)
HEADERS += $$files(../src/services/*.h)
HEADERS += $$files(../src/undo/*.h)
HEADERS += $$files(../src/sessions/*.h)
HEADERS += $$files(../src/filter/*.h)
HEADERS += $$files(../src/visualization/*.h)
HEADERS += $$files(../src/sessions/widgets/*.h)
HEADERS += $$files(../src/sessions/data_access/*.h)
HEADERS += $$files(../src/sessions/data_access/model*.h)
HEADERS += $$files(../src/framework/include/*.h)
HEADERS += $$files(../src/globals/includes/*.h)
HEADERS += $$files(../src/modules/anonymize/*.h)
HEADERS += $$files(../src/modules/binaryviewer/*.h)
HEADERS += $$files(../src/modules/compare/*.h)
HEADERS += $$files(../src/modules/graph/*.h)
HEADERS += $$files(../src/modules/search/*.h)
HEADERS += $$files(../src/modules/copyattr/*.h)
HEADERS += $$files(../src/modules/uiutil/*.h)
HEADERS += $$files(../src/modules/services/*.h)
HEADERS += $$files(../src/modules/widgets/*.h)
HEADERS += $$files(../src/modules/xslt/*.h)
HEADERS += $$files(../src/modules/metadata/*.h)
HEADERS += $$files(../src/modules/xml/*.h)
HEADERS += $$files(../src/modules/delegates/*.h)
HEADERS += $$files(../src/modules/encoding/*.h)
HEADERS += $$files(../src/modules/xsd/*.h)
HEADERS += $$files(../src/modules/namespace/*.h)

INCLUDEPATH += ../src
INCLUDEPATH += ../src/sessions/include
INCLUDEPATH += ../src/sessions
INCLUDEPATH += ../src/framework/include
INCLUDEPATH += ../src/globals/includes

DEPENDPATH += ../src/globals/includes
DEPENDPATH += ../src/modules/anonymize
DEPENDPATH += ../src/modules/binaryviewer
DEPENDPATH += ../src/modules/compare
DEPENDPATH += ../src/modules/graph
DEPENDPATH += ../src/modules/search
DEPENDPATH += ../src/modules/copyattr
DEPENDPATH += ../src/modules/uiutil
DEPENDPATH += ../src/modules/widgets
DEPENDPATH += ../src/modules/xslt
DEPENDPATH += ../src/modules/metadata
DEPENDPATH += ../src/modules/xml
DEPENDPATH += ../src/modules/delegates
DEPENDPATH += ../src/modules/encoding
DEPENDPATH += ../src/modules/xsd
DEPENDPATH += ../src/modules/namespace
DEPENDPATH += ../src
DEPENDPATH += .

FORMS += $$files(../src/balsamiqsrc/*.ui)
FORMS += $$files(../src/*.ui)
FORMS += $$files(../src/filter/*.ui)
FORMS += $$files(../src/widgets/*.ui)
FORMS += $$files(../src/xsdeditor/*.ui)
FORMS += $$files(../src/xsdeditor/widgets/*.ui)
FORMS += $$files(../src/xsdeditor/validator/*.ui)
FORMS += $$files(../src/xsdeditor/items/*.ui)
FORMS += $$files(../src/config/*.ui)
FORMS += $$files(../src/visualization/*.ui)
FORMS += $$files(../src/extraction/*.ui)
FORMS += $$files(../src/sessions/widgets/*.ui)
FORMS += $$files(../src/modules/anonymize/*.ui)
FORMS += $$files(../src/modules/binaryviewer/*.ui)
FORMS += $$files(../src/modules/compare/*.ui)
FORMS += $$files(../src/modules/graph/*.ui)
FORMS += $$files(../src/modules/copyattr/*.ui)
FORMS += $$files(../src/modules/xslt/*.ui)
FORMS += $$files(../src/modules/search/*.ui)
FORMS += $$files(../src/modules/metadata/*.ui)
FORMS += $$files(../src/modules/encoding/*.ui)
FORMS += $$files(../src/modules/xsd/*.ui)
FORMS += $$files(../src/modules/xml/*.ui)
FORMS += $$files(../src/modules/namespace/*.ui)

RESOURCES = ../src/risorse.qrc
