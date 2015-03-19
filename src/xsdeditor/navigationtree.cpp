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

#include "xsdeditor/navigationtree.h"
#include "xsdeditor/xschema.h"
#include "utils.h"
#include <QHeaderView>
#include "utils.h"


#define THE_STYLE   "QTreeView::item { show-decoration-selected: 0 } \n\
QTreeView {\
     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                 stop: 0 #FFFFF0, stop: 1 #FFFFD0);\
 }\n\
QTreeView::item:selected {\
    border: 0px none #000;\
    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                        stop: 0 #F0FFFF, stop: 1 #D0FFFF);\
}\n\
QTreeView::branch:has-siblings:!has-children:!adjoins-item {\
    border-image: url(:/special/empty) 0;\
}\n\
\
QTreeView::branch:!has-children:has-siblings:adjoins-item {\
    border-image: url(:/special/empty) 0;\
}\n\
\
QTreeView::branch:!has-children:!has-siblings:adjoins-item {\
    border-image: url(:/special/empty) 0;\
}\n\
\
QTreeView::branch:has-children:closed {\
        border-image: url(:/special/empty) 0;\
        image: url(:/tree/treeClosed);\
}\n\
\
QTreeView::branch:open:has-children {\
        border-image: url(:/special/empty) 0;\
        image: url(:/tree/treeOpen);\
} \
"


NavigationTree::NavigationTree(QWidget *parent) :
    QTreeWidget(parent),
    //TODO: check with current style!
    navigationFont("Arial", 16, QFont::Black),
    chaptersNavFont("Arial", 14, QFont::Bold),
    tilesColor(190, 190, 190),
    titlesBrush(tilesColor),
    transparentBrush(QColor::fromRgba(qRgba(0, 0, 0, 0))),
    highlightBrush(QColor::fromRgba(qRgb(255, 200, 40)))
{
    init();
}

NavigationTree::~NavigationTree()
{

}

void NavigationTree::init()
{

    useOnlyThisSchemaOrAll = true ;

    setExpandsOnDoubleClick(false);
    typeSimpleIcon.addPixmap(QPixmap(QString::fromUtf8(":/xsdimages/simpleTypeIcon")), QIcon::Normal, QIcon::Off);
    typeComplexIcon.addPixmap(QPixmap(QString::fromUtf8(":/xsdimages/typesIcon")), QIcon::Normal, QIcon::Off);
    referenceIcon.addPixmap(QPixmap(QString::fromUtf8(":/xsdimages/referencesIcon")), QIcon::Normal, QIcon::Off);
    elementIcon.addPixmap(QPixmap(QString::fromUtf8(":/xsdimages/elementsIcon")), QIcon::Normal, QIcon::Off);
    attributeIcon.addPixmap(QPixmap(QString::fromUtf8(":/xsdimages/attributesIcon")), QIcon::Normal, QIcon::Off);

    topLevelNavigation = NULL ;
    topLevelElements  = NULL ;
    topLevelTypes  = NULL ;
    topLevelAttributes  = NULL ;
    topLevelReferences  = NULL ;
    topLevelOtherElements = NULL ;
    topLevelAttributeGroups = NULL ;
    topLevelGroups = NULL ;

    topLevelNavigationSpacer = NULL ;
    topLevelElementsSpacer  = NULL ;
    topLevelTypesSpacer  = NULL ;
    topLevelAttributesSpacer  = NULL ;
    topLevelReferencesSpacer  = NULL ;
    topLevelOtherElementsSpacer = NULL ;
    topLevelAttributeGroupsSpacer = NULL ;
    topLevelGroupsSpacer = NULL ;

    setFrameStyle(QFrame::NoFrame);
    setAttribute(Qt::WA_MacShowFocusRect, false);
    setAutoFillBackground(true);
    setAlternatingRowColors(false);

    // Set the palette: the problem is to hilite the text only and not the background without a custom delegate
    QPalette palette = this->palette();
    ///QColor backgroundColor(252, 255, 227);
    ///QColor windowColor(252, 255, 227);
    QColor highlightColor(68, 83, 205);
    QColor textColor(0, 0, 45);
    QColor hiTextColor(0, 40, 40);
    ///palette.setColor(QPalette::Base, backgroundColor);
    palette.setColor(QPalette::Highlight, highlightColor);
    palette.setColor(QPalette::Text, textColor);
    palette.setColor(QPalette::HighlightedText, hiTextColor);
    ///palette.setColor(QPalette::Window, windowColor);
    setPalette(palette);

    header()->hide();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTextElideMode(Qt::ElideMiddle);
    setRootIsDecorated(true);

    setupNavigationBaseItems();
    Utils::TODO_NEXT_RELEASE("Configure style");
    setRootIsDecorated(false);
    //setStyleSheet(THE_STYLE);
}


