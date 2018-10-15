/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
#include "xsdeditor/xschemaoutlineelement.h"
#include "xsdeditor/items/xgraphicsitems.h"
#include "xsdeditor/xsdgraphiccontext.h"
#include "xsdeditor/xsdgraphicsconfiguration.h"
#include "xsdeditor/xsdbackgroundconfig.h"
#include "qxmleditdata.h"
#include <QAbstractTextDocumentLayout>

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
    LineItem *line();
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
    void showChildLine();
    void suppressChildLine();

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
        DISPLAYSTR_NEW0,
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
    QRectF _bounds;
    qreal _yToAdd;
    int _id ;
    static int _instances;

    enum EIntersectType {
        IntersectNoneBefore,
        IntersectBefore,
        IntersectIncluded,
        IntersectAfter,
        IntersectBeforeAndAfter,
        IntersectNoneAfter,
    };


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
    qreal calcChildrenHeightAndDisposeStrategyHorPyramidNew0(XSDItemContext *context);
    virtual qreal offsetHeight();
    qreal calcYPosHalf(qreal yPosExt, qreal offsetY);
    qreal calcYPosTop(qreal yPosExt, qreal offsetY);
    void reDisposeAllStrategyHorPyramid(XSDItemContext *context);
    void disposeObjectHorPyramid(XSDItemContext *context, const int level, const qreal xPos, const qreal yPos);
    //-- region(New0)
    qreal placeAllStrategyHorPyramidNew0(XSDItemContext *context);
    qreal calcChildrenHeight(XSDItemContext *context, const bool isRecursive);
    qreal calcChildrenHeightStrategyNew0(XSDItemContext *context, const bool isRecursive);
    void placeObjectNew0(XSDItemContext *context, const int level, const qreal xPos, const qreal yPos, QRectF &overallBounds);
    bool updateObjectPlacementNew0(XSDItem *root, XSDItemContext *context, QVector<QRectF> &currBounds, QList<QGraphicsItem *> &rendered, QList<XSDItem *> &itemsRendered, QStack<XSDItem*> chain, const qreal extraGapValue);
    void recalcDispose(XSDItemContext *context);
    void updateSummaryLineBounds(const qreal gapThis, const qreal gap, const bool isEnlarging);
    void moveDownBy(const qreal gap);
    qreal updateAnObjectPlacementNew0(XSDItem *target, QVector<QRectF> &currBounds, const qreal thisGap, const qreal gapValue, const int index);
    qreal calcOverallHeight(QList<QGraphicsItem*> &rendered);
    void finalOffset(const QRectF &bounds);
    void finalPos(QRectF &bounds, const bool isFirst);
    void finalOffset__old();
    void finalOffset__old1();
    //-- endregion(New0)
    qreal recalcChildrenPosStrategyUnder(XSDItemContext *context);
    void preAddChildren(XSchemaObject *object);
    virtual void afterDispose();
    virtual void afterDisposeAllChildren();
    QRectF calcDependentBounds();
    virtual qreal marginBottom();
    virtual qreal extraSpace();
    void doShowInfo(const bool showInfo, const QString &tipText, QList<QGraphicsItem*> &items);
    void createIconInfo(QGraphicsItem *theParent, const int xPos = 10, const int yPos = 10);
    void buildTooltip();
    virtual QString preTooltipString();
    void resetLayoutData();
    static EIntersectType intersectionType(const QRectF &candidate, const QRectF &source);
    static qreal checkVerticalCollision(const QRectF &candidate, const QRectF &source, const qreal originalGapValue);

    static void dump_layout_notice(const QString &notice, XSDItem *target, const QRectF &thisBounds, const qreal extraGap);
    static QString dump_rect_string(const QRectF &thisBounds);
    static void dump_layout_info(const QString &msg);
    static void dump_layout_intersect(const int index, const EIntersectType type);
    static void dump_layout_situation(const QString &msgPassed, const QVector<QRectF> &currBounds);
    static void dump_layout_indexes(const int insertAtPosition, const int lastItemLessThan,
                                    const int itemToBeSplitBefore, const int firstItemToBeSuppressed, const int lastItemToBeSuppressed,
                                    const int itemToBeSplitAfter, const int firstItemGreaterThan);
    void dump_layout_check_congruence(const QVector<QRectF> &currBounds);
    static void dump_layout_trace(XSDItem *target, const QString &msg);
    static void dump_layout_item(XSDItem *target);
    void dump_layout_items();
    static QRectF splitRectBefore(const QRectF &current, const QRectF &source);
    static QRectF splitRectAfter(const QRectF &current, const QRectF &source);
    void drawChildrenPort(XSDItemContext *context);
    void updateRectIncludingAll(QRectF &target);

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
    virtual QString itemLabelForChart() = 0 ;
    virtual QColor itemColorForChart();
    QColor itemColorForGroupsForChart();
    RChild *chain();
    QList<XSchemaObject*> childrenSource();
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
    QString dumpAsString(const int indent);
    static void resetId();

