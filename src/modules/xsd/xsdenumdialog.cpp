/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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

#include "xsdenumdialog.h"
#include "ui_xsdenumdialog.h"
#include "xmlwidgetutilities.h"
#include "modules/xsd/choosefacetdialog.h"
#include "modules/xsd/xsdhelper.h"
#include "modules/xsd/xsddefaultannotationeditor.h"
#include "utils.h"

/****** Reference data:

<xs:simpleType name="aType">
  <xs:restriction base="xs:string">
    <xs:enumeration value="one"/>
    <xs:enumeration value="two">
        <xs:annotation>
          <xs:documentation>this is a comment for "two"</xs:documentation>
        </xs:annotation>
     </xs:enumeration>
    <xs:enumeration value="three"/>
  </xs:restriction>
</xs:simpleType>
***************/

XSDEnumDialog::XSDEnumDialog(QWidget *parent, QList<XSDFacet*> inputFacets) :
    QDialog(parent),
    ui(new Ui::XSDEnumDialog)
{
    ui->setupUi(this);
    _XSDAnnotationEditProvider = this ;
    foreach(XSDFacet * facet, inputFacets) {
        _facets.append(facet->clone());
    }
    displayData();
    selectionChanged();
}

XSDEnumDialog::~XSDEnumDialog()
{
    delete ui;
    if(NULL != _XSDAnnotationEditProvider) {
        _XSDAnnotationEditProvider->autoDelete();
    }
    EMPTYPTRLIST(_facets, XSDFacet);
}

QList<XSDFacet*> XSDEnumDialog::result()
{
    QList<XSDFacet*> theResult;
    foreach(XSDFacet * facet, _facets) {
        theResult.append(facet->clone());
    }

    return theResult ;
}

void XSDEnumDialog::displayData()
{
    ui->facets->setColumnCount(4);
    QStringList headerLabels;
    headerLabels << tr("Mod");
    headerLabels << tr("Type");
    headerLabels << tr("Value");
    headerLabels << tr("Annotation");
    ui->facets->setHorizontalHeaderLabels(headerLabels);
    ui->facets->horizontalHeader()->setStretchLastSection(true);
    XmlWidgetUtilities::setTableBehavior(ui->facets);

    ui->facets->setRowCount(_facets.size());
    int row = 0;
    foreach(XSDFacet * facet, _facets) {
        setRow(facet, row, false, false);
        row ++;
    }
    ui->facets->resizeColumnsToContents();
    selectionChanged();
}

QTableWidgetItem *XSDEnumDialog::createItem(XSDFacet *facet, const QString &text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & (~Qt::ItemIsEditable));
    item->setData(Qt::UserRole, qVariantFromValue((void*)facet));
    return item;
}


void XSDEnumDialog::insertRow(XSDFacet *facet, const bool selectItem)
{
    int rows = ui->facets->rowCount();
    ui->facets->setRowCount(rows + 1);
    setRow(facet, rows, true, selectItem);
}

void XSDEnumDialog::updateRow(XSDFacet * facet, const int row, const bool selectItem)
{
    setRow(facet, row, true, selectItem);
}

