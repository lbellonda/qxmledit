/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#include "xsdvalidationexecutor.h"
#include "utils.h"
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include "validatormessagehandler.h"
#include "schemavalidator.h"

XSDValidationExecutor::XSDValidationExecutor()
{

}

XSDValidationExecutor::~XSDValidationExecutor()
{

}

QPair<int, QString> XSDValidationExecutor::execute(const QString &dataFile, const QString &schemaFile)
{
    Utils::TODO_THIS_RELEASE("fare");
    QXmlSchema schemaHandler;
    ValidatorMessageHandler messageHandler;
    QUrl schemaUrl = QUrl::fromLocalFile(schemaFile);
    if(!schemaHandler.load(schemaUrl)) {
        return QPair<int, QString>(-1, QObject::tr("Error loading schema."));
    }
    if(!schemaHandler.isValid()) {
        return QPair<int, QString>(-2, QObject::tr("Schema is invalid."));
    }
    schemaHandler.setMessageHandler(&messageHandler);
    QXmlSchemaValidator schemaValidator(schemaHandler);
    QUrl dataUrl = QUrl::fromLocalFile(dataFile);
    if(schemaValidator.validate(dataUrl)) {
        return QPair<int, QString>(0, QObject::tr("XML is valid."));
    } else {
        QString msg = QObject::tr("%1\nError: %2").arg(QObject::tr("XML does not conform to schema. Validation failed.")).arg(messageHandler.descriptionInPlainText());
        return QPair<int, QString>(1, msg);
    }
}
