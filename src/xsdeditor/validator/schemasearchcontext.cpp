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
//#define LOG_CONSOLE
#include "schemasearchcontext.h"
#include "utils.h"
#include "element.h"

//------------------------------------------------------------------------
SchemaSearchContextPosition::SchemaSearchContextPosition()
{
    _source = NULL ;
    _pos = -1 ;
}

SchemaSearchContextPosition::~SchemaSearchContextPosition()
{
    //
}

Element* SchemaSearchContextPosition::source()
{
    return _source ;
}


void SchemaSearchContextPosition::setSource(Element* value)
{
    _source = value ;
}

int SchemaSearchContextPosition::pos()
{
    return _pos ;
}


void SchemaSearchContextPosition::setPos(const int value)
{
    _pos = value ;
}

//------------------------------------------------------------------------



SchemaSearchContext::SchemaSearchContext()
{
    Utils::TODO_NEXT_RELEASE("INIT VARIABLES");
    _currentIndex = 0 ;
    _currentSource = NULL ;
    _parentElement = NULL;
    _target = NULL ;
}

SchemaSearchContext::~SchemaSearchContext()
{
}

bool SchemaSearchContext::initScan()
{
    // Predecrement, to have the search begin from start.
    _currentIndex = -1 ;
    return advanceSource();
}

XSingleElementContent* SchemaSearchContext::foundItem()
{
    return _foundItem ;
}


void SchemaSearchContext::setFoundItem(XSingleElementContent* value)
{
    _foundItem = value ;
}


bool SchemaSearchContext::advanceSource()
{
    TRACEQ(QString(".....Advance source enter"));
    if(NULL != _parentElement) {
        //skip comment, text, ...
        while(!isEof()) {

            TRACEQ(QString(".....Advance source next element"));
            _currentIndex ++;
            Element *candidate = _parentElement->getChildAt(_currentIndex);
            if(NULL != candidate) {
                if(candidate->getType() == Element::ET_ELEMENT) {
                    TRACEQ(QString(".....Advance source is Element:%1, index:%2").arg(candidate->tag()).arg(_currentIndex));
                    _currentSource = candidate ;
                    return true;
                }
                TRACEQ(QString(".....Advance source index:%1 is not an element").arg(_currentIndex));
            }
        } // end while
        TRACEQ(QString(".....Advance source no elements found"));
    } else {
        //set eof condition
        TRACEQ(QString(".....No parent element, setting EOF"));
        _currentIndex = _targetIndex ;
    }
    return false;
}

bool SchemaSearchContext::isEof()
{
    bool isEof = (_currentIndex >= _targetIndex);
    return isEof;
}

void SchemaSearchContext::setTarget(Element *parent, Element *source, const int sourceIndex)
{
    _parentElement = parent ;
    _target = source ;
    _targetIndex = sourceIndex ;
}

QString SchemaSearchContext::targetTag()
{
    return _target->tag();
}

QString SchemaSearchContext::sourceTag()
{
    return _currentSource->tag();
}


void SchemaSearchContext::savePosition(SchemaSearchContextPosition& position)
{
    position.setSource(_currentSource);
    position.setPos(_currentIndex);
}

void SchemaSearchContext::restorePosition(SchemaSearchContextPosition& position)
{
    _currentSource = position.source();
    _currentIndex = position.pos();
}

bool SchemaSearchContext::sourceIsTarget()
{
    if((_currentSource == _target) && (_targetIndex == _currentIndex)) {
        return true ;
    }
    return false;
}
