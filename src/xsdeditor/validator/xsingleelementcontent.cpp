/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
//#define LOG_CONSOLE
#include "xmlEdit.h"
#include "xsdeditor/validator/xsingleelementcontent.h"
#include "xsdeditor/xschema.h"
#include "utils.h"

int XSingleElementContent::instances = 0 ;

XSingleElementContent::XSingleElementContent(XSchemaObject *newItem)
{
    _id = ++ XSingleElementContent::instances ;
    _item = newItem ;
    calculateOccurrences(_item);
    _occurrences = 0 ;
}

XSingleElementContent::~XSingleElementContent()
{
    reset();
}

int XSingleElementContent::id()
{
    return _id ;
}


void XSingleElementContent::reset()
{
    foreach(XSingleElementContent * child, _children) {
        delete child ;
    }
    _children.clear();
    foreach(XSingleElementContent * child, _attributes) {
        delete child ;
    }
    _attributes.clear();
}

XSchemaObject *XSingleElementContent::item()
{
    return _item;
}

XSingleElementContent *XSingleElementContent::addAChild(XSchemaObject *object)
{
    XSingleElementContent *newChild = new XSingleElementContent(object);
    if(NULL != newChild) {
        _children.append(newChild);
    }
    return newChild ;
}

void XSingleElementContent::calculateOccurrences(XSchemaObject *object)
{
    Utils::TODO_NEXT_RELEASE("COMPLETARE");
    _minOccurs = object->minOccurrences() ;
    _maxOccurs = object->maxOccurrences() ;
}

bool XSingleElementContent::isOptional()
{
    if(0 == _minOccurs) {
        return true ;
    }
    return false ;
}

bool XSingleElementContent::isCollectable()
{
    switch(_item->getType()) {
    default:
        return true ;
    case SchemaTypeChoice:
    case SchemaTypeAll:
    case SchemaTypeGroup:
        return false;
    }
}

bool XSingleElementContent::isTerminal()
{
    bool result = _item->getType() == SchemaTypeElement ;
    return result ;
}

bool XSingleElementContent::isChoice()
{
    bool result = _item->getType() == SchemaTypeChoice ;
    return result ;
}

bool XSingleElementContent::isAll()
{
    bool result = _item->getType() == SchemaTypeAll ;
    return result ;
}

QList<XSingleElementContent*> &XSingleElementContent::children()
{
    return _children ;
}


//-------- Region: search

void XSingleElementContent::schemaSearchInit()
{
    Utils::TODO_NEXT_RELEASE(" _isExploring = true ;");
    // reset the total counter of itself and all its children recursively
    resetOccurrencesRecursive(true);
}

void XSingleElementContent::resetOccurrences()
{
    _occurrences = 0 ;
}

void XSingleElementContent::resetOccurrencesRecursive(const bool resetSelf)
{
//    _exploring = true ;
    if(resetSelf) {
        _occurrences = 0;
    }
    foreach(XSingleElementContent * child, _children) {
        child->resetOccurrences();
    }
}

//void XSingleElementContent::confirm()
//{
//    if( _isExploring ) {
//        _isExploring = false;
//        _occurrences ++ ;
//    }
//}


void XSingleElementContent::dump(const int indent)
{
    QString s;
    for(int i = 0 ; i < indent ; i ++) {
        s += ".";
    }
    s += item()->name();
    foreach(XSingleElementContent * child, _children) {
        child->dump(indent + 1);
    }
}


