#/**************************************************************************
# *  This file is part of QXmlEdit                                         *
# *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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

INST_AVOID_PRECOMP_HEADERS=$$(QXMLEDIT_INST_AVOID_PRECOMP_HEADERS)
isEmpty(INST_AVOID_PRECOMP_HEADERS) {
    INST_AVOID_PRECOMP_HEADERS = ""
}

INST_USE_C11=$$(QXMLEDIT_INST_USE_C11)
isEmpty(INST_USE_C11) {
    INST_USE_C11 = ""
}


# This is necessary to build the test executable as an app
DEFINES += LIBQXMLEDIT_LIBRARY_STATIC
DEFINES += QXMLEDITSESSIONS_LIBRARY_STATIC
DEFINES += QXMLEDIT_NOMAIN
DEFINES += QXMLEDIT_TEST

QT       += gui xml xmlpatterns svg testlib network sql

macx: {
    QT       += macextras
}

greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += printsupport widgets core
}

greaterThan(QT_MAJOR_VERSION, 4) {
    equals(QT_MAJOR_VERSION, 5) {
        greaterThan(QT_MINOR_VERSION, 6) {
            QT       += scxml
        }
    }
    greaterThan(QT_MAJOR_VERSION, 5) {
        QT       += scxml
    }
}

greaterThan(QT_MAJOR_VERSION, 4) {
win32 {
    QT += winextras
    DEFINES += "_NO_W32_PSEUDO_MODIFIERS"
    DEFINES += "NOGDI"
}
}

lessThan(QT_MAJOR_VERSION, 5) {
    QMAKE_CXXFLAGS +=-Wno-unused-local-typedefs
}


QMAKE_CXXFLAGS +=-Werror=format-security

equals(INST_AVOID_PRECOMP_HEADERS, "") {
 CONFIG += precompile_header
 PRECOMPILED_HEADER  = ../src/precompiled_app.h
}

QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS +=-Werror

TARGET = qxmledittest
CONFIG   += console
CONFIG   -= app_bundle

equals(INST_USE_C11, "y") {
 QMAKE_CXXFLAGS +=-std=c++11
}

TEMPLATE = app
DESTDIR = ../build

SOURCES += \
    searchInFilesLogic.cpp \
    comparexml.cpp \
    tst_qxmledit.cpp \
    testcomment.cpp \
    testxsd.cpp \
    testpaste.cpp \
    testsplit.cpp \
    fakeuidelegate.cpp \
    testmixedcontent.cpp \
    testbase64.cpp \
    testeditingcommands.cpp \
    testexecution.cpp \
    testvalidation.cpp \
    app.cpp \
    testuifile.cpp \
    testsessions.cpp \
    testsqlaccess.cpp \
    sqlliteaccess.cpp \
    testeditelements.cpp \
    testundoredo.cpp \
    testvis.cpp \
    testdoctype.cpp \
    testxsddiff.cpp \
    exportcsvtest.cpp \
    testfilterattributes.cpp \
    testcomparexml.cpp \
    testbase.cpp \
    helpers/testcomparexmlunithelper.cpp \
    helpers/comparetreewithxml.cpp \
    testspringandforces.cpp \
    testsearch.cpp \
    testcopyattrs.cpp \
    testxsdnavigation.cpp \
    testclipboard.cpp \
    testnewfromtemplate.cpp \
    testxsltmode.cpp \
    testhelpers/XSLTAutoModeDialogprivatetest.cpp \
    teststyle.cpp \
    testhelpers/XsltElementDialogprivatetest.cpp \
    helpers/comparetrees.cpp \
    testxsdcopy.cpp \
    testhelpers/testsnippethandler.cpp \
    testbinaryviewer.cpp \
    testhelpers/testbvd.cpp \
    testsearchlets.cpp \
    testreadonly.cpp \
    testmetadata.cpp \
    testexport.cpp \
    testprolog.cpp \
    testencoding.cpp \
    testapp.cpp \
    testlengthattributes.cpp \
    testhelpers/editelementtest.cpp \
    testcolor.cpp \
    testhelpers/testconfigcolors.cpp \
    testindent.cpp \
    testcontainer.cpp \
    helpers/compareregolawitheditor.cpp \
    helpers/comparexsdwithxml.cpp \
    testxsdview.cpp \
    testxsdload.cpp \
    testhelpers/fakenetworkaccessmanager.cpp \
    testhelpers/fakenetworkreply.cpp \
    helpers/compareschemaobjects.cpp \
    testxsdvalidation.cpp \
    testencodingutils.cpp \
    testselectxsd.cpp \
    testxsdmode.cpp \
    testhelpers/testdialogxsdtypes.cpp \
    testanonymize.cpp \
    testhelpers/xsd/testannotxsdannotationeditprovider.cpp \
    testhelpers/xsd/testannotxsdannotationeditor.cpp \
    testhelpers/xsd/testxsdfullannotationsdialog.cpp \
    testhelpers/xsd/testxsdsinglecommentdialog.cpp \
    testreplace.cpp \
    testnillable.cpp \
    testxsitype.cpp \
    testusernamespaces.cpp \
    testxmlutils.cpp \
    testinsertxsdreference.cpp \
    testsortattributes.cpp \
    testopeninnewwindow.cpp \
    testelement.cpp \
    testloadfile.cpp \
    testhelpers/testmainwindow.cpp \
    testreplica.cpp \
    testutils.cpp \
    testconfig.cpp \
    testdeletesiblings.cpp \
    testcommandline.cpp \
    testeditenum.cpp \
    testnamespaces.cpp \
    testspecials.cpp \
    testscxml.cpp \
    testhelpers/scxmlautomodedialogprivatetest.cpp \
    testexecxslt.cpp \
    testspecialview.cpp \
    testformattinginfo.cpp \
    testhelpers/xsd/testxsdprintinfo.cpp \
    teststartupandbehavior.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

