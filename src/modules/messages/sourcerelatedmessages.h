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

#ifndef SOURCERELATEDMESSAGES_H
#define SOURCERELATEDMESSAGES_H

#include "xmlEdit.h"
#include "modules/messages/sourceerror.h"
#include <QListWidgetItem>

namespace Ui
{
class SourceRelatedMessages;
}

class SourceRelatedMessages : public QWidget
{
    Q_OBJECT
    QList<SourceMessage*> _messages;

    void reset();
    void loadData();
public:
    explicit SourceRelatedMessages(QWidget *parent = 0);
    virtual ~SourceRelatedMessages();

    void setMessages(QList<SourceMessage*> &messages);
    void clear();
    QList<int> selection();

signals:
    void navigateTo(QList<int> path);
private:
    Ui::SourceRelatedMessages *ui;

private slots:
    void on_messages_itemDoubleClicked(QListWidgetItem *item);
    void on_go_clicked();
    void on_messages_itemSelectionChanged();
};

#endif // SOURCERELATEDMESSAGES_H
