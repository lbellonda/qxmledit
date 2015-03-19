/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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

#include "compareregolawitheditor.h"

CompareRegolaWithEditor::CompareRegolaWithEditor()
{
}

CompareRegolaWithEditor::~CompareRegolaWithEditor()
{
}

QString CompareRegolaWithEditor::errorString()
{
    return _error;
}

bool CompareRegolaWithEditor::error(const QString &msg)
{
    _error = msg;
    return false ;
}

bool CompareRegolaWithEditor::compareTreeWithRegola(Regola *regola, QTreeWidget *tree)
{
    bool result = false;

    QList<QTreeWidgetItem*> itemChildren;
    int itemChildrenCount = tree->topLevelItemCount();
    for( int i = 0 ; i < itemChildrenCount ; i ++ ) {
        itemChildren.append(tree->topLevelItem(i));
    }
    result = compareWith( regola->getChildItems(), itemChildren, NULL);
    return result;
}


bool CompareRegolaWithEditor::compareWith( QVector<Element*> *elements, QList<QTreeWidgetItem*> &items, Element* parent)
{
    if( elements->size() != items.size() ) {
        return error(QString("element size differ at %1 size: %2/%3").arg(makePath(parent)).arg(elements->size()).arg(items.size())) ;
    }
    int index = 0;
    foreach( Element* element, *elements ) {
        QTreeWidgetItem* item = items.at(index);
        if(!compareItem(element, item)) {
            return error(QString("element does not compare equal at %1 / %2").arg(makePath(parent)).arg(index));
        }
        QList<QTreeWidgetItem*> itemChildren;
        int itemChildrenCount = item->childCount();
        for( int i = 0 ; i < itemChildrenCount ; i ++ ) {
            itemChildren.append(item->child(i));
        }
        if(!compareWith(element->getChildItems(), itemChildren, element)) {
            return false;
        }
        index++;
    }
    //===
    return true;
}


// TODO code the colors
bool CompareRegolaWithEditor::compareItem( Element* element, QTreeWidgetItem* item)
{
    Element *el = Element::fromItemData(item);
    if( el->getType() != element->getType() ) {
        return error(QString("element cmp type '%1' / '%2' at %3").arg(el->getType() ).arg(element->getType()).arg(makePath(element)));
    }
    if( el->isElement() ) {
        if( el->tag() != element->tag() ) {
            return error(QString("element tag '%1' / '%2' at %3").arg(el->tag()).arg(element->tag()).arg(makePath(element)));
        }
    }
    return true ;
}

QString CompareRegolaWithEditor::makePath( Element* element)
{
    if( NULL == element ) {
        return "?";
    }
    QList<int> path = element->indexPath();
    QString pathS = "" ;
    bool isFirst = true;
    foreach( int i, path ) {
        if(isFirst) {
            isFirst = false ;
        } else {
            pathS += ",";
        }
        pathS += QString("%1").arg(i) ;
    }
    return pathS ;
}
