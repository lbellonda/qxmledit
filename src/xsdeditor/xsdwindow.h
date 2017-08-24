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
#ifndef XSDWINDOW_H
#define XSDWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <xmlEdit.h>

#include "xsdeditor/xschema.h"
#include "xsdeditor/items/xgraphicsitems.h"
#include "xsdeditor/xsdgraphiccontext.h"
#include "xsdeditor/xsdgraphicsconfiguration.h"
#include "xsdeditor/xsdbackgroundconfig.h"
#include "qxmleditdata.h"

namespace Ui
{
class XSDWindow;
}

#define XSD_ITEM_DATA   0

class XSDScene : public QGraphicsScene
{
    Q_OBJECT
    XSDGraphicsBackgroundConfiguration _bkConfig;

    void configureAndSetGradient(XSDGraphicsBackgroundConfiguration *bkgConfig, QGradient &gradient);
    void applyBackground(XSDGraphicsBackgroundConfiguration *bkgConfig);
    void calculateGradientPoints(XSDGraphicsBackgroundConfiguration *bkgConfig, QPoint &startPoint, QPoint &endPoint);

public:
    XSDScene();
    ~XSDScene();

    void updateBounds();
    void gotoItem(QGraphicsItem *item);
    void setBackgroundCfg(XSDGraphicsBackgroundConfiguration *bkgConfig);
};


class ElementItem ;
class ChoiceItem ;
/**
  questa classe serve a tenere i collegamenti tra la finestra dell'editor
  e l'elemento generico. Da fuori accedo all'item e all'oggetto contenuto
  TODO: decidere se va derivata o posseduta. Se derivo, posso implmentare
      i metodi.
  **/


class RChild;
class XSDItem ;

class XMainMenuBuilder
{
public:
    virtual QMenu *buildMenu(XSchemaObject *item) = 0;
};

class ItemMenuBuilder
{
public:
    virtual QMenu *buildMenu() = 0;
};

class XItemMenuBuilder : public ItemMenuBuilder
{
    XSDItem * _item ;
    XMainMenuBuilder *_rootMenuBuilder;

public:
    virtual QMenu *buildMenu();
    explicit XItemMenuBuilder(XSDItem * newItem) ;
    ~XItemMenuBuilder();

    void setRootMenuBuilder(XMainMenuBuilder *rootMenuBuilder);
    XMainMenuBuilder *rootMenuBuilder();
};


/**
  questa classe gestisce i figli a livello grafico
  */


class RChild
{
    static int instances ;
    static int totalInstances;
    XSDItem *_item;
    XSDItem *_parent;
    LineItem *_line;
    int _id;
public:
    RChild(XSDItem *newParent, XSDItem *newItem, LineItem *newLine);
    ~RChild();
    XSDItem *item() const
    {
        return _item ;
    }
    void updatePosition() ;
};

class RChildren
{
    QList<RChild*> _children;
    QGraphicsLineItem* _secondLine;
public:
    QGraphicsLineItem* _line;

    RChildren();
    ~RChildren();

    void reset();

    int childrenSize();
    RChild* childAt(const int index);
    bool createChild(XSDItem *parent, XSDItem *item);
    QList<RChild*> &children()
    {
        return _children;
    }
    int findObject(XSchemaObject *object);
    void deleteAt(const int &position);
    bool init(XSDItem *parent);
    QGraphicsLineItem *secondLine(XSDItem *parent);
};


class XSDItemContext
{
public:
    enum EDrawStrategy {
        ONE_AFTER,
        COMPACT
    };

    /**
     * @brief The ESchemaRenderingStrategy enum, used for rendering strategy
     */
    enum ESchemaRenderingStrategy {
        // children under parent
        DISPLAYSTR_UNDER,
        // children as horizontal pyramid
        DISPLAYSTR_HOR_PYRAMID,
    };


private:
    EDrawStrategy _strategy;
    ESchemaRenderingStrategy _renderingStrategy;
    int _gapBetweenChildren;
    int _stemLength;
    bool _isDebug;

public:
    XSDItemContext(QXmlEditData *appData);
    ~XSDItemContext();

