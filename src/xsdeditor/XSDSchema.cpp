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

#include "xsdeditor/xschema.h"
#include <QClipboard>
#include <QApplication>
#include <QFile>
#include <QHash>
#include "utils.h"
#include "xmlutils.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "xsdeditor/io/xschemaloader.h"

//------------------------------------------------------------------------------------------------------------------

QString XSDSchema::_xsdURI("http://www.w3.org/2001/XMLSchema") ;

XSDSchema::XSDSchema(XSDSchema *newMainSchema) : XSchemaObject(NULL, this), _ownInfoPool(this)
{
    _mainSchema = newMainSchema ;
    _infoPool = &_ownInfoPool ;
    if(NULL != _mainSchema) {
        _infoPool = _mainSchema->infoPool();
    }
    _attributeFormDefault = EQ_MISSING ;
    _elementFormDefault = EQ_MISSING ;
    buildBaseTypes();
    _infoPool->addInclude(this); // by definition
}

XSDSchema::~XSDSchema()
{
    reset();
    foreach(XSchemaObject * o, _baseTypes.values()) {
        if(NULL != o) {
            delete o;
        }
    }
    _baseTypes.clear();
}

void XSDSchema::reset()
{
    XSchemaObject::reset();
    _ownInfoPool.resetLite();
    foreach(XSDSchema * schema, _importedSchemas) {
        delete schema ;
    }
    _importedSchemas.clear();
    _importedSchemasByNamespace.clear();
    _importedSchemasByObject.clear();

    foreach(XSDSchema * schema, _includedSchemas) {
        delete schema ;
    }
    _includedSchemasByObject.clear();

    foreach(XSDSchema * schema, _importedSchemas) {
        delete schema ;
    }
    _redefinedSchemasByObject.clear();

    //emit resetAction(); TODO
}

XSDSchema *XSDSchema::mainSchema() const
{
    return _mainSchema;
}

void XSDSchema::setMainSchema(XSDSchema *mainSchema)
{
    _mainSchema = mainSchema;
}

QString XSDSchema::defaultNamespace()
{
    return _defaultNamespace ;
}

QString XSDSchema::targetNamespace()
{
    return _targetNamespace;
}

void XSDSchema::setLocation(const QString &newLocation)
{
    _location = newLocation;
}

QString XSDSchema::location()
{
    return _location ;
}

QList<XSDSchema*> XSDSchema::schemasByNamespace(const QString &theNamespace)
{
    if(infoPool()->targetNamespace() == theNamespace) {
        return infoPool()->includes();
    }
    QList<XSDSchema*> result = _importedSchemasByNamespace.values(theNamespace);
    return result;
}

XSchemaElement* XSDSchema::addElement()
{
    return realAddElement();
}

XSchemaAttribute *XSDSchema::addAttribute()
{
    return realAddAttribute();
}

bool XSDSchema::saveToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString stringText = getAsString();
    clipboard->setText(stringText);
    return true;
}

QString XSDSchema::getAsString()
{
    QDomDocument    document;
    if(!generateDom(document)) {
        XsdError(tr("Unable to convert schema document to string"));
        return "";
    }
    return document.toString(4);
}

QString XSDSchema::qName(const QString &name)
{
    return _namespacePrefix + ":" + name ;
}

QList<XSchemaObject*> XSDSchema::getChildrenByType(const ESchemaType expected)
{
    QList<XSchemaObject*> outVect;
    foreach(XSchemaObject * child, _children) {
        bool test = false;
        if(expected == SchemaGenericElement) {
            test = (child->getType() == SchemaTypeElement) && !static_cast<XSchemaElement*>(child)->isTypeOrElement();
        } else if(expected == SchemaGenericType) {
            test = (child->getType() == SchemaTypeElement) && static_cast<XSchemaElement*>(child)->isTypeOrElement() ;
        } else {
            test = (child->getType() == expected);
        }
        if(test) {
            outVect.append(child);
        }
    }
    return outVect;
}

QList<XSchemaInclude*> XSDSchema::getIncludes()
{
    // use template in the include?
    QList<XSchemaObject*> data = getChildrenByType(SchemaTypeInclude) ;
    QList<XSchemaInclude*> result ;
    foreach(XSchemaObject * o, data) {
        result << (XSchemaInclude*)o;
    }
    return result;
}

QList<XSchemaRedefine*> XSDSchema::getRedefines()
{
    QList<XSchemaObject*> data = getChildrenByType(SchemaTypeRedefine) ;
    QList<XSchemaRedefine*> result ;
    foreach(XSchemaObject * o, data) {
        result << (XSchemaRedefine*)o;
    }
    return result;
}

