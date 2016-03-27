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


#ifndef EXTRACTFRAGMENTSDIALOG_H
#define EXTRACTFRAGMENTSDIALOG_H

#include "xmlEdit.h"
#include "libQXmlEdit_global.h"
#include <QDialog>
#include <QComboBox>
#include <QTimer>
#include <QButtonGroup>
#include "extractionoperation.h"

namespace Ui
{
class ExtractFragmentsDialog;
}



class LIBQXMLEDITSHARED_EXPORT ExtractFragmentsDialog : public QDialog
{
    Q_OBJECT

    ExtractResults *_extractResult;
    ExtractionOperation _operation;
    QString _errorMessage;
    QTimer delayTimer;
    bool _isAutoDelete;

public:
    explicit ExtractFragmentsDialog(ExtractResults *extractResult, QWidget *parent = 0);
    ~ExtractFragmentsDialog();
    void setAutoDelete();

private:
    Ui::ExtractFragmentsDialog *ui;

    void LoadComboBoxCfrOpr(QComboBox *combo);
    void initUIFromOperation();
    void fillOperationFromUI();
    bool checkOperationParameters();
    void enableControls();
    void loadComboData(QComboBox *box);
    void connectUpdates();
    QString showSingleNaming(QStringList &names);
    void addNamePattern(QStringList &list, const QString &str);
    void closeEvent(QCloseEvent * event);

    QButtonGroup _groupExtractOptions;
    QButtonGroup _groupSplitOptions;

private slots:
    void accept();
    void reject();
    void on_buttonBox_accepted();
    void on_extractAll_clicked();
    void on_extractRange_clicked();
    void on_extractWithCfr_clicked();
    void on_makeSubfolders_clicked();

    void on_cmdViewInBrowser_clicked();
    void on_cmdDirScan_clicked();
    void on_sourceFIleSearch_clicked();
    void on_chkExtractFiles_clicked();
    void editTextChanged(const QString & text);
    void on_chkDeleteUsingPath_clicked();
    void showNaming();
    void on_splitByDepth_clicked();
    void on_splitByPath_clicked();
};

#endif // EXTRACTFRAGMENTSDIALOG_H
