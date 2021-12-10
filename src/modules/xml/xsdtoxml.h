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

#ifndef XSDTOXML_H
#define XSDTOXML_H

#include "xmlEdit.h"
#include "regola.h"
#include "utils.h"
#include "operationresult.h"
#include "applicationdata.h"

class XSDToXML
{
protected:
    bool _started ;
    ApplicationData *_appData;
    OperationResult *_result;
    QTemporaryDir *_tempDir;
    QString _dirPath;
    QString _sourceFilePath;
    QString _instanceData;
    QString _localNameOfGlobalElement;

    static const int TimeoutExec = 30000;
public:
    XSDToXML(ApplicationData *appData);
    virtual ~XSDToXML();

    bool generateData(OperationResult *result, Regola *regola, const QString &localNameOfGlobalElement);
    QString data();
    static bool checkForConfiguration(ApplicationData *appData, QWidget *parent);

private:
    bool saveData(Regola *regola);
    void deleteData();
    bool addError(OperationResult *result, const QString &msgText);
    QString getXSD2Inst();

    QString trunc(const QString &msgText);
    QStringList makeArguments();
    bool readResults();
protected:
    virtual bool execute();
    QString dirPath();
    QString resultPath();
    QString sourceFilePath();

#ifdef  QXMLEDIT_TEST
    friend class TestXMLBeans;
#endif

};

#endif // XSDTOXML_H