QList<XSchemaImport*> XSDSchema::getImports()
{
    QList<XSchemaObject*> data = getChildrenByType(SchemaTypeImport) ;
    QList<XSchemaImport*> result ;
    foreach(XSchemaObject * o, data) {
        result << (XSchemaImport*)o;
    }
    return result;
}


QDomElement XSDSchema::createElementWithNamespace(QDomDocument &document, const QString &tagName)
{
    //TODO: broken namespace support????
    if(!xsdURI().isEmpty()) {
        return document.createElement(qName(tagName));
        //return document.createElementNS( xsdURI(), tagName );
    } else {
        return document.createElement(tagName);
    }
}

QString XSDSchema::xsdNS()
{
    return xsdURI() ;
}

bool XSDSchema::existsNamespace(const QString &namespaceToSearch)
{
    if(namespaceToSearch.isEmpty()) {
        return false;
    }
    if(_namespacePrefix == namespaceToSearch) {
        return false;
    }
    return _namespacesByPrefix.contains(namespaceToSearch);
}

bool XSDSchema::generateDom(QDomDocument &document, QDomNode & /*parent*/)
{
    return generateDom(document);
}

bool XSDSchema::isElementFormQualified()
{
    if((EQ_UNQUALIFIED == _elementFormDefault) || (EQ_MISSING == _elementFormDefault)) {
        return false ;
    }
    return true ;
}

bool XSDSchema::isAttributeFormQualified()
{
    if((EQ_UNQUALIFIED == _attributeFormDefault) || (EQ_MISSING == _attributeFormDefault)) {
        return false ;
    }
    return true ;
}

QString XSDSchema::findTargetNamespace()
{
    return infoPool()->targetNamespace();
}

bool XSDSchema::generateDom(QDomDocument &document)
{
    QDomElement node = createElementWithNamespace(document, IO_XSD_SCHEMA);
    if(_attributeFormDefault != EQ_MISSING) {
        node.setAttribute(IO_SCHEMA_ATTR_AFORMDEFAULT, (EQ_UNQUALIFIED == _attributeFormDefault) ? IO_SCHEMA_UNQUALIFIED : IO_SCHEMA_QUALIFIED);
    }
    if(_elementFormDefault != EQ_MISSING) {
        node.setAttribute(IO_SCHEMA_ATTR_EFORMDEFAULT, (EQ_UNQUALIFIED == _elementFormDefault) ? IO_SCHEMA_UNQUALIFIED : IO_SCHEMA_QUALIFIED);
    }
    addAttrNotEmpty(node, IO_SCHEMA_ATTR_TARGETNAMESPACE, _targetNamespace);
    addAttrNotEmpty(node, IO_SCHEMA_ATTR_ID, _id);
    addAttrNotEmpty(node, IO_SCHEMA_ATTR_VERSION, _version);

    if(!generateInnerDom(document, node)) {
        return false;
    }
    QString blockDefaultVal = finalListToString(_blockDefault);
    addAttrNotEmpty(node, IO_SCHEMA_ATTR_BLOCKDEFAULT, blockDefaultVal);
    QString finalDefaultVal = finalListToString(_finalDefault);
    addAttrNotEmpty(node, IO_SCHEMA_ATTR_FINALDEFAULT, finalDefaultVal);
    addAttrNotEmpty(node, XML_LANGUAGE, _language);
    //TODO xmlns

    addOtherAttributesToDom(node);

    const QString nsPrefix(XML_NS_PREFIX ":");
    foreach(QString aNameSpace, _namespacesByPrefix.keys()) {
        if(aNameSpace.isEmpty()) {
            addAttrNotEmpty(node, XML_NS_PREFIX, _namespacesByPrefix[aNameSpace]);
        } else {
            addAttrNotEmpty(node, nsPrefix + aNameSpace, _namespacesByPrefix[aNameSpace]);
        }
    }

    document.appendChild(node);
    return true ;
}

QStringList XSDSchema::allNamespaces()
{
    QStringList result;
    foreach(QString value, _namespaces.values()) {
        result.append(value);
    }
    return result;
}

void XSDSchema::addNamespace(QDomAttr &attr)
{
    //TODO: check if same namespace.
    //TODO: not my namespace, pls
    QString name = attr.name();
    int index = name.indexOf(':');
    if(index <= 0) {
        XsdError("TODO");
    }
    QString nsPrefix = name.mid(index + 1);
    _namespaces.insert(attr.value());
    _namespacesByPrefix.insert(nsPrefix, attr.value());
    _prefixesByNamespace.insert(attr.value(), nsPrefix);
}

