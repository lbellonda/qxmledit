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

#define PIXMAP_DEFAULT_XSD_OBJECT   ":/xsdimages/base"

const QString XSchemaObject::_xsdXML("http://www.w3.org/XML/1998/namespace");

//--------------------------

XEnums::XEnums()
{
}

XEnums::~XEnums()
{
}

XEnums::XBool XEnums::parseBool(const QString input)
{
    // cfr: 3.2.2.1 Lexical representation
    if(input.isEmpty()) {
        return XBOOL_UNSET;
    }
    if(input == "true") {
        return XBOOL_TRUE ;
    }
    if(input == "1") {
        return XBOOL_TRUE ;
    }
    if(input == "false") {
        return XBOOL_FALSE ;
    }
    if(input == "0") {
        return XBOOL_FALSE ;
    }
    XsdError(QString(tr("Invalid boolean value:%1").arg(input)));
    return XBOOL_UNSET;
}

QString XEnums::boolToString(const XBool value)
{
    // cfr: 3.2.2.2 Canonical representation
    switch(value) {
    case XBOOL_UNSET:
        return "";
    case XBOOL_TRUE:
        return "true";
    case XBOOL_FALSE:
        return "false";
    }
    XsdError(QString(tr("Invalid boolean value:%1").arg(value)));
    return "";
}

/* TODO: delete?
XBool::XBool(const bool newValue)
{
    setValue(newValue);
}

XBool::~XBool()
{
}

bool XBool::value()
{
    return _value ;
}

void XBool::setValue(const bool newValue)
{
    _value = newValue ;
}

*/

//-----------------------------------------------------
RestrictionFacets::RestrictionFacets()
{
    _isExaminedDiff = false ;
}

QString RestrictionFacets::getFacet(const QString &name, const QString &value)
{
    if(!value.isEmpty()) {
        return QString("\n%1 = %2").arg(name).arg(value);
    } else {
        return "";
    }
}

QString RestrictionFacets::toString()
{
    QString res ;

    res += getFacet(IO_XSD_MINEXCLUSIVE, _minExclusive);
    res += getFacet(IO_XSD_MININCLUSIVE, _minInclusive);
    res += getFacet(IO_XSD_MAXEXCLUSIVE, _maxExclusive);
    res += getFacet(IO_XSD_MAXINCLUSIVE, _maxInclusive);
    res += getFacet(IO_XSD_TOTALDIGITS, _totalDigits);
    res += getFacet(IO_XSD_FRACTIONDIGITS, _fractionDigits);
    res += getFacet(IO_XSD_LENGTH, _length);
    res += getFacet(IO_XSD_MINLENGTH, _minLength);
    res += getFacet(IO_XSD_MAXLENGTH, _maxLength);
    QString bullet = QString::fromUtf8("\n \u2022 \"%1\"");
    if(!_enumeration.isEmpty()) {
        res += "\nEnumeration:";
        foreach(QString restrictionValue, _enumeration) {
            res += QString(bullet).arg(restrictionValue);
        }
    }
    // TODO: WRONG!!!!
    res += getFacet(IO_XSD_WHITESPACE, _whiteSpace);
    res += getFacet(IO_XSD_PATTERN, _pattern);
    return res;
}

QString RestrictionFacets::toStringDiffHtml()
{
    QString res ;
    res += "<html><body>" ;
    res += Utils::escapeHTML(getFacet(IO_XSD_MINEXCLUSIVE, _minExclusive));
    res += Utils::escapeHTML(getFacet(IO_XSD_MININCLUSIVE, _minInclusive));
    res += Utils::escapeHTML(getFacet(IO_XSD_MAXEXCLUSIVE, _maxExclusive));
    res += Utils::escapeHTML(getFacet(IO_XSD_MAXINCLUSIVE, _maxInclusive));
    res += Utils::escapeHTML(getFacet(IO_XSD_TOTALDIGITS, _totalDigits));
    res += Utils::escapeHTML(getFacet(IO_XSD_FRACTIONDIGITS, _fractionDigits));
    res += Utils::escapeHTML(getFacet(IO_XSD_LENGTH, _length));
    res += Utils::escapeHTML(getFacet(IO_XSD_MINLENGTH, _minLength));
    res += Utils::escapeHTML(getFacet(IO_XSD_MAXLENGTH, _maxLength));

    if(_isExaminedDiff) {
        if(!_enumerationAdded.isEmpty() || !_enumerationEquals.isEmpty() || !_enumerationDeleted.isEmpty()) {
            res += "<br/>Enumeration:";
            foreach(QString restrictionValue, _enumerationAdded) {
                res += QString("<br/>&#x2022;<span style=\"color: #4C4\"> \"%1\" (Added)</span>").arg(Utils::escapeHTML(restrictionValue));
            }
            foreach(QString restrictionValue, _enumerationEquals) {
                res += QString("<br/>&#x2022;<span style=\"color: #888\">  \"%1\"</span>").arg(Utils::escapeHTML(restrictionValue));
            }
            foreach(QString restrictionValue, _enumerationDeleted) {
                res += QString("<br/>&#x2022;<span style=\"color: #F44\"> \"%1\" (Deleted)</span>").arg(Utils::escapeHTML(restrictionValue));
            }
        }
    } else {
        if(!_enumeration.isEmpty()) {
            res += "<br/>Enumeration:";
            foreach(QString restrictionValue, _enumeration) {
                res += QString("<br/>&#x2022;<span style=\"color: #000\"> \"%1\"</span>").arg(Utils::escapeHTML(restrictionValue));
            }
        }
    }
    // TODO: WRONG!!!!
    res += Utils::escapeHTML(getFacet(IO_XSD_WHITESPACE, _whiteSpace));
    res += Utils::escapeHTML(getFacet(IO_XSD_PATTERN, _pattern));
    res += "</body></html>" ;
    return res;
}

