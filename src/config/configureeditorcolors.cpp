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


#include "configureeditorcolors.h"
#include "config.h"
#include "utils.h"
#include "ui_configureeditorcolors.h"
#include <QColorDialog>

#define SAMPLE_DATA ":colors/sampleData.xml"

ConfigureEditorColors::ConfigureEditorColors(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureEditorColors)
{
    _fileFilter = Utils::getFileFilterForCSV();
    _started = false;
    _data = NULL ;
    ui->setupUi(this);
    ui->treeColors->setColumnCount(2);
    QStringList treeLabels ;
    treeLabels << tr("Color");
    treeLabels << tr("Sample");
    ui->treeColors->setHeaderLabels(treeLabels);
    enableButtons();
    loadSampleData();
    ui->editor->setReadOnly(true);
    ui->editor->setEncodingLabelVisible(false);
    ui->editor->showButtons(false);
    ui->editor->showToolbar(false);
    _started = true;
}

ConfigureEditorColors::~ConfigureEditorColors()
{
    delete ui;
}

void ConfigureEditorColors::init(ApplicationData* data)
{
    _data = data ;
    ui->editor->setData(qApp, data, false, NULL);
    loadList();
    enableButtons();
    _started = true ;
}


void ConfigureEditorColors::saveIfChanged()
{
    // do nothing
}

void ConfigureEditorColors::enableButtons()
{
    bool selection = (ui->treeColors->currentItem() != NULL) ? true : false  ;
    ui->cmdChangeColor->setEnabled(selection);
    ui->cmdResetColor->setEnabled(selection);
}

void ConfigureEditorColors::on_treeColors_itemSelectionChanged()
{
    enableButtons();
}

void ConfigureEditorColors::on_treeColors_itemDoubleClicked(QTreeWidgetItem * /*item*/, int /*column*/)
{
    on_cmdChangeColor_clicked();
}

void ConfigureEditorColors::on_cmdResetColor_clicked()
{
    QTreeWidgetItem *item = selItem();
    if(NULL != item) {
        ColorEntry *entry = entryOfItem(item);
        if(NULL != entry) {
            entry->reset();
            updateEntry(item);
            _data->colorManager()->saveConfiguration();
            _data->updateStyleVersion();
            updateSample();
        }
    }
}

void ConfigureEditorColors::on_cmdChangeColor_clicked()
{
    QTreeWidgetItem *item = selItem();
    if(NULL != item) {
        ColorEntry *entry = entryOfItem(item);
        if(NULL != entry) {
            QColor color = QColorDialog::getColor(entry->color(), this, tr("Choose the color"));
            if(color.isValid()) {
                entry->setColor(color);
                updateEntry(item);
                _data->colorManager()->saveConfiguration();
                _data->updateStyleVersion();
                updateSample();
            }
        }
    }
}

void ConfigureEditorColors::on_cmdResetAllColors_clicked()
{
    _data->colorManager()->resetColors();
    _data->colorManager()->saveConfiguration();
    _data->updateStyleVersion();
    loadList();
}

ColorEntry *ConfigureEditorColors::selEntry()
{
    QTreeWidgetItem *selectionItem = ui->treeColors->currentItem();
    if(NULL != selectionItem) {
        QVariant data = selectionItem->data(0, Qt::UserRole);
        return (ColorEntry*)data.value<void*>();
    }
    return NULL ;
}

QTreeWidgetItem *ConfigureEditorColors::selItem()
{
    QTreeWidgetItem *selectionItem = ui->treeColors->currentItem();
    return selectionItem ;
}

ColorEntry *ConfigureEditorColors::entryOfItem(QTreeWidgetItem *item)
{
    ColorEntry *entry = NULL ;
    if(NULL != item) {
        QVariant data = item->data(0, Qt::UserRole);
        entry = (ColorEntry*)data.value<void*>();
    }
    return entry ;
}

void ConfigureEditorColors::updateEntry(QTreeWidgetItem *item)
{
    if(NULL != item) {
        ColorEntry *entry = entryOfItem(item);
        if(NULL != entry) {
            item->setTextColor(1, entry->color());
            item->setData(0, Qt::DecorationRole, entry->color());
        }
    }
}

