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


#ifndef REMOVEPREFIXDIALOG_H
#define REMOVEPREFIXDIALOG_H

#include "xmlEdit.h"
#include "regola.h"
#include "widgets/elementselectiontypewidget.h"

class RemovePrefixInfo
{
public:
    QString removedPrefix;
    Element *element;
    TargetSelection::Type targetSelection;
    bool isAllPrefixes;
    PrefixInfo prefixInfo;
    RemovePrefixInfo()
    {
        element = NULL ;
        targetSelection = TargetSelection::AllItems;
        isAllPrefixes = true ;
    }
};

namespace Ui
{
class RemovePrefixDialog;
}

class RemovePrefixDialog : public QDialog
{
    Q_OBJECT
    RemovePrefixInfo *_info;
    ElementSelectionTypeWidget *_selWidget;

    void finishSetup();
    void updateSelection();
    void enableChoices();
    void enableOk();
    bool checkOK();
protected:
    void accept();
public:
    static bool getRemovePrefixInfo(QWidget *window, RemovePrefixInfo *info);

    explicit RemovePrefixDialog(RemovePrefixInfo *info, QWidget *parent = 0);
    virtual ~RemovePrefixDialog();

private:
    Ui::RemovePrefixDialog *ui;

private slots:
    void onSelectionTypeChanged(TargetSelection::Type selectionType);
    void on_onePrefix_clicked();
    void on_allPrefixes_clicked();
    void on_prefixes_currentTextChanged(const QString & text);
    void on_prefixes_editTextChanged(const QString & text);
};

#endif // REMOVEPREFIXDIALOG_H
