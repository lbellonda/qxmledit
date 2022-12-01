#/**************************************************************************
# *  This file is part of QXmlEdit                                         *
# *  Copyright (C) 2019-2022 by Luca Bellonda and individual contributors  *
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

QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -Wno-deprecated-declarations
lessThan(QT_MAJOR_VERSION, 5) {
    QMAKE_CXXFLAGS +=-Wno-unused-local-typedefs
}

QMAKE_CXXFLAGS +=-Werror=format-security

isEmpty( INST_DISABLE_COMPILE_WARNINGS ) {
    QMAKE_CXXFLAGS +=-Werror
}

equals(INST_USE_C11, "y") {
 QMAKE_CXXFLAGS +=-std=c++11
}


#########################################################################
