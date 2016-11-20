/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2016 by Luca Bellonda and individual contributors  *
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

#ifndef SNIPPETHANDLER_H
#define SNIPPETHANDLER_H

#include <QDialog>
#include <QMap>

#include "snippet.h"
#include "regola.h"
#include "applicationdata.h"

namespace Ui
{
class SnippetHandler;
}

class SnippetHandler : public QDialog
{
    Q_OBJECT
public:
    SnippetHandler(ApplicationData *data, const bool setEditMode, QWidget *parent = 0);
    ~SnippetHandler();

    Regola *regola();

    void insertFragment(QWidget *parent, const QString &text);

protected:
    void changeEvent(QEvent *e);
    virtual void loadSnippets();
    virtual void accept();
    virtual void initComplete();
    void fillTree();
    void refineUI();

protected:
    Ui::SnippetHandler *ui;
    bool _isEditMode;
    QMap<QString, Snippet*> _snippets;
    QMultiMap<QString, QTreeWidgetItem*> _snippetItems;
    QMap<QString, QTreeWidgetItem*> _topLevelItems;
    Regola *_regola;
    ApplicationData *_data;


    //-------------- methods -------------------
    void deleteSnippet(Snippet* snippet);


    void updateSnippetInUI(Snippet* snippet);
    bool deleteSnippetFromUI(Snippet* snippet, const bool shouldReInsert);
    bool insertSnippetInUI(Snippet* snippet);

    bool deleteSnippetFromPersistence(Snippet *snippet);
    bool persistSnippet(QWidget *parent, Snippet *snippet);

    void insertSnippet();
    void updateCurentSnippet();
    void deleteCurrentSnippet();

    virtual Snippet *getSelSnippet();
    bool editSnippet(const bool isInsert, Snippet *snippet);
    bool scanDirectory(const QString &dirName, const bool isReadOnly);

    void evaluateSelection();
    QString buildFilePath(const QString &dirPath, Snippet* snippet);


private slots:
    void on_newCmd_clicked();
    void on_modifyCmd_clicked();
    void on_deleteCmd_clicked();
    void on_snippets_itemSelectionChanged();
    void on_snippets_itemDoubleClicked(QTreeWidgetItem * item, int column);
protected slots:
    void completeUI();

};

#endif // SNIPPETHANDLER_H