void XSDSchema::addDefaultNamespace(const QString &ns)
{
    _namespaces.insert(ns);
    _namespacesByPrefix.insert("", ns);
    _defaultNamespace = ns;
}

QString XSDSchema::namespacePrefix()
{
    return _namespacePrefix ;
}

QString XSDSchema::namespaceURI()
{
    return _namespaceURI ;
}


XSDSchema* XSDSchema::schema()
{
    return this ;
}

QList<XSchemaElement *> XSDSchema::collectCandidateRootElement()
{
    QList<XSchemaElement *> items;
    QSet<QString> references;
    scanForInnerElementReference(this, references);
    QStringList prefixesToUse;
    if(!_targetNamespace.isEmpty() && _prefixesByNamespace.contains(_targetNamespace)) {
        QList<QString> values = _prefixesByNamespace.values(_targetNamespace);
        foreach(const QString &prefix, values) {
            prefixesToUse.append(prefix);
        }
    }
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeElement) {
            XSchemaElement* element = static_cast<XSchemaElement*>(child);
            if(!element->isTypeOrElement()) {
                if(!references.contains(element->name())) {
                    // build namespace alias
                    if(!_targetNamespace.isEmpty()) {
                        bool found = false;
                        foreach(const QString &nsPrefix, prefixesToUse) {
                            if(!nsPrefix.isEmpty()) {
                                const QString &nameToTest = XmlUtils::makeQualifiedName(nsPrefix, element->name());
                                if(references.contains(nameToTest)) {
                                    found = true;
                                    break;
                                }
                            }
                        }
                        // foreach
                        if(!found) {
                            items.append(element);
                        }
                    } else {
                        items.append(element);
                    }
                } // if contains
            }
        }
    }
    return items;
}

//------------------------ I/O handling ------------------------------------------------------------------------------

bool XSDSchema::scanSchema(XSDLoadContext *loadContext, const QDomElement &schema)
{
    if(!isValidSchema(schema)) {
        return false ;
    }
    addObjectToLoadContext(loadContext, const_cast<QDomElement &>(schema));

    _namespacePrefix = schema.prefix() ;
    _namespaceURI = schema.namespaceURI();

    _attributeFormDefault = EQ_MISSING ;
    _elementFormDefault = EQ_MISSING ;

    QDomNamedNodeMap attributes = schema.attributes();
    int numAttrs = attributes.length();
    for(int i = 0 ; i < numAttrs ; i++) {
        QDomNode node = attributes.item(i);
        QDomAttr attr = node.toAttr();
        QString name = node.nodeName();
        QString attrName = attr.name();
        if(name == IO_SCHEMA_ATTR_AFORMDEFAULT) {
            if(attr.nodeValue() == IO_SCHEMA_QUALIFIED) {
                _attributeFormDefault = EQ_QUALIFIED ;
            }
        } else if(name == IO_SCHEMA_ATTR_EFORMDEFAULT) {
            if(attr.nodeValue() == IO_SCHEMA_QUALIFIED) {
                _elementFormDefault = EQ_QUALIFIED ;
            }
        } else if(name == IO_SCHEMA_ATTR_TARGETNAMESPACE) {
            _targetNamespace = attr.nodeValue();
        } else if(name == IO_SCHEMA_ATTR_ID) {
            _id = attr.value();
        } else if(name == IO_SCHEMA_ATTR_VERSION) {
            _version = attr.value();
        } else if((attr.prefix() == "") && (attr.name() == XML_NS_PREFIX)) {
            addDefaultNamespace(attr.value());
        } else if(attr.prefix() == XML_NS_PREFIX) {
            addNamespace(attr);
        } else if(attrName == IO_SCHEMA_ATTR_BLOCKDEFAULT) {
            _blockDefault = decodeFinalListToString(attr.value());
        } else if(attrName == IO_SCHEMA_ATTR_FINALDEFAULT) {
            _finalDefault = decodeFinalListToString(attr.value());
        } else if((attr.localName() == IO_XML_LANGUAGE) && (attr.prefix() == XML_PREFIX)) {
            _language = attr.value();
        } else {
            if(!readOtherAttributes(attr)) {
                raiseError(loadContext, this, attr, false);
            }
        }
    } // for

    //TODO QList <EFinalType> _blockDefault;
    //TODO QList <EFinalType> _finalDefault;
    //TODO xmlns
    //TODO others attributes
    //TODO: comments
    int nodi = schema.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = schema.childNodes().item(i) ;
        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            QString name = element.localName();

            if(element.namespaceURI() == namespaceURI()) {
                if(name == IO_XSD_TAGINCLUDE) {
                    readHandleObject(loadContext, element, new XSchemaInclude(this, this));
                } else if(name == IO_XSD_TAGIMPORT) {
                    readHandleObject(loadContext, element, new XSchemaImport(this, this));
                } else if(name == IO_XSD_TAGREDEFINE) {
                    readHandleObject(loadContext, element, new XSchemaRedefine(this, this));
                } else if(name == IO_XSD_TAGGROUP) {
                    readHandleObject(loadContext, element, new XSchemaGroup(this, this));
                } else if(name == IO_XSD_TAGATTRIBUTEGROUP) {
                    readHandleObject(loadContext, element, new XSchemaAttributeGroup(this, this));
                } else if((name == IO_XSD_TAGELEMENT)
                          || (name == IO_XSD_COMPLEXTYPE)
                          || (name == IO_XSD_SIMPLETYPE)) {
                    readHandleObject(loadContext, element, new XSchemaElement(this, this));
                } else if(name == IO_XSD_TAGATTRIBUTE) {
                    readHandleObject(loadContext, element, new XSchemaAttribute(this, this));
                } else if(name == IO_XSD_TAGNOTATION) {
                    readHandleObject(loadContext, element, new XSchemaNotation(this, this));
                } else if(name == IO_XSD_ANNOTATION) {
                    readHandleObject(loadContext, element, new XSchemaAnnotation(this, this));
                } else {
                    raiseError(loadContext, this, element, true);
                }
            } else {
                raiseError(loadContext, this, element, true);
            }
        }
    }
    registerData();
    regenerateInternalLists();
    return true;
}

