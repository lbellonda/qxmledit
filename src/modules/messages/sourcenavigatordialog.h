/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#ifndef SOURCENAVIGATORDIALOG_H
#define SOURCENAVIGATORDIALOG_H

#include "xmlEdit.h"
#include "modules/messages/sourceerror.h"
#include <QListWidgetItem>

namespace Ui
{
class SourceNavigatorDialog;
}

class SourceNavigatorDialog : public QDialog
{
    Q_OBJECT
    QList<SourceMessage*> *_messages;

    void loadData();

public:
    explicit SourceNavigatorDialog(QWidget *parent, QList<SourceMessage*> *messages);
    ~SourceNavigatorDialog();

    QList<int> selection();

private:
    Ui::SourceNavigatorDialog *ui;
private slots:
    void on_messages_itemDoubleClicked(QListWidgetItem *item);
};

#endif // SOURCENAVIGATORDIALOG_H
