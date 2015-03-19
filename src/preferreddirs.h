/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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
#ifndef PREFERREDDIRS_H
#define PREFERREDDIRS_H

#include <QDialog>
#include <QStringListModel>

namespace Ui
{
class PreferredDirs;
}

class PreferredDirs : public QDialog
{
    Q_OBJECT
public:

    PreferredDirs(QStringList &theOriginalValues, QWidget *parent = 0);
    ~PreferredDirs();

    static bool checkNewDir(const int size, const bool isUI);
    static bool checkDuplicateDir(QStringList &list, const QString &newDir, const bool isUI);

protected:
    void changeEvent(QEvent *e);

private:
    QStringList &originalValues;
    QStringList dirs;
    QStringListModel *model;
    Ui::PreferredDirs *ui;

    void fillData();
    void handleSelects(const bool isItemValid);

private slots:
    void accept();
    void on_add_clicked();
    void on_remove_clicked();
    void on_dirList_activated(const QModelIndex & index);
};

bool configurePreferredDirs(QWidget *parent, QStringList &originalValues);

#endif // PREFERREDDIRS_H
