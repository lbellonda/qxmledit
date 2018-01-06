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


#ifndef SCHEMAREFERENCESDIALOG_H
#define SCHEMAREFERENCESDIALOG_H

#include <QDialog>
#include "modules/namespace/nstableutils.h"

class NamespaceManager;
class NamespaceReferenceEntry;

namespace Ui
{
class SchemaReferencesDialog;
}

class SchemaReferencesDialog : public QDialog
{
    Q_OBJECT
    NsTableUtils _tableUtils;
    NamespaceManager *_namespaceManager;

    int setRow(const int desiredRow, const QString &uri, const QString &location);
    void setupData(NamespaceReferenceEntry *entry);
    void enableButtons();
    void enableOk();
    bool valueForRow(QTableWidget *table, const int row, NamespaceResult *setValues);
    void setupTable(QTableWidget *widget);
    void enableOK();
    bool okEnabled();
    void modCurrentRow();
    bool isValidReference(NamespaceResult &result);
public:
    explicit SchemaReferencesDialog(QWidget *parent, NamespaceManager *namespaceManager, NamespaceReferenceEntry *entry);
    ~SchemaReferencesDialog();

    void getResults(NamespaceReferenceEntry *result);

private:
    Ui::SchemaReferencesDialog *ui;

public slots:
    void accept();

private slots:
    void on_cmdSelectNs_clicked();
    void on_cmdModNs_clicked();
    void on_cmdAddNs_clicked();
    void on_cmdDelNs_clicked();
    void on_otherNs_cellDoubleClicked(int row, int column);
    void on_otherNs_itemSelectionChanged();
    void on_enableNoNsSchemaLocation_clicked();
    void on_noNsSchemaLocation_textChanged(const QString & text);
};

#endif // SCHEMAREFERENCESDIALOG_H
