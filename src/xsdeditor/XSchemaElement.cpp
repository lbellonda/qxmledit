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

/**
  Condizioni:
  Figli  Attributi
  ---------------
   No  | No    : SimpleType
   No  | si    : ComplexType SimpleContent
   Si  | No    : Complex Type Complex Content
   Si  | Si    : Complex Type Complex Content


  */
#include "xsdeditor/xschema.h"
#include "xsdeditor/xsddefinitions.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "xmlutils.h"
#include "utils.h"


//------------------------------------

XTypeQueryInfo::XTypeQueryInfo()
{
    _typeInfo = Pure ;
    _isComplex = false;
    _isSimpleTypeUnion = false;
    _isSimpleTypeList = false;
}

XTypeQueryInfo::~XTypeQueryInfo()
{

}

bool XTypeQueryInfo::isRestriction()
{
    bool result = _typeInfo == Restriction ;
    return result ;
}

bool XTypeQueryInfo::isExtension()
{
    bool result = _typeInfo == Extension ;
    return result ;
}

bool XTypeQueryInfo::hasEnum()
{
    bool result = !_enums.isEmpty();
    return result ;
}

void XTypeQueryInfo::setEnums(const QStringList &newEnums)
{
    _enums = newEnums ;
}

QStringList XTypeQueryInfo::enums()
{
    return _enums;
}

void XTypeQueryInfo::setComplexRestriction(const QString &newType)
{
    _typeName = newType ;
    _isComplex = true;
    _typeInfo = Restriction ;
}

void XTypeQueryInfo::setComplexExtension(const QString &newType)
{
    _typeName = newType ;
    _isComplex = true;
    _typeInfo = Extension ;
}

void XTypeQueryInfo::setSimpleRestriction(const QString &newType)
{
    _typeName = newType ;
    _isComplex = false;
    _typeInfo = Restriction ;
}

void XTypeQueryInfo::setSimpleExtension(const QString &newType)
{
    _typeName = newType ;
    _isComplex = false;
    _typeInfo = Extension ;
}

QString XTypeQueryInfo::name()
{
    return _typeName;
}

void XTypeQueryInfo::setName(const QString &newName)
{
    _typeName = newName ;
}


QString XTypeQueryInfo::qualifiedTypeName() const
{
    return _qualifiedTypeName;
}

void XTypeQueryInfo::setQualifiedTypeName(const QString &value)
{
    _qualifiedTypeName = value;
}

bool XTypeQueryInfo::isSimpleTypeUnion() const
{
    return _isSimpleTypeUnion;
}

void XTypeQueryInfo::setIsSimpleTypeUnion(bool value)
{
    _isSimpleTypeUnion = value;
}

bool XTypeQueryInfo::isSimpleTypeList() const
{
    return _isSimpleTypeList;
}

void XTypeQueryInfo::setIsSimpleTypeList(bool value)
{
    _isSimpleTypeList = value;
}



QString XTypeQueryInfo::unionValue() const
{
    return _unionValue;
}

void XTypeQueryInfo::setUnionValue(const QString &value)
{
    _unionValue = value;
}

QString XTypeQueryInfo::listValue() const
{
    return _listValue;
}

void XTypeQueryInfo::setListValue(const QString &value)
{
    _listValue = value;
}

//------------------------------------

XSchemaElement::XSchemaElement(XSchemaObject *newParent, XSchemaRoot *newRoot, const ElementCategory category) : XSchemaObject(newParent, newRoot)
{
    INIT_XSD_ATTRD(_defaultValue);
    INIT_XSD_ATTRD(_fixed);
    INIT_XSD_ATTRD(_ref);
    INIT_XSD_ATTR(_nillable, XEnums::XBOOL_TRUE) ;
    INIT_XSD_ATTR(_abstract, XEnums::XBOOL_FALSE) ;
    INIT_XSD_ATTR(_form, EQ_MISSING) ;
    INIT_XSD_ATTRD(_xsdType);
    INIT_XSD_ATTRD(_block);
    _isPredefined = false ;
    _elementCategory = category ;
    _maxOccurs.defaultValue = 1 ;
    _minOccurs.defaultValue = 1 ;
    _mixed = XEnums::XBOOL_FALSE;
    _isTypeOrElement = false ;
    _annotation = NULL ;
    _innerAnnotation = NULL ;
}

XSchemaElement::~XSchemaElement()
{
    if(_innerAnnotation) {
        delete _innerAnnotation ;
        _innerAnnotation = NULL ;
    }
    reset();
}

bool XSchemaElement::isPredefined() const
{
    return _isPredefined;
}

void XSchemaElement::setIsPredefined(bool value)
{
    _isPredefined = value ;
}


QString XSchemaElement::nameOrReference()
{
    if(!name().isEmpty()) {
        return name();
    }
    QString reference = referencedObjectName();
    return reference;
}

QString XSchemaElement::tagName()
{
    if(isTypeOrElement()) {
        switch(category()) {
        case EES_COMPLEX_DERIVED:
        case EES_COMPLEX_DEFINITION:
            return "complexType";
        default:
            return "simpleType";
        }
    } else {
        return "element";
    }
}

bool XSchemaElement::isTypeOrElement()
{
    return _isTypeOrElement ;
}

bool XSchemaElement::canAddAttribute()
{
    switch(category()) {
    case EES_SIMPLETYPE_WITHATTRIBUTES:
    case EES_COMPLEX_DERIVED:
    case EES_COMPLEX_DEFINITION:
        return true ;
    default:
        return false ;
    }
}

bool XSchemaElement::canAddSequence()
{
    if(category() == EES_COMPLEX_DEFINITION) {
        return true ;
    }
    return false ;
}

bool XSchemaElement::canAddChoice()
{
    if(category() == EES_COMPLEX_DEFINITION) {
        return true ;
    }
    return false ;
}

bool XSchemaElement::canAddRestriction()
{
    switch(_elementCategory) {
    default:
        return false;

    case EES_SIMPLETYPE_ONLY:
    case EES_SIMPLETYPE_WITHATTRIBUTES:
    case EES_COMPLEX_DERIVED:
        return true ; //TODO !hasChild();
    }
}


XSchemaElement::ElementCategory XSchemaElement::category()
{
    return _elementCategory ;
}

/**
  does a content transformation to set it cohererent with desired setting
  */
void XSchemaElement::setCategory(const XSchemaElement::ElementCategory newCategory)
{
    if(newCategory == _elementCategory) {
        return ;
    }
    _elementCategory = newCategory ;
    switch(_elementCategory) {
    default:
        XsdError("TODO bad type (3)");
        reset();
        break;
    case EES_EMPTY:
        reset();
        break;
    case EES_REFERENCE:
        reset();
        break;
    case EES_SIMPLETYPE_ONLY:
        reset();
        break;
    case EES_SIMPLETYPE_WITHATTRIBUTES:
        XSchemaObject::reset();
        break;
    case EES_COMPLEX_DERIVED:
        //TODO: elimina i figli non coerenti
        XSchemaObject::reset();
        break;
    case EES_COMPLEX_DEFINITION:
        //TODO: elimina i figli non coerenti
        break;
    }
    emit propertyChanged(PROPERTY_ELEMENT_CATEGORY);
}

XReferenceType XSchemaElement::referencedObjectType()
{
    if(EES_REFERENCE == _elementCategory) {
        if(isTypeOrElement()) {
            return XRT_TYPE;
        } else {
            return XRT_ELEMENT;
        }
    }
    return XRT_NONE ;
}

QString XSchemaElement::referencedObjectName()
{
    return _ref ;
}

QString XSchemaElement::occurrencesDescr()
{
    return occurrencesDescrString(_minOccurs, _maxOccurs);
}

