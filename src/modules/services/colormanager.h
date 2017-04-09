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


#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include    "modules/services/colorentry.h"

#include <QHash>
#include "libQXmlEdit_global.h"

class LIBQXMLEDITSHARED_EXPORT ColorManager
{
    QHash<QString, ColorEntry*> _colorsByKey;
    QList<ColorEntry*>  _colors;

    ColorEntry _tags;
    ColorEntry _procInstr;
    ColorEntry _attributeNames;
    ColorEntry _attributeValues;
    ColorEntry _comments;
    ColorEntry _info;
    ColorEntry _textDecl;

    ColorEntry _anonElementPreview;
    ColorEntry _anonTextPreview;
    ColorEntry _anonAttrPreview;

    void addColor(ColorEntry *entry);
public:
    ColorManager();
    ~ColorManager();

    void readCfg();
    void resetColors();
    void saveConfiguration();
    ColorEntry* colorFromKey(const QString &color);


    QList<ColorEntry*>  colors();

    ColorEntry *tags();
    ColorEntry *procInstr();
    ColorEntry *attributeNames();
    ColorEntry *attributeValues();
    ColorEntry *comments();
    ColorEntry *info();
    ColorEntry *textDecl();

    ColorEntry *anonElementPreview();
    ColorEntry *anonTextPreview();
    ColorEntry *anonAttrPreview();


    static QColor tagColorDefault();
    static QColor attributeNameColorDefault();
    static QColor attributeValueColorDefault();
};

#endif // COLORMANAGER_H
