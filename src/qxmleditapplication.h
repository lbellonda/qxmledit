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


#ifndef QXMLEDITAPPLICATION_H
#define QXMLEDITAPPLICATION_H

#include <QApplication>

class ApplicationData;

class QXmlEditApplication : public QApplication
{
    Q_OBJECT
private:
    ApplicationData *_appData;
public:
    explicit QXmlEditApplication(int &argc, char **argv);
    ~QXmlEditApplication();

    ApplicationData *appData() const;
    void setAppData(ApplicationData *appData);

protected:
    bool event(QEvent *event);
signals:

public slots:
    void onNewWindow();
    void onCodePagesTools();
    void onManageSessions();
    void onEncodingTools();
    void onSplitFile();
    void onViewData();

};

#endif // QXMLEDITAPPLICATION_H