    EDrawStrategy getStrategy();
    ESchemaRenderingStrategy renderingStrategy() const;
    void setRenderingStrategy(const ESchemaRenderingStrategy displayType);
    int gapBetweenChildren() const;
    void setGapBetweenChildren(int gapBetweenChildren);
    int stemLength() const;
    void setStemLength(int stemLength);
    bool isDebug() const;
    void setIsDebug(bool isDebug);
};

class XSDItem : public QObject, protected ItemServiceExecutor
{
    Q_OBJECT
protected:
    RChildren _children ;
    XsdGraphicContext * _context;
    XItemMenuBuilder _menuBuilder;
    QGraphicsPixmapItem* _iconInfo ;
    RChild  *_chain;
    QGraphicsPixmapItem *_iconExtraAttrs;
    qreal _childrenHeight;
    bool _childrenSizeInvalid;
    qreal _realChildrenHeight;
    bool _isBase;
    bool _isDiff ;

    //---------------------------------------------------
public:
    static QColor colorEqualsStart;
    static QColor colorEqualsEnd;
    static QColor colorAddStart;
    static QColor colorAddEnd;
    static QColor colorDelStart;
    static QColor colorDelEnd;
    static QColor colorModStart;
    static QColor colorModEnd;
    static QColor colorInvalidStart;
    static QColor colorInvalidEnd;
    //---------------------------------------------------
protected:
    void setObject(XSchemaObject *newObject, XSchemaObject *oldObject);
    void removeObject(XSchemaObject *object);
    void createExtraAttrsIcon(QGraphicsItem *parent, const int xPos);
    QGraphicsTextItem *createTypeItem(QGraphicsItem *parent, XsdGraphicContext *context);
    qreal calcChildrenHeightAndDisposeStrategyHorPyramid(XSDItemContext *context);
    qreal calcChildrenHeightStrategyHorPyramid(XSDItemContext *context);
    virtual qreal offsetHeight();
    qreal calcYPosHalf(qreal yPosExt, qreal offsetY);
    qreal calcYPosTop(qreal yPosExt, qreal offsetY);
    void reDisposeAllStrategyHorPyramid(XSDItemContext *context);
    void disposeObjectHorPyramid(XSDItemContext *context, const int level, const qreal xPos, const qreal yPos);
    qreal recalcChildrenPosStrategyUnder(XSDItemContext *context);
    void preAddChildren(XSchemaObject *object);
    virtual void afterDispose(const int yPos, const int height);
    virtual void afterDisposeAllChildren();
    QRectF calcDependentBounds();
    virtual qreal marginBottom();
    virtual qreal extraSpace();
    void doShowInfo(const bool showInfo, const QString &tipText, QList<QGraphicsItem*> &items);
    void createIconInfo(QGraphicsItem *theParent, const int xPos = 10, const int yPos = 10);
    void buildTooltip();
    virtual QString preTooltipString();

public:
    XSDItem(XsdGraphicContext *newContext);
    virtual ~XSDItem();

