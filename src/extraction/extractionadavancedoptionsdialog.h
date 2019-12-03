/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#ifndef EXTRACTIONADAVANCEDOPTIONSDIALOG_H
#define EXTRACTIONADAVANCEDOPTIONSDIALOG_H

#include "xmlEdit.h"
#include "extractionoperation.h"

namespace Ui
{
class ExtractionAdavancedOptionsDialog;
}

class ExtractionAdavancedOptionsDialog : public QDialog
{
    Q_OBJECT
    ExtractionOperation *_operation;

    void selectAllOperation(const bool how);
public:
    explicit ExtractionAdavancedOptionsDialog(ExtractionOperation *theOperation, QWidget *parent = 0);
    virtual ~ExtractionAdavancedOptionsDialog();

protected:
    virtual void accept();
    void setup();
    void setupScripts();
    QSet<QString> setOfScripts();
    QTableWidgetItem* newTableItem(const QString &text, const QString &userData);
private:
    Ui::ExtractionAdavancedOptionsDialog *ui;
#ifdef QXMLEDIT_TEST
    friend class TestSplit;
#endif

private slots:
    void on_cmdSelectNone_clicked();
    void on_cmdSelectAll_clicked();
};

#endif // EXTRACTIONADAVANCEDOPTIONSDIALOG_H