// TODO: sostituire
bool RestrictionFacets::scanForFacets(const QString &name, const QString &value)
{
    if(name == IO_XSD_MINEXCLUSIVE) {
        _minExclusive = value;
        return true;
    } else if(name == IO_XSD_MININCLUSIVE) {
        _minInclusive = value;
        return true;
    } else if(name == IO_XSD_MAXEXCLUSIVE) {
        _maxExclusive = value;
        return true;
    } else if(name == IO_XSD_MAXINCLUSIVE) {
        _maxInclusive = value;
        return true;
    } else if(name == IO_XSD_TOTALDIGITS) {
        _totalDigits = value;
        return true;
    } else if(name == IO_XSD_FRACTIONDIGITS) {
        _fractionDigits = value;
        return true;
    } else if(name == IO_XSD_LENGTH) {
        _length = value;
        return true;
    } else if(name == IO_XSD_MINLENGTH) {
        _minLength = value;
        return true;
    } else if(name == IO_XSD_MAXLENGTH) {
        _maxLength = value;
        return true;
    } else if(name == IO_XSD_ENUMERATION) {   // TODO: WRONG!!!!
        _enumeration.append(value);
        return true;
    } else if(name == IO_XSD_WHITESPACE) {  // TODO: WRONG!!!!
        _whiteSpace = value;
        return true;
    } else if(name == IO_XSD_PATTERN) {
        _pattern = value;
        return true;
    }
    return false;
}

void RestrictionFacets::addEnumsToList(QStringList &lst)
{
    foreach(QString restrictionValue, _enumeration) {
        lst << restrictionValue;
    }
}


//-----------------------------------------------------

int XSchemaObject::instances = 0 ;

XSchemaObject::XSchemaObject(XSchemaObject *newParent, XSchemaRoot *newRoot)
{
    instances ++;
    _instanceId = instances ;
    _isRedefinition = false ;
    _parent = newParent;
    _root = newRoot;
    _annotation = NULL;
    _compareStatus = XSDCompareState::COMPARE_NOT_EXAMINED;
    _name_used = false ;
}

XSchemaObject::~XSchemaObject()
{
    reset();
    _parent = NULL ;
    _root = NULL ;
}

bool XSchemaObject::isRedefinition() const
{
    return _isRedefinition;
}

void XSchemaObject::setIsRedefinition(bool value)
{
    _isRedefinition = value ;
}


QString XSchemaObject::name()
{
    return _name;
}

QString XSchemaObject::nameOrReference()
{
    return _name;
}

QString XSchemaObject::id()
{
    return _id;
}

XSchemaAnnotation *XSchemaObject::annotation()
{
    return _annotation ;
}


void XSchemaObject::deleteObject()
{
    if(NULL != _parent) {
        _parent->deleteChild(this);
    } else {
        reset();
        emit deleted(this);
        delete this;
    }
}

void XSchemaObject::deleteChild(XSchemaObject *theChild)
{
    // theChild must be valid
    int indexOfChild = _children.indexOf(theChild);
    if(indexOfChild >= 0) {
        _children.removeAt(indexOfChild);
        emit childRemoved(theChild);
        delete theChild;
    }
}


void XSchemaObject::setName(const QString &newName)
{
    if(_name != newName) {
        _name = newName ;
        emit nameChanged(_name);
    }
}

void XSchemaObject::setId(const QString &newId)
{
    if(_id != newId) {
        _id = newId ;
        emit idChanged(_id);
    }
}

