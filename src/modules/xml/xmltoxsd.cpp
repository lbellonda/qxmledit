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

#include "xmltoxsd.h"

XMLToXSD::XMLToXSD(ApplicationData *appData)
{
    _started = false;
    _appData = appData ;
    _result = NULL ;
    _tempDir = NULL;
}

XMLToXSD::~XMLToXSD()
{
    deleteData();
}

bool XMLToXSD::generateXSD(OperationResult *result, Regola *regola, const GenXSDOption option, const int enumerationThreshold, const bool simpleContentTypeSmart)
{
    if(_started) {
        return addError(result, QObject::tr("Operation already started"));
    }
    _started = true ;
    _schemaData = "";
    _result = result ;
    _result->setError(false);
    if(!saveData(regola)) {
        return false;
    }
    if(execute(option, enumerationThreshold, simpleContentTypeSmart)) {
        readResults();
    }
    deleteData();
    return result->isOk();
}


QStringList XMLToXSD::makeArguments(const GenXSDOption option, const int enumerationThreshold, const bool simpleContentTypeSmart)
{
    QStringList options;
    options.append("-enumerations");
    if(enumerationThreshold > 0) {
        options.append(QString("%1").arg(enumerationThreshold));
    } else {
        options.append("never");
    }
    options.append("-simple-content-types");
    options.append(simpleContentTypeSmart ? "smart" : "string");
    options.append("-design");
    switch(option) {
    case GENXSD_VENETIAN_BLIND:
        options.append("vb");
        break;
    case GENXSD_RUSSIAN_DOLL:
        options.append("rd");
        break;
    case GENXSD_SALAMI_SLICE:
        options.append("ss");
        break;
    }
    options.append("-outDir");
    options.append(_dirPath);
    options.append("-outPrefix");
    options.append("qxmleditschema");
    options.append(sourceFilePath());
    return options;
}

bool XMLToXSD::saveData(Regola *regola)
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
    _sourceFilePath = _dirPath + "/input.xml";

    bool modifiedStatus = regola->isModified();
    if(!regola->write(_sourceFilePath, false)) {
        addError(_result, QObject::tr("Error saving data."));
    }
    regola->setModified(modifiedStatus);
    return _result->isOk();
}

void XMLToXSD::deleteData()
{
    if(NULL != _tempDir) {
        _tempDir->remove();
        delete _tempDir;
        _tempDir = NULL ;
    }
}

QString XMLToXSD::getInst2XSD()
{
    return _appData->inst2XSDPath();
}

bool XMLToXSD::execute(const GenXSDOption option, const int enumerationThreshold, const bool simpleContentTypeSmart)
{
    QProcess xmlToXsdProcess;
    QString inst2XSD = getInst2XSD();
    QStringList arguments = makeArguments(option, enumerationThreshold, simpleContentTypeSmart);
    xmlToXsdProcess.start(inst2XSD, arguments);
    if(!xmlToXsdProcess.waitForStarted()) {
        return addError(_result, QObject::tr("Process not started."));
    }
    xmlToXsdProcess.closeWriteChannel();
    if(!xmlToXsdProcess.waitForFinished(TimeoutExec)) {
        addError(_result, QObject::tr("Timeout executing process."));
    } else {
        QProcess::ExitStatus status = xmlToXsdProcess.exitStatus();
        if(QProcess::NormalExit == status) {
            if(0 != xmlToXsdProcess.exitCode()) {
                addError(_result, QObject::tr("Process code not zero:%1.").arg(xmlToXsdProcess.exitCode()));
            } else {
                _result->setError(false);
            }
        }
    }
    if(!_result->isOk()) {
        QByteArray stdErr = xmlToXsdProcess.readAllStandardError() ;
        QByteArray stdOut = xmlToXsdProcess.readAllStandardOutput() ;
        QString stdErrString(stdErr);
        QString stdOutString(stdOut);
        addError(_result, Utils::truncateString(stdErrString));
        addError(_result, Utils::truncateString(stdOutString));
    }
    return _result->isOk() ;
}

bool XMLToXSD::addError(OperationResult *result, const QString &msgText)
{
    result->setMessage(result->message() + msgText);
    result->setError(true);
    return false;
}

QString XMLToXSD::resultPath()
{
    return _tempDir->path() + "/qxmleditschema0.xsd";
}

bool XMLToXSD::readResults()
{
    bool isError = false;
    _schemaData = Utils::readUTF8FileString(resultPath(), isError);
    if(isError) {
        addError(_result, QObject::tr("Unable to read schema"));
    }
    return _result->isOk();
}

QString XMLToXSD::schemaData()
{
    return _schemaData ;
}

QString XMLToXSD::dirPath()
{
    return _dirPath ;
}

QString XMLToXSD::sourceFilePath()
{
    return _sourceFilePath;
}

bool XMLToXSD::checkForConfiguration(ApplicationData *appData, QWidget *parent)
{
    QString path = appData->inst2XSDPath();
    if(!QFile::exists(path)) {
        Utils::error(parent, QObject::tr("This feature requires Apache XMLBeans. Please open configuration panel and set the path to xsd2inst"));
        return false;
    }
    return true ;
}
