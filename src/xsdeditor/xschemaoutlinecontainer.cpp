/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017-2018 by Luca Bellonda and individual contributors  *
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

#include "xschemaoutlineelement.h"
#include "xsdeditor/xsddefinitions.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "utils.h"

XSchemaOutlineContainer::XSchemaOutlineContainer(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaObject(newParent, newRoot)
{
    _target = NULL ;
}

XSchemaOutlineContainer::~XSchemaOutlineContainer()
{
}

void XSchemaOutlineContainer::setSchemaObject(XSchemaObject *target)
{
    _target = target ;
}

XSchemaObject *XSchemaOutlineContainer::schemaObject()
{
    return _target ;
}


QString XSchemaOutlineContainer::tagName()
{
    return "XSchemaOutlineContainer::tagName";
}

bool XSchemaOutlineContainer::generateDom(QDomDocument &, QDomNode &)
{
    return false;
}

XSDCompareObject::EXSDCompareObject XSchemaOutlineContainer::innerCompareTo(XSchemaObject *, XSDCompareOptions &)
{
    return  XSDCompareObject::XSDOBJECT_UNCHANGED;
}

//------

XSchemaOutlineSequence::XSchemaOutlineSequence(XSchemaObject *newParent, XSchemaRoot *newRoot): XSchemaOutlineContainer(newParent, newRoot)
{
    _sequence = NULL ;
}

XSchemaOutlineSequence::~XSchemaOutlineSequence()
{
}

void XSchemaOutlineSequence::setSchemaObject(XSchemaObject *target)
{
    XSchemaOutlineContainer::setSchemaObject(target);
    _sequence = NULL ;
    if(NULL != target) {
        if(target->getType() == SchemaTypeSequence) {
            _sequence = static_cast<XSchemaSequence *>(target);
        } else {
            Utils::error(tr("Expected 'sequence' in outline mode, found type:%1").arg(target->getType()));
        }
    }
}

QString XSchemaOutlineSequence::description()
{
    if(NULL != _sequence) {
        return occurrencesDescrString(_sequence->minOccurs(), _sequence->maxOccurs());
    } else {
        return QString("Sequence") ;
    }
}

//------

XSchemaOutlineGroup::XSchemaOutlineGroup(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaOutlineContainer(newParent, newRoot)
{
    _isAlreadyProcessed = false ;
    _group = NULL ;
}

XSchemaOutlineGroup::~XSchemaOutlineGroup()
{
}

void XSchemaOutlineGroup::setSchemaObject(XSchemaObject *target)
{
    XSchemaOutlineContainer::setSchemaObject(target);
    _group = NULL ;
    if(NULL != target) {
        if(target->getType() == SchemaTypeGroup) {
            _group = static_cast<XSchemaGroup *>(target);
        } else {
            Utils::error(tr("Expected 'group' in outline mode, found type:%1").arg(target->getType()));
        }
    }
}

QString XSchemaOutlineGroup::description()
{
    if(NULL != _group) {
        return occurrencesDescrString(_group->minOccurs(), _group->maxOccurs());
    } else {
        return QString("Group") ;
    }
}

//----------------------------------------------------------------------------

XSchemaOutlineChoice::XSchemaOutlineChoice(XSchemaObject *newParent, XSchemaRoot *newRoot): XSchemaOutlineContainer(newParent, newRoot)
{
    _choice = NULL ;
}

XSchemaOutlineChoice::~XSchemaOutlineChoice()
{
}

void XSchemaOutlineChoice::setSchemaObject(XSchemaObject *target)
{
    XSchemaOutlineContainer::setSchemaObject(target);
    _choice = NULL ;
    if(NULL != target) {
        if(target->getType() == SchemaTypeChoice) {
            _choice = static_cast<XSchemaChoice *>(target);
        } else {
            Utils::error(tr("Expected 'choice' in outline mode, found type:%1").arg(target->getType()));
        }
    }
}

QString XSchemaOutlineChoice::description()
{
    if(NULL != _choice) {
        return occurrencesDescrString(_choice->minOccurs(), _choice->maxOccurs());
    } else {
        return QString("Choice") ;
    }
}

//----------------------------------------------------------------------------

XSchemaOutlineAll::XSchemaOutlineAll(XSchemaObject *newParent, XSchemaRoot *newRoot): XSchemaOutlineContainer(newParent, newRoot)
{
    _all = NULL ;
}

XSchemaOutlineAll::~XSchemaOutlineAll()
{
}

void XSchemaOutlineAll::setSchemaObject(XSchemaObject *target)
{
    XSchemaOutlineContainer::setSchemaObject(target);
    _all = NULL ;
    if(NULL != target) {
        if(target->getType() == SchemaTypeAll) {
            _all = static_cast<XSchemaAll *>(target);
        } else {
            Utils::error(tr("Expected 'all' in outline mode, found type:%1").arg(target->getType()));
        }
    }
}

QString XSchemaOutlineAll::description()
{
    if(NULL != _all) {
        return occurrencesDescrString(_all->minOccurs(), _all->maxOccurs());
    } else {
        return QString("All") ;
    }
}
//----

XSchemaOutlineAny::XSchemaOutlineAny(XSchemaObject *newParent, XSchemaRoot *newRoot): XSchemaOutlineContainer(newParent, newRoot)
{
    _any = NULL ;
}

XSchemaOutlineAny::~XSchemaOutlineAny()
{
}

void XSchemaOutlineAny::setSchemaObject(XSchemaObject *target)
{
    XSchemaOutlineContainer::setSchemaObject(target);
    _any = NULL ;
    if(NULL != target) {
        if(target->getType() == SchemaTypeAny) {
            _any = static_cast<XSchemaAny *>(target);
        } else {
            Utils::error(tr("Expected 'any' in outline mode, found type:%1").arg(target->getType()));
        }
    }
}

QString XSchemaOutlineAny::description()
{
    if(NULL != _any) {
        return occurrencesDescrString(_any->minOccurs(), _any->maxOccurs());
    } else {
        return QString("Any") ;
    }
}