QString XSchemaElement::nameDescr()
{
    QString ownName ;
    if(_parent->getType() == SchemaTypeSchema) {
        ownName = name();
    }

    QString result ;
    switch(_elementCategory) {
    default:
        XsdError("TODO bad type (3)");
        break;
    case EES_EMPTY:
        result = name();
        break;
    case EES_REFERENCE:
        if(!ownName.isEmpty()) {
            result = QString(tr("%1 Reference to %2")).arg(name()).arg(_ref);
        } else {
            result = QString(tr("Reference to %1")).arg(_ref);
        }
        break;
    case EES_SIMPLETYPE_ONLY:
    case EES_SIMPLETYPE_WITHATTRIBUTES: {
        result = name();
        break;
        case EES_COMPLEX_DERIVED:
            result = name();
            break;
        case EES_COMPLEX_DEFINITION:
            result = name();
            break;
        }
    }
    return result;
}

QString XSchemaElement::simpleComplexDescr()
{
    QString result ;
    switch(_elementCategory) {
    default:
        XsdError("TODO bad type (3)");
        break;
    case EES_EMPTY:
    case EES_REFERENCE:
        break;
    case EES_SIMPLETYPE_ONLY:
    case EES_SIMPLETYPE_WITHATTRIBUTES:
        result = QString(tr("simple"));
        break;
    case EES_COMPLEX_DERIVED:
        result = QString(tr("complex"));
        break;
    case EES_COMPLEX_DEFINITION:
        result = QString(tr("complex"));
        break;
    }
    return result;
}

QString XSchemaElement::description()
{
    QString occurrences ;
    if((_minOccurs.isSet) && (_maxOccurs.isSet)) {
        occurrences = QString("%1 .. %2").arg(_minOccurs.toString()).arg(_maxOccurs.toString());
    } else if(_minOccurs.isSet) {
        occurrences = QString("%1 .. ").arg(_minOccurs.toString());
    } else if(_maxOccurs.isSet) {
        occurrences = QString(" .. %1").arg(_maxOccurs.toString());
    }

    QString ownName ;
    if(_parent->getType() == SchemaTypeSchema) {
        ownName = name();
    }
    QString result ;
    switch(_elementCategory) {
    default:
        XsdError("TODO bad type (3)");
        break;
    case EES_EMPTY :
        if(!ownName.isEmpty()) {
            result = QString(tr("'%1' simple")).arg(name());
        } else {
            result = QString(tr("simple"));
        }
        break;

    case EES_REFERENCE:
        if(_isTypeOrElement) {
            result = QString(tr("Reference to %1")).arg(_ref);
        } else {
            result = QString(tr("Reference to %1")).arg(_ref);
        }
        break;
    case EES_SIMPLETYPE_ONLY:
    case EES_SIMPLETYPE_WITHATTRIBUTES:
        if(!ownName.isEmpty()) {
            result = QString(tr("'%1' simple")).arg(name());
        } else {
            result = QString(tr("simple"));
        }
        break;
    case EES_COMPLEX_DERIVED:
        if(!ownName.isEmpty()) {
            result = QString(tr("'%1' complex")).arg(name());
        } else {
            result = QString(tr("complex"));
        }
        break;
    case EES_COMPLEX_DEFINITION:
        if(!ownName.isEmpty()) {
            result = QString(tr("'%1' complex")).arg(name());
        } else {
            result = QString(tr("complex"));
        }
        break;
    }
    if(!xsdType().isEmpty()) {
        result.append(" (");
        result.append(_xsdType);
        result.append(")");
    }
    if(!occurrences.isEmpty()) {
        result.append("\n");
        result += occurrences ;
    }
    return result ;
}


void XSchemaElement::reset()
{
    XSchemaObject::reset();
    resetAttributes();
}

void XSchemaElement::resetAttributes()
{
    foreach(XSchemaObject * attribute, _attributes) {
        delete attribute;
    }
    _attributes.clear();
}

XSchemaAttribute* XSchemaElement::addAttribute()
{
    XSchemaAttribute* attribute = new XSchemaAttribute(this, _root);
    if(NULL != attribute) {
        _attributes.append(attribute);
        emit childAdded(attribute);
    }
    return attribute ;
}

XSchemaElement* XSchemaElement::addElement()
{
    return realAddElement();
}


XSchemaChoice* XSchemaElement::addChoice()
{
    return realAddChoice();
}

XSchemaSequence* XSchemaElement::addSequence()
{
    return realAddSequence();
}

XSchemaObject* XSchemaElement::addRestriction()
{
    switch(_elementCategory) {
    default:
        return NULL;

    case EES_SIMPLETYPE_ONLY:
    case EES_SIMPLETYPE_WITHATTRIBUTES:
        return realAddSimpleTypeRestriction();
    case EES_COMPLEX_DERIVED:
        XsdError("TODO");
        //return realAddComplexTypeRestriction();
        return NULL;
    }
}

bool XSchemaElement::generateDom(QDomDocument &document, QDomNode &parent)
{
    QDomElement node ;
    if(!_isTypeOrElement) {
        node = createElement(document, IO_XSD_TAGELEMENT);
        // attribute evaluation
    }
    QDomElement typeSpecifier ;
    switch(category()) {
    default:
        XsdError("TODO bad type (2)");

    case EES_EMPTY:
        break;
    case EES_REFERENCE:
        if(!ref().isEmpty()) {
            if(_isTypeOrElement) {
                XsdError("type with reference set "); //TODO
            }
            node.setAttribute(IO_ELEMENT_ATTR_REF, ref());
        }
        break;
    case EES_SIMPLETYPE_ONLY:
        typeSpecifier = createElement(document, IO_XSD_SIMPLETYPE);
        writeHandleSimpleType(document, typeSpecifier);
        break;
    case EES_SIMPLETYPE_WITHATTRIBUTES:
        typeSpecifier = createElement(document, IO_XSD_SIMPLETYPE);
        writeHandleComplexType(document, typeSpecifier);
        break;
    case EES_COMPLEX_DERIVED:
        typeSpecifier = createElement(document, IO_XSD_COMPLEXTYPE);
        writeHandleComplexType(document, typeSpecifier);
        break;
    case EES_COMPLEX_DEFINITION:
        typeSpecifier = createElement(document, IO_XSD_COMPLEXTYPE);
        writeHandleComplexType(document, typeSpecifier);
        break;
    }
    if(_isTypeOrElement) {
        parent.appendChild(typeSpecifier);
    } else {
        if(NULL != _annotation) {
            _annotation->generateDom(document, node);
        }
        if(!typeSpecifier.isNull()) {
            node.appendChild(typeSpecifier);
        }
        writeAppendElementAttributes(document, node);
        parent.appendChild(node);
    }
    return true ;
}

void XSchemaElement::writeHandleSimpleType(QDomDocument &document, QDomElement &typeSpecifier)
{
    // handles type attributes
    //TODO
    if(!_isTypeOrElement) {
        addAttrNotEmpty(typeSpecifier, IO_SIMPLETYPE_ATTR_ID, _containedId);
    } else {
        addAttrNotEmpty(typeSpecifier, IO_SIMPLETYPE_ATTR_ID, id());
        addAttrNotEmpty(typeSpecifier, IO_SIMPLETYPE_ATTR_NAME, name());
        //TODOaddAttrNotEmpty( typeSpecifier, IO_SIMPLETYPE_ATTR_FINAL, final() );
    }
    //TODO: any other attrs _innerTypeAttributes
    if(_isTypeOrElement) {
        if(NULL != _annotation) {
            _annotation->generateDom(document, typeSpecifier);
        }
    } else {
        if(NULL != _innerAnnotation) {
            _innerAnnotation->generateDom(document, typeSpecifier);
        }
    }
    foreach(XSchemaObject * child, _children) {
        child->generateDom(document, typeSpecifier);
    }
}