/* why?
bool XSchemaObject::operator< ( const XSchemaObject & other ) const
{
    QString thisName = _name;
    QString otherName = other._name;
    if( thisName < otherName ) {
        return true ;
    }
    return false;
}
*/

QList<XSchemaObject*> &XSchemaObject::getChildren()
{
    return _children ;
}

void XSchemaObject::reset()
{
    foreach(XSchemaObject * object, _children) {
        emit childRemoved(object);
        if((object->xsdParent() != this) || (object->_root != _root)) {
            XsdError(tr("broken relation in objects"));
        }
        delete object;
    }
    _children.clear();
    _otherAttributes.clear();
    if(_annotation) {
        delete _annotation ;
        _annotation = NULL ;
    }
}

void XSchemaObject::reparent(XSchemaObject *newParent)
{
    _parent = newParent ;
    reparentRoot(newParent->_root);
}

void XSchemaObject::reparentRoot(XSchemaRoot *newRoot)
{
    _root = newRoot;
    foreach(XSchemaObject * child, getChildren()) {
        child->reparentRoot(_root);
    }
    if(NULL != _annotation) {
        _annotation->reparentRoot(_root);
    }
}

int XSchemaObject::getUUID()
{
    return _instanceId;
}

QString XSchemaObject::key()
{
    return _key ;
}

bool XSchemaObject::isSimpleType()
{
    return false ;
}

XSchemaRoot *XSchemaObject::root()
{
    return _root;
}

XSchemaObject* XSchemaObject::addChild(XSchemaObject *child)
{
    if(NULL != child) {
        _children.append(child);
    }
    return child ;
}


bool XSchemaObject::canAddElement()
{
    return false ;
}

bool XSchemaObject::canAddAttribute()
{
    return false ;
}

bool XSchemaObject::canAddSequence()
{
    return false ;
}

bool XSchemaObject::canAddChoice()
{
    return false ;
}

bool XSchemaObject::canAddRestriction()
{
    return false ;
}

bool XSchemaObject::canAddExtension()
{
    return false;
}

bool XSchemaObject::canAddUnion()
{
    return false;
}

bool XSchemaObject::canAddList()
{
    return false;
}

bool XSchemaObject::canAddChild(const ESchemaType /*newType*/)
{
    return false;
}

XSchemaElement *XSchemaObject::addElement()
{
    return NULL ;
}

XSchemaAttribute *XSchemaObject::addAttribute()
{
    return NULL ;
}

XSchemaChoice* XSchemaObject::addChoice()
{
    return NULL ;
}

XSchemaSequence* XSchemaObject::addSequence()
{
    return NULL ;
}

XSchemaObject* XSchemaObject::addRestriction()
{
    return NULL ;
}

XSchemaObject* XSchemaObject::addExtension()
{
    return NULL ;
}

XSchemaSimpleTypeUnion* XSchemaObject::addUnion()
{
    return realAddUnion();
}

XSchemaSimpleTypeList* XSchemaObject::addList()
{
    return realAddList() ;
}

XSchemaElement* XSchemaObject::realAddElement()
{
    XSchemaElement* element = new XSchemaElement(this, _root);
    XSchemaObject::addChild(element);
    emit childAdded(element);
    return element ;
}


XSchemaChoice* XSchemaObject::realAddChoice()
{
    XSchemaChoice* choice = new XSchemaChoice(this, _root);
    XSchemaObject::addChild(choice);
    emit childAdded(choice);
    return choice;
}

XSchemaSequence* XSchemaObject::realAddSequence()
{
    XSchemaSequence* sequence = new XSchemaSequence(this, _root);
    XSchemaObject::addChild(sequence);
    emit childAdded(sequence);
    return sequence;
}

XSchemaAttribute* XSchemaObject::realAddAttribute()
{
    XSchemaAttribute* attribute = new XSchemaAttribute(this, _root);
    XSchemaObject::addChild(attribute);
    emit childAdded(attribute);
    return attribute;
}

XSchemaSimpleTypeUnion* XSchemaObject::realAddUnion()
{
    XSchemaSimpleTypeUnion* xunion = new XSchemaSimpleTypeUnion(this, _root);
    XSchemaObject::addChild(xunion);
    emit childAdded(xunion);
    return xunion;
}

XSchemaSimpleTypeList* XSchemaObject::realAddList()
{
    XSchemaSimpleTypeList* list = new XSchemaSimpleTypeList(this, _root);
    XSchemaObject::addChild(list);
    emit childAdded(list);
    return list;
}

XSchemaSimpleTypeRestriction* XSchemaObject::realAddSimpleTypeRestriction()
{
    XSchemaSimpleTypeRestriction* child = new XSchemaSimpleTypeRestriction(this, _root);
    XSchemaObject::addChild(child);
    emit childAdded(child);
    return child ;
}

