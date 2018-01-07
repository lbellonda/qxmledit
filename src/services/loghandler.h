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


#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include <QObject>


#include <QString>
#include <QFile>
#include <QTextStream>
#include "log.h"

class QMessageLogContext;

class LogHandler : public QObject, public FrwLogger
{
    Q_OBJECT
    static LogHandler *instance;
    bool        _enabled;
    QString     _fileName;
    bool        _init;
    bool        _isWriteImmediate;
    ELogLevel   _level;

    void log(const ELogLevel level, const LogInfo *source, const QString & message);
    QString levelToString(const ELogLevel level);

public:
    explicit LogHandler(QObject *parent = 0);
    ~LogHandler();

    static void msgHandler(QtMsgType type, const char *msg);
    static void msgHandlerCategory(QtMsgType type, const QMessageLogContext &msgContext, const QString &msg);

    bool isEnabled();
    void setEnabled(const bool how);
    ELogLevel getLevel();
    void setLevel(const ELogLevel newLevel);
    bool isLoggable(const ELogLevel level);

    void debug(const QString &message, const LogInfo *source = NULL);
    void info(const QString &message, const LogInfo *source = NULL);
    void warning(const QString &message, const LogInfo *source = NULL);
    void error(const QString &message, const LogInfo *source = NULL);

signals:

public slots:

};

#endif // LOGHANDLER_H
