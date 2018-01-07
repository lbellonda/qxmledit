/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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


#include "xsdfullannotationsdialog.h"
#include "ui_xsdfullannotationsdialog.h"
#include "xsdsinglecommentdialog.h"
#include "utils.h"

XSDFullAnnotationsDialog::XSDFullAnnotationsDialog(XSDAnnotationModel *newModel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XSDFullAnnotationsDialog)
{
    _model = newModel->clone();
    ui->setupUi(this);
    setupUI();
}

XSDFullAnnotationsDialog::~XSDFullAnnotationsDialog()
{
    if(NULL != _model) {
        delete _model ;
    }
    delete ui;
}

int XSDFullAnnotationsDialog::runExec()
{
    return exec();
}

void XSDFullAnnotationsDialog::setupUI()
{
    ui->annotations->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->annotations->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->annotations, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(onItemClicked(QTableWidgetItem *)));
    connect(ui->annotations, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(onItemChanged(QTableWidgetItem *)));

    enableButtons();
    renderModel();
}

void XSDFullAnnotationsDialog::addNewItem(XSchemaObject *newItem)
{
    _model->insert(newItem);
}

void XSDFullAnnotationsDialog::updateItem(const int index, XInfoBase *newItem)
{
    XSchemaObject *base = _model->child(index);
    _model->replaceOrAddChild(base, newItem);
}

void XSDFullAnnotationsDialog::deleteItem(const int index)
{
    XSchemaObject *base = _model->child(index);
    _model->removeItem(base);
}

XSDAnnotationModel *XSDFullAnnotationsDialog::data()
{
    return _model->clone();
}

void XSDFullAnnotationsDialog::renderItem(const int row, XSchemaObject *srcObj)
{
    bool isEnabled = false;
    QString type ;
    QString lang;
    QString src;
    QString annot;
    if(srcObj->getType() == SchemaTypeAppInfo) {
        XAppInfo *realObj = static_cast<XAppInfo*>(srcObj);
        isEnabled = true ;
        lang = "--";
        src = realObj->source();
        annot = realObj->contentString().trimmed();
        type = "appInfo";
    } else if(srcObj->getType() == SchemaTypeDocumentation) {
        XDocumentation *realObj = static_cast<XDocumentation*>(srcObj);
        isEnabled = true ;
        lang = realObj->language();
        src = realObj->source();
        annot = realObj->contentString().trimmed();
        type = "documentation" ;
    } else {
        type = tr("<other>");
        lang = "--";
        src = "--" ;
        annot = tr("<Not an annotation>");
    }
    setItem(row, 0, type, srcObj, isEnabled);
    setItem(row, 1, lang, srcObj, isEnabled);
    setItem(row, 2, src, srcObj, isEnabled);
    setItem(row, 3, annot, srcObj, isEnabled);
}

void XSDFullAnnotationsDialog::setItem(const int row, const int column, const QString &text, XSchemaObject *src, const bool isEnabled)
{
    QTableWidgetItem *item = new QTableWidgetItem(text) ;
    item->setData(Qt::UserRole, qVariantFromValue((void*)src));
    item->setFlags(item->flags() &= ~(Qt::ItemIsEditable));
    if(!isEnabled) {
        item->setFlags(item->flags() &= ~(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled));
    }
    ui->annotations->setItem(row, column, item);
}

void XSDFullAnnotationsDialog::renderModel(const bool retainSelection)
{
    renderModel(retainSelection, false, -1);
}

void XSDFullAnnotationsDialog::renderModel(const bool retainSelection, const bool setSelection, const int newSelectionIndex)
{
    int currentSelectionIndex = ui->annotations->currentRow();
    ui->annotations->setUpdatesEnabled(false);
    ui->annotations->clearContents();
    ui->annotations->setRowCount(_model->childrenCount());
    int index = 0 ;
    foreach(XSchemaObject * obj, *_model->childrenList()) {
        renderItem(index, obj);
        index++;
    }
    if(retainSelection) {
        ui->annotations->setCurrentCell(currentSelectionIndex, 0);
    } else if(setSelection) {
        ui->annotations->setCurrentCell(newSelectionIndex, 0);
    }
    ui->annotations->resizeColumnsToContents();
    ui->annotations->setUpdatesEnabled(true);
    enableButtons();
}