void XSchemaElement::writeHandleComplexType(QDomDocument &document, QDomElement &typeSpecifier)
{
    // handles type attributes
    //TODO LOOK FOR PROPERTIES PARENT DEPENDENT
    addAttrNotEmpty(typeSpecifier, IO_COMPLEXTYPE_ATTR_ABSTRACT, XEnums::boolToString(abstract()));
    //TODO addAttrNotEmpty(typeSpecifier, IO_COMPLEXTYPE_ATTR_BLOCK, block() );
    //TODO addAttrNotEmpty(typeSpecifier, IO_COMPLEXTYPE_ATTR_FINAL, final() );
    addAttrNotEmpty(typeSpecifier, IO_COMPLEXTYPE_ATTR_ID, id());  // TODO: innerid?
    addAttrNotEmpty(typeSpecifier, IO_GENERIC_MIXED, XEnums::boolToString(mixed()));
    if(_isTypeOrElement) {
        addAttrNotEmpty(typeSpecifier, IO_GENERIC_NAME, name());
    }
    addOtherAttributesToDom(typeSpecifier);

    writeAppendNotAttributes(document, typeSpecifier);

    writeAppendAttributes(document, typeSpecifier);
}

void XSchemaElement::writeAppendElementAttributes(QDomDocument &/*document*/, QDomElement &node)
{
    //TODO
    //addAttributeIfNotEmpty();
    /*TODO
    if( !id().isEmpty() ) {
        node.setAttribute( "id", id() );
    }*//*
TODO: apply specific rules
*/
    addAttrNotEmpty(node, IO_ELEMENT_ATTR_NAME, name());
    addAttrNotEmpty(node, IO_ELEMENT_ATTR_ID, id());
    addAttrNotEmpty(node, IO_ELEMENT_ATTR_TYPE, xsdType());
    addAttrNotEmpty(node, IO_ELEMENT_ATTR_REF, ref());

    addAttrNotEmpty(node, IO_ELEMENT_ATTR_ABSTRACT, XEnums::boolToString(abstract()));
    //TODO block = (#all | List of (extension | restriction | substitution))
    addAttrNotEmpty(node, IO_ELEMENT_ATTR_DEFAULT, defaultValue());
    //TODO final = (#all | List of (extension | restriction))
    addAttrNotEmpty(node, IO_ELEMENT_ATTR_FIXED, fixed());
    //TODO form = (qualified | unqualified)
    _minOccurs.addAttrToNode(node, IO_GENERIC_MINOCCURS);
    _maxOccurs.addAttrToNode(node, IO_GENERIC_MAXOCCURS);
    addAttrNotEmpty(node, IO_ELEMENT_ATTR_NILLABLE, XEnums::boolToString(nillable()));
    //  substitutionGroup = QName
    addOtherAttributesToDom(node);
}

void XSchemaElement::writeAppendAttributes(QDomDocument &document, QDomElement &node)
{
    // attributes (attribute|attributeGroup)*,anyAttribute?
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeAttribute) {
            QDomElement elmentAttr = createElement(document, IO_XSD_ATTRIBUTE);
            //TODO: fill attributes in their own class
            node.appendChild(elmentAttr);
        }
    }
}

void XSchemaElement::writeAppendNotAttributes(QDomDocument &document, QDomElement &node)
{
    foreach(XSchemaObject * child, _children) {
        if(child->getType() != SchemaTypeAttribute) {
            child->generateDom(document, node);
        }
    }
}

//----------------------- accessors -----------------
/*
bool XSchemaElement::abstract()
{
    return _abstract ;
}

void XSchemaElement::setAbstract(bool newAbstract)
{
    if( _abstract != newAbstract ) {
        _abstract = newAbstract ;
        emit abstractChanged(_abstract);
    }
}
*/

IMPL_QPROP(XEnums::XBool, XSchemaElement, abstract, setAbstract, PROPERTY_ELEMENT_ABSTRACT)
IMPL_QPROP(XEnums::XBool, XSchemaElement, nillable, setNillable, PROPERTY_ELEMENT_NILLABLE)
IMPL_QPROP(XEnums::XBool, XSchemaElement, mixed, setMixed, PROPERTY_ELEMENT_MIXED)
IMPL_QPROPS(XSchemaElement, fixed, setFixed, PROPERTY_ELEMENT_FIXED)
IMPL_QPROPS(XSchemaElement, defaultValue, setDefaultValue, PROPERTY_ELEMENT_DEFAULTVALUE)
// TODO IMPL_QPROP(XOccurrence,XSchemaElement,maxOccurs,setMaxOccurs,PROPERTY_ELEMENT_MAXOCCURS);
// TODO IMPL_QPROP(XOccurrence,XSchemaElement,minOccurs,setMinOccurs,PROPERTY_ELEMENT_MINOCCURS);
//TODO IMPL_QPROP(XSchemaElement::EFinalType, XSchemaElement, final, setFinal, PROPERTY_ELEMENT_FINAL);
//TODO IMPL_QPROP(XSchemaElement::EFinalType, XSchemaElement, block, setBlock, PROPERTY_ELEMENT_BLOCK);
IMPL_QPROPS(XSchemaElement, xsdType, setXsdType, PROPERTY_ELEMENT_TYPE)
IMPL_QPROPS(XSchemaElement, ref, setRef, PROPERTY_ELEMENT_REF)
#if 0

IMPL_QPROPS(XSchemaElement, substitutionGroup, setSubstitutionGroup, PROPERTY_ELEMENT_SUBSTITUTIONGROUP);

#endif


bool XSchemaElement::isSimpleType()
{
    return (_elementCategory == EES_SIMPLETYPE_ONLY);
}

bool XSchemaElement::isComplexType()
{
    switch(_elementCategory) {
    default:
        return false;
    case EES_SIMPLETYPE_WITHATTRIBUTES:
    case EES_COMPLEX_DERIVED:
    case EES_COMPLEX_DEFINITION:
        return true;
    }
}

