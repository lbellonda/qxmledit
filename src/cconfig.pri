#/**************************************************************************
# *  This file is part of QXmlEdit                                         *
# *  Copyright (C) 2018-2019 by Luca Bellonda and individual contributors  *
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

# Additional resources not included with this script:
#.desktop file in: install_scripts/environment/desktop/QXmlEdit.desktop
#Icon in: install_scripts/environment/icon/qxmledit.png


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

LIB_VERSIONED=$$(QXMLEDIT_VERSIONED)
!isEmpty(QXMLEDIT_VERSIONED) {
    LIB_VERSIONED=$$QXMLEDIT_VERSIONED
}
message("LIB_VERSIONED $$LIB_VERSIONED")

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
