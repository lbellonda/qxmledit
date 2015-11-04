/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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

#include "replicamanager.h"
#include "regola.h"
#include "modules/replica/replicacommand.h"

ReplicaManager::ReplicaManager(QObject *parent) :
    QObject(parent)
{
}

ReplicaManager::~ReplicaManager()
{
}

bool ReplicaManager::apply(QTreeWidget *widget, Regola *regola, Element *selected, ReplicaCommand *cmd)
{
    bool changed = false;

    if(NULL != widget) {
        widget->setUpdatesEnabled(false);
    }
    regola->clearUndo();
    if(NULL == selected->parent()) {
        changed = applyReplicaToElement(regola, cmd, selected, 0, 1);
    } else {
        Element *parent = selected->parent();
        int startIndex = selected->indexOfSelfAsChild();
        int totalCount = parent->getChildItemsCount();
        int index = 0 ;
        for(int i = startIndex ; i < totalCount ; i ++) {
            Element *element = parent->getChildAt(i);
            if(element->isElement()) {
                if(applyReplicaToElement(regola, cmd, element, index + cmd->startNumber(), totalCount)) {
                    changed = true ;
                }
                index ++ ;
            }
        }
    }
    if(NULL != widget) {
        widget->setUpdatesEnabled(true);
    }
    return changed ;
}

bool ReplicaManager::applyReplicaToElement(Regola *regola, ReplicaCommand *cmd, Element *element, const int startIndex, const int numSiblings)
{
    bool changed = false;
    bool changedThis = false;
    int totalWidthPattern = totalWidth(numSiblings + startIndex, base(cmd));
    Attribute *attribute = element->getAttribute(cmd->attributeName());
    if((NULL == attribute) || ((NULL != attribute) && cmd->overwrite())) {
        QString oldValue ;
        if(NULL != attribute) {
            oldValue = attribute->value ;
        }
        QString newId = makeId(cmd, startIndex, totalWidthPattern);
        QString newValue ;
        if(oldValue.isEmpty()) {
            newValue = newId ;
        } else {
            if(cmd->atEnd()) {
                newValue = oldValue.append(cmd->separator()).append(newId);
            } else {
                newValue = newId.append(cmd->separator()).append(oldValue);
            }
        }
        element->setAttribute(cmd->attributeName(), newValue);
        changed = true;
        changedThis = true ;
    }
    if(cmd->recursive()) {
        int index = 0 ;
        int totalChildren = element->getChildItemsCount();
        foreach(Element * child, *element->getChildItems()) {
            if(child->isElement()) {
                if(applyReplicaToElement(regola, cmd, child, index, totalChildren)) {
                    changed = true ;
                }
                index++ ;
            }
        }
    }
    if(changedThis && (NULL != regola)) {
        regola->updateElement(element);
    }
    return changed ;
}

int ReplicaManager::totalWidth(int count, int base)
{
    int data = count ;
    int width = 0;
    do {
        width++;
        // integer division
        data /= base ;
    } while(data > 0) ;
    return width ;
}

int ReplicaManager::base(ReplicaCommand *cmd)
{
    switch(cmd->format()) {
    case ReplicaCommand::NumberPadded:
    case ReplicaCommand::NumberUnpadded:
    default:
        return 10;
        break;
    case ReplicaCommand::AlphaPadded:
    case ReplicaCommand::AlphaUnpadded:
        return 26;
        break;
    }
}


QString ReplicaManager::formatNumber(const int index, const bool isPadded, const int totalWidth)
{
    QString number = QString("%1").arg(index);
    if(isPadded) {
        QString result ;
        result.fill('0', totalWidth).append(number);
        return result.right(totalWidth);
    }
    return number;
}

QString ReplicaManager::formatAlpha(const int index, const bool isPadded, const int totalWidth)
{
    int valueNumber = index ;
    QString value = "";
    bool first = true;
    do {
        int digit = valueNumber % 26;
        QChar ch = 'A' + ((isPadded || first) ? 0 : -1) + digit;
        value = QString("%1%2").arg(ch).arg(value);
        valueNumber /= 26 ;
        first = false;
    } while(valueNumber > 0);
    if(isPadded) {
        QString result ;
        result.fill('A', totalWidth).append(value);
        return result.right(totalWidth);
    }
    return value;
}

QString ReplicaManager::makeId(ReplicaCommand *cmd, int index, int totalWidth)
{
    switch(cmd->format()) {
    case ReplicaCommand::NumberPadded:
        return formatNumber(index, true, totalWidth);
        break;
    default:
    case ReplicaCommand::NumberUnpadded:
        return formatNumber(index, false, totalWidth);
    case ReplicaCommand::AlphaPadded:
        return formatAlpha(index, true, totalWidth);
        break;
    case ReplicaCommand::AlphaUnpadded:
        return formatAlpha(index, false, totalWidth);
        break;
    }
}