QString XSDSchema::elementsQualifiedString()
{
    if((XSchemaObject::EQ_MISSING == _elementFormDefault) || (XSchemaObject::EQ_UNQUALIFIED == _elementFormDefault)) {
        return "unqualified";
    }
    return "qualified";
}

QString XSDSchema::attributesQualifiedString()
{
    if((XSchemaObject::EQ_MISSING == _attributeFormDefault) || (XSchemaObject::EQ_UNQUALIFIED == _attributeFormDefault)) {
        return "unqualified";
    }
    return "qualified";
}

bool XSDSchema::scanSchemaNS(const QDomElement &schema)
{
    // TODO questo supporto e molto deficitario, cambiare, al limite leggere DOPO aver individuato il proprio namespace.
    if(! schema.nodeName().endsWith(IO_XSD_SCHEMA)) {
        return false ;
    }
    const QString nsAttrPrefix = QString(XML_NS_PREFIX ":");
    QDomNamedNodeMap attributes = schema.attributes();
    int numAttrs = attributes.length();
    for(int i = 0 ; i < numAttrs ; i++) {
        QDomNode node = attributes.item(i);
        QDomAttr attr = node.toAttr();
        QString attrName = attr.name();
        if(attrName.startsWith(nsAttrPrefix)) {
            addNamespace(attr);
        } else if(attr.prefix().isEmpty() && (attr.name() == XML_NS_PREFIX)) {
            addDefaultNamespace(attr.value());
        }

    } // for

    return true;
}

bool XSDSchema::canAddChild(const ESchemaType newType)
{
    switch(newType) {
    case SchemaTypeElement:
    case SchemaTypeAttribute:
    case SchemaTypeInclude:
    case SchemaTypeImport:
    case SchemaTypeRedefine:
    //TODO case SchemaTypeSimpleType????
    //TODO   complexType
    case SchemaTypeGroup:
    case SchemaTypeAttributeGroup:
    case SchemaTypeNotation:
    case SchemaTypeAnnotation:
        return true;
        break;
    default:
        return false;
    }
}

bool XSDSchema::isValidSchema(const QDomElement &schema)
{
    return IsXsdValid(schema.namespaceURI(), schema.localName());
}

bool XSDSchema::IsXsdValid(const QString &nsUri, const QString &localName)
{
    if(!((localName == IO_XSD_SCHEMA) && (nsUri == xsdURI()))) {
        return false ;
    }
    return true;

}

void XSDSchema::registerData()
{
    foreach(XSchemaObject * child, _children) {
        switch(child->getType()) {
        default:
            break;
        case SchemaTypeRedefine:
            infoPool()->addRedefinedTypes(static_cast<XSchemaRedefine *>(child));
            break;
        }
    }
}

void XSDSchema::postLoad()
{
    regenerateInternalLists();
}

