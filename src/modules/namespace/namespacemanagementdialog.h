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


#ifndef NAMESPACEMANAGEMENTDIALOG_H
#define NAMESPACEMANAGEMENTDIALOG_H

#include "xmlEdit.h"
#include <QButtonGroup>
#include "modules/namespace/namespacecommands.h"
#include "modules/namespace/nstableutils.h"

namespace Ui
{
class NamespaceManagementDialog;
}

class NamespaceManager;
class Element;

class NamespaceManagementDialog : public QDialog
{
    Q_OBJECT

    QButtonGroup _group;
    Element *_element;
    NamespaceManager *_nsManager;
    NsTableUtils _tableUtils;
    QHash<QString, QString> _visibleNamespaces ;
    bool _goComboNs ;

public:
    explicit NamespaceManagementDialog(QWidget *parent, Element *element, NamespaceManager *nsManager);
    ~NamespaceManagementDialog();

    NamespaceCommands *getCommands();

private:
    Ui::NamespaceManagementDialog *ui;

    void loadOtherDeclarations();
    void retrieveUriDescription(const QString & text, NamespaceResult *data);
    void init();
    void setupData();
    void decodePrefix();
    void updateButtonsNs();
    void enableOk();

private slots:
    void on_cmdRemNs_clicked();
    void on_nsList_itemSelectionChanged();
    void on_cmdAddNs_clicked();
    void on_cmdModNs_clicked();
    void on_cmdSelect_clicked();
    void on_cmdClear_clicked();
    void on_uri_currentIndexChanged(int index);
    void on_nsList_itemDoubleClicked(QTableWidgetItem * /*item*/);
    void on_prefix_textChanged(const QString & text);
    void on_uri_editTextChanged(const QString & text);
};

#endif // NAMESPACEMANAGEMENTDIALOG_H