# object files
OBJECTS_DIR = ../build/test/obj
MOC_DIR = ../build/test/moc
UI_DIR = ../build/test/ui
UI_HEADERS_DIR = ../build/test/ui/include
UI_SOURCES_DIR = ../build/test/ui/src
RCC_DIR = ../build/test/rcc

INCLUDEPATH += ../src

include(test.pri)

HEADERS += \
    comparexml.h \
    testcomment.h \
    testxsd.h \
    testpaste.h \
    testsplit.h \
    fakeuidelegate.h \
    testmixedcontent.h \
    testbase64.h \
    testeditingcommands.h \
    testexecution.h \
    testvalidation.h \
    app.h \
    testuifile.h \
    testsessions.h \
    testsqlaccess.h \
    sqlliteaccess.h \
    testeditelements.h \
    testundoredo.h \
    testvis.h \
    testdoctype.h \
    testxsddiff.h \
    exportcsvtest.h \
    testfilterattributes.h \
    testcomparexml.h \
    testbase.h \
    helpers/testcomparexmlunithelper.h \
    helpers/comparetreewithxml.h \
    testspringandforces.h \
    testsearch.h \
    testcopyattrs.h \
    testxsdnavigation.h \
    testclipboard.h \
    testnewfromtemplate.h \
    testxsltmode.h \
    testhelpers/xslnavigator.h \
    testhelpers/XSLTAutoModeDialogprivatetest.h \
    teststyle.h \
    testhelpers/XsltElementDialogprivatetest.h \
    helpers/comparetrees.h \
    testxsdcopy.h \
    testhelpers/testsnippethandler.h \
    testbinaryviewer.h \
    testhelpers/testbvd.h \
    testsearchlets.h \
    testreadonly.h \
    testmetadata.h \
    testexport.h \
    testprolog.h \
    testencoding.h \
    testapp.h \
    testlengthattributes.h \
    testhelpers/editelementtest.h \
    testcolor.h \
    testhelpers/testconfigcolors.h \
    testindent.h \
    testcontainer.h \
    helpers/compareregolawitheditor.h \
    helpers/comparexsdwithxml.h \
    testxsdview.h \
    testxsdload.h \
    TestQXmlEdit.h \
    testhelpers/fakenetworkaccessmanager.h \
    testhelpers/fakenetworkreply.h \
    helpers/compareschemaobjects.h \
    testxsdvalidation.h \
    testencodingutils.h \
    testselectxsd.h \
    testxsdmode.h \
    testhelpers/testdialogxsdtypes.h \
    testanonymize.h \
    testhelpers/xsd/testannotxsdannotationeditprovider.h \
    testhelpers/xsd/testannotxsdannotationeditor.h \
    testhelpers/xsd/testxsdfullannotationsdialog.h \
    testhelpers/xsd/testxsdsinglecommentdialog.h \
    testreplace.h \
    testnillable.h \
    testxsitype.h \
    testusernamespaces.h \
    testxmlutils.h \
    testinsertxsdreference.h \
    testsortattributes.h \
    testopeninnewwindow.h \
    testelement.h \
    testloadfile.h \
    testhelpers/testmainwindow.h \
    testreplica.h \
    testutils.h \
    testconfig.h \
    testdeletesiblings.h \
    testcommandline.h \
    testeditenum.h \
    testnamespaces.h \
    testspecials.h \
    testscxml.h \
    testhelpers/scxmlautomodedialogprivatetest.h \
    testexecxslt.h \
    testspecialview.h \
    testformattinginfo.h \
    testhelpers/xsd/testxsdprintinfo.h \
    teststartupandbehavior.h

#OTHER_FILES += \

RESOURCES += testrsc.qrc
