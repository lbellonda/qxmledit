/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "scxmlnavigatorwidget.h"
#include "ui_scxmlnavigatorwidget.h"
#include "modules/specialized/scxml/scxmlinfo.h"
#include "utils.h"

SCXMLNavigatorWidget::SCXMLNavigatorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SCXMLNavigatorWidget)
{
    _info = NULL ;
    _isInfoEnabled = false;
    Utils::TODO_THIS_RELEASE("todo");
    Utils::TODO_THIS_RELEASE("fare sort alpha");
    ui->setupUi(this);
    ui->states->setHeaderLabel(tr("States"));
    _visIsFlat = false ;
    //ui->states->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
}

SCXMLNavigatorWidget::~SCXMLNavigatorWidget()
{
    reset();
    delete ui;
}

void SCXMLNavigatorWidget::reset()
{
    ui->states->clear();
    if(NULL != _info) {
        delete _info ;
    }
}

void SCXMLNavigatorWidget::setEnabledInfo(const bool value)
{
    if(_isInfoEnabled != value) {
        _isInfoEnabled = value ;
        if(_isInfoEnabled) {
            Utils::TODO_THIS_RELEASE("applyNewInfo(_)");
        }
    }
}

bool SCXMLNavigatorWidget::isEnabledInfo()
{
    return _isInfoEnabled ;
}

void SCXMLNavigatorWidget::applyNewInfo(SCXMLInfo *info)
{
    Utils::TODO_THIS_RELEASE("todo evitare setting inutili");
    /*if(!sameModel()) {
        currentPath = xx;
        currentName = "";
        applyModel();
        restoreSelection(currentPath, currentName);
    }*/
    if(NULL == info) {
        reset();
        _info = info ;
    } else {
        if(!compareIfSameAndUpdateSchemas(_info, info)) {
            reset();
            _info = info ;
            Utils::TODO_THIS_RELEASE("Applica");
            redisplay(ui->flat->isChecked());
            /*foreach(SCXMLState *state, _info->children()) {
                loadState(state, NULL);
                / *QTreeWidgetItem *item = new QTreeWidgetItem();
                item->setText(0, state->id());
                ui->states->addTopLevelItem(item);
                item->setExpanded(true);
                loadChildren(state, item);* /
            }*/
        }
    }
}

void SCXMLNavigatorWidget::redisplay(const bool isFlat)
{
    ui->states->setUpdatesEnabled(false);
    ui->states->clear();
    _visIsFlat = isFlat ;
    foreach(SCXMLState *state, _info->children()) {
        loadState(state, NULL, isFlat);
    }
    if(_visIsFlat) {
        ui->states->sortByColumn(0, Qt::AscendingOrder);
        ui->states->setSortingEnabled(true);
    } else {
        ui->states->setSortingEnabled(false);
    }
    ui->states->setUpdatesEnabled(true);
}

void SCXMLNavigatorWidget::loadState(SCXMLState *state, QTreeWidgetItem *parentItem, const bool isFlat)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    QString stateName = state->id();
    if(state->isParallel()) {
        stateName += QString(" %1").arg(tr("parallel"));
    }
    item->setText(0, stateName);
    item->setData(0, Qt::UserRole, qVariantFromValue((void*)state));
    if(isFlat || (NULL == parentItem)) {
        ui->states->addTopLevelItem(item);
    } else {
        parentItem->addChild(item);
    }
    item->setExpanded(true);
    foreach(SCXMLState *child, state->children()) {
        loadState(child, item, isFlat);
    }
}

bool SCXMLNavigatorWidget::compareIfSameAndUpdateSchemas(SCXMLInfo *oldOne, SCXMLInfo *newOne)
{
    if(NULL == oldOne) {
        return false;
    }
    if(!compareIfSameAndUpdateSchemas(oldOne->children(), newOne->children())) {
        return false;
    }
    return true ;
}

bool SCXMLNavigatorWidget::compareIfSameAndUpdateSchemas(QList<SCXMLState *> oldOne, QList<SCXMLState *> newOne)
{
    if(oldOne.size() != newOne.size()) {
        return false;
    }
    const int len = oldOne.size();
    for(int i = 0 ; i < len ; i ++) {
        SCXMLState *oldElm = oldOne.at(i);
        SCXMLState *newElm = newOne.at(i);
        if(oldElm->id() != newElm->id()) {
            return false;
        }
        if(oldElm->isParallel() != newElm->isParallel()) {
            return false;
        }
        oldElm->setElement(newElm->element());
        if(!compareIfSameAndUpdateSchemas(oldElm->children(), newElm->children())) {
            return false ;
        }

    }
    return true;
}

void SCXMLNavigatorWidget::on_showCmd_clicked()
{
    SCXMLState *state = getSelectedItem();
    if(NULL != state) {
        emit goToState(state->id(), state->element());
    }
}

void SCXMLNavigatorWidget::on_editCmd_clicked()
{
    SCXMLState *state = getSelectedItem();
    if(NULL != state) {
        emit editState(state->id(), state->element());
    }
}

void SCXMLNavigatorWidget::on_flat_clicked()
{
    const bool newFlat = ui->flat->isChecked();
    if(_visIsFlat != newFlat) {
        redisplay(newFlat);
    }
}

SCXMLState *SCXMLNavigatorWidget::getSelectedItem()
{
    if(ui->states->selectedItems().size() > 0) {
        QTreeWidgetItem *item = ui->states->selectedItems().at(0);
        QVariant data = item->data(0, Qt::UserRole);
        SCXMLState *state = (SCXMLState *)data.value<void*>();
        return state ;
    }
    return NULL;
}

void SCXMLNavigatorWidget::on_states_itemSelectionChanged()
{
    Utils::TODO_THIS_RELEASE("selezione abilita pulasnte");
    SCXMLState *state = getSelectedItem();
    if(NULL != state) {
        emit goToState(state->id(), state->element());
    }
}

void SCXMLNavigatorWidget::on_states_itemDoubleClicked(QTreeWidgetItem */*item*/, int /*column*/)
{
    SCXMLState *state = getSelectedItem();
    if(NULL != state) {
        emit editState(state->id(), state->element());
    }
}

void SCXMLNavigatorWidget::on_states_itemClicked(QTreeWidgetItem * /*item*/, int /*column*/)
{
    // nothing atm
}
