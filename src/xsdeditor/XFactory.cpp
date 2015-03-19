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

#include "xsdeditor/xschema.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "utils.h"

// factory method
XSchemaObject* XSchemaObject::addNewChild(const ESchemaType newType)
{
    switch(newType) {
    default:
        return NULL;
    case SchemaTypeSchema:
        return new XSDSchema(NULL);
    case SchemaTypeElement: return new XSchemaElement(this, _root);
    case SchemaTypeAttribute: return new XSchemaAttribute(this, _root);
    case SchemaTypeChoice: return new XSchemaChoice(this, _root);
    case SchemaTypeSequence: return new XSchemaSequence(this, _root);
    case SchemaTypeSimpleContent: Utils::error(tr("pure virtual function called")); //("return new XSchemaSimpleContent(this, _root);"); return NULL;
    case SchemaTypeSimpleContentRestriction: return new XSchemaSimpleContentRestriction(this, _root);
    case SchemaTypeSimpleContentExtension: return new XSchemaSimpleContentExtension(this, _root);
    case SchemaTypeComplexContent: Utils::error(tr("pure virtual function called")); //("return new XSchemaComplexContent(this, _root);"); return NULL;
    case SchemaTypeComplexContentRestriction: return new XSchemaComplexContentRestriction(this, _root);
    case SchemaTypeComplexContentExtension: return new XSchemaComplexContentExtension(this, _root);
        //case SchemaTypeSimpleType:abstract
    case SchemaTypeSimpleTypeRestriction: return new XSchemaSimpleTypeRestriction(this, _root);
    case SchemaTypeSimpleTypeUnion: return new XSchemaSimpleTypeUnion(this, _root);
    case SchemaTypeSimpleTypeList: return new XSchemaSimpleTypeList(this, _root);

    case SchemaTypeAnnotation: return new XSchemaAnnotation(this, _root);
        //SchemaTypeBaseDocumentation:abstract
        //TODO case SchemaTypeDocumentation:return new XSchema( this, _root );
        //TODO case SchemaTypeAppInfo:return new XSchema( this, _root );
    case SchemaTypeAll: return new XSchemaAll(this, _root);
    case SchemaTypeAny: return new XSchemaAny(this, _root);
    case SchemaTypeAnyAttribute: return new XSchemaAnyAttribute(this, _root);
    case SchemaTypeAttributeGroup: return new XSchemaAttributeGroup(this, _root);
    case SchemaTypeField: return new XSchemaField(this, _root);
    case SchemaTypeGroup: return new XSchemaGroup(this, _root);
    case SchemaTypeImport: return new XSchemaImport(this, _root);
    case SchemaTypeInclude: return new XSchemaInclude(this, _root);
    case SchemaTypeKey: return new XSchemaKey(this, _root);
    case SchemaTypeKeyRef: return new XSchemaKeyRef(this, _root);
    case SchemaTypeNotation: return new XSchemaNotation(this, _root);
    case SchemaTypeRedefine: return new XSchemaRedefine(this, _root);
    case SchemaTypeSelector: return new XSchemaSelector(this, _root);
    case SchemaTypeUnique: return new XSchemaUnique(this, _root);
    }
}