XReferenceType XSchemaObject::referencedObjectType()
{
    return XRT_NONE;
}

QString XSchemaObject::referencedObjectName()
{
    return "";
}

XSchemaObject *XSchemaObject::xsdParent()
{
    return _parent ;
}

bool XSchemaObject::generateInnerDom(QDomDocument &document, QDomNode &parent)
{
    foreach(XSchemaObject * child, _children) {
        if(!child->generateDom(document, parent)) {
            return false;
        }
    }
    return true ;
}

QString XSchemaObject::description()
{
    return tagName() + (_name.isEmpty() ? "" : " (" + _name + ")") ;
}

QPixmap XSchemaObject::icon()
{
    // TODO: use cache
    QPixmap pixmap ;
    pixmap.load(PIXMAP_DEFAULT_XSD_OBJECT);
    return pixmap ;
}

QColor XSchemaObject::color()
{
    return QColor::fromRgbF(.9, .9, .8, 1) ;
}

bool XSchemaObject::containsSimpleType()
{
    foreach(XSchemaObject * child, _children) {
        switch(child->getType()) {
        case SchemaTypeSimpleTypeRestriction:
        case SchemaTypeSimpleTypeUnion:
        case SchemaTypeSimpleTypeList:
            return true;
        default:
            break;
        }
    }
    return false;
}

QString XSchemaObject::fromFinalTypeToString(const EFinalType fType)
{
    switch(fType) {
    default:
        return "?";
    case FINALTYPE_NONE: return "" ;
    case FINALTYPE_ALL: return FINALTYPE_ALL_STRING;
    case FINALTYPE_RESTRICTION: return FINALTYPE_RESTRICTION_STRING;
    case FINALTYPE_EXTENSION: return FINALTYPE_EXTENSION_STRING;
    case FINALTYPE_LIST: return FINALTYPE_LIST_STRING;
    case FINALTYPE_UNION: return FINALTYPE_UNION_STRING;
    case FINALTYPE_SUBSTITUTION: return FINALTYPE_SUBSTITUTION_STRING;

    }
}

QString XSchemaObject::finalListToString(QList<EFinalType> &types)
{
    QString result ;
    bool isFirst = true;
    if(!types.isEmpty()) {
        foreach(EFinalType type, types) {
            if(type == FINALTYPE_ALL) {
                return FINALTYPE_ALL_STRING;
            } else {
                if(!isFirst) {
                    result += " ";
                }
                result += fromFinalTypeToString(type);
            }
            isFirst = false;
        }
    }
    return result;
}

bool XSchemaObject::findBaseObjects(XSchemaInquiryContext & /*context*/, QList<XSchemaObject*> & /*baseElements*/, QList<XSchemaObject*> & /*baseAttributes*/)
{
    // do nothing
    return true;
}

XSchemaObject::EQualified XSchemaObject::decodeForm(const QString &value)
{
    if(value == "qualified") {
        return XSchemaObject::EQ_QUALIFIED ;
    } else if(value == "unqualified") {
        return XSchemaObject::EQ_UNQUALIFIED ;
    } else {
        return XSchemaObject::EQ_MISSING ;
    }
}

QList<XSchemaObject::EFinalType> XSchemaObject::decodeFinalListToString(const QString &encodedData)
{
    QList<EFinalType> result ;
    if(! encodedData.isEmpty()) {
        QStringList options = encodedData.split(" ");
        foreach(QString value, options) {
            if(value == FINALTYPE_ALL_STRING) {
                result.append(FINALTYPE_ALL);
            } else if(value == FINALTYPE_RESTRICTION_STRING) {
                result.append(FINALTYPE_RESTRICTION);
            } else if(value == FINALTYPE_EXTENSION_STRING) {
                result.append(FINALTYPE_EXTENSION);
            } else if(value == FINALTYPE_LIST_STRING) {
                result.append(FINALTYPE_LIST);
            } else if(value == FINALTYPE_UNION_STRING) {
                result.append(FINALTYPE_UNION);
            } else if(value == FINALTYPE_SUBSTITUTION_STRING) {
                result.append(FINALTYPE_SUBSTITUTION);
            } else {
                XsdError(tr("Invalid final/block value: %1").arg(encodedData));
            }
        }
    }
    return result ;
}

void XSchemaObject::addAttrNotEmpty(QDomElement element, const QString &attributeName, const QString &value)
{
    if(!value.isEmpty()) {
        element.setAttribute(attributeName, value);
    }
}

void XSchemaObject::addElementIfNotEmpty(QDomElement &element, const QString &elementTag, const QString &value)
{
    if(!value.isEmpty()) {
        QDomDocument document = element.ownerDocument();
        QDomElement innerElement = createElement(document, elementTag);
        QDomText textNode = document.createTextNode(value);
        innerElement.appendChild(textNode);
        element.appendChild(innerElement);
    }
}

