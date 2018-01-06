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

#include "comparexsdwithxml.h"
#include <xsdeditor/xschema.h>
#include <xsdeditor/xsdwindow.h>
#include <element.h>

CompareXSDWithXML::CompareXSDWithXML()
{
}

CompareXSDWithXML::~CompareXSDWithXML()
{
}


Regola *CompareXSDWithXML::loadReference(const QString &fileName)
{
    Regola *regola = NULL ;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        QString errorMsg ;
        int errorLine = 0, errorColumn = 0;
        if(document.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
            regola = new Regola(document, fileName);
        } else {
            error(QString("CompareXSDWithXML: data not xml '%1'").arg(fileName));
        }
        file.close();
    } else {
        error(QString("CompareXSDWithXML:Unable to load file '%1'").arg(fileName));
    }
    return regola ;
}

bool CompareXSDWithXML::compareTreeWithFile(const QString &fileName, XSDItem *item)
{
    bool result = false;
    Regola *regola = loadReference(fileName);
    QList<int> path ;
    QList<QString> pathString ;
    if( NULL != regola ) {
        path.append(0);
        pathString.append(regola->root()->tag());
        if(compareWith(regola->root(), item, path, pathString)) {
            result = true ;
        }
        delete regola;
    }
    return result;
}

bool CompareXSDWithXML::compareWith( Element *element, XSDItem *item, QList<int> &path, QList<QString> &pathString )
{
    if( !compareItem(element, item, path, pathString) ) {
        return false;
    }
    RChildren *itemChildren = item->rChildren();
    if( element->getChildItems()->size() != itemChildren->childrenSize() ) {
        return error( QString("element size differ, expected:%1 found:%2  path: %3").arg(element->getChildItems()->size()).arg(itemChildren->childrenSize()).arg(makePath(path,pathString)));
    }
    int index = 0;
    foreach( Element* childElement, element->getChildItemsRef() ) {
        XSDItem* itemChild = itemChildren->childAt(index)->item();
        path.append(index);
        pathString.append(childElement->tag());
        if(!compareWith( childElement, itemChild, path, pathString )) {
            return false;
        }
        path.removeLast();
        pathString.removeLast();
        index++;
    }
    return true;
}

bool CompareXSDWithXML::compareItem( Element* element, XSDItem *item, QList<int> &path, QList<QString> &pathString )
{
    QString elementType = element->tag();
    QString elementName ;
    Attribute * attributeC = element->getAttribute("name");
    if( NULL != attributeC ) {
        elementName = attributeC->value;
    }

    QString itemClassName = item->metaObject()->className();
    if( elementType != itemClassName ) {
        return error(QString("Element expected:'%1', found:'%2' path:%3").arg(elementType).arg(itemClassName).arg(makePath(path,pathString)));
    }

    if( !elementName.isEmpty() ) {
        QString itemName ;
        if( NULL != item->item() ) {
            itemName = item->item()->name();
        }
        if( elementName != itemName ) {
            return error(QString("Element name expected:'%1', found:'%2' path:%3").arg(elementName).arg(itemName).arg(makePath(path,pathString)));
        }
    }
    return true ;
}


QString CompareXSDWithXML::makePath( QList<int> &path, QList<QString> &pathString )
{
    QString result = " numeric:*";
    result += listIntToString(path);
    result += "* as path:";
    result += QStringList(pathString).join(",");
    return result;
}

//-----------------------------------------------


QString CompareXSDWithXML::dumpItemTree( XSDItem *item, int indent)
{
    QString result ;
    result += dumpItem(item, indent);
    RChildren *itemChildren = item->rChildren();
    for( int index = 0; index < itemChildren->childrenSize() ; index++ ) {
        XSDItem* childItem = itemChildren->childAt(index)->item();
        result += dumpItemTree( childItem, indent+2);
    }
    result += indents(indent);
    result += closeItem(item, indent);
    return result;
}

QString CompareXSDWithXML::dumpItem( XSDItem *item, int indent )
{
    QString result = indents(indent);
    QString itemClassName = item->metaObject()->className();
    result += "<";
    result += itemClassName ;
    result += " name=\""  ;
    QString itemName ;
    if( NULL != item->item() ) {
        itemName = item->item()->name();
    }
    result += itemName  ;
    result += "\">\n"  ;
    return result ;
}

QString CompareXSDWithXML::closeItem( XSDItem *item, int indent )
{
    QString result = indents(indent);
    QString itemClassName = item->metaObject()->className();
    result += "</";
    result += itemClassName ;
    result += ">\n"  ;
    return result ;
}

QString CompareXSDWithXML::indents( int indent )
{
    QString result ;
    for( int i = 0 ; i < indent ; i++ ) {
        result += " ";
    }
    return result ;

}