public slots:
    virtual void childAdded(XSchemaObject *newChild);
#ifdef QXMLEDIT_TEST
    friend class TestXSDView ;
#endif
};


class RootItem : public XSDItem
{
    Q_OBJECT

protected:
    PolygonItem *_graphicsItem;
    QPolygonF _contour;
    XSDSchema *_item;

    virtual void init(XsdGraphicContext *context);

    QString preTooltipString();

public:
    //IS_TYPE(TypeSchema)

    RootItem(XsdGraphicContext *newContext, XSDSchema *newSchema = NULL, QGraphicsItem * parent = 0);
    virtual ~RootItem();

    XSDSchema *schema() const;
    virtual void setItem(XSDSchema *newItem)  ;
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

    virtual QString itemLabelForChart();

private slots:
    void objectDeleted(XSchemaObject* child);
    void childRemoved(XSchemaObject* child);
};

class RootOutlineItem : public RootItem
{
    Q_OBJECT
    QString _chosenRoot;

protected:
    QList<XSchemaOutlineContainer*> _outlineItems ;

    virtual void init(XsdGraphicContext *context);
    void outlineModeChildren();
    void removeModel();

public:
    //IS_TYPE(TypeSchema)

    RootOutlineItem(XsdGraphicContext *newContext, XSDSchema *newSchema = NULL, QGraphicsItem * parent = 0);
    virtual ~RootOutlineItem();

    virtual void setItem(XSDSchema *newItem)  ;
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
        return "RootOutlineItem";
    }

    QString chosenRoot() const;
    void setChosenRoot(const QString &chosenRoot);

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
    virtual void afterDispose();
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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();

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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();

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

    void changeGraphics();
    virtual QString itemClassName()
    {
        return "ElementItem";
    }
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();

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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();

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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();


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
    void setBackground(const qreal width, const qreal height);

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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
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
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
};


class OutlineElementItem : public XSDItem
{
    Q_OBJECT

    QPolygonF _contour;
    XSchemaOutlineElement *_item;
    GraphicsRoundRectItem *_graphicsItem;
    QGraphicsTextItem *_textItem ;
    QGraphicsPixmapItem *_iconAttrs;

    void init(XsdGraphicContext *newContext);
    void reset();

private slots:
    void textChanged();
    void elmNameChanged(const QString &newName);
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
    void childAdded(XSchemaObject *newChild);
protected:
    void setIconType();

public:

    OutlineElementItem(XsdGraphicContext *newContext, XSchemaOutlineElement *newItem, QGraphicsItem * parent = 0);
    virtual ~OutlineElementItem();

    XSchemaOutlineElement *element() const ;
    void setItem(XSchemaOutlineElement *newItem)  ;

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }

    void changeGraphics();
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
    virtual QString itemClassName()
    {
        return "OutlineElementItem";
    }
};

class OutlineSequenceItem : public XSDItem
{
    Q_OBJECT
protected:
    XSchemaOutlineSequence *_item;
    QRectF _contour;
    GraphicsRectItem *_graphicsItem;
    QGraphicsTextItem *_labelItem;
    QGraphicsPixmapItem *_icon;
    qreal _extraSpace;

    void init(XsdGraphicContext *newContext);
    virtual void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    virtual qreal extraSpace();
    virtual QString preTooltipString();

public:

    OutlineSequenceItem(XsdGraphicContext *newContext, XSchemaOutlineSequence *newItem, QGraphicsItem * parent = 0);
    virtual ~OutlineSequenceItem();

    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }

    XSchemaOutlineSequence *sequence() const ;
    void setItem(XSchemaOutlineSequence *newItem)  ;
    virtual QString iconName();
    virtual QString labelText();

    virtual QString itemClassName()
    {
        return "OutlineSequenceItem";
    }
};

