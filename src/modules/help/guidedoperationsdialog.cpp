/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#include "guidedoperationsdialog.h"
#include "ui_guidedoperationsdialog.h"
#include "applicationdata.h"
#include "utils.h"

GuidedOperationsDialog::GuidedOperationsDialog(QXmlEditApplication *application, ApplicationData *appData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GuidedOperationsDialog)
{
    Utils::TODO_THIS_RELEASE("cambia profilo deve cambiare il testo a se3conda dell'utente");
    Utils::TODO_THIS_RELEASE("fare hide window senza uscire");
    _application = application;
    _appData = appData;
    ui->setupUi(this);
    setWindowTitle(APPLICATION_NAME);
    makeButtonsSameSize();
    applyUserProfile();
    setAcceptDrops(true);
}

GuidedOperationsDialog::~GuidedOperationsDialog()
{
    delete ui;
}

void GuidedOperationsDialog::makeButtonsSameSize()
{
    int maxSizeX = 0 ;
    QList<QPushButton*> buttons = findChildren<QPushButton*>();
    foreach(QPushButton* button, buttons) {
        maxSizeX = qMax(maxSizeX, button->size().width());
    }
    maxSizeX *= 1.3;
    foreach(QPushButton* button, buttons) {
        button->setFixedSize(maxSizeX, button->height());
    }
    QSize currentSize = ui->l1->sizeHint();
    const int spacerGap = maxSizeX >> 1 ;
    ui->l1->spacerItem()->changeSize(spacerGap, currentSize.height(), QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->l2->spacerItem()->changeSize(spacerGap, currentSize.height(), QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->l3->spacerItem()->changeSize(spacerGap, currentSize.height(), QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->l4->spacerItem()->changeSize(spacerGap, currentSize.height(), QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->l5->spacerItem()->changeSize(spacerGap, currentSize.height(), QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->l6->spacerItem()->changeSize(spacerGap, currentSize.height(), QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->v1Layout->update();
}

void GuidedOperationsDialog::reject()
{
    if(_appData->isUserGuidedOperation()) {
        on_cmdQuit_clicked();
    } else {
        QDialog::reject();
    }
}

void GuidedOperationsDialog::on_cmdNew_clicked()
{
    emit triggerNew();
}

void GuidedOperationsDialog::on_cmdQuit_clicked()
{
    if(Utils::askYN(this, tr("Really quit?"))) {
        emit triggerQuit();
    }
}

void GuidedOperationsDialog::on_cmdOpen_clicked()
{
    emit triggerOpen();
}

void GuidedOperationsDialog::on_cmdValidate_clicked()
{
    emit triggerValidate();
}

void GuidedOperationsDialog::on_cmdCfgFormatting_clicked()
{
    emit triggerFormatting();
}

void GuidedOperationsDialog::on_cmdCfgVision_clicked()
{
    emit triggerCfgVision();
}

void GuidedOperationsDialog::on_cmdCfgEditing_clicked()
{
    emit triggerCfgEditing();
}

void GuidedOperationsDialog::on_cmdConfigure_clicked()
{
    emit triggerConfigure();
}

void GuidedOperationsDialog::on_cmdUserProfile_clicked()
{
    if(Utils::askYN(this, tr("Really change profile?"))) {
        emit triggerUserProfile();
    }
}

void GuidedOperationsDialog::on_cmdExtractFile_clicked()
{
    emit triggerExtractFile();
}

void GuidedOperationsDialog::on_cmdViewXSD_clicked()
{
    emit triggerViewXSD();
}

void GuidedOperationsDialog::on_cmdCompare_clicked()
{
    Utils::TODO_THIS_RELEASE("fare");
    emit triggerCompare();
}

void GuidedOperationsDialog::on_cmdAnon_clicked()
{
    Utils::TODO_THIS_RELEASE("fare");
    emit triggerAnon();
}

void GuidedOperationsDialog::on_cmdUserManual_clicked()
{
    Utils::TODO_THIS_RELEASE("fare");
    emit triggerUserManual();
}

void GuidedOperationsDialog::on_cmdEditingShortcut_clicked()
{
    Utils::TODO_THIS_RELEASE("fare");
    emit triggerEditingShortcut();
}

void GuidedOperationsDialog::applyUserProfile()
{
    Utils::TODO_THIS_RELEASE("fare gli spacer ed allinea");
    Utils::TODO_THIS_RELEASE("fare con bottoni");
    if(_appData->isUserGuidedOperation()) {
        ui->buttonBox->hide();
        ui->cmdUserProfile->setText(tr("Expert User"));
    } else {
        ui->buttonBox->show();
        ui->cmdUserProfile->setText(tr("Guided User"));
    }
}

void GuidedOperationsDialog::on_cmdBase64_clicked()
{
    Utils::TODO_THIS_RELEASE("fare");
    emit triggerBase64();
}

void GuidedOperationsDialog::on_cmdViewDataMap_clicked()
{
    Utils::TODO_THIS_RELEASE("fare");
    emit triggerViewXMLMap();
}


void GuidedOperationsDialog::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat(Utils::URIDropType)) {
        event->acceptProposedAction();
    }
}

void GuidedOperationsDialog::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasFormat(Utils::URIDropType)) {
        QString filePath = "" ;
        event->acceptProposedAction();
        if(event->mimeData()->hasUrls()) {
            foreach(QUrl url, event->mimeData()->urls()) {
                filePath = url.toLocalFile();
                break;
            }
        }
        if(!filePath.isEmpty()) {
            emit openFile(filePath);
        }
    } else {
        event->ignore();
    }
}
