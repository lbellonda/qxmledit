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


#include "namespacereferenceentry.h"

SingleNamespaceReferenceEntry::SingleNamespaceReferenceEntry()
{
}

SingleNamespaceReferenceEntry::SingleNamespaceReferenceEntry(const QString &uri, const QString &location)
{
    _uri = uri ;
    _schemaLocation = location ;
}

SingleNamespaceReferenceEntry::~SingleNamespaceReferenceEntry()
{
}


QString SingleNamespaceReferenceEntry::uri() const
{
    return _uri;
}

void SingleNamespaceReferenceEntry::setUri(const QString &uri)
{
    _uri = uri;
}

QString SingleNamespaceReferenceEntry::schemaLocation() const
{
    return _schemaLocation;
}

void SingleNamespaceReferenceEntry::setSchemaLocation(const QString &schemaLocation)
{
    _schemaLocation = schemaLocation;
}


//--------------------------

NamespaceReferenceEntry::NamespaceReferenceEntry()
{
    _useNoNamespaceSchemaLocation = false;
}

NamespaceReferenceEntry::NamespaceReferenceEntry(const QString &noNsRef)
{
    _noNamespaceSchemaLocation = noNsRef ;
    _useNoNamespaceSchemaLocation = true ;
}

NamespaceReferenceEntry::~NamespaceReferenceEntry()
{
    reset();
}

bool NamespaceReferenceEntry::isUseNoNamespaceSchemaLocation() const
{
    return _useNoNamespaceSchemaLocation;
}

void NamespaceReferenceEntry::setUseNoNamespaceSchemaLocation(const bool value, const QString &noNamespaceSchemaLocation)
{
    setUseNoNamespaceSchemaLocation(value);
    setNoNamespaceSchemaLocation(noNamespaceSchemaLocation);
}

void NamespaceReferenceEntry::setUseNoNamespaceSchemaLocation(const bool value)
{
    _useNoNamespaceSchemaLocation = value;
}

QString NamespaceReferenceEntry::noNamespaceSchemaLocation() const
{
    return _noNamespaceSchemaLocation;
}

void NamespaceReferenceEntry::setNoNamespaceSchemaLocation(const QString &noNamespaceSchemaLocation)
{
    _noNamespaceSchemaLocation = noNamespaceSchemaLocation;
}

QList<SingleNamespaceReferenceEntry *> NamespaceReferenceEntry::references() const
{
    return _references;
}

void NamespaceReferenceEntry::setReferences(const QList<SingleNamespaceReferenceEntry *> &references)
{
    _references = references;
}

void NamespaceReferenceEntry::reset()
{
    foreach(SingleNamespaceReferenceEntry * entry, _references) {
        delete entry ;
    }
    _references.clear();
}

void NamespaceReferenceEntry::addSchemaReference(const QString &uri, const QString &location)
{
    SingleNamespaceReferenceEntry *entry = new SingleNamespaceReferenceEntry(uri, location);
    _references.append(entry);
}

bool NamespaceReferenceEntry::compareTo(NamespaceReferenceEntry *other)
{
    if(NULL == other) {
        return false;
    }
    if(_useNoNamespaceSchemaLocation != other->_useNoNamespaceSchemaLocation) {
        return false;
    }
    if(_noNamespaceSchemaLocation != other->_noNamespaceSchemaLocation) {
        return false;
    }
    if(_references.size() != other->_references.size()) {
        return false;
    }
    int max = _references.size();
    FORINT(i, max) {
        SingleNamespaceReferenceEntry* src = _references.at(i);
        SingleNamespaceReferenceEntry* oth = other->_references.at(i);
        if(src->schemaLocation() != oth->schemaLocation()) {
            return false;
        }
        if(src->uri() != oth->uri()) {
            return false;
        }
    }
    return true;
}

QString NamespaceReferenceEntry::toString()
{
    QString result ;
    if(_useNoNamespaceSchemaLocation) {
        result += "using NoNamespaceSchemaLocation\n";
    }
    result += QString("NoNamespaceSchemaLocation=%1\n").arg(_noNamespaceSchemaLocation);
    result += QString("namespaces size=%1\n").arg(_references.size());
    int max = _references.size();
    FORINT(i, max) {
        SingleNamespaceReferenceEntry* src = _references.at(i);
        result += QString("%1 %2 %3\n").arg(i).arg(src->uri()).arg(src->schemaLocation());
    }
    return result ;
}
