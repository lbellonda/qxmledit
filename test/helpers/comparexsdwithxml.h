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


#ifndef COMPAREXSDWITHXML_H
#define COMPAREXSDWITHXML_H

#include "xsdeditor/xschema.h"
#include "../testbase.h"

#include <QStack>

class XSDItem;

class CompareXSDWithXML : public TestBase
{

    QString makePath( QList<int> &path, QList<QString> &pathString );
    bool compareItem( Element* element, XSDItem *item, QList<int> &path, QList<QString> &pathString );
    bool compareWith( Element *element, XSDItem *item, QList<int> &path, QList<QString> &pathString );
    Regola *loadReference(const QString &fileName);
    QString dumpItem( XSDItem *item, int indent );
    QString closeItem( XSDItem *item, int indent );
    QString indents( int indent );

public:
    CompareXSDWithXML();
    ~CompareXSDWithXML();

    bool compareTreeWithFile(const QString &fileName, XSDItem *item);
    QString dumpItemTree( XSDItem *item, int indent);


};

#endif // COMPAREXSDWITHXML_H
