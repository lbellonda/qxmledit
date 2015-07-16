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

#include "elementeditor.h"
#include "WidgetUtility.h"
#include "xsdeditor/xproperties.h"
#include "ui_elementeditor.h"

#include "utils.h"

QStringList WidgetUtility::xsdTypesList;
QSet<QString> WidgetUtility::xsdTypesSet;
bool WidgetUtility::xsdDataLoaded = false ;
extern const char * const XSDDATATYPE_NAME [] ;

void WidgetUtility::LoadComboBoxFinalType(QComboBox *combo)
{
    combo->addItem(tr(""), XSchemaObject::FINALTYPE_NONE);
    combo->addItem(tr("#all"), XSchemaObject::FINALTYPE_ALL);
    combo->addItem(tr("restriction"), XSchemaObject::FINALTYPE_RESTRICTION);
    combo->addItem(tr("extension"), XSchemaObject::FINALTYPE_EXTENSION);
    //TODO combo->addItem(tr("ext. restr."), XSchemaObject::FINALTYPE_RESTRICTION_EXTENSION );
}

void WidgetUtility::LoadComboBoxElementType(QComboBox *combo)
{
    combo->addItem(tr("Reference"), XSchemaElement::EES_REFERENCE);
    combo->addItem(tr("Simple type without attibutes (ST)"), XSchemaElement::EES_SIMPLETYPE_ONLY);
    combo->addItem(tr("Simple type with attibutes (CT SC)"), XSchemaElement::EES_SIMPLETYPE_WITHATTRIBUTES);
    combo->addItem(tr("Complex Type derived (CT, CC)"), XSchemaElement::EES_COMPLEX_DERIVED);
    combo->addItem(tr("Complex Type defined (CT)"), XSchemaElement::EES_COMPLEX_DEFINITION);
}

bool WidgetUtility::getComboBoxFinalTypeSelected(QComboBox *combo, XSchemaObject::EFinalType &result)
{
    int item = combo->currentIndex();
    if(item < 0) {
        return false ;
    }
    result = (XSchemaObject::EFinalType)(combo->itemData(item).toInt());
    return true;
}

bool WidgetUtility::getComboBoxIntSelected(QComboBox *combo, int &result)
{
    int item = combo->currentIndex();
    if(item < 0) {
        return false ;
    }
    result = combo->itemData(item).toInt();
    return true;
}

bool WidgetUtility::selectItemWithData(QComboBox *combo, const int dataToSelect)
{
    int selectedIndex = getComboItemByInt(combo, dataToSelect) ;
    combo->setCurrentIndex(selectedIndex);
    if(selectedIndex >= 0) {
        return true ;
    }
    return false;
}

int WidgetUtility::getComboItemByInt(QComboBox *combo, const int dataToSelect)
{
    int items = combo->count();
    for(int i = 0 ; i < items ; i ++) {
        int tag = combo->itemData(i).toInt();
        if(dataToSelect == tag) {
            return i;
        }
    }
    return -1 ;
}

void WidgetUtility::loadComboTypes(QComboBox *combo, QStringList &customTypes)
{
    // load std types
    QStringList types = WidgetUtility::getStdTypes();
    // load  custom types
    combo->addItems(types);
    combo->addItems(customTypes);
}


QStringList WidgetUtility::getStdTypes()
{
    if(!xsdDataLoaded) {
        // build a list, then an hash set
        const char * const * scanPtr = XSDDATATYPE_NAME ;
        //char ** scanPtr = XSDDATATYPE_NAME ;
        while(*scanPtr) {
            QString data = *scanPtr;
            scanPtr++;
            xsdTypesList.append(data);
            xsdTypesSet.insert(data);
        }
    }
    return xsdTypesList ;
}

QStringList WidgetUtility::getStdTypes(const QString &prefix)
{
    QStringList result ;
    QStringList intermediateRes = getStdTypes();
    if(prefix != "xs") {
        QString newPrefix = prefix ;
        if(!prefix.isEmpty()) {
            newPrefix += ":";
        }
        foreach(QString str, intermediateRes) {
            result << str.replace("xs:", newPrefix);
        }
        return result ;
    } else {
        return intermediateRes ;
    }
}


const char * const XSDDATATYPE_NAME [] = {

    "xs:anyURI",
    "xs:base64Binary",
    "xs:boolean",
    "xs:byte",
    "xs:date",
    "xs:dateTime",
    "xs:decimal",
    "xs:double",
    "xs:duration",
    "xs:float",
    "xs:gDay",
    "xs:gMonth",
    "xs:gMonthDay",
    "xs:gYear",
    "xs:gYearMonth",
    "xs:hexBinary",
    "xs:ID",
    "xs:IDREF",
    "xs:IDREFS",
    "xs:int",
    "xs:integer",
    "xs:language",
    "xs:long",
    "xs:Name",
    "xs:NCName",
    "xs:negativeInteger",
    "xs:NMTOKEN",
    "xs:NMTOKENS",
    "xs:nonNegativeInteger",
    "xs:nonPositiveInteger",
    "xs:normalizedString",
    "xs:positiveInteger",
    "xs:QName",
    "xs:short",
    "xs:string",
    "xs:time",
    "xs:token",
    "xs:unsignedByte",
    "xs:unsignedInt",
    "xs:unsignedLong",
    "xs:unsignedShort",
    NULL
};

void WidgetUtility::setCheckState(const XEnums::XBool value, QCheckBox *box)
{
    switch(value) {
    default:
    case XEnums::XBOOL_UNSET:
        box->setCheckState(Qt::PartiallyChecked);
        break;
    case XEnums::XBOOL_FALSE:
        box->setCheckState(Qt::Unchecked);
        break;
    case XEnums::XBOOL_TRUE:
        box->setCheckState(Qt::Checked);
        break;
    }
}

XEnums::XBool WidgetUtility::getCheckState(QCheckBox *box)
{
    switch(box->checkState()) {
    default:
    case Qt::PartiallyChecked:
        return XEnums::XBOOL_UNSET;
    case Qt::Unchecked:
        return XEnums::XBOOL_FALSE ;
    case Qt::Checked:
        return XEnums::XBOOL_TRUE ;
    }
}
