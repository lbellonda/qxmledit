/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "xmlwidgetutilities.h"

#include "elementitemdelegate.h"
#include "qattributedelegate.h"
#include "paintinfo.h"
#include "utils.h"
#include "config.h"
#include "element.h"
#include "modules/delegates/elementitemsingledelegate.h"

XmlWidgetUtilities::XmlWidgetUtilities()
{
    _tree = NULL ;
    _tagDelegate = NULL ;
}

XmlWidgetUtilities::~XmlWidgetUtilities()
{
}

void XmlWidgetUtilities::setTree(QTreeWidget *target)
{
    _tree = target ;
}

ElementItemSingleDelegate *XmlWidgetUtilities::tagDelegate()
{
    return _tagDelegate ;
}

QAttributeDelegate *XmlWidgetUtilities::setUpTreeXmlDelegates(QWidget *parent, QTreeWidget *treeWidget, PaintInfo * paintInfo)
{
    ElementItemSingleDelegate *newDelegate = new ElementItemSingleDelegate(paintInfo, parent);
    _tagDelegate = newDelegate ;
    treeWidget->setItemDelegateForColumn(0, newDelegate);
    return NULL ;
}

void XmlWidgetUtilities::resetTree(QTreeWidget *treeWidget, PaintInfo *paintInfo)
{
    treeWidget->setColumnCount(1);
    treeWidget->clear();
    QStringList headers;
    headers << tr("Elements");
    treeWidget->setHeaderLabels(headers);
    treeWidget->setHeaderHidden(true);
    setDataColumnTitle(treeWidget, paintInfo, paintInfo->compactView());
    calcColumnState(treeWidget, paintInfo);
}


void XmlWidgetUtilities::calcColumnState(QTreeWidget *treeWidget, PaintInfo *paintInfo)
{
    bool isShowSize = paintInfo->showElementSize();
    if(isShowSize) {
        treeWidget->showColumn(paintInfo->columnForChildren);
        treeWidget->showColumn(paintInfo->columnForSize);
    } else {
        treeWidget->hideColumn(paintInfo->columnForSize);
        treeWidget->hideColumn(paintInfo->columnForChildren);
    }
}

void XmlWidgetUtilities::setDataColumnTitle(QTreeWidget *treeWidget, PaintInfo *paintInfo, const bool isCompactView)
{
    QHeaderView *header = treeWidget->header() ;
    QString text;
    if(isCompactView) {
        text = tr("Attributes/Text");
    } else {
        text = tr("Attributes");
    }
    QAbstractItemModel* model = header->model();
    model->setHeaderData(paintInfo->columnForAttributes, header->orientation(), text);
}

void XmlWidgetUtilities::resizeTreeColumns(QTreeWidget *treeWidget)
{
    treeWidget->setUpdatesEnabled(false);
    int columns = treeWidget->columnCount();
    for(int i = 0 ; i < columns ; i ++) {
        treeWidget->resizeColumnToContents(i);
    }
    treeWidget->setUpdatesEnabled(true);
}

Element *XmlWidgetUtilities::getSelectedItem(QTreeWidget *treeWidget)
{
    if((NULL != treeWidget) && (treeWidget->selectedItems().size() > 0)) {
        Element *element = Element::fromItemData(treeWidget->selectedItems().at(0));
        return element;
    }
    return NULL;
}

Element *XmlWidgetUtilities::getSelectedItem()
{
    return getSelectedItem(_tree);
}

bool XmlWidgetUtilities::filterEvent(QObject * obj, QEvent * event, QHash<void *, QString> *mapDataAnon)
{
    if(obj == _tree) {
        if(event->type() == QEvent::ToolTip) {
            QHelpEvent *toolTipEvent = static_cast<QHelpEvent *>(event);
            QPoint pos = toolTipEvent->pos();
            int headerHeight = _tree->header()->height();
            QTreeWidgetItem *item = _tree->itemAt(pos.x(), pos.y() - headerHeight);
            if(NULL != item) {
                Element *element = Element::fromItemData(item);
                if(NULL != element) {
                    switch(element->getType()) {
                    case Element::ET_COMMENT: {
                        QString comment = tr("Comment:\n%1").arg(element->getComment());
                        QToolTip::showText(toolTipEvent->globalPos(), comment);
                    }
                    break;
                    case Element::ET_ELEMENT:
                    case Element::ET_TEXT:
                        QToolTip::showText(toolTipEvent->globalPos(), element->columnViewTooltipData(mapDataAnon).toString());
                        break;
                    case Element::ET_ATTRIBUTE:
                    case Element::ET_PROCESSING_INSTRUCTION:
                    default:
                        QToolTip::showText(toolTipEvent->globalPos(), element->tag());
                        break;
                    }
                    return true ;

                }
            }
        }
    }
    return false;
}
