/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "baseddata.h"
#include "xmlutils.h"
#include "utils.h"

BaseDData::BaseDData(Element *toModifyElement)
{
    element = toModifyElement;
}

BaseDData::~BaseDData()
{
}

QString BaseDData::attributeString(const QString &name, const QString &defaultValue)
{
    Attribute *a = element->getAttribute(name);
    if(NULL != a) {
        return a->value ;
    }
    return defaultValue ;
}

void BaseDData::setAttributeStringIfMissing(const QString &name, const QString &value)
{
    Attribute *a = element->getAttribute(name);
    if(NULL == a) {
        element->setAttribute(name, value);
    }
}

void BaseDData::setAttributeString(const QString &name, const QString &value)
{
    element->setAttribute(name, value);
}

void BaseDData::assignTag(const QString &tag, Regola *regola, Element *parent)
{
    QString prefix;
    if(NULL != parent) {
        QString name;
        XmlUtils::decodeQualifiedName(parent->tag(), prefix, name);
    }
    QString newTag = XmlUtils::makeQualifiedName(prefix, tag);
    if(NULL != regola) {
        newTag =  regola->addNameToPool(newTag);
    }
    element->setTag(newTag);
}

bool BaseDData::error(QWidget *window, const QString &attrName)
{
    QString msg = QObject::tr("Invalid value for attribute '%1'").arg(attrName);
    Utils::error(window, msg);
    return false;
}

bool BaseDData::checkIDREFS(QWidget *window, const QString &attrName)
{
    QString value = element->getAttributeValue(attrName);
    if(!value.isEmpty()) {
        QStringList values = value.split(" ");
        foreach(const QString &v, values) {
            if(!XmlUtils::isNCNAME(v)) {
                return error(window, attrName);
            }
        }
    }
    return true;
}

bool BaseDData::checkNMTOKEN(QWidget *window, const QString &attrName)
{
    QString value = element->getAttributeValue(attrName);
    if(!value.isEmpty()) {
        if(!XmlUtils::isNMTOKEN(value)) {
            return error(window, attrName);
        }
    }
    return true;
}

bool BaseDData::checkID(QWidget *window, const QString &attrName)
{
    QString value = element->getAttributeValue(attrName);
    if(!value.isEmpty()) {
        if(!XmlUtils::isNCNAME(value)) {
            return error(window, attrName);
        }
    }
    return true;
}