void XSchemaObject::addFacetIfNotEmpty(QDomElement &element, const QString &elementTag, const QString &value)
{
    if(!value.isEmpty()) {
        QDomDocument document = element.ownerDocument();
        QDomElement innerElement = createElement(document, elementTag);
        innerElement.setAttribute(IO_FACET_ATTR_VALUE, value);
        element.appendChild(innerElement);
    }
}

QDomElement XSchemaObject::createElement(QDomDocument &document, const QString &tagName)
{
    return _root->createElementWithNamespace(document, tagName);
}

void XSchemaObject::addOtherAttributesToDom(QDomElement & element)
{
    foreach(QString otherKey, _otherAttributes.keys()) {
        element.setAttribute(otherKey, _otherAttributes.value(otherKey));
    }
}

bool XSchemaObject::readOtherAttributes(QDomAttr & attribute)
{
    if(_root->existsNamespace(attribute.prefix())) {
        _otherAttributes.insert(attribute.nodeName(), attribute.value());   //TODO check namespace
        return true;
    }
    return false;
}

void XSchemaObject::readHandleObject(XSDLoadContext *loadContext, QDomElement &element, XSchemaObject *newObject)
{
    if(NULL == newObject) {
        raiseErrorForObject(loadContext, element);
    }
    addChild(newObject);
    newObject->loadFromDom(loadContext, element);
}

void XSchemaObject::readHandleObject(XSDLoadContext *loadContext, QDomElement &element, XSchemaObject *parent, XSchemaObject *newObject)
{
    parent->readHandleObject(loadContext, element, newObject);
}

void XSchemaObject::loadFromDom(XSDLoadContext *loadContext, QDomElement &elementToExamine)
{
    XScanContext context ;
    scanDom(loadContext, elementToExamine, &context);
}

QString XSchemaObject::getLoadKey(QDomNode &elementToExamine)
{
    // make the key
    QString key ;
    QDomNode node = elementToExamine ;
    do {
        QDomNode parent = node.parentNode();
        QDomNodeList childNodes = parent.childNodes();
        int nodes = childNodes.size();
        for(int i = 0 ; i < nodes ; i ++) {
            QDomNode nd = childNodes.at(i);
            if(nd == node) {
                key = QString("*%1%2").arg(i).arg(key) ;
                break;
            }
        }
        node = parent ;
    } while(!node.isNull());
    return key;
}

void XSchemaObject::addObjectToLoadContext(XSDLoadContext *loadContext, QDomElement &elementToExamine)
{
    if(NULL != loadContext->inputKeyMap()) {
        QString key = getLoadKey(elementToExamine);
        loadContext->setKey(key, this);
        _key = key;
    }
}

bool XSchemaObject::scanDom(XSDLoadContext *loadContext, QDomElement &elementToExamine, void *context)
{
    addObjectToLoadContext(loadContext, elementToExamine);

    QDomNamedNodeMap attributes = elementToExamine.attributes();
    int numAttrs = attributes.length();
    for(int i = 0 ; i < numAttrs ; i++) {
        QDomNode node = attributes.item(i);
        QDomAttr attr = node.toAttr();
        scanForAttributes(loadContext, attr, context);
    } // for

    int nodi = elementToExamine.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = elementToExamine.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            scanForElements(loadContext, element, context);
        }
    }
    validateAfterRead(loadContext, elementToExamine, context);

    return true;
}

void XSchemaObject::scanForAttributes(XSDLoadContext * /*loadContext*/, QDomAttr & /*attribute*/, void * /*context*/)
{
}

void XSchemaObject::scanForElements(XSDLoadContext * /*loadContext*/, QDomElement & /*element*/, void * /*context*/)
{
}

void XSchemaObject::validateAfterRead(XSDLoadContext * /*loadContext*/, QDomElement & /*node*/, void * /*context*/)
{
}

void XSchemaObject::readHandleAnnotation(XSDLoadContext *loadContext, QDomElement &element)
{
    if(NULL != _annotation) {
        raiseError(loadContext, this, element, true);
    }
    _annotation = new XSchemaAnnotation(this, _root) ;
    if(NULL == _annotation) {
        raiseErrorForObject(loadContext, element);
    }
    _annotation->loadFromDom(loadContext, element);
}