void XSDSchema::regenerateInternalLists()
{
    foreach(XSchemaObject * child, _children) {
        switch(child->getType()) {
        default:
            break;
        case SchemaTypeRedefine: {
            foreach(XSchemaObject * redef, child->getChildren()) {
                switch(redef->getType()) {
                case SchemaTypeAnnotation:
                    break;
                default:
                    redef->setIsRedefinition(true);
                    break;
                }
            }
        }
        break;
        }
    }

    //---
}

void XSDSchema::scanForInnerElements(XSchemaObject *parent, QList<XSchemaObject*> &lst)
{
    foreach(XSchemaObject * child, parent->getChildren()) {
        if(child->getType() == SchemaTypeElement) {
            if(!static_cast<XSchemaElement*>(child)->isTypeOrElement()) {
                lst.append(child);
            }
        }
        scanForInnerElements(child, lst);
    }
}

/**
 * @brief XSDSchema::scanForInnerElementReference
 * @param parent
 * @param references
 * @warning: a reference to itself is valid
 */
void XSDSchema::scanForInnerElementReference(XSchemaObject *parent, QSet<QString> &references)
{
    foreach(XSchemaObject * child, parent->getChildren()) {
        if(child->getType() == SchemaTypeElement) {
            if(!static_cast<XSchemaElement*>(child)->isTypeOrElement()) {
                XSchemaElement* element = static_cast<XSchemaElement*>(child);
                if(element->referencedObjectType() == XRT_ELEMENT) {
                    // Check for itself
                    if(!element->isReferencingItself())  {
                        references.insert(element->referencedObjectName());
                    }
                }
            }
        }
        scanForInnerElementReference(child, references);
    }
}

XSchemaElement *XSDSchema::findSimpleType(const QString &nameToSearch)
{
    QString nameWONamespaces = nameToSearch ;
    int indexOf = nameWONamespaces.indexOf(":");
    if(indexOf >= 0) {
        nameWONamespaces = nameWONamespaces.mid(indexOf + 1);
    }
    //---
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeElement) {
            XSchemaElement* el = (XSchemaElement*)child;
            if(el->isSimpleType() && el->isTypeOrElement() && (nameWONamespaces == el->name())) {
                return el;
            }
        }
    }
    return NULL ;
}

void XSDSchema::buildBaseTypes()
{
    for(int typeIndex = MIN_XSD_TYPE ; typeIndex < MAX_XSD_TYPE ; typeIndex++) {
        addBaseType((EXType)typeIndex);
    }
}

void XSDSchema::processInclude(XSchemaInclude * includeObject, XSDSchema * includedSchema)
{
    infoPool()->addInclude(includedSchema);
    includedSchema->setInfoPool(infoPool());
    _includedSchemas.append(includedSchema);
    _includedSchemasByObject.insert(includeObject, includedSchema);
}

/**
 * @brief XSDSchema::includedSchemaByObject
 * @param includeObject
 * @param includedSchema
 * @return the schema included by the given object, if any
 */
XSDSchema *XSDSchema::includedSchemaByObject(XSchemaInclude *includeObject)
{
    if(_includedSchemasByObject.contains(includeObject)) {
        return _includedSchemasByObject[includeObject];
    }
    return NULL ;
}

XSDSchema *XSDSchema::importedSchemaByObject(XSchemaImport *includeObject)
{
    if(_importedSchemasByObject.contains(includeObject)) {
        return _importedSchemasByObject[includeObject];
    }
    return NULL ;
}
void XSDSchema::processRedefine(XSchemaRedefine * redefineObject, XSDSchema * includedSchema)
{
    infoPool()->addRedefine(includedSchema);
    includedSchema->setInfoPool(infoPool());
    _redefinedSchemas.append(includedSchema);
    _redefinedSchemasByObject.insert(redefineObject, includedSchema);
}

/**
 * @brief XSDSchema::redefinedSchemaByObject
 * @param includeObject
 * @param includedSchema
 * @return the schema redefined by the given object, if any
 */
XSDSchema *XSDSchema::redefinedSchemaByObject(XSchemaRedefine *redefineObject)
{
    if(_redefinedSchemasByObject.contains(redefineObject)) {
        return _redefinedSchemasByObject[redefineObject];
    }
    return NULL ;
}

void XSDSchema::processImport(XSchemaImport * importObject, XSDSchema * importedSchema)
{
    _importedSchemas.append(importedSchema);
    _importedSchemasByNamespace.insertMulti(importObject->targetNamespace(), importedSchema);
    _importedSchemasByObject.insert(importObject, importedSchema);
}

XSchemaInfoPool *XSDSchema::infoPool()
{
    return _infoPool;
}

void XSDSchema::setInfoPool(XSchemaInfoPool *value)
{
    _infoPool = value ;
}