    XSDItem *addChild(XSchemaObject *child);
    XSDItem *factoryCreateItem(XSchemaObject *newChild, QGraphicsItem *parent);
    static XSDItem *createItem(XsdGraphicContext *context, XSchemaObject *newChild, QGraphicsItem *parent);
    void remove(XSchemaObject *child);
    void newChildPosition(QGraphicsItem *newChild);
    virtual QGraphicsItem *graphicItem() = 0;
    virtual XSchemaObject *item() = 0;
    RChild *chain();
    void setChain(RChild* newChain);
    void setRootContext(XsdGraphicContext *value) ;
    void totalSize(QRectF &totalSize);
    qreal recalcChildrenPos(XSDItemContext *context);
    void afterPositionChange();
    QRectF measureOptimumDimensions(const int nItems, QGraphicsItem ** items);
    QRectF measureOptimumDimensions(QList<QGraphicsItem*> &items);
    void execMenuEvent(QGraphicsSceneContextMenuEvent *event, QGraphicsItem *item) ;
    void setGradientColor(QLinearGradient &gradient, const XSDCompareState::EXSDCompareState state);
    void setToolTipState(QGraphicsItem *item, const XSDCompareState::EXSDCompareState state);
    QRectF disposeHorizontallyAndAlignLower(QList<QGraphicsItem*> &items, const int xPos, const int yPos);
    void setStdFontToItem(QGraphicsTextItem *gItem, const bool isBigger, const bool isBold, const bool isIalic);
    void setGradientColor(GraphicsRoundRectItem *item, const XSDCompareState::EXSDCompareState state);
    XsdGraphicContext *context();
    RChildren *rChildren();
    virtual QString itemClassName() = 0 ;

public slots:
    virtual void childAdded(XSchemaObject *newChild);
};


class RootItem : public XSDItem
{
    Q_OBJECT

    PolygonItem *_graphicsItem;
    QPolygonF _contour;
    XSDSchema *_item;

    void init(XsdGraphicContext *context);

protected:
    QString preTooltipString();

public:
    //IS_TYPE(TypeSchema)

    RootItem(XsdGraphicContext *newContext, XSDSchema *newSchema = NULL, QGraphicsItem * parent = 0);
    virtual ~RootItem();

    XSDSchema *schema() const;
    void setItem(XSDSchema *newItem)  ;
    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }

    virtual QString itemClassName()
    {
        return "RootItem";
    }

private slots:
    void objectDeleted(XSchemaObject* child);
    void childRemoved(XSchemaObject* child);
};


class ContainerItem : public XSDItem
{
    Q_OBJECT

    static const int ContainerLabelPad = 10 ;

    int _topOffsetForLabel;
    int _marginBottom ;
    GraphicsRectItem *_graphicsItem;
    PolygonItem *_boundsItem;
    TextItem *_label;
    QPolygonF _contour;
    XSchemaContainer *_item;
    QGraphicsLineItem *_debugLine;

    void init(XsdGraphicContext *context);
protected:
    virtual void afterDispose(const int yPos, const int height);
    virtual void afterDisposeAllChildren();

public:

    ContainerItem(XsdGraphicContext *newContext, XSchemaContainer *newItem, QGraphicsItem * parent = 0);
    virtual ~ContainerItem();

    void setItem(XSchemaContainer *newItem)  ;
    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }

    virtual QString itemClassName()
    {
        return "ContainerItem";
    }
    virtual qreal offsetHeight();
    virtual qreal marginBottom();

private slots:
    void objectDeleted(XSchemaObject* child);
    void childRemoved(XSchemaObject* child);
};


class AttributeItem : public XSDItem
{
    Q_OBJECT
    QRectF _contour;
    XSchemaAttribute *_item;
    GraphicsRoundRectItem *_graphics;
    TextItem *_textItem ;
    QGraphicsPixmapItem *_iconLink;
    QGraphicsPixmapItem *_iconType;

    void init();

private slots:

    void textChanged();
    void nameChanged(const QString &newName);
public slots:
    virtual void childAdded(XSchemaObject *newChild);

public:

    AttributeItem(XsdGraphicContext *newContext, XSchemaAttribute *newItem, QGraphicsItem * parent = 0);
    virtual ~AttributeItem();

    XSchemaAttribute *item() const ;
    void setItem(XSchemaAttribute *newItem)  ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphics ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }

    virtual QString itemClassName()
    {
        return "AttributeItem";
    }

};


class ElementItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour;
    XSchemaElement *_item;
    GraphicsRoundRectItem *_graphicsItem;
    QGraphicsTextItem *_textItem ;
    QGraphicsTextItem *_typeItem ;
    QGraphicsTextItem *_propertiesItem ;
    QList<AttributeItem*> _attributes;
    QGraphicsPixmapItem *_iconLink;
    QGraphicsPixmapItem *_iconType;
    QGraphicsLineItem *_separator;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void textChanged();
    void elmNameChanged(const QString &newName);
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
    virtual void childAdded(XSchemaObject *newChild);

protected:
    void setIconType();

public:

    //IS_TYPE(TypeElement)

    ElementItem(XsdGraphicContext *newContext, XSchemaElement *newItem, QGraphicsItem * parent = 0);
    virtual ~ElementItem();

    XSchemaElement *element() const ;
    void setItem(XSchemaElement *newItem)  ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }

    virtual AttributeItem *addAttribute(XSchemaAttribute *attribute, QPointF pos);

    void changeGraphics();
    virtual QString itemClassName()
    {
        return "ElementItem";
    }
};

class ChoiceItem : public XSDItem
{
    Q_OBJECT

    QRectF _contour;
    XSchemaChoice *_item;
    QGraphicsPixmapItem *_iconItem;
    CircleItem *_graphicsItem;
    QGraphicsTextItem *_labelItem;
    qreal _extraSpace;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    virtual qreal extraSpace();
    virtual QString preTooltipString();

public:

    ChoiceItem(XsdGraphicContext *newContext, XSchemaChoice *newItem, QGraphicsItem * parent = 0);
    virtual ~ChoiceItem();

    XSchemaChoice *choice() const ;
    void setItem(XSchemaChoice *newItem)  ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "ChoiceItem";
    }
};

class SequenceItem : public XSDItem
{
    Q_OBJECT

    QRectF _contour;
    XSchemaSequence *_item;
    GraphicsRectItem *_graphicsItem;
    QGraphicsTextItem *_labelItem;
    QGraphicsPixmapItem *_icon;
    qreal _extraSpace;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    virtual qreal extraSpace();
    virtual QString preTooltipString();

public:

    SequenceItem(XsdGraphicContext *newContext, XSchemaSequence *newItem, QGraphicsItem * parent = 0);
    virtual ~SequenceItem();

    XSchemaSequence *sequence() const ;
    void setItem(XSchemaSequence *newItem)  ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "SequenceItem";
    }

};


class RestrictionItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour ;
    //TODO: a generic restriction?
    XSchemaObject *_item;
    PolygonItem *_graphicsItem;
    QGraphicsTextItem *_facets;
    QGraphicsTextItem *_name;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

public:

    RestrictionItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * parent = 0);
    virtual ~RestrictionItem();

    XSchemaObject *restriction() const ;
    void setItem(XSchemaObject *newItem) ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "RestrictionItem";
    }

};

class UnionItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour ;
    XSchemaSimpleTypeUnion *_item;
    PolygonItem *_graphicsItem;
    QGraphicsSimpleTextItem *_pName ;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

public:

    UnionItem(XsdGraphicContext *newContext, XSchemaSimpleTypeUnion *newItem, QGraphicsItem * parent = 0);
    virtual ~UnionItem();

    XSchemaSimpleTypeUnion *xunion() const ;
    void setItem(XSchemaObject *newItem) ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "UnionItem";
    }

};

class ListItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour ;
    XSchemaSimpleTypeList *_item;
    PolygonItem *_graphicsItem;
    QGraphicsSimpleTextItem *_nameWidget ;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

public:

    ListItem(XsdGraphicContext *newContext, XSchemaSimpleTypeList *newItem, QGraphicsItem * parent = 0);
    virtual ~ListItem();

    XSchemaSimpleTypeList *list() const ;
    void setItem(XSchemaObject *newItem) ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "ListItem";
    }
};

class GenericItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour ;
    XSchemaObject *_item;
    PolygonItem *_graphicsItem;
    QGraphicsPixmapItem *_icon;
    QGraphicsTextItem *_textItem ;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    void setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor);

public:

    GenericItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * parent = 0);
    virtual ~GenericItem();

    //XSchemaObject *all() const ;
    void setItem(XSchemaObject *newItem) ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "GenericItem";
    }

};


class GroupItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour ;
    XSchemaGroup *_item;
    PolygonItem *_graphicsItem;
    QGraphicsPixmapItem *_icon;
    QGraphicsTextItem *_textItem ;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    void setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor);

public:

    GroupItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * parent = 0);
    virtual ~GroupItem();

    //XSchemaObject *all() const ;
    void setItem(XSchemaObject *newItem) ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "GroupItem";
    }

};

class AttributeGroupItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour ;
    XSchemaAttributeGroup *_item;
    PolygonItem *_graphicsItem;
    QGraphicsPixmapItem *_icon;
    QGraphicsTextItem *_textItem ;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    void setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor);

public:

    AttributeGroupItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * parent = 0);
    virtual ~AttributeGroupItem();

    //XSchemaObject *all() const ;
    void setItem(XSchemaObject *newItem) ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "AttributeGroupItem";
    }

};

class IncludeItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour ;
    XSchemaObject *_item;
    PolygonItem *_graphicsItem;
    QGraphicsPixmapItem *_icon;
    QGraphicsTextItem *_textItem ;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    void setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor);

public:

    IncludeItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * parent = 0);
    virtual ~IncludeItem();

    void setItem(XSchemaObject *newItem) ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "IncludeItem";
    }

};

class RedefineItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour ;
    XSchemaObject *_item;
    PolygonItem *_graphicsItem;
    QGraphicsPixmapItem *_icon;
    QGraphicsTextItem *_textItem ;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    void setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor);

public:

    RedefineItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * parent = 0);
    virtual ~RedefineItem();

    void setItem(XSchemaObject *newItem) ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "RedefineItem";
    }
};

class ImportItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour ;
    XSchemaObject *_item;
    PolygonItem *_graphicsItem;
    QGraphicsPixmapItem *_icon;
    QGraphicsTextItem *_textItem ;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    void setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor);

public:

    ImportItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * parent = 0);
    virtual ~ImportItem();

    void setItem(XSchemaObject *newItem) ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "ImportItem";
    }

};

class AllItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour ;
    XSchemaAll *_item;
    PolygonItem *_graphicsItem;
    QGraphicsPixmapItem *_icon;
    QGraphicsTextItem *_textItem ;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    void setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor);

public:

    AllItem(XsdGraphicContext *newContext, XSchemaAll *newItem, QGraphicsItem * parent = 0);
    virtual ~AllItem();

    void setItem(XSchemaObject *newItem) ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "AllItem";
    }

};

class DerivationItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour ;
    XSchemaObject *_item;
    PolygonItem *_graphicsItem;
    QGraphicsPixmapItem *_icon;
    QGraphicsTextItem *_textItem ;
    QGraphicsPixmapItem *_iconLink;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    void setGraphics(/*const QPixmap & pixmap,*/ QColor bkColor);

public:

    DerivationItem(XsdGraphicContext *newContext, XSchemaObject *newItem, QGraphicsItem * parent = 0);
    virtual ~DerivationItem();

    //XSchemaObject *all() const ;
    void setItem(XSchemaObject *newItem) ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }
    virtual QString itemClassName()
    {
        return "DerivationItem";
    }
};

class XSDWindow;


class MainItemMenuBuilder : public XMainMenuBuilder
{
    QMap<ESchemaType, QAction*> _actions;
    QList<ESchemaType> _actionsOrdered;
    XSDWindow *_parent;
public:
    virtual QMenu *buildMenu(XSchemaObject *target);
    explicit MainItemMenuBuilder(XSDWindow *newParent) ;
    ~MainItemMenuBuilder();