void XSDEnumDialog::setRow(XSDFacet * facet, const int row, const bool isModified, const bool selectItem)
{
    QTableWidgetItem *itemModified = createItem(facet, isModified ? "*" : " ");
    if(isModified) {
        itemModified->setBackgroundColor(QColor(Qt::red));
    }
    itemModified->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QTableWidgetItem *itemType = createItem(facet, facet->typeString());
    itemType->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QTableWidgetItem *itemValue = createItem(facet, facet->value());
    QTableWidgetItem *itemAnnotation = createItem(facet, facet->annotationString());
    ui->facets->setItem(row, 0, itemModified);
    ui->facets->setItem(row, 1, itemType);
    ui->facets->setItem(row, 2, itemValue);
    ui->facets->setItem(row, 3, itemAnnotation);
    if(selectItem) {
        ui->facets->setCurrentCell(row, 0, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        ui->facets->selectRow(row);
    }
}

void XSDEnumDialog::selectionChanged()
{
    int index = currentSelectedIndex();
    bool isOneSelected = index >= 0 ;
    ui->cmdAnnot->setEnabled(isOneSelected);
    ui->cmdMod->setEnabled(isOneSelected);
    ui->cmdDel->setEnabled(isOneSelected);
    ui->cmdMoveUp->setEnabled(index > 0);
    ui->cmdMoveDown->setEnabled(isOneSelected && (index < (_facets.size() - 1)));
}

int XSDEnumDialog::currentSelectedIndex()
{
    return ui->facets->currentRow();
}


void XSDEnumDialog::addFacet(XSDFacet *newFacet)
{
    if(NULL != newFacet) {
        _facets.append(newFacet);
        insertRow(newFacet, true);
        ui->facets->resizeColumnsToContents();
        ui->facets->horizontalHeader()->setStretchLastSection(true);
    }
}

void XSDEnumDialog::updateFacet(XSDFacet *facet)
{
    int index = _facets.indexOf(facet);
    if(index >= 0) {
        updateRow(facet, index, true);
        ui->facets->resizeColumnsToContents();
        ui->facets->horizontalHeader()->setStretchLastSection(true);
    }
}

void XSDEnumDialog::deleteFacetAt(const int index)
{
    QTableWidgetItem *item = ui->facets->item(index, 0);
    XSDFacet *facet = getUserData(item);
    ui->facets->removeRow(index);
    _facets.removeOne(facet);
    delete facet;
    ui->facets->resizeColumnsToContents();
    selectionChanged();
}

void XSDEnumDialog::on_cmdAdd_clicked()
{
    XSDFacet *newFacet = new XSDFacet(XSDFacet::Enumeration, "");
    ChooseFacetDialog dialog(this, newFacet);
    if(dialog.exec() == QDialog::Accepted) {
        addFacet(newFacet);
    } else {
        delete newFacet;
    }
}

void XSDEnumDialog::on_cmdMod_clicked()
{
    XSDFacet *facet = selectedFacet();
    if(NULL != facet) {
        ChooseFacetDialog dialog(this, facet);
        if(dialog.exec() == QDialog::Accepted) {
            updateFacet(facet);
        }
    }
}

void XSDEnumDialog::on_cmdDel_clicked()
{
    int currentRow = ui->facets->currentRow();
    if(currentRow >= 0) {
        deleteFacetAt(currentRow);
    }
}

void XSDEnumDialog::on_facets_itemDoubleClicked(QTableWidgetItem * /*item*/)
{
    on_cmdMod_clicked();
}

void XSDEnumDialog::on_facets_currentItemChanged(QTableWidgetItem * /*current*/, QTableWidgetItem * /*previous*/)
{
    selectionChanged();
}

XSDFacet * XSDEnumDialog::getUserData(QTableWidgetItem *item)
{
    QVariant data = item->data(Qt::UserRole);
    XSDFacet *facet = (XSDFacet *)data.value<void*>();
    return facet ;
}

XSDFacet *XSDEnumDialog::facetAt(const int index)
{
    if((index >= 0) && (index < _facets.size())) {
        return _facets.at(index);
    }
    return NULL ;
}

XSDFacet *XSDEnumDialog::selectedFacet()
{
    int currentRow = ui->facets->currentRow();
    if(currentRow >= 0) {
        QTableWidgetItem *item = ui->facets->item(currentRow, 0);
        return getUserData(item);
    }
    return NULL ;
}

void XSDEnumDialog::on_cmdMoveUp_clicked()
{
    int currentRow = ui->facets->currentRow();
    if(currentRow >= 0) {
        moveUp(currentRow);
    }
}

void XSDEnumDialog::on_cmdMoveDown_clicked()
{
    int currentRow = ui->facets->currentRow();
    if(currentRow < (_facets.size() - 1)) {
        moveDown(currentRow);
    }
}

void XSDEnumDialog::moveUp(const int index)
{
    if(index > 0) {
        XSDFacet *facet = _facets.at(index);
        _facets.removeAt(index);
        _facets.insert(index - 1, facet);
        XmlWidgetUtilities::swapTableRows(ui->facets, index, index - 1);
        ui->facets->setCurrentCell(index - 1, 0, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    }
}

void XSDEnumDialog::moveDown(const int index)
{
    if(index < (_facets.size() - 1)) {
        XSDFacet *facet = _facets.at(index);
        _facets.removeAt(index);
        _facets.insert(index + 1, facet);
        XmlWidgetUtilities::swapTableRows(ui->facets, index, index + 1);
        ui->facets->setCurrentCell(index + 1, 0, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    }
}

void XSDEnumDialog::on_cmdAnnot_clicked()
{
    XSDFacet *facet = selectedFacet();
    if(NULL != facet) {
        editAnnotation(facet);
    }
}

void XSDEnumDialog::editAnnotation(XSDFacet *facet)
{
    XSDAnnotationEditProvider * provider = XSDAnnotationEditProviderObject();
    XSDAnnotationEditor *editor = provider->newEditor(this);
    editor->exec(facet->annotation());
    if(editor->hasResult()) {
        facet->setAnnotation(editor->annotationAsSchema());
        updateFacet(facet);
    }
    delete editor ;
}


XSDAnnotationEditProvider *XSDEnumDialog::XSDAnnotationEditProviderObject()
{
    return _XSDAnnotationEditProvider;
}

void XSDEnumDialog::setXSDAnnotationEditProviderObject(XSDAnnotationEditProvider *newProvider)
{
    if(NULL != _XSDAnnotationEditProvider) {
        _XSDAnnotationEditProvider->autoDelete();
    }
    _XSDAnnotationEditProvider = newProvider ;
}

void XSDEnumDialog::autoDelete()
{
    // do nothing
}

XSDAnnotationEditor *XSDEnumDialog::newEditor(QWidget *window)
{
    return new XSDDefaultAnnotationEditor(window) ;
}