void ConfigureEditorColors::loadList()
{
    int selection = -1 ;
    QTreeWidgetItem *item = ui->treeColors->currentItem();
    if(NULL != item) {
        selection = ui->treeColors->indexOfTopLevelItem(item);
    }
    ui->treeColors->clear();
    foreach(ColorEntry * entry, _data->colorManager()->colors()) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, translateKey(entry->key()));
        item->setText(1, translateKey(entry->key()));
        item->setData(0, Qt::UserRole, qVariantFromValue((void*)entry));
        updateEntry(item);
        ui->treeColors->addTopLevelItem(item);
    }
    if(selection != -1) {
        QTreeWidgetItem *item = ui->treeColors->topLevelItem(selection);
        if(NULL != item) {
            ui->treeColors->setCurrentItem(item);
        }
    }
    ui->treeColors->resizeColumnToContents(0);
    ui->treeColors->resizeColumnToContents(1);
    enableButtons();
    updateSample();
}

QString ConfigureEditorColors::translateKey(const QString &key)
{
    if(key == Config::KEY_COLOR_TAGS) {
        return tr("tags");
    }
    if(key == Config::KEY_COLOR_DECL_PROCINSTR) {
        return tr("processing instruction");
    }
    if(key == Config::KEY_COLOR_ATTRNAMES) {
        return tr("attribute names");
    }
    if(key == Config::KEY_COLOR_ATTRVALUES) {
        return tr("attribute values");
    }
    if(key == Config::KEY_COLOR_COMMENTS) {
        return tr("comments");
    }
    if(key == Config::KEY_COLOR_INFO) {
        return tr("info");
    }
    if(key == Config::KEY_COLOR_TEXTDECL) {
        return tr("text");
    }
    if(key == Config::KEY_COLOR_ANONELPREVIEW) {
        return tr("anonymized element in preview");
    }
    if(key == Config::KEY_COLOR_ANONTEXTPREVIEW) {
        return tr("anonymized text in preview");
    }
    if(key == Config::KEY_COLOR_ANONATTRPREVIEW) {
        return tr("anonymized attribute in preview");
    }
    return key;
}

void ConfigureEditorColors::on_cmdExport_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(
                           this, tr("Export"),
                           QXmlEditData::sysFilePathForOperation(_lastFolder),
                           _fileFilter);
    if(!filePath.isEmpty()) {
        _lastFolder = filePath ;
        QFile file(filePath);
        if(file.open(QIODevice::WriteOnly)) {
            QTextStream textStream(&file);
            textStream.setCodec("UTF-8");
            writeExport(textStream);
            file.flush();
            if(file.error() != QFile::NoError) {
                Utils::error(this, tr("Error exporting data"));
            }
            file.close();
        } else {
            Utils::error(this, tr("Error opening file"));
        }
    }
}


void ConfigureEditorColors::on_cmdImport_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
                           this, tr("Import"),
                           QXmlEditData::sysFilePathForOperation(_lastFolder),
                           _fileFilter);
    if(!filePath.isEmpty()) {
        _lastFolder = filePath ;
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly)) {
            QTextStream textStream(&file);
            textStream.setCodec("UTF-8");
            import(textStream);
            if(file.error() != QFile::NoError) {
                Utils::error(this, tr("Error importing data"));
            }
            file.close();
        } else {
            Utils::error(this, tr("Error opening file"));
        }
    }
    loadList();
    enableButtons();
}

void ConfigureEditorColors::writeExport(QTextStream &stream)
{
    foreach(ColorEntry * entry, _data->colorManager()->colors()) {
        stream << "\"";
        stream << entry->key();
        stream << "\",\"";
        QColor color = entry->color();
        stream << Utils::fmtNumber(true, (color.red() << 16) + (color.green() << 8) + color.blue(), 6, false).toUpper();
        stream << "\"\n";
    }
    stream.flush();
}


void ConfigureEditorColors::import(QTextStream &stream)
{
    while(!stream.atEnd()) {
        QString row = stream.readLine();
        QStringList values = row.split(",");
        if(values.size() > 1) {
            QString key = values.at(0);
            QString value = values.at(1);
            key = key.replace("\"", "");
            value = value.replace("\"", "");
            ColorEntry *color = _data->colorManager()->colorFromKey(key);
            if(NULL != color) {
                color->setColorFromString(value);
            }
        }
    }
    _data->updateStyleVersion();
}



void ConfigureEditorColors::loadSampleData()
{
    QFile file(SAMPLE_DATA);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            ui->editor->setDocument(document, "", false);
            ui->editor->setReadOnly(true);
        }
        file.close();
    }
}

void ConfigureEditorColors::updateSample()
{
    ui->editor->invalidatePaintData();
}

