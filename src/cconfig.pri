#/**************************************************************************
# *  This file is part of QXmlEdit                                         *
# *  Copyright (C) 2018-2022 by Luca Bellonda and individual contributors  *
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


# TO CUSTOMIZE INSTALLATION DIRECTORY SET THESE VARIABLES
# Note:
#  WITH SOME MORE WORK WE CAN USE VARIABLES PASSED TO MAKE LIKE THAT
#  in pro DEFINES += UNIX_RESOURCE_PATH=\"$(QXMLEDIT_DATADIR)\"
#  and calling make QXMLEDIT_DATADIR=/usr/share/qxmledit

############################ BEGIN INSTALLATION FOLDERS DECLARATION ###########################################

#message(QXMLEDIT_INST_DATA_DIR = $$(QXMLEDIT_INST_DATA_DIR))
#message(QXMLEDIT_INST_DIR = $$(QXMLEDIT_INST_DIR))
#message(QXMLEDIT_INST_DOC_DIR = $$(QXMLEDIT_INST_DOC_DIR))
#message(QXMLEDIT_INST_LIB_DIR = $$(QXMLEDIT_INST_LIB_DIR))
#message(QXMLEDIT_INST_TRANSLATIONS_DIR = $$(QXMLEDIT_INST_TRANSLATIONS_DIR))
#message(QXMLEDIT_UNIX_LOWERCASE_NAME = $$(QXMLEDIT_UNIX_LOWERCASE_NAME))
#message(LIB_VERSIONED=$$(QXMLEDIT_VERSIONED))

###########################################

INST_DATA_DIR=$$(QXMLEDIT_INST_DATA_DIR)
!isEmpty(QXMLEDIT_INST_DATA_DIR) {
    INST_DATA_DIR=$$QXMLEDIT_INST_DATA_DIR
}
isEmpty(INST_DATA_DIR) {
    INST_DATA_DIR = /opt/qxmledit
}
message("Inst data dir $$INST_DATA_DIR")

###########################################
INST_METAINFO_DIR=$$(QXMLEDIT_INST_METAINFO_DIR)
!isEmpty(QXMLEDIT_INST_METAINFO_DIR) {
    INST_METAINFO_DIR=$$QXMLEDIT_INST_METAINFO_DIR
}
isEmpty(INST_METAINFO_DIR) {
    INST_METAINFO_DIR = /opt/qxmledit
}

# enabling
INSTALL_METAINFO_ENABLED="Y"
DONTUSE_METAINFO=$$(QXMLEDIT_NO_METAINFO)
!isEmpty(QXMLEDIT_NO_METAINFO) {
    DONTUSE_METAINFO=$$QXMLEDIT_NO_METAINFO
}

!isEmpty(DONTUSE_METAINFO) {
    INSTALL_METAINFO_ENABLED="N"
    message("No METAINFO")
} else {
    message("METAINFO enabled")
    message("Inst metainfo dir $$INST_METAINFO_DIR")
}

###########################################

INST_DESKTOPINFO_DIR=$$(QXMLEDIT_INST_DESKTOPINFO_DIR)
!isEmpty(QXMLEDIT_INST_DESKTOPINFO_DIR) {
    INST_DESKTOPINFO_DIR=$$QXMLEDIT_INST_DESKTOPINFO_DIR
}
isEmpty(INST_DESKTOPINFO_DIR) {
    INST_DESKTOPINFO_DIR = /opt/qxmledit
}

# enabling
INSTALL_DESKTOPINFO_ENABLED="Y"
DONTUSE_DESKTOPINFO=$$(QXMLEDIT_NO_DESKTOPINFO)
!isEmpty(QXMLEDIT_NO_DESKTOPINFO) {
    DONTUSE_DESKTOPINFO=$$QXMLEDIT_NO_DESKTOPINFO
}

!isEmpty(DONTUSE_DESKTOPINFO) {
    INSTALL_DESKTOPINFO_ENABLED="N"
    message("No DESKTOPINFO")
} else {
    message("DESKTOPINFO enabled")
    message("Inst desktop info dir $$INST_DESKTOPINFO_DIR")
}

###########################################

INST_ICON_DIR=$$(QXMLEDIT_INST_ICON_DIR)
!isEmpty(QXMLEDIT_INST_ICON_DIR) {
    INST_ICON_DIR=$$QXMLEDIT_INST_ICON_DIR
}
isEmpty(INST_ICON_DIR) {
    INST_ICON_DIR = $$INST_DATA_DIR
}
message("Inst icon dir $$INST_ICON_DIR")
# enabling
INSTALL_ICON_ENABLED="Y"
DONTUSE_ICON=$$(QXMLEDIT_NO_ICON)
!isEmpty(QXMLEDIT_NO_ICON) {
    DONTUSE_ICON=$$QXMLEDIT_NO_ICON
}

!isEmpty(DONTUSE_ICON) {
    INSTALL_ICON_ENABLED="N"
    message("No ICON")
} else {
    message("ICON enabled")
}

