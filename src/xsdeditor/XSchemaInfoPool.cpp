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


#include "xsdeditor/xschema.h"
#include "utils.h"

//------------------------------------------------------------------------------------------------------------------

XSchemaInfoPool::XSchemaInfoPool(XSDSchema *theMainSchema)
{
    _mainSchema = theMainSchema;
}

XSchemaInfoPool::~XSchemaInfoPool()
{

}

QList<XSDSchema*> XSchemaInfoPool::includes()
{
    return _includesAndRedefines.toList();
}

void XSchemaInfoPool::addInclude(XSDSchema *newInclude)
{
    _includesAndRedefines.insert(newInclude);
}

void XSchemaInfoPool::addRedefine(XSDSchema *newInclude)
{
    _includesAndRedefines.insert(newInclude);
    // types already registered
    // it should check if the redefinitions are legal.
}

void XSchemaInfoPool::reset()
{
    _includesAndRedefines.clear();
}

void XSchemaInfoPool::resetLite()
{
    _includesAndRedefines.clear();
    _includesAndRedefines.insert(_mainSchema);
}
void XSchemaInfoPool::addRedefinedTypes(XSchemaRedefine * redefine)
{
    foreach(XSchemaObject * child, redefine->getChildren()) {
        switch(child->getType()) {
        case SchemaTypeElement:
        case SchemaTypeGroup:
        case SchemaTypeSimpleType:
        case SchemaTypeAttributeGroup:
            _redefinitions.append(child);
        default:
            break;
        }
    }
}

/**
 * @brief XSchemaInfoPool::getObject invoked from any schema of the pool
 * @param name
 * @param type
 * @return
 */
XSchemaObject* XSchemaInfoPool::getObject(const QString &name, const ESchemaType type)
{
    return _mainSchema->findTopObject(name, type);
}

XSchemaObject* XSchemaInfoPool::findObject(const QString &name, const ESchemaType type)
{
    if((SchemaGenericType == type) || (SchemaGenericElement ==  type)) {
        return findElementOrType(name, (SchemaGenericType == type) ? true : false);
    }
    foreach(XSchemaObject * child, _redefinitions) {
        if((type == child->getType()) && (name == child->name())) {
            return child;
        }
    }

    foreach(XSchemaObject * schema, _includesAndRedefines.values()) {
        foreach(XSchemaObject * child, schema->getChildren()) {
            if((type == child->getType()) && (name == child->name())) {
                return child;
            }
        }
    }
    return NULL ;
}

XSchemaElement* XSchemaInfoPool::findElementOrType(const QString &name, const bool isType)
{
    foreach(XSchemaObject * child, _redefinitions) {
        if((SchemaTypeElement == child->getType()) && (name == child->name())) {
            if(isType == static_cast<XSchemaElement*>(child)->isTypeOrElement()) {
                return static_cast<XSchemaElement*>(child);
            }
        }
    }

    foreach(XSchemaObject * schema, _includesAndRedefines.values()) {
        foreach(XSchemaObject * child, schema->getChildren()) {
            if((SchemaTypeElement == child->getType()) && (name == child->name())) {
                if(isType == static_cast<XSchemaElement*>(child)->isTypeOrElement()) {
                    return static_cast<XSchemaElement*>(child);
                }
            }
        }
    }
    return NULL ;
}



QString XSchemaInfoPool::targetNamespace() const
{
    return _mainSchema->targetNamespace();
}

/*void XSchemaInfoPool::setTargetNamespace(const QString &targetNamespace)
{
    _targetNamespace = targetNamespace;
}*/

XSDSchema *XSchemaInfoPool::mainSchema()
{
    return _mainSchema ;
}

QList<XSchemaObject*> XSchemaInfoPool::redefinitions()
{
    return _redefinitions ;
}
