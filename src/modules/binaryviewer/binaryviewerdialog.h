/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#ifndef BINARYVIEWERDIALOG_H
#define BINARYVIEWERDIALOG_H

#include <QDialog>
#include "libQXmlEdit_global.h"
#include <QItemSelection>
#include "binaryviewermodel.h"


namespace Ui
{
class BinaryViewerDialog;
}


class LIBQXMLEDITSHARED_EXPORT BinaryViewerDialog : public QDialog
{
    Q_OBJECT

    BinaryViewerModel _model;
    QStringList _recentFiles;


public:
    explicit BinaryViewerDialog(QStringList recentFiles, QWidget *parent = 0);
    virtual ~BinaryViewerDialog();

private:
    Ui::BinaryViewerDialog *ui;
protected:
    void finishSetup(QStringList recentFiles);
    void start();
    void calcEnablePages();
    void moveToPage(const bool isNext);
    void moveToPageAbs(const int newPage);
    void refreshData();
    void selectRowAndEnsureIsVisible(const int newRow, const bool setIsVisible = true);
    int selRow();
    int lastRow();
    void search(const bool searchForward);
    void enableSearch();
    void enableGoto();
    void assignFile(const QString &filePath);
    void setNullModel();
    void msgNoModel();
    void msgInvalidAddress();
protected:
    void assignIO(QIODevice *io);
    virtual void keyPressEvent(QKeyEvent *e);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void on_cmdBrowse_clicked();
    void onCurrentPageChanged(int currentPage);
    void on_cmdPageNext_clicked();
    void on_cmdPagePrev_clicked();
    void on_cmbEncodings_activated(const QString & text);
    void on_cmdSearchNext_clicked();
    void on_cmdSearchPrev_clicked();
    void on_searchBox_textChanged(const QString & text);
    void selectionChangedBinaryRow(const QItemSelection& selected, const QItemSelection& deselected);
    void on_fileComboBox_activated(const QString & text);
    void on_cmdGoToAddress_clicked();
    void on_gotoAddress_textChanged(const QString & text);
};

#endif // BINARYVIEWERDIALOG_H
