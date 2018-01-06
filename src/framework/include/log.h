/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#ifndef QXMLEDIT_FRAMEWORK_LOG_H
#define QXMLEDIT_FRAMEWORK_LOG_H

// this code inspired by KLabeler
class LogInfo
{

public:
    QString source;     // module
    QString method;     //
    QString type;       // enter function, exit, etc

};

class FrwLogger
{

public:
    enum ELogLevel { DEBUG, INFO, WARNING, ERROR };


    virtual bool isEnabled() = 0;
    virtual void setEnabled(const bool how) = 0;
    virtual ELogLevel getLevel() = 0;
    virtual void setLevel(const ELogLevel newLevel) = 0;
    virtual bool isLoggable(const ELogLevel level) = 0;

    virtual void debug(const QString &message, const LogInfo *source = NULL) = 0;
    virtual void info(const QString &message, const LogInfo *source = NULL) = 0;
    virtual void warning(const QString &message, const LogInfo *source = NULL) = 0;
    virtual void error(const QString &message, const LogInfo *source = NULL) = 0;

};


#endif // QXMLEDIT_FRAMEWORK_LOG_H
