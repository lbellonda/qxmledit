/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#include "xsdeditor/validator/xelementcontent.h"
#include "xsdeditor/xschema.h"

XElementContent::XElementContent()
{
    _attributeContainer = NULL ;
}

XElementContent::~XElementContent()
{
    reset();
    if(NULL != _attributeContainer) {
        delete _attributeContainer ;
    }
}


XSchemaAttributesCollection *XElementContent::attributeContainer() const
{
    return _attributeContainer;
}

void XElementContent::setAttributeContainer(XSchemaAttributesCollection *attributeContainer)
{
    _attributeContainer = attributeContainer;
}

void XElementContent::reset()
{
    foreach(XSingleElementContent * child, _allowedItems) {
        delete child ;
    }
    _allowedItems.clear();
    foreach(XSingleElementContent * child, _forbiddenItems) {
        delete child ;
    }
    _forbiddenItems.clear();
}

XSchemaObject *XElementContent::item(const QString &itemName)
{
    foreach(XSingleElementContent * child, _allowedItems) {
        if(child->item()->name() == itemName) {
            return child->item();
        }
    }
    return NULL ;
}

QList<XSingleElementContent*> &XElementContent::allowedItems()
{
    return _allowedItems;
}

XSingleElementContent *XElementContent::addAllowed(XValidationContext *context, XSchemaObject *object)
{
    XSingleElementContent *newAllowed = new XSingleElementContent(object);
    if(NULL != newAllowed) {
        _allowedItems.append(newAllowed);
        context->setCurrentTarget(newAllowed);
    } else {
        context->setCurrentTarget(NULL);
        context->setError(QString(tr("Unable to add allowed item %1")).arg((NULL != object) ? object->name() : "?"));
    }
    return newAllowed ;
}

