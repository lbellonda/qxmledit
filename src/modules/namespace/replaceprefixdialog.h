/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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


#ifndef REPLACEPREFIXDIALOG_H
#define REPLACEPREFIXDIALOG_H

#include "xmlEdit.h"
#include "regola.h"
#include "widgets/elementselectiontypewidget.h"

class ReplacePrefixInfo
{
public:
    QString toReplacePrefix;
    QString setPrefix;
    Element *element;
    TargetSelection::Type targetSelection;
    bool isAllPrefixes;
    PrefixInfo prefixInfo;
    ReplacePrefixInfo()
    {
        element = NULL ;
        targetSelection = TargetSelection::AllItems;
        isAllPrefixes = false;
    }
};

namespace Ui
{
class ReplacePrefixDialog;
}

class ReplacePrefixDialog : public QDialog
{
    Q_OBJECT
    ReplacePrefixInfo *_info;
    ElementSelectionTypeWidget *_selWidget;

    void finishSetup();
    void updateSelection();
    void enableChoices();
    void enableOk();
    bool checkOK();
protected:
    void accept();
public:
    static bool getReplacePrefixInfo(QWidget *window, ReplacePrefixInfo *info);

    explicit ReplacePrefixDialog(ReplacePrefixInfo *info, QWidget *parent = 0);
    virtual ~ReplacePrefixDialog();

private:
    Ui::ReplacePrefixDialog *ui;

private slots:
    void on_replacePrefix_editTextChanged(const QString &);
    void onSelectionTypeChanged(TargetSelection::Type selectionType);
    void on_onePrefix_clicked();
    void on_allPrefixes_clicked();
    void on_prefixes_currentTextChanged(const QString & text);
    void on_prefixes_editTextChanged(const QString & text);
};

#endif // REPLACEPREFIXDIALOG_H
