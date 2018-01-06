/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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
#include <QClipboard>
#include <QApplication>
#include <QFile>
#include "utils.h"
#include "xsdeditor/XSchemaIOContants.h"


XSchemaOther::XSchemaOther(XSchemaObject *newParent, XSchemaRoot *newRoot): XSchemaObject(newParent, newRoot)
{
    extElement = NULL ;
}

XSchemaOther::~XSchemaOther()
{
    reset();
}

void XSchemaOther::setElement(Element *newElement)
{
    extElement = newElement;
}

Element *XSchemaOther::element()
{
    return extElement ;
}

void XSchemaOther::reset()
{
    // nothing
}

XSchemaOther* XSchemaOther::clone()
{
    XSchemaOther *cloned = new XSchemaOther(NULL, NULL);
    cloned->extData = extData;
    cloned->extElement = extElement;
    return cloned;
}

QVariant XSchemaOther::getExtData() const
{
    return extData;
}

void XSchemaOther::setExtData(const QVariant &value)
{
    extData = value;
}
