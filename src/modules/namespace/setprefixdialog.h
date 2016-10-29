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


#ifndef SETPREFIXDIALOG_H
#define SETPREFIXDIALOG_H

#include "xmlEdit.h"
#include "regola.h"
#include "widgets/elementselectiontypewidget.h"

class SetPrefixInfo
{
public:
    QString setPrefix;
    Element *element;
    TargetSelection::Type targetSelection;
    PrefixInfo prefixInfo;
    SetPrefixInfo() {
        element = NULL ;
        targetSelection = TargetSelection::AllItems;
    }
};

namespace Ui
{
class SetPrefixDialog;
}

class SetPrefixDialog : public QDialog
{
    Q_OBJECT
    SetPrefixInfo *_info;
    ElementSelectionTypeWidget *_selWidget;

    void finishSetup();
    void enableOk();
    bool checkOK();
protected:
    void accept();
public:
    static bool getSetPrefixInfo(QWidget *window, SetPrefixInfo *info);

    explicit SetPrefixDialog(SetPrefixInfo *info, QWidget *parent = 0);
    ~SetPrefixDialog();

private:
    Ui::SetPrefixDialog *ui;
private slots:
    void onSelectionTypeChanged(TargetSelection::Type selectionType);
    void on_prefixes_currentTextChanged(const QString & text);
    void on_prefixes_editTextChanged(const QString & text);
};

#endif // SETPREFIXDIALOG_H
