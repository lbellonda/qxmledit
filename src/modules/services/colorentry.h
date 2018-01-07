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


#ifndef COLORENTRY_H
#define COLORENTRY_H

#include <QColor>
#include "libQXmlEdit_global.h"


class LIBQXMLEDITSHARED_EXPORT ColorEntry
{

    QString _key;
    QColor _color;
    QColor _defaultValue;
    bool _isSet;

    QColor parseColorFromString(const QString &newColor);

public:
    ColorEntry(const QString &newKey, const QColor newDefaultValue);
    ~ColorEntry();

    void readCfg();
    void saveCfg();
    void reset();
    void setColorFromString(const QString &newColor);

    QString key() const;
    void setKey(const QString &key);
    QColor color() const;
    void setColor(const QColor &color);
    bool isSet() const;
    void setIsSet(bool isSet);
    QColor defaultValue() const;
    void setDefaultValue(const QColor &defaultValue);
};

#endif // COLORENTRY_H