class OutlineChoiceItem : public XSDItem
{
    Q_OBJECT
protected:
    XSchemaOutlineChoice *_item;
    QRectF _contour;
    GraphicsRectItem *_graphicsItem;
    QGraphicsTextItem *_labelItem;
    QGraphicsPixmapItem *_icon;
    qreal _extraSpace;

    void init(XsdGraphicContext *newContext);
    virtual void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    virtual qreal extraSpace();
    virtual QString preTooltipString();

public:

    OutlineChoiceItem(XsdGraphicContext *newContext, XSchemaOutlineChoice *newItem, QGraphicsItem * parent = 0);
    virtual ~OutlineChoiceItem();

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }

    XSchemaOutlineChoice *choice() const ;
    void setItem(XSchemaOutlineChoice *newItem)  ;
    virtual QString iconName();
    virtual QString labelText();
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();

    virtual QString itemClassName()
    {
        return "OutlineChoiceItem";
    }
};

class OutlineAllItem : public XSDItem
{
    Q_OBJECT
protected:
    XSchemaOutlineAll *_item;
    QRectF _contour;
    GraphicsRectItem *_graphicsItem;
    QGraphicsTextItem *_labelItem;
    qreal _extraSpace;

    void init(XsdGraphicContext *newContext);
    virtual void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    virtual qreal extraSpace();
    virtual QString preTooltipString();

public:

    OutlineAllItem(XsdGraphicContext *newContext, XSchemaOutlineAll *newItem, QGraphicsItem * parent = 0);
    virtual ~OutlineAllItem();

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }

    XSchemaOutlineAll *all() const ;
    void setItem(XSchemaOutlineAll *newItem)  ;
    virtual QString labelText();

    virtual QString itemClassName()
    {
        return "OutlineAllItem";
    }
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
};

class OutlineGroupItem : public XSDItem
{
    Q_OBJECT
protected:
    XSchemaOutlineGroup *_item;
    QRectF _contour;
    GraphicsRectItem *_graphicsItem;
    QGraphicsTextItem *_labelItem;
    qreal _extraSpace;

    void init(XsdGraphicContext *newContext);
    virtual void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    virtual qreal extraSpace();
    virtual QString preTooltipString();

public:

    OutlineGroupItem(XsdGraphicContext *newContext, XSchemaOutlineGroup *newItem, QGraphicsItem * parent = 0);
    virtual ~OutlineGroupItem();

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }

    XSchemaOutlineGroup *group() const ;
    void setItem(XSchemaOutlineGroup *newItem)  ;
    virtual QString labelText();

    virtual QString itemClassName()
    {
        return "OutlineGroupItem";
    }
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
};

class OutlineAnyItem : public XSDItem
{
    Q_OBJECT
protected:
    XSchemaOutlineAny *_item;
    QRectF _contour;
    GraphicsRectItem *_graphicsItem;
    QGraphicsTextItem *_labelItem;
    qreal _extraSpace;

