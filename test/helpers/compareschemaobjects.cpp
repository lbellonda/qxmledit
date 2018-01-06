/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#include "compareschemaobjects.h"

CompareSchemaObjects::CompareSchemaObjects()
{
}

CompareSchemaObjects::~CompareSchemaObjects()
{
}


bool CompareSchemaObjects::compareListXObjects(const QString &msg, QStringList reference, QList<XSchemaObject*> &compare, const ESchemaType type)
{
    QString errorMessage = msg ;
    errorMessage += "\n";
    bool isOk = true;
    if( reference.count() != compare.count() ) {
        isOk = false;
        errorMessage += QString("arguments count differs Expected:%1 found: %2\n").arg(reference.count()).arg(compare.count());
    }
    // name with name list
    QList<XSchemaObject*> localCompare;
    localCompare.append(compare);
    foreach( QString src, reference ) {
        bool found = false;
        foreach (XSchemaObject* comp, localCompare) {
            if( comp->name() == src ) {
                found = true ;
                if( comp->getType() != type ) {
                    errorMessage += QString("Object '%1' found type: %2, expected %3\n").arg(comp->name()).arg(comp->getType()).arg(type);
                    isOk = false;
                }
                localCompare.removeOne(comp);
                break;
            }
        } // foreach
        if(!found) {
            errorMessage += QString("Item not found: '%1'\n").arg(src);
            isOk = false;
        }
    } // foreach reference
    foreach (XSchemaObject* comp, localCompare) {
        errorMessage += QString("Item not expected: '%1'\n").arg(comp->name());
        isOk = false;
    }

    if(!isOk) {
        QString err = "items differs\n";
        err += errorMessage;
        error = err;
        return false ;
    }
    return true ;
}


bool CompareSchemaObjects::compareListXReferences(const QString &msg, QList<QPair<QString,ESchemaType> > &reference, QList<XSchemaObject*> &compare)
{
    QString errorMessage = msg ;
    errorMessage += "\n";
    bool isOk = true;
    if( reference.count() != compare.count() ) {
        isOk = false;
        errorMessage += QString("arguments count differs Expected:%1 found: %2\n").arg(reference.count()).arg(compare.count());
    }
    // name with name list
    QList<XSchemaObject*> localCompare;
    localCompare.append(compare);
    QPair <QString,ESchemaType>  src;
    foreach( src, reference ) {
        bool found = false;
        foreach (XSchemaObject* comp, localCompare) {
            if( comp->getType() == src.second ) {
                switch(comp->getType()) {
                case SchemaTypeInclude:
                    if( src.first == static_cast<XSchemaInclude*>(comp)->schemaLocation() ) {
                        found = true ;
                    }
                    break;
                case SchemaTypeRedefine:
                    if( src.first == static_cast<XSchemaRedefine*>(comp)->schemaLocation() ) {
                        found = true ;
                    }
                    break;
                case SchemaTypeImport:
                    if( src.first == static_cast<XSchemaImport*>(comp)->schemaLocation() ) {
                        found = true ;
                    }
                    break;
                default:
                    break;
                }
            } // if
            if( found ) {
                localCompare.removeOne(comp);
                break;
            }
        } // foreach inner
        if(!found) {
            errorMessage += QString("Item not found: '%1':'%2'\n").arg(src.second).arg(src.first);
            isOk = false;
        }
    } // foreach reference
    foreach (XSchemaObject* comp, localCompare) {
        errorMessage += QString("Item not expected: '%1'\n").arg(comp->name());
        isOk = false;
    }

    if(!isOk) {
        QString err = "items differs\n";
        err += errorMessage;
        error = err;
        return false ;
    }
    return true ;
}

