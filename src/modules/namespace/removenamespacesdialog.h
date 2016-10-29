/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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


#ifndef REMOVENAMESPACESDIALOG_H
#define REMOVENAMESPACESDIALOG_H

#include "xmlEdit.h"
#include "regola.h"
#include "widgets/elementselectiontypewidget.h"

class RemoveNamespacesInfo
{
public:
    QString removedNamespace;
    Element *element;
    TargetSelection::Type targetSelection;
    bool isAllNamespaces;
    NamespacesInfo namespacesInfo;
    bool isRemoveDeclarations;
    RemoveNamespacesInfo() {
        element = NULL ;
        targetSelection = TargetSelection::AllItems;
        isAllNamespaces = true ;
        isRemoveDeclarations = true ;
    }
};

namespace Ui
{
class RemoveNamespacesDialog;
}

class RemoveNamespacesDialog : public QDialog
{
    Q_OBJECT
    RemoveNamespacesInfo *_info;
    ElementSelectionTypeWidget *_selWidget;

    void finishSetup();
    void updateSelection();
    void enableChoices();
    void enableOk();
    void updatePrefixes();
protected:
    void accept();

public:
    static bool getRemoveNamespacesInfo(QWidget *window, RemoveNamespacesInfo *info);

    explicit RemoveNamespacesDialog(RemoveNamespacesInfo *info, QWidget *parent = 0);
    ~RemoveNamespacesDialog();

private:
    Ui::RemoveNamespacesDialog *ui;

private slots:
    void onSelectionTypeChanged(TargetSelection::Type selectionType);
    void on_allNamespaces_clicked();
    void on_oneNamespace_clicked();
    void on_namespaces_currentIndexChanged(const QString &);
};

#endif // REMOVENAMESPACESDIALOG_H
