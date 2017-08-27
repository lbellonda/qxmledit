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

#include "xsdeditor/xsddefinitions.h"
#include "xsdeditor/xschema.h"
#include "utils.h"
#include "xmlutils.h"

/*
 * namespace resolution rules
 * element                 | schema has |
 * has ns                  | target ns  | resolution
 * ------------------------+------------+---------------------
 *                         |   yes      | use main schema target ns if equal
 *  yes                    +------------+---------------------
 *                         |   no       | use other ns
 * ------------------------+------------+---------------------
 *       resolve           |   yes      | main schema has targetNamespace? defaultns? yes: use schema data
 *  no   schema            |            |                       no: use other ns
 *       default           +------------+---------------------
 *       namespacedefault  |   no       | use the schema
 * ------------------------+------------+---------------------
 *
 * element
 * prefix+name -> ns+name (if no prefix and default ns, use defaultns)
 *
 */

//-------------------------------------------------------------------------------------

XSchemaObject* XSDSchema::findTopObject(const QString &name, const ESchemaType type)
{
    QString objectNamespace;
    QString prefix;
    QString localName = name;
    int colonIndex = name.indexOf(":");
    if(colonIndex >= 0) {
        localName = name.mid(colonIndex + 1);
        prefix = name.left(colonIndex);
    }

    //--- default namespace handling
    if(prefix.isEmpty()) {
        // own schema default namespace, if any
        objectNamespace = defaultNamespace();
    } else {
        // find the real namespace from prefix
        objectNamespace = infoPool()->mainSchema()->_namespacesByPrefix[prefix];
        if(objectNamespace.isEmpty()) {
            return NULL ;
        }
    }
    /**
      * note: no namespace equals target namespace empty
      */
    if(objectNamespace == infoPool()->targetNamespace()) {
        return infoPool()->findObject(localName, type);
    } else if(objectNamespace == _xsdURI) {
        // look for a predefined type
        if(SchemaGenericType == type) {
            return baseType(localName);
        }
        return NULL ;
    } else {
        // import are local references from each schema
        return findReferencedObjectWithNamespace(objectNamespace, localName, type);
    }
}

XSchemaObject* XSDSchema::findReferencedObjectWithNamespace(const QString &findNamespace, const QString &name, const ESchemaType type)
{
    QList<XSDSchema*> schemas = schemasByNamespace(findNamespace);
    foreach(XSDSchema * schema, schemas) {
        XSchemaObject* result = schema->infoPool()->findObject(name, type);
        if(NULL != result) {
            return result ;
        }
    }
    return NULL;
}

//-------------------------------------------------------

XSchemaElement *XSDSchema::topLevelType(const QString &referenceName)
{
    return (XSchemaElement*) infoPool()->getObject(referenceName, SchemaGenericType);
}

XSchemaElement *XSDSchema::topLevelElement(const QString &referenceName)
{
    Utils::TODO_THIS_RELEASE("se prefisso, trova prefisso ->ns, poi controlla se uguale a targetns, se si, serca non qualificato o qualificato con ns originale");
    XSchemaElement* element = (XSchemaElement*) infoPool()->getObject(referenceName, SchemaGenericElement);
    if(NULL == element) {
        QString prefix, name;
        XmlUtils::decodeQualifiedName(referenceName, prefix, name);
        Utils::TODO_THIS_RELEASE("check se appropriato");
        // look for target ns and prefixes for target ns, if appropriate, look for element name
        const QString &ns = _namespacesByPrefix[prefix] ;
        if(ns == _targetNamespace) {
            element = (XSchemaElement*) infoPool()->getObject(name, SchemaGenericElement);
        }
        if(NULL == element) {
            // look in imported under namespace
            Utils::TODO_THIS_RELEASE("seguire le specifiche");
            /*if( _importedSchemasByNamespace.contains(ns) ) {
                foreach( XSDSchema* importedSchema, _root->_importedSchemasByNamespace[ns] ) {
                    foreach( XSchemaObject* aChild, importedSchema->topLevelElements(true)) {
                        if( aChild.name() == name ) {
                            return static_cast<XSchemaElement*>(aChild);
                        }
                    }
                }
            }*/
        }

    }
    return element;
}

XSchemaAttribute *XSDSchema::topLevelAttribute(const QString &referenceName)
{
    return (XSchemaAttribute*) infoPool()->getObject(referenceName, SchemaTypeAttribute);
}

XSchemaAttributeGroup *XSDSchema::topLevelAttributeGroup(const QString &referenceName)
{
    return (XSchemaAttributeGroup*) infoPool()->getObject(referenceName, SchemaTypeAttributeGroup);
}

XSchemaGroup *XSDSchema::topLevelGroup(const QString &referenceName)
{
    return (XSchemaGroup*) infoPool()->getObject(referenceName, SchemaTypeGroup);
}

XSchemaElement *XSDSchema::topLevelType(XSchemaFindReferenceContext &context, const QString &referenceName)
{
    XSchemaElement* theType  = topLevelType(referenceName);
    if(NULL != theType) {
        context.setIsPredefinedType(theType->isPredefined());
        context.setResolved(theType);
    }
    return context.resolved();
}

XSchemaElement *XSDSchema::topLevelElement(XSchemaFindReferenceContext &context, const QString &referenceName)
{
    context.setIsPredefinedType(false);
    context.setResolved(topLevelElement(referenceName));
    return context.resolved();
}