void XSchemaObject::invalidObjectState(XSDLoadContext *loadContext, XSchemaObject *origin, QDomNode &node, const QString &fieldName)
{
    QString parentName ;
    QString className ;
    const QMetaObject *mo = origin->metaObject();
    if(NULL != mo)     {
        className = mo->className();
    }
    QDomNode parent = node.parentNode();
    if(!parent.isNull()) {
        parentName = parent.nodeName();
    }
    QString msg = tr("[%1] Element '%2' (contained in '%4' at line %5, position %6) with invalid field '%3'.")
                  .arg(className)
                  .arg(node.nodeName())
                  .arg(fieldName)
                  .arg(parentName)
                  .arg(node.lineNumber())
                  .arg(node.columnNumber());
    if(loadContext->isPolicyThrowError()) {
        throw new XsdException(msg);
    } else {
        loadContext->addError(XSD_LOAD_GENERICERROR, msg);
    }
}

void XSchemaObject::raiseError(XSDLoadContext * loadContext, XSchemaObject *origin, QDomNode &node, const bool isElement)
{
    QString parentName ;
    QString className ;
    const QMetaObject *mo = origin->metaObject();
    if(NULL != mo)     {
        className = mo->className();
    }
    QDomNode parent = node.parentNode();
    if(!parent.isNull()) {
        parentName = parent.nodeName();
    }
    QString msg;
    if(isElement) {
        msg = tr("[%5] Unexpected element '%1' contained in '%2' at line %3, position %4.")
              .arg(node.nodeName())
              .arg(parentName)
              .arg(node.lineNumber())
              .arg(node.columnNumber())
              .arg(className);
    } else {
        msg = tr("[%5] Unexpected attribute '%1' contained in '%2' at line %3, position %4.")
              .arg(node.nodeName())
              .arg(parentName)
              .arg(node.lineNumber())
              .arg(node.columnNumber())
              .arg(className);
    }
    if(loadContext->isPolicyThrowError()) {
        throw new XsdException(msg);
    } else {
        loadContext->addError(XSD_LOAD_GENERICERROR, msg);
    }
}

void XSchemaObject::raiseError(XSDLoadContext *loadContext, const EXSDLoadError errorCode, XSchemaObject *origin, QDomNode &node, const QString &message)
{
    QString parentName ;
    QString className ;
    const QMetaObject *mo = origin->metaObject();
    if(NULL != mo)     {
        className = mo->className();
    }
    QDomNode parent = node.parentNode();
    if(!parent.isNull()) {
        parentName = parent.nodeName();
    }
    QString msg = tr("[%5] %6 '%1' contained in '%2' at line %3, position %4.")
                  .arg(node.nodeName())
                  .arg(parentName)
                  .arg(node.lineNumber())
                  .arg(node.columnNumber())
                  .arg(className)
                  .arg(message);
    if(loadContext->isPolicyThrowError()) {
        throw new XsdException(msg);
    } else {
        loadContext->addError(errorCode, message);
    }
}

void XSchemaObject::raiseError(XSDLoadContext *loadContext, XSchemaObject *origin, QDomNode &node, const QString &message)
{
    raiseError(loadContext, XSD_LOAD_GENERICERROR, origin, node, message);
}

void XSchemaObject::raiseErrorForObject(XSDLoadContext *context, QDomNode & node)
{
    QString msg = tr("Unable to get new object of type: %1")
                  .arg(node.nodeName());

    if(context->isPolicyThrowError()) {
        throw new XsdException(msg);
    } else {
        context->addError(XSD_LOAD_GENERICERROR, msg);
    }
}

QStringList XSchemaObject::readFinalAttribute(const QString &value)
{
    QStringList result = value.split(" ");
    return result ;
}

XSchemaObject::EProcessContent XSchemaObject::decodeProcessContents(const QString &value)
{
    if(value == IO_ANY_ATTR_PROCESSCONTENTS_LAX) {
        return lax;
    }
    if(value == IO_ANY_ATTR_PROCESSCONTENTS_SKIP) {
        return skip;
    }
    if(value == IO_ANY_ATTR_PROCESSCONTENTS_STRICT) {
        return strict;
    }
    return none ;
}

QString XSchemaObject::processContentstoString(const EProcessContent pc)
{
    switch(pc) {
    case lax:
        return IO_ANY_ATTR_PROCESSCONTENTS_LAX;
    case skip:
        return IO_ANY_ATTR_PROCESSCONTENTS_SKIP ;
    case strict:
        return IO_ANY_ATTR_PROCESSCONTENTS_STRICT;
    default:
        return "" ;
    }
}

void XSchemaObject::scanForContentAnnotation(XSDLoadContext *loadContext, QDomElement &element, void * context)
{
    XScanContext *theContext = (XScanContext*) context;
    QString name = element.localName();
    if(element.namespaceURI() == _root->namespaceURI()) {
        if(name == IO_XSD_ANNOTATION) {
            if(theContext ->childFound) {
                raiseError(loadContext, this, element, true);
            }
            readHandleAnnotation(loadContext, element);
            theContext ->childFound = true ;
            return;
        }
    }
    raiseError(loadContext, this, element, true);
}

