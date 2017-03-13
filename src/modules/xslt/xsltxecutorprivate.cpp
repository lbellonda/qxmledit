/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

// private XSLTExecutor classes

#include "xsltexecutor.h"
#include <QXmlQuery>
#include <QXmlSerializer>
#include <QTextCodec>
#include "utils.h"

XSLTExecutor::MessageHandler::MessageHandler(MessagesOperationResult &result) : _result(result)
{
}

XSLTExecutor::MessageHandler::~MessageHandler()
{
}

void XSLTExecutor::MessageHandler::handleMessage(QtMsgType type,
        const QString &description,
        const QUrl &/*identifier*/,
        const QSourceLocation &sourceLocation)
{
    _mutex.lock();
    Utils::TODO_THIS_RELEASE("testme");
    SourceMessage *message = new SourceMessage();
    message->setCode(XSLTExecutor::ErrorRuntime);
    message->setDescription(description);
    SourceMessage::EType thisType ;
    switch(type) {
    case QtDebugMsg:
        thisType = SourceMessage::Debug;
        break;
    case QtWarningMsg:
        thisType = SourceMessage::Warning;
        break;
    case QtCriticalMsg:
        thisType = SourceMessage::Error;
        break;
    case QtFatalMsg:
        thisType = SourceMessage::Error;
        break;
#if QT_VERSION >= QT_VERSION_CHECK(5,5,0)
    case QtInfoMsg:
        thisType = SourceMessage::Info;
        break;
#endif
    default:
        thisType = SourceMessage::Error;
        break;
    }
    message->setType(thisType);
    message->setLine(sourceLocation.line());
    message->setColumn(sourceLocation.column());
    _result.addMessage(message);
    _result.setError(true);
    _mutex.unlock();
}


//-----------------------------------------------------------------

XSLTExecutor::InputHolder::InputHolder()
{
}

XSLTExecutor::InputHolder::~InputHolder()
{
}

bool XSLTExecutor::InputHolder::isFile()
{
    return false ;
}

QString XSLTExecutor::InputHolder::fileName()
{
    return "" ;
}

//-----------------------------------------------------------------

XSLTExecutor::OutputHolder::OutputHolder()
{
}

XSLTExecutor::OutputHolder::~OutputHolder()
{
}

bool XSLTExecutor::OutputHolder::isFile()
{
    return false ;
}

QString XSLTExecutor::OutputHolder::fileName()
{
    return "" ;
}

//-----------------------------------------------------------------

XSLTExecutor::InputFileHolder::InputFileHolder(const QString &fileName)
{
    _fileName = fileName ;
    _device.setFileName(_fileName);
}

XSLTExecutor::InputFileHolder::~InputFileHolder()
{
}

QIODevice *XSLTExecutor::InputFileHolder::device()
{
    return &_device ;
}

bool XSLTExecutor::InputFileHolder::isFile()
{
    return true ;
}

QString XSLTExecutor::InputFileHolder::fileName()
{
    return _fileName ;
}

//-----------------------------------------------------------------

XSLTExecutor::InputStringHolder::InputStringHolder(const QString &outString)
{
    _target = outString ;
}

XSLTExecutor::InputStringHolder::~InputStringHolder()
{
}

QIODevice *XSLTExecutor::InputStringHolder::device()
{
    // only for the current Qt version
    _data = _target.toUtf8();
    _device.setData(_data);
    return &_device ;
}
//-----------------------------------------------------------------

XSLTExecutor::OutputFileHolder::OutputFileHolder(const QString &fileName)
{
    _fileName = fileName ;
    _device.setFileName(_fileName);
}

XSLTExecutor::OutputFileHolder::~OutputFileHolder()
{
}

bool XSLTExecutor::OutputFileHolder::openForWrite()
{
    return _device.open(QFile::ReadWrite);
}

bool XSLTExecutor::OutputFileHolder::close()
{
    if(_device.isOpen()) {
        const bool result = _device.flush();
        _device.close();
        return result;
    }
    return false ;
}

bool XSLTExecutor::OutputFileHolder::evaluateTo(QXmlQuery &query)
{
    QXmlSerializer result(query, &_device);
    const bool success = query.evaluateTo(&result);
    return success ;
}

bool XSLTExecutor::OutputFileHolder::isFile()
{
    return true ;
}

QString XSLTExecutor::OutputFileHolder::fileName()
{
    return _fileName ;
}

//-----------------------------------------------------------------

XSLTExecutor::OutputStringHolder::OutputStringHolder(QString *outString)
{
    _target = outString ;
}

XSLTExecutor::OutputStringHolder::~OutputStringHolder()
{
}

bool XSLTExecutor::OutputStringHolder::openForWrite()
{
    return true;
}

bool XSLTExecutor::OutputStringHolder::close()
{
    return true;
}

bool XSLTExecutor::OutputStringHolder::evaluateTo(QXmlQuery &query)
{
    const bool success = query.evaluateTo(_target);
    return success ;
}
