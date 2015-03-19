/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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

#include "comparetreewithxml.h"

CompareTreeWithXml::CompareTreeWithXml()
{
}

CompareTreeWithXml::~CompareTreeWithXml()
{
}


Regola *CompareTreeWithXml::loadReference(const QString &fileName)
{
    Regola *regola = NULL ;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        QString errorMsg ;
        int errorLine = 0, errorColumn = 0;
        if(document.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
            regola = new Regola(document, fileName);
        }
        file.close();
    }
    return regola ;
}

bool CompareTreeWithXml::compareWith( QVector<Element*> *elements, QList<QTreeWidgetItem*> &items)
{
    if( elements->size() != items.size() ) {
        _error = "element size differ";
        return false;
    }
    int index = 0;
    foreach( Element* element, *elements ) {
        QTreeWidgetItem* item = items.at(index);
        index++;
        if(!compareItem(element, item)) {
            _error = "element does not compare equal";
            return false;
        }
        QList<QTreeWidgetItem*> itemChildren;
        int itemChildrenCount = item->childCount();
        for( int i = 0 ; i < itemChildrenCount ; i ++ ) {
            itemChildren.append(item->child(i));
        }
        if(!compareWith(element->getChildItems(), itemChildren)) {
            return false;
        }
    }
    //===
    return true;
}


// TODO code the colors
bool CompareTreeWithXml::compareItem( Element* element, QTreeWidgetItem* item)
{
    QString textE ;
    Attribute * attributeT = element->getAttribute("text");
    if( NULL != attributeT ) {
        textE = attributeT->value;
    }
    QString colorName ;
    Attribute * attributeC = element->getAttribute("color");
    if( NULL != attributeC ) {
        colorName = attributeC->value;
    }
    if(!QColor::isValidColor(colorName)) {
        return false;
    }
    QColor colorE;
    colorE.setNamedColor(colorName);

    QString textI = item->text(0);
    QColor colorI = item->backgroundColor(0);

    if( textE != textI ) {
        return false;
    }
    if( colorI != colorE ) {
        return false;
    }
    return true ;
}


bool CompareTreeWithXml::compareTreeWithFile(const QString &fileName, QTreeWidget *tree)
{
    bool result = false;
    Regola *regola = loadReference(fileName);
    if( NULL != regola ) {

        QList<QTreeWidgetItem*> itemChildren;
        int itemChildrenCount = tree->topLevelItemCount();
        for( int i = 0 ; i < itemChildrenCount ; i ++ ) {
            itemChildren.append(tree->topLevelItem(i));
        }
        result = compareWith( regola->getChildItems(), itemChildren);
        delete regola;
    }
    return result;
}


