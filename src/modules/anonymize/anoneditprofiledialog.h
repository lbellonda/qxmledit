/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#ifndef ANONEDITPROFILEDIALOG_H
#define ANONEDITPROFILEDIALOG_H

#include <QDialog>
#include <data/GenericPersistentData.h>

namespace Ui
{
class AnonEditProfileDialog;
}

class AnonEditProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnonEditProfileDialog(const bool isInsert, GenericPersistentData *snippet, QWidget *parent = 0);
    ~AnonEditProfileDialog();
    bool editProfile();

protected:
    void changeEvent(QEvent *e);
    bool checkIfAcceptable();
    QSet<QString> tagsFromUI();
    void enableOkIf();
private:
    Ui::AnonEditProfileDialog *ui;
    GenericPersistentData* _snippet ;
    bool _isInsert;

    bool areDataOK();
    void setUpData();
public Q_SLOTS:
    virtual void accept();

private slots:
    void on_name_textChanged(const QString & text);
    void on_tags_textChanged(const QString & text);
    void on_description_textChanged(const QString & text);
};

#endif // ANONEDITPROFILEDIALOG_H
