/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#ifndef AnonSettingsWidget_H
#define AnonSettingsWidget_H

#include <QWidget>

namespace Ui
{
class AnonSettingsWidget;
}

class AnonSettingsWidget : public QWidget
{
    Q_OBJECT

    bool _isText;
public:
    explicit AnonSettingsWidget(QWidget *parent = 0);
    ~AnonSettingsWidget();

    bool isIncludeChecked();
    bool isRecursiveChecked();
    bool isFixedChecked();
    QString fixedValueText();
    bool isUseNamespace();
    void setUseNamespace(const bool how);

    void setExcludeChecked(const bool how);
    void setMethodDefaultChecked(const bool how);
    void enableRecursive(const bool how);

    void setText(const bool isText);

private:
    Ui::AnonSettingsWidget *ui;

    void enableItems();

private slots:
    void on_include_clicked();
    void on_exclude_clicked();
    void on_methodDefault_clicked();
    void on_methodFixed_clicked();
    void on_FixedValue_textEdited(const QString & text);
};

#endif // AnonSettingsWidget_H
