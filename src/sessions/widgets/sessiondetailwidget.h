/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#ifndef SESSIONDETAILWIDGET_H
#define SESSIONDETAILWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include "../models.h"

namespace Ui
{
class SessionDetailWidget;
}

class SessionDetailWidget : public QWidget
{
    Q_OBJECT
    FileModel* _fileSelected;

    void emitChangedSelection();
public:
    SessionFilesDataModel _filesModel;
    SessionAccessDataModel _accessModel;

    explicit SessionDetailWidget(QWidget *parent = 0);
    ~SessionDetailWidget();

    void setNewModel(SessionModel *sessionModel);

    FileModel* selectedFile();

private:
    Ui::SessionDetailWidget *ui;

signals:
    void fileSelected(FileModel* model);
    void fileDoubleClicked(FileModel* model);

private slots:
    void onFilesSelectionChanged(QItemSelection const&selected, QItemSelection const& deselected);
    void onAccessesSelectionChanged(QItemSelection const&selected, QItemSelection const& deselected);
    void on_accesses_doubleClicked(const QModelIndex & index);
    void on_files_doubleClicked(const QModelIndex & index);

};

#endif // SESSIONDETAILWIDGET_H
