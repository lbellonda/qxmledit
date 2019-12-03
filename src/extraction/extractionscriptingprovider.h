/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#ifndef EXTRACTIONSCRIPTINGPROVIDER_H
#define EXTRACTIONSCRIPTINGPROVIDER_H

#include "xmlEdit.h"
#include "scripting/model/extractionscriptfiltermodel.h"

class ExtractionScriptingProvider : public QObject
{
    Q_OBJECT

public:
    enum EPredefinedScripts {
        PredefinedScriptTrimAttributes = -1,
        PredefinedScriptRemoveEmptyAttributes = -2
    };

    explicit ExtractionScriptingProvider(QObject *parent = NULL);
    virtual ~ExtractionScriptingProvider();

    QList<ExtractionScriptFilterModel*> getPredefinedScripts(bool &isError);
    ExtractionScriptFilterModel* getPredefinedScript(const EPredefinedScripts id);
    ExtractionScriptFilterModel* getPredefinedScriptFromScriptId(const QString &id);
    static QString fromStandardEnumToId(const EPredefinedScripts predefinedScriptId);

protected:
    ExtractionScriptFilterModel* readPredefinedScript(const EPredefinedScripts predefinedScript);
    ExtractionScriptFilterModel* readAPredefinedScript(const QString &id, const QString &filePath, const QString &name, const QString &description);
signals:

public slots:


};

#endif // EXTRACTIONSCRIPTINGPROVIDER_H
