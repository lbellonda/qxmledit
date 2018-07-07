/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#ifndef SEARCHCOMMANDDIALOG_H
#define SEARCHCOMMANDDIALOG_H

#include "xmlEdit.h"

namespace Ui
{
class SearchCommandDialog;
}

class MenuSearchData;

class SearchCommandDialog : public QDialog
{
    Q_OBJECT
    QList<QAction*> _actionList;
    QAction *_selectedAction;
    QList<MenuSearchData*> _actions;

public:
    explicit SearchCommandDialog(QList<QAction*> theActions, QWidget *parent = 0);
    ~SearchCommandDialog();

    QAction *selectedAction();
    virtual void accept();
private:
    Ui::SearchCommandDialog *ui;

    void updateList(const QString &text);
    QString normalizeText(const QString &text);
    QString normalizeTextForSearch(const QString &text);
    void buildActions();
    QAction *evalSelection();
    void enableOK();
    bool filterAction(QAction *action);
    void addItem(MenuSearchData *data);
    QList<QAction*> filterActions(QList<QAction*> sourceActions);
private slots:
    void on_search_textChanged(const QString &text);
    void on_commands_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);
    void on_commands_itemDoubleClicked(QTreeWidgetItem *, int);
    void on_allWords_toggled();
#ifdef QXMLEDIT_TEST
    friend class TestHelp ;
#endif
};

#endif // SEARCHCOMMANDDIALOG_H
