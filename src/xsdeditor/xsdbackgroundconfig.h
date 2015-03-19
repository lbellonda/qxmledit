/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#ifndef XSDBACKGROUNDCONFIG_H
#define XSDBACKGROUNDCONFIG_H

#include <QDialog>
#include <QIcon>

#include "xsdeditor/xsdgraphicsconfiguration.h"

namespace Ui
{
class XSDBackgroundConfig;
}

class XSDBackgroundConfig : public QDialog
{
    Q_OBJECT

    XSDGraphicsBackgroundConfiguration _configuration;
    bool _started ;
    QIcon _gradientLinearIcon;
    QIcon _gradientSolidIcon;
    QIcon _gradientRadialIcon;
    QIcon _gradientHorizontalIcon;
    QIcon _gradientVerticalIcon;
    QIcon _gradientDiagonalIcon;

public:
    explicit XSDBackgroundConfig(QWidget *parent = 0);
    ~XSDBackgroundConfig();

private:
    Ui::XSDBackgroundConfig *ui;

    void valuesChanged();
    void setupData();
    void enableUI();
    void setupConnections();
    void initValues();
    void setBkColorToButton(QPushButton *button, const QColor &color);

private slots:
    void on_mainColor_clicked();
    void on_alternateColor_clicked();
    void on_gradientTypeCombo_currentIndexChanged(int index);
    void on_gradientDirectionCombo_currentIndexChanged(int index);
    void on_gradientUnitsCombo_currentIndexChanged(int index);
    void on_repetitionAt_valueChanged(int value);
    void onReset();

signals:
    void onBackgroundConfigurationChanged(XSDGraphicsBackgroundConfiguration *configuration);
};

#endif // XSDBACKGROUNDCONFIG_H
