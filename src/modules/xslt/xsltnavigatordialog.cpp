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


#include "xsltnavigatordialog.h"
#include "ui_xsltnavigatordialog.h"
#include "xslthelper.h"
#include "element.h"
#include "utils.h"

#define TYPE_ROLE   (Qt::UserRole)
#define NAME_ROLE   (Qt::UserRole+1)

#define TEMPLATE    "template"
#define FUNCTION    "function"

Element *XSLTNavigatorDialog::go()
{
    Utils::TODO_THIS_RELEASE("dismettere e spostare i test");
    show();
    if(exec()) {
        return _elementChoosen;
    }
    return NULL ;
}

XSLTNavigatorDialog::XSLTNavigatorDialog(XsltHelper *helper, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XSLTNavigatorDialog)
{
    Utils::TODO_THIS_RELEASE("dismettere e spostare i test");
    _elementChoosen = NULL ;
    _topTemplateItem = NULL ;
    _topFunctionsItem = NULL ;
    _helper = helper;
    ui->setupUi(this);
    finishSetup();
#ifdef QXMLEDIT_TEST
    testStart();
#endif
}


XSLTNavigatorDialog::~XSLTNavigatorDialog()
{
    delete ui;
}


void XSLTNavigatorDialog::finishSetup()
{
    ui->cmdNavigate->setEnabled(false);
    ui->treeNavigator->setUpdatesEnabled(false);
    ui->treeNavigator->setHeaderHidden(true);
    // insert top level items
    _topTemplateItem = new QTreeWidgetItem();
    _topTemplateItem->setText(0, tr("Templates"));
    _topTemplateItem->setData(0, Qt::UserRole, "t");
    _topTemplateItem->setFlags(_topTemplateItem->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable /*| Qt::ItemIsEnabled*/));
    ui->treeNavigator->addTopLevelItem(_topTemplateItem);

    _topFunctionsItem = new QTreeWidgetItem();
    _topFunctionsItem->setText(0, tr("Functions"));
    _topFunctionsItem->setData(0, Qt::UserRole, "f");
    _topFunctionsItem->setFlags(_topFunctionsItem->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable /*| Qt::ItemIsEnabled*/));
    ui->treeNavigator->addTopLevelItem(_topFunctionsItem);

    // add the other items
    loadTemplates(_topTemplateItem);
    loadFunctions(_topFunctionsItem);
    _topTemplateItem->setExpanded(true);
    _topFunctionsItem->setExpanded(true);
    ui->treeNavigator->setUpdatesEnabled(true);
}

void XSLTNavigatorDialog::loadChildrenItems(QTreeWidgetItem *topItem, QSet<QString> &data, const QString &typeString, const QString &dataType)
{
    // sort items
    QMap<QString, QString> sortedMap;
    foreach(QString tmp, data) {
        sortedMap.insert(tmp, tmp);
    }
    foreach(QString sorted, sortedMap.keys()) {
        QTreeWidgetItem *itemItem = new QTreeWidgetItem();
        itemItem->setText(0, sorted);
        itemItem->setFlags((itemItem->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsUserCheckable)) | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemItem->setData(0, TYPE_ROLE, dataType);
        itemItem->setData(0, NAME_ROLE, sorted);
        topItem->addChild(itemItem);
    }
    if(data.count() == 0) {
        topItem->setText(0, tr("%1 (none)").arg(typeString));
    } else {
        topItem->setText(0, QString("%1 (%2)").arg(typeString).arg(data.count()));
    }
}


void XSLTNavigatorDialog::loadTemplates(QTreeWidgetItem  *topItem)
{
    QSet<QString> allTemplates = _helper->templateNames();
    loadChildrenItems(topItem, allTemplates, tr("Templates"), TEMPLATE);
}

void XSLTNavigatorDialog::loadFunctions(QTreeWidgetItem  *topItem)
{
    QSet<QString> allFunctions = _helper->functionNames();
    loadChildrenItems(topItem, allFunctions, tr("Functions"), FUNCTION);
}

void XSLTNavigatorDialog::on_treeNavigator_itemDoubleClicked(QTreeWidgetItem * item, int /*column*/)
{
    QString dataType = item->data(0, TYPE_ROLE).toString();
    QString name = item->data(0, NAME_ROLE).toString();
    if(dataType == TEMPLATE) {
        _elementChoosen = _helper->findTemplate(name);
    } else {
        _elementChoosen = _helper->findFunction(name);
    }
    if(NULL != _elementChoosen) {
        accept();
    }
}

void XSLTNavigatorDialog::on_cmdNavigate_clicked()
{
    QTreeWidgetItem *currentItem = ui->treeNavigator->currentItem() ;
    if(NULL != currentItem) {
        on_treeNavigator_itemDoubleClicked(currentItem, 0) ;
    }
}

void XSLTNavigatorDialog::on_treeNavigator_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * /*previous*/)
{
    ui->cmdNavigate->setEnabled((NULL != current));
}

