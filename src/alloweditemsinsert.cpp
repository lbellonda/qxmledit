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

#include "alloweditemsinsert.h"
#include "xsdeditor/xschema.h"
#include "ui_alloweditemsinsert.h"
#include "widgets/selectionchoosedelegate.h"
#include "utils.h"

AIIAttribute::AIIAttribute()
{
    isChecked = false ;
    index = 0 ;
    data = NULL ;
}

AIIAttribute::~AIIAttribute()
{
}

//-----

bool ChooseItemsBySchema(QWidget *parent, XElementContent *content, QList<XSchemaObject*> *result, QList<QPair<QString, QString> > * attributes)
{
    AllowedItemsInsert dialog(content, result, attributes, parent);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false ;
}

/***************************************************************************************
AllowedItemsInsert::AllowedItemsInsert(XElementContent *content, QList<XSchemaObject*> *result, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AllowedItemsInsert)
{
    _selection = result ;
    _content = content ;
    ui->setupUi(this);

    QTreeWidgetItem *parentItem = NULL ;
    foreach(XSingleElementContent * targetContent, content->items()) {
        QTreeWidgetItem *uiItem = new QTreeWidgetItem();
        uiItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        uiItem->setText(0, targetContent->item()->name());
        uiItem->setData(0, Qt::UserRole, qVariantFromValue((void*)targetContent->item()));
        uiItem->setCheckState(0, Qt::Unchecked);
        //TODO uiItem->setText(1, targetContent->item()->userDescription());
        if(parentItem) {
            parentItem->addChild(uiItem);
        } else {
            ui->allowedItems->addTopLevelItem(uiItem);
        }
    }
}
*************************************************************************************/

AllowedItemsInsert::AllowedItemsInsert(XElementContent *content, QList<XSchemaObject*> *result, QList<QPair<QString, QString> > * resultAttributes, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AllowedItemsInsert)
{
    Utils::TODO_NEXT_RELEASE("Gestione memoria su clear, unselect");
    Utils::TODO_NEXT_RELEASE("fare gestore di undo");
    /****************** future releases ***********
    _redoing = false ;
    ***********************************************/
    _attributesResult = resultAttributes ;
    _selection = result ;
    _content = content ;
    _delegate = new SelectionChooseDelegate(this);
    ui->setupUi(this);
    ui->allowedItems->setColumnCount(2);
    ui->attributes->setItemDelegate(_delegate);

    foreach(XSingleElementContent * targetContent, content->allowedItems()) {
        addItem(targetContent, NULL);

    }
    ui->allowedItems->expandAll();
    ui->allowedItems->resizeColumnToContents(0);
    ui->allowedItems->resizeColumnToContents(1);

    setupAttributes();
    enableControls();
}

void AllowedItemsInsert::addItem(XSingleElementContent *target, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *uiItem = new QTreeWidgetItem();

    QString name = target->item()->name();
    if(name.isEmpty()) {
        name = target->item()->descriptionForProposal();
    }
    if(name.isEmpty()) {
        name = target->item()->description();
    }
    uiItem->setText(0, name);
    XSchemaAnnotation *annotation = target->item()->annotation();
    if(NULL != annotation) {
        uiItem->setText(1, annotation->text());
    }
    uiItem->setData(0, Qt::UserRole, qVariantFromValue((void*)target));
    uiItem->setCheckState(0, Qt::Unchecked);

    if(parentItem) {
        parentItem->addChild(uiItem);
    } else {
        ui->allowedItems->addTopLevelItem(uiItem);
    }
    uiItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    foreach(XSingleElementContent * child, target->children()) {
        addItem(child, uiItem);
    }
    /*
    bool isChoice = target->isChoice();
    if(isChoice) {
        uiItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        foreach(XSingleElementContent * child, target->children()) {
            addItem(child, uiItem);
        }
    } else {
        uiItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }*/
}

void AllowedItemsInsert::unselectRecursive(QTreeWidgetItem * item)
{
    item->setCheckState(0, Qt::Unchecked);
    int childrenCount = item->childCount() ;
    for(int i = 0 ; i < childrenCount ; i ++) {
        QTreeWidgetItem *child = item->child(i);
        unselectRecursive(child);
    }
}

bool AllowedItemsInsert::isSameItem(QTreeWidgetItem * item, XSingleElementContent *reference)
{
    QVariant data = item->data(0, Qt::UserRole);
    XSingleElementContent *pObject = (XSingleElementContent*)data.value<void*>();
    if(pObject->id() == reference->id()) {
        return true ;
    }
    return false;
}


void AllowedItemsInsert::on_allowedItems_itemClicked(QTreeWidgetItem * item, int /*column*/)
{
    /****************** future releases ***********
    if( _redoing ) {
        return ;
    }
    ***********************************************/

    QVariant data = item->data(0, Qt::UserRole);
    XSingleElementContent *pObject = (XSingleElementContent *)data.value<void*>();
    //if( parent is a choice, if this is selected, unselect others ) {
    QTreeWidgetItem * parentItem = item->parent() ;
    if(NULL != parentItem) {
        QVariant dataParent = parentItem->data(0, Qt::UserRole);
        XSingleElementContent *pParent = (XSingleElementContent *)dataParent.value<void*>();
        if(pParent->isChoice() && (item->checkState(0) == Qt::Checked)) {
            int childrenCount = parentItem->childCount() ;
            for(int i = 0 ; i < childrenCount ; i ++) {
                QTreeWidgetItem *child = parentItem->child(i);
                if(!isSameItem(child, pObject)) {
                    unselectRecursive(child);
                }
            }
        }

        /*if( pParent->isChoice() && item->flags(Qt::Checked) ) {
            foreach( QTQTreeWidgetItem * child, parentItem->childrenchild() ) {
                if( child != item ) {
        }*/
    }
    saveState();
    enableControls();
}

