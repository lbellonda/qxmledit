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

#ifndef BASEDDATA_H
#define BASEDDATA_H

#include "xmlEdit.h"
#include "regola.h"

class BaseDData
{
    bool error(QWidget *window, const QString &attrName);
public:
    Element *element;

    BaseDData(Element *toModifyElement);
    virtual ~BaseDData();

    QString attributeString(const QString &name, const QString &defaultValue = "");
    void setAttributeString(const QString &name, const QString &value);
    void setAttributeStringIfMissing(const QString &name, const QString &value);
    void assignTag(const QString &tag, Regola *regola, Element *parent);
    bool checkIDREFS(QWidget *window, const QString &attrName);
    bool checkNMTOKEN(QWidget *window, const QString &attrName);
    bool checkID(QWidget *window, const QString &attrName);
};

#endif // BASEDDATA_H