    void init(XsdGraphicContext *newContext);
    virtual void reset();

private slots:
    void itemChanged(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

protected:
    virtual qreal extraSpace();
    virtual QString preTooltipString();

public:

    OutlineAnyItem(XsdGraphicContext *newContext, XSchemaOutlineAny *newItem, QGraphicsItem * parent = 0);
    virtual ~OutlineAnyItem();

    virtual QGraphicsItem *graphicItem()
    {
        return _graphicsItem ;
    }
    virtual XSchemaObject *item()
    {
        return _item ;
    }

    XSchemaOutlineAny *any() const ;
    void setItem(XSchemaOutlineAny *newItem)  ;
    virtual QString labelText();

    virtual QString itemClassName()
    {
        return "OutlineAnyItem";
    }
    virtual QString itemLabelForChart();
    virtual QColor itemColorForChart();
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

class XSDRootChooseProvider
{
public:
    XSDRootChooseProvider();
    virtual ~XSDRootChooseProvider();

    virtual QString chooseRoot(QWidget *parent, QList<XSchemaElement*> elements) = 0 ;
};


class XSDPrintInfo
{
public:
    static const QString EndSeparator;
    //---
    bool debugging;
    bool singleBoxDebugging;
    bool error ;
    bool useLinks;
    QPrinter *printer;
    QPainter *painter;
    QRectF pageBounds;
    QRectF printingBounds;
    qreal em;
    qreal availableHeight;
    qreal currentY;
    bool isCalculating;
    int pages;
    int pageBottomMargin;
    int totalPages;
    bool simple;
    QString fileName;
    QString cssTemplate;
    QHash<QString, QString> mapForTypesLink;
    bool qtSyntax ;
    XSDPrintInfo();
    virtual ~XSDPrintInfo();
    virtual bool init();
    virtual void newPage();
    virtual void newPageIfNeeded(const qreal requestedSpace);
    void setPrinter(QPrinter *thePrinter, QPainter *painter, const QRectF &printRect, const double baseResolution);
    void printPageNumber(const int pageNumber, const int totalPages);
    virtual void reset();
    virtual void printBox(const QString &htmlText);
    /**
     * @brief setSimple simple or complete report
     * @param value if true the types and elements detail are unrolled into referencing elements, if complete the types are referenced
     */
    void setSimple(const bool value);
    bool isSimple();
    virtual QString cssFinal();
    virtual QString tableRowClassForIndex(const int index);
    virtual QString tableAttributeRowClassForIndex(const int index);
    virtual QStringList translateCSS(QStringList inputData);
    QString decode(const QString &inputData);
    virtual void updatePageHeight(const int heightInPage);
    virtual bool blockIsValid(QTextBlock block, const int blockIndex);
    virtual QRectF blockBoundingBox(QAbstractTextDocumentLayout *layout, QTextBlock block, const int blockIndex);
    virtual void setupDocument(QTextDocument &document);
    virtual QSizeF documentSize(QTextDocument &document);
    qreal maxAvailablePageHeight();
    void debugString(const QString &text);
    bool checkHeader(const QString &theHtmlText);
};

class XSDPrintInfoHTML : public XSDPrintInfo
{
    QString _text ;
public:
    XSDPrintInfoHTML();
    virtual ~XSDPrintInfoHTML();
    virtual void printBox(const QString &htmlText);
    virtual void reset();
    virtual void newPage();
    virtual void newPageIfNeeded(const qreal requestedSpace);
    QString text();
    void initForHTML(QWidget *aPainter);
};

class XSDWindow ;

class XSDPrintConfig
{
public:
    enum EImageType {
        ImageRaster,
        ImageDotViz
    };
    bool diagramImageExternal;
    EImageType imageType;
    QString dotVizPath;
    //--
    XSDPrintConfig();
    ~XSDPrintConfig();
    void init();
};

class XSDPrint
{
    XSDPrintConfig _config;
    XSDWindow *_window;
    QXmlEditData *_appData;

    static const int TimeoutDotViz = 40000;
    static const QString ElementAnchor;
    static const QString TypeAnchor;
    static const QString AttributeGroupAnchor;
    static const QString AttributeAnchor;
    static const QString GroupAnchor;
    //--
    static const QString InnerElementsAnchor;
    static const QString ReferencesAnchor;

    void calculatePageRect(QPainter *painter, QRectF &destArea);
    void printIndexStart(QString &text);
    void printIndexEnd(XSDPrintInfo &xsdPrintInfo, QString &text);
    bool printSchemaIndexList(XSDPrintInfo &xsdPrintInfo, QString &text, QList<XSchemaObject*> data, const QString &type, const QString &label);
    void printIndexSchemaAttributeGroups(XSDPrintInfo &xsdPrintInfo, QString &text);
    void printIndexSchemaAttributes(XSDPrintInfo &xsdPrintInfo, QString &text);
    void printIndexSchemaGroups(XSDPrintInfo &xsdPrintInfo, QString &text);
    void printIndexSchemaInnerElements(XSDPrintInfo &xsdPrintInfo, QString &text);
    void printIndexSchemaTypes(XSDPrintInfo &xsdPrintInfo, QString &text);
    void printIndexSchemaElements(XSDPrintInfo &xsdPrintInfo, QString &text);
    void printIndexSchemaIntroduction(XSDPrintInfo &xsdPrintInfo, QString &text);
    QString nameAttributeGroups();
    QString nameAttributes();
    QString nameGroups();
    QString nameInnerElements();
    QString nameTypes();
    QString nameElements();
    QString nameReferences();

    void printSchemaData(XSDPrintInfo &xsdPrintInfo, const bool isCalculating);
    void printSchemaElements(XSDPrintInfo &xsdPrintInfo);
    void printSchemaInnerElements(XSDPrintInfo &xsdPrintInfo);
    void printSchemaTypes(XSDPrintInfo &xsdPrintInfo);
    void printSchemaIntroduction(XSDPrintInfo &xsdPrintInfo);
    void printSchemaIndex(XSDPrintInfo &xsdPrintInfo);
    void printSchemaGroups(XSDPrintInfo &xsdPrintInfo);
    void printSchemaAttributes(XSDPrintInfo &xsdPrintInfo);
    void printSchemaAttributeGroups(XSDPrintInfo &xsdPrintInfo);
    void printSchemaEnd(XSDPrintInfo &xsdPrintInfo);
    int printSchemaInfo(XSDPrintInfo &xsdPrintInfo, XSDSchema *schema);
    void printHeader(XSDPrintInfo &xsdPrintInfo, const QString &headerText, const QString &key);
    int printSingleElement(XSDPrintInfo &xsdPrintInfo, XSchemaElement *element);
    void addChildrenElements(XSDPrintInfo &xsdPrintInfo, XSchemaOutlineContainer *xElement, QString &elementText);
    bool addSingleChildrenElements(XSDPrintInfo &xsdPrintInfo, XSchemaOutlineContainer *xElement, QString &text);
    bool isContainerRecursive(XSchemaOutlineContainer *container);
    void paintChildContainer(XSDPrintInfo &xsdPrintInfo, XSchemaOutlineContainer *childContainer, QString &text);
    void printElementChildrenInfo(XSDPrintInfo &xsdPrintInfo, QString &elementText, XSchemaElement *element);
    int printSingleType(XSDPrintInfo &xsdPrintInfo, XSchemaElement *element);
    int printRedefine(XSDPrintInfo &xsdPrintInfo, XSchemaRedefine *object);
    int printImport(XSDPrintInfo &xsdPrintInfo, XSchemaImport *object);
    int printInclude(XSDPrintInfo &xsdPrintInfo, XSchemaInclude *object);
    int printSingleGroup(XSDPrintInfo &xsdPrintInfo, XSchemaGroup *group);
    int printSingleAttribute(XSDPrintInfo &xsdPrintInfo, XSchemaAttribute *attribute);
    int printSingleAttributeGroup(XSDPrintInfo &xsdPrintInfo, XSchemaAttributeGroup *attributeGroup);
    void printGroupChildrenInfo(XSDPrintInfo &xsdPrintInfo, QString &text, XSchemaGroup *group);
    QString printAnnotationString(XSchemaObject *object, const QString className = "");
    QString getAttributesOfElement(XSDPrintInfo &xsdPrintInfo, XSchemaElement *element);
    XSchemaObject *schema() const;
    XSDScene *scene();
    QString getSingleTypeInner(XSDPrintInfo &xsdPrintInfo, XSchemaElement *element);
    QString htmlANameForObject(const QString &theType, XSchemaObject *element);
    QString headerNameForLink(const QString &theType);
    //-------------------
    QString followItem(XSDItem *item, const int level);
    QString indentLine(const int level);
    QString escapeStringForDot(const QString &input);
    //-------------------
    bool createExternalRasterImageForHTML(const QString &fileName);
    QString imageFileFromHTMLFile(const QString &fileName);
    QString relativeImageFileFromHTMLFile(const QString &fileName);
    QString createDotVizCommands();
    QByteArray createRasterImageForHTML();
    void initPrintInfoForHTML(XSDPrintInfoHTML &xsdPrintInfo, const bool qtSyntax, const bool isSimple);
    QString innerGetAsHTML(XSDPrintInfoHTML &xsdPrintInfo, const bool insertImages, const QString &fileName, const bool forceImagesInline);
    QByteArray createInternalImageForHTML();
    bool createExternalImageForHTML(const QString &fileName);
    bool createExternalDotVizImageForHTML(const QString &fileName);
    QByteArray createInternalDotVizImageForHTML();
    bool innerCreateExternalDotVizImageForHTML(const QString &fileName);
    QStringList createDotVizCommandLine(const QString &commandFileName, const QString &imageFileName);
    QString dotVizPath();
    QString getAsHTML(const bool qtSyntax, const bool insertImages, const bool isSimple, const bool forceInlineImages);
    void enterPrintMethod();
    void exitPrintMethod();
    void printPDFToFileInternal(const QString &filePath, const bool isSimple);
    int pointerSize();
    void appendEnums(QString &text, const QStringList &enums);
    void appendOtherFacets(QString &text, XTypeQueryInfo &typeInfo);
    void appendOtherFacets(QString &text, AttrCollectInfo *typeInfo);
public:

#ifdef QXMLEDIT_TEST
    friend class TestXSDView;
    friend class TestXSDWindow;
    XSDPrint();
#endif

    XSDPrint(XSDWindow *window, QXmlEditData *appData);
    ~XSDPrint();

    void printPDFToFile(const QString &filePath, const bool isSimple);
    QString getAsHTML(const bool qtSyntax, const bool insertImages, const bool isSimple);
    bool saveHTMLToFile(const QString &fileName, const bool isSimple);
    QXmlEditData *getAppData() const;
    void setAppData(QXmlEditData *appData);
    void paintScene(XSDPrintInfo *xsdPrintInfo, QPainter *painter, const QRectF &sourceArea, const QRectF &destArea, const int pageNumber, const int totalPages, const int row, const int column);
};

class XSDWindow : public QMainWindow, private XSDRootChooseProvider
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
    XSDRootChooseProvider *_xsdRootChooseProvider;
    bool _chooseProviderSet;

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
    void setChooseProvider(XSDRootChooseProvider *newValue);
    void setOutlineMode(const bool isOutline);
    void printPDF(const bool isSimple);
    QString fileName();
    XSDScene *scene();
    void restoreSelection(QList<QGraphicsItem*> &itemsToSelect);

protected:
    void changeEvent(QEvent *e);
    void addEditors();
    void setElementsPanel();
    QAction *createMenuAction(const QString &label, const ESchemaType type);
    XSchemaObject *fromItemData(QTreeWidgetItem *item);
    void setupNavigationBaseItems();
    void setNavigationTargetSelection(XSchemaObject *newSelection);
    void setNavSplitterWidgetSizes(const int width0, const int width1);
    bool copyElementActionExecute(XSchemaObject *object);
    bool copyFacetsActionExecute(XSchemaObject *object);
    XSDItem * setZoomObject(XSchemaObject *subject);
    void evalObjZoom();
    void setupSplitter();
    void selectLastObject();
    virtual QString chooseRoot(QWidget *parent, QList<XSchemaElement*> elements);
    bool askIfSimpleReport();
    XSchemaObject * resolveName(const XReferenceType referenceType, const QString &name);

protected:
    Ui::XSDWindow *ui;
protected:
    QGraphicsView *_view;
    XSDScene *_scene;
    QString _stringToLoad;
    XsdGraphicContext _context;
    IXSDController *_controller;
    QString _fileName;
    XSDBackgroundConfig _backgroundConfig;
    XSDItemContext _itemContext;
    int currentHistoryPosition;
    XSDPrint _printManager;
    //-----------------------

    bool completeUi();
    bool newRoot();
    void clear();
    bool newSchema();
    bool setNewSchema(XSDSchema *schema);
    bool showRoot();
    QStack<XSchemaObject*> history;

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
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    bool exportAsHtml(const QString &fileName, const bool isSimple);

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
    void xon_gotoAction_triggered();
    void xon_copyNameAction_triggered();
    void xon_copyElementAction_triggered();
    void xon_copyFacetsAction_triggered();

    void selectionChanged();
    void xon_loadFromString_triggered();

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
    void on_cmdOutline_clicked();

    void onCompareAnnotationAction(bool newState);
    void on_xsdCompareTools_swapReference();
    void on_compareSummary_objectDoubleClicked(XSchemaObject* target);
    void on_actionSwapReferenceAndTarget_triggered();
    void on_actionConfigureAspect_triggered();
    void onBackgroundConfigurationChanged(XSDGraphicsBackgroundConfiguration * configuration);
    void on_cmdShowDepend_clicked();
    void on_htmlCmd_clicked();
    void on_cmdReport_clicked();

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
