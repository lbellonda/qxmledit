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
#include "snippethandler.h"
#include "ui_snippethandler.h"
#include "snippet_interface.h"
#include "editsnippet.h"
#include "utils.h"

#define COLUMN_DATA_TAG (0)
#define COLUMN_DATA_SNIPPET (1)

SnippetManager::SnippetManager()
{
}

SnippetManager::~SnippetManager()
{
}

void SnippetManager::editSnippets(ApplicationData *data, QWidget *parent)
{
    SnippetHandler handler(data, true, parent);
    handler.setModal(true);
    handler.exec();
}

Regola* SnippetManager::chooseSnippets(ApplicationData *data, QWidget *parent)
{
    SnippetHandler handler(data, false, parent);
    handler.setModal(true);
    if(handler.exec() == QDialog::Accepted) {
        return handler.regola();
    }
    return NULL;
}

void SnippetManager::insertFragmentInSnippets(QWidget *parent, ApplicationData *data, const QString &text)
{
    SnippetHandler handler(data, true, parent);
    handler.insertFragment(parent, text);
}

//------------------------------------------------------------------

SnippetHandler::SnippetHandler(ApplicationData *data, const bool setEditMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnippetHandler),
    _isEditMode(setEditMode),
    _regola(NULL),
    _data(data)
{
    ui->setupUi(this);
    refineUI();
    QTimer::singleShot(1, this, SLOT(completeUI()));
    // exit from the constructor...
}


SnippetHandler::~SnippetHandler()
{
    Snippet::resetSnippetContainer(_snippets);
    delete ui;
}

void SnippetHandler::initComplete()
{
}

void SnippetHandler::completeUI()
{
    loadSnippets();
    fillTree();
    initComplete();
}

void SnippetHandler::refineUI()
{
    ui->snippets->setColumnCount(2);
    ui->snippets->clear();
    QStringList headers;
    headers << tr("Name");
    headers << tr("Description");
    ui->snippets->setHeaderLabels(headers);
    evaluateSelection();
}

Regola *SnippetHandler::regola()
{
    return _regola;
}

void SnippetHandler::changeEvent(QEvent *e)
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

void SnippetHandler::loadSnippets()
{
    //Snippet::loadMockSnippet(_snippets);
    if(! scanDirectory(_data->snippetsLocalDir(), false)) {
        Utils::error(this, tr("Error reading user's snippets."));
    }
    if(! scanDirectory(_data->snippetsProgramDir(), true)) {
        Utils::error(this, tr("Error reading snippets."));
    }
}

void SnippetHandler::accept()
{
    bool isOk = false ;
    if(!_isEditMode) {
        QDomDocument document ;
        Snippet *snippet = NULL ;
        snippet = getSelSnippet();
        if(NULL != snippet) {
            if(document.setContent(snippet->payload())) {
                _regola = new Regola(document, "");
                isOk = true;
            }
        } else {
            Utils::error(this, tr("No snippet selected."));
            return ;
        }
    } else {
        // if in edit, close window.
        isOk = true ;
    }
    if(isOk) {
        QDialog::accept();
    } else {
        Utils::error(this, tr("Error performing operation."));
    }
}


void SnippetHandler::fillTree()
{
    bool isOk = true;
    ui->snippets->setUpdatesEnabled(false);
    foreach(Snippet * entry, _snippets.values()) {
        if(!insertSnippetInUI(entry)) {
            isOk = false;
        }
    }

    int topLevelCount = ui->snippets->topLevelItemCount();
    for(int i = 0 ; i < topLevelCount ; i++) {
        QTreeWidgetItem* tagItem = ui->snippets->topLevelItem(i);
        tagItem->setExpanded(true);
    }
    for(int i = 0 ; i < ui->snippets->columnCount() ; i ++) {
        ui->snippets->resizeColumnToContents(i);
    }
    ui->snippets->setUpdatesEnabled(true);
    if(!isOk) {
        Utils::error(this, tr("An error occurred")) ;
    }
}