XInfoBase *XSDFullAnnotationsDialog::currentSelectionAnnotation()
{
    QTableWidgetItem * currentItem = ui->annotations->currentItem();
    if(NULL != currentItem) {
        QVariant data = currentItem->data(Qt::UserRole);
        XSchemaObject *pElement = (XSchemaObject *)data.value<void*>();
        if(pElement->isAnnotationElement()) {
            return static_cast<XInfoBase*>(pElement) ;
        }
    }
    return NULL ;
}


//-----

void XSDFullAnnotationsDialog::on_cmdNew_clicked()
{
    XInfoBase *annotation = new XDocumentation(NULL, NULL);
    XSDSingleCommentDialog editSingle(false, annotation, this);
    if(editSingle.exec() == QDialog::Accepted) {
        _model->addChild(editSingle.createModel());
        renderModel();
    }
    delete annotation;
}

void XSDFullAnnotationsDialog::on_cmdModify_clicked()
{
    XInfoBase *src = currentSelectionAnnotation();
    if(NULL != src) {
        XSDSingleCommentDialog editSingle(false, src, this);
        if(editSingle.exec() == QDialog::Accepted) {
            _model->replaceOrAddChild(src, editSingle.createModel());
            renderModel(true);
        }
    }
}

void XSDFullAnnotationsDialog::on_cmdDelete_clicked()
{
    XSchemaObject *src = currentSelectionAnnotation();
    if(NULL != src) {
        if(Utils::askYN(this, tr("Do you really want to delete this annotation?"))) {
            _model->removeItem(src);
            renderModel();
        }
    }
}

void XSDFullAnnotationsDialog::on_annotations_itemDoubleClicked(QTableWidgetItem * /*item*/)
{
    on_cmdModify_clicked();
}

//-----

void XSDFullAnnotationsDialog::onItemClicked(QTableWidgetItem *)
{
    enableButtons();
}

void XSDFullAnnotationsDialog::onItemChanged(QTableWidgetItem *)
{
    enableButtons();
}

void XSDFullAnnotationsDialog::enableButtons()
{
    bool enableEdit = false ;
    XSchemaObject *selection = currentSelectionAnnotation();
    if(NULL != selection) {
        enableEdit = true ;
    }
    ui->cmdModify->setEnabled(enableEdit);
    ui->cmdDelete->setEnabled(enableEdit);
    int currentRow = ui->annotations->currentRow();
    if(currentRow > 0) {
        ui->cmdMoveUp->setEnabled(true);
    } else {
        ui->cmdMoveUp->setEnabled(false);
    }
    int size = ui->annotations->rowCount();
    if((currentRow >= 0) && (currentRow < (size - 1))) {
        ui->cmdMoveDown->setEnabled(true);
    } else {
        ui->cmdMoveDown->setEnabled(false);
    }
}

void XSDFullAnnotationsDialog::on_cmdMoveUp_clicked()
{
    setEnabled(false);
    int currentRow = ui->annotations->currentRow();
    if(currentRow > 0) {
        if(_model->moveItem(currentRow, currentRow - 1)) {
            renderModel(false, true, currentRow - 1);
        }
    }
    setEnabled(true);
}

void XSDFullAnnotationsDialog::on_cmdMoveDown_clicked()
{
    setEnabled(false);
    int currentRow = ui->annotations->currentRow();
    if((currentRow >= 0) && (currentRow < (_model->childrenCount() - 1))) {
        if(_model->moveItem(currentRow, currentRow + 1)) {
            renderModel(false, true, currentRow + 1);
        }
    }
    setEnabled(true);
}