void NavigationTree::setupNavigationBaseItems()
{
    topLevelNavigation = new QTreeWidgetItem(0);
    topLevelNavigation->setText(0, tr("NAVIGATION"));
    topLevelNavigation->setFlags(topLevelNavigation->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled));
    topLevelNavigation->setFont(0, navigationFont);
    topLevelNavigation->setForeground(0, titlesBrush);
    addTopLevelItem(topLevelNavigation);

    //--- Base items
    topLevelElementsSpacer = addSpacer();

    topLevelElements = addTopLevelCategory(tr("ELEMENTS"));
    // TODO: TEST
    //topLevelElements->setFlags(topLevelNavigation->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable) );

    //--- spacer
    topLevelGroupsSpacer = addSpacer();

    topLevelGroups = addTopLevelCategory(tr("GROUPS"));

    //--- spacer
    topLevelTypesSpacer =  addSpacer();

    topLevelTypes = addTopLevelCategory(tr("TYPES"));

    //--- spacer
    topLevelAttributesSpacer = addSpacer();

    topLevelAttributes = addTopLevelCategory(tr("ATTRIBUTES"));

    //--- spacer
    topLevelAttributeGroupsSpacer = addSpacer();

    topLevelAttributeGroups = addTopLevelCategory(tr("ATTRIBUTE GROUPS"));

    //--- spacer
    topLevelReferencesSpacer = addSpacer();

    topLevelReferences = addTopLevelCategory(tr("REFERENCES"));

    //--- spacer
    topLevelOtherElementsSpacer = addSpacer();

    topLevelOtherElements = addTopLevelCategory(tr("INNER ELEMENTS"));
}

QTreeWidgetItem* NavigationTree::addSpacer()
{
    QTreeWidgetItem *spacer = new QTreeWidgetItem(0);
    spacer->setFlags(spacer->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled));
    addTopLevelItem(spacer);
    return spacer;
}

void NavigationTree::emptyNavigationBox()
{
    QList<QTreeWidgetItem *> toDeleteList ;
    toDeleteList = topLevelElements->takeChildren();
    Utils::deleteQWIList(toDeleteList);

    toDeleteList = topLevelTypes->takeChildren();
    Utils::deleteQWIList(toDeleteList);

    toDeleteList = topLevelAttributes->takeChildren();
    Utils::deleteQWIList(toDeleteList);

    toDeleteList = topLevelReferences->takeChildren();
    Utils::deleteQWIList(toDeleteList);

    toDeleteList = topLevelOtherElements->takeChildren();
    Utils::deleteQWIList(toDeleteList);

    toDeleteList = topLevelAttributeGroups->takeChildren();
    Utils::deleteQWIList(toDeleteList);

    toDeleteList = topLevelGroups->takeChildren();
    Utils::deleteQWIList(toDeleteList);

    allNavigationItems.clear() ;
}

QTreeWidgetItem *NavigationTree::addTopLevelCategory(const QString &label)
{
    QTreeWidgetItem * topLevelItem = new QTreeWidgetItem(0);
    topLevelItem->setText(0, label);
    topLevelItem->setFont(0, chaptersNavFont);
    topLevelItem->setForeground(0, titlesBrush);
    //topLevelItem->setExpanded(true);
    //topLevelItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
    topLevelItem->setExpanded(true);
    addTopLevelItem(topLevelItem);
    return topLevelItem ;
}

QList<XSchemaObject*> NavigationTree::sortObjectsByName(const QList<XSchemaObject*> &objects)
{
    QList<XSchemaObject*> result ;
    QMultiMap<QString, XSchemaObject*> map;
    QList<QString> keys;
    foreach(XSchemaObject * object, objects) {
        QString nameLowerCase = object->name().toLower();
        map.insert(nameLowerCase, object);
        keys.append(nameLowerCase);
    }
    qSort(keys);
    foreach(QString key, keys) {
        QList<XSchemaObject*> values = map.values(key);
        result.append(values);
    }
    return result;
}

QString NavigationTree::mangleName(XSchemaObject * object)
{
    QString name ;
    if(object->isRedefinition()) {
        name = tr("%1 (redefined)").arg(object->name());
    } else {
        name = object->name();
    }
    return name;
}

QTreeWidgetItem *NavigationTree::objectItem(XSchemaObject * object, QIcon icon)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(0);
    item->setText(0, mangleName(object));
    item->setIcon(0, icon);
    item->setData(0, Qt::UserRole, qVariantFromValue((void*)object));
    return item;
}

void NavigationTree::loadObjects(QList<XSchemaObject*> source, QTreeWidgetItem *topLevelItem, QIcon icon, QTreeWidgetItem *topLevelItemSpace)
{
    QList<XSchemaObject*>objects = sortObjectsByName(source);
    if(objects.isEmpty()) {
        topLevelItem->setHidden(true);
        topLevelItemSpace->setHidden(true);
    } else {
        topLevelItem->setHidden(false);
        topLevelItemSpace->setHidden(false);
        foreach(XSchemaObject * object, objects) {
            QTreeWidgetItem *item = objectItem(object, icon);
            topLevelItem->addChild(item);
            allNavigationItems.append(item);
        }
    }
}

