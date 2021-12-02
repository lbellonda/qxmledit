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

#ifndef XMLTOXSD_H
#define XMLTOXSD_H

#include "xmlEdit.h"
#include "regola.h"
#include "utils.h"
#include "operationresult.h"
#include "applicationdata.h"

class XMLToXSD
{
    bool _started ;
    ApplicationData *_appData;
    QTemporaryDir *_tempDir;
    QString _dirPath;
    QString _sourceFilePath;
    QString _schemaData;
protected:
    OperationResult *_result;

    static const int TimeoutExec = 30000;
public:
    enum GenXSDOption {
        GENXSD_RUSSIAN_DOLL,
        GENXSD_SALAMI_SLICE,
        GENXSD_VENETIAN_BLIND,
        GENXSD_DEFAULT = GENXSD_VENETIAN_BLIND
    };

    XMLToXSD(ApplicationData *appData);
    virtual ~XMLToXSD();

    bool generateXSD(OperationResult *result, Regola *regola, const GenXSDOption option, const int enumerationThreshold, const bool simpleContentTypeSmart);
    QString schemaData();
private:

    bool saveData(Regola *regola);
    void deleteData();
    bool addError(OperationResult *result, const QString &msgText);
    QString getInst2XSD();
    virtual bool execute(const GenXSDOption option, const int enumerationThreshold, const bool simpleContentTypeSmart);
    QString trunc(const QString &msgText);
    QStringList makeArguments(const GenXSDOption option, const int enumerationThreshold, const bool simpleContentTypeSmart);
    bool readResults();
protected:
    QString dirPath();
    QString resultPath();
    QString sourceFilePath();

#ifdef  QXMLEDIT_TEST
    friend class TestXMLBeans;
#endif
};

#endif // XMLTOXSD_H