void XSchemaElement::loadFromDom(XSDLoadContext *loadContext, QDomElement &element)
{
    reset();

    addObjectToLoadContext(loadContext, element);

    //TODO: manca il set del tipo interno
    if(element.localName() == IO_XSD_COMPLEXTYPE) {
        _isTypeOrElement = true ;
        readHandleComplexType(loadContext, element, true);
        return ;
    } else if(element.localName() == IO_XSD_SIMPLETYPE) {
        _isTypeOrElement = true ;
        readHandleSimpleType(loadContext, element, true);
        return ;
    } else {
        _isTypeOrElement = false ;
    }
// TODO : category
    QDomNamedNodeMap attributes = element.attributes();
    int numAttrs = attributes.length();
    for(int i = 0 ; i < numAttrs ; i++) {
        QDomNode node = attributes.item(i);
        QDomAttr attr = node.toAttr();
        QString name = node.nodeName();

        /*if( isComplexType ) { TODO: check if fully handled separately
            if( name == IO_COMPLEXTYPE_ATTR_ABSTRACT ) {
                //TODO abstract = "false"
            } else if( name == IO_COMPLEXTYPE_ATTR_BLOCK ) {
              //TODO block = "#all"
            } else if( name == IO_COMPLEXTYPE_ATTR_FINAL ) {
              //TODO final = "#all"
            } else if( name == IO_COMPLEXTYPE_ATTR_ID ) {
              _id = attr.value();
            } else if( name == IO_COMPLEXTYPE_ATTR_MIXED ) {
              //TODO mixed = "false"
            } else if( name == IO_GENERIC_NAME ) {
              _name = attr.value();
             } else {
                 if( !readOtherAttributes(attr) ) {
                     raiseError( this, attr, false );
                 }
             }*/
        /*} TODO: DELETE else if( isSimpleType ) { TODO: check if fully handled separately
             if( name == IO_SIMPLETYPE_ATTR_FINAL ) {
               //TODO final = "#all"
             } else if( name == IO_SIMPLETYPE_ATTR_ID ) {
               _id = attr.value();
             } else if( name == IO_SIMPLETYPE_ATTR_NAME ) {
               _name = attr.value();
              } else {
                  if( !readOtherAttributes(attr) ) {
                      raiseError( this, attr, false );
                  }
              }*/
        /*if(!_isTypeOrElement) {*/
        if(name == IO_ELEMENT_ATTR_ID) {
            _id = attr.value() ;
        } else if(name == IO_ATTRIBUTE_ATTR_FORM) {
            SET_XSD_ATTR(_form, decodeForm(attr.value()));
        } else if(name == IO_ELEMENT_ATTR_NAME) {
            SET_XSD_ATTR(_name, attr.value());
        } else if(name == IO_ELEMENT_ATTR_REF) {
            SET_XSD_ATTR(_ref, attr.value());
        } else if(name == IO_ATTRIBUTE_ATTR_TYPE) {
            SET_XSD_ATTR(_xsdType, attr.value()) ;
        } else if(name == IO_ELEMENT_ATTR_SUBSTGROUP) {
            //TODO
        } else if(name == IO_ELEMENT_ATTR_DEFAULT) {
            SET_XSD_ATTR(_defaultValue, attr.value());
        } else if(name == IO_ELEMENT_ATTR_FIXED) {
            SET_XSD_ATTR(_fixed, attr.value());
        } else if(name == IO_ELEMENT_ATTR_FORM) {
            //TODO
        } else if(name == IO_ELEMENT_ATTR_MAXOCCURS) {
            if(!_maxOccurs.setValueFromAttribute(attr.value())) {
                raiseError(loadContext, this, attr, false);
            }
        } else if(name == IO_ELEMENT_ATTR_MINOCCURS) {
            if(!_minOccurs.setValueFromAttribute(attr.value())) {
                raiseError(loadContext, this, attr, false);
            }
        } else if(name == IO_ELEMENT_ATTR_NILLABLE) {
            SET_XSD_ATTR(_nillable, XEnums::parseBool(attr.value()));
        } else if(name == IO_ELEMENT_ATTR_ABSTRACT) {
            SET_XSD_ATTR(_abstract, XEnums::parseBool(attr.value()));
        } else if(name == IO_ELEMENT_ATTR_BLOCK) {
            SET_XSD_ATTR(_block, decodeFinalListToString(attr.value()));
        } else if(name == IO_ELEMENT_ATTR_FINAL) {
            _final = decodeFinalListToString(attr.value());
        } else {
            if(!readOtherAttributes(attr)) {
                raiseError(loadContext, this, attr, false);
            }
        }
        /*} // element*/
    }
    if(!_isTypeOrElement && ! _ref.isEmpty()) {
        _elementCategory = EES_REFERENCE ;
    }
    bool simpleTypeHandled = false;
    bool complexTypeHandled = false;
    QString namespaceURI = _root->namespaceURI();
    int nodi = element.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            QString name = element.localName();
            if(element.namespaceURI() == namespaceURI) {
                if(name == IO_XSD_ANNOTATION) {
                    readHandleAnnotation(loadContext, element, false);
                } else if(name == IO_XSD_SIMPLETYPE) {
                    if(complexTypeHandled) {
                        raiseErrorTypesME(loadContext, element);
                    } else {
                        simpleTypeHandled = true;
                        readHandleSimpleType(loadContext, element, false);
                    }
                } else if(name == IO_XSD_COMPLEXTYPE) {
                    if(simpleTypeHandled) {
                        raiseErrorTypesME(loadContext, element);
                    } else {
                        complexTypeHandled = true ;
                        readHandleComplexType(loadContext, element, false);
                    }
                } else if(name == IO_XSD_UNIQUE) {
                    readHandleObject(loadContext, element, new XSchemaUnique(this, _root));
                } else if(name == IO_XSD_KEY) {
                    readHandleObject(loadContext, element, new XSchemaKey(this, _root));
                } else if(name == IO_XSD_KEYREF) {
                    readHandleObject(loadContext, element, new XSchemaKeyRef(this, _root));
                } else {
                    raiseError(loadContext, this, element, true);
                }
            }
        }
    }
    validateAfterRead(loadContext, element, NULL);
}

//TODO la cosa si complica, il validatore w3c permette un id nel simpletype contenuto in un elemento
// la specifica dice altro.
bool XSchemaElement::readHandleSimpleType(XSDLoadContext *loadContext, QDomElement &element, const bool isTopLevel)
{
    bool isOk = true ;
    _elementCategory = EES_SIMPLETYPE_ONLY ;
    QDomNamedNodeMap attributes = element.attributes();
    int numAttrs = attributes.length();
    for(int i = 0 ; i < numAttrs ; i++) {
        QDomNode node = attributes.item(i);
        QDomAttr attr = node.toAttr();
        QString name = node.nodeName();

        if(isTopLevel) {
            if(name == IO_SIMPLETYPE_ATTR_ID) {
                if(_isTypeOrElement) {
                    _id = attr.value() ;
                } else {
                    _containedId = attr.value() ;
                }
            } else if(name == IO_SIMPLETYPE_ATTR_NAME) {
                if(_isTypeOrElement) {
                    SET_XSD_ATTR(_name, attr.value());
                }
            } else if(name == IO_SIMPLETYPE_ATTR_FINAL) {
                _final = decodeFinalListToString(attr.value());
            } else if(name == IO_ATTRIBUTE_ATTR_REF) {
                raiseError(loadContext, XSD_LOAD_ERROR_TYPE_TOP_REF_SET, this, element, tr("Type definition: reference not allowed."));
            } else {
                if(!readOtherAttributes(attr)) {
                    raiseError(loadContext, this, attr, false);
                }
            }
        } else {
            if(name == IO_SIMPLETYPE_ATTR_ID) {
                if(_isTypeOrElement) {
                    _id = attr.value() ;
                } else {
                    _containedId = attr.value() ;
                }
            } else if(name == IO_SIMPLETYPE_ATTR_NAME) {
                raiseError(loadContext, XSD_LOAD_ERROR_TYPE_AN_NAME_SET, this, element, tr("Type definition: name set, not allowed."));
            } else if(name == IO_ATTRIBUTE_ATTR_REF) {
                raiseError(loadContext, XSD_LOAD_ERROR_TYPE_TOP_REF_SET, this, element, tr("Type definition: reference not allowed."));
            } else {
                if(!readOtherAttributes(attr)) {
                    raiseError(loadContext, this, attr, false);
                }
            }
        }
    }
    QString namespaceURI = _root->namespaceURI();
    int nodi = element.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;

        if(childNode.isElement()) {
            bool childFound = false;
            QDomElement element = childNode.toElement();
            QString localName = element.localName();
            if(element.namespaceURI() == namespaceURI) {
                if(localName == IO_XSD_ANNOTATION) {
                    readHandleAnnotation(loadContext, element, !_isTypeOrElement);
                } else if(localName == IO_XSD_RESTRICTION) {
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    childFound = true;
                    readHandleObject(loadContext, element, new XSchemaSimpleTypeRestriction(this, _root));
                } else if(localName == IO_XSD_LIST) {
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    childFound = true;
                    readHandleObject(loadContext, element, new XSchemaSimpleTypeList(this, _root));
                } else if(localName == IO_XSD_UNION) {
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    childFound = true;
                    readHandleObject(loadContext, element, new XSchemaSimpleTypeUnion(this, _root));
                } else {
                    raiseError(loadContext, this, childNode, true);
                }
            }
        }
    }
    validateAfterRead(loadContext, element, NULL);
    return isOk;
}

