/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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

#include "wchecker.h"
#include <QTableWidgetItem>

UIHelperManager::UIHelperManager()
{
    _enabled = true ;
}

UIHelperManager::~UIHelperManager()
{
    reset();
}


void UIHelperManager::reset()
{
    foreach(UIWidgetHelper * h, _children) {
        delete h;
    }
    _children.clear();
}

void UIHelperManager::addHelper(UIWidgetHelper* helper)
{
    _children.append(helper);
    helper->activate();
}

void UIHelperManager::fireEvent()
{
    foreach(UIWidgetHelper * h, _children) {
        h->fireEvent();
    }
}

void UIHelperManager::pause()
{
    _enabled = false ;
    foreach(UIWidgetHelper * h, _children) {
        h->pause();
    }
}
//-------------------------------------------------------------------------

UIWidgetHelper::UIWidgetHelper(QWidget *newSource)
{
    _parent = NULL ;
    _source = newSource;
    _enabled = false;
}

UIWidgetHelper::~UIWidgetHelper()
{
}

void UIWidgetHelper::pause()
{
    _enabled = false;
}


void UIWidgetHelper::setParent(UIWidgetHelper *newParent)
{
    _parent = newParent;
}

bool UIWidgetHelper::isEnabled()
{
    return _enabled;
}

//-------------------------------------------------------------------------

UIDataBaseCheckWidgetHelper::UIDataBaseCheckWidgetHelper(QWidget *src) : UIWidgetHelper(src)
{
}

UIDataBaseCheckWidgetHelper::~UIDataBaseCheckWidgetHelper()
{
}

void UIDataBaseCheckWidgetHelper::addAllWidget(QWidget* w)
{
    _allCheckedHandler.append(w);
}

void UIDataBaseCheckWidgetHelper::addSomeWidget(QWidget* w)
{
    _someCheckedHandler.append(w);
}

void UIDataBaseCheckWidgetHelper::addNoneWidget(QWidget* w)
{
    _noneCheckedHandler.append(w);
}

void UIDataBaseCheckWidgetHelper::enableList(QList<QWidget*> &list, const bool setOn)
{
    if(setOn) {
        foreach(QWidget * w, list) {
            if(!w->isEnabled()) {
                w->setEnabled(true);
            }
        }
    } else {
        foreach(QWidget * w, list) {
            if(w->isEnabled()) {
                w->setEnabled(false);
            }
        }
    }
}



//-------------------------------------------------------------------------


UITableCheckWidgetHelper::UITableCheckWidgetHelper(QTableWidget *src) : UIDataBaseCheckWidgetHelper(src)
{
    _table = src;
}

UITableCheckWidgetHelper::~UITableCheckWidgetHelper()
{
}

void UITableCheckWidgetHelper::activate()
{
    //_table = (QTableWidget *)_source ;
    //Q_ASSERT(_source!=NULL);
    connect(_table, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(onItemClicked(QTableWidgetItem *)));
    connect(_table, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(onItemChanged(QTableWidgetItem *)));
}

void UITableCheckWidgetHelper::fireEvent()
{
    _enabled = true ;
    itemClickedEval();
}

void UITableCheckWidgetHelper::onItemClicked(QTableWidgetItem *)
{
    itemClickedEval();
}

void UITableCheckWidgetHelper::onItemChanged(QTableWidgetItem *)
{
    itemClickedEval();
}

void UITableCheckWidgetHelper::itemClickedEval()
{
    if(!_enabled) {
        return ;
    }
    if(NULL != _parent) {
        if(!_parent->isEnabled()) {
            enableList(_allCheckedHandler, false);
            enableList(_noneCheckedHandler, false);
            enableList(_someCheckedHandler, false);
            return ;
        }
    }

    bool someSelected = false;
    bool noneSelected = true;
    bool allSelected = false;
    int selectedCount = 0 ;
    int rows = _table->rowCount();
    for(int row = 0 ; row < rows ; row ++) {
        QTableWidgetItem *item0 = _table->item(row, 0);
        if(item0->checkState() == Qt::Checked) {
            selectedCount ++ ;
        }
    }
    if(selectedCount > 0) {
        someSelected = true ;
        noneSelected = false;
    } else if(0 == selectedCount) {
        someSelected = false ;
        noneSelected = true ;
        allSelected = false ;
    }
    if((selectedCount == rows) && (selectedCount > 0)) {
        someSelected = true ;
        noneSelected = false;
        allSelected = true ;
    }

    // evaluate
    if(_table->isEnabled()) {
        enableList(_allCheckedHandler, allSelected);
        enableList(_noneCheckedHandler, noneSelected);
        enableList(_someCheckedHandler, someSelected);
    } else {
        enableList(_allCheckedHandler, false);
        enableList(_noneCheckedHandler, false);
        enableList(_someCheckedHandler, false);
    }
}

