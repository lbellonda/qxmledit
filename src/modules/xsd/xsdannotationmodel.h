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


#ifndef XSDANNOTATIONMODEL_H
#define XSDANNOTATIONMODEL_H

#include <QObject>
#include "xsdeditor/xschema.h"
class XSDOperationParameters;

class XSDAnnotationModel : public QObject
{
    Q_OBJECT
    QList<XSchemaObject*> _children;

public:
    explicit XSDAnnotationModel(QObject *parent = 0);
    virtual ~XSDAnnotationModel();

    bool hasOnlyOneInfo();
    XSchemaObject* child(const int index);
    void addChild(XSchemaObject *newChild);
    void reset();
    bool hasInfo();
    XInfoBase *firstAnnotation();
    bool replaceOrAddChild(XSchemaObject *origItem, XInfoBase *newItem);
    bool moveItem(const int startPosition, const int endPosition);
    bool removeItem(XSchemaObject *origItem);
    Element *makeElementList(Element *parent, XSDOperationParameters *params);
    XSchemaAnnotation *makeElementList(XSchemaAnnotation *parent);
    int childrenCount();
    QList<XSchemaObject*> *childrenList();

    void insert(XSchemaObject *newItem);
    XSDAnnotationModel *clone();

signals:

public slots:

};

#endif // XSDANNOTATIONMODEL_H
