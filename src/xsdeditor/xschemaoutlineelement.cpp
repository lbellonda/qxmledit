/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

XSchemaOutlineElement::XSchemaOutlineElement(XSchemaObject *newParent, XSchemaRoot *newRoot) : XSchemaObject(newParent, newRoot)
{
    /*INIT_XSD_ATTRD(_defaultValue);
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
    _innerAnnotation = NULL ;*/
}

XSchemaOutlineElement::~XSchemaOutlineElement()
{
    /*if(_innerAnnotation) {
        delete _innerAnnotation ;
        _innerAnnotation = NULL ;
    }*/
    reset();
}


void XSchemaOutlineElement::reset()
{
    Utils::TODO_THIS_RELEASE("reset figli");
    XSchemaObject::reset();
}

XSchemaElement *XSchemaOutlineElement::getElement() const
{
    return _element;
}

void XSchemaOutlineElement::setElement(XSchemaElement *value)
{
    const bool calculate = (_element != value);
    _element = value;
    if(calculate) {
        calculateElement();
    }
}

QString XSchemaOutlineElement::tagName()
{
    Utils::TODO_THIS_RELEASE("fare");
    return "boh";
}

XSDCompareObject::EXSDCompareObject XSchemaOutlineElement::innerCompareTo(XSchemaObject *, XSDCompareOptions &)
{
    return  XSDCompareObject::XSDOBJECT_UNCHANGED;
}

bool XSchemaOutlineElement::generateDom(QDomDocument &, QDomNode &)
{
    return false;
}

void XSchemaOutlineElement::calculateElement()
{
    reset();
    Utils::TODO_THIS_RELEASE("attenzione alla ricorsione");
    if( NULL == _element ) {
        Utils::TODO_THIS_RELEASE("fare");
        return ;
    }
    if( _element->isTypeOrElement()) {
        Utils::TODO_THIS_RELEASE("fare");
        _name = _element->name();
    } else {
        if(XRT_ELEMENT == _element->referencedObjectType()) {
            XSchemaElement *referencedElement = _element->getReferencedElement();
            _name = referencedElement->name();
        } else {
            _name = _element->name();
        }
    }
}
/*
Remember:
element: (simpleType|complexType)?
complexType:
    simpleContent|complexContent|((group|all|choice|sequence)
complex:
    - group (all, choice, seq)
    - all (elements)
    - seq (elm, group, choice, seq, any)
    - choice (==seq)
extension: group|all|choice|sequence
  restriction:(group|all|choice|sequence)


*/
