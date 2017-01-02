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

#include "xsltnavigatorwidget.h"
#include "ui_xsltnavigatorwidget.h"
#include "modules/xslt/xslthelper.h"
#include "element.h"
#include "utils.h"

XSLTNavigatorWidget::XSLTNavigatorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XSLTNavigatorWidget)
{
    Utils::TODO_THIS_RELEASE("fare");
    _isInfoEnabled = false;
    ui->setupUi(this);
    ui->treeNavigator->setHeaderLabel(tr("Functions and templates"));
}

XSLTNavigatorWidget::~XSLTNavigatorWidget()
{
    delete ui;
}

void XSLTNavigatorWidget::setEnabledInfo(const bool value)
{
    if(_isInfoEnabled != value) {
        _isInfoEnabled = value ;
    }
}

bool XSLTNavigatorWidget::isEnabledInfo()
{
    return _isInfoEnabled ;
}

void XSLTNavigatorWidget::applyNewInfo(XsltHelper *xsltHelper)
{
    Utils::TODO_THIS_RELEASE("selezione abilita pulasnte");
    ui->showCmd->setEnabled(false);
    ui->editCmd->setEnabled(false);
    ui->treeNavigator->setUpdatesEnabled(false);
    ui->treeNavigator->setHeaderHidden(false);
    ui->treeNavigator->clear();
    if(NULL != xsltHelper) {
        // insert top level items
        QTreeWidgetItem * topTemplateItem = new QTreeWidgetItem();
        topTemplateItem->setText(0, tr("Templates"));
        topTemplateItem->setData(0, Qt::UserRole, "t");
        topTemplateItem->setFlags(topTemplateItem->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable /*| Qt::ItemIsEnabled*/));
        ui->treeNavigator->addTopLevelItem(topTemplateItem);

        QTreeWidgetItem *topFunctionsItem = new QTreeWidgetItem();
        topFunctionsItem->setText(0, tr("Functions"));
        topFunctionsItem->setData(0, Qt::UserRole, "f");
        topFunctionsItem->setFlags(topFunctionsItem->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable /*| Qt::ItemIsEnabled*/));
        ui->treeNavigator->addTopLevelItem(topFunctionsItem);

        // add the other items
        loadTemplates(xsltHelper, topTemplateItem);
        loadFunctions(xsltHelper, topFunctionsItem);
        topTemplateItem->setExpanded(true);
        topFunctionsItem->setExpanded(true);
    }
    ui->treeNavigator->setUpdatesEnabled(true);
}

void XSLTNavigatorWidget::loadChildrenItems(const bool isTemplate, QTreeWidgetItem *topItem, QHash<QString, Element*> &data, const QString &typeString)
{
    // sort items
    QMap<QString, QString> sortedMap;
    foreach(QString tmp, data.keys()) {
        sortedMap.insert(tmp, tmp);
    }
    Utils::TODO_THIS_RELEASE("se template forma alternativa con match e mode");
    foreach(QString sorted, sortedMap.keys()) {
        QList<Element*> elements = data.values(sorted);
        foreach(Element *element, elements) {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            QString name = sorted;
            if(isTemplate) {
                QString match = element->getAttributeValue("match");
                QString mode = element->getAttributeValue("mode");
                if(name.isEmpty()) {
                    name = tr("match='%1' mode='%2'").arg(match).arg(mode);
                }
            }
            item->setText(0, name);
            item->setToolTip(0, name);
            item->setFlags((item->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsUserCheckable)) | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(0, Qt::UserRole, qVariantFromValue((void*)element));
            topItem->addChild(item);
        }
    }
    if(data.count() == 0) {
        topItem->setText(0, tr("%1 (none)").arg(typeString));
    } else {
        topItem->setText(0, QString("%1 (%2)").arg(typeString).arg(data.count()));
    }
}

void XSLTNavigatorWidget::loadTemplates(XsltHelper *xsltHelper, QTreeWidgetItem  *topItem)
{
    QHash<QString, Element *> allTemplates = xsltHelper->templateNamesMap();
    loadChildrenItems(true, topItem, allTemplates, tr("Templates"));
}

void XSLTNavigatorWidget::loadFunctions(XsltHelper *xsltHelper, QTreeWidgetItem *topItem)
{
    QHash<QString, Element *> allFunctions = xsltHelper->functionNamesMap();
    loadChildrenItems(false, topItem, allFunctions, tr("Functions"));
}

Element *XSLTNavigatorWidget::getSelectedItem()
{
    if(ui->treeNavigator->selectedItems().size() > 0) {
        QTreeWidgetItem *item = ui->treeNavigator->selectedItems().at(0);
        QVariant data = item->data(0, Qt::UserRole);
        Element *element = (Element *)data.value<void*>();
        return element ;
    }
    return NULL;
}

void XSLTNavigatorWidget::on_treeNavigator_itemDoubleClicked(QTreeWidgetItem * /*item*/, int /*column*/)
{
    on_editCmd_clicked();
}

void XSLTNavigatorWidget::on_editCmd_clicked()
{
    Element *element = getSelectedItem();
    if(NULL != element) {
        emit edit(element);
    }
}

void XSLTNavigatorWidget::on_treeNavigator_itemClicked(QTreeWidgetItem * /*item*/, int /*column*/)
{
    on_showCmd_clicked();
}

void XSLTNavigatorWidget::on_showCmd_clicked()
{
    Element *element = getSelectedItem();
    if(NULL != element) {
        emit goTo(element);
    }
}

void XSLTNavigatorWidget::on_treeNavigator_itemSelectionChanged()
{
    bool enabled = false;
    Element *element = getSelectedItem();
    if(NULL != element) {
        enabled = true;
    }
    ui->showCmd->setEnabled(enabled);
    ui->editCmd->setEnabled(enabled);
    on_showCmd_clicked();
}

