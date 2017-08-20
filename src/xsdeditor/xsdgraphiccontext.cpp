/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#include "xsdeditor/xsdgraphiccontext.h"
#include "xsdeditor/xsdwindow.h"
#include "qxmleditconfig.h"
#include "utils.h"

XsdGraphicContext::XsdGraphicContext(QObject *parent) :
    QObject(parent)
{
    _isDebug = false ;
    Utils::TODO_THIS_RELEASE("commento");
    _hideAttributes = true ;
    _rootItem = NULL ;
    _schema = NULL ;
    _scene = NULL ;
    _contextType = CONTEXT_GRAPHICS;
    Utils::TODO_THIS_RELEASE("commento");
    //_contextType = CONTEXT_OUTLINE;
    _showAllSchema = false ;
    Utils::TODO_THIS_RELEASE("commento");
    _showBaseObjects = true ;
    QString fontFamily = Config::getString(Config::KEY_XSD_FONTFAMILY, "") ;
    if(!fontFamily.isEmpty()) {
        _normalFont.setFamily(fontFamily);
    }
    int fontSize = Config::getInt(Config::KEY_XSD_FONTSIZE, 9) ;
    _normalFont.setPointSize(fontSize);

    QString mainFontFamily = Config::getString(Config::KEY_XSD_MAINFONTFAMILY, "") ;
    if(!mainFontFamily.isEmpty()) {
        _mainFont.setFamily(mainFontFamily);
    }
    fontSize = Config::getInt(Config::KEY_XSD_MAINFONTSIZE, 10) ;
    _mainFont.setPointSize(fontSize);

    bool fontBold = Config::getBool(Config::KEY_XSD_MAINFONTBOLD, true) ;
    _mainFont.setBold(fontBold);

    QString typeFontFamily = Config::getString(Config::KEY_XSD_TYPEFONTFAMILY, "") ;
    if(!typeFontFamily.isEmpty()) {
        _typeFont.setFamily(mainFontFamily);
    }
    fontSize = Config::getInt(Config::KEY_XSD_TYPEFONTSIZE, 8) ;
    _typeFont.setPointSize(fontSize);

    bool fontItalic = Config::getBool(Config::KEY_XSD_TYPEFONTITALIC, true) ;
    _typeFont.setItalic(fontItalic);
}


XsdGraphicContext::~XsdGraphicContext()
{
    clear();
}


void XsdGraphicContext::resetRoot()
{
    _globalSearchContext.reset();
}

bool XsdGraphicContext::isShowAllSchema() const
{
    return _showAllSchema;
}

bool XsdGraphicContext::isDebug() const
{
    return _isDebug;
}


void XsdGraphicContext::setShowAllSchema(bool showAllSchema)
{
    _showAllSchema = showAllSchema;
}

XSchemaInquiryContext &XsdGraphicContext::searchContext()
{
    return _globalSearchContext;
}


XSDItemContext *XsdGraphicContext::itemContext() const
{
    return _itemContext;
}

void XsdGraphicContext::setItemContext(XSDItemContext *itemContext)
{
    _itemContext = itemContext;
}

void XsdGraphicContext::clear()
{
    if(NULL != _rootItem) {
        delete _rootItem;
        _rootItem = NULL ;
    }

    if(_schema != NULL) {
        delete _schema ;
        _schema = NULL;
    }
    _mapObjectsToItems.clear();
}

void XsdGraphicContext::clearCache()
{
    _mapObjectsToItems.clear();
}

void XsdGraphicContext::addObject(XSchemaObject* object, XSDItem* item)
{
    if(NULL != object) {
        _mapObjectsToItems[object] = item;
    }
}

void XsdGraphicContext::removeObject(XSchemaObject* object)
{
    if(NULL != object) {
        _mapObjectsToItems.remove(object);
    }
}

XSDItem *XsdGraphicContext::getItemOfObject(XSchemaObject* object)
{
    if(_mapObjectsToItems.contains(object)) {
        return _mapObjectsToItems[object];
    }
    return NULL ;
}

XMainMenuBuilder *XsdGraphicContext::menuBuilder()
{
    return _menuBuilder ;
}

void XsdGraphicContext::setMenuBuilder(XMainMenuBuilder *newBuilder)
{
    _menuBuilder = newBuilder ;
}

RootItem *XsdGraphicContext::rootItem()
{
    return _rootItem;
}

XSDSchema *XsdGraphicContext::schema()
{
    return _schema;
}

void XsdGraphicContext::setRootItem(RootItem *newRootItem)
{
    _rootItem = newRootItem ;
}

void XsdGraphicContext::setSchema(XSDSchema *newSchema)
{
    _schema = newSchema ;
}

XsdGraphicContext::EContextType XsdGraphicContext::contextType()
{
    return _contextType ;
}


void XsdGraphicContext::setContextType(const XsdGraphicContext::EContextType value)
{
    _contextType = value ;
}

QGraphicsScene *XsdGraphicContext::scene() const
{
    return _scene;
}

void XsdGraphicContext::setScene(QGraphicsScene *scene)
{
    _scene = scene;
}

bool XsdGraphicContext::isShowBaseObjects() const
{
    return _showBaseObjects;
}

void XsdGraphicContext::setShowBaseObjects(bool showBaseObjects)
{
    _showBaseObjects = showBaseObjects;
}

bool XsdGraphicContext::isHideAttributes() const
{
    return _hideAttributes;
}

void XsdGraphicContext::setHideAttributes(bool hideAttributes)
{
    _hideAttributes = hideAttributes;
}

bool XsdGraphicContext::isOutline()
{
    return CONTEXT_OUTLINE == _contextType ;
}
