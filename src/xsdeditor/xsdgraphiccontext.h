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

#ifndef XSDGRAPHICCONTEXT_H
#define XSDGRAPHICCONTEXT_H

#include <QObject>
#include <QHash>
#include <QFont>

class XSDItem ;
class XSchemaObject;
class XMainMenuBuilder;
class RootItem;
class XSDSchema;
class XSDItemContext;
class QGraphicsScene;

#include "xschemaoperationcontext.h"

class XsdGraphicContext : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief The ContextType enum, used for the differents graphics render
     */
    enum EContextType {
        // used for normal graphics
        CONTEXT_GRAPHICS,
        // used for compare results
        CONTEXT_DIFF
    };

private:
    QHash<XSchemaObject*, XSDItem*> _mapObjectsToItems;
    XMainMenuBuilder *_menuBuilder;
    RootItem *_rootItem;
    XSDSchema *_schema;
    EContextType _contextType;
    QFont _mainFont;
    QFont _normalFont;
    QFont _typeFont;
    XSDItemContext *_itemContext;
    QGraphicsScene *_scene;
    bool _showBaseObjects;
    XSchemaInquiryContext _globalSearchContext;
    bool _showAllSchema;
    bool _isDebug;
    // TODO QStack<XSDItem*> _navigation;

public:
    explicit XsdGraphicContext(QObject *parent = 0);
    ~XsdGraphicContext();

    void resetRoot();
    XSchemaInquiryContext &searchContext();

    void addObject(XSchemaObject* object, XSDItem* item);
    void removeObject(XSchemaObject* object);
    XSDItem *getItemOfObject(XSchemaObject* object);
    XMainMenuBuilder *menuBuilder();
    void setMenuBuilder(XMainMenuBuilder *newBuilder);

    bool isDebug() const;

    RootItem *rootItem();
    XSDSchema *schema();

    void setRootItem(RootItem *rootItem);
    void setSchema(XSDSchema *schema);

    EContextType contextType();
    void setContextType(const EContextType value);

    void clear();

    QFont mainFont()
    {
        return _mainFont;
    }

    QFont normalFont()
    {
        return _normalFont;
    }
    QFont typeFont()
    {
        return _typeFont;
    }

    XSDItemContext *itemContext() const;
    void setItemContext(XSDItemContext *itemContext);

    QGraphicsScene *scene() const;
    void setScene(QGraphicsScene *scene);

    bool isShowBaseObjects() const;
    void setShowBaseObjects(bool showBaseObjects);

    bool isShowAllSchema() const;
    void setShowAllSchema(bool showAllSchema);

signals:

public slots:

};

#endif // XSDGRAPHICCONTEXT_H
