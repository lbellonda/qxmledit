/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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

#include "xmleditwidgetprivate.h"

#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QFileInfo>
#include "xmleditwidget.h"
#include "xsdeditor/io/xschemaloader.h"
#include "validatormessagehandler.h"
#include "schemavalidator.h"
#include "elementitemdelegate.h"
#include "alloweditemsinsert.h"
#include "qattributedelegate.h"
#include "config.h"
#include "findtextparams.h"
#include "regola.h"
#include "utils.h"
#include "xsdeditor/xsdplugin.h"
#include "extraction/extractresults.h"
#include "xmleditwidgetprivate.h"
#include "xsdeditor/xsdcompare.h"
#include "modules/graph/nodesrelationsdialog.h"
#include "modules/widgets/lineeditwithcompleter.h"
#include "modules/search/searchresultsdialog.h"
#include "xmlwidgetutilities.h"
#include "modules/xslt/xsltnavigatordialog.h"
#include "modules/metadata/metadatainfodialog.h"
#include "modules/encoding/codepagedialog.h"
#include "modules/delegates/elementitemsingledelegate.h"
#include "modules/xsd/xsdhelper.h"
#include "modules/xsd/xsdtypedialog.h"
#include "modules/xsd/xsdhelper.h"
#include "modules/xsd/xsddefaultannotationeditor.h"
#include "modules/xml/xmlindentationdialog.h"

//-----------------------------------

void XmlEditWidgetPrivate::validateUsingDocumentReferences()
{
    if(NULL == regola) {
        return;
    }
    if(!regola->userDefinedXsd().isEmpty()) {
        regola->setUserDefinedXsd("");
    }
    onActionValidate();
    emit p->newXSDSchemaForValidation("");
}


void XmlEditWidgetPrivate::onActionValidate()
{
    if(NULL == regola) {
        return;
    }
    QString schemaUrl = regola->userDefinedXsd();
    validateWithFile(schemaUrl);
}


void XmlEditWidgetPrivate::validateWithFile(const QString &schemaUrl)
{
    if(NULL == regola) {
        return;
    }
    QXmlSchema schemaHandler;
    ValidatorMessageHandler messageHandler;
    if(!schemaUrl.isEmpty()) {
        QFile schemaFile(schemaUrl);
        schemaFile.unsetError();
        if(schemaFile.open(QIODevice::ReadOnly)) {
            QByteArray schema = schemaFile.readAll() ;
            if(schemaFile.error()) {
                schemaFile.close();
                Utils::error(tr("Error opening schema file."));
                return ;
            }
            schemaFile.close();
            if(!schemaHandler.load(schema)) {
                Utils::error(p->window(), tr("Error loading schema"));
                return ;
            }
            if(!schemaHandler.isValid()) {
                Utils::error(p, tr("Schema is invalid"));
                return ;
            }
        } // if file
    }
    QByteArray dataXml = regola->getAsText().toUtf8();
    schemaHandler.setMessageHandler(&messageHandler);
    QXmlSchemaValidator schemaValidator(schemaHandler);
    if(schemaValidator.validate(dataXml)) {
        Utils::message(p, tr("XML is valid."));
    } else {
        Utils::error(p, tr("%1\nError: %2").arg(tr("XML does not conform to schema. Validation failed.")).arg(messageHandler.descriptionInPlainText()));
        showValidationResults(QString::fromUtf8(dataXml), messageHandler) ;
    }
}


void XmlEditWidgetPrivate::onActionValidateNewFile()
{
    if(NULL == regola) {
        return;
    }
    QString filePath = QFileDialog::getOpenFileName(p, tr("Open Schema File"),
                       QXmlEditData::sysFilePathForOperation(regola->fileName()), tr("XML Schema files (*.xsd);;All files (*);;"));
    if(!filePath.isEmpty()) {
        regola->setUserDefinedXsd(filePath);
        onActionValidate();
        computeSelectionState();
        emit p->newXSDSchemaForValidation(filePath);
    }
}
