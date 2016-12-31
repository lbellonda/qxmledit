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

#include "sourcerelatedmessages.h"
#include "ui_sourcerelatedmessages.h"
#include "utils.h"

SourceRelatedMessages::SourceRelatedMessages(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourceRelatedMessages)
{
    ui->setupUi(this);
    ui->go->setEnabled(false);
    Utils::TODO_THIS_RELEASE("portare da sourcenavigator");
    Utils::TODO_THIS_RELEASE("abilitare go");
}

SourceRelatedMessages::~SourceRelatedMessages()
{
    reset();
    delete ui;
}

void SourceRelatedMessages::on_messages_itemDoubleClicked(QListWidgetItem * /*item*/)
{
    on_go_clicked();
}

void SourceRelatedMessages::on_go_clicked()
{
    QListWidgetItem *item = ui->messages->currentItem();
    if(NULL != item) {
        QVariant data = item->data(Qt::UserRole);
        SourceMessage *message = static_cast<SourceMessage*>(data.value<void*>());
        if(NULL != message) {
            emit navigateTo(message->path());
        }
    }
}

void SourceRelatedMessages::on_messages_itemSelectionChanged()
{
    bool how = false;
    if(ui->messages->currentItem() != NULL) {
        how = true ;
    }
    ui->go->setEnabled(how);
}

void SourceRelatedMessages::reset()
{
    ui->messages->clear();
    EMPTYPTRLIST(_messages, SourceMessage);
}

void SourceRelatedMessages::setMessages(QList<SourceMessage*> &messages)
{
    ui->messages->setUpdatesEnabled(false);
    reset();
    foreach(SourceMessage *msg, messages) {
        _messages.append(msg);
    }
    loadData();
    ui->messages->setUpdatesEnabled(true);
}

void SourceRelatedMessages::loadData()
{
    foreach(SourceMessage *msg, _messages) {
        QString text = QString("%1 %2 %3")
                       .arg(msg->type())
                       .arg(msg->source())
                       .arg(msg->description());
        QListWidgetItem *item = new QListWidgetItem(text);
        QIcon icon;
        switch(msg->type()) {
        case SourceMessage::Error:
            icon.addPixmap(QPixmap(QString::fromUtf8(":/specials/error")), QIcon::Normal, QIcon::Off);
            item->setIcon(icon);
            break;
        case SourceMessage::Info:
            icon.addPixmap(QPixmap(QString::fromUtf8(":/specials/info")), QIcon::Normal, QIcon::Off);
            item->setIcon(icon);
            break;
        default:
        case SourceMessage::Warning:
            icon.addPixmap(QPixmap(QString::fromUtf8(":/specials/warning")), QIcon::Normal, QIcon::Off);
            item->setIcon(icon);
            break;
        }
        item->setData(Qt::UserRole, qVariantFromValue((void*)msg));
        ui->messages->addItem(item);
    }
}


QList<int> SourceRelatedMessages::selection()
{
    const int posiz = ui->messages->currentRow();
    if(posiz >= 0) {
        return _messages.at(posiz)->path();
    }
    return QList<int>();
}

