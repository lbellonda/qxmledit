/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#ifndef SCXMLINFO_H
#define SCXMLINFO_H

#include "xmlEdit.h"

class Element ;

class SCXMLState
{
    QList<SCXMLState*> _children;
    Element *_element;
    QString _id;
    bool _parallel;
public:
    SCXMLState();
    virtual ~SCXMLState();

    void allStates(QStringList &result);

    void addChild(SCXMLState *state);
    bool isParallel() const;
    Element *element() const;
    void setElement(Element *element);
    QString id() const;
    void setId(const QString &id);
    void setParallel(bool parallel);
};

class SCXMLInfo
{
    QList<SCXMLState*> _children;
public:
    SCXMLInfo();
    virtual ~SCXMLInfo();

    QStringList allStates();
    void addChild(SCXMLState *state);
};

#endif // SCXMLINFO_H
