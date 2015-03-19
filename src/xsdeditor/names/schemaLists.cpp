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
#include "xsdeditor/xsddefinitions.h"
#include "utils.h"

/***
 * Objects handling
 */
//------------ region(topLevelList) ---------------------------------------------------------

void XSDSchema::childListByName(const ESchemaType expected, QMap<QString, XSchemaObject *>&set, QList<XSchemaObject*> list)
{
    foreach(XSchemaObject * child, list) {
        bool test = false;
        if(expected == SchemaGenericElement) {
            test = (child->getType() == SchemaTypeElement) && !static_cast<XSchemaElement*>(child)->isTypeOrElement();
        } else if(expected == SchemaGenericType) {
            test = (child->getType() == SchemaTypeElement) && static_cast<XSchemaElement*>(child)->isTypeOrElement() ;
        } else {
            test = (child->getType() == expected);
        }
        if(test) {
            if(!set.contains(child->name())) {
                set.insert(child->name(), child);
            }
        }
    }
}

/**
 * @brief XSDSchema::topLevelListByName
 * @param expected
 * @param onlyThisSchemaOrAll
 * @return a unique list of objects by name giving predecence to redefinitions
 */
QList<XSchemaObject*> XSDSchema::topLevelListByName(const ESchemaType expected, const bool onlyThisSchemaOrAll)
{
    // redefinitions
    QMap<QString, XSchemaObject *>set;
    childListByName(expected, set, infoPool()->redefinitions());
    childListByName(expected, set, _children);
    if(!onlyThisSchemaOrAll) {
        foreach(XSDSchema * schema, infoPool()->includes()) {
            childListByName(expected, set, schema->_children);
        }
    }
    return set.values();
}

QList<XSchemaObject*> XSDSchema::getChildrenByType(const ESchemaType expected, const bool onlyThisSchemaOrAll)
{
    QList<XSchemaObject*> result;
    result.append(getChildrenByType(expected));
    if(!onlyThisSchemaOrAll) {
        foreach(XSDSchema * schema, infoPool()->includes()) {
            if(schema != this) {
                result.append(schema->getChildrenByType(expected));
            }
        }
    }
    return result;
}

QList<XSchemaObject*> XSDSchema::scanForAllInnerElements(const bool onlyThisSchemaOrAll)
{
    QList<XSchemaObject*> result;
    foreach(XSchemaObject * child, _children) {
        scanForInnerElements(child, result);
    }
    if(!onlyThisSchemaOrAll) {
        foreach(XSDSchema * schema, infoPool()->includes()) {
            foreach(XSchemaObject * child, schema->_children) {
                if(schema != this) {
                    scanForInnerElements(child, result);
                }
            }
        }
    }
    return result;
}


// no redefinitions
QList<XSchemaObject*> XSDSchema::topLevelElements(const bool onlyThisSchemaOrAll)
{
    return getChildrenByType(SchemaGenericElement, onlyThisSchemaOrAll);
}

// redefinitions possible
QList<XSchemaObject*> XSDSchema::topLevelTypes(const bool onlyThisSchemaOrAll)
{
    return topLevelListByName(SchemaGenericType, onlyThisSchemaOrAll);
}

// no redefinitions
QList<XSchemaObject*> XSDSchema::topLevelAttributes(const bool onlyThisSchemaOrAll)
{
    return getChildrenByType(SchemaTypeAttribute, onlyThisSchemaOrAll);
}

// redefinitions possible
QList<XSchemaObject*> XSDSchema::topLevelGroups(const bool onlyThisSchemaOrAll)
{
    return topLevelListByName(SchemaTypeGroup, onlyThisSchemaOrAll);
}

// redefinitions possible
QList<XSchemaObject*> XSDSchema::topLevelAttributeGroups(const bool onlyThisSchemaOrAll)
{
    return topLevelListByName(SchemaTypeAttributeGroup, onlyThisSchemaOrAll);
}

// no redefinitions, but from any schema
QList<XSchemaObject*> XSDSchema::allInnerElements(const bool onlyThisSchemaOrAll)
{
    return scanForAllInnerElements(onlyThisSchemaOrAll);
}

// no redefinitions
QList<XSchemaObject*> XSDSchema::topLevelReferences()
{
    QList<XSchemaObject*> result;
    result.append(getChildrenByType(SchemaTypeInclude));
    result.append(getChildrenByType(SchemaTypeRedefine));
    result.append(getChildrenByType(SchemaTypeImport));
    return result ;
}


//------------ endregion(topLevelList) ---------------------------------------------------------



