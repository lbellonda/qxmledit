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

#ifndef BALSAMIQPROXY_H
#define BALSAMIQPROXY_H

#include <QMap>
#include <QVector>
#include <QString>
#include <QDomNode>

#define BMML_FIELD_HEXCOLOR     "hexColor"
#define BMML_FIELD_FONTSIZE     "fontSize"
#define BMML_FIELD_ATTRIBUTE_W  "w"
#define BMML_FIELD_ATTRIBUTE_H  "h"
#define BMML_FIELD_ATTRIBUTE_TREEDATA  "DP_TREEDATA"


#define BMML_APPLICATION_TAG "**:Application"
#define BMML_PROPERTY_TEXT  "text"
#define BMML_PROPERTY_HASHEADER "hasHeader"
#define BMML_ATTRIBUTE_ISINGROUP "isInGroup"
#define BMML_ATTRIBUTE_X    "x"
#define BMML_ATTRIBUTE_Y    "y"
#define BMML_ATTRIBUTE_ID   "controlID"
#define BMML_PROPERTY_SIZE  "size"
#define BMML_PROPERTY_COLOR "color"
#define BMML_ATTRIBUTE_MEASUREDW    "measuredW"
#define BMML_ATTRIBUTE_MEASUREDH    "measuredH"
#define BMML_ATTRIBUTE_W    "w"
#define BMML_ATTRIBUTE_H    "h"


#define BMML_BUTTON    "com.balsamiq.mockups::Button"
#define BMML_LABEL    "com.balsamiq.mockups::Label"
#define BMML_LINKBAR   "com.balsamiq.mockups::LinkBar"
#define BMML_DATAGRID   "com.balsamiq.mockups::DataGrid"
#define BMML_LIST       "com.balsamiq.mockups::List"
#define BMML_TABBAR       "com.balsamiq.mockups::TabBar"
#define BMML_VRULE      "com.balsamiq.mockups::VRule"
#define BMML_HRULE      "com.balsamiq.mockups::HRule"
#define BMML_TEXTINPUT  "com.balsamiq.mockups::TextInput"
#define BMML_COMBOBOX   "com.balsamiq.mockups::ComboBox"
#define BMML_TEXTAREA   "com.balsamiq.mockups::TextArea"
#define BMML_PARAGRAPH   "com.balsamiq.mockups::Paragraph"
#define BMML_CHECKBOX   "com.balsamiq.mockups::CheckBox"
#define BMML_RADIOBUTTON    "com.balsamiq.mockups::RadioButton"
#define BMML_TREE      "com.balsamiq.mockups::Tree"

#define BMML_GROUP   "__group__"

class BalsamiqProxy
{
    QString _controlType;
    QMap<QString, QString> _properties;
    QMap<QString, QString> _attributes;
    QMap<QString, QString> _calculatedFields;
    QVector<BalsamiqProxy*> _children;
    QString _id;

    void reset();

public:
    BalsamiqProxy();
    ~BalsamiqProxy();

    QDomNode node;

    QString type();
    QString id();
    void setType(const QString &newType);
    void setId(const QString &newId);
    void setProperty(const QString &key, const QString &value);
    void setAttribute(const QString &key, const QString &value);
    void setField(const QString &key, const QString &value);
    void addChild(BalsamiqProxy *newChild);
    QVector<BalsamiqProxy*> &children();
    QMap<QString, QString> &calculatedFields();
    void normalizeId(BalsamiqProxy *parent);
    void houseWork(BalsamiqProxy *parent);
    bool isInGroup();
    bool hasHeader();

    QString attribute(const QString &name);
    QString property(const QString &name);
    QString field(const QString &name);
    void dump();

private:
    void filterText();
    void filterFontSize();
    void filterColor();
    void normalizeDimensions();
};

#endif // BALSAMIQPROXY_H
