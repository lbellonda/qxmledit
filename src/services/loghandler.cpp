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


#include "loghandler.h"
#include <QDateTime>

#define SEPARATOR   "|"

LogHandler *LogHandler::instance = NULL ;

LogHandler::LogHandler(QObject *parent) :
    QObject(parent)
{
    instance = this ;
    _init = false;
    _enabled = true ;
    _level = INFO ;
    _isWriteImmediate = false ;
}

LogHandler::~LogHandler()
{
    instance = NULL ;
}

/*
static void dumpMsg(const char *type, const char *msg)
{
    fprintf(stderr, "%s: %s\n", type, msg);
    fflush(stderr);
}
*/

void LogHandler::msgHandler(QtMsgType type, const char *msg)
{
    if(NULL == instance) {
        instance = new LogHandler();
    }
    if(NULL == instance) {
        return ;
    }
    switch(type) {
    default:
        instance->log(FrwLogger::INFO, NULL, msg);
        break;
    case QtDebugMsg:
        instance->log(FrwLogger::DEBUG, NULL, msg);
        break;
    case QtWarningMsg:
        instance->log(FrwLogger::WARNING, NULL, msg);
        break;
    case QtCriticalMsg:
        instance->log(FrwLogger::ERROR, NULL, msg);
        break;
    case QtFatalMsg:
        instance->log(FrwLogger::ERROR, NULL, msg);
        break;
    }
}

void LogHandler::msgHandlerCategory(QtMsgType type, const QMessageLogContext & /*msgContext*/, const QString &msg)
{
    msgHandler(type, msg.toLatin1());
}

bool LogHandler::isEnabled()
{
    return _enabled ;
}

void LogHandler::setEnabled(const bool how)
{
    _enabled = how ;
}

LogHandler::ELogLevel LogHandler::getLevel()
{
    return _level ;
}

void LogHandler::setLevel(const ELogLevel newLevel)
{
    switch(newLevel) {
    case DEBUG:
    case INFO:
    case WARNING:
    case ERROR:
        _level = newLevel ;
        break;
    default:
        _level = INFO ;
        break;
    }
}

bool LogHandler::isLoggable(const ELogLevel logLevel)
{
    if(logLevel >= _level) {
        return true ;
    }
    return false;
}

QString LogHandler::levelToString(const ELogLevel level)
{
    switch(level) {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    default:
        return "????";
    }
}

void LogHandler::debug(const QString &message, const LogInfo *source)
{
    log(FrwLogger::DEBUG, source, message);
}

void LogHandler::info(const QString &message, const LogInfo *source)
{
    log(FrwLogger::INFO, source, message);
}

void LogHandler::warning(const QString &message, const LogInfo *source)
{
    log(FrwLogger::WARNING, source, message);
}

void LogHandler::error(const QString &message, const LogInfo *source)
{
    log(FrwLogger::ERROR, source, message);
}

void LogHandler::log(const ELogLevel level, const LogInfo *source, const QString & message)
{
    if(!isEnabled() || !isLoggable(level)) {
        return;
    }
    QDateTime timestamp = QDateTime::currentDateTime();

    QString msg = timestamp.toString("yyyy.MM.dd hh:mm:ss.zzz") ;
    msg += SEPARATOR ;
    msg += levelToString(level);
    msg += SEPARATOR ;
    msg += (NULL != source ? source->source : "") ;
    msg += SEPARATOR ;
    msg += (NULL != source ? source->method : "");
    msg += SEPARATOR ;
    msg += (NULL != source ? source->type : "");
    msg += SEPARATOR;
    msg += message;
    msg += "\n";
    fprintf(stderr, "%s", msg.toLatin1().data());
    fflush(stderr);

}

