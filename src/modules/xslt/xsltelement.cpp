/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "xsltelement.h"
#include "regola.h"
#include "modules/xslt/xsltmode.h"
#include "utils.h"

XsltElement::XsltElement(QString newTag)
{
    isClearAttributes = false ;
    _tag = newTag;
    _insertMode = XsltInsertable::InsAnyWhere ;
    cardinality = 0 ;
    use = false;
    position = XsltPosition::PositionAll;
    isOpenContext = false ;
    completionType = XsltCompletionType::All;
}


XsltElement::~XsltElement()
{
}


void XsltElement::setInsertMode(XsltInsertable::EInsertMode newMode)
{
    _insertMode = newMode ;
}


QString XsltElement::tagName()
{
    return _tag;
}

bool XsltElement::canInsertChild(XsltElement *newChild)
{
    if(childrenClasses.contains(newChild->thisClass)) {
        return true;
    }
    return false;
}

bool XsltElement::isInsertAtTop()
{
    if(XsltInsertable::InsTopLevel == _insertMode) {
        return true ;
    }
    return false;
}

bool XsltElement::isInsertAnywhere()
{
    if(XsltInsertable::InsAnyWhere == _insertMode) {
        return true ;
    }
    return false;
}

XsltElement* XsltElement::findXslParent(XslLevel *level, QMap<QString, XsltElement*> &elementsByTag)
{
    // find the
    XsltElement *el = elementsByTag[level->simpleTag];
    return el;
}

bool XsltElement::isInsertableAsChild(XslContext *context, QMap<QString, XsltElement*> &elementsByTag)
{
    bool isInsertableFlag = false ;

    if(!context->levels.isEmpty()) {
        if(context->levels.first()->isXsl) {
            XsltElement *elp = findXslParent(context->levels.first(), elementsByTag);
            if((NULL != elp) && elp->canInsertChild(this)) {
                isInsertableFlag = true;
            }
        } else {
            if(isInsertAnywhere()) {
                isInsertableFlag = true ;
            }
        }
    }
    return isInsertableFlag ;
} // isInsertable


/**
 *
if top level, no append
if child of xsl iif legal
if not child of xsl iif is anywhere, but the parent must be legal, e.g. call-template
*/

bool XsltElement::isAppendable(XslContext *context, QMap<QString, XsltElement*> &elementsByTag)
{
    if(context->levels.isEmpty()) {
        return false;
    }
    if(isInsertAtTop()) {
        return false;
    }
    bool isAppendableFlag = false ;

    bool isFirst = true ;
    foreach(XslLevel * level, context->levels) {
        if(isFirst) {
            if((thisClass == "p")) {
                if(level->isXsl) {
                    // if it is not the same, do not append
                    if(level->simpleTag != tagName()) {
                        return false;
                    }
                }
            }
            isFirst = false ;
            continue;
        }
        if(level->isXsl) {
            XsltElement *elp = findXslParent(level, elementsByTag);
            if((NULL != elp) && elp->canInsertChild(this)) {
                isAppendableFlag = true ;
            }
            break; // found the xsl element that can decide
        }
    }
    return isAppendableFlag ;
}