void XSchemaObject::generateFacets(QDomElement &node, RestrictionFacets &facets)
{
    // TODO: factor this (complex type, etc.)
    addFacetIfNotEmpty(node, IO_XSD_MINEXCLUSIVE, facets._minExclusive);
    addFacetIfNotEmpty(node, IO_XSD_MININCLUSIVE, facets._minInclusive);
    addFacetIfNotEmpty(node, IO_XSD_MAXEXCLUSIVE, facets._maxExclusive);
    addFacetIfNotEmpty(node, IO_XSD_MAXINCLUSIVE, facets._maxInclusive);
    addFacetIfNotEmpty(node, IO_XSD_TOTALDIGITS, facets._totalDigits);
    addFacetIfNotEmpty(node, IO_XSD_FRACTIONDIGITS, facets._fractionDigits);
    addFacetIfNotEmpty(node, IO_XSD_LENGTH, facets._length);
    addFacetIfNotEmpty(node, IO_XSD_MINLENGTH, facets._minLength);
    addFacetIfNotEmpty(node, IO_XSD_MAXLENGTH, facets._maxLength);
    foreach(QString restrictionValue, facets._enumeration) {
        addFacetIfNotEmpty(node, IO_XSD_ENUMERATION, restrictionValue);   // TODO: WRONG!!!!
    }
    // TODO: WRONG!!!!
    addFacetIfNotEmpty(node, IO_XSD_WHITESPACE, facets._whiteSpace);
    addFacetIfNotEmpty(node, IO_XSD_PATTERN, facets._pattern);
}

bool XSchemaObject::hasAReference()
{
    return false;
}


QString XSchemaObject::descriptionForProposal()
{
    return name();
}

QString XSchemaObject::getOhterAttrsAsTextList()
{
    QString result ;
    foreach(QString key, _otherAttributes.keys()) {
        result.append(key);
        result.append("=\"");
        result.append(_otherAttributes[key]);
        result.append("\"\n");
    }
    return result;
}

QStringList XSchemaObject::getOhterAttrsNames()
{
    QStringList result ;
    foreach(QString key, _otherAttributes.keys()) {
        result.append(key);
    }
    return result;
}

bool XSchemaObject::hasOtherAttributes()
{
    return !_otherAttributes.isEmpty();
}

bool XSchemaObject::hasOtherAttribute(const QString &attrName)
{
    if(_otherAttributes.contains(attrName)) {
        return true;
    }
    return false;
}

QString XSchemaObject::getOtherAttribute(const QString &attrName)
{
    if(_otherAttributes.contains(attrName)) {
        return _otherAttributes[attrName];
    }
    return "";
}


bool XSchemaObject::addAttributeToCollection(const QString & name, XSchemaAttributesCollection & attributesCollection, XSchemaAttribute * inputAttribute)
{
    if(NULL == inputAttribute) {
        return false;
    }
    //no name and ref togheter
    // if ref, no inner simple type
    if(inputAttribute->hasAReference()) {
        XSchemaAttribute *finalAttribute = _root->schema()->topLevelAttribute(inputAttribute->referencedObjectName());
        if(NULL != finalAttribute) {
            addAttributeToCollection(finalAttribute->name(), attributesCollection, finalAttribute);
        }
        return true ;
    }
    //decode the type
    QString typeName;
    QString defaultValue = inputAttribute->defaultValue();
    XSchemaSimpleTypeRestriction *simpleTypeRestriction = NULL ;
    typeName = inputAttribute->xsdType();
    if(!inputAttribute->xsdType().isEmpty()) {
        XSchemaElement *simpleTypeElement = _root->schema()->findSimpleType(inputAttribute->xsdType());
        if(NULL != simpleTypeElement) {
            simpleTypeRestriction = simpleTypeElement->getSimpleTypeRestriction();
        }
    } else {
        simpleTypeRestriction = inputAttribute->getSimpleTypeRestriction();
        if(NULL != simpleTypeRestriction) {
            typeName = simpleTypeRestriction->base();
        }
    }
    QStringList enums;
    while(NULL != simpleTypeRestriction) {
        if(typeName.isEmpty()) {
            typeName = simpleTypeRestriction->base();
        }
        //collectRestrictions;
        simpleTypeRestriction->addEnumsToListIfAny(enums);

        if(!simpleTypeRestriction->base().isEmpty()) {
            XSchemaElement *simpleTypeElement = _root->schema()->findSimpleType(simpleTypeRestriction->base());
            simpleTypeRestriction = NULL ;
            if(NULL != simpleTypeElement) {
                simpleTypeRestriction = simpleTypeElement->getSimpleTypeRestriction();
            }
        } else {
            simpleTypeRestriction = NULL ;
            break;
        }
    }
    if(NULL != inputAttribute) {
        attributesCollection.insert(name, inputAttribute, typeName, enums, defaultValue);
        return true;
    }
    return false;
}