void SnippetHandler::deleteSnippet(Snippet* snippet)
{
    if(snippet->isReadOnly()) {
        Utils::error(this, tr("This snipped can't be deleted")) ;
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

//togli lo s. elimina il tag padre sse e vuoto e non cmopreso nei nuovi tags.
bool SnippetHandler::deleteSnippetFromUI(Snippet* snippet, const bool shouldReInsert)
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

    /*if( !isOk ) { TODO
        Utils::error( this, tr("An error occurred") ) ;
    }*/
    return isOk ;
}


bool SnippetHandler::insertSnippetInUI(Snippet* snippet)
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
       Utils::error( this, tr("An error occurred processing informations." ) ) ;
    }*/
    return isOk ;
}

void SnippetHandler::updateSnippetInUI(Snippet* snippet)
{
    bool isOk = true ;
    if(!deleteSnippetFromUI(snippet, true)) {
        isOk = false;
    }
    if(!insertSnippetInUI(snippet)) {
        isOk = false ;
    }
    if(!isOk) {
        Utils::error(this, tr("An error occurred")) ;
    }
}

void SnippetHandler::deleteCurrentSnippet()
{
    Snippet *snippet = getSelSnippet();
    if(NULL == snippet) {
        return ;
    }
    deleteSnippet(snippet);
}

void SnippetHandler::updateCurentSnippet()
{
    Snippet *snippet = getSelSnippet();
    if(NULL == snippet) {
        return ;
    }
    if(snippet->isReadOnly()) {
        Utils::error(this, tr("This snipped can't be modified.")) ;
        return ;
    }
    if(editSnippet(false, snippet)) {
        if(!persistSnippet(this, snippet)) {
            Utils::error(this, tr("Error saving snippet. Data not saved."));
        }
        updateSnippetInUI(snippet);
    }
}

void SnippetHandler::insertSnippet()
{
    Snippet *snippet = new Snippet();
    if(NULL == snippet) {
        Utils::error(this, tr("Unable to create new snippet."));
        return ;
    }

    if(editSnippet(true, snippet)) {
        if(!persistSnippet(this, snippet)) {
            Utils::error(this, tr("Error saving snippet. Data not saved."));
        }
        _snippets.insert(snippet->id(), snippet);
        updateSnippetInUI(snippet);
    }
}

void SnippetHandler::insertFragment(QWidget *parent, const QString &text)
{
    Snippet *snippet = new Snippet();
    if(NULL == snippet) {
        Utils::error(parent, tr("Unable to create new snippet."));
        return ;
    }
    snippet->setPayload(text);
    if(::editSnippet(true, snippet, parent)) {
        if(!persistSnippet(parent, snippet)) {
            Utils::error(parent, tr("Error saving snippet. Data not saved."));
        }
    }
}


bool SnippetHandler::persistSnippet(QWidget *parent, Snippet *snippet)
{
    QString dataDir = _data->snippetsLocalDir();
    QDir dir(dataDir);
    if(!dir.exists()) {
        if(!dir.mkpath(dataDir)) {
            Utils::error(parent, tr("Error creating data directory."));
            return false;
        }
    }
    QString filePath = buildFilePath(dataDir, snippet) ;
    // scrivi il nuovo con il nome vecchio+estensione
    QString newFilePath = filePath + ".new~" ;
    if(!snippet->save(newFilePath)) {
        Utils::error(parent, tr("Error saving data. Old file is still intact."));
        return false ;
    }
    // rinomina il vecchio
    QString backupFilePath = filePath + "~"  ;
    if(QFile::exists(backupFilePath)) {
        if(!QFile::remove(backupFilePath)) {
            Utils::error(parent, tr("Error saving data: cleaning backup file."));
            return false ;
        }
    }
    if(QFile::exists(filePath)) {
        if(!QFile::rename(filePath, backupFilePath)) {
            Utils::error(parent, tr("Error renaming old file. You can access written data at file '%1'. Old data are untouched").arg(newFilePath));
            return false ;
        }
    }
    // rinomina il nuovo nel nome vecchio
    if(!QFile::rename(newFilePath, filePath)) {
        Utils::error(parent, tr("Error renaming new file. You can access written data at file '%1'. You can find old data in the backup file").arg(newFilePath));
        return false ;
    }
    return true;
}

