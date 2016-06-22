/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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
#include "schemachooser.h"
#include "ui_schemachooser.h"
#include "SchemaInterface.h"
#include "qxmleditconfig.h"
#include "utils.h"
#include "qxmleditdata.h"
#include <QInputDialog>
#include <QFileDialog>

QString chooseSchemaFile(QWidget *parent, bool &isOk)
{
    QString result ;
    isOk = false;
    SchemaChooser dialog(parent);
    if(dialog.exec() == QDialog::Accepted) {
        isOk = true ;
        result = dialog.selection();
    }
    return result;
}

SchemaChooser::SchemaChooser(QWidget *parent/*, QNetworkAccessManager *networkAccessManager*/) :
    QDialog(parent),
    _started(false),
    ui(new Ui::SchemaChooser)
{
    ui->setupUi(this);
    enableOK(false);
    ui->remPrefsSchema->setEnabled(false);
    ui->favoritesList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->lastUsedList->setSelectionMode(QAbstractItemView::SingleSelection);
    loadPreferredData();
    loadLastUsedData();
    _started = true ;
}


SchemaChooser::~SchemaChooser()
{
    delete ui;
}


QString SchemaChooser::selection()
{
    return _selection;
}

void SchemaChooser::loadPreferredData()
{
    _preferitesFilesNames.clear();
    Config::loadStringArray(Config::KEY_SCHEMA_PREFERITES, _preferitesFilesNames);
    foreach(QString file, _preferitesFilesNames) {
        /*QListWidgetItem *item = */new QListWidgetItem(file, ui->favoritesList) ;
    }
}

void SchemaChooser::loadLastUsedData()
{
    Config::loadStringArray(Config::KEY_SCHEMA_LAST, _lastFilesNames);
    foreach(QString file, _lastFilesNames) {
        /*QListWidgetItem *item = */new QListWidgetItem(file, ui->lastUsedList) ;
    }
}

void SchemaChooser::saveSelection()
{
    _lastFilesNames.removeAll(_selection);
    _lastFilesNames.insert(0, _selection);
    while(_lastFilesNames.size() > MAX_LAST_FILES) {
        _lastFilesNames.removeLast();
    }
    Config::saveStringArray(Config::KEY_SCHEMA_LAST, _lastFilesNames);
}


void SchemaChooser::on_lastUsedList_currentItemChanged(QListWidgetItem * current, QListWidgetItem * /*previous*/)
{
    if(NULL != current) {
        setSelection(current->text());
    }
}

void SchemaChooser::on_lastUsedList_itemClicked(QListWidgetItem * item)
{
    if(_started) {
        if(NULL != item) {
            setSelection(item->text());
        }
    }
}

void SchemaChooser::on_lastUsedList_itemDoubleClicked(QListWidgetItem * item)
{
    if(NULL != item) {
        setSelection(item->text());
        accept();
    }
}

void SchemaChooser::on_favoritesList_currentItemChanged(QListWidgetItem * current, QListWidgetItem * /*previous*/)
{
    if(!_started) {
        if(NULL != current) {
            setSelection(current->text());
        }
    }
    ui->remPrefsSchema->setEnabled(NULL != current);
}

void SchemaChooser::on_favoritesList_itemClicked(QListWidgetItem * item)
{
    if(NULL != item) {
        setSelection(item->text());
    }
}

void SchemaChooser::on_favoritesList_itemDoubleClicked(QListWidgetItem * item)
{
    if(NULL != item) {
        setSelection(item->text());
        accept();
    }
}

void SchemaChooser::setSelection(const QString &newSelection)
{
    _selection = newSelection ;
    ui->textSelection->setText(_selection);
}

void SchemaChooser::on_addPrefsSchema_clicked()
{
    bool isOk;
    QString newUrl = QInputDialog::getText(this,
                                           tr("Add a new schema URL to Preferites"),
                                           tr("Schema URL:"),
                                           QLineEdit::Normal,
                                           "",
                                           &isOk);
    if(isOk && !newUrl.isEmpty()) {
        if(_preferitesFilesNames.contains(newUrl)) {
            Utils::error(this, tr("The URL is already present in the favorite ones."));
            return ;
        }
        _preferitesFilesNames.append(newUrl);
        QListWidgetItem *newItem = new QListWidgetItem(newUrl, ui->favoritesList) ;
        ui->favoritesList->setCurrentItem(newItem);
        Config::saveStringArray(Config::KEY_SCHEMA_PREFERITES, _preferitesFilesNames);
    }
}

void SchemaChooser::on_remPrefsSchema_clicked()
{
    QString urlToRemove;
    QListWidgetItem *item = ui->favoritesList->currentItem();
    if(NULL != item) {
        urlToRemove = item->text();
    }
    if(urlToRemove.isEmpty()) {
        Utils::errorNoSel(this);
        return ;
    }
    _preferitesFilesNames.removeAll(urlToRemove);
    Config::saveStringArray(Config::KEY_SCHEMA_PREFERITES, _preferitesFilesNames);
    ui->favoritesList->clear();
    loadPreferredData();
}

void SchemaChooser::accept()
{
    if(!_selection.isEmpty()) {
        saveSelection();
        QDialog::accept();
    } else {
        Utils::errorNoSel(this);
    }
}

void SchemaChooser::on_browseFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Choose a File "),
                       QXmlEditData::sysFilePathForOperation(ui->textSelection->text()), tr("XSD files (*.xsd);;All files (*)"));
    if(!filePath.isEmpty()) {
        ui->textSelection->setText(filePath);
    }
}

void SchemaChooser::enableOK(const bool isEnabled)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isEnabled);
}

void SchemaChooser::on_textSelection_textChanged(const QString & text)
{
    enableOK(!text.isEmpty());
    _selection = text ;
}
