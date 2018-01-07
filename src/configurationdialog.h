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


#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include "xmlEdit.h"
#include <QDialog>
#include <QIcon>

#include "config/configurestyle.h"
#include "config/configvalidation.h"
#include "config/configurationgeneraloptions.h"
#include "config/configuresessionswidget.h"
#include "config/configureeditmodesdialog.h"
#include "config/configurexmlmanagementdialog.h"
#include "config/configureeditorcolors.h"
#include "applicationdata.h"

namespace Ui
{
class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

    enum ETabs {
        TAB_GENERAL = 0,
        TAB_STYLE,
        TAB_EDITORCOLORS,
        TAB_VALIDATION,
        TAB_SESSIONS,
        TAB_EDITMODES,
        TAB_XMLMETADATA
    };

    ApplicationData* _data;
    ConfigurationGeneralOptions *_generalOptions;
    ConfigureStyle  *_configureStyle;
    ConfigValidation* _configureValidation;
    ConfigureSessionsWidget* _configureSessions;
    ConfigureEditModesDialog* _configureEditModes;
    ConfigureXMLManagementDialog *_configureXmlManagement;
    ConfigureEditorColors *_configureEditorColors;

    QIcon generalIcon;
    QIcon styleIcon;
    QIcon editorColorsIcon;
    QIcon validationIcon;
    QIcon sessionsIcon;
    QIcon editModesIcon;
    QIcon xmlManagementIcon;

    void setTab(const ETabs tab, QWidget *widget, QIcon &icon, const QString &title, const QString &tooltip);
    void init(ApplicationData* data);

public:
    explicit ConfigurationDialog(ApplicationData* data, QWidget *parent = 0);
    ~ConfigurationDialog();

    static void doOptions(QWidget * const parentWindow, ApplicationData* data);



protected:

private:
    Ui::ConfigurationDialog *ui;

private slots:
    void on_buttonBox_accepted();
    void accept();

#ifdef QXMLEDIT_TEST
    friend class TestIndent;
#endif
};

#endif // CONFIGURATIONDIALOG_H
