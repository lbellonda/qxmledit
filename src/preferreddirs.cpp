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
#include "preferreddirs.h"
#include "ui_preferreddirs.h"
#include "utils.h"
#include "qxmledit.h"
#include <QFileDialog>
#include <QVariant>

bool configurePreferredDirs(QWidget *parent, QStringList &originalValues)
{
    PreferredDirs editDirs(originalValues, parent) ;
    editDirs.setModal(true);
    if(editDirs.exec() == QDialog::Accepted) {
        return true ;
    }
    return false ;
}

//-------------------------------------------------------------------------

PreferredDirs::PreferredDirs(QStringList &theOriginalValues, QWidget *parent) :
    QDialog(parent),
    originalValues(theOriginalValues),
    dirs(theOriginalValues), //copy constr,
    model(NULL),
    ui(new Ui::PreferredDirs)

{
    ui->setupUi(this);
    ui->dirList->setSelectionMode(QAbstractItemView::SingleSelection);
    fillData();
    handleSelects(false);
}

PreferredDirs::~PreferredDirs()
{
    delete ui;
}

void PreferredDirs::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch(e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PreferredDirs::fillData()
{
    model = new QStringListModel(this);
    model->setStringList(dirs);
    ui->dirList->setModel(model);
}

bool PreferredDirs::checkNewDir(const int size, const bool isUI)
{
    if(size >= QXmlEditParameters::NUM_PREF_DIRS) {
        if(isUI) {
            Utils::error(tr("Reached the maxmimum number of directories. Cannot add another one"));
        }
        return false ;
    }
    return true ;
}

bool PreferredDirs::checkDuplicateDir(QStringList &list, const QString &newDir, const bool isUI)
{
    if(list.contains(newDir)) {
        if(isUI) {
            Utils::error(tr("Directory is already in the list."));
        }
        return false ;
    }
    return true ;
}


void PreferredDirs::on_add_clicked()
{
    if(!PreferredDirs::checkNewDir(model->rowCount(), true)) {
        return;
    }
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Choose the directory"),
                      "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dirPath.isEmpty()) {
        /* TODO: it is unnecessary? QDir dir(filePath);
        dirPath = dir.canonicalPath () ;
        */
        QStringList dirList = model->stringList();
        if(!checkDuplicateDir(dirList, dirPath, true)) {
            return ;
        }
        if(model->insertRow(model->rowCount())) {
            QModelIndex index = model->index(model->rowCount() - 1, 0);
            if(model->setData(index, QVariant(dirPath))) {
                //all ok
            } else {
                Utils::error(this, tr("Unable to register data"));
            }
        } else {
            Utils::error(this, tr("Unable to make room for data"));
        }
    }
}

void PreferredDirs::on_dirList_activated(const QModelIndex & index)
{
    bool isValid = index.isValid();
    handleSelects(isValid);
}

void PreferredDirs::handleSelects(const bool isItemValid)
{
    ui->remove->setEnabled(isItemValid);
}

void PreferredDirs::on_remove_clicked()
{
    QModelIndex index = ui->dirList->currentIndex() ;
    if(index.isValid()) {
        if(Utils::askYN(this, tr("Really delete selected item?"))) {
            model->removeRow(index.row());
            handleSelects(ui->dirList->currentIndex().isValid());
        }
    }
}

void PreferredDirs::accept()
{
    originalValues = model->stringList();
    QDialog::accept();
}
