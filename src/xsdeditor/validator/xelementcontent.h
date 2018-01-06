/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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


#ifndef XELEMENTCONTENT_H
#define XELEMENTCONTENT_H

#include "xsdeditor/validator/xsingleelementcontent.h"
#include "xsdeditor/xschema.h"

class XValidationContext;

#include <QObject>

class XElementContent : public QObject
{
    Q_OBJECT

    QList<XSingleElementContent*> _allowedItems;
    QList<XSingleElementContent*> _forbiddenItems;
    XSchemaAttributesCollection *_attributeContainer;

    void reset();
public:
    XElementContent();
    ~XElementContent();

    XSchemaObject *item(const QString &itemName);
    QList<XSingleElementContent*> &allowedItems();
    XSingleElementContent *addAllowed(XValidationContext *context, XSchemaObject *object);
    XSchemaAttributesCollection *attributeContainer() const;
    void setAttributeContainer(XSchemaAttributesCollection *attributeContainer);
};

#endif // XELEMENTCONTENT_H
