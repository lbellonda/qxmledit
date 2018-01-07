/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef ANONPROFILEDIALOG_H
#define ANONPROFILEDIALOG_H

#include <QDialog>
#include <QMap>

#include <data/DataInterface.h>
#include "qxmleditdata.h"
#include "modules/anonymize/anonprofile.h"

namespace Ui
{
class AnonProfileDialog;
}

class AnonProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnonProfileDialog(QXmlEditData *data, QWidget *parent = 0);
    ~AnonProfileDialog();
    GenericPersistentData *result();

    GenericPersistentData* insertProfile(AnonProfile *inputProfile);
    bool persistSnippet(GenericPersistentData *snippet);
protected:
    Ui::AnonProfileDialog *ui;
    QMap<int, GenericPersistentData*> _snippets;
    QMultiMap<int, QTreeWidgetItem*> _snippetItems;
    QMap<QString, QTreeWidgetItem*> _topLevelItems;
    GenericPersistentData *_result;
    QXmlEditData *_data;

    void changeEvent(QEvent *e);
    virtual void loadSnippets();
    virtual void accept();
    virtual void initComplete();
    void fillTree();
    void refineUI();
    void updateUISnipptes();

    //-------------- methods -------------------
    void deleteSnippet(GenericPersistentData* snippet);
    void updateSnippetInUI(GenericPersistentData* snippet);
    bool deleteSnippetFromUI(GenericPersistentData* snippet, const bool shouldReInsert);
    bool insertSnippetInUI(GenericPersistentData* snippet);

    bool deleteSnippetFromPersistence(GenericPersistentData *snippet);

    void insertSnippet();
    void updateCurentSnippet();
    void deleteCurrentSnippet();

    virtual GenericPersistentData *getSelSnippet();
    bool editSnippet(const bool isInsert, GenericPersistentData *snippet);
    void evaluateSelection();

private slots:
    void on_newCmd_clicked();
    void on_modifyCmd_clicked();
    void on_deleteCmd_clicked();
    void on_snippets_itemSelectionChanged();
    void on_snippets_itemDoubleClicked(QTreeWidgetItem * item, int column);
protected slots:
    void completeUI();

};

#endif // ANONPROFILEDIALOG_H
