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


#include "anonprofiledialog.h"
#include "anoneditprofiledialog.h"
#include "ui_anonprofiledialog.h"
#include "utils.h"

#define COLUMN_DATA_TAG (0)
#define COLUMN_DATA_SNIPPET (1)

AnonProfileDialog::AnonProfileDialog(QXmlEditData *data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnonProfileDialog),
    _data(data)
{
    _result = NULL ;
    ui->setupUi(this);
    refineUI();
    QTimer::singleShot(1, this, SLOT(completeUI()));
}

AnonProfileDialog::~AnonProfileDialog()
{
    foreach(GenericPersistentData * entry, _snippets.values()) {
        delete entry;
    }
    _snippets.clear();
    delete ui;
}

void AnonProfileDialog::initComplete()
{
}

void AnonProfileDialog::completeUI()
{
    loadSnippets();
    fillTree();
    initComplete();
}

void AnonProfileDialog::refineUI()
{
    ui->snippets->setColumnCount(2);
    ui->snippets->clear();
    QStringList headers;
    headers << tr("Name");
    headers << tr("Description");
    ui->snippets->setHeaderLabels(headers);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    evaluateSelection();
}

GenericPersistentData* AnonProfileDialog::result()
{
    if(NULL == _result) {
        return NULL ;
    }
    QList<GenericPersistentData*> list;
    bool isOk = true ;
    OperationStatus* oper = _data->storageManager()->readGenericData(QXMLE_PDATA_TypeAnonProfile, _result->id(), list);
    if((NULL == oper) || !oper->isOk()) {
        isOk = false;
    }
    if(NULL != oper) {
        delete oper;
    }
    if(isOk) {
        return list.first();
    }
    return NULL ;
}

void AnonProfileDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch(e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AnonProfileDialog::loadSnippets()
{
    QList<GenericPersistentData*> list;
    bool isOk = true ;
    OperationStatus* oper = _data->storageManager()->readAllGenericData(QXMLE_PDATA_TypeAnonProfile, list);
    if((NULL == oper) || !oper->isOk()) {
        isOk = false;
    }
    if(NULL != oper) {
        delete oper ;
    }
    if(isOk) {
        foreach(GenericPersistentData * searchlet, list) {
            _snippets.insert(searchlet->id(), searchlet);
        }
    }
}

void AnonProfileDialog::accept()
{
    GenericPersistentData *snippet = NULL ;
    snippet = getSelSnippet();
    if(NULL != snippet) {
        _result = snippet;
        QDialog::accept();
    } else {
        Utils::error(this, tr("No profile selected."));
        return ;
    }
}


void AnonProfileDialog::fillTree()
{
    bool isOk = true;
    ui->snippets->setUpdatesEnabled(false);
    foreach(GenericPersistentData * entry, _snippets.values()) {
        if(!insertSnippetInUI(entry)) {
            isOk = false;
        }
    }
    updateUISnipptes();
    ui->snippets->setUpdatesEnabled(true);
    if(!isOk) {
        Utils::error(this, tr("An error occurred")) ;
    }
}

void AnonProfileDialog::updateUISnipptes()
{
    ui->snippets->setSortingEnabled(false);
    int topLevelCount = ui->snippets->topLevelItemCount();
    for(int i = 0 ; i < topLevelCount ; i++) {
        QTreeWidgetItem* tagItem = ui->snippets->topLevelItem(i);
        if(!tagItem->isExpanded()) {
            tagItem->setExpanded(true);
        }
    }
    for(int i = 0 ; i < ui->snippets->columnCount() ; i ++) {
        ui->snippets->resizeColumnToContents(i);
    }
    ui->snippets->sortByColumn(0, Qt::AscendingOrder);
    ui->snippets->setSortingEnabled(true);
}

void AnonProfileDialog::deleteSnippet(GenericPersistentData* snippet)
{
    if(snippet->isReadOnly()) {
        Utils::error(this, tr("This profile can't be deleted")) ;
        return ;
    }
    bool isOk = true;
    if(deleteSnippetFromPersistence(snippet)) {
        if(!deleteSnippetFromUI(snippet, false)) {
            isOk = false;
        }
        _snippets.remove(snippet->id());
        delete snippet;
    } else {
        isOk = false ;
    }
    if(!isOk) {
        Utils::error(this, tr("An error occurred")) ;
    }
}

//togli lo s. elimina il tag padre sse e vuoto e non compreso nei nuovi tags.
bool AnonProfileDialog::deleteSnippetFromUI(GenericPersistentData* snippet, const bool shouldReInsert)
{
    bool isOk = true;
    foreach(QTreeWidgetItem * item, _snippetItems.values(snippet->id())) {
        QTreeWidgetItem *parent = item->parent();
        parent->takeChild(parent->indexOfChild(item));
        delete item ;
        QString tagLiteral = parent->data(COLUMN_DATA_TAG, Qt::UserRole).toString();
        if((parent->childCount() == 0)
                && (!shouldReInsert || (shouldReInsert && !snippet->tags().contains(tagLiteral)))) {
            int topLevelIndex = ui->snippets->indexOfTopLevelItem(parent);
            if(topLevelIndex > -1) {
                ui->snippets->takeTopLevelItem(topLevelIndex);
                delete parent;
                _topLevelItems.remove(tagLiteral);
            } else {
                isOk = false;
            }
        }
    } // foreach items

    // forget items
    _snippetItems.remove(snippet->id());

    return isOk ;
}


bool AnonProfileDialog::insertSnippetInUI(GenericPersistentData* snippet)
{
    //--------------- codice ----
    bool isOk = true;
    foreach(QString tag, snippet->tags()) {
        QTreeWidgetItem* tagItem ;
        if(! _topLevelItems.contains(tag)) {
            tagItem = new QTreeWidgetItem(ui->snippets);
            if(NULL == tagItem) {
                isOk = false;
            } else {
                _topLevelItems.insert(tag, tagItem);
                tagItem->setText(0, tag);
                tagItem->setData(COLUMN_DATA_TAG, Qt::UserRole, tag);
                tagItem->setData(COLUMN_DATA_SNIPPET, Qt::UserRole, qVariantFromValue((void*)NULL));
                ui->snippets->addTopLevelItem(tagItem);
            }
        } else {
            tagItem = _topLevelItems[tag];
        }
        // post top level processing
        QTreeWidgetItem* item = new QTreeWidgetItem(tagItem);
        if(NULL == item) {
            isOk = false;
        } else {
            item->setText(0, snippet->name());
            item->setText(1, snippet->description());
            tagItem->addChild(item);
            item->setData(COLUMN_DATA_TAG, Qt::UserRole, QVariant(""));
            item->setData(COLUMN_DATA_SNIPPET, Qt::UserRole, qVariantFromValue((void*)snippet));
            _snippetItems.insertMulti(snippet->id(), item);
        }
    } // foreach tags
    /*if( !isOk ) { TODO
       Utils::error( this, tr("An error occurred processing information." ) ) ;
    }*/
    return isOk ;
}

void AnonProfileDialog::updateSnippetInUI(GenericPersistentData* snippet)
{
    bool isOk = true ;
    if(!deleteSnippetFromUI(snippet, true)) {
        isOk = false;
    }
    if(!insertSnippetInUI(snippet)) {
        isOk = false ;
    }
    updateUISnipptes();
    if(!isOk) {
        Utils::error(this, tr("An error occurred")) ;
    }
}

void AnonProfileDialog::deleteCurrentSnippet()
{
    GenericPersistentData *snippet = getSelSnippet();
    if(NULL == snippet) {
        return ;
    }
    deleteSnippet(snippet);
}

void AnonProfileDialog::updateCurentSnippet()
{
    GenericPersistentData *snippet = getSelSnippet();
    if(NULL == snippet) {
        return ;
    }
    if(snippet->isReadOnly()) {
        Utils::error(this, tr("This profile can't be modified.")) ;
        return ;
    }
    if(editSnippet(false, snippet)) {
        if(!persistSnippet(snippet)) {
            Utils::error(this, tr("Error saving profile. Data not saved."));
        }
        updateSnippetInUI(snippet);
    }
}

void AnonProfileDialog::insertSnippet()
{
    GenericPersistentData *snippet = _data->storageManager()->newPersistentDatum(QXMLE_PDATA_TypeAnonProfile);
    if(NULL == snippet) {
        Utils::error(this, tr("Unable to create new profile."));
        return ;
    }
    AnonProfile *newProfile = new AnonProfile();
    QString newPayload = newProfile->toXMLSerializedString();
    snippet->setPayload(newPayload);
    if(editSnippet(true, snippet)) {
        if(!persistSnippet(snippet)) {
            Utils::error(this, tr("Error saving profile. Data not saved."));
        }
        _snippets.insert(snippet->id(), snippet);
        updateSnippetInUI(snippet);
    }
}

GenericPersistentData * AnonProfileDialog::insertProfile(AnonProfile *inputProfile)
{
    GenericPersistentData *snippet = _data->storageManager()->newPersistentDatum(QXMLE_PDATA_TypeAnonProfile);
    if(NULL == snippet) {
        Utils::error(parentWidget(), tr("Unable to create new profile."));
        return NULL;
    }
    QString newPayload = inputProfile->toXMLSerializedString();
    snippet->setPayload(newPayload);
    if(editSnippet(true, snippet)) {
        if(persistSnippet(snippet)) {
            return snippet;
        } else {
            Utils::error(parentWidget(), tr("Error saving profile. Data not saved."));
        }
    }
    return NULL;
}

bool AnonProfileDialog::persistSnippet(GenericPersistentData *snippet)
{
    bool isOk = true;
    OperationStatus* oper = NULL ;
    if(snippet->id() == 0) {
        oper = _data->storageManager()->insertGenericData(snippet);
    } else {
        oper = _data->storageManager()->updateGenericData(snippet);
    }
    if((NULL == oper) || !oper->isOk()) {
        isOk = false;
        Utils::error(parentWidget(), tr("Error saving data."));
    }
    if(NULL != oper) {
        delete oper;
    }
    return isOk;
}

bool AnonProfileDialog::deleteSnippetFromPersistence(GenericPersistentData *snippet)
{
    bool isOk = true;
    OperationStatus* oper = _data->storageManager()->deleteGenericData(snippet);
    if((NULL == oper) || !oper->isOk()) {
        isOk = false;
        Utils::error(this->parentWidget(), tr("Error deleting data."));
    }
    if(NULL != oper) {
        delete oper;
    }
    return isOk;
}

bool AnonProfileDialog::editSnippet(const bool isInsert, GenericPersistentData *snippet)
{
    AnonEditProfileDialog dlg(isInsert, snippet, this);
    return dlg.editProfile();
}

GenericPersistentData *AnonProfileDialog::getSelSnippet()
{
    if(ui->snippets->selectedItems().size() > 0) {
        QTreeWidgetItem *item = ui->snippets->selectedItems().at(0);
        if(NULL != ui) {
            QVariant data = item->data(COLUMN_DATA_SNIPPET, Qt::UserRole);
            GenericPersistentData *snippet = (GenericPersistentData *)data.value<void*>();
            return snippet ;
        }
    }
    return NULL;
}


void AnonProfileDialog::on_newCmd_clicked()
{
    insertSnippet();
}

void AnonProfileDialog::on_modifyCmd_clicked()
{
    updateCurentSnippet();
}

void AnonProfileDialog::on_deleteCmd_clicked()
{
    deleteCurrentSnippet();
}

void AnonProfileDialog::on_snippets_itemSelectionChanged()
{
    evaluateSelection();
}

void AnonProfileDialog::evaluateSelection()
{
    GenericPersistentData *snippet = getSelSnippet();
    bool isEnabledOk = false ;
    bool isEnabledSel = false ;
    if(NULL != snippet) {
        isEnabledSel = !snippet->isReadOnly();
        isEnabledOk = true ;
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isEnabledOk);
    ui->deleteCmd->setEnabled(isEnabledSel);
    ui->modifyCmd->setEnabled(isEnabledSel);
}

void AnonProfileDialog::on_snippets_itemDoubleClicked(QTreeWidgetItem * item, int /*column*/)
{
    if(NULL != item) {
        accept();
    }
}