bool XSchemaElement::readHandleComplexType(XSDLoadContext *loadContext, QDomElement &element1, const bool isTopLevel)
{
    bool isOk = true ;
    _elementCategory = EES_COMPLEX_DEFINITION ;
    QDomNamedNodeMap attributes = element1.attributes();
    int numAttrs = attributes.length();
    for(int i = 0 ; i < numAttrs ; i++) {
        QDomNode node = attributes.item(i);
        QDomAttr attr = node.toAttr();
        QString name = node.nodeName();

        if(isTopLevel) {
            if((name == IO_GENERIC_ID)) {
                if(_isTypeOrElement) {
                    _id = attr.value() ;
                } else {
                    _containedId = attr.value() ;
                }
            } else if((name == IO_GENERIC_NAME) && _isTypeOrElement) {
                SET_XSD_ATTR(_name, attr.value());
            } else if(name == IO_COMPLEXTYPE_ATTR_ABSTRACT) {
                _abstract = XEnums::parseBool(attr.value());
            } else if(name == IO_COMPLEXTYPE_ATTR_BLOCK) {
                SET_XSD_ATTR(_block, decodeFinalListToString(attr.value()));
            } else if(name == IO_COMPLEXTYPE_ATTR_FINAL) {
                _final = decodeFinalListToString(attr.value());
            } else if(name == IO_COMPLEXTYPE_ATTR_ID) {
                _id = attr.value();
            } else if(name == IO_GENERIC_MIXED) {
                _mixed = XEnums::parseBool(attr.value());
            } else if(name == IO_ATTRIBUTE_ATTR_REF) {
                raiseError(loadContext, XSD_LOAD_ERROR_TYPE_TOP_REF_SET, this, element1, tr("Type definition: reference not allowed."));
            } else {
                if(!readOtherAttributes(attr)) {
                    raiseError(loadContext, this, attr, false);
                }
            }
        } else {
            if((name == IO_GENERIC_ID)) {
                if(_isTypeOrElement) {
                    _id = attr.value() ;
                } else {
                    _containedId = attr.value() ;
                }
            } else if(name == IO_COMPLEXTYPE_ATTR_ID) {
                _id = attr.value();
            } else if(name == IO_GENERIC_MIXED) {
                _mixed = XEnums::parseBool(attr.value());
            } else if(name == IO_ATTRIBUTE_ATTR_REF) {
                raiseError(loadContext, XSD_LOAD_ERROR_TYPE_TOP_REF_SET, this, element1, tr("Type definition: reference not allowed."));
            } else if(name == IO_GENERIC_NAME) {
                raiseError(loadContext, XSD_LOAD_ERROR_TYPE_AN_NAME_SET, this, element1, tr("Type definition: name set, not allowed."));
            } else {
                if(!readOtherAttributes(attr)) {
                    raiseError(loadContext, this, attr, false);
                }
            }
        }
    }

    QString namespaceURI = _root->namespaceURI();
    int nodi = element1.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element1.childNodes().item(i) ;

        bool childFound = false;
        if(childNode.isElement()) {
            QDomElement childElement = childNode.toElement();
            QString localName = childElement.localName();
            if(childElement.namespaceURI() == namespaceURI) {
                if(localName == IO_XSD_ANNOTATION) {
                    readHandleAnnotation(loadContext, childElement, !_isTypeOrElement);
                } else if(localName == IO_XSD_SIMPLECONTENT) {
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    XSchemaSimpleContent *newObject = readSimpleContentChild(loadContext, childElement);
                    if(NULL == newObject) {
                        raiseErrorForObject(loadContext, childElement);
                    }
                    addChild(newObject);
                    childFound = true ;
                } else if(localName == IO_XSD_COMPLEXCONTENT) {
                    //TODO
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    XSchemaComplexContent *newObject = readComplexContentChild(loadContext, childElement);
                    if(NULL == newObject) {
                        raiseErrorForObject(loadContext, childElement);
                    }
                    addChild(newObject);
                    childFound = true ;
//---------------------- one at max ---------------------
                } else if(localName == IO_XSD_GROUP) {
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    readHandleObject(loadContext, childElement, new XSchemaGroup(this, _root));
                    childFound = true ;
                } else if(localName == IO_XSD_ALL) {
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    readHandleObject(loadContext, childElement, new XSchemaAll(this, _root));
                    childFound = true ;
                } else if(localName == IO_XSD_CHOICE) {
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    readHandleObject(loadContext, childElement, new XSchemaChoice(this, _root));
                    childFound = true ;
                } else if(localName == IO_XSD_SEQUENCE) {
                    //TODO
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    // TODO: look ahead to find the type of
                    readHandleObject(loadContext, childElement, new XSchemaSequence(this, _root));
                    childFound = true ;
//---------------------- any number ---------------------
                } else if(localName == IO_XSD_ATTRIBUTE) {
                    // TODO: attributes!
                    XSchemaAttribute* attribute = new XSchemaAttribute(this, _root);
                    if(NULL == attribute) {
                        raiseErrorForObject(loadContext, childElement);
                    }
                    _attributes.append(attribute);
                    attribute->loadFromDom(loadContext, childElement);
                } else if(localName == IO_XSD_ATTRIBUTEGROUP) {
                    //TODO
                    //readHandleObject(childElement, new XSchemaAttributeGroup(this, _root));

                    XSchemaAttributeGroup* attributeGroup = new XSchemaAttributeGroup(this, _root);
                    if(NULL == attributeGroup) {
                        raiseErrorForObject(loadContext, childElement);
                    }
                    _attributes.append(attributeGroup);
                    attributeGroup->loadFromDom(loadContext, childElement);

                } else if(localName == IO_XSD_ANYATTRIBUTE) {
                    if(childFound) {
                        raiseError(loadContext, this, childNode, true);
                    }
                    // TODO: look ahead to foind the type of
                    readHandleObject(loadContext, childElement, new XSchemaAnyAttribute(this, _root));
                    childFound = true ;
                }
            } else {
                raiseError(loadContext, this, childNode, true);
            }
        }
    }
    validateComplexType(element1, !_isTypeOrElement);
    validateAfterRead(loadContext, element1, NULL);
    return isOk;
}

bool XSchemaElement::isExtension()
{
    if(_elementCategory == EES_COMPLEX_DERIVED) {
        if(_children.size() > 0) {
            XSchemaObject *child = _children.at(0);
            if(child->getType() == SchemaTypeComplexContentExtension) {
                return true;
            }
        }
    }
    return false;
}

bool XSchemaElement::isRestriction()
{
    if(_elementCategory == EES_COMPLEX_DERIVED) {
        if(_children.size() > 0) {
            XSchemaObject *child = _children.at(0);
            if(child->getType() == SchemaTypeComplexContentRestriction) {
                return true;
            }
        }
    }
    return false;
}

bool XSchemaElement::isSimpleExtension()
{
    if(_elementCategory == EES_SIMPLETYPE_WITHATTRIBUTES) {
        if(_children.size() > 0) {
            XSchemaObject *child = _children.at(0);
            if(child->getType() == SchemaTypeSimpleContentExtension) {
                return true;
            }
        }
    }
    return false;
}

bool XSchemaElement::isSimpleRestiction()
{
    if(_elementCategory == EES_SIMPLETYPE_WITHATTRIBUTES) {
        if(_children.size() > 0) {
            XSchemaObject *child = _children.at(0);
            if(child->getType() == SchemaTypeSimpleContentRestriction) {
                return true;
            }
        }
    }
    return false;
}

void XSchemaElement::validateComplexType(QDomElement &/*element*/, const bool /*isInner*/)
{
    //TODO
}

void XSchemaElement::readHandleAnnotation(XSDLoadContext *loadContext, QDomElement &element, const bool isInner)
{
    if(!isInner) {
        XSchemaObject::readHandleAnnotation(loadContext, element);
    } else {
        if(NULL != _innerAnnotation) {
            raiseError(loadContext, this, element, true);
        }
        _innerAnnotation = new XSchemaAnnotation(this, _root) ;
        if(NULL == _innerAnnotation) {
            raiseError(loadContext, this, element, true);
        }
        _innerAnnotation->loadFromDom(loadContext, element);
    }
}