bool XSchemaObject::addAttributeGroupToCollection(XSchemaAttributesCollection & attributesCollection, XSchemaAttributeGroup * inputAttributeGroup)
{
    if(NULL == inputAttributeGroup) {
        return false;
    }
    XSchemaAttributeGroup *finalAttributeGroup = inputAttributeGroup;
    if(inputAttributeGroup->hasAReference()) {
        finalAttributeGroup = _root->schema()->topLevelAttributeGroup(inputAttributeGroup->referencedObjectName());
        return addAttributeGroupToCollection(attributesCollection, finalAttributeGroup);
    }
    if(NULL != finalAttributeGroup) {
        foreach(XSchemaObject * child, finalAttributeGroup->getChildren()) {
            if(child->getType() == SchemaTypeAttribute) {
                XSchemaAttribute *aChild = static_cast<XSchemaAttribute*>(child);
                addAttributeToCollection(aChild->name(), attributesCollection, aChild);
            } else if(child->getType() == SchemaTypeAttributeGroup) {
                XSchemaAttributeGroup *aChild = static_cast<XSchemaAttributeGroup*>(child);
                addAttributeGroupToCollection(attributesCollection, aChild);
            }
        }
        return true;
    }
    return false;
}

XSchemaElement *XSchemaObject::getReferencedType(const QString &typeName)
{
    XSchemaElement *baseTypeOrElement = _root->schema()->topLevelType(typeName);
    return baseTypeOrElement ;
}

bool XSchemaObject::hasOtherAttributeHavingValue(const QString &attrName, const QString &attrValue)
{
    if(getOtherAttribute(attrName) == attrValue) {
        return true;
    }
    return false;
}

XSchemaObject *XSchemaObject::findChildWithOtherAttributeHavingValue(const QString &attrName, const QString &attrValue)
{
    foreach(XSchemaObject * child, getChildren()) {
        if(child->hasOtherAttributeHavingValue(attrName, attrValue)) {
            return child;
        }
        XSchemaObject *childRes = child->findChildWithOtherAttributeHavingValue(attrName, attrValue);
        if(NULL != childRes) {
            return childRes ;
        }
    }
    if(getType() == SchemaTypeElement) {
        XSchemaElement  *element = static_cast<XSchemaElement*>(this);
        foreach(XSchemaObject * child, element->attributes()) {
            if(child->hasOtherAttributeHavingValue(attrName, attrValue)) {
                return child;
            }
            XSchemaObject *childRes = child->findChildWithOtherAttributeHavingValue(attrName, attrValue);
            if(NULL != childRes) {
                return childRes ;
            }
        }
    }
    return NULL ;
}


QString XSchemaObject::localName(const QString &name)
{
    QString localName = name;
    int colonIndex = name.indexOf(":");
    if(colonIndex >= 0) {
        localName = name.mid(colonIndex + 1);
    }
    return localName ;
}

bool XSchemaObject::isTopLevel()
{
    if(NULL != _parent) {
        if(_parent->getType() == SchemaTypeSchema) {
            return true;
        }
    }
    return false;
}

bool XSchemaObject::isInRedefine()
{
    if(NULL != _parent) {
        if(_parent->getType() == SchemaTypeRedefine) {
            return true;
        }
    }
    return false;
}

bool XSchemaObject::hasSimpleType()
{
    if(_children.size() > 0) {
        XSchemaObject *o = _children.at(0);
        if(o->getType() == SchemaTypeElement) {
            XSchemaElement *element = (XSchemaElement*)o;
            if(element->isTypeOrElement() && (element->category() == XSchemaElement::EES_SIMPLETYPE_ONLY)) {
                return true;
            }
        }
    }
    return false ;
}

XSchemaObject* XSchemaObject::hasChildTypeOf(const ESchemaType expectedType)
{
    if(_children.size() > 0) {
        XSchemaObject *o = _children.at(0);
        if(o->getType() == expectedType) {
            return o;
        }
    }
    return NULL ;
}

bool XSchemaObject::hasKey()
{
    return (NULL != hasChildTypeOf(SchemaTypeKey));
}

bool XSchemaObject::hasKeyRef()
{
    return (NULL != hasChildTypeOf(SchemaTypeKeyRef));
}

bool XSchemaObject::hasUnique()
{
    return (NULL != hasChildTypeOf(SchemaTypeUnique));
}