    void addAction(const ESchemaType type, QAction* action);
};

#include "xsdeditor/xsdcontroller.h"

class XSDWindow : public QMainWindow
{
    Q_OBJECT
protected:
    enum EEditor {
        EDITOR_NONE = 0,
        EDITOR_SEQUENCE = 1,
        EDITOR_ELEMENT = 2,
        EDITOR_SCHEMA = 3,
        EDITOR_CHOICE = 4,
        EDITOR_ATTRIBUTE = 5,
        EDITOR_INCLUDE = 6,
        EDITOR_GENERIC = 7
    };

    static const int InitialWidth = 100;
    static const int InitialHeight = 100;

    QAction *addAttributeAction;
    QAction *addSequenceAction;
    QAction *addChoiceAction;
    QAction *addRestrictionAction;
    QAction *addExtensionAction;
    QAction *addUnionAction;
    QAction *addListAction;
    QAction *deleteAction;
    QAction *_gotoAction;
    QAction *_copyNameAction;
    QAction *_copyElementAction;
    QAction *_copyFacetsAction;
    QAction *compareCommentsAction;
    QAction *compareShowOnlyDifferencesAction ;
    QAction *compareSwapReferenceAction;


    QMenu *itemContextMenu;
    MainItemMenuBuilder _menuBuilder;
    int     oldSizeFirstWidget ;
    QString _title;
    XSDItem * _mainItem;

    // compare options
    bool _compareComments;
    bool _compareShowOnlyDifferencesAction;
    bool _isSwap ;
    bool _isInError;
    QStack<XSchemaObject*> _viewStack;
    QXmlEditData *_appData;
    QString _selectedElementKey;
    QString _returnKey;

public:
    explicit XSDWindow(QXmlEditData *appData, QWidget *parent = 0);
    virtual ~XSDWindow();

    Q_PROPERTY(IXSDController* controller READ controller WRITE setController)

    IXSDController *controller();
    void setController(IXSDController *value);

    bool isInError();

    QMenu *buildMenu(XSchemaObject *target);
    QAction *getDeleteAction();
    QAction *getGotoAction();
    QAction *getCopyNameAction();
    QAction *getCopyElementAction();
    QAction *getCopyFacetsAction();
    void loadString(const QString &inputData);
    void loadStringImmediate(const QString &inputData);
    void setTitle(const QString &newTitle);
    void setFileName(const QString &newFileName);
    XsdGraphicContext::EContextType contextType();
    void setContextType(const XsdGraphicContext::EContextType value);
    bool setSchema(XSDSchema *theNewSchema);
    XSchemaObject *schema();
    void setXSDCompareReferencePath(const QString &path);
    void setXSDCompareTargetPath(const QString &path);
    void setXSDCompareSummary(QList<XSchemaObject*> &added, QList<XSchemaObject*> &modified, QList<XSchemaObject*> &deleted);
    void setInputKeyMap(QHash<QString, Element*> *theInputKeyMap);
    void setSelectedObjectKey(const QString &theSelectedElementKey);
    void setSelectedExitKey(const QString &key);
    QString selectedExitKey();
    XSDItem *root();

protected:
    void changeEvent(QEvent *e);
    void addEditors();
    void setElementsPanel();
    QAction *createMenuAction(const QString &label, const ESchemaType type);
    XSchemaObject *fromItemData(QTreeWidgetItem *item);
    void setupNavigationBaseItems();
    void setNavigationTargetSelection(XSchemaObject *newSelection);
    void setNavSplitterWidgetSizes(const int width0, const int width1);
    void paintScene(QPainter *painter, const QRectF &sourceArea, const QRectF &destArea, const int pageNumber, const int totalPages);
    void calculatePageRect(QPainter *painter, QRectF &destArea);
    void restoreSelection(QList<QGraphicsItem*> &itemsToSelect);
    bool copyElementActionExecute(XSchemaObject *object);
    bool copyFacetsActionExecute(XSchemaObject *object);
    XSDItem * setZoomObject(XSchemaObject *subject);
    void evalObjZoom();
    void setupSplitter();
    void selectLastObject();

protected:
    Ui::XSDWindow *ui;
protected:
    QGraphicsView *_view;
    XSDScene *_scene;
    QString _stringToLoad;
    XsdGraphicContext _context;
    IXSDController *_controller;
    QString fileName;
    XSDBackgroundConfig _backgroundConfig;
    XSDItemContext _itemContext;
    //-----------------------