bool SnippetHandler::deleteSnippetFromPersistence(Snippet *snippet)
{
    QString dataDir = _data->snippetsLocalDir();
    QDir dir(dataDir);
    if(!dir.exists()) {
        if(!dir.mkpath(dataDir)) {
            Utils::error(this, tr("Error creating data directory."));
            return false;
        }
    }
    QString filePath = buildFilePath(dataDir, snippet);
    return snippet->remove(filePath);
}

bool SnippetHandler::editSnippet(const bool isInsert, Snippet *snippet)
{
#ifdef  _TEST
    QStringList list = snippet->tags();
    QStringList newList ;
    bool isFirst = true;
    foreach(QString s, list) {
        if(isFirst) {
            isFirst = false ;
            newList.append(s);
        } else {
            QString n = s + "_x";
            newList.append(n);
        }
    }
    snippet->setTags(newList);
    return true;
#endif
    return ::editSnippet(isInsert, snippet, this);
}

Snippet *SnippetHandler::getSelSnippet()
{
    if(ui->snippets->selectedItems().size() > 0) {
        QTreeWidgetItem *item = ui->snippets->selectedItems().at(0);
        // att ai top level TODO i top lvevel hanno un NULL
        if(NULL != ui) {
            QVariant data = item->data(COLUMN_DATA_SNIPPET, Qt::UserRole);
            Snippet *snippet = (Snippet *)data.value<void*>();
            return snippet ;
        }
    }
    return NULL;
}


void SnippetHandler::on_newCmd_clicked()
{
    insertSnippet();
}

void SnippetHandler::on_modifyCmd_clicked()
{
    updateCurentSnippet();
}

void SnippetHandler::on_deleteCmd_clicked()
{
    deleteCurrentSnippet();
}

bool SnippetHandler::scanDirectory(const QString &dirName, const bool isReadOnly)
{
    bool isOK = true ;
    QDir dir(dirName);
    if(!dir.exists()) {
        return true;
    }
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    QStringList names;
    names.append("*.xml");
    dir.setNameFilters(names);
    QFileInfoList fileList = dir.entryInfoList();
    foreach(QFileInfo info, fileList) {
        Snippet *snippet = new Snippet();
        if(NULL == snippet) {
            isOK = false;
        } else {
            snippet->setReadOnly(isReadOnly);
            if(!snippet->read(info.absoluteFilePath())) {
                isOK = false ;
                delete snippet;
            } else {
                _snippets.insert(snippet->id(), snippet);
            }
        }
    }
    return isOK;
}

void SnippetHandler::on_snippets_itemSelectionChanged()
{
    evaluateSelection();
}

void SnippetHandler::evaluateSelection()
{
    Snippet *snippet = getSelSnippet();
    if(_isEditMode) {
        bool isEnabledSel = false ;
        if(NULL != snippet) {
            isEnabledSel = !snippet->isReadOnly();
        }
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(false);
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(false);
        ui->buttonBox->button(QDialogButtonBox::Close)->setEnabled(true);
        ui->buttonBox->button(QDialogButtonBox::Close)->setVisible(true);
        ui->deleteCmd->setEnabled(isEnabledSel);
        ui->modifyCmd->setEnabled(isEnabledSel);
    } else {
        bool isEnabledOk = false ;
        if(NULL != snippet) {
            isEnabledOk = true ;
        }
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isEnabledOk);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(true);
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(true);
        ui->buttonBox->button(QDialogButtonBox::Close)->setEnabled(false);
        ui->buttonBox->button(QDialogButtonBox::Close)->setVisible(false);
        ui->deleteCmd->setEnabled(false);
        ui->modifyCmd->setEnabled(false);
        ui->newCmd->setEnabled(false);
        ui->deleteCmd->setVisible(false);
        ui->modifyCmd->setVisible(false);
        ui->newCmd->setVisible(false);

    }
}

QString SnippetHandler::buildFilePath(const QString &dirPath, Snippet* snippet)
{
    QString filePath = dirPath + QDir::separator() + snippet->id() + ".xml";
    return filePath ;
}

void SnippetHandler::on_snippets_itemDoubleClicked(QTreeWidgetItem * /*item*/, int /*column*/)
{
    if(_isEditMode) {
        updateCurentSnippet();
    } else {
        accept();
    }
}
