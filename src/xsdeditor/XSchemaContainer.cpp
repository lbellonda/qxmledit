/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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

#include "xsdeditor/xschema.h"

XSchemaContainer::XSchemaContainer(XSchemaObject *newParent, XSchemaRoot *newRoot): XSchemaObject(newParent, newRoot)
{
}

XSchemaContainer::~XSchemaContainer()
{
    reset();
}


void XSchemaContainer::reset()
{
    _existingChildren.clear();
    XSchemaObject::reset();
}


bool XSchemaContainer::generateDom(QDomDocument & /*document*/, QDomNode & /*parent*/)
{
    return false;
}

XSDCompareObject::EXSDCompareObject XSchemaContainer::innerCompareTo(XSchemaObject * /*target*/, XSDCompareOptions & /*options*/)
{
    return XSDCompareObject::XSDOBJECT_DIFFERENT;
}

QString XSchemaContainer::label() const
{
    return _label;
}

void XSchemaContainer::setLabel(const QString &value)
{
    _label = value;
}

void XSchemaContainer::setFromElement(XSchemaElement *element)
{
    QString title = element->isTypeOrElement() ? tr("Type: %1") : tr("Element: %1");
    setLabel(title.arg(element->name()));
    setName(element->name());
}

void XSchemaContainer::setFromGroup(XSchemaGroup *object)
{
    QString title = tr("Group: %1") ;
    setLabel(title.arg(object->name()));
    setName(object->name());
}

void XSchemaContainer::setFromAttributeGroup(XSchemaAttributeGroup *object)
{
    QString title = tr("Attribute group: %1") ;
    setLabel(title.arg(object->name()));
    setName(object->name());
}

void XSchemaContainer::setFromAttribute(XSchemaAttribute *attribute)
{
    QString title = tr("Attribute: %1");
    setLabel(title.arg(attribute->name()));
    setName(attribute->name());
}


//------------------------------------------------------------------------
