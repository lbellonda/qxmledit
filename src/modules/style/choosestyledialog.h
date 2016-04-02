/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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


#ifndef CHOOSESTYLEDIALOG_H
#define CHOOSESTYLEDIALOG_H

#include <QDialog>
#include "paintinfo.h"
#include "modules/style/displaystylesetting.h"

class XmlEditWidget;

namespace Ui
{
class ChooseStyleDialog;
}

class ChooseStyleDialog : public QDialog
{
    Q_OBJECT

    PaintInfo _defaultPaintInfo;
    XmlEditWidget *_editorEssential;
    XmlEditWidget *_editorCompact ;
    XmlEditWidget *_editorExtended ;
    XmlEditWidget *_editorFullDetails;

public:
    explicit ChooseStyleDialog(QWidget *parent = 0);
    ~ChooseStyleDialog();

    enum EStyles {
        Essential = 0,
        Compact = 1,
        Extended = 2,
        FullDetail = 3,
        MaxStyle
    };

    DisplayStyleSetting *selectedStyle();

protected:
    XmlEditWidget *setupEditor(const int index, QWidget *theParent, DisplayStyleSetting *setting);
    void reject();

private:

    DisplayStyleSetting *_selectedStyle ;
    DisplayStyleSetting _displaySettings[MaxStyle];
    DisplayStyleSetting _displaySettingsPredefined;

    Ui::ChooseStyleDialog *ui;

    void setup();
    void loadSampleData(XmlEditWidget *editor);
    QString makeDetail(DisplayStyleSetting *setting);
private slots:
    void on_tabStyle_currentChanged(int index);
    void on_cmdUsePredefined_clicked();
    void on_cmdUseThis_clicked();
};

#endif // CHOOSESTYLEDIALOG_H