SchemaSearchContext::ESearchState XSingleElementContent::scanForPosition(SchemaSearchContext * context, const bool isInAll)
{
    TRACEQ(QString("[scanForPosition enter]"));
    if(isTerminal()) {
        TRACEQ(QString("[scanForPosition return check isTerminal ]"));
        return scanForElement(context, isInAll);
    }
    QSet<QString> allMap;
    if(isAll()) {
        foreach(XSingleElementContent * child, _children) {
            allMap.insert(child->item()->name());
        }
    }
    bool redo = false;
    resetOccurrencesRecursive(true);
    SchemaSearchContextPosition pos;
    do {
        redo = false;
        resetOccurrencesRecursive(false);
        bool found = false;

        foreach(XSingleElementContent * child, _children) {

            TRACEQ(QString("..scanForPosition child %1").arg(child->item()->name()));

            if(isAll()) {
                if(!allMap.contains(child->item()->name())) {
                    continue ;
                }
            }
            if(isChoice()) {
                TRACEQ(QString("..is a choice"));
                context->savePosition(pos);
            }
            SchemaSearchContext::ESearchState result = child->scanForPosition(context, isAll());
            switch(result) {
            default:
                Utils::TODO_NEXT_RELEASE("errore");
                return SchemaSearchContext::ERRORSTOP;
            //------------------------------
            case SchemaSearchContext::FOUND:
                TRACEQ(QString("..scanForPosition returns FOUND"));
                return  SchemaSearchContext::FOUND;
            //------------------------------
            case SchemaSearchContext::NOTFOUND:
                TRACEQ(QString("..scanForPosition NOT FOUND"));
                if(!(isChoice() || isAll())) {
                    TRACEQ(QString("..scanForPosition returns NOT FOUND, no choice, all"));
                    return SchemaSearchContext::NOTFOUND;
                }
                break;
            case SchemaSearchContext::SKIP:
                // this is a no-op
                TRACEQ(QString("..scanForPosition SKIP"));
                break;
            //------------------------------
            case SchemaSearchContext::ERRORSTOP:
                return SchemaSearchContext::ERRORSTOP;
            //------------------------------
            case SchemaSearchContext::OK:
                TRACEQ(QString("..scanForPosition OK"));
                handleOk(context);
                found = true ;
                break;
            case SchemaSearchContext::NOK:
                TRACEQ(QString("..scanForPosition NOK"));
                if(SchemaSearchContext::ERRORSTOP == handleNOk(context)) {
                    TRACEQ(QString("..scanForPosition ERROR following a NOK"));
                    return SchemaSearchContext::ERRORSTOP;
                }
                break;
            }
            if(isChoice()) {
                if(found) {
                    TRACEQ(QString("..scanForPosition break being a choice"));
                    break;
                }
                TRACEQ(QString("..scanForPosition restore position"));
                context->restorePosition(pos);
            }
            if(isAll()) {
                allMap.remove(child->item()->name());
            }
        } // foreach child

        if(found) {
            TRACEQ(QString("..scanForPosition FOUND"));

            // if it is an all, all the children must be scanned using permutations.
            // Only when the map is empty, the all is resolved. At each revolution, a found must be raised.
            if(isAll()) {
                if(!allMap.isEmpty()) {
                    TRACEQ(QString("..Is an ALL with remaining items, redo"));
                    redo = true ;
                }
            } else {
                _occurrences ++ ;
                if(_occurrences < _maxOccurs) {

                    TRACEQ(QString("..check for EOF"));

                    if(context->isEof()) {
                        TRACEQ(QString("..scanForPosition NOT FOUND"));
                        return SchemaSearchContext::NOTFOUND;
                    }
                    TRACEQ(QString("..scanForPosition redo"));
                    redo = true ;
                    // until OK
                } else {
                    // NB: can be only equal, not major
                    TRACEQ(QString("..scanForPosition FOUND"));
                    return SchemaSearchContext::OK;
                }
            }// else of isAll()
        } else { // no match
            TRACEQ(QString("..scanForPosition call evalTerminalCondition"));
            return evalTerminalCondition();
        } // end of evaluation
    } while(redo);
    // it is impossible to be here
    TRACEQ(QString("..scanForPosition returns ERROR AT END"));
    return SchemaSearchContext::ERRORSTOP ;
}

SchemaSearchContext::ESearchState XSingleElementContent::handleOk(SchemaSearchContext * /*context*/)
{
    return SchemaSearchContext::OK;
}

SchemaSearchContext::ESearchState XSingleElementContent::handleNOk(SchemaSearchContext * /*context*/)
{
    /*if(child.isOptional) {
        // does not advance source
        return SchemaSearchContext::OK;
    }*/
    return SchemaSearchContext::NOK;
}

