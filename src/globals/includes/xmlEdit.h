/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
 *    as indicated in the AUTHORS file                                    *
 *  lbellonda _at_ gmail.com                                              *
 *                                                                        *
 * This library is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU Library General Public            *
 * License as published by the Free Software Foundation; either           *
 * version 2 of the License, or (at your option) any later version.       *
 *                                                                        *
 * This library is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 * Library General Public License for more details.                       *
 *                                                                        *
 * You should have received a copy of the GNU Library General Public      *
 * License along with this library; if not, write to the                  *
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,       *
 * Boston, MA  02110-1301  USA                                            *
 **************************************************************************/

#ifndef H_XML_EDIT
#define H_XML_EDIT


#define DECL_CONSTRUCTOR(clazz) clazz(); virtual ~clazz();
#define IMPL_CONSTRUCTOR(clazz) clazz::clazz() {} \
    clazz::~clazz() {}

// Version numbers are divided like that: 0x0005.02.00

//------------------------------------------------
// QXMLEDIT_RELEASE_VERSION:
// 0: snapshot
// 1: rc
// 2: release
#define QXMLEDIT_RELEASE_VERSION    2

#if QXMLEDIT_RELEASE_VERSION  ==  0
#define QXMLEDIT_VERSION_IS_SNAPSHOT 1
#elif QXMLEDIT_RELEASE_VERSION  ==  1
#define QXMLEDIT_VERSION_RC 1
#elif QXMLEDIT_RELEASE_VERSION  >  2
// case 2 is missing
#error "Invalid QXMLEDIT_RELEASE_VERSION"
#endif

#if defined(QXMLEDIT_VERSION_IS_SNAPSHOT)
#define VERSION_BRANCH "-SNAPSHOT"
#elif defined(QXMLEDIT_VERSION_RC)
#define VERSION_BRANCH "-RC"
#else
#define VERSION_BRANCH ""
#endif

//--------------------------------------------------------------------------------
#define VERSION_BASE "0.9.13"
#define VERSION_NUMBER 0x00090D00
//--------------------------------------------------------------------------------

// this is the version number of the welcome dialog
#define VERSION_NUMBER_WELCOME 0x00050200
//------------------------------------------------

#define VERSION_IDENTIFIER VERSION_BASE VERSION_BRANCH
#define VERSION_ID "$VER " VERSION_IDENTIFIER
#define VERSION_STRING "QXmlEdit " VERSION_IDENTIFIER

//debug switches
//#define _DEBUG_BALSAMIQ
//#define QXML_DEBUG
#define XSD_INTEGRATION

#include "qtincludes.h"

extern const char VERSION[] ;

//TODO:
#define ORGANIZATION_NAME   "QXmlEdit"
#define ORGANIZATION_DOMAIN "QXmlEdit"
#define APPLICATION_NAME    "QXmlEdit"


void msgBoxDebug(const QString message);

#ifdef  QXML_DEBUG
#define D(x)    x
#else
#define D(x)
#endif

#define STRINGIZE_(x)    #x
#define STRINGIZE(x)    STRINGIZE_(x)


//-------- logging (enabled by define)----------------
#ifdef LOG_CONSOLE
#include <stdio.h>
#define TRACEQ(x) do{printf("%s\n", x.toLatin1().data() ); fflush(stdout);} while(0)
#define TRACE(x) do{printf("%s\n", x); fflush(stdout);} while(0)
#define TRACE2(x, y) do{printf(x, y); fflush(stdout);} while(0)
#else
#define TRACEQ(x)
#define TRACE(x)
#define TRACE2(x, y)
#endif


//----- working environment
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)

#if defined(Q_OS_MAC) || defined(Q_OS_MACX)
#define ENVIRONMENT_MACOS
#endif

#else

#if defined(Q_OS_MAC) || defined(Q_OS_MACX)
#define ENVIRONMENT_MACOS
#endif

#endif

#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
#define ENVIRONMENT_WINDOWS
#endif

//----- end working environment

#define FORINT(var,limit) for(int var = 0 ; var < limit ; var ++ )
#define EMPTYPTRLIST(object, Type) foreach( Type *var, object) { delete var; } object.clear();

#if QT_VERSION >= QT_VERSION_CHECK(5,3,1)
#define    STYLEVIEWITEM_OPTION4(option) const QStyleOptionViewItem &option4(option);
#else
#define    STYLEVIEWITEM_OPTION4(option) const QStyleOptionViewItemV4 &option4(option);
#endif

#endif /*H_XML_EDIT*/
