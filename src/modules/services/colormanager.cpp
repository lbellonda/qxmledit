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


#include "colormanager.h"
#include "qxmleditconfig.h"

#define TAGS_COLOR_DEFAULT  QColor(0x20, 0x40, 0xC0)
#define PROCINSTR_COLOR_DEFAULT QColor(0, 0xC0, 0xC0)
#define ATTRNAMES_COLOR_DEFAULT QColor(0x00, 0x80, 0x00)
#define ATTRVALUES_COLOR_DEFAULT QColor(0xC0, 0x00, 0x00)
#define COMMENTS_COLOR_DEFAULT QColor(0x80, 0x80, 0x80)
#define INFO_COLOR_DEFAULT  QColor(0xB0, 0xB0, 0xB0)
#define TEXTDECL_COLOR_DEFAULT  QColor(0x80, 0x80, 0x80)
#define ANONELPREVIEW_COLOR_DEFAULT QColor(0x9D, 0x9D, 0x80)
#define ANONTEXTPREVIEW_COLOR_DEFAULT   QColor(0x9D, 0x9D, 0x80)
#define ANONATTRPREVIEW_COLOR_DEFAULT    QColor(0xA0, 0xA0, 0xA0)

QColor ColorManager::tagColorDefault()
{
    return TAGS_COLOR_DEFAULT;
}

QColor ColorManager::attributeNameColorDefault()
{
    return ATTRNAMES_COLOR_DEFAULT;
}

QColor ColorManager::attributeValueColorDefault()
{
    return ATTRVALUES_COLOR_DEFAULT;
}

ColorManager::ColorManager() :
    _tags(Config::KEY_COLOR_TAGS, TAGS_COLOR_DEFAULT),
    _procInstr(Config::KEY_COLOR_DECL_PROCINSTR, PROCINSTR_COLOR_DEFAULT),
    _attributeNames(Config::KEY_COLOR_ATTRNAMES, ATTRNAMES_COLOR_DEFAULT),
    _attributeValues(Config::KEY_COLOR_ATTRVALUES, ATTRVALUES_COLOR_DEFAULT),
    _comments(Config::KEY_COLOR_COMMENTS, COMMENTS_COLOR_DEFAULT),
    _info(Config::KEY_COLOR_INFO, INFO_COLOR_DEFAULT),
    _textDecl(Config::KEY_COLOR_TEXTDECL, TEXTDECL_COLOR_DEFAULT),
    _anonElementPreview(Config::KEY_COLOR_ANONELPREVIEW, ANONELPREVIEW_COLOR_DEFAULT),
    _anonTextPreview(Config::KEY_COLOR_ANONTEXTPREVIEW, ANONTEXTPREVIEW_COLOR_DEFAULT),
    _anonAttrPreview(Config::KEY_COLOR_ANONATTRPREVIEW, ANONATTRPREVIEW_COLOR_DEFAULT)

{
    addColor(&_tags);
    addColor(&_procInstr);
    addColor(&_attributeNames);
    addColor(&_attributeValues);
    addColor(&_comments);
    addColor(&_info);
    addColor(&_textDecl);
    //---
    addColor(&_anonElementPreview);
    addColor(&_anonTextPreview);
    addColor(&_anonAttrPreview);
}

ColorManager::~ColorManager()
{
}

ColorEntry* ColorManager::colorFromKey(const QString &color)
{
    if(_colorsByKey.keys().contains(color)) {
        return _colorsByKey[color];
    }
    return NULL ;
}

void ColorManager::addColor(ColorEntry *entry)
{
    _colors.append(entry);
    _colorsByKey.insert(entry->key(), entry);
}

QList<ColorEntry*> ColorManager::colors()
{
    return _colors ;
}

void ColorManager::readCfg()
{
    foreach(ColorEntry * entry, _colors) {
        entry->readCfg();
    }
}

void ColorManager::resetColors()
{
    foreach(ColorEntry * entry, _colors) {
        entry->reset();
    }
}

void ColorManager::saveConfiguration()
{
    foreach(ColorEntry * entry, _colors) {
        entry->saveCfg();
    }
}

//-------

ColorEntry *ColorManager::tags()
{
    return &_tags;
}

ColorEntry *ColorManager::procInstr()
{
    return &_procInstr;
}

ColorEntry *ColorManager::attributeNames()
{
    return &_attributeNames;
}

ColorEntry *ColorManager::attributeValues()
{
    return &_attributeValues;
}

ColorEntry *ColorManager::comments()
{
    return &_comments;
}

ColorEntry *ColorManager::info()
{
    return &_info;
}

ColorEntry *ColorManager::textDecl()
{
    return &_textDecl;
}

ColorEntry *ColorManager::anonElementPreview()
{
    return &_anonElementPreview;
}

ColorEntry *ColorManager::anonTextPreview()
{
    return &_anonTextPreview;
}

ColorEntry *ColorManager::anonAttrPreview()
{
    return &_anonAttrPreview;
}

