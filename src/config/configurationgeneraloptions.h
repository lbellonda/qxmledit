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

#ifndef CONFIGURATIONGENERALOPTIONS_H
#define CONFIGURATIONGENERALOPTIONS_H

#include <QWidget>
#include "libQXmlEdit_global.h"
#include "applicationdata.h"

namespace Ui
{
class ConfigurationGeneralOptions;
}

class ConfigurationGeneralOptions : public QWidget
{
    Q_OBJECT

    ApplicationData* _data;
    QString _elementFontName;
    int _elementFontSize;
    bool _elementFontItalic;
    bool _elementFontBold;
    bool _started;

public:
    explicit ConfigurationGeneralOptions(QWidget *parent = 0);
    ~ConfigurationGeneralOptions();

    void init(ApplicationData* data);
    void checkEnablingStatus();
    void setElementFontInfo();
    void saveIfChanged();

private:
    Ui::ConfigurationGeneralOptions *ui;

    void save();
private slots:
    void on_fixedSizeAttributeFontSize_valueChanged(int value);
    void on_enableFWAFont_stateChanged(int state);
    void on_fixedSizeAttrComboFont_currentFontChanged(const QFont & font);
    void on_changeElementsFont_clicked();
    void on_enableEFontSize_stateChanged(int state);
    void on_enableLogs_stateChanged(int state);
    void on_logLevelCombo_currentIndexChanged(int index);
    void on_enableExperiments_stateChanged(int state);
    void on_chkDefBold_stateChanged(int state);
    void on_cbOpenFilesInNewWindow_stateChanged(int state);
    void on_cbShowImagesInTextTooltip_stateChanged(int state);
    void on_cbSingleInstance_stateChanged(int state);
    void on_cbAttrLen_stateChanged(int /*state*/);
    void on_cbSHowElementsIcon_stateChanged(int /*state*/);
};

#endif // CONFIGURATIONGENERALOPTIONS_H
