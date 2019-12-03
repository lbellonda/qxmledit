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

#include "extractionscriptingprovider.h"
#include "utils.h"

#define FILE_PredefinedScriptTrimAttributes    ":/extractionscript/trimAttributes.xml"
#define FILE_PredefinedScriptRemoveEmptyAttributes ":/extractionscript/removeEmptyAttributes.xml"

ExtractionScriptingProvider::ExtractionScriptingProvider(QObject *parent) : QObject(parent)
{
}

ExtractionScriptingProvider::~ExtractionScriptingProvider()
{
}

ExtractionScriptFilterModel* ExtractionScriptingProvider::getPredefinedScriptFromScriptId(const QString &idString)
{
    if(QString("%1").arg(PredefinedScriptTrimAttributes) == idString) {
        return readPredefinedScript(PredefinedScriptTrimAttributes);
    }
    if(QString("%1").arg(PredefinedScriptRemoveEmptyAttributes) == idString) {
        return readPredefinedScript(PredefinedScriptRemoveEmptyAttributes);
    }
    return NULL ;
}

ExtractionScriptFilterModel* ExtractionScriptingProvider::getPredefinedScript(const EPredefinedScripts id)
{
    ExtractionScriptFilterModel* result = readPredefinedScript(id);
    return result ;
}

QList<ExtractionScriptFilterModel*> ExtractionScriptingProvider::getPredefinedScripts(bool &isError)
{
    isError = false;
    QList<ExtractionScriptFilterModel*> result;
    ExtractionScriptFilterModel* trimAttributesModel = readPredefinedScript(PredefinedScriptTrimAttributes);
    ExtractionScriptFilterModel* emptyAttributesModel = readPredefinedScript(PredefinedScriptRemoveEmptyAttributes);
    if((NULL != trimAttributesModel) && (NULL != emptyAttributesModel)) {
        result.append(trimAttributesModel);
        result.append(emptyAttributesModel);
    } else {
        isError = true ;
        DELETE_IF_NOTNULL(trimAttributesModel)
        DELETE_IF_NOTNULL(emptyAttributesModel)
    }
    return result ;
}

QString ExtractionScriptingProvider::fromStandardEnumToId(const EPredefinedScripts predefinedScriptId)
{
    return QString("%1").arg(predefinedScriptId);
}

ExtractionScriptFilterModel* ExtractionScriptingProvider::readPredefinedScript(const EPredefinedScripts predefinedScript)
{
    switch(predefinedScript) {
    default:
        return NULL;
    case PredefinedScriptTrimAttributes:
        return readAPredefinedScript(fromStandardEnumToId(predefinedScript), FILE_PredefinedScriptTrimAttributes, tr("Trim Attributes"), tr("Trim attributes"));
    case PredefinedScriptRemoveEmptyAttributes:
        return readAPredefinedScript(fromStandardEnumToId(predefinedScript), FILE_PredefinedScriptRemoveEmptyAttributes, tr("Remove empty attributes"), tr("Remove empty attributes"));
    }
}

ExtractionScriptFilterModel* ExtractionScriptingProvider::readAPredefinedScript(const QString &id, const QString &filePath, const QString &name, const QString &description)
{
    bool isError = false;
    QString codedValue = Utils::readUTF8FileString(filePath, isError);
    if(isError) {
        return  NULL ;
    }
    ExtractionScriptFilterModel* model = ExtractionScriptFilterModel::fromXMLString(codedValue);
    model->setId(id);
    model->setName(name);
    model->setDescription(description);
    return model ;
}