void NavigationTree::loadNavigationBox(XSDSchema *schema)
{
    QList<XSchemaObject*>objects ;

    loadObjects(schema->topLevelElements(useOnlyThisSchemaOrAll), topLevelElements, elementIcon, topLevelElementsSpacer);

    objects = sortObjectsByName(schema->topLevelTypes(useOnlyThisSchemaOrAll));
    foreach(XSchemaObject * object, objects) {
        QIcon icon = typeComplexIcon ;
        if(((XSchemaElement*)object)->isSimpleType()) {
            icon = typeSimpleIcon;
        }
        QTreeWidgetItem *item = objectItem(object, icon);
        topLevelTypes->addChild(item);
        allNavigationItems.append(item);
    }

    loadObjects(schema->topLevelAttributes(useOnlyThisSchemaOrAll), topLevelAttributes, attributeIcon, topLevelAttributesSpacer);

    loadObjects(schema->topLevelAttributeGroups(useOnlyThisSchemaOrAll), topLevelAttributeGroups, attributeIcon, topLevelAttributeGroupsSpacer);

    loadObjects(schema->topLevelGroups(useOnlyThisSchemaOrAll), topLevelGroups, QIcon(), topLevelGroupsSpacer) ;

    objects = sortObjectsByName(schema->topLevelReferences());
    topLevelReferences->setHidden(objects.isEmpty());
    topLevelReferencesSpacer->setHidden(objects.isEmpty());

    foreach(XSchemaObject * object, objects) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        QString name = object->name();
        if(object->getType() == SchemaTypeImport) {
            name = ((XSchemaImport*)object)->description();
        } else if(object->getType() == SchemaTypeInclude) {
            name = ((XSchemaInclude*)object)->description();
        } else if(object->getType() == SchemaTypeRedefine) {
            name = ((XSchemaInclude*)object)->description();
        }
        item->setText(0, name);
        item->setToolTip(0, name);
        item->setIcon(0, referenceIcon);
        item->setData(0, Qt::UserRole, qVariantFromValue((void*)object));
        topLevelReferences->addChild(item);
        allNavigationItems.append(item);
    }

    objects = sortObjectsByName(schema->allInnerElements(useOnlyThisSchemaOrAll));
    foreach(XSchemaObject * object, objects) {
        QString name = object->nameOrReference();
        if(!name.isEmpty()) {
            // find parent
            XSchemaObject *obj = object->xsdParent();
            QString parentName ;
            while(obj != NULL) {
                // top level?
                XSchemaObject *theParent = obj->xsdParent();
                if((NULL != theParent) && (theParent->getType() == SchemaTypeSchema)) {
                    parentName = obj->nameOrReference();
                    break;
                }
                obj = obj->xsdParent();
            }
            QString finalName ;
            if(!parentName.isEmpty()) {
                finalName = QString("%1 (%2)").arg(name).arg(parentName);
            } else {
                finalName = name ;
            }

            QTreeWidgetItem *item = new QTreeWidgetItem(0);
            item->setText(0, finalName);
            item->setIcon(0, elementIcon);
            item->setData(0, Qt::UserRole, qVariantFromValue((void*)object));
            topLevelOtherElements->addChild(item);
            allNavigationItems.append(item);
        }
    }
    expandAll();
}

void NavigationTree::setNavigationTargetSelection(XSchemaObject *newSelection)
{
    if(NULL == newSelection) {
        clearSelection();
    } else {
        // finds the selection by using tags
        bool isFound = false;
        // search all the items.
        foreach(QTreeWidgetItem * item, allNavigationItems) {
            if(fromItemData(item) == newSelection) {
                setCurrentItem(item);
                isFound = true;
                break;
            }
        }
        if(!isFound) {
            clearSelection();
        }
    }
}

XSchemaObject *NavigationTree::fromItemData(QTreeWidgetItem *item)
{
    if(NULL == item) {
        return NULL ;
    }
    QVariant data = item->data(0, Qt::UserRole);
    XSchemaObject *object = (XSchemaObject *)data.value<void*>();
    return object ;
}

void NavigationTree::newSearchText(const QString &newText)
{
    setUpdatesEnabled(false);
    matchedItems.clear();
    QString text = newText.toUpper().trimmed();
    bool isEmpty = text.isEmpty();
    foreach(QTreeWidgetItem * item, allNavigationItems) {
        if(isEmpty) {
            item->setBackground(0, transparentBrush) ;
            item->setHidden(false);
        } else {
            if(item->text(0).contains(newText, Qt::CaseInsensitive)) {
                matchedItems.append(item);
                item->setBackground(0, highlightBrush) ;
                item->setHidden(false);
            } else {
                item->setHidden(true);
            }
        }
    }
    setUpdatesEnabled(true);
}
