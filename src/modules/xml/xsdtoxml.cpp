/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2021 by Luca Bellonda and individual contributors       *
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

#include "xsdtoxml.h"
#include <utils.h>

XSDToXML::XSDToXML(ApplicationData *appData)
{
    Utils::TODO_THIS_RELEASE("TODO: remove unused vars");
    _started = false;
    _appData = appData ;
    _result = NULL ;
    _tempDir = NULL;
}

XSDToXML::~XSDToXML()
{
    deleteData();
}

void XSDToXML::deleteData()
{
    if(NULL != _tempDir) {
        _tempDir->remove();
        delete _tempDir;
        _tempDir = NULL ;
    }
}

bool XSDToXML::generateData(OperationResult *result, Regola *regola, const QString &localNameOfGlobalElement)
{
    if(_started) {
        return addError(result, QObject::tr("Operation already started"));
    }
    _localNameOfGlobalElement = localNameOfGlobalElement;
    _started = true ;
    _result = result ;
    _result->setError(false);
    if(!saveData(regola)) {
        return false;
    }
    execute();
    deleteData();
    return _result->isOk();
}


QStringList XSDToXML::makeArguments()
{
    QStringList options;
    options.append(sourceFilePath());
    options.append("-name");
    options.append(_localNameOfGlobalElement);
    return options;
}

bool XSDToXML::saveData(Regola *regola)
{
    if((NULL == regola) || regola->isEmpty(false)) {
        return addError(_result, QObject::tr("No data to save."));
    }
    _tempDir = new QTemporaryDir();
    _tempDir->setAutoRemove(false);
    if(!_tempDir->isValid()) {
        return addError(_result, QObject::tr("Problems creating directory."));
    }
    _dirPath = _tempDir->path() ;
    _sourceFilePath = _dirPath + "/input.xsd";

    bool modifiedStatus = regola->isModified();
    if(!regola->write(_sourceFilePath, false)) {
        addError(_result, QObject::tr("Error saving data."));
    }
    regola->setModified(modifiedStatus);
    return _result->isOk();
}

QString XSDToXML::getXSD2Inst()
{
    return _appData->xsd2InstPath();
}

bool XSDToXML::execute()
{
    QProcess xsdToXmlProcess;
    QString xsd2Inst = getXSD2Inst();
    QStringList arguments = makeArguments();
    xsdToXmlProcess.start(xsd2Inst, arguments);
    if(!xsdToXmlProcess.waitForStarted()) {
        return addError(_result, QObject::tr("Process not started."));
    }
    xsdToXmlProcess.closeWriteChannel();
    if(!xsdToXmlProcess.waitForFinished(TimeoutExec)) {
        addError(_result, QObject::tr("Timeout executing process."));
    } else {
        QProcess::ExitStatus status = xsdToXmlProcess.exitStatus();
        if(QProcess::NormalExit == status) {
            if(0 != xsdToXmlProcess.exitCode()) {
                addError(_result, QObject::tr("Process code not zero:%1.").arg(xsdToXmlProcess.exitCode()));
            } else {
                _instanceData = QString::fromLocal8Bit(xsdToXmlProcess.readAllStandardOutput()) ;
                _result->setError(false);
            }
        }
    }
    if(!_result->isOk()) {
        QByteArray stdErr = xsdToXmlProcess.readAllStandardError() ;
        QByteArray stdOut = xsdToXmlProcess.readAllStandardOutput() ;
        QString stdErrString(stdErr);
        QString stdOutString(stdOut);
        addError(_result, Utils::truncateString(stdErrString));
        addError(_result, Utils::truncateString(stdOutString));
    }
    return _result->isOk() ;
}

bool XSDToXML::addError(OperationResult *result, const QString &msgText)
{
    result->setMessage(result->message() + msgText);
    result->setError(true);
    return false;
}

QString XSDToXML::dirPath()
{
    return _dirPath ;
}

QString XSDToXML::sourceFilePath()
{
    return _sourceFilePath;
}

QString XSDToXML::data()
{
    return _instanceData;
}
