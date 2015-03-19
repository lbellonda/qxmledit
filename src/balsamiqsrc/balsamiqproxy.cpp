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

#include "xmlEdit.h"
#include "balsamiqproxy.h"
#include <stdio.h>
#include <QUrl>

#define STRING_TRUE "true"

BalsamiqProxy::BalsamiqProxy()
{
}

BalsamiqProxy::~BalsamiqProxy()
{
    reset();
}

void BalsamiqProxy::reset()
{
    foreach(BalsamiqProxy * proxy, _children) {
        delete proxy;
    }
    _children.clear();
}

void BalsamiqProxy::setProperty(const QString &key, const QString &value)
{
    _properties.insert(key, value);
}

void BalsamiqProxy::setAttribute(const QString &key, const QString &value)
{
    _attributes.insert(key, value);
}

void BalsamiqProxy::setField(const QString &key, const QString &value)
{
    _calculatedFields.insert(key, value);
}

void BalsamiqProxy::addChild(BalsamiqProxy* child)
{
    _children.append(child);
}

QString BalsamiqProxy::type()
{
    return _controlType ;
}

void BalsamiqProxy::setType(const QString &newType)
{
    _controlType = newType ;
}

QString BalsamiqProxy::id()
{
    return _id ;
}

void BalsamiqProxy::setId(const QString &newId)
{
    _id = newId ;
}

QVector<BalsamiqProxy*> &BalsamiqProxy::children()
{
    return _children ;
}

QMap<QString, QString> &BalsamiqProxy::calculatedFields()
{
    return _calculatedFields;
}

QString BalsamiqProxy::attribute(const QString &name)
{
    if(_attributes.contains(name)) {
        return _attributes[name];
    } else {
        return "" ;
    }
}

QString BalsamiqProxy::property(const QString &name)
{
    if(_properties.contains(name)) {
        return _properties[name];
    } else {
        return "" ;
    }
}

QString BalsamiqProxy::field(const QString &name)
{
    if(_calculatedFields.contains(name)) {
        return _calculatedFields[name];
    } else {
        return "" ;
    }
}

void BalsamiqProxy::normalizeDimensions()
{
    int sizeW = attribute(BMML_ATTRIBUTE_W).toInt();
    int sizeH = attribute(BMML_ATTRIBUTE_H).toInt();
    if(-1 == sizeW) {
        sizeW = attribute(BMML_ATTRIBUTE_MEASUREDW).toInt() ;
    }
    if(-1 == sizeH) {
        sizeH = attribute(BMML_ATTRIBUTE_MEASUREDH).toInt() ;
    }
    if(0 == sizeW) {
        sizeW = -1 ;
    }
    if(0 == sizeH) {
        sizeH = -1 ;
    }

    setField(BMML_FIELD_ATTRIBUTE_W, QString::number(sizeW)) ;
    setField(BMML_FIELD_ATTRIBUTE_H, QString::number(sizeH)) ;
}

void BalsamiqProxy::normalizeId(BalsamiqProxy *parent)
{
    if(NULL == parent) {
        return ;
    }
    QString parentId = parent->id();
    QString newId = parentId + "__" + id();
    setId(newId);
    setAttribute(BMML_ATTRIBUTE_ID, newId);
}

void BalsamiqProxy::filterText()
{
    QString text = property(BMML_PROPERTY_TEXT);
    if(!text.isEmpty()) {
        QString txt = QUrl::fromPercentEncoding(text.toLatin1());
        // convert to xml data
        int len = txt.length();
        QString  newText ;
        for(int i = 0 ; i < len ; i ++) {
            QChar ch = txt.at(i);
            if(!((ch == '<') || (ch == '>') || (ch == '&'))) {       //TODO consider .isLetterOrNumber() || ch.isSpace() ) {
                newText += ch;
            } else {
                newText += "&#";
                newText += QString::number(ch.unicode());
                newText += ";";
            }
        }
        setProperty(BMML_PROPERTY_TEXT, newText);
    }
}

void BalsamiqProxy::filterColor()
{
    QString text = property(BMML_PROPERTY_COLOR);
    if(!text.isEmpty()) {
        int color = text.toInt();
        QString colorString = QString("color=\"#%1\"").arg(color, 0, 16);
        // convert to xml data
        setField(BMML_FIELD_HEXCOLOR, colorString);
    }
}

void BalsamiqProxy::filterFontSize()
{
    QString text = property(BMML_PROPERTY_SIZE);
    if(!text.isEmpty()) {
        int size = text.toInt();
        QString fontString = QString("fontSize=\"%1\"").arg(size);
        // convert to xml data
        setField(BMML_FIELD_FONTSIZE, fontString);
    }
}

void BalsamiqProxy::houseWork(BalsamiqProxy *parent)
{
    normalizeId(parent);
    filterText();
    filterColor();
    filterFontSize();
    normalizeDimensions();
}

bool BalsamiqProxy::isInGroup()
{
    QString groupId = attribute(BMML_ATTRIBUTE_ISINGROUP);
    if(groupId.toInt() > 0) {
        return true ;
    }
    return false;
}

bool BalsamiqProxy::hasHeader()
{
    QString header = property(BMML_PROPERTY_HASHEADER);
    if(header == STRING_TRUE) {
        return true ;
    }
    return false;
}

void BalsamiqProxy::dump()
{
    printf("------------------------\n");
    printf(" Attributes:\n");
    QMap<QString, QString>::iterator iterator;
    for(iterator = _attributes.begin(); iterator != _attributes.end(); ++iterator) {
        QString res = QString(" -%1: %2\n").arg(iterator.key()).arg(iterator.value());
        printf("%s", res.toLatin1().data()) ;
    }
    printf(" Properties:\n");
    for(iterator = _properties.begin(); iterator != _properties.end(); ++iterator) {
        QString res = QString(" -%1: %2\n").arg(iterator.key()).arg(iterator.value());
        printf("%s", res.toLatin1().data()) ;
    }
    printf("------------------------\n");
}
