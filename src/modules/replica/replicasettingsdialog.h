/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#ifndef REPLICASETTINGSDIALOG_H
#define REPLICASETTINGSDIALOG_H

#include <QDialog>
#include "xmlEdit.h"
#include "element.h"
#include "modules/replica/replicacommand.h"

namespace Ui
{
class ReplicaSettingsDialog;
}

class ReplicaSettingsDialog : public QDialog
{
    Q_OBJECT
    Element *_base;
    ReplicaCommand _result;
public:
    explicit ReplicaSettingsDialog(Element *base, QWidget *parent = 0);
    ~ReplicaSettingsDialog();
    ReplicaCommand *result();

protected:
    virtual void accept();

private:
    Ui::ReplicaSettingsDialog *ui;

    void setupValues();
    void readValues(ReplicaCommand *cmd);
    ReplicaCommand::EErrorCode checkData(ReplicaCommand *cmd);
    bool evalOk();
private slots:
    void on_attributes_editTextChanged(const QString & text);
    void on_separator_textEdited(const QString & text);
    void on_startNumber_valueChanged(int i);
#ifdef  QXMLEDIT_TEST
    friend class TestReplica;
#endif
};

#endif // REPLICASETTINGSDIALOG_H
