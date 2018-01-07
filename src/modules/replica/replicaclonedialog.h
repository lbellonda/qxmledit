/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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


#ifndef REPLICACLONEDIALOG_H
#define REPLICACLONEDIALOG_H

#include <QDialog>

namespace Ui
{
class ReplicaCloneDialog;
}

class ReplicaCloneInfo;
class ReplicaCommand;
class Element ;

class ReplicaCloneDialog : public QDialog
{
    Q_OBJECT
    Element *_element;
    ReplicaCommand *_fillInfo;
    bool _fillWasCleared;

    void deleteFillInfo();
    void enableDeleteFillInfo();
    void addIndex();
public:
    explicit ReplicaCloneDialog(QWidget *parent, Element *element);
    ~ReplicaCloneDialog();

    virtual void accept();

    ReplicaCloneInfo *results();

private:
    Ui::ReplicaCloneDialog *ui;
private slots:
    void on_addIndex_clicked();
    void on_removeIndex_clicked();
};

#endif // REPLICACLONEDIALOG_H