void AllowedItemsInsert::saveState()
{
    /****************** future releases ***********
        QList<QTreeWidgetItem *> items = ui->allowedItems->selectedItems () ;
        QSet<int> *values = new QSet<int>();
        if( NULL == values ) {
            Utils::errorOutOfMem();
            return ;
        }
        foreach(QTreeWidgetItem * item, items ) {
            QVariant data = item->data(0, Qt::UserRole);
            XSingleElementContent *pItem = (XSingleElementContent *)data.value<void*>();
            values->insert(pItem->id());
        }
        _undo.push(values);
        _redo.clear();
    ***********************************************/
}

void AllowedItemsInsert::restoreState()
{
    /****************** future releases ***********
    ************************************/
}

void AllowedItemsInsert::enableControls()
{
    /****************** future releases ***********
    ui->undo->setEnabled( !_undo.isEmpty() );
    ui->redo->setEnabled( !_redo.isEmpty() );
    ****************************************/
    ui->undo->setVisible(false);
    ui->redo->setVisible(false);
}

AllowedItemsInsert::~AllowedItemsInsert()
{
    delete ui;
}
#if 0
void AllowedItemsInsert::saveState()
{
    _redo.clear();
    _undo.push(currentState());
}
#endif

void AllowedItemsInsert::collectSelected(QTreeWidgetItem *parentItem)
{
    //if(parentItem->checkState(0) == Qt::Checked) {TODO
    if(parentItem->checkState(0) == Qt::Checked) {
        QVariant data = parentItem->data(0, Qt::UserRole);
        XSingleElementContent *pObject = (XSingleElementContent*)data.value<void*>();
        if(pObject->isCollectable()) {
            _selection->append(pObject->item());
        }
    }
    int childCount = parentItem->childCount();
    for(int i = 0 ; i < childCount ; i ++) {
        QTreeWidgetItem *item = parentItem->child(i);
        collectSelected(item);
    }
    //}
}

void AllowedItemsInsert::accept()
{
    setEnabled(false);
    // Look for selected items.
    // alternative: use QTreeWidgetItemIterator
    _selection->clear();
    int childCount = ui->allowedItems->topLevelItemCount();
    for(int i = 0 ; i < childCount ; i ++) {
        QTreeWidgetItem *item = ui->allowedItems->topLevelItem(i);
        collectSelected(item);
    }
    // collect attributes
    int attrRows = ui->attributes->rowCount();
    for(int row = 0 ; row < attrRows ; row ++) {
        QTableWidgetItem *lead = ui->attributes->item(row, 0);
        if(lead->checkState() == Qt::Checked) {
            QTableWidgetItem *nameItem = ui->attributes->item(row, 1);
            QTableWidgetItem *valueItem = ui->attributes->item(row, 2);
            QPair<QString, QString> newAttr(nameItem->text(), valueItem->text());
            _attributesResult->append(newAttr);
        }
    }
    // end attributes

    QDialog::accept();
}

void AllowedItemsInsert::on_undo_clicked()
{
    /****************** future releases ***********
      Utils::TODO_NEXT_RELEASE("on_undo_clicked()");
    _redoing = true ;
    state = _undo.pop();
    applyState(state);
    _redo.push(state);
    _redoing = false;
    enableControls();
    *********************************************/
}

void AllowedItemsInsert::on_redo_clicked()
{
    /****************** future releases ***********
    Utils::TODO_NEXT_RELEASE("on_redo_clicked()");
    _redoing = true ;
    state = _redo.pop();
    applyState(state);
    _undo.push(state);
    _redoing = false ;
    enableControls();
    *********************************************/
}

void AllowedItemsInsert::addAttributeItem(QTableWidget *table, AttrCollectInfo* info, const QString &name, const QString &value)
{
    int rows = table->rowCount();
    table->setRowCount(rows + 1);
    {
        QTableWidgetItem *itemMod = new QTableWidgetItem("");
        itemMod->setFlags(itemMod->flags() & (~Qt::ItemIsEditable));
        itemMod->setCheckState(Qt::Unchecked);
        itemMod->setFlags(itemMod->flags() | (Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable));
        itemMod->setData(Qt::UserRole, qVariantFromValue((void*)info));
        table->setItem(rows, 0, itemMod);
    }
    {
        Utils::TODO_NEXT_RELEASE("anche tipo");
        QTableWidgetItem *itemName = new QTableWidgetItem(name);
        itemName->setFlags(itemName ->flags() & ~(Qt::ItemIsEditable));
        itemName->setData(Qt::UserRole, qVariantFromValue((void*)info));
        table->setItem(rows, 1, itemName);
    }
    {
        QTableWidgetItem *itemValue = new QTableWidgetItem(value);
        itemValue->setFlags(itemValue->flags() & ~(Qt::ItemIsUserCheckable));
        itemValue->setData(Qt::UserRole, qVariantFromValue((void*)info));
        table->setItem(rows, 2, itemValue);
    }

}

void AllowedItemsInsert::setupAttributes()
{
    ui->attributes->setColumnCount(3);
    QStringList headers ;
    headers << tr("Use") << tr("Name") << tr("Value");
    ui->attributes->setHorizontalHeaderLabels(headers);
    if(NULL != _content->attributeContainer()) {
        foreach(QString name, _content->attributeContainer()->attributes.keys()) {
            AttrCollectInfo* attrInfo = _content->attributeContainer()->attributes[name];
            addAttributeItem(ui->attributes, attrInfo, name, attrInfo->defaultValue);
        }
    }
    ui->attributes->resizeColumnsToContents();
}
