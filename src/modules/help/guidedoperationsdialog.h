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

#ifndef GUIDEDOPERATIONSDIALOG_H
#define GUIDEDOPERATIONSDIALOG_H

#include "xmlEdit.h"
#include <QShortcut>

class QXmlEditApplication ;
class ApplicationData ;
class QDragEnterEvent;
class QDropEvent;

namespace Ui
{
class GuidedOperationsDialog;
}

class GuidedOperationsDialog : public QDialog
{
    Q_OBJECT
    QXmlEditApplication *_application;
    ApplicationData *_appData ;
    QShortcut *_shortcutOpen;
    QShortcut *_shortcutNew;

    void makeButtonsSameSize();

protected:
    virtual void reject();
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

public:
    explicit GuidedOperationsDialog(QXmlEditApplication *application, ApplicationData *appData, QWidget *parent = 0);
    ~GuidedOperationsDialog();

    void applyUserProfile();

private:
    Ui::GuidedOperationsDialog *ui;

signals:
    void triggerNew();
    void triggerQuit();
    void triggerOpen();
    void triggerValidate();
    void triggerClose();
    void triggerFormatting();
    void triggerCfgVision();
    void triggerCfgEditing();
    void triggerConfigure();
    void triggerUserProfile();
    void triggerExtractFile();
    void triggerViewXSD();
    void triggerCompare();
    void triggerAnon();
    void triggerUserManual();
    void triggerEditingShortcut();
    void triggerBase64();
    void triggerViewXMLMap();
    void openFile(const QString &fullFilePath);

private slots:
    void on_cmdNew_clicked();
    void on_cmdQuit_clicked();
    void on_cmdOpen_clicked();
    void on_cmdValidate_clicked();
    void on_cmdCfgVision_clicked();
    void on_cmdCfgFormatting_clicked();
    void on_cmdCfgEditing_clicked();
    void on_cmdConfigure_clicked();
    void on_cmdUserProfile_clicked();
    void on_cmdExtractFile_clicked();
    void on_cmdViewXSD_clicked();
    void on_cmdCompare_clicked();
    void on_cmdAnon_clicked();
    void on_cmdUserManual_clicked();
    void on_cmdEditingShortcut_clicked();
    void on_cmdBase64_clicked();
    void on_cmdViewDataMap_clicked();
    void onShortcutOpen();
    void onShortcutNew();
};

#endif // GUIDEDOPERATIONSDIALOG_H
