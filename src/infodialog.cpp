/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#include "infodialog.h"
#include "ui_infodialog.h"
#include "regola.h"
#include "utils.h"

void showInfo(QWidget *parent, Regola *regola)
{
    InfoDialog infoDialog(parent, regola);
    infoDialog.exec();
}

InfoDialog::InfoDialog(QWidget *parent, Regola *regola) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    _regola = regola;
    ui->setupUi(this);
    setupInfo();
}

InfoDialog::~InfoDialog()
{
    delete ui;
}


void InfoDialog::setupInfo()
{
    // 1- encoding
    setEncodingInfo();

    //1.1 file info
    setFileInfo();

    // 2 - XMLSchema
    setSchemaReferenceInfo();

    // 3 -namespaces
    setNamespaceInfo();
}

void InfoDialog::setFileInfo()
{
    if(_regola->fileName().isEmpty()) {
        ui->fileInfo->setText(tr("No file information."));
    } else {
        QFileInfo fileInfo(_regola->fileName());
        fileInfo.refresh();
        QString filePath = fileInfo.absoluteFilePath();
        bool isWritable = fileInfo.isWritable();
        QDateTime creationTime = fileInfo.created();
        QDateTime lastModTime = fileInfo.lastModified();
        QString owner = fileInfo.owner();
        qint64 size = fileInfo.size();

        QString info1 = Utils::escapeHTML(tr("Path: %1").arg(filePath));
        QString info2 = Utils::escapeHTML(tr("Size: %1 (%2 bytes) %3").arg(Utils::getSizeForPresentation(size)).arg(size).arg((isWritable ? tr("writable") : "")));
        QString info3 = Utils::escapeHTML(tr("Created on: %1 by %2").arg(creationTime.toString(Qt::DefaultLocaleLongDate)).arg(owner));
        QString info4 = Utils::escapeHTML(tr("Last modified on: %1").arg(lastModTime.toString(Qt::DefaultLocaleLongDate)));

        QString info = QString("<html><body>%1<br/>%2<br/>%3<br/>%4<br/></body></html>").arg(info1).arg(info2).arg(info3).arg(info4);

        ui->fileInfo->setTextFormat(Qt::RichText);
        ui->fileInfo->setText(info);
    }
}

void InfoDialog::setEncodingInfo()
{
    ui->lblEncoding->setText(_regola->encoding());
}

void InfoDialog::setSchemaReferenceInfo()
{
    QHash<QString, QString> references = _regola->mapOfSchemaReferences();
    bool referencesEmpty = references.isEmpty() && _regola->noNameSpaceXsd().isEmpty();
    ui->lblNoSchemaReferences->setVisible(referencesEmpty);
    ui->tableReferences->setVisible(!referencesEmpty);
    if(!referencesEmpty) {
        ui->tableReferences->setColumnCount(3);
        QStringList labels ;
        labels << tr("Namespace") << tr("URI") << tr("role");
        ui->tableReferences->setHorizontalHeaderLabels(labels);
        foreach(QString key, references.keys()) {
            QString uri = references[key];
            QString role = Regola::roleForKnownUri(uri);
            insSchemaReference(key, uri, role);
        }
        if(!_regola->noNameSpaceXsd().isEmpty()) {
            QString uri = _regola->noNameSpaceXsd();
            QString role = Regola::roleForKnownUri(uri);
            insSchemaReference("", uri, role);
        }
        ui->tableReferences->resizeColumnsToContents();
    }
}

void InfoDialog::insSchemaReference(const QString &ns, const QString &uri, const QString &role)
{
    int row = ui->tableReferences->rowCount();
    ui->tableReferences->setRowCount(row + 1);
    QTableWidgetItem *item0 = new QTableWidgetItem(ns);
    ui->tableReferences->setItem(row, 0, item0);
    QTableWidgetItem *item1 = new QTableWidgetItem(uri);
    ui->tableReferences->setItem(row, 1, item1);
    QTableWidgetItem *item2 = new QTableWidgetItem(role);
    ui->tableReferences->setItem(row, 2, item2);
}

void InfoDialog::setNamespaceInfo()
{
    QHash<QString, QSet<QString> > nameSpacesMap = _regola->allNamespaces();
    bool referencesEmpty = nameSpacesMap.isEmpty() ;
    ui->lblNoNamespace->setVisible(referencesEmpty);
    ui->tableNamespaces->setVisible(!referencesEmpty);
    if(!referencesEmpty) {
        ui->tableNamespaces->setColumnCount(3);
        QStringList labels ;
        labels << tr("Prefix") << tr("Namespace URI") << tr("role");
        ui->tableNamespaces->setHorizontalHeaderLabels(labels);
        foreach(QString nsUri, nameSpacesMap.keys()) {
            QSet<QString> prefixes = nameSpacesMap[nsUri];
            QString role = Regola::roleForKnownUri(nsUri);
            foreach(QString prefix, prefixes.values()) {
                insNamespace(prefix, nsUri, role);
            }
        }
        ui->tableNamespaces->resizeColumnsToContents();
    }
}

void InfoDialog::insNamespace(const QString &ns, const QString &uri, const QString &role)
{
    int row = ui->tableNamespaces->rowCount();
    ui->tableNamespaces->setRowCount(row + 1);
    QTableWidgetItem *item0 = new QTableWidgetItem(ns);
    ui->tableNamespaces->setItem(row, 0, item0);
    QTableWidgetItem *item1 = new QTableWidgetItem(uri);
    ui->tableNamespaces->setItem(row, 1, item1);
    QTableWidgetItem *item2 = new QTableWidgetItem(role);
    ui->tableNamespaces->setItem(row, 2, item2);
}
