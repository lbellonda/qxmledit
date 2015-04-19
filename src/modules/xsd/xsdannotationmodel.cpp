/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#include "xsdannotationmodel.h"
#include "modules/xsd/xsdhelper.h"
#include "utils.h"

XSDAnnotationModel::XSDAnnotationModel(QObject *parent) :
    QObject(parent)
{
}

XSDAnnotationModel::~XSDAnnotationModel()
{
    reset();
}

bool XSDAnnotationModel::hasOnlyOneInfo()
{
    int annotationCount = 0 ;
    foreach(XSchemaObject * object, _children) {
        if(object->isAnnotationElement()) {
            annotationCount++;
            if(annotationCount > 1) {
                return false;
            }
        }
    }
    return true;
}

bool XSDAnnotationModel::hasInfo()
{
    foreach(XSchemaObject * object, _children) {
        if(object->isAnnotationElement()) {
            return true ;
        }
    }
    return false;
}

void XSDAnnotationModel::addChild(XSchemaObject *newChild)
{
    _children.append(newChild);
}

void XSDAnnotationModel::reset()
{
    while(_children.size() > 0) {
        delete _children.first();
        _children.removeFirst();
    }
}


XInfoBase *XSDAnnotationModel::firstAnnotation()
{
    foreach(XSchemaObject * object, _children) {
        if(object->isAnnotationElement()) {
            return static_cast<XInfoBase*>(object);
        }
    }
    return NULL ;
}

XSchemaObject* XSDAnnotationModel::child(const int index)
{
    if((index < _children.size()) && (index >= 0)) {
        return _children.at(index);
    }
    return NULL ;
}

bool XSDAnnotationModel::replaceOrAddChild(XSchemaObject *origItem, XInfoBase *newItem)
{
    if(NULL == newItem) {
        return false ;
    }
    int index = 0 ;
    foreach(XSchemaObject * object, _children) {
        if(object == origItem) {
            _children.replace(index, newItem);
            return true ;
        }
        index ++ ;
    }
    _children.append(newItem);
    return false ;
}


bool XSDAnnotationModel::removeItem(XSchemaObject *origItem)
{
    if(NULL == origItem) {
        return false ;
    }
    int index = 0 ;
    foreach(XSchemaObject * object, _children) {
        if(object == origItem) {
            delete object;
            _children.removeAt(index);
            return true ;
        }
        index ++ ;
    }
    return false ;
}

void XSDAnnotationModel::insert(XSchemaObject *newItem)
{
    if(NULL != newItem) {
        _children.append(newItem);
    }
}

int XSDAnnotationModel::childrenCount()
{
    return _children.size();
}

QList<XSchemaObject*> *XSDAnnotationModel::childrenList()
{
    return &_children;
}

Element *XSDAnnotationModel::makeElementList(Element *parent, XSDOperationParameters *params)
{
    XSDHelper helper;
    foreach(XSchemaObject * object, _children) {
        Element *newChild = NULL ;
        switch(object->getType()) {
        case SchemaOtherElement:
            newChild = helper.makeElementOther(static_cast<XSchemaOther*>(object), parent);
            break;

        case SchemaTypeDocumentation:
            newChild = helper.makeElementDocumentation(static_cast<XDocumentation*>(object), parent, params);
            break;

        case SchemaTypeAppInfo:
            newChild = helper.makeElementAppInfo(static_cast<XAppInfo*>(object), parent, params);
            break;
        default:
            break;
        }
        if(NULL != newChild) {
            parent->addChild(newChild);
        }
    }
    return parent ;
}


XSDAnnotationModel *XSDAnnotationModel::clone()
{
    XSDAnnotationModel *newModel = new XSDAnnotationModel();
    foreach(XSchemaObject * object, _children) {
        XSchemaObject *newChild = NULL ;
        switch(object->getType()) {
        case SchemaOtherElement:
            newChild = static_cast<XSchemaOther*>(object)->clone();
            break;

        case SchemaTypeDocumentation:
            newChild = static_cast<XDocumentation*>(object)->clone();
            break;

        case SchemaTypeAppInfo:
            newChild = static_cast<XAppInfo*>(object)->clone();
            break;
        default:
            break;
        }
        if(NULL != newChild) {
            newModel->_children.append(newChild);
        }
    }
    return newModel ;
}


bool XSDAnnotationModel::moveItem(const int startPosition, const int endPosition)
{
    int size = _children.size();
    if((startPosition != endPosition) && (startPosition >= 0) && (startPosition < size) && (endPosition >= 0) && (endPosition < size)) {
        XSchemaObject *source = _children.at(startPosition);
        _children.removeAt(startPosition);
        _children.insert(endPosition, source);
        return true ;
    } else {
        return false;
    }
}

