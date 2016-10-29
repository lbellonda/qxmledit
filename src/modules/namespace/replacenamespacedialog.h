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


#ifndef REPLACENAMESPACEDIALOG_H
#define REPLACENAMESPACEDIALOG_H

#include "xmlEdit.h"
#include "regola.h"
#include "widgets/elementselectiontypewidget.h"

class ReplaceNamespaceInfo
{
public:
    QString nsURI;
    QString setPrefix;
    QString toReplaceNSUri;
    Element *element;
    TargetSelection::Type targetSelection;
    NamespacesInfo namespacesInfo;
    bool remapPrefixes;
    ReplaceNamespaceInfo() {
        element = NULL ;
        targetSelection = TargetSelection::AllItems;
        remapPrefixes = false;
    }
};

namespace Ui
{
class ReplaceNamespaceDialog;
}

class ReplaceNamespaceDialog : public QDialog
{
    Q_OBJECT
    ReplaceNamespaceInfo *_info;
    ElementSelectionTypeWidget *_selWidget;
    NamespaceManager *_namespaceManager ;
    QSet<QString> _namespacesCited;
    QSet<QString> _prefixesCited;
    bool _remapPrefixes;

    void finishSetup();
    void updateSelection();
    void enableChoices();
    void enableOk();
    void updatePrefixes();
    void mergePrefixesInfo();
    void mergeNamespacesInfo();
    bool checkOK();
protected:
    void accept();

public:
    static bool getReplaceNamespaceInfo(QWidget *window, ReplaceNamespaceInfo *info, NamespaceManager *namespaceManager);

public:
    explicit ReplaceNamespaceDialog(ReplaceNamespaceInfo *info, QWidget *parent = 0);
    ~ReplaceNamespaceDialog();

    NamespaceManager *namespaceManager() const ;
    void setNamespaceManager(NamespaceManager *namespaceManager);

private:
    Ui::ReplaceNamespaceDialog *ui;

private slots:
    void on_select_clicked();
    void onSelectionTypeChanged(TargetSelection::Type selectionType);
    void on_prefix_currentTextChanged(const QString &);
    void on_prefix_editTextChanged(const QString &);
    void on_namespaces_currentTextChanged(const QString &);
    void on_namespaces_editTextChanged(const QString &);
    void on_namespaceToReplace_currentIndexChanged(int);
};

#endif // REPLACENAMESPACEDIALOG_H
