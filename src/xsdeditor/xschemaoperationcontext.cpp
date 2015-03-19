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


#include "xschemaoperationcontext.h"
#include "xschema.h"

//------------------------------------------------------------------------

XSchemaOperationContext::XSchemaOperationContext(QObject *parent) :
    QObject(parent)
{
}

XSchemaOperationContext::~XSchemaOperationContext()
{
}

//------------------------------------------------------------------------

XSchemaInquiryContext::XSchemaInquiryContext(QObject *parent) :
    XSchemaOperationContext(parent)
{
}

XSchemaInquiryContext::~XSchemaInquiryContext()
{
}


void XSchemaInquiryContext::reset()
{
    _enrolled.clear();
    _typeErrors.clear();
}

void XSchemaInquiryContext::resetErrors()
{
    _typeErrors.clear();
}

void XSchemaInquiryContext::enroll(XSchemaObject* object)
{
    _enrolled.insert(object);
}

bool XSchemaInquiryContext::isEnrolled(XSchemaObject* object)
{
    return _enrolled.contains(object);
}

void XSchemaInquiryContext::addError(XSchemaElement*element)
{
    QString err;
    if(NULL != element) {
        if(!element->name().isEmpty()) {
            err = element->name();
        } else if(!element->ref().isEmpty()) {
            err = element->ref();
        }
    }
    if(err.isEmpty()) {
        _typeErrors.insert(tr("Unknown"));
    } else {
        _typeErrors.insert(err);
    }
}

QStringList XSchemaInquiryContext::typeErrors()
{
    return _typeErrors.toList();
}

//------------------------------------------------------------------------

XSchemaFindReferenceContext::XSchemaFindReferenceContext(QObject *parent) :
    XSchemaOperationContext(parent)
{
    _isPredefinedType = false ;
    _resolved = NULL ;
}

XSchemaFindReferenceContext::~XSchemaFindReferenceContext()
{
}


bool XSchemaFindReferenceContext::isPredefinedType() const
{
    return _isPredefinedType;
}

void XSchemaFindReferenceContext::setIsPredefinedType(bool isPredefinedType)
{
    _isPredefinedType = isPredefinedType;
}

XSchemaElement *XSchemaFindReferenceContext::resolved() const
{
    return _resolved;
}

void XSchemaFindReferenceContext::setResolved(XSchemaElement *resolved)
{
    _resolved = resolved;
}

QString XSchemaFindReferenceContext::namespaceUri() const
{
    return _namespaceUri;
}

void XSchemaFindReferenceContext::setNamespaceUri(const QString &value)
{
    _namespaceUri = value;
}

QString XSchemaFindReferenceContext::namespacePrefix() const
{
    return _namespacePrefix;
}

void XSchemaFindReferenceContext::setNamespacePrefix(const QString &namespacePrefix)
{
    _namespacePrefix = namespacePrefix;
}