    bool completeUi();
    bool newRoot();
    void clear();
    bool newSchema();
    bool setNewSchema(XSDSchema *schema);
    bool showRoot();
    QStack<XSchemaObject*> history;
    int currentHistoryPosition;

    XSDItem *itemData(QGraphicsItem *item);
    XSDItem *getSelectedItem();
    XSchemaObject *getSelectedSchemaObject();
    void setPropertyTarget(XSchemaObject *target);

    void emptyNavigationBox();
    void loadNavigationBox(XSDSchema *schema);
    void enableHistory();
    void truncateHistory();
    void historyNewTarget(XSchemaObject *target);
    void jumpToObject(XSchemaObject *target);
    void callController();
    bool checkNullObject(XSchemaObject *object);
    void informationError();
    void deleteAllItems();

protected slots:
#ifdef  XSD_EVENTS_HANDLED
    void on_cmdA_clicked();
    void on_cmdE_clicked();
    void on_newCmd_clicked();
    void on_cmdC_clicked();
    void on_cmdS_clicked();
    void on_newR_clicked();
    void on_saveToClipbopard_clicked();
    void on_load_clicked();
    void on_fromClipb_clicked();

    void on_addElementAction_triggered();
    void on_addAttributeAction_triggered();
    void on_addSequenceAction_triggered();
    void on_addChoiceAction_triggered();
    void on_addRestrictionAction_triggered();
    void on_addExtensionAction_triggered();
    void on_addUnionAction_triggered();
    void on_addListAction_triggered();
    void on_deleteAction_triggered();

    void onAddChildAction();
#endif
    void on_gotoAction_triggered();
    void on_copyNameAction_triggered();
    void on_copyElementAction_triggered();
    void on_copyFacetsAction_triggered();

    void selectionChanged();
    void on_loadFromString_triggered();

    void on_cmdZoomIn_clicked();
    void on_cmdZoomOut_clicked();
    void on_cmdZoom1_clicked();
    void on_closeButton_clicked();
    void on_clearSearch_clicked();
    void on_searchBox_textChanged(const QString &newText);
    void on_forwardButton_clicked();
    void on_backButton_clicked();

    void on_navigation_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_navigationCmd_clicked();
    void on_printCmd_clicked();
    void on_svgCmd_clicked();

    void on_cmdObjZoomIn_clicked();
    void on_cmdObjZoomOut_clicked();
    void on_cmdObjZoomReset_clicked();
    void on_cmdShowBase_clicked();

    void onCompareAnnotationAction(bool newState);
    void on_xsdCompareTools_swapReference();
    void on_compareSummary_objectDoubleClicked(XSchemaObject* target);
    void on_actionSwapReferenceAndTarget_triggered();
    void on_actionConfigureAspect_triggered();
    void onBackgroundConfigurationChanged(XSDGraphicsBackgroundConfiguration * configuration);
    void on_cmdShowDepend_clicked();

protected:
    virtual bool close();
public:
    QEventLoop *EVENTLOOP;
    void closeEvent(QCloseEvent *event);

    XsdGraphicContext * context()
    {
        return &_context;
    }

};

void updateScenePosition(QGraphicsItem *item);

#endif // XSDWINDOW_H