XSchemaSimpleContent *XSchemaElement::readSimpleContentChild(XSDLoadContext *loadContext, QDomElement &element)
{
    XSchemaSimpleContent *newObject = NULL ;
    QString namespaceURI = _root->namespaceURI();
    int nodi = element.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;
        if(childNode.isElement()) {
            QDomElement childElement = childNode.toElement();
            QString localName = childElement.localName();
            if(childElement.namespaceURI() == namespaceURI) {
                if(localName == IO_XSD_SIMPLECONTENT_RESTRICTION) {
                    newObject = new XSchemaSimpleContentRestriction(this, _root);
                    newObject->readBaseProperties(loadContext, element);
                    _elementCategory = EES_SIMPLETYPE_WITHATTRIBUTES ;
                    return newObject ;
                }
                if(localName == IO_XSD_SIMPLECONTENT_EXTENSION) {
                    newObject = new XSchemaSimpleContentExtension(this, _root);
                    newObject->readBaseProperties(loadContext, element);
                    _elementCategory = EES_SIMPLETYPE_WITHATTRIBUTES ;
                    return newObject ;
                }
            }
        }
    }
    raiseError(loadContext, this, element, true);
    return NULL;
}


XSchemaComplexContent *XSchemaElement::readComplexContentChild(XSDLoadContext *loadContext, QDomElement &element)
{
    XSchemaComplexContent *newObject = NULL ;
    QString namespaceURI = _root->namespaceURI();
    int nodi = element.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element.childNodes().item(i) ;
        if(childNode.isElement()) {
            QDomElement childElement = childNode.toElement();
            QString localName = childElement.localName();
            //printf("%s\n", localName.toLatin1().data());
            if(childElement.namespaceURI() == namespaceURI) {
                if(localName == IO_XSD_COMPLEXCONTENT_RESTRICTION) {
                    newObject = new XSchemaComplexContentRestriction(this, _root);
                    newObject->readBaseProperties(loadContext, element);
                    _elementCategory = EES_COMPLEX_DERIVED ;
                    return newObject ;
                }
                if(localName == IO_XSD_COMPLEXCONTENT_EXTENSION) {
                    newObject = new XSchemaComplexContentExtension(this, _root);
                    newObject->readBaseProperties(loadContext, element);
                    _elementCategory = EES_COMPLEX_DERIVED ;
                    return newObject ;
                }
            }
        }
    }
    //fflush(stdout);
    raiseError(loadContext, this, element, true);
    return NULL ;
}
//usare unbounded per il display

bool XSchemaElement::hasAReference()
{
    return !_ref.isEmpty();
}

int XSchemaElement::minOccurrences()
{
    return _minOccurs.occurrences;
}

int XSchemaElement::maxOccurrences()
{
    return _maxOccurs.occurrences;
}

void XSchemaElement::reparent(XSchemaObject *newParent)
{
    XSchemaObject::reparent(newParent);
    foreach(XSchemaObject * child, attributes()) {
        child->reparentRoot(_root);
    }
    if(NULL != _innerAnnotation) {
        _innerAnnotation->reparentRoot(_root);
    }

}

/**
 * @brief xmlRepresenatation
 * @return a xml string representation of the element
 */

QString XSchemaElement::xmlRepresentation()
{
    XSchemaXmlRepresentationInfo info;
    XSchemaElement *elm = NULL ;
    switch(category()) {
    case EES_EMPTY:
        elm = resolveType(this);
        break;
    case EES_REFERENCE:
        elm = resolveType(resolveReference(this));
        break;
    case EES_SIMPLETYPE_ONLY:
    case EES_SIMPLETYPE_WITHATTRIBUTES:
    case EES_COMPLEX_DERIVED:
    case EES_COMPLEX_DEFINITION:
        elm = resolveType(this);
        break;
    default:
        //
        //Utils::error(QString("cat %1").arg(category()));
        break;
    }
    info.tag = localName(nameOrReference());
    QString result ;
    if(NULL != elm) {
        XSchemaAttributesCollection attributesCollection;
        elm->collectAttributes(attributesCollection);
        info.setAttributes(attributesCollection);
        result = info.buildXmlRepresentation();
    }
    return result;
}

XSchemaAttributesCollection *XSchemaElement::attributesRepresentation()
{
    XSchemaAttributesCollection *result = new XSchemaAttributesCollection();
    XSchemaXmlRepresentationInfo info;
    XSchemaElement *elm = NULL ;
    switch(category()) {
    case EES_EMPTY:
        elm = resolveType(this);
        break;
    case EES_REFERENCE:
        elm = resolveType(resolveReference(this));
        break;
    case EES_SIMPLETYPE_ONLY:
    case EES_SIMPLETYPE_WITHATTRIBUTES:
    case EES_COMPLEX_DERIVED:
    case EES_COMPLEX_DEFINITION:
        elm = resolveType(this);
        break;
    default:
        //
        //Utils::error(QString("cat %1").arg(category()));
        break;
    }
    info.tag = localName(nameOrReference());
    if(NULL != elm) {
        elm->collectAttributes(*result);
    }
    return result;
}

XSchemaElement *XSchemaElement::resolveReference(XSchemaElement *base)
{
    XSchemaElement *result = base;
    while((NULL != result) && (result->category() == EES_REFERENCE)) {
        result = _root->schema()->topLevelElement(result->ref());
    }
    return result;
}

XSchemaElement *XSchemaElement::resolveType(XSchemaElement *base)
{
    if(NULL == base) {
        return NULL ;
    }
    if(base->isRestriction()) {
        return base;
    } else if(base->isExtension()) {
        return base;
    } else if(base->isSimpleExtension()) {
        return base ;
    } else if(base->isSimpleExtension()) {
        return base ;
    } else if(base->xsdType().isEmpty()) {
        return base ;
    } else {
        return _root->schema()->topLevelType(base->xsdType());
    }
    return NULL ;
}


XSchemaElement *XSchemaElement::resolveReference(XSchemaFindReferenceContext &context, XSchemaElement *base)
{
    XSchemaElement *result = base;
    if((result->category() == EES_REFERENCE)) {
        result = _root->schema()->topLevelElement(context, base->ref());
        context.setResolved(result);
    }

    return result;
}

XSchemaElement *XSchemaElement::resolveType(XSchemaFindReferenceContext &context, XSchemaElement *base)
{
    if(NULL == base) {
        context.setResolved(NULL);
    } else if(base->isRestriction()) {
        context.setResolved(base);
    } else if(base->isExtension()) {
        context.setResolved(base);
    } else if(base->isSimpleExtension()) {
        context.setResolved(base);
    } else if(base->isSimpleExtension()) {
        context.setResolved(base);
    } else if(base->xsdType().isEmpty()) {
        context.setResolved(base);
    } else {
        context.setResolved(_root->schema()->topLevelType(context, base->xsdType()));
    }
    return context.resolved();
}

/**
 * @brief XSchemaElement::collectAttributes
 * @param attributesCollection
 * @param recursive
 * @return  true if all ok.
 */
bool XSchemaElement::collectAttributes(XSchemaAttributesCollection & attributesCollection)
{
    switch(category()) {
    case EES_EMPTY:
    case EES_SIMPLETYPE_ONLY:
        collectAttributeOfSimpleDerived(attributesCollection);
        break;
    case EES_REFERENCE:
        // we should never go here
        return false;
    case EES_SIMPLETYPE_WITHATTRIBUTES:
    case EES_COMPLEX_DEFINITION:
        collectAttributeOfSimpleDerived(attributesCollection);
        break;
    case EES_COMPLEX_DERIVED:
        collectAttributeOfComplexDerived(attributesCollection);
        break;
    default:
        return false ;
    }
    collectAttributesCollection(attributesCollection);
    return true;
}

XSchemaComplexContentExtension *XSchemaElement::getExtension()
{
    if(_children.size() > 0) {
        XSchemaObject *child = _children.at(0);
        if(child->getType() == SchemaTypeComplexContentExtension) {
            return static_cast<XSchemaComplexContentExtension *>(child);
        }
    }
    return NULL ;
}

