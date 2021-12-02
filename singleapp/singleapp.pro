#/**************************************************************************
# *  This file is part of QXmlEdit                                         *
# *  Copyright (C) 2011-2020 by Luca Bellonda and individual contributors  *
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

include("../src/version.pri")
include("../src/cconfig.pri")
include("../src/coptions.pri")

USE_QWTPLOT="Y"
DONTUSE_QWTPLOT=$$(QXMLEDIT_NO_QWTPLOT)
!isEmpty(DONTUSE_QWTPLOT) {
    USE_QWTPLOT="N"
    message("No QWTPlot")
}

############################ END INSTALLATION FOLDERS DECLARATION #############################################


TEMPLATE = subdirs

CONFIG += ordered

equals(USE_QWTPLOT, "Y") {
    greaterThan(QT_MAJOR_VERSION, 4) {
        SUBDIRS += ../external/qwtplot3d/qwtplot3d.pro
    }
}

SUBDIRS += app.pro