###########################################

INST_DIR=$$(QXMLEDIT_INST_DIR)
!isEmpty(QXMLEDIT_INST_DIR) {
    INST_DIR=$$QXMLEDIT_INST_DIR
}
isEmpty(INST_DIR) {
    INST_DIR = /opt/qxmledit
}
message("Inst dir $$INST_DIR")

###########################################

INST_DOC_DIR=$$(QXMLEDIT_INST_DOC_DIR)
!isEmpty(QXMLEDIT_INST_DOC_DIR) {
    INST_DOC_DIR=$$QXMLEDIT_INST_DOC_DIR
}

isEmpty(INST_DOC_DIR) {
    INST_DOC_DIR = /opt/qxmledit
}
message("Inst doc dir $$INST_DOC_DIR")

###########################################
USE_FAKE_SOURCES=$$(QXMLEDIT_USE_FAKE_SOURCES)
!isEmpty(QXMLEDIT_USE_FAKE_SOURCES) {
    USE_FAKE_SOURCES=$$QXMLEDIT_USE_FAKE_SOURCES
}

!isEmpty(USE_FAKE_SOURCES) {
    USE_FAKE_SOURCES = true
} else {
    USE_FAKE_SOURCES = false
}

message("Use fake sources $$USE_FAKE_SOURCES")

###########################################

INST_LIB_DIR=$$(QXMLEDIT_INST_LIB_DIR)
!isEmpty(QXMLEDIT_INST_LIB_DIR) {
    INST_LIB_DIR=$$QXMLEDIT_INST_LIB_DIR
}

isEmpty(INST_LIB_DIR) {
    INST_LIB_DIR = /opt/qxmledit
}

message("Inst lib dir $$INST_LIB_DIR")

###########################################

INST_INCLUDE_DIR=$$(QXMLEDIT_INST_INCLUDE_DIR)
!isEmpty(QXMLEDIT_INST_INCLUDE_DIR) {
    INST_INCLUDE_DIR=$$QXMLEDIT_INST_INCLUDE_DIR
}

isEmpty(INST_INCLUDE_DIR) {
    INST_INCLUDE_DIR = /opt/qxmledit/include
}

message("Inst include dir $$INST_INCLUDE_DIR")

###########################################

INST_TRANSLATIONS_DIR=$$(QXMLEDIT_INST_TRANSLATIONS_DIR)
!isEmpty(QXMLEDIT_INST_TRANSLATIONS_DIR) {
    INST_TRANSLATIONS_DIR=$$QXMLEDIT_INST_TRANSLATIONS_DIR
}

isEmpty(INST_TRANSLATIONS_DIR) {
    INST_TRANSLATIONS_DIR=$$INST_DATA_DIR/translations
}

message("Inst translations dir $$INST_TRANSLATIONS_DIR")

###########################################

COMPILE_DISABLE_TIPS=$$(QXMLEDIT_COMPILE_DISABLE_TIPS)
!isEmpty(QXMLEDIT_COMPILE_DISABLE_TIPS) {
    COMPILE_DISABLE_TIPS=$$QXMLEDIT_COMPILE_DISABLE_TIPS
}

!isEmpty(COMPILE_DISABLE_TIPS) {
    DEFINES += COMPILE_DISABLE_TIPS
    message("Tips disabled")
}
isEmpty(COMPILE_DISABLE_TIPS) {
    message("Tips enabled")
}

###########################################
ENABLE_SCXML="N"
greaterThan(QT_MAJOR_VERSION, 4) {
    equals(QT_MAJOR_VERSION, 5) {
        greaterThan(QT_MINOR_VERSION, 6) {
            ENABLE_SCXML="Y"
        }
    }
    greaterThan(QT_MAJOR_VERSION, 5) {
        ENABLE_SCXML="Y"
    }
}

DISABLE_SCXML=$$(QXMLEDIT_DISABLE_SCXML)
!isEmpty(DISABLE_SCXML) {
    ENABLE_SCXML="N"
}
!isEmpty(QXMLEDIT_DISABLE_SCXML) {
    ENABLE_SCXML="N"
}

isEqual(ENABLE_SCXML, "Y") {
    DEFINES += "QXMLEDIT_QT_SCXML_ENABLED"
    message("SCXML enabled")
}

!isEqual(ENABLE_SCXML, "Y") {
    message("SCXML disabled")
}
###########################################

USE_QWTPLOT="Y"
DONTUSE_QWTPLOT=$$(QXMLEDIT_NO_QWTPLOT)
!isEmpty(QXMLEDIT_NO_QWTPLOT) {
    DONTUSE_QWTPLOT=$$QXMLEDIT_NO_QWTPLOT
}

!isEmpty(DONTUSE_QWTPLOT) {
    USE_QWTPLOT="N"
    message("No QWTPlot")
}
isEmpty(DONTUSE_QWTPLOT) {
    message("QWTPlot enabled")
}