XSchemaComplexContentRestriction *XSchemaElement::getRestriction()
{
    if(_children.size() > 0) {
        XSchemaObject *child = _children.at(0);
        if(child->getType() == SchemaTypeComplexContentRestriction) {
            return static_cast<XSchemaComplexContentRestriction *>(child);
        }
    }
    return NULL ;
}

XSchemaSimpleContentExtension *XSchemaElement::getSimpleContentExtension()
{
    if(_children.size() > 0) {
        XSchemaObject *child = _children.at(0);
        if(child->getType() == SchemaTypeSimpleContentExtension) {
            return static_cast<XSchemaSimpleContentExtension *>(child);
        }
    }
    return NULL ;
}

XSchemaSimpleContentRestriction *XSchemaElement::getSimpleContentRestriction()
{
    if(_children.size() > 0) {
        XSchemaObject *child = _children.at(0);
        if(child->getType() == SchemaTypeSimpleContentRestriction) {
            return static_cast<XSchemaSimpleContentRestriction *>(child);
        }
    }
    return NULL ;
}

XSchemaSimpleTypeRestriction *XSchemaElement::getSimpleTypeRestriction()
{
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeSimpleTypeRestriction) {
            return (XSchemaSimpleTypeRestriction*) child ;
        }
    }
    return NULL ;
}

XSchemaSimpleTypeList *XSchemaElement::getSimpleTypeList()
{
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeSimpleTypeList) {
            return static_cast<XSchemaSimpleTypeList*>(child);
        }
    }
    return NULL ;
}

XSchemaSimpleTypeUnion *XSchemaElement::getSimpleTypeUnion()
{
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeSimpleTypeUnion) {
            return static_cast<XSchemaSimpleTypeUnion*>(child);
        }
    }
    return NULL ;
}

XSchemaElement *XSchemaElement::getReferencedType()
{
    XSchemaElement *baseTypeOrElement = _root->schema()->topLevelType(xsdType());
    return baseTypeOrElement ;
}

XSchemaElement *XSchemaElement::getReferencedElement()
{
    Utils::TODO_NEXT_RELEASE("riferimento qualificato?");
    XSchemaElement *baseTypeOrElement = _root->schema()->topLevelElement(referencedObjectName());
    return baseTypeOrElement;
}

void XSchemaElement::collectAttributeOfComplexDerived(XSchemaAttributesCollection & attributesCollection)
{
    XSchemaElement *baseTypeOrElement ;
    if(!hasAReference()) {
        baseTypeOrElement = this ;
    } else {
        if(isTypeOrElement()) {
            baseTypeOrElement = getReferencedElement();
        } else {
            baseTypeOrElement = getReferencedType();
        }
    }
    if(NULL != baseTypeOrElement) {
        baseTypeOrElement->collectAttributesCollection(attributesCollection);
    }
    XSchemaComplexContentRestriction *restriction = baseTypeOrElement->getRestriction();
    if(NULL != restriction) {
        restriction->collectAttributes(attributesCollection);
    } else {
        XSchemaComplexContentExtension *extension = baseTypeOrElement->getExtension();
        if(NULL != extension) {
            extension->collectAttributes(attributesCollection);
            XSchemaElement *extType = extension->getBaseType();
            if(NULL != extType) {
                extType->collectAttributes(attributesCollection);
            }
        }
    }
}

void XSchemaElement::collectAttributeOfSimpleDerived(XSchemaAttributesCollection & attributesCollection)
{
    XSchemaElement *baseTypeOrElement ;
    if(!hasAReference()) {
        baseTypeOrElement = this ;
    } else {
        if(isTypeOrElement()) {
            baseTypeOrElement = getReferencedElement();
        } else {
            baseTypeOrElement = getReferencedType();
        }
    }
    if(NULL != baseTypeOrElement) {
        baseTypeOrElement->collectAttributesCollection(attributesCollection);
    }
    XSchemaSimpleContentRestriction *restriction = baseTypeOrElement->getSimpleContentRestriction();
    if(NULL != restriction) {
        restriction->collectAttributes(attributesCollection);
    } else {
        XSchemaSimpleContentExtension *extension = baseTypeOrElement->getSimpleContentExtension();
        if(NULL != extension) {
            extension->collectAttributes(attributesCollection);
            XSchemaElement *extType = extension->getBaseType();
            if(NULL != extType) {
                extType->collectAttributes(attributesCollection);
            }
        }
    }
}

void XSchemaElement::collectAttributesCollection(XSchemaAttributesCollection & attributesCollection)
{
    foreach(XSchemaObject * child, _attributes) {
        if(child->getType() == SchemaTypeAttribute) {
            XSchemaAttribute *aChild = static_cast<XSchemaAttribute*>(child);
            addAttributeToCollection(aChild->name(), attributesCollection, aChild);
        } else if(child->getType() == SchemaTypeAttributeGroup) {
            XSchemaAttributeGroup *aChild = static_cast<XSchemaAttributeGroup*>(child);
            addAttributeGroupToCollection(attributesCollection, aChild);
        }
    }
}


bool XSchemaElement::isReferencingItself()
{
    if(referencedObjectType() != XRT_ELEMENT) {
        return false;
    }
    const QString &refName = ref();
    if((NULL == _root) || refName.isEmpty()) {
        return false;
    }
    XSchemaElement *topLevelElement = _root->schema()->topLevelElement(refName);
    if(NULL == topLevelElement) {
        return false;
    }
    // look for parent in the schema
    XSchemaObject *theParent = xsdParent();
    while(theParent != NULL) {
        if(theParent == topLevelElement) {
            return true;
        }
        theParent = theParent->xsdParent();
    }
    return false;
}


void XSchemaElement::raiseErrorTypesME(XSDLoadContext *loadContext, QDomElement &node)
{
    raiseError(loadContext, XSD_LOAD_ERROR_ELEMENT_SIMPLE_AND_COMPLEX_TYPE, this, node, tr("Element: type and either <simpleType> or <complexType> are mutually exclusive."));
}




/*
 refer to: 3.3.3 Constraints on XML Representations of Element Declarations XML Schema Part 1: Structures Second Edition.html#cvc-elt
 */

void XSchemaElement::validateAfterRead(XSDLoadContext *loadContext, QDomElement &node, void *context)
{
    // all must be true:
    //1 default and fixed must not both be present.
    if(isTypeOrElement()) {
        validateAfterReadType(loadContext, node, context);
    } else {
        validateAfterReadElement(loadContext, node, context);
    }
}

void XSchemaElement::validateAfterReadType(XSDLoadContext *loadContext, QDomElement &node, void * /*context*/)
{
    // parent of item is not <schema>
    if(_ref_used) {
        raiseError(loadContext, XSD_LOAD_ERROR_TYPE_TOP_REF_SET, this, node, tr("Type definition: reference not allowed."));
    }
    if(isTopLevel() || isInRedefine()) {
        if(!_name_used) {
            raiseError(loadContext, XSD_LOAD_ERROR_TYPE_TOP_NAME_NOT_SET, this, node, tr("Type definition: name not set."));
        }
    } else {
        if(_name_used) {
            raiseError(loadContext, XSD_LOAD_ERROR_TYPE_AN_NAME_SET, this, node, tr("Type definition: name set, not allowed."));
        }
    }
}

