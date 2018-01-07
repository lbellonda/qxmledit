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


#ifndef ANONIMYZEBATCHDIALOG_H
#define ANONIMYZEBATCHDIALOG_H

#include <QDialog>
#include <QProgressDialog>
#include <QFuture>
#include <QComboBox>
#include <QAbstractButton>
#include <QToolButton>
#include "modules/anonymize/anonoperationbatch.h"

class QXmlEditData ;

namespace Ui
{
class AnonimyzeBatchDialog;
}

class AnonProfile ;
class GenericPersistentData;

class AnonimyzeBatchDialog : public QDialog
{
    Q_OBJECT

    static const int NormalView = 0;
    static const int ProgressView = 1;


    bool    _running, _aborted ;
    QFuture<void> _future ;
    AnonOperationBatch _operation;
    QString _inputFilePath;
    QString _outputFilePath;
    AnonProfile *_profile;
    QXmlEditData *_data;

public:
    explicit AnonimyzeBatchDialog(QXmlEditData *theData, QWidget *parent = 0);
    ~AnonimyzeBatchDialog();

private:
    Ui::AnonimyzeBatchDialog *ui;
    bool checkParameters();
    void startOperation();
    void setupFolders();
    //void setupRecentFolders(QComboBox *combo, const bool isSave);
    void setupRecentFolders(QToolButton *button, const bool isSave);
    void setInputFilePath(const QString &theFilePath);
    void setOutputFilePath(const QString &theFilePath);
    //QString fileForSelection(QComboBox *combo, int index, const QString &message, const bool isOpen);
    QString fileForSelection(const QString &value, const QString &message, const bool isOpen);
    AnonProfile* getProfileFromProfileData(GenericPersistentData *input);
    void enableOk();
    void doOperationWorkThread();
    void freeResources();
    void setAborted();
    void endOfOperation();
    void opStart();
    void closeEvent(QCloseEvent * event);

private slots:
    void accept();
    void reject();
    //void on_cbInputFile_activated(int index);
    //void on_cbOutputFile_activated(int index);
    void on_cmdChooseProfile_clicked();
    void checkIfDone();
    void on_buttonBox_clicked(QAbstractButton * button);
    void onOpCanceled();
    void on_cmdCancel_clicked();
    //bool close();
    void onChooseInput();
    void onChooseOutput();
};

#endif // ANONIMYZEBATCHDIALOG_H