###########################################

INST_AVOID_PRECOMP_HEADERS=$$(QXMLEDIT_INST_AVOID_PRECOMP_HEADERS)
!isEmpty(QXMLEDIT_INST_AVOID_PRECOMP_HEADERS) {
    INST_AVOID_PRECOMP_HEADERS=$$QXMLEDIT_INST_AVOID_PRECOMP_HEADERS
}
isEmpty(INST_AVOID_PRECOMP_HEADERS) {
    INST_AVOID_PRECOMP_HEADERS = ""
    message("Precompiled headers activated")
}
!isEmpty(INST_AVOID_PRECOMP_HEADERS) {
    message("Precompiled headers disabled")
}

###########################################

INST_USE_C11=$$(QXMLEDIT_INST_USE_C11)
!isEmpty(QXMLEDIT_INST_USE_C11) {
    INST_USE_C11=$$QXMLEDIT_INST_USE_C11
}
isEmpty(INST_USE_C11) {
    INST_USE_C11 = ""
    message("No C11 support")
}
!isEmpty(INST_USE_C11) {
    message("C11 support")
}

###########################################

TARGET_NAME_UNIXSTYLE=$$(QXMLEDIT_UNIX_LOWERCASE_NAME)
!isEmpty(QXMLEDIT_UNIX_LOWERCASE_NAME) {
    TARGET_NAME_UNIXSTYLE=$$QXMLEDIT_UNIX_LOWERCASE_NAME
}
message("TARGET_NAME_UNIXSTYLE $$TARGET_NAME_UNIXSTYLE")


###########################################

LIB_SUFFIX=$$(QXMLEDIT_LIB_SUFFIX)
!isEmpty(QXMLEDIT_LIB_SUFFIX) {
    LIB_SUFFIX=$$QXMLEDIT_LIB_SUFFIX
}
message("LIB_SUFFIX $$LIB_SUFFIX")

LIB_NOVERSIONED=""
windows: {
    LIB_NOVERSIONED="Y"
}

ENV_QXMLEDIT_NOVERSIONED=$$(QXMLEDIT_NOVERSIONED)
!isEmpty(ENV_QXMLEDIT_NOVERSIONED) {
    LIB_NOVERSIONED=$$(QXMLEDIT_NOVERSIONED)
}

!isEmpty(QXMLEDIT_NOVERSIONED) {
    LIB_NOVERSIONED=$$QXMLEDIT_NOVERSIONED
}

if(!equals(LIB_NOVERSIONED, "Y")) {
    if(!equals(LIB_NOVERSIONED, "N")) {
        if(!equals(LIB_NOVERSIONED, "")) {
            error("Invalid value for LIB_NOVERSIONED: $$LIB_NOVERSIONED, must be Y,N or empty");
        }
    }
}

message("LIB_NOVERSIONED $$LIB_NOVERSIONED")

###########################################

INST_DISABLE_COMPILE_WARNINGS=$$(QXMLEDIT_INST_DISABLE_COMPILE_WARNINGS)
!isEmpty(QXMLEDIT_INST_DISABLE_COMPILE_WARNINGS) {
    INST_DISABLE_COMPILE_WARNINGS=$$QXMLEDIT_INST_DISABLE_COMPILE_WARNINGS
}
isEmpty(INST_DISABLE_COMPILE_WARNINGS) {
    INST_DISABLE_COMPILE_WARNINGS = ""
    message("Compiler warnings enabled")
}
!isEmpty(INST_DISABLE_COMPILE_WARNINGS) {
    message("Compiler warnings disabled")
}

############################ END INSTALLATION FOLDERS DECLARATION #############################################

###########################################

EVAL_QXMLEDIT_DISABLE_TODO_CONFIG=$$(QXMLEDIT_DISABLE_TODO_CONFIG)
isEmpty(EVAL_QXMLEDIT_DISABLE_TODO_CONFIG) {
    EVAL_QXMLEDIT_DISABLE_TODO_CONFIG=$$QXMLEDIT_DISABLE_TODO_CONFIG
}

!isEmpty(EVAL_QXMLEDIT_DISABLE_TODO_CONFIG) {
    DEFINES += "QXMLEDIT_DISABLE_TODO"
    message("Disabled TODO")
}

isEmpty(EVAL_QXMLEDIT_DISABLE_TODO_CONFIG) {
    message("TODO Enabled")
}

###########################################

QXMLEDIT_LIB_SUFFIX = ""
!equals(LIB_SUFFIX, "") {
    QXMLEDIT_LIB_SUFFIX = -$$QXMLEDIT_VERSION
    message("QXMLEDIT_LIB_SUFFIX Enabled: $$QXMLEDIT_LIB_SUFFIX")
} else {
    message("QXMLEDIT_LIB_SUFFIX disabled")
}

isEqual(LIB_NOVERSIONED, "Y") {
    VERSION=""
    message("no version")
}

###########################################