// this is a element specialized test
SchemaSearchContext::ESearchState XSingleElementContent::evaluateMatch(SchemaSearchContext * context, const bool isInAll)
{
    TRACEQ(QString("..evaluateMatch enter, evaluating '%1' '%2'").arg(item()->name()).arg(context->targetTag()));
    if(item()->name() == context->targetTag()) {
        TRACEQ(QString("..evaluateMatch tag match"));

        if(context->sourceIsTarget() || isInAll) {

            TRACEQ(QString("..evaluateMatch source is target, FOUND"));
            context->setFoundItem(this);
            return SchemaSearchContext::FOUND ;
        }
        TRACEQ(QString("..evaluateMatch returns OK"));
        return SchemaSearchContext::OK ;
    } else if(context->sourceTag() == item()->name()) {
        TRACEQ(QString("..evaluateMatch returns OK"));
        return SchemaSearchContext::OK ;
    }
    TRACEQ(QString("..evaluateMatch returns NOK"));
    return SchemaSearchContext::NOK ;
}

/**
this function calculates the match of a given element with the source
returns : ERROR as an error, NOK as "not found."
if OK, a match is given (_occurrence reports the multeplicity), and the parent can mark itself counted too.
Notes: if a choice branch is explored and fails, it should return NOK (NOT error, given that there is a global evaluation).
*/
SchemaSearchContext::ESearchState XSingleElementContent::scanForElement(SchemaSearchContext * context, const bool isAll)
{
    TRACEQ(QString("..scanForElement enter"));
    _occurrences = 0 ;
    bool redoTest = false ;
    do {
        SchemaSearchContext::ESearchState state = evaluateMatch(context, isAll);
        if(SchemaSearchContext::FOUND == state) {
            TRACEQ(QString("..scanForElement return FOUND"));
            return SchemaSearchContext::FOUND ;
        }
        if(state == SchemaSearchContext::OK) {
            TRACEQ(QString("..scanForElement state is OK"));
            _occurrences ++ ;
            if(isAll) {
                TRACEQ(QString("..scanForElement return OK for ALL"));
                return SchemaSearchContext::OK;
            }
            if(_occurrences <= _maxOccurs) {
                TRACEQ(QString("..scanForElement advancing, (occurrences [%1] < _maxOccurs [%2] )").arg(_occurrences).arg(_maxOccurs));
                context->advanceSource();
                if(context->isEof()) {
                    TRACEQ(QString("..scanForElement is EOF return NOTFOUND"));
                    return SchemaSearchContext::NOTFOUND;
                }
                TRACEQ(QString("..scanForElement redo test"));
                redoTest = true ;
// until OK
            } else {
// NB: can be only equal, not major
                TRACEQ(QString("..scanForElement return OK"));
                return SchemaSearchContext::OK;
            }
        } else { // no match
            TRACEQ(QString("..scanForElement return evalTerminalCondition"));
            return evalTerminalCondition();
        } // end of evaluation

    } while(!redoTest);
// it is impossible to be here
    TRACEQ(QString("..scanForElement error at end"));
    return SchemaSearchContext::ERRORSTOP ;
}

/**
evaluate the return state from a match in a general sense
*/
SchemaSearchContext::ESearchState XSingleElementContent::evalTerminalCondition()
{
    TRACEQ(QString("..evalTerminalCondition enter"));
    if(0 == _occurrences) {
        TRACEQ(QString("..evalTerminalCondition 0 occurences"));
        if(isOptional()) {
// this is a skip flag, use this because else the parent can believe that a match is really done.
            TRACEQ(QString("..evalTerminalCondition return skip being optional"));
            return SchemaSearchContext::SKIP ;
        } else {
            TRACEQ(QString("..evalTerminalCondition returns NOK"));
            return SchemaSearchContext::NOK ;
        }
    } else { // occurrences > 0
        if(_occurrences < _minOccurs) {
            TRACEQ(QString("..evalTerminalCondition NOK (occurrences [%1] < _minOccurs [%2] )").arg(_occurrences).arg(_minOccurs));
            return SchemaSearchContext::NOK ;
        }
        TRACEQ(QString("..evalTerminalCondition returns OK"));
        return SchemaSearchContext::OK ;
    }
}


