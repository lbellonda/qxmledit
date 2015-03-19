/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "anonattrexcdialog.h"
#include "ui_anonattrexcdialog.h"
#include "utils.h"
#include "modules/xml/elmpath.h"

AnonAttrExcDialog::AnonAttrExcDialog(Element *element, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnonAttrExcDialog)
{
    _element = element ;
    ui->setupUi(this);
    loadData();
    on_cmdSelectAll_clicked();
    setupUi();
}

AnonAttrExcDialog::~AnonAttrExcDialog()
{
    delete ui;
}

AnonSettingsWidget *AnonAttrExcDialog::settingsWidget()
{
    return ui->settings;
}

void AnonAttrExcDialog::selectAllAttributes()
{
    selectAllItems(true);
}

void AnonAttrExcDialog::setupUi()
{
    ui->settings->enableRecursive(false);
    ui->settings->setExcludeChecked(true);
    ui->settings->setMethodDefaultChecked(true);

    ui->attributes->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->attributes->setSelectionMode(QAbstractItemView::SingleSelection);

    UITableCheckWidgetHelper *helper = new UITableCheckWidgetHelper(ui->attributes);
    helper->addSomeWidget(ui->buttonBox->button(QDialogButtonBox::Ok));
    helper->addSomeWidget(ui->cmdDeselectAll);
    _uiManager.addHelper(helper);
    _uiManager.fireEvent();
}

void AnonAttrExcDialog::loadData()
{
    ui->attributes->setUpdatesEnabled(false);
    ui->attributes->setColumnCount(3);
    QStringList labels;
    labels << tr("Selected") << tr("Name") << tr("value");
    ui->attributes->setHorizontalHeaderLabels(labels);
    bool error = false;

    int rows = _element->getAttributesList().size();
    ui->attributes->setRowCount(rows);
    int row = 0 ;
    foreach(Attribute * attribute, _element->getAttributesList()) {
        QTableWidgetItem *item0 = new QTableWidgetItem();
        if(NULL == item0) {
            error = true ;
        } else {
            item0->setFlags(((item0->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable)) & (~Qt::ItemIsEditable));
            item0->setCheckState(Qt::Unchecked);
            ui->attributes->setItem(row, 0, item0);
        }
        QTableWidgetItem *item1 = new QTableWidgetItem();
        if(NULL == item1) {
            error = true ;
        } else {
            item1->setText(attribute->name);
            item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
            ui->attributes->setItem(row, 1, item1);
        }
        QTableWidgetItem *item2 = new QTableWidgetItem();
        if(NULL == item2) {
            error = true ;
        } else {
            item2->setText(attribute->value);
            item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
            ui->attributes->setItem(row, 2, item2);
        }
        row++;
    }
    if(error) {
        Utils::error(this, tr("Fatal error"));
        close();
    }
    ui->attributes->setUpdatesEnabled(true);
}


void AnonAttrExcDialog::retrieveSelection(QList<Attribute*> &resList)
{
    int rows = ui->attributes->rowCount();
    for(int row = 0 ; row < rows ; row ++) {
        QTableWidgetItem *item0 = ui->attributes->item(row, 0);
        if(item0->checkState() == Qt::Checked) {
            Attribute *attribute = new Attribute();
            if(NULL != attribute) {
                QTableWidgetItem *itemName = ui->attributes->item(row, 1);
                QTableWidgetItem *itemValue = ui->attributes->item(row, 2);
                attribute->name = itemName->text();
                attribute->value = itemValue->text();
                resList.append(attribute);
            } else {
                Utils::errorOutOfMem(this);
            }
        }
    }
}

QList<AnonException*> AnonAttrExcDialog::getExceptions()
{
    QList<Attribute*> resList;
    QList<AnonException*> result ;
    retrieveSelection(resList);
    foreach(Attribute * attribute, resList) {
        result.append(getAnException(attribute));
        delete attribute;
    }
    return result ;
}

QString AnonAttrExcDialog::makePath(const QString &elementPath, const QString &attributeName)
{
    return QString("%1/@%2").arg(elementPath).arg(attributeName);
}

AnonException *AnonAttrExcDialog::getAnException(Attribute *attribute)
{
    AnonException *exception = new AnonException();

    exception->setUseNamespace(ui->settings->isUseNamespace());
    if(exception->isUseNamespace()) {
        ElmPathResolver resolver;
        resolver.select(_element);
        exception->setPath(resolver.attributePath(attribute->name));
    } else {
        exception->setPath(makePath(_element->pathString(), attribute->name));
    }
    if(ui->settings->isIncludeChecked()) {
        exception->setCriteria(AnonInclusionCriteria::Include);
    } else {
        exception->setCriteria(AnonInclusionCriteria::Exclude);
    }
    if(ui->settings->isFixedChecked()) {
        exception->setAnonType(AnonType::FixedValue);
        exception->setFixedValue(ui->settings->fixedValueText());
    } else {
        exception->setAnonType(AnonType::UseDefault);
    }
    return exception;
}


void AnonAttrExcDialog::accept()
{
    QDialog::accept();
}


void AnonAttrExcDialog::selectAllItems(const bool select)
{
    ui->attributes->setUpdatesEnabled(false);
    int rows = ui->attributes->rowCount();
    Qt::CheckState state = select ? Qt::Checked : Qt::Unchecked ;
    for(int row = 0 ; row < rows ; row ++) {
        QTableWidgetItem *item0 = ui->attributes->item(row, 0);
        item0->setCheckState(state);
    }
    ui->attributes->setUpdatesEnabled(true);
}

void AnonAttrExcDialog::on_cmdSelectAll_clicked()
{
    selectAllItems(true);
}

void AnonAttrExcDialog::on_cmdDeselectAll_clicked()
{
    selectAllItems(false);
}

