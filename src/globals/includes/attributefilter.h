/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#ifndef ATTRIBUTEFILTER_H
#define ATTRIBUTEFILTER_H

#include "libQXmlEdit_global.h"
#include "xmlEdit.h"


class LIBQXMLEDITSHARED_EXPORT AttributeFilter
{
    /**
     * @brief _attributeNames: the list of the attributes
     */
    QSet<QString> _attributeNames;
    bool _isWhiteList;
    bool _isTemporary;
public:
    explicit AttributeFilter();
    ~AttributeFilter();

    bool isIsWhiteList();
    void setIsWhiteList(const bool value);
    QSet<QString> attributeNames();
    bool checkIfSkipped(const QString &name);

    void addName(const QString &name);
    bool isTemporary();
    void setTemporary(const bool newState);
};

#endif // ATTRIBUTEFILTER_H
