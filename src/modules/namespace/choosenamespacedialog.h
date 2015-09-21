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


#ifndef CHOOSENAMESPACEDIALOG_H
#define CHOOSENAMESPACEDIALOG_H

#include "xmlEdit.h"
#include "modules/namespace/nstableutils.h"
#include "modules/namespace/namespaceresult.h"

namespace Ui
{
class ChooseNamespaceDialog;
}

class NamespaceManager ;
class UserNamespace;
class NamespaceDef;

class ChooseNamespaceDialog : public QDialog
{
    Q_OBJECT

    NamespaceManager *_nsManager;
    NsTableUtils _tableUtils;

public:
    explicit ChooseNamespaceDialog(QWidget *parent, NamespaceManager *nsManager, NamespaceResult *base);
    ~ChooseNamespaceDialog();

    void nsChosen(NamespaceResult *returnValue);
    void enablePrefix(const bool isEnabled);

private:
    Ui::ChooseNamespaceDialog *ui;

    void loadData(NamespaceResult *base);
    void loadPredefinedNs();
    void enableButtons();
    void loadUserNs();
    void setUserNamespace(const int row, UserNamespace *ns);
    bool editNamespace(UserNamespace *ns);
    bool persistNamespace(UserNamespace *ns);
    UserNamespace *namespaceForRow(const int row);
    NamespaceDef *predefNamespaceForRow(const int row);
    void setValues(const QString &prefix, const QString &uri, const QString &schemaLocation, const QString &description);
    bool nsIsLegal(const QString &prefix, const QString &uri);
    void enableUNButtons();
    QString retrieveUriDescription(const QString & text, QString &schemaLocation);

private slots:
    void on_cmdMod_clicked();
    void on_cmdIns_clicked();
    void on_cmdDel_clicked();
    void on_predefNamespaces_cellClicked(int row, int column);
    void on_predefNamespaces_cellDoubleClicked(int row, int column);
    void on_userNamespaces_cellClicked(int row, int column);
    void on_userNamespaces_cellDoubleClicked(int row, int column);
    void on_uri_textChanged(const QString & /*text*/);
    void on_prefix_textChanged(const QString & /*text*/);
    void on_userNamespaces_itemSelectionChanged();
};

#endif // CHOOSENAMESPACEDIALOG_H
