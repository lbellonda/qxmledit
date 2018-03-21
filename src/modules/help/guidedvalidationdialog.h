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

#ifndef GUIDEDVALIDATIONDIALOG_H
#define GUIDEDVALIDATIONDIALOG_H

#include "xmlEdit.h"
#include <QFuture>
#include <QPair>
#include <QDropEvent>

class ApplicationData;

struct InfoPartGuidedValidationDialog {
    QLabel *icon;
    QPushButton *button;
    QLineEdit *edit;
};

namespace Ui
{
class GuidedValidationDialog;
}

class GuidedValidationDialog : public QDialog
{
    Q_OBJECT

    static const int PageOk = 0 ;
    static const int PageEmpty = 1 ;
    static const int PageKo = 2 ;
    static const int PageWait = 3 ;
    static const int PageError = 4 ;

    int _sizeIcon;
    bool _isError ;
    bool _isValid ;
    QString _message ;
    bool _calculating ;
    ApplicationData *_appData;
    InfoPartGuidedValidationDialog _xml;
    InfoPartGuidedValidationDialog _xsd;
    QList<QFuture<QPair<int, QString> > > _waitingForCancelTaskList;
    QFuture<QPair<int, QString> > _currentCalculation;

protected:
    void accept();
    void reject();
    //---------------------------
    void loadXML(const QString &fileName);
    void loadXSD(const QString &fileName);
    void executeCalculation();
    void stopCurrentCalculation();
    void executeNewCalculation();
    void updateStatus();
    void waitEndCalculation();
    void abortCalculation();
    void goodbyeCalculation();
    QPair<int, QString> executeCalculationInThread(const QString &dataFile, const QString &schemaFile);
    void updateStatusFile(InfoPartGuidedValidationDialog *info, const bool how, const QString &filePath);
    bool checkEndCalculation();
    QString chooseFile(const QString &prevChoice);
    QString dataFile();
    QString schemaFile();
    virtual void dropEvent(QDropEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    bool isReady() ;

public:
    explicit GuidedValidationDialog(QWidget *parent, ApplicationData *appData);
    ~GuidedValidationDialog();

    void resetData();

    static void showValidationDialog(ApplicationData *appData);

private:
    Ui::GuidedValidationDialog *ui;

private slots:
    void checkIfDone();
    void on_cmdLoadXML_clicked();
    void on_cmdLoadXSD_clicked();

};

#endif // GUIDEDVALIDATIONDIALOG_H
