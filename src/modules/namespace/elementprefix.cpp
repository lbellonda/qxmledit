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

#include "regola.h"
#include "xmlutils.h"
#include "utils.h"

bool Element::replacePrefix(const QString &oldPrefix, const QString &newPrefix, TargetSelection::Type targetSelection,
                            const bool isAllPrefixes, ElementUndoObserver *observer)
{
    if(!isElement()) {
        // nothing to do
        return true ;
    }
    bool ok = true;
    ElementUndoInfo info;
    bool changed = false ;
    QString name, prefix;
    XmlUtils::decodeQualifiedName(tag(), prefix, name);
    if((isAllPrefixes && !prefix.isEmpty())
            || (!isAllPrefixes && (oldPrefix == prefix))) {
        QString newTag = XmlUtils::makeQualifiedName(newPrefix, name);
        info.tag = tag();
        setTagAuto(newTag);
        changed = true ;
    }
    bool attrChanged = false ;
    int attrIndex = 0 ;
    serializeAttributesToInfo(info);
    foreach(Attribute * attr, getAttributesList()) {
        XmlUtils::decodeQualifiedName(attr->name, prefix, name);
        if((isAllPrefixes && !prefix.isEmpty())
                || (!isAllPrefixes && (oldPrefix == prefix))) {
            if(!XmlUtils::isDeclaringNS(attr->name)) {
                QString newName = XmlUtils::makeQualifiedName(newPrefix, name);
                attr->name = attributeNameAuto(newName);
                changed = true ;
                attrChanged = true ;
            }
        }
        attrIndex ++ ;
    }
    if(!attrChanged) {
        info.undoReplaceAttributes();
    }
    if(changed) {
        info.path  = indexPath();
        if(!observer->observe(&info)) {
            ok = false;
        }
    }

    bool recursive = TargetSelection::isRecursive(targetSelection);
    if(recursive) {
        foreach(Element * child, getChildItemsRef()) {
            if(child->isElement()) {
                if(!child->replacePrefix(oldPrefix, newPrefix, targetSelection, isAllPrefixes, observer)) {
                    ok = false;
                }
            }
        }
    }
    return ok;
}


bool Element::setPrefix(const QString &newPrefix, TargetSelection::Type targetSelection,
                        ElementUndoObserver *observer)
{
    if(!isElement()) {
        // nothing to do
        return true ;
    }
    bool ok = true;
    ElementUndoInfo info;
    QString name, prefix;
    XmlUtils::decodeQualifiedName(tag(), prefix, name);
    QString newTag = XmlUtils::makeQualifiedName(newPrefix, name);
    if(newTag != info.tag) {
        info.tag = tag();
        setTagAuto(newTag);
        info.path  = indexPath();
        if(!observer->observe(&info)) {
            ok = false;
        }
    }
    bool recursive = TargetSelection::isRecursive(targetSelection);
    if(recursive) {
        foreach(Element * child, getChildItemsRef()) {
            if(child->isElement()) {
                if(!child->setPrefix(newPrefix, targetSelection, observer)) {
                    ok = false;
                }
            }
        }
    }
    return ok;
}

bool Element::removePrefix(const QString &removedPrefix, TargetSelection::Type targetSelection,
                           const bool isAllPrefixes, ElementUndoObserver *observer)
{
    if(!isElement()) {
        // nothing to do
        return true ;
    }
    bool ok = true;
    ElementUndoInfo info;
    bool changed = false ;
    QString name, prefix;
    XmlUtils::decodeQualifiedName(tag(), prefix, name);
    if((isAllPrefixes && !prefix.isEmpty())
            || (!isAllPrefixes && (removedPrefix == prefix))) {
        info.tag = tag();
        setTag(name);
        changed = true ;
    }
    bool attrChanged = false ;
    int attrIndex = 0 ;
    serializeAttributesToInfo(info);
    foreach(Attribute * attr, getAttributesList()) {
        XmlUtils::decodeQualifiedName(attr->name, prefix, name);
        if((isAllPrefixes && !prefix.isEmpty())
                || (!isAllPrefixes && (removedPrefix == prefix))) {
            if(!XmlUtils::isDeclaringNS(attr->name)) {
                attr->name = name;
                changed = true ;
                attrChanged = true ;
            }
        }
        attrIndex ++ ;
    }
    if(!attrChanged) {
        info.undoReplaceAttributes();
    }
    if(changed) {
        info.path  = indexPath();
        if(!observer->observe(&info)) {
            ok = false;
        }
    }
    bool recursive = TargetSelection::isRecursive(targetSelection);
    if(recursive) {
        foreach(Element * child, getChildItemsRef()) {
            if(!child->removePrefix(removedPrefix, targetSelection, isAllPrefixes, observer)) {
                ok = false;
            }
        }
    }
    return ok;
}
