/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
#ifndef SCHEMACHOOSER_H
#define SCHEMACHOOSER_H

#include <QDialog>
#include <QListWidget>

namespace Ui
{
class SchemaChooser;
}

class SchemaChooser : public QDialog
{
    Q_OBJECT

    static const int MAX_LAST_FILES = 10 ;

    bool _started;
    QString _selection;
    QStringList _lastFilesNames;
    QStringList _preferitesFilesNames;
public:
    explicit SchemaChooser(QWidget *parent = 0);
    ~SchemaChooser();

    QString selection();

private:
    Ui::SchemaChooser *ui;

    void loadData();
    void saveSelection();
    void loadPreferredData();
    void loadLastUsedData();
    void setSelection(const QString &newSelection);
    void enableOK(const bool isEnabled);

private slots:
    void accept();
    void on_lastUsedList_currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous) ;
    void on_lastUsedList_itemClicked(QListWidgetItem * item);
    void on_favoritesList_currentItemChanged(QListWidgetItem * current, QListWidgetItem * /*previous*/);
    void on_favoritesList_itemClicked(QListWidgetItem * item);
    void on_addPrefsSchema_clicked();
    void on_remPrefsSchema_clicked();
    void on_browseFile_clicked();
    void on_textSelection_textChanged(const QString & text);
    void on_favoritesList_itemDoubleClicked(QListWidgetItem * item);
    void on_lastUsedList_itemDoubleClicked(QListWidgetItem * item);

};

#endif // SCHEMACHOOSER_H
