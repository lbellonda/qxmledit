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


#include "sessiondetailwidget.h"
#include "ui_sessiondetailwidget.h"

SessionDetailWidget::SessionDetailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SessionDetailWidget)
{
    _fileSelected = NULL ;

    ui->setupUi(this);
    ui->files->setModel(&_filesModel);
    ui->accesses->setModel(&_accessModel);
}

SessionDetailWidget::~SessionDetailWidget()
{
    delete ui;
}

void SessionDetailWidget::setNewModel(SessionModel *sessionModel)
{
    _fileSelected = NULL ;

    _filesModel.setModel(sessionModel);
    ui->files->setModel(NULL);
    ui->files->setModel(&_filesModel);
    ui->files->resizeColumnsToContents();

    _accessModel.setModel(sessionModel);
    ui->accesses->setModel(NULL);
    ui->accesses->setModel(&_accessModel);
    ui->accesses->resizeColumnsToContents();

    connect(ui->files->selectionModel(), SIGNAL(selectionChanged(QItemSelection const&, QItemSelection const &)), this, SLOT(onFilesSelectionChanged(QItemSelection const&, QItemSelection const&)));
    connect(ui->accesses->selectionModel(), SIGNAL(selectionChanged(QItemSelection const&, QItemSelection const &)), this, SLOT(onAccessesSelectionChanged(QItemSelection const&, QItemSelection const&)));

}


FileModel* SessionDetailWidget::selectedFile()
{
    return _fileSelected ;
}

void SessionDetailWidget::onFilesSelectionChanged(QItemSelection const& selected, QItemSelection const& /*deselected*/)
{
    _fileSelected = NULL ;
    if(selected.indexes().size() > 0) {
        QModelIndex index = selected.indexes().at(0);
        QVariant data = ui->files->selectionModel()->model()->data(index, Qt::UserRole);
        _fileSelected = static_cast<FileModel *>(data.value<void*>());
    }
    emitChangedSelection();
}

void SessionDetailWidget::onAccessesSelectionChanged(QItemSelection const& selected, QItemSelection const& /*deselected*/)
{
    _fileSelected = NULL ;
    if(selected.indexes().size() > 0) {
        QModelIndex index = selected.indexes().at(0);
        QVariant data = ui->accesses->selectionModel()->model()->data(index, Qt::UserRole);
        AccessModel *access = static_cast<AccessModel *>(data.value<void*>());
        if(NULL != access) {
            _fileSelected = access->fileModel ;
        }
    }
    emitChangedSelection();
}

void SessionDetailWidget::emitChangedSelection()
{
    emit fileSelected(_fileSelected);
}

void SessionDetailWidget::on_accesses_doubleClicked(const QModelIndex & index)
{
    QVariant data = _accessModel.data(index, Qt::UserRole);
    AccessModel *access = static_cast<AccessModel *>(data.value<void*>());
    if(NULL != access) {
        emit fileDoubleClicked(access->fileModel);
    }
}

void SessionDetailWidget::on_files_doubleClicked(const QModelIndex & index)
{
    QVariant data = _filesModel.data(index, Qt::UserRole);
    FileModel *file = static_cast<FileModel *>(data.value<void*>());
    if(NULL != file) {
        emit fileDoubleClicked(file);
    }
}
