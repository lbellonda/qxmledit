/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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

#ifndef BALSAMIQBASEDIALOG_H
#define BALSAMIQBASEDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui
{
class BalsamiqUI;
}

class BalsamiqBaseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BalsamiqBaseDialog(QWidget *parent = 0);
    ~BalsamiqBaseDialog();

protected:
    void changeEvent(QEvent *e);


signals:

public slots:

private slots:
    void on_buttonBox_clicked(QAbstractButton * button);
    void on_cmdOutputDir_clicked();
    void on_cmdChooseFiles_clicked();
    void on_overwriteFiles_clicked();
    void on_cmdOpenDestinationDir_clicked();
    void on_outputDir_textEdited(const QString & text);

private:
    Ui::BalsamiqUI *ui;
    QStringList _files;
    QString _inputDir;

    void loadSettings();
    void checkConditions();

};

#endif // BALSAMIQBASEDIALOG_H