void XSchemaElement::validateAfterReadElement(XSDLoadContext *loadContext, QDomElement &node, void * /*context*/)
{
    // all must be true:
    //1 default and fixed must not both be present.
    if(_defaultValue_used && _fixed_used) {
        raiseError(loadContext, XSD_LOAD_ERROR_ELEMENT_DEFAULT_AND_FIXED, this, node, tr("Element: 'fixed' and 'use' must not both be present."));
    }
    // parent of item is not <schema>
    if(!isTopLevel()) {
        //2.1 One of ref or name must be present, but not both.
        if((!_ref_used && !_name_used) || (_ref_used && _name_used)) {
            raiseError(loadContext, XSD_LOAD_ERROR_ELEMENT_NAME_AND_REF, this, node, tr("Element: One of ref or name must be present, but not both."));
        }
        //2.2 If ref is present, then all of <complexType>, <simpleType>, <key>, <keyref>, <unique>, nillable, default, fixed, form, block and type must be absent, i.e. only minOccurs, maxOccurs, id are allowed in addition to ref, along with <annotation>.
        if(_ref_used) {
            if(_form_used || _xsdType_used || isSimpleType() || isComplexType() || hasKey()  || hasKeyRef() || hasUnique() || _defaultValue_used || _block_used) {
                raiseError(loadContext, XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE, this, node, tr("Element: If ref is present, then all of <simpleType>, form and type must be absent."));
            }
        }
        // 3 type and either <simpleType> or <complexType> are mutually exclusive
        if(_xsdType_used && (isSimpleType() || isComplexType())) {
            raiseErrorTypesME(loadContext, node);
        }
    } else {
        Utils::TODO_NEXT_RELEASE("finire");
        /* TODO: implement Validation Rule: Element Locally Valid (Type)
        // element locally valid
        //3.3.4 2 Its {abstract} must be false.
        if( _abstract_used ) {
            raiseError(loadContext, XSD_LOAD_ERROR_ELEMENT_ABSTRACT, this, node, tr("Element: Its {abstract} must be false."));
        }
        TODO:
            if( XBool::XBOOL_FALSE == _nillable ) {
                // 3.3.4 3.1 If {nillable} is false, then ...
                if( exists nil con namespace ) {
                    raiseError(loadContext, XSD_LOAD_ERROR_ELEMENT_NILLBALE_NIL, this, node, tr("Element: {nillable} is false and nil apecified."));
                }
            }
            if( XBool::XBOOL_TRUE == _nillable ) {
                if( exitst nil e vale true ) {
                    if( hasChildren || _fixed_used ) {
                        raiseError(loadContext, XSD_LOAD_ERROR_ELEMENT_NILLBALE_FIXED, this, node, tr("Element: {nillable} is true and children or fixed."));
                    }
                }
            }
        }
        */
    }
}

void XSchemaElement::collectAllAttributes(XSchemaInquiryContext &context, QList<XSchemaObject*> & baseAttributes)
{
    XSchemaElement *realElement = NULL ;
    if(isTypeOrElement()) {
        realElement = this ;
    } else {
        if(XRT_ELEMENT == referencedObjectType()) {
            XSchemaElement *referencedElement = getReferencedElement();
            if(NULL == referencedElement) {
                // no way, unable to reference something.
                return ;
            }
            realElement = referencedElement ;
        } else {
            const QString type = typeString();
            if(!type.isEmpty()) {
                realElement = resolveType(this);
            }
            if(NULL == realElement) {
                realElement = this ;
            }
        }
    }
    QList<XSchemaObject*> baseElements;
    bool isEnrolledSub = false;
    context.setEnrollFlag(&isEnrolledSub);
    findBaseObjects(context, baseElements, baseAttributes);
}

bool XSchemaElement::getTypeInfoAndRestrictions(XTypeQueryInfo &typeInfo)
{
    qualifiedTypeOrElementName(typeInfo) ;
    switch(category()) {
    case EES_EMPTY: {
        XSchemaElement *baseTypeOrElement = finalTypeOrElement() ;
        if(NULL != baseTypeOrElement) {
            if(baseTypeOrElement->isComplexType()) {
                collectTypeInfoOfComplexDerived(typeInfo);
                return true;
            }
        }
    }
    collectTypeInfoOfSimpleType(typeInfo);
    return true;
    break;
    case EES_SIMPLETYPE_ONLY:
        collectTypeInfoOfSimpleType(typeInfo);
        return true ;
        break;
    case EES_REFERENCE:
        // we should never go here
        return false;
    case EES_SIMPLETYPE_WITHATTRIBUTES:
    case EES_COMPLEX_DEFINITION:
        collectTypeInfoOfComplexDerived(typeInfo);
        return true ;
        break;
    case EES_COMPLEX_DERIVED:
        collectTypeInfoOfComplexDerived(typeInfo);
        return true ;
        break;
    default:
        return false ;
    }
    return true;
}

XSchemaElement *XSchemaElement::finalTypeOrElement()
{
    XSchemaElement *baseTypeOrElement = this ;
    if(!isTypeOrElement()) {
        if(hasAReference()) {
            baseTypeOrElement = getReferencedElement();
        }
        if(!baseTypeOrElement->xsdType().isEmpty()) {
            baseTypeOrElement = baseTypeOrElement->getReferencedType();
        }
    }
    return baseTypeOrElement ;
}

void XSchemaElement::qualifiedTypeOrElementName(XTypeQueryInfo &typeInfo)
{
    XSchemaElement *baseTypeOrElement = this ;
    if(!isTypeOrElement()) {
        if(hasAReference()) {
            typeInfo.setQualifiedTypeName(ref());
        }
        if(!baseTypeOrElement->xsdType().isEmpty()) {
            typeInfo.setQualifiedTypeName(baseTypeOrElement->xsdType());
        }
    }
}

void XSchemaElement::collectTypeInfoOfSimpleType(XTypeQueryInfo &typeInfo)
{
    XSchemaElement *baseTypeOrElement = finalTypeOrElement() ;
    if(NULL != baseTypeOrElement) {
        XSchemaSimpleTypeRestriction *restriction = baseTypeOrElement->getSimpleTypeRestriction();
        if(NULL != restriction) {
            typeInfo.setSimpleRestriction(restriction->base());
            QStringList enums ;
            restriction->addEnumsToListIfAny(enums);
            if(!enums.isEmpty()) {
                typeInfo.setEnums(enums);
            }
        } else {
            XSchemaSimpleTypeList *aList = baseTypeOrElement->getSimpleTypeList();
            if(NULL != aList) {
                typeInfo.setIsSimpleTypeList(true);
                typeInfo.setListValue(aList->itemType());
            } else {
                XSchemaSimpleTypeUnion *aUnion = baseTypeOrElement->getSimpleTypeUnion();
                if(NULL != aUnion) {
                    typeInfo.setIsSimpleTypeUnion(true);
                    typeInfo.setUnionValue(aUnion->memberTypes());
                }
            }
        }
    }
}

void XSchemaElement::collectTypeInfoOfSimpleDerived(XTypeQueryInfo &typeInfo)
{
    XSchemaElement *baseTypeOrElement = finalTypeOrElement() ;
    if(NULL != baseTypeOrElement) {
        XSchemaSimpleContentRestriction *restriction = baseTypeOrElement->getSimpleContentRestriction();
        if(NULL != restriction) {
            typeInfo.setSimpleRestriction(restriction->getBaseType()->name());
            QStringList enums ;
            restriction->addEnumsToListIfAny(enums);
            if(!enums.isEmpty()) {
                typeInfo.setEnums(enums);
            }
        } else {
            XSchemaSimpleContentExtension *extension = baseTypeOrElement->getSimpleContentExtension();
            if(NULL != extension) {
                typeInfo.setSimpleExtension(extension->getBaseType()->name());
            }
        }
    }
}

void XSchemaElement::collectTypeInfoOfComplexDerived(XTypeQueryInfo &typeInfo)
{
    XSchemaElement *baseTypeOrElement = finalTypeOrElement();
    if(NULL != baseTypeOrElement) {
        typeInfo.setName(baseTypeOrElement->name());
        XSchemaComplexContentRestriction *restriction = baseTypeOrElement->getRestriction();
        if(NULL != restriction) {
            typeInfo.setComplexRestriction(restriction->getBaseType()->name());
        } else {
            XSchemaComplexContentExtension *extension = baseTypeOrElement->getExtension();
            if(NULL != extension) {
                typeInfo.setComplexExtension(extension->getBaseType()->name());
            }
        }
    }
}

